/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#include "vaultic_apdu.h"
#include "vaultic_common.h"
#include "vaultic_key_aux.h"
#include "vaultic_api.h"
#include "vaultic_comms.h"
#include "vaultic_utils.h"
#include "vaultic_mem.h"
#include "vaultic_crc16.h"
#include "vaultic_command.h"
#include "vaultic_ecdsa_key_aux.h"


/* VltEcdsaKey aux functions
 * =======================
 *
 * The VltEcdsaKey aux functions marshal the input key data into a fixed-size
 * command buffer, packing the buffer so that it is completely filled where
 * possible then issuing the command using VltCommand.
 *
 * The cases listed earlier in the source are 'simple' and involve packing only
 * fixed-size data. The later cases, especially the private key cases, are more
 * complex and must cope with stopping and re-starting the packing of data
 * within individual fields. 
 *
 * The routines are mostly very similar and could be merged together to save
 * space at the expense of making the simple cases use the more generic code.
 *
 * State variables are maintained which tell us which field we're processing and
 * the offset within that field. The routines are structured so that when we run
 * out of space packing a buffer we send the buffer then immediately resume
 * packing the next input byte. This keeps the buffers as full as possible,
 * minimising the number of chunks/commands which need sending.
 *
 * Common variables:
 *
 * u16KeyObjLen is the length of the key object.
 * u16Remaining holds the total number of bytes remaining to be sent. This does
 *              not include the size of any APDUs.
 * u16MaxChunk  is the largest number of bytes we can dispatch in a single
 *              VltCommand call.
 * u16Chunk     is the number of bytes we will be sending this iteration.
 * field        records which field of the input structure we're packing.
 * u16Offset    records the offset within that field.
 * pu8Data      points to the next available free byte in the output buffer.
 *
 * Chaining
 * --------
 * While u16Remaining exceeds u16MaxChunk there's still data to be sent. In this
 * case we set the chaining flag. Otherwise we leave it clear.
 *
 * APDU
 * ----
 * To reduce buffer overhead the comms layer may use a single buffer for both
 * input and output buffers. For this reason we must re-build the APDU on every
 * iteration, even though it's always the same.
 *
 * Field Packing
 * -------------
 * Fields are packed either by outputting the individual bytes to ensure correct
 * endian ordering (all VaultIC data uses big endian ordering) or by copying the
 * raw data across directly, in the case of arrays.
 *
 * Fields of the same size within a structure are treated identically by the
 * code.
 *
 * If the field requires key masking then the host_memcpyxor function is used.
 *
 * When the end of the field is met, the field variable is incremented which
 * causes the code to move onto the next field. The outer while loop ensures
 * that data continues to be packed as long as there is space in the buffer.
 *
 * CRCs
 * ----
 * The final two bytes of every put key data is the CRC of the plaintext key
 * object. This isn't computed for the entire input data, but just the key
 * object.
 *
 * Variables:
 *
 * u16Crc       holds the current CRC.
 *
 * The CRC is computed on the data prior to it being sent. When there are only
*  two bytes remaining to be sent the CRC data is written out.
 *
 * Limitations
 * -----------
 * Two byte fields can't be resumed (if there's only one byte free in the buffer
 * it won't use that then later come back and write the second) as it's not
 * worth the effort. In these cases the outer loop ensures that the loop is
 * terminated early if there aren't two bytes available.
 */

/**
 * Externs 
 */
extern VLT_MEM_BLOB Command;                            /* declared in vaultic_api.c */
extern VLT_MEM_BLOB Response;                           /* declared in vaultic_api.c */
extern VLT_U16 idx;                                     /* declared in vaultic_api.c */

extern VLT_U16 u16CalculatedCrc;
extern VLT_U16 u16ReceivedCrc;

extern VLT_STS VltReadKeyCommand(VLT_MEM_BLOB *command, 
    VLT_MEM_BLOB *response,
    VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,    
    VLT_PSW pSW);

#if VLT_ENABLE_KEY_ECDSA == VLT_ENABLE
    static VLT_U16 crcStartPos;
#endif /* VLT_ENABLE_KEY_ECDSA == VLT_ENABLE */


#if VLT_ENABLE_KEY_ECDSA == VLT_ENABLE

