/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_lib.h
 *
 * \defgroup VaultIcLib Vault IC Library
 * 
 * \brief VaultIC API as DLL.
 * 
 * \par Description:
 *
 * This file provides an interface to the VaultIC API in DLL form.
 */
/*@{*/

#ifndef VAULTIC_LIB_H
#define VAULTIC_LIB_H

#include "vaultic_config.h"

/*
*
*
*
*
*/
#define VLT_PFN(x) (*pfn##x)
#define VLT_MTHD(x) pfn##x

#define SUBMIT_PASSWORD                     "VltSubmitPassword"
#define INITIALIZE_UPDATE                   "VltInitializeUpdate"
#define EXTERNAL_AUTHENTICATE               "VltExternalAuthenticate"
#define MANAGE_AUTHENTICATION_DATA          "VltManageAuthenticationData"
#define GET_AUTHENTICATION_INFO             "VltGetAuthenticationInfo"
#define CANCEL_AUTHENTICATION               "VltCancelAuthentication"
#define GET_CHALLENGE                       "VltGetChallenge"
#define GENERIC_INTERNAL_AUTHENTICATE       "VltGenericInternalAuthenticate"
#define GENERIC_EXTERNAL_AUTHENTICATE       "VltGenericExternalAuthenticate"
#define INITIALIZE_ALGORITHM                "VltInitializeAlgorithm"
#define PUT_KEY                             "VltPutKey"
#define READ_KEY                            "VltReadKey"
#define DELETE_KEY                          "VltDeleteKey"
#define ENCRYPT                             "VltEncrypt"
#define DECRYPT                             "VltDecrypt"
#define GENERATE_ASSURANCE_MESSAGE          "VltGenerateAssuranceMessage"
#define GENERATE_VERIFY_SIGNATURE           "VltGenerateVerifySignature"
#define COMPUTE_MESSAGE_DIGEST              "VltComputeMessageDigest"
#define GENERATE_RANDOM                     "VltGenerateRandom"
#define GENERATE_KEYPAIR                    "VltGenerateKeyPair"
#define BEGIN_TRANSACTION                   "VltBeginTransaction"
#define END_TRANSACTION                     "VltEndTransaction"
#define SELECT_FILEORDIRECTORY              "VltSelectFileOrDirectory"
#define LIST_FILES                          "VltListFiles"
#define CREATE_FILE                         "VltCreateFile"
#define CREATE_FOLDER                       "VltCreateFolder"
#define DELETE_FILE                         "VltDeleteFile"
#define DELETE_FOLDER                       "VltDeleteFolder"
#define WRITE_FILE                          "VltWriteFile"
#define READ_FILE                           "VltReadFile"
#define SEEK_FILE                           "VltSeekFile"
#define SET_PRIVILEGES                      "VltSetPrivileges"
#define SET_ATTRIBUTES                      "VltSetAttributes"
#define GET_INFO                            "VltGetInfo"
#define SELF_TEST                           "VltSelfTest"
#define SET_STATUS                          "VltSetStatus"
#define SET_CONFIG                          "VltSetConfig"
#define SET_GPIO_DIR                        "VltSetGpioDirection"
#define WRITE_GPIO                          "VltWriteGpio"
#define READ_GPIO                           "VltReadGpio"
#define TEST_CASE1                          "VltTestCase1"
#define TEST_CASE2                          "VltTestCase2"
#define TEST_CASE3                          "VltTestCase3"
#define TEST_CASE4                          "VltTestCase4"
#define AUTH_INIT                           "VltAuthInit"
#define AUTH_CLOSE                          "VltAuthClose"
#define AUTH_GETSTATE                       "VltAuthGetState"

/*
* Identity Authentication Commands
*/
typedef VLT_STS VLT_PFN(VltSubmitPassword)(VLT_U8 u8UserID, 
    VLT_U8 u8RoleID,
    VLT_U8 u8PasswordLength,
    const VLT_U8 *pu8Password );

typedef VLT_STS VLT_PFN(VltInitializeUpdate)(VLT_U8 u8UserID, 
    VLT_U8 u8RoleID,
    VLT_U8 u8HostChallengeLength,
    const VLT_U8 *pu8HostChallenge,
    VLT_INIT_UPDATE *pRespData );

typedef VLT_STS VLT_PFN(VltExternalAuthenticate)(VLT_U8 u8AuthLevel,
    VLT_U8 u8ChannelLevel,
    VLT_U8 u8CryptogramLength,
    const VLT_U8 *pu8Cryptogram );

