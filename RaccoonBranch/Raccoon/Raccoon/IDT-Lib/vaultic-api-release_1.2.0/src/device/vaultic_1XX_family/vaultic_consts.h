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
 * \def VLT_CREATOR
 * \brief System Creator .
 */
#define VLT_CREATOR                             (VLT_U8)0x00
/**
 * \def VLT_USER
 * \brief System User 1.
 */
#define VLT_USER                                (VLT_U8)0x01
#define VLT_USER_ID_LIMIT                       (VLT_U8)0x02


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
 * \def VLT_MANUFACTURER
 * \brief The Manufacturer can personalise and configure 
 * the chip and perform maintenance operations.
 */
#define VLT_MANUFACTURER                        (VLT_U8)0x04
/**
 * \def VLT_EVERYONE
 * \brief A role that is not authenticated.
 */
#define VLT_EVERYONE                            (VLT_U8)0x10


/*******************************************************
 Manage Authentication Operation IDs: 
*******************************************************/
/**
 * \def VLT_UPDATE_USER
 * \brief Updates operator's authentication data
 */
#define VLT_UPDATE_USER                         (VLT_U8)0x04




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
 * \def VLT_LOGIN_STRONG
 * \breif Used to indicate that Strong Authentication is
 * required as an authentication mechanism.
 */
#define VLT_LOGIN_STRONG                        (VLT_U8)0x04



/*******************************************************
 Manage Authentication Data Useful IDs: 
*******************************************************/
#define VLT_PWD_KEY_INDEX                       (VLT_U8)0x00
#define VLT_DEFAULT_TRY_COUNT                   (VLT_U8)0x03
#define VLT_NUMBER_OF_PWDS                      (VLT_U8)0x01

/*******************************************************
 Initialize Algorithm Mode IDs : 
*******************************************************/
#define VLT_DIGEST_MODE                         (VLT_U8)0x08
#define VLT_SIGN_MODE                           (VLT_U8)0x40
#define VLT_VERIFY_MODE                         (VLT_U8)0x80


/*******************************************************
 Algorithm IDs
*******************************************************/

/* Note: Identifiers using "3DES" in the data sheet have become "TDES" in
 *       these identifiers. */

/* Signer identifiers (VLT_ALG_SIG_*) */

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
/**
 * \def VLT_ALG_DIG_SHA256
 * \brief FIPS 186-3 SHA-256 digest
 */
#define VLT_ALG_DIG_SHA256                      (VLT_U8)0x32

/**
 * \def VLT_ALG_KPG_ECDSA
 * \brief ECDSA key pair generation over binary fields 
 * GF(p) ( B-xxx  and K-xxx )
 */
#define VLT_ALG_KPG_ECDSA_GF2M                  (VLT_U8)0x74



/*******************************************************
 Key IDs
*******************************************************/
#define VLT_KEY_ECDSA_GF2M_PRIV                 (VLT_U8)0x9B
#define VLT_KEY_ECDSA_GF2M_PUB                  (VLT_U8)0x0A
#define VLT_KEY_HOST_DEVICE_ID                  (VLT_U8)0x60
#define VLT_KEY_ECDSA_DOMAIN_PARMS              (VLT_U8)0x70
#define VLT_KEY_RAW                             (VLT_U8)0xFF
#define VLT_ECDSA_PUBLIC_STATIC_PART_LENGTH     (VLT_U8)0x06
#define VLT_ECDSA_PRIVATE_STATIC_PART_LENGTH    (VLT_U8)0x08
#define VLT_ECDSA_PARAMS_STATIC_PART_LENGTH     (VLT_U8)0x0A
#define VLT_HOST_DEV_ID_STATIC_PART_LENGTH      (VLT_U8)0x01
#define VLT_HOST_DEV_ID_STRING_LENGTH           (VLT_U8)0x10
#define VLT_PUTKEY_FIXED_DATA_LENGTH            (VLT_U8)0x08



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
 * \def VLT_DEACTIVATED
 * \brief Denotes the system mute state, please 
 * consult the the Vault IC Generic Datasheet for a detailed 
 * description. 
 */
