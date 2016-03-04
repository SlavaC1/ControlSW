/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#include "vaultic_common.h"
#include "vaultic_symmetric_signer.h"
#include "vaultic_cipher.h"
#include "vaultic_mem.h"
#include "vaultic_DES.h"
#include "vaultic_TDES.h"
#include "vaultic_AES.h"
#include "vaultic_padding.h"
#include "vaultic_utils.h"
#include "vaultic_signer_aes_cmac.h"


/**
 * TODO: All the tests here are "cut and paste" driven. 
 * These test lend themselves very nicely to being data
 * driven. This is something that MUST be done before
 * we can ship the product out of the door !
 */

/**
 * Conditional Compilation Flag to 
 * add or remove the tests for 
 * all the ciphers supported. 
 */
#if( VLT_ENABLE_CIPHER_TESTS )

VLT_STS DoTDES_EEE_CBC_PKCS7( )
{
    VLT_STS status = VLT_FAIL ;
    CIPHER_PARAMS theParams;
    KEY_BLOB theKey;
    VLT_U32 textCapacity = 64;
    VLT_U32 textLength = 32;
    VLT_U8 text[128];
    VLT_U8 IV[] = { 0xAB, 0x6C, 0x7D, 0x15, 0xBA, 0xC6, 0xD7, 0x51,};
    VLT_U8 keyData[] = 
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
        0x10, 0x32, 0x54, 0x76, 0x98, 0xBA, 0xDC, 0xFE,
        0xA1, 0xB2, 0xC4, 0xD6, 0xE8, 0xFA, 0x8C, 0x9E 
    };          
    
    VLT_U8 plainTextVector1[] = 
    {
        //
        // "The Gods too are fond of a joke"
        //
        // Text size is 32 characters long.
        //

        0x54, 0x68, 0x65, 0x20, 0x67, 0x6f, 0x64, 0x73,
        0x20, 0x74, 0x6f, 0x6f, 0x20, 0x61, 0x72, 0x65,
        0x20, 0x66, 0x6f, 0x6e, 0x64, 0x20, 0x6f, 0x66,
        0x20, 0x61, 0x20, 0x6a, 0x6f, 0x6b, 0x65, 0x00
    };

    /**
     * Algorithm DES EEE
     * Padding PKCS7#
     * Block Mode CBC 
     * IV set to zeros
     */
    theParams.algoID = VLT_ALG_CIP_TDES_3K_EEE;
    theParams.paddingScheme = PADDING_PKCS7;
    theParams.chainMode = BLOCK_MODE_CBC;
    theParams.pIV = IV;
    
    /**
     * Key Type KEY_TDES_3K
     * Key Size 24 Bytes
     */
    theKey.keyType = VLT_KEY_TDES_3K;
    theKey.keySize = sizeof(keyData)/sizeof(VLT_U8);
    theKey.keyValue = keyData;

    /**
     * Initialise the Cipher
     */
    if( VLT_OK != ( status = CipherInit( VLT_ENCRYPT_MODE , &theKey, (VLT_PU8)&theParams ) ) )
    {
        return( status );
    }
    
    /**
     * Copy the plain text vector in the text.
     */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( text, plainTextVector1, textLength );

    /**
     * Encrypt the text
     */
    if( VLT_OK != ( status = CipherDoFinal( text, textLength, textCapacity, text, &textLength, textCapacity ) ) )
    {
        return( status );
    }

    /**
     * Initialise the Cipher
     */
    if( VLT_OK != ( status = CipherInit( VLT_DECRYPT_MODE , &theKey, (VLT_PU8)&theParams ) ) )
    {
        return( status );
    }

    /**
     * Decrypt the text
     */
    if( VLT_OK != ( status = CipherDoFinal( text, textLength, textCapacity, text, &textLength, textCapacity ) ) )
    {
        return( status );
    }

    return( VLT_OK );
}

VLT_STS DoTDES3K_EDE_CBC_PKCS7( )
{
    VLT_STS status = VLT_FAIL ;
    CIPHER_PARAMS theParams;
    KEY_BLOB theKey;
    VLT_U32 textCapacity = 64;
    VLT_U32 textLength;
    VLT_U8 text[64];
    
    VLT_U8 vectorIV[] = 
    {
        0xab, 0x6c, 0x7d, 0x15, 0xba, 0xc6, 0xd7, 0x51
    };

    VLT_U8 vectorKey[] = 
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 
        0x10, 0x32, 0x54, 0x76, 0x98, 0xba, 0xdc, 0xfe, 
        0xa1, 0xb3, 0xc4, 0xd6, 0xe9, 0xfb, 0x8c, 0x9e 
    };

    VLT_U8 vectorPlainText[] = 
    {
        0x54, 0x68, 0x65, 0x20, 0x67, 0x6f, 0x64, 0x73, 
        0x20, 0x74, 0x6f, 0x6f, 0x20, 0x61, 0x72, 0x65, 
        0x20, 0x66, 0x6f, 0x6e, 0x64, 0x20, 0x6f, 0x66, 
        0x20, 0x61, 0x20, 0x6a, 0x6f, 0x6b, 0x65, 0x00 

    };

    VLT_U8 vectorCipherText[] = 
    {
        0x0c, 0x81, 0xcd, 0x1b, 0xb9, 0xc9, 0xf9, 0x6c, 
        0x7d, 0xc5, 0x52, 0xe8, 0x96, 0x54, 0xf9, 0xea, 
        0x98, 0x6a, 0xac, 0x0c, 0x79, 0x89, 0x15, 0x83, 
        0x7c, 0xb2, 0x6a, 0x09, 0xf7, 0x51, 0x1b, 0x23, 
        0xfe, 0xdc, 0x5d, 0x04, 0x84, 0xee, 0xc0, 0x5a 
    };

    /**
     * Algorithm DES EDE
     * Padding PKCS7#
     * Block Mode CBC 
     */
    theParams.algoID = VLT_ALG_CIP_TDES_3K_EDE;
    theParams.paddingScheme = PADDING_PKCS7;
    theParams.chainMode = BLOCK_MODE_CBC;
    theParams.pIV = vectorIV;
    
    /**
     * Key Type KEY_TDES_3K
     * Key Size 24 Bytes
     */
    theKey.keyType = VLT_KEY_TDES_3K;
    theKey.keySize = sizeof(vectorKey)/sizeof(VLT_U8);
    theKey.keyValue = vectorKey;

    textLength = ( sizeof(vectorPlainText)/sizeof(VLT_U8) );

    /**
     * Initialise the Cipher
     */
    if( VLT_OK != ( status = CipherInit( VLT_ENCRYPT_MODE , &theKey, (VLT_PU8)&theParams ) ) )
    {
        return( status );
    }
    
    /**
     * Copy the plain text vector in the text.
     */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( text, vectorPlainText, textLength );

    /**
     * Encrypt the text
     */
    if( VLT_OK != ( status = CipherDoFinal( text, textLength, textCapacity, text, &textLength, textCapacity ) ) )
    {
        return( status );
    }

    /**
     * Initialise the Cipher
     */
    if( VLT_OK != ( status = CipherInit( VLT_DECRYPT_MODE , &theKey, (VLT_PU8)&theParams ) ) )
    {
        return( status );
    }

    textLength = ( sizeof(vectorCipherText)/sizeof(VLT_U8) );

    if( 0 != host_memcmp( text, vectorCipherText, textLength ) )
    {
        return( VLT_FAIL );
    }

    /**
     * Decrypt the text
     */
    if( VLT_OK != ( status = CipherDoFinal( text, textLength, textCapacity, text, &textLength, textCapacity ) ) )
    {
        return( status );
    }

    textLength = ( sizeof(vectorPlainText)/sizeof(VLT_U8) );

    if( 0 != host_memcmp( text, vectorPlainText, textLength ) )
    {
        return( VLT_FAIL );
    }

    return( VLT_OK );
}
VLT_STS DoTDES3K_EDE_ECB_PKCS7( )
{
    VLT_STS status = VLT_FAIL ;
    CIPHER_PARAMS theParams;
    KEY_BLOB theKey;
    VLT_U32 textCapacity = 64;
    VLT_U32 textLength;
    VLT_U8 text[64];
    
    VLT_U8 vectorKey[] = 
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 
        0x10, 0x32, 0x54, 0x76, 0x98, 0xba, 0xdc, 0xfe, 
        0xa1, 0xb3, 0xc4, 0xd6, 0xe9, 0xfb, 0x8c, 0x9e 
    };

    VLT_U8 vectorPlainText[] = 
    {
        0x54, 0x68, 0x65, 0x20, 0x67, 0x6f, 0x64, 0x73, 
        0x20, 0x74, 0x6f, 0x6f, 0x20, 0x61, 0x72, 0x65, 
        0x20, 0x66, 0x6f, 0x6e, 0x64, 0x20, 0x6f, 0x66, 
        0x20, 0x61, 0x20, 0x6a, 0x6f, 0x6b, 0x65, 0x00 
    };

    VLT_U8 vectorCipherText[] = 
    {
        0x4e, 0x85, 0x11, 0x1b, 0x80, 0xa2, 0x42, 0xae, 
        0xcf, 0x09, 0xf5, 0x39, 0x6e, 0xed, 0xee, 0x4c, 
        0x4d, 0xc8, 0x67, 0xb3, 0xa8, 0xc3, 0x02, 0x9f, 
        0x0e, 0x52, 0x86, 0x3f, 0x7e, 0x6b, 0x8e, 0xc7, 
        0x29, 0xf7, 0x16, 0x3f, 0xa9, 0xfd, 0xbc, 0x45 
    };

    /**
     * Algorithm DES EDE
     * Padding PKCS7#
     * Block Mode CBC 
     */
    theParams.algoID = VLT_ALG_CIP_TDES_3K_EDE;
    theParams.paddingScheme = PADDING_PKCS7;
    theParams.chainMode = BLOCK_MODE_ECB;
    //host_memcpy( theParams.IV, vectorIV, DES_BLOCK_SIZE );
    
    /**
     * Key Type KEY_TDES_3K
     * Key Size 24 Bytes
     */
    theKey.keyType = VLT_KEY_TDES_3K;
    theKey.keySize = sizeof(vectorKey)/sizeof(VLT_U8);
    theKey.keyValue = vectorKey;

    textLength = ( sizeof(vectorPlainText)/sizeof(VLT_U8) );

    /**
     * Initialise the Cipher
     */
    if( VLT_OK != ( status = CipherInit( VLT_ENCRYPT_MODE , &theKey, (VLT_PU8)&theParams ) ) )
    {
        return( status );
    }
    
    /**
     * Copy the plain text vector in the text.
     */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( text, vectorPlainText, textLength );

    /**
     * Encrypt the text
     */
    if( VLT_OK != ( status = CipherDoFinal( text, textLength, textCapacity, text, &textLength, textCapacity ) ) )
    {
        return( status );
    }

    /**
     * Initialise the Cipher
     */
    if( VLT_OK != ( status = CipherInit( VLT_DECRYPT_MODE , &theKey, (VLT_PU8)&theParams ) ) )
    {
        return( status );
    }

    textLength = ( sizeof(vectorCipherText)/sizeof(VLT_U8) );

    if( 0 != host_memcmp( text, vectorCipherText, textLength ) )
    {
        return( VLT_FAIL );
    }

    /**
     * Decrypt the text
     */
    if( VLT_OK != ( status = CipherDoFinal( text, textLength, textCapacity, text, &textLength, textCapacity ) ) )
    {
        return( status );
    }

    textLength = ( sizeof(vectorPlainText)/sizeof(VLT_U8) );

    if( 0 != host_memcmp( text, vectorPlainText, textLength ) )
    {
        return( VLT_FAIL );
    }

    return( VLT_OK );
}


