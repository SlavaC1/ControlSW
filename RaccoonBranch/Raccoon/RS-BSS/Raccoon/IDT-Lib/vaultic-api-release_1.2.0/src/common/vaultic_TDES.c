/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#include "vaultic_common.h"
#include "vaultic_mem.h"
#include "vaultic_cipher.h"
#include "vaultic_DES.h"
#include "vaultic_TDES.h"

/**
 * Error Codes
 */
#define ETDESIIVLDKEY               VLT_ERROR( VLT_TDES, 0 ) 
#define ETDESIIVLDPRM               VLT_ERROR( VLT_TDES, 1 ) 
#define ETDESDFFLD                  VLT_ERROR( VLT_TDES, 2 ) 
#define ETDESIIVLDKEYMD             VLT_ERROR( VLT_TDES, 3 )
#define ETDESIIVLDKEYTP             VLT_ERROR( VLT_TDES, 4 )
#define ETDESIIVLDKEYCD             VLT_ERROR( VLT_TDES, 5 )
#define ETDESTST3KEEEFLDENCP        VLT_ERROR( VLT_TDES, 6 )
#define ETDESTST3KEEEFLDDECP        VLT_ERROR( VLT_TDES, 7 )
#define ETDESTST2KEDEFLDENCP        VLT_ERROR( VLT_TDES, 8 )
#define ETDESTST2KEDEFLDDECP        VLT_ERROR( VLT_TDES, 9 )
#define ETDESTST3KEDEFLDENCP        VLT_ERROR( VLT_TDES, 10 )
#define ETDESTST3KEDEFLDDECP        VLT_ERROR( VLT_TDES, 11 )

#if( ( VLT_ENABLE_CIPHER_TDES == VLT_ENABLE ) && ( VLT_ENABLE_CIPHER_DES == VLT_ENABLE ) )

/**
 * Private Data 
 */
static VLT_U8 desKey1[DES_KEY_SIZE];
static VLT_U8 desKey2[DES_KEY_SIZE];
static VLT_U8 desKey3[DES_KEY_SIZE];

static KEY_BLOB K1;
static KEY_BLOB K2;
static KEY_BLOB K3;
static VLT_U8 operationMode;
static VLT_U8 keyingMode;


VLT_STS TDesInit( VLT_U8 opMode, const KEY_BLOB* pKey, VLT_PU8 pParams )
{
    /**
     * Make sure we have a valid key
     */
    if( ( NULL == pKey )||
        ( ( VLT_KEY_TDES_3K != pKey->keyType ) && 
        ( VLT_KEY_TDES_2K != pKey->keyType ) ) )
    {
        return( ETDESIIVLDKEY );
    }

    /**
     * Make sure we have a valid params pointer
     */
    if( NULL == pParams )
    {
        return( ETDESIIVLDPRM );
    }
    
    /**
     *  Cache the operational mode (Encrypt/Decrypt)     
     */
    if( ( VLT_ENCRYPT_MODE == opMode ) || 
        ( VLT_DECRYPT_MODE == opMode ) )
    {
        operationMode = opMode; 
    }
    else
    {
        return( ETDESIIVLDKEYCD );
    }


    /**
     *  Cache the keying mode (EEE/EDE)  
     */
    if( ( TDES_EDE == (VLT_U8)(*pParams)) || 
        ( TDES_EEE == (VLT_U8)(*pParams) ) )
    {
        keyingMode = (VLT_U8)(*pParams);    
    }
    else
    {
        return( ETDESIIVLDKEYMD );
    }

    /**
     *  Modify appropriately and cache the key   
     */
    K1 = *pKey;
    K1.keySize = DES_KEY_SIZE;
    K1.keyValue = desKey1;
    
    K2 = *pKey;
    K2.keySize = DES_KEY_SIZE;
    K2.keyValue = desKey2;

    K3 = *pKey;
    K3.keySize = DES_KEY_SIZE;
    K3.keyValue = desKey3;

    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( K1.keyValue, &pKey->keyValue[(DES_KEY_SIZE * 0 )], DES_KEY_SIZE );
    (void)host_memcpy( K2.keyValue, &pKey->keyValue[(DES_KEY_SIZE * 1 )], DES_KEY_SIZE );
    if( VLT_KEY_TDES_2K == pKey->keyType )
    {
        /**
         * For TDES 2K keys K1 == K3
         *
         */
        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_memcpy( K3.keyValue, &pKey->keyValue[(DES_KEY_SIZE * 0 )], DES_KEY_SIZE );
    }
    else if( VLT_KEY_TDES_3K == pKey->keyType )
    {
        /**
         * For TDES 3K keys K1 != K3
         *
         */
        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_memcpy( K3.keyValue, &pKey->keyValue[(DES_KEY_SIZE * 2 )], DES_KEY_SIZE );
    }
    else
    {
        return( ETDESIIVLDKEYTP );
    }   

    /**
     * All the cached keys should be of type
     * DES so the underlying cipher can handle it.
     */
    K1.keyType = VLT_KEY_DES;
    K2.keyType = VLT_KEY_DES;
    K3.keyType = VLT_KEY_DES;

    return( VLT_OK );
}

