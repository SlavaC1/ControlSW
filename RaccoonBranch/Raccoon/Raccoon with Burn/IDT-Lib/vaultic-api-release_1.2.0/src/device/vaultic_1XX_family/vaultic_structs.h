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

#ifndef WIN32
#include <PCSC/wintypes.h>
#include <PCSC/pcsclite.h>
#include <PCSC/winscard.h>
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
   //     		/**
   //      * \struct _VltPcscInitParams
   //      * \brief Specific PCSC comms parameters, 
   //      * particularly applicable to ISO7816 T0 and T1          
   //      */
   //     struct _VltPcscInitParams
   //     {
   //         /**
   //          * String descriptor of the ISO 7816 card reader  
   //          * attached to the host.
   //          */
   //         VLT_PU8 pu8ReaderString;

            ///* VaultIC Handle for minidriver*/
            //SCARDHANDLE hCard;

            ///* PC/SC system handle */
            //SCARDCONTEXT hContext;

            //VLT_U32 u32Protocol;
   //     }
   //     VltPcscInitParams;
        
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
 * \typedef VLT_TWI_PARAMS
 *
 * \brief The type definition of the structure _VltTwiParams
 */
typedef struct _VltTwiParams VLT_TWI_PARAMS;





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
     * - #VLT_LOGIN_STRONG
     */
    VLT_U8 u8Method;

    /** 
     * Operator ID :
     * - #VLT_CREATOR 
     * - #VLT_USER
     */
    VLT_U8 u8UserID;

    /** 
     * Role :
     * - #VLT_APPROVED_USER 
     * - #VLT_MANUFACTURER
     */
    VLT_U8 u8RoleID;

    /** Maximum allowed consecutive authentication failures (1 to 127) */
    VLT_U8 u8TryCount;
    
    /**
     * \struct _cleartext 
     * \brief The specialised password based authentication data.
     */
    struct _cleartext
    {
        /** Cleartext password length (32) */
        VLT_U8 u8PasswordLength;
        /** Cleartext password */
        VLT_U8 u8Password[32];
    }
    cleartext;

        
    
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
     * - #VLT_LOGIN_STRONG
     */
    VLT_U8  u8AuthMethod;

    /** 
     * Bit-field of roles assumed by the operator:
     * - #VLT_APPROVED_USER     
     * - #VLT_MANUFACTURER      
     * - #VLT_EVERYONE   
     */
    VLT_U8  u8Roles;

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
 * INITIALIZE ALGORITHM STRUCTURES
 * -------------------------------------------------------------------------- */




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
     * - #VLT_ALG_SIG_ECDSA_GF2M
     * - #VLT_ALG_DIG_SHA256
     */
    VLT_U8 u8AlgoID;

    /**
     * The ECDSA DSA structure
     */
    VLT_ALG_OBJ_ECDSA_DSA   EcdsaDsa;        
};

/**
 * \typedef VLT_ALGO_PARAMS
 *
 * \brief The type definition of the structure _VltAlgoParamsObj
 */
typedef struct _VltAlgoParamsObj VLT_ALGO_PARAMS;



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
    VLT_PU16 pu16KeyObjectLen;
    VLT_PU8 pu8KeyObject;
    VLT_U16 u16Crc;
};
/**
 * \typedef VLT_KEY_OBJ_RAW
 *
 * \brief The type definition of the structure _VltRawKey
 */
typedef struct _VltRawKey VLT_KEY_OBJ_RAW;



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

    VLT_U8 u8DomainParamsGroup; /**< DSA domain parameter file group. */
    VLT_U8 u8DomainParamsIndex; /**< DSA domain parameter file index. */
    VLT_U8 u8Assurance; /**< DSA public key assurance value. */
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

    VLT_U8 u8DomainParamsGroup; /**< DSA domain parameter file group. */
    VLT_U8 u8DomainParamsIndex; /**< DSA domain parameter file index. */
    VLT_U8 u8PublicKeyGroup; /**< DSA public key file group. */
    VLT_U8 u8PublicKeyIndex; /**< DSA public key file index. */
};
/**
 * \typedef VLT_KEY_OBJ_ECDSA_PRIV
 *
 * \brief The type definition of the structure _VltEcdsaPrivateKeyObject
 */
