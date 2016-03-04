/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_structs.h
 *
 * \defgroup Structures Commonly used API structures
 *
 * \brief Structures used by the VaultIC API.
 *
 * \par Description:
 * A collection of structures used to specify values to the VaultIC API.
 */
/*@{*/

#ifndef VAULTIC_STRUCTS_H
#define VAULTIC_STRUCTS_H

#include "vaultic_typedefs.h"

#if(VLT_ENABLE_ISO7816 == VLT_ENABLE )
#ifndef WIN32
#include "PCSC/winscard.h"
#endif
#endif
/* --------------------------------------------------------------------------
 * API INITIALISATION STRUCTURES
 * -------------------------------------------------------------------------- */

/**
 * \struct _VltInitCommsParams
 *
 * \brief Comms layer parameters.
 */
struct _VltInitCommsParams
{
    /**
     * Selects the communication peripheral, possible values are:
     * - #VLT_ISO_T0_COMMS
     * - #VLT_ISO_T1_COMMS
     * - #VLT_USB_COMMS
     * - #VLT_SPI_COMMS
     * - #VLT_TWI_COMMS
     */
    VLT_U8 u8CommsProtocol;
    /**
     * \union _params
     *
     * \brief Parameters that vary according to the comms protocol.
     */
    union _params
    {
        /**
         * \struct _VltPcscInitParams
         * \brief Specific PCSC comms parameters, 
         * particularly applicable to ISO7816 T0 and T1          
         */
        struct _VltPcscInitParams
        {
            /**
             * String descriptor of the ISO 7816 card reader  
             * attached to the host.
             */
            VLT_PU8 pu8ReaderString;
#if(VLT_ENABLE_ISO7816 == VLT_ENABLE )
			/* VaultIC smart card Handle */
			SCARDHANDLE hCard;

			/* PC/SC system handle */
			SCARDCONTEXT hContext;
#endif
			VLT_U32 u32Protocol;
        }
        VltPcscInitParams;

        /**
         * \struct _VltBlockProtocolParams
         * \brief Specific Block Protocol comms parameters, 
         * particularly applicable to TWI and SPI communication
         * peripherals
         */
        struct _VltBlockProtocolParams
        {
            /**
             * Self Test Delay.  The number of milliseconds to wait for 
             * allowing communications with the VaultIC to allow the self tests
             * to complete.  Failure to set this long enough will see mean that
             * commands are not service by the VaultIC 
             */
            VLT_U16 u16msSelfTestDelay;

            /**
             * After Header Delay. The delay between the block header and the
             * block data being sent to the Vault IC
             */
            VLT_U32 u32AfterHdrDelay;

            /**
             * Inter Block delay.  The delay between a block being received
             * from the Vault IC and the next block being sent to the Vault IC
             */
            VLT_U32 u32InterBlkDelay;

            /**
             * Inter Header Byte delay.  The delay between the bytes 1 and 2 
             * and 2 and 3 of the block header being transmitted
             */
            VLT_U32 u32InterHdrByteDelay;

            /**
             * The TWI/SPI bit rate value is specified as
             * bitrate in KHz.
             */
            VLT_U16 u16BitRate;

            /**
             * The TWI/SPI check sum mode, possible values are:             
             * - #BLK_PTCL_CHECKSUM_SUM8
             * - #BLK_PTCL_CHECKSUM_CRC16
             */
            VLT_U8 u8CheckSumMode;

            /**
             * \struct _VltPeripheralParams
             *
             * \brief Parameters specific to the Peripheral.
             */
            struct _VltPeripheralParams
            {
#if( VLT_PLATFORM != VLT_EMBEDDED )
                /**
                 * The Serial number of the connected Total Phase adapter
                 */
                VLT_U32 u32SerialNo;
#endif
                /**
                 * \union _PeriphParams
                 *
                 * \brief Parameters specific to the Peripheral.
                 */
                union _PeriphParams
                {
                     /**
                     * \struct _VltSpiParams
                     * \brief Specific SPI Comms parameters, 
                     */
                    struct _VltSpiParams
                    {
                        /**
                         * Delay between the end of the block being sent and the
                         * first attempt at receiving data.  This value is 
                         * specified in microseconds
                         */
                        VLT_U32 u32FstPollByteDelay;

                        /**
                         * The delay between polling bytes while the command is 
                         * processed by the Vault IC.  This value is specified
                         * in microseconds
                         */
                        VLT_U32 u32IntPollByteDelay;

                        /**
                         * Delay between bytes being sent over the SPI bus.
                         * This value is specified in microseconds //CHEETAH only
                         */
                        VLT_U32 u32IntByteDelay;

                        /**
                         * The number of retries to make when attempting to receive
                         * data from the Vault IC
                         */
                        VLT_U32 u32PollMaxRetries;

                        /*
                         * The amount of time (in microseconds) to wait from enabling slave
                         * select to transmitting data
                         */
                        VLT_U32 u32SlaveSelectDelay;

#if( VLT_PLATFORM != VLT_EMBEDDED )
                        /**
                         * The type of SPI Adapter connected to the PC, possible 
                         * values are:
                         * - #VLT_SPI_AARDVARK_ADAPTER
                         * - #VLT_SPI_CHEETAH_ADAPTER
                         */
                        VLT_U8 u8AdapterType;
#endif
                    }
                    VltSpiParams;

                    /**
                     * \struct _VltTwiParams
                     * \brief Specific TWI Comms parameters, 
                     */
                    struct _VltTwiParams
                    {
                        /*
                         * The TWI Bus Timeout
                         */
                        VLT_U16 u16BusTimeout;

                        /**
                         * The TWI address value
                         */
                        VLT_U8 u8Address;

                        /**
                         * The timeout for a response, in milliseconds
                         */
                        VLT_U32 u32msTimeout;
                    }
                    VltTwiParams;
                }
                PeriphParams;
            }
            VltPeripheralParams;
        }
        VltBlockProtocolParams;
    }
    Params;
};
/**
 * \typedef VLT_INIT_COMMS_PARAMS
 *
 * \brief The type definition of the structure _VltInitCommsParams
 */
typedef struct _VltInitCommsParams VLT_INIT_COMMS_PARAMS;

/**
 * \typedef VLT_SPI_PARAMS
 *
 * \brief The type definition of the structure _VltSpiParams
 */
typedef struct _VltSpiParams VLT_SPI_PARAMS;

/**
 * \typedef VLT_TWI_PARAMS
 *
 * \brief The type definition of the structure _VltTwiParams
 */
typedef struct _VltTwiParams VLT_TWI_PARAMS;

/* --------------------------------------------------------------------------
 * INITIALIZE UPDATE STRUCTURES
 * -------------------------------------------------------------------------- */

/**
 * \struct _VltInitUpdateScp02RespData
 *
 * \brief SCP02 response data from VltInitializeUpdate.
 */
struct _VltInitUpdateScp02RespData
{
    /**
     * The Device Serial Number of length #VLT_CHIP_SERIAL_NUMBER_LENGTH
     */
    VLT_U8  au8SerialNumber[VLT_CHIP_SERIAL_NUMBER_LENGTH];
    /**
     * The Keyset index
     */
    VLT_U8  u8KeySetIndex;
    /**
     * The SCP identifier
     */
    VLT_U8  u8ScpID;
    /**
     * The Sequence Counter, SC
     */
    VLT_U16 u16SequenceCounter;
    /**
     * The Device challenge Cd, of length #VLT_SCP02_DEVICE_CHALLENGE_LENGTH
     */
    VLT_U8  au8DeviceChallenge[VLT_SCP02_DEVICE_CHALLENGE_LENGTH];
    /**
     * The Device authentication cryptogram ACd, of length #VLT_SCPXX_CRYPTOGRAM_LENGTH
     */
    VLT_U8  au8Cryptogram[VLT_SCPXX_CRYPTOGRAM_LENGTH];
};
/**
 * \typedef VLT_INIT_UPDATE_SCP02
 *
 * \brief The type definition of the structure _VltInitUpdateScp02RespData
 */
typedef struct _VltInitUpdateScp02RespData VLT_INIT_UPDATE_SCP02;

/**
 * \struct _VltInitUpdateScp03RespData
 *
 * \brief SCP03 response data from VltInitializeUpdate.
 */
struct _VltInitUpdateScp03RespData
{
    /**
     * The Device Serial Number of length #VLT_CHIP_SERIAL_NUMBER_LENGTH
     */
    VLT_U8 u8SerialNumber[VLT_CHIP_SERIAL_NUMBER_LENGTH];
    /**
     * The Keyset index
     */
    VLT_U8 u8KeySetIndex;
    /**
     * The SCP identifier
     */
    VLT_U8 u8ScpID;
    /**
     * The Device challenge Cd, of length #VLT_SCP03_DEVICE_CHALLENGE_LENGTH
     */
    VLT_U8 u8DeviceChallenge[VLT_SCP03_DEVICE_CHALLENGE_LENGTH];
    /**
     * The Device authentication cryptogram ACd, of length #VLT_SCPXX_CRYPTOGRAM_LENGTH
     */
    VLT_U8 u8Cryptogram[VLT_SCPXX_CRYPTOGRAM_LENGTH];
};
/**
 * \typedef VLT_INIT_UPDATE_SCP03
 *
 * \brief The type definition of the structure _VltInitUpdateScp03RespData
 */
