/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_consts.h
 *
 * \defgroup Constants Commonly used API Constants
 *
 * \brief Constants used by VaultIC.
 * 
 * \par Description:
 * This file declares the constants used by VaultIC.
 */
/*@{*/
#ifndef VAULTIC_CONSTS_H
#define VAULTIC_CONSTS_H

/*******************************************************
 Operators IDs: 
*******************************************************/
/**
 * \def VLT_USER0
 * \brief System User 0.
 */
#define VLT_USER0                               (VLT_U8)0x00
/**
 * \def VLT_USER1
 * \brief System User 1.
 */
#define VLT_USER1                               (VLT_U8)0x01
/**
 * \def VLT_USER2
 * \brief System User 2.
 */
#define VLT_USER2                               (VLT_U8)0x02
/**
 * \def VLT_USER3
 * \brief System User 3.
 */
#define VLT_USER3                               (VLT_U8)0x03
/**
 * \def VLT_USER4
 * \brief System User 4.
 */
#define VLT_USER4                               (VLT_U8)0x04
/**
 * \def VLT_USER5
 * \brief System User 5.
 */
#define VLT_USER5                               (VLT_U8)0x05
/**
 * \def VLT_USER6
 * \brief System User 6.
 */
#define VLT_USER6                               (VLT_U8)0x06
/**
 * \def VLT_USER7
 * \brief System User 7.
 */
#define VLT_USER7                               (VLT_U8)0x07
#define VLT_USER_ID_LIMIT                       (VLT_U8)0x07


/*******************************************************
 Operator Defined Role IDs: 
*******************************************************/
/**
 * \def VLT_APPROVED_USER
 * \brief A user is assumed to perform general security 
 * services and approved-only cryptographic operations.
 */
#define VLT_APPROVED_USER                       (VLT_U8)0x01
/**
 * \def VLT_NON_APPROVED_USER
 * \brief A non approved user is assumed to perform both 
 * approved and non approved cryptographic operations.
 */
#define VLT_NON_APPROVED_USER                   (VLT_U8)0x02
/**
 * \def VLT_MANUFACTURER
 * \brief The Manufacturer can personalise and configure 
 * the chip and perform maintenance operations.
 */
#define VLT_MANUFACTURER                        (VLT_U8)0x04
/**
 * \def VLT_ADMINISTRATOR
 * \brief The Administrator can usually manage the approved 
 * users authentication data.
 */
#define VLT_ADMINISTRATOR                       (VLT_U8)0x08
/**
 * \def VLT_NON_APPROVED_ADMINISTRATOR
 * \brief The Non Approved Administrator can usually manage 
 * the non approved users authentication data.
 */
#define VLT_NON_APPROVED_ADMINISTRATOR          (VLT_U8)0x10
/**
 * \def VLT_EVERYONE
 * \brief A role that is not authenticated.
 */
#define VLT_EVERYONE                            (VLT_U8)0x20


/*******************************************************
 Manage Authentication Operation IDs: 
*******************************************************/
/**
 * \def VLT_CREATE_USER
 * \brief Defines and creates a new operator
 */
#define VLT_CREATE_USER                         (VLT_U8)0x01
/**
 * \def VLT_DELETE_USER
 * \brief Removes an existing operator and associated 
 * authentication data
 */
#define VLT_DELETE_USER                         (VLT_U8)0x02
/**
 * \def VLT_UPDATE_USER
 * \brief Updates operator's authentication data
 */
#define VLT_UPDATE_USER                         (VLT_U8)0x04
/**
 * \def VLT_LOCK_USER
 * \brief Lock operator (set authentication try counter to zero)
 */
#define VLT_LOCK_USER                           (VLT_U8)0x10
/**
 * \def VLT_UNLOCK_USER
 * \brief Unlock operator (set authentication try counter to max tries counter)
 */
#define VLT_UNLOCK_USER                         (VLT_U8)0x20


/*******************************************************
 Secure Channel Level IDs: 
*******************************************************/
/**
 * \def VLT_NO_CHANNEL
 * \brief No protection (mutual authentication only)
 */
#define VLT_NO_CHANNEL                          (VLT_U8)0x00
/**
 * \def VLT_CMAC
 * \brief APDU command MAC
 */
#define VLT_CMAC                                (VLT_U8)0x01
/**
 * \def VLT_CMAC_CENC
 * \brief APDU command MAC + encrypted command data field
 */
#define VLT_CMAC_CENC                           (VLT_U8)0x03
/**
 * \def VLT_CMAC_RMAC
 * \brief APDU command MAC + APDU response MAC
 */
#define VLT_CMAC_RMAC                           (VLT_U8)0x11
/**
 * \def VLT_CMAC_CENC_RMAC
 * \brief APDU command MAC + encrypted command data field + 
 * APDU response MAC
 */
#define VLT_CMAC_CENC_RMAC                      (VLT_U8)0x13
/**
 * \def VLT_CMAC_CENC_RMAC_RENC
 * \brief APDU command MAC + encrypted command data field + 
 *  encrypted APDU resonse + APDU response MAC
 */
#define VLT_CMAC_CENC_RMAC_RENC                 (VLT_U8)0x33

/*******************************************************
 Manage Authentication Security Option IDs: 
*******************************************************/
#define VLT_DELETE_ON_LOCK                      (VLT_U8)0x01
#define VLT_NO_DELETE_ON_LOCK                   (VLT_U8)0x00

/*******************************************************
 Defines used to identify the current users
 login method.
*******************************************************/
/**
 * \def VLT_LOGIN_PASSWORD
 * \brief Used to indicate that a password is required as an
 * authentication access mechanism.
 */
#define VLT_LOGIN_PASSWORD                      (VLT_U8)0x01
/**
 * \def VLT_LOGIN_SCP02
 * \brief Used to indicate that Secure Channel Protocol 2 is 
 * is required as an authentication access mechanism.
 */
#define VLT_LOGIN_SCP02                         (VLT_U8)0x02
/**
 * \def VLT_LOGIN_SCP03
 * \brief Used to indicate that Secure Channel Protocol 3 is 
 * is required as an authentication access mechanism.
 */
#define VLT_LOGIN_SCP03                         (VLT_U8)0x03
/**
 * \def VLT_LOGIN_MS
 * \brief Used to indicate that Microsoft Channel is 
 * is required as an authentication access mechanism.
 */
#define VLT_LOGIN_MS                            (VLT_U8)0x80
/**
 * \def VLT_LOGIN_STRONG
 * \breif Used to indicate that Strong Authentication is
 * required as an authentication mechanism.
 */
#define VLT_LOGIN_STRONG                        (VLT_U8)0x04

/*******************************************************
 Manage Authentication Data Useful IDs: 
*******************************************************/
#define VLT_SMAC_KEY_INDEX                      (VLT_U8)0x00
#define VLT_SENC_KEY_INDEX                      (VLT_U8)0x01
#define VLT_MS_KEY_INDEX                        (VLT_U8)0x00
#define VLT_PWD_KEY_INDEX                       (VLT_U8)0x00
#define VLT_DEFAULT_TRY_COUNT                   (VLT_U8)0x03
#define VLT_SCP_NUMBER_OF_KEYS                  (VLT_U8)0x02
#define VLT_MS_NUMBER_OF_KEYS                   (VLT_U8)0x01
#define VLT_NUMBER_OF_PWDS                      (VLT_U8)0x01

