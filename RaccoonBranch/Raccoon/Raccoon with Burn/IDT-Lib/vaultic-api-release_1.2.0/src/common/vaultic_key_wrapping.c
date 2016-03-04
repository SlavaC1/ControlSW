/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#include "vaultic_common.h"
#include "vaultic_key_wrapping.h"
#include "vaultic_api.h"
#include "vaultic_cipher.h"
#include "vaultic_mem.h"
#include "vaultic_secure_channel.h"
#include "vaultic_apdu.h"
#include "vaultic_comms.h"
#include "vaultic_utils.h"
#include "vaultic_command.h"
#include "vaultic_crc16.h"

/**
 * Externs 
 */
extern VLT_MEM_BLOB Command;                /* declared in vaultic_api.c */
extern VLT_MEM_BLOB Response;               /* declared in vaultic_api.c */
extern VLT_U16 idx;                         /* declared in vaultic_api.c */

/**
 * Private Defs
 */
#define ST_UNINIT       0x00
#define ST_PARAMS_INIT  0x10
#define ST_CIPHER_INIT  0x20

#define NO_MODE         0x00
#define WRAP_MODE       0x10
#define UNWRAP_MODE     0x20

#if( VLT_ENABLE_KEY_WRAPPING == VLT_ENABLE )
    /*
    * Private Data
    */
    static VLT_U8 u8CachedKTSKeyGroup = 0;
    static VLT_U8 u8CachedKTSKeyIndex = 0;
    static WRAP_PARAMS theWrapParams;
    static KEY_BLOB theKey;
    static VLT_U16 u16ReceivedCrc = 0xFFFF;
    static VLT_U8 keyWrappingState = ST_UNINIT;
    static VLT_U8 keyWrappingMode = NO_MODE;

    /*
    * Local methods
    */
    static VLT_STS VltReadEncryptedKey( VLT_U8 u8KeyGroup,
        VLT_U8 u8KeyIndex,    
        VLT_PSW pSW );
#endif /* #if( VLT_ENABLE_KEY_WRAPPING == VLT_ENABLE ) */


