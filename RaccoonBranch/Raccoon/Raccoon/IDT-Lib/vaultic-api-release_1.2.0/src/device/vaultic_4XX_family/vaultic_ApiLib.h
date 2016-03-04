/* --------------------------------------------------------------------------
 * VaultIC
 * Copyright (C) Atmel Corporation, 2010. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#ifndef __VAULTICAPI_H__
#define __VAULTICAPI_H__
 

#include "vaultic_common.h"

#if ( VLT_ENABLE_ISO7816 == VLT_ENABLE )
#ifndef WIN32
#include "PCSC/wintypes.h"
#endif
#endif

#ifdef __cplusplus
 
extern "C" 
{
	
/* --------------------------------------------------------------------------
 * LIBRARY MANAGEMENT FUNCTIONS
 * -------------------------------------------------------------------------- 
 */

/**
 * \fn VltApiInit(VLT_INIT_COMMS_PARAMS* pInitCommsParams)
 *
 * \brief Initialises the VaultIC API.
 * \par Description:
 *
 * The VltApiInit() method provides an initialisation entry point to the 
 * entire VaultIC API library. Upon successful completion a number of internal
 * system resources would be allocated and used by subsequent API calls, these
 * resources will remain in use until a call to the VltApiClose() method is made.
 * If the call to the VltApiInit() is unsuccessful calls to the rest of the API 
 * methods will produced undefined results. 
 *
 * \param pInitCommsParams [IN] Parameters passed to communications layer.
 *  
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned.
 *  
 * \par Example:
 * \code 
 *  VLT_STS status = VLT_FAIL 
 *  VLT_INIT_COMMS_PARAMS params;
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
 *  status = VltApiInit( &params );
 *  if( VLT_OK != status )
 *  {
 *      return( VltApiClose() );
 *  }
 * \endcode
 */
VLT_STS VltApiInit(VLT_INIT_COMMS_PARAMS* pInitCommsParams);

/**
 * \fn VltApiClose
 *
 * \brief Closes down the VaultIC API.
 *
 * \par Description:
 *
 * The VltApiClose() method provides an finalisation entry point to the 
 * entire VaultIC API library. Upon successful completion a number of internal
 * system resources previously allocated would be released, subsequent calls 
 * to the rest of the API methods will produced undefined results.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned.
 *
 * \par Example:
 * See VltApiInit()
 */
VLT_STS VltApiClose( void );

/* --------------------------------------------------------------------------
 * IDENTITY AUTHENTICATION COMMANDS
 * -------------------------------------------------------------------------- */

/**
 * \fn VLT_STS VltSubmitPassword( VLT_U8 u8UserID, 
 *   VLT_U8 u8RoleID,
 *   VLT_U8 u8PasswordLength,
 *   const VLT_U8 *pu8Password );
 *
 * \brief Authenticates an operator using a password.
 *
 * \par Description:
 *
 * After successful execution, this command closes any previously opened Secure
 * Channel (i.e. the response is sent applying the current security level, and
 * the security level is reset afterwards). If the submission fails (i.e. wrong
 * password), the previous authentication and secure channel are preserved.
 *
 * \param u8UserID [IN]  Operator ID (0..7). Possible values are: 
 * - #VLT_USER0 
 * - #VLT_USER1 
 * - #VLT_USER2 
 * - #VLT_USER3 
 * - #VLT_USER4 
 * - #VLT_USER5 
 * - #VLT_USER6 
 * - #VLT_USER7 
 *
 * \param u8RoleID         [IN]  Role ID. Possible values are:
 * - #VLT_APPROVED_USER     
 * - #VLT_NON_APPROVED_USER 
 * - #VLT_MANUFACTURER      
 * - #VLT_ADMINISTRATOR     
 * - #VLT_EVERYONE            
 *
 * \param u8PasswordLength [IN]  Password length (4..32).
 * \param pu8Password      [IN]  Password.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC. 
 */
VLT_STS VltSubmitPassword( VLT_U8 u8UserID, 
    VLT_U8 u8RoleID,
    VLT_U8 u8PasswordLength,
    const VLT_U8 *pu8Password );

