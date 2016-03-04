/* --------------------------------------------------------------------------
 * VaultIC API for the VaultIC 100 family of devices.
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_api.h
 * 
 * \defgroup VaultIcApi Vault IC API
 *
 * \brief Interface functions to VaultIC.
 * 
 * \par Description:
 *
 * This file declares the interface functions for each command that the VaultIC
 * 100 family supports. There is an approximate 1:1 mapping between functions 
 * and VaultIC 100 family commands. In cases where a VaultIC interface supports 
 * multiple modes of operation it is presented as two C interfaces for clarity 
 * and type safety.
 */
/*@{*/

#ifndef VAULTIC_API_H
#define VAULTIC_API_H

#ifndef WIN32
#include <PCSC/wintypes.h>
#endif
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
 * \fn VLT_STS VltSubmitPassword( VLT_U8 u8PasswordLength,
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
 * \param u8PasswordLength [IN]  Password length (32).
 * \param pu8Password      [IN]  Password.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC. 
 */
VLT_STS VltSubmitPassword( VLT_U8 u8PasswordLength,
    const VLT_U8 *pu8Password );


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
 * \param u8UserID  [IN]  Operator ID. Possible values are:
 * - #VLT_CREATOR
 * - #VLT_USER
 *
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
 * \fn VltInternalAuthenticate( VLT_U8 u8UserID, 
 *         VLT_U8 u8RoleID,
 *         VLT_U8 u8HostChallengeLength,
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
 * \param u8UserID [IN]  Operator ID. Possible values are: 
 * - #VLT_USER
 * - 0x00
 *
 * \param u8RoleID         [IN]  Role ID. Possible values are:
 * - #VLT_APPROVED_USER     
 * - 0x00
 *
 * \param u8ChallengeLengths  [IN]     The length of the host and device 
 *                                     challenges (Ch & Cd).
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
VLT_STS VltInternalAuthenticate(VLT_U8 u8UserID, 
    VLT_U8 u8RoleID,
    VLT_U8 u8ChallengeLengths,
    const VLT_U8 *pu8HostChallenge,
    VLT_PU8 pu8DeviceChallenge,
    VLT_PU16 pu16SignatureLength,
    VLT_PU8 pu8Signature  );

/**
 * \fn VltExternalAuthenticate(VLT_U8 u8HostChallengeLength,
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
  * \param u8UserID [IN]  Operator ID. Possible values are: 
 * - #VLT_USER
 *
 * \param u8RoleID         [IN]  Role ID. Possible values are:
 * - #VLT_APPROVED_USER     
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
 * \see VltInternalAuthenticate command.
 */
VLT_STS VltExternalAuthenticate( VLT_U8 u8UserID,
    VLT_U8 u8RoleID,
    VLT_U8 u8HostChallengeLength,
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
 * \fn VltGenerateAssuranceMessage( VLT_PU8 pu8SignerIdLength,
    VLT_PU8 pu8SignerID,
    VLT_ASSURANCE_MESSAGE* pAssuranceMsg );
 *
 * \brief Generates an assurance message for private key possession assurance.
 *
 * \par Description:
 *
 * This command generates a valid assurance message for private key possession
 * assurance.
 *
 * \param pu8SignerIdLength [IN,OUT] The SignerID length must equal 
 *                                   VLT_GA_SIGNER_ID_LENGTH.
 * \param pu8SignerID       [IN,OUT] The SignerID, the VaultIC returns the 
 *                                   actual assigned SignerID, check the values
 *                                   match.
 * \param pAssuranceMsg     [OUT]    Returned assurance message.
 * 
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltGenerateAssuranceMessage(VLT_PU8 pu8SignerIdLength,
    VLT_PU8 pu8SignerID,
    VLT_ASSURANCE_MESSAGE* pAssuranceMsg );

/**
 * \fn VltVerifyAssuranceMessage( VLT_PU8 pu8SignerIdLength,
    VLT_PU8 pu8SignerID,
    VLT_ASSURANCE_MESSAGE* pAssuranceMsg );
 *
 * \brief Verifies an assurance message for private key possession assurance.
 *
 * \par Description:
 *
 * This command verifies signed assurance message for private key possession
 * assurance.  The VltGenerateAssurance method MUST be called immediately
 * before this command.
 *
 * \param u8SignedAssuranceMsgLength [IN] The Signed assurance message length.
 * \param pu8SignedAssuranceMsg      [IN] The Signed, assurance message signed 
 *                                        using hosts private key.  VaultIC will
 *                                        verfy the signed message using the 
 *                                        hosts public key (stored in the 
 *                                        vaultIC).
 * 
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltVerifyAssuranceMessage(VLT_U8 u8SignedAssuranceMsgLength,
    VLT_PU8 pu8SignedAssuranceMsg);

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

/**
 * \fn VLT_STS VltUpdateSignature(
 *		VLT_U32 u32MessagePartLength,
 *      const VLT_PU8 *pu8MessagePart);
 *
 * \brief Continue a multi-part signature from a message.
 *
 * \par Description:
 *
 * This command continues a multiple-part signature operation, 
 * processing another data part.
 *
 * Before the first call to this command, the VltInitializeAlgorithm() command 
 * shall be previously sent with the algorithm identifier set with a valid Signer 
 * Identifier. The signer key is fetched at this time and the signer engine is 
 * initialized with specific algorithm parameters.
 *
 * \param u32MessagePartLength     [IN]     Length of input message part.
 * \param pu8MessagePart           [IN]     Input message part.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltUpdateSignature(VLT_U32 u32MessagePartLength,
        const VLT_U8 *pu8MessagePart);
/**
 * \fn VltComputeSignatureFinal( VLT_PU16 pu16SignatureLength,
 *         VLT_PU8 pu8Signature );
 *
 * \brief Finishes a multiple-part signature operation, returning the signature.
 *
 * \par Description:
 *
 * \param pu16SignatureLength  [IN,OUT] On entry this holds the maximum size of
 *                                      the signature buffer. On exit it is set
 *                                      to the amount of signature buffer used.
 * \param pu8Signature         [OUT]     Buffer to receive signature.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
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
/**
 * \fn VLT_STS VltUpdateVerify(VLT_U32 u32MessagePartLength,
 *  const VLT_U8 *pu8PartMessage);
 *
 * \brief Continue a multi-part verification from a message.
 *
 * \par Description:
 *
 * This command continues a multiple-part verification operation, 
 * processing another data part.
 *
 * Before the first call to this command, the VltInitializeAlgorithm() command 
 * shall be previously sent with the algorithm identifier set with a valid Signer 
 * Identifier. The signer key is fetched at this time and the signer engine is 
 * initialized with specific algorithm parameters.
 *
 * \param u32MessagePartLength     [IN]     Length of input message part.
 * \param pu8MessagePart           [IN]     Input message part.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltUpdateVerify(VLT_U32 u32MessagePartLength,
    const VLT_U8 *pu8PartMessage);
/**
 * \fn VLT_STS VltComputeVerifyFinal(VLT_U32 u32SignatureLength,
 *  const VLT_U8 *pu8Signature);
 *
 * \brief Finishes a multiple-part verification operation, checking the signature.
 *
 * \par Description:
 *
 * \param u32SignatureLength  [IN]	It is set to the  signature buffer length.
 * \param pu8Signature        [IN]  Buffer which contains the signature.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltComputeVerifyFinal(VLT_U32 u32SignatureLength,
    const VLT_U8 *pu8Signature);
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
/**
 * \fn VLT_STS VltUpdateMessageDigest( VLT_U32 u32MessagePartLength,
 *  const VLT_U8 *pu8PartMessage);
 *
 * \brief Continue a multi-part message digest operation from a message.
 *
 * \par Description:
 *
 * This command continues a multiple-part message digest operation, 
 * processing another data part.
 *
 * Before the first call to this command, the VltInitializeAlgorithm() command 
 * shall be previously sent with the algorithm identifier set with a valid Signer 
 * Identifier. The signer key is fetched at this time and the signer engine is 
 * initialized with specific algorithm parameters.
 *
 * \param u32MessagePartLength     [IN]     Length of input message part.
 * \param pu8MessagePart           [IN]     Input message part.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltUpdateMessageDigest( VLT_U32 u32MessagePartLength,
    const VLT_U8 *pu8PartMessage);
/**
 * \fn VLT_STS VltComputeMessageDigestFinal( VLT_PU8 pu8DigestLength,
 *   VLT_PU8 pu8Digest );
 *
 * \brief Finishes a multiple-part message-digesting operation, returnning the 
 * message digest.
 *
 * \par Description:
 *
 * \param pu8DigestLength	[IN,OUT]	It is set to the  signature buffer length.
 * \param pu8Digest			[OUT]		Buffer which contains the message digest.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
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
 *         VLT_U8 u8PrivateKeyGroup,
 *         VLT_U8 u8PrivateKeyIndex,
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
 * \param u8PrivateKeyGroup         [IN]  Private key group index.
 * \param u8PrivateKeyIndex         [IN]  Private key index.
 * \param u8DomainParamsGroup       [IN]  Domain params key group index.
 * \param u8DomainParamsIndex       [IN]  Domain params key index.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltGenerateKeyPair(VLT_U8 u8PublicKeyGroup,
    VLT_U8 u8PublicKeyIndex,
    VLT_U8 u8PrivateKeyGroup,
    VLT_U8 u8PrivateKeyIndex,
    const VLT_KEY_GEN_DATA *pKeyGenData );

/* --------------------------------------------------------------------------
 * FILE SYSTEM SERVICES
 * -------------------------------------------------------------------------- */


/**
 * \fn VltSelectFile(const VLT_U8 *pu8Path, 
 *         VLT_U8 u8PathLength, 
 *         VLT_SELECT *pRespData );
 *
 * \brief Selects a file or directory.
 *
 * \par Description:
 *
 * Selects a file and retrieves the file size and the access
 * conditions.
 *
 * The Current File Position is initialized to the first
 * byte of the file.
 *
 * Secure Transaction is supported.
 *
 * \param pu8Path      [IN]  Path.
 * \param u8PathLength [IN]  Path length, including NULL terminator.
 * \param pRespData    [OUT] Returned file size, access conditions and
 *                           attributes.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltSelectFile(const VLT_U8 *pu8Path, 
    VLT_U8 u8PathLength, 
    VLT_SELECT *pRespData );

/**
 * \fn VltWriteFile( const VLT_U8 *pu8Data,
 *         VLT_U8 u8DataLength );
 *
 * \brief Updates part of the contents of the current file.
 *
 * \par Description:
 *
 * Updates part of the contents of the current file, from the current file
 * position. Write operation is now allowed on read-only files.
 *
 * The current file position is set at the end of the written data.
 *
 * \param pu8Data        [IN]  Data to write.
 * \param u8DataLength   [IN]  Length of data (1..255).
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltWriteFile( const VLT_U8 *pu8Data,
    VLT_U8 u8DataLength );

/**
 * \fn VltReadFile( VLT_PU16 pu16ReadLength,
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
 * of bytes that can be read.
 *
 * Important:
 * If the user�s authentication method is SCP02 or 3, the channel level is CMac
 * or higher. The *pu16ReadLength value will be ignored due to the mechanics of
 * the secure channel.
 * The VaultIC will return 256 bytes or the whole file if the file size is 
 * less than 256 bytes.
 * Ensure the buffer passed to store the data read is no smaller than 256 bytes
 * or a buffer overflow is possible on SCP users.
 *
 * \param pu16ReadLength [IN,OUT] On entry this holds the maximum size of the
 *                                buffer. On exit it is set to the amount of
 *                                buffer received. 256 bytes is the max requested
 *                                size, values larger will generate an error.
 * \param pu8RespData    [IN]     Ptr to a buffer where the data read will be 
 *                                stored.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltReadFile( VLT_PU16 pu8ReadLength,
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

#if (VLT_ENABLE_SECURE_COUNTERS == VLT_ENABLE)
#if (VAULT_IC_TARGET == VAULTIC100)
/**
 * \fn VLT_STS VltSetSecureCounters(VLT_U8 u8CounterNumber, 
 *							 VLT_U8 u8CounterGroup,
 *							 VLT_U8 u8CounterValueLength, 
 *							 VLT_PU8 pu8CounterValue );
 *
 * \brief Set the specified counter value.
 *
 * \par Description:
 *
 * Set the specified counter value.
 *
 * \param u8CounterNumber [IN]	  The Counter Number.
 * \param u8CounterGroup [IN]	  The Counter Group.
 * \param u8CounterValueLen [IN]	  counter value length.
 * \param pu8CounterValue [IN]	  New counter value 
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltSetSecureCounters(VLT_U8 u8CounterNumber, 
							 VLT_U8 u8CounterGroup,
							 VLT_U8 u8CounterValueLen, 
							 VLT_PU8 pu8CounterValue );

/**
 * \fn VltIncrementCounter(VLT_U8 u8CounterNumber, VLT_U8 u8CounterGroup, 
 *			VLT_COUNTER_DATA* pCounterData,
 *		VLT_U16 u16Signaturelen, VLT_PU8 pu8Signature );
 *
 * \brief Increment the specified counter value.
 *
 * \par Description:
 *
 * Increment the specified counter value.
 *
 * \param u8CounterNumber [IN]	  The Counter Number.
 * \param u8CounterGroup [IN]	  The Counter Group.
 * \param pCounterData [IN]	      Contains Nounce and Amount
 * \param u16Signaturelen [IN]	  Signature length
 * \param pu8Signature [IN]		  The ECDSA Signature: SIGNHOST(abNounce || abAmount)
 * \param pCounterResponse [OUT]  Increment counter response data
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltIncrementCounter(VLT_U8 u8CounterNumber, VLT_U8 u8CounterGroup, 
	VLT_COUNTER_DATA* pCounterData,
	VLT_U16 u16Signaturelen, VLT_PU8 pu8Signature,VLT_COUNTER_RESPONSE* pCounterResponse );

/**
 * \fn VltDecrementCounter(VLT_U8 u8CounterNumber, VLT_U8 u8CounterGroup, 
 *		VLT_COUNTER_DATA* pCounterData,VLT_COUNTER_RESPONSE* pCounterResponse);
 *
 * \brief Decrement the specified counter value.
 *
 * \par Description:
 *
 * Decrement the specified counter value.
 *
 * \param u8CounterNumber [IN]	  The Counter Number.
 * \param u8CounterGroup [IN]	  The Counter Group.
 * \param pCounterData [IN]	      Contains Nounce and Amount
 * \param pCounterResponse [OUT]  Decrement counter response data
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltDecrementCounter(VLT_U8 u8CounterNumber, VLT_U8 u8CounterGroup, 
	VLT_COUNTER_DATA* pCounterData,VLT_COUNTER_RESPONSE* pCounterResponse);
#else
 /**
 * \fn VltIncrementCounter(VLT_U8 u8CounterMask, VLT_U8 u8CounterGroup, 
 *			VLT_COUNTER_DATA* pCounterData,
 *		VLT_U16 u16Signaturelen, VLT_PU8 pu8Signature );
 *
 * \brief Increment the specified counter value.
 *
 * \par Description:
 *
 * Increment the specified counter value.
 *
 * \param u8CounterMask [IN]	  The Counter Mask Byte.
 * \param u8CounterGroup [IN]	  The Counter Group.
 * \param pCounterData [IN]	      Contains Nounce and Amount
 * \param u16Signaturelen [IN]	  Signature length
 * \param pu8Signature [IN]		  The ECDSA Signature: SIGNHOST(abNounce || abAmount)
 * \param pCounterResponse [OUT]  Increment counter response data
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
 VLT_STS VltIncrementCounter(VLT_U8 u8CounterMask, VLT_U8 u8CounterGroup, 
		VLT_COUNTER_DATA* pCounterData,
		VLT_U16 u16Signaturelen, VLT_PU8 pu8Signature,VLT_COUNTER_RESPONSE* pCounterResponse );
/**
 * \fn VltDecrementCounter(VLT_U8 u8CounterMask, VLT_U8 u8CounterGroup, 
 *		VLT_COUNTER_DATA* pCounterData,
 *		VLT_U8 u8Signaturelen, VLT_PU8 pu8Signature );
 *
 * \brief Decrement the specified counter value.
 *
 * \par Description:
 *
 * Decrement the specified counter value.
 *
 * \param u8CounterMask [IN]	  The Counter Mask Byte.
 * \param u8CounterGroup [IN]	  The Counter Group.
 * \param pCounterData [IN]	      Contains Nounce and Amount
 * \param pCounterResponse [OUT]  Decrement counter response data
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltDecrementCounter(VLT_U8 u8CounterMask, VLT_U8 u8CounterGroup, 
		VLT_COUNTER_DATA* pCounterData,VLT_COUNTER_RESPONSE* pCounterResponse);

/**
 * \fn VLT_STS VltSetSecureCounters(VLT_U8 u8CounterGroup,
 *							 VLT_U8 u8CounterValueLength, 
 *							 VLT_PU8 pu8CounterValue );
 *
 * \brief Defines the initial values of the secure counters of the VaultIC150X chip. 
 * All counters of a group are set at the same time
 *
 * \par Description:
 *
 * Set the specified group counter values.
 *
 * \param u8CounterNumber [IN]	  The Counter Number.
 * \param u8CounterGroup [IN]	  The Counter Group.
 * \param u8CounterValueLen [IN]  the counter value length.
 * \param pu8CounterValue [IN]	  New counter value 
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltSetSecureCounters(VLT_U8 u8CounterGroup,
							 VLT_U8 u8CounterValueLength, 
							 VLT_PU8 pu8CounterValue);
#endif
#endif

#if (VLT_ENABLE_CHIP_DEACTIVATION == VLT_ENABLE)

/**
 * \fn VltDeactivate(VLT_PU8 pu8KeyId,VLT_U8 u8KeyIdLen,
 *   VLT_PU8 pu8Keydata,
 *   VLT_U8 u8KeyDataLen);
 *
 * \brief Changes the Life Cycle state.
 *
 * \par Description: Changes the Life Cycle state.
 *
 *
 * \param pu8KeyId  [IN]  Challenge data buffer.
 * \param u8KeyIdLength  [IN]  Length of challenge data buffer.
 * \param pu8KeyData [IN]  Key Data buffer.
 * \param u8KeyDataLength [IN]  Length of key data buffer.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltDeactivate(VLT_PU8 pu8KeyId,VLT_U8 u8KeyIdLength,
    VLT_PU8 pu8KeyData,
    VLT_U8 u8KeyDataLength);


/**
 * \fn VltGetChallenge(VLT_PU8 pu8Challenge,VLT_PU8 u8ChallengeLen);
 *
 * \brief The 16-byte challenge.
 *
 * \par Description: This command is part of the re-activation process of theVaultIC160D. During this process the
 * Activate Command used the 16-byte challenge returned by the GetChallenge command to provide
 * a dynamic authentication template.
 *
 *
 * \param pu8Challenge  [IN]  Challenge data buffer.
 * \param u8ChallengeLength  [IN]  Length of challenge data buffer.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltGetChallenge(VLT_PU8 pu8Challenge,VLT_PU8 u8ChallengeLength);

/**
 * \fn VltActivate(VLT_PU8 pu8Challenge,VLT_U8 u8ChallengeLen,
 *   VLT_PU8 pu8Keydata,
 *   VLT_U8 u8KeyDataLen);
 *
 * \brief Reactivate chip.
 *
 * \par Description: Changes the Life Cycle state.
 *
 *
 * \param pu8Challenge  [IN]  Challenge data buffer.
 * \param u8ChallengeLen  [IN]  Length of challenge data buffer.
 * \param pu8Keydata [IN]  Key Data buffer.
 * \param u8KeyDataLen [IN]  Length of key data buffer.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltActivate(VLT_PU8 pu8Challenge,VLT_U8 u8ChallengeLen,
    VLT_PU8 pu8Keydata,
    VLT_U8 u8KeyDataLen);

#endif

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
VLT_STS VltGetLibInfo( VLT_LIBRARY_INFO* pLibraryInfo );

#if (VLT_ENABLE_ISO7816 == VLT_ENABLE)
/**
 * \fn VLT_STS VltCardEvent(VLT_PU8 pu8ReaderName, DWORD dwTimeout,PDWORD pdwEventState);
 *
 * \brief Watch events on specified reader.
 *
 * \par Description:
 *
 * \param pu8ReaderName         [IN]  Number of input bytes (1..255).
 * \param dwTimeout             [IN]  Input bytes.
 * \param pdwEventState			[IN]  Number of output bytes (1..255).
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltCardEvent(VLT_PU8 pu8ReaderName, DWORD dwTimeout,PDWORD pdwEventState);
/**
 * \fn VLT_STS VltSelectCard( SCARDHANDLE hScard, SCARDCONTEXT hCxt, DWORD dwProtocol);
 *
 * \brief Select a card.
 *
 * \par Description:
 *
 * \param hScard         [IN]  Number of input bytes (1..255).
 * \param hCxt           [IN]  Input bytes.
 * \param dwProtocol	 [IN]  Number of output bytes (1..255).
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltSelectCard( SCARDHANDLE hScard, SCARDCONTEXT hCxt, DWORD dwProtocol);
#endif

/*@}*/
#endif /*VAULTIC_API_H*/