typedef VLT_STS VLT_PFN(VltManageAuthenticationData)(const VLT_MANAGE_AUTH_DATA *pAuthSetup );

typedef VLT_STS VLT_PFN(VltGetAuthenticationInfo)(VLT_U8 u8UserID, 
    VLT_AUTH_INFO *pRespData );

typedef VLT_STS VLT_PFN(VltCancelAuthentication)( void );

typedef VLT_STS VLT_PFN(VltGetChallenge)(const VLT_GENERIC_AUTH_SETUP_DATA *pAuthParameters, 
    VLT_PU8 pu8DeviceChallenge );

typedef VLT_STS VLT_PFN(VltGenericInternalAuthenticate)(const VLT_GENERIC_AUTH_SETUP_DATA *pAuthParameters,
    const VLT_U8 *pu8HostChallenge,
    VLT_PU8 pu8DeviceChallenge,
    VLT_PU16 pu16SignatureLength,
    VLT_PU8 u8Signature );


typedef VLT_STS VLT_PFN(VltGenericExternalAuthenticate)(VLT_U8 u8HostChallengeLength,
    const VLT_U8 *pu8HostChallenge,
    VLT_U16 u16HostSignatureLength,
    const VLT_U8 *pu8HostSignature );

/*
* Crypto Commands
*/
typedef VLT_STS VLT_PFN (VltDeriveKey)(VLT_U8 u8keyGroup,
    VLT_U8 u8keyIndex,
	const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
	VLT_U8 u8DerivatedKeyType,
	VLT_U16 u16WDerivatedKeyLen,
	const VLT_KEY_DERIVATION *pKeyDerivation );

typedef VLT_STS VLT_PFN (VltConstructDHAgreement)(
	VLT_U8 u8resultKeyGroup,
    VLT_U8 u8resultKeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
	const VLT_KEY_MATERIAL *pKeyMaterial );

typedef VLT_STS VLT_PFN(VltInitializeAlgorithm)(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_U8 u8ModeID,
    const VLT_ALGO_PARAMS *pAlgorithm );

typedef VLT_STS VLT_PFN(VltUnInitializeAlgorithm)();

typedef VLT_STS VLT_PFN(VltPutKey)(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
    const VLT_KEY_OBJECT *pKeyObj );

typedef VLT_STS VLT_PFN(VltReadKey)(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJECT *pKeyObj );

typedef VLT_STS VLT_PFN(VltDeleteKey)(VLT_U8 u8KeyGroup, VLT_U8 u8KeyIndex );

typedef VLT_STS VLT_PFN(VltEncrypt)(VLT_U32 u32PlainTextLength,
    const VLT_U8 *pu8plainText,
    VLT_PU32 pu32CipherTextLength,
    VLT_PU8 pu8CipherText );

typedef VLT_STS VLT_PFN(VltDecrypt)(VLT_U32 u32CipherTextLength,
    const VLT_U8 *pu8CipherText,
    VLT_PU32 pu32PlainTextLength,
    VLT_PU8 pu8PlainText );

typedef VLT_STS VLT_PFN(VltGenerateAssuranceMessage)(  VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_PU8 pu8SignerIdLength,
    VLT_PU8 pu8SignerID,
    VLT_ASSURANCE_MESSAGE* pAssuranceMsg );

typedef VLT_STS VLT_PFN(VltGenerateSignature)(VLT_U32 u32MessageLength,
    const VLT_U8 *pu8Message,
    VLT_PU16 pu16SignatureLength,
    VLT_PU8 pu8ReturnedSignature );

typedef VLT_STS VLT_PFN(VltVerifySignature)(VLT_U32 u32MessageLength,
    const VLT_U8 *pu8Message,
    VLT_U16 u16SignatureLength,
    const VLT_U8 *pu8Signature );

typedef VLT_STS VLT_PFN(VltUpdateSignature)(VLT_U32 u32MessageLength,
						   const VLT_U8 *pu8Message);

typedef VLT_STS VLT_PFN(VltComputeSignatureFinal)( VLT_PU16 pu16SignatureLength,
    VLT_PU8 pu8Signature );

typedef VLT_STS VLT_PFN(VltUpdateVerify)(VLT_U32 u32MessageLength,
						   const VLT_U8 *pu8Message);

typedef VLT_STS VLT_PFN(VltComputeVerifyFinal)(VLT_U16 u32SignatureLength,
						   const VLT_U8 *pu8Signature);

typedef VLT_STS VLT_PFN(VltUpdateMessageDigest)( VLT_U32 u32MessageLength,
    const VLT_U8 *pu8Message);