/*******************************************************
 Initialize Algorithm Mode IDs : 
*******************************************************/
#define VLT_ENCRYPT_MODE                        (VLT_U8)0x01
#define VLT_DECRYPT_MODE                        (VLT_U8)0x02
#define VLT_GENERIC_STRONG_AUTH_MODE            (VLT_U8)0x04
#define VLT_DIGEST_MODE                         (VLT_U8)0x08
#define VLT_WRAP_KEY_MODE                       (VLT_U8)0x10
#define VLT_UNWRAP_KEY_MODE                     (VLT_U8)0x20
#define VLT_SIGN_MODE                           (VLT_U8)0x40
#define VLT_VERIFY_MODE                         (VLT_U8)0x80


/*******************************************************
 Algorithm IDs
*******************************************************/

/* Note: Identifiers using "3DES" in the data sheet have become "TDES" in
 *       these identifiers. */

/* Signer identifiers (VLT_ALG_SIG_*) */


/**
 * \def VLT_ALG_SIG_MAC_ISO9797_ALG1
 * \brief ISO/IEC 9797-1 Algorithm 1 CBC-MAC using 3DES-EDE
 */
#define VLT_ALG_SIG_MAC_ISO9797_ALG1            (VLT_U8)0x00
/**
 * \def VLT_ALG_SIG_MAC_ISO9797_ALG3
 * \brief ISO/IEC 9797-1 Algorithm 3 CBC-MAC using DES
 */
#define VLT_ALG_SIG_MAC_ISO9797_ALG3            (VLT_U8)0x01
/**
 * \def VLT_ALG_SIG_CMAC_AES
 * \brief NIST SP 800-38B AES CMAC
 */
#define VLT_ALG_SIG_CMAC_AES                    (VLT_U8)0x11
/**
 * \def VLT_ALG_SIG_HMAC
 * \brief FIPS 198 HMAC - Hash-based Message Authentication Code
 */
#define VLT_ALG_SIG_HMAC                        (VLT_U8)0x10
/**
 * \def VLT_ALG_SIG_HOTP
 * \brief RFC4226 Has-based OTP
 */
#define VLT_ALG_SIG_HOTP                        (VLT_U8)0x03
/**
 * \def VLT_ALG_SIG_TOTP
 * \brief OATH Draft v5 Time-based OTP
 */
#define VLT_ALG_SIG_TOTP                        (VLT_U8)0x04
/**
 * \def VLT_ALG_SIG_RSASSA_PKCS_PSS
 * \brief PKCS#1 v2.1 RSASSA-PSS signer 
 */
#define VLT_ALG_SIG_RSASSA_PKCS_PSS             (VLT_U8)0x16
/**
 * \def VLT_ALG_SIG_RSASSA_PKCS
 * \brief PKCS#1 v2.1 RSASSA-PKCS1-v1_5 signer 
 */
#define VLT_ALG_SIG_RSASSA_PKCS                 (VLT_U8)0x17
/**
 * \def VLT_ALG_SIG_RSASSA_X509
 * \brief RSASSA-X509 raw RSA signer 
 */
#define VLT_ALG_SIG_RSASSA_X509                 (VLT_U8)0x05
/**
 * \def VLT_ALG_SIG_DSA
 * \brief FIPS 186-3 DSA - Digital Signature Algorithm signer
 */
#define VLT_ALG_SIG_DSA                         (VLT_U8)0x18
/**
 * \def VLT_ALG_SIG_ECDSA_GFP
 * \brief FIPS 186-3 signer over prime fields GF(p) ( P-xxx )
 */
#define VLT_ALG_SIG_ECDSA_GFP                   (VLT_U8)0x19
/**
 * \def VLT_ALG_SIG_ECDSA_GF2M
 * \brief FIPS 186-3 signer over binary fields GF(p) ( B-xxx and K-xxx )
 */
#define VLT_ALG_SIG_ECDSA_GF2M                  (VLT_U8)0x1A

/* Digest identifiers (VLT_ALG_DIG_*) */
/**
 * \def VLT_ALG_DIG_SHA1
 * \brief FIPS 186-3 SHA-1 digest
 */
#define VLT_ALG_DIG_SHA1                        (VLT_U8)0x30
/**
 * \def VLT_ALG_DIG_SHA224
 * \brief FIPS 186-3 SHA-224 digest
 */
#define VLT_ALG_DIG_SHA224                      (VLT_U8)0x31
/**
 * \def VLT_ALG_DIG_SHA256
 * \brief FIPS 186-3 SHA-256 digest
 */
#define VLT_ALG_DIG_SHA256                      (VLT_U8)0x32
/**
 * \def VLT_ALG_DIG_SHA384
 * \brief FIPS 186-3 SHA-384 digest
 */
#define VLT_ALG_DIG_SHA384                      (VLT_U8)0x33
/**
 * \def VLT_ALG_DIG_SHA512
 * \brief FIPS 186-3 SHA-512 digest
 */
#define VLT_ALG_DIG_SHA512                      (VLT_U8)0x34

/* Key Transport Scheme identifiers (VLT_ALG_KTS_*) */

/**
 * \def VLT_ALG_KTS_RSA_OAEP_BASIC
 * \brief NIST SP800-56B Key Transport Scheme based on 
 * RSAES-OAEP algorithm without key confirmation
 */
#define VLT_ALG_KTS_RSA_OAEP_BASIC              (VLT_U8)0x83
/**
 * \def VLT_ALG_KTS_AES
 * \brief Generic Key Transport Scheme based on AES
 */
#define VLT_ALG_KTS_AES                         (VLT_U8)0x92
/**
 * \def VLT_ALG_KTS_TDES_3K_EEE
 * \brief Generic Key Transport Scheme based on 3DES-EEE
 */
#define VLT_ALG_KTS_TDES_3K_EEE                 (VLT_U8)0x91
/**
 * \def VLT_ALG_KTS_TDES_3K_EDE
 * \brief Generic Key Transport Scheme based on 3DES-EDE
 */
#define VLT_ALG_KTS_TDES_3K_EDE                 (VLT_U8)0x90

/* Cipher identifiers (VLT_ALG_CIP_*) */
/**
 * \def VLT_ALG_CIP_DES
 * \brief FIPS 46-3 Single DES cipher
 */
#define VLT_ALG_CIP_DES                         (VLT_U8)0x40
/**
 * \def VLT_ALG_CIP_TDES_2K_EDE
 * \brief 3DES-EDE Algorithm cipher with double length key.
 */
#define VLT_ALG_CIP_TDES_2K_EDE                 (VLT_U8)0x41
/**
 * \def VLT_ALG_CIP_TDES_3K_EDE
 * \brief 3DES-EDE Algorithm cipher with triple length key.
 */
#define VLT_ALG_CIP_TDES_3K_EDE                 (VLT_U8)0x50
/**
 * \def VLT_ALG_CIP_TDES_3K_EEE
 * \brief 3DES-EEE Algorithm cipher with triple length key.
 */