VLT_STS VltPutKey_EcdsaPublic(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
    VLT_U8 u8KeyID,
    const VLT_KEY_OBJ_ECDSA_PUB* pKeyObj,
    VLT_PSW pSW)
{
    enum { Initial, QLen, Qx, Qy, DpGroup, DpIndex, Assurance, End } field;

    VLT_STS       status = VLT_FAIL;
    VLT_U16       u16MaxChunk;
    VLT_U16       u16KeyObjLen;
    VLT_U16       u16Offset;
    VLT_U16       u16Remaining;
    VLT_U16       u16Crc = VLT_CRC16_CCITT_INIT_0s;

    /*
     * Validate all input parameters.
     */
    if ( ( NULL == pKeyFilePrivileges ) ||
         ( NULL == pKeyObj ) ||
         ( NULL == pSW )||
         ( NULL == pKeyObj->pu8Qx ) ||
         ( NULL == pKeyObj->pu8Qy ) )
    {
        return ( EPKEPUBNULLPARA );
    }

    /* We need to split the data up into chunks, the size of which the comms
     * layer tells us. */

    u16MaxChunk = VltCommsGetMaxSendSize();

    /* Work out the size of abKeyObject. */
    u16KeyObjLen = VLT_ECDSA_PUBLIC_STATIC_PART_LENGTH + ( pKeyObj->u16QLen * 2 );

    /* The field and u16Offset variables control which field we're working on
     * and the offset within that field when we are marshalling variable-sized
     * buffers. */
    field     = Initial;
    u16Offset = 0; /* 0 => first chunk */

    u16Remaining = VLT_PUTKEY_FIXED_DATA_LENGTH + u16KeyObjLen;

    while( 0 != u16Remaining )
    {
        VLT_U16 u16Chunk = 0;

        /* Build APDU. We have to do this on every iteration as the output
         * of the previous iteration will have overwritten it (assuming a
         * shared buffer). */

        idx = VLT_APDU_DATA_OFFSET;

        if(u16Remaining > u16MaxChunk)
        {
            u16Chunk = u16MaxChunk;
            Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_CHAINING;
        }
        else
        {
            u16Chunk = u16Remaining;
            Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL; 
        }
        Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_PUT_KEY;
        Command.pu8Data[VLT_APDU_P1_OFFSET] = u8KeyGroup;
        Command.pu8Data[VLT_APDU_P2_OFFSET] = u8KeyIndex;
        Command.pu8Data[VLT_APDU_P3_OFFSET] = LIN(WRAPPED_BYTE(u16Chunk));

        /* Build Data In */
        if(field == Initial) /* building the first part of the data */
        {
            /* bmAccess */
            Command.pu8Data[idx++] = pKeyFilePrivileges->u8Read;
            Command.pu8Data[idx++] = pKeyFilePrivileges->u8Write;
            Command.pu8Data[idx++] = pKeyFilePrivileges->u8Delete;
            Command.pu8Data[idx++] = pKeyFilePrivileges->u8Execute;

            /* wKeyObjectLength */
            Command.pu8Data[idx++] = (VLT_U8) ((u16KeyObjLen >> 8) & 0xFF);
            Command.pu8Data[idx++] = (VLT_U8) ((u16KeyObjLen >> 0) & 0xFF);
        }

        /* abKeyObject data */
        crcStartPos = idx;

        if(field == Initial) /* building the first part of the data */
        {
            /* bKeyID */
            Command.pu8Data[idx++] = u8KeyID;

            field++; /* proceed to next field */
        }

        /* Need at least two bytes to proceed (due to two-byte fields not
         * being resumable). */
        while( ( field <= Assurance ) && 
            ( NumBufferBytesAvail( u16Chunk, idx ) >= 2 ) )
        {
            VLT_U16       u16Len = 0;
            const VLT_U8 *pu8Data = NULL;

            switch (field)
            {
            case QLen:
            case Qx:
                u16Len = pKeyObj->u16QLen;
                pu8Data = pKeyObj->pu8Qx;
                break;
            case Qy:
                u16Len = pKeyObj->u16QLen;
                pu8Data = pKeyObj->pu8Qy;
                break;
            default:
                /* Do nothing */
                break;
            }

            if( QLen == field )
            {
                Command.pu8Data[idx++] = (VLT_U8) ((u16Len >> 8) & 0xFF); 
                Command.pu8Data[idx++] = (VLT_U8) ((u16Len >> 0) & 0xFF);

                field++; /* proceed to next field */
            }

            else if( ( Qx == field ) || ( Qy == field ) )
            {
                VLT_U16 u16Avail;
                VLT_U16 u16RemainingQLen;
                VLT_U16 u16PartialQLen;

                u16RemainingQLen = u16Len - u16Offset;

                u16Avail = NumBufferBytesAvail( u16Chunk, idx );
                if(u16RemainingQLen > u16Avail)
                {
                    u16PartialQLen = u16Avail; /* bytes of 'Q' remaining to be sent */
                }
                else
                {
                    u16PartialQLen = u16RemainingQLen;
                    field++; /* proceed to next field */
                }

                if(u16PartialQLen)
                {
                    /*
                    * No need to check the return type as pointer has been validated
                    */
                    (void)host_memcpy( &(Command.pu8Data[idx]),
                        &pu8Data[u16Offset],
                        u16PartialQLen);

                    idx += u16PartialQLen;
                    u16Offset += u16PartialQLen;

                    if( u16Offset == u16Len ) /* end of field */
                    {
                        u16Offset = 0;
                    }
                }
            }
            else if( DpGroup == field )
            {
                Command.pu8Data[idx++] = pKeyObj->u8DomainParamsGroup;
                field++; /* proceed to next field */
            }
            else if( DpIndex == field)
            {
                Command.pu8Data[idx++] = pKeyObj->u8DomainParamsIndex;
                field++; /* proceed to next field */
            }
            else if( Assurance == field)
            {
                Command.pu8Data[idx++] = pKeyObj->u8Assurance;
                field++; /* proceed to next field */
            }
        }

        /* Decrement the remaining number of bytes to be sent. */
        u16Remaining -= NumBytesInBuffer( idx );

        /* Update the CRC-16 with the (partial) data. */
        u16Crc = VltCrc16Block( u16Crc,
            &(Command.pu8Data[crcStartPos]),
            ( idx  - crcStartPos ) );

        /* Emit the CRC-16 once there's no data remaining. */

        /* It's entirely possible that we will fall through all of the above
         * code with 'u16Remaining' at zero and enter here where we construct a
         * data block only containing the wCRC data. */

        /* We need two bytes free in the buffer for the wCRC field. */
        if( ( NUM_CRC_BYTES == u16Remaining ) && 
            ( NumBufferBytesAvail( u16Chunk, idx ) >= NUM_CRC_BYTES ) )
        {
            Command.pu8Data[idx++] = (VLT_U8) ((u16Crc >> 8) & 0xFF);
            Command.pu8Data[idx++] = (VLT_U8) ((u16Crc >> 0) & 0xFF);
            u16Remaining -= NUM_CRC_BYTES;
        }

        /* Send the command */

        status = VltCommand( &Command, &Response, idx, 0, pSW );
        if(VLT_OK != status)
        {
            return status;
        }

        /* React to the status word */

        switch (*pSW)
        {
        case VLT_STATUS_COMPLETED:
        case VLT_STATUS_SUCCESS:
            break;

        default:
            return VLT_OK; /* unexpected status word */
        }
    }

    return status;
}




