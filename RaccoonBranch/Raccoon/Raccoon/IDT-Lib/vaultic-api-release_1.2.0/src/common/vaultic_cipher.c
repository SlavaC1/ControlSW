/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#include "vaultic_common.h"
#include "vaultic_cipher.h"
#include "vaultic_DES.h"
#include "vaultic_TDES.h"
#include "vaultic_AES.h"
#include "vaultic_mem.h"
#include "vaultic_padding.h"

/**
 * Error Codes
 */
#define ECPHIINVLDALGO      VLT_ERROR( VLT_CIPHER, 0 )
#define ECPHINOTSUPPORTED   VLT_ERROR( VLT_CIPHER, 2 )
#define ECPHIIVLDPRM        VLT_ERROR( VLT_CIPHER, 3 )
#define ECPHCLSNOTSUPPORTED VLT_ERROR( VLT_CIPHER, 4 )
#define ECPHDFNOTSUPPORTED  VLT_ERROR( VLT_CIPHER, 5 )
#define ECPHUPDNOTSUPPORTED VLT_ERROR( VLT_CIPHER, 6 )
#define ECPHBLKNOTSUPPORTED VLT_ERROR( VLT_CIPHER, 7 )
#define ECPHICHNMODE        VLT_ERROR( VLT_CIPHER, 8 )
#define ECPHIPADUNKNOWN     VLT_ERROR( VLT_CIPHER, 9 )
#define ECPHUPDINVLDBLOCK   VLT_ERROR( VLT_CIPHER, 10 )
#define ECPHUPDINVLDLEN     VLT_ERROR( VLT_CIPHER, 11 )
#define ECPHUPDNULLPARAM    VLT_ERROR( VLT_CIPHER, 12 )
#define ECPHUPDINVLDCPCT    VLT_ERROR( VLT_CIPHER, 13 )

/**
 * Private Defs
 */
#define ST_UNKNOWN          0x00
#define ST_INITIALISED      0x10
#define ST_UPDATED          0x20
#define ST_FINALISED        0x30
#define MAX_BLOCK_SZ        0x20
#define MAX_IV_SZ           0x10


typedef VLT_STS (*pfnCipherInit)( VLT_U8 opMode, const KEY_BLOB* pKey, VLT_PU8 pParams );
typedef VLT_STS (*pfnCipherClose)( void );
typedef VLT_STS (*pfnCipherDoFinal)( 
    VLT_PU8 pDataIn, 
    VLT_PU32 pDataInLen, 
    VLT_PU8 pDataOut, 
    VLT_PU32 pDataOutLen);

typedef VLT_STS (*pfnCipherUpdate)( 
    VLT_PU8 pDataIn, 
    VLT_PU32 pDataInLen, 
    VLT_PU8 pDataOut, 
    VLT_PU32 pDataOutLen);

typedef VLT_U16 (*pfnCipherGetBlockSize)( void );

typedef struct _cipher
{
    pfnCipherInit cipherInit;
    pfnCipherClose cipherClose;
    pfnCipherDoFinal cipherDoFinal;
    pfnCipherUpdate cipherUpdate;
    pfnCipherGetBlockSize cipherGetBlockSize;

} Cipher;


/**
 * Private Data
 */
static CIPHER_PARAMS params = { 0 };

/**
 * The cipher function pointer structure will never 
 * be set to zero, by default it will point to the 
 * TDes implementation.
 */
static Cipher theCipher;
static VLT_U8 chainBlock[MAX_BLOCK_SZ];
static VLT_U8 tempBlock[MAX_BLOCK_SZ];
static VLT_U8 workingBlock[MAX_BLOCK_SZ];
static VLT_U8 operationalMode = 0;
static VLT_U8 cipherState = ST_UNKNOWN;