typedef VLT_STS VLT_PFN(VltComputeMessageDigestFinal)( VLT_PU8 pu8DigestLength,
    VLT_PU8 pu8Digest);

typedef VLT_STS VLT_PFN(VltComputeMessageDigest)(VLT_U32 u32MessageLength,
    const VLT_U8 *pu8Message,
    VLT_PU8 pu8DigestLength,
    VLT_PU8 pu8Digest );

typedef VLT_STS VLT_PFN(VltGenerateRandom)(VLT_U8 u8NumberOfCharacters, 
    VLT_PU16 pu16RandomCharacters );

typedef VLT_STS VLT_PFN(VltGenerateKeyPair)(VLT_U8 u8PublicKeyGroup,
    VLT_U8 u8PublicKeyIndex,
    const VLT_FILE_PRIVILEGES *pPublicKeyFilePrivileges,
    VLT_U8 u8PrivateKeyGroup,
    VLT_U8 u8PrivateKeyIndex,
    const VLT_FILE_PRIVILEGES *pPrivateKeyFilePrivileges,
    const VLT_KEY_GEN_DATA *pKeyGenData );

/*
* File System Commands
*/ 
typedef VLT_STS VLT_PFN(VltBeginTransaction)( void );

typedef VLT_STS VLT_PFN(VltEndTransaction)( void );

typedef VLT_STS VLT_PFN(VltSelectFileOrDirectory)(const VLT_U8 *pu8Path, 
    VLT_U8 u8PathLength, 
    VLT_SELECT *pRespData );

typedef VLT_STS VLT_PFN(VltListFiles)(VLT_PU16 pu16ListRespLength,
     VLT_PU8 pu8RespData );

typedef VLT_STS VLT_PFN(VltCreateFile)(VLT_U8 u8UserID,
    VLT_U32 u32FileSize, 
    const VLT_FILE_PRIVILEGES *pFilePriv,
    VLT_U8 u8FileAttribute,
    VLT_U16 u16FileNameLength,
    const VLT_U8 *pu8FileName );

typedef VLT_STS VLT_PFN(VltCreateFolder)(VLT_U8 u8PublicRead,
    const VLT_FILE_PRIVILEGES *pFilePriv,                                   
    VLT_U8 u8FolderAttribute,
    VLT_U16 u16FolderNameLength,
    const VLT_U8 *pu8FolderName );

typedef VLT_STS VLT_PFN(VltDeleteFile)( void );

typedef VLT_STS VLT_PFN(VltDeleteFolder)(VLT_U8 u8Recursion );

typedef VLT_STS VLT_PFN(VltWriteFile)(const VLT_U8 *pu8Data,
    VLT_U8 u8DataLength,
    VLT_U8 u8ReclaimSpace );

typedef VLT_STS VLT_PFN(VltReadFile)(VLT_PU16 pu16ReadLength,VLT_PU8 pu8RespData );

typedef VLT_STS VLT_PFN(VltSeekFile)(VLT_U32 u32SeekLength );

typedef VLT_STS VLT_PFN(VltSetPrivileges)(const VLT_FILE_PRIVILEGES *pFilePriv );

typedef VLT_STS VLT_PFN(VltSetAttributes)(VLT_U8 u8Attribute );

/*
* Manufacturing Commands
*/
typedef VLT_STS VLT_PFN(VltGetInfo)(VLT_TARGET_INFO *pRespData );

typedef VLT_STS VLT_PFN(VltSelfTest)( void );

typedef VLT_STS VLT_PFN(VltSetStatus)(VLT_U8 u8State );

typedef VLT_STS VLT_PFN(VltSetConfig)(VLT_U8 u8ConfigItem, VLT_U8 u8DataLength, VLT_PU8 pu8Data );

typedef VLT_STS VLT_PFN(VltTestCase1)( void );

typedef VLT_STS VLT_PFN(VltTestCase2)(VLT_U8 u8RequestedDataLength, 
    VLT_PU8 pu8RespData );

typedef VLT_STS VLT_PFN(VltTestCase3)(VLT_U8 u8DataLength, const VLT_U8 *pu8Data );

typedef VLT_STS VLT_PFN(VltTestCase4)(VLT_U8 u8DataLength,
    const VLT_U8 *pu8Data, 
    VLT_U8 u8RequestedDataLength,
    VLT_PU8 pu8RespData );

/*
* GPIO Interface Command
*/
typedef VLT_STS VLT_PFN(VltSetGpioDirection)( VLT_U8 u8GpioDirMask, 
    VLT_U8 u8GpioMode );