VLT_STS VltPutKey_EcdsaPrivate(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
    VLT_U8 u8KeyID,
    const VLT_KEY_OBJ_ECDSA_PRIV* pKeyObj,
    VLT_PSW pSW)
{
    enum { Initial, Mask, DLen, D, DpGroup, DpIndex, PubGroup, PubIndex, End } field;

    VLT_STS       status = VLT_FAIL;
    VLT_U16       u16MaxChunk;
    VLT_U16       u16KeyObjLen;
    VLT_U16       u16Offset;
    VLT_U16       u16Remaining;
    VLT_U16       u16Crc = VLT_CRC16_CCITT_INIT_0s;

    /*
     * Validate all input parameters.
     */
    if ( ( NULL == pKeyFilePrivileges ) ||
         ( NULL == pKeyObj ) ||
         ( NULL == pSW )||
         ( NULL == pKeyObj->pu8D ) )
    {
        return ( EPKEPRIVNULLPARA );
    }

    /* We need to split the data up into chunks, the size of which the comms
     * layer tells us. */

    u16MaxChunk = VltCommsGetMaxSendSize();

    /* Work out the size of abKeyObject. */
    u16KeyObjLen = VLT_ECDSA_PRIVATE_STATIC_PART_LENGTH + pKeyObj->u16DLen;

    /* The field and u16Offset variables control which field we're working on
     * and the offset within that field when we are marshalling variable-sized
     * buffers. */
    field     = Initial;
    u16Offset = 0; /* 0 => first chunk */

    u16Remaining = VLT_PUTKEY_FIXED_DATA_LENGTH + u16KeyObjLen;

    while( 0 != u16Remaining )
    {
        VLT_U16 u16Chunk = 0;

        /* Build APDU. We have to do this on every iteration as the output
         * of the previous iteration will have overwritten it (assuming a
         * shared buffer). */
        idx = VLT_APDU_DATA_OFFSET;

        if(u16Remaining > u16MaxChunk)
        {
            u16Chunk = u16MaxChunk;
            Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_CHAINING;
        }
        else
        {
            u16Chunk = u16Remaining;
            Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL; 
        }
        Command.pu8Data[VLT_APDU_INS_OFFSET]= VLT_INS_PUT_KEY;
        Command.pu8Data[VLT_APDU_P1_OFFSET] = u8KeyGroup;
        Command.pu8Data[VLT_APDU_P2_OFFSET] = u8KeyIndex;
        Command.pu8Data[VLT_APDU_P3_OFFSET] = LIN(WRAPPED_BYTE(u16Chunk));

        /* Build Data In */
        if(field == Initial) /* building the first part of the data */
        {
            /* bmAccess */
            Command.pu8Data[idx++] = pKeyFilePrivileges->u8Read;
            Command.pu8Data[idx++] = pKeyFilePrivileges->u8Write;
            Command.pu8Data[idx++] = pKeyFilePrivileges->u8Delete;
            Command.pu8Data[idx++] = pKeyFilePrivileges->u8Execute;

            /* wKeyObjectLength */
            Command.pu8Data[idx++] = (VLT_U8) ((u16KeyObjLen >> 8) & 0xFF);
            Command.pu8Data[idx++] = (VLT_U8) ((u16KeyObjLen >> 0) & 0xFF);
        }

        /* abKeyObject data */
        crcStartPos = idx;

        if(field == Initial) /* building the first part of the data */
        {
            /* bKeyID */
            Command.pu8Data[idx++] = u8KeyID;

            field++; /* proceed to next field */
        }

        /* Need at least three bytes to proceed (due to three-byte fields not
         * being resumable). */
        while ( ( field <= PubIndex ) && ( NumBufferBytesAvail( u16Chunk, idx ) >= 3 ) )
        {
            VLT_U16       u16Len = 0;
            const VLT_U8 *pu8Data = NULL;

            if( field <= D )
            {
                u16Len = pKeyObj->u16DLen;
                pu8Data = pKeyObj->pu8D;
            }

            if( Mask == field )
            {
                Command.pu8Data[idx++] = pKeyObj->u8Mask;
                field++; /* proceed to next field */
            }

            else if( DLen == field )
            {
                Command.pu8Data[idx++] = (VLT_U8) ((u16Len >> 8) & 0xFF); 
                Command.pu8Data[idx++] = (VLT_U8) ((u16Len >> 0) & 0xFF);
                field++; /* proceed to next field */
            }

            else if( D == field )
            {
                VLT_U16 u16Avail;
                VLT_U16 u16RemainingDLen;
                VLT_U16 u16PartialDLen;

                u16RemainingDLen = u16Len - u16Offset;

                u16Avail = NumBufferBytesAvail( u16Chunk, idx );
                if(u16RemainingDLen > u16Avail)
                {
                    u16PartialDLen = u16Avail; /* bytes of 'D' remaining to be sent */
                }
                else
                {
                    u16PartialDLen = u16RemainingDLen;
                    field++; /* proceed to next field */
                }

                if(u16PartialDLen)
                {
                    VLT_U8 u8Mask;

                    u8Mask = pKeyObj->u8Mask;

                    /*
                    * No need to check the return type as pointer has been validated
                    */
                    (void)host_memcpyxor( &(Command.pu8Data[idx]), 
                        &pu8Data[u16Offset], 
                        u16PartialDLen, 
                        u8Mask);

                    idx += u16PartialDLen;
                    u16Offset += u16PartialDLen;

                    if(u16Offset == u16Len) /* end of field */
                    {
                        u16Offset = 0;
                    }
                }
            }
            else if( DpGroup == field )
            {
                Command.pu8Data[idx++] = pKeyObj->u8DomainParamsGroup;
                field++; /* proceed to next field */
            }
            else if( DpIndex == field )
            {
                Command.pu8Data[idx++] = pKeyObj->u8DomainParamsIndex;
                field++; /* proceed to next field */
            }
            else if( PubGroup == field )
            {
                Command.pu8Data[idx++] = pKeyObj->u8PublicKeyGroup;
                field++; /* proceed to next field */
            }
            else if( PubIndex == field )
            {
                Command.pu8Data[idx++] = pKeyObj->u8PublicKeyIndex;
                field++; /* proceed to next field */
            }           
        }

        /* Decrement the remaining number of bytes to be sent. */
        u16Remaining -= NumBytesInBuffer( idx );

        /* Update the CRC-16 with the (partial) data. */
        u16Crc = VltCrc16Block( u16Crc,
            &(Command.pu8Data[crcStartPos]),
            ( idx - crcStartPos ) );

        /* Emit the CRC-16 once there's no data remaining. */

        /* It's entirely possible that we will fall through all of the above
         * code with 'u16Remaining' at zero and enter here where we construct a
         * data block only containing the wCRC data. */

        /* We need two bytes free in the buffer for the wCRC field. */
        if( ( NUM_CRC_BYTES == u16Remaining ) && 
            ( NumBufferBytesAvail( u16Chunk, idx ) >= NUM_CRC_BYTES ) )
        {
            Command.pu8Data[idx++]= (VLT_U8) ((u16Crc >> 8) & 0xFF);
            Command.pu8Data[idx++] = (VLT_U8) ((u16Crc >> 0) & 0xFF);
            u16Remaining -= NUM_CRC_BYTES;
        }

        /* Send the command */

        status = VltCommand( &Command, &Response, idx, 0, pSW );
        if(VLT_OK != status)
        {
            return status;
        }

        /* React to the status word */

        switch (*pSW)
        {
        case VLT_STATUS_COMPLETED:
        case VLT_STATUS_SUCCESS:
            break;

        default:
            return VLT_OK; /* unexpected status word */
        }
    }

    return status;
}



