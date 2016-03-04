/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_ms_auth.c
 * 
 * \brief Interface to Microsoft Card Minidriver Authentication.
 * 
 * \par Description:
 * Provides a simple method for the host to log a user into the vaultIC,
 * provided the host has knowledge of the users secret authentication Key.
 */

#include "vaultic_common.h"
#include "vaultic_ms_auth.h"
#include "vaultic_api.h"
#include "vaultic_mem.h"
#include "vaultic_cipher.h"
#include "vaultic_apdu.h"



/* Definitions */
#define CRYPTOGRAM_LENGTH 8
#define DEVICE_CHALLENGE_LENGTH 8

/**
 * Private function prototypes
 */
VLT_STS VltCalculateCryptogram( VLT_PU8 pu8DeviceChallenge,
    KEY_BLOB* pKeyTDes3k,
    VLT_PU8 pu8Cryptogram );




VLT_STS VltMsInit( VLT_U8 u8UserID, 
    VLT_U8 u8RoleID, 
    KEY_BLOB* pKeyTDes3k )
{
#if( VLT_ENABLE_MS_AUTH == VLT_ENABLE )

    VLT_STS status = VLT_FAIL;
    VLT_INIT_UPDATE respData;
    VLT_U8 au8Cryptogram[CRYPTOGRAM_LENGTH];
    
    /*
    * Check that the input parameters are valid
    */
    if ( NULL == pKeyTDes3k )
    {
        return EMSINITNULLPARAM;
    }

    /* Check the key type, key length, and key ptr are all valid for MS auth.*/
    if ( ( VLT_KEY_TDES_3K != pKeyTDes3k->keyType ) ||
         ( TDES_3K_KEY_SIZE != pKeyTDes3k->keySize ) ||
         ( NULL == pKeyTDes3k->keyValue ) )
    {
        return ( EMSINITINVDPARAM );
    }

    /*
    * Ensure any currently logged in user is logged out.
    */
    VltMsClose();

    
    
    /*
    * Initialise Update
    */
    status = VltInitializeUpdate( u8UserID, 
        u8RoleID,
        0, // 0 host challenge length.
        0, // NULL host challenge buffer ptr.
        &respData );
    

    /*
    * Check that the call to Initialize Update was successful
    */
    if( VLT_OK != status )
    {
        return ( EMSINITUPDTFAIL );
    }

    /*
    * Check that the response has given either SCP02 or SCP03 data
    */
    if ( VLT_LOGIN_MS != respData.u8LoginMethodID )
    {
        return ( EMSUSERNOTMS );
    }

    /*
    * Calculate the Microsoft Card Minidriver Authentication Cryptogram
    * using Encrypt the returned device challenge using 3Des EEE 3K EDE.
    */
    status = VltCalculateCryptogram( respData.data.MS.u8DeviceChallenge,
        pKeyTDes3k,
        &au8Cryptogram[0] );

    
    if ( VLT_OK == status )
    {
        status = VltExternalAuthenticate(VLT_LOGIN_MS,
                VLT_NO_CHANNEL,
                CRYPTOGRAM_LENGTH,
                &au8Cryptogram[0] );

        if( VLT_OK != status )
        {
            status = EMSEXTAUTHFAIL;
        }
    }
    
    return( status );

#else
    return( EMETHODNOTSUPPORTED );
#endif

}

VLT_STS VltMsClose( void )
{
#if( VLT_ENABLE_MS_AUTH == VLT_ENABLE )
    return VltCancelAuthentication( );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}


VLT_STS VltCalculateCryptogram( VLT_PU8 pu8DeviceChallenge,
    KEY_BLOB* pKeyTDes3k,
    VLT_PU8 pu8Cryptogram )
{
#if( VLT_ENABLE_MS_AUTH == VLT_ENABLE )

    VLT_STS status = VLT_FAIL;
    CIPHER_PARAMS cipherParams;
    VLT_U32 u32DataLen;


    /* Check the parameters are valid. */
    if ( ( NULL == pu8DeviceChallenge ) ||
         ( NULL == pKeyTDes3k ) ||
         ( NULL == pu8Cryptogram ) )
    {
        return ( EMSCGRAMNULLPARAM );
    }

    
    /* Setup the cipher params structure with the correct 
     * params to calculate the MS cryptogram. */
    cipherParams.algoID = VLT_ALG_CIP_TDES_3K_EEE;
    cipherParams.chainMode = BLOCK_MODE_ECB;
    cipherParams.paddingScheme = PADDING_NONE;
    cipherParams.pIV = NULL;
    
    /* Initalise the cipher for 3Des EEE 3K Encrypt */
    status = CipherInit( VLT_ENCRYPT_MODE, 
        pKeyTDes3k, 
        (VLT_PU8)&cipherParams );
        
    if(VLT_OK == status)
    {
        /*
        * Cipher the data
        */
        status = CipherDoFinal(pu8DeviceChallenge, 
            DEVICE_CHALLENGE_LENGTH, 
            DEVICE_CHALLENGE_LENGTH,
            pu8Cryptogram,
            &u32DataLen,
            CRYPTOGRAM_LENGTH); 
    }

    /* Clear the current state of the cipher. */
    CipherClose();

    return ( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}


