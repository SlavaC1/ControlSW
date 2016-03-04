/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#ifndef _VAULTIC_TDES3K_EEE_
#define _VAULTIC_TDES3K_EEE_

#ifdef __cplusplus
    extern "C"
    {
#endif

#define TDES_EEE    21
#define TDES_EDE    42

#define ROUNDS          3
#define TDES_3KEY_SIZE  ( DES_KEY_SIZE  * ROUNDS )
#define TDES_2KEY_SIZE  ( DES_KEY_SIZE  * 2 )



VLT_STS TDesInit( VLT_U8 opMode, const KEY_BLOB* pKey, VLT_PU8 pParams );
VLT_STS TDesClose( void );

VLT_STS TDesDoFinal( VLT_PU8 pDataIn, VLT_PU32 pDataInLen, VLT_PU8 pDataOut, VLT_PU32 pDataOutLen );
VLT_STS TDesUpdate( VLT_PU8 pDataIn, VLT_PU32 pDataInLen, VLT_PU8 pDataOut, VLT_PU32 pDataOutLen );
VLT_U16 TDesGetBlockSize( void );
VLT_STS TDesTest( void );

#ifdef __cplusplus
    };
#endif

#endif/*_VAULTIC_TDES3K_EEE_*/
