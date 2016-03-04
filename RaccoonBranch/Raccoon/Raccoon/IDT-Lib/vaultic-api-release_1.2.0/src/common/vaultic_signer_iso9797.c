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
#include "vaultic_signer_iso9797.h"


/**
 * Error Codes
 */
#define INVALID_NULL_PARAMS          VLT_ERROR( VLT_ISO9797_MAC, 0 )
#define SIGNER_ALGO_ID_NOT_SUP       VLT_ERROR( VLT_ISO9797_MAC, 1 )
#define SIGNER_PADDING_UNKNOWN       VLT_ERROR( VLT_ISO9797_MAC, 2 )
#define SINGER_CLS_NOT_SUP           VLT_ERROR( VLT_ISO9797_MAC, 3 )
#define SIGNER_NOT_SETUP             VLT_ERROR( VLT_ISO9797_MAC, 4 )
#define SIGNER_DOFIN_NULLPARAM       VLT_ERROR( VLT_ISO9797_MAC, 5 )
#define SIGNER_IV_LEN_NOT_SUP        VLT_ERROR( VLT_ISO9797_MAC, 6 )
#define SIGNER_UPD_NULL_PARAMS       VLT_ERROR( VLT_ISO9797_MAC, 7 )
#define SIGNER_UPD_INVALID_CAP       VLT_ERROR( VLT_ISO9797_MAC, 8 )
#define SIGNER_UPD_NON_MUL_BLK_SZ    VLT_ERROR( VLT_ISO9797_MAC, 9 )
#define SIGNER_GMS_BLK_SZ_NOT_SUP    VLT_ERROR( VLT_ISO9797_MAC, 10 )
#define SIGNER_UPD_INVALID_BLOCK     VLT_ERROR( VLT_ISO9797_MAC, 11 )
#define SIGNER_DOF_NULL_PARAMS       VLT_ERROR( VLT_ISO9797_MAC, 12 )
#define SIGNER_DOF_INVALID_CAP       VLT_ERROR( VLT_ISO9797_MAC, 13 )
#define SIGNER_DOFSUP_NULL_PARAM     VLT_ERROR( VLT_ISO9797_MAC, 14 )
#define SIGNER_OPMODE_NOT_SUP        VLT_ERROR( VLT_ISO9797_MAC, 15 )
#define SIGNER_EMPT_DOFIN_NPAD       VLT_ERROR( VLT_ISO9797_MAC, 16 )


#define ST_UNKNOWN          0x00
#define ST_INITIALISED      0x10
#define ST_FINALISED        0x20


#define VLT_DES_IV_SIZE     0x08
#define VLT_AES_IV_SIZE     0x10
#define VLT_PAD_DONE        0x01
#define VLT_PAD_NOT_DONE    0x00


#if( ( VLT_ENABLE_CIPHER_DES == VLT_ENABLE ) && ( VLT_ENABLE_CIPHER_TDES == VLT_ENABLE ) )

/**
 * Private Data
 */
static VLT_U8 signerState = ST_UNKNOWN;
static SIGNER_PARAMS params = { 0 };
static CIPHER_PARAMS cipherParams = { 0 };
static KEY_BLOB theKey = { 0 };
static VLT_U8 aIv[VLT_AES_IV_SIZE];
static VLT_U32 blockCounter = 0;





/**
 * Private function prototypes
 */
VLT_STS DoFinalSetup( VLT_PU8 pIV, VLT_U8 IvLen );



