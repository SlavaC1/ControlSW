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


/* VltKey aux functions
 * ========================
 *
 * The VltReadKey aux functions retrieve key data from the VaultIC and unpack it
 * into a structure which is assumed to have been appropriately allocated by the
 * client.
 *
 * The cases listed earlier in the source are 'simple' and involve unpacking
 * only fixed-size data. The later cases, especially the private key cases, are
 * more complex and must cope with repeatedly re-issuing commands to VaultIC
 * until the decoding is complete.
 *
 * The routines are mostly very similar and could be merged together to save
 * space at the expense of making the simple cases use the more generic code.
 *
 * State variables are maintained which tell us which field we're processing and
 * the offset within that field. The routines are structured so that we unpack
 * until we run out of bytes at which point we fetch another full buffer. This
 * keeps the buffer requests as big as possible, minimising the number of
 * chunks/commands which need to be received. (The alternative would be to try
 * to receive one field at a time).
 *
 * Common variables:
 *
 * field        records the field of the output structure we're unpacking.
 * pu8Data      points to the next byte to be consumed in the input buffer.
 * u16Offset    records the offset within that field.
 * u16Avail     holds the number of available input bytes.
 * u16BufLen    holds the number of bytes in the buffer.
 * pu8Buf       points to the next available free byte in the output buffer. 
 *
 * Field Unpacking
 * ---------------
 * Fields are treated identically by the code. They're unpacked by copying the
 * bytes across into the output buffer. Where appropriate and when the buffer is
 * full the buffer contents are then endian-swapped in place.
 *
 * host_memcpyxor is often used irrespective of a field which requires masking.
 * For non-masked fields u8Mask will be zero.
 *
 * Strings
 * -------
 * Strings received from VaultIC are terminated and the size of the terminator
 * is included in the string length field. To ensure consistency with VltPutKey
 * the string length is decremented before being returned. (We don't attempt to
 * /not/ copy the terminator).
 *
 * CRCs
 * ----
 * CRCs are presently ignored (so not faulted if incorrect).
 */

/**
 * Externs 
 */
extern VLT_MEM_BLOB Command;                            /* declared in vaultic_api.c */
extern VLT_MEM_BLOB Response;                           /* declared in vaultic_api.c */
extern VLT_U8 apduHeader[VLT_APDU_TYPICAL_HEADER_SZ];   /* declared in vaultic_api.c */
extern VLT_U16 idx;                                     /* declared in vaultic_api.c */

/**
 * Local Static Variables
 */
VLT_U16 u16CalculatedCrc = VLT_CRC16_CCITT_INIT_0s;
VLT_U16 u16ReceivedCrc = 0xFFFF;


void ReadKeyInitCrc( void )
{
    u16CalculatedCrc = VLT_CRC16_CCITT_INIT_0s;
}

VLT_STS VltReadKeyCommand( VLT_MEM_BLOB *command,
    VLT_MEM_BLOB *response,
    VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,    
    VLT_PSW pSW )
{
    VLT_STS status;        
    idx = VLT_APDU_DATA_OFFSET;

    /* Check the pointers are valid */
    if ( ( NULL == command ) ||
         ( NULL == response ) ||
         ( NULL == pSW ) )
    {
        return ( ERKCMDNULLPARA );
    }

    /* Check the command and response buffer pointers are vaild.*/
    if ( ( NULL == Command.pu8Data ) ||
         ( NULL == Response.pu8Data ) )
    {
        return ( ERKCMDNULLPARA );
    }
    
    /* Build APDU */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_READ_KEY;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = u8KeyGroup;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = u8KeyIndex;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = WRAPPED_BYTE(256); /* request as much as possible */

    /* Send the command */
    status = VltCommand( &Command, &Response, idx, 0, pSW );

    /* adjust the response size to take in account the status word size */
    Response.u16Len -= VLT_SW_SIZE;
         
    if( VLT_OK != status)
    {
        return( status );
    }

    if( ( *pSW != VLT_STATUS_RESPONDING ) &&
        ( *pSW != VLT_STATUS_SUCCESS ) &&
        ( *pSW != VLT_STATUS_EOF )   )
    {
        return ERKINVLDRSP;
    }

    if( *pSW == VLT_STATUS_NONE )
    {
        return( status );
    }

    /* Update the CRC */
    if( *pSW == VLT_STATUS_SUCCESS )
    {      
        Response.u16Len -= NUM_CRC_BYTES;

        u16CalculatedCrc = VltCrc16Block( u16CalculatedCrc, 
            Response.pu8Data, 
            Response.u16Len );

        /* Retrieve received CRC */
        u16ReceivedCrc = VltEndianReadPU16( 
            &Response.pu8Data[ Response.u16Len ] );

    }else if( *pSW == VLT_STATUS_RESPONDING || *pSW == VLT_STATUS_EOF)
    {        
        u16CalculatedCrc = VltCrc16Block( u16CalculatedCrc, 
            Response.pu8Data, 
            Response.u16Len );
    }

    return VLT_OK;
}


