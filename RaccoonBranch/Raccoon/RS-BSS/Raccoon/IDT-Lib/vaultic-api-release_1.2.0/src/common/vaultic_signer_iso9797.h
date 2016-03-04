/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#ifndef _VAULTIC_SIGNER_ISO9797_
#define _VAULTIC_SIGNER_ISO9797_

#ifdef __cplusplus
    extern "C"
    {
#endif

    VLT_STS SignerIso9797Init( VLT_U8 opMode, const KEY_BLOB* pkey, VLT_PU8 pParams );
    VLT_STS SignerIso9797Close( void );
    VLT_STS SignerIso9797DoFinal( 
        VLT_PU8 pMessage, 
        VLT_U32 messageLen, 
        VLT_U32 messageCapacity, 
        VLT_PU8 pMac, 
        VLT_PU32 pMacLen, 
        VLT_U32 macCapacity );

    VLT_STS SignerIso9797Update( VLT_PU8 pMessage, VLT_U32 messageLen, VLT_U32 messageCapacity );
    VLT_U16 SignerIso9797GetBlockSize( void );

#ifdef __cplusplus
    };
#endif

#endif//_VAULTIC_SIGNER_ISO9797_
