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
#include "vaultic_cipher.h"
#include "vaultic_cipher_tests.h"
#include "vaultic_crc16.h"
#include "vaultic_command.h"
#include "vaultic_readkey_aux.h"


/* VltReadKey aux functions
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
static VLT_U16 u16CalculatedCrc = VLT_CRC16_CCITT_INIT_0s;
static VLT_U16 u16ReceivedCrc = 0xFFFF;


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
        ( *pSW != VLT_STATUS_SUCCESS ) )
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

    }else if( *pSW == VLT_STATUS_RESPONDING  )
    {        
        u16CalculatedCrc = VltCrc16Block( u16CalculatedCrc, 
            Response.pu8Data, 
            Response.u16Len );
    }

    return VLT_OK;
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
        ( NULL == keyObj->pu8KeyObject ) )
    {
        return ( ERKRAWNULLPARA );
    }

    if( ( NULL == keyObj->pu16ClearKeyObjectLen ) &&
        ( NULL == keyObj->pu16EncKeyObjectLen ) )
    {
        return ( ERKRAWNULLPARA );
    }
    
    // Safe to use the ptr to the key object length
	u16RequestedLen = (TRUE==keyObj->isEncryptedKey)?*keyObj->pu16EncKeyObjectLen:*keyObj->pu16ClearKeyObjectLen;
    
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
		if( FALSE == keyObj->isEncryptedKey ) 
		{
			*keyObj->pu16ClearKeyObjectLen = u16KeyObjLen;
		}
		else
		{
			*keyObj->pu16EncKeyObjectLen = u16KeyObjLen;
		}
        return( ERKRAWNOROOM );
    }

    return( VLT_OK );
}

/* The VltReadKey_* functions are called once a first buffer of data is
 * available. If there are more than '256' bytes to receive (i.e. more than a
 * single chunk of data) these function will make more requests to retrieve it.
 */

#if( VLT_ENABLE_KEY_SECRET == VLT_ENABLE )

VLT_STS VltReadKey_Secret(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_SECRET* keyObj,
    VLT_PSW pSW)
{    
    /* Start unpacking after the bKeyID */
    idx = 1;

    if ( ( NULL == keyObj ) ||
         ( NULL == pSW ) ||
         ( NULL == keyObj->pu8Key ) ||
         ( 0 == keyObj->u16KeyLength ) )
    {
        return ( ERKSECNULLPARA );
    }
    
    /* We're expecting the entire response to be in the buffer */
    if (*pSW != VLT_STATUS_SUCCESS)
    {
        return ERKINVLDRSP;
    }        

    /* Unpack the key object */
    keyObj->u8Mask = Response.pu8Data[idx++];

    /* Check there is enough room to store the key object in the host buffer.*/
    /* the actual length is stored in keyObj->u16KeyLength. */ 
    if ( keyObj->u16KeyLength < VltEndianReadPU16( &Response.pu8Data[idx] ) )
    {
        return ( ERKSECNOROOM );
    }

    keyObj->u16KeyLength = VltEndianReadPU16( &Response.pu8Data[idx] );
    idx += 2;

    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpyxor( keyObj->pu8Key, 
        &Response.pu8Data[idx], 
        keyObj->u16KeyLength, 
        keyObj->u8Mask );
    
    /* Validate received CRC */
    if( u16ReceivedCrc != u16CalculatedCrc )
    {
        return( ERKSCIVLDCRC );
    }
     
    return VLT_OK;
}

#endif /* ( VLT_ENABLE_KEY_SECRET == VLT_ENABLE ) */

#if( VLT_ENABLE_KEY_HOTP == VLT_ENABLE )

VLT_STS VltReadKey_Hotp( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_HOTP* keyObj,
    VLT_PSW pSW)
{    
    /* Start unpacking after the bKeyID */
    idx = 1;

    /*
     * Validate all input parameters.
     */
    if ( ( NULL == keyObj ) ||
         ( NULL == pSW ) ||
         ( NULL == keyObj->pu8Key ) ||
         ( NULL == keyObj->pu8MovingFactor ) )
    {
        return ( ERKHPNULLPARA );
    }

    /* We're expecting the entire response to be in the buffer */
    if (*pSW != VLT_STATUS_SUCCESS)
    {
        return ERKINVLDRSP;
    }

    /* Unpack the key object */
    keyObj->u8Mask = Response.pu8Data[idx++];

    /* Check there is enough room to store the key object in the host buffer.*/
    if ( keyObj->u16KeyLength < VltEndianReadPU16( &Response.pu8Data[idx] ) )
    {
        return ( ERKHPNOROOM );
    }

    keyObj->u16KeyLength = VltEndianReadPU16( &Response.pu8Data[idx] );
    idx += 2;

    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpyxor( keyObj->pu8Key, 
        &Response.pu8Data[idx], 
        keyObj->u16KeyLength, 
        keyObj->u8Mask );

    idx += keyObj->u16KeyLength;

    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( keyObj->pu8MovingFactor, 
        &Response.pu8Data[idx],  
        VLT_KEY_HOTP_MOVINGFACTOR_LENGTH );

    /* Validate received CRC */
    if( u16ReceivedCrc != u16CalculatedCrc )
    {
        return( ERKHOTPIVLDCRC );
    }
     
    return VLT_OK;
}

#endif /* ( VLT_ENABLE_KEY_HOTP == VLT_ENABLE ) */

#if( VLT_ENABLE_KEY_TOTP == VLT_ENABLE )

VLT_STS VltReadKey_Totp( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_TOTP* keyObj,
    VLT_PSW pSW )
{
    /* Start unpacking after the bKeyID */
    idx = 1;

    /*
     * Validate all input parameters.
     */
    if ( ( NULL == keyObj ) ||
         ( NULL == pSW ) ||
         ( NULL == keyObj->pu8Key ) )
    {
        return ( ERKTPNULLPARA );
    }

    /* We're expecting the entire response to be in the buffer */
    if (*pSW != VLT_STATUS_SUCCESS)
    {
        return ERKINVLDRSP;
    }
    

    /* Unpack the key object */
    keyObj->u8Mask = Response.pu8Data[idx++];

    /* Check there is enough room to store the key object in the host buffer.*/
    if ( keyObj->u16KeyLength < VltEndianReadPU16( &Response.pu8Data[idx] ) )
    {
        return ( ERKTPNOROOM );
    }
    
    keyObj->u16KeyLength = VltEndianReadPU16( &Response.pu8Data[idx] );
    idx += 2;
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpyxor( keyObj->pu8Key, 
        &Response.pu8Data[idx], 
        keyObj->u16KeyLength, 
        keyObj->u8Mask );

    /* Validate received CRC */
    if( u16ReceivedCrc != u16CalculatedCrc )
    {
        return( ERKTOTPIVLDCRC );
    }

    return VLT_OK;
}

