#include "stdafx.h"
#include "VaultIC100Adapter.h"
#include "IDTHWInf.h"
#include "vaultic_apdu.h"
#include "Utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

using namespace std;
using namespace RSCommon;

namespace StratasysIDTSrv
{

#define COUNTER_NUMBER		1
#define COUNTER_GROUP		1
#define CERTIFICATE_PATH	"/cert/.cer"
#define DEVICE_MANUFACTURER_PASSWORD  "C908010C049D31876FEFE6DBF746CE2DB041327492B1BD724B7889D80C0E5995"

	VaultIC100Adapter::VaultIC100Adapter()
		: m_theBaseApi(NULL), m_cartridgeInfo(new VLT_TARGET_INFO()), m_bUseUsb(false)
	{

	}

	VaultIC100Adapter::~VaultIC100Adapter()
	{
		Cleanup();
	}

	void VaultIC100Adapter::Cleanup()
	{
		if (m_theBaseApi != NULL)
		{
			m_bIsInitialized = false;
		}

		delete m_cartridgeInfo;
		m_theBaseApi = NULL;
	}

	int VaultIC100Adapter::InitHW(unsigned char *pucCartridgesCount)
	{
		LOG_EXT(LEVEL_DEBUG, "Initializing hardware...");
		int returnValue;
		int status = 0;
		unsigned int uiDeviceSerialNumber = 0;
		int cartridgesCount;

#ifdef ENABLE_USB
		returnValue = ParseDeviceSerialNumber(&uiDeviceSerialNumber);
		m_bUseUsb = ((returnValue == VLT_OK) && (uiDeviceSerialNumber != 0));
#endif

		if (m_bUseUsb)
		{		
			cartridgesCount = 1;
			status = 1;
		}
		else
		{
			IdtClose();
			returnValue = IdtInit(&cartridgesCount);
			if (returnValue == FALSE)
			{
				return FCB_INIT_FAILED;
			}

			returnValue = GetInPlaceStatus(&status);
			if (returnValue != IDTLIB_SUCCESS)
			{
				return returnValue;
			}
		}

		m_ucCartridgesCount = *pucCartridgesCount = (unsigned char)cartridgesCount;
		if (status != 0)
		{
			unsigned char cartridgeNum = 0;
			while (cartridgeNum < m_ucCartridgesCount)
			{
				if ((status & (1 << cartridgeNum)) != 0)
				{
					break;
				}

				cartridgeNum++;
			}

			if (cartridgeNum < m_ucCartridgesCount)
			{
				returnValue = InitLibrary(cartridgeNum, uiDeviceSerialNumber);
			}
		}

		return returnValue;
	}