typedef struct _VltEcdsaPrivateKeyObject VLT_KEY_OBJ_ECDSA_PRIV;


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
         * The Public ECDSA Key Structure.
         */ 
        VLT_KEY_OBJ_ECDSA_PUB    EcdsaPubKey;
        /**
         * The Private ECDSA Key Structure.
         */ 
        VLT_KEY_OBJ_ECDSA_PRIV   EcdsaPrivKey;
        /**
         * The ECDSA Parameters Key Structure
         */
        VLT_KEY_OBJ_ECDSA_PARAMS EcdsaParamsKey;
        /**
         * The Device ID Key Structure
         */
        VLT_KEY_OBJ_ID HostDeviceIdKey;
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
 * \struct _VltGenerateEcdsaKeyPairData
 *
 * \brief ECDSA Key Generator parameters.
 */
struct _VltGenerateEcdsaKeyPairData
{
    /** Domain Parameters Group */
    VLT_U8 u8DomainParamsGroup;

    /** Domain Parameters Index */
    VLT_U8 u8DomainParamsIndex;
};
/**
 * \typedef VLT_KEY_GEN_ECDSA_DSA_DATA
 *
 * \brief The type definition of the structure _VltGenerateEcdsaKeyPairData
 */
typedef struct _VltGenerateEcdsaKeyPairData VLT_KEY_GEN_ECDSA_DATA;




/**
 * \struct _VltGenerateKeyPairData
 *
 * \brief Abstract Generate Key Pair parameter structure.
 */
struct _VltGenerateKeyPairData
{
    /** Key pair generator identifier. See VLT_ALG_KPG_*. */
    VLT_U8 u8AlgoID;

    /**
     * The ECDSA/DSA specific Generate Key Pair structure
     */
    VLT_KEY_GEN_ECDSA_DATA EcdsaKeyGenObj;

};
/**
 * \typedef VLT_KEY_GEN_DATA
 *
 * \brief The type definition of the structure _VltGenerateKeyPairData
 */
typedef struct _VltGenerateKeyPairData VLT_KEY_GEN_DATA;



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
     * - 0x01: ACTIVATED and in FIPS mode
     * - 0x02: ACTIVATED and not in FIPS mode
     * - 0x03: CREATION state
     * - 0x04: TERMINATED state
     */
    VLT_U8  u8Mode;

    /** Available bytes in the file system */
    VLT_U32 u32Space;
    /** Tamper attack indicator */
    VLT_U8  u8Attack;
    /** Tamper attack counter */
    VLT_U16 u16AttackCounter;    
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
 * \struct _KeyBlobArray
 * 
 * \brief The Key Blob Array parameters. 
 * 
 * \par Description:
 * The _KeyBlobArray structure is used to configure the identity authentication service. Its members are:
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
 *  - 1 = Library is compatible with version 1.0.X of the VaultIC
 *  - 2 = Library is compatible with version 1.2.0 of the VaultIC
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






/* --------------------------------------------------------------------------
 * INITIALIZE CRYPTO SERVICE ALGORITHMIC STRUCTURES
 * -------------------------------------------------------------------------- */

/**
 * \struct _CryptoServiceSymmetricCipherAlgorithmParameters
 *
 * \brief Symmetric Cipher parameters.
 */
struct _CryptoServiceSymmetricCipherAlgorithmParameters
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
 * \typedef CRYPTO_SERVICE_ALG_OBJ_SYM_CIPHER
 *
 * \brief The type definition of the structure _CryptoServiceSymmetricCipherAlgorithmParameters
 */
typedef struct _CryptoServiceSymmetricCipherAlgorithmParameters CRYPTO_SERVICE_ALG_OBJ_SYM_CIPHER;





/**
 * \struct _CryptoServiceCMacSignerAlgorithmParameters
 *
 * \brief CMAC Signer parameters.
 */
struct _CryptoServiceCMacSignerAlgorithmParameters
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
 * \typedef CRYPTO_SERVICE_ALG_OBJ_CMAC
 *
 * \brief The type definition of the structure _CryptoServiceCMacSignerAlgorithmParameters
 */