VLT_STS SignerIso9797Init( VLT_U8 opMode, const KEY_BLOB* pKey, VLT_PU8 pParams )
{   
    VLT_STS status = VLT_FAIL;
            
    /**
     * Make sure we have a valid params pointers
     */
    if( ( NULL == pParams ) ||
        ( NULL == pKey ) )
    {
        return( INVALID_NULL_PARAMS );
    }
    else if ( VLT_SIGN_MODE != opMode )
    {
        return ( SIGNER_OPMODE_NOT_SUP );
    }
    else
    {
        /**
         * Cache the parameters.
         */
        params = *((SIGNER_PARAMS*)pParams);

        /**
         * Make sure we have a valid params pointers
         */
        if ( NULL == params.pIV )
        {
            return( INVALID_NULL_PARAMS );
        }

        if( ( params.algoID != VLT_ALG_SIG_MAC_ISO9797_ALG1 ) &&
            ( params.algoID != VLT_ALG_SIG_MAC_ISO9797_ALG3 ) )
        {
            /**
             * Clear the singerState to signify the
             * fact that something has gone pear
             * shaped and we shouldn't deligate
             * any further calls to the concrete
             * methods.
             */
            signerState = ST_UNKNOWN;

            /**
             * Return the appropriate error and
             * exit gracefully. 
             */
            return( SIGNER_ALGO_ID_NOT_SUP );
        }
    }
    
    /**
     * Save a copy of the key!
     * Mac Algo 3 and AES CMAC
     * require key switching 
     * in the final block!
     * This should only be done
     * on success but tweeking of
     * the key length is required
     * for Mac Alg3.
     */
    theKey = *pKey;

    /**
     * Check and setup the required parameters for 
     * the Chipher init method call.
     */
    switch(params.algoID)
    {
        case VLT_ALG_SIG_MAC_ISO9797_ALG1:
            cipherParams.algoID = VLT_ALG_CIP_TDES_2K_EDE;
            cipherParams.chainMode = BLOCK_MODE_CBC;
            /**
             * The signer has to do padding itself
             * because of Cipher mode switching 
             * in MAC algorithm 3, on the last block
             * if the Cipher does the padding, the 
             * last block, can become the second last
             * block, due to padding method 2, PKCS5 etc.
             * The result is the last two blocks are 
             * processed in the mode switch, and the 
             * wrong answer is produced.
             */
            cipherParams.paddingScheme = PADDING_NONE;
            break;          
        case VLT_ALG_SIG_MAC_ISO9797_ALG3:
            cipherParams.algoID = VLT_ALG_CIP_DES;
            cipherParams.chainMode = BLOCK_MODE_CBC;
            cipherParams.paddingScheme = PADDING_NONE;
            theKey.keyType = VLT_KEY_DES;
            theKey.keySize = DES_KEY_SIZE;
            break;          
        case VLT_ALG_SIG_CMAC_AES:            
        default:
            return( SIGNER_ALGO_ID_NOT_SUP );
    }

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
            return(SIGNER_PADDING_UNKNOWN);
    }       

    
    /*
     * Check the IV length is supported.
     */
    if( VLT_DES_IV_SIZE != params.ivSize )
    {
        return (SIGNER_IV_LEN_NOT_SUP);
    }

    /**
     * Store the address of the IV into the Cipher params IV ptr.
     */
    cipherParams.pIV = params.pIV;
    
    /**
     * Delegate the call to the initialisation method
     * to the cipher, and let the cipher use the appropriate algorithm.
     */
    status = CipherInit( VLT_ENCRYPT_MODE, &theKey, (VLT_PU8)&cipherParams );

    /**
     * Prepare to accept the first block 
     * of data.
     */
    if( VLT_OK == status )
    {
        signerState = ST_INITIALISED; 

        /**
         * Save a copy again!
         * Mac Algo 3 does a mixture 
         * of DES and TDSES so the 
         * keyLen has been modified
         * to DES_KEY_SIZE for the 
         * first part, TDES_KEY_SIZE
         * is needed for the final 
         * part!
         */
        theKey = *pKey;

        /**
         * Setup the IV in the IV buffer if the 
         * do final is called with a message 
         * shorter or equal to the block size.
         * In this case the cipher will be re-
         * initalized with a new algorithm.
         */
        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_memcpy(&aIv[0], &params.pIV[0], params.ivSize);

        /**
         * This is used to determine if do final 
         * was called empty and update has not 
         * been called (sign an empty message).
         */
        blockCounter = 0;
    }

    return( status );
}

VLT_STS SignerIso9797Close( void )
{
    if( ST_UNKNOWN != signerState )
    {
        signerState = ST_UNKNOWN;
        blockCounter = 0;
        return( CipherClose() );
    }
    return( SINGER_CLS_NOT_SUP );
}