VLT_STS VltKeyWrappingInit( VLT_U8 u8KTSKeyGroup,
    VLT_U8 u8KTSKeyIndex,
    WRAP_PARAMS* pWrapParams,
    VLT_KEY_OBJECT* pKTSKey )
{
#if( VLT_ENABLE_KEY_WRAPPING == VLT_ENABLE )
    VLT_STS status = VLT_FAIL;
    VLT_U8 u8SecChnlState = VLT_USER_NOT_AUTHENTICATED;

    /*
    * Check the validity of the input parameters
    */
    if( ( NULL == pWrapParams ) || 
		( pWrapParams->chainMode != BLOCK_MODE_ECB && NULL == pWrapParams->pIV ) ||
        ( NULL == pKTSKey ) || 
        ( NULL == pKTSKey->data.SecretKey.pu8Key ) )
    {
        return( EKWINITNULLPARAM );
    }

    /*
    * Check if a Secure channel is enabled.  If it is, don't allow the
    * initialisation of the service
    */
    if( VLT_OK == VltScpGetState( &u8SecChnlState ) )
    {
        if( u8SecChnlState == VLT_USER_AUTHENTICATED )
        {
            status = EKWINITSCPENBLD;
        }
        else
        {
            status = VLT_OK;
        }
    }
    else
    {
        /*
        * As Secure Channel hasn't been enabled the wrap call can proceed
        */
        status = VLT_OK;
    }

    if( VLT_OK == status ) 
    {
        /*
        * Check that the Algo ID being passed in is supported
        */
        switch( pWrapParams->algoID )
        {
        case VLT_ALG_KTS_TDES_3K_EEE:
        case VLT_ALG_KTS_TDES_3K_EDE:      
        case VLT_ALG_KTS_AES:
            status = VLT_OK;
            break;

        default:
            status = EKWINITINVLDALGOID;
            break;
        }
    }

    if( VLT_OK == status )
    {
        /*
        * Check that the KTS key being passed in is supported
        */
        switch( pKTSKey->u8KeyID )
        {
        case VLT_KEY_TDES_3K:
        case VLT_KEY_AES_128:      
        case VLT_KEY_AES_192:
        case VLT_KEY_AES_256:
            status = VLT_OK;
            break;

        default:
            status = EKWINITINVLDKTSKEY;
            break;
        }
    }

    if( VLT_OK == status )
    {
        /*
        * Cache the Key Group and Key Index
        */
        u8CachedKTSKeyGroup = u8KTSKeyGroup;
        u8CachedKTSKeyIndex = u8KTSKeyIndex;

        /*
        * Cache the Wrap Params
        */
        theWrapParams = *pWrapParams;

        /*
        * Cache the key data that will be required.
        */
        theKey.keyType = pKTSKey->u8KeyID;
        theKey.keySize = pKTSKey->data.SecretKey.u16KeyLength;
        theKey.keyValue = pKTSKey->data.SecretKey.pu8Key;

        /*
        * Set the state to initialised and the mode to no mode
        */
        keyWrappingState = ST_PARAMS_INIT;
        keyWrappingMode = NO_MODE;
    }

    return( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}

VLT_STS VltUnwrapKey( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
    const VLT_KEY_OBJ_RAW* pKeyObj )
{
#if( VLT_ENABLE_KEY_WRAPPING == VLT_ENABLE )
    VLT_STS status = VLT_FAIL;
    VLT_U8 u8SecChnlState = VLT_USER_NOT_AUTHENTICATED;
    
    /*
    * Check the validity of the input parameters
    */
    if( ( NULL == pKeyFilePrivileges ) || 
        ( NULL == pKeyObj ) || 
        ( NULL == pKeyObj->pu16ClearKeyObjectLen ) || 
        ( NULL == pKeyObj->pu8KeyObject ) )
    {
        return( EKWWKNULLPARAM );
    }

    /*
    * Check that the key wrapping has been initialised
    */
    if( ST_UNINIT == keyWrappingState )
    {
        return ( EKWWKUNINIT );
    }

    /*
    * Check if a Secure channel is enabled.  If it is, don't allow the wrap
    */
    if( VLT_OK == VltScpGetState( &u8SecChnlState ) )
    {
        if( u8SecChnlState == VLT_USER_AUTHENTICATED )
        {
            status = EKWWKSCPENBLD;
        }
        else
        {
            status = VLT_OK;
        }
    }
    else
    {
        /*
        * As Secure Channel hasn't been enabled the wrap call can proceed
        */
        status = VLT_OK;
    }

    if( VLT_OK == status )
    {
        /*
        * Initialise the Cipher
        */
        status = CipherInit( VLT_ENCRYPT_MODE,
            &theKey,
            (VLT_PU8)&theWrapParams );

        if( VLT_OK == status )
        {
            /*
            * The cipher was initialised correctly so update the state
            */
            keyWrappingState = ST_CIPHER_INIT;
        }
    }

    if( ( VLT_OK == status ) && ( WRAP_MODE != keyWrappingMode ) )
    {
        /*
        * Call Initialize Algorithm on the VaultIC to set it up to unwrap the
        * wrapped key we are about to send down. Only do this if it hasn't 
        * already been called
        */
        VLT_ALGO_PARAMS algorithm;

        algorithm.u8AlgoID = theWrapParams.algoID;
        algorithm.data.SymCipher.u8Padding = theWrapParams.paddingScheme;
        algorithm.data.SymCipher.u8Mode = theWrapParams.chainMode;

		if (theWrapParams.pIV != NULL)
		{
        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_memcpy( &(algorithm.data.SymCipher.u8Iv[0]),
            theWrapParams.pIV,
            CipherGetBlockSize() );

        algorithm.data.SymCipher.u8IvLength = (VLT_U8)CipherGetBlockSize();
		}
		else
			 algorithm.data.SymCipher.u8IvLength = 0;

        status = VltInitializeAlgorithm( u8CachedKTSKeyGroup, 
            u8CachedKTSKeyIndex,
            VLT_UNWRAP_KEY_MODE,
            &algorithm );

        if( VLT_OK == status )
        {
            /*
            * Update the mode to wrap
            */
            keyWrappingMode = WRAP_MODE;
        }
    }

    if( VLT_OK == status )
    {
        VLT_U16 u16Remaining = 
                VLT_PUTKEY_FIXED_DATA_LENGTH + *pKeyObj->pu16ClearKeyObjectLen;
        VLT_U16 u16KeyBytesRemaining = *(pKeyObj->pu16ClearKeyObjectLen);
        VLT_U16 u16MaxChunk = VltCommsGetMaxSendSize();
        VLT_U16 u16Offset = 0;

        while( 0 != u16Remaining )
        {
            VLT_SW Sw = VLT_STATUS_NONE;
            VLT_U16 u16Avail = 0;
            VLT_U16 u16PartialKeyLen = 0;
            VLT_U32 u32CipherDataLen = 0;
            VLT_U8 u8Final = 0;

            /*
            * Set index at the start of the data portion of the buffer
            */
            idx = VLT_APDU_DATA_OFFSET;

            /*
            * Build the data in
            */
            if( 0 == u16Offset )
            {
                /* 
                * Add the Key Priviliges 
                */
                Command.pu8Data[idx++] = pKeyFilePrivileges->u8Read;
                Command.pu8Data[idx++] = pKeyFilePrivileges->u8Write;
                Command.pu8Data[idx++] = pKeyFilePrivileges->u8Delete;
                Command.pu8Data[idx++] = pKeyFilePrivileges->u8Execute;

                /*
                * Add the Key Length
                */
                Command.pu8Data[idx++] = 
                    (VLT_U8)( (*pKeyObj->pu16ClearKeyObjectLen >> 8 ) & 0xFF );

                Command.pu8Data[idx++] = 
                    (VLT_U8)( (*pKeyObj->pu16ClearKeyObjectLen >> 0 ) & 0xFF );
            }

            u16Avail = NumBufferBytesAvail( u16MaxChunk, idx );

            if(u16KeyBytesRemaining > u16Avail)
            {
                /*
                * There is more key data remaining than can be transferred
                * in one transaction
                */
                u16PartialKeyLen = ( u16Avail / CipherGetBlockSize() )
                    * CipherGetBlockSize();
            }
            else
            {
                /*
                * The remaining data might all be able to be transferred in
                * one transaction
                */
                if( u16Avail >= (u16KeyBytesRemaining + CipherGetBlockSize() ) )
                {
                    u16PartialKeyLen = u16KeyBytesRemaining;

                    /*
                    * Flag that this will be the final block to be encrypted
                    */
                    u8Final = 1;
                }
                else
                {
                    u16PartialKeyLen = 
                        u16KeyBytesRemaining - CipherGetBlockSize();
                }
            }

            /*
            * Copy the number of bytes of the partial key into the buffer
            */
            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memcpy( &(Command.pu8Data[idx]),
                &( (pKeyObj->pu8KeyObject[u16Offset]) ), 
                u16PartialKeyLen  );

            /*
            * Now encrypt the data in the buffer
            */
            if( 1 == u8Final )
            {
                status = CipherDoFinal( &(Command.pu8Data[idx]), 
                    u16PartialKeyLen, 
                    Command.u16Capacity - VLT_APDU_DATA_OFFSET, 
                    &(Command.pu8Data[idx]),
                    &u32CipherDataLen,
                    Command.u16Capacity - VLT_APDU_DATA_OFFSET );
            }
            else
            {
                status = CipherUpdate( &(Command.pu8Data[idx]), 
                    u16PartialKeyLen, 
                    Command.u16Capacity - VLT_APDU_DATA_OFFSET, 
                    &(Command.pu8Data[idx]),
                    &u32CipherDataLen,
                    Command.u16Capacity - VLT_APDU_DATA_OFFSET );
            }

            if( VLT_OK == status )
            {
                /*
                * Update the index to reflect the data that has just been added
                */
                idx += (VLT_U16)u32CipherDataLen;

                /*
                * Subtract the number of key bytes that have just been added to
                * the buffer from the number of key bytes remaining to be sent
                */
                u16KeyBytesRemaining -= u16PartialKeyLen;

                /*
                * Decrement the remaining number of bytes to be sent.
                */
                if( 0 == u16Offset )
                {
                    /*
                    * The first time the File Privileges and the length are
                    * included so include them plus the partial key length
                    * which won't incude any padding bytes if some have been
                    * added
                    */
                    u16Remaining -= 
                        ( VLT_PUTKEY_FIXED_DATA_LENGTH - NUM_CRC_BYTES ) +
                        u16PartialKeyLen;
                }
                else
                {
                    /*
                    * Subtract the partial key length that was added to
                    * the buffer
                    */
                    u16Remaining -= u16PartialKeyLen;
                }

                /*
                * Update the offset into the key
                */
                u16Offset += u16PartialKeyLen;

                /*
                * We need two bytes free in the buffer for the wCRC field.
                */
                if( ( NUM_CRC_BYTES == u16Remaining ) &&
                    ( NumBufferBytesAvail( u16MaxChunk, idx ) >= NUM_CRC_BYTES ) )
                {
                    Command.pu8Data[idx++] = 
                        (VLT_U8)( ( pKeyObj->u16Crc >> 8 ) & 0xFF );

                    Command.pu8Data[idx++] = 
                        (VLT_U8)( (pKeyObj->u16Crc >> 0 ) & 0xFF );

                    u16Remaining -= NUM_CRC_BYTES;
                }

                /*
                * Setup the command header
                */
                if(0 == u16Remaining)
                {
                    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL; 
                    
                }
                else
                {
                    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_CHAINING;
                }
                Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_PUT_KEY;
                Command.pu8Data[VLT_APDU_P1_OFFSET] = u8KeyGroup;
                Command.pu8Data[VLT_APDU_P2_OFFSET] = u8KeyIndex;
                Command.pu8Data[VLT_APDU_P3_OFFSET] = 
                    LIN( WRAPPED_BYTE( NumBytesInBuffer( idx ) ) );


                /*
                * Send the command
                */
                status = VltCommand( &Command, &Response, idx, 0, &Sw );
                if(VLT_OK != status)
                {
                    break;
                }

                if( ( VLT_STATUS_COMPLETED != Sw ) && 
                    ( VLT_STATUS_SUCCESS != Sw ) )
                {
                    /*
                    * The status word indicates a problem so set that as the 
                    * return  value and break out of the while loop
                    */
                    status = Sw;
                    break;
                }
            }
            else
            {
                /*
                * Break out of the loop as the cipher failed
                */
                break;
            }
        }
    }

    if( VLT_OK != status )
    {
        /*
        * Set the state back to unitialised and the mode to no mode
        */
        keyWrappingState = ST_UNINIT;
        keyWrappingMode = NO_MODE;
    }

    return( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}

VLT_STS VltWrapKey( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_RAW* pKeyObj )
{
#if( VLT_ENABLE_KEY_WRAPPING == VLT_ENABLE )
    VLT_STS status = VLT_FAIL;
    VLT_U8 u8SecChnlState = VLT_USER_NOT_AUTHENTICATED;

    /*
    * Check the validity of the input parameter
    */
    if( ( NULL == pKeyObj ) || 
        ( NULL == pKeyObj->pu16ClearKeyObjectLen ) || 
        ( NULL == pKeyObj->pu8KeyObject ) )
    {
        return( EKWUKNULLPARAM );
    }

    /*
    * Check that the key wrapping has been initialised
    */
    if( ST_UNINIT == keyWrappingState )
    {
        return ( EKWUKUNINIT );
    }

    /*
    * Check if a Secure channel is enabled.  If it is, don't allow the wrap
    */
    if( VLT_OK == VltScpGetState( &u8SecChnlState ) )
    {
        if( u8SecChnlState == VLT_USER_AUTHENTICATED )
        {
            status = EKWUKSCPENBLD;
        }
        else
        {
            status = VLT_OK;
        }
    }
    else
    {
        /*
        * As Secure Channel hasn't been enabled the unwrap call can proceed
        */
        status = VLT_OK;
    }

    if( VLT_OK == status )
    {
        /*
        * Initialise the Cipher
        */
        status = CipherInit( VLT_DECRYPT_MODE, 
            &theKey, 
            (VLT_PU8)&theWrapParams );
        
        if( VLT_OK == status )
        {
            /*
            * The cipher was initialised correctly so update the state
            */
            keyWrappingState = ST_CIPHER_INIT;
        }

    }

    if( ( VLT_OK == status ) && ( UNWRAP_MODE != keyWrappingMode ) )
    {
        /*
        * Call Initialize Algorithm  on the VaultIC to set it up to wrap the
        * key we are about to receive and unwrap. Only do this if it hasn't 
        * already been called
        */
        VLT_ALGO_PARAMS algorithm;

        algorithm.u8AlgoID = theWrapParams.algoID;
        algorithm.data.SymCipher.u8Padding = theWrapParams.paddingScheme;
        algorithm.data.SymCipher.u8Mode = theWrapParams.chainMode;

        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_memcpy( &(algorithm.data.SymCipher.u8Iv[0]),
            theWrapParams.pIV,
            CipherGetBlockSize() );

        algorithm.data.SymCipher.u8IvLength = (VLT_U8)CipherGetBlockSize();

        status = VltInitializeAlgorithm( u8CachedKTSKeyGroup, 
            u8CachedKTSKeyIndex,
            VLT_WRAP_KEY_MODE,
            &algorithm );

        if( VLT_OK == status )
        {
            /*
            * Update the mode to unwrap
            */
            keyWrappingMode = UNWRAP_MODE;
        }
    }

    if( VLT_OK == status)
    {   
        VLT_SW Sw = VLT_STATUS_NONE;
        VLT_U16 u16KeyObjLen = 0;
        VLT_BOOL bReadComplete = FALSE;
        VLT_U16 u16RequestedLen = *(pKeyObj->pu16ClearKeyObjectLen);
        VLT_U16 u16CalculatedCrc = VLT_CRC16_CCITT_INIT_0s;

        status = VltReadEncryptedKey( u8KeyGroup, u8KeyIndex, &Sw );

        if( VLT_OK == status )
        {
            do
            {
                /* 
                * Copy the data into the user's buffer if we have enough space
                */
                if( ( u16KeyObjLen + Response.u16Len ) <= u16RequestedLen )
                {
                    /*
                    * No need to check the return type as pointer has been validated
                    */
                    (void)host_memcpy( &(pKeyObj->pu8KeyObject[ u16KeyObjLen ]),
                        Response.pu8Data, 
                        Response.u16Len );

                    /*
                    * Update the CRC
                    */
                    u16CalculatedCrc = VltCrc16Block( u16CalculatedCrc,
                        &(Response.pu8Data[0]),
                        Response.u16Len );
                }

                /*
                * Update the length
                */
                u16KeyObjLen += Response.u16Len;

                if( Sw == VLT_STATUS_SUCCESS )
                {
                    /*
                    * We have received the whole key exit the loop
                    */
                    bReadComplete = TRUE;

                    /*
                    * Assign the received CRC value into the struct
                    * returned to the host side caller. 
                    */
                    pKeyObj->u16Crc = u16ReceivedCrc;

                    /*
                    * Validate received CRC
                    */
                    if( u16ReceivedCrc != u16CalculatedCrc )
                    {
                        status = EKWUKSCIVLDCRC;
                    }
                }
                else if( Sw == VLT_STATUS_RESPONDING )
                {
                    /*
                    * Read more data
                    */
                    status = VltReadEncryptedKey( u8KeyGroup,
                        u8KeyIndex,
                        &Sw );

                    if( VLT_OK != status )
                    {
                        /*
                        * Break out of the while loop as something has gone
                        * wrong
                        */
                        break;
                    }
                }
                else
                {
                    /*
                    * Set the status word as the return value and break out
                    * of the while loop
                    */
                    status = Sw;
                    break;
                }
            }while( bReadComplete == FALSE );

            /* 
            * If we have run out of space let the caller know
            * the true length of the key requested and return
            * the appropriate error code.
            */
            if( u16KeyObjLen > u16RequestedLen )
            {
                *(pKeyObj->pu16ClearKeyObjectLen) = u16KeyObjLen;
                status = EKWUKWNOROOM;
            }
        }
    }

    if( VLT_OK != status )
    {
        /*
        * Set the state back to unitialised and the mode to no mode
        */
        keyWrappingState = ST_UNINIT;
        keyWrappingMode = NO_MODE;
    }

    return( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}

VLT_STS VltKeyWrappingClose( void )
{
#if( VLT_ENABLE_KEY_WRAPPING == VLT_ENABLE )
    VLT_STS status = VLT_OK;

    if( ST_CIPHER_INIT == keyWrappingState )
    {
        status = CipherClose( );
    }

    /*
    * Set the state to uninitialised and the mode to none
    */
    keyWrappingState = ST_UNINIT;
    keyWrappingMode = NO_MODE;

    return( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}

#if( VLT_ENABLE_KEY_WRAPPING == VLT_ENABLE )
static VLT_STS VltReadEncryptedKey( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,    
    VLT_PSW pSW )
{
    VLT_STS status = VLT_FAIL;
    VLT_U32 u32DataOutLen = 0;
    
    /* Build APDU */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_READ_KEY;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = u8KeyGroup;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = u8KeyIndex;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = 
        WRAPPED_BYTE( VltCommsGetMaxReceiveSize() );

    idx = VLT_APDU_DATA_OFFSET;

    /*
    * Send the command
    */
    status = VltCommand( &Command, &Response, idx, 0, pSW );

    if( VLT_OK == status )
    {
        /*
        * Adjust the response size to take in account the status word size
        */
        Response.u16Len -= VLT_SW_SIZE;

        /*
        * Decrypt the recieved data if the status word is valid
        */
        if( *pSW == VLT_STATUS_RESPONDING )
        {
            /*
            * Decrypt the data
            */
            status = CipherUpdate( &(Response.pu8Data[0]),
                Response.u16Len,
                Response.u16Capacity,
                &(Response.pu8Data[0]),
                &u32DataOutLen,
                Response.u16Capacity );
        }
        else if( *pSW == VLT_STATUS_SUCCESS )
        {
            /*
            * This is the last block so store the revceved CRC
            */
            Response.u16Len -= NUM_CRC_BYTES;

            /* Retrieve received CRC */
            u16ReceivedCrc = VltEndianReadPU16( 
                &Response.pu8Data[ Response.u16Len ] );

            /*
            * Decrypt the data
            */
            status = CipherDoFinal( &(Response.pu8Data[0]),
                Response.u16Len,
                Response.u16Capacity,
                &(Response.pu8Data[0]),
                &u32DataOutLen,
                Response.u16Capacity );

            if( VLT_OK == status )
            {
                /*
                * Adjust the length as some padding may have been removed
                */
                Response.u16Len = (VLT_U16)u32DataOutLen;
            }
        }
        else
        {
            status = EKWRKINVLDRSP;
        }
    }

    return( status );
}
#endif /* #if( VLT_ENABLE_KEY_WRAPPING == VLT_ENABLE ) */
