/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#include "vaultic_common.h"
#include "vaultic_mem.h"
#include "vaultic_cipher.h"
#include "vaultic_DES.h"
#include "vaultic_DES_stats.h"

/**
 * Error Codes
 */
#define EDESIIVLDKEY            VLT_ERROR( VLT_DES, 0 ) 
#define EDESIIVLDPRM            VLT_ERROR( VLT_DES, 1 ) 
#define EDESDFINVLDDL           VLT_ERROR( VLT_DES, 2 ) 
#define EDESDFINVLDDT           VLT_ERROR( VLT_DES, 3 ) 
#define EDESDFINVLDLEN          VLT_ERROR( VLT_DES, 4 ) 
#define EDESDFINVLDMOD          VLT_ERROR( VLT_DES, 5 ) 
#define EDESTESTFLDENCRYPT      VLT_ERROR( VLT_DES, 6 ) 
#define EDESTESTFLDDECRYPT      VLT_ERROR( VLT_DES, 7 ) 

#if( VLT_ENABLE_CIPHER_DES == VLT_ENABLE )
/**
 * Private Methods 
 */
static VLT_STS GetSubkeys( void );
static VLT_STS RegConvert( VLT_PU8 ipstr, VLT_PU32 opbit );
static VLT_STS BitConvert( VLT_PU32 ipbit, VLT_PU8 opstr );
static VLT_STS Encryption( void );
static VLT_STS Decryption( void );    
static VLT_STS Do_Encrypt( VLT_PU8 Data, VLT_PU8 Key, VLT_PU8 DataOut );
static VLT_STS Do_Decrypt( VLT_PU8 Data, VLT_PU8 Key, VLT_PU8 DataOut );

/**
 * Private Data 
 */
static VLT_U8 operationMode;
static VLT_U8 desKey[DES_KEY_SIZE];

static VLT_U32 InputData[64];
static VLT_U32 OutputData[64];
static VLT_U32 KeyInput[64];
static VLT_U32 SubKey[16][48];



VLT_STS DesInit( VLT_U8 opMode, const KEY_BLOB* pKey, VLT_PU8 pParams )
{
    /**
     * Make sure we have a valid key 
     */
    if( ( NULL == pKey ) || ( VLT_KEY_DES != pKey->keyType ) )
    {
        return( EDESIIVLDKEY );
    }

    /**
     * Make sure we have a valid params pointer
     */
    if( NULL == pParams )
    {
        return( EDESIIVLDPRM );
    }
    
    /**
     *  Cache the operational mode (Encrypt/Decrypt)     
     */
    operationMode = opMode;

    /**
     *  Cache the key    
     */ 
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( desKey, pKey->keyValue, DES_KEY_SIZE );


    return( VLT_OK );
}

VLT_STS DesClose( void )
{
    /**
     *  Clear the key    
     */ 
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memset( desKey, 0x00, DES_KEY_SIZE );  

    return( VLT_OK );
}

VLT_STS DesDoFinal( VLT_PU8 pDataIn, VLT_PU32 pDataInLen, VLT_PU8 pDataOut, VLT_PU32 pDataOutLen )
{
    VLT_U32 rounds = 0;
    VLT_U32 count = 0;

    /**
     *  Ensure the input data pointer and input data length pointer isn't null.
     */ 
    if(( NULL == pDataIn )||( NULL == pDataInLen))
    {
        return( EDESDFINVLDDT );
    }

    /**
     *  Ensure the output data pointer and output data length pointer isn't null
     */     
    if(( NULL == pDataOut )||( NULL == pDataOutLen))
    {
        return( EDESDFINVLDDL );
    }

    /**
     *  Ensure the data length passed in is a 
     *  length multiple of DES_BLOCK_SIZE.
     */ 
    if( 0 != (*pDataInLen % DES_BLOCK_SIZE ) )
    {
        return( EDESDFINVLDLEN );
    }

    /**
     *  Determine the number of blocks we need
     *  to process.
     */ 
    rounds = *pDataInLen / DES_BLOCK_SIZE;

    /**
     *  Process the data in place.
     */ 
    for( count = 0; count < rounds; count++ )
    {
        if(  VLT_ENCRYPT_MODE == operationMode )
        {
            Do_Encrypt( &pDataIn[( count * DES_BLOCK_SIZE )], 
                desKey, 
                &pDataOut[( count * DES_BLOCK_SIZE )] );
        }
        else if(  VLT_DECRYPT_MODE == operationMode )
        {
            Do_Decrypt( &pDataIn[( count * DES_BLOCK_SIZE )], 
                desKey, 
                &pDataOut[( count * DES_BLOCK_SIZE )] );
        }
        else
        {
            return( EDESDFINVLDMOD );
        }       
    }

    /**
     * Current implementation only deals with multiples of block size
     * so, its ok to make the out length the same as in length
     */
     *pDataOutLen = *pDataInLen;

    return( VLT_OK );
}