VLT_STS VltPutKey_EcdsaParams( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
    VLT_U8 u8KeyID,
    const VLT_KEY_OBJ_ECDSA_PARAMS* pKeyObj,
    VLT_PSW pSW )
{
    enum { Init, QLen, Q, Gx, Gy, Gz, A, B, NLen, N, H, Assurance, End } field;

    VLT_STS status = VLT_FAIL;
    VLT_U16 u16MaxChunk;
    VLT_U16 u16KeyObjLen;
    VLT_U16 u16Offset;
    VLT_U16 u16Remaining;
    VLT_U16 u16Crc = VLT_CRC16_CCITT_INIT_0s;

    /*
     * Validate all input parameters.
     */
    if ( ( NULL == pKeyFilePrivileges ) ||
         ( NULL == pKeyObj ) ||
         ( NULL == pSW ) ||
         ( NULL == pKeyObj->pu8Q ) ||
         ( NULL == pKeyObj->pu8Gx) ||
         ( NULL == pKeyObj->pu8Gy) ||
         ( NULL == pKeyObj->pu8Gz ) ||
         ( NULL == pKeyObj->pu8A ) ||
         ( NULL == pKeyObj->pu8B ) ||
         ( NULL == pKeyObj->pu8N ) )
    {
        return ( EPKECDSAPRMNULLPARA );
    }

    /* We need to split the data up into chunks, the size of which the comms
     * layer tells us. */

    u16MaxChunk = VltCommsGetMaxSendSize();

    /*
    * Work out the size of abKeyObject.
    */
    u16KeyObjLen = VLT_ECDSA_PARAMS_STATIC_PART_LENGTH + ( pKeyObj->u16QLen * 6 ) 
        + pKeyObj->u16NLen;

    /* 
    * The field and u16Offset variables control which field we're working on
    * and the offset within that field when we are marshalling variable-sized
    * buffers.
    */
    field = Init;
    u16Offset = 0; /* 0 => first chunk */

    u16Remaining = VLT_PUTKEY_FIXED_DATA_LENGTH + u16KeyObjLen;

    while( 0 != u16Remaining )
    {
        VLT_U16 u16Chunk = 0;

        /* Build APDU. We have to do this on every iteration as the output
         * of the previous iteration will have overwritten it (assuming a
         * shared buffer). */

        idx = VLT_APDU_DATA_OFFSET;

        if( u16Remaining > u16MaxChunk )
        {
            u16Chunk = u16MaxChunk;
            Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_CHAINING;
        }
        else
        {
            u16Chunk = u16Remaining;
            Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL; 
        }
        Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_PUT_KEY;
        Command.pu8Data[VLT_APDU_P1_OFFSET] = u8KeyGroup;
        Command.pu8Data[VLT_APDU_P2_OFFSET] = u8KeyIndex;
        Command.pu8Data[VLT_APDU_P3_OFFSET] = LIN(WRAPPED_BYTE(u16Chunk));

        /* Build Data In */
        if( field == Init ) /* building the first part of the data */
        {
            /* bmAccess */
            Command.pu8Data[idx++] = pKeyFilePrivileges->u8Read;
            Command.pu8Data[idx++] = pKeyFilePrivileges->u8Write;
            Command.pu8Data[idx++] = pKeyFilePrivileges->u8Delete;
            Command.pu8Data[idx++] = pKeyFilePrivileges->u8Execute;

            /* wKeyObjectLength */
            Command.pu8Data[idx++] = (VLT_U8) ((u16KeyObjLen >> 8) & 0xFF);
            Command.pu8Data[idx++] = (VLT_U8) ((u16KeyObjLen >> 0) & 0xFF);
        }

        /* abKeyObject data */
        crcStartPos = idx;

        if( field == Init ) /* building the first part of the data */
        {
            /* bKeyID */
            Command.pu8Data[idx++] = u8KeyID;

            field++; /* proceed to next field */
        }

        /* Need at least two bytes to proceed (due to two-byte fields not
         * being resumable). */
        while( ( field <= Assurance) && 
            ( NumBufferBytesAvail( u16Chunk, idx ) >= 2 ) )
        {
            VLT_U16 u16Len = 0;
            const VLT_U8* pu8Data = NULL;

            switch (field)
            {
            case QLen:
                u16Len = pKeyObj->u16QLen;
                pu8Data = pKeyObj->pu8Q;
                break;
            case Q:
                u16Len = pKeyObj->u16QLen;
                pu8Data = pKeyObj->pu8Q;
                break;
            case Gx:
                u16Len = pKeyObj->u16QLen;
                pu8Data = pKeyObj->pu8Gx;
                break;
            case Gy:
                u16Len = pKeyObj->u16QLen;
                pu8Data = pKeyObj->pu8Gy;
                break;
            case Gz:
                u16Len = pKeyObj->u16QLen;
                pu8Data = pKeyObj->pu8Gz;
                break;
            case A:
                u16Len = pKeyObj->u16QLen;
                pu8Data = pKeyObj->pu8A;
                break;
            case B:
                u16Len = pKeyObj->u16QLen;
                pu8Data = pKeyObj->pu8B;
                break;
            case NLen:
            case N:
                u16Len = pKeyObj->u16NLen;
                pu8Data = pKeyObj->pu8N;
                break;
            default:
                /* No need to do anything*/
                break;
            }

            if( ( QLen == field ) || ( NLen == field ) )
            {
                Command.pu8Data[idx++] = (VLT_U8)((u16Len >> 8) & 0xFF); 
                Command.pu8Data[idx++] = (VLT_U8)((u16Len >> 0) & 0xFF);

                field++; /* proceed to next field */
            }

            else if( ( Q == field ) || 
                ( Gx == field ) ||
                ( Gy == field ) ||
                ( Gz == field ) || 
                ( A == field ) || 
                ( B == field ) ||
                ( N == field ) )
            {
                VLT_U16 u16Avail;
                VLT_U16 u16RemainingLen;
                VLT_U16 u16PartialLen;

                u16RemainingLen = u16Len - u16Offset;

                u16Avail = NumBufferBytesAvail( u16Chunk, idx );
                if(u16RemainingLen > u16Avail)
                {
                    u16PartialLen = u16Avail; /* bytes of data remaining to be sent */
                }
                else
                {
                    u16PartialLen = u16RemainingLen;
                    field++; /* proceed to next field */
                }

                if( u16PartialLen )
                {
                    /*
                    * No need to check the return type as pointer has been validated
                    */
                    (void)host_memcpy( &(Command.pu8Data[idx]),
                        &pu8Data[u16Offset],
                        u16PartialLen );

                    idx += u16PartialLen;
                    u16Offset += u16PartialLen;

                    if( u16Offset == u16Len ) /* end of field */
                    {
                        u16Offset = 0;
                    }
                }
            }
            if( H == field  )
            {
                Command.pu8Data[idx++] = (VLT_U8)((pKeyObj->u32H >> 24) & 0xFF); 
                Command.pu8Data[idx++] = (VLT_U8)((pKeyObj->u32H >> 16) & 0xFF);
                Command.pu8Data[idx++] = (VLT_U8)((pKeyObj->u32H >> 8) & 0xFF); 
                Command.pu8Data[idx++] = (VLT_U8)((pKeyObj->u32H >> 0) & 0xFF);
                field++; /* proceed to next field */
            }

            else if( Assurance == field )
            {
                Command.pu8Data[idx++] = pKeyObj->u8Assurance;
                field++; /* proceed to next field */
            }
        }

        /* Decrement the remaining number of bytes to be sent. */
        u16Remaining -= NumBytesInBuffer( idx );

        /* Update the CRC-16 with the (partial) data. */
        u16Crc = VltCrc16Block( u16Crc,
            &(Command.pu8Data[crcStartPos]),
            ( idx  - crcStartPos ) );

        /* Emit the CRC-16 once there's no data remaining. */

        /* It's entirely possible that we will fall through all of the above
         * code with 'u16Remaining' at zero and enter here where we construct a
         * data block only containing the wCRC data. */

        /* We need two bytes free in the buffer for the wCRC field. */
        if( ( NUM_CRC_BYTES == u16Remaining ) && 
            ( NumBufferBytesAvail( u16Chunk, idx ) >= NUM_CRC_BYTES ) )
        {
            Command.pu8Data[idx++] = (VLT_U8) ((u16Crc >> 8) & 0xFF);
            Command.pu8Data[idx++] = (VLT_U8) ((u16Crc >> 0) & 0xFF);
            u16Remaining -= NUM_CRC_BYTES;
        }

        /* Send the command */

        status = VltCommand( &Command, &Response, idx, 0, pSW );
        if(VLT_OK != status)
        {
            return status;
        }

        /* React to the status word */

        switch (*pSW)
        {
        case VLT_STATUS_COMPLETED:
        case VLT_STATUS_SUCCESS:
            break;

        default:
            return VLT_OK; /* unexpected status word */
        }
    }

    return( status );
}