/**
 * \fn VltInitializeUpdate( VLT_U8 u8UserID, 
 *       VLT_U8 u8RoleID,
 *       VLT_U8 u8HostChallengeLength,
 *       const VLT_U8 *pu8HostChallenge,
 *       VLT_INIT_UPDATE *pRespData );
 *
 * \brief Performs Secure Channel initiation, or Microsoft Card Minidriver
 * authentication.
 *
 * \par Description:
 *
 * This command is used in conjunction with the VltExternalAuthenticate()
 * command to perform a SCP Secure Channel initation or to initiate a Microsoft
 * Card Minidriver authentication.
 * This command first closes any current secure channel. The authentication try
 * counter is decremented here, and is reset in the VltExternalAuthenticate()
 * command if successful.
 *
 * \param u8UserID              [IN]  Operator ID (0..7).Possible values are:
 * - #VLT_USER0 
 * - #VLT_USER1 
 * - #VLT_USER2 
 * - #VLT_USER3 
 * - #VLT_USER4 
 * - #VLT_USER5 
 * - #VLT_USER6 
 * - #VLT_USER7 
 *
 * \param u8RoleID              [IN]  Role ID. Possible values are:
 * - #VLT_APPROVED_USER     
 * - #VLT_NON_APPROVED_USER 
 * - #VLT_MANUFACTURER      
 * - #VLT_ADMINISTRATOR     
 * - #VLT_EVERYONE            
 *
 * \param u8HostChallengeLength [IN]  Host challenge length (or 0 for MS).
 * \param pu8HostChallenge      [IN]  Host challenge (or NULL for MS).
 * \param pRespData             [OUT] ::VLT_INIT_UPDATE structure to receive
 *                                    SCP02, SCP03 response data, or MS Device
 *                                    Challenge.
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltInitializeUpdate( VLT_U8 u8UserID, 
    VLT_U8 u8RoleID,
    VLT_U8 u8HostChallengeLength,
    const VLT_U8 *pu8HostChallenge,
    VLT_INIT_UPDATE *pRespData );

/**
 * \fn VltExternalAuthenticate( VLT_U8 u8AuthLevel,
 *       VLT_U8 u8ChannelLevel,
 *       VLT_U8 u8CryptogramLength,
 *       const VLT_U8 *pu8Cryptogram );
 *
 * \brief Completes a strong operator authentication and secure channel
 * initiation.
 *
 * \par Description:
 *
 * Completes a strong operator authentication and secure channel initiation.
 * The security level is defined and will be applied to subsequent APDU
 * commands.
 * A call to this method typically follows a call to the VltInitializeUpdate() method.
 *
 * \param u8AuthLevel        [IN]  The authentication level, possible values are :
 * - #VLT_LOGIN_SCP02                           
 * - #VLT_LOGIN_SCP03
 * - #VLT_LOGIN_MS
 *
 * \param u8ChannelLevel     [IN]  Security level, possible values are;
 * - #VLT_NO_CHANNEL          
 * - #VLT_CMAC               
 * - #VLT_CMAC_CENC          
 * - #VLT_CMAC_RMAC          
 * - #VLT_CMAC_CENC_RMAC     
 * - #VLT_CMAC_CENC_RMAC_RENC
 *  
 * \param u8CryptogramLength [IN]  Cryptogram length.
 * \param pu8Cryptogram      [IN]  Cryptogram.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltExternalAuthenticate( VLT_U8 u8AuthLevel,
    VLT_U8 u8ChannelLevel,
    VLT_U8 u8CryptogramLength,
    const VLT_U8 *pu8Cryptogram );

/**
 * \fn VltManageAuthenticationData( const VLT_MANAGE_AUTH_DATA *pAuthSetup );
 *
 * \brief Set or update authentication data for an operator.
 *
 * \par Description:
 *
 * Authenticated operator assuming manufacturer role can conditionally add,
 * delete, modify, lock or unlock any operator authentication data without
 * restriction if the device is in VLT_CREATION state.
 * Authenticated operator assuming administrator role can conditionally add,
 * delete, modify, lock, or unlock any operator authentication data without
 * restriction and in any state (except VLT_TERMINATED state) if the
 * Administrator Security Policy is set.
 * Any authenticated operator can only modify its own authentication data with
 * some restrictions. Add, delete, lock and unlock operations are not permitted.
 * The supplied authentication data must have the same authentication method,
 * the same assumed roles, the same security level and the same security option.
 * Any mismatch will result in an error.  Authentication data update preserves
 * both try counter and sequence counter.
 *
 * \param pAuthSetup [IN]  Manage Authentication Data data structure.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltManageAuthenticationData( const VLT_MANAGE_AUTH_DATA *pAuthSetup );

/**
 * \fn VltGetAuthenticationInfo( VLT_U8 u8UserID, 
 *         VLT_AUTH_INFO *pRespData );
 *
 * \brief Gets info about authentication data of an operator.
 *
 * \param u8UserID  [IN]  Operator ID (0..7).Possible values are:
 * - #VLT_USER0 
 * - #VLT_USER1 
 * - #VLT_USER2 
 * - #VLT_USER3 
 * - #VLT_USER4 
 * - #VLT_USER5 
 * - #VLT_USER6 
 * - #VLT_USER7 
 * \param pRespData [OUT] Authentication Info data structure ::VLT_AUTH_INFO.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltGetAuthenticationInfo( VLT_U8 u8UserID, 
    VLT_AUTH_INFO *pRespData );

/**
 * \fn VltCancelAuthentication( void );
 *
 * \brief Resets the VaultIC authentication state and closes secure channel.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltCancelAuthentication( void );

/**
 * \fn VltGetChallenge( const VLT_GENERIC_AUTH_SETUP_DATA *pAuthParameters, 
 *         VLT_PU8 pu8DeviceChallenge );
 *
 * \brief Initiates a generic Host Unilateral Authentication to the VaultIC.
 *
 * \par Description:
 *
 * This command is used to initiate a generic Host Unilateral Authentication to
 * the VaultIC chip (i.e. the VaultIC chip authenticates an external entity).
 * The VltInitializeAlgorithm() command shall be previously sent with the
 * algorithm identifier set with a valid Signer Identifier. The signer key is
 * fetched at this time and the signer engine is initialized with specific
 * algorithm parameters.
 * The response to the VltGetChallenge() command contains the VaultIC
 * challenge Cd that will be used by the host in its signature computation
 * during the strong authentication. The length of the challenge is defined by
 * the Generic Strong Authentication parameters ::VLT_GENERIC_AUTH_SETUP_DATA.
 * This command shall be followed by a VltGenericExternalAuthenticate()
 * command. If not, the authentication process is aborted and the command is
 * processed normally.
 *
 * \param pAuthParameters    [IN]  Generic Strong Authentication parameters.
 * \param pu8DeviceChallenge [OUT] Device challenge (Cd).
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltGetChallenge( const VLT_GENERIC_AUTH_SETUP_DATA *pAuthParameters, 
    VLT_PU8 pu8DeviceChallenge );

/**
 * \fn VltGenericInternalAuthenticate( 
 *         const VLT_GENERIC_AUTH_SETUP_DATA *pAuthParameters,
 *         const VLT_U8 *pu8HostChallenge,
 *         VLT_PU8 pu8DeviceChallenge,
 *         VLT_PU16 pu16SignatureLength,
 *         VLT_PU8 pu8Signature );
 *
 * \brief Perform a generic Device Unilateral Authentication of the VaultIC, or
 * generic Mutual Authentication.
 *
 * \par Description:
 *
 * This command can be used alone to perform a generic Device Unilateral
 * Authentication of the VaultIC or may be followed by a 
 * VltGenericExternalAuthenticate() command in order to perform a generic Mutual
 * Authentication.
 *
 * \param pAuthParameters     [IN]     Generic Strong Authentication parameters.
 * \param pu8HostChallenge    [IN]     Host challenge (Ch).
 * \param pu8DeviceChallenge  [OUT]    Buffer to receive VaultIC challenge Cd.
 * \param pu16SignatureLength [IN,OUT] On entry this holds the maximum size of
 *                                     the signature buffer. On exit it is set
 *                                     to the amount of signature buffer used.
 * \param pu8Signature        [OUT]    Buffer to receive VaultIC signature SIGNk.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltGenericInternalAuthenticate( const VLT_GENERIC_AUTH_SETUP_DATA *pAuthParameters,
    const VLT_U8 *pu8HostChallenge,
    VLT_PU8 pu8DeviceChallenge,
    VLT_PU16 pu16SignatureLength,
    VLT_PU8 pu8Signature );

/**
 * \fn VltGenericExternalAuthenticate(VLT_U8 u8HostChallengeLength,
 *         const VLT_U8 *pu8HostChallenge,
 *         VLT_U16 u16HostSignatureLength,
 *         const VLT_U8 *pu8HostSignature );
 *
 * \brief Used after VltGenericInternalAuthenticate() to complete a generic
 * Mutual Authentication.
 *
 * \par Description:
 *
 * This command may be used after a GenericInteralAuthenticate() to complete
 * a generic Mutual Authentication protocol, or can be sent after a 
 * VltGetChallenge() command to complete a generic Host Unilateral Authentication
 * to the VaultIC.
 * This command gets a signature from a host and a host challenge Ch, and
 * attempts a verification. It returns the result of this verification. The
 * applicable security parameters object and key index are VLT_SELECTed in a
 * prior 
 *
 * \param u8HostChallengeLength  [IN]  Host challenge (Ch) length.
 * \param pu8HostChallenge       [IN]  Host challenge (Ch).
 * \param u16HostSignatureLength [IN]  Host signature (SIGNk) length.
 * \param pu8HostSignature       [IN]  Host signature (SIGNk).
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 *
 * \see VltGetChallenge command 
 * \see VltGenericInternalAuthenticate command.
 */