VLT_STS DesUpdate( VLT_PU8 pDataIn, VLT_PU32 pDataInLen, VLT_PU8 pDataOut, VLT_PU32 pDataOutLen )
{
    return( DesDoFinal(pDataIn, pDataInLen, pDataOut, pDataOutLen ) );
}

VLT_U16 DesGetBlockSize( void )
{
    return( DES_BLOCK_SIZE );
}


static VLT_STS RegConvert( VLT_PU8 inputchars, VLT_PU32 outputbits )
{
    VLT_U32 byte_counter, bit_counter, k;
    VLT_U8 bitmask[8] = {128, 64, 32, 16, 8, 4, 2, 1};

    for (byte_counter = 0; byte_counter <= 7; byte_counter++)
    {
        k = byte_counter * 8;
        for (bit_counter = 0; bit_counter <= 7; bit_counter++)
        {
            if (inputchars[byte_counter] & bitmask[bit_counter])
            {
                outputbits[k+ bit_counter] = 1;
            }
            else
            {
                outputbits[k+ bit_counter] = 0;
            }
        }
    }
    return( VLT_OK );
}

static VLT_STS BitConvert( VLT_PU32 inputbits, VLT_PU8 outputchars )
{
    VLT_U32 byte_counter, bit_counter;
    VLT_U8 bitmask[8] = {128, 64, 32, 16, 8, 4, 2, 1};  
    

    for (byte_counter = 0; byte_counter <= 7; byte_counter++)
    {
        // Initialise element to zero

        outputchars[byte_counter] = 0;

        for (bit_counter = 0; bit_counter <= 7; bit_counter++)
        {
            if (inputbits[(byte_counter * 8 ) + bit_counter])
            {
                outputchars[byte_counter] = outputchars[byte_counter] | bitmask[bit_counter];
            }
        }
    }
    return( VLT_OK );
}

static VLT_STS GetSubkeys( void )
{
    VLT_U32 i,j,n,shift;
    VLT_U32 Croll,Droll;
    VLT_U32 Cn[28],Dn[28];
    VLT_U32 tmp1[56];   

    for (i = 0; i < 56; i++)
        tmp1[i] = KeyInput[PC1[i]-1];       // permutation choice 1 

    for (i = 0; i < 28; i++)
    {
        Cn[i] = tmp1[i];                    // build C0 
        Dn[i] = tmp1[i+28];                 // build D0 
    }

    for (n = 0; n < 16; n++)
    {
        if (n==0 || n==1 || n==8 || n==15)
            shift = 1;                      // determine # of shift 
        else
            shift = 2;

        for (i = 1; i <= shift; i++)        // left shifting Cn and Dn 
        {
            Croll = Cn[0];
            Droll = Dn[0];
            for (j = 0; j < (28-1); j++)
            {
                Cn[j] = Cn[j+1];
                Dn[j] = Dn[j+1];
            }
            Cn[27] = Croll;                 // wrap around operation 
            Dn[27] = Droll;                 // wrap around operation 
        }

        for (i = 0; i < 28; i++)            // merge CnDn together 
        {                                   // to from tmp1 = CnDn 
            tmp1[i] = Cn[i];
            tmp1[i+28] = Dn[i];
        }

        for (i = 0; i <48; i++)
        {
            SubKey[n][i] = tmp1[PC2[i]-1];  // subkey generation 
        }
    }                                       // end of n loop 
    return( VLT_OK );
}   