typedef struct _CryptoServiceCMacSignerAlgorithmParameters CRYPTO_SERVICE_ALG_OBJ_CMAC;





/**
 * \struct _CryptoServiceEcdsaAlgorithmParameters
 *
 * \brief ECDSA signer parameters.
 */
struct _CryptoServiceEcdsaAlgorithmParameters
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
 * \typedef CRYPTO_SERVICE_ALG_OBJ_ECDSA
 *
 * \brief The type definition of the structure _CryptoServiceEcdsaAlgorithmParameters
 */
typedef struct _CryptoServiceEcdsaAlgorithmParameters CRYPTO_SERVICE_ALG_OBJ_ECDSA;




/**
 * \struct _CryptoServiceAlgoParamsObj
 *
 * \brief Algorithm parameters.
 *
 * \par Description:
 * Used by VltInitializeAlgorithm.
 */
struct _CryptoServiceAlgoParamsObj
{
    /** 
     * Algorithm identifier. An identifier for a signer, 
     * digest, key transport
     * scheme or cipher. Possible values are: 
     * - #VLT_ALG_SIG_MAC_ISO9797_ALG1
     * - #VLT_ALG_SIG_MAC_ISO9797_ALG3
     * - #VLT_ALG_SIG_CMAC_AES
     * - #VLT_ALG_SIG_ECDSA_GF2M
     */
    VLT_U8 u8AlgoID;

    /** Union of algorithm parameter structures */
    union _crypto_service_apdata
    {
        /**
         * The CMAC structure
         */
        CRYPTO_SERVICE_ALG_OBJ_CMAC        Cmac;
        /**
         * The ECDSA DSA structure
         */
        CRYPTO_SERVICE_ALG_OBJ_ECDSA   Ecdsa;
    }
    cs_data;
};

/**
 * \typedef CRYPTO_SERVICE_ALGO_PARAMS
 *
 * \brief The type definition of the structure _CryptoServiceAlgoParamsObj
 */
typedef struct _CryptoServiceAlgoParamsObj CRYPTO_SERVICE_ALGO_PARAMS;







/* --------------------------------------------------------------------------
 * CRYPTO SERVICE KEY OBJECT STRUCTURES
 * -------------------------------------------------------------------------- */

/**
 * \struct CRYPTO_SERVICE_KEY_OBJ_SECRET
 *
 * \brief A secret key object (e.g. DES, TDES, AES, HMAC).
 */
struct _CryptoServiceSecretKeyObject
{
    VLT_U16 u16KeyLength; /**< Secret key length in bytes. */
    VLT_PU8 pu8Key;       /**< Secret key value. */
};
/**
 * \typedef CRYPTO_SERVICE_KEY_OBJ_SECRET
 *
 * \brief The type definition of the structure _CryptoServiceSecretKeyObject
 */
typedef struct _CryptoServiceSecretKeyObject CRYPTO_SERVICE_KEY_OBJ_SECRET;



/**
 * \struct _CryptoServiceEcdsaDomainParametersKeyObject
 *
 * \brief An ECDSA paremeters key object.
 */
struct _CryptoServiceEcdsaDomainParametersKeyObject
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
};
/**
 * \typedef CRYPTO_SERVICE_KEY_OBJ_ECDSA_PARAMS
 *
 * \brief The type definition of the structure _CryptoServiceEcdsaDomainParametersKeyObject
 */
typedef struct _CryptoServiceEcdsaDomainParametersKeyObject CRYPTO_SERVICE_KEY_OBJ_ECDSA_PARAMS;





/**
 * \struct _VltEcdsaPublicKeyObject
 *
 * \brief An ECDSA public key object.
 */
struct _CryptoServiceEcdsaPublicKeyObject
{
    VLT_U16 u16QLen;    /**< Length of EC prime in bytes. */
    VLT_PU8 pu8Qx;      /**< EC public key Qx. */
    VLT_PU8 pu8Qy;      /**< EC public key Qy. */
    CRYPTO_SERVICE_KEY_OBJ_ECDSA_PARAMS Params; /**<ECDSA domain params> */    
};
/**
 * \typedef CRYPTO_SERVICE_KEY_OBJ_ECDSA_PUB
 *
 * \brief The type definition of the structure _CryptoServiceEcdsaPublicKeyObject
 */