typedef VLT_STS VLT_PFN(VltWriteGpio)( VLT_U8 u8GpioValue );

typedef VLT_STS VLT_PFN(VltReadGpio)(VLT_PU8 pu8GpioValue);

/*
* Identity Authentication Service Methods
*/
typedef VLT_STS VLT_PFN(VltAuthInit)( VLT_U8 u8AuthMethod,
    VLT_U8 u8UserID, 
    VLT_U8 u8RoleID, 
    VLT_U8 u8ChannelLevel, 
    KEY_BLOB_ARRAY keys );

typedef VLT_STS VLT_PFN(VltAuthClose)( void );

typedef VLT_STS VLT_PFN(VltAuthGetState)( VLT_PU8 pu8State );

/*
* Key Wrapping Service Methods
*/
typedef VLT_STS VLT_PFN(VltKeyWrappingInit)( VLT_U8 u8KTSKeyGroup,
    VLT_U8 u8KTSKeyIndex,
    WRAP_PARAMS* pWrapParams,
    VLT_KEY_OBJECT* pKTSKey );

typedef VLT_STS VLT_PFN(VltUnwrapKey)( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
    const VLT_KEY_OBJ_RAW* pKeyObj );

typedef VLT_STS VLT_PFN(VltWrapKey)( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_RAW* pKeyObj );

typedef VLT_STS VLT_PFN(VltKeyWrappingClose)( void );

/*
* File System Service Methods
*/
typedef VLT_STS VLT_PFN(VltFsOpenFile)( VLT_U16 u16FileNameLength,
    const VLT_U8* pu8FileName, 
    VLT_U8 u8TransactionMode,
    VLT_FS_ENTRY_PARAMS *pFsFileParams );

typedef VLT_STS VLT_PFN(VltFsCloseFile)( void );

typedef VLT_STS VLT_PFN(VltFsCreate)( VLT_U16 u16EntryNameLen,
    const VLT_U8* pu8EntryName,
    const VLT_FS_ENTRY_PARAMS *pFsEntryParams,
    VLT_U8 u8UserId );

typedef VLT_STS VLT_PFN(VltFsDelete)( VLT_U16 u16EntryNameLen,
    const VLT_U8* pu8EntryName,
    VLT_U8 u8Recursion );

typedef VLT_STS VLT_PFN(VltFsReadFile)( VLT_U32 u32Offset,
    VLT_PU8 pu8DataOut, 
    VLT_PU32 pu32DataLength );

typedef VLT_STS VLT_PFN(VltFsWriteFile)( VLT_U32 u32Offset,
    VLT_PU8 pu8DataIn, 
    VLT_U32 u32DataLength,
    VLT_U8 u8ReclaimSpace );

typedef VLT_STS VLT_PFN(VltFsListFiles)(VLT_U16 u16FolderNameLength,
    const VLT_U8* pu8FolderName,
    VLT_PU16 pu16ListRespLength,
    VLT_PU8 pu8RespData );

typedef VLT_STS VLT_PFN(VltFsSetPrivileges)( VLT_U16 u16EntryNameLength,
    const VLT_U8* pu8EntryName, 
    const VLT_FS_ENTRY_PRIVILEGES* pFsEntryPrivileges );

typedef VLT_STS VLT_PFN(VltFsSetAttributes)( VLT_U16 u16EntryNameLength,
    const VLT_U8* pu8EntryName, 
    const VLT_FS_ENTRY_ATTRIBS* pFsEntryAttributes );

typedef VLT_STS VLT_PFN(VltCrc16)( VLT_PU16 pu16Crc, 
    const VLT_U8 *pu8Block, 
    VLT_U16 u16Length );