#define VLT_DEACTIVATED							(VLT_U8)0xD8

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
#define VLT_FIPS_MODE                           (VLT_U8)0x02
#define VLT_FIPS_MODE_SZ                        (VLT_U8)0x01
#define VLT_ECDSA_DOMAIN_PARAM_CHOICE           (VLT_U8)0x03
#define VLT_ECDSA_DOMAIN_PARAM_CHOICE_SZ        (VLT_U8)0x01
#define VLT_LOCK_SUBMIT_PASSWORD                (VLT_U8)0x07
#define VLT_LOCK_SUBMIT_PASSWORD_SZ             (VLT_U8)0x01
#define VLT_I2C_ADDRESS                         (VLT_U8)0xCB
#define VLT_I2C_ADDRESS_SZ                      (VLT_U8)0x01
#define VLT_USER_FILE_ACCESS                    (VLT_U8)0xD0
#define VLT_USER_FILE_ACCESS_SZ                 (VLT_U8)0x01
#define VLT_COMMS_CHANNEL_ACCESS                (VLT_U8)0x88
#define VLT_COMMS_CHANNEL_SZ                    (VLT_U8)0x01
#define VLT_ECDSA_PARAMS_USER_DEF_CURVE_CHOICE  (VLT_U8)0x00
#define VLT_ECDSA_PARAMS_PRE_LOADED_B233_CHOICE (VLT_U8)0x01
#define VLT_ECDSA_PARAMS_PRE_LOADED_K233_CHOICE (VLT_U8)0x02
#define VLT_ECDSA_PARAMS_PRE_LOADED_B283_CHOICE (VLT_U8)0x03
#define VLT_ECDSA_PARAMS_PRE_LOADED_K283_CHOICE (VLT_U8)0x04
#define VLT_ECDSA_PARAMS_CHOICE_SZ              (VLT_U8)0x01
#define VLT_RF_BAUD_RATES						(VLT_U8)0x80
#define VLT_RF_TIMEOUTS							(VLT_U8)0x81
#define VLT_COUNTER_MODE_PARAM					(VLT_U8)0xD1
#define VLT_COUNTER_MODE_SZ						(VLT_U8)0x01

/*******************************************************
 Counters mode
*******************************************************/
#define VLT_COUNTER_MODE						(VLT_U8)0x00
#define VLT_DIRECT_MODE							(VLT_U8)0x01

/*******************************************************
 Curve identifiers used in ECDSA strong authentication
*******************************************************/
#define VLT_ECDSA_CURVE_B233	VLT_ECDSA_PARAMS_PRE_LOADED_B233_CHOICE
#define VLT_ECDSA_CURVE_K233	VLT_ECDSA_PARAMS_PRE_LOADED_K233_CHOICE
#define VLT_ECDSA_CURVE_B283	VLT_ECDSA_PARAMS_PRE_LOADED_B283_CHOICE
#define VLT_ECDSA_CURVE_K283	VLT_ECDSA_PARAMS_PRE_LOADED_K283_CHOICE

/*******************************************************
 Strong authentication modes
*******************************************************/
#define VLT_AUTH_UNILATERAL			(VLT_U8)0
#define VLT_AUTH_MUTUAL_FIPS		(VLT_U8)1
#define VLT_AUTH_MUTUAL_NON_FIPS    (VLT_U8)2 

/*******************************************************
 Set Config Parameter IDs: 
*******************************************************/
#define VLT_PRIVATE_EXPONENT                    (VLT_U8)0x00
#define VLT_CRT                                 (VLT_U8)0x01


/*******************************************************
 Returned Data Structures Lengths 
*******************************************************/
#define VLT_FIRMWARE_VERSION_LENGTH             (VLT_U8)0x20
#define VLT_FIRMWARE_DATE_LENGTH                (VLT_U8)0x14
#define VLT_MAX_IV_LENGTH                       (VLT_U8)0x10


/**
 * \def VLT_CHIP_SERIAL_NUMBER_LENGTH
 * \brief The length in bytes of the serial number.
 */
#define VLT_CHIP_SERIAL_NUMBER_LENGTH           (VLT_U8)0x08
#define VLT_INTERNAL_AUTH_HOST_CHALLENGE_LENGTH (VLT_U8)0x08
#define VLT_INTERNAL_AUTH_DEV_CHALLENGE_LENGTH  (VLT_U8)0x08
#define VLT_MAX_CHALLENGE_LENGTH                (VLT_U8)0x8
#define VLT_MAX_SIGNATURE_LENGTH                (VLT_U16)0x78
#define VLT_KEY_LENGTH_ATTRIBUTE_SIZE           (VLT_U8)0x02
#define VLT_TARGET_INFO_LENGTH                  (VLT_U8)0x35

/**
*\ Secure counter, the length in bytes of the nounce.
*/
#define VLT_NOUNCE_LENGTH           (VLT_U8)0x08
/**
*\ Secure counter, the length in bytes of the counter.
*/
#define VLT_COUNTER_LENGTH			(VLT_U8)0x04
/**
*\ Secure counter, the length in bytes of the RND.
*/
#define VLT_RND_LENGTH				(VLT_U8)0x08

/**
*\ Secure counter, the length in bytes of all counters values.
*/
#define VLT_FULL_COUNTERS_LENGTH	(VLT_U8)0x20

/*******************************************************
 Command Class Byte Defines
*******************************************************/
#define VLT_CLA_NO_CHANNEL                      (VLT_U8)0x80
#define VLT_CLA_CHAINING                        (VLT_U8)0x90


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
#define VLT_INS_GEN_EXTERNAL_AUTHENTICATE       (VLT_U8)0xA0
#define VLT_INS_TEST_CASE_1                     (VLT_U8)0x11
#define VLT_INS_TEST_CASE_2                     (VLT_U8)0x12
#define VLT_INS_TEST_CASE_3                     (VLT_U8)0x13
#define VLT_INS_TEST_CASE_4                     (VLT_U8)0x14
#define VLT_INS_DEACTIVATE						(VLT_U8)0x0D
#define VLT_INS_ACTIVATE						(VLT_U8)0x0A
#define VLT_INS_GETCHALLENGE                    (VLT_U8)0x84
#define VLT_INS_INCREMENT_COUNTER               (VLT_U8)0x71
#define VLT_INS_DECREMENT_COUNTER               (VLT_U8)0x70
#define VLT_INS_SET_COUNTER						(VLT_U8)0xC8

