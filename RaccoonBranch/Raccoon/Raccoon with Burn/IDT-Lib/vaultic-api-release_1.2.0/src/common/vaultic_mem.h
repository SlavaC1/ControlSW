/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_mem.h
 * 
 * \brief Interface to host memory functions.
 * 
 * \par Description:
 * The VaultIC API calls these functions instead of directly calling memcpy,
 * memset and memcmp to allow the customer to provide alternative
 * implementations.
 */

#ifndef VAULTIC_MEM_H
#define VAULTIC_MEM_H

/**
* Copies len bytes from "src" to "dest" buffer.
* \todo To be implemented by the target platform.
*/
VLT_STS host_memcpy(VLT_PU8 dest, const VLT_U8 *src, VLT_U32 len);

/**
* Sets len bytes in "src" buffer to the specified value.
* \todo To be implemented by the target platform.
*/
VLT_STS host_memset(VLT_PU8 dest, VLT_U8 value, VLT_U32 len);

/**
* Compares len bytes between "src1" and "src2" buffers.
* \retval 0 if buffers are equal
* \retval !=0 if buffers are different
* \todo To be implemented by the target platform.
*/
VLT_STS host_memcmp(const VLT_U8 *src1, const VLT_U8 *src2, VLT_U32 len);

/**
*  XORs  "dest" and "src" buffers and places the contents in the dest buffer.
* \retval VLT_OK if successful otherwise an appropriate error code
* \todo To be implemented by the target platform.
*/
VLT_STS host_memxor(VLT_PU8 dest, const VLT_U8 *src, VLT_U32 len);

/**
 * \fn host_memcpyxor(VLT_PU8 dest, const VLT_U8 *src, VLT_U32 len, VLT_U8 mask)
 * \brief memcpy with bytewise exclusive or.
 * \todo To be implemented by the target platform.
 */
VLT_STS host_memcpyxor(VLT_PU8 dest, const VLT_U8 *src, VLT_U32 len, VLT_U8 mask);

/**
 * Left bit shifts the "arrayIn" the number of bit positions specified by "bitsToShift".
 * \param arrayIn holds a pointer to the array that will be shifted, the shifting is 
 * done in place. 
 * \param arrayInLen the length of the array passed in. 
 * \param bitsToShift the number of bits to left shift, it cannot exceed 8.
 * \retval If successful it return VLT_OK otherwise VLT_FAIL.
 * \todo To be implemented by the target platform.
 */
VLT_STS host_lshift( VLT_PU8 arrayIn, VLT_U32 arrayInLen, VLT_U8 bitsToShift );

#endif/*VAULTIC_MEM_H*/