VLT_STS DoTDES2K_EDE_CBC_PKCS7( )
{
    VLT_STS status = VLT_FAIL ;
    CIPHER_PARAMS theParams;
    KEY_BLOB theKey;
    VLT_U32 textCapacity = 64;
    VLT_U32 textLength;
    VLT_U8 text[128];

    VLT_U8 vectorKey[] = 
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 
        0x10, 0x32, 0x54, 0x76, 0x98, 0xba, 0xdc, 0xfe
    };
    VLT_U8 vectorIV[] = 
    {
        0xab, 0x6c, 0x7d, 0x15, 0xba, 0xc6, 0xd7, 0x51 
    };

    VLT_U8 vectorPlainText[] = 
    {
        0x54, 0x68, 0x65, 0x20, 0x67, 0x6f, 0x64, 0x73, 
        0x20, 0x74, 0x6f, 0x6f, 0x20, 0x61, 0x72, 0x65, 
        0x20, 0x66, 0x6f, 0x6e, 0x64, 0x20, 0x6f, 0x66, 
        0x20, 0x61, 0x20, 0x6a, 0x6f, 0x6b, 0x65, 0x00 
    };
    VLT_U8 vectorCipherText[] = 
    {
        0x39, 0xae, 0xf2, 0x08, 0x7f, 0xbf, 0x0c, 0xf7, 
        0xe0, 0x21, 0x5f, 0xcc, 0x58, 0xac, 0x81, 0x39, 
        0x11, 0xf0, 0x61, 0x96, 0x41, 0xfb, 0x8e, 0xa3, 
        0xaa, 0x83, 0xdc, 0x28, 0x57, 0x98, 0x72, 0x90, 
        0x94, 0x7d, 0xc4, 0xee, 0x12, 0x64, 0xb3, 0x12 
    };

    /**
     * Algorithm DES EEE
     * Padding PKCS7#
     * Block Mode CBC 
     * IV set to zeros
     */
    theParams.algoID = VLT_ALG_CIP_TDES_2K_EDE;
    theParams.paddingScheme = PADDING_PKCS7;
    theParams.chainMode = BLOCK_MODE_CBC;
    theParams.pIV = vectorIV;
    
    /**
     * Key Type KEY_TDES_2K
     * Key Size 16 Bytes
     */
    theKey.keyType = VLT_KEY_TDES_2K;
    theKey.keySize = sizeof(vectorKey)/sizeof(VLT_U8);
    theKey.keyValue = vectorKey;
    /**
     * Initialise the Cipher
     */
    if( VLT_OK != ( status = CipherInit( VLT_ENCRYPT_MODE , &theKey, (VLT_PU8)&theParams ) ) )
    {
        return( status );
    }

    textLength = sizeof(vectorPlainText)/sizeof(VLT_U8);

    /**
     * Copy the plain text vector in the text.
     */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( text, vectorPlainText, textLength );

    /**
     * Encrypt the text
     */
    if( VLT_OK != ( status = CipherDoFinal( text, textLength, textCapacity, text, &textLength, textCapacity ) ) )
    {
        return( status );
    }
    
    textLength = sizeof(vectorCipherText)/sizeof(VLT_U8);

    if( 0 != host_memcmp( text, vectorCipherText, textLength ) )
    {
        return( VLT_FAIL );
    }

    /**
     * Initialise the Cipher
     */
    if( VLT_OK != ( status = CipherInit( VLT_DECRYPT_MODE , &theKey, (VLT_PU8)&theParams ) ) )
    {
        return( status );
    }

    /**
     * Decrypt the text
     */
    if( VLT_OK != ( status = CipherDoFinal( text, textLength, textCapacity, text, &textLength, textCapacity ) ) )
    {
        return( status );
    }

    textLength = sizeof(vectorPlainText)/sizeof(VLT_U8);

    if( 0 != host_memcmp( text, vectorPlainText, textLength ) )
    {
        return( VLT_FAIL );
    }

    return( VLT_OK );
}