#define VLT_ALG_CIP_TDES_3K_EEE                 (VLT_U8)0x51
/**
 * \def VLT_ALG_CIP_AES
 * \brief FIPS 197 AES Cipher 
 */
#define VLT_ALG_CIP_AES                         (VLT_U8)0x52
 /**
 * \def VLT_ALG_CIP_AES_GCM
 * \brief NIST SP800-38D AES Cipher 
 */
#define VLT_ALG_CIP_AES_GCM                     (VLT_U8)0x53
 /**
 * \def VLT_ALG_CIP_AES_CCM
 * \brief NIST SP800-38E AES Cipher 
 */
#define VLT_ALG_CIP_AES_CCM                     (VLT_U8)0x54
/**
 * \def VLT_ALG_CIP_RSAES_PKCS_OAEP
 * \brief PKCS#1 v2.1 RSAES-OAEP cipher 
 */
#define VLT_ALG_CIP_RSAES_PKCS_OAEP             (VLT_U8)0x42
/**
 * \def VLT_ALG_CIP_RSAES_PKCS
 * \brief PKCS#1 v2.1 RSAES-PKCS1-v1_5 cipher 
 */
#define VLT_ALG_CIP_RSAES_PKCS                  (VLT_U8)0x43
/**
 * \def VLT_ALG_CIP_RSAES_X509
 * \brief RSAES-X509 raw RSA cipher with no padding 
 */
#define VLT_ALG_CIP_RSAES_X509                  (VLT_U8)0x44

/* Key Pair Generator identifiers (VLT_ALG_KPG_*) */
/**
 * \def VLT_ALG_KPG_RSASSA
 * \brief RSA Key pair generation for signer engines
 */
#define VLT_ALG_KPG_RSASSA                      (VLT_U8)0x70
/**
 * \def VLT_ALG_KPG_RSAES
 * \brief RSA Key pair generation for cipher engines
 */
#define VLT_ALG_KPG_RSAES                       (VLT_U8)0x73
/**
 * \def VLT_ALG_KPG_DSA
 * \brief DSA Key pair generation
 */
#define VLT_ALG_KPG_DSA                         (VLT_U8)0x71
/**
 * \def VLT_ALG_KPG_ECDSA_GFP
 * \brief ECDSA key pair generation over prime fields GF(p) ( P-xxx )
 */
#define VLT_ALG_KPG_ECDSA_GFP                   (VLT_U8)0x72
/**
 * \def VLT_ALG_KPG_ECDSA_GF2M
 * \brief ECDSA key pair generation over binary fields 
 * GF(p) ( B-xxx  and K-xxx )
 */
#define VLT_ALG_KPG_ECDSA_GF2M                  (VLT_U8)0x74

/**
 * \def VLT_ALG_KAS_STATIC_UNIFIED_BSI_ECC_DH_GFp
 * \stricter version of Standard Diffie-Hellman primitive defined by ANS
 * X9.63) over prime fields GF(p) curves (P-xxx)
 */
#define VLT_ALG_KAS_STATIC_UNIFIED_BSI_ECC_DH_GFp			(VLT_U8)0xA0
/**
 * \def VLT_ALG_KAS_STATIC_UNIFIED_BSI_ECC_DH_GF2m
 * \(stricter version of Standard Diffie-Hellman primitive defined by ANS
 * X9.63) over binary fields and Koblitz GF(2m) curves (B-xxx and K-xxx)
 */
#define VLT_ALG_KAS_STATIC_UNIFIED_BSI_ECC_DH_GF2m			(VLT_U8)0xA1
/**
 * \def VLT_ALG_KAS_ONEPASS_BSI_ECC_DH_GFp
 * \(stricter version of Standard Diffie-Hellman primitive defined by ANS
 * X9.63) over prime fields GF(p) curves (P-xxx)
 */
#define VLT_ALG_KAS_ONEPASS_BSI_ECC_DH_GFp					(VLT_U8)0xA2
/**
 * \def VLT_ALG_KAS_ONEPASS_BSI_ECC_DH_GF2m
 * \(stricter version of Standard Diffie-Hellman primitive defined by ANS
 * X9.63) ober binary fields and Koblitz GF(2m) curves (B-xxx and K-xxx)
 */
#define VLT_ALG_KAS_ONEPASS_BSI_ECC_DH_GF2m					(VLT_U8)0xA3

#define VLT_ALG_KDF_CONCATENATION_X963						(VLT_U8)0xC0
#define VLT_ALG_KDF_CONCATENATION_NIST						(VLT_U8)0xD0
#define VLT_ALG_KDF_HASH_MODE								(VLT_U8)0xC1
#define VLT_ALG_KDF_COUNTER_MODE							(VLT_U8)0xD1



/*******************************************************
 Key IDs
*******************************************************/
#define VLT_KEY_DES                             (VLT_U8)0x80
#define VLT_KEY_TDES_2K                         (VLT_U8)0x81
#define VLT_KEY_TDES_3K                         (VLT_U8)0x82
#define VLT_KEY_AES_128                         (VLT_U8)0x85
#define VLT_KEY_AES_192                         (VLT_U8)0x86
#define VLT_KEY_AES_256                         (VLT_U8)0x87
#define VLT_KEY_HMAC                            (VLT_U8)0x89
#define VLT_KEY_HOTP                            (VLT_U8)0x8A
#define VLT_KEY_TOTP                            (VLT_U8)0x8B
#define VLT_KEY_RSASSA_PUB                      (VLT_U8)0x02
#define VLT_KEY_RSASSA_PRIV                     (VLT_U8)0x90
#define VLT_KEY_RSASSA_PRIV_CRT                 (VLT_U8)0x91
#define VLT_KEY_RSAES_PUB                       (VLT_U8)0x03
#define VLT_KEY_RSAES_PRIV                      (VLT_U8)0x92
#define VLT_KEY_RSAES_PRIV_CRT                  (VLT_U8)0x93
#define VLT_KEY_DSA_PUB                         (VLT_U8)0x21
#define VLT_KEY_DSA_PRIV                        (VLT_U8)0xA1
#define VLT_KEY_ECDSA_GF2M_PRIV                 (VLT_U8)0x9C
#define VLT_KEY_ECDSA_GF2M_PUB                  (VLT_U8)0x0B
#define VLT_KEY_HOST_DEVICE_ID                  (VLT_U8)0x60
#define VLT_KEY_ECDSA_DOMAIN_PARMS              (VLT_U8)0x70
#define VLT_KEY_DSA_DOMAIN_PARAMS               (VLT_U8)0x71
#define VLT_KEY_RAW                             (VLT_U8)0xFF
#define VLT_KEY_SECRET_VALUE					(VLT_U8)0xB0
#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_2_1 )
#define VLT_KEY_ECC_PUB							(VLT_U8)0x0A
#define VLT_KEY_ECC_PRIV						(VLT_U8)0x9B
#else
#define VLT_KEY_ECDSA_PRIV                      (VLT_U8)0x9B
#define VLT_KEY_ECDSA_PUB                       (VLT_U8)0x0A
#endif