typedef struct _VltInitUpdateScp03RespData VLT_INIT_UPDATE_SCP03;

/**
 * \struct _VltInitUpdateMsRespData
 *
 * \brief MS response data from VltInitializeUpdate.
 */
struct _VltInitUpdateMsRespData
{
    /**
     * The Device challenge Cd, of length #VLT_MS_DEVICE_CHALLENGE_LENGTH
     */
    VLT_U8 u8DeviceChallenge[VLT_MS_DEVICE_CHALLENGE_LENGTH];
};
/**
 * \typedef VLT_INIT_UPDATE_MS
 *
 * \brief The type definition of the structure _VltInitUpdateMsRespData
 */
typedef struct _VltInitUpdateMsRespData VLT_INIT_UPDATE_MS;

/**
 * \struct _VltInitializeUpdateRspData
 *
 * \brief Response data from VltInitializeUpdate.
 */
struct _VltInitializeUpdateRspData
{
    /**
     * Specifies the login method, possible values are:
     * - #VLT_LOGIN_SCP02
     * - #VLT_LOGIN_SCP03
     * - #VLT_LOGIN_MS
     */
    VLT_U8 u8LoginMethodID;
    /**
     * \union _data
     * \brief Provides an abstraction of the Initialize Update
     * response data for the 3 main authentication mechanisms.
     */
    union _data 
    {
        /**
         * The Scp02 response specialisation
         */
        VLT_INIT_UPDATE_SCP02 Scp02;
        /**
         * The Scp03 response specialisation
         */
        VLT_INIT_UPDATE_SCP03 Scp03;
        /**
         * The MS response specialisation
         */
        VLT_INIT_UPDATE_MS MS;
    }
    data;
};
/**
 * \typedef VLT_INIT_UPDATE
 *
 * \brief The type definition of the structure _VltInitializeUpdateRspData
 */
typedef struct _VltInitializeUpdateRspData VLT_INIT_UPDATE;

/* --------------------------------------------------------------------------
 * MANAGE AUTHENTICATION STRUCTURES
 * -------------------------------------------------------------------------- */

/**
 * \struct _VltManageAuthKeyStructure
 *
 * \brief A key structure.
 */
struct _VltManageAuthKeyStructure
{
    VLT_U8  u8KeyID;      /**< Key ID */
    VLT_U8  u8Mask;       /**< Masking byte */
    VLT_U16 u16KeyLength; /**< Length of the key, in bytes */
    VLT_PU8 pu8Key;       /**< The key itself */
};

/**
 * \typedef VLT_KEY
 *
 * \brief The type definition of the structure _VltManageAuthKeyStructure
 */
typedef struct _VltManageAuthKeyStructure VLT_KEY;

/**
 * \struct _VltManageAuthenticationData
 *
 * \brief Authentication data for VltManageAuthenticationData.
 *
 * \par Description:
 * This is specified when an operator is created or updated.
 */
struct _VltManageAuthenticationData
{
    /** 
     * Authentication method identifier:
     * - #VLT_LOGIN_PASSWORD
     * - #VLT_LOGIN_SCP02
     * - #VLT_LOGIN_SCP03
     * - #VLT_LOGIN_MS
     */
    VLT_U8 u8Method;

    /** 
     * Operator ID :
     * - #VLT_USER0 
     * - #VLT_USER1 
     * - #VLT_USER2 
     * - #VLT_USER3 
     * - #VLT_USER4 
     * - #VLT_USER5 
     * - #VLT_USER6 
     * - #VLT_USER7 
     */
    VLT_U8 u8UserID;

    /** 
     * Bit-field of roles assumed by the operator:
     * - #VLT_APPROVED_USER     
     * - #VLT_NON_APPROVED_USER 
     * - #VLT_MANUFACTURER      
     * - #VLT_ADMINISTRATOR
     * - #VLT_NON_APPROVED_ADMINISTRATOR
     * - #VLT_EVERYONE   
     */
    VLT_U8 u8RoleID;

    /** 
     * Operation identifier:     
     * - #VLT_CREATE_USER    
     * - #VLT_DELETE_USER    
     * - #VLT_UPDATE_USER    
     * - #VLT_LOCK_USER      
     * - #VLT_UNLOCK_USER    
     */
    VLT_U8 u8OperationID;

    /** 
     * Minimum security level required for SCP secure channel.  
     * - #VLT_NO_CHANNEL          
     * - #VLT_CMAC               
     * - #VLT_CMAC_CENC          
     * - #VLT_CMAC_RMAC          
     * - #VLT_CMAC_CENC_RMAC     
     * - #VLT_CMAC_CENC_RMAC_RENC
     */
    VLT_U8 u8ChannelLevel;

    /** 
     * Security option:
     *
     *  - 0x01: When operator is locked, authentication data, files and keys
     *          owned by this operator are wiped. Folders owned by the operator
     *          are not deleted.
     *
     *  - 0x00: Operator's data is persistent
     */
    VLT_U8 u8SecurityOption;

    /** Maximum allowed consecutive authentication failures (1 to 127) */
    VLT_U8 u8TryCount;
    /**
     * \union _mauthdata 
     * \brief Provides an abstraction of the authentication specific data.
     */
    union _mauthdata 
    {
        /**
         * \struct _cleartext 
         * \brief The specialised password based authentication data.
         */
        struct _cleartext
        {
            /** Cleartext password length (4 to 32) */
            VLT_U8 u8PasswordLength;
            /** Cleartext password */
            VLT_U8 u8Password[32];
        }
        cleartext;

        /**
         * \struct _secret 
         * \brief The specialised SCP based authentication data.
         */
        struct _secret
        {
            /** Number of secret keys specified (1 or 2) */
            VLT_U8  u8NumberOfKeys;
            /** Secret keys */
            VLT_KEY aKeys[2];
        }
        secret;
    }
    data;
};
/**
 * \typedef VLT_MANAGE_AUTH_DATA
 *
 * \brief The type definition of the structure _VltManageAuthenticationData
 */
typedef struct _VltManageAuthenticationData VLT_MANAGE_AUTH_DATA;

/* --------------------------------------------------------------------------
 * GET AUTHENTICATION INFO STRUCTURES
 * -------------------------------------------------------------------------- */

/**
 * \struct _VltGetAuthenticationInfoRespData
 *
 * \brief Authentication info response from VltGetInfo.
 */
struct _VltGetAuthenticationInfoRespData
{
    /** Authentication method identifier.
     *  Returns 0xFF if the operator is not defined. */
    /** 
     * Authentication method identifier, Returns 
     * 0xFF if the operator is not defined. 
     * Possible values are:
     * - #VLT_LOGIN_PASSWORD
     * - #VLT_LOGIN_SCP02
     * - #VLT_LOGIN_SCP03
     * - #VLT_LOGIN_MS
     */
    VLT_U8  u8AuthMethod;

    /** 
     * Bit-field of roles assumed by the operator:
     * - #VLT_APPROVED_USER     
     * - #VLT_NON_APPROVED_USER 
     * - #VLT_MANUFACTURER      
     * - #VLT_ADMINISTRATOR
     * - #VLT_NON_APPROVED_ADMINISTRATOR
     * - #VLT_EVERYONE   
     */
    VLT_U8  u8Roles;

    /** 
     * Minimum security level required for SCP secure channel. 
     * Returns zero for non SCP authentication methods. Possible values are:
     * - #VLT_NO_CHANNEL          
     * - #VLT_CMAC               
     * - #VLT_CMAC_CENC          
     * - #VLT_CMAC_RMAC          
     * - #VLT_CMAC_CENC_RMAC     
     * - #VLT_CMAC_CENC_RMAC_RENC
     */
    VLT_U8  u8MinSecurityLevel;

    /** Remaining allowed consecutive authentication failures */
    VLT_U8  u8RemainingTryCount;

    /** Maximum allowed consecutive authentication failures */
    VLT_U8  u8MaxTries;

    /** SCP02 sequence counter.
     *  Returns zero for non SCP02 authentication method. */
    VLT_U16 u16SequenceCount;
};
/**
 * \typedef VLT_AUTH_INFO
 *
 * \brief The type definition of the structure _VltGetAuthenticationInfoRespData
 */
typedef struct _VltGetAuthenticationInfoRespData VLT_AUTH_INFO;

/* --------------------------------------------------------------------------
 * GENERIC STRONG AUTHENTICATION STRUCTURES
 * -------------------------------------------------------------------------- */

/**
 * \struct _VltGenericAuthenticationInfo
 *
 * \brief Generic Strong Authentication parameters.
 *
 * \par Description:
 * Used by VltGenericInternalAuthenticate and VltGetChallenge.
 */
