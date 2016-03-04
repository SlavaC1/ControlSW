#include "stdafx.h"
#include "VaultIC100Adapter.h"
#include "IDTHWInf.h"
#include "vaultic_apdu.h"
#include "Utils.h"
#ifdef K163
#include "K163_K1_Curve.h"
#endif

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
#define IDD_PATH			"/app/file1"
#define SIGNATURE_PATH		"/app/file2"

	VaultIC100Adapter::VaultIC100Adapter()
		: m_theBaseApi(NULL), m_cartridgeInfo(new VLT_TARGET_INFO()), m_bIsFcbInitialized(false)
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
		int cartridgesCount;

		IdtClose();
		returnValue = IdtInit(&cartridgesCount);
		if (returnValue == FALSE)
		{
#ifdef ENABLE_USB
			bool bUseUsb = InitUsbDevice();
			if (bUseUsb)
			{
				return IDTLIB_SUCCESS;
			}
#endif
			return FCB_INIT_FAILED;
		}

		m_bIsFcbInitialized = true;
		m_ucCartridgesCount = *pucCartridgesCount = (unsigned char)cartridgesCount;
		returnValue = GetInPlaceStatus(&status);
		if (returnValue != IDTLIB_SUCCESS)
		{
			return returnValue;
		}

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

			if ((m_ucCartridgesCount > 1) && (cartridgeNum < m_ucCartridgesCount))
			{
				returnValue = InitLibrary(cartridgeNum, 0);
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
	bool VaultIC100Adapter::InitUsbDevice()
	{
		unsigned int uiDeviceSerialNumber = 0;
		int status = ParseDeviceSerialNumber(&uiDeviceSerialNumber);
		bool bUseUsb = ((status == VLT_OK) && (uiDeviceSerialNumber != 0));
		if (bUseUsb)
		{
			if (m_ucCartridgesCount != 1)
			{
				m_ucCartridgesCount = 1;
				InitLibrary(0, uiDeviceSerialNumber);
			}
		}
		else
		{
			m_ucCartridgesCount = 0;
		}

		return bUseUsb;
	}

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

	int VaultIC100Adapter::SelectError(int returnedError, int suggestedValue)
	{
		if ((returnedError == 0xCF0D) ||
			(returnedError == 0xCF0F) ||
			(returnedError == 0x6581) ||
			(returnedError == 0x65FF))
		{
			return returnedError;
		}

		return suggestedValue;
	}

#if (ENABLE_BURNING == 1)

	int VaultIC100Adapter::Login(const unsigned char* acPassword, unsigned char usPasswordLength)
	{
		if (m_theBaseApi == NULL)
		{
			return HW_NOT_INITIALIZED;
		}

		int returnValue;

		LOG_EXT(LEVEL_DEBUG, "Logging in as manufacturer...");
		returnValue = m_theBaseApi->VltSubmitPassword(usPasswordLength, acPassword);
		if (returnValue != VLT_OK)
		{
			if (returnValue = 0x6300)
			{
				return IDT_LOCKED;
			}

			return SelectError(returnValue, LOGIN_FAILED);
		}

		LOG_EXT(LEVEL_DEBUG, "Setting creation mode...");
		returnValue = m_theBaseApi->VltSetStatus(VLT_CREATION);
		if (returnValue != VLT_OK)
		{
			if (returnValue == 0x6985)
			{
				return ACCESS_DENIED;
			}

			return SelectError(returnValue, TRANSITION_FAILED);
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

#ifdef K163
		LOG_EXT(LEVEL_DEBUG, "Setting K-163 curve...");
		unsigned char ucCurve = VLT_ECDSA_PARAMS_USER_DEF_CURVE_CHOICE;
		returnValue = m_theBaseApi->VltSetConfig(VLT_ECDSA_DOMAIN_PARAM_CHOICE, VLT_ECDSA_PARAMS_CHOICE_SZ, &ucCurve);
		if (returnValue != VLT_OK)
		{
			return SelectError(returnValue, CONFIGURATION_FAILED);
		}

		VLT_FILE_PRIVILEGES KeyPrivileges = { 0x03, 0x00, 0x0, 0x2 };
		VLT_KEY_OBJECT DomainParams;
		DomainParams.u8KeyID = VLT_KEY_ECDSA_DOMAIN_PARMS;
		DomainParams.data.EcdsaParamsKey.u16QLen = ucBinaryFieldLength;
		DomainParams.data.EcdsaParamsKey.pu8Q = auc_Q;
		DomainParams.data.EcdsaParamsKey.pu8Gx = auc_Gx;
		DomainParams.data.EcdsaParamsKey.pu8Gy = auc_Gy;
		DomainParams.data.EcdsaParamsKey.pu8Gz = auc_Gz;
		DomainParams.data.EcdsaParamsKey.pu8A = auc_A;
		DomainParams.data.EcdsaParamsKey.pu8B = auc_B;
		DomainParams.data.EcdsaParamsKey.u16NLen = ucBinaryFieldLength;
		DomainParams.data.EcdsaParamsKey.pu8N = auc_N;
		DomainParams.data.EcdsaParamsKey.u32H = us_H;
		DomainParams.data.EcdsaParamsKey.u8Assurance = 0;
		returnValue = m_theBaseApi->VltPutKey((VLT_U8)enDeviceKeyGroup, (VLT_U8)enDomainParamsKeyIndex, &KeyPrivileges, &DomainParams);
		if (returnValue != VLT_OK)
		{
			return SelectError(returnValue, CONFIGURATION_FAILED);
		}
#endif
#ifdef K283
		LOG_EXT(LEVEL_DEBUG, "Setting K-283 curve...");
		unsigned char ucCurve = VLT_ECDSA_CURVE_K283;
		returnValue = m_theBaseApi->VltSetConfig(VLT_ECDSA_DOMAIN_PARAM_CHOICE, VLT_ECDSA_PARAMS_CHOICE_SZ, &ucCurve);
		if (returnValue != VLT_OK)
		{
			return SelectError(returnValue, CONFIGURATION_FAILED);
		}
#endif

		LOG_EXT(LEVEL_DEBUG, "Setting non-FIPS mode...");
		unsigned char ucFipsMode = 0;
		//unsigned char ucFipsMode = 1;
		returnValue = m_theBaseApi->VltSetConfig(VLT_FIPS_MODE, VLT_FIPS_MODE_SZ, &ucFipsMode);
		if (returnValue != VLT_OK)
		{
			return SelectError(returnValue, CONFIGURATION_FAILED);
		}

		LOG_EXT(LEVEL_DEBUG, "Enforce I2C communication...");
		unsigned char ucCommunicationChannel = 3; //always use I2C
		returnValue = m_theBaseApi->VltSetConfig(VLT_COMMS_CHANNEL_ACCESS, VLT_COMMS_CHANNEL_SZ, &ucCommunicationChannel);
		if (returnValue != VLT_OK)
		{
			return SelectError(returnValue, CONFIGURATION_FAILED);
		}

		LOG_EXT(LEVEL_DEBUG, "Allow read and write access to file system...");
		unsigned char ucEveryone = 0; // everyone can access static file system
		returnValue = m_theBaseApi->VltSetConfig(VLT_USER_FILE_ACCESS, VLT_USER_FILE_ACCESS_SZ, &ucEveryone);
		if (returnValue != VLT_OK)
		{
			return SelectError(returnValue, CONFIGURATION_FAILED);
		}

		return IDTLIB_SUCCESS;
	}

	int VaultIC100Adapter::SetKeys(PubKey* pubKS)
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
			return SelectError(returnValue, KEY_GENERATION_FAILED);
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
			return SelectError(returnValue, KEY_GENERATION_FAILED);
		}

		EC2NPoint Q = pubKS->GetPublicElement();
		int keyCoeffPaddedSize = GetPaddedLength(KEY_COEFF_SIZE, 4);
		VLT_PU8 Qx = new VLT_U8[keyCoeffPaddedSize];
		int padding = keyCoeffPaddedSize - Q.x.ByteCount();
		if (padding > 0)
		{
			memset(Qx, 0, padding);
		}
		Q.x.Encode(Qx + padding, Q.x.ByteCount());

		VLT_PU8 Qy = new VLT_U8[keyCoeffPaddedSize];
		padding = keyCoeffPaddedSize - Q.y.ByteCount();
		if (padding > 0)
		{
			memset(Qy, 0, padding);
		}
		Q.y.Encode(Qy + padding, Q.y.ByteCount());

		LOG_EXT(LEVEL_DEBUG, "Writing host public key...");
		KeyObj.u8KeyID = VLT_KEY_ECDSA_GF2M_PUB;
		KeyObj.data.EcdsaPubKey.u16QLen = keyCoeffPaddedSize;
		KeyObj.data.EcdsaPubKey.pu8Qx = Qx;
		KeyObj.data.EcdsaPubKey.pu8Qy = Qy;
		KeyObj.data.EcdsaPubKey.u8DomainParamsGroup = (VLT_U8)enDeviceKeyGroup;
		KeyObj.data.EcdsaPubKey.u8DomainParamsIndex = (VLT_U8)enDomainParamsKeyIndex;
		KeyObj.data.EcdsaPubKey.u8Assurance = 0;
		returnValue = m_theBaseApi->VltPutKey((VLT_U8)enHostKeyGroup, (VLT_U8)enPublicKeyIndex, &KeyPrivileges, &KeyObj);
		delete[] Qx;
		delete[] Qy;
		if (returnValue != VLT_OK)
		{
			return SelectError(returnValue, KEY_GENERATION_FAILED);
		}

		LOG_EXT(LEVEL_DEBUG, "Generating device private and public key pair...");
		VLT_KEY_GEN_DATA keyGeneratorData;
		keyGeneratorData.u8AlgoID = VLT_ALG_KPG_ECDSA_GF2M;
		keyGeneratorData.EcdsaKeyGenObj.u8DomainParamsGroup = enDeviceKeyGroup;
		keyGeneratorData.EcdsaKeyGenObj.u8DomainParamsIndex = enDomainParamsKeyIndex;
		returnValue = m_theBaseApi->VltGenerateKeyPair((VLT_U8)enDeviceKeyGroup, (VLT_U8)enPublicKeyIndex, (VLT_U8)enDeviceKeyGroup, (VLT_U8)enPrivateKeyIndex, &keyGeneratorData);
		if (returnValue != VLT_OK)
		{
			return SelectError(returnValue, KEY_GENERATION_FAILED);
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
		returnValue = m_theBaseApi->VltSetConfig(VLT_COUNTER_MODE_PARAM, VLT_COUNTER_MODE_SZ, &ucCounterMode);
		if (returnValue != VLT_OK)
		{
			return SelectError(returnValue, COUNTER_SET_FAILED);
		}

		returnValue = m_theBaseApi->VltSetSecureCounters(COUNTER_NUMBER, COUNTER_GROUP, sizeof(uiCouterValue), (VLT_PU8)(&uiCouterValue));
		if (returnValue != VLT_OK)
		{
			return SelectError(returnValue, COUNTER_SET_FAILED);
		}

		ucCounterMode = VLT_COUNTER_MODE;
		returnValue = m_theBaseApi->VltSetConfig(VLT_COUNTER_MODE_PARAM, VLT_COUNTER_MODE_SZ, &ucCounterMode);
		if (returnValue != VLT_OK)
		{
			return SelectError(returnValue, COUNTER_SET_FAILED);
		}

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
			return SelectError(returnValue, TRANSITION_FAILED);
		}

		LOG_EXT(LEVEL_DEBUG, "Logging out as manufacturer...");
		returnValue = m_theBaseApi->VltCancelAuthentication();
		if (returnValue != VLT_OK)
		{
			return SelectError(returnValue, LOGIN_FAILED);
		}

		return IDTLIB_SUCCESS;
	}

	int VaultIC100Adapter::WriteMaterialInfo(unsigned char* aucMaterialInfo, unsigned short usMaterialInfoLength)
	{
		if (m_theBaseApi == NULL)
		{
			return HW_NOT_INITIALIZED;
		}

		int returnValue;

		LOG_EXT(LEVEL_DEBUG, "Selecting File1...");
		VLT_SELECT respData;
		returnValue = m_theBaseApi->VltSelectFile((VLT_U8*)IDD_PATH, strlen(IDD_PATH), &respData);
		if (returnValue != VLT_OK)
		{
			return SelectError(returnValue, FILE_SELECTION_FAILED);
		}

		LOG_EXT(LEVEL_DEBUG, "Writing material info into File1...");
		unsigned char ucMaterialInfoLength = (unsigned char)usMaterialInfoLength;
		returnValue = m_theBaseApi->VltWriteFile(&ucMaterialInfoLength, 1);
		returnValue = m_theBaseApi->VltWriteFile(aucMaterialInfo, (VLT_U8)usMaterialInfoLength);
		if (returnValue != VLT_OK)
		{
			return SelectError(returnValue, WRITING_IDD_FILE_FAILED);
		}

		return IDTLIB_SUCCESS;
	}

	int VaultIC100Adapter::WriteSignature(unsigned char* aucSignature, unsigned short usSignatureLength)
	{
		if (m_theBaseApi == NULL)
		{
			return HW_NOT_INITIALIZED;
		}

		int returnValue;

		LOG_EXT(LEVEL_DEBUG, "Selecting File2...");
		VLT_SELECT respData;
		returnValue = m_theBaseApi->VltSelectFile((VLT_U8*)SIGNATURE_PATH, strlen(SIGNATURE_PATH), &respData);
		if (returnValue != VLT_OK)
		{
			return SelectError(returnValue, FILE_SELECTION_FAILED);
		}

		LOG_EXT(LEVEL_DEBUG, "Writing signature into File2...");
		unsigned char ucSignatureLength = (unsigned char)usSignatureLength;
		returnValue = m_theBaseApi->VltWriteFile(&ucSignatureLength, 1);
		returnValue = m_theBaseApi->VltWriteFile(aucSignature, (VLT_U8)usSignatureLength);
		if (returnValue != VLT_OK)
		{
			return SelectError(returnValue, WRITING_SIGNATURE_FILE_FAILED);
		}

		return IDTLIB_SUCCESS;
	}

