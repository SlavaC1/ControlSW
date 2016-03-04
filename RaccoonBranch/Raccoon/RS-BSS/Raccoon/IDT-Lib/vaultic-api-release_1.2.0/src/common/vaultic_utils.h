/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_utils.h
 * 
 * \brief Interface to utility functions used by the VaultIC API.
 * 
 * \par Description:
 * This file declares utility functions used by the VaultIC API.
 */

#ifndef VAULTIC_UTILS_H
#define VAULTIC_UTILS_H

#include "vaultic_config.h"

/**
 * \fn VltEndianReadPU16(const VLT_U8 *p)
 * \brief Reads a big endian U16 from the specified pointer.
 */
VLT_U16 VltEndianReadPU16(const VLT_U8 *p);

/**
 * \fn VltEndianReadPU32(const VLT_U8 *p)
 * \brief Reads a big endian U32 from the specified pointer.
 */
VLT_U32 VltEndianReadPU32(const VLT_U8 *p);


/**
 * \macro NELEMS
 * \brief Returns the number of elements in an array.
 */
#define NELEMS(array) ((VLT_U32)(sizeof(array) / sizeof(array[0])))


/**
 * \macro WRAPPED_BYTE
 * \brief To fit in a byte, lengths of 256 must be encoded as zero.
 */
#define WRAPPED_BYTE(b) ((VLT_U8) ((b) & 0xFF))

/**
 * \macro UNWRAPPED_BYTE
 * \brief Reverses the action of WRAPPED_BYTE.
 */
#define UNWRAPPED_BYTE(b) (((b) != 0) ? (b) : 0x100)


/**
 * \macro LIN
 * \brief No-op macro for source level documentation purposes. Signifies that
 * the P3 argument is LIN.
 */
#define LIN(b) (b)

/**
 * \macro LEXP
 * \brief No-op macro for source level documentation purposes. Signifies that
 * the P3 argument is LEXP.
 */
#define LEXP(b) (b)

VLT_U16 NumBytesInBuffer( VLT_U16 u16Idx );

VLT_U16 NumBufferBytesAvail( VLT_U16 u16MaxBytes, VLT_U16 u16Idx );


#endif /*VAULTIC_UTILS_H*/