struct _VltGenericAuthenticationInfo
{
    /** Challenge size for authentication protocols.
     *  8 bytes recommended, 64 bytes maximum. */
    VLT_U8  u8ChallengeSize;

    /** 
     * bOption field, possible values are:
     *  - #VLT_GEN_AUTH_DO_NOT_USE_IDENTIFIERS
     *  - #VLT_GEN_AUTH_USE_IDENTIFIERS 
     */
    VLT_U8  u8Option;

#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X )

    /** Device identifier path length */
    VLT_U16 u16DeviceIDPathLength;

    /** Device identifier path */
    VLT_PU8 pu8DeviceIDPath;

    /** Host identifier path length */
    VLT_U16 u16HostIDPathLength;

    /** Host identifier path */
    VLT_PU8 pu8HostIDPath;

#endif /* ( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X ) */

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)

    /** Device identifier group */
    VLT_U8 u8DeviceIdGroup;

    /** Device identifier index */
    VLT_U8 u8DeviceIdIndex;

    /** Host identifier group */
    VLT_U8 u8HostIdGroup;

    /** Host identifier index */
    VLT_U8 u8HostIdIndex;
    
#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */

};

/**
 * \typedef VLT_GENERIC_AUTH_SETUP_DATA
 *
 * \brief The type definition of the structure _VltGenericAuthenticationInfo
 */
typedef struct _VltGenericAuthenticationInfo VLT_GENERIC_AUTH_SETUP_DATA;


/* --------------------------------------------------------------------------
 * INITIALIZE ALGORITHM STRUCTURES
 * -------------------------------------------------------------------------- */

/**
 * \struct _VltSymmetricCipherAlgorithmParameters
 *
 * \brief Symmetric Cipher parameters.
 */
struct _VltSymmetricCipherAlgorithmParameters
{
    /**
     * Chaining mode, possible values are :
     * 
     * - #BLOCK_MODE_ECB
     * - #BLOCK_MODE_CBC
     * - #BLOCK_MODE_OFB
     * - #BLOCK_MODE_CFB
     */
    VLT_U8 u8Mode;   
    /**
     * Padding method, possible values are :
     * - #PADDING_ISO9797_METHOD2
     * - #PADDING_NONE           
     * - #PADDING_PKCS5          
     * - #PADDING_PKCS7          
     */
    VLT_U8 u8Padding;               
    /**
     * Length of initial vector 
     */
    VLT_U8 u8IvLength;              
    /**
     * Initial vector, unused by ECB mode 
     */
    VLT_U8 u8Iv[VLT_MAX_IV_LENGTH]; 
};
/**
 * \typedef VLT_ALG_OBJ_SYM_CIPHER
 *
 * \brief The type definition of the structure _VltSymmetricCipherAlgorithmParameters
 */
typedef struct _VltSymmetricCipherAlgorithmParameters VLT_ALG_OBJ_SYM_CIPHER;

/**
 * \struct _VltCMacSignerAlgorithmParameters
 *
 * \brief CMAC Signer parameters.
 */
struct _VltCMacSignerAlgorithmParameters
{
    /**
     * Padding method, possible values are :
     * - #PADDING_ISO9797_METHOD2
     * - #PADDING_NONE           
     * - #PADDING_PKCS5          
     * - #PADDING_PKCS7          
     */
    VLT_U8 u8Padding; 
    /**
     * Length of initial vector 
     */
    VLT_U8 u8IvLength;
    /**
     * Initial vector, unused by ECB mode 
     */
    VLT_U8 u8Iv[VLT_MAX_IV_LENGTH]; 
};
/**
 * \typedef VLT_ALG_OBJ_CMAC
 *
 * \brief The type definition of the structure _VltCMacSignerAlgorithmParameters
 */
typedef struct _VltCMacSignerAlgorithmParameters VLT_ALG_OBJ_CMAC;

/**
 * \struct _VltHMacSignerAlgorithmParameters
 *
 * \brief HMAC Signer parameters.
 */
struct _VltHMacSignerAlgorithmParameters
{
    /**
     * Digest identifier 
     */
    VLT_U8 u8Digest; 
    /**
     * Truncated HMAC output size, in bytes 
     */
    VLT_U8 u8Output; 
};
/**
 * \typedef VLT_ALG_OBJ_HMAC
 *
 * \brief The type definition of the structure _VltHMacSignerAlgorithmParameters
 */
typedef struct _VltHMacSignerAlgorithmParameters VLT_ALG_OBJ_HMAC;

/**
 * \struct _VltOtpAlgorithmParameters
 *
 * \brief OTP parameters.
 */
struct _VltOtpAlgorithmParameters
{
    /**
     * Required number of digits (6+) 
     */
    VLT_U8 u8Output; 
};
/**
 * \typedef VLT_ALG_OBJ_OTP
 *
 * \brief The type definition of the structure _VltOtpAlgorithmParameters
 */
typedef struct _VltOtpAlgorithmParameters VLT_ALG_OBJ_OTP;

/**
 * \struct _VltRsaesOaepAlgorithmParameters
 *
 * \brief RSAES_PKCS_OAEP cipher parameters.
 */
struct _VltRsaesOaepAlgorithmParameters
{
    /** Digest identifier */
    VLT_U8  u8DigestOaep;

    /** Digest identifier applied during MGF1 masking.
     * We recommended keeping it the same as the OAEP digest. */
    VLT_U8  u8DigestMgf1;

    /** Label length. If zero, no label is used. */
    VLT_U16 u16LLen;

    /** Label */
    VLT_PU8 pu8Label;
};
/**
 * \typedef VLT_ALG_OBJ_RSAES_OAEP
 *
 * \brief The type definition of the structure _VltRsaesOaepAlgorithmParameters
 */
typedef struct _VltRsaesOaepAlgorithmParameters VLT_ALG_OBJ_RSAES_OAEP;

/**
 * \struct _VltRsassaPssAlgorithmParameters
 *
 * \brief RSASSA_PKCS_PSS signer parameters.
 */
struct _VltRsassaPssAlgorithmParameters
{
    /** Digest algorithm applied to the input message and during PSS encoding:
     *
     *  - D7: Input mode:
     *      - 0: Input raw message, internally hashed
     *      - 1: Input predigested message
     *  - D6..0: Digest identifier
     */
    VLT_U8 u8DigestPss;

    /** Digest identifier applied during MGF1 masking.
     * We recommended keeping it the same as the PSS digest. */
    VLT_U8 u8DigestMgf1;

    /** Salt length. Must be in the range 0 to u8DigestPss output length. */
    VLT_U8 u8SaltLength;
};
/**
 * \typedef VLT_ALG_OBJ_RSASSA_PSS
 *
 * \brief The type definition of the structure _VltRsassaPssAlgorithmParameters
 */
typedef struct _VltRsassaPssAlgorithmParameters VLT_ALG_OBJ_RSASSA_PSS;

/**
 * \struct _VltRsassaPkcsAlgorithmParameters
 * \brief RSASSA_PKCS signer parameters.
 */
struct _VltRsassaPkcsAlgorithmParameters
{
    /** Digest algorithm applied to the input message:
     *
     *  - D7: Input mode:
     *      - 0: Input raw message, internally hashed
     *      - 1: Input predigested message
     *  - D6..0: Digest identifier, only valid if D7 == 0
     */
    VLT_U8 u8Digest;
};
/**
 * \typedef VLT_ALG_OBJ_RSASSA_PKCS
 *
 * \brief The type definition of the structure _VltRsassaPkcsAlgorithmParameters
 */
typedef struct _VltRsassaPkcsAlgorithmParameters VLT_ALG_OBJ_RSASSA_PKCS;

/**
 * \struct _VltEcdsaDsaAlgorithmParameters
 *
 * \brief DSA and ECDSA signer parameters.
 */
struct _VltEcdsaDsaAlgorithmParameters
{
    /** Digest algorithm applied to the input message:
     *
     *  - D7: Input mode:
     *      - 0: Input raw message, internally hashed
     *      - 1: Input predigested message
     *  - D6..0: Digest identifier, only valid if D7 == 0
     */
    VLT_U8 u8Digest;
};
/**
 * \typedef VLT_ALG_OBJ_ECDSA_DSA
 *
 * \brief The type definition of the structure _VltEcdsaDsaAlgorithmParameters
 */
typedef struct _VltEcdsaDsaAlgorithmParameters VLT_ALG_OBJ_ECDSA_DSA;

/**
 * \struct _VltKasStaticUnifiedAlgorithmParameters
 *
 * \brief Establish Key material parameters.
 */
struct _VltKasStaticUnifiedAlgorithmParameters
{
	/**Static initiator ECDH public key object group */
	VLT_U8 u8PubKeyGroup;
	/**Static initiator ECDH public key object index */
	VLT_U8 u8PubKeyIndex;
};
/**
 * \typedef VLT_ALG_OBJ_KAS_STATIC_UNIFIED
 *
 * \brief The type definition of the structure _VltKasStaticUnifiedAlgorithmParameters
 */
