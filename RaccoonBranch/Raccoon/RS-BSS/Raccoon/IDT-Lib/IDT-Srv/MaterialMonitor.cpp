#include "stdafx.h"
#include "MaterialMonitor.h"
#include "IDTagAdapterFactory.h"
#include "UniqueLock.h"
#include "osrng.h"
#include "oids.h"
#include "Utils.h"

#include <sstream>
#include <iomanip>

#ifndef _WIN32
#include <errno.h>
#endif

using namespace std;
using namespace RSCommon;

namespace StratasysIDTSrv
{
#define TOKEN_SIZE				VLT_INTERNAL_AUTH_HOST_CHALLENGE_LENGTH
#define INIT_TIMEOUT			120

	static void* ExecuteVerifyMessage(void* args);

	/// <summary>
	/// Initializes a new instance of the <see cref="MaterialMonitor"/> class.
	/// </summary>
	MaterialMonitor::MaterialMonitor()
		: BaseMaterialMonitor(), m_certificates(NULL), m_veriferS(NULL), m_bVerified(false)
	{
		m_tagAdapter = IDTagAdapterFactory::CreateIDTagAdapter();
		m_ucCartridgesCount = 0;

		LOG_EXT(LEVEL_INFO, "IDT-Srv initialized.");
	}

	/// <summary>
	/// Finalizes an instance of the <see cref="MaterialMonitor"/> class.
	/// </summary>
	MaterialMonitor::~MaterialMonitor()
	{
		if (m_veriferS != NULL)
		{
			delete m_veriferS;
		}

		if (m_certificates != NULL)
		{
			delete[] m_certificates;
		}

		delete m_tagAdapter;
	}

	/// <summary>
	/// Initializes underlying hardware.
	/// </summary>
	/// <returns>0 on success, non-zero for failures.</returns>
	int MaterialMonitor::InitHW(unsigned char *pucCartridgesCount)
	{
		int returnValue = BaseMaterialMonitor::InitHW(pucCartridgesCount);
		if (returnValue != IDTLIB_SUCCESS)
		{
			return returnValue;
		}

		UniqueLock uniqueLock(m_mtx);
		m_bInitialized = false;

		returnValue = m_tagAdapter->InitHW(pucCartridgesCount);
		if (returnValue != IDTLIB_SUCCESS)
		{
			LOG_EXT(LEVEL_FATAL, "Error initializing HW (error code 0x" << hex << (short)returnValue << ").");
			return returnValue;
		}

		if (m_ucCartridgesCount != *pucCartridgesCount)
		{
			m_ucCartridgesCount = *pucCartridgesCount;
			InitCertificates();
		}

		LOG_EXT(LEVEL_INFO, "HW initialized successfully. Number of cartridges: " << (unsigned short)(*pucCartridgesCount) << ".");
		m_bInitialized = true;
		return returnValue;
	}

	int MaterialMonitor::GetInPlaceStatus(int *pStatus)
	{
		UniqueLock uniqueLock(m_mtx);

		int returnValue = IDTLIB_SUCCESS;
		if (pStatus == NULL)
		{
			returnValue = FCB_NULL_PARAM;
			LOG_EXT(LEVEL_ERROR, "Status pointer is null (error code 0x" << hex << (short)returnValue << ").");
			return returnValue;
		}

		if (!m_bInitialized)
		{
			returnValue = HW_NOT_INITIALIZED;
			LOG_EXT(LEVEL_ERROR, "Hardware not initialized (error code 0x" << hex << (short)returnValue << ").");
			return returnValue;
		}

		LOG_EXT(LEVEL_DEBUG, "Getting in-place status:");
		returnValue = m_tagAdapter->GetInPlaceStatus(pStatus);
		if (returnValue == IDTLIB_SUCCESS)
		{
			unsigned char ucCartridgesCount = m_tagAdapter->GetCartridgesCount();
			if (ucCartridgesCount != m_ucCartridgesCount)
			{
				m_ucCartridgesCount = ucCartridgesCount;
				InitCertificates();
			}

			LOG_EXT(LEVEL_DEBUG, "Status: 0x" << setfill('0') << setw(4) << hex << *pStatus << ".");
		}
		else
		{
			LOG_EXT(LEVEL_ERROR, "Error getting in-place status (error code 0x" << hex << (short)returnValue << ").");
		}

		return returnValue;
	}

#if (ENABLE_BURNING == 1)

	/// <summary>
	/// Generates certificate from material information and burns it into tag.
	/// </summary>
	/// <param name="ucCartridgeNum">The cartridge number.</param>
	/// <param name="aucMaterialInfo">The material information.</param>
	/// <param name="usMaterialInfoLength">The material information length.</param>
	/// <param name="aucPrvKS">SSYS private key.</param>
	/// <param name="aucPubKS">SSYS public key.</param>
	/// <returns>0 on success, non-zero for failures.</returns>
	int MaterialMonitor::ResetCartridge(unsigned char ucCartridgeNum, const unsigned char* acPassword, unsigned char usPasswordLength, const unsigned char* aucPubKS)
	{
		UniqueLock uniqueLock(m_mtx);

		int returnValue = IDTLIB_SUCCESS;

		try
		{
			if (aucPubKS == NULL && m_pubKS == NULL)
			{
				returnValue = INVALID_HOST_KEY;
				LOG_EXT(LEVEL_ERROR, "Invalid host key (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			LOG_EXT(LEVEL_INFO, "Reset cartridge #" << (unsigned short)ucCartridgeNum << ":");
			LOG_EXT(LEVEL_INFO, "Switching to cartridge #"<< (unsigned short)ucCartridgeNum << "...");
			returnValue = m_tagAdapter->SwitchToCartridge(ucCartridgeNum);
			if (returnValue != IDTLIB_SUCCESS)
			{
				LOG_EXT(LEVEL_ERROR, "Error switching to cartridge #"<< (unsigned short)ucCartridgeNum << " (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			LOG_EXT(LEVEL_INFO, "Logging in to cartridge #"<< (unsigned short)ucCartridgeNum << " ...");
			returnValue = m_tagAdapter->Login(acPassword, usPasswordLength);
			if (returnValue != IDTLIB_SUCCESS)
			{
				LOG_EXT(LEVEL_ERROR, "Error logging in to cartridge #"<< (unsigned short)ucCartridgeNum << " (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			LOG_EXT(LEVEL_INFO, "Configuring cartridge #"<< (unsigned short)ucCartridgeNum << "...");
			returnValue = m_tagAdapter->Configure();
			if (returnValue != IDTLIB_SUCCESS)
			{
				LOG_EXT(LEVEL_ERROR, "Error configuring to cartridge #"<< (unsigned short)ucCartridgeNum << " (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			LOG_EXT(LEVEL_INFO, "Setting keys of cartridge #"<< (unsigned short)ucCartridgeNum << "...");
			if (m_pubKS == NULL)
			{
				m_pubKS = LoadPublicKey(aucPubKS);
			}

			returnValue = m_tagAdapter->SetKeys(m_pubKS);
			if (returnValue != IDTLIB_SUCCESS)
			{
				LOG_EXT(LEVEL_ERROR, "Error setting keys of cartridge #"<< (unsigned short)ucCartridgeNum << " (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			LOG_EXT(LEVEL_INFO, "Setting counter of cartridge #"<< (unsigned short)ucCartridgeNum << "...");
			returnValue = m_tagAdapter->SetCounter(200000000u);
			if (returnValue != IDTLIB_SUCCESS)
			{
				LOG_EXT(LEVEL_ERROR, "Error setting counter of cartridge #"<< (unsigned short)ucCartridgeNum << " (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			LOG_EXT(LEVEL_INFO, "Logging out from cartridge #"<< (unsigned short)ucCartridgeNum << " ...");
			returnValue = m_tagAdapter->Logout();
			if (returnValue != IDTLIB_SUCCESS)
			{
				LOG_EXT(LEVEL_ERROR, "Error logging out from cartridge #"<< (unsigned short)ucCartridgeNum << " (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			LOG_EXT(LEVEL_INFO, "Setting ID certificate for cartridge #" << (unsigned short)ucCartridgeNum << " ended successfully.");
			return returnValue;
		}
		catch (exception& e)
		{
			LOG_EXT(LEVEL_ERROR, "Exception caught: " << e.what() << ".");
			return EXCEPTION_CAUGHT;
		}
	}

	int MaterialMonitor::GetDeviceSerialNumber(unsigned char ucCartridgeNum, unsigned char* aucSerialNumber, unsigned char* pucSerialNumberLength)
	{
		UniqueLock uniqueLock(m_mtx);

		int returnValue = IDTLIB_SUCCESS;

		if (!m_bInitialized)
		{
			returnValue = HW_NOT_INITIALIZED;
			LOG_EXT(LEVEL_ERROR, "Hardware not initialized (error code 0x" << hex << (short)returnValue << ").");
			return returnValue;
		}

		try
		{
			LOG_EXT(LEVEL_INFO, "Getting serial number of cartridge #" << (unsigned short)ucCartridgeNum << ":");
			LOG_EXT(LEVEL_INFO, "Switching to cartridge #"<< (unsigned short)ucCartridgeNum << "...");
			returnValue = m_tagAdapter->SwitchToCartridge(ucCartridgeNum);
			if (returnValue != IDTLIB_SUCCESS)
			{
				LOG_EXT(LEVEL_ERROR, "Error switching to cartridge #"<< (unsigned short)ucCartridgeNum << " (number of cartridges: " << (unsigned short)m_ucCartridgesCount << ", error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			LOG_EXT(LEVEL_INFO, "Reading serial number...");
			returnValue = m_tagAdapter->GetSerialNumber(aucSerialNumber, pucSerialNumberLength);
			if (returnValue != IDTLIB_SUCCESS)
			{
				LOG_EXT(LEVEL_ERROR, "Error reading serial number (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			char hexSerialNumber[32]; // max serial number length
			ByteArrayToHexString(aucSerialNumber, *pucSerialNumberLength, hexSerialNumber);
			LOG_EXT(LEVEL_INFO, "Serial number of cartridge #" << (unsigned short)ucCartridgeNum << " was read successfully. Serial number: '" << hexSerialNumber << "'");
			return returnValue;
		}
		catch (exception& e)
		{
			LOG_EXT(LEVEL_ERROR, "Exception caught: " << e.what() << ".");
			return EXCEPTION_CAUGHT;
		}
	}

	int MaterialMonitor::ReadTagPublicKey(unsigned char ucCartridgeNum, unsigned char* aucPubKC, unsigned short* pusKeyLength)
	{
		UniqueLock uniqueLock(m_mtx);

		int returnValue = IDTLIB_SUCCESS;

		if (!m_bInitialized)
		{
			returnValue = HW_NOT_INITIALIZED;
			LOG_EXT(LEVEL_ERROR, "Hardware not initialized (error code 0x" << hex << (short)returnValue << ").");
			return returnValue;
		}

		try
		{
			LOG_EXT(LEVEL_INFO, "Reading public key of cartridge #" << (unsigned short)ucCartridgeNum << ":");
			LOG_EXT(LEVEL_INFO, "Switching to cartridge #"<< (unsigned short)ucCartridgeNum << "...");
			returnValue = m_tagAdapter->SwitchToCartridge(ucCartridgeNum);
			if (returnValue != IDTLIB_SUCCESS)
			{
				LOG_EXT(LEVEL_ERROR, "Error switching to cartridge #"<< (unsigned short)ucCartridgeNum << " (number of cartridges: " << (unsigned short)m_ucCartridgesCount << ", error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			int returnValue = m_tagAdapter->ReadPubKC(aucPubKC, pusKeyLength);
			if (returnValue != IDTLIB_SUCCESS)
			{
				LOG_EXT(LEVEL_ERROR, "Error reading tag public key (error code 0x" << hex << (short)returnValue << ").");
			}

			return returnValue;
		}
		catch (exception& e)
		{
			LOG_EXT(LEVEL_ERROR, "Exception caught: " << e.what() << ".");
			return EXCEPTION_CAUGHT;
		}
	}

	int MaterialMonitor::BurnSignedIdd(unsigned char ucCartridgeNum, unsigned char* aucIdd, unsigned short usIddLength, unsigned char* aucSignature, unsigned short usSignatureLength)
	{
		UniqueLock uniqueLock(m_mtx);

		int returnValue = IDTLIB_SUCCESS;

		if (!m_bInitialized)
		{
			returnValue = HW_NOT_INITIALIZED;
			LOG_EXT(LEVEL_ERROR, "Hardware not initialized (error code 0x" << hex << (short)returnValue << ").");
			return returnValue;
		}

		try
		{
			LOG_EXT(LEVEL_INFO, "Getting serial numer of cartridge #" << (unsigned short)ucCartridgeNum << ":");
			LOG_EXT(LEVEL_INFO, "Switching to cartridge #"<< (unsigned short)ucCartridgeNum << "...");
			returnValue = m_tagAdapter->SwitchToCartridge(ucCartridgeNum);
			if (returnValue != IDTLIB_SUCCESS)
			{
				LOG_EXT(LEVEL_ERROR, "Error switching to cartridge #"<< (unsigned short)ucCartridgeNum << " (number of cartridges: " << (unsigned short)m_ucCartridgesCount << ", error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			LOG_EXT(LEVEL_INFO, "Writing IDD...");
			returnValue = m_tagAdapter->WriteMaterialInfo(aucIdd, usIddLength);
			if (returnValue != IDTLIB_SUCCESS)
			{
				LOG_EXT(LEVEL_ERROR, "Error writing IDD (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			LOG_EXT(LEVEL_INFO, "Writing signature...");
			returnValue = m_tagAdapter->WriteSignature(aucSignature, usSignatureLength);
			if (returnValue != IDTLIB_SUCCESS)
			{
				LOG_EXT(LEVEL_ERROR, "Error writing signature (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			LOG_EXT(LEVEL_INFO, "Signed IDD was written successfully into cartridge #" << (unsigned short)ucCartridgeNum << ".");
			return returnValue;
		}
		catch (exception& e)
		{
			LOG_EXT(LEVEL_ERROR, "Exception caught: " << e.what() << ".");
			return EXCEPTION_CAUGHT;
		}
	}

#endif

	int MaterialMonitor::ResetCartridgeInfo(unsigned char ucCartridgeNum)
	{
		if (ucCartridgeNum >= m_ucCartridgesCount)
		{
			return FCB_SELECT_CHANNEL_FAILED;
		}

		if (m_certificates != NULL &&
			m_certificates[ucCartridgeNum] != NULL)
		{
			delete m_certificates[ucCartridgeNum];
			m_certificates[ucCartridgeNum] = NULL;
		}

		return IDTLIB_SUCCESS;
	}

	/// <summary>
	/// Authenticates cartridge by verifying its certificate and its signature of a random token.
	/// </summary>
	/// <param name="ucCartridgeNum">The cartridge number.</param>
	/// <param name="aucPubKS">Stratasys public key.</param>
	/// <param name="aucIdd">The tag identification data (output parameter).</param>
	/// <param name="usIddLength">The tag identification data length (output parameter).</param>
	/// <param name="uiCurrentVolume">The tag current volume (output parameter).</param>
	/// <returns>0 on success, non-zero for failures.</returns>
	int MaterialMonitor::AuthenticateCartridge(unsigned char ucCartridgeNum, const unsigned char *aucPubKS, unsigned char *aucIdd, unsigned short* pusIddLength, unsigned int* puiCurrentVolume)
	{
		UniqueLock uniqueLock(m_mtx);

		int returnValue = IDTLIB_SUCCESS;
		if (aucPubKS == NULL && m_pubKS == NULL)
		{
			returnValue = INVALID_HOST_KEY;
			LOG_EXT(LEVEL_ERROR, "Invalid host key (error code 0x" << hex << (short)returnValue << ").");
			return returnValue;
		}

		if (aucIdd == NULL || pusIddLength == NULL || puiCurrentVolume == NULL)
		{
			returnValue = AUTHENTICATE_NULL_PARAMS;
			LOG_EXT(LEVEL_ERROR, "Invalid parameters (error code 0x" << hex << (short)returnValue << ").");
			return returnValue;
		}

		if (!m_bInitialized)
		{
			returnValue = HW_NOT_INITIALIZED;
			LOG_EXT(LEVEL_ERROR, "Hardware not initialized (error code 0x" << hex << (short)returnValue << ").");
			return returnValue;
		}

		try
		{
			LOG_EXT(LEVEL_INFO, "Cartridge #" << (unsigned short)ucCartridgeNum << " authentication started:");
			LOG_EXT(LEVEL_INFO, "Reset cartridge #"<< (unsigned short)ucCartridgeNum << " info...");
			int returnValue = ResetCartridgeInfo(ucCartridgeNum);
			if (returnValue != IDTLIB_SUCCESS)
			{
				LOG_EXT(LEVEL_ERROR, "Error resetting cartridge #"<< (unsigned short)ucCartridgeNum << " info (number of cartridges: " << (unsigned short)m_ucCartridgesCount << ", error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			LOG_EXT(LEVEL_INFO, "Switching to cartridge #"<< (unsigned short)ucCartridgeNum << "...");
			returnValue = m_tagAdapter->SwitchToCartridge(ucCartridgeNum);
			if (returnValue != IDTLIB_SUCCESS)
			{
				LOG_EXT(LEVEL_ERROR, "Error switching to cartridge #"<< (unsigned short)ucCartridgeNum << " (number of cartridges: " << (unsigned short)m_ucCartridgesCount << ", error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			LOG_EXT(LEVEL_INFO, "Reading identification data certificate...");
			unsigned char aucSignature[SIGNATURE_SIZE];
			unsigned short usSignatureLength;
			returnValue = ReadMaterialInfo(aucIdd, pusIddLength);
			if (returnValue == IDTLIB_SUCCESS)
			{
				returnValue = ReadSignature(aucSignature, &usSignatureLength);
			}

			if (returnValue != IDTLIB_SUCCESS)
			{
				if (returnValue == EMPTY_CERTIFICATE_FILE)
				{
					LOG_EXT(LEVEL_ERROR, "Tag is blank (error code 0x" << hex << (short)returnValue << ").");
					LOG_EXT(LEVEL_INFO, "Getting current volume...");
					GetCurrentVolume(puiCurrentVolume);
					return returnValue;
				}

				returnValue = INVALID_CERTIFICATE_FILE;
				LOG_EXT(LEVEL_ERROR, "Error identification data certificate (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			PubKey* pubKS;
			if (aucPubKS != NULL)
			{
				pubKS = LoadPublicKey(aucPubKS);
				if (pubKS == NULL)
				{
					return INVALID_HOST_KEY;
				}
			}
			else
			{
				pubKS = m_pubKS;
			}

			unsigned char aucBuffer[255];
			unsigned short usCertificateLength;
			m_tagAdapter->GenerateCertificate(aucIdd, *pusIddLength, aucBuffer, &usCertificateLength);
			memcpy(aucBuffer + usCertificateLength, aucSignature, usSignatureLength);

			LOG_EXT(LEVEL_INFO, "Verifying ID certificate...");
			SetVerifier(pubKS);
			pthread_t verifierThread;
			VerifyParameters* verifyParameters = CreateVerifyParameters(
				m_veriferS,
				aucBuffer,
				usCertificateLength,
				aucBuffer + usCertificateLength,
				usSignatureLength);
			CreateVerifierThread(&verifierThread, verifyParameters);

			LOG_EXT(LEVEL_INFO, "Sending random token for IDT to sign...");
			byte aucHostChallenge[TOKEN_SIZE];
			GenerateToken(aucHostChallenge, TOKEN_SIZE);

			byte aucDeviceChallenge[64]; // expected device challenge length
			byte aucSignedDeviceChallenge[SIGNATURE_SIZE];
			unsigned short usDeviceChallengeLength = sizeof(aucDeviceChallenge);
			unsigned short usSignedDeviceChallengeLength = sizeof(aucSignedDeviceChallenge);
			returnValue = ChallengeHostDevice(aucHostChallenge, TOKEN_SIZE, aucDeviceChallenge, &usDeviceChallengeLength, aucSignedDeviceChallenge, &usSignedDeviceChallengeLength);

			void* status;
			pthread_join(verifierThread, &status);
			delete verifyParameters;
			if (!m_bVerified)
			{
				returnValue = INVALID_CERTIFICATE_SIGNATURE;
				LOG_EXT(LEVEL_ERROR, "Error verifying ID certificate (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			if (returnValue != IDTLIB_SUCCESS)
			{
				LOG_EXT(LEVEL_ERROR, "Error sending random token to IDT (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			IdentificationData* certificate = new IdentificationData();
			certificate->Decode(aucBuffer);

			LOG_EXT(LEVEL_INFO, "Verifying IDT signature...");
			verifyParameters = CreateVerifyParameters(
				certificate->GetVerifier(),
				aucDeviceChallenge,
				usDeviceChallengeLength,
				aucSignedDeviceChallenge,
				usSignedDeviceChallengeLength);
			CreateVerifierThread(&verifierThread, verifyParameters);

			LOG_EXT(LEVEL_INFO, "Getting current volume...");
			returnValue = GetCurrentVolume(puiCurrentVolume);

			pthread_join(verifierThread, &status);
			delete verifyParameters;
			if (!m_bVerified)
			{
				returnValue = INVALID_TOKEN_SIGNATURE;
				LOG_EXT(LEVEL_ERROR, "Invalid signature (error code 0x" << hex << (short)returnValue << ").");
				delete certificate;
				return returnValue;
			}

			if (returnValue != IDTLIB_SUCCESS)
			{
				LOG_EXT(LEVEL_ERROR, "Error getting current volume (error code 0x" << hex << (short)returnValue << ").");
				delete certificate;
				return returnValue;
			}

			m_certificates[ucCartridgeNum] = certificate;
			LOG_EXT(LEVEL_INFO, "Cartridge #" << (unsigned short)ucCartridgeNum << " authentication ended successfully. Current weight: " << *puiCurrentVolume << " milligrams.");
			return returnValue;
		}
		catch (exception& e)
		{
			LOG_EXT(LEVEL_ERROR, "Exception caught: " << e.what() << ".");
			return EXCEPTION_CAUGHT;
		}
	}

	VerifyParameters* MaterialMonitor::CreateVerifyParameters(const Verif *verifier, const unsigned char *aucMessage, unsigned short usMessageLength, const unsigned char *aucSignedMessage, unsigned short usSignedMessageLength)
	{
		VerifyParameters* verifyParameters = new VerifyParameters;

		verifyParameters->Verifier = verifier;
		verifyParameters->Message = aucMessage;
		verifyParameters->MessageLength = usMessageLength;
		verifyParameters->SignedMessage = aucSignedMessage;
		verifyParameters->SignedMessageLength = usSignedMessageLength;
		verifyParameters->Verified = &(this->m_bVerified);

		return verifyParameters;
	}

	int MaterialMonitor::CreateVerifierThread(pthread_t* verifierThread, VerifyParameters* verifyParameters)
	{
		pthread_attr_t verifierThreadAttr;
		pthread_attr_init(&verifierThreadAttr);
		pthread_attr_setdetachstate(&verifierThreadAttr, PTHREAD_CREATE_JOINABLE);

		int rc = pthread_create(verifierThread, &verifierThreadAttr, ExecuteVerifyMessage, (void *)verifyParameters);
		pthread_attr_destroy(&verifierThreadAttr);
		return rc;
	}

	void MaterialMonitor::InitCertificates()
	{
		if (m_certificates != NULL)
		{
			delete[] m_certificates;
		}

		m_certificates = new IdentificationData*[m_ucCartridgesCount];
		for (unsigned char uc = 0; uc < m_ucCartridgesCount; uc++)
		{
			m_certificates[uc] = NULL;
		}
	}

	int MaterialMonitor::ReadMaterialInfo(unsigned char* aucMaterialInfo, unsigned short* pusMaterialInfoLength)
	{
		int returnValue = m_tagAdapter->ReadMaterialInfo(aucMaterialInfo, pusMaterialInfoLength);
		return returnValue;
	}

	int MaterialMonitor::ReadSignature(unsigned char* aucSignature, unsigned short* pusSignatureLength)
	{
		int returnValue = m_tagAdapter->ReadSignature(aucSignature, pusSignatureLength);
		return returnValue;
	}	

	/// <summary>
	/// Generates a random token.
	/// </summary>
	/// <param name="token">The token (output parameter).</param>
	/// <param name="blockSize">Size of the block.</param>
	void MaterialMonitor::GenerateToken(unsigned char *aucToken, unsigned int uiBlockSize)
	{
		AutoSeededRandomPool prng;
		prng.GenerateBlock(aucToken, uiBlockSize);
	}

	/// <summary>
	/// Send random token for signature.
	/// </summary>
	/// <param name="ucCartridgeNum">The cartridge number.</param>
	/// <param name="token">The token to sign.</param>
	/// <param name="tokenLength">Length of the token.</param>
	/// <param name="aucSignedToken">The signed token (output parameter).</param>
	/// <param name="usSignedTokenLength">Length of the signed token (output parameter).</param>
	/// <returns>0 on success, non-zero for failures.</returns>
	int MaterialMonitor::ChallengeHostDevice(const unsigned char* aucHostChallenge, unsigned short usHostChallengeLength, unsigned char* aucDeviceChallenge, unsigned short* pusDeviceChallengeLength, unsigned char* aucSignedDeviceChallenge, unsigned short* pusSignedDeviceChallengeLength)
	{
		int returnValue = m_tagAdapter->ChallengeHostDevice(aucHostChallenge, usHostChallengeLength, aucDeviceChallenge, pusDeviceChallengeLength, aucSignedDeviceChallenge, pusSignedDeviceChallengeLength);
		return returnValue;
	}

	/// <summary>
	/// Verifies cartridge material consumption, by decreasing volume to consume from cartridge current volume,
	/// checking if new value is equals to the expected value and verifying signature.
	/// </summary>
	/// <param name="ucCartridgeNum">The cartridge number.</param>
	/// <param name="uiComsumption">The material volume to consume.</param>
	/// <param name="uiNewVolume">Cartridge new volume.</param>
	/// <returns>0 on success, non-zero for failures.</returns>
	int MaterialMonitor::UpdateConsumption(unsigned char ucCartridgeNum, unsigned int uiComsumption, unsigned int *puiNewVolume, bool bSkipAuthentication)
	{
		int returnValue = IDTLIB_SUCCESS;
		if (puiNewVolume == NULL)
		{
			returnValue = CONSUMPTION_NULL_PARAMS;
			LOG_EXT(LEVEL_ERROR, "Invalid parameters (error code 0x" << hex << (short)returnValue << ").");
			return returnValue;
		}

		if (!bSkipAuthentication)
		{
			if (m_certificates == NULL ||
				m_certificates[ucCartridgeNum] == NULL)
			{
				returnValue = CARTRIDGE_NOT_AUTHENTICATED;
				LOG_EXT(LEVEL_ERROR, "Cartridge #"<< (unsigned short)ucCartridgeNum << " not authenticated (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}
		}

		CounterResponse counterResponse;
		returnValue = UpdateConsumption(ucCartridgeNum, uiComsumption, &counterResponse);
		if (returnValue != IDTLIB_SUCCESS)
		{
			return returnValue;
		}

		try
		{
			Verif* verifier = NULL;
			if (bSkipAuthentication)
			{
				try
				{
					unsigned char aucPubKC[2 * KEY_COEFF_SIZE];
					unsigned short usKeyLength;
					int returnValue = m_tagAdapter->ReadPubKC(aucPubKC, &usKeyLength);
					if (returnValue != IDTLIB_SUCCESS)
					{
						LOG_EXT(LEVEL_ERROR, "Error reading tag public key (error code 0x" << hex << (short)returnValue << ").");
					}

					EC2NPoint Q;
					Q.identity = false;
					Q.x.Decode(aucPubKC, usKeyLength / 2);
					Q.y.Decode(aucPubKC + usKeyLength / 2, usKeyLength / 2);
					PubKey pubKC;
					pubKC.Initialize(CURVE, Q);
					verifier = new Verif(pubKC);
				}
				catch (...)
				{
					returnValue = INVALID_VOLUME_SIGNATURE;
					LOG_EXT(LEVEL_ERROR, "Error verifying signature (error code 0x" << hex << (short)returnValue << ").");
					return returnValue;
				}
			}
			else
			{
				verifier = m_certificates[ucCartridgeNum]->GetVerifier();
			}

			LOG_EXT(LEVEL_INFO, "Verifying signature...");
			bool ok = VerifyVolume(verifier, counterResponse);
			if (!ok)
			{
				returnValue = INVALID_VOLUME_SIGNATURE;
				LOG_EXT(LEVEL_ERROR, "Error verifying signature (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			if (bSkipAuthentication)
			{
				delete verifier;
			}

			*puiNewVolume = counterResponse.GetCounterValue();
			LOG_EXT(LEVEL_INFO, "Cartridge #" << (unsigned short)ucCartridgeNum << " material consumption verified successfully.");
			return returnValue;
		}
		catch (exception& e)
		{
			LOG_EXT(LEVEL_ERROR, "Exception caught: " << e.what() << ".");
			return EXCEPTION_CAUGHT;
		}
	}

	int MaterialMonitor::UpdateConsumption(unsigned char ucCartridgeNum, unsigned int uiComsumption, CounterResponse* pCounterResponse)
	{
		UniqueLock uniqueLock(m_mtx);
		int returnValue = IDTLIB_SUCCESS;
		if (pCounterResponse == NULL)
		{
			returnValue = CONSUMPTION_NULL_PARAMS;
			LOG_EXT(LEVEL_ERROR, "Invalid parameters (error code 0x" << hex << (short)returnValue << ").");
			return returnValue;
		}

		if (!m_bInitialized)
		{
			returnValue = HW_NOT_INITIALIZED;
			LOG_EXT(LEVEL_ERROR, "Hardware not initialized (error code 0x" << hex << (short)returnValue << ").");
			return returnValue;
		}

		try
		{
			LOG_EXT(LEVEL_INFO, "Cartridge #" << (unsigned short)ucCartridgeNum << " material consumption started...");
			LOG_EXT(LEVEL_INFO, "Switching to cartridge #"<< (unsigned short)ucCartridgeNum << "...");
			int returnValue = m_tagAdapter->SwitchToCartridge(ucCartridgeNum);
			if (returnValue != IDTLIB_SUCCESS)
			{
				LOG_EXT(LEVEL_ERROR, "Error switching to cartridge #"<< (unsigned short)ucCartridgeNum << " (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			LOG_EXT(LEVEL_INFO, "Decreasing counter by " << uiComsumption << "...");
			returnValue = DecreaseVolume(uiComsumption, pCounterResponse);
			if (returnValue != IDTLIB_SUCCESS)
			{
				LOG_EXT(LEVEL_ERROR, "Error decreasing volume (error code 0x" << hex << (short)returnValue << ").");
				if (returnValue == MATERIAL_OVERCONSUMPTION)
				{
					DecreaseVolume(0, pCounterResponse);
				}

				return returnValue;
			}

			LOG_EXT(LEVEL_INFO, "Cartridge #" << (unsigned short)ucCartridgeNum << " material consumption ended successfully. Current weight: " << pCounterResponse->GetCounterValue() << " milligrams.");
			return returnValue;
		}
		catch (exception& e)
		{
			LOG_EXT(LEVEL_ERROR, "Exception caught: " << e.what() << ".");
			return EXCEPTION_CAUGHT;
		}
	}

	/// <summary>
	/// Gets current material volume of a cartridge.
	/// </summary>
	/// <param name="ucCartridgeNum">The cartridge number.</param>
	/// <param name="puiCurrentVolume">The current volume (output parameter).</param>
	/// <returns>0 on success, non-zero for failures.</returns>
	int MaterialMonitor::GetCurrentVolume(unsigned int *puiCurrentVolume)
	{
		int returnValue = m_tagAdapter->GetCurrentVolume(puiCurrentVolume);
		return returnValue;
	}

	/// <summary>
	/// Decreases material volume of a cartridge.
	/// </summary>
	/// <param name="ucCartridgeNum">The cartridge number.</param>
	/// <param name="consumption">The material volume to consume.</param>
	/// <param name="newSignedVolume">The new material volume, signed by cartridge private key.</param>
	/// <returns>0 on success, non-zero for failures.</returns>
	int MaterialMonitor::DecreaseVolume(unsigned int uiConsumption, CounterResponse *newSignedVolume)
	{
		int returnValue = m_tagAdapter->DecreaseConsumption(uiConsumption, newSignedVolume);
		return returnValue;
	}

	/// <summary>
	/// Verifies signature of new cartridge volume.
	/// </summary>
	/// <param name="PubKC">Cartridge verifier.</param>
	/// <param name="signedVolume">The signed volume.</param>
	/// <returns>0 on success, non-zero for failures.</returns>
	bool MaterialMonitor::VerifyVolume(const Verif *verifier, const CounterResponse &signedVolume)
	{
		unsigned char* aucRandomMessage = signedVolume.GetRandomMessage();
		unsigned char* aucSignature = signedVolume.GetSignature();
		unsigned short usSignatureLength = signedVolume.GetSignatureLength();
		bool ok = verifier->VerifyMessage(
			aucRandomMessage,
			RND_LENGTH,
			aucSignature,
			usSignatureLength);

		return ok;
	}

	/// <summary>
	/// Sets the verifier.
	/// </summary>
	/// <param name="pubKS">The verifyer public key.</param>
	void MaterialMonitor::SetVerifier(PubKey* pubKS)
	{
		bool changed = (m_veriferS == NULL);
		if (!changed)
		{
			EC2NPoint currentQ = m_veriferS->AccessKey().GetPublicElement(), newQ = pubKS->GetPublicElement();
			size_t bufLength = currentQ.x.ByteCount() + currentQ.y.ByteCount();
			byte *currentBuf = new byte[bufLength];
			byte *newBuf = new byte[bufLength];

			currentQ.x.Encode(currentBuf, currentQ.x.ByteCount());
			currentQ.y.Encode(currentBuf + currentQ.x.ByteCount(), currentQ.y.ByteCount());
			newQ.x.Encode(newBuf, newQ.x.ByteCount());
			newQ.y.Encode(newBuf + newQ.x.ByteCount(), newQ.y.ByteCount());

			changed = (memcmp(currentBuf, newBuf, bufLength) != 0);
			delete[] newBuf;
			delete[] currentBuf;
		}

		if (changed)
		{
			if (m_veriferS != NULL)
			{
				delete m_veriferS;
			}

			m_veriferS = new ECDSA<EC2N, SHA256>::Verifier(*pubKS);
		}
	}

	static void *ExecuteVerifyMessage(void* args)
	{
		VerifyParameters* executeVerifyParameters = new VerifyParameters;
		*executeVerifyParameters = *((VerifyParameters*)args);

		try
		{
			*executeVerifyParameters->Verified = executeVerifyParameters->Verifier->VerifyMessage(
				executeVerifyParameters->Message,
				executeVerifyParameters->MessageLength,
				executeVerifyParameters->SignedMessage,
				executeVerifyParameters->SignedMessageLength);
		}
		catch (exception& e)
		{
			LOG_EXT(LEVEL_ERROR, "Error verifying message: " << e.what() << ".");
			*executeVerifyParameters->Verified = false;
		}

		delete executeVerifyParameters;
		return NULL;
	}
}

IMaterialMonitor* CreateMaterialMonitor()
{
	return new StratasysIDTSrv::MaterialMonitor();
}