typedef struct _VaultICApi 
{
    VLT_MTHD(VltSubmitPassword)                 VltSubmitPassword; 
    VLT_MTHD(VltInitializeUpdate)               VltInitializeUpdate;
    VLT_MTHD(VltExternalAuthenticate)           VltExternalAuthenticate;
    VLT_MTHD(VltManageAuthenticationData)       VltManageAuthenticationData;
    VLT_MTHD(VltGetAuthenticationInfo)          VltGetAuthenticationInfo;   
    VLT_MTHD(VltCancelAuthentication)           VltCancelAuthentication;
    VLT_MTHD(VltGetChallenge)                   VltGetChallenge;
    VLT_MTHD(VltGenericInternalAuthenticate)    VltGenericInternalAuthenticate;
    VLT_MTHD(VltGenericExternalAuthenticate)    VltGenericExternalAuthenticate;
    VLT_MTHD(VltInitializeAlgorithm)            VltInitializeAlgorithm;
    VLT_MTHD(VltUnInitializeAlgorithm)          VltUnInitializeAlgorithm;
    VLT_MTHD(VltPutKey)                         VltPutKey;
    VLT_MTHD(VltReadKey)                        VltReadKey;
    VLT_MTHD(VltDeleteKey)                      VltDeleteKey;
    VLT_MTHD(VltEncrypt)                        VltEncrypt;
    VLT_MTHD(VltDecrypt)                        VltDecrypt;
    VLT_MTHD(VltGenerateAssuranceMessage)       VltGenerateAssuranceMessage;
    VLT_MTHD(VltGenerateSignature)              VltGenerateSignature;
    VLT_MTHD(VltVerifySignature)                VltVerifySignature;
    VLT_MTHD(VltComputeMessageDigest)           VltComputeMessageDigest;
    VLT_MTHD(VltGenerateRandom)                 VltGenerateRandom;
    VLT_MTHD(VltGenerateKeyPair)                VltGenerateKeyPair;
    VLT_MTHD(VltBeginTransaction)               VltBeginTransaction;
    VLT_MTHD(VltEndTransaction)                 VltEndTransaction;
    VLT_MTHD(VltSelectFileOrDirectory)          VltSelectFileOrDirectory;
    VLT_MTHD(VltListFiles)                      VltListFiles;
    VLT_MTHD(VltCreateFile)                     VltCreateFile;
    VLT_MTHD(VltCreateFolder)                   VltCreateFolder;
    VLT_MTHD(VltDeleteFile)                     VltDeleteFile;
    VLT_MTHD(VltDeleteFolder)                   VltDeleteFolder;
    VLT_MTHD(VltWriteFile)                      VltWriteFile;
    VLT_MTHD(VltReadFile)                       VltReadFile;
    VLT_MTHD(VltSeekFile)                       VltSeekFile;
    VLT_MTHD(VltSetPrivileges)                  VltSetPrivileges;
    VLT_MTHD(VltSetAttributes)                  VltSetAttributes;
    VLT_MTHD(VltGetInfo)                        VltGetInfo;
    VLT_MTHD(VltSelfTest)                       VltSelfTest;
    VLT_MTHD(VltSetStatus)                      VltSetStatus;
    VLT_MTHD(VltSetConfig)                      VltSetConfig;
    VLT_MTHD(VltSetGpioDirection)               VltSetGpioDirection;
    VLT_MTHD(VltWriteGpio)                      VltWriteGpio;
    VLT_MTHD(VltReadGpio)                       VltReadGpio;
    VLT_MTHD(VltTestCase1)                      VltTestCase1;
    VLT_MTHD(VltTestCase2)                      VltTestCase2;
    VLT_MTHD(VltTestCase3)                      VltTestCase3;
    VLT_MTHD(VltTestCase4)                      VltTestCase4;
	VLT_MTHD(VltUpdateSignature)				VltUpdateSignature;
    VLT_MTHD(VltComputeSignatureFinal)			VltComputeSignatureFinal;
    VLT_MTHD(VltUpdateVerify)					VltUpdateVerify;
	VLT_MTHD(VltComputeVerifyFinal)				VltComputeVerifyFinal;
    VLT_MTHD(VltUpdateMessageDigest)			VltUpdateMessageDigest;
	VLT_MTHD(VltComputeMessageDigestFinal)		VltComputeMessageDigestFinal;
#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_2_1 )
	VLT_MTHD(VltDeriveKey)						VltDeriveKey;
	VLT_MTHD(VltConstructDHAgreement)           VltConstructDHAgreement; 
#endif

}VAULTIC_API;


typedef struct _VaultICAuth 
{
    VLT_MTHD(VltAuthInit)                        VltAuthInit;
    VLT_MTHD(VltAuthClose)                       VltAuthClose;
    VLT_MTHD(VltAuthGetState)                    VltAuthGetState;
}VAULTIC_AUTH;

typedef struct _VaultICKeyWrap
{
    VLT_MTHD(VltKeyWrappingInit)                VltKeyWrappingInit;
    VLT_MTHD(VltKeyWrappingClose)               VltKeyWrappingClose;
    VLT_MTHD(VltWrapKey)                        VltWrapKey;
    VLT_MTHD(VltUnwrapKey)                      VltUnwrapKey;
}VAULTIC_KEY_WRAPPING;