VLT_STS CipherInit( VLT_U8 opMode, const KEY_BLOB* pKey, VLT_PU8 pParams )
{   
    VLT_U8 keyMode ;
    VLT_STS status = VLT_FAIL;

    /**
     * Make sure we have a valid params pointer
     */
    if( NULL == pParams )
    {
        return( ECPHIIVLDPRM );
    }
    else
    {
        /**
         * Cache the parameters.
         */
        params = *((CIPHER_PARAMS*)pParams);

        if( ( params.algoID != VLT_ALG_CIP_DES ) &&
            ( params.algoID != VLT_ALG_CIP_TDES_2K_EDE ) &&
            ( params.algoID != VLT_ALG_CIP_TDES_3K_EDE ) &&
            ( params.algoID != VLT_ALG_CIP_TDES_3K_EEE ) &&
            ( params.algoID != VLT_ALG_CIP_AES ) &&
            ( params.algoID != VLT_ALG_KTS_TDES_3K_EEE ) &&
            ( params.algoID != VLT_ALG_KTS_TDES_3K_EDE ) &&
            ( params.algoID != VLT_ALG_KTS_AES ) )
        {
            /**
             * Clear the cipherState to signify the
             * fact that something has gone pear
             * shaped and we shouldn't deligate
             * any further calls to the concrete
             * cipher methods.
             */
            cipherState = ST_UNKNOWN;

            /**
             * Return the appropriate error and
             * exit gracefully. 
             */
            return( ECPHIINVLDALGO );
        }
    }

    /**
     * Set all the function pointers
     * to the actual concrete cipher 
     * methods based on the algo Id.
     */
    switch(params.algoID)
    {

    #if( VLT_ENABLE_CIPHER_DES == VLT_ENABLE )
        case VLT_ALG_CIP_DES:
            theCipher.cipherInit = DesInit;
            theCipher.cipherDoFinal = DesDoFinal;
            theCipher.cipherGetBlockSize = DesGetBlockSize;         
            theCipher.cipherUpdate = DesUpdate;
            theCipher.cipherClose = DesClose;
        break;
    #endif /* ( VLT_ENABLE_CIPHER_DES == VLT_ENABLE )*/

    #if( VLT_ENABLE_CIPHER_TDES == VLT_ENABLE )
        case VLT_ALG_CIP_TDES_2K_EDE:   
        case VLT_ALG_CIP_TDES_3K_EDE:
        case VLT_ALG_CIP_TDES_3K_EEE:
        case VLT_ALG_KTS_TDES_3K_EEE:
        case VLT_ALG_KTS_TDES_3K_EDE:
            theCipher.cipherInit = TDesInit;
            theCipher.cipherDoFinal = TDesDoFinal;
            theCipher.cipherGetBlockSize = TDesGetBlockSize;            
            theCipher.cipherUpdate = TDesUpdate;
            theCipher.cipherClose = TDesClose;
            break;
    #endif /* ( VLT_ENABLE_CIPHER_TDES == VLT_ENABLE ) */

    #if( VLT_ENABLE_CIPHER_AES == VLT_ENABLE )
        case VLT_ALG_CIP_AES:
        case VLT_ALG_KTS_AES:
            theCipher.cipherInit = AesInit;
            theCipher.cipherDoFinal = AesDoFinal;
            theCipher.cipherGetBlockSize = AesGetBlockSize;         
            theCipher.cipherUpdate = AesUpdate;
            theCipher.cipherClose = AesClose;
            break;          
    #endif/*( VLT_ENABLE_CIPHER_AES == VLT_ENABLE )*/

        default:
            return( ECPHINOTSUPPORTED );
    }
    
    /**
     * Check and setup the Keying mode.
     */
    switch(params.algoID)
    {
        case VLT_ALG_CIP_TDES_2K_EDE:
            keyMode = TDES_EDE;
            break;          
        case VLT_ALG_CIP_TDES_3K_EDE:
        case VLT_ALG_KTS_TDES_3K_EDE:
            keyMode = TDES_EDE;
            break;          
        case VLT_ALG_CIP_TDES_3K_EEE:
        case VLT_ALG_KTS_TDES_3K_EEE:
            keyMode = TDES_EEE; 
            break;
        case VLT_ALG_CIP_DES:
            /**
             * Do nothing for des the 
             * key mode is not relevant.
             */
            break;
        case VLT_ALG_CIP_AES:
        case VLT_ALG_KTS_AES:
            /**
             * Do nothing for aes the 
             * key mode is not relevant.
             */
            break;
        default:
            return( ECPHINOTSUPPORTED );
    }

    /**
     * Check the chaining mode.
     */
    switch( params.chainMode )
    {   
        case BLOCK_MODE_ECB:        
        case BLOCK_MODE_CBC:
            break;
        case BLOCK_MODE_CFB:
        case BLOCK_MODE_OFB:
        default:
            return(ECPHICHNMODE);
    }   

    /**
     * Initialise the chaining block to zeros
     */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memset( chainBlock, 0x00, theCipher.cipherGetBlockSize() );
        
    /**
     * Check the padding scheme.
     */
    switch( params.paddingScheme )
    {   
        case PADDING_ISO9797_METHOD2:                   
        case PADDING_NONE:
        case PADDING_PKCS5:
        case PADDING_PKCS7:
            break;
        default:
            return(ECPHIPADUNKNOWN);
    }       

    /**
     * Cache the operationalMode, we'll need it
     * when we are doing the padding.
     */
    operationalMode = opMode;

    /**
     * Delegate the call to the initialisation method
     * of the appropriate cipher.
     */
    status = theCipher.cipherInit( opMode, pKey, &keyMode );

    /**
     * Prepare to accept the first block 
     * of data.
     */
    if( VLT_OK == status )
    {
        cipherState = ST_INITIALISED;
    }

    return( status );
}