	/// <summary>
	/// Initializes VaultIC100 library.
	/// </summary>
	/// <returns>0 on success, non-zero for failures.</returns>
	int VaultIC100Adapter::InitLibrary(unsigned char ucCartridgeNum, unsigned int uiDeviceSerialNumber)
	{
		LOG_EXT(LEVEL_DEBUG, "Initializing VaultIC100 library...");
		int returnValue;

		// Get VaultIC100 definitions
		VLT_INIT_COMMS_PARAMS commParams;
		commParams.u8CommsProtocol = VLT_TWI_COMMS;
		commParams.Params.VltBlockProtocolParams.VltPeripheralParams.PeriphParams.VltTwiParams.u16BusTimeout = 250;
		commParams.Params.VltBlockProtocolParams.VltPeripheralParams.PeriphParams.VltTwiParams.u32msTimeout = 4000;
		commParams.Params.VltBlockProtocolParams.VltPeripheralParams.PeriphParams.VltTwiParams.u8Address = 0x5F;
		commParams.Params.VltBlockProtocolParams.u16BitRate = 100;
		commParams.Params.VltBlockProtocolParams.u8CheckSumMode = BLK_PTCL_CHECKSUM_SUM8;
		commParams.Params.VltBlockProtocolParams.u32AfterHdrDelay = 1000;
		commParams.Params.VltBlockProtocolParams.u32InterBlkDelay = 1000;
		commParams.Params.VltBlockProtocolParams.u32InterHdrByteDelay = 1000;
		commParams.Params.VltBlockProtocolParams.u16msSelfTestDelay = 2500;
		commParams.Params.VltBlockProtocolParams.VltPeripheralParams.u32SerialNo = uiDeviceSerialNumber;

		IdtSelectChannel(ucCartridgeNum);
		returnValue = VltInitLibrary(&commParams);

		if (returnValue != VLT_OK)
		{
			return returnValue;
		}

		m_theBaseApi = VltGetApi();
		m_bIsInitialized = true;

		return IDTLIB_SUCCESS;
	}

#ifdef ENABLE_USB
	int VaultIC100Adapter::ParseDeviceSerialNumber(unsigned int *puiDeviceSerialNumber)
	{
		VLT_STS status = VLT_OK;

		//
		// Determine the size of the xml string that will be returned on the second call,
		// set the stringSize parameter to zero and the string to NULL. On return the
		// stringSize will be set to the actual number of characters of the XML string
		// that contains the list of devices.
		VLT_U32 stringSize = 0;
		if( VLT_OK != ( status = VltFindDevices( &stringSize, NULL ) ) )
		{
			return status;
		}

		//
		// Allocate enough memory to hold the xml string with the list of
		// devices.
		VLT_PU8 pXmlString = (VLT_PU8)NULL;
		if( NULL == ( pXmlString = (VLT_PU8)malloc(stringSize ) ) )
		{
			status = VLT_FIND_DEVS_MALLOC_FAIL;
			return status;
		}

		//
		// Now call the VltFindDevices again with all the appropriate parameters
		// On return the pXmlString will be filled with the device list.
		if( VLT_OK != ( status = VltFindDevices( &stringSize, pXmlString ) ) )
		{
			free(pXmlString);
			return status;
		}

		//
		// Assign the XML string to a standard string
		string strXMLDevs =(const char*)pXmlString;
		free(pXmlString);

		status = ParseDeviceSerialNumber(strXMLDevs, puiDeviceSerialNumber);
		return status;
	}

	int VaultIC100Adapter::ParseDeviceSerialNumber(string strXmlDevs, unsigned int *puiDeviceSerialNumber)
	{
		if (VLT_ENABLE_AARDVARK == VLT_DISABLE)
		{
			*puiDeviceSerialNumber = 0;
			return VLT_OK;
		}

		VLT_STS status = VLT_FAIL;

		// Check if there are any PC/SC readers connected
		bool readerFound = false;
		string strReaderName("");

		const char *pXmlData = "<peripheral idx=\"00\">";
		const char *interfaceNodeName = "<interface type=\"aardvark\">";

		size_t startPos, endPos;
		startPos = strXmlDevs.find(interfaceNodeName);
		if( string::npos != startPos )
		{
			endPos = strXmlDevs.find("</interface>", startPos);
			if( string::npos == endPos)
			{
				status = VLT_MALFORMED_XML_AARDVARK_INTFC;
				return status;
			}

			string strDevice( strXmlDevs.substr( startPos, ( endPos - startPos ) ) );

			// Reset start pointer & item number
			startPos = 0;

			do
			{
				// It does so check for entries for a reader
				startPos = strDevice.find("<peripheral idx=", startPos);
				if (string::npos == startPos)
				{
					// No more reader in the list = end of loop
					break;
				}
				else
				{
					// Start of the reader name is after the opening tag,
					// end position is at beginning of the closing tag
					startPos += strlen(pXmlData);
					endPos = strDevice.find("</peripheral>", startPos);
					if (string::npos == endPos)
					{
						break;
					}

					// Get the device serial number
					strReaderName = strDevice.substr(startPos, (endPos - startPos));
					readerFound = true;
				}
			} while (true);
		}

		if (!readerFound)
		{
			//status = VLT_NO_READER_CONNECTED;
			//return status;
			*puiDeviceSerialNumber = 0;
			return VLT_OK;
		}

		startPos = strXmlDevs.find(interfaceNodeName);
		startPos = strXmlDevs.find(pXmlData, startPos);
		startPos += strlen(pXmlData);
		endPos = strXmlDevs.find("</peripheral>", startPos);

		// Get the reader name
		strReaderName = strXmlDevs.substr(startPos, (endPos - startPos));
		*puiDeviceSerialNumber = strtoul(strReaderName.c_str(), NULL, 10);
		status = VLT_OK;

		return status;
	}
#endif


#if (ENABLE_BURNING == 1)