#endif

	int VaultIC100Adapter::ReadMaterialInfo(unsigned char* aucMaterialInfo, unsigned short* pusMaterialInfoLength)
	{
		if (m_theBaseApi == NULL)
		{
			return HW_NOT_INITIALIZED;
		}

		LOG_EXT(LEVEL_DEBUG, "Selecting File1...");
		VLT_SELECT respData;
		int returnValue = m_theBaseApi->VltSelectFile((const VLT_U8*)IDD_PATH, (VLT_U8)strlen(IDD_PATH), &respData);
		if (returnValue != VLT_OK)
		{
			return FILE_SELECTION_FAILED;
		}

		LOG_EXT(LEVEL_DEBUG, "Reading material info...");
		unsigned short usFileLength = 89;
		unsigned char aucBuffer[89];
		returnValue = m_theBaseApi->VltReadFile(&usFileLength, aucBuffer);
		if ((returnValue != VLT_OK) &&
			(returnValue != VLT_EOF) &&
			(returnValue != VLT_STATUS_EOF))
		{
			return SelectError(returnValue, INVALID_CERTIFICATE_FILE);
		}

		bool bIsEmpty = IsFileEmpty(aucBuffer, usFileLength);
		if (bIsEmpty)
		{
			return EMPTY_CERTIFICATE_FILE;
		}

		*pusMaterialInfoLength = aucBuffer[0];
		memcpy(aucMaterialInfo, aucBuffer + 1, *pusMaterialInfoLength);
		return IDTLIB_SUCCESS;
	}

	int VaultIC100Adapter::ReadSignature(unsigned char* aucSignature, unsigned short* pusSignatureLength)
	{
		if (m_theBaseApi == NULL)
		{
			return HW_NOT_INITIALIZED;
		}

		LOG_EXT(LEVEL_DEBUG, "Selecting File2...");
		VLT_SELECT respData;
		int returnValue = m_theBaseApi->VltSelectFile((const VLT_U8*)SIGNATURE_PATH, (VLT_U8)strlen(SIGNATURE_PATH), &respData);
		if (returnValue != VLT_OK)
		{
			return SelectError(returnValue, FILE_SELECTION_FAILED);
		}

		LOG_EXT(LEVEL_DEBUG, "Reading signature...");
		unsigned short usFileLength = 89;
		unsigned char aucBuffer[89];
		returnValue = m_theBaseApi->VltReadFile(&usFileLength, aucBuffer);
		if ((returnValue != VLT_OK) &&
			(returnValue != VLT_EOF) &&
			(returnValue != VLT_STATUS_EOF))
		{
			bool bIsEmpty = IsFileEmpty(aucBuffer, usFileLength);
			return SelectError(returnValue, (bIsEmpty) ? EMPTY_CERTIFICATE_FILE : INVALID_CERTIFICATE_FILE);
		}

		*pusSignatureLength = aucBuffer[0];
		memcpy(aucSignature, aucBuffer + 1, *pusSignatureLength);
		return IDTLIB_SUCCESS;
	}

	bool VaultIC100Adapter::IsFileEmpty(const unsigned char *aucFile, unsigned short usLength)
	{
		for (unsigned short us = 0; us < usLength; us++)
		{
			if (aucFile[us] != 0xFF)
			{
				return false;
			}
		}

		return true;
	}

	int VaultIC100Adapter::ReadPubKC(unsigned char* aucPubKC, unsigned short* pusKeyLength)
	{
		if (m_theBaseApi == NULL)
		{
			return HW_NOT_INITIALIZED;
		}

		LOG_EXT(LEVEL_DEBUG, "Read tag public key...");
		// Length of buffers passed to VaultIC100 shall be 4n; trail, if exists is padded with zeroes
		int keyCoeffPaddedSize = GetPaddedLength(KEY_COEFF_SIZE, 4);
		VLT_PU8 aucReadQx = new VLT_U8[keyCoeffPaddedSize];
		VLT_PU8 aucReadQy = new VLT_U8[keyCoeffPaddedSize];

		VLT_KEY_OBJECT readKey;
		readKey.u8KeyID = VLT_KEY_ECDSA_GF2M_PUB;
		readKey.data.EcdsaPubKey.u16QLen = keyCoeffPaddedSize;
		readKey.data.EcdsaPubKey.pu8Qx = aucReadQx;
		readKey.data.EcdsaPubKey.pu8Qy = aucReadQy;

		int returnValue = m_theBaseApi->VltReadKey((VLT_U8)enDeviceKeyGroup, (VLT_U8)enPublicKeyIndex, &readKey);
		if (returnValue == VLT_OK)
		{
			int padding = keyCoeffPaddedSize - KEY_COEFF_SIZE;
			unsigned short offset = 0;
			memcpy(aucPubKC + offset, aucReadQx + padding, readKey.data.EcdsaPubKey.u16QLen - padding);
			offset += readKey.data.EcdsaPubKey.u16QLen - padding;
			memcpy(aucPubKC + offset, aucReadQy + padding, readKey.data.EcdsaPubKey.u16QLen - padding);
			offset += readKey.data.EcdsaPubKey.u16QLen - padding;

			*pusKeyLength = offset;
			returnValue = IDTLIB_SUCCESS;
		}
		else
		{
			returnValue = SelectError(returnValue, INVALID_DEVICE_KEY);
		}

		delete[] aucReadQx;
		delete[] aucReadQy;

		return returnValue;
	}

	int VaultIC100Adapter::GenerateCertificate(const unsigned char *aucIdd, unsigned short usIddLength, unsigned char *aucCertificate, unsigned short* pusCertificateLength)
	{
		if (m_theBaseApi == NULL)
		{
			return HW_NOT_INITIALIZED;
		}

		LOG_EXT(LEVEL_DEBUG, "Generating certificate file...");
		size_t offset = 0;
		memcpy(aucCertificate + offset, aucIdd, usIddLength);
		offset += usIddLength;

		unsigned char aucPubKC[2 * KEY_COEFF_SIZE];
		unsigned short usKeyLength;
		int returnValue = ReadPubKC(aucPubKC, &usKeyLength);
		if (returnValue != IDTLIB_SUCCESS)
		{
			return returnValue;
		}

		memcpy(aucCertificate + offset, aucPubKC, usKeyLength);
		offset += usKeyLength;

		*pusCertificateLength = offset;
		return IDTLIB_SUCCESS;
	}	

	/// <summary>
	/// Sends challenge to be signed by device.
	/// </summary>
	/// <returns>0 on success, non-zero for failures.</returns>
	int VaultIC100Adapter::ChallengeHostDevice(const unsigned char *aucHostChallenge, unsigned short usHostChallengeLength, unsigned char *aucDeviceChallenge, unsigned short *pusDeviceChallengeLength, unsigned char *aucSignedDeviceChallenge, unsigned short *pusSignedDeviceChallengeLength)
	{
		if (m_theBaseApi == NULL)
		{
			return HW_NOT_INITIALIZED;
		}

		LOG_EXT(LEVEL_DEBUG, "Perform internal authentication...");

		// Length of buffers passed to VaultIC100 shall be 8n; trail, if exists is padded with zeroes
		unsigned char *aucTmpSignedDeviceChallenge;
		int signatureSize = GetPaddedLength(SIGNATURE_SIZE, 8);
		if (signatureSize == SIGNATURE_SIZE)
		{
			aucTmpSignedDeviceChallenge = aucSignedDeviceChallenge;
		}
		else
		{
			*pusSignedDeviceChallengeLength = signatureSize;
			aucTmpSignedDeviceChallenge = new unsigned char[signatureSize];
		}

		int returnValue = m_theBaseApi->VltInternalAuthenticate(VLT_USER, VLT_APPROVED_USER, (VLT_U8)usHostChallengeLength, aucHostChallenge, aucDeviceChallenge, pusSignedDeviceChallengeLength, aucTmpSignedDeviceChallenge);
		if (returnValue != VLT_OK)
		{
			return SelectError(returnValue, CHALLENGE_FAILED);
		}

		unsigned short offset = 0;
		if (aucTmpSignedDeviceChallenge != aucSignedDeviceChallenge)
		{
			int padding = (signatureSize - SIGNATURE_SIZE) / 2;
			offset = 0;
			memcpy(aucSignedDeviceChallenge + offset, aucTmpSignedDeviceChallenge + offset + padding, SIGNATURE_SIZE / 2);
			offset += SIGNATURE_SIZE / 2;
			padding += (signatureSize - SIGNATURE_SIZE) / 2;
			memcpy(aucSignedDeviceChallenge + offset, aucTmpSignedDeviceChallenge + offset + padding, SIGNATURE_SIZE / 2);
			offset += SIGNATURE_SIZE / 2;

			*pusSignedDeviceChallengeLength = offset;
			delete[] aucTmpSignedDeviceChallenge;
		}

		offset = VLT_INTERNAL_AUTH_DEV_CHALLENGE_LENGTH;
		memcpy(aucDeviceChallenge + offset, aucHostChallenge, usHostChallengeLength);
		offset += usHostChallengeLength;

		VLT_KEY_OBJECT hostIdObject;
		hostIdObject.u8KeyID = VLT_KEY_HOST_DEVICE_ID;
		unsigned char hostID[VLT_HOST_DEV_ID_STRING_LENGTH];
		VLT_KEY_OBJ_ID hostObjID;
		hostObjID.pu8StringId = hostID;
		hostObjID.u16StringLen = sizeof(hostID);
		hostIdObject.data.HostDeviceIdKey = hostObjID;
		returnValue = m_theBaseApi->VltReadKey((VLT_U8)enHostKeyGroup, (VLT_U8)enHostDeviceIDKeyIndex, &hostIdObject);

		memcpy(aucDeviceChallenge + offset, hostID, sizeof(hostID));
		offset += sizeof(hostID);
		*pusDeviceChallengeLength = offset;

		return IDTLIB_SUCCESS;
	}

	/// <summary>
	/// Gets the current volume of a cartridge.
	/// </summary>
	/// <param name="ucCartridgeNum">The cartridge number.</param>
	/// <param name="puiCurrentVolume">The cartridge current volume (output parameter).</param>
	/// <returns>0 on success, non-zero for failures.</returns>
	int VaultIC100Adapter::GetCurrentVolume(unsigned int *puiCurrentVolume)
	{
		CounterResponse signedVolume;
		int returnValue = DecreaseConsumption(0, &signedVolume);
		*puiCurrentVolume = signedVolume.GetCounterValue();
		return returnValue;
	}

	/// <summary>
	/// Decreases consumption from cartridge current volume.
	/// </summary>
	/// <param name="uiConsumption">The volume consumption.</param>
	/// <param name="newSignedVolume">The cartridge new volume and its signature by PrvKC (output parameter).</param>
	/// <returns>0 on success, non-zero for failures.</returns>
	int VaultIC100Adapter::DecreaseConsumption(unsigned int uiConsumption, CounterResponse *newSignedVolume)
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
			return SelectError(returnValue, GENERATE_RANDOM_FAILED);
		}

		LOG_EXT(LEVEL_DEBUG, "Decrementing counter by " << uiConsumption << "...");
		counterData.amount.u8AmountLength = VLT_COUNTER_LENGTH;
		memcpy(counterData.amount.u8Amount, &uiConsumption, sizeof(unsigned int));
		counterData.nounce.u8NounceLength = VLT_NOUNCE_LENGTH;

		// Length of buffers passed to VaultIC100 shall be 8n; trail, if exists is padded with zeroes
		int signatureSize = GetPaddedLength(SIGNATURE_SIZE, 8);
		unsigned char* pucSignature = new unsigned char[signatureSize];
		VLT_COUNTER_RESPONSE counterResponse;
		counterResponse.pu8Signature = pucSignature;
		counterResponse.u16SignatureLength = signatureSize;

		returnValue = m_theBaseApi->VltDecrementCounter(COUNTER_NUMBER, COUNTER_GROUP, &counterData, &counterResponse);
		if (returnValue == VLT_OK)
		{
			newSignedVolume->SetCounterValue(counterResponse.u8CntValue);
			newSignedVolume->SetMessage(counterResponse.u8Rnd, m_cartridgeInfo->au8Serial, counterData.nounce.u8Nounce);

			unsigned char aucSignature[SIGNATURE_SIZE];
			int padding = (signatureSize - SIGNATURE_SIZE) / 2;
			int offset = 0;
			memcpy(aucSignature + offset, counterResponse.pu8Signature + offset + padding, SIGNATURE_SIZE / 2);
			offset += SIGNATURE_SIZE / 2;
			padding += (signatureSize - SIGNATURE_SIZE) / 2;
			memcpy(aucSignature + offset, counterResponse.pu8Signature + offset + padding, SIGNATURE_SIZE / 2);
			newSignedVolume->SetSignature(aucSignature, SIGNATURE_SIZE);

			returnValue = IDTLIB_SUCCESS;
		}
		else
		{
			if (returnValue == 0x6A80)
			{
				returnValue = MATERIAL_OVERCONSUMPTION;
			}
		}

		delete[] pucSignature;
		return returnValue;
	}

	int VaultIC100Adapter::GetInPlaceStatus(int *pStatus)
	{
		int returnValue = HW_NOT_INITIALIZED;
		if (m_bIsFcbInitialized)
		{
			returnValue = IDTagAdapter::GetInPlaceStatus(pStatus);
		}
#ifdef ENABLE_USB
		else
		{
			if ((returnValue != IDTLIB_SUCCESS) || (*pStatus == 0))
			{
				bool bUseUsb = InitUsbDevice();
				if (bUseUsb)
				{
					*pStatus = 1;
				}

				returnValue = IDTLIB_SUCCESS;
			}
		}
#endif

		return returnValue;
	}

	/// <summary>
	/// Gets the serial numbe of a cartridge.
	/// </summary>
	/// <param name="ucSerialNumberLength">The cartridge serial number (output parameter).</param>
	/// <returns>0 on success, non-zero for failures.</returns>
	int VaultIC100Adapter::GetSerialNumber(unsigned char *aucSerialNumber, unsigned char *ucSerialNumberLength)
	{
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
			return SelectError(returnValue, GET_INFO_FAILED);
		}

		return IDTLIB_SUCCESS;
	}
}