VLT_STS VltGenericExternalAuthenticate(VLT_U8 u8HostChallengeLength,
    const VLT_U8 *pu8HostChallenge,
    VLT_U16 u16HostSignatureLength,
    const VLT_U8 *pu8HostSignature );

/* --------------------------------------------------------------------------
 * CRYPTO SERVICES
 * -------------------------------------------------------------------------- */

/**
 * \fn VltInitializeAlgorithm( VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,
 *         VLT_U8 u8Mode,
 *         const VLT_ALGO_PARAMS *pAlgorithm );
 *
 * \brief Initializes a cryptographic algorithm.
 *
 * \par Description:
 *
 *
 * Initializes a cryptographic algorithm, a cryptographic key and conditionally
 * some specific algorithm parameters for subsequent cryptographic services.
 * The key is identified by the key group index and the key index given by the
 * u8KeyGroup and u8KeyIndex parameters. The key is fetched from the Internal
 * Key Ring.
 * The command data field optionally carries specific algorithm parameters. They
 * define all algorithm parameters to be applied to any subsequent cryptographic
 * operations.
 * \b Note: The logged-in user must have the Execute privilege on the involved key
 * file.
 * The VltInitializeAlgorithm() *shall* be sent before the following
 * cryptographic services:
 * - VltEncrypt()
 * - VltGenerateSignature() 
 * - VltComputeMessageDigest()
 * - VltGetChallenge()
 * - VltGenericInternalAuthenticate()
 *
 * The VltInitializeAlgorithm command *may* be sent before the following
 * cryptographic services:
 *
 * - VltPutKey() for key unwrapping (key sent encrypted to the
 *   device)
 * - VltReadKey() for key wrapping (key sent encrypted from the
 *   device)
 *
 * Any other command will discard the algorithm, wipe its parameters and unload
 * the key. Command Chaining is allowed for the underlying cryptographic
 * service.
 * As soon as the service type changes, the algorithm being initialized is
 * discarded. Algorithm parameters cannot be shared between different
 * cryptographic operations.
 * 
 * \b Note: A non-approved user must be logged-in to initialize a non-approved
 * algorithm.
 *
 * \param u8KeyGroup [IN]  Key Group index. Shall be zero for digest
 *                         initialization.
 * \param u8KeyIndex [IN]  Key index. Shall be zero for digest initialization.
 * \param u8Mode     [IN]  Mode of operation for subsequent commands.
 * \param pAlgorithm [OUT] Algorithm parameters.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltInitializeAlgorithm( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_U8 u8Mode,
    const VLT_ALGO_PARAMS *pAlgorithm );

/**
 * \fn VltPutKey( VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,
 *         const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
 *         const VLT_KEY_OBJECT *pKeyObj );
 *
 * \brief Imports a key into the internal Key Ring.
 *
 * \par Description:
 *
 * Imports a key into the internal Key Ring. The key is identified by the key
 * group index and they key index given by \a u8KeyGroup and \a u8KeyIndex. The
 * key file holding the imported key is automatically created and owned by the
 * currently logged-in user.
 *
 * The key can either be transported in plaintext or encrypted with a cipher
 * available. If key unwrapping is required, the VltInitialiseAlgorithm()
 * command shall be previously sent with the algorithm identifier set with a
 * valid Key Transport Scheme identifier.
 *
 * Note: The put key command shall be sent two times to download a key pair.
 *
 * Caution: In Approved Mode of operation, secret or private keys cannot be
 * downloaded in plaintext. A secure channel with encrypted command data field
 * C-ENC level shall be opened or a key wrapping mechanism shall be used.
 *
 * \param u8KeyGroup         [IN]  Key Group index.
 * \param u8KeyIndex         [IN]  Key index.
 * \param pKeyFilePrivileges [IN]  Key file Access Conditions. The logged-in
 *                                 user must grant its own user ID write
 *                                 permission on the key file.
 * \param pKeyObj            [IN]  Key object.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltPutKey( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
    const VLT_KEY_OBJECT *pKeyObj );

#ifdef PKCS11_VAULTIC_4XX_ROM2
VLT_STS VltPutKey_EcdsaParams( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
    VLT_U8 u8KeyID,
    const VLT_KEY_OBJ_ECDSA_PARAMS* pKeyObj,
    VLT_PSW pSW );
#endif
/**
 * \fn VltReadKey( VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,
 *         VLT_KEY_OBJECT *pKeyObj );
 *
 * \brief Exports a key from the internal Key Ring.
 *
 * \par Description:
 *
 * Exports a key from the internal Key Ring. The key is identified by the key
 * group index and the key index given by \a u8KeyGroup and \a u8KeyIndex.
 *
 * The key can either be transported in plaintext or encrypted by any cipher
 * available. If key wrapping is required, the VltInitializeAlgorithm()
 * command shall be previously sent with the algorithm identifier set with a
 * valid Key Transport Scheme identifier.
 *
 * Note: The logged-in user must have the read privilege on the involved key
 * file.
 *
 * \b Caution: In Approved Mode of operation, secret or private keys cannot be
 * extracted in plaintext. A secure channel with encrypted response data field
 * R-ENC level shall be opened or a key unwrapping mechanism shall be used.
 *
 * \param u8KeyGroup [IN]     Key Group index.
 * \param u8KeyIndex [IN]     Key index.
 * \param pKeyObj    [IN,OUT] Key object.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
  */
VLT_STS VltReadKey( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJECT *pKeyObj );