VLT_STS DoTDES2K_EDE_ECB_PKCS7( )
{
    VLT_STS status = VLT_FAIL ;
    CIPHER_PARAMS theParams;
    KEY_BLOB theKey;
    VLT_U32 textCapacity = 64;
    VLT_U32 textLength;
    VLT_U8 text[128];

    VLT_U8 vectorKey[] = 
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 
        0x10, 0x32, 0x54, 0x76, 0x98, 0xba, 0xdc, 0xfe 
    };

    VLT_U8 vectorPlainText[] = 
    {
        0x54, 0x68, 0x65, 0x20, 0x67, 0x6f, 0x64, 0x73, 
        0x20, 0x74, 0x6f, 0x6f, 0x20, 0x61, 0x72, 0x65, 
        0x20, 0x66, 0x6f, 0x6e, 0x64, 0x20, 0x6f, 0x66, 
        0x20, 0x61, 0x20, 0x6a, 0x6f, 0x6b, 0x65, 0x00 
    };

    VLT_U8 vectorCipherText[] = 
    {
        0x14, 0x0b, 0x8e, 0x4c, 0x72, 0x1a, 0x1e, 0xcf, 
        0xb5, 0x30, 0xd1, 0x22, 0x3d, 0x8b, 0x3c, 0x02, 
        0x83, 0x25, 0x97, 0xbd, 0x88, 0xa2, 0x30, 0x71, 
        0xca, 0xff, 0xe7, 0xa0, 0xa6, 0xb1, 0xd0, 0x0b, 
        0x58, 0x02, 0x0a, 0xb4, 0xdc, 0x14, 0xa2, 0x4a 
    };

    /**
     * Algorithm DES EDE
     * Padding PKCS7#
     * Block Mode ECB
     */
    theParams.algoID = VLT_ALG_CIP_TDES_2K_EDE;
    theParams.paddingScheme = PADDING_PKCS7;
    theParams.chainMode = BLOCK_MODE_ECB;
    
    /**
     * Key Type KEY_TDES_2K
     * Key Size 16 Bytes
     */
    theKey.keyType = VLT_KEY_TDES_2K;
    theKey.keySize = sizeof(vectorKey)/sizeof(VLT_U8);
    theKey.keyValue = vectorKey;

    /**
     * Initialise the Cipher
     */
    if( VLT_OK != ( status = CipherInit( VLT_ENCRYPT_MODE , &theKey, (VLT_PU8)&theParams ) ) )
    {
        return( status );
    }

    textLength = sizeof(vectorPlainText)/sizeof(VLT_U8);

    /**
     * Copy the plain text vector in the text.
     */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( text, vectorPlainText, textLength );

    /**
     * Encrypt the text
     */
    if( VLT_OK != ( status = CipherDoFinal( text, textLength, textCapacity, text, &textLength, textCapacity ) ) )
    {
        return( status );
    }
    
    textLength = sizeof(vectorCipherText)/sizeof(VLT_U8);

    if( 0 != host_memcmp( text, vectorCipherText, textLength ) )
    {
        return( VLT_FAIL );
    }

    /**
     * Initialise the Cipher
     */
    if( VLT_OK != ( status = CipherInit( VLT_DECRYPT_MODE , &theKey, (VLT_PU8)&theParams ) ) )
    {
        return( status );
    }

    /**
     * Decrypt the text
     */
    if( VLT_OK != ( status = CipherDoFinal( text, textLength, textCapacity, text, &textLength, textCapacity ) ) )
    {
        return( status );
    }

    textLength = sizeof(vectorPlainText)/sizeof(VLT_U8);

    if( 0 != host_memcmp( text, vectorPlainText, textLength ) )
    {
        return( VLT_FAIL );
    }

    return( VLT_OK );
}

VLT_STS DoDES_CBC_PKCS7( )
{
    VLT_STS status = VLT_FAIL ;
    CIPHER_PARAMS theParams;
    KEY_BLOB theKey;
    VLT_U32 textCapacity = 64;
    VLT_U32 textLength ;
    VLT_U8 text[128];
    VLT_U8 vectorKey[] = 
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef 
    };

    VLT_U8 vectorIV[] = 
    {
        0xab, 0x6c, 0x7d, 0x15, 0xba, 0xc6, 0xd7, 0x51
    };

    VLT_U8 vectorPlainText[] = 
    {
        0x54, 0x68, 0x65, 0x20, 0x67, 0x6f, 0x64, 0x73, 
        0x20, 0x74, 0x6f, 0x6f, 0x20, 0x61, 0x72, 0x65, 
        0x20, 0x66, 0x6f, 0x6e, 0x64, 0x20, 0x6f, 0x66, 
        0x20, 0x61, 0x20, 0x6a, 0x6f, 0x6b, 0x65, 0x00 
    };

    VLT_U8 vectorCipherText[] = 
    {
        0xf4, 0x3b, 0x54, 0x59, 0x34, 0x4e, 0x21, 0xd7, 
        0x18, 0x03, 0x14, 0xdb, 0x28, 0x81, 0x06, 0xcc, 
        0xdc, 0x37, 0x69, 0x89, 0xe7, 0x16, 0xec, 0xb3, 
        0xc5, 0xcc, 0x04, 0x19, 0xa0, 0x66, 0x98, 0x45, 
        0xcf, 0xbf, 0x46, 0xbb, 0x29, 0x05, 0xd7, 0xe5 
    };

    /**
     * Algorithm DES 
     * Padding PKCS7#
     * Block Mode CBC    
     */
    theParams.algoID = VLT_ALG_CIP_DES;
    theParams.paddingScheme = PADDING_PKCS7;
    theParams.chainMode = BLOCK_MODE_CBC;
    theParams.pIV = vectorIV;
    
    /**
     * Key Type KEY_DES
     * Key Size 8 Bytes
     */
    theKey.keyType = VLT_KEY_DES;
    theKey.keySize = sizeof(vectorKey)/sizeof(VLT_U8);
    theKey.keyValue = vectorKey;

    /**
     * Initialise the Cipher
     */
    if( VLT_OK != ( status = CipherInit( VLT_ENCRYPT_MODE , &theKey, (VLT_PU8)&theParams ) ) )
    {
        return( status );
    }
    
    textLength = sizeof(vectorPlainText)/sizeof(VLT_U8);

    /**
     * Copy the plain text vector in the text.
     */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( text, vectorPlainText, textLength );

    /**
     * Encrypt the text
     */
    if( VLT_OK != ( status = CipherDoFinal( text, textLength, textCapacity, text, &textLength, textCapacity ) ) )
    {
        return( status );
    }

    textLength = sizeof(vectorCipherText)/sizeof(VLT_U8);

    if( 0 != host_memcmp( text, vectorCipherText, textLength ) )
    {
        return( VLT_FAIL );
    }

    /**
     * Initialise the Cipher
     */
    if( VLT_OK != ( status = CipherInit( VLT_DECRYPT_MODE , &theKey, (VLT_PU8)&theParams ) ) )
    {
        return( status );
    }

    /**
     * Decrypt the text
     */
    if( VLT_OK != ( status = CipherDoFinal( text, textLength, textCapacity, text, &textLength, textCapacity ) ) )
    {
        return( status );
    }

    textLength = sizeof(vectorPlainText)/sizeof(VLT_U8);

    if( 0 != host_memcmp( text, vectorPlainText, textLength ) )
    {
        return( VLT_FAIL );
    }

    return( VLT_OK );
}