/*******************************************************
 Manage Authentication Data Defines:
*******************************************************/
#define VLT_MANAGE_AUTH_DATA_KEYLEN_FIELD_LENGTH (VLT_U8)0x02
#define VLT_AUTH_DATA_HEADER_LENGTH             (VLT_U8)0x07
#define VLT_SECRET_KEY_HEADER_LENGTH            (VLT_U8)0x04
#define VLT_PASSWORD_REQUIRED_LEN               (VLT_U8)0x20


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

#define VLT_CERTIFICATE_FILE_SIZE               (VLT_U16)0x3D0
#define VLT_FILE_1_SIZE                         (VLT_U8)0x79
#define VLT_FILE_2_SIZE                         (VLT_U8)0x79
#define VLT_FILE_3_SIZE                         (VLT_U8)0x11


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
#define VLT_GA_MESSAGE_LENGTH                   (VLT_U8)0x20

/*******************************************************
 Deactivate/Activate Defines:
*******************************************************/
#define HASH_CHALLENGE_SIZE						(VLT_U8)0x20
#define CHALLENGE_SIZE							(VLT_U8)0x10
#define KEYID_LEN								(VLT_U8)0x02
#define KEYDATA_LEN								(VLT_U8)0x20

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
/*
* \def VLT_CPB_ENABLE_IDENTITY_AUTH_V2
* \breif Library supports the identity authentication service version 2.
* 
* The specific authentication method supported can be found by 
* interrogating the state of the capabilities for VLT_CPB_ENABLE_SCP02,
* VLT_CPB_ENABLE_SCP03, VLT_CPB_ENABLE_MS_AUTH and VLT_CPB_ENABLE_STRONG_AUTH.
*/
#define VLT_CPB_ENABLE_IDENTITY_AUTH_V2            (VLT_U32)0x00800000
/*
* \def VLT_CPB_ENABLE_STRONG_AUTH
* \breif Library supports the strong authentication service.
* 
* The specific identity authentication service supported can be found by 
* interrogating the state of the capabilities for  
* VLT_CPB_ENABLE_IDENTITY_AUTH_V2. This is required for to perform
* strong authentication using the service.
*/
#define VLT_CPB_ENABLE_STRONG_AUTH                  (VLT_U32)0x01000000
/*
* \def VLT_CPB_ENABLE_ECDSA_SIGNER
* \breif Library supports the an internal ECDSA signer.
* 
* The ECDSA signer is internal only, it is not exposed as an interface on the 
* library.  The ECDSA signer is used as part of the identity authentication 
* service. The identity service enable state can be Interrogated by the state 
* of VLT_CPB_ENABLE_IDENTITY_AUTH_V2. 
*/
#define VLT_CPB_ENABLE_ECDSA_SIGNER                  (VLT_U32)0x02000000
/*
* \def VLT_CPB_ENABLE_ECDSA_K233
* \breif Library supports the an internal ECDSA K-233.
* 
* The define is used to add or remove support for K-233 curve.  The curve is 
* used within the identity authentication service.  
*/
#define VLT_CPB_ENABLE_ECDSA_K233                  (VLT_U32)0x04000000
/*
* \def VLT_CPB_ENABLE_ECDSA_K283
* \breif Library supports the an internal ECDSA K-283.
* 
* The define is used to add or remove support for K-283 curve.  The curve is 
* used within the identity authentication service.  
*/
#define VLT_CPB_ENABLE_ECDSA_K283                  (VLT_U32)0x08000000
/*
* \def VLT_CPB_ENABLE_ECDSA_B233
* \breif Library supports the an internal ECDSA B-233-R1.
* 
* The define is used to add or remove support for B-233 curve.  The curve is 
* used within the identity authentication service.  
*/
#define VLT_CPB_ENABLE_ECDSA_B233                  (VLT_U32)0x10000000
/*
* \def VLT_CPB_ENABLE_ECDSA_B283
* \breif Library supports the an internal ECDSA B-283-R1.
* 
* The define is used to add or remove support for B-283 curve.  The curve is 
* used within the identity authentication service.  
*/
#define VLT_CPB_ENABLE_ECDSA_B283                  (VLT_U32)0x20000000






/**
* \def VLT_MICRO_SECS_IN_MSEC
* \brief Number of microseconds in a millisecond
*/
#define VLT_MICRO_SECS_IN_MSEC                  (VLT_U32)1000


#endif /*VAULTIC_CONSTS_H*/

/*@}*/