#define VLT_SECRET_KEY_STATIC_PART_LENGTH       (VLT_U8)0x04
#define VLT_HOTP_KEY_STATIC_PART_LENGTH         (VLT_U8)0x20
#define VLT_TOTP_KEY_STATIC_PART_LENGTH         (VLT_U8)0x18

#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X )
#define VLT_DSA_PUBLIC_STATIC_PART_LENGTH       (VLT_U8)0x05
#define VLT_DSA_PRIVATE_STATIC_PART_LENGTH      (VLT_U8)0x06
#define VLT_ECDSA_PUBLIC_STATIC_PART_LENGTH     (VLT_U8)0x05
#define VLT_ECDSA_PRIVATE_STATIC_PART_LENGTH    (VLT_U8)0x06
#define VLT_RSA_PRIVATE_STATIC_PART_LENGTH      (VLT_U8)0x06
#define VLT_RSA_PUBLIC_STATIC_PART_LENGTH       (VLT_U8)0x05
#define VLT_RSA_CRT_PRIVATE_STATIC_PART_LENGTH  (VLT_U8)0x04
#endif /* ( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X ) */

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)
#define VLT_DSA_PUBLIC_STATIC_PART_LENGTH       (VLT_U8)0x06
#define VLT_DSA_PRIVATE_STATIC_PART_LENGTH      (VLT_U8)0x08
#define VLT_ECDSA_PUBLIC_STATIC_PART_LENGTH     (VLT_U8)0x06
#define VLT_ECDSA_PRIVATE_STATIC_PART_LENGTH    (VLT_U8)0x08
#define VLT_RSA_PRIVATE_STATIC_PART_LENGTH      (VLT_U8)0x08
#define VLT_RSA_PUBLIC_STATIC_PART_LENGTH       (VLT_U8)0x06
#define VLT_RSA_CRT_PRIVATE_STATIC_PART_LENGTH  (VLT_U8)0x06
#define VLT_DSA_PARAMS_STATIC_PART_LENGTH       (VLT_U8)0x0B
#define VLT_ECDSA_PARAMS_STATIC_PART_LENGTH     (VLT_U8)0x0A
#define VLT_HOST_DEV_ID_STATIC_PART_LENGTH      (VLT_U8)0x03
#define VLT_ECDH_PUBLIC_STATIC_PART_LENGTH      (VLT_U8)0x05
#define VLT_ECDH_PRIVATE_STATIC_PART_LENGTH     (VLT_U8)0x08
#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */

#define VLT_PUTKEY_FIXED_DATA_LENGTH            (VLT_U8)0x08

/* XOTP in this case means HOTP or TOTP */
#define VLT_KEY_XOTP_KEY_LENGTH                 (VLT_U8)0x14
#define VLT_KEY_HOTP_MOVINGFACTOR_LENGTH        (VLT_U8)0x08


/*******************************************************
 Put Key Assuarance Values:
*******************************************************/
#define VLT_UNASSURED                               (VLT_U8)0x00
#define VLT_DPV_ASSURED_EXPLICIT_VALIDATION         (VLT_U8)0x03
#define VLT_DPV_ASSURED_COMPLIANT_GENERATION        (VLT_U8)0x04
#define VLT_DPV_ASSURED_SELECTION_FROM_TRUSTED_SET  (VLT_U8)0x05
#define VLT_PKV_ASSURED_EXPLICIT_VALIDATION         (VLT_U8)0x23
#define VLT_PKV_ASSURED_COMPLIANT_GENERATION        (VLT_U8)0x24
#define VLT_PKV_ASSURED_COOPERATIVE_GENERATION      (VLT_U8)0x25
#define VLT_PKV_ASSURED_KEY_REGENERATION            (VLT_U8)0x26


/*******************************************************
 VaultIC Life Cycle State IDs: 
*******************************************************/
/**
 * \def VLT_CREATION
 * \brief Denotes the system creation state, please consult the
 * the Vault IC Generic Datasheet for a detailed description. 
 */
#define VLT_CREATION                            (VLT_U8)0x01
/**
 * \def VLT_OPERATIONAL_ACTIVE
 * \brief Denotes the system operational active state, please 
 * consult the the Vault IC Generic Datasheet for a detailed 
 * description. 
 */
#define VLT_OPERATIONAL_ACTIVE                  (VLT_U8)0x07
/**
 * \def VLT_OPERATIONAL_DEACTIVE
 * \brief Denotes the system operational deactive state, please 
 * consult the the Vault IC Generic Datasheet for a detailed 
 * description. 
 */
#define VLT_OPERATIONAL_DEACTIVE                (VLT_U8)0x87
/**
 * \def VLT_TERMINATED
 * \brief Denotes the system terminated state, please 
 * consult the the Vault IC Generic Datasheet for a detailed 
 * description. 
 */
#define VLT_TERMINATED                          (VLT_U8)0xFF


/*******************************************************
 Set Config Parameter IDs: 
*******************************************************/
#define VLT_USB_VENDORID_PRODUCTID_DEVICEID     (VLT_U8)0x81
#define VLT_USB_ATTRIBUTES                      (VLT_U8)0x82
#define VLT_USB_MANUFACTURER_STRING             (VLT_U8)0x83
#define VLT_USB_PRODUCT_STRING                  (VLT_U8)0x84
#define VLT_USB_CONFIG_STRING                   (VLT_U8)0x85
#define VLT_USB_INTERFACE_STRING                (VLT_U8)0x86
#define VLT_USB_SERIAL_NUMBER_STRING            (VLT_U8)0x87
#define VLT_USB_MAX_POWER                       (VLT_U8)0x89
#define VLT_USB_MAX_POWER_SZ                    (VLT_U8)0x01
#define VLT_USE_AARDVARK_TWI_TIMEOUT            (VLT_U8)0x8A
#define VLT_COMMUNICATION_CHANNEL	            (VLT_U8)0x88
#define VLT_COMMUNICATION_CHANNEL_SZ            (VLT_U8)0x01
#define VLT_ADMIN_SECURITY_POLICY               (VLT_U8)0x01
#define VLT_ISO8601_DATE_TIME                   (VLT_U8)0xCF
#define VLT_ISO8601_DATE_TIME_SZ                (VLT_U8)0x15
#define VLT_I2C_ADDRESS                         (VLT_U8)0xCB
#define VLT_I2C_ADDRESS_SZ                      (VLT_U8)0x01
#define VLT_AARDVARK_TIMEOUT                    (VLT_U8)0x01
#define VLT_AARDVARK_TWI_TIMEOUT_SZ             (VLT_U8)0x01
#define VLT_DEFAULT_TIMEOUT                     (VLT_U8)0x00
#define VLT_SPI_POWER_SAVING_MODE               (VLT_U8)0xCA
#define VLT_SPI_IDLE_MODE                       (VLT_U8)0x00
#define VLT_SPI_POWERDOWN_MODE                  (VLT_U8)0x01
#define VLT_SPI_POWER_SAVING_MODE_SZ            (VLT_U8)0x01
#define VLT_GPIO_ACCESS_MODE		            (VLT_U8)0xCC
#define VLT_GPIO_ACCESS_MODE_SZ					(VLT_U8)0x01