VLT_STS DoDES_ECB_PKCS7( )
{
    VLT_STS status = VLT_FAIL ;
    CIPHER_PARAMS theParams;
    KEY_BLOB theKey;
    VLT_U32 textCapacity = 64;
    VLT_U32 textLength;
    VLT_U8 text[64];

    VLT_U8 vectorKey[] = 
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef
    };
    
    VLT_U8 vectorPlainText[] = 
    {
        0x54, 0x68, 0x65, 0x20, 0x67, 0x6f, 0x64, 0x73, 
        0x20, 0x74, 0x6f, 0x6f, 0x20, 0x61, 0x72, 0x65, 
        0x20, 0x66, 0x6f, 0x6e, 0x64, 0x20, 0x6f, 0x66, 
        0x20, 0x61, 0x20, 0x6a, 0x6f, 0x6b, 0x65, 0x00
    };

    VLT_U8 vectorCipherText[] = 
    {
        0xce, 0x79, 0xe2, 0x78, 0x97, 0xf7, 0x8b, 0x86, 
        0x0e, 0xed, 0x1e, 0x3b, 0xea, 0xdd, 0xa5, 0xa7, 
        0xcb, 0xae, 0x75, 0x48, 0x31, 0x35, 0xa9, 0xfb, 
        0xbd, 0x2f, 0x69, 0xc2, 0x53, 0xa5, 0x71, 0xe8, 
        0x08, 0x6f, 0x9a, 0x1d, 0x74, 0xc9, 0x4d, 0x4e 
    };

    /**
     * Algorithm DES
     * Padding PKCS7#
     * Block Mode ECB    
     */
    theParams.algoID = VLT_ALG_CIP_DES;
    theParams.paddingScheme = PADDING_PKCS7;
    theParams.chainMode = BLOCK_MODE_ECB;   
    
    /**
     * Key Type KEY_DES
     * Key Size 8 Bytes
     */
    theKey.keyType = VLT_KEY_DES;
    theKey.keySize = sizeof(vectorKey)/sizeof(VLT_U8);
    theKey.keyValue = vectorKey;

    /**
     * Initialise the Cipher
     */
    if( VLT_OK != ( status = CipherInit( VLT_ENCRYPT_MODE , &theKey, (VLT_PU8)&theParams ) ) )
    {
        return( status );
    }

    textLength = sizeof(vectorPlainText)/sizeof(VLT_U8);

    /**
     * Copy the plain text vector in the text.
     */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( text, vectorPlainText, textLength );

    /**
     * Encrypt the text
     */
    if( VLT_OK != ( status = CipherDoFinal( text, textLength, textCapacity, text, &textLength, textCapacity ) ) )
    {
        return( status );
    }
    
    textLength = sizeof(vectorCipherText)/sizeof(VLT_U8);

    if( 0 != host_memcmp( text, vectorCipherText, textLength ) )
    {
        return( VLT_FAIL );
    }

    /**
     * Initialise the Cipher
     */
    if( VLT_OK != ( status = CipherInit( VLT_DECRYPT_MODE , &theKey, (VLT_PU8)&theParams ) ) )
    {
        return( status );
    }

    /**
     * Decrypt the text
     */
    if( VLT_OK != ( status = CipherDoFinal( text, textLength, textCapacity, text, &textLength, textCapacity ) ) )
    {
        return( status );
    }

    textLength = sizeof(vectorPlainText)/sizeof(VLT_U8);

    if( 0 != host_memcmp( text, vectorPlainText, textLength ) )
    {
        return( VLT_FAIL );
    }

    return( VLT_OK );
}


VLT_STS DoMAC_Algo1_NoPad( )
{
    VLT_STS status = VLT_FAIL ;
    SIGNER_PARAMS theParams;
    KEY_BLOB theKey;
    VLT_U32 textCapacity = 64;
    VLT_U32 textLength;
    VLT_U8 text[64];

    VLT_U8 vectorKey[] = 
    {
        0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57
    };
    
    VLT_U8 vectorPlainText[] = 
    {
        0x6E, 0xEE, 0xEE, 0x70, 0x1C, 0x83, 0x73, 0x76, 
        0xF2, 0x15, 0x3A, 0x86, 0xF2, 0x28, 0xE9, 0xBC,
        0x16, 0x98, 0xD2, 0x85, 0x89, 0xED, 0x5C, 0x36,
        0x15, 0x5E, 0xF3, 0x13, 0x2A, 0xBC, 0x30, 0x06
    };

    VLT_U8 vectorCipherText[] = 
    {
        0x17, 0x1E, 0x8A, 0x5C, 0xCA, 0xEE, 0x97, 0x0C
    };

    VLT_U8 vectorIV[] = 
    {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
    };

    /**
     * Algorithm VLT_ALG_SIG_MAC_ISO9797_ALG1
     * Padding None
     * ivSize 8      
     */
    theParams.algoID = VLT_ALG_SIG_MAC_ISO9797_ALG1;
    theParams.paddingScheme = PADDING_NONE;
    theParams.ivSize = NELEMS(vectorIV);
    theParams.pIV = vectorIV;
    
    /**
     * Key Type KEY_DES
     * Key Size 8 Bytes
     */
    theKey.keyType = VLT_KEY_TDES_2K;
    theKey.keySize = NELEMS(vectorKey);
    theKey.keyValue = vectorKey;

    /**
     * Initialise the Signer
     */
    if( VLT_OK != ( status = SymmetricSignerInit( VLT_SIGN_MODE , &theKey, (VLT_PU8)&theParams ) ) )
    {
        return( status );
    }

    textLength = NELEMS(vectorPlainText);

    /**
     * Copy the plain text vector in the text.
     */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( text, vectorPlainText, textLength );

    /**
     * Sign the text using Mac Algo 1
     */
    if( VLT_OK != ( status = SymmetricSignerDoFinal( text, textLength, textCapacity, text, &textLength, textCapacity ) ) )
    {
        return( status );
    }
    
    /**
     * Test the returned length of the Mac
     */
    if ( textLength != NELEMS(vectorCipherText) )
    {
        return( VLT_FAIL );
    }
    

    if( 0 != host_memcmp( text, vectorCipherText, textLength ) )
    {
        return( VLT_FAIL );
    }

    /**
     * Test closing the signer
     */
    if( VLT_OK != ( status = SymmetricSignerClose() ) )
    {
        return( VLT_FAIL );
    }

    return( VLT_OK );
}

VLT_STS DoMAC_Algo1_Method2( )
{
    VLT_STS status = VLT_FAIL ;
    SIGNER_PARAMS theParams;
    KEY_BLOB theKey;
    VLT_U32 textCapacity = 64;
    VLT_U32 textLength;
    VLT_U8 text[64];

    VLT_U8 vectorKey[] = 
    {
        0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57
    };
    
    VLT_U8 vectorPlainText[] = 
    {
        0x6E, 0xEE, 0xEE, 0x70, 0x1C, 0x83, 0x73, 0x76, 
        0xF2, 0x15, 0x3A, 0x86, 0xF2, 0x28, 0xE9, 0xBC,
        0x16, 0x98, 0xD2, 0x85, 0x89, 0xED, 0x5C, 0x36,
        0x15, 0x5E, 0xF3, 0x13, 0x2A, 0xBC, 0x30, 0x06
    };

    VLT_U8 vectorCipherText[] = 
    {
        0xC7, 0x9F, 0x57, 0x5E, 0x3D, 0xDE, 0x44, 0x70
    };

    VLT_U8 vectorIV[] = 
    {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
    };

    /**
     * Algorithm VLT_ALG_SIG_MAC_ISO9797_ALG1
     * Padding Method 2
     * ivSize 8      
     */
    theParams.algoID = VLT_ALG_SIG_MAC_ISO9797_ALG1;
    theParams.paddingScheme = PADDING_ISO9797_METHOD2;
    theParams.ivSize = NELEMS(vectorIV);
    theParams.pIV = vectorIV;
    
    /**
     * Key Type KEY_DES
     * Key Size 8 Bytes
     */
    theKey.keyType = VLT_KEY_TDES_2K;
    theKey.keySize = NELEMS(vectorKey);
    theKey.keyValue = vectorKey;

    /**
     * Initialise the Signer
     */
    if( VLT_OK != ( status = SymmetricSignerInit( VLT_SIGN_MODE , &theKey, (VLT_PU8)&theParams ) ) )
    {
        return( status );
    }

    textLength = NELEMS(vectorPlainText);

    /**
     * Copy the plain text vector in the text.
     */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( text, vectorPlainText, textLength );

    /**
     * Sign the text using Mac Algo 1
     */
    if( VLT_OK != ( status = SymmetricSignerDoFinal( text, textLength, textCapacity, text, &textLength, textCapacity ) ) )
    {
        return( status );
    }
    
    /**
     * Test the returned length of the Mac
     */
    if ( textLength != NELEMS(vectorCipherText) )
    {
        return( VLT_FAIL );
    }
    

    if( 0 != host_memcmp( text, vectorCipherText, textLength ) )
    {
        return( VLT_FAIL );
    }
    
    /**
     * Test closing the signer
     */
    if( VLT_OK != ( status = SymmetricSignerClose() ) )
    {
        return( VLT_FAIL );
    }

    return( VLT_OK );
}

