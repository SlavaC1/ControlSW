/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#include "vaultic_common.h"
#include "vaultic_cipher.h"
#include "vaultic_padding.h"
#include "vaultic_mem.h"
#include "vaultic_utils.h"
#include "vaultic_crypto.h"
#include "vaultic_symmetric_signer.h"
#include "vaultic_ecdsa_signer.h"


/**
 * Private Defs
 */
#define ST_UNKNOWN          0x00
#define ST_INITIALISED      0x10
#define ST_UPDATED          0x20
#define ST_FINALISED        0x30



/**
 * Private Data
 */
static VLT_U8 cryptoState = ST_UNKNOWN;

/**
 * The reason for not having a common Init method is legacy.
 * The vaultic 460 only supports symmetric algorithms, the 
 * 460 interface has been published, so can not be changed.
 * The vaultic 100 supports ECDSA, this requires a different
 * interface to pass ECDSA keys and domain parameters.
 */
typedef VLT_STS (*pfnSymmetricCryptoInit)( VLT_U8 opMode, const KEY_BLOB* pkey, VLT_PU8 pParams );
typedef VLT_STS (*pfnAsymmetricCryptoInit)( const CRYPTO_SERVICE_KEY_OBJECT* pKey, 
    VLT_U8 opMode, 
    const CRYPTO_SERVICE_ALGO_PARAMS* pAlgoParams  );

typedef VLT_STS (*pfnCryptoClose)( void );

typedef VLT_STS (*pfnCryptoDoFinal)( 
    VLT_PU8 pMessage, 
    VLT_U32 messageLen, 
    VLT_U32 messageCapacity, 
    VLT_PU8 pMac, 
    VLT_PU32 pMacLen, 
    VLT_U32 macCapacity );

typedef VLT_STS (*pfnCryptoUpdate)( VLT_PU8 pMessage, VLT_U32 messageLen, VLT_U32 messageCapacity );
typedef VLT_U16 (*pfnCryptoGetBlockSize)( void );


typedef struct _crypto
{
    pfnSymmetricCryptoInit symmetricCryptoInit;
    pfnAsymmetricCryptoInit asymmetricCryptoInit; 
    pfnCryptoClose cryptoClose;
    pfnCryptoDoFinal cryptoDoFinal;
    pfnCryptoUpdate cryptoUpdate;
    
} Crypto;


/**
 * The crypto function pointer structure is set to 0
 * by default.  It be setup in the CryptoInit method.
 */
static Crypto theCrypto = 
{
    0,
    0,
    0,
    0,
    0
};



VLT_STS CryptoInit( const CRYPTO_SERVICE_KEY_OBJECT* pKey, 
    VLT_U8 u8OpMode, 
    const CRYPTO_SERVICE_ALGO_PARAMS* pAlgoParams  )
{   
    VLT_STS status = VLT_FAIL;


    if ( ( NULL == pKey ) ||
         ( NULL == pAlgoParams ) )
         
    {
        return ( ECRYPTOINITNULLPARAM );
    }


    /* Check the requested algorithm  matches supported algorithms. */
    switch ( pAlgoParams->u8AlgoID )
    {
    
#if ( ( VLT_ENABLE_CIPHER_DES == VLT_ENABLE ) && ( VLT_ENABLE_CIPHER_TDES == VLT_ENABLE ) && ( VLT_ENABLE_CIPHER_AES == VLT_ENABLE ) )
    case VLT_ALG_SIG_MAC_ISO9797_ALG1:
    case VLT_ALG_SIG_MAC_ISO9797_ALG3:
    case VLT_ALG_SIG_CMAC_AES:   
        /* Setup the required function pointers for symmetric signer .*/
        theCrypto.cryptoClose = SymmetricSignerClose;
        theCrypto.cryptoDoFinal = SymmetricSignerDoFinal;
        theCrypto.cryptoUpdate = SymmetricSignerUpdate;
        theCrypto.symmetricCryptoInit = SymmetricSignerInit;

        /* 
         * Construct the required parameters and call the 
         * symmetric signer init method.
         */
        key.keyType = pKey->u8KeyID, 
        key.keySize = pKey->cs_data.SecretKey.u16KeyLength,
        key.keyValue = pKey->cs_data.SecretKey.pu8Key;
                
        signerParams.algoID = pAlgoParams->u8AlgoID;
        signerParams.pIV = (VLT_PU8)&pAlgoParams->cs_data.Cmac.u8Iv;
        signerParams.ivSize = pAlgoParams->cs_data.Cmac.u8IvLength;
        signerParams.paddingScheme = pAlgoParams->cs_data.Cmac.u8Padding;

        /*
         * Make the call to the Symmetric Signer Init method.
         */
        if ( NULL != theCrypto.symmetricCryptoInit )
        {
            status = theCrypto.symmetricCryptoInit( u8OpMode, &key, (VLT_PU8)&signerParams );
        }        

        break;
#endif /* #if ( ( VLT_ENABLE_CIPHER_DES == VLT_ENABLE ) && ( VLT_ENABLE_CIPHER_TDES == VLT_ENABLE ) && ( VLT_ENABLE_CIPHER_AES == VLT_ENABLE ) ) */
    
#if (  VLT_ENABLE_ECDSA_SIGNER == VLT_ENABLE )  
    case VLT_ALG_SIG_ECDSA_GF2M:
        /* Setup the required function pointers for Ecdsa signer .*/
        theCrypto.cryptoClose = EcdsaSignerClose;
        theCrypto.cryptoDoFinal = EcdsaSignerDoFinal;
        theCrypto.cryptoUpdate = EcdsaSignerUpdate;
        theCrypto.asymmetricCryptoInit = EcdsaSignerInit;

        if ( NULL != theCrypto.asymmetricCryptoInit )
        {
            /*
             * Make the call to the Asymmetric Signer Init method.
             */
            status = theCrypto.asymmetricCryptoInit( pKey, u8OpMode, pAlgoParams );
        }

        break;
#endif /* #if (  VLT_ENABLE_ECDSA_SIGNER == VLT_ENABLE ) */
    default:
        /* Error the algoritm requested is not supported. */
        status = ECRYPTOALGNOTSUP;
        
        break;
    };


    /* 
     * If the init method reports success, set the state to initalised. 
     */
    if ( VLT_OK == status )
    {
        cryptoState = ST_INITIALISED;
    }
    else
    {
        cryptoState = ST_UNKNOWN;
    }


    return ( status );
}