VLT_STS VltReadKey_EcdsaPublic(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_ECDSA_PUB* keyObj,
    VLT_PSW pSW)
{    
    enum { QLen, Qx, Qy, DpGroup, DpIndex, Assurance, End } field;

    VLT_STS       status; 
    VLT_U16       u16Offset;
    VLT_U16       u16Avail;
    
    VLT_U16       keyDataLen = Response.pu8Data[2]*2+6;
    VLT_U16		  keyDataLenVic100 = Response.u16Len;

	VLT_U16 u16CalculatedCrcVic100 = VLT_CRC16_CCITT_INIT_0s;
	VLT_U16 u16ReceivedCrcVic100;

    ReadKeyInitCrc();
    
    u16CalculatedCrc = VltCrc16Block( u16CalculatedCrc, 
            Response.pu8Data, 
            keyDataLen );

    /* Retrieve received CRC */
    u16ReceivedCrc = VltEndianReadPU16( 
            &Response.pu8Data[ keyDataLen ] );

	/////////////////////////////////
	//    VAULTIC 100 workaround   //
	/////////////////////////////////
	u16CalculatedCrcVic100 = VltCrc16Block( u16CalculatedCrcVic100, 
		Response.pu8Data, 
		keyDataLenVic100 );

	/* Retrieve received CRC */
	u16ReceivedCrcVic100 = VltEndianReadPU16( 
		&Response.pu8Data[ keyDataLenVic100 ] );
	/////////////////////////////////

    /*
     * Validate all input parameters.
     */
    if ( ( NULL == keyObj ) ||
         ( NULL == pSW ) ||
         ( NULL == keyObj->pu8Qx ) ||
         ( NULL == keyObj->pu8Qy ) )
    {
        return ( ERKEPUBNULLPARA );
    }


    /* offset into output buffer */
    u16Offset = 0;                     

    /* skip bKeyID since caller handles it */
    idx = 1;    
    u16Avail  = Response.u16Len  - 1;

    /* Unpack the key object */

    /* We loop until we step beyond the last field. Buffers are incrementally
     * filled with data as it arrives. We can spend multiple iterations filling
     * in a single field. We must cope with values arriving in awkward ways,
     * e.g. a 2-byte field could arrive as the final byte and initial byte of
     * two separate requests. */

    for( field = QLen; field < End; )
    {
        VLT_U16 u16BufLen;
        VLT_U16 u16BufferSize;
        VLT_PU8 pu8Buf;

        /* Fill the buffer up if it's empty */

        if (u16Avail == 0)
        {
            status = VltReadKeyCommand( &Command, &Response, u8KeyGroup,
                u8KeyIndex, pSW );

            if (VLT_OK != status)
            {
                return status;
            }

            /* use entire buffer this iteration */
            idx = 0; 
            u16Avail = Response.u16Len;
        }

        /* Turn each field into a buffer length and pointer */

        switch( field )
        {
            case QLen:
                u16BufLen = 2;
                pu8Buf = (VLT_U8 *) &keyObj->u16QLen;
                u16BufferSize = keyObj->u16QLen;
                break;

            case Qx:
                u16BufLen = keyObj->u16QLen;
                pu8Buf = keyObj->pu8Qx;
                break;

            case Qy:
                u16BufLen = keyObj->u16QLen;
                pu8Buf = keyObj->pu8Qy;
                break;

            case DpGroup:
                u16BufLen = 1;
                pu8Buf = (VLT_U8 *)&keyObj->u8DomainParamsGroup;                
                break;

            case DpIndex:
                u16BufLen = 1;
                pu8Buf = (VLT_U8 *)&keyObj->u8DomainParamsIndex;                
                break;

            case Assurance:
                u16BufLen = 1;
                pu8Buf = (VLT_U8 *)&keyObj->u8Assurance;
                break;

            default:
                return ERKBADFIELD;
        }

        /* Fill the buffer as much as possible */

        {
            VLT_U16 u16Remain;
            VLT_U16 u16Copy;

            u16Remain = u16BufLen - u16Offset;
            if (u16Avail > u16Remain)
            {
                u16Copy = u16Remain;
            }
            else
            {
                u16Copy = u16Avail;
            }

            if( ( field == QLen ) &&
                ( u16BufferSize < VltEndianReadPU16( &Response.pu8Data[idx] ) ) )
            {
                return ( ERKEPUBNOROOM );
            }

            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memcpy(pu8Buf + u16Offset, &Response.pu8Data[idx], u16Copy);
            idx += u16Copy;
            u16Offset += u16Copy;
            u16Avail -= u16Copy;

            if (u16Offset == u16BufLen) /* buffer full? */
            {
                /* When reading the length values we accumulate the bytes
                 * piecemeal in the 2-byte value itself, treating it as a small
                 * buffer. Once enough bytes are ready we (may) need to endian
                 * swap them, which we do here. */

                if (field == QLen)
                {
                    keyObj->u16QLen = VltEndianReadPU16(pu8Buf);
                }

                u16Offset = 0;

                /* move to next field */
                field++; 
            }
        }
    }

    /* Validate received CRC */
    if( u16ReceivedCrc != u16CalculatedCrc )
    {
		if( u16ReceivedCrcVic100 != u16CalculatedCrcVic100 )
		 return( ERKECDSAPBIVLDCRC );
    }

    return VLT_OK;
}