/*******************************************************
 Set Config Parameter IDs: 
*******************************************************/
#define VLT_PRIVATE_EXPONENT                    (VLT_U8)0x00
#define VLT_CRT                                 (VLT_U8)0x01


/*******************************************************
 Generic Authentication Defines IDs: 
*******************************************************/
/**
 * \def VLT_GEN_AUTH_DO_NOT_USE_IDENTIFIERS
 * \brief Generic Strong Authentication parameter field 
 * bOption, Do not use Identifiers.
 */
#define VLT_GEN_AUTH_DO_NOT_USE_IDENTIFIERS     (VLT_U8)0x00
/**
 * \def VLT_GEN_AUTH_USE_IDENTIFIERS
 * \brief Generic Strong Authentication parameter field 
 * bOption, Use identifiers in authentication protocol.
 */
#define VLT_GEN_AUTH_USE_IDENTIFIERS            (VLT_U8)0x01

/*******************************************************
 Returned Data Structures Lengths 
*******************************************************/
#define VLT_FIRMWARE_VERSION_LENGTH             (VLT_U8)0x20
#define VLT_FIRMWARE_DATE_LENGTH                (VLT_U8)0x14

/**
 * \def VLT_CHIP_SERIAL_NUMBER_LENGTH
 * \brief The length in bytes of the serial number.
 */
#define VLT_CHIP_SERIAL_NUMBER_LENGTH           (VLT_U8)0x08

/**
 * \def VLT_SCP03_DEVICE_CHALLENGE_LENGTH
 * \brief The length in bytes of the SCP03 device challenge.
 */
#define VLT_SCP03_DEVICE_CHALLENGE_LENGTH       (VLT_U8)0x08

/**
 * \def VLT_SCPXX_CRYPTOGRAM_LENGTH
 * \brief The length in bytes of the SCP cryptogram.
 */
#define VLT_SCPXX_CRYPTOGRAM_LENGTH             (VLT_U8)0x08

#define VLT_INIT_UPDATE_HOST_CHALLENGE_LENGTH   (VLT_U8)0x08
#define VLT_MAX_CHALLENGE_LENGTH                (VLT_U8)0x40
#define VLT_MAX_SIGNATURE_LENGTH                (VLT_U16)0x200
#define VLT_MAX_IV_LENGTH                       (VLT_U8)0x10
#define VLT_AES_CMAC_LENGTH                     (VLT_U8)0x08
#define VLT_KEY_LENGTH_ATTRIBUTE_SIZE           (VLT_U8)0x02



#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X )
#define VLT_TARGET_INFO_LENGTH_WDATE            (VLT_U8)0x48
#define VLT_TARGET_INFO_LENGTH_WODATE           (VLT_U8)0x34
#endif /* ( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X ) */

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)
#define VLT_TARGET_INFO_LENGTH_WDATE            (VLT_U8)0x49
#define VLT_TARGET_INFO_LENGTH_WODATE           (VLT_U8)0x35
#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */

/*******************************************************
 Command Class Byte Defines
*******************************************************/
#define VLT_CLA_NO_CHANNEL                      (VLT_U8)0x80
#define VLT_CLA_CHANNEL                         (VLT_U8)0x84
#define VLT_CLA_CHAINING                        (VLT_U8)0x90

#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X )
#define VLT_CLA_LEGACY                          (VLT_U8)0x00
#define VLT_CLA_SCP_BIT                         (VLT_U8)(0)
#define VLT_CLA_CHAINING_BIT                    (VLT_U8)(0)
#endif /* ( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X ) */

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)
#define VLT_CLA_SCP_BIT                         (VLT_U8)(1<<2)
#define VLT_CLA_CHAINING_BIT                    (VLT_U8)(1<<4)
#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */

/*******************************************************
 Command Instruction Byte Defines
*******************************************************/
#define VLT_INS_SUBMIT_PASSWORD                 (VLT_U8)0x20
#define VLT_INS_INITIALIZE_UPDATE               (VLT_U8)0x88
#define VLT_INS_EXTERNAL_AUTHENTICATE_SCP       (VLT_U8)0x82
#define VLT_INS_EXTERNAL_AUTHENTICATE_MS        (VLT_U8)0x83
#define VLT_INS_MANAGE_AUTHENTICATION_DATA      (VLT_U8)0x24
#define VLT_INS_GET_AUTHENTICATION_INFO         (VLT_U8)0x25
#define VLT_INS_CANCEL_AUTHENTICATION           (VLT_U8)0x28
#define VLT_INS_GET_CHALLENGE                   (VLT_U8)0x84
#define VLT_INS_GEN_INTERNAL_AUTHENTICATE       (VLT_U8)0x89
#define VLT_INS_INITIALIZE_ALGORITHM            (VLT_U8)0x23
#define VLT_INS_PUT_KEY                         (VLT_U8)0x47
#define VLT_INS_READ_KEY                        (VLT_U8)0x48
#define VLT_INS_DELETE_KEY                      (VLT_U8)0x49
#define VLT_INS_ENCRYPT_DECRYPT                 (VLT_U8)0x2A
#define VLT_INS_GENERATE_VERIFY_SIGNATURE       (VLT_U8)0x2C
#define VLT_INS_GENERATE_ASSURANCE_MESSAGE      (VLT_U8)0x2F
#define VLT_INS_COMPUTE_MESSAGE_DIGEST          (VLT_U8)0x2B
#define VLT_INS_GENERATE_RANDOM                 (VLT_U8)0x2D
#define VLT_INS_GENERATE_KEY_PAIR               (VLT_U8)0x46
#define VLT_INS_BEGIN_TRANSACTION               (VLT_U8)0xFD
#define VLT_INS_END_TRANSACTION                 (VLT_U8)0xFE
#define VLT_INS_SELECT                          (VLT_U8)0xA4
#define VLT_INS_LIST_FILES                      (VLT_U8)0xA5
#define VLT_INS_GET_RESPONSE                    (VLT_U8)0xC0
#define VLT_INS_CREATE_FILE                     (VLT_U8)0xCF
#define VLT_INS_CREATE_FOLDER                   (VLT_U8)0xCD
#define VLT_INS_DELETE_FILE                     (VLT_U8)0xDF
#define VLT_INS_DELETE_FOLDER                   (VLT_U8)0xDD
#define VLT_INS_WRITE_FILE                      (VLT_U8)0xF1
#define VLT_INS_READ_FILE                       (VLT_U8)0xF0
#define VLT_INS_SEEK_FILE                       (VLT_U8)0xFC
#define VLT_INS_SET_PRIVILEGES                  (VLT_U8)0xCB
#define VLT_INS_SET_ATTRIBUTES                  (VLT_U8)0xCA
#define VLT_INS_GET_INFO                        (VLT_U8)0x01
#define VLT_INS_SELF_TEST                       (VLT_U8)0x02
#define VLT_INS_SET_STATUS                      (VLT_U8)0xAA
#define VLT_INS_SET_GPIO                        (VLT_U8)0x32
#define VLT_INS_READ_GPIO                       (VLT_U8)0x34
#define VLT_INS_WRITE_GPIO                      (VLT_U8)0x36
#define VLT_INS_SET_CONFIG                      (VLT_U8)0xCE