	int VaultIC100Adapter::Login()
	{
		if (m_theBaseApi == NULL)
		{
			return HW_NOT_INITIALIZED;
		}

		int returnValue;

		const char* textualPassword = DEVICE_MANUFACTURER_PASSWORD;
		unsigned char passwordLength = (unsigned char)strlen(textualPassword);
		unsigned char *password = new unsigned char[passwordLength / 2];
		HexStringToByteArray(textualPassword, passwordLength, password);

		LOG_EXT(LEVEL_DEBUG, "Logging in as manufacturer...");
		returnValue = m_theBaseApi->VltSubmitPassword(passwordLength / 2, password);
		delete[] password;
		if (returnValue != VLT_OK)
		{
			if (returnValue = 0x6300)
			{
				return IDT_LOCKED;
			}

			return LOGIN_FAILED;
		}

		LOG_EXT(LEVEL_DEBUG, "Setting creation mode...");
		returnValue = m_theBaseApi->VltSetStatus(VLT_CREATION);
		if (returnValue != VLT_OK)
		{
			if (returnValue == 0x6985)
			{
				return ACCESS_DENIED;
			}

			return TRANSITION_FAILED;
		}

		return IDTLIB_SUCCESS;
	}

	int VaultIC100Adapter::Configure()
	{
		if (m_theBaseApi == NULL)
		{
			return HW_NOT_INITIALIZED;
		}

		int returnValue;

		LOG_EXT(LEVEL_DEBUG, "Setting K-283 curve...");
		unsigned char ucCurve = VLT_ECDSA_CURVE_K283;
		returnValue = m_theBaseApi->VltSetConfig(VLT_ECDSA_DOMAIN_PARAM_CHOICE, VLT_ECDSA_PARAMS_CHOICE_SZ, &ucCurve);
		if (returnValue != VLT_OK)
		{
			return CONFIGURATION_FAILED;
		}

		LOG_EXT(LEVEL_DEBUG, "Setting non-FIPS mode...");
		unsigned char ucFipsMode = 0;
		//unsigned char ucFipsMode = 1;
		returnValue = m_theBaseApi->VltSetConfig(VLT_FIPS_MODE, VLT_FIPS_MODE_SZ, &ucFipsMode);
		if (returnValue != VLT_OK)
		{
			return CONFIGURATION_FAILED;
		}

		LOG_EXT(LEVEL_DEBUG, "Enforce I2C communication...");
		unsigned char ucCommunicationChannel = 3; //always use I2C
		returnValue = m_theBaseApi->VltSetConfig(VLT_COMMS_CHANNEL_ACCESS, VLT_COMMS_CHANNEL_SZ, &ucCommunicationChannel);
		if (returnValue != VLT_OK)
		{
			return CONFIGURATION_FAILED;
		}

		return IDTLIB_SUCCESS;
	}

