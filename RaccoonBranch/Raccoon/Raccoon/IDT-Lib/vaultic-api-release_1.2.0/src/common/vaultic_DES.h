/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#ifndef VAULTIC_DES_H
#define VAULTIC_DES_H


#ifdef __cplusplus
    extern "C"
    {
#endif

    VLT_STS DesInit( VLT_U8 opMode, const KEY_BLOB* pKey, VLT_PU8 pParams );
    VLT_STS DesClose( void );

    VLT_STS DesDoFinal( VLT_PU8 pDataIn, VLT_PU32 pDataInLen, VLT_PU8 pDataOut, VLT_PU32 pDataOutLen );
    VLT_STS DesUpdate( VLT_PU8 pDataIn, VLT_PU32 pDataInLen, VLT_PU8 pDataOut, VLT_PU32 pDataOutLen );
    VLT_U16 DesGetBlockSize( void );
    VLT_STS DesTest( void );

#ifdef __cplusplus
    };
#endif


#endif//VAULTIC_DES_H