typedef struct _CryptoServiceEcdsaPublicKeyObject CRYPTO_SERVICE_KEY_OBJ_ECDSA_PUB;





/**
 * \struct _CryptoServiceEcdsaPrivateKeyObject
 *
 * \brief An ECDSA private key object.
 */
struct _CryptoServiceEcdsaPrivateKeyObject
{
    VLT_U16 u16DLen;    /**< Length of EC prime in bytes. */
    VLT_PU8 pu8D;       /**< EC private key d. */
    CRYPTO_SERVICE_KEY_OBJ_ECDSA_PARAMS Params; /**<ECDSA domain params> */
};
/**
 * \typedef CRYPTO_SERVICE_KEY_OBJ_ECDSA_PRIV
 *
 * \brief The type definition of the structure _CryptoServiceEcdsaPrivateKeyObject
 */
typedef struct _CryptoServiceEcdsaPrivateKeyObject CRYPTO_SERVICE_KEY_OBJ_ECDSA_PRIV;





/**
 * \struct _VltKeyObject
 *
 * \brief A key Object for use with CryptoInit().
 */
struct _CryptoServiceKeyObject
{
    /** Key identifier. See VLT_KEY_*. */
    VLT_U8 u8KeyID;

    /** Union of key object structures */
    union _crypto_service_kodata
    {
        /**
         * The Secret Key Structure.
         */ 
        CRYPTO_SERVICE_KEY_OBJ_SECRET       SecretKey;
        /**
         * The Public ECDSA Key Structure.
         */ 
        CRYPTO_SERVICE_KEY_OBJ_ECDSA_PUB    EcdsaPubKey;
        /**
         * The Private ECDSA Key Structure.
         */ 
        CRYPTO_SERVICE_KEY_OBJ_ECDSA_PRIV   EcdsaPrivKey;
    }
    cs_data;
};

/**
 * \typedef CRYPTO_SERVICE_KEY_OBJECT
 *
 * \brief The type definition of the structure _CryptoServiceKeyObject
 */
typedef struct _CryptoServiceKeyObject CRYPTO_SERVICE_KEY_OBJECT;






/* --------------------------------------------------------------------------
 * VAULTIC IDENTITY AUTHENTICATION STRUCTURES
 * -------------------------------------------------------------------------- */

/**
 * \struct _VltSecureChannel
 * 
 * \brief Allows channel attributes to be passed to the authentication service.
 * 
 * \par Description:
 * The structure is used to contain the arguments needed to establish  
 * secure channel authentication.
 */
struct _VltSecureChannel
{
    /**
     * User ID :
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
    *  Role ID :
    * - #VLT_APPROVED_USER     
    * - #VLT_NON_APPROVED_USER 
    * - #VLT_MANUFACTURER      
    * - #VLT_ADMINISTRATOR
    * - #VLT_NON_APPROVED_ADMINISTRATOR
    * - #VLT_EVERYONE
    */
    VLT_U8 u8RoleID;
    
    /**
     * Secure Channel Level (valid for SCP02 and SCP03) :
     * - #VLT_NO_CHANNEL
     * - #VLT_CMAC
     * - #VLT_CMAC_CENC
     * - #VLT_CMAC_RMAC
     * - #VLT_CMAC_CENC_RMAC
     * - #VLT_CMAC_CENC_RMAC_RENC
     */
    VLT_U8 u8ChannelLevel;

    /**
    * structure containing a the number of keys and a pointer 
     *   to an array of type KEY_BLOB.
     *  
     * \par Example:
     * \code 
     *  // This example is for an approved user who�s auth method is SCP03.
     *  VLT_STS status = VLT_FAIL
     *  KEY_BLOB keys[2] =
     *  {
     *      { VLT_KEY_AES_256, AES_256_KEY_SIZE, &aucSMacKey[0] },
     *      { VLT_KEY_AES_256, AES_256_KEY_SIZE, &aucSEncKey[0] }
     *  };
     *
     *  KEY_BLOB_ARRAY arrayOfKeys = 
     *  {
     *      2, // The number of keys
     *      &keys[0], // KEY_BLOB 1
     *      &keys[1]  // KEY_BLOB 2
     *  };
     * 
     */
    KEY_BLOB_ARRAY keys;
};