typedef struct _VltKasStaticUnifiedAlgorithmParameters VLT_ALG_OBJ_KAS_STATIC_UNIFIED;
/**
 * \struct _VltOnePassAlgorithmParameters
 *
 * \brief  Establish Key material parameters.
 */
struct _VltOnePassAlgorithmParameters
{	
	/**Public Key object length, MSB first	*/
	VLT_U16 u8KLen;
	/** Ephemeral initiator ECDH public key object */
    VLT_PU8 pu8keyObject;
};
/**
 * \typedef VLT_ALG_OBJ_KAS_ONEPASS
 *
 * \brief The type definition of the structure _VltOnePassAlgorithmParameters
 */
typedef struct _VltOnePassAlgorithmParameters 	VLT_ALG_OBJ_KAS_ONEPASS;




/**
 * \struct _VltAlgoParamsObj
 *
 * \brief Algorithm parameters.
 *
 * \par Description:
 * Used by VltInitializeAlgorithm.
 */
struct _VltAlgoParamsObj
{
    /** 
     * Algorithm identifier. An identifier for a signer, 
     * digest, key transport
     * scheme or cipher. Possible values are: 
     * - #VLT_ALG_SIG_MAC_ISO9797_ALG1
     * - #VLT_ALG_SIG_MAC_ISO9797_ALG3
     * - #VLT_ALG_SIG_CMAC_AES
     * - #VLT_ALG_SIG_HMAC
     * - #VLT_ALG_SIG_HOTP
     * - #VLT_ALG_SIG_TOTP
     * - #VLT_ALG_SIG_RSASSA_PKCS_PSS
     * - #VLT_ALG_SIG_RSASSA_PKCS
     * - #VLT_ALG_SIG_RSASSA_X509
     * - #VLT_ALG_SIG_DSA
     * - #VLT_ALG_SIG_ECDSA_GFP
     * - #VLT_ALG_SIG_ECDSA_GF2M
     * - #VLT_ALG_DIG_SHA1
     * - #VLT_ALG_DIG_SHA224
     * - #VLT_ALG_DIG_SHA256
     * - #VLT_ALG_DIG_SHA384
     * - #VLT_ALG_DIG_SHA512
     * - #VLT_ALG_KTS_RSA_OAEP_BASIC
     * - #VLT_ALG_KTS_AES
     * - #VLT_ALG_KTS_TDES_3K_EEE
     * - #VLT_ALG_KTS_TDES_3K_EDE
     * - #VLT_ALG_CIP_DES
     * - #VLT_ALG_CIP_TDES_2K_EDE
     * - #VLT_ALG_CIP_TDES_3K_EDE
     * - #VLT_ALG_CIP_TDES_3K_EEE
     * - #VLT_ALG_CIP_AES
     * - #VLT_ALG_CIP_RSAES_PKCS_OAEP
     * - #VLT_ALG_CIP_RSAES_PKCS
     * - #VLT_ALG_CIP_RSAES_X509
     * - #VLT_ALG_KPG_RSASSA
     * - #VLT_ALG_KPG_RSAES
     * - #VLT_ALG_KPG_DSA
     * - #VLT_ALG_KPG_ECDSA_GFP
     * - #VLT_ALG_KPG_ECDSA_GF2M
	 * - #VLT_ALG_KPG_ECDH_GFP
     * - #VLT_ALG_KPG_ECDH_GF2M
     */
    VLT_U8 u8AlgoID;

    /** Union of algorithm parameter structures */
    union _apdata
    {
        /**
         * The Symetric structure
         */
        VLT_ALG_OBJ_SYM_CIPHER  SymCipher;
        /**
         * The CMAC structure
         */
        VLT_ALG_OBJ_CMAC        Cmac;
        /**
         * The HMAC structure
         */
        VLT_ALG_OBJ_HMAC        Hmac;
        /**
         * The OTP structure
         */
        VLT_ALG_OBJ_OTP         Otp;
        /**
         * The RSAES OAEP structure
         */
        VLT_ALG_OBJ_RSAES_OAEP  RsaesOaep;
        /**
         * The RSASSA PSS structure
         */
        VLT_ALG_OBJ_RSASSA_PSS  RsassaPss;
        /**
         * The RSASSA PKCS structure
         */
        VLT_ALG_OBJ_RSASSA_PKCS RsassaPkcs;
        /**
         * The ECDSA DSA structure
         */
        VLT_ALG_OBJ_ECDSA_DSA   EcdsaDsa;
    }
    data;
};

/**
 * \typedef VLT_ALGO_PARAMS
 *
 * \brief The type definition of the structure _VltAlgoParamsObj
 */
typedef struct _VltAlgoParamsObj VLT_ALGO_PARAMS;

/**
 * \struct _VltKeyMaterialsObj
 *
 * \brief Algorithm parameters.
 *
 * \par Description:
 * Used by VltConstructDHAgreement.
 */
struct _VltKeyMaterialsObj
{
	/** 
     * Algorithm identifier. An identifier for a key agrrement, 
	 * Possible values are: 
	 * - #VLT_ALG_KAS_STATIC_UNIFIED_BSI_ECC_DH_GFp
	 * - #VLT_ALG_KAS_STATIC_UNIFIED_BSI_ECC_DH_GF2m
	 * - #VLT_ALG_KAS_ONEPASS_BSI_ECC_DH_GFp
	 * - #VLT_ALG_KAS_ONEPASS_BSI_ECC_DH_GF2m
	 */
	 VLT_U8 u8AlgoID;

	 /** Union of algorithm parameter structures */
	 union _apkeyMaterialData
	 {
		 VLT_ALG_OBJ_KAS_STATIC_UNIFIED	staticUnified;

		 VLT_ALG_OBJ_KAS_ONEPASS			onePass;
	 }
	 data;

	 VLT_U8 u8StaticKeyGroup;	/**< Static key file group. */
	 VLT_U8 u8StaticKeyIndex;	/**< Static key parameter file index. */
};

/**
 * \typedef VLT_KEY_MATERIAL
 *
 * \brief The type definition of the structure _VltKeyMaterialsObj
 */
typedef struct _VltKeyMaterialsObj VLT_KEY_MATERIAL;

/**
 * \typedef VLT_KEY_DERIVATION
 *
 * \brief The type definition of the structure _VltKeyDerivationObj
 */
typedef struct _VltKeyDerivationObj VLT_KEY_DERIVATION;
/* --------------------------------------------------------------------------
 * FILE PRIVILEGES STRUCTURES
 * -------------------------------------------------------------------------- */

/**
 * \struct _VltFilePrivileges
 *
 * \brief File and directory access conditions.
 *
 * \par Description:
 *
 * At file or directory creation time, permission to perform an operation can be
 * granted to some combination of the defined operators. This structure
 * specifies that combination.
 *
 * Each element of the structure is a bit-field. In the bit-field, bit N, if
 * set, indicates that operator N has been granted that permission.
 */
struct _VltFilePrivileges
{
    /** Read access for files, list access for directories. */
    VLT_U8 u8Read;
    /** Write access for files, create access for directories. */
    VLT_U8 u8Write;
    /** Delete access for both files and directories. */
    VLT_U8 u8Delete;
    /** Execute access for files. */
    VLT_U8 u8Execute;
};

/**
 * \typedef VLT_FILE_PRIVILEGES
 *
 * \brief The type definition of the structure _VltFilePrivileges
 */
typedef struct _VltFilePrivileges VLT_FILE_PRIVILEGES;

/* Algo*/
/**
 * \struct _VltConcatenationParameters
 *
 * \brief  Derive Key parameters.
 */
struct _VltConcatenationParameters
{	
	/**Approved digest algorithm identifier applied during KDF */
	VLT_U8 u8digest;
	/**Length of AlgoId field */
	VLT_U16 u16algoIdLen;
	/**String that identifies the AlgoId */
    VLT_PU8 pu8algoId;
	/**Length of sUInfo field */
	VLT_U16 u16UInfoLen;
	/**String that identifies the initiator U */
    VLT_PU8 pu8UInfo;
	/**Length of sVInfo field */
	VLT_U16 u16VInfoLen;
	/**String that identifies the responder V */
    VLT_PU8 pu8VInfo;
	/**Length of SuppPubInfo field */
	VLT_U16 u16suppPubInfoLen;
	/**A bit string containing additional, mutually-known public information */
    VLT_PU8 pu8suppPubInfo;
	/**Length of sVInfo field */
	VLT_U16 u16suppPrivInfoLen;
	/**A bit string containing additional, mutually-known public information */
    VLT_PU8 pu8suppPrivInfo;
};
/**
 * \typedef VLT_ALG_OBJ_KDF_CONCATENATION
 *
 * \brief The type definition of the structure _VltConcatenationParameters
 */
typedef struct _VltConcatenationParameters 	VLT_ALG_OBJ_KDF_CONCATENATION;

