/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_options.h
 * 
 * \brief Customer configuration options for the VaultIC API.
 * 
 * \par Description:
 * This file presents the options which are available to the client when
 * configuring the VaultIC API.
 */

#ifndef VAULTIC_OPTIONS_H
#define VAULTIC_OPTIONS_H

/*
* ------------------
* Endianness Options
* ------------------
*
* Please set the VLT_ENDIANNESS macro in vaultic_config.h.
*/
#define VLT_LITTLE_ENDIAN           0
#define VLT_BIG_ENDIAN              1

/**
 * ------------------
 * Platform Options
 * ------------------
 *
 * Please set the VLT_PLATFORM macro in vaultic_config.h.
 */
#define VLT_WINDOWS                 0
#define VLT_LINUX                   1
#define VLT_MAC_OS                  2
#define VLT_EMBEDDED                3

/**
 * Vault IC version macros
 */
#define VAULTIC_VERSION_1_0_X       0x01

#define VAULTIC_VERSION_1_2_X       0x02
#define VAULTIC_VERSION_1_2_0       0x02
#define VAULTIC_VERSION_1_2_1       0x06


/** 
 * Conditional Compilation Options
 */
#define VLT_ENABLE                  1   
#define VLT_DISABLE                 0   

/**
 * Vault IC target macros
 */
#define VAULTIC400					1
#define VAULTIC100					2

#endif /*VAULTIC_OPTIONS_H*/