/**
 * \fn VltDeleteKey(VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex );
 *
 * \brief Deletes a key in the internal Key Ring.
 *
 * \par Description:
 *
 * Deletes a key in the internal Key Ring. The key is identified by the key
 * group index and the key index given by \a u8KeyGroup and \a u8KeyIndex.
 *
 * Note: The logged-in user must have the Delete privilege on the involved key
 * file.
 *
 * \param u8KeyGroup [IN]  Key Group index
 * \param u8KeyIndex [IN]  Key index
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltDeleteKey(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex );

/**
 * \fn VltEncrypt( VLT_U32 u32PlainTextLength,
 *         const VLT_U8 *pu8PlainText,
 *         VLT_PU32 pu32CipherTextLength,
 *         VLT_PU8 pu8CipherText );
 *
 * \brief Encrypts the provided message.
 *
 * \par Description:
 *
 * This command encrypts the provided message and outputs the result.
 *
 * The VltInitializeAlgorithm() command shall be previously sent with the
 * algorithm identifier set with a valid Cipher Identifier. The cipher key is
 * fetched at this time and the cipher engine is initialized with specific
 * algorithm parameters.
 *
 * \param u32PlainTextLength   [IN]     Length of plaintext.
 * \param pu8PlainText         [IN]     Plaintext.
 * \param pu32CipherTextLength [IN,OUT] On entry this holds the maximum size
 *                                      of the ciphertext buffer. On exit it is
 *                                      set to the amount of ciphertext buffer
 *                                      used.
 * \param pu8CipherText        [OUT]    Buffer to receive ciphertext.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltEncrypt( VLT_U32 u32PlainTextLength,
    const VLT_U8 *pu8PlainText,
    VLT_PU32 pu32CipherTextLength,
    VLT_PU8 pu8CipherText );

/**
 * \fn VltDecrypt( VLT_U32 u32CipherTextLength,
 *         const VLT_U8 *pu8CipherText,
 *         VLT_PU32 pu32PlainTextLength,
 *         VLT_PU8 pu8PlainText );
 *
 * \brief Decrypts the provided message.
 *
 * \par Description:
 *
 * This command decrypts the provided message and outputs the result.
 *
 * The VltInitializeAlgorithm() command shall be previously sent with the
 * algorithm identifier set with a valid Cipher Identifier. The cipher key is
 * fetched at this time and the cipher engine is initialized with specific
 * algorithm parameters.
 *
 * \param u32CipherTextLength [IN]     Length of ciphertext.
 * \param pu8CipherText       [IN]     Ciphertext.
 * \param pu32PlainTextLength [IN,OUT] On entry this holds the maximum size
 *                                     of the plaintext buffer. On exit it is
 *                                     set to the amount of plaintext buffer
 *                                     used.
 * \param pu8PlainText        [OUT]    Buffer to receive plaintext.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltDecrypt( VLT_U32 u32CipherTextLength,
    const VLT_U8 *pu8CipherText,
    VLT_PU32 pu32PlainTextLength,
    VLT_PU8 pu8PlainText );

/**
 * \fn VltGenerateAssuranceMessage( VLT_ASSURANCE_MESSAGE* pAssuranceMsg );
 *
 * \brief Generates an assurance message for private key possession assurance.
 *
 * \par Description:
 *
 * This command generates a valid assurance message for private key possession
 * assurance.
 *
 * \param pAssuranceMsg     [OUT]    Returned assurance message.
 * 
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltGenerateAssuranceMessage( VLT_ASSURANCE_MESSAGE* pAssuranceMsg );

/**
 * \fn VltGenerateSignature( VLT_U32 u32MessageLength,
 *         const VLT_U8 *pu8Message,
 *         VLT_PU16 pu16SignatureLength,
 *         VLT_PU8 pu8Signature );
 *
 * \brief Generates a signature from a message.
 *
 * \par Description:
 *
 * This command gets a raw message or a hashed message and returns its
 * signature.
 *
 * The VltInitialiseAlgorithm() command shall be previously sent with the
 * algorithm identifier set with a valid Signer Identifier. The signer key is
 * fetched at this time and the signer engine is initialized with specific
 * algorithm parameters.
 *
 * This command is also used for One Time Password Generation.
 *
 * \param u32MessageLength     [IN]     Length of input message.
 * \param pu8Message           [IN]     Input message.
 * \param pu16SignatureLength  [IN,OUT] On entry this holds the maximum size of
 *                                      the signature buffer. On exit it is set
 *                                      to the amount of signature buffer used.
 * \param pu8Signature         [IN]     Buffer to receive signature.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltGenerateSignature( VLT_U32 u32MessageLength,
    const VLT_U8 *pu8Message,
    VLT_PU16 pu16SignatureLength,
    VLT_PU8 pu8Signature );

VLT_STS VltUpdateSignature(VLT_U32 u32MessageLength,
						   const VLT_U8 *pu8Message);

VLT_STS VltComputeSignatureFinal( VLT_PU16 pu16SignatureLength,
    VLT_PU8 pu8Signature );

/**
 * \fn VltVerifySignature( VLT_U32 u32MessageLength,
 *         const VLT_U8 *pu8Message,
 *         VLT_U16 u16SignatureLength,
 *         const VLT_U8 *pu8Signature );
 *
 * \brief Generates a signature from a message.
 *
 * \par Description:
 *
 * This command gets a raw message or a hashed message and a signature and
 * verifies the signature.
 *
 * The VltInitialiseAlgorithm() command shall be previously sent with the
 * algorithm identifier set with a valid Signer Identifier. The signer key is
 * fetched at this time and the signer engine is initialized with specific
 * algorithm parameters.
 *
 * This command is also used for One Time Password Generation.
 *
 * \param u32MessageLength   [IN]  Length of input message.
 * \param pu8Message         [IN]  Input message.
 * \param u16SignatureLength [IN]  Length of buffer to receive signature.
 * \param pu8Signature       [IN]  Buffer to receive signature.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltVerifySignature( VLT_U32 u32MessageLength,
    const VLT_U8 *pu8Message,
    VLT_U16 u16SignatureLength,
    const VLT_U8 *pu8Signature );

VLT_STS VltUpdateVerify(VLT_U32 u32MessageLength,
						   const VLT_U8 *pu8Message);

VLT_STS VltComputeVerifyFinal(VLT_U16 u32MessageLength,
						   const VLT_U8 *pu8Message);
/**
 * \fn VltComputeMessageDigest( VLT_U32 u32MessageLength,
 *         const VLT_U8 *pu8Message,
 *         VLT_PU8 pu8DigestLength,
 *         VLT_PU8 pu8Digest );
 *
 * \brief Computes the digest of a message.
 *
 * \par Description:
 *
 * This command computes a digest of the provided message.
 *
 * The VltInitializeAlgorithm() command shall be previously sent with the
 * algorithm identifier set with a valid Digest Identifier.
 *
 * \param u32MessageLength [IN]     Length of input message.
 * \param pu8Message       [IN]     Input message.
 * \param pu8DigestLength  [IN,OUT] On entry this holds the maximum size of the
 *                                  digest buffer. On exit it is set to the
 *                                  amount of digest buffer used.
 * \param pu8Digest        [IN]     Buffer to receive digest.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltComputeMessageDigest( VLT_U32 u32MessageLength,
    const VLT_U8 *pu8Message,
    VLT_PU8 pu8DigestLength,
    VLT_PU8 pu8Digest );

VLT_STS VltUpdateMessageDigest( VLT_U32 u32MessageLength,
    const VLT_U8 *pu8Message);

VLT_STS VltComputeMessageDigestFinal( VLT_PU8 pu8DigestLength,
    VLT_PU8 pu8Digest );

/**
 * \fn VltGenerateRandom( VLT_U8 u8NumberOfCharacters, 
 *         VLT_PU8 pu8RandomCharacters );
 *
 * \brief Generates random bytes.
 *
 * \par Description:
 *
 * This command fills the buffer with the requested number of random bytes.
 *
 * \param u8NumberOfCharacters  [IN]  Number of random characters to
 *                                    generate (1..255).
 * \param pu8RandomCharacters   [OUT] Buffer to receive random characters.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltGenerateRandom( VLT_U8 u8NumberOfCharacters, 
    VLT_PU8 pu8RandomCharacters );

/**
 * \fn VltGenerateKeyPair( VLT_U8 u8PublicKeyGroup,
 *         VLT_U8 u8PublicKeyIndex,
 *         const VLT_FILE_PRIVILEGES *pPublicKeyFilePrivileges,
 *         VLT_U8 u8PrivateKeyGroup,
 *         VLT_U8 u8PrivateKeyIndex,
 *         const VLT_FILE_PRIVILEGES *pPrivateKeyFilePrivileges,
 *         const VLT_KEY_GEN_DATA *pKeyGenData );
 *
 * \brief Generates a public key pair and stores it in the key ring.
 *
 * \par Description:
 *
 * Generates a public key pair and stores it in the key ring. This command can
 * generate DSA, ECDSA and RSA keys. The key files holding the generated key
 * pair are automatically created and owned by the currently logged-in user.
 *
 * \param u8PublicKeyGroup          [IN]  Public key group index.
 * \param u8PublicKeyIndex          [IN]  Public key index.
 * \param pPublicKeyFilePrivileges  [IN]  Public key file Access Conditions. The
 *                                        logged-in operator must grant its own 
 *                                        user ID write permission on the key
 *                                        file.
 * \param u8PrivateKeyGroup         [IN]  Private key group index.
 * \param u8PrivateKeyIndex         [IN]  Private key index.
 * \param pPrivateKeyFilePrivileges [IN]  Private key file Access Conditions. The
 *                                        logged-in operator must grant its own
 *                                        user ID write permission on the key
 *                                        file.
 * \param pKeyGenData               [IN]  Algorithm Parameters Object.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltGenerateKeyPair( VLT_U8 u8PublicKeyGroup,
    VLT_U8 u8PublicKeyIndex,
    const VLT_FILE_PRIVILEGES *pPublicKeyFilePrivileges,
    VLT_U8 u8PrivateKeyGroup,
    VLT_U8 u8PrivateKeyIndex,
    const VLT_FILE_PRIVILEGES *pPrivateKeyFilePrivileges,
    const VLT_KEY_GEN_DATA *pKeyGenData );

/* --------------------------------------------------------------------------
 * FILE SYSTEM SERVICES
 * -------------------------------------------------------------------------- */