/**
 * \struct _VltCounterModeParameters
 *
 * \brief  Derive Key parameters.
 */
struct _VltCounterModeParameters
{	
	/**Approved Psedo Random Function PRF_ identifier applied during KDF */
	VLT_U8 u8PRF;
	/**Length of sInfo field */
	VLT_U16 u16wContextLen;
	/**String that identifies the initiator U */
    VLT_PU8 pu8sContext;
	/**Length of sLabel field */
	VLT_U16 u16wLabelLen;
	/**String that identifies the purpose for the derived keying material */
    VLT_PU8 pu8sLabel;
};
/**
 * \typedef VLT_ALG_OBJ_KDF_COUNTER_MODE
 *
 * \brief The type definition of the structure _VltCounterModeParameters
 */
typedef struct _VltCounterModeParameters 	VLT_ALG_OBJ_KDF_COUNTER_MODE;


/**
 * \struct _VltCounterModeParameters
 *
 * \brief  Derive Key parameters.
 */
struct _VltHashModeParameters
{
	/**digest to use for derivation */
	VLT_U8 u8digest;				
	/**Prepend data length */
	VLT_U16	u16prependLen;		
	/**Pointer to a value to add to the beginning of the message input to the hash function */
	VLT_PU8	pu8prependData;		
	/**Append data length */
	VLT_U16	u16appendLen;	
	/**Pointer to a value to add to the end of the message input to the hash function */
	VLT_PU8	pu8appendData;		
};

/**
 * \typedef VLT_ALG_OBJ_KDF_HASH_MODE
 *
 * \brief The type definition of the structure _VltHashModeParameters
 */
typedef struct _VltHashModeParameters 	VLT_ALG_OBJ_KDF_HASH_MODE;

/**
 * \struct _VltKeyDerivationObj
 *
 * \brief Algorithm parameters.
 *
 * \par Description:
 * Used by VltDeriveKey.
 */
struct _VltKeyDerivationObj
{
	/** 
     * Algorithm identifier. An identifier for a key agrrement, 
	 * Possible values are: 
	 * - #VLT_ALG_KDF_CONCATENATION
	 * - #VLT_ALG_KDF_COUNTER_MODE
	 */
	 VLT_U8 u8AlgoID;

	 /** Union of algorithm parameter structures */
	 union _apkeyDerivationData
	 {
		 VLT_ALG_OBJ_KDF_CONCATENATION		concatenation_SP800_56A;

		 VLT_ALG_OBJ_KDF_COUNTER_MODE		counterMode_SP800_108;

		 VLT_ALG_OBJ_KDF_HASH_MODE			hashMode;
	 }
	 data;

	 VLT_U8 u8SecretKeyGroup;	/** DH Agreement or Master key file group. */
	 VLT_U8 u8SecretKeyIndex;	/** DH Agreement or Master key parameter file index. */
};

/* --------------------------------------------------------------------------
 * KEY OBJECT STRUCTURES
 * -------------------------------------------------------------------------- */
/**
 * \struct _VltRawKey
 *
 * \brief A raw key object format (e.g. all keys ).
 */
struct _VltRawKey
{
    VLT_PU8 pu8KeyObject;
    VLT_U16 u16Crc;
	VLT_BOOL  isEncryptedKey;
	VLT_PU16 pu16ClearKeyObjectLen;
	VLT_PU16 pu16EncKeyObjectLen;
};
/**
 * \typedef VLT_KEY_OBJ_RAW
 *
 * \brief The type definition of the structure _VltRawKey
 */
typedef struct _VltRawKey VLT_KEY_OBJ_RAW;

/**
 * \struct _VltSecretKeyObject
 *
 * \brief A secret key object (e.g. DES, TDES, AES, HMAC).
 */
struct _VltSecretKeyObject
{
    VLT_U8  u8Mask;       /**< Mask value. Zero if masking is not required. */
    VLT_U16 u16KeyLength; /**< Secret key length in bytes. */
    VLT_PU8 pu8Key;       /**< Secret key value. */
};
/**
 * \typedef VLT_KEY_OBJ_SECRET
 *
 * \brief The type definition of the structure _VltSecretKeyObject
 */
typedef struct _VltSecretKeyObject VLT_KEY_OBJ_SECRET;

/**
 * \struct _VltHotpKeyObject
 *
 * \brief An HOTP key object.
 */
struct _VltHotpKeyObject
{
    VLT_U8  u8Mask;          /**< Mask value. Zero if masking is not required. */
    VLT_U16 u16KeyLength;    /**< Secret key length in bytes. */
    VLT_PU8 pu8Key;          /**< HMAC-SHA-1 key value. */
    VLT_PU8 pu8MovingFactor; /**< Moving factor - 8 byte counter value. */
};
/**
 * \typedef VLT_KEY_OBJ_SECRET
 *
 * \brief The type definition of the structure _VltSecretKeyObject
 */
typedef struct _VltHotpKeyObject VLT_KEY_OBJ_HOTP;

/**
 * \struct _VltTotpKeyObject
 *
 * \brief An TOTP key object.
 */
struct _VltTotpKeyObject
{
    VLT_U8  u8Mask;       /**< Mask value. Zero if masking is not required. */
    VLT_U16 u16KeyLength; /**< Secret key length in bytes. */
    VLT_PU8 pu8Key;       /**< HMAC-SHA-1 key value. */
};
/**
 * \typedef VLT_KEY_OBJ_TOTP
 *
 * \brief The type definition of the structure _VltTotpKeyObject
 */
typedef struct _VltTotpKeyObject VLT_KEY_OBJ_TOTP;

/**
 * \struct _VltRsaPublicKeyObject
 *
 * \brief An RSA public key object.
 */
struct _VltRsaPublicKeyObject
{
    VLT_U16 u16NLen; /**< Length of modulus n value in bytes. */
    VLT_PU8 pu8N;    /**< Modulus n value. */
    VLT_U16 u16ELen; /**< Length of public exponent e in bytes. */
    VLT_PU8 pu8E;    /**< Public exponent e. */
#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)
    VLT_U8 u8Assurance; /**< RSA public key assurance value. */
#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */
};
/**
 * \typedef VLT_KEY_OBJ_RSA_PUB
 *
 * \brief The type definition of the structure _VltRsaPublicKeyObject
 */
typedef struct _VltRsaPublicKeyObject VLT_KEY_OBJ_RSA_PUB;

/**
 * \struct _VltRsaPrivateKeyObject
 *
 * \brief An RSA private key object.
 */
struct _VltRsaPrivateKeyObject
{
    VLT_U8  u8Mask;  /**< Mask value. Zero if masking is not required. */
    VLT_U16 u16NLen; /**< Length of modulus n value in bytes. */
    VLT_PU8 pu8N;    /**< Modulus n value. */
    VLT_U16 u16DLen; /**< Length of private exponent d in bytes. */
    VLT_PU8 pu8D;    /**< Public exponent d. */
#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)
    VLT_U8 u8PublicKeyGroup; /**< RSA public key file group. */
    VLT_U8 u8PublicKeyIndex; /**< RSA public key file index. */
#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */
};
/**
 * \typedef VLT_KEY_OBJ_RSA_PRIV
 *
 * \brief The type definition of the structure _VltRsaPrivateKeyObject
 */
typedef struct _VltRsaPrivateKeyObject VLT_KEY_OBJ_RSA_PRIV;

/**
 * \struct _VltRsaPrivateCrtKeyObject
 *
 * \brief An RSA CRT private key object.
 */
struct _VltRsaPrivateCrtKeyObject
{
    VLT_U8  u8Mask;  /**< Mask value. Zero if masking is not required. */
    VLT_U16 u16PLen; /**< Prime length in bytes. */
    VLT_PU8 pu8P;    /**< Prime factor p. */
    VLT_PU8 pu8Q;    /**< Prime factor q. */
    VLT_PU8 pu8Dp;   /**< dp value. */
    VLT_PU8 pu8Dq;   /**< dq value. */
    VLT_PU8 pu8Ip;   /**< ip value. */
#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)
    VLT_U8 u8PublicKeyGroup; /**< RSA public key file group. */
    VLT_U8 u8PublicKeyIndex; /**< RSA public key file index. */
#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)*/

};
/**
 * \typedef VLT_KEY_OBJ_RSA_PRIV_CRT
 *
 * \brief The type definition of the structure _VltRsaPrivateCrtKeyObject
 */
typedef struct _VltRsaPrivateCrtKeyObject VLT_KEY_OBJ_RSA_PRIV_CRT;

/**
 * \struct _VltDsaPublicKeyObject
 *
 * \brief A DSA public key object.
 */
struct _VltDsaPublicKeyObject
{
    VLT_U16 u16YLen;    /**< Length of public key y in bytes. */
    VLT_PU8 pu8Y;       /**< Public key y. */

#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X )