	int VaultIC100Adapter::SetKeys(const unsigned char *aucPubKS)
	{
		if (m_theBaseApi == NULL)
		{
			return HW_NOT_INITIALIZED;
		}

		int returnValue;

		LOG_EXT(LEVEL_DEBUG, "Writing host ID...");
		VLT_FILE_PRIVILEGES KeyPrivileges = { 0x03, 0x00, 0x0, 0x2 };
		VLT_KEY_OBJECT KeyObj;
		KeyObj.u8KeyID = VLT_KEY_HOST_DEVICE_ID;
		KeyObj.data.HostDeviceIdKey.u16StringLen = VLT_HOST_DEV_ID_STRING_LENGTH;
		KeyObj.data.HostDeviceIdKey.pu8StringId = aucHostID;
		returnValue = m_theBaseApi->VltPutKey((VLT_U8)enHostKeyGroup, (VLT_U8)enHostDeviceIDKeyIndex, &KeyPrivileges, &KeyObj);
		if (returnValue != VLT_OK)
		{
			return KEY_GENERATION_FAILED;
		}

		LOG_EXT(LEVEL_DEBUG, "Writing device ID...");
		unsigned char aucDeviceID[VLT_HOST_DEV_ID_STRING_LENGTH];
		unsigned char ucDeviceIDLength;
		GetSerialNumber(aucDeviceID, &ucDeviceIDLength);
		memset(aucDeviceID + ucDeviceIDLength, '.', VLT_HOST_DEV_ID_STRING_LENGTH - ucDeviceIDLength);
		KeyObj.data.HostDeviceIdKey.pu8StringId = aucDeviceID;
		returnValue = m_theBaseApi->VltPutKey((VLT_U8)enDeviceKeyGroup, (VLT_U8)enHostDeviceIDKeyIndex, &KeyPrivileges, &KeyObj);
		if (returnValue != VLT_OK)
		{
			return KEY_GENERATION_FAILED;
		}

		LOG_EXT(LEVEL_DEBUG, "Writing host public key...");
		ECDSA<EC2N, SHA256>::PublicKey pubKS;
		pubKS.Load(StringSource(aucPubKS, PUBLIC_KEY_SIZE, true).Ref());
		EC2NPoint Q = pubKS.GetPublicElement();
		VLT_U8 Qx[KEY_COEFF_SIZE];
		VLT_U8 Qy[KEY_COEFF_SIZE];
		Q.x.Encode(Qx, sizeof(Qx));
		Q.y.Encode(Qy, sizeof(Qy));

		KeyObj.u8KeyID = VLT_KEY_ECDSA_GF2M_PUB;
		KeyObj.data.EcdsaPubKey.u16QLen = KEY_COEFF_SIZE;
		KeyObj.data.EcdsaPubKey.pu8Qx = Qx;
		KeyObj.data.EcdsaPubKey.pu8Qy = Qy;
		KeyObj.data.EcdsaPubKey.u8DomainParamsGroup = (VLT_U8)enDeviceKeyGroup;
		KeyObj.data.EcdsaPubKey.u8DomainParamsIndex = (VLT_U8)enDomainParamsKeyIndex;
		KeyObj.data.EcdsaPubKey.u8Assurance = 0;
		returnValue = m_theBaseApi->VltPutKey((VLT_U8)enHostKeyGroup, (VLT_U8)enPublicKeyIndex, &KeyPrivileges, &KeyObj);
		if (returnValue != VLT_OK)
		{
			return KEY_GENERATION_FAILED;
		}

		LOG_EXT(LEVEL_DEBUG, "Generating device private and public key pair...");
		VLT_KEY_GEN_DATA keyGeneratorData;
		keyGeneratorData.u8AlgoID = VLT_ALG_KPG_ECDSA_GF2M;
		keyGeneratorData.EcdsaKeyGenObj.u8DomainParamsGroup = enDeviceKeyGroup;
		keyGeneratorData.EcdsaKeyGenObj.u8DomainParamsIndex = enDomainParamsKeyIndex;
		returnValue = m_theBaseApi->VltGenerateKeyPair((VLT_U8)enDeviceKeyGroup, (VLT_U8)enPublicKeyIndex, (VLT_U8)enDeviceKeyGroup, (VLT_U8)enPrivateKeyIndex, &keyGeneratorData);
		if (returnValue != VLT_OK)
		{
			return KEY_GENERATION_FAILED;
		}

		return IDTLIB_SUCCESS;
	}

	int VaultIC100Adapter::SetCounter(unsigned int uiCouterValue)
	{
		if (m_theBaseApi == NULL)
		{
			return HW_NOT_INITIALIZED;
		}

		int returnValue;

		LOG_EXT(LEVEL_DEBUG, "Reset counter value to " << uiCouterValue << "...");
		unsigned char ucCounterMode = VLT_DIRECT_MODE;
		returnValue = m_theBaseApi->VltSetConfig( VLT_COUNTER_MODE_PARAM, VLT_COUNTER_MODE_SZ, &ucCounterMode);
		if (returnValue != VLT_OK)
		{
			return COUNTER_SET_FAILED;
		}

		returnValue = m_theBaseApi->VltSetSecureCounters(COUNTER_NUMBER, COUNTER_GROUP, sizeof(uiCouterValue), (VLT_PU8)(&uiCouterValue));
		if (returnValue != VLT_OK)
		{
			return COUNTER_SET_FAILED;
		}

		ucCounterMode = VLT_COUNTER_MODE;
		returnValue = m_theBaseApi->VltSetConfig( VLT_COUNTER_MODE_PARAM, VLT_COUNTER_MODE_SZ, &ucCounterMode);
		if (returnValue != VLT_OK)
		{
			return COUNTER_SET_FAILED;
		}

		return IDTLIB_SUCCESS;
	}