VLT_STS VltReadKey_EcdsaPrivate( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_ECDSA_PRIV* keyObj,
    VLT_PSW pSW )
{    
    enum { Mask, DLen, D, DpGroup, DpIndex, PubGroup, PubIndex, End } field;

    VLT_STS       status;    
    VLT_U16       u16Offset;
    VLT_U16       u16Avail;
    VLT_U8        u8Mask;

    /*
     * Validate all input parameters.
     */
    if ( ( NULL == keyObj ) ||
         ( NULL == pSW ) ||         
         ( NULL == keyObj->pu8D ) )
    {
        return ( ERKEPRIVNULLPARA );
    }


    /* offset into output buffer */
    u16Offset = 0;                     
    
    /* skip bKeyID since caller handles it */
    idx = 1;
    u16Avail  = Response.u16Len - 1;
    /* Unpack the key object */

    /* We loop until we step beyond the last field. Buffers are incrementally
     * filled with data as it arrives. We can spend multiple iterations filling
     * in a single field. We must cope with values arriving in awkward ways,
     * e.g. a 2-byte field could arrive as the final byte and initial byte of
     * two separate requests. */

    for (field = Mask; field <= PubIndex; )
    {
        VLT_U16 u16BufLen;
        VLT_U16 u16BufferSize;
        VLT_PU8 pu8Buf;

        /* Fill the buffer up if it's empty */

        if (u16Avail == 0)
        {
            status = VltReadKeyCommand( &Command, &Response, u8KeyGroup,
                u8KeyIndex, pSW );

            if (VLT_OK != status)
            {
                return status;
            }

            /* use entire buffer this iteration */
            idx = 0; 
            u16Avail = Response.u16Len;
        }

        /* Turn each field into a buffer length and pointer */
        u8Mask = 0;

        switch( field )
        {
            case Mask:
                u16BufLen = 1;
                pu8Buf    = &keyObj->u8Mask;
                break;

            case DLen:
                u16BufLen = 2;
                pu8Buf    = (VLT_U8 *)&keyObj->u16DLen;
                u16BufferSize = keyObj->u16DLen;
                break;

            case D:
                u16BufLen = keyObj->u16DLen;
                pu8Buf    = keyObj->pu8D;
                u8Mask    = keyObj->u8Mask;
                break;

            case DpGroup:
                u16BufLen = 1;
                pu8Buf = (VLT_U8 *)&keyObj->u8DomainParamsGroup;
                break;

            case DpIndex:
                u16BufLen = 1;
                pu8Buf = (VLT_U8 *)&keyObj->u8DomainParamsIndex;
                break;

            case PubGroup:
                u16BufLen = 1;
                pu8Buf = (VLT_U8 *)&keyObj->u8PublicKeyGroup;
                break;

            case PubIndex:
                u16BufLen = 1;
                pu8Buf = (VLT_U8 *)&keyObj->u8PublicKeyIndex;
                break;

            default:
                return ERKBADFIELD;
        }

        /* Fill the buffer as much as possible */

        {
            VLT_U16 u16Remain;
            VLT_U16 u16Copy;

            u16Remain = u16BufLen - u16Offset;
            if( u16Avail > u16Remain )
            {
                u16Copy = u16Remain;
            }
            else
            {
                u16Copy = u16Avail;
            }

            if( ( field == DLen ) &&
                ( u16BufferSize < VltEndianReadPU16( &Response.pu8Data[idx] ) ) )
            {
                return ( ERKEPRIVNOROOM );
            }

            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memcpyxor( pu8Buf + u16Offset, &Response.pu8Data[idx], u16Copy, u8Mask );
            idx += u16Copy;
            u16Offset += u16Copy;
            u16Avail  -= u16Copy;

            if (u16Offset == u16BufLen) /* buffer full? */
            {
                /* When reading the length values we accumulate the bytes
                 * piecemeal in the 2-byte value itself, treating it as a small
                 * buffer. Once enough bytes are ready we (may) need to endian
                 * swap them, which we do here. */
                if( field == DLen )
                {
                    keyObj->u16DLen = VltEndianReadPU16(pu8Buf);
                }  

                u16Offset = 0;

                /* move to next field */
                field++; 
            }
        }
    }

    /* Validate received CRC */
    if( u16ReceivedCrc != u16CalculatedCrc )
    {
        return( ERKECDSAPRIVLDCRC );
    }

    return VLT_OK;
}




