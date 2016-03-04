/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_config.h
 * 
 * \brief Customer configuration for VaultIC API.
 * 
 * \par Description:
 * The customer should alter this file to customise the VaultIC API to match
 * their host platform. The available options are declared in vaultic_options.h.
 */

#ifndef VAULTIC_CONFIG_H
#define VAULTIC_CONFIG_H

#include "vaultic_options.h"


#define VLT_ENDIANNESS      VLT_BIG_ENDIAN

#ifdef WIN32
    #define VLT_PLATFORM        VLT_WINDOWS
#elif defined( __linux__ )
    #define VLT_PLATFORM        VLT_LINUX
#elif defined( __APPLE__ )
    #define VLT_PLATFORM        VLT_MAC_OS
#else
    #define VLT_PLATFORM        VLT_EMBEDDED
#endif




/**
 * #undef the following to inhibit the relevant code from being built into the
 * VaultIC API.
 */
#if( ( VLT_PLATFORM == VLT_WINDOWS  ) || ( VLT_PLATFORM == VLT_LINUX ) || ( VLT_PLATFORM == VLT_MAC_OS )  )

    #define VLT_ENABLE_KEY_SECRET         VLT_DISABLE
    #define VLT_ENABLE_KEY_HOTP           VLT_DISABLE
    #define VLT_ENABLE_KEY_TOTP           VLT_DISABLE
    #define VLT_ENABLE_KEY_RSA            VLT_DISABLE
    #define VLT_ENABLE_KEY_DSA            VLT_DISABLE
    #define VLT_ENABLE_KEY_ECDSA          VLT_ENABLE
    #define VLT_ENABLE_KEY_HOST_DEVICE_ID VLT_ENABLE
    #define VLT_ENABLE_CIPHER_TESTS       VLT_DISABLE
    /* 
     * Only one IDENTITY_AUTH can be used, use 
     * VLT_ENABLE_IDENTITY_AUTH2 as the 
     * interface is extensible.
     */
    #define VLT_ENABLE_IDENTITY_AUTH      VLT_DISABLE
    #define VLT_ENABLE_IDENTITY_AUTH_V2   VLT_ENABLE    
    #define VLT_ENABLE_SCP02              VLT_DISABLE
    #define VLT_ENABLE_SCP03              VLT_DISABLE
    #define VLT_ENABLE_MS_AUTH            VLT_DISABLE    
    #define VLT_ENABLE_STRONG_AUTH        VLT_ENABLE
    #define VLT_ENABLE_BLOCK_PROTOCOL     VLT_ENABLE
    #define VLT_ENABLE_ISO7816            VLT_DISABLE
    #define VLT_ENABLE_TWI                VLT_ENABLE
    #define VLT_ENABLE_SPI                VLT_DISABLE
    #define VLT_ENABLE_CIPHER_AES         VLT_DISABLE
    #define VLT_ENABLE_CIPHER_TDES        VLT_DISABLE
    #define VLT_ENABLE_CIPHER_DES         VLT_DISABLE
    #define VLT_ENABLE_FAST_CRC16CCIT     VLT_ENABLE
    #define VLT_ENABLE_KEY_WRAPPING       VLT_DISABLE
    #define VLT_ENABLE_FILE_SYSTEM        VLT_DISABLE
    #define VLT_ENABLE_AARDVK_SPPRSS_ERR  VLT_DISABLE
    #define VLT_ENABLE_ECDSA_SIGNER       VLT_ENABLE
    #define VLT_ENABLE_ECDSA_K233         VLT_ENABLE
    #define VLT_ENABLE_ECDSA_K283         VLT_ENABLE
    #define VLT_ENABLE_ECDSA_B233         VLT_ENABLE
    #define VLT_ENABLE_ECDSA_B283         VLT_ENABLE
    #define VLT_ENABLE_MULTI_SLOT         VLT_DISABLE
    #define VLT_ENABLE_AARDVARK           VLT_ENABLE
    #define VLT_ENABLE_CHEETAH            VLT_DISABLE
	#define VLT_ENABLE_CHIP_DEACTIVATION  VLT_DISABLE	
	#define VLT_ENABLE_SECURE_COUNTERS	  VLT_ENABLE
#else

    #define VLT_ENABLE_KEY_SECRET         VLT_DISABLE
    #define VLT_ENABLE_KEY_HOTP           VLT_DISABLE
    #define VLT_ENABLE_KEY_TOTP           VLT_DISABLE
    #define VLT_ENABLE_KEY_RSA            VLT_DISABLE
    #define VLT_ENABLE_KEY_DSA            VLT_DISABLE
    #define VLT_ENABLE_KEY_ECDSA          VLT_DISABLE
    #define VLT_ENABLE_KEY_HOST_DEVICE_ID VLT_DISABLE
    #define VLT_ENABLE_CIPHER_TESTS       VLT_DISABLE
    #define VLT_ENABLE_IDENTITY_AUTH      VLT_DISABLE
    #define VLT_ENABLE_IDENTITY_AUTH2     VLT_DISABLE    
    #define VLT_ENABLE_SCP02              VLT_DISABLE
    #define VLT_ENABLE_SCP03              VLT_DISABLE
    #define VLT_ENABLE_STRONG_AUTH        VLT_DISABLE
    #define VLT_ENABLE_BLOCK_PROTOCOL     VLT_DISABLE
    #define VLT_ENABLE_ISO7816            VLT_DISABLE
    #define VLT_ENABLE_TWI                VLT_DISABLE
    #define VLT_ENABLE_SPI                VLT_DISABLE
    #define VLT_ENABLE_CIPHER_AES         VLT_DISABLE
    #define VLT_ENABLE_CIPHER_TDES        VLT_DISABLE
    #define VLT_ENABLE_CIPHER_DES         VLT_DISABLE
    #define VLT_ENABLE_FAST_CRC16CCIT     VLT_DISABLE
    #define VLT_ENABLE_KEY_WRAPPING       VLT_DISABLE
    #define VLT_ENABLE_MS_AUTH            VLT_DISABLE
    #define VLT_ENABLE_FILE_SYSTEM        VLT_DISABLE
    #define VLT_ENABLE_AARDVK_SPPRSS_ERR  VLT_DISABLE
    #define VLT_ENABLE_ECDSA_SIGNER       VLT_DISABLE
    #define VLT_ENABLE_AARDVARK           VLT_DISABLE
    #define VLT_ENABLE_CHEETAH            VLT_DISABLE
	#define VLT_ENABLE_CHIP_DEACTIVATION  VLT_DISABLE
	#define VLT_ENABLE_SECURE_COUNTERS	  VLT_DISABLE
#endif



/**
 * Set appropriate version for the target Vault IC
 * to ensure the correct functionality is included.
 */
#define VAULT_IC_VERSION     VAULTIC_VERSION_1_2_0
#define VAULT_IC_TARGET      VAULTIC100


#endif /*VAULTIC_CONFIG_H*/