	int VaultIC100Adapter::GenerateCertificate(const unsigned char *aucMaterialInfo, unsigned short usMaterialInfoLength, const unsigned char *aucPrvKS, unsigned char *aucCertificate, unsigned short *usCertificateLength)
	{
		if (m_theBaseApi == NULL)
		{
			return HW_NOT_INITIALIZED;
		}

		int returnValue;

		LOG_EXT(LEVEL_DEBUG, "Reading device public key...");
		unsigned char aucQx[KEY_COEFF_SIZE];
		unsigned char aucQy[KEY_COEFF_SIZE];
		VLT_KEY_OBJECT read_key;
		read_key.u8KeyID = VLT_KEY_ECDSA_GF2M_PUB;
		read_key.data.EcdsaPubKey.u16QLen = KEY_COEFF_SIZE;
		read_key.data.EcdsaPubKey.pu8Qx = aucQx;
		read_key.data.EcdsaPubKey.pu8Qy = aucQy;
		returnValue = m_theBaseApi->VltReadKey((VLT_U8)enDeviceKeyGroup, (VLT_U8)enPublicKeyIndex,&read_key);
		if (returnValue != VLT_OK)
		{
			return KEY_GENERATION_FAILED;
		}

		LOG_EXT(LEVEL_DEBUG, "Generating certificate file...");
		try
		{
			IDTagAdapter::GenerateCertificate(aucMaterialInfo, usMaterialInfoLength, read_key.data.EcdsaPubKey.pu8Qx, read_key.data.EcdsaPubKey.pu8Qy, aucPrvKS, aucCertificate, usCertificateLength);
		}
		catch (exception& e)
		{
			LOG_EXT(LEVEL_ERROR, "Exception caught: " << e.what() << ".");
			return INVALID_HOST_KEY;
		}

		return IDTLIB_SUCCESS;
	}

	int VaultIC100Adapter::WriteCertificate(unsigned char *aucCertificate, unsigned short usCertificateLength)
	{
		if (m_theBaseApi == NULL)
		{
			return HW_NOT_INITIALIZED;
		}

		int returnValue;

		LOG_EXT(LEVEL_DEBUG, "Writing certificate file...");
		VLT_SELECT respData;

		returnValue = m_theBaseApi->VltSelectFile((const VLT_U8*)CERTIFICATE_PATH, (VLT_U8)strlen(CERTIFICATE_PATH), &respData);
		if (returnValue != VLT_OK)
		{
			return CERTIFICATE_WRITE_FAILED;
		}

		unsigned short offset = 0;
		while (offset < usCertificateLength)
		{
			VLT_U8 dataLength = (VLT_U8)min((VLT_U16)(usCertificateLength - offset), (VLT_U16)VLT_MAX_APDU_SND_DATA_SZ);
			returnValue = m_theBaseApi->VltWriteFile(aucCertificate + offset, dataLength);
			if (returnValue != VLT_OK)
			{
				if (returnValue == VLT_EOF ||
					returnValue == 0x9001)
				{
					// reached end of file
					break;
				}

				return CERTIFICATE_WRITE_FAILED;
			}

			offset += dataLength;
		}

		m_theBaseApi->VltSeekFile(0);
		return IDTLIB_SUCCESS;
	}

	int VaultIC100Adapter::Logout()
	{
		if (m_theBaseApi == NULL)
		{
			return HW_NOT_INITIALIZED;
		}

		int returnValue;

		LOG_EXT(LEVEL_DEBUG, "Setting operational mode...");
		returnValue = m_theBaseApi->VltSetStatus(VLT_OPERATIONAL_ACTIVE);
		if (returnValue != VLT_OK)
		{
			return TRANSITION_FAILED;
		}

		LOG_EXT(LEVEL_DEBUG, "Logging out as manufacturer...");
		returnValue = m_theBaseApi->VltCancelAuthentication();
		if (returnValue != VLT_OK)
		{
			return LOGIN_FAILED;
		}

		return IDTLIB_SUCCESS;
	}

#endif