VLT_STS CryptoClose( void )
{
    VLT_STS status = VLT_FAIL;

    if( ST_UNKNOWN != cryptoState )
    {
        if (NULL != theCrypto.cryptoClose )
        {
            status=  theCrypto.cryptoClose( );
        }
    }
    else
    {
        status = ECRYPTOCLSNOTSUPPORTED;
    }

    return( status );
}



VLT_STS CryptoDoFinal(
    VLT_PU8 pu8Message, 
    VLT_U32 u32messageLen, 
    VLT_U32 u32messageCapacity, 
    VLT_PU8 pu8Output, 
    VLT_PU32 pu32OutputLen, 
    VLT_U32 u32OutPutCapacity )
{
    VLT_STS status = VLT_FAIL;

    if( ( ST_UNKNOWN == cryptoState ) ||
        ( ST_FINALISED == cryptoState ) )
    {
        return( ECRYPTODFNOTSUPPORTED );
    }

    /*
     * Check the input arguments are valid.
     */
    if ( ( NULL == pu8Message ) ||
         ( NULL == pu8Output ) ||
         ( NULL == pu32OutputLen ) )
    {
        return ( ECRYPTODFNULLPARAM );
    }


    /*
     * Call The Crypto DoFinal Method
     */
    if ( NULL != theCrypto.cryptoDoFinal )
    {
        status = theCrypto.cryptoDoFinal( pu8Message, 
            u32messageLen, 
            u32messageCapacity, 
            pu8Output, 
            pu32OutputLen, 
            u32OutPutCapacity );
    }
    
    return ( status );
}



VLT_STS cryptoUpdate( VLT_PU8 pu8Message, 
    VLT_U32 u32MessageLen, 
    VLT_U32 u32MessageCapacity,
    VLT_PU8 pu8Output, 
    VLT_PU32 pu32OutputLen, 
    VLT_U32 u32OutPutCapacity )
{
    VLT_STS status = VLT_FAIL;

    /* 
     * These variables are not used at present but have been added to allow
     * ciphers to be connected at a later date as these params are needed.
     * VLT_PU8 pu8Output, 
     * VLT_PU32 pu32OutputLen, 
     * VLT_U32 u32OutPutCapacity
     */


    /* Check the state is valid before making the call. */
    if( ( ST_UNKNOWN == cryptoState ) ||
        ( ST_FINALISED == cryptoState ) )
    {
        return( ECRYPTOUPDNOTSUPPORTED );
    }

    /* Check the message pointer is valid. */
    if ( NULL == pu8Message )
    {
        status = ECRYPTOUNULLPARAM;
    }

    /* Call The Crypto Update Method */
    if ( NULL != theCrypto.cryptoUpdate ) 
    {
        status = theCrypto.cryptoUpdate( pu8Message, 
            u32MessageLen, 
            u32MessageCapacity );
    }

    return( status );
}