/**
 * \fn VltBeginTransaction( void );
 *
 * \brief Starts Transactions mechanism on file system updates.
 *
 * \par Description:
 *
 * Starts Transactions mechanism on file system updates.
 *
 * VltBeginTransaction command shall be invoked prior to start sensitive file
 * update operations. All the following file system updates will be protected
 * against power-loss event. File system integrity and data consistency will be
 * guaranteed,
 *
 * If a tear event occurs while a transaction is in progress, any updates to
 * files content and file system structure are discarded. VaultIC anti-tearing
 * engine will restore the file system as it was when VltBeginTransaction
 * command has been received. The transaction is committed by 
 * VltEndTransaction() command.
 *
 * \b Caution: When a transaction is started, the currently selected file or folder
 * is unselected. A VltSelectFileOrDirectory() command shall be sent inside
 * the transaction.
 *
 * \b Caution: The content of the file system which is update outside a secure
 * transaction is not predictable following a tear or reset during the update.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltBeginTransaction( void );

/**
 * \fn VltEndTransaction( void );
 *
 * \brief Commits Transactions mechanism on file system updates.
 *
 * \par Description:
 *
 * Commits Transactions mechanism on file system updates.
 *
 * Any file system updates during a transaction are only done conditionally. All
 * these conditional updates shall be committed at the very end of the
 * transaction sending VltEndTransaction command.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltEndTransaction( void );

/**
 * \fn VltSelectFileOrDirectory(const VLT_U8 *pu8Path, 
 *         VLT_U8 u8PathLength, 
 *         VLT_SELECT *pRespData );
 *
 * \brief Selects a file or directory.
 *
 * \par Description:
 *
 * Selects a file or directory and retrieves the file size and the access
 * conditions.
 *
 * When selecting a file, the Current File Position is initialized to the first
 * byte of the file.
 *
 * Secure Transaction is supported.
 *
 * \param pu8Path      [IN]  Path.
 * \param u8PathLength [IN]  Path length, excluding NUL terminator.
 * \param pRespData    [OUT] Returned file size, access conditions and
 *                           attributes.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltSelectFileOrDirectory(const VLT_U8 *pu8Path, 
    VLT_U8 u8PathLength, 
    VLT_SELECT *pRespData );

/**
 * \fn VltListFiles( VLT_PU16 pu16ListRespLength,
 *         VLT_PU8 pu8RespData );
 *
 * \brief Returns a list of files in the currently selected directory.
 *
 * \par Description:
 *
 * Returns a multi-string containing the names of files and directories located
 * in the currently selected directory.
 *
 * Files or directories with the hidden attribute are skipped out the listing.
 *
 * Note: A multi-string is a sequence of NUL-terminated strings. The sequence is
 * terminated by a NUL byte, therefore the multi-string ends up by two
 * consecutive bytes.
 *
 * Note: The order in which files are reported may be different before and after
 * a secure transaction performed on the selected directory.
 *
 * Note: The logged-in operator must have the List privilege on the selected
 * directory.
 *
 * A single NULL byte is returned if the current directory does not contain any
 * files or sub-directories.
 *
 * An error code is returned f the buffer passed in is not large enough to 
 * accomodate all of the data returned by the Vault IC.  The pu16ListRespLength
 * parameter is updated to the size of the buffer required to return the full 
 * directoty listing.  A partial directory listing will be contained within the
*  buffer, but this should be ignored.  A buffer of the size reported  back by 
* the updated pu16ListRespLength should be constructed and passed as the input
* paramteters to VltListFiles.
 *
 * \param pu16ListRespLength [IN,OUT] On entry this holds the maximum size of
 *                                    the buffer. On exit it is set to the
 *                                    amount of buffer used.
 * \param pu8RespData        [IN]     Buffer to receive multi-string.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltListFiles( VLT_PU16 pu16ListRespLength,
    VLT_PU8 pu8RespData );

/**
 * \fn VltCreateFile( VLT_U8 u8UserID,
 *         VLT_U32 u32FileSize, 
 *         const VLT_FILE_PRIVILEGES *pFilePriv,
 *         VLT_U8 u8FileAttribute,
 *         VLT_U16 u16FileNameLength,
 *         const VLT_U8 *pu8FileName );
 *
 * \brief Creates a new file.
 *
 * \par Description:
 *
 * Creates a new file in the currently selected directory. The newly created
 * file becomes the currently selected file.
 *
 * \b Note: The logged-in operator must have the Create privilege on the selected
 * directory.
 *
 * \param u8UserID [IN]  Operator ID (0..7). Possible values are: 
 * - #VLT_USER0 
 * - #VLT_USER1 
 * - #VLT_USER2 
 * - #VLT_USER3 
 * - #VLT_USER4 
 * - #VLT_USER5 
 * - #VLT_USER6 
 * - #VLT_USER7 
 * \param u32FileSize       [IN]  Initial file size in bytes. Maximum file size
 *                                is 65535 bytes.
 * \param pFilePriv         [IN]  Access conditions.
 * \param u8FileAttribute   [IN]  Attributes.
 * \param u16FileNameLength [IN]  Length of file name (1..8).
 * \param pu8FileName       [IN]  File name.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltCreateFile( VLT_U8 u8UserID,
    VLT_U32 u32FileSize, 
    const VLT_FILE_PRIVILEGES *pFilePriv,
    VLT_U8 u8FileAttribute,
    VLT_U16 u16FileNameLength,
    const VLT_U8 *pu8FileName );

/**
 * \fn VltCreateFolder( VLT_U8 u8UserID,
 *         const VLT_FILE_PRIVILEGES *pFilePriv,                                   
 *         VLT_U8 u8FolderAttribute,
 *         VLT_U16 u16FolderNameLength,
 *         const VLT_U8 *pu8FolderName );
 *
 * \brief Creates a new folder.
 *
 * \par Description:
 *
 * Creates a new sub-directory in the currently selected directory.
 *
 * Note: The logged-in operator must have the Create privilege on the selected
 * directory.
 *
 * \param u8UserID [IN]  Operator ID (0..7). Possible values are: 
 * - #VLT_USER0 
 * - #VLT_USER1 
 * - #VLT_USER2 
 * - #VLT_USER3 
 * - #VLT_USER4 
 * - #VLT_USER5 
 * - #VLT_USER6 
 * - #VLT_USER7 
 * \param pFilePriv           [IN]  Access conditions.
 * \param u8FolderAttribute   [IN]  Attributes.
 * \param u16FolderNameLength [IN]  Length of folder name (1..8).
 * \param pu8FolderName       [IN]  Folder name.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltCreateFolder( VLT_U8 u8UserID,
    const VLT_FILE_PRIVILEGES *pFilePriv,                                   
    VLT_U8 u8FolderAttribute,
    VLT_U16 u16FolderNameLength,
    const VLT_U8 *pu8FolderName );

/**
 * \fn VltDeleteFile( void );
 *
 * \brief Deletes the current file.
 *
 * \par Description:
 *
 * Deletes the current file. Read-only files are protected against deletion.
 *
 * Once the file is deleted. The parent directory is still the current directory
 * but no file is selected.
 *
 * Note: The logged-in operator must have the Delete privilege on the selected
 * directory.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltDeleteFile( void );

 /**
 * \fn VltDeleteFolder( VLT_U8 u8Recursion );
 *
 * \brief Deletes the current directory.
 *
 * \par Description:
 *
 * Deletes the current directory.
 *
 * Once the folder is deleted, the root directory becomes the current directory.
 *
 * Note: The logged-in operator must have the Delete privilege on the selected
 * directory.
 *
 * Note: If recursion is required, Delete privilege must be granted on all files
 * and directories located under the selected directory.
 *
 * \param u8Recursion [IN]  If non-zero delete all contained files and sub-
 *                          folders.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltDeleteFolder( VLT_U8 u8Recursion );

/**
 * \fn VltWriteFile( const VLT_U8 *pu8Data,
 *         VLT_U8 u8DataLength,
 *         VLT_U8 u8ReclaimSpace );
 *
 * \brief Updates part of the contents of the current file.
 *
 * \par Description:
 *
 * Updates part of the contents of the current file, from the current file
 * position. Write operation is now allowed on read-only files.
 *
 * If the provided data go beyond the End Of File, the file will grow
 * automatically provided there is enough space in the file system. It is also
 * possible to shrink the file and discard previous data that were beyond the
 * new End Of File. File system space is allocated/reclaimed accordingly.
 *
 * The current file position is set at the end of the written data.
 *
 * \param pu8Data        [IN]  Data to write.
 * \param u8DataLength   [IN]  Length of data (1..255).
 * \param u8ReclaimSpace [IN] #VLT_NO_RECLAIM_SPACE or #VLT_RECLAIM_SPACE
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltWriteFile( const VLT_U8 *pu8Data,
    VLT_U8 u8DataLength,
    VLT_U8 u8ReclaimSpace );

/**
 * \fn VltReadFile( VLT_PU8 pu8ReadLength,
 *         VLT_PU8 pu8RespData );
 *
 * \brief Reads part of the contents of the current file.
 *
 * \par Description:
 *
 * Reads part of the contents of the current file from the current file
 * position.
 *
 * If the requested length goes beyond the End of File, only the available data
 * are returned and a specific status is returned.
 *
 * The current file position is set at the end of the read data.
 *
 * If pu8ReadLength specifies a value larger than the Vault IC can return, an
 * error will be returned and pu8ReadLength will be set to the maximum number
 * of bytes that can be read
 *
 * \param pu8ReadLength [IN,OUT]  On entry this holds the maximum size of the
 *                                buffer. On exit it is set to the amount of
 *                                buffer used.
 * \param pu8RespData    [IN]     Data to read.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltReadFile( VLT_PU16 pu16ReadLength,
    VLT_PU8 pu8RespData );

/**
 * \fn VltSeekFile( VLT_U32 u32SeekLength );
 *
 * \brief Sets the current file position.
 *
 * \par Description:
 *
 * Sets the current file position.
 *
 * If the provided offset is beyond the End Of File, a specific status is
 * returned, and the current file position is set just after the last byte of
 * the file (so that a write operation can be performed to append data to the
 * file).
 *
 * Note: The logged-in operator must have read or write privilege on the
 * selected file.
 *
 * \param u32SeekLength [IN]  The new file position relative to the beginning of
 *                            the file.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltSeekFile( VLT_U32 u32SeekLength );

/**
 * \fn VltSetPrivileges( const VLT_FILE_PRIVILEGES *pFilePriv );
 *
 * \brief Updates Access Conditions of the currently selected file or folder.
 *
 * \par Description:
 *
 * Updates Access Conditions of the currently selected file or folder.
 *
 * Note: The logged-in operator must be the owner of the selected file or
 * folder.
 *
 * Note: This operation is protected against tear event without the need to
 * initiate a transaction.
 *
 * \param pFilePriv [IN]  File or folder Access Conditions.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltSetPrivileges( const VLT_FILE_PRIVILEGES *pFilePriv );

/**
 * \fn VltSetAttributes( VLT_U8 u8Attribute );
 *
 * \brief Updates Attributes of the currently selected file.
 *
 * \par Description:
 *
 * Updates Attributes of the currently selected file.
 *
 * Note: The logged-in operator must be the owner of the selected file.
 *
 * Note: This operation is protected against tear event without the need to
 * initiate a transaction.
 *
 * \param u8Attribute [IN]  Attributes.
 *
  * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltSetAttributes( VLT_U8 u8Attribute );

/* --------------------------------------------------------------------------
 * MANUFACTURING COMMANDS
 * -------------------------------------------------------------------------- */

