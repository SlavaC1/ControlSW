/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_crc16.h
 * 
 * \brief Provides simple implementation of a CRC16 CCITT
 * 
 * \par Description:
 * This file provides an implementation of a CRC16 CCITT.
 *
 * \todo 
 * Extract and provide a simple function based interface to allow the customer
 * to provide their own CRC implementation should they want to.
 */

#ifndef __VAULTIC_CRC16_H__
#define __VAULTIC_CRC16_H__

#include "vaultic_typedefs.h"
#define NUM_CRC_BYTES      (VLT_U8)0x02

/**
 * \fn VltCrc16Block(VLT_U16 u16Crc, 
 *         const VLT_U8 *pu8Block,
 *         VLT_U16 u16Length)
 *
 * \brief Calculates the CRC16 of the input block.
 *
 * \param[in] u16Crc    Old CRC16, or VLT_CRC16_CCIT_INIT to begin.
 * \param[in] pu8Block  Block of bytes to CRC.
 * \param[in] u16Length Length of block.
 *
 * \return New CRC16.
 */
VLT_U16 VltCrc16Block(VLT_U16 u16Crc, 
    const VLT_U8 *pu8Block,
    VLT_U16 u16Length);

/**
 * \fn VltCrc16( VLT_PU16 pu16Crc, 
 *         const VLT_U8 *pu8Block,
 *         VLT_U16 u16Length)
 *
 * \brief Calculates the CRC16 of the input block.
 *
 * \param[in] pu16Crc   Pointer to the Old CRC16, or to the initialised value.
 *
 * \param[in] pu8Block  Block of bytes to CRC.
 *
 * \param[in] u16Length Length of block.
 *
 * \return VLT_OK if successful otherwise an appropriate error code.
 */
VLT_STS VltCrc16( VLT_PU16 pu16Crc, 
    const VLT_U8 *pu8Block,
    VLT_U16 u16Length);

#endif/*__VAULTIC_CRC16_H__*/