VLT_STS SignerIso9797DoFinal(
    VLT_PU8 pMessage, 
    VLT_U32 messageLen, 
    VLT_U32 messageCapacity, 
    VLT_PU8 pMac, 
    VLT_PU32 pMacLen, 
    VLT_U32 macCapacity )
{
    VLT_STS status = VLT_FAIL;  
    VLT_U8 blockSize = 0;
    VLT_U32 byteCount = 0;
    VLT_U32 workingLen = 0;
    VLT_U8 paddingPerformed = VLT_PAD_NOT_DONE;
    VLT_PU8 pMessageIt = &pMessage[byteCount];
    VLT_U8 hasGrownByBlockSize = FALSE;
    /**
     * With padding the final block can be double the 
     * block size, so double the size of the buffer. 
     */
    VLT_U8 workingBuffer[VLT_AES_CMAC_LENGTH * 2]; 

    

    if ( ( ST_UNKNOWN == signerState ) ||
         ( ST_FINALISED == signerState ) )

    {
        return( SIGNER_NOT_SETUP ); 
    }


    /**
     * Ensure we haven't been passed
     * a null pointer by the caller.
     */
    if ( ( NULL == pMessage ) ||
         ( NULL == pMac )     ||
         ( NULL == pMacLen ) )
    {
        return( SIGNER_DOF_NULL_PARAMS );
    }


    /**
     * Cache the block size, we'll use it 
     * frequently.
     */
    workingLen = blockSize = (VLT_U8)CipherGetBlockSize( );

    
    /**
     * For the SignerIso9797Update the capacity of
     * the buffer passed to us by the caller
     * should be equal or larger than that
     * of the data buffer length.
     */
    if ( ( messageLen > messageCapacity ) ||
         ( messageLen == 0 )              ||
         ( blockSize > macCapacity) )
    {
        return( SIGNER_DOF_INVALID_CAP );
    }

    
    if ( ( blockCounter == 0 )      &&
         ( messageLen < blockSize ) &&
         ( params.paddingScheme == PADDING_NONE ) )
    {
        /**
         * Can not process an empty do final 
         * with no padding.
         */
        return ( SIGNER_EMPT_DOFIN_NPAD );
    }

    while( 0 != ( messageLen - byteCount ) )
    {
        /**
         * The cipher requires the output buffer
         * to be as big as the message buffer, which 
         * this method has no control over, so the signer
         * will send BLOCK_SIZED chunks to the cipher
         * and discard the answer for each block
         * until the last block in do final is processed.
         */        
        if ( ( messageLen - byteCount ) <= blockSize )
        {
            if ( messageLen > blockSize )
            {
                /**
                 * Copy the last block, to setup the IV for the next if 
                 * MAC algo 1 is being used.
                 */
                /*
                * No need to check the return type as pointer has been validated
                */
                (void)host_memcpy( &aIv[0], &workingBuffer[0], blockSize );
            }
            
            /**
             * Check if Padding is needed
             */
            if ( ( PADDING_NONE != params.paddingScheme ) &&
                 ( VLT_PAD_DONE != paddingPerformed ) )
            {
                /**
                 * Working in a small internal buffer so
                 * the current count (byteCount) has to be 
                 * removed from the message size to ensure
                 * the correct sizes are passed to padding
                 * to ensure padding thinks there is enough
                 * room in the working buffer.  The working
                 * length is left so the correct number of 
                 * remaining bytes are copied.
                 */
                messageLen = workingLen = messageLen - byteCount;
              
                if ( VLT_OK != ( status = PaddingAdd( 
                    params.paddingScheme, 
                    blockSize, 
                    &workingBuffer[0],
                    &messageLen, /* Used because of the copying blockSize bytes at a time. */
                    NELEMS(workingBuffer) ) ) )
                {
                    return ( status );
                }
                /**
                 * Add the padding length onto the message.
                 */
                messageLen += byteCount;
                

                paddingPerformed = VLT_PAD_DONE;
            }

            /**
             * Padding can make the remaining bytes double
             * and result in two blocks remaining to be signed
             * Re-test the message length and take different 
             * action if the message length has grown such 
             * another block is required.
             */
            if ( ( messageLen - byteCount ) <= blockSize )
            {
                signerState = ST_FINALISED;

                /**
                 * Get the cipher to perform a different 
                 * action on do final.  Mac Alg3 switchs from DES to
                 * TDES on the do final, and AES_CMAC can use one of
                 * two different session keys on final!
                 */
                DoFinalSetup ( aIv, blockSize );

                
                if ( ( VLT_PAD_DONE == paddingPerformed ) &&
                     ( hasGrownByBlockSize ) )
                {
                    /**
                    * Setup the the latst block copy from the end 
                    * of the buffer to the start, no needed
                    * but simpler than more logic later.
                    */
                    pMessageIt = &workingBuffer[blockSize];
                }
            }
            else
            {
                /**
                 * Padding has added block size
                 * more bytes so another round 
                 * is required.
                 */
                 hasGrownByBlockSize = TRUE;
            }
        }
        
        
        
        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_memcpy(&workingBuffer[0], pMessageIt, workingLen);
        

        if ( ST_FINALISED != signerState )
        {
            if( VLT_OK == ( status = CipherUpdate(
                workingBuffer,
                blockSize,
                messageCapacity,
                workingBuffer,
                &workingLen,
                NELEMS(workingBuffer)) ) )
            {
                /**
                 * It is possible for the cipher 
                 * to return a length twice 
                 * the blockSize, or the blockSize, 
                 * if it is any other value exit 
                 * with an appropriate error code.
                 */
                if ( ( workingLen != blockSize ) &&
                     ( workingLen != ( blockSize * 2 ) ) )
                {               
                    return( SIGNER_UPD_INVALID_BLOCK );
                }                   
            }
            else
            {
                return( status );
            }
        }
        else if ( ST_FINALISED == signerState )
        {
            if( VLT_OK == ( status = CipherDoFinal(
                workingBuffer,
                blockSize,
                messageCapacity,
                workingBuffer,
                &workingLen,
                NELEMS(workingBuffer)) ) )
            {
                /**
                 * It is possible for the cipher 
                 * to return a length twice 
                 * the blockSize, or the blockSize, 
                 * if it is any other value exit 
                 * with an appropriate error code.
                 */
                if ( ( workingLen != blockSize ) &&
                     ( workingLen != ( blockSize * 2 ) ) )
                {               
                    return( SIGNER_UPD_INVALID_BLOCK );
                }                   
            }
            else
            {
                return( status );
            }
            
            if ( workingLen == ( blockSize * 2 ) )
            {
                workingLen = blockSize;
            }
            else
            {
                workingLen = 0;
            }
            /**
             * Take a block sized chunk and 
             * copy it into the output buffer.
             */
            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memcpy(pMac, &workingBuffer[workingLen], blockSize);
            
            /**
             * Set the length of the Mac
             */
            *pMacLen = blockSize;

            break;
        }
        
        /**
         * Add block size to the byte count and 
         * increment the message ptr by block size.
         */
        byteCount += blockSize;
        pMessageIt += blockSize;
        blockCounter++;
    }
    return ( VLT_OK );
}