    VLT_U16 u16PathLen; /**< Length of DSA parameter object path length. */
    VLT_PU8 pu8sParams; /**< Path to DSA parameter object file. */

#endif /* ( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X ) */

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)

    VLT_U8 u8DomainParamsGroup; /**< DSA domain parameter file group. */
    VLT_U8 u8DomainParamsIndex; /**< DSA domain parameter file index. */
    VLT_U8 u8Assurance; /**< DSA public key assurance value. */

#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */

};
/**
 * \typedef VLT_KEY_OBJ_DSA_PUB
 *
 * \brief The type definition of the structure _VltDsaPublicKeyObject
 */
typedef struct _VltDsaPublicKeyObject VLT_KEY_OBJ_DSA_PUB;

/**
 * \struct _VltDsaPrivateKeyObject
 *
 * \brief A DSA private key object.
 */
struct _VltDsaPrivateKeyObject
{
    VLT_U8  u8Mask;     /**< Mask value. Zero if masking is not required. */
    VLT_U16 u16XLen;    /**< Length of private key x in bytes. */
    VLT_PU8 pu8X;       /**< Private key x. */

#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X )

    VLT_U16 u16PathLen; /**< Length of DSA parameter object path length. */
    VLT_PU8 pu8sParams; /**< Path to DSA parameter object file. */

#endif /* ( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X ) */

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)

    VLT_U8 u8DomainParamsGroup; /**< DSA domain parameter file group. */
    VLT_U8 u8DomainParamsIndex; /**< DSA domain parameter file index. */
    VLT_U8 u8PublicKeyGroup; /**< DSA public key file group. */
    VLT_U8 u8PublicKeyIndex; /**< DSA public key file index. */

#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */
};
/**
 * \typedef VLT_KEY_OBJ_DSA_PRIV
 *
 * \brief The type definition of the structure _VltDsaPrivateKeyObject
 */
typedef struct _VltDsaPrivateKeyObject VLT_KEY_OBJ_DSA_PRIV;

/**
 * \struct _VltEcdsaPublicKeyObject
 *
 * \brief An ECDSA public key object.
 */
struct _VltEcdsaPublicKeyObject
{
    VLT_U16 u16QLen;    /**< Length of EC prime in bytes. */
    VLT_PU8 pu8Qx;      /**< EC public key Qx. */
    VLT_PU8 pu8Qy;      /**< EC public key Qy. */

#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X )

    VLT_U16 u16PathLen; /**< Length of ECDSA parameter object path length. */
    VLT_PU8 pu8sParams; /**< Path to ECDSA parameter object file. */

#endif /* ( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X ) */

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)

    VLT_U8 u8DomainParamsGroup; /**< DSA domain parameter file group. */
    VLT_U8 u8DomainParamsIndex; /**< DSA domain parameter file index. */
    VLT_U8 u8Assurance; /**< DSA public key assurance value. */

#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */
};
/**
 * \typedef VLT_KEY_OBJ_ECDSA_PUB
 *
 * \brief The type definition of the structure _VltEcdsaPublicKeyObject
 */
typedef struct _VltEcdsaPublicKeyObject VLT_KEY_OBJ_ECDSA_PUB;

/**
 * \struct _VltEcdsaPrivateKeyObject
 *
 * \brief An ECDSA private key object.
 */
struct _VltEcdsaPrivateKeyObject
{
    VLT_U8  u8Mask;     /**< Mask value. Zero if masking is not required. */
    VLT_U16 u16DLen;    /**< Length of EC prime in bytes. */
    VLT_PU8 pu8D;       /**< EC private key d. */

#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X )

    VLT_U16 u16PathLen; /**< Length of ECDSA parameter object path length. */
    VLT_PU8 pu8sParams; /**< Path to ECDSA parameter object file. */

#endif /* ( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X ) */

#if ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)

    VLT_U8 u8DomainParamsGroup; /**< DSA domain parameter file group. */
    VLT_U8 u8DomainParamsIndex; /**< DSA domain parameter file index. */
    VLT_U8 u8PublicKeyGroup; /**< DSA public key file group. */
    VLT_U8 u8PublicKeyIndex; /**< DSA public key file index. */

#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */

};
/**
 * \typedef VLT_KEY_OBJ_ECDSA_PRIV
 *
 * \brief The type definition of the structure _VltEcdsaPrivateKeyObject
 */
typedef struct _VltEcdsaPrivateKeyObject VLT_KEY_OBJ_ECDSA_PRIV;

#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_2_1 )
/**
 * \struct _VltEcdhPublicKeyObject
 *
 * \brief An ECDH public key object.
 */
struct _VltEcdhPublicKeyObject
{
	VLT_U16 u16QLen;    /**< Length of EC prime in bytes. */
    VLT_PU8 pu8Qx;      /**< EC public key Qx. */
    VLT_PU8 pu8Qy;      /**< EC public key Qy. */


    VLT_U8 u8DomainParamsGroup; /**< ECC domain parameter file group. */
    VLT_U8 u8DomainParamsIndex; /**< ECC domain parameter file index. */

};
/**
 * \typedef VLT_KEY_OBJ_ECDH_PRIV
 *
 * \brief The type definition of the structure _VltEcdhPublicKeyObject
 */
typedef struct _VltEcdhPrivateKeyObject VLT_KEY_OBJ_ECDH_PRIV;

/**
 * \struct _VltEcdhPrivateKeyObject
 *
 * \brief An ECDH private key object.
 */
struct _VltEcdhPrivateKeyObject
{
	VLT_U16 u16DLen;    /**< Length of EC prime in bytes. */
    VLT_PU8 pu8D;      /**< EC private key */
	VLT_U8 u8Mask;

    VLT_U8 u8DomainParamsGroup;		/**< ECC domain parameter file group. */
    VLT_U8 u8DomainParamsIndex;		/**< ECC domain parameter file index. */
	VLT_U8 u8PublicKeyGroup;		/**<  public key file group. */
    VLT_U8 u8PublicKeyIndex;		/**<  public key file index. */
};
/**
 * \typedef VLT_ALG_OBJ_ECDH_PUB
 *
 * \brief The type definition of the structure _VltEcdhPublicKeyObject
 */
typedef struct _VltEcdhPublicKeyObject VLT_KEY_OBJ_ECDH_PUB;
#endif

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) 
/**
 * \struct _VltDsaDomainParametersKeyObject
 *
 * \brief An DSA paremeters key object.
 */
struct _VltDsaDomainParametersKeyObject
{
    VLT_U16 u16PLen;
    VLT_PU8 pu8P;
    VLT_PU8 pu8G;
    VLT_U16 u16QLen;
    VLT_PU8 pu8Q;
    VLT_U8  u8DigestId;
    VLT_U16 u16Counter;
    VLT_U16 u16SeedLen;
    VLT_PU8 pu8Seed;
    VLT_U8  u8Assurance;
};
/**
 * \typedef VLT_KEY_OBJ_DSA_PARAMS
 *
 * \brief The type definition of the structure _VltDsaDomainParametersKeyObject
 */
typedef struct _VltDsaDomainParametersKeyObject VLT_KEY_OBJ_DSA_PARAMS;
#endif /* ( VAULT_IC_VERSION == VAULTIC_VERSION_1_2_X ) */

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) 
/**
 * \struct _VltEcdsaDomainParametersKeyObject
 *
 * \brief An ECDSA paremeters key object.
 */
struct _VltEcdsaDomainParametersKeyObject
{
    VLT_U16 u16QLen;
    VLT_PU8 pu8Q;
    VLT_PU8 pu8Gx;
    VLT_PU8 pu8Gy;
    VLT_PU8 pu8Gz;
    VLT_PU8 pu8A;
    VLT_PU8 pu8B;
    VLT_U16 u16NLen;
    VLT_PU8 pu8N;
    VLT_U32 u32H;
    VLT_U8  u8Assurance;
};
/**
 * \typedef VLT_KEY_OBJ_ECDSA_PARAMS
 *
 * \brief The type definition of the structure _VltEcdsaDomainParametersKeyObject
 */
typedef struct _VltEcdsaDomainParametersKeyObject VLT_KEY_OBJ_ECDSA_PARAMS;
#endif /* ( VAULT_IC_VERSION == VAULTIC_VERSION_1_2_X ) */

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) 

/**
 * \struct _VltIdKeyObject
 *
 * \brief A Host/Device ID key object.
 */
struct _VltIdKeyObject
{
    VLT_U16 u16StringLen; /**< The ID String length*/
    VLT_PU8 pu8StringId;  /**< The ID String */
};

/**
 * \typedef VLT_KEY_OBJ_ID
 *
 * \brief The type definition of the structure _VltIdKeyObject
 */
typedef struct _VltIdKeyObject VLT_KEY_OBJ_ID;

#endif/*( VAULT_IC_VERSION == VAULTIC_VERSION_1_2_X )*/

/**
 * \struct _VltKeyObject
 *
 * \brief A key Object for use with VltPutKey().
 */