	/// <summary>
	/// Reads certificate file.
	/// </summary>
	/// <param name="ucCartridgeNum">The cartridge number.</param>
	/// <param name="aucCertificate">The certificate (output parameter).</param>
	/// <returns>0 on success, non-zero for failures.</returns>
	int VaultIC100Adapter::ReadIDCertificate(unsigned char *aucCertificate)
	{
		if (m_theBaseApi == NULL)
		{
			return HW_NOT_INITIALIZED;
		}

		VLT_SELECT respData;

		LOG_EXT(LEVEL_DEBUG, "Reading certificate file...");
		int returnValue = m_theBaseApi->VltSelectFile((const VLT_U8*)CERTIFICATE_PATH, (VLT_U8)strlen(CERTIFICATE_PATH), &respData);
		if (returnValue != VLT_OK)
		{
			return returnValue;
		}

		unsigned short offset = 0;
		while (offset < CERTIFICATE_SIZE)
		{
			VLT_U16 readLength = min(VLT_MAX_APDU_RCV_DATA_SZ, (VLT_U16)(CERTIFICATE_SIZE - offset));
			returnValue = m_theBaseApi->VltReadFile(&readLength, (VLT_PU8)aucCertificate + offset);
			if (returnValue != VLT_OK)
			{
				if (returnValue == VLT_EOF ||
					returnValue == 0x9001)
				{
					// reached end of file
					break;
				}

				return returnValue;
			}

			offset += readLength;
		}

		return IDTLIB_SUCCESS;
	}

	bool VaultIC100Adapter::IsCertificateEmpty(const unsigned char *aucCertificate)
	{
		if (aucCertificate == NULL)
		{
			return true;
		}

		int i;
		unsigned char emptyFileIndicator = aucCertificate[0];
		for (i = 0; i < CERTIFICATE_SIZE - 12; i++)
		{
			if (aucCertificate[i] != emptyFileIndicator)
			{
				return false;
			}
		}

		for (; i< CERTIFICATE_SIZE; i++)
		{
			if (aucCertificate[i] != 0)
			{
				return false;
			}
		}

		return true;
	}

	/// <summary>
	/// Sends challenge to be signed by device.
	/// </summary>
	/// <param name="ucCartridgeNum">The cartridge number.</param>
	/// <returns>0 on success, non-zero for failures.</returns>
	int VaultIC100Adapter::ChallengeHostDevice(unsigned char ucCartridgeNum, const unsigned char *aucHostChallenge, unsigned short usHostChallengeLength, unsigned char *aucDeviceChallenge, unsigned short *pusDeviceChallengeLength, unsigned char *aucSignedDeviceChallenge, unsigned short *pusSignedDeviceChallengeLength)
	{
		if (m_theBaseApi == NULL)
		{
			return HW_NOT_INITIALIZED;
		}

		LOG_EXT(LEVEL_DEBUG, "Perform internal authentication...");
		int returnValue = m_theBaseApi->VltInternalAuthenticate(VLT_USER, VLT_APPROVED_USER, (VLT_U8)usHostChallengeLength, aucHostChallenge, aucDeviceChallenge, pusSignedDeviceChallengeLength, aucSignedDeviceChallenge);
		if (returnValue != VLT_OK)
		{
			return returnValue;
		}

		unsigned short offset = VLT_INTERNAL_AUTH_DEV_CHALLENGE_LENGTH;
		memcpy(aucDeviceChallenge + offset, aucHostChallenge, usHostChallengeLength);
		offset += usHostChallengeLength;
		memcpy(aucDeviceChallenge + offset, aucHostID, sizeof(aucHostID));
		offset += sizeof(aucHostID);
		*pusDeviceChallengeLength = offset;

		return IDTLIB_SUCCESS;
	}

	/// <summary>
	/// Gets the current volume of a cartridge.
	/// </summary>
	/// <param name="ucCartridgeNum">The cartridge number.</param>
	/// <param name="puiCurrentVolume">The cartridge current volume (output parameter).</param>
	/// <returns>0 on success, non-zero for failures.</returns>
	int VaultIC100Adapter::GetCurrentVolume(unsigned char ucCartridgeNum, unsigned int *puiCurrentVolume)
	{
		CounterResponse signedVolume;
		int returnValue = DecreaseConsumption(ucCartridgeNum, 0, &signedVolume);
		*puiCurrentVolume = signedVolume.GetCounterValue();
		return returnValue;
	}

