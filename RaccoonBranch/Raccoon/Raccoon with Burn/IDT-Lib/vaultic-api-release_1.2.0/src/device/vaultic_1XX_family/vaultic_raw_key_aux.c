/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#include "vaultic_apdu.h"
#include "vaultic_common.h"
#include "vaultic_api.h"
#include "vaultic_comms.h"
#include "vaultic_utils.h"
#include "vaultic_mem.h"
#include "vaultic_crc16.h"
#include "vaultic_command.h"
#include "vaultic_key_aux.h"
#include "vaultic_raw_key_aux.h"


/* VltPutKey aux functions
 * =======================
 *
 * The VltRawPutKey aux functions marshal the input key data into a fixed-size
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

#if( ( VLT_ENABLE_KEY_SECRET == VLT_ENABLE ) ||\
     ( VLT_ENABLE_KEY_HOTP == VLT_ENABLE ) ||\
     ( VLT_ENABLE_KEY_TOTP == VLT_ENABLE ) ||\
     ( VLT_ENABLE_KEY_RSA == VLT_ENABLE ) ||\
     ( VLT_ENABLE_KEY_DSA == VLT_ENABLE ) ||\
     ( VLT_ENABLE_KEY_ECDSA == VLT_ENABLE ) ||\
     ( VLT_ENABLE_KEY_HOST_DEVICE_ID == VLT_ENABLE ) )
#endif

VLT_STS VltPutKey_Raw( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
    const VLT_KEY_OBJ_RAW* pKeyObj,
    VLT_PSW pSW )
{
    VLT_STS status = VLT_FAIL;
    VLT_U16 u16Remaining = 0;
    VLT_U16 u16MaxChunk = 0;
    VLT_U16 u16Offset = 0;
    VLT_U16 u16KeyBytesRemaining = 0;
    
    /*
     * Validate all input parameters.
     */
    if( ( NULL == pKeyFilePrivileges ) ||
        ( NULL == pKeyObj ) ||
        ( NULL == pSW ) ||
        ( NULL == pKeyObj->pu16KeyObjectLen ) ||
        ( NULL == pKeyObj->pu8KeyObject ) )
    {
        return ( EPKRAWNULLPARA );
    }
    

    /* We need to split the data up into chunks, the size of which the comms
     * layer tells us. */
    u16MaxChunk = VltCommsGetMaxSendSize();

    u16Remaining = VLT_PUTKEY_FIXED_DATA_LENGTH + *pKeyObj->pu16KeyObjectLen;
    u16KeyBytesRemaining = *pKeyObj->pu16KeyObjectLen;

    while( 0 != u16Remaining )
    {
        VLT_U16 u16Chunk = 0;
        VLT_U16 u16PartialKeyLen = 0;
        VLT_U16 u16Avail = 0;

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
        if(u16Offset == 0) /* building the first part of the data */
        {
            /* bmAccess */
            Command.pu8Data[idx++] = pKeyFilePrivileges->u8Read;
            Command.pu8Data[idx++] = pKeyFilePrivileges->u8Write;
            Command.pu8Data[idx++] = pKeyFilePrivileges->u8Delete;
            Command.pu8Data[idx++] = pKeyFilePrivileges->u8Execute;

            /* wKeyObjectLength */
            Command.pu8Data[idx++] = 
                (VLT_U8)( (*pKeyObj->pu16KeyObjectLen >> 8 ) & 0xFF );

            Command.pu8Data[idx++] = 
                (VLT_U8)( (*pKeyObj->pu16KeyObjectLen >> 0 ) & 0xFF );
        }

        
        /* How much space is available for the key data? */
        u16Avail = NumBufferBytesAvail( u16Chunk, idx );

        if(u16KeyBytesRemaining > u16Avail)
        {
            u16PartialKeyLen = u16Avail;
        }
        else
        {
            u16PartialKeyLen = u16KeyBytesRemaining;
        }

        
        /* If 'u16Offset' has maxed out then u16PartialKeyLen could now be zero.
         * We need to cope with such cases as it's possible that the key data
         * will finish on an exact chunk boundary, leaving only the CRC to be
         * sent in its own chunk. */
        host_memcpy( &(Command.pu8Data[idx]),
            &( (pKeyObj->pu8KeyObject[u16Offset]) ), 
            u16PartialKeyLen  );

        idx += u16PartialKeyLen;
       
        /* If the remaining data is too big we'll need to send it in multiple
         * chunks. */
        if( 0 == u16Offset )
        {
            u16KeyBytesRemaining -= u16Avail; /* bytes of key remaining to be sent. */
        }
        else
        {
            u16KeyBytesRemaining -= NumBytesInBuffer( idx );
        }

        /* Decrement the remaining number of bytes to be sent. */
        u16Remaining -= NumBytesInBuffer( idx );

        /* Update the offset into the key*/
        u16Offset += u16PartialKeyLen;
        
        /* It's entirely possible that we will fall through the above code with
         * 'u16Remaining' at zero and enter here where we construct a data block
         * only containing the wCRC data. */

        /* We need two bytes free in the buffer for the wCRC field. */
        if( ( NUM_CRC_BYTES == u16Remaining ) &&
            ( NumBufferBytesAvail( u16Chunk, idx  ) >= NUM_CRC_BYTES ) )
        {
            Command.pu8Data[idx++] = (VLT_U8) ((pKeyObj->u16Crc >> 8) & 0xFF);
            Command.pu8Data[idx++] = (VLT_U8) ((pKeyObj->u16Crc >> 0) & 0xFF);
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

    return ( status );
}




VLT_STS VltReadKey_Raw( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_RAW* keyObj,
    VLT_PSW pSW )
{
    VLT_STS status = VLT_FAIL;
    VLT_U16 u16KeyObjLen = 0;
    VLT_BOOL bReadComplete = FALSE;
    VLT_U16 u16RequestedLen = 0;
    
    if( ( NULL == keyObj ) ||
        ( NULL == pSW ) ||
        ( NULL == keyObj->pu16KeyObjectLen ) ||
        ( NULL == keyObj->pu8KeyObject ) )
    {
        return ( ERKRAWNULLPARA );
    }

    
    
    // Safe to use the ptr to the key object length
    u16RequestedLen = *keyObj->pu16KeyObjectLen;
    

    do
    {
        /* Copy the data in the user's buffer if we have enough space */
        if( ( u16KeyObjLen + Response.u16Len ) <= u16RequestedLen )
        {
            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memcpy( &keyObj->pu8KeyObject[ u16KeyObjLen ], 
                Response.pu8Data, 
                Response.u16Len );
        }        

        /* Udpate the length */
        u16KeyObjLen += Response.u16Len;

        if( *pSW == VLT_STATUS_SUCCESS )
        {
            /* We have received the whole key exit the loop */
            bReadComplete = TRUE;

            /* Validate received CRC */
            if( u16ReceivedCrc != u16CalculatedCrc )
            {
                return( ERKSCIVLDCRC );
            }
            else
            {
                /* Assign the received CRC value into the struct
                 returned to the host side caller. */
                keyObj->u16Crc = u16ReceivedCrc;
            }
        }
        else if( *pSW == VLT_STATUS_RESPONDING )
        {
            /* read more data */
            status = VltReadKeyCommand( &Command, 
                &Response, 
                u8KeyGroup,
                u8KeyIndex, 
                pSW );

            if( VLT_OK != status )
            {
                return( status );
            }
        }
        else
        {
            return( *pSW );
        }
    }
    while( bReadComplete == FALSE );

    /** 
     * If we have run out of space let the caller know
     * the true length of the key requested and return
     * the appropriate error code.
     */
    if( u16KeyObjLen > u16RequestedLen )
    {
        *keyObj->pu16KeyObjectLen = u16KeyObjLen;
        return( ERKRAWNOROOM );
    }

    return( VLT_OK );
}