struct _VltKeyObject
{
    /** Key identifier. See VLT_KEY_*. */
    VLT_U8 u8KeyID;

    /** Union of key object structures */
    union _kodata
    {
        /**
         * The Raw Key Structure, any type of key 
         * can be treated as a raw key.
         */        
        VLT_KEY_OBJ_RAW          RawKey;
        /**
         * The Secret Key Structure.
         */ 
        VLT_KEY_OBJ_SECRET       SecretKey;
        /**
         * The HOTP Key Structure.
         */ 
        VLT_KEY_OBJ_HOTP         HotpKey;
        /**
         * The TOTP Key Structure.
         */ 
        VLT_KEY_OBJ_TOTP         TotpKey;
        /**
         * The Public RSA Key Structure.
         */ 
        VLT_KEY_OBJ_RSA_PUB      RsaPubKey;
        /**
         * The Private RSA Key Structure.
         */ 
        VLT_KEY_OBJ_RSA_PRIV     RsaPrivKey;
        /**
         * The Private CRT RSA Key Structure.
         */ 
        VLT_KEY_OBJ_RSA_PRIV_CRT RsaPrivCrtKey;
        /**
         * The Public DSA Key Structure.
         */ 
        VLT_KEY_OBJ_DSA_PUB      DsaPubKey;
        /**
         * The Private DSA Key Structure.
         */ 
        VLT_KEY_OBJ_DSA_PRIV     DsaPrivKey;
        /**
         * The Public ECDSA Key Structure.
         */ 
        VLT_KEY_OBJ_ECDSA_PUB    EcdsaPubKey;
        /**
         * The Private ECDSA Key Structure.
         */ 
        VLT_KEY_OBJ_ECDSA_PRIV   EcdsaPrivKey;
#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) 
        /**
         * The DSA Parameters Key Structure
         */
        VLT_KEY_OBJ_DSA_PARAMS DsaParamsKey;

        /**
         * The ECDSA Parameters Key Structure
         */
        VLT_KEY_OBJ_ECDSA_PARAMS EcdsaParamsKey;

        /**
         * The Device ID Key Structure
         */
        VLT_KEY_OBJ_ID HostDeviceIdKey;
#endif //VAULTIC_VERSION_1_2_X
    }
    data;
};

/**
 * \typedef VLT_KEY_OBJECT
 *
 * \brief The type definition of the structure _VltKeyObject
 */
typedef struct _VltKeyObject VLT_KEY_OBJECT;


/* --------------------------------------------------------------------------
 * GENERATE ASSURANCE MESSAGE STRUCTURE
 * --------------------------------------------------------------------------
 */
/**
 * \struct _VltAssuranceMessage
 *
 * \brief The parameter structured used for the  VltGenerateAssuranceMessage().
 */
struct _VltAssuranceMessage
{
    /**
     * Verifier ID pointer 
     * VLT_GA_VERIFIER_ID_LENGTH defines the required length.
     */
    VLT_PU8  pu8VerifierID;
    /**
     * Verifier ID Length
     * [IN] The host Verifier ID buffer size.
     * [OUT] The length of the received Verifier ID.
     */
    VLT_U8 u8VerifierIdLength;
    /**
     * Assurance Message ponter
     * VLT_GA_MESSAGE_LENGTH defines the required length.
     */
    VLT_PU8  pu8AssuranceMessage;
    /**
     * Assurance Message Length
     * [IN] The host Assurance Message buffer size.
     * [OUT] The length of the received Verifier ID.
     */
    VLT_U8 u8AssuranceMessageLength;
};

/**
 * \typedef VLT_ASSURANCE_MESSAGE
 *
 * \brief The type definition of the structure _VltAssuranceMessage
 */
typedef struct _VltAssuranceMessage VLT_ASSURANCE_MESSAGE;


/* --------------------------------------------------------------------------
 * GENERATE KEY PAIR STRUCTURES
 * --------------------------------------------------------------------------
 */

/**
 * \struct _VltGenerateRsaKeyPairData
 *
 * \brief RSA Key Generator parameters.
 */
struct _VltGenerateRsaKeyPairData
{
    /** 0 for private exponent, or 1 for CRT */
    VLT_U8  u8Option;

    /** Modulus length, in bytes.
     * Shall be a multiple of 4 bytes. */
    VLT_U16 u16Length;

    /** Public exponent length, in bytes.
     * Shall be a multiple of 4 bytes. If not pad the exponent value with zero
     * bytes on the MSB side. */
    VLT_U16 u16ELen;

    /** Public exponent 'e'.
     * An odd positive integer such that 2**16 < e < * 2**256.
     */
    VLT_PU8 pu8e;
};
/**
 * \typedef VLT_KEY_GEN_RSA_DATA
 *
 * \brief The type definition of the structure _VltGenerateRsaKeyPairData
 */
typedef struct _VltGenerateRsaKeyPairData VLT_KEY_GEN_RSA_DATA;

/**
 * \struct _VltGenerateEcdsaDsaKeyPairData
 *
 * \brief DSA or ECDSA Key Generator parameters.
 */
struct _VltGenerateEcdsaDsaKeyPairData
{
#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X )

    /** Domain parameters path length */
    VLT_U16 u16PathLength;

    /** Path to DSA Domain Parameters, or ECDSA Domain Parameters file */
    VLT_PU8 pu8DomainPath; 

#endif /* ( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X ) */

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) 

    /** Domain Parameters Group */
    VLT_U8 u8DomainParamsGroup;

    /** Domain Parameters Index */
    VLT_U8 u8DomainParamsIndex;

#endif /*(VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) ) */
};
/**
 * \typedef VLT_KEY_GEN_ECDSA_DSA_DATA
 *
 * \brief The type definition of the structure _VltGenerateEcdsaDsaKeyPairData
 */
typedef struct _VltGenerateEcdsaDsaKeyPairData VLT_KEY_GEN_ECDSA_DSA_DATA;

/**
 * \struct _VltGenerateKeyPairData
 *
 * \brief Abstract Generate Key Pair parameter structure.
 */
struct _VltGenerateKeyPairData
{
    /** Key pair generator identifier. See VLT_ALG_KPG_*. */
    VLT_U8 u8AlgoID;

    /** Union of key pair generator parameter objects. */
    union _gkpdata
    {
        /**
         * The RSA specific Generate Key Pair structure
         */
        VLT_KEY_GEN_RSA_DATA       RsaKeyGenObj;
        /**
         * The ECDSA/DSA specific Generate Key Pair structure
         */
        VLT_KEY_GEN_ECDSA_DSA_DATA EcdsaDsaKeyGenObj;
    }
    data;
};
/**
 * \typedef VLT_KEY_GEN_DATA
 *
 * \brief The type definition of the structure _VltGenerateKeyPairData
 */
typedef struct _VltGenerateKeyPairData VLT_KEY_GEN_DATA;

/* --------------------------------------------------------------------------
 * SELECT FILE OR DIRECTORY STRUCTURES
 * -------------------------------------------------------------------------- */

/**
 * \struct _VltSelectFileRespData
 *
 * \brief Response data from VltSelectFileOrDirectory().
 */
struct _VltSelectFileRespData
{
    /** File size.
     * This Reflects the length of data present in the file, and not the file
     * system space allocated to this file, which may be greater. */
    VLT_U32             u32FileSize;

    /** Access conditions */
    VLT_FILE_PRIVILEGES FileAccess;

    /** Attributes */
    VLT_U8              u8FileAttribute;
};
/**
 * \typedef VLT_SELECT
 *
 * \brief The type definition of the structure _VltSelectFileRespData
 */
typedef struct _VltSelectFileRespData VLT_SELECT;

/* --------------------------------------------------------------------------
 * GET INFO STRUCTURES
 * -------------------------------------------------------------------------- */

/**
 * \struct _VltTargetInformationRespData
 *
 * \brief Response data from VltGetInfo.
 */
struct _VltTargetInformationRespData
{
    /** Firmware version */
    VLT_U8  au8Firmware[VLT_FIRMWARE_VERSION_LENGTH];
    /** Device serial number */
    VLT_U8  au8Serial[VLT_CHIP_SERIAL_NUMBER_LENGTH];
    /** Current Life Cycle state */
    VLT_U8  u8State;
    /** Self-tests result */
    VLT_U8  u8SelfTests;
    /** Current authenticated Role ID */
    VLT_U8  u8Role;
    /** Current mode of operation:
     *
     * - 0x00: Idle mode 
     * - 0x01: Approved mode
     * - 0x02: Non-approved mode
     * - 0x03: Personalization mode
     * - 0x04: Maintenance mode
     */
#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) 
    VLT_U8  u8Mode;
#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) ) */
    /** Available bytes in the file system */
    VLT_U32 u32Space;
    /** Tamper attack indicator */
    VLT_U8  u8Attack;
    /** Tamper attack counter */
    VLT_U16 u16AttackCounter;
    /* Current date and time in ISO8601 format: YYYY-MM-DDThh:mm:ssZ */
    VLT_U8  au8Date[VLT_FIRMWARE_DATE_LENGTH];
};
/**
 * \typedef VLT_TARGET_INFO
 *
 * \brief The type definition of the structure _VltTargetInformationRespData
 */