static VLT_STS Encryption( void )
{
    VLT_U32 i, j, k, n;                         // general counter 
    VLT_U32 tmp[64];                            // Intermmediate array
    VLT_U32 Rn[32];                             // right block, 32 bits 
    VLT_U32 Ln[32];                             // left blcok, 32 bits 
    VLT_U32 Ln1[32];                            // tmp left blcok, 32 bits 
    VLT_U32 blk48[48];                          // general 48 bit block 
    VLT_U32 fRK[32];                            // result of function f 
    VLT_U32 row,col;

    // Get the sub keys
    GetSubkeys();

    for (i = 0; i < 64; i++)
    {
        tmp[i] = InputData[IP[i]-1];                // initial permutation 
    }

    for (i = 0; i < 32; i++)
    {
        Ln[i] = tmp[i];                             // build L0 
        Rn[i] = tmp[i+32];                          // build R0 
    }

    for (n = 0; n < 16; n++)
    {
        for (i = 0; i < 32; i++)
        {
            Ln1[i] = Rn[i];                         // Ln store the orginial of Rn 
        }

        for (i = 0; i < 48; i++)
        {
            blk48[i] = Rn[Eselect[i]-1];            // E bit-selection process 
        }

        for (i = 0; i < 48; i++)
        {
            blk48[i] = (blk48[i] != SubKey[n][i]);  // blk48 XOR subkey 
        }

        for (i = 0; i <= 7; i++)
        {
            j = i * 6;
            row = blk48[j]*2 + blk48[j+5];
            col = blk48[j+1]*8 + blk48[j+2]*4 + blk48[j+3]*2 + blk48[j+4];
            k = Sn[i][row][col];                    // look up table 
            tmp[i*4] = ((8 & k) != 0);              // test MSB of k 
            tmp[(i*4)+1] = ((4 & k) != 0);
            tmp[(i*4)+2] = ((2 & k) != 0);
            tmp[(i*4)+3] = ((1 & k) != 0);          // test LSB of k 
        }

        for (i = 0; i < 32;i ++)
        {
            fRK[i] = tmp[P[i]-1];                   // P permutation 
        
            Rn[i] = (Ln[i] != fRK[i]);              // Ln XOR fRK 
        
            Ln[i] = Ln1[i];                         // Ln = Rn 
        }
    }

    for (i = 0; i < 32; i++)                        // tmp is the preoutput 
    {                                               // tmp = RnLn 
        tmp[i] = Rn[i];
        tmp[i+32] = Ln[i];
    }

    for (i = 0; i < 64; i++)
    {
        OutputData[i] = tmp[IPinv[i]-1];            // inverse initial permutation 
    }
    return( VLT_OK );
}   
                                                // end of Encryption routine 

static VLT_STS Decryption(void)
{
    // Decryption algorithm
    VLT_U32 i, j, k, n;         // general counter 
    VLT_U32 tmp[64];            // Intermmediate array
    VLT_U32 Rn[32];             // right block, 32 bits 
    VLT_U32 Ln[32];             // left blcok, 32 bits 
    VLT_U32 Ln1[32];            // tmp left blcok, 32 bits 
    VLT_U32 blk48[48];          // general 48 bit block 
    VLT_U32 fLK[32];            // result of function f 
    VLT_U32 row,col;    

    // Get the sub keys

    GetSubkeys();

    for (i = 0; i < 64; i++)
    {
        tmp[i] = InputData[IP[i]-1];    // initial permutation(inv) 
    }

    for (i = 0; i < 32; i++)
    {
        Rn[i] = tmp[i];     // build R16 
        Ln[i] = tmp[i+32];  // build L16 
    }

    for (n = 15; ; n--)
    {
        for (i = 0; i < 32; i++)
        {
            Ln1[i] = Ln[i];             // Ln store the orginial of Ln 
        }

        for (i = 0; i < 48; i++)
        {
            blk48[i] = Ln[Eselect[i]-1]; // E bit-selection process 
        }
        for (i = 0; i < 48; i++)
        {
            blk48[i] = (blk48[i] != SubKey[n][i]);       // blk48 XOR subkey 
        }

        for (i = 0; i <= 7; i++)
        {
            j = i * 6;
            row = blk48[j]*2 + blk48[j+5];
            col = blk48[j+1]*8 + blk48[j+2]*4 + blk48[j+3]*2 + blk48[j+4];
            k = Sn[i][row][col];                            // look up table 
            tmp[i*4] = ((8 & k) != 0);                      // test MSB of k 
            tmp[(i*4)+1] = ((4 & k) != 0);
            tmp[(i*4)+2] = ((2 & k) != 0);
            tmp[(i*4)+3] = ((1 & k) != 0);                  // test LSB of k 
        }

        for (i = 0; i < 32; i++)
        {
            fLK[i] = tmp[P[i]-1];           // P permutation 
        
            Ln[i] = (Rn[i] != fLK[i]);      // Rn XOR fLK 
        
            Rn[i] = Ln1[i];                 // Ln = Rn 
        }

        if( 0 == n )
        {
            break;
        }
    }

    for (i = 0; i < 32; i++)                // tmp is the preoutput 
    {                                       // tmp = RnLn 
        tmp[i] = Ln[i];
        tmp[i+32] = Rn[i];
    }

    for (i = 0; i < 64; i++)
    {
         OutputData[i] = tmp[IPinv[i]-1];       // initial permutation 
    }
    return( VLT_OK );
}                                   // end of Decryption routine 