typedef struct _VltSecureChannel VLT_SCP_AUTH_PARAMS;



/**
 * \struct _VltMs
 * 
 * \brief Allows microsoft authentication attributes to be passed to the 
 * authentication service.
 * 
 * \par Description:
 * The structure is used to contain the arguments needed to establish  
 * microsoft authentication.
 */
struct _VltMs
{
    /**
     * User ID :
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
    *  Role ID :
    * - #VLT_APPROVED_USER     
    * - #VLT_NON_APPROVED_USER 
    * - #VLT_MANUFACTURER      
    * - #VLT_ADMINISTRATOR
    * - #VLT_NON_APPROVED_ADMINISTRATOR
    * - #VLT_EVERYONE
    */
    VLT_U8 u8RoleID;
    
    /**
     * structure KEY_BLOB is a symetric key container.
     *  
     * \par Example:
     * \code 
     *  // This example is for an approved user who�s auth method is SCP03.
     *  VLT_STS status = VLT_FAIL
     *  KEY_BLOB key = { VLT_KEY_TDES_3K, TDES_3K_KEY_SIZE, &aucKey[0] };
     * 
     */
    KEY_BLOB key;
};

typedef struct _VltMs VLT_MS_AUTH_PARAMS;


/**
 * \struct _VltStrongAuth
 * 
 * \brief Allows strong authentication attributes to be passed to the 
 * authentication service.
 * 
 * \par Description:
 * The structure is used to contain the arguments needed to establish  
 * strong mutual authentication.
 */
struct _VltStrongAuth
{
    /**
     * User ID :
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
    *  Role ID :
    * - #VLT_APPROVED_USER     
    * - #VLT_NON_APPROVED_USER 
    * - #VLT_MANUFACTURER      
    * - #VLT_ADMINISTRATOR
    * - #VLT_NON_APPROVED_ADMINISTRATOR
    * - #VLT_EVERYONE
    */
    VLT_U8 u8RoleID;

    /**
     * Algorithm object:
     */
    CRYPTO_SERVICE_ALGO_PARAMS AlgoParams;

    /**
     * Signing key Object.
     */
    CRYPTO_SERVICE_KEY_OBJECT SigningKey;

    /**
     * Verifying Key Object.
     */
    CRYPTO_SERVICE_KEY_OBJECT VerifyingKey;

    /**
     * Host Identifier String Length.
     */
    VLT_U8 u8HostIdLength;

    /**
     * Host Identifier String.
     */
    VLT_PU8 pu8HostId;

    /**
     * Device Identifier String Length.
     */
    VLT_U8 u8DeviceIdLength;

    /**
     * Device Identifier String.
     */
    VLT_PU8 pu8DeviceId;
};

typedef struct _VltStrongAuth VLT_STRONG_AUTH_PARAMS;



/**
 * \struct _VltIdentityAuth
 * 
 * \brief the identity authentication object.
 * 
 * \par Description:
 * The _VltIdentityAuth structure holds information on the type of 
 * identity authentication and information required to perform
 * the authentication.
 */
struct _VltIdentityAuth
{
    /**
     * The Authentication Method:  
     * - #VLT_LOGIN_SCP02
     * - #VLT_LOGIN_SCP03
     * - #VLT_LOGIN_MS
     * - #VLT_STRONG
     */
    VLT_U8 u8AuthMethod;
    
    union _auth_data
    {
        /**
         * Secure Channel Authentication Structure
         */
        VLT_SCP_AUTH_PARAMS SecureChannelAuth;

        /**
         * Microsoft Authentication Structure
         */
        VLT_MS_AUTH_PARAMS  MicrosoftAuth;