VLT_STS SignerIso9797Update( VLT_PU8 pMessage, VLT_U32 messageLen, VLT_U32 messageCapacity )
{
    VLT_STS status = VLT_FAIL;
    VLT_U8 blockSize = 0;
    VLT_U32 byteCount = 0;
    VLT_U32 workingLen = 0;
    VLT_U8 workingBuffer[VLT_AES_CMAC_LENGTH];

    if ( ( ST_UNKNOWN == signerState ) ||
         ( ST_FINALISED == signerState ) )
    {
        return( SIGNER_NOT_SETUP );
    }

    
    /**
     * Ensure we haven't been passed
     * a null pointer by the caller.
     */
    if ( NULL == pMessage )
    {
        return( SIGNER_UPD_NULL_PARAMS );
    }


    /**
     * Cache the block size, we'll use it 
     * frequently.
     */
    blockSize = (VLT_U8)CipherGetBlockSize( );


    /**
     * For the SignerIso9797Update the capacity of
     * the buffer passed to us by the caller
     * should be equal or larger than that
     * of the data buffer length.
     */
    if ( ( messageLen > messageCapacity ) ||
         ( messageLen < blockSize )      ||
         ( messageCapacity < blockSize ) )
    {
        return( SIGNER_UPD_INVALID_CAP );
    }


    /**
     * Update only deals with data lengths
     * multiple of the block size, if the 
     * client has passed us anything else 
     * other than that then we should exit
     * gracefully-ish!
     */
    if( 0 != ( messageLen % blockSize ) )
    {
        return( SIGNER_UPD_NON_MUL_BLK_SZ );
    }
    

    while( 0 != ( messageLen - byteCount ) )
    {
        /**
         * Right, the cipher requires the output buffer
         * to be as big as the message buffer, which 
         * this method has no control over, so the signer
         * will send BLOCK_SIZED chunks to the cipher
         * and discard the answer for each block
         * until the last block in do final is processed.
         */
        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_memcpy(&workingBuffer[0], &pMessage[byteCount], blockSize);

        if( VLT_OK == ( status = CipherUpdate(
            workingBuffer,
            blockSize,
            messageCapacity,
            workingBuffer,
            &workingLen,
            NELEMS(workingBuffer)) ) )
        {
            /**
             * It should be impossible for the cipher 
             * to return a length not equal to 
             * the blockSize, nevertheless if it does
             * exit with an appropriate error code.
             */
            if( workingLen != blockSize )
            {               
                return( SIGNER_UPD_INVALID_BLOCK );
            }                   
        }
        else
        {
            return( status );
        }
        
        byteCount += blockSize;
        blockCounter++;
    }

    /**
     * Setup the IV in the IV buffer if the 
     * do final is called with a message 
     * shorter or equal to the block size.
     * In this case the cipher will be re-
     * initalized with a new algorithm.
     */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy(&aIv[0], &workingBuffer[0], params.ivSize);

    return( VLT_OK );
}