static VLT_STS Do_Encrypt( VLT_PU8 DataIn, VLT_PU8 Key, VLT_PU8 DataOut )
{   
    RegConvert(DataIn, InputData);
    RegConvert(Key, KeyInput);
    Encryption();
    BitConvert(OutputData, DataOut);

    return( VLT_OK );
}

static VLT_STS Do_Decrypt( VLT_PU8 DataIn, VLT_PU8 Key, VLT_PU8 DataOut )
{   
    RegConvert(DataIn, InputData);
    RegConvert(Key, KeyInput);
    Decryption();
    BitConvert(OutputData, DataOut);

    return( VLT_OK );
}

#endif /* ( VLT_ENABLE_CIPHER_DES == VLT_ENABLE ) */



/**
 * The test vectors have been derived from Sun Microsystems
 * cryptographic engine. The provider used was Sun JCE.
 */
VLT_STS DesTest( void )
{
#if( VLT_ENABLE_CIPHER_TESTS == VLT_ENABLE )
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

    VLT_U8 cipherTextVector1[] = 
    {
        0xce, 0x79, 0xe2, 0x78, 0x97, 0xf7, 0x8b, 0x86,
        0x0e, 0xed, 0x1e, 0x3b, 0xea, 0xdd, 0xa5, 0xa7,
        0xcb, 0xae, 0x75, 0x48, 0x31, 0x35, 0xa9, 0xfb,
        0xbd, 0x2f, 0x69, 0xc2, 0x53, 0xa5, 0x71, 0xe8
    };

    VLT_U8 key[] = 
    { 
        0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF
    };


    VLT_U8 params = 0;
    VLT_U8 text[( DES_BLOCK_SIZE * 4 ) ];

    /**
     * The key blob
     */
    KEY_BLOB desKey;
    
    /**
     * Text to be encrypted with the known test plain text vector.
     */
    VLT_U32 plainTextSize = (sizeof(plainTextVector1)/sizeof(VLT_U8));
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( text, plainTextVector1, plainTextSize );

    /**
     * setup the key blob
     */
    desKey.keyType = VLT_KEY_DES;
    desKey.keySize = DES_KEY_SIZE;
    desKey.keyValue = key;

    DesInit( VLT_ENCRYPT_MODE, &desKey, &params ); 
    DesUpdate( text, &plainTextSize, text, &plainTextSize );

    if( 0 != host_memcmp( text, cipherTextVector1, DES_BLOCK_SIZE ) )
    {
        return( EDESTESTFLDENCRYPT );
    }

    DesInit( VLT_DECRYPT_MODE, &desKey, &params ); 
    DesUpdate( text, &plainTextSize, text, &plainTextSize );
    
    if( 0 != host_memcmp( text, plainTextVector1, DES_BLOCK_SIZE ) )
    {
        return( EDESTESTFLDDECRYPT );
    }

    return( VLT_OK );
#else 
    return( VLT_FAIL );
#endif /* ( VLT_ENABLE_CIPHER_TESTS == VLT_ENABLE ) */
}