VLT_STS TDesClose( void )
{
    /**
     *  Clear the key    
     */ 
    (void)host_memset( (VLT_PU8)&K1, 0x00, sizeof(KEY_BLOB) );    
    (void)host_memset( (VLT_PU8)&K2, 0x00, sizeof(KEY_BLOB) );    
    (void)host_memset( (VLT_PU8)&K3, 0x00, sizeof(KEY_BLOB) );    

    return( DesClose() );
}

VLT_STS TDesDoFinal( VLT_PU8 pDataIn, VLT_PU32 pDataInLen, VLT_PU8 pDataOut, VLT_PU32 pDataOutLen )
{
    VLT_STS status = VLT_FAIL;  
    KEY_BLOB* pKeys[ROUNDS] = { &K1, &K2, &K3 };
    VLT_U8 n = 0;
    VLT_U8 dummy = 0;
    VLT_PU8 pu8OperationData = pDataIn; /* For first round Op data is the input data. */

    if( VLT_DECRYPT_MODE == operationMode )
    {
        pKeys[0] = &K3;
        pKeys[1] = &K2;
        pKeys[2] = &K1;
    }

    for( n = 0; n < ROUNDS; n++ )
    {
        /**
         * If we are on EDE we need to change to decrypt mode.
         */
        if( ( 1 == n ) && ( TDES_EDE == keyingMode ) )
        {
            operationMode = VLT_DECRYPT_MODE;
        }

        if( VLT_OK == ( status = DesInit( operationMode, pKeys[n], &dummy ) ) )
        {   
            if( VLT_OK != ( status = DesDoFinal( 
                pu8OperationData, 
                pDataInLen, 
                pDataOut, 
                pDataOutLen ) ) )
            {
                return( status );
            }   
        }
        else
        {
            return( status );
        }
        
        if ( 0 == n )
        {
            /* 
             * Update the pu8OperationData ptr to ensure the remaining rounds
               input data is the output of the previous round.
             */
            pu8OperationData = pDataOut;
        }

        /**
         * If we are on EDE we need to change back to encrypt mode.
         */
        if( ( 1 == n ) && ( TDES_EDE == keyingMode ) )
        {
            operationMode = VLT_ENCRYPT_MODE;
        }
    }

    return( VLT_OK );
}