typedef struct _VaultICFileSystem
{
    VLT_MTHD(VltFsOpenFile)                     VltFsOpenFile;
    VLT_MTHD(VltFsCloseFile)                    VltFsCloseFile;
    VLT_MTHD(VltFsCreate)                       VltFsCreate;
    VLT_MTHD(VltFsDelete)                       VltFsDelete;
    VLT_MTHD(VltFsReadFile)                     VltFsReadFile;
    VLT_MTHD(VltFsWriteFile)                    VltFsWriteFile;
    VLT_MTHD(VltFsListFiles)                    VltFsListFiles;
    VLT_MTHD(VltFsSetPrivileges)                VltFsSetPrivileges;
    VLT_MTHD(VltFsSetAttributes)                VltFsSetAttributes;
}VAULTIC_FILE_SYSTEM;

typedef struct _VaultICCRC16
{
    VLT_MTHD(VltCrc16)                          VltCrc16;
}VAULTIC_CRC16;

#if( VLT_PLATFORM == VLT_WINDOWS )
    #define _API_ __declspec(dllexport)
#else 
    #define _API_
#endif 

#ifdef __cplusplus 
extern "C" 
{
#endif 
    
    /**
     * \fn VltInitLibrary( VLT_INIT_COMMS_PARAMS* pInitCommsParams )
     *
     * \brief Initialises the Vault IC API library.
     *
     * \par Description:
     *
     * The VltInitLibrary() method is responsible for initialising the Vault IC
     * API library
     *
     * \param pInitCommsParams [IN] Parameters passed to communications layer.
     *  
     * \return Upon successful completion a VLT_OK status will be returned otherwise
     * the appropriate error code will be returned.
     *
     * \par Example:
     * \code
     *  typedef VLT_STS (*pfnInitLibrary)(VLT_INIT_COMMS_PARAMS* commsParams);
     *  HMODULE hMod;
     *  pfnInitLibrary VltInitLibrary;
     *  VLT_STS status = VLT_FAIL 
     *  VLT_INIT_COMMS_PARAMS params;
     *
     *  hMod = LoadLibrary( "C:\\Temp\\VaultICApiDLL.dll" );
     *
     *  if( NULL == ( VltInitLibrary = (pfnInitLibrary)GetProcAddress( hMod, 
     *      "VltInitLibrary" ) ) )
     *  {
     *      return ( -1 );
     *  }
     * 
     *  params.u8CommsProtocol = VLT_TWI_COMMS;
     *  params.Params.VltBlockProtocolParams.u16BitRate = 100;
     *  params.Params.VltBlockProtocolParams.u8CheckSumMode = BLK_PTCL_CHECKSUM_SUM8;
     *
     *  params.Params.VltBlockProtocolParams.u32AfterHdrDelay = 1000;
     *  params.Params.VltBlockProtocolParams.u32InterBlkDelay = 1000;
     *
     *  params.Params.VltBlockProtocolParams.VltPeripheralParams.PeriphParams.VltTwiParams.u16BusTimeout = 450;
     *  params.Params.VltBlockProtocolParams.VltPeripheralParams.PeriphParams.VltTwiParams.u8Address = 0x5F;
     *
     *  params.Params.VltBlockProtocolParams.VltPeripheralParams.u32SerialNo = 1234567890;
     * 
     *  status = VltInitLibrary( &params );
     *
     * \endcode
     */
    _API_ VLT_STS VltInitLibrary( VLT_INIT_COMMS_PARAMS* pInitCommsParams );

    /**
     * \fn VltCloseLibrary( void )
     *
     * \brief Closes the Vault IC API library.
     *
     * \par Description:
     *
     * The VltCloseLibrary() method is responsible for closing the Vault IC
     * API library
     *  
     * \return Upon successful completion a VLT_OK status will be returned otherwise
     * the appropriate error code will be returned.
     *
     * \par Example:
     * \code 
     *  VLT_STS status = VLT_FAIL; 
     * 
     *  status = VltCloseLibrary();   
     * 
     * \endcode
     */
    _API_ VLT_STS VltCloseLibrary( void );

    /**
     * \fn VltGetApi( void )
     *
     * \brief Provides a structure of the available API methods .
     *
     * \par Description:
     *
     * The VltGetApi() method is responsible for providing a means of calling
     * the Vault IC API methods.  It does this by providing a pointer to a
     * #VAULTIC_API structure.
     *  
     * \return Upon successful completion a pointer to a #VAULTIC_API structure
     *
     * \par Example:
     * \code 
     *  VAULTIC_API* pTheApi;
     * 
     *  if( NULL == ( pTheApi = VltGetApi() )
     *  {
     *      return -1;
     *  }
     * 
     * \endcode
     */
    _API_ VAULTIC_API* VltGetApi( void );

    /**
     * \fn VltGetAuth( void )
     *
     * \brief Provides a structure of the available Identity Authentication
     * methods .
     *
     * \par Description:
     *
     * The VltGetAuth() method is responsible for providing a means of calling
     * the Vault IC Identity Authentication service methods.  It does this by 
     * providing a pointer to a #VAULTIC_AUTH structure.
     *  
     * \return Upon successful completion a pointer to a #VAULTIC_AUTH structure
     *
     * \par Example:
     * \code 
     *  VAULTIC_AUTH* pTheAuth;
     * 
     *  if( NULL == ( pTheAuth = VltGetAuth() )
     *  {
     *      return -1;
     *  }
     * 
     * \endcode
     */
    _API_ VAULTIC_AUTH* VltGetAuth( void );

    /**
     * \fn VltGetKeyWrapping( void )
     *
     * \brief Provides a structure of the available Key Wrapping service
     * methods .
     *
     * \par Description:
     *
     * The VltGetKeyWrapping() method is responsible for providing a means of 
     * calling the Vault IC Key Wrapping service methods.  It does this by
     * providing a pointer to a VAULTIC_KEY_WRAPPING structure.
     *  
     * \return Upon successful completion a pointer to a VAULTIC_KEY_WRAPPING 
     * structure
     *
     * \par Example:
     * \code 
     *  VAULTIC_KEY_WRAPPING* pTheKeyWrapping;
     * 
     *  if( NULL == ( pTheKeyWrapping = VltGetKeyWrapping() )
     *  {
     *      return -1;
     *  }
     * 
     * \endcode
     */
    _API_ VAULTIC_KEY_WRAPPING* VltGetKeyWrapping( void );

    /**
     * \fn VltGetFileSystem( void )
     *
     * \brief Provides a structure of the available File System service
     * methods .
     *
     * \par Description:
     *
     * The VltGetFileSystem() method is responsible for providing a means of 
     * calling the Vault IC File System service methods.  It does this by
     * providing a pointer to a VAULTIC_FILE_SYSTEM structure.
     *  
     * \return Upon successful completion a pointer to a VAULTIC_FILE_SYSTEM 
     * structure
     *
     * \par Example:
     * \code 
     *  VAULTIC_FILE_SYSTEM* pTheFileSystem;
     * 
     *  if( NULL == ( pTheFileSystem = VltGetFileSystem() )
     *  {
     *      return -1;
     *  }
     * 
     * \endcode
     */
    _API_ VAULTIC_FILE_SYSTEM* VltGetFileSystem( void );

    /**
     * \fn VltGetCrc16( void )
     *
     * \brief Provides a structure of the available CRC16 CCITT service
     *  method.
     *
     * \par Description:
     *
     * The VltGetCrc16() method is responsible for providing a means of 
     * calculating the CRC16 CCITT value of a block of data.  It does this by
     * providing a pointer to a VAULTIC_CRC16 structure.
     *  
     * \return Upon successful completion a pointer to a VAULTIC_CRC16 
     * structure
     *
     * \par Example:
     * \code 
     *  VAULTIC_CRC16* pTheCrc16;
     *  VLT_STS status = VLT_FAIL;
     * 
     *  if( NULL == ( pTheCrc16 = VltGetCrc16() )
     *  {
     *      return( VLT_FAIL );
     *  }
     *  
     *  VLT_U16 Crc16 = VLT_CRC16_CCITT_INIT_0s;
     *  VLT_U8 pBinaryBlock[32] = 
     *  {
     *      0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
     *      0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 
     *      0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 
     *      0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
     *  };
     *  
     *  //
     *  // Calculate the crc16 of the first 10 bytes of the binary block 
     *  // array.
     *  //
     *  if( VLT_OK != ( status = pTheCrc16->VltCrc16( &Crc16, &pBinaryBlock[0], 10 ) ) )
     *  {
     *      return( status );
     *  }
     * 
     *  //
     *  // Calculate the crc16 of the remaining 22 bytes of the binary block 
     *  // array. On return from the call the Crc16 variable will hold the CRC16 
     *  // CCITT calculation of the entire 32 byte binary block.
     *  //
     *  if( VLT_OK != ( status = pTheCrc16->VltCrc16( &Crc16, &pBinaryBlock[10], 22 ) ) )
     *  {
     *      return( status );
     *  }
     * 
     *  return( VLT_OK );
     *
     * \endcode
     */
    _API_ VAULTIC_CRC16* VltGetCrc16( void );

    /**
     * \fn VltGetLibraryInfo( VLT_LIBRARY_INFO* pLibraryInfo )
     *
     * \brief Populates a provided #VLT_LIBRARY_INFO structure with information
     * about the library
     *
     * \par Description:
     *
     * The VltGetLibraryInfo() method provides the caller with the capabilities of
     * the current version of the library.  The information is provided in the
     * #VLT_LIBRARY_INFO structure
     *
     * * \return Upon successful completion a VLT_OK status will be returned otherwise
     * the appropriate error code will be returned.
     *
     * \par Note:
     * Please note this method is not supported in the embedded environments.
     *
     * \par Example:
     * \code 
     *  VLT_STS status = VLT_FAIL; 
     *  VLT_LIBRARY_INFO libInfo;
     * 
     *  status = VltGetLibraryInfo( &libInfo );   
     * 
     * \endcode
     */
    _API_ VLT_STS VltGetLibraryInfo( VLT_LIBRARY_INFO* pLibraryInfo );

    /**
     * \fn VltFindDevices( VLT_PU32 pSize, VLT_PU8 pXmlReaderString )
     *
     * \brief Provides an XML based list of all supported type of devices currently connected to the system.
     *
     * \par Description:
     *
     * The VltFindDevices() method provides the caller an XML based list of all 
     * supported type of devices currently connected to the system. The caller 
     * can then parse the data in order to use them in calling the VltInit() method.
     * 
     * Due to the dymanic nature of the number of supported types of devices connected, 
     * the VltFindDevices() method has to be called twice to determine the amount of 
     * data that will be eventually returned. The caller needs to call the VltFindDevices()  
     * the first time with the value of pSize set to 0 and the pXmlReaderString set to NULL, 
     * upon successful completion the VltFindDevices(), will populate the pSize with the 
     * actual number of bytes required to return the xml based data.
     * The caller then needs to allocate enough memory to hold the number of bytes returned
     * by the previous call to the VltFindDevices() in pSize.
     * Once enough memory has been allocated then the caller has to make a second call to the
     * VltFindDevices() method with pSize set to the value from the previous call and 
     * pXmlReaderString set to the pointer of the recently allocated buffer.
     * The second call will update the pSize variable to indicate the amount of data written
     * to the pXmlReaderString, while the pXmlReaderString will be populated with a null 
     * terminated XML string, an example of this is shown below:
     * 
     * \code
     * <?xml version="1.0" encoding="ISO-8859-1"?>
     * <devices>
     *      <interface type="pcsc">
     *          <peripheral idx="00">Gemplus USB Smart Card Reader 0</peripheral>
     *      </interface>
     *      <interface type="aardvark">
     *          <peripheral idx="00">2237366715</peripheral>
     *          <peripheral idx="01">2237367164</peripheral>
     *      </interface>
     *      <interface type="cheetah">
     *          <peripheral idx="00">1363924836</peripheral>
     *      </interface>
     *  </devices>
     * \endcode
     *
     * The VltFindDevices() method has a fundamentaly different dependency model from 
     * all other VaultIC API library methods. The VltFindDevices() method may be 
     * called before a call to the VltInitLibrary() method has been made.
     *
     * \par Note:
     * Please note this method is not supported in the embedded environments. Also
     * this method depends on having the Total Phase aardvark.dll/so and/or cheetah.dll/so 
     * binaries located in the same path as your executable, or in the standard 
     * operating system library search path.
     * 
     * \return Upon successful completion a VLT_OK status will be returned otherwise
     * the appropriate error code will be returned.
     *
     * \par Example:
     * \code 
     *  VLT_STS status = VLT_FAIL ;
     *  VLT_U32 size = 0;
     *  VLT_PU8 pXmlString;
     * 
     *  // size must be set to zero for the first call.
     *  size = 0;
     *  VltFindDevices( &size , NULL );   
     *  pXmlString = (VLT_PU8)malloc( size );
     *
     *  VltFindDevices( &size , pXmlString );
     *  free( (void*)pXmlString );
     * 
     * \endcode
     */
    _API_ VLT_STS VltFindDevices( VLT_PU32 pSize, VLT_PU8 pXmlReaderString );

#ifdef __cplusplus 
}
#endif 
/*@}*/
#endif /*VAULTIC_LIB_H*/