	/// <summary>
	/// Decreases consumption from cartridge current volume.
	/// </summary>
	/// <param name="ucCartridgeNum">The cartridge number.</param>
	/// <param name="uiConsumption">The volume consumption.</param>
	/// <param name="newSignedVolume">The cartridge new volume and its signature by PrvKC (output parameter).</param>
	/// <returns>0 on success, non-zero for failures.</returns>
	int VaultIC100Adapter::DecreaseConsumption(unsigned char ucCartridgeNum, unsigned int uiConsumption, CounterResponse *newSignedVolume)
	{
		if (m_theBaseApi == NULL)
		{
			return HW_NOT_INITIALIZED;
		}

		LOG_EXT(LEVEL_DEBUG, "Generate random bytes for nounce...");

		VLT_COUNTER_DATA counterData;
		int returnValue = m_theBaseApi->VltGenerateRandom(VLT_NOUNCE_LENGTH, counterData.nounce.u8Nounce);
		if (returnValue != VLT_OK)
		{
			return GENERATE_RANDOM_FAILED;
		}

		LOG_EXT(LEVEL_DEBUG, "Decrementing counter by " << uiConsumption << "...");
		counterData.amount.u8AmountLength = VLT_COUNTER_LENGTH;
		memcpy(counterData.amount.u8Amount, &uiConsumption, sizeof(unsigned int));
		counterData.nounce.u8NounceLength = VLT_NOUNCE_LENGTH;

		unsigned char signature[SIGNATURE_SIZE];
		VLT_COUNTER_RESPONSE counterResponse;
		counterResponse.pu8Signature = signature;
		counterResponse.u16SignatureLength = SIGNATURE_SIZE;

		returnValue = m_theBaseApi->VltDecrementCounter(COUNTER_NUMBER, COUNTER_GROUP, &counterData, &counterResponse);
		if (returnValue != VLT_OK)
		{
			if (returnValue == 0x6A80)
			{
				return MATERIAL_OVERCONSUMPTION;
			}

			return returnValue;
		}

		newSignedVolume->SetCounterValue(counterResponse.u8CntValue);
		newSignedVolume->SetMessage(counterResponse.u8Rnd, m_cartridgeInfo->au8Serial, counterData.nounce.u8Nounce);
		newSignedVolume->SetSignature(counterResponse.pu8Signature, counterResponse.u16SignatureLength);
		return IDTLIB_SUCCESS;
	}

	int VaultIC100Adapter::GetInPlaceStatus(int *pStatus)
	{
		if (m_bUseUsb)
		{
			if (!m_bIsInitialized)
			{
				return FCB_GET_IN_PLACE_STATUS_FAILED;
			}

			*pStatus = 1;
			return IDTLIB_SUCCESS;
		}

		return IDTagAdapter::GetInPlaceStatus(pStatus);
	}

	/// <summary>
	/// Gets the serial numbe of a cartridge.
	/// </summary>
	/// <param name="ucCartridgeNum">The cartridge number.</param>
	/// <param name="ucSerialNumberLength">The cartridge serial number (output parameter).</param>
	/// <returns>0 on success, non-zero for failures.</returns>
	int VaultIC100Adapter::GetSerialNumber(unsigned char *aucSerialNumber, unsigned char *ucSerialNumberLength)
	{
		if (m_cartridgeInfo == NULL)
		{
			return 1;
		}

		memcpy(aucSerialNumber, m_cartridgeInfo->au8Serial, VLT_CHIP_SERIAL_NUMBER_LENGTH);
		*ucSerialNumberLength = VLT_CHIP_SERIAL_NUMBER_LENGTH;
		return IDTLIB_SUCCESS;
	}

	int VaultIC100Adapter::SwitchToCartridge(unsigned char ucCartridgeNum)
	{
		int returnValue;
		if (!m_bIsInitialized)
		{
			returnValue = InitLibrary(ucCartridgeNum, 0);
			if (returnValue != IDTLIB_SUCCESS)
			{
				return returnValue;
			}
		}

		if (m_theBaseApi == NULL)
		{
			return HW_NOT_INITIALIZED;
		}

		returnValue = IDTagAdapter::SwitchToCartridge(ucCartridgeNum);
		if (returnValue != IDTLIB_SUCCESS)
		{
			return returnValue;
		}

		returnValue = m_theBaseApi->VltGetInfo(m_cartridgeInfo);
		if (returnValue != VLT_OK)
		{
			return returnValue;
		}

		return IDTLIB_SUCCESS;
	}
}