        /**
         * Strong Authentication Structure
         */
        VLT_STRONG_AUTH_PARAMS StrongAuth;
    }
    data;
};

typedef struct _VltIdentityAuth VLT_IDENTITY_AUTH;


/* --------------------------------------------------------------------------
 * STRONG AUTHENTICATION DATA TYPES
 * -------------------------------------------------------------------------- */

/**
 * \struct _VltEcdsaPublicKeyObject
 *
 * \brief An ECDSA public key object.
 */
struct _VltSaEcdsaPublicKey
{
    VLT_U16 u16QLen;    /* Length of EC prime in bytes. */
    VLT_PU8 pu8Qx;      /* public key X co-ordinate */
    VLT_PU8 pu8Qy;      /* public key Y co-ordinate */
};
typedef struct _VltSaEcdsaPublicKey VLT_ECDSA_PUBLIC_KEY;


/**
 * \struct _CryptoServiceEcdsaPrivateKeyObject
 *
 * \brief An ECDSA private key object.
 */
struct _VltSaEcdsaPrivateKey
{
    VLT_U16 u16DLen;    /**< Length of EC prime in bytes. */
    VLT_PU8 pu8D;       /**< EC private key d. */
};
typedef struct _VltSaEcdsaPrivateKey VLT_ECDSA_PRIVATE_KEY;


/**
 * \struct _VltEcdsaDomainParams
 *
 * \brief ECDSA domain parameters
 */
struct _VltSaEcdsaDomainParams
{
    VLT_U16 u16QLen;	/* field size in bits e.g 233 for */
    VLT_PU8 pu8Q;		/* reduction polynomial (excluding MSB) */
    VLT_PU8 pu8Gx;	    /* Gx - base generator point X co-ordinate */
    VLT_PU8 pu8Gy;		/* Gy - base generator point Y co-ordinate */
    VLT_PU8 pu8Gz;		/* Gz - base generator point Z co-ordinate: must be 1 projective co-ordinates are not supported */
    VLT_PU8 pu8A;		/* a co-efficient of defining curve equation */
    VLT_PU8 pu8B;		/* b co-efficient of defining curve equation */
    VLT_U16 u16NLen;	/* base point order length */
    VLT_PU8 pu8N;		/* n - base point order: number of points on curve that are multiples of G */
    VLT_U32 u32H;		/* h co-factor (number of points in field = n * h) */
};
typedef struct _VltSaEcdsaDomainParams VLT_ECDSA_DOMAIN_PARAMS;


typedef struct _VltSACryptoParams VLT_SA_CRYPTO_PARAMS;

/**
 * \struct VTL_SA_ECDSA_CRYPTO_PARAMS
 *
 * \brief ECDSA cryptographic params used in strong authentication
 */
struct _VltSaEcdsaCryptoParams
{
	VLT_ECDSA_DOMAIN_PARAMS domainParams;	
	VLT_ECDSA_PUBLIC_KEY hostPublicKey;
	VLT_ECDSA_PRIVATE_KEY hostPrivateKey;
	VLT_ECDSA_PUBLIC_KEY devicePublicKey;
};

/**
 * \typedef CRYPTO_SERVICE_KEY_OBJ_ECDSA_PARAMS
 *
 * \brief The type definition of the structure _CryptoServiceEcdsaDomainParametersKeyObject
 */
typedef struct _VltSaEcdsaCryptoParams VTL_SA_ECDSA_CRYPTO_PARAMS;

/**
 * \struct VLT_SA_PROTOCOL_PARAMS
 * 
 * \brief Allows strong authentication protocol information to be 
 * passed to the authentication service.
 * 
 * \par Description:
 * The structure is used to contain the arguments needed to establish  
 * strong mutual authentication.
 */
struct _VltSAProtocolParams
{
    /**
     * User ID :
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
    *  Role ID :
    * - #VLT_APPROVED_USER     
    * - #VLT_MANUFACTURER      
    * - #VLT_EVERYONE
    */
    VLT_U8 u8RoleID;

    /**
    *  Authentication Mode :
    * - #VLT_AUTH_UNILATERAL
    * - #VLT_AUTH_MUTUAL_FIPS      
    * - #VLT_AUTH_MUTUAL_NON_FIPS      
    */
    VLT_U8 u8AuthModeID;