/**
 * \fn VltGetInfo( VLT_TARGET_INFO *pRespData );
 *
 * \brief Returns information about the security module.
 *
 * \par Description:
 *
 * This command returns some information about the security module: chip
 * identifiers, unique serial number, life cycle state and available file system
 * space.
 *
 * \param pRespData [OUT] Structure to receive information.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltGetInfo( VLT_TARGET_INFO *pRespData );

/**
 * \fn VltSelfTest( void );
 *
 * \brief Initiates and runs self testing.
 *
 * \par Description:
 *
 * Initiates and runs self testing.
 *
 * Self-tests sequence ensures that VaultIC is working properly and is
 * automatically performed at each power-up or reset. The self-tests command
 * allows any user, authenticated or not, to initiate the same test flow
 * on-demand for periodic test of the VaultIC.
 *
 * During self-tests operation, all approved cryptographic algorithms are tested
 * using known-answer and firmware integrity is checked using CRC-16 CCITT.
 *
 * If self-tests failed, any authentication is cancelled, any secure channel is
 * closed and the VaultIC is automatically switched to TERMINATED state.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltSelfTest( void );

/**
 * \fn VltSetStatus( VLT_U8 u8State );
 *
 * \brief Changes Life Cycle state.
 *
 * \param u8State [OUT] New state value.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltSetStatus( VLT_U8 u8State );

/**
 * \fn VltSetConfig(VLT_U8 u8ConfigItem,
 *         VLT_U8 u8DataLength,
 *         VLT_PU8 pu8ConfigData );
 *
 * \brief Sets VaultIC configuration parameters.
 *
 * \par Description:
 *
 * Defines internal parameters of the VaultIC chip. These settings are applied
 * using the anti-tearing mechanism and permanently stored into the internal
 * memory.
 *
 * Caution: Setting wrong values may damage the VaultIC chip. Use with caution.
 *
 * \param u8ConfigItem  [IN]  VLT_USB_*, VLT_ADMIN_*
 * \param u8DataLength  [IN]  Length of data buffer.
 * \param pu8ConfigData [IN]  Data buffer containing configuration value.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltSetConfig(VLT_U8 u8ConfigItem,
    VLT_U8 u8DataLength,
    VLT_PU8 pu8ConfigData );

/**
 * \fn VltSetGpioDirection( VLT_U8 u8GpioDirMask, 
 *         VLT_U8 u8GpioMode );
 *
 * \brief Manages the VaultIC GPIO lines.
 *
 * \par Description:
 *
 * Changes the direction of the GPIO lines between input and output directions.
 * Optionally activates CMOS mode for output lines.
 *
 *
 * \param u8GpioDirMask [IN]  Bitfield representing the GPIO direction. Each 
 *                            bit corresponds to a physical line:
 *                            Di: 0 <= i <= 7 direction for GPIO(i) where:
 *                            0 = Input Direction
 *                            1 = Output Direction
 *
 * \param u8GpioMode    [IN]  Bitfield representing the GPIO output mode. Each 
 *                            bit corresponds to a physical line:
 *                            Di: 0 <= i <= 7 direction for GPIO(i) where:
 *                            0 = Open Drain Mode
 *                            1 = CMOS Mode
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltSetGpioDirection( VLT_U8 u8GpioDirMask, 
    VLT_U8 u8GpioMode );

/**
 * \fn VltWriteGpio( VLT_U8 u8GpioValue );
 *
 * \brief Writes to the VaultIC GPIO lines.
 *
 * \par Description:
 *
 * Sets the value of the current GPIO outputs.
 *
 * \param u8GpioValue   [IN]  Bitfield representing the GPIO output mode. Each 
 *                            bit corresponds to a physical line:
 *                            Di: 0 <= i <= 7 output for GPIO(i) where:
 *                            0 = Sets to logic low 
 *                            1 = Sets to logic high 
 *
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltWriteGpio( VLT_U8 u8GpioValue );

/**
 * \fn VltReadGpio( VLT_PU8 pu8GpioValue );
 *
 * \brief Reads value of the VaultIC GPIO lines.
 *
 * \par Description:
 *
 * Reads the physical value of the GPIO lines (both inputs and outputs)
 *
 * \param pu8GpioValue  [IN]  Bitfield representing the port physical value.
 *                            Each bit corresponds to a physical line:
 *                            Di: 0 <= i <= 7 output for GPIO(i) where:
 *                            0 = Reads as logic low 
 *                            1 = Reads as logic high 
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltReadGpio( VLT_PU8 pu8GpioValue );

/**
 * \fn VltTestCase1( void );
 *
 * \brief Dummy APDU case 1 command for integration testing.
 *
 * \par Description:
 *
 * This command is a dummy APDU case 1 command for integration testing purposes.
 *
 * When a secure channel has been initiated, this command must be sent INSIDE
 * the secure channel, i.e. applying the security level required by the current
 * secure channel. Not doing so will close the secure channel and
 * de-authenticate the user.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltTestCase1( void );

/**
 * \fn VltTestCase2( VLT_U8 u8RequestedDataLength, 
 *         VLT_PU8 pu8RespData );
 *
 * \brief Dummy APDU case 2 command for integration testing.
 *
 * \par Description:
 *
 * This command is a dummy APDU case 2 command for integration testing purposes.
 *
 * When a secure channel has been initiated, this command must be sent INSIDE
 * the secure channel, i.e. applying the security level required by the current
 * secure channel. Not doing so will close the secure channel and
 * de-authenticate the user.
 *
 * \param u8RequestedDataLength [IN]  Number of output bytes (1..255).
 * \param pu8RespData           [IN]  Buffer to receive output bytes.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltTestCase2( VLT_U8 u8RequestedDataLength, 
    VLT_PU8 pu8RespData );

/**
 * \fn VltTestCase3(VLT_U8 u8DataLength,
 *         const VLT_U8 *pu8Data );
 *
 * \brief Dummy APDU case 3 command for integration testing.
 *
 * \par Description:
 *
 * This command is a dummy APDU case 3 command for integration testing purposes.
 *
 * When a secure channel has been initiated, this command must be sent INSIDE
 * the secure channel, i.e. applying the security level required by the current
 * secure channel. Not doing so will close the secure channel and
 * de-authenticate the user.
 *
 * \param u8DataLength [IN]  Number of input bytes (1..255).
 * \param pu8Data      [IN]  Input bytes.
 * 
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltTestCase3(VLT_U8 u8DataLength,
     const VLT_U8 *pu8Data );

/**
 * \fn VltTestCase4(VLT_U8 u8DataLength,
 *         const VLT_U8 *pu8Data, 
 *         VLT_U8 u8RequestedDataLength,
 *         VLT_PU8 pu8RespData );
 *
 * \brief Dummy APDU case 4 command for integration testing.
 *
 * \par Description:
 *
 * This command is a dummy APDU case 4 command for integration testing purposes.
 *
 * When a secure channel has been initiated, this command must be sent INSIDE
 * the secure channel, i.e. applying the security level required by the current
 * secure channel. Not doing so will close the secure channel and
 * de-authenticate the user.
 *
 * \param u8DataLength          [IN]  Number of input bytes (1..255).
 * \param pu8Data               [IN]  Input bytes.
 * \param u8RequestedDataLength [IN]  Number of output bytes (1..255).
 * \param pu8RespData           [IN]  Buffer to receive output bytes.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltTestCase4(VLT_U8 u8DataLength,
    const VLT_U8 *pu8Data, 
    VLT_U8 u8RequestedDataLength,
    VLT_PU8 pu8RespData );

/**
 * \fn VltGetLibInfo( VLT_LIBRARY_INFO* pLibraryInfo )
 *
 * \brief Provides the properties of the VaultIC API Library.
 *
 * \par Description:
 *
 * The VltGetLibInfo() method provides the caller with a list of properties
 * currently supported by the Vault IC API Library. The properties are provided 
 * as an comprehensive structure of type ::VLT_LIBRARY_INFO. It is the 
 * responsibility of the caller to allocate the memory that will be used for the 
 * ::VLT_LIBRARY_INFO structure and subsequently pass in a pointer to it. Upon 
 * successful completion the VltGetLibInfo() will populate the fields of the 
 * structure with the appropriate values. 
 *
 * The VltGetLibInfo() method has a fundamentaly different dependency model from 
 * all other VaultIC API library methods. The VltGetLibInfo() method may be 
 * called before a call to the VltInit() method has been made.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned.
 *
 * \par Example:
 * \code 
 *  VLT_STS status = VLT_FAIL 
 *  VLT_LIBRARY_INFO libraryInfo;
 *  VLT_BOOL isAuthSupported = FALSE;
 * 
 *  status = VltGetLibraryInfo( &libraryInfo );
 *  if( VLT_OK != status )
 *  {
 *      // Exit the call
 *      return( -1 );
 *  }
 * 
 *  // Determine if the Authentication Service is supported
 *  if( libraryInfo.capabilities & VLT_CPB_ENABLE_IDENTITY_AUTH )
 *  {
 *      isAuthSupported = TRUE;
 *  }
 * 
 * \endcode
 */
