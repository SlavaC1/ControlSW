/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#include "vaultic_common.h"
#include "vaultic_symmetric_signer.h"
#include "vaultic_cipher.h"
#include "vaultic_padding.h"
#include "vaultic_mem.h"
#include "vaultic_utils.h"
#include "vaultic_signer_iso9797.h"
#include "vaultic_signer_aes_cmac.h"


/**
 * Error Codes
 */
#define ESGNRIIVLDMODE           VLT_ERROR( VLT_SIGNER, 0 )
#define ESGNRIIVLDPRM            VLT_ERROR( VLT_SIGNER, 1 )
#define ESGNRIINVLDALGO          VLT_ERROR( VLT_SIGNER, 2 )
#define ESGNRINOTSUPPORTED       VLT_ERROR( VLT_SIGNER, 3 )
#define ESGNRIIVLDPAD            VLT_ERROR( VLT_SIGNER, 4 )
#define ESGNRCLSNOTSUPPORTED     VLT_ERROR( VLT_SIGNER, 5 )
#define ESGNRDFNOTSUPPORTED      VLT_ERROR( VLT_SIGNER, 6 )
#define ESGNRUPDNOTSUPPORTED     VLT_ERROR( VLT_SIGNER, 7 )
#define ESGNRUPNULLMSG           VLT_ERROR( VLT_SIGNER, 8 )
#define ESGNRUPZEROMSGLEN        VLT_ERROR( VLT_SIGNER, 9 )
#define ESGNRUPMINVLDLEN         VLT_ERROR( VLT_SIGNER, 10 )
#define ESGNRDFNULLMAC           VLT_ERROR( VLT_SIGNER, 11 )
#define ESGNRDFNULLMACLEN        VLT_ERROR( VLT_SIGNER, 12 )
#define ESGNRDFNULLMSG           VLT_ERROR( VLT_SIGNER, 13 )
#define ESGNRDFZEROMSGLEN        VLT_ERROR( VLT_SIGNER, 14 )

/**
 * Private Defs
 */
#define ST_UNKNOWN          0x00
#define ST_INITIALISED      0x10
#define ST_UPDATED          0x20
#define ST_FINALISED        0x30

#define VLT_DES_IV_SIZE     0x08
#define VLT_AES_IV_SIZE     0x10

typedef VLT_STS (*pfnSignerInit)( VLT_U8 opMode, const KEY_BLOB* pkey, VLT_PU8 pParams );
typedef VLT_STS (*pfnSignerClose)( void );

typedef VLT_STS (*pfnSignerDoFinal)( 
    VLT_PU8 pMessage, 
    VLT_U32 messageLen, 
    VLT_U32 messageCapacity, 
    VLT_PU8 pMac, 
    VLT_PU32 pMacLen, 
    VLT_U32 macCapacity );

typedef VLT_STS (*pfnSignerUpdate)( VLT_PU8 pMessage, VLT_U32 messageLen, VLT_U32 messageCapacity );
typedef VLT_U16 (*pfnSignerGetBlockSize)( void );

typedef struct _singer
{
    pfnSignerInit signerInit;
    pfnSignerClose signerClose;
    pfnSignerDoFinal signerDoFinal;
    pfnSignerUpdate signerUpdate;
    pfnSignerGetBlockSize signerGetBlockSize;

} Signer;

/**
 * The signer function pointer structure will  is set to zero.
 * The SymmetricSignerInit method willsetup the function pointers.
 */
static Signer theSigner = 
{
    0,
    0,
    0,
    0,
    0
};

/**
 * Private Data
 */
static VLT_U8 signerState = ST_UNKNOWN;
static SIGNER_PARAMS params = { 0 };