VLT_STS CipherClose( void )
{
    if( ST_UNKNOWN != cipherState )
    {
        return( theCipher.cipherClose() );
    }
    return( ECPHCLSNOTSUPPORTED );
}

VLT_STS CipherDoFinal( VLT_PU8 pDataIn, 
        VLT_U32 DataInLen, 
        VLT_U32 dataInCapacity, 
        VLT_PU8 pDataOut, 
        VLT_PU32 pDataOutLen, 
        VLT_U32 dataOutCapacity )
{
    VLT_STS status = VLT_FAIL;      

    if ( ( ST_UNKNOWN == cipherState ) ||
         ( ST_FINALISED == cipherState ) )

    {
        return( ECPHDFNOTSUPPORTED );   
    }

    /**
     * Ensure we haven't been passed
     * null pointers by the caller.
     */
    if( ( NULL == pDataIn )||
        ( NULL == pDataOutLen ) ||
        ( NULL == pDataOut ) )
    {
        return( ECPHUPDNULLPARAM );
    }
    
    /**
     * Apply the padding if we have been called to
     * encrypt data.
     */
    if( ( VLT_ENCRYPT_MODE == operationalMode ) && 
        ( PADDING_NONE != params.paddingScheme ) )
    {
        status = PaddingAdd( params.paddingScheme, 
            theCipher.cipherGetBlockSize(), 
            pDataIn,
            &DataInLen, 
            dataInCapacity );
    }
    else
    {
        status = VLT_OK;
    }

    /**
     * Process the data, encrypt/decrypt
     */
    if( VLT_OK == status )
    {
        status = CipherUpdate( 
            pDataIn, 
            DataInLen,
            dataInCapacity,
            pDataOut, 
            pDataOutLen,
            dataOutCapacity);
    }
    
    
    if( VLT_OK == status )
    {
        if( VLT_DECRYPT_MODE == operationalMode )
        {
            status = PaddingRemove( params.paddingScheme, 
                theCipher.cipherGetBlockSize(), 
                pDataOut, 
                pDataOutLen );
        }
    }

    /**
     * Set the appropriate cipher state;
     */ 
    cipherState = ST_FINALISED;

    return( status );
}