VLT_STS VltDeriveKey(VLT_U8 u8keyGroup,
    VLT_U8 u8keyIndex,
	const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
	VLT_U8 u8DerivatedKeyType,
	VLT_U16 u16WDerivatedKeyLen,
	const VLT_KEY_DERIVATION *pKeyDerivation );

VLT_STS VltConstructDHAgreement(	
	VLT_U8 u8resultKeyGroup,
    VLT_U8 u8resultKeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
	const VLT_KEY_MATERIAL *pKeyMaterial );

VLT_STS VltGetLibInfo( VLT_LIBRARY_INFO* pLibraryInfo );

VLT_STS VltIso7816PtclDetectReaders( VLT_PU32 pSize, VLT_PU8 pXmlReaderString );

VLT_STS VltFindDevices( VLT_PU32 pSize, VLT_PU8 pXmlReaderString );

#if(VLT_ENABLE_ISO7816 == VLT_ENABLE )
VLT_STS VltSelectCard( SCARDHANDLE hScard, SCARDCONTEXT hCxt, DWORD dwProtocol);

VLT_STS VltIso7816PtclClose( void );
#endif

VLT_STS VltCloseLibrary( void );

VLT_STS VltUnInitializeAlgorithm();

VLT_STS VltInitLibrary( VLT_INIT_COMMS_PARAMS* pInitCommsParams );

}
#endif
 
#endif //__VAULTICAPI_H__