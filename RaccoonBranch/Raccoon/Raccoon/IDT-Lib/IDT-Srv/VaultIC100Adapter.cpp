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
	int VaultIC100Adapter::ChallengeHostDevice(const unsigned char *aucHostChallenge, unsigned short usHostChallengeLength, unsigned char *aucDeviceChallenge, 
		unsigned short *pusDeviceChallengeLength, unsigned char *aucSignedDeviceChallenge, unsigned short *pusSignedDeviceChallengeLength)
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
	/// <param name="random">Random array for replacing VltGenerateRandom (output parameter).</param>
	/// <returns>0 on success, non-zero for failures.</returns>
	int VaultIC100Adapter::DecreaseConsumption(unsigned int uiConsumption, CounterResponse *newSignedVolume, unsigned char *random)
	{
		if (m_theBaseApi == NULL)
		{
			return HW_NOT_INITIALIZED;
		}

		LOG_EXT(LEVEL_DEBUG, "Generate random bytes for nounce...");

		VLT_COUNTER_DATA counterData;
		if(random == NULL)
		{
			int returnValue = m_theBaseApi->VltGenerateRandom(VLT_NOUNCE_LENGTH, counterData.nounce.u8Nounce);
			if (returnValue != VLT_OK)
			{
				return SelectError(returnValue, GENERATE_RANDOM_FAILED);
			}
		}
		else
		{
			memcpy(counterData.nounce.u8Nounce, random, VLT_NOUNCE_LENGTH);
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

		int returnValue = m_theBaseApi->VltDecrementCounter(COUNTER_NUMBER, COUNTER_GROUP, &counterData, &counterResponse);
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
