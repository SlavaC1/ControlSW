/* --------------------------------------------------------------------------
* VaultIC
* Copyright (C) Atmel Corporation, 2010. All Rights Reserved.
* -------------------------------------------------------------------------- */

#ifndef __VAULTICAPI_H__
#define __VAULTICAPI_H__


#include "vaultic_common.h"
#include "vaultic_lib.h"

#ifndef WIN32
#include "PCSC/wintypes.h"
#endif

#ifdef __cplusplus

extern "C" 
{
	/* --------------------------------------------------------------------------
	* LIBRARY MANAGEMENT FUNCTIONS
	* -------------------------------------------------------------------------- 
	*/
	VLT_STS VltApiInit(VLT_INIT_COMMS_PARAMS* pInitCommsParams);

	VLT_STS VltApiClose( void );

	/* --------------------------------------------------------------------------
	* IDENTITY AUTHENTICATION COMMANDS
	* -------------------------------------------------------------------------- */
	VLT_STS VltSubmitPassword( VLT_U8 u8PasswordLength,
		const VLT_U8 *pu8Password );

	VLT_STS VltManageAuthenticationData( const VLT_MANAGE_AUTH_DATA *pAuthSetup );

	VLT_STS VltGetAuthenticationInfo( VLT_U8 u8UserID, 
		VLT_AUTH_INFO *pRespData );

	VLT_STS VltCancelAuthentication( void );

	VLT_STS VltInternalAuthenticate(VLT_U8 u8UserID, 
		VLT_U8 u8RoleID,
		VLT_U8 u8ChallengeLengths,
		const VLT_U8 *pu8HostChallenge,
		VLT_PU8 pu8DeviceChallenge,
		VLT_PU16 pu16SignatureLength,
		VLT_PU8 pu8Signature  );

	VLT_STS VltExternalAuthenticate( VLT_U8 u8UserID,
		VLT_U8 u8RoleID,
		VLT_U8 u8HostChallengeLength,
		const VLT_U8 *pu8HostChallenge,
		VLT_U16 u16HostSignatureLength,
		const VLT_U8 *pu8HostSignature );

	/* --------------------------------------------------------------------------
	* CRYPTO SERVICES
	* -------------------------------------------------------------------------- */
	VLT_STS VltInitializeAlgorithm( VLT_U8 u8KeyGroup,
		VLT_U8 u8KeyIndex,
		VLT_U8 u8Mode,
		const VLT_ALGO_PARAMS *pAlgorithm );

	VLT_STS VltPutKey( VLT_U8 u8KeyGroup,
		VLT_U8 u8KeyIndex,
		const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
		const VLT_KEY_OBJECT *pKeyObj );

	VLT_STS VltReadKey( VLT_U8 u8KeyGroup,
		VLT_U8 u8KeyIndex,
		VLT_KEY_OBJECT *pKeyObj );

	VLT_STS VltGenerateAssuranceMessage(VLT_PU8 pu8SignerIdLength,
		VLT_PU8 pu8SignerID,
		VLT_ASSURANCE_MESSAGE* pAssuranceMsg );

	VLT_STS VltVerifyAssuranceMessage(VLT_U8 u8SignedAssuranceMsgLength,
		VLT_PU8 pu8SignedAssuranceMsg);

	VLT_STS VltGenerateSignature( VLT_U32 u32MessageLength,
		const VLT_U8 *pu8Message,
		VLT_PU16 pu16SignatureLength,
		VLT_PU8 pu8Signature );

	VLT_STS VltVerifySignature( VLT_U32 u32MessageLength,
		const VLT_U8 *pu8Message,
		VLT_U16 u16SignatureLength,
		const VLT_U8 *pu8Signature );

	VLT_STS VltUpdateSignature(VLT_U32 u32MessagePartLength,
						   const VLT_U8 *pu8MessagePart);

	VLT_STS VltComputeSignatureFinal( VLT_PU16 pu16SignatureLength,
	    VLT_PU8 pu8Signature );

	VLT_STS VltUpdateVerify(VLT_U32 u32MessageLength,
						   const VLT_U8 *pu8Message);

	VLT_STS VltComputeVerifyFinal(VLT_U16 u32SignatureLength,
						   const VLT_U8 *pu8Signature);

	VLT_STS VltComputeMessageDigest( VLT_U32 u32MessageLength,
		const VLT_U8 *pu8Message,
		VLT_PU8 pu8DigestLength,
		VLT_PU8 pu8Digest );

	VLT_STS VltUpdateMessageDigest( VLT_U32 u32MessageLength,
		const VLT_U8 *pu8Message);

	VLT_STS VltComputeMessageDigestFinal( VLT_PU8 pu8DigestLength,
		VLT_PU8 pu8Digest );

	VLT_STS VltGenerateRandom( VLT_U8 u8NumberOfCharacters, 
		VLT_PU8 pu8RandomCharacters );

	VLT_STS VltGenerateKeyPair(VLT_U8 u8PublicKeyGroup,
		VLT_U8 u8PublicKeyIndex,
		VLT_U8 u8PrivateKeyGroup,
		VLT_U8 u8PrivateKeyIndex,
		const VLT_KEY_GEN_DATA *pKeyGenData );


	VLT_STS VltSelectFile(const VLT_U8 *pu8Path, 
		VLT_U8 u8PathLength, 
		VLT_SELECT *pRespData );

	VLT_STS VltWriteFile( const VLT_U8 *pu8Data,
		VLT_U8 u8DataLength );

	VLT_STS VltReadFile( VLT_PU16 pu8ReadLength,
		VLT_PU8 pu8RespData );

	VLT_STS VltSeekFile( VLT_U32 u32SeekLength );

	VLT_STS VltGetInfo( VLT_TARGET_INFO *pRespData );

	VLT_STS VltSelfTest( void );

	VLT_STS VltSetStatus( VLT_U8 u8State );

	VLT_STS VltSetConfig(VLT_U8 u8ConfigItem,
		VLT_U8 u8DataLength,
		VLT_PU8 pu8ConfigData );

#if (VLT_ENABLE_SECURE_COUNTERS == VLT_ENABLE)
#if (VAULT_IC_TARGET == VAULTIC100)
	VLT_STS VltIncrementCounter(VLT_U8 u8CounterNumber, VLT_U8 u8CounterGroup, 
		VLT_COUNTER_DATA* pCounterData,
		VLT_U16 u16Signaturelen, VLT_PU8 pu8Signature,VLT_COUNTER_RESPONSE* pCounterResponse );

	VLT_STS VltDecrementCounter(VLT_U8 u8CounterNumber, VLT_U8 u8CounterGroup, 
		VLT_COUNTER_DATA* pCounterData,VLT_COUNTER_RESPONSE* pCounterResponse);

	VLT_STS VltSetSecureCounters(VLT_U8 u8CounterNumber, 
		VLT_U8 u8CounterGroup,
		VLT_U8 u8CounterValueLength, 
		VLT_PU8 pu8CounterValue );
#else
	VLT_STS VltIncrementCounter(VLT_U8 u8CounterMask, VLT_U8 u8CounterGroup, 
		VLT_COUNTER_DATA* pCounterData,
		VLT_U16 u16Signaturelen, VLT_PU8 pu8Signature,VLT_COUNTER_RESPONSE* pCounterResponse );

	VLT_STS VltDecrementCounter(VLT_U8 u8CounterMask, VLT_U8 u8CounterGroup, 
		VLT_COUNTER_DATA* pCounterData,VLT_COUNTER_RESPONSE* pCounterResponse);

	VLT_STS VltSetSecureCounters(VLT_U8 u8CounterGroup,
		VLT_U8 u8CounterValueLength, 
		VLT_PU8 pu8CounterValue);
#endif
#endif

#if (VLT_ENABLE_CHIP_DEACTIVATION == VLT_ENABLE)	
	VLT_STS VltDeactivate(VLT_PU8 pu8KeyId,VLT_U8 u8KeyIdLen,
		VLT_PU8 pu8Keydata,
		VLT_U8 u8KeyDataLen);

	VLT_STS VltGetChallenge(VLT_PU8 pu8Challenge,VLT_PU8 pu8ChallengeLen);

	VLT_STS VltActivate(VLT_PU8 pu8Challenge,VLT_U8 u8ChallengeLen,
		VLT_PU8 pu8Keydata,
		VLT_U8 u8KeyDataLen);
#endif

	VLT_STS VltTestCase1( void );

	VLT_STS VltTestCase2( VLT_U8 u8RequestedDataLength, 
		VLT_PU8 pu8RespData );

	VLT_STS VltTestCase3(VLT_U8 u8DataLength,
		const VLT_U8 *pu8Data );

	VLT_STS VltTestCase4(VLT_U8 u8DataLength,
		const VLT_U8 *pu8Data, 
		VLT_U8 u8RequestedDataLength,
		VLT_PU8 pu8RespData );

	VLT_STS VltGetLibInfo( VLT_LIBRARY_INFO* pLibraryInfo );

	VLT_STS EcdsaSignerInit(
		const VLT_ECDSA_DOMAIN_PARAMS* pDomainParams,
		const VLT_ECDSA_PRIVATE_KEY* pPrivateKey,
		const VLT_ECDSA_PUBLIC_KEY* pPublicKey,
		VLT_U8 u8OpMode);

	VLT_STS EcdsaSignerClose(void);

	VLT_STS EcdsaSignerDoFinal(
		VLT_PU8 pu8Message,
		VLT_U32 u32messageLen,
		VLT_U32 u32messageCapacity,
		VLT_PU8 pu8Signature,
		VLT_PU32 pu32SignatureLen,
		VLT_U32 u32SignatureCapacity);

	VLT_STS EcdsaSignerUpdate(
		VLT_PU8 pu8Message,
		VLT_U32 u32MessageLen,
		VLT_U32 u32MessageCapacity);

#if(VLT_ENABLE_ISO7816 == VLT_ENABLE )        
    VLT_STS VltCardEvent(VLT_PU8 readerName, PDWORD cardState);

	VLT_STS VltSelectCard( SCARDHANDLE hScard, SCARDCONTEXT hCxt, DWORD dwProtocol);
#endif
}
#endif

#endif //__VAULTICAPI_H__