VLT_STS TDesUpdate( VLT_PU8 pDataIn, VLT_PU32 pDataInLen, VLT_PU8 pDataOut, VLT_PU32 pDataOutLen )
{
    VLT_STS status = VLT_FAIL;  
    KEY_BLOB* pKeys[ROUNDS] = { &K1, &K2, &K3 };
    VLT_U8 n = 0;
    VLT_U8 dummy = 0;
    VLT_PU8 pu8OperationData = pDataIn; /* For first round Op data is the input data. */
    

    if( VLT_DECRYPT_MODE == operationMode )
    {
        pKeys[0] = &K3;
        pKeys[1] = &K2;
        pKeys[2] = &K1;
    }

    for( n = 0; n < ROUNDS; n++ )
    {
        /**
         * If we are on EDE we need to change to decrypt mode.
         */
        if( ( 1 == n ) && 
            ( TDES_EDE == keyingMode ) && 
            ( VLT_ENCRYPT_MODE == operationMode ) )
        {
            operationMode = VLT_DECRYPT_MODE;
        }
        /**
         * If we are on EDE Decrypt we need to change to encrypt mode.
         */
        else if ( ( 1 == n ) && 
            ( TDES_EDE == keyingMode ) && 
            ( VLT_DECRYPT_MODE == operationMode ) )
        {
            operationMode = VLT_ENCRYPT_MODE;
        }


        if( VLT_OK == ( status = DesInit( operationMode, pKeys[n], &dummy ) ) )
        {   
            if( VLT_OK != ( status = DesUpdate( 
                pu8OperationData, 
                pDataInLen, 
                pDataOut, 
                pDataOutLen) ) )
            {
                return( status );
            }           
        }
        else
        {
            return( status );
        }

        if ( 0 == n )
        {
            /* 
             * Update the pu8OperationData ptr to ensure the remaining rounds
               input data is the output of the previous round.
             */
            pu8OperationData = pDataOut;
        }

        /**
         * If we are on EDE encrypt we need to change back to encrypt mode.
         */
        if( ( 1 == n ) && 
            ( TDES_EDE == keyingMode ) &&
            ( VLT_DECRYPT_MODE == operationMode ) )
        {
            operationMode = VLT_ENCRYPT_MODE;
        }
        /**
         * If we are on EDE decrypt we need to change back to decrypt mode.
         */
        else if( ( 1 == n ) && 
            ( TDES_EDE == keyingMode ) &&
            ( VLT_ENCRYPT_MODE == operationMode ) )
        {
            operationMode = VLT_DECRYPT_MODE;
        }
    }

    return( VLT_OK );
}

VLT_U16 TDesGetBlockSize( void )
{
    return( DES_BLOCK_SIZE );
}

#endif /* ( VLT_ENABLE_CIPHER_TDES == VLT_ENABLE ) */

#if( VLT_ENABLE_CIPHER_TESTS == VLT_ENABLE )

VLT_STS TDes3KEEETest()
{
    VLT_U8 plainTextVector1[] = {0x4e, 0x6f, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74 };
    VLT_U8 cipherTextVector1[] = {0x3b, 0x8f, 0xad, 0x2b, 0x39, 0xe3, 0x86, 0xbc };
    VLT_U8 desKey[] = 
    { 
        0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 
        0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 
        0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF 
    };
    KEY_BLOB keyBlob;
    VLT_U8 params = TDES_EEE;
    VLT_U8 text[( DES_BLOCK_SIZE * 2 ) ];

    /**
     * Text to be encrypted with the known test plain text vector.
     */
    VLT_U32 plainTextSize = (sizeof(plainTextVector1)/sizeof(VLT_U8));
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( text, plainTextVector1, plainTextSize );

    /**
     * Setup the key blob
     */
    keyBlob.keyType = VLT_KEY_TDES_3K;
    keyBlob.keySize = TDES_3KEY_SIZE;
    keyBlob.keyValue = desKey;

    TDesInit( VLT_ENCRYPT_MODE, &keyBlob, &params ); 
    TDesUpdate( text, &plainTextSize, text, &plainTextSize );

    if( 0 != host_memcmp( text, cipherTextVector1, DES_BLOCK_SIZE ) )
    {
        return( ETDESTST3KEEEFLDENCP );
    }

    TDesInit( VLT_DECRYPT_MODE, &keyBlob, &params ); 
    TDesUpdate( text, &plainTextSize, text, &plainTextSize );
    
    if( 0 != host_memcmp( text, plainTextVector1, DES_BLOCK_SIZE ) )
    {
        return( ETDESTST3KEEEFLDDECP );
    }

    return( VLT_OK );
}