VLT_STS SymmetricSignerInit( VLT_U8 opMode, const KEY_BLOB* pKey, VLT_PU8 pParams )
{   
    VLT_STS status = VLT_FAIL;

    /**
     * Our signer supports only one mode, ensure we have
     * been passed the correct one.
     */
    if( VLT_SIGN_MODE != opMode )
    {
        return( ESGNRIIVLDMODE );
    }

    /**
     * Make sure we have a valid params pointer
     */
    if( NULL == pParams )
    {
        return( ESGNRIIVLDPRM );
    }
    else
    {
        /**
         * Cache the parameters.
         */
        params = *((SIGNER_PARAMS*)pParams);

        if( ( params.algoID != VLT_ALG_SIG_MAC_ISO9797_ALG1 ) &&
            ( params.algoID != VLT_ALG_SIG_MAC_ISO9797_ALG3 ) &&
            ( params.algoID != VLT_ALG_SIG_CMAC_AES ) )
        {
            /**
             * Clear the signerState to signify the
             * fact that something has gone pear
             * shaped and we shouldn't deligate
             * any further calls to the concrete
             * cipher methods.
             */
            signerState = ST_UNKNOWN;

            /**
             * Return the appropriate error and
             * exit gracefully. 
             */
            return( ESGNRIINVLDALGO );
        }
    }

    /**
     * Set all the function pointers
     * to the actual concrete cipher 
     * methods based on the algo Id.
     */
    switch(params.algoID)
    {
    #if( ( VLT_ENABLE_CIPHER_DES == VLT_ENABLE ) && ( VLT_ENABLE_CIPHER_TDES == VLT_ENABLE ) )
        case VLT_ALG_SIG_MAC_ISO9797_ALG1:
        case VLT_ALG_SIG_MAC_ISO9797_ALG3:  
            theSigner.signerInit = SignerIso9797Init;
            theSigner.signerClose = SignerIso9797Close;
            theSigner.signerDoFinal = SignerIso9797DoFinal;
            theSigner.signerUpdate = SignerIso9797Update;
            theSigner.signerGetBlockSize = SignerIso9797GetBlockSize;
            break;
    #endif /*( ( VLT_ENABLE_CIPHER_DES == VLT_INCLUDE ) && ( VLT_ENABLE_CIPHER_DES == VLT_ENABLE_CIPHER_TDES ) )*/

    #if( VLT_ENABLE_CIPHER_AES == VLT_ENABLE )
        case VLT_ALG_SIG_CMAC_AES:  
            theSigner.signerInit = SignerAesCmacInit;
            theSigner.signerClose = SignerAesCmacClose;
            theSigner.signerDoFinal = SignerAesCmacDoFinal;
            theSigner.signerUpdate = SignerAesCmacUpdate;
            theSigner.signerGetBlockSize = SignerAesCmacGetBlockSize;
            break;          
    #endif /* ( VLT_ENABLE_CIPHER_AES == VLT_ENABLE ) */

        default:
            return( ESGNRINOTSUPPORTED );
    }   

    /**
     * Check the padding scheme. The only padding scheme 
     * supported by our signers is ISO9797 Padding Method 2.
     */
    if( PADDING_ISO9797_METHOD2 != params.paddingScheme )
    {
        return( ESGNRIIVLDPAD );
    }   

    /**
     * Delegate the call to the initialisation method
     * of the appropriate signer.
     */
    status = theSigner.signerInit( opMode, pKey, pParams );

    /**
     * Prepare to accept the first block 
     * of data.
     */
    if( VLT_OK == status )
    {
        signerState = ST_INITIALISED;
    }

    return( status );
}

VLT_STS SymmetricSignerClose( void )
{
    if( ST_UNKNOWN != signerState )
    {
        return( theSigner.signerClose() );
    }
    return( ESGNRCLSNOTSUPPORTED );
}


VLT_STS SymmetricSignerDoFinal(
    VLT_PU8 pMessage, 
    VLT_U32 messageLen, 
    VLT_U32 messageCapacity, 
    VLT_PU8 pMac, 
    VLT_PU32 pMacLen, 
    VLT_U32 macCapacity )
{
    VLT_STS status = VLT_FAIL;

    if( ( ST_UNKNOWN == signerState ) ||
        ( ST_FINALISED == signerState ) )
    {
        return( ESGNRDFNOTSUPPORTED );
    }

    /**
     * Ensure we haven't been passed an null 
     * message pointer.
     */
    if( NULL == pMessage )
    {
        return( ESGNRDFNULLMSG );
    }

    /**
     * This signer doesn't deal with zero length
     * messages.
     */
    if( 0 == messageLen )
    {
        return( ESGNRDFZEROMSGLEN );
    }

    /**
     * Ensure we haven't been passed an null 
     * mac pointer.
     */
    if( NULL == pMac )
    {
        return( ESGNRDFNULLMAC );
    }

    /**
     * Ensure we haven't been passed an null 
     * mac pointer.
     */
    if( NULL == pMacLen )
    {
        return( ESGNRDFNULLMACLEN );
    }

    /**
     * Delegate the call to the DoFinal method
     * of the appropriate signer.
     */
    if( VLT_OK == ( status = theSigner.signerDoFinal( 
        pMessage, 
        messageLen, 
        messageCapacity,
        pMac, 
        pMacLen, 
        macCapacity ) ) )
    {
        /**
         * Update the signer state.
         */
        signerState = ST_FINALISED;
    }
    
    return ( status );
}

VLT_STS SymmetricSignerUpdate( VLT_PU8 pMessage, VLT_U32 messageLen, VLT_U32 messageCapacity )
{
    VLT_STS status = VLT_FAIL;

    if( ( ST_UNKNOWN == signerState ) ||
        ( ST_FINALISED == signerState ) )
    {
        return( ESGNRUPDNOTSUPPORTED );
    }

    /**
     * Ensure we haven't been passed an null 
     * message pointer.
     */
    if( NULL == pMessage )
    {
        return( ESGNRUPNULLMSG );
    }

    /**
     * This signer doesn't deal with zero length
     * messages.
     */
    if( 0 == messageLen )
    {
        return( ESGNRUPZEROMSGLEN );
    }

    /**
     * Update only deals with data lengths
     * multiple of the block size, if the 
     * client has passed us anything else 
     * other than that then we should exit
     * gracefully-ish!
     */
    if( 0 != ( messageLen % SymmetricSignerGetBlockSize() ) )
    {
        return( ESGNRUPMINVLDLEN );
    }

    /**
     * Delegate the call to the Update method
     * of the appropriate signer.
     */
    if( VLT_OK == ( status = theSigner.signerUpdate( pMessage, 
        messageLen, messageCapacity ) ) )
    {
        /**
         * Update the signer state.
         */
        signerState = ST_UPDATED;
    }

    return( status );
}

VLT_U16 SymmetricSignerGetBlockSize( void )
{   
    if( NULL == theSigner.signerGetBlockSize )
    {
        return( EMETHODNOTSUPPORTED );
    }

    /**
     * Delegate the call to the GetBlockSize method
     * of the appropriate signer.
     */
    return( theSigner.signerGetBlockSize() );
}
