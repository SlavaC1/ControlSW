/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#ifndef _VAULTIC_SIGNER_AES_CMAC_
#define _VAULTIC_SIGNER_AES_CMAC_

#ifdef __cplusplus
    extern "C"
    {
#endif

    VLT_STS SignerAesCmacInit( VLT_U8 opMode, const KEY_BLOB* pkey, VLT_PU8 pParams );
    VLT_STS SignerAesCmacClose( void );
    VLT_STS SignerAesCmacDoFinal( 
        VLT_PU8 pMessage, 
        VLT_U32 messageLen, 
        VLT_U32 messageCapacity, 
        VLT_PU8 pMac, 
        VLT_PU32 pMacLen, 
        VLT_U32 macCapacity );

    VLT_STS SignerAesCmacUpdate( VLT_PU8 pMessage, VLT_U32 messageLen, VLT_U32 messageCapacity );
    VLT_U16 SignerAesCmacGetBlockSize( void );
    VLT_STS AesCMacTest();

#ifdef __cplusplus
    };
#endif

#endif//_VAULTIC_SIGNER_