VLT_STS DoMAC_Algo3_NoPad( )
{
    VLT_STS status = VLT_FAIL ;
    SIGNER_PARAMS theParams;
    KEY_BLOB theKey;
    VLT_U32 textCapacity = 64;
    VLT_U32 textLength;
    VLT_U8 text[64];

    VLT_U8 vectorKey[] = 
    {
        0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57
    };
    
    VLT_U8 vectorPlainText[] = 
    {
        0x6E, 0xEE, 0xEE, 0x70, 0x1C, 0x83, 0x73, 0x76, 
        0xF2, 0x15, 0x3A, 0x86, 0xF2, 0x28, 0xE9, 0xBC,
        0x16, 0x98, 0xD2, 0x85, 0x89, 0xED, 0x5C, 0x36,
        0x15, 0x5E, 0xF3, 0x13, 0x2A, 0xBC, 0x30, 0x06
    };

    VLT_U8 vectorCipherText[] = 
    {
        0xBC, 0x9E, 0xF7, 0xAE, 0x11, 0x80, 0x52, 0x54
    };

    VLT_U8 vectorIV[] = 
    {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
    };

    /**
     * Algorithm VLT_ALG_SIG_MAC_ISO9797_ALG3
     * Padding None
     * ivSize 8      
     */
    theParams.algoID = VLT_ALG_SIG_MAC_ISO9797_ALG3;
    theParams.paddingScheme = PADDING_NONE;
    theParams.ivSize = NELEMS(vectorIV);
    theParams.pIV = vectorIV;
    
    /**
     * Key Type KEY_DES
     * Key Size 8 Bytes
     */
    theKey.keyType = VLT_KEY_TDES_2K;
    theKey.keySize = NELEMS(vectorKey);
    theKey.keyValue = vectorKey;

    /**
     * Initialise the Signer
     */
    if( VLT_OK != ( status = SymmetricSignerInit( VLT_SIGN_MODE , &theKey, (VLT_PU8)&theParams ) ) )
    {
        return( status );
    }

    textLength = NELEMS(vectorPlainText);

    /**
     * Copy the plain text vector in the text.
     */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( text, vectorPlainText, textLength );

    /**
     * Sign the text using Mac Algo 3
     */
    if( VLT_OK != ( status = SymmetricSignerDoFinal( text, textLength, textCapacity, text, &textLength, textCapacity ) ) )
    {
        return( status );
    }
    
    /**
     * Test the returned length of the Mac
     */
    if ( textLength != NELEMS(vectorCipherText) )
    {
        return( VLT_FAIL );
    }
    

    if( 0 != host_memcmp( text, vectorCipherText, textLength ) )
    {
        return( VLT_FAIL );
    }

    /**
     * Test closing the signer
     */
    if( VLT_OK != ( status = SymmetricSignerClose() ) )
    {
        return( VLT_FAIL );
    }

    return( VLT_OK );
}

VLT_STS DoMoreMAC_Algo3_NoPad( )
{
    VLT_STS status = VLT_FAIL ;
    SIGNER_PARAMS theParams;
    KEY_BLOB theKey;
    VLT_U32 textCapacity = 64;
    VLT_U32 textLength;
    VLT_U8 text[64];

    VLT_U8 vectorKey[] = 
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
        0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10
    };
    
    VLT_U8 vectorPlainText[] = 
    {
        'N','o','w',' ', 'i','s',' ','t',
        'h','e',' ','t','i','m','e', ' ',
        'f','o','r',' ','a','l','l', ' '
    };

    VLT_U8 vectorCipherText[] = 
    {
        //0x4a, 0xf4, 0x07, 0x8e, 0x8e, 0x67, 0xe7, 0x63
        0xe9, 0x08, 0x62, 0x30, 0xca, 0x3b, 0xe7, 0x96
    };

    VLT_U8 vectorIV[] = 
    {
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
    };

    /**
     * Algorithm VLT_ALG_SIG_MAC_ISO9797_ALG3
     * Padding None
     * ivSize 8      
     */
    theParams.algoID = VLT_ALG_SIG_MAC_ISO9797_ALG3;
    theParams.paddingScheme = PADDING_ISO9797_METHOD2;
    theParams.ivSize = NELEMS(vectorIV);
    theParams.pIV = vectorIV;
    
    /**
     * Key Type KEY_DES
     * Key Size 8 Bytes
     */
    theKey.keyType = VLT_KEY_TDES_2K;
    theKey.keySize = NELEMS(vectorKey);
    theKey.keyValue = vectorKey;

    /**
     * Initialise the Signer
     */
    if( VLT_OK != ( status = SymmetricSignerInit( VLT_SIGN_MODE , &theKey, (VLT_PU8)&theParams ) ) )
    {
        return( status );
    }

    textLength = NELEMS(vectorPlainText);

    /**
     * Copy the plain text vector in the text.
     */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( text, vectorPlainText, textLength );

    /**
     * Sign the text using Mac Algo 3
     */
    if( VLT_OK != ( status = SymmetricSignerDoFinal( text, textLength, textCapacity, text, &textLength, textCapacity ) ) )
    {
        return( status );
    }
    
    /**
     * Test the returned length of the Mac
     */
    if ( textLength != NELEMS(vectorCipherText) )
    {
        return( VLT_FAIL );
    }
    

    if( 0 != host_memcmp( text, vectorCipherText, textLength ) )
    {
        return( VLT_FAIL );
    }

    /**
     * Test closing the signer
     */
    if( VLT_OK != ( status = SymmetricSignerClose() ) )
    {
        return( VLT_FAIL );
    }

    return( VLT_OK );
}

VLT_STS DoMAC_Algo3_Method2( )
{
    VLT_STS status = VLT_FAIL ;
    SIGNER_PARAMS theParams;
    KEY_BLOB theKey;
    VLT_U32 textCapacity = 64;
    VLT_U32 textLength;
    VLT_U8 text[64];

    VLT_U8 vectorKey[] = 
    {
        0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57
    };
    
    VLT_U8 vectorPlainText[] = 
    {
        0x6E, 0xEE, 0xEE, 0x70, 0x1C, 0x83, 0x73, 0x76, 
        0xF2, 0x15, 0x3A, 0x86, 0xF2, 0x28, 0xE9, 0xBC,
        0x16, 0x98, 0xD2, 0x85, 0x89, 0xED, 0x5C, 0x36,
        0x15, 0x5E, 0xF3, 0x13, 0x2A, 0xBC, 0x30, 0x06
    };

    VLT_U8 vectorCipherText[] = 
    {
        0x68, 0x98, 0x76, 0x1A, 0x44, 0x27, 0x62, 0x26
    };

    VLT_U8 vectorIV[] = 
    {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
    };

    /**
     * Algorithm VLT_ALG_SIG_MAC_ISO9797_ALG3
     * Padding Method 2
     * ivSize 8      
     */
    theParams.algoID = VLT_ALG_SIG_MAC_ISO9797_ALG3;
    theParams.paddingScheme = PADDING_ISO9797_METHOD2;
    theParams.ivSize = NELEMS(vectorIV);
    theParams.pIV = vectorIV;
    
    /**
     * Key Type KEY_DES
     * Key Size 8 Bytes
     */
    theKey.keyType = VLT_KEY_TDES_2K;
    theKey.keySize = NELEMS(vectorKey);
    theKey.keyValue = vectorKey;

    /**
     * Initialise the Signer
     */
    if( VLT_OK != ( status = SymmetricSignerInit( VLT_SIGN_MODE , &theKey, (VLT_PU8)&theParams ) ) )
    {
        return( status );
    }

    textLength = NELEMS(vectorPlainText);

    /**
     * Copy the plain text vector in the text.
     */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( text, vectorPlainText, textLength );

    /**
     * Sign the text using Mac Algo 3
     */
    if( VLT_OK != ( status = SymmetricSignerDoFinal( text, textLength, textCapacity, text, &textLength, textCapacity ) ) )
    {
        return( status );
    }
    
    /**
     * Test the returned length of the Mac
     */
    if ( textLength != NELEMS(vectorCipherText) )
    {
        return( VLT_FAIL );
    }
    

    if( 0 != host_memcmp( text, vectorCipherText, textLength ) )
    {
        return( VLT_FAIL );
    }
    
    /**
     * Test closing the signer
     */
    if( VLT_OK != ( status = SymmetricSignerClose() ) )
    {
        return( VLT_FAIL );
    }

    return( VLT_OK );
}



VLT_STS DoMAC_Algo3_Method2UpdateDoFinal( )
{
    VLT_STS status = VLT_FAIL ;
    SIGNER_PARAMS theParams;
    KEY_BLOB theKey;
    VLT_U32 textCapacity = 64;
    VLT_U32 textLength;
    VLT_U8 text[64];
    VLT_U32 updateLength = 0;

    VLT_U8 vectorKey[] = 
    {
        0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57
    };
    
    VLT_U8 vectorPlainText[] = 
    {
        0x6E, 0xEE, 0xEE, 0x70, 0x1C, 0x83, 0x73, 0x76, 
        0xF2, 0x15, 0x3A, 0x86, 0xF2, 0x28, 0xE9, 0xBC,
        0x16, 0x98, 0xD2, 0x85, 0x89, 0xED, 0x5C, 0x36,
        0x15, 0x5E, 0xF3, 0x13, 0x2A, 0xBC, 0x30, 0x06
    };

    VLT_U8 vectorCipherText[] = 
    {
        0x68, 0x98, 0x76, 0x1A, 0x44, 0x27, 0x62, 0x26
    };

    VLT_U8 vectorIV[] = 
    {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
    };

    /**
     * Algorithm VLT_ALG_SIG_MAC_ISO9797_ALG3
     * Padding Method 2
     * ivSize 8      
     */
    theParams.algoID = VLT_ALG_SIG_MAC_ISO9797_ALG3;
    theParams.paddingScheme = PADDING_ISO9797_METHOD2;
    theParams.ivSize = NELEMS(vectorIV);
    theParams.pIV = vectorIV;
    
    /**
     * Key Type KEY_DES
     * Key Size 8 Bytes
     */
    theKey.keyType = VLT_KEY_TDES_2K;
    theKey.keySize = NELEMS(vectorKey);
    theKey.keyValue = vectorKey;

    /**
     * Initialise the Signer
     */
    if( VLT_OK != ( status = SymmetricSignerInit( VLT_SIGN_MODE , &theKey, (VLT_PU8)&theParams ) ) )
    {
        return( status );
    }

    textLength = NELEMS(vectorPlainText);

    /**
     * Copy the plain text vector in the text.
     */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( text, vectorPlainText, textLength );


    /**
     * Sign the text using Mac Algo, sign the first half of the message
     */
    updateLength = textLength/2; 
    if( VLT_OK != ( status = SymmetricSignerUpdate( text, updateLength, textCapacity ) ) )
    {
        return( status );
    }


    /**
     * Sign the remaining text using Mac Algo 3 do final.
     */
    if( VLT_OK != ( status = SymmetricSignerDoFinal( &text[updateLength], updateLength, textCapacity, text, &updateLength, textCapacity ) ) )
    {
        return( status );
    }
    
    /**
     * Test the returned length of the Mac
     */
    if ( updateLength != NELEMS(vectorCipherText) )
    {
        return( VLT_FAIL );
    }
    

    if( 0 != host_memcmp( text, vectorCipherText, updateLength ) )
    {
        return( VLT_FAIL );
    }
    
    /**
     * Test closing the signer
     */
    if( VLT_OK != ( status = SymmetricSignerClose() ) )
    {
        return( VLT_FAIL );
    }

    return( VLT_OK );
}


VLT_STS DoInitSignerNullArgs( )
{
    VLT_STS status = VLT_FAIL ;
    SIGNER_PARAMS theParams;
    SIGNER_PARAMS* pTheParams = 0;
    KEY_BLOB* pTheKey = 0;
    KEY_BLOB theKey;
    VLT_U32 textCapacity = 64;
    VLT_U32 textLength;
    VLT_U8 text[64];

    VLT_U8 vectorKey[] = 
    {
        0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57
    };
    
    VLT_U8 vectorPlainText[] = 
    {
        0x6E, 0xEE, 0xEE, 0x70, 0x1C, 0x83, 0x73, 0x76, 
        0xF2, 0x15, 0x3A, 0x86, 0xF2, 0x28, 0xE9, 0xBC,
        0x16, 0x98, 0xD2, 0x85, 0x89, 0xED, 0x5C, 0x36,
        0x15, 0x5E, 0xF3, 0x13, 0x2A, 0xBC, 0x30, 0x06
    };

    VLT_U8 vectorCipherText[] = 
    {
        0x68, 0x98, 0x76, 0x1A, 0x44, 0x27, 0x62, 0x26
    };

    VLT_U8 vectorIV[] = 
    {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
    };

    /**
     * Algorithm VLT_ALG_SIG_MAC_ISO9797_ALG1
     * Padding Method 2
     * ivSize 8      
     */
    theParams.algoID = VLT_ALG_SIG_MAC_ISO9797_ALG3;
    theParams.paddingScheme = PADDING_ISO9797_METHOD2;
    theParams.ivSize = NELEMS(vectorIV);
    theParams.pIV = vectorIV;
    
    /**
     * Key Type KEY_DES
     * Key Size 8 Bytes
     */
    theKey.keyType = VLT_KEY_TDES_2K;
    theKey.keySize = NELEMS(vectorKey);
    theKey.keyValue = vectorKey;

    /**
     * Initialise the Signer with a null Key ptr
     */
    if( VLT_OK == ( status = SymmetricSignerInit( VLT_SIGN_MODE , pTheKey, (VLT_PU8)&theParams ) ) )
    {
        return( status );
    }
    
    /**
     * Initialise the Signer with a null Params ptr
     */
    if( VLT_OK == ( status = SymmetricSignerInit( VLT_SIGN_MODE , &theKey, (VLT_PU8)pTheParams ) ) )
    {
        return( status );
    }

    /**
     * Initialise the Cipher with a invalid mode
     */
    if( VLT_OK == ( status = SymmetricSignerInit( 55 , &theKey, (VLT_PU8)&theParams ) ) )
    {
        return( status );
    }

    /**
     * Initialise the Signer with a valid mode and check it still produces the 
     * correct answer.
     */
    if( VLT_OK != ( status = SymmetricSignerInit( VLT_SIGN_MODE , &theKey, (VLT_PU8)&theParams ) ) )
    {
        return( status );
    }

    textLength = NELEMS(vectorPlainText);

    /**
     * Copy the plain text vector in the text.
     */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( text, vectorPlainText, textLength );

    /**
     * Sign the text using Mac Algo 1
     */
    if( VLT_OK != ( status = SymmetricSignerDoFinal( text, textLength, textCapacity, text, &textLength, textCapacity ) ) )
    {
        return( status );
    }
    
    /**
     * Test the returned length of the Mac
     */
    if ( textLength != NELEMS(vectorCipherText) )
    {
        return( VLT_FAIL );
    }
    

    if( 0 != host_memcmp( text, vectorCipherText, textLength ) )
    {
        return( VLT_FAIL );
    }
    
    /**
     * Test closing the signer
     */
    if( VLT_OK != ( status = SymmetricSignerClose() ) )
    {
        return( VLT_FAIL );
    }

    return( VLT_OK );
}


VLT_STS DoFinalSignerNullArgs( )
{
    VLT_STS status = VLT_FAIL ;
    SIGNER_PARAMS theParams;
    KEY_BLOB theKey;
    VLT_U32 textCapacity = 64;
    VLT_U32 textLength;
    VLT_U8 text[64];

    VLT_U8 vectorKey[] = 
    {
        0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57
    };
    
    VLT_U8 vectorPlainText[] = 
    {
        0x6E, 0xEE, 0xEE, 0x70, 0x1C, 0x83, 0x73, 0x76, 
        0xF2, 0x15, 0x3A, 0x86, 0xF2, 0x28, 0xE9, 0xBC,
        0x16, 0x98, 0xD2, 0x85, 0x89, 0xED, 0x5C, 0x36,
        0x15, 0x5E, 0xF3, 0x13, 0x2A, 0xBC, 0x30, 0x06
    };

    VLT_U8 vectorCipherText[] = 
    {
        0x68, 0x98, 0x76, 0x1A, 0x44, 0x27, 0x62, 0x26
    };

    VLT_U8 vectorIV[] = 
    {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
    };

    /**
     * Algorithm VLT_ALG_SIG_MAC_ISO9797_ALG1
     * Padding Method 2
     * ivSize 8      
     */
    theParams.algoID = VLT_ALG_SIG_MAC_ISO9797_ALG3;
    theParams.paddingScheme = PADDING_ISO9797_METHOD2;
    theParams.ivSize = NELEMS(vectorIV);
    theParams.pIV = vectorIV;
    
    /**
     * Key Type KEY_DES
     * Key Size 8 Bytes
     */
    theKey.keyType = VLT_KEY_TDES_2K;
    theKey.keySize = NELEMS(vectorKey);
    theKey.keyValue = vectorKey;

    /**
     * Initialise the Signer
     */
    if( VLT_OK != ( status = SymmetricSignerInit( VLT_SIGN_MODE , &theKey, (VLT_PU8)&theParams ) ) )
    {
        return( status );
    }

    textLength = NELEMS(vectorPlainText);

    /**
     * Copy the plain text vector in the text.
     */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( text, vectorPlainText, textLength );

    /**
     * Sign the text using Mac Algo 1 NUL msg ptr
     */
    if( VLT_OK == ( status = SymmetricSignerDoFinal( 0, textLength, textCapacity, text, &textLength, textCapacity ) ) )
    {
        return( status );
    }

    /**
     * Sign the text using Mac Algo 1 Null Text length
     */
    if( VLT_OK == ( status = SymmetricSignerDoFinal( text, 0, textCapacity, text, &textLength, textCapacity ) ) )
    {
        return( status );
    }

    /**
     * Sign the text using Mac Algo 1 message capacity length set to 0
     */
    if( VLT_OK == ( status = SymmetricSignerDoFinal( text, textLength, 0, text, &textLength, textCapacity ) ) )
    {
        return( status );
    }

    /**
     * Sign the text using Mac Algo 1 Null output buffer ptr
     */
    if( VLT_OK == ( status = SymmetricSignerDoFinal( text, textLength, textCapacity, 0, &textLength, textCapacity ) ) )
    {
        return( status );
    }

    /**
     * Sign the text using Mac Algo 1 Null output length ptr
     */
    if( VLT_OK == ( status = SymmetricSignerDoFinal( text, textLength, textCapacity, text, 0, textCapacity ) ) )
    {
        return( status );
    }

    /**
     * Sign the text using Mac Algo 1 output capacity length set to 0
     */
    if( VLT_OK == ( status = SymmetricSignerDoFinal( text, textLength, textCapacity, text, &textLength, 0 ) ) )
    {
        return( status );
    }
    
    /**
     * Sign the text using Mac Algo 1
     */
    if( VLT_OK != ( status = SymmetricSignerDoFinal( text, textLength, textCapacity, text, &textLength, textCapacity ) ) )
    {
        return( status );
    }
    
    /**
     * Test the returned length of the Mac
     */
    if ( textLength != NELEMS(vectorCipherText) )
    {
        return( VLT_FAIL );
    }
    

    if( 0 != host_memcmp( text, vectorCipherText, textLength ) )
    {
        return( VLT_FAIL );
    }
    
    /**
     * Test closing the signer
     */
    if( VLT_OK != ( status = SymmetricSignerClose() ) )
    {
        return( VLT_FAIL );
    }

    return( VLT_OK );
}

VLT_STS DoAES192_CBC_NoPadding()
{
    VLT_STS status = VLT_FAIL ;
    CIPHER_PARAMS theParams;
    KEY_BLOB theKey;
    VLT_U32 textCapacity = 64;
    VLT_U32 textLength;
    VLT_U8 text[64];
    
    VLT_U8 vectorIV[] = 
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    VLT_U8 vectorKey[] = 
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
        0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
        0x11, 0x13, 0x15, 0x67, 0x89, 0xAB, 0xCD, 0x0F
    };

    VLT_U8 vectorPlainText[] = 
    {
        0x54, 0x68, 0x65, 0x20, 0x67, 0x6f, 0x64, 0x73, 
        0x20, 0x74, 0x6f, 0x6f, 0x20, 0x61, 0x72, 0x65, 
        0x20, 0x66, 0x6f, 0x6e, 0x64, 0x20, 0x6f, 0x66, 
        0x20, 0x61, 0x20, 0x6a, 0x6f, 0x6b, 0x65, 0x00 

    };

    VLT_U8 vectorCipherText[] = 
    {
        0xbc, 0x2b, 0x37, 0x6a, 0x67, 0x2c, 0x28, 0xc5, 
        0x6a, 0xb4, 0x06, 0xc6, 0xda, 0x28, 0xca, 0xc1,
        0xd6, 0x57, 0xbe, 0x9f, 0xe7, 0x19, 0xcb, 0x7d, 
        0xcc, 0x8e, 0x21, 0x9f, 0xe0, 0x97, 0xf8, 0x4f
    };

    /**
     * Algorithm AES 192
     * Padding None
     * Block Mode CBC 
     */
    theParams.algoID = VLT_ALG_CIP_AES;
    theParams.paddingScheme = PADDING_NONE;
    theParams.chainMode = BLOCK_MODE_CBC;
    theParams.pIV = vectorIV;
    
    /**
     * Key Type AES 192
     * Key Size 24 Bytes
     */
    theKey.keyType = VLT_KEY_AES_192;
    theKey.keySize = sizeof(vectorKey)/sizeof(VLT_U8);
    theKey.keyValue = vectorKey;

    textLength = ( sizeof(vectorPlainText)/sizeof(VLT_U8) );

    /**
     * Initialise the Cipher
     */
    if( VLT_OK != ( status = CipherInit( VLT_ENCRYPT_MODE , &theKey, (VLT_PU8)&theParams ) ) )
    {
        return( status );
    }
    
    /**
     * Copy the plain text vector in the text.
     */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( text, vectorPlainText, textLength );

    /**
     * Encrypt the text
     */
    if( VLT_OK != ( status = CipherDoFinal( text, textLength, textCapacity, text, &textLength, textCapacity ) ) )
    {
        return( status );
    }

    /**
     * Initialise the Cipher
     */
    if( VLT_OK != ( status = CipherInit( VLT_DECRYPT_MODE , &theKey, (VLT_PU8)&theParams ) ) )
    {
        return( status );
    }

    textLength = ( sizeof(vectorCipherText)/sizeof(VLT_U8) );

    if( 0 != host_memcmp( text, vectorCipherText, textLength ) )
    {
        return( VLT_FAIL );
    }

    /**
     * Decrypt the text
     */
    if( VLT_OK != ( status = CipherDoFinal( text, textLength, textCapacity, text, &textLength, textCapacity ) ) )
    {
        return( status );
    }

    textLength = ( sizeof(vectorPlainText)/sizeof(VLT_U8) );

    if( 0 != host_memcmp( text, vectorPlainText, textLength ) )
    {
        return( VLT_FAIL );
    }

    return( VLT_OK );
}

VLT_STS DoAES256_CBC_PKCS5Padding()
{
    VLT_STS status = VLT_FAIL ;
    CIPHER_PARAMS theParams;
    KEY_BLOB theKey;
    VLT_U32 textCapacity = 64;
    VLT_U32 textLength;
    VLT_U8 text[64];
    
    VLT_U8 vectorIV[] = 
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    VLT_U8 vectorKey[] = 
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
        0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
        0x11, 0x13, 0x15, 0x67, 0x89, 0xAB, 0xCD, 0x0F,
        0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF
    };

    VLT_U8 vectorPlainText[] = 
    {
        0x54, 0x68, 0x65, 0x20, 0x67, 0x6f, 0x64, 0x73, 
        0x20, 0x74, 0x6f, 0x6f, 0x20, 0x61, 0x72, 0x65, 
        0x20, 0x66, 0x6f, 0x6e, 0x64, 0x20, 0x6f, 0x66, 
        0x20, 0x61, 0x20, 0x6a, 0x6f, 0x6b, 0x65, 0x00 
    };

    VLT_U8 vectorCipherText[] = 
    {
        0xc8, 0x08, 0xab, 0xa8, 0xdd, 0x75, 0x9e, 0x8e, 
        0x6d, 0x2f, 0x1b, 0x67, 0xaa, 0x55, 0xe5, 0x01,
        0x8b, 0x02, 0x29, 0xf0, 0xe1, 0x03, 0x8a, 0x85, 
        0x2a, 0xf9, 0x50, 0xc7, 0x5f, 0x35, 0x9e, 0x5d,
        0x31, 0xad, 0x5d, 0xdb, 0x4a, 0x88, 0x34, 0x33, 
        0x9c, 0x04, 0x2e, 0xc8, 0xf2, 0xc7, 0x14, 0xd1
    };

    /**
     * Algorithm AES 256
     * Padding None
     * Block Mode CBC 
     */
    theParams.algoID = VLT_ALG_CIP_AES;
    theParams.paddingScheme = PADDING_PKCS5;
    theParams.chainMode = BLOCK_MODE_CBC;
    theParams.pIV = vectorIV;
    
    /**
     * Key Type AES 256
     * Key Size 32 Bytes
     */
    theKey.keyType = VLT_KEY_AES_256;
    theKey.keySize = sizeof(vectorKey)/sizeof(VLT_U8);
    theKey.keyValue = vectorKey;

    textLength = ( sizeof(vectorPlainText)/sizeof(VLT_U8) );

    /**
     * Initialise the Cipher
     */
    if( VLT_OK != ( status = CipherInit( VLT_ENCRYPT_MODE , &theKey, (VLT_PU8)&theParams ) ) )
    {
        return( status );
    }
    
    /**
     * Copy the plain text vector in the text.
     */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( text, vectorPlainText, textLength );

    /**
     * Encrypt the text
     */
    if( VLT_OK != ( status = CipherDoFinal( text, textLength, textCapacity, text, &textLength, textCapacity ) ) )
    {
        return( status );
    }

    /**
     * Initialise the Cipher
     */
    if( VLT_OK != ( status = CipherInit( VLT_DECRYPT_MODE , &theKey, (VLT_PU8)&theParams ) ) )
    {
        return( status );
    }

    textLength = ( sizeof(vectorCipherText)/sizeof(VLT_U8) );

    if( 0 != host_memcmp( text, vectorCipherText, textLength ) )
    {
        return( VLT_FAIL );
    }

    /**
     * Decrypt the text
     */
    if( VLT_OK != ( status = CipherDoFinal( text, textLength, textCapacity, text, &textLength, textCapacity ) ) )
    {
        return( status );
    }

    textLength = ( sizeof(vectorPlainText)/sizeof(VLT_U8) );

    if( 0 != host_memcmp( text, vectorPlainText, textLength ) )
    {
        return( VLT_FAIL );
    }

    return( VLT_OK );
}

#endif/*VLT_ENABLE_CIPHER_TESTS*/

VLT_STS DoCipherTests( void )
{
#if( VLT_ENABLE_CIPHER_TESTS == VLT_ENABLE )

    VLT_STS status = VLT_FAIL;

    /**
     * These little cluster of tests are here
     * for debug purposes.
     */
    if( VLT_OK != ( status = DesTest() ) )
    {
        return( status );
    }

    if( VLT_OK != ( status = TDesTest() ) )
    {
        return( status );
    }
    
    if( VLT_OK != ( status = AesTest() ) )
    {
        return( status );
    }
    
    if( VLT_OK != ( status = PaddingTests() ) )
    {
        return( status );
    }

    if( VLT_OK != ( status = DoTDES_EEE_CBC_PKCS7() ) )
    {
        return( status );
    }   

    if( VLT_OK != ( status = DoTDES3K_EDE_CBC_PKCS7() ) )
    {
        return( status );
    }

    if( VLT_OK != ( status = DoTDES2K_EDE_CBC_PKCS7() ) )
    {
        return( status );
    }

    if( VLT_OK != ( status = DoTDES3K_EDE_ECB_PKCS7() ) )
    {
        return( status );
    }

    if( VLT_OK != ( status = DoTDES2K_EDE_ECB_PKCS7() ) )
    {
        return( status );
    }

    if( VLT_OK != ( status = DoDES_CBC_PKCS7() ) )
    {
        return( status );
    }
    
    if( VLT_OK != ( status = DoDES_ECB_PKCS7() ) )
    {
        return( status );
    }

    if ( VLT_OK != ( status = DoMAC_Algo1_NoPad() ) )
    {
        return( status );
    }

    if ( VLT_OK != ( status = DoMAC_Algo1_Method2() ) )
    {
        return( status );
    }

    if ( VLT_OK != ( status = DoMAC_Algo3_NoPad() ) )
    {
        return( status );
    }

    if ( VLT_OK != ( status = DoMoreMAC_Algo3_NoPad() ) )
    {
        return( status );
    }

    if ( VLT_OK != ( status = DoMAC_Algo3_Method2() ) )
    {
        return( status );
    }

    if ( VLT_OK != ( status = DoInitSignerNullArgs() ) )
    {
        return ( status );
    }

    if ( VLT_OK != ( status = DoFinalSignerNullArgs() ) )
    {
        return ( status );
    }

    if ( VLT_OK != ( status = DoMAC_Algo3_Method2UpdateDoFinal() ) )
    {
        return ( status );
    }

    if( VLT_OK != ( status = DoAES192_CBC_NoPadding() ) )
    {
        return( status );
    }

    if( VLT_OK != ( status = DoAES256_CBC_PKCS5Padding() ) )
    {
        return( status );
    }

    if( VLT_OK != ( status = AesCMacTest() ) )
    {
        return( status );
    }

#endif/*( VLT_ENABLE_CIPHER_TESTS == VLT_ENABLE )*/
    return( VLT_OK );
}