#endif /* ( VLT_ENABLE_KEY_TOTP == VLT_ENABLE ) */

#if( VLT_ENABLE_KEY_RSA == VLT_ENABLE )

#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X )
VLT_STS VltReadKey_RsaPublic( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_RSA_PUB* keyObj,
    VLT_PSW pSW )
{
    enum { NLen, N, ELen, E, End } field;

    VLT_STS status;    
    VLT_U16 u16Offset;
    VLT_U16 u16Avail;

    /*
     * Validate all input parameters.
     */
    if ( ( NULL == keyObj ) ||
         ( NULL == pSW ) ||
         ( NULL == keyObj->pu8E ) ||
         ( NULL == keyObj->pu8N ) )
    {
        return ( ERKRPUBNULLPARA );
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

    for (field = NLen; field <= E; )
    {
        VLT_U16 u16BufLen;
        VLT_U16 u16BufferSize;
        VLT_PU8 pu8Buf;

        /* Fill the buffer up if it's empty */

        if (u16Avail == 0)
        {

            status = VltReadKeyCommand( &Command, 
                &Response, 
                u8KeyGroup,
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

        switch (field)
        {
        case NLen:
            u16BufLen = 2;
            pu8Buf = (VLT_U8 *)&keyObj->u16NLen;
            u16BufferSize = keyObj->u16NLen;
            break;

        case N:
            u16BufLen = keyObj->u16NLen;
            pu8Buf = keyObj->pu8N;
            break;

        case ELen:
            u16BufLen = 2;
            pu8Buf = (VLT_U8 *)&keyObj->u16ELen;
            u16BufferSize = keyObj->u16ELen;
            break;

        case E:
            u16BufLen = keyObj->u16ELen;
            pu8Buf = keyObj->pu8E;
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

            /* Check there is enough room to store the N - Modulus, 
               and E - Public Exponent before updating NLen or ELen. */ 
            if ( ( ( field == NLen ) ||
                   ( field == ELen ) ) &&
                   ( u16BufferSize < VltEndianReadPU16( &Response.pu8Data[idx] ) ) )
            {
                return ( ERKRPUBNOROOM );
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
                if (field == NLen)
                {
                    keyObj->u16NLen = VltEndianReadPU16(pu8Buf);
                }
                else if (field == ELen)
                {
                    keyObj->u16ELen = VltEndianReadPU16(pu8Buf);
                }

                u16Offset = 0;

                field++; /* move to next field */
            }
        }
    }
    
    /* Validate received CRC */
    if( u16ReceivedCrc != u16CalculatedCrc )
    {
        return( ERKRSAPBIVLDCRC );
    }

    return VLT_OK;
}

VLT_STS VltReadKey_RsaPrivate(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_RSA_PRIV* keyObj,
    VLT_PSW pSW)
{
    enum { Mask, NLen, N, DLen, D, End } field;

    VLT_STS status;    
    VLT_U16 u16Offset;
    VLT_U16 u16Avail;
    VLT_U8 u8Mask;

    /*
     * Validate all input parameters.
     */
    if ( ( NULL == keyObj ) ||
         ( NULL == pSW ) ||
         ( NULL == keyObj->pu8D ) ||
         ( NULL == keyObj->pu8N ) )
    {
        return ( ERKRPRIVNULLPARA );
    }

    
    u16Offset = 0;                     /* offset into output buffer */
    
    /* skip bKeyID since caller handles it */
    idx = 1;
    u16Avail = Response.u16Len - 1 ;

    /* Unpack the key object */

    /* We loop until we step beyond the last field. Buffers are incrementally
     * filled with data as it arrives. We can spend multiple iterations filling
     * in a single field. We must cope with values arriving in awkward ways,
     * e.g. a 2-byte field could arrive as the final byte and initial byte of
     * two separate requests. */

    for (field = Mask; field <= D; )
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

        switch (field)
        {
            case Mask:
                u16BufLen = 1;
                pu8Buf = &keyObj->u8Mask;
                break;

            case NLen:
                u16BufLen = 2;
                pu8Buf = (VLT_U8 *) &keyObj->u16NLen;
                u16BufferSize = keyObj->u16NLen;
                break;

            case N:
                u16BufLen = keyObj->u16NLen;
                pu8Buf = keyObj->pu8N;
                break;

            case DLen:
                u16BufLen = 2;
                pu8Buf = (VLT_U8 *) &keyObj->u16DLen;
                u16BufferSize = keyObj->u16DLen;
                break;

            case D:
                u16BufLen = keyObj->u16DLen;
                pu8Buf = keyObj->pu8D;
                u8Mask = keyObj->u8Mask;
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

            /* Check there is enough room to store the N - Modulus, 
               and D - Private Exponent before updating NLen or DLen. */ 
            if ( ( ( field == NLen ) ||
                   ( field == DLen ) ) &&
                   ( u16BufferSize < VltEndianReadPU16( &Response.pu8Data[idx] ) ) )
            {
                return ( ERKRPRIVNOROOM );
            } 

            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memcpyxor( pu8Buf + u16Offset, &Response.pu8Data[idx], u16Copy, u8Mask );
            idx += u16Copy;
            u16Offset += u16Copy;
            u16Avail -= u16Copy;

            if( u16Offset == u16BufLen ) /* buffer full? */
            {
                /* When reading the length values we accumulate the bytes
                 * piecemeal in the 2-byte value itself, treating it as a small
                 * buffer. Once enough bytes are ready we (may) need to endian
                 * swap them, which we do here. */

                if (field == NLen)
                {
                    keyObj->u16NLen = VltEndianReadPU16(pu8Buf);
                }
                else if (field == DLen)
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
        return( ERKRSAPRIVLDCRC );
    }
    
    return VLT_OK;
}

VLT_STS VltReadKey_RsaPrivateCrt( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_RSA_PRIV_CRT* keyObj,
    VLT_PSW pSW )
{    
    enum { Mask, PLen, P, Q, DP, DQ, IP, End } field;

    VLT_STS       status;
    VLT_U16       u16Offset;
    VLT_U16       u16Avail;
    VLT_U8        u8Mask;

    /*
     * Validate all input parameters.
     */
    if ( ( NULL == keyObj ) ||
         ( NULL == pSW ) ||
         ( NULL == keyObj->pu8Dp ) ||
         ( NULL == keyObj->pu8Dq ) ||
         ( NULL == keyObj->pu8Ip ) ||
         ( NULL == keyObj->pu8P ) ||
         ( NULL == keyObj->pu8Q ) )
    {
        return ( ERKRCRTNULLPARA );
    }


    /* offset into output buffer */
    u16Offset = 0;                     
    
    /* skip bKeyID since caller handles it */        
    idx = 1;
    u16Avail = Response.u16Len - 1 ;

    /* Unpack the key object */

    /* We loop until we step beyond the last field. Buffers are incrementally
     * filled with data as it arrives. We can spend multiple iterations filling
     * in a single field. We must cope with values arriving in awkward ways,
     * e.g. a 2-byte field could arrive as the final byte and initial byte of
     * two separate requests. */

    for( field = Mask; field <= IP; )
    {
        VLT_U16 u16BufLen;
        VLT_PU8 pu8Buf;

        /* Fill the buffer up if it's empty */

        if( 0 == u16Avail )
        {

            status = VltReadKeyCommand( &Command, &Response, u8KeyGroup,
                u8KeyIndex, pSW);

            if (VLT_OK != status)
            {
                return status;
            }

            /* use entire buffer this iteration */
            idx = 0;
            u16Avail = Response.u16Len ;
        }

        /* Turn each field into a buffer length and pointer */
        u8Mask = 0;

        switch (field)
        {
            case Mask:
                u16BufLen = 1;
                pu8Buf    = &keyObj->u8Mask;
                break;

            case PLen:
                u16BufLen = 2;
                pu8Buf    = (VLT_U8 *) &keyObj->u16PLen;
                break;

            case P:
                u16BufLen = keyObj->u16PLen;
                pu8Buf    = keyObj->pu8P;
                u8Mask    = keyObj->u8Mask;
                break;

            case Q:
                u16BufLen = keyObj->u16PLen;
                pu8Buf    = keyObj->pu8Q;
                u8Mask    = keyObj->u8Mask;
                break;

            case DP:
                u16BufLen = keyObj->u16PLen;
                pu8Buf    = keyObj->pu8Dp;
                u8Mask    = keyObj->u8Mask;
                break;

            case DQ:
                u16BufLen = keyObj->u16PLen;
                pu8Buf    = keyObj->pu8Dq;
                u8Mask    = keyObj->u8Mask;
                break;

            case IP:
                u16BufLen = keyObj->u16PLen;
                pu8Buf    = keyObj->pu8Ip;
                u8Mask    = keyObj->u8Mask;
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

            /* Check there is enough room to store the prime P 
               before updating PLen. */
            if ( ( field == PLen ) &&
                 ( keyObj->u16PLen < 
                   VltEndianReadPU16( &Response.pu8Data[idx] ) ) )
            {
                return ( ERKRCRTNOROOM );
            }

            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memcpyxor( pu8Buf + u16Offset, &Response.pu8Data[idx], u16Copy, u8Mask);
            idx += u16Copy;
            u16Offset += u16Copy;
            u16Avail -= u16Copy;

            if (u16Offset == u16BufLen) /* buffer full? */
            {
                /* When reading the length values we accumulate the bytes
                 * piecemeal in the 2-byte value itself, treating it as a small
                 * buffer. Once enough bytes are ready we (may) need to endian
                 * swap them, which we do here. */

                if (field == PLen)
                {
                    keyObj->u16PLen = VltEndianReadPU16(pu8Buf);
                }

                u16Offset = 0;

                field++; /* move to next field */
            }
        }
    }
    
    /* Validate received CRC */
    if( u16ReceivedCrc != u16CalculatedCrc )
    {
        return( ERKRSACRIVLDCRC );
    }

    return VLT_OK;
}
#endif /* (VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X) */

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)
VLT_STS VltReadKey_RsaPublic( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_RSA_PUB* keyObj,
    VLT_PSW pSW )
{
    enum { NLen, N, ELen, E, Assurance, End } field;

    VLT_STS status;    
    VLT_U16 u16Offset;
    VLT_U16 u16Avail;

    /*
     * Validate all input parameters.
     */
    if ( ( NULL == keyObj ) ||
         ( NULL == pSW ) ||
         ( NULL == keyObj->pu8E ) ||
         ( NULL == keyObj->pu8N ) )
    {
        return ( ERKRPUBNULLPARA );
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

    for (field = NLen; field <= /*Assurance*/E; )
    {
        VLT_U16 u16BufLen;
        VLT_U16 u16BufferSize;
        VLT_PU8 pu8Buf;

        /* Fill the buffer up if it's empty */

        if (u16Avail == 0)
        {

            status = VltReadKeyCommand( &Command, 
                &Response, 
                u8KeyGroup,
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

        switch (field)
        {
        case NLen:
            u16BufLen = 2;
            pu8Buf = (VLT_U8 *)&keyObj->u16NLen;
            u16BufferSize = keyObj->u16NLen;
            break;

        case N:
            u16BufLen = keyObj->u16NLen;
            pu8Buf = keyObj->pu8N;
            break;

        case ELen:
            u16BufLen = 2;
            pu8Buf = (VLT_U8 *)&keyObj->u16ELen;
            u16BufferSize = keyObj->u16ELen;
            break;

        case E:
            u16BufLen = keyObj->u16ELen;
            pu8Buf = keyObj->pu8E;
            break;

     /*   case Assurance:
            u16BufLen = 1;
            pu8Buf = (VLT_U8 *)&keyObj->u8Assurance;
            break;*/

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

            /* Check there is enough room to store the N - Modulus, 
               and E - Public Exponent before updating NLen or ELen. */ 
            if ( ( ( field == NLen ) ||
                   ( field == ELen ) ) &&
                   ( u16BufferSize < VltEndianReadPU16( &Response.pu8Data[idx] ) ) )
            {
                return ( ERKRPUBNOROOM );
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
                if (field == NLen)
                {
                    keyObj->u16NLen = VltEndianReadPU16(pu8Buf);
                }
                else if (field == ELen)
                {
                    keyObj->u16ELen = VltEndianReadPU16(pu8Buf);
                }

                u16Offset = 0;

                field++; /* move to next field */
            }
        }
    }
    
    /* Validate received CRC */
    if( u16ReceivedCrc != u16CalculatedCrc )
    {
        return( ERKRSAPBIVLDCRC );
    }

    return VLT_OK;
}

VLT_STS VltReadKey_RsaPrivate(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_RSA_PRIV* keyObj,
    VLT_PSW pSW)
{
    enum { Mask, NLen, N, DLen, D, PbGroup, PbIndex, End } field;

    VLT_STS status;    
    VLT_U16 u16Offset;
    VLT_U16 u16Avail;
    VLT_U8 u8Mask;

    /*
     * Validate all input parameters.
     */
    if ( ( NULL == keyObj ) ||
         ( NULL == pSW ) ||
         ( NULL == keyObj->pu8D ) ||
         ( NULL == keyObj->pu8N ) )
    {
        return ( ERKRPRIVNULLPARA );
    }

    
    u16Offset = 0;                     /* offset into output buffer */
    
    /* skip bKeyID since caller handles it */
    idx = 1;
    u16Avail = Response.u16Len - 1 ;

    /* Unpack the key object */

    /* We loop until we step beyond the last field. Buffers are incrementally
     * filled with data as it arrives. We can spend multiple iterations filling
     * in a single field. We must cope with values arriving in awkward ways,
     * e.g. a 2-byte field could arrive as the final byte and initial byte of
     * two separate requests. */

    for( field = Mask; field <= PbIndex; )
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

        switch (field)
        {
            case Mask:
                u16BufLen = 1;
                pu8Buf = &keyObj->u8Mask;
                break;

            case NLen:
                u16BufLen = 2;
                pu8Buf = (VLT_U8 *) &keyObj->u16NLen;
                u16BufferSize = keyObj->u16NLen;
                break;

            case N:
                u16BufLen = keyObj->u16NLen;
                pu8Buf = keyObj->pu8N;
                break;

            case DLen:
                u16BufLen = 2;
                pu8Buf = (VLT_U8 *) &keyObj->u16DLen;
                u16BufferSize = keyObj->u16DLen;
                break;

            case D:
                u16BufLen = keyObj->u16DLen;
                pu8Buf = keyObj->pu8D;
                u8Mask = keyObj->u8Mask;
                break;

            case PbGroup:
                u16BufLen = 1;
                pu8Buf = (VLT_U8 *)&keyObj->u8PublicKeyGroup;                
                break;

            case PbIndex:
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

            /* Check there is enough room to store the N - Modulus, 
               and D - Private Exponent before updating NLen or DLen. */ 
            if ( ( ( field == NLen ) ||
                   ( field == DLen ) ) &&
                   ( u16BufferSize < VltEndianReadPU16( &Response.pu8Data[idx] ) ) )
            {
                return ( ERKRPRIVNOROOM );
            } 

            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memcpyxor( pu8Buf + u16Offset, &Response.pu8Data[idx], u16Copy, u8Mask );
            idx += u16Copy;
            u16Offset += u16Copy;
            u16Avail -= u16Copy;

            if( u16Offset == u16BufLen ) /* buffer full? */
            {
                /* When reading the length values we accumulate the bytes
                 * piecemeal in the 2-byte value itself, treating it as a small
                 * buffer. Once enough bytes are ready we (may) need to endian
                 * swap them, which we do here. */

                if (field == NLen)
                {
                    keyObj->u16NLen = VltEndianReadPU16(pu8Buf);
                }
                else if (field == DLen)
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
        return( ERKRSAPRIVLDCRC );
    }
    
    return VLT_OK;
}

VLT_STS VltReadKey_RsaPrivateCrt( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_RSA_PRIV_CRT* keyObj,
    VLT_PSW pSW )
{    
    enum { Mask, PLen, P, Q, DP, DQ, IP, PbGroup, PbIndex, End } field;

    VLT_STS       status;
    VLT_U16       u16Offset;
    VLT_U16       u16Avail;
    VLT_U8        u8Mask;

    /*
     * Validate all input parameters.
     */
    if ( ( NULL == keyObj ) ||
         ( NULL == pSW ) ||
         ( NULL == keyObj->pu8Dp ) ||
         ( NULL == keyObj->pu8Dq ) ||
         ( NULL == keyObj->pu8Ip ) ||
         ( NULL == keyObj->pu8P ) ||
         ( NULL == keyObj->pu8Q ) )
    {
        return ( ERKRCRTNULLPARA );
    }


    /* offset into output buffer */
    u16Offset = 0;                     
    
    /* skip bKeyID since caller handles it */        
    idx = 1;
    u16Avail = Response.u16Len - 1 ;

    /* Unpack the key object */

    /* We loop until we step beyond the last field. Buffers are incrementally
     * filled with data as it arrives. We can spend multiple iterations filling
     * in a single field. We must cope with values arriving in awkward ways,
     * e.g. a 2-byte field could arrive as the final byte and initial byte of
     * two separate requests. */

    for( field = Mask; field <= PbIndex; )
    {
        VLT_U16 u16BufLen;
        VLT_PU8 pu8Buf;

        /* Fill the buffer up if it's empty */

        if( 0 == u16Avail )
        {

            status = VltReadKeyCommand( &Command, &Response, u8KeyGroup,
                u8KeyIndex, pSW);

            if (VLT_OK != status)
            {
                return status;
            }

            /* use entire buffer this iteration */
            idx = 0;
            u16Avail = Response.u16Len ;
        }

        /* Turn each field into a buffer length and pointer */
        u8Mask = 0;

        switch (field)
        {
            case Mask:
                u16BufLen = 1;
                pu8Buf    = &keyObj->u8Mask;
                break;

            case PLen:
                u16BufLen = 2;
                pu8Buf    = (VLT_U8 *) &keyObj->u16PLen;
                break;

            case P:
                u16BufLen = keyObj->u16PLen;
                pu8Buf    = keyObj->pu8P;
                u8Mask    = keyObj->u8Mask;
                break;

            case Q:
                u16BufLen = keyObj->u16PLen;
                pu8Buf    = keyObj->pu8Q;
                u8Mask    = keyObj->u8Mask;
                break;

            case DP:
                u16BufLen = keyObj->u16PLen;
                pu8Buf    = keyObj->pu8Dp;
                u8Mask    = keyObj->u8Mask;
                break;

            case DQ:
                u16BufLen = keyObj->u16PLen;
                pu8Buf    = keyObj->pu8Dq;
                u8Mask    = keyObj->u8Mask;
                break;

            case IP:
                u16BufLen = keyObj->u16PLen;
                pu8Buf    = keyObj->pu8Ip;
                u8Mask    = keyObj->u8Mask;
                break;

            case PbGroup:
                u16BufLen = 1;
                pu8Buf = (VLT_U8 *)&keyObj->u8PublicKeyGroup;                
                break;

            case PbIndex:
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
            if (u16Avail > u16Remain)
            {
                u16Copy = u16Remain;
            }
            else
            {
                u16Copy = u16Avail;
            }

            /* Check there is enough room to store the prime P 
               before updating PLen. */
            if ( ( field == PLen ) &&
                 ( keyObj->u16PLen < 
                   VltEndianReadPU16( &Response.pu8Data[idx] ) ) )
            {
                return ( ERKRCRTNOROOM );
            }

            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memcpyxor( pu8Buf + u16Offset, &Response.pu8Data[idx], u16Copy, u8Mask);
            idx += u16Copy;
            u16Offset += u16Copy;
            u16Avail -= u16Copy;

            if (u16Offset == u16BufLen) /* buffer full? */
            {
                /* When reading the length values we accumulate the bytes
                 * piecemeal in the 2-byte value itself, treating it as a small
                 * buffer. Once enough bytes are ready we (may) need to endian
                 * swap them, which we do here. */

                if (field == PLen)
                {
                    keyObj->u16PLen = VltEndianReadPU16(pu8Buf);
                }

                u16Offset = 0;

                field++; /* move to next field */
            }
        }
    }
    
    /* Validate received CRC */
    if( u16ReceivedCrc != u16CalculatedCrc )
    {
        return( ERKRSACRIVLDCRC );
    }

    return VLT_OK;
}
#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */

#endif /* ( VLT_ENABLE_KEY_RSA == VLT_ENABLE ) */

#if( VLT_ENABLE_KEY_DSA == VLT_ENABLE )

#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X )

VLT_STS VltReadKey_DsaPublic( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_DSA_PUB* keyObj,
    VLT_PSW pSW )
{
    enum { YLen, Y, PathLen, Path, End } field;

    VLT_STS       status;    
    VLT_U16       u16Offset;
    VLT_U16       u16Avail;

    /*
     * Validate all input parameters.
     */
    if ( ( NULL == keyObj ) ||
         ( NULL == pSW ) ||
         ( NULL == keyObj->pu8sParams ) ||
         ( NULL == keyObj->pu8Y ) )
    {
        return ( ERKDPUBNULLPARA );
    }


    /* offset into output buffer */
    u16Offset = 0;                     

    /* skip bKeyID since caller handles it */
    idx = 1;
    u16Avail = Response.u16Len - 1;

    /* Unpack the key object */

    /* We loop until we step beyond the last field. Buffers are incrementally
     * filled with data as it arrives. We can spend multiple iterations filling
     * in a single field. We must cope with values arriving in awkward ways,
     * e.g. a 2-byte field could arrive as the final byte and initial byte of
     * two separate requests. */

    for (field = YLen; field <= Path; )
    {
        VLT_U16 u16BufLen;
        VLT_U16 u16BufferSize;
        VLT_PU8 pu8Buf;

        /* Fill the buffer up if it's empty */

        if (u16Avail == 0)
        {

            status = VltReadKeyCommand( &Command, &Response, u8KeyGroup,
                u8KeyIndex, pSW);

            if (VLT_OK != status)
            {
                return status;
            }

            /* use entire buffer this iteration */
            idx = 0;
            u16Avail = Response.u16Len;
        }
        /* Turn each field into a buffer length and pointer */

        switch (field)
        {
            case YLen:
                u16BufLen = 2;
                pu8Buf    = (VLT_U8 *) &keyObj->u16YLen;
                u16BufferSize = keyObj->u16YLen;
                break;

            case Y:
                u16BufLen = keyObj->u16YLen;
                pu8Buf    = keyObj->pu8Y;
                break;

            case PathLen:
                u16BufLen = 2;
                pu8Buf    = (VLT_U8 *) &keyObj->u16PathLen;
                u16BufferSize = keyObj->u16PathLen;
                break;

            case Path:
                u16BufLen = keyObj->u16PathLen;
                pu8Buf    = keyObj->pu8sParams;
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

            if ( ( ( field == YLen ) ||
                   ( field == PathLen ) ) &&
                   ( u16BufferSize < VltEndianReadPU16( &Response.pu8Data[idx] ) ) )
            {
                return ( ERKDPUBNOROOM );
            }

            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memcpy( pu8Buf + u16Offset, &Response.pu8Data[idx], u16Copy);
            idx += u16Copy;
            u16Offset += u16Copy;
            u16Avail -= u16Copy;

            if (u16Offset == u16BufLen) /* buffer full? */
            {
                /* When reading the length values we accumulate the bytes
                 * piecemeal in the 2-byte value itself, treating it as a small
                 * buffer. Once enough bytes are ready we (may) need to endian
                 * swap them, which we do here. */

                if (field == YLen)
                {
                    keyObj->u16YLen = VltEndianReadPU16(pu8Buf);
                }
                else if (field == PathLen)
                {
                    keyObj->u16PathLen = VltEndianReadPU16(pu8Buf);
                }

                u16Offset = 0;

                field++; /* move to next field */
            }
        }
    }
   
    /* Validate received CRC */
    if( u16ReceivedCrc != u16CalculatedCrc )
    {
        return( ERKDSAPBIVLDCRC );
    }

    return VLT_OK;
}
#endif /* ( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X ) */

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)
VLT_STS VltReadKey_DsaPublic( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_DSA_PUB* keyObj,
    VLT_PSW pSW )
{    
    enum { YLen, Y, DpGroup, DpIndex, Assurance, End } field;

    VLT_STS       status;    
    VLT_U16       u16Offset;
    VLT_U16       u16Avail;

    /*
     * Validate all input parameters.
     */
    if( ( NULL == keyObj ) ||
        ( NULL == pSW ) ||
        ( NULL == keyObj->pu8Y ) )
    {
        return ( ERKDPUBNULLPARA );
    }


    /* offset into output buffer */
    u16Offset = 0;                     

    /* skip bKeyID since caller handles it */
    idx = 1;
    u16Avail = Response.u16Len - 1;

    /* Unpack the key object */

    /* We loop until we step beyond the last field. Buffers are incrementally
     * filled with data as it arrives. We can spend multiple iterations filling
     * in a single field. We must cope with values arriving in awkward ways,
     * e.g. a 2-byte field could arrive as the final byte and initial byte of
     * two separate requests. */

    for( field = YLen; field <= Assurance; )
    {
        VLT_U16 u16BufLen;
        VLT_U16 u16BufferSize;
        VLT_PU8 pu8Buf;

        /* Fill the buffer up if it's empty */

        if( 0 == u16Avail )
        {

            status = VltReadKeyCommand( &Command, &Response, u8KeyGroup,
                u8KeyIndex, pSW);

            if( VLT_OK != status )
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
            case YLen:
                u16BufLen = 2;
                pu8Buf = (VLT_U8 *) &keyObj->u16YLen;
                u16BufferSize = keyObj->u16YLen;
                break;

            case Y:
                u16BufLen = keyObj->u16YLen;
                pu8Buf = keyObj->pu8Y;
                break;

            case DpGroup:
                u16BufLen = 1;
                pu8Buf = (VLT_U8 *) &keyObj->u8DomainParamsGroup;                
                break;

            case DpIndex:
                u16BufLen = 1;
                pu8Buf = (VLT_U8 *) &keyObj->u8DomainParamsIndex;
                break;

            case Assurance:
                u16BufLen = 1;
                pu8Buf = (VLT_U8 *) &keyObj->u8Assurance;
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

            if( ( field == YLen ) &&
                ( u16BufferSize < VltEndianReadPU16( &Response.pu8Data[idx] ) ) ) 
            {
                return ( ERKDPUBNOROOM );
            }

            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memcpy( pu8Buf + u16Offset, &Response.pu8Data[idx], u16Copy );
            idx += u16Copy;
            u16Offset += u16Copy;
            u16Avail -= u16Copy;

            if( u16Offset == u16BufLen ) /* buffer full? */
            {
                /* When reading the length values we accumulate the bytes
                 * piecemeal in the 2-byte value itself, treating it as a small
                 * buffer. Once enough bytes are ready we (may) need to endian
                 * swap them, which we do here. */

                if( field == YLen )
                {
                    keyObj->u16YLen = VltEndianReadPU16(pu8Buf);
                }

                u16Offset = 0;

                field++; /* move to next field */
            }
        }
    }
   
    /* 
     * Validate received CRC, the calculation takes place
     * within the call ot the VltReadKey() method.
     */
    if( u16ReceivedCrc != u16CalculatedCrc )
    {
        return( ERKDSAPBIVLDCRC );
    }

    return VLT_OK;
}
#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */

#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X )
VLT_STS VltReadKey_DsaPrivate( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_DSA_PRIV* keyObj,
    VLT_PSW pSW)
{
    enum { Mask, XLen, X, PathLen, Path, End } field;

    VLT_STS status;
    VLT_U16 u16Offset;
    VLT_U16 u16Avail;
    VLT_U8 u8Mask;

    /*
     * Validate all input parameters.
     */
    if ( ( NULL == keyObj ) ||
         ( NULL == pSW ) ||
         ( NULL == keyObj->pu8sParams ) ||
         ( NULL == keyObj->pu8X ) )
    {
        return ( ERKDPRIVNULLPARA );
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

    for (field = Mask; field <= Path; )
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

        switch (field)
        {
            case Mask:
                u16BufLen = 1;
                pu8Buf    = &keyObj->u8Mask;
                break;

            case XLen:
                u16BufLen = 2;
                pu8Buf    = (VLT_U8 *) &keyObj->u16XLen;
                u16BufferSize = keyObj->u16XLen;
                break;

            case X:
                u16BufLen = keyObj->u16XLen;
                pu8Buf    = keyObj->pu8X;
                u8Mask    = keyObj->u8Mask;
                break;

            case PathLen:
                u16BufLen = 2;
                pu8Buf    = (VLT_U8 *) &keyObj->u16PathLen;
                u16BufferSize = keyObj->u16PathLen;
                break;

            case Path:
                u16BufLen = keyObj->u16PathLen;
                pu8Buf    = keyObj->pu8sParams;
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

            if ( ( ( field == XLen ) ||
                   ( field == PathLen ) ) &&
                   ( u16BufferSize < VltEndianReadPU16( &Response.pu8Data[idx] ) ) )
            {
                return ( ERKDPRIVNOROOM );
            }
            
            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memcpyxor( pu8Buf + u16Offset, &Response.pu8Data[idx], u16Copy, u8Mask );
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

                if( field == XLen )
                {
                    keyObj->u16XLen = VltEndianReadPU16(pu8Buf);
                }
                else if( field == PathLen )
                {
                    keyObj->u16PathLen = VltEndianReadPU16(pu8Buf);
                }

                u16Offset = 0;

                field++; /* move to next field */
            }
        }
    }
    
    /* Validate received CRC */
    if( u16ReceivedCrc != u16CalculatedCrc )
    {
        return( ERKDSAPRIVLDCRC );
    }

    return VLT_OK;
}
#endif /* ( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X ) */

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)
VLT_STS VltReadKey_DsaPrivate( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_DSA_PRIV* keyObj,
    VLT_PSW pSW)
{    
    enum { Mask, XLen, X, DpGroup, DpIndex, PbGroup, PbIndex, End } field;

    VLT_STS status;
    VLT_U16 u16Offset;
    VLT_U16 u16Avail;
    VLT_U8 u8Mask;

    /*
     * Validate all input parameters.
     */
    if( ( NULL == keyObj ) ||
        ( NULL == pSW ) ||
        ( NULL == keyObj->pu8X ) )
    {
        return ( ERKDPRIVNULLPARA );
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

    for( field = Mask; field <= PbIndex; )
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

        u8Mask = 0;

        switch( field )
        {
            case Mask:
                u16BufLen = 1;
                pu8Buf    = &keyObj->u8Mask;
                break;

            case XLen:
                u16BufLen = 2;
                pu8Buf    = (VLT_U8 *) &keyObj->u16XLen;
                u16BufferSize = keyObj->u16XLen;
                break;

            case X:
                u16BufLen = keyObj->u16XLen;
                pu8Buf    = keyObj->pu8X;
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

            case PbGroup:
                u16BufLen = 1;
                pu8Buf = (VLT_U8 *)&keyObj->u8PublicKeyGroup;
                break;

            case PbIndex:
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
            if (u16Avail > u16Remain)
            {
                u16Copy = u16Remain;
            }
            else
            {
                u16Copy = u16Avail;
            }

            if( ( field == XLen ) &&
                ( u16BufferSize < VltEndianReadPU16( &Response.pu8Data[idx] ) ) )
            {
                return ( ERKDPRIVNOROOM );
            }
            
            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memcpyxor( pu8Buf + u16Offset, &Response.pu8Data[idx], u16Copy, u8Mask );
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

                if( field == XLen )
                {
                    keyObj->u16XLen = VltEndianReadPU16(pu8Buf);
                }

                u16Offset = 0;

                field++; /* move to next field */
            }
        }
    }
    
    /* Validate received CRC */
    if( u16ReceivedCrc != u16CalculatedCrc )
    {
        return( ERKDSAPRIVLDCRC );
    }

    return VLT_OK;
}

VLT_STS VltReadKey_DsaParams( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_DSA_PARAMS* keyObj,
    VLT_PSW pSW)
{        
    enum { PLen, P, G, QLen, Q, DigestId, Counter, SeedLen, Seed, Assurance, End } field;

    VLT_STS status;
    VLT_U16 u16Offset;
    VLT_U16 u16Avail;


    /*
     * Validate all input parameters.
     */
    if( ( NULL == keyObj ) ||
        ( NULL == pSW ) ||
        ( NULL == keyObj->pu8G ) ||
        ( NULL == keyObj->pu8P ) ||
        ( NULL == keyObj->pu8Q ) ||
        ( NULL == keyObj->pu8Seed ) )
    {
        
        return ( ERKDSAPARAMSNULLPARA );
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

    for( field = PLen; field <= Assurance; )
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
            case PLen:
                u16BufLen = 2;
                pu8Buf = (VLT_U8 *)&keyObj->u16PLen;
                u16BufferSize = keyObj->u16PLen;
                break;

            case P:
                u16BufLen = keyObj->u16PLen;
                pu8Buf = (VLT_U8 *)keyObj->pu8P;                
                break;

            case G:
                u16BufLen = keyObj->u16PLen;
                pu8Buf = (VLT_U8 *)keyObj->pu8G;
                break;

            case QLen:
                u16BufLen = 2;
                pu8Buf = (VLT_U8 *)&keyObj->u16QLen;
                u16BufferSize = keyObj->u16QLen;
                break;

            case Q:
                u16BufLen = keyObj->u16QLen;
                pu8Buf = (VLT_U8 *)keyObj->pu8Q;
                break;

            case DigestId:
                u16BufLen = 1;
                pu8Buf = (VLT_U8 *)&keyObj->u8DigestId;
                break;

            case Counter:
                u16BufLen = 2;
                pu8Buf = (VLT_U8 *)&keyObj->u16Counter;
                break;

            case SeedLen:
                u16BufLen = 2;
                pu8Buf = (VLT_U8 *)&keyObj->u16SeedLen;
                u16BufferSize = keyObj->u16SeedLen;
                break;

            case Seed:
                u16BufLen = keyObj->u16SeedLen;
                pu8Buf = (VLT_U8 *)keyObj->pu8Seed;
                break;

            case Assurance:
                u16BufLen = 1;
                pu8Buf = (VLT_U8 *)&keyObj->u8Assurance;
                break;

            default:
                return ERKDSAPARAMSBADFIELD;
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

            if( ( ( field == PLen ) ||
                ( field == QLen ) ||
                ( field == SeedLen ) ) &&
                ( u16BufferSize < VltEndianReadPU16( &Response.pu8Data[idx] ) ) )
            {
                return ( ERKDPRIVNOROOM );
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

                if( field == PLen )
                {
                    keyObj->u16PLen = VltEndianReadPU16(pu8Buf);
                }

                if( field == QLen )
                {
                    keyObj->u16QLen = VltEndianReadPU16(pu8Buf);
                }

                if( field == SeedLen )
                {
                    keyObj->u16SeedLen = VltEndianReadPU16(pu8Buf);
                }

                if( field == Counter )
                {
                    keyObj->u16Counter = VltEndianReadPU16(pu8Buf);
                }

                u16Offset = 0;

                field++; /* move to next field */
            }
        }
    }
    
    /* Validate received CRC */
    if( u16ReceivedCrc != u16CalculatedCrc )
    {
        return( ERKDSAPARAMSIVLDCRC );
    }

    return VLT_OK;
}

#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */

#endif /* ( VLT_ENABLE_KEY_DSA == VLT_ENABLE ) */


#if( VLT_ENABLE_KEY_ECDSA == VLT_ENABLE )

#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X )
VLT_STS VltReadKey_EcdsaPublic(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_ECDSA_PUB* keyObj,
    VLT_PSW pSW)
{
    enum { QLen, Qx, Qy, PathLen, Path, End } field;

    VLT_STS       status; 
    VLT_U16       u16Offset;
    VLT_U16       u16Avail;

    /*
     * Validate all input parameters.
     */
    if ( ( NULL == keyObj ) ||
         ( NULL == pSW ) ||
         ( NULL == keyObj->pu8sParams ) ||
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

    for (field = QLen; field <= Path; )
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

        switch (field)
        {
            case QLen:
                u16BufLen = 2;
                pu8Buf    = (VLT_U8 *) &keyObj->u16QLen;
                u16BufferSize = keyObj->u16QLen;
                break;

            case Qx:
                u16BufLen = keyObj->u16QLen;
                pu8Buf    = keyObj->pu8Qx;
                break;

            case Qy:
                u16BufLen = keyObj->u16QLen;
                pu8Buf    = keyObj->pu8Qy;
                break;

            case PathLen:
                u16BufLen = 2;
                pu8Buf    = (VLT_U8 *) &keyObj->u16PathLen;
                u16BufferSize = keyObj->u16PathLen;
                break;

            case Path:
                u16BufLen = keyObj->u16PathLen;
                pu8Buf    = keyObj->pu8sParams;
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

            if ( ( ( field == QLen ) ||
                   ( field == PathLen ) ) &&
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
                else if (field == PathLen)
                {
                    keyObj->u16PathLen = VltEndianReadPU16(pu8Buf);
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
        return( ERKECDSAPBIVLDCRC );
    }

    return VLT_OK;
}

VLT_STS VltReadKey_EcdsaPrivate( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_ECDSA_PRIV* keyObj,
    VLT_PSW pSW )
{
    enum { Mask, DLen, D, PathLen, Path, End } field;

    VLT_STS       status;    
    VLT_U16       u16Offset;
    VLT_U16       u16Avail;
    VLT_U8        u8Mask;

    /*
     * Validate all input parameters.
     */
    if ( ( NULL == keyObj ) ||
         ( NULL == pSW ) ||
         ( NULL == keyObj->pu8sParams ) ||
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

    for (field = Mask; field <= Path; )
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

        switch (field)
        {
            case Mask:
                u16BufLen = 1;
                pu8Buf    = &keyObj->u8Mask;
                break;

            case DLen:
                u16BufLen = 2;
                pu8Buf    = (VLT_U8 *) &keyObj->u16DLen;
                u16BufferSize = keyObj->u16DLen;
                break;

            case D:
                u16BufLen = keyObj->u16DLen;
                pu8Buf    = keyObj->pu8D;
                u8Mask    = keyObj->u8Mask;
                break;

            case PathLen:
                u16BufLen = 2;
                pu8Buf    = (VLT_U8 *) &keyObj->u16PathLen;
                u16BufferSize = keyObj->u16PathLen;
                break;

            case Path:
                u16BufLen = keyObj->u16PathLen;
                pu8Buf    = keyObj->pu8sParams;
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

            if ( ( ( field == DLen ) ||
                   ( field == PathLen ) ) &&
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

                if (field == DLen)
                {
                    keyObj->u16DLen = VltEndianReadPU16(pu8Buf);
                }
                else if (field == PathLen)
                {
                    keyObj->u16PathLen = VltEndianReadPU16(pu8Buf);
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
#endif /* ( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X ) */

#ifdef VLT_ENABLE_KEY_ECDH
#if ( VAULT_IC_VERSION == VAULTIC_VERSION_1_2_1 )
VLT_STS VltReadKey_EcdhPublic(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_ECDH_PUB* keyObj,
    VLT_PSW pSW)
{
	enum { QLen, Qx, Qy, DpGroup, DpIndex, End } field;

	VLT_STS       status; 
    VLT_U16       u16Offset;
    VLT_U16       u16Avail;

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

    for( field = QLen; field <= DpIndex; )
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
        return( ERKECDSAPBIVLDCRC );
    }

    return VLT_OK;
}

VLT_STS VltReadKey_EcdhPrivate(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_ECDH_PRIV* keyObj,
    VLT_PSW pSW)
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
#endif //VAULTIC_VERSION_1_0_X
#endif


#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) 
VLT_STS VltReadKey_EcdsaPublic(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_ECDSA_PUB* keyObj,
    VLT_PSW pSW)
{    
    enum { QLen, Qx, Qy, DpGroup, DpIndex,/* Assurance,*/ End } field;

    VLT_STS       status; 
    VLT_U16       u16Offset;
    VLT_U16       u16Avail;

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

    for( field = QLen; field < End/*<= Assurance*/; )
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

         /*   case Assurance:
                u16BufLen = 1;
                pu8Buf = (VLT_U8 *)&keyObj->u8Assurance;
                break;*/

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

#endif /* ( VAULT_IC_VERSION == VAULTIC_VERSION_1_2_X ) */

#endif /* ( VLT_ENABLE_KEY_ECDSA == VLT_ENABLE ) */


#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) 

#if( VLT_ENABLE_KEY_HOST_DEVICE_ID == VLT_ENABLE )

VLT_STS VltReadKey_IdKey( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_ID* keyObj,
    VLT_PSW pSW )
{    
    enum { StrLen, String, End } field;

    VLT_STS status;
    VLT_U16 u16Offset;
    VLT_U16 u16Avail;

    /*
     * Validate all input parameters.
     */
    if( ( NULL == keyObj ) ||
        ( NULL == pSW ) ||
        ( NULL == keyObj->pu8StringId ) )
    {
        return ( ERKIDNULLPARA );
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

    for( field = StrLen; field <= String; )
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
            case StrLen:
                u16BufLen = 2;
                pu8Buf = (VLT_U8 *)&keyObj->u16StringLen;
                u16BufferSize = keyObj->u16StringLen;
                break;

            case String:
                u16BufLen = keyObj->u16StringLen;
                pu8Buf = (VLT_U8 *)keyObj->pu8StringId;
                break;

            default:
                return ERKIDBADFIELD;
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

            if( ( field == StrLen ) &&
                ( u16BufferSize < VltEndianReadPU16( &Response.pu8Data[idx] ) ) )
            {
                return ( ERKIDNOROOM );
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

                if( field == StrLen )
                {
                    keyObj->u16StringLen = VltEndianReadPU16(pu8Buf);                   
                }
                               
                u16Offset = 0;

                field++; /* move to next field */
            }
        }
    }
    
    /* Validate received CRC */
    if( u16ReceivedCrc != u16CalculatedCrc )
    {
        return( ERKIDIVLDCRC );
    }

    return VLT_OK;
}

#endif /* ( VLT_ENABLE_KEY_HOST_DEVICE_ID == VLT_ENABLE ) */

#endif /* ( VAULT_IC_VERSION == VAULTIC_VERSION_1_2_X ) */
