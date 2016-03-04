/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#include "vaultic_common.h"
#include "vaultic_padding.h"
#include "vaultic_mem.h"

/**
 * Error Codes
 */
#define EPDDADDPADUNKNOWN       VLT_ERROR( VLT_PADDING, 0 )
#define EPDDRMVPADUNKNOWN       VLT_ERROR( VLT_PADDING, 1 )
#define EPDDADDBFTOOSMALL       VLT_ERROR( VLT_PADDING, 2 )
#define EPDDADDINVLDPARAMS      VLT_ERROR( VLT_PADDING, 3 )
#define EPDDRMVINVLDPARAMS      VLT_ERROR( VLT_PADDING, 4 )
#define EPDDRMVFRMTISO9797      VLT_ERROR( VLT_PADDING, 5 )
#define EPDDRMVIVLDISO9797      VLT_ERROR( VLT_PADDING, 6 )
#define EPDDRMVIVLDPKCS5        VLT_ERROR( VLT_PADDING, 7 )
#define EPDDRMVIVLDPKCS7        VLT_ERROR( VLT_PADDING, 8 )
#define EPDDADDINVLDBLKSZ       VLT_ERROR( VLT_PADDING, 9 )
#define EPDDADDINVLDCASE        VLT_ERROR( VLT_PADDING, 10 )
#define EPDDRMVZEROLEN          VLT_ERROR( VLT_PADDING, 11 )
#define EPDDRMVINVLDBLKSIZE     VLT_ERROR( VLT_PADDING, 12 )
#define EPDDRMVNOTBLKSIZE       VLT_ERROR( VLT_PADDING, 13 )


VLT_STS PaddingAdd( VLT_U8 paddingMode, VLT_U16 blockSize, VLT_PU8 pData, VLT_PU32 pDataLen,
    VLT_U32 bufferCapacity )
{
    VLT_U8 paddingSize = 0;

    /**
     * Ensure we haven't been passed any null pointers.
     */
    if( ( NULL == pData ) ||
        ( NULL == pDataLen ) )
    {
        return( EPDDADDINVLDPARAMS );
    }

    /**
     * The block size can't be zero
     */
    if( 0 == blockSize )
    {
        return( EPDDADDINVLDBLKSZ );
    }

    /**
     * Determine how much padding we need.
     */
    paddingSize = (VLT_U8)( blockSize - (*pDataLen % blockSize ) );

    /**
     * Make sure we have enough space to add the padding.
     */
    if( (paddingSize + *pDataLen) > bufferCapacity )
    {
        return( EPDDADDBFTOOSMALL );
    }

    switch( paddingMode )
    {   
        case PADDING_ISO9797_METHOD2:
            /**
             * If the padding size is zero then the padding required is
             * equal to the block size.
             */
            if( 0 == paddingSize )
            {
                paddingSize = (VLT_U8)blockSize;
            }
            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memset( &pData[*pDataLen], 0x00, paddingSize );
            pData[*pDataLen] = PADDING_METHOD2_MARK;            
            break;
        case PADDING_NONE:
            /**
             * We have been asked not to pad something that
             * doesn't alging to the block size specified.
             * Data blocks passed to a cipher should match
             * the block size.
             */
            if( 0 != paddingSize )
            {
                paddingSize = 0;
                return( EPDDADDINVLDCASE );
            }
            paddingSize = 0;
            break;
        case PADDING_PKCS5:
        case PADDING_PKCS7:
            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memset( &pData[*pDataLen], paddingSize, paddingSize );         
            break;
        default:
            return(EPDDADDPADUNKNOWN);
    }           

    /**
     * Update the length
     */
    *pDataLen += paddingSize;

    return( VLT_OK );
}

