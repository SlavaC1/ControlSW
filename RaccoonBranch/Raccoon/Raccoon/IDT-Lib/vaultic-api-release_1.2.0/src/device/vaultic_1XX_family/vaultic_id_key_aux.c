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
#include "vaultic_id_key_aux.h"


/* VltPutKey aux functions
 * =======================
 *
 * The VltIDPutKey aux functions marshal the input key data into a fixed-size
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

#if VLT_ENABLE_KEY_HOST_DEVICE_ID == VLT_ENABLE
    static VLT_U16 crcStartPos;
#endif /* VLT_ENABLE_KEY_HOST_DEVICE_ID == VLT_ENABLE */


#if VLT_ENABLE_KEY_HOST_DEVICE_ID == VLT_ENABLE

VLT_STS VltPutKey_IdKey( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
    VLT_U8 u8KeyID,
    const VLT_KEY_OBJ_ID* pKeyObj,
    VLT_PSW pSW )
{
    

    VLT_STS status = VLT_FAIL;
    VLT_U16 u16KeyObjLen;
    VLT_U16 u16Crc = VLT_CRC16_CCITT_INIT_0s;

    /*
     * Validate all input parameters.
     */
    if ( ( NULL == pKeyFilePrivileges ) ||
         ( NULL == pKeyObj ) ||
         ( NULL == pSW ) ||
         ( NULL == pKeyObj->pu8StringId ) )
    {
        return ( EPKIDNULLPARA );
    }

    /*
     * Check the key object length is equal to the fixed ID lengths
     */
    if ( VLT_HOST_DEV_ID_STRING_LENGTH != pKeyObj->u16StringLen )
    {
        return ( ERKIDNOROOM );
    }
    
    /*
    * Work out the size of abKeyObject.
    */
    u16KeyObjLen = VLT_HOST_DEV_ID_STATIC_PART_LENGTH + pKeyObj->u16StringLen;

    /* 
    * Build the command header
    */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL; 
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_PUT_KEY;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = u8KeyGroup;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = u8KeyIndex;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = LIN(WRAPPED_BYTE(
        VLT_PUTKEY_FIXED_DATA_LENGTH + u16KeyObjLen));

    /* set the index offset */
    idx = VLT_APDU_DATA_OFFSET;
    
    /* bmAccess */
    Command.pu8Data[idx++] = pKeyFilePrivileges->u8Read;
    Command.pu8Data[idx++] = pKeyFilePrivileges->u8Write;
    Command.pu8Data[idx++] = pKeyFilePrivileges->u8Delete;
    Command.pu8Data[idx++] = pKeyFilePrivileges->u8Execute;

    /* wKeyObjectLength */
    Command.pu8Data[idx++] = (VLT_U8) ((u16KeyObjLen >> 8) & 0xFF);
    Command.pu8Data[idx++] = (VLT_U8) ((u16KeyObjLen >> 0) & 0xFF);
    

    /* abKeyObject data */
    crcStartPos = idx;

    /* bKeyID */
    Command.pu8Data[idx++] = u8KeyID;

    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( &(Command.pu8Data[idx]),
        &pKeyObj->pu8StringId[0],
        pKeyObj->u16StringLen );

    idx += pKeyObj->u16StringLen;
    
   

    /* Update the CRC-16 with the (partial) data. */
    u16Crc = VltCrc16Block( u16Crc,
        &(Command.pu8Data[crcStartPos]),
        ( idx  - crcStartPos ) );

    /* Emit the CRC-16 once there's no data remaining. */

    

    /* We need two bytes free in the buffer for the wCRC field. */
    Command.pu8Data[idx++] = (VLT_U8) ((u16Crc >> 8) & 0xFF);
    Command.pu8Data[idx++] = (VLT_U8) ((u16Crc >> 0) & 0xFF);
    
    

    /* Send the command */
    status = VltCommand( &Command, &Response, idx, 0, pSW );
    if(VLT_OK != status)
    {
        return status;
    }

    /* Let put key decide on the action to take upon status and SW. */

    return( status );
}


VLT_STS VltReadKey_IdKey( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_ID* keyObj,
    VLT_PSW pSW )
{    
    VLT_U16 u16Avail;
    VLT_U8  u8KeyDataLen = VLT_HOST_DEV_ID_STRING_LENGTH + 1;
    
    /*
     * Validate all input parameters.
     */
    if( ( NULL == keyObj ) ||
        ( NULL == pSW ) ||
        ( NULL == keyObj->pu8StringId ) )
    {
        return ( ERKIDNULLPARA );
    }

    
    /*Inititalize the CRC for the read key*/
    ReadKeyInitCrc();
    
    u16CalculatedCrc = VltCrc16Block(u16CalculatedCrc, Response.pu8Data, u8KeyDataLen);
    u16ReceivedCrc = VltEndianReadPU16(&Response.pu8Data[u8KeyDataLen]);
    
    /* Calculate the length of the Key ID string. */
    idx = 1;
    u16Avail  = Response.u16Len - 1;

    
    /* Verify the keyID string is equal to the expected length. */
    if( ( VLT_HOST_DEV_ID_STRING_LENGTH != u16Avail ) && 
        ( VLT_HOST_DEV_ID_STRING_LENGTH != keyObj->u16StringLen ) )
    {
        return ( ERKIDNOROOM );
    }
    

    /* Assign the length of the ID string */
    keyObj->u16StringLen = u16Avail;
    

    /* No need to check the return type as pointer has been validated */
    (void)host_memcpy( keyObj->pu8StringId, 
        &Response.pu8Data[idx], 
        VLT_HOST_DEV_ID_STRING_LENGTH );
    
    
    /* Validate received CRC */
    if( u16ReceivedCrc != u16CalculatedCrc )
    {
        return( ERKIDIVLDCRC );
    }

    return VLT_OK;
}

#endif /* VLT_ENABLE_KEY_HOST_DEVICE_ID == VLT_ENABLE */