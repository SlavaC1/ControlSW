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
#include "vaultic_signer_aes_cmac.h"

/**
 * Error Codes
 */
#define EAESCMACIIVLDPARAM           VLT_ERROR( VLT_AES_CMAC, 0 )
#define EAESCMACIIVLDKEY             VLT_ERROR( VLT_AES_CMAC, 1 )
#define EAESCMACDFIVLDMSG            VLT_ERROR( VLT_AES_CMAC, 2 )   
#define EAESCMACDFIVLDMAC            VLT_ERROR( VLT_AES_CMAC, 3 )
#define EAESCMACDFIVLDMACLEN         VLT_ERROR( VLT_AES_CMAC, 4 )
#define EAESCMACDFIVLDMACCAP         VLT_ERROR( VLT_AES_CMAC, 5 )
#define EAESCMACDFIVLDMSGCAP         VLT_ERROR( VLT_AES_CMAC, 6 )
#define EAESCMACUPIVLDMSG            VLT_ERROR( VLT_AES_CMAC, 7 )
#define EAESCMACUPIVLDMSGLEN         VLT_ERROR( VLT_AES_CMAC, 8 )
#define EAESCMACUPIVLDMSGCAP         VLT_ERROR( VLT_AES_CMAC, 9 )
#define EINVALIDOPMODE               VLT_ERROR( VLT_AES_CMAC, 10 )

#if( VLT_ENABLE_CIPHER_AES == VLT_ENABLE )

/**
 * Private Methods
 */
VLT_STS generateSubKeys( const KEY_BLOB* pKey, VLT_PU8 K1, VLT_PU8 K2 );

/**
 * The Sub Keys K1 and K2.
 */
static VLT_U8 K1[AES_BLOCK_SIZE];
static VLT_U8 K2[AES_BLOCK_SIZE];
static VLT_U8 mac[AES_BLOCK_SIZE];

/**
 * Private Data
 */
static CIPHER_PARAMS cipherParams =
{
    VLT_ALG_CIP_AES,
    BLOCK_MODE_ECB,
    PADDING_NONE,
    0 /* N/A for ECB */
};

static KEY_BLOB theKey = { 0 };


VLT_STS SignerAesCmacInit( VLT_U8 opMode, const KEY_BLOB* pKey, VLT_PU8 pParams )
{   

    VLT_STS status = VLT_FAIL;

    /**
     * NOTE: Extensive validation of all the parameters should happen at
     * the interface level since they are common to all the Signers.    
     */
    if( NULL == pKey )
    {
        return( EAESCMACIIVLDKEY );
    }

    /**
     * Before we initialise the signer ensure that we are purging
     * any residual data from previous calls.
     * We are not checking the return value by design.
     */
    SignerAesCmacClose();

    /**
     * Cache the key Blob
     */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( (VLT_PU8)&theKey, (VLT_PU8)pKey, sizeof(KEY_BLOB) );

    /**
     * Carry out the subkey generation.
     */
    if( VLT_OK != ( status = generateSubKeys( &theKey, K1, K2 ) ) )
    {
        return( status );
    }

    /*
     * Check the op mode
     */
    if ( VLT_SIGN_MODE != opMode )
    {
        return ( EINVALIDOPMODE );
    }

    /**
     * Now Initialise the underlying cipher for the mac generation.
     */
    cipherParams.algoID = VLT_ALG_CIP_AES;
    cipherParams.chainMode = BLOCK_MODE_ECB;
    cipherParams.paddingScheme = PADDING_NONE;
    cipherParams.pIV = 0; /*N/A for ECB*/
    if( VLT_OK != ( status = CipherInit(
        VLT_ENCRYPT_MODE,
        &theKey,
        (VLT_PU8)&cipherParams ) ) )
    {
        return( status );
    }

    return( VLT_OK );
}

VLT_STS SignerAesCmacClose( void )
{
    /**
     * Clear every piece of residual data used by the AES CMAC
     * signer.
     */ 
    (void)host_memset( (VLT_PU8)&cipherParams, 0x00, sizeof(CIPHER_PARAMS) );
    (void)host_memset( (VLT_PU8)&theKey, 0x00, sizeof(KEY_BLOB) );
    (void)host_memset( (VLT_PU8)K1, 0x00, AES_BLOCK_SIZE );
    (void)host_memset( (VLT_PU8)K2, 0x00, AES_BLOCK_SIZE );   
    (void)host_memset( (VLT_PU8)mac, 0x00, AES_BLOCK_SIZE );
    
    return( CipherClose() );
}

/**
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   +                   Algorithm AES-CMAC                              +
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   +                                                                   +
   +   Input    : K    ( 128-bit key )                                 +
   +            : M    ( message to be authenticated )                 +
   +            : len  ( length of the message in octets )             +
   +   Output   : T    ( message authentication code )                 +
   +                                                                   +
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   +   Constants: const_Zero is 0x00000000000000000000000000000000     +
   +              const_Bsize is 16                                    +
   +                                                                   +
   +   Variables: K1, K2 for 128-bit subkeys                           +
   +              M_i is the i-th block (i=1..ceil(len/const_Bsize))   +
   +              M_last is the last block xor-ed with K1 or K2        +
   +              n      for number of blocks to be processed          +
   +              r      for number of octets of last block            +
   +              flag   for denoting if last block is complete or not +
   +                                                                   +
   +   Step 1.  (K1,K2) := Generate_Subkey(K);                         +
   +   Step 2.  n := ceil(len/const_Bsize);                            +
   +   Step 3.  if n = 0                                               +
   +            then                                                   +
   +                 n := 1;                                           +
   +                 flag := false;                                    +
   +            else                                                   +
   +                 if len mod const_Bsize is 0                       +
   +                 then flag := true;                                +
   +                 else flag := false;                               +
   +                                                                   +
   +   Step 4.  if flag is true                                        +
   +            then M_last := M_n XOR K1;                             +
   +            else M_last := padding(M_n) XOR K2;                    +
   +   Step 5.  X := const_Zero;                                       +
   +   Step 6.  for i := 1 to n-1 do                                   +
   +                begin                                              +
   +                  Y := X XOR M_i;                                  +
   +                  X := AES-128(K,Y);                               +
   +                end                                                +
   +            Y := M_last XOR X;                                     +
   +            T := AES-128(K,Y);                                     +
   +   Step 7.  return T;                                              +
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

VLT_STS SignerAesCmacDoFinal(
    VLT_PU8 pMessage,
    VLT_U32 messageLen,
    VLT_U32 messageCapacity,
    VLT_PU8 pMac,
    VLT_PU32 pMacLen,
    VLT_U32 macCapacity )
{
    VLT_STS status = VLT_FAIL;
    VLT_BOOL bNoPaddingRequired = TRUE;
    VLT_U32 blockCount = 0;
    VLT_U32 padSize = 0;
    VLT_U8 M_last[AES_BLOCK_SIZE];

    /**
     * Rudimentary input parameter validation
     */
    if( NULL == pMessage )
    {
        return( EAESCMACDFIVLDMSG );
    }

    if( NULL == pMac )
    {
        return( EAESCMACDFIVLDMAC );
    }

    if( NULL == pMacLen )
    {
        return( EAESCMACDFIVLDMACLEN );
    }

    if( macCapacity < AES_BLOCK_SIZE )
    {
        return( EAESCMACDFIVLDMACCAP );
    }

    /**
     * Step 1.  (K1,K2) := Generate_Subkey(K);
     * has already taken place during the SignerAesCmacInit method call.
     */

    /**
     *   Step 2.  n := ceil(len/const_Bsize);
     *   Step 3.  if n = 0
     *            then
     *                 n := 1;
     *                 flag := false;
     *            else
     *                 if len mod const_Bsize is 0
     *                 then flag := true;
     *                 else flag := false;
     */
    if( AES_BLOCK_SIZE > messageLen )
    {
        blockCount = 1;
        bNoPaddingRequired = FALSE;
    }
    else
    {
        blockCount = ( messageLen / AES_BLOCK_SIZE );

        if( 0 == ( messageLen % AES_BLOCK_SIZE ) )
        {
            bNoPaddingRequired = TRUE;
        }
        else
        {
            bNoPaddingRequired = FALSE;         
        }
    }

    /**
      *   Step 4.  if flag is true
      *            then M_last := M_n XOR K1;
      *            else M_last := padding(M_n) XOR K2;
     */
    if( TRUE == bNoPaddingRequired )
    {
        /**
         * Cache the last block
         */
        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_memcpy( M_last, &pMessage[ ( ( blockCount - 1 ) * AES_BLOCK_SIZE ) ],
            AES_BLOCK_SIZE );

        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_memxor( M_last, K1, AES_BLOCK_SIZE );
    }
    else
    {   
        VLT_U8 paddingRequired = (VLT_U8)( AES_BLOCK_SIZE - ( messageLen % AES_BLOCK_SIZE ) );

        /**
         * Ensure the calling method has got enough buffer to
         * add any padding.
         */
        if( messageCapacity < ( messageLen + paddingRequired ) )
        {
            return( EAESCMACDFIVLDMSGCAP );
        }

        blockCount = ( ( messageLen + paddingRequired ) / AES_BLOCK_SIZE );

        /**
         * Cache the last block
         */
        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_memcpy( M_last, &pMessage[ ( ( blockCount - 1 ) * AES_BLOCK_SIZE ) ],
            AES_BLOCK_SIZE );

        padSize = ( messageLen % AES_BLOCK_SIZE );

        if( VLT_OK != ( status = PaddingAdd( PADDING_ISO9797_METHOD2, AES_BLOCK_SIZE,
            M_last, &padSize, ( messageCapacity - ( messageLen - padSize ) ) ) ) )
        {
            return( status );
        }
        
        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_memxor( M_last, K2, AES_BLOCK_SIZE );
    }

    /**
     *   Step 5.  X := const_Zero;
     *   Step 6.  for i := 1 to n-1 do
     *                begin
     *                  Y := X XOR M_i;
     *                  X := AES-128(K,Y);
     *                end
     *            Y := M_last XOR X;
     *            T := AES-128(K,Y);
     */
    if( 1 == blockCount )
    {
        if( VLT_OK != ( status = SignerAesCmacUpdate( M_last,
            AES_BLOCK_SIZE,
            AES_BLOCK_SIZE ) ) )
        {
            return( status );
        }
    }
    else
    {
        if( VLT_OK != ( status = SignerAesCmacUpdate( pMessage,
            ( ( blockCount - 1 ) * AES_BLOCK_SIZE ),
            messageCapacity ) ) )
        {
            return( status );
        }

        /**
         * Do the last block
         */
        if( VLT_OK != ( status = SignerAesCmacUpdate( M_last,
            AES_BLOCK_SIZE,
            AES_BLOCK_SIZE ) ) )
        {
            return( status );
        }
    }


    /**
     * Return the MAC back to the caller.
     */
    *pMacLen = AES_BLOCK_SIZE;
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( pMac, mac, AES_BLOCK_SIZE );
    
    return ( VLT_OK );
}

VLT_STS SignerAesCmacUpdate( VLT_PU8 pMessage, VLT_U32 messageLen, VLT_U32 messageCapacity )
{
    VLT_STS status = VLT_FAIL;

    /**
     * The Update only deals with multiples blocks of AES_BLOCK_SIZE
     */
    VLT_U32 blockCount = ( messageLen / AES_BLOCK_SIZE );
    VLT_U32 i = 0;
    VLT_U32 outCount = 0;
    VLT_U8 block[AES_BLOCK_SIZE];

    /**
     * Rudimentary input parameter validation
     */
    if( NULL == pMessage )
    {
        return( EAESCMACUPIVLDMSG );
    }

    if( 0 == messageLen )
    {
        return( EAESCMACUPIVLDMSGLEN );
    }

    if( ( 0 == messageCapacity ) ||
        ( messageLen > messageCapacity ) )
    {
        return( EAESCMACUPIVLDMSGCAP );
    }

    /**
     * clear the working block.
     */
    (void)host_memset( block, 0x00, AES_BLOCK_SIZE );

    for( i = 0; i < blockCount; i++ )
    {
        /**
         * Do not modify the original text, cache the block.
         */
        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_memcpy( block, &pMessage[ ( AES_BLOCK_SIZE * i ) ], AES_BLOCK_SIZE );
        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_memxor( block, mac, AES_BLOCK_SIZE );

        if( VLT_OK != ( status = CipherUpdate( block,
            AES_BLOCK_SIZE,
            AES_BLOCK_SIZE,
            mac,
            &outCount,
            AES_BLOCK_SIZE ) ) )
        {
            return( status );
        }
    }

    return ( VLT_OK );
}

VLT_U16 SignerAesCmacGetBlockSize( void )
{
    return ( CipherGetBlockSize() );
}


/**

RFC 4493                 The AES-CMAC Algorithm                June 2006


   Figure 2.2 specifies the subkey generation algorithm.

   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   +                    Algorithm Generate_Subkey                      +
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   +                                                                   +
   +   Input    : K (128-bit key)                                      +
   +   Output   : K1 (128-bit first subkey)                            +
   +              K2 (128-bit second subkey)                           +
   +-------------------------------------------------------------------+
   +                                                                   +
   +   Constants: const_Zero is 0x00000000000000000000000000000000     +
   +              const_Rb   is 0x00000000000000000000000000000087     +
   +   Variables: L          for output of AES-128 applied to 0^128    +
   +                                                                   +
   +   Step 1.  L := AES-128(K, const_Zero);                           +
   +   Step 2.  if MSB(L) is equal to 0                                +
   +            then    K1 := L << 1;                                  +
   +            else    K1 := (L << 1) XOR const_Rb;                   +
   +   Step 3.  if MSB(K1) is equal to 0                               +
   +            then    K2 := K1 << 1;                                 +
   +            else    K2 := (K1 << 1) XOR const_Rb;                  +
   +   Step 4.  return K1, K2;                                         +
   +                                                                   +
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
VLT_STS generateSubKeys( const KEY_BLOB* pKey, VLT_PU8 K1, VLT_PU8 K2 )
{   
    CIPHER_PARAMS theParams;
    VLT_STS status = VLT_FAIL;
    VLT_U32 outLen = 0;
    VLT_U8 L[AES_BLOCK_SIZE] =
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    VLT_U8 Zero[AES_BLOCK_SIZE] =
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    VLT_U8 Rb[AES_BLOCK_SIZE] =
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x87
    };  

    /**
     *
     * Step 1.  L := AES-128(K, const_Zero);
     *
     */ 

    /**
     * Algorithm AES
     * Padding None
     * Block Mode ECB
     */
    theParams.algoID = VLT_ALG_CIP_AES;
    theParams.paddingScheme = PADDING_NONE;
    theParams.chainMode = BLOCK_MODE_ECB;
    theParams.pIV = 0;  

    /**
     * Initialise the Cipher
     */
    if( VLT_OK != ( status = CipherInit( VLT_ENCRYPT_MODE , pKey, (VLT_PU8)&theParams ) ) )
    {
        return( status );
    }   

    /**
     * Encrypt the text
     */
    if( VLT_OK != ( status = CipherDoFinal( Zero,
        AES_BLOCK_SIZE,
        AES_BLOCK_SIZE,
        L,
        &outLen,
        AES_BLOCK_SIZE ) ) )
    {
        return( status );
    }

    /**
     * Release the Cipher.
     */
    if( VLT_OK != ( status = CipherClose() ) )
    {
        return( status );
    }

    /**
     *
     * Step 2.  if MSB(L) is equal to 0
     *          then    K1 := L << 1;
     *          else    K1 := (L << 1) XOR const_Rb;
     *
     */ 
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( K1, L, AES_BLOCK_SIZE );
    if( 0 == ( K1[0] & 0x80 ) )
    {
        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_lshift( K1, 16, 1 );
    }
    else
    {
        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_lshift( K1, 16, 1 );
        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_memxor( K1, Rb, 16 );
    }

    /**
     *
     * Step 3.  if MSB(K1) is equal to 0
     *          then    K2 := K1 << 1;
     *          else    K2 := (K1 << 1) XOR const_Rb;
     *
     */ 
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( K2, K1, AES_BLOCK_SIZE );
    if( 0 == ( K2[0] & 0x80 ) )
    {
        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_lshift( K2, 16, 1 );
    }
    else
    {
        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_lshift( K2, 16, 1 );
        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_memxor( K2, Rb, 16 );
    }

    return( VLT_OK );
}

#endif /*( VLT_ENABLE_CIPHER_AES == VLT_ENABLE )*/

#if( VLT_ENABLE_CIPHER_TESTS == VLT_ENABLE )

VLT_STS AesCMacExample2()
{
    VLT_STS status = VLT_FAIL ;

    VLT_U8 K[] =
    {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
    };

    VLT_U8 Msg[] =
    {
        0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
        0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a
    };

    VLT_U8 ExpectedMac[] =
    {
        0x07, 0x0a, 0x16, 0xb4, 0x6b, 0x4d, 0x41, 0x44,
        0xf7, 0x9b, 0xdd, 0x9d, 0xd0, 0x4a, 0x28, 0x7c
    };

    VLT_U8 CalculatedMac[] =
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    VLT_U32 macLen = AES_BLOCK_SIZE;

    KEY_BLOB keyBlob =
    {
        VLT_KEY_AES_128,
        AES_128_KEY_SIZE,
        K
    };
    
    if( VLT_OK != ( status = SignerAesCmacInit( VLT_SIGN_MODE,
        &keyBlob, 0 ) ) )
    {
        return( status );
    }

    if( VLT_OK != ( status = SignerAesCmacDoFinal( Msg,
        NELEMS(Msg),
        NELEMS(Msg),
        CalculatedMac,
        &macLen,
        NELEMS(CalculatedMac) ) ) )
    {
        return( status );
    }

    SignerAesCmacClose();

    if( host_memcmp( CalculatedMac, ExpectedMac, NELEMS(ExpectedMac) ) )
    {
        return( VLT_FAIL );
    }

    return( VLT_OK );
}

VLT_STS AesCMacExample3()
{
    VLT_STS status = VLT_FAIL ;

    VLT_U8 K[] =
    {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
    };

    VLT_U8 Msg[] =
    {
        0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
        0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
        0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
        0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
        0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    VLT_U8 ExpectedMac[] =
    {
        0xdf, 0xa6, 0x67, 0x47, 0xde, 0x9a, 0xe6, 0x30,
        0x30, 0xca, 0x32, 0x61, 0x14, 0x97, 0xc8, 0x27
    };

    VLT_U8 CalculatedMac[] =
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    VLT_U32 macLen = AES_BLOCK_SIZE;

    KEY_BLOB keyBlob =
    {
        VLT_KEY_AES_128,
        AES_128_KEY_SIZE,
        K
    };
    
    if( VLT_OK != ( status = SignerAesCmacInit( VLT_SIGN_MODE,
        &keyBlob, 0 ) ) )
    {
        return( status );
    }

    if( VLT_OK != ( status = SignerAesCmacDoFinal( Msg,
        40,
        NELEMS(Msg),
        CalculatedMac,
        &macLen,
        NELEMS(CalculatedMac) ) ) )
    {
        return( status );
    }

    SignerAesCmacClose();

    if( host_memcmp( CalculatedMac, ExpectedMac, NELEMS(ExpectedMac) ) )
    {
        return( VLT_FAIL );
    }

    return( VLT_OK );
}


VLT_STS AesCMacExample4()
{
    VLT_STS status = VLT_FAIL ;

    VLT_U8 K[] =
    {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
    };

    VLT_U8 Msg[] =
    {
        0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
        0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
        0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
        0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
        0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
        0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
        0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,
        0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10
    };

    VLT_U8 ExpectedMac[] =
    {
        0x51, 0xf0, 0xbe, 0xbf, 0x7e, 0x3b, 0x9d, 0x92,
        0xfc, 0x49, 0x74, 0x17, 0x79, 0x36, 0x3c, 0xfe
    };

    VLT_U8 CalculatedMac[] =
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    VLT_U32 macLen = AES_BLOCK_SIZE;

    KEY_BLOB keyBlob =
    {
        VLT_KEY_AES_128,
        AES_128_KEY_SIZE,
        K
    };
    
    if( VLT_OK != ( status = SignerAesCmacInit( VLT_SIGN_MODE,
        &keyBlob, 0 ) ) )
    {
        return( status );
    }

    if( VLT_OK != ( status = SignerAesCmacDoFinal( Msg,
        NELEMS(Msg),
        NELEMS(Msg),
        CalculatedMac,
        &macLen,
        NELEMS(CalculatedMac) ) ) )
    {
        return( status );
    }

    SignerAesCmacClose();

    if( host_memcmp( CalculatedMac, ExpectedMac, NELEMS(ExpectedMac) ) )
    {
        return( VLT_FAIL );
    }

    return( VLT_OK );
}
#endif/*( VLT_ENABLE_CIPHER_TESTS == VLT_ENABLE )*/

/**
 *
 * The following test vectors are the same as those of [NIST-CMAC].  The
 * following vectors are also the output of the test program in Appendix
 * A.
 *
 */
VLT_STS AesCMacTest()
{
    VLT_STS status = VLT_FAIL;

#if( VLT_ENABLE_CIPHER_TESTS == VLT_ENABLE )
    if( VLT_OK != ( status = AesCMacExample2() ) )
    {
        return( status );
    }

    if( VLT_OK != ( status = AesCMacExample3() ) )
    {
        return( status );
    }
    
    if( VLT_OK != ( status = AesCMacExample4() ) )
    {
        return( status );
    }
#endif /*( VLT_ENABLE_CIPHER_TESTS == VLT_ENABLE )*/

    return( status );
}