    /**
     * Host Identifier 
     */
    VLT_U8 u8HostIdLength;
    VLT_PU8 pu8HostId;

    /**
     * Device Identifier 
     */
    VLT_U8 u8DeviceIdLength;
    VLT_PU8 pu8DeviceId;

};

typedef struct _VltSAProtocolParams VLT_SA_PROTOCOL_PARAMS;

/**
 * \struct VLT_SA_CRYPTO_PARAMS
 * 
 * \brief Allows cryptographic information to be passed to 
 *  the authentication service.
 * 
 * \par Description:
 * The structure is used to contain the arguments needed to establish  
 * strong mutual authentication.
 */
struct _VltSACryptoParams
{
    /**
     * Algortihm ID :
     * - #VLT_ALG_SIG_ECDSA_GF2M only for VaultIC 100 
     */
    VLT_U8 u8AlgID;

    union _crypto_params
    {
        /**
         * ECDSA authentication crypto data
         */
        VTL_SA_ECDSA_CRYPTO_PARAMS ecdsa;
    }
    params;

};

/**
 * \struct VLT_COUNTER_NOUNCE
 * 
 * \brief Allows cryptographic information to be passed to 
 *  the increment or decrement counter service.
 * 
 * \par Description:
 * The structure is used to contain the arguments needed to increment  
 * or decrement counter.
 */
struct _Nounce
{
	/** Nounce length (8) */
	VLT_U8 u8NounceLength;
	/** Cleartext password */
	VLT_U8 u8Nounce[VLT_NOUNCE_LENGTH];
};

typedef struct _Nounce VLT_COUNTER_NOUNCE;

/**
 * \struct VLT_COUNTER_AMOUNT
 * 
 * \brief Allows cryptographic information to be passed to 
 *  the increment or decrement counter service.
 * 
 * \par Description:
 * The structure is used to contain the arguments needed to increment  
 * or decrement counter.
 */
struct _Amount
{
	/** Nounce length (8) */
	VLT_U8 u8AmountLength;
	/** Cleartext password */
	VLT_U8 u8Amount[VLT_COUNTER_LENGTH];
};

typedef struct _Amount VLT_COUNTER_AMOUNT;

/**
 * \struct VLT_COUNTER_DATA
 * 
 * \brief Allows cryptographic information to be passed to 
 *  the increment or decrement counter service.
 * 
 * \par Description:
 * The structure is used to contain the arguments needed to increment  
 * or decrement counter.
 */
struct _counterData
{
	/** Nounce */
	VLT_COUNTER_NOUNCE nounce;
	/** Amount */
	VLT_COUNTER_AMOUNT amount;
};

typedef struct _counterData VLT_COUNTER_DATA;

/**
 * \struct VLT_COUNTER_RESPONSE
 * 
 * \brief Allows to receive information of secure counter command.
 * 
 * \par Description:
 * The structure is used to contain the arguments returned by increment  
 * or decrement counter command.
 */
#if (VAULT_IC_TARGET == VAULTIC100)
struct _counterResponse
{
	/* New counter value */
	VLT_U8 u8CntValue[VLT_COUNTER_LENGTH];
	/** RND value length */
	VLT_U8 u8Rnd[VLT_RND_LENGTH];
	/** Signature length */
	VLT_U16 u16SignatureLength;
	/** Signature */
	VLT_PU8 pu8Signature;
};
#else
struct _counterResponse
{
	/* Nb counter updated */
	VLT_U8 u8NbCount;
	/* New counter value */
	VLT_U8 u8CntValue[VLT_FULL_COUNTERS_LENGTH];
	/** Nounce length (8) */
	VLT_U8 u8Rnd[VLT_RND_LENGTH];
	/** Signature length */
	VLT_U16 u16SignatureLength;
	/** Signature */
	VLT_PU8 pu8Signature;
};
#endif

typedef struct _counterResponse VLT_COUNTER_RESPONSE;

/*@}*/
#endif /* VAULTIC_STRUCTS_H */