#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X )
#define VLT_INS_GEN_EXTERNAL_AUTHENTICATE       (VLT_U8)0x83
#define VLT_INS_TEST_CASE_1                     (VLT_U8)0x10
#define VLT_INS_TEST_CASE_2                     (VLT_U8)0x20
#define VLT_INS_TEST_CASE_3                     (VLT_U8)0x30
#define VLT_INS_TEST_CASE_4                     (VLT_U8)0x40
#endif /* ( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X ) */

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)
#define VLT_INS_GEN_EXTERNAL_AUTHENTICATE       (VLT_U8)0x8A
#define VLT_INS_TEST_CASE_1                     (VLT_U8)0x11
#define VLT_INS_TEST_CASE_2                     (VLT_U8)0x12
#define VLT_INS_TEST_CASE_3                     (VLT_U8)0x13
#define VLT_INS_TEST_CASE_4                     (VLT_U8)0x14
#define VLT_INS_ESTABLISH_KEY_MATERIAL          (VLT_U8)0x45
#define VLT_INS_DERIVE_KEY				        (VLT_U8)0x44
#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */

/*******************************************************
 Initalise Update Defines:
*******************************************************/
#define VLT_INITIALIZE_UPDATE_SCP_RSP_LENGTH    (VLT_U8)0x1C
#define VLT_INITIALIZE_UPDATE_MS_RSP_LENGTH     (VLT_U8)0x08
#define VLT_SCP_ID_SCP02                        (VLT_U8)0x02
#define VLT_SCP_ID_SCP03                        (VLT_U8)0x03
#define VLT_SCP_CHIP_SERIAL_OFFSET              (VLT_U8)0x00
#define VLT_SCP_CHIP_SERIAL_LENGTH              (VLT_U8)0x08
#define VLT_SCP_RFU_OFFSET                      (VLT_U8)0x08
#define VLT_SCP_RFU_LENGTH                      (VLT_U8)0x02
#define VLT_SCP02_SEQUENCE_CTR_OFFSET           (VLT_U8)0x0C
#define VLT_SCP02_SEQUENCE_CTR_LENGTH           (VLT_U8)0x02
#define VLT_SCP_SEQUENCE_CTR_FIELD_LENGTH       (VLT_U8)0x02
#define VLT_SCP_KEYSET_FIELD_OFFSET             (VLT_U8)0x0A
#define VLT_SCP_IDENTIFIER_OFFSET               (VLT_U8)0x0B
#define VLT_SCP03_DEVICE_CHALLENGE_OFFSET       (VLT_U8)0x0C
#define VLT_SCP03_DEVICE_CHALLENGE_LENGTH       (VLT_U8)0x08
#define VLT_SCP02_DEVICE_CHALLENGE_OFFSET       (VLT_U8)0x0E
/**
 * \def VLT_SCP02_DEVICE_CHALLENGE_LENGTH
 * \brief The length in bytes of the SCP02 device challenge.
 */
#define VLT_SCP02_DEVICE_CHALLENGE_LENGTH       (VLT_U8)0x06
#define VLT_SCP_CRYPTOGRAM_OFFSET               (VLT_U8)0x14
#define VLT_SCP_CRYPTOGRAM_LENGTH               (VLT_U8)0x08
#define VLT_MS_DEVICE_CHALLENGE_OFFSET          (VLT_U8)0x00
/**
 * \def VLT_MS_DEVICE_CHALLENGE_LENGTH
 * \brief The length in bytes of the MS device challenge.
 */
#define VLT_MS_DEVICE_CHALLENGE_LENGTH          (VLT_U8)0x08

/*******************************************************
 External Authenticate Defines:
*******************************************************/
#define VLT_EXTERNAL_AUTH_SCP_LENGTH            (VLT_U8)0x10
#define VLT_EXTERNAL_AUTH_MS_LENGTH             (VLT_U8)0x08

/*******************************************************
 Manage Authentication Data Defines:
*******************************************************/
#define VLT_MANAGE_AUTH_DATA_KEYLEN_FIELD_LENGTH (VLT_U8)0x02
#define VLT_AUTH_DATA_HEADER_LENGTH             (VLT_U8)0x07
#define VLT_SECRET_KEY_HEADER_LENGTH            (VLT_U8)0x04

/*******************************************************
 Select File or Directory Data Defines:
*******************************************************/
#define VLT_SF_RESPONSE_LENGTH                  (VLT_U8)0x09

/*******************************************************
 File System Defines:
*******************************************************/
#define VLT_SEEK_FILE_LENGTH_SIZE               (VLT_U8)0x04
#define VLT_FILE_PATH_LENGTH_SIZE               (VLT_U8)0x02
#define VLT_FILE_PRIVILEGES_LENGTH              (VLT_U8)0x04
#define VLT_FILE_ATTRIBUTES_LENGTH              (VLT_U8)0x01

#define VLT_FILE_MAXIMUM_SIZE                   (VLT_U32)0xFFFF

/*******************************************************
 Get Authentication Info Defines:
*******************************************************/
#define VLT_GET_AUTH_INFO_P3                    (VLT_U8)0x07
#define VLT_GET_AUTH_INFO_METHOD_OFFSET         (VLT_U8)0x00
#define VLT_GET_AUTH_INFO_ROLE_OFFSET           (VLT_U8)0x01
#define VLT_GET_AUTH_INFO_SEC_LEVEL_OFFSET      (VLT_U8)0x02
#define VLT_GET_AUTH_INFO_REM_TRY_CNT_OFFSET    (VLT_U8)0x03
#define VLT_GET_AUTH_INFO_MAX_TRY_CNT_OFFSET    (VLT_U8)0x04
#define VLT_GET_AUTH_INFO_SEQUENCE_CNT_OFFSET   (VLT_U8)0x05
#define VLT_GET_AUTH_INFO_SEQUENCE_CNT_LENGTH   (VLT_U8)0x02

/*******************************************************
 Generate Assurance Message Defines:
*******************************************************/
#define VLT_GA_SIGNER_ID_LENGTH                 (VLT_U8)0x08
#define VLT_GA_VERIFIER_ID_LENGTH               (VLT_U8)0x08
#define VLT_GA_MESSAGE_LENGTH                   (VLT_U8)0x50


/*******************************************************
 Write File Defines:
*******************************************************/
/**
 * \def VLT_RECLAIM_SPACE
 * \brief Specify that data after the new end of file 
 * should be reclaimed.
 */
#define VLT_RECLAIM_SPACE                       (VLT_U8)0x01
/**
 * \def VLT_NO_RECLAIM_SPACE
 * \brief Specify that data after the new end of file 
 * should not be reclaimed.
 */
#define VLT_NO_RECLAIM_SPACE                    (VLT_U8)0x00
/*******************************************************
 Padding Methods
*******************************************************/
/**
 * \def PADDING_ISO9797_METHOD2
 * \brief Denotes ISO9797-1 Padding Method 2
 */