VLT_U16 SignerIso9797GetBlockSize( void )
{
    if( ST_UNKNOWN != signerState )
    {
        return( CipherGetBlockSize() );
    }
    return( SIGNER_GMS_BLK_SZ_NOT_SUP );
}


VLT_STS DoFinalSetup( VLT_PU8 pIV, VLT_U8 IvLen )
{
    VLT_U16 status = VLT_FAIL;
    
    /**
     * Check the IV ptr is not null
     */
    if ( NULL == pIV )
    {
        return ( SIGNER_DOFSUP_NULL_PARAM );
    }

    switch (params.algoID)
    {
        case VLT_ALG_SIG_MAC_ISO9797_ALG1:
            /* No setup is required */
            break;          
        case VLT_ALG_SIG_MAC_ISO9797_ALG3:
            if (VLT_OK != ( status = CipherClose() ) )
            {
                return ( status );
            }
            /**
             * The final step for a MAC Algo 3 is a
             * switch to TDES! So re-init the Cipher.
             */
            cipherParams.algoID = VLT_ALG_CIP_TDES_2K_EDE;
            cipherParams.pIV = pIV;
            
            if ( VLT_OK != (status = CipherInit( 
                VLT_ENCRYPT_MODE, 
                &theKey, 
                (VLT_PU8)&cipherParams ) ) )
            {
                return ( status );
            }

            /**
             * Job done, the final block should now perform 
             * a 3DES using the same both parts of the 2K
             * DES Key. 
             * (k1 DES) -> (k2 DES-1) -> (k1 DES) = MAC
             */
            break;          
        case VLT_ALG_SIG_CMAC_AES:            
        default:
            return( SIGNER_ALGO_ID_NOT_SUP );
    }

    return ( VLT_OK );
}

#endif /* ( ( VLT_ENABLE_CIPHER_DES == VLT_ENABLE ) && ( VLT_ENABLE_CIPHER_TDES == VLT_ENABLE ) ) */