VLT_STS PaddingRemove( VLT_U8 paddingMode, VLT_U16 blockSize, VLT_PU8 pData, VLT_PU32 pDataLen )
{

    VLT_U8 paddingSize = 0;
    VLT_U8 paddingValue = 0;    

    /**
     * Ensure we haven't been passed any null pointers.
     */
    if( ( NULL == pData ) ||
        ( NULL == pDataLen ) )
    {
        return( EPDDRMVINVLDPARAMS );
    }

    /**
     * The data length passed can't be zero or less
     * that the block size.
     */
    if( ( 0 == *pDataLen ) ||
        ( blockSize > *pDataLen ) )
    {
        return( EPDDRMVZEROLEN );
    }

    /**
     * The blockSize can't be zero!
     */
    if( 0 == blockSize )
    {
        return( EPDDRMVINVLDBLKSIZE );
    }

    /**
     * The data Length passed must be multiple of
     * the block size.
     */
    if( 0 != ( *pDataLen % blockSize ) )
    {
        return( EPDDRMVNOTBLKSIZE );
    }

    switch( paddingMode )
    {   
        case PADDING_ISO9797_METHOD2:
            
            paddingValue = pData[ ( *pDataLen - 1 ) ];

            if( PADDING_METHOD2_MARK == paddingValue )
            {
                paddingSize = 1;
            }
            else if( 0 == paddingValue )
            {

                /**
                 * Keep looking backwards until you find
                 * the 0x80 marker or until we have looked at
                 * enough bytes to cover a block size length.
                 */
                while( ++paddingSize <= blockSize )
                {
                    /**
                     * The only values expected are 0s and 0x80 if anything
                     * else if found we need to return an error.
                     */
                    paddingValue = pData[ (*pDataLen - paddingSize) ];

                    if( PADDING_METHOD2_MARK == paddingValue )
                    {
                        break;
                    }
                    else if ( 0 == paddingValue )
                    {                       
                        continue;
                    }
                    else
                    {
                        return( EPDDRMVFRMTISO9797 );
                    }
                }
            }
            else
            {
                return( EPDDRMVIVLDISO9797 );
            }
            
            break;
        case PADDING_NONE:
            break;
        case PADDING_PKCS5:
            paddingSize = pData[ ( *pDataLen - 1) ];
            /**
             * PKCS5 always pads and llegedly the padding value
             * shouldn't be more than 8.
             */
            if( ( MAX_PKCS5_PAD_SZ < paddingSize  ) && ( 0 == paddingSize ) )
            {
                return( EPDDRMVIVLDPKCS5 );
            }

            break;
        case PADDING_PKCS7:
            paddingSize = pData[ ( *pDataLen - 1) ];
            /**
             * PKCS7 always pads
             */
            if( 0 == paddingSize )
            {
                return( EPDDRMVIVLDPKCS7 );
            }
            break;
        default:
            return(EPDDRMVPADUNKNOWN);
    }

    /**
     * Update the length to reflect the removal
     * of the padding.
     */
    *pDataLen -= paddingSize;

    return( VLT_OK );
}

#if( VLT_ENABLE_CIPHER_TESTS == VLT_ENABLE )