#define PADDING_ISO9797_METHOD2                 (VLT_U8)0x02
/**
 * \def PADDING_NONE
 * \brief Denotes No Padding.
 */
#define PADDING_NONE                            (VLT_U8)0x00
/**
 * \def PADDING_PKCS5
 * \brief Denotes PKCS5 Padding.
 */
#define PADDING_PKCS5                           (VLT_U8)0x05
/**
 * \def PADDING_PKCS7
 * \brief Denotes PKCS7 Padding.
 */
#define PADDING_PKCS7                           (VLT_U8)0x07

/*******************************************************
 Chaining mode identifiers
*******************************************************/
/**
 * \def BLOCK_MODE_ECB
 * \brief Denotes the Electronic Code Book Chaining Mode
 */
#define BLOCK_MODE_ECB                          (VLT_U8)0x00
/**
 * \def BLOCK_MODE_CBC
 * \brief Denotes the Cipher Block Chaining Mode
 */
#define BLOCK_MODE_CBC                          (VLT_U8)0x01
/**
 * \def BLOCK_MODE_OFB
 * \brief Denotes the Output Feedback Register Chaining Mode
 */
#define BLOCK_MODE_OFB                          (VLT_U8)0x02
/**
 * \def BLOCK_MODE_CFB
 * \brief Denotes the Cipher Feedback Register Chaining Mode
 */
#define BLOCK_MODE_CFB                          (VLT_U8)0x03
/**
 * \def BLOCK_MODE_CTR
 * \brief Denotes the Cipher Feedback Register Chaining Mode
 */
#define BLOCK_MODE_CTR                          (VLT_U8)0x04


#define DES_KEY_SIZE                            (VLT_U8)8
#define TDES_KEY_SIZE                           (VLT_U8)16
#define TDES_3K_KEY_SIZE                        (VLT_U8)24
#define DES_BLOCK_SIZE                          (VLT_U8)8
#define AES_128_KEY_SIZE                        (VLT_U16)16
#define AES_192_KEY_SIZE                        (VLT_U16)24
#define AES_256_KEY_SIZE                        (VLT_U16)32
#define AES_BLOCK_SIZE                          (VLT_U8)16

/*********************************************************
 Status Codes
*********************************************************/
#define VLT_STATUS_NONE                         (VLT_SW)0x0000
#define VLT_STATUS_GET_RESPONSE                 (VLT_SW)0x6100
#define VLT_STATUS_COMPLETED                    (VLT_SW)0x6200
#define VLT_STATUS_ABORTED                      (VLT_SW)0x6300
#define VLT_STATUS_RESPONDING                   (VLT_SW)0x6301
#define VLT_STATUS_REISSUE                      (VLT_SW)0x6C00
#define VLT_STATUS_SUCCESS                      (VLT_SW)0x9000
#define VLT_STATUS_EOF                          (VLT_SW)0x9001

#define VLT_STATUS_NEXT_MESSAGE_PART_EXPECTED   (VLT_SW)0x6200
#define VLT_STATUS_NEXT_SIGNATURE_PART_EXPECTED (VLT_SW)0x6201

#define VLT_STATUS_TRANSACTION_MEM_FAILURE      (VLT_SW)0x6581

/**
* \def VLT_CRC16_CCITT_INIT_Fs
* \brief Used to initialise the CRC16 CCITT calculation with 0xFFFF.
*/
#define VLT_CRC16_CCITT_INIT_Fs                 (VLT_U16)0xFFFF
/**
* \def VLT_CRC16_CCITT_INIT_0s
* \brief Used to initialise the CRC16 CCITT calculation with 0x0000.
*/
#define VLT_CRC16_CCITT_INIT_0s                 (VLT_U16)0x0000

/*******************************************************
 Communication mode identifiers
*******************************************************/
/**
* \def VLT_ISO_T0_COMMS
* \brief Selects ISO7816 T0 communications
*/
#define VLT_ISO_T0_COMMS                        (VLT_U8)0x00
/**
* \def VLT_ISO_T1_COMMS
* \brief Selects ISO7816 T1 communications
*/
#define VLT_ISO_T1_COMMS                        (VLT_U8)0x01
/**
* \def VLT_USB_COMMS
* \brief Selects USB communications  
*/
#define VLT_USB_COMMS                           (VLT_U8)0x02
/**
* \def VLT_SPI_COMMS
* \brief Selects SPI communications  
*/
#define VLT_SPI_COMMS                           (VLT_U8)0x03
/**
* \def VLT_TWI_COMMS
* \brief Selects TWI communications  
*/
#define VLT_TWI_COMMS                           (VLT_U8)0x04

/**
* \def VLT_ISO_T1_COMMS
* \brief Selects ISO7816 T0 or T1 communications
*/
#define VLT_ISO_T0_OR_T1_COMMS                  (VLT_U8)0x05

/*******************************************************
 Block Protocol Checksum Modes
*******************************************************/
/**
* \def BLK_PTCL_CHECKSUM_SUM8
* \brief Sets TWI and SPI checksum communications to SUM 8
*/
#define BLK_PTCL_CHECKSUM_SUM8                  (VLT_U8)0x01
/**
* \def BLK_PTCL_CHECKSUM_CRC16
* \brief Sets TWI and SPI checksum communications to CRC CCITT
*/
#define BLK_PTCL_CHECKSUM_CRC16                 (VLT_U8)0x02

/*
* \def VLT_USER_NOT_AUTHENTICATED
* \brief User has not successfully logged in.
*/
#define VLT_USER_NOT_AUTHENTICATED              (VLT_U8)0x00
/*
* \def VLT_USER_AUTHENTICATED
* \brief User has successfully logged in.
*/
#define VLT_USER_AUTHENTICATED                  (VLT_U8)0x01


/*
* \def VLT_SPI_AARDVARK_ADAPTER
* \brief Sets the SPI adapter to be a Total Phase Aardvark
*/
#define VLT_SPI_AARDVARK_ADAPTER                (VLT_U8)0x00
/*
* \def VLT_SPI_CHEETAH_ADAPTER
* \brief Sets the SPI adapter to be a Total Phase Cheetah
*/
#define VLT_SPI_CHEETAH_ADAPTER                 (VLT_U8)0x01

/*******************************************************
Library capabilities options
*******************************************************/