VLT_STS TDes2KEDETest()
{
    VLT_U8 plainTextVector1[] = {0x4e, 0x6f, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74 };
    VLT_U8 cipherTextVector1[] = { 0x3d, 0x25, 0x7e, 0x11, 0x77, 0x01, 0xb7, 0xee };

    VLT_U8 desKey[] = 
    { 
        0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,         
        0x10, 0x32, 0x54, 0x76, 0x98, 0xBA, 0xDC, 0xFE 
    };
    KEY_BLOB keyBlob;
    VLT_U8 params = TDES_EDE;
    VLT_U8 text[( DES_BLOCK_SIZE * 2 ) ];

    /**
     * Text to be encrypted with the known test plain text vector.
     */
    VLT_U32 plainTextSize = (sizeof(plainTextVector1)/sizeof(VLT_U8));
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( text, plainTextVector1, plainTextSize );

    /**
     * Setup the key blob
     */
    keyBlob.keyType = VLT_KEY_TDES_2K;
    keyBlob.keySize = TDES_2KEY_SIZE;
    keyBlob.keyValue = desKey;
    
    TDesInit( VLT_ENCRYPT_MODE, &keyBlob, &params ); 
    TDesUpdate( text, &plainTextSize, text, &plainTextSize );

    if( 0 != host_memcmp( text, cipherTextVector1, DES_BLOCK_SIZE ) )
    {
        return( ETDESTST2KEDEFLDENCP );
    }

    TDesInit( VLT_DECRYPT_MODE, &keyBlob, &params ); 
    TDesUpdate( text, &plainTextSize, text, &plainTextSize );
    
    if( 0 != host_memcmp( text, plainTextVector1, DES_BLOCK_SIZE ) )
    {
        return( ETDESTST2KEDEFLDDECP );
    }

    return( VLT_OK );
}

VLT_STS TDes3KEDETest()
{
    VLT_U8 plainTextVector1[] = {0x4e, 0x6f, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74 };
    VLT_U8 cipherTextVector1[] = { 0x63, 0x7c, 0xbc, 0x27, 0x03, 0xb9, 0x9b, 0x15 };
    

    VLT_U8 desKey[] = 
    { 
        0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,         
        0x10, 0x32, 0x54, 0x76, 0x98, 0xBA, 0xDC, 0xFE, 
        0xA1, 0xB2, 0xC4, 0xD6, 0xE8, 0xFA, 0x8C, 0x9E 
    };
    KEY_BLOB keyBlob;
    VLT_U8 params = TDES_EDE;
    VLT_U8 text[( DES_BLOCK_SIZE * 2 ) ];

    /**
     * Text to be encrypted with the known test plain text vector.
     */
    VLT_U32 plainTextSize = (sizeof(plainTextVector1)/sizeof(VLT_U8));
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( text, plainTextVector1, plainTextSize );

    /**
     * Setup the key blob
     */
    keyBlob.keyType = VLT_KEY_TDES_3K;
    keyBlob.keySize = TDES_3KEY_SIZE;
    keyBlob.keyValue = desKey;

    TDesInit( VLT_ENCRYPT_MODE, &keyBlob, &params ); 
    TDesUpdate( text, &plainTextSize, text, &plainTextSize );

    if( 0 != host_memcmp( text, cipherTextVector1, DES_BLOCK_SIZE ) )
    {
        return( ETDESTST3KEDEFLDENCP );
    }

    TDesInit( VLT_DECRYPT_MODE, &keyBlob, &params ); 
    TDesUpdate( text, &plainTextSize, text, &plainTextSize );
    
    if( 0 != host_memcmp( text, plainTextVector1, DES_BLOCK_SIZE ) )
    {
        return( ETDESTST3KEDEFLDDECP );
    }

    return( VLT_OK );
}

VLT_STS TDesTest( void )
{
    VLT_STS status = VLT_FAIL;

    if( VLT_OK != ( status = TDes3KEEETest() ) )
    {
        return( status );
    }

    if( VLT_OK != ( status = TDes2KEDETest() ) )
    {
        return( status );
    }

    if( VLT_OK != ( status = TDes3KEDETest() ) )
    {
        return( status );
    }

    return( VLT_OK );
}
#endif /*( VLT_ENABLE_CIPHER_TESTS == VLT_ENABLE )*/
