/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#ifndef VAULTIC_PADDING_H
#define VAULTIC_PADDING_H

#ifdef __cplusplus
    extern "C"
    {
#endif

#define PADDING_METHOD2_MARK        0x80
#define MAX_PKCS5_PAD_SZ            0x08

VLT_STS PaddingAdd( VLT_U8 paddingMode, VLT_U16 blockSize, VLT_PU8 pData, 
    VLT_PU32 pDataLen, VLT_U32 bufferCapacity );

VLT_STS PaddingRemove( VLT_U8 paddingMode, VLT_U16 blockSize, VLT_PU8 pData, 
    VLT_PU32 pDataLen );

VLT_STS PaddingTests( void );

#ifdef __cplusplus
    };
#endif

#endif/*VAULTIC_PADDING_H*/