/*
* \def VLT_CPB_ENABLE_KEY_SECRET
* \brief Library supports the use of the ::VLT_KEY_OBJ_SECRET type in all relevant API methods. 
*/
#define VLT_CPB_ENABLE_KEY_SECRET               (VLT_U32)0x00000001
/*
* \def VLT_CPB_ENABLE_KEY_SECRET
* \brief Library supports the use of the ::VLT_KEY_OBJ_HOTP type in all relevant API methods. 
*/
#define VLT_CPB_ENABLE_KEY_HOTP                 (VLT_U32)0x00000002
/*
* \def VLT_CPB_ENABLE_KEY_TOTP
* \brief Library supports the use of the ::VLT_KEY_OBJ_TOTP type in all relevant API methods. 
*/
#define VLT_CPB_ENABLE_KEY_TOTP                 (VLT_U32)0x00000004
/*
* \def VLT_CPB_ENABLE_KEY_RSA
* \brief Library supports the use of the ::VLT_KEY_OBJ_RSA_PUB, ::VLT_KEY_OBJ_RSA_PRIV and VLT_KEY_OBJ_RSA_PRIV_CRT types in all relevant API methods. 
*/
#define VLT_CPB_ENABLE_KEY_RSA                  (VLT_U32)0x00000008    
/*
* \def VLT_CPB_ENABLE_KEY_DSA
* \brief Library supports the use of the ::VLT_KEY_OBJ_DSA_PUB and ::VLT_KEY_OBJ_DSA_PRIV types in all relevant API methods. 
*/
#define VLT_CPB_ENABLE_KEY_DSA                  (VLT_U32)0x00000010
/*
* \def VLT_CPB_ENABLE_KEY_ECDSA
* \brief Library supports the use of the ::VLT_KEY_OBJ_ECDSA_PUB and ::VLT_KEY_OBJ_ECDSA_PRIV types in all relevant API methods. 
*/
#define VLT_CPB_ENABLE_KEY_ECDSA                (VLT_U32)0x00000020
/*
* \def VLT_CPB_ENABLE_CIPHER_TESTS
* \brief Library supports running the Cipher Self Tests after initialisation.
*/
#define VLT_CPB_ENABLE_CIPHER_TESTS             (VLT_U32)0x00000040
/*
* \def VLT_CPB_ENABLE_IDENTITY_AUTH
* \brief Library supports the identity authentication service.
* 
* The specific authentication method supported can be found by 
* interrogating the state of the capabilities for VLT_CPB_ENABLE_SCP02,
* VLT_CPB_ENABLE_SCP03 and VLT_CPB_ENABLE_MS_AUTH.
*/
#define VLT_CPB_ENABLE_IDENTITY_AUTH            (VLT_U32)0x00000080
/*
* \def VLT_CPB_ENABLE_SCP02
* \brief Library supports SCP02 as an authentication method.
*/
#define VLT_CPB_ENABLE_SCP02                    (VLT_U32)0x00000100
/*
* \def VLT_CPB_ENABLE_SCP03
* \brief Library supports SCP03 as an authentication method.
*/
#define VLT_CPB_ENABLE_SCP03                    (VLT_U32)0x00000200
/*
* \def VLT_CPB_ENABLE_MS_AUTH
* \brief Library supports Microsoft's Smart Card Minidriver as an authentication method.
*/
#define VLT_CPB_ENABLE_MS_AUTH                  (VLT_U32)0x00000400
/*
* \def VLT_CPB_ENABLE_BLOCK_PROTOCOL
* \brief Library supports the proprietary Block Protocol for peripherals such as TWI and SPI.
*/
#define VLT_CPB_ENABLE_BLOCK_PROTOCOL           (VLT_U32)0x00000800
/*
* \def VLT_CPB_ENABLE_ISO7816
* \brief Library supports the ISO 7816 communication protocol.
*/
#define VLT_CPB_ENABLE_ISO7816                  (VLT_U32)0x00001000
/*
* \def VLT_CPB_ENABLE_TWI
* \brief Library supports communcation with the Vault IC via TWI.
*/
#define VLT_CPB_ENABLE_TWI                      (VLT_U32)0x00002000
/*
* \def VLT_CPB_ENABLE_SPI
* \brief Library supports communcation with the Vault IC via SPI.
*/
#define VLT_CPB_ENABLE_SPI                      (VLT_U32)0x00004000
/*
* \def VLT_CPB_ENABLE_SPI
* \brief Library supports the AES Cipher (128, 192 and 256 bit variant).
*/
#define VLT_CPB_ENABLE_CIPHER_AES               (VLT_U32)0x00008000
/*
* \def VLT_CPB_ENABLE_SPI
* \brief Library supports the Triple DES Cipher (EEE and EDE modes).
*/
#define VLT_CPB_ENABLE_CIPHER_TDES              (VLT_U32)0x00010000
/*
* \def VLT_CPB_ENABLE_SPI
* \brief Library supports the DES Cipher.
*/
#define VLT_CPB_ENABLE_CIPHER_DES               (VLT_U32)0x00020000
/*
* \def VLT_CPB_ENABLE_SPI
* \brief Library supports the table driven CRC16 CCIT calculations.
*/
#define VLT_CPB_ENABLE_FAST_CRC16CCIT           (VLT_U32)0x00040000
/*
* \def VLT_CPB_ENABLE_SPI
* \brief Library supports the key Wrapping Service.
*/
#define VLT_CPB_ENABLE_KEY_WRAPPING             (VLT_U32)0x00080000
/*
* \def VLT_CPB_ENABLE_KEY_HOST_DEVICE_ID
* \brief Library supports the use of the ::VLT_KEY_OBJ_ID type in all relevant API methods. 
*/
#define VLT_CPB_ENABLE_KEY_HOST_DEVICE_ID       (VLT_U32)0x00100000
/*
* \def VLT_CPB_ENABLE_FILE_SYSTEM
* \brief Library supports the File System Service. 
*/
#define VLT_CPB_ENABLE_FILE_SYSTEM              (VLT_U32)0x00200000
/*
* \def VLT_CPB_ENABLE_AARDVRKSUPP_ERR
* \brief Library supports the File System Service. 
*/
#define VLT_CPB_ENABLE_AARDVRKSUPP_ERR          (VLT_U32)0x00400000

/*******************************************************
File System Service definitions
*******************************************************/
/**
* \def VLT_FILE_ENTRY
* \brief Create a file within the file system
*/
#define VLT_FILE_ENTRY                          (VLT_U8)0x00
/**
* \def VLT_FOLDER_ENTRY
* \brief Create a folder within the file system
*/
#define VLT_FOLDER_ENTRY                        (VLT_U8)0x01
/**
* \def VLT_TRANSACTION_DISABLED
* \brief Do not operate within a transaction
*/
#define VLT_TRANSACTION_DISABLED                (VLT_U8)0x00
/**
* \def VLT_TRANSACTION_ENABLED
* \brief Operate within a transaction
*/
#define VLT_TRANSACTION_ENABLED                 (VLT_U8)0x01

/**
* \def VLT_SEEK_FROM_START
* \brief Seek from the start of the file
*/
#define VLT_SEEK_FROM_START                     (VLT_U8)0x00
/**
* \def VLT_SEEK_TO_END
* \brief Seek to the end of the file
*/
#define VLT_SEEK_TO_END                         (VLT_U32)0xFFFFFFFF
/**
* \def VLT_NON_RECURSIVE_DELETE
* \brief Delete the folder non recursively
*/
#define VLT_NON_RECURSIVE_DELETE                (VLT_U8)0x00
/**
* \def VLT_RECURSIVE_DELETE
* \brief Delete the folder and any contained files and folders
*/
#define VLT_RECURSIVE_DELETE                    (VLT_U8)0x01

/*******************************************************
Timer Definitions
*******************************************************/
/**
* \def VLT_FILE_ENTRY
* \brief Create a file within the file system
*/
#define VLT_MICRO_SECS_IN_MSEC                  (VLT_U32)1000


#endif /*VAULTIC_CONSTS_H*/

/*@}*/