VLT_STS CipherUpdate( VLT_PU8 pDataIn, 
        VLT_U32 DataInLen, 
        VLT_U32 dataInCapacity, 
        VLT_PU8 pDataOut, 
        VLT_PU32 pDataOutLen, 
        VLT_U32 dataOutCapacity )
{
    VLT_STS status = VLT_FAIL;
    VLT_U16 blockSize = 0;
    VLT_U32 byteCount = 0;
    VLT_U32 workingLen = 0;
    

    if ( ( ST_UNKNOWN == cipherState ) ||
         ( ST_FINALISED == cipherState ) )
    {
        return( ECPHUPDNOTSUPPORTED );
    }

    /**
     * Cache the block size, we'll use it 
     * frequently.
     */
    blockSize = theCipher.cipherGetBlockSize( );

    /**
     * Ensure we haven't been passed
     * null pointers by the caller.
     */
    if( ( NULL == pDataIn )     ||
        ( NULL == pDataOutLen ) ||
        ( NULL == pDataOut ) )
    {
        return( ECPHUPDNULLPARAM );
    }

    /**
     * For the CipherUpdate the capacity of
     * the buffer passed to us by the caller
     * should be equal or larger than that
     * of the data buffer length.
     */
    if( ( DataInLen > dataInCapacity ) || 
        ( DataInLen > dataOutCapacity ) )
    {
        return( ECPHUPDINVLDCPCT );
    }

    /**
     * Update only deals with data lengths
     * multiple of the block size, if the 
     * client has passed us anything else 
     * other than that then we should exit
     * gracefully-ish!
     */
    if( 0 != ( DataInLen % blockSize ) )
    {
        return( ECPHUPDINVLDLEN );
    }

    /**
     * Chunk things up in multiples of the
     * block size.
     */
    while( 0 != ( DataInLen - byteCount ) )
    {
        /*
         * Perform a copy of the input data into a temp buffer
         * this is needed to ensure the input data is not trashed
         * if CBC mode is selected.
         */
        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_memcpy( &workingBlock[0], &pDataIn[byteCount], blockSize );
        
        /**
         * Do the chaining
         */
        if( VLT_ENCRYPT_MODE == operationalMode )
        {
            if( BLOCK_MODE_CBC == params.chainMode )
            {
                if( ST_INITIALISED == cipherState )
                {
                    /*
                     * Make a copy of the IV of the first round.
                     */
                    /*
                    * No need to check the return type as pointer has been validated
                    */
                    (void)host_memcpy( chainBlock, &(params.pIV[0]), blockSize );
                    
                    cipherState = ST_UPDATED;
                }

                /*
                * No need to check the return type as pointer has been validated
                */
                (void)host_memxor( &workingBlock[0], chainBlock, blockSize );
            }
        }
        else
        {
            if( BLOCK_MODE_CBC == params.chainMode )
            {
                /*
                * No need to check the return type as pointer has been validated
                */
                (void)host_memcpy( tempBlock, &pDataIn[byteCount], blockSize );
            }
        }

        
        /**
         * Set the working length
         */
        workingLen = blockSize;


        /**
         * Do the Encrypt/Decrypt
         */
        if( VLT_OK == ( status = theCipher.cipherUpdate( 
            //&pData[byteCount], 
            &workingBlock[0], /* workingBlock is used to ensure the pDataIn is preserved */
            &workingLen,
            &pDataOut[byteCount],
            &workingLen) ) )
        {
            /**
             * It should be impossible for the block
             * cipher to return a length not equal to 
             * the blockSize, nevertheless if it does
             * exit with an appropriate error code and 
             * set the chaining block back to the IV 
             * for the next call.
             */
            if( workingLen != blockSize )
            {               
                return( ECPHUPDINVLDBLOCK );
            }                       
            
        }
        else
        {
            return( status );
        }

        /**
         * Do the chaining
         */
        if( VLT_ENCRYPT_MODE == operationalMode )
        {
            if( BLOCK_MODE_CBC == params.chainMode )
            {
                /*
                * No need to check the return type as pointer has been validated
                */
                (void)host_memcpy( chainBlock, &pDataOut[byteCount], blockSize );
            }
        }
        else
        {
            if( BLOCK_MODE_CBC == params.chainMode )
            {
                if( ST_INITIALISED == cipherState )
                {
                    /*
                    * No need to check the return type as pointer has been validated
                    */
                    (void)host_memxor( &pDataOut[byteCount], &(params.pIV[0]), blockSize );
                    cipherState = ST_UPDATED;
                }
                else
                {
                    /*
                    * No need to check the return type as pointer has been validated
                    */
                    (void)host_memxor( &pDataOut[byteCount], chainBlock, blockSize );             
                }

                /*
                * No need to check the return type as pointer has been validated
                */
                (void)host_memcpy( chainBlock, tempBlock, blockSize );
            }
        }
        
        /**
         * Update the byte count to 
         * move to the next block of data.
         */
        byteCount += workingLen;
    }       
    
    *pDataOutLen = byteCount;

    return( VLT_OK );
}

VLT_U16 CipherGetBlockSize( void )
{
    if( ST_UNKNOWN != cipherState )
    {
        return( theCipher.cipherGetBlockSize() );
    }
    return( ECPHBLKNOTSUPPORTED );
}