typedef struct _VltTargetInformationRespData VLT_TARGET_INFO;

/* --------------------------------------------------------------------------
 * INTERNAL STRUCTURES
 * -------------------------------------------------------------------------- */

/**
 * \struct _VltMemBlob
 *
 * \brief A buffer.
 * \todo Hide this definition in an internal header.
 */
struct _VltMemBlob
{
    VLT_U16 u16Len;      /**< Length of the buffer, in bytes. */
    VLT_U16 u16Capacity; /**< Capacity of the buffer, in bytes. */
    VLT_PU8 pu8Data;     /**< The buffer itself. */
};
/**
 * \typedef VLT_MEM_BLOB
 *
 * \brief The type definition of the structure _VltMemBlob
 */
typedef struct _VltMemBlob VLT_MEM_BLOB;

/**
 * \struct _keyBlob
 *
 * \brief A key object for use by vaultic_cipher and vaultic_signer, do not use
 * this for API functions.
 * \todo Hide this definition in an internal header.
 */
struct _keyBlob
{
    VLT_U8  keyType;
    VLT_U16 keySize;
    VLT_PU8 keyValue;
};
/**
 * \typedef KEY_BLOB
 *
 * \brief The type definition of the structure _keyBlob
 */
typedef struct _keyBlob KEY_BLOB;

/**
 * \struct _VltWrapParams
 * 
 * \brief The Key Wrapping parameters. 
 * 
 * \par Description:
 * The _VltWrapParams structure is used to configure the key wrapping service. Its members are:
 * \li algoID Specifies which cipher will be used.
 * \li paddingScheme Specifies which padding mode will be used.
 * \li chainMode Specifies which chain mode will be used.
 * \li pIV An array of characters used as the Initialisation Vector.
 */
struct _VltWrapParams
{
    VLT_U8 algoID;
    VLT_U8 paddingScheme;
    VLT_U8 chainMode;
    VLT_PU8 pIV;
};

/**
 * \typedef WRAP_PARAMS
 *
 * \brief The type definition of the structure _VltWrapParams
 */
typedef struct _VltWrapParams WRAP_PARAMS;


/**
 * \struct _KeyBlobArray
 * 
 * \brief The Key Blob Array parameters. 
 * 
 * \par Description:
 * The _VltWrapParams structure is used to configure the identity authentication service. Its members are:
 * \li u8ArraySize Specifies the number of keys being passed.
 * \li pKeys pointer to an array of type KEY_BLOB.
 */
struct _KeyBlobArray {
    VLT_U8 u8ArraySize;
    KEY_BLOB* pKeys[VLT_MANAGE_AUTH_DATA_KEYLEN_FIELD_LENGTH];
};


/**
 * \typedef KEY_BLOB_ARRAY
 *
 * \brief The type definition of the structure _KeyBlobArray
 */
typedef struct _KeyBlobArray KEY_BLOB_ARRAY;

/**
 * \struct _VltLibraryInfo
 * 
 * \brief The VaultIC library information 
 * 
 * \par Description:
 * The _VltLibraryInfo structure is used to return the properties of the VaultIC API Library. 
 * Its members are:
 * 
 * \li pVersion A pointer to a null terminated string of characters that denotes the version of the library.
 * \li capabilities A bitfield of the capabilities supported by the library build. Possible values are:
 *  - #VLT_CPB_ENABLE_KEY_SECRET    
 *  - #VLT_CPB_ENABLE_KEY_HOTP      
 *  - #VLT_CPB_ENABLE_KEY_TOTP      
 *  - #VLT_CPB_ENABLE_KEY_RSA       
 *  - #VLT_CPB_ENABLE_KEY_DSA       
 *  - #VLT_CPB_ENABLE_KEY_ECDSA     
 *  - #VLT_CPB_ENABLE_CIPHER_TESTS  
 *  - #VLT_CPB_ENABLE_SCP02         
 *  - #VLT_CPB_ENABLE_SCP03         
 *  - #VLT_CPB_ENABLE_BLOCK_PROTOCOL
 *  - #VLT_CPB_ENABLE_ISO7816       
 *  - #VLT_CPB_ENABLE_TWI           
 *  - #VLT_CPB_ENABLE_SPI           
 *  - #VLT_CPB_ENABLE_CIPHER_AES    
 *  - #VLT_CPB_ENABLE_CIPHER_TDES   
 *  - #VLT_CPB_ENABLE_CIPHER_DES    
 *  - #VLT_CPB_ENABLE_FAST_CRC16CCIT
 *  - #VLT_CPB_ENABLE_KEY_WRAPPING  
 *  - #VLT_CPB_ENABLE_MS_AUTH       
 *  - #VLT_CPB_ENABLE_IDENTITY_AUTH 
 * \li reserved A bitfiled reserved for future use, set to 0.
 * \li fwCompatibilityVersion A value that signifies the version compatibility of the library with the VaultIC.
 *  - 1 = Library is compatible with version 1.0.2 of the VaultIC
 *  - 2 = Library is compatible with version 1.1.0 of the VaultIC
 */
struct _VltLibraryInfo
{
    VLT_PU8 pVersion;
    VLT_U32 capabilities;
    VLT_U32 reserved;
    VLT_U8 fwCompatibilityVersionId;
};

/**
 * \typedef VLT_LIBRARY_INFO
 *
 * \brief The type definition of the structure _VltLibraryInfo
 */
typedef struct _VltLibraryInfo VLT_LIBRARY_INFO;

/**
 * \struct _VltFsEntryAttributes
 * \brief The attributes of the file system entry
 */
struct _VltFsEntryAttributes
{
     /**
      * Specifies that file system entry cannot be written to 
      */
    VLT_UINT readOnly : 1;

    /**
      * Specifies that file system entry is a system entry 
      */
    VLT_UINT system : 1;

    /**
      * Specifies that file system entry is hidden 
      */
    VLT_UINT hidden : 1;
};

typedef struct _VltFsEntryAttributes VLT_FS_ENTRY_ATTRIBS;

/**
 * \struct _VltUserAccess
 * 
 * \brief User access for the different users of the system
 * 
 * \par Description:
 * The _VltUserAccess structure holds information on file sytem entry acces
 * for different users
 */
struct _VltUserAccess
{
    VLT_UINT user0 : 1;
    VLT_UINT user1 : 1;
    VLT_UINT user2 : 1;
    VLT_UINT user3 : 1;
    VLT_UINT user4 : 1;
    VLT_UINT user5 : 1;
    VLT_UINT user6 : 1;
    VLT_UINT user7 : 1;
};

typedef struct _VltUserAccess VLT_USER_ACCESS;

/**
 * \struct _VltFsEntryPrivileges
 * 
 * \brief Privileges on a File System entry for Read, Write, Delete and Execute
 * 
 * \par Description:
 * The _VltFsEntryPrivileges structure holds information on file sytem entries regarding
 * the access privileges
 */
struct _VltFsEntryPrivileges
{
    VLT_USER_ACCESS readPrivilege;

    VLT_USER_ACCESS writePrivilege;

    VLT_USER_ACCESS deletePrivilege;

    VLT_USER_ACCESS executePrivilege;
    
};

typedef struct _VltFsEntryPrivileges VLT_FS_ENTRY_PRIVILEGES;


/**
 * \struct _VltFsEntryParams
 * 
 * \brief Parameters for entries within the fie system used by the File System
 * service
 * 
 * \par Description:
 * The _VltFsEntryParams structure holds information on file sytem entries for
 * the File System service
 */
struct _VltFsEntryParams
{
    /**
     * The file system entry privileges
     */
    VLT_FS_ENTRY_PRIVILEGES filePriv;

    /**
     *  Size of the entry in bytes.  Folders are zero bytes in size
     */
    VLT_U32 u32FileSize;

    /**
     * The type of entry either #VLT_FILE_ENTRY or #VLT_FOLDER_ENTRY
     */
    VLT_U8 u8EntryType;

    /**
     * The attributes for the file system entry
     */
    VLT_FS_ENTRY_ATTRIBS attribs;
};

typedef struct _VltFsEntryParams VLT_FS_ENTRY_PARAMS;

/**
 * \struct _VltDelayPairing
 * 
 * \brief Allows the specification of a delay before a byte is transmitted
 * 
 * \par Description:
 * The _VltDelayPairing structure holds information on the byte position
 * to add a delay before the byte is transmitted
 */
struct _VltDelayPairing
{
    /*
     * The byte position to place the delay before transmission of the byte
     */
    VLT_U16 u16BytePos;
    
    /*
     * The delay time in microseconds
     */
    VLT_U32 u32DelayTime;
};

typedef struct _VltDelayPairing VLT_DELAY_PAIRING;


/*@}*/
#endif /* VAULTIC_STRUCTS_H */
