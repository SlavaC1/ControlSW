/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#include "vaultic_common.h"
#include "vaultic_mem.h"
#include "string.h"


/**
 * Error Codes
 */
#define EMEMCPYNULLPARAMS      VLT_ERROR( VLT_MEM, 0 )
#define EMEMCMPNULLPARAMS      VLT_ERROR( VLT_MEM, 1 )
#define EMEMXORNULLPARAMS      VLT_ERROR( VLT_MEM, 2 )
#define EMEMCPYXORNULLPARAMS   VLT_ERROR( VLT_MEM, 3 )
#define EMEMLSHFTNULLPARAMS    VLT_ERROR( VLT_MEM, 4 )

/**
 * Private Macros
 */
#define BITS_PER_BYTE             (VLT_U8)0x08

VLT_STS host_memcpy(VLT_PU8 dest, const VLT_U8 *src, VLT_U32 len)
{
    /*
    * Check that the pointers are valid
    */
    if( (NULL == dest) || ( NULL == src ) )
    {
        return( EMEMCPYNULLPARAMS );
    }

    memcpy( (void*)dest, (const void*)src, (size_t)len );

    return( VLT_OK );
}

/**
* Sets len bytes in "src" buffer to the specified value.
* \todo To be implemented by the target platform.
*/
VLT_STS host_memset(VLT_PU8 dest, VLT_U8 value, VLT_U32 len)
{
    /*
    * Check that the pointer is valid
    */
    if( NULL == dest )
    {
        return( EMEMCMPNULLPARAMS );
    }

    memset( (void*)dest, (int)value, (size_t)len );

    return( VLT_OK );
}

/**
* Compares len bytes between "src1" and "src2" buffers.
* \retval 0 if buffers are equal
* \retval !=0 if buffers are different
* \todo To be implemented by the target platform.
*/
VLT_STS host_memcmp(const VLT_U8 *src1, const VLT_U8 *src2, VLT_U32 len)
{
    return( (VLT_STS)memcmp( (const void*)src1, 
        (const void*)src2, (size_t)len ) );
}

VLT_STS host_memxor(VLT_PU8 dest, const VLT_U8 *src, VLT_U32 len)
{
    VLT_U32 i;

    /*
    * Check that the pointers are valid
    */
    if( (NULL == dest) || ( NULL == src ) )
    {
        return( EMEMXORNULLPARAMS );
    }

    for( i = 0; i < len; i++ )
    {
        dest[i] ^= src[i];
    }
    
    return( VLT_OK );
}

VLT_STS host_memcpyxor(VLT_PU8 dest, const VLT_U8 *src, VLT_U32 len, VLT_U8 mask)
{
    /*
    * Check that the pointers are valid
    */
    if( (NULL == dest) || ( NULL == src ) )
    {
        return( EMEMCPYXORNULLPARAMS );
    }

    /* If the mask is zero then call host_memcpy in the expectation that it
     * will be quicker. */
    if (mask == 0)
    {
        return host_memcpy(dest, src, len);
    }

    while (len--)
    {
        *dest++ = *src++ ^ mask;
    }

    return( VLT_OK );
}

VLT_STS host_lshift( VLT_PU8 arrayIn, VLT_U32 arrayInLen, VLT_U8 bitsToShift )
{
    VLT_U32 i = 0;
    VLT_U8 v = ( BITS_PER_BYTE - bitsToShift );

    /*
    * Check that the pointer is valid
    */
    if( NULL == arrayIn )
    {
        return( EMEMLSHFTNULLPARAMS );
    }

    if( BITS_PER_BYTE < bitsToShift )
    {
        return( VLT_FAIL );
    }

    for( i = 0; i < ( arrayInLen -1 ); i++ )
    {
        arrayIn[i] <<= bitsToShift;
        arrayIn[i] |= ( arrayIn[i+1] >> v );
    }

    arrayIn[i++] <<= bitsToShift;

    return( VLT_OK );
}
