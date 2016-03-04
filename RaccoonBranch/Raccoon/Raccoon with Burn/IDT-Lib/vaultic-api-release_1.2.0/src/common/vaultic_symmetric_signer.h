/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#ifndef VAULTIC_SYMMETRIC_SIGNER_H
#define VAULTIC_SYMMETRIC_SIGNER_H

#ifdef __cplusplus
    extern "C"
    {
#endif



/**
 * \struct _signerParams
 *
 * \brief Parmaters used by the Signer.
 */
typedef struct _signerParams
{
    VLT_U8 algoID;
    VLT_U8 paddingScheme;
    VLT_U8 ivSize;
    VLT_PU8 pIV;
} SIGNER_PARAMS;


    VLT_STS SymmetricSignerInit( VLT_U8 opMode, const KEY_BLOB* pkey, VLT_PU8 pParams );
    VLT_STS SymmetricSignerClose( void );

    VLT_STS SymmetricSignerDoFinal( 
        VLT_PU8 pMessage, 
        VLT_U32 messageLen, 
        VLT_U32 messageCapacity, 
        VLT_PU8 pMac, 
        VLT_PU32 pMacLen, 
        VLT_U32 macCapacity );

    VLT_STS SymmetricSignerUpdate( VLT_PU8 pMessage, VLT_U32 messageLen, VLT_U32 messageCapacity );
    VLT_U16 SymmetricSignerGetBlockSize( void );

#ifdef __cplusplus
    };
#endif

#endif//VAULTIC_SIGNER_H