VLT_STS PaddingTests( void )
{
    /**
    * "The Brown Fox has Jumped the Fence!"
    */
    VLT_U8 vector1[] =
    {
        0x54, 0x68, 0x65, 0x20, 0x42, 0x72, 0x6f, 0x77,
        0x6e, 0x20, 0x46, 0x6f, 0x78, 0x20, 0x68, 0x61,
        0x73, 0x20, 0x4a, 0x75, 0x6d, 0x70, 0x65, 0x64,
        0x20, 0x74, 0x68, 0x65, 0x20, 0x46, 0x65, 0x6e,
        0x63, 0x65, 0x21
    };

    VLT_U32 vector1Size = 35;

    VLT_U8 vector1Method2[] =
    {
        0x54, 0x68, 0x65, 0x20, 0x42, 0x72, 0x6f, 0x77,
        0x6e, 0x20, 0x46, 0x6f, 0x78, 0x20, 0x68, 0x61,
        0x73, 0x20, 0x4a, 0x75, 0x6d, 0x70, 0x65, 0x64,
        0x20, 0x74, 0x68, 0x65, 0x20, 0x46, 0x65, 0x6e,
        0x63, 0x65, 0x21, 0x80, 0x00, 0x00, 0x00, 0x00
    };
    VLT_U32 vector1Method2Size = 40;
    
    VLT_U8 vector1PKCS7[] =
    {
        0x54, 0x68, 0x65, 0x20, 0x42, 0x72, 0x6f, 0x77,
        0x6e, 0x20, 0x46, 0x6f, 0x78, 0x20, 0x68, 0x61,
        0x73, 0x20, 0x4a, 0x75, 0x6d, 0x70, 0x65, 0x64,
        0x20, 0x74, 0x68, 0x65, 0x20, 0x46, 0x65, 0x6e,
        0x63, 0x65, 0x21, 0x05, 0x05, 0x05, 0x05, 0x05
    };
    VLT_U8 vector1PKCS7Size = 40;

    VLT_U8 text[40];
    VLT_U32 textSize = vector1Size;

    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memset( text, 0x00,  textSize );
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( text, vector1, textSize );
    
    /**
     * The buffer doesn't have enough capacity.
     */
    if( VLT_OK == PaddingAdd( PADDING_ISO9797_METHOD2, 8, text, &textSize, textSize ) )
    {
        return( VLT_FAIL );
    }

    /**
     * The padding method shouldn't be recognised.
     */
    if( VLT_OK == PaddingAdd( 20, 8, text, &textSize, sizeof(text)/sizeof(VLT_U8 ) ) )
    {
        return( VLT_FAIL );
    }

    /**
     * The text pointer is null
     */
    if( VLT_OK == PaddingAdd( PADDING_NONE, 8, 0, &textSize, textSize ) )
    {
        return( VLT_FAIL );
    }

    /**
     * The text length pointer is null
     */
    if( VLT_OK == PaddingAdd( PADDING_NONE, 8, text, 0, textSize ) )
    {
        return( VLT_FAIL );
    }

    /**
     * All ok the padding method 2 vector should match
     */
    if( VLT_OK != PaddingAdd( PADDING_ISO9797_METHOD2, 8, text,
        &textSize, sizeof(text)/sizeof(VLT_U8 ) ) )
    {
        return( VLT_FAIL );
    }
    if( textSize != vector1Method2Size )
    {
        return( VLT_FAIL );
    }
    if( 0 != host_memcmp( text, vector1Method2, vector1Method2Size ) )
    {
        return( VLT_FAIL );
    }
    if( VLT_OK != PaddingRemove( PADDING_ISO9797_METHOD2, 8, text, &textSize ) )
    {
        return( VLT_FAIL );
    }
    if( textSize != vector1Size )
    {
        return( VLT_FAIL );
    }
    if( 0 != host_memcmp( text, vector1, vector1Size ) )
    {
        return( VLT_FAIL );
    }

    /**
     * All ok the PKCS7 vector should match
     */
    if( VLT_OK != PaddingAdd( PADDING_PKCS7, 8, text,
        &textSize, sizeof(text)/sizeof(VLT_U8 ) ) )
    {
        return( VLT_FAIL );
    }
    if( textSize != vector1PKCS7Size )
    {
        return( VLT_FAIL );
    }
    if( 0 != host_memcmp( text, vector1PKCS7, vector1PKCS7Size ) )
    {
        return( VLT_FAIL );
    }
    if( VLT_OK != PaddingRemove( PADDING_PKCS7, 8, text, &textSize ) )
    {
        return( VLT_FAIL );
    }
    if( textSize != vector1Size )
    {
        return( VLT_FAIL );
    }
    if( 0 != host_memcmp( text, vector1, vector1Size ) )
    {
        return( VLT_FAIL );
    }

    return( VLT_OK );
}

#endif /* ( VLT_ENABLE_CIPHER_TESTS == VLT_ENABLE ) */