VLT_STS VltReadKey_EcdsaParams( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_ECDSA_PARAMS* keyObj,
    VLT_PSW pSW )
{    
    enum { QLen, Q, Gx, Gy, Gz, A, B, NLen, N, H, Assurance, End } field;

    VLT_STS status;
    VLT_U16 u16Offset;
    VLT_U16 u16Avail;


    /*
     * Validate all input parameters.
     */
    if( ( NULL == keyObj ) ||
        ( NULL == pSW ) ||
        ( NULL == keyObj->pu8A ) ||
        ( NULL == keyObj->pu8B ) ||
        ( NULL == keyObj->pu8Gx )||
        ( NULL == keyObj->pu8Gy )||
        ( NULL == keyObj->pu8Gz )||
        ( NULL == keyObj->pu8N ) ||
        ( NULL == keyObj->pu8Q ) )
    {
        return ( ERKECDSAPARAMSNULLPARA );
    }

    /* offset into output buffer */
    u16Offset = 0;                     
    
    /* skip bKeyID since caller handles it */
    idx = 1;
    u16Avail  = Response.u16Len - 1;

    /* Unpack the key object */

    /* We loop until we step beyond the last field. Buffers are incrementally
     * filled with data as it arrives. We can spend multiple iterations filling
     * in a single field. We must cope with values arriving in awkward ways,
     * e.g. a 2-byte field could arrive as the final byte and initial byte of
     * two separate requests. */

    for( field = QLen; field <= Assurance; )
    {
        VLT_U16 u16BufLen;
        VLT_U16 u16BufferSize;
        VLT_PU8 pu8Buf;

        /* Fill the buffer up if it's empty */

        if( u16Avail == 0 )
        {
            status = VltReadKeyCommand( &Command, &Response, u8KeyGroup,
                u8KeyIndex, pSW );

            if (VLT_OK != status)
            {
                return status;
            }

            /* use entire buffer this iteration */
            idx = 0; 
            u16Avail = Response.u16Len;
        }

        /* Turn each field into a buffer length and pointer */
        switch( field )
        {
            case QLen:
                u16BufLen = 2;
                pu8Buf = (VLT_U8 *)&keyObj->u16QLen;
                u16BufferSize = keyObj->u16QLen;
                break;

            case Q:
                u16BufLen = keyObj->u16QLen;
                pu8Buf = (VLT_U8 *)keyObj->pu8Q;
                break;

            case Gx:
                u16BufLen = keyObj->u16QLen;
                pu8Buf = (VLT_U8 *)keyObj->pu8Gx;
                break;

            case Gy:
                u16BufLen = keyObj->u16QLen;
                pu8Buf = (VLT_U8 *)keyObj->pu8Gy;
                break;

            case Gz:
                u16BufLen = keyObj->u16QLen;
                pu8Buf = (VLT_U8 *)keyObj->pu8Gz;
                break;

            case A:
                u16BufLen = keyObj->u16QLen;
                pu8Buf = (VLT_U8 *)keyObj->pu8A;
                break;

            case B:
                u16BufLen = keyObj->u16QLen;
                pu8Buf = (VLT_U8 *)keyObj->pu8B;
                break;

            case NLen:
                u16BufLen = 2;
                pu8Buf = (VLT_U8 *)&keyObj->u16NLen;
                u16BufferSize = keyObj->u16NLen;
                break;

            case N:
                u16BufLen = keyObj->u16NLen;
                pu8Buf = (VLT_U8 *)keyObj->pu8N;
                break;

            case H:
                u16BufLen = 4;
                pu8Buf = (VLT_U8 *)&keyObj->u32H;
                break;

            case Assurance:
                u16BufLen = 1;
                pu8Buf = (VLT_U8 *)&keyObj->u8Assurance;
                break;

            default:
                return ERKECDSAPARAMSBADFIELD;
        }

        /* Fill the buffer as much as possible */
        {
            VLT_U16 u16Remain;
            VLT_U16 u16Copy;

            u16Remain = u16BufLen - u16Offset;
            if (u16Avail > u16Remain)
            {
                u16Copy = u16Remain;
            }
            else
            {
                u16Copy = u16Avail;
            }

            if( ( ( field == QLen ) ||
                ( field == NLen ) ) &&
                ( u16BufferSize < VltEndianReadPU16( &Response.pu8Data[idx] ) ) )
            {
                return ( ERKECDSAPARAMSNOROOM );
            }
            
            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memcpy( pu8Buf + u16Offset, &Response.pu8Data[idx], u16Copy );
            idx += u16Copy;
            u16Offset += u16Copy;
            u16Avail -= u16Copy;

             /* buffer full? */
            if( u16Offset == u16BufLen )
            {
                /* When reading the length values we accumulate the bytes
                 * piecemeal in the 2-byte value itself, treating it as a small
                 * buffer. Once enough bytes are ready we (may) need to endian
                 * swap them, which we do here. */

                if( field == QLen )
                {
                    keyObj->u16QLen = VltEndianReadPU16(pu8Buf);                   
                }

                if( field == NLen )
                {
                    keyObj->u16NLen = VltEndianReadPU16(pu8Buf);
                }
                
                if( field == H )
                {
                    keyObj->u32H = VltEndianReadPU32(pu8Buf);
                }
                
                u16Offset = 0;

                field++; /* move to next field */
            }
        }
    }
    
    /* Validate received CRC */
    if( u16ReceivedCrc != u16CalculatedCrc )
    {
        return( ERKECDSAPARAMSIVLDCRC );
    }

    return VLT_OK;
}

#endif /* VLT_ENABLE_KEY_ECDSA == VLT_ENABLE */



