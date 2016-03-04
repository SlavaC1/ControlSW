#include "stdafx.h"
#include "MaterialMonitor.h"
#include "IDTagAdapterFactory.h"
#include "UniqueLock.h"
#include "osrng.h"
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
#if (ENABLE_BURNING == 1)
		m_bCorrupt = false;
#endif

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
		UniqueLock uniqueLock(m_mtx);

		int returnValue = IDTLIB_SUCCESS;
		if (pucCartridgesCount == NULL)
		{
			returnValue = FCB_NULL_PARAM;
			LOG_EXT(LEVEL_FATAL, "Error initializing HW (error code 0x" << hex << (short)returnValue << ").");
			return returnValue;
		}

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
			if (m_certificates != NULL)
			{
				delete[] m_certificates;
			}

			m_certificates = new IDCertificate*[m_ucCartridgesCount];
		}

		for (unsigned char uc = 0; uc < m_ucCartridgesCount; uc++)
		{
			m_certificates[uc] = NULL;
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
			LOG_EXT(LEVEL_DEBUG, "Status: " "0x" << setfill('0') << setw(4) << hex << *pStatus << ".");
		}
		else
		{
			LOG_EXT(LEVEL_ERROR, "Error Getting in-place status (error code 0x" << hex << (short)returnValue << ").");
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
	int MaterialMonitor::BurnIDC(unsigned char ucCartridgeNum, const unsigned char *aucMaterialInfo, unsigned short usMaterialInfoLength, const unsigned char *aucPrvKS, const unsigned char *aucPubKS)
	{
		UniqueLock uniqueLock(m_mtx);

		int returnValue = IDTLIB_SUCCESS;
		bool areParametersValid = true;
		if (m_bCorrupt)
		{
			areParametersValid = (aucMaterialInfo != NULL) &&
				(aucPrvKS != NULL) &&
				((aucPubKS != NULL) || (m_pubKS != NULL));
		}
		else
		{
			areParametersValid = (aucMaterialInfo == NULL) ||
				((aucPrvKS != NULL) && ((aucPubKS != NULL) || (m_pubKS != NULL)));
		}

		if (!areParametersValid)
		{
			returnValue = BURN_NULL_PARAMS;
			LOG_EXT(LEVEL_ERROR, "Invalid parameters.");
			return returnValue;
		}

		try
		{
			LOG_EXT(LEVEL_INFO, "Setting ID certificate for cartridge #" << (unsigned short)ucCartridgeNum << " started:");
			LOG_EXT(LEVEL_INFO, "Switching to cartridge #"<< (unsigned short)ucCartridgeNum << "...");
			returnValue = m_tagAdapter->SwitchToCartridge(ucCartridgeNum);
			if (returnValue != IDTLIB_SUCCESS)
			{
				LOG_EXT(LEVEL_ERROR, "Error switching to cartridge #"<< (unsigned short)ucCartridgeNum << " (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			unsigned char aucCertificate[CERTIFICATE_SIZE];
			unsigned short usCertificateLength = CERTIFICATE_SIZE;
			if (m_bCorrupt)
			{
				ReadIDCertificate(aucCertificate);
			}

			LOG_EXT(LEVEL_INFO, "Logging in to cartridge #"<< (unsigned short)ucCartridgeNum << " ...");
			returnValue = m_tagAdapter->Login();
			if (returnValue != IDTLIB_SUCCESS)
			{
				LOG_EXT(LEVEL_ERROR, "Error logging in to cartridge #"<< (unsigned short)ucCartridgeNum << " (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			if (aucMaterialInfo != NULL)
			{
				LOG_EXT(LEVEL_INFO, "Configuring cartridge #"<< (unsigned short)ucCartridgeNum << "...");
				returnValue = m_tagAdapter->Configure();
				if (returnValue != IDTLIB_SUCCESS)
				{
					LOG_EXT(LEVEL_ERROR, "Error configuring to cartridge #"<< (unsigned short)ucCartridgeNum << " (error code 0x" << hex << (short)returnValue << ").");
					return returnValue;
				}

				unsigned char pubKS[PUBLIC_KEY_SIZE];
				if (aucPubKS != NULL)
				{
					returnValue = SetPubKS(aucPubKS);
					if (returnValue != IDTLIB_SUCCESS)
					{
						LOG_EXT(LEVEL_ERROR, "Error setting host public key (error code 0x" << hex << (short)returnValue << ").");
						return returnValue;
					}
				}

				try
				{
					m_pubKS->Save(ArraySink(pubKS, PUBLIC_KEY_SIZE).Ref());
				}
				catch (...)
				{
					returnValue = INVALID_HOST_KEY;
					LOG_EXT(LEVEL_ERROR, "Invalid host key.");
					return returnValue;
				}

				LOG_EXT(LEVEL_INFO, "Setting keys of cartridge #"<< (unsigned short)ucCartridgeNum << "...");
				returnValue = m_tagAdapter->SetKeys(pubKS);
				if (returnValue != IDTLIB_SUCCESS)
				{
					LOG_EXT(LEVEL_ERROR, "Error setting keys of cartridge #"<< (unsigned short)ucCartridgeNum << " (error code 0x" << hex << (short)returnValue << ").");
					return returnValue;
				}

				LOG_EXT(LEVEL_INFO, "Setting counter of cartridge #"<< (unsigned short)ucCartridgeNum << "...");
				MaterialInformation materialInfo;
				materialInfo.Decode(aucMaterialInfo);
				returnValue = m_tagAdapter->SetCounter((unsigned int)(materialInfo.InitialWeight) * 1000);
				if (returnValue != IDTLIB_SUCCESS)
				{
					LOG_EXT(LEVEL_ERROR, "Error setting counter of cartridge #"<< (unsigned short)ucCartridgeNum << " (error code 0x" << hex << (short)returnValue << ").");
					return returnValue;
				}

				if (m_bCorrupt)
				{
					LOG_EXT(LEVEL_INFO, "Modifying cartridge #"<< (unsigned short)ucCartridgeNum << " certificate...");
					memcpy(aucCertificate, aucMaterialInfo, usMaterialInfoLength);
				}
				else
				{
					LOG_EXT(LEVEL_INFO, "Generate cartridge #"<< (unsigned short)ucCartridgeNum << " certificate...");
					returnValue = m_tagAdapter->GenerateCertificate(aucMaterialInfo, usMaterialInfoLength, aucPrvKS, aucCertificate, &usCertificateLength);
					if (returnValue != IDTLIB_SUCCESS)
					{
						LOG_EXT(LEVEL_ERROR, "Error generating cartridge #"<< (unsigned short)ucCartridgeNum << " certificate (error code 0x" << hex << (short)returnValue << ").");
						return returnValue;
					}

					bool ok;
					LOG_EXT(LEVEL_INFO, "Verifying ID certificate...");
					try
					{
						SetVerifier(m_pubKS);
						ok = m_veriferS->VerifyMessage(aucCertificate, usCertificateLength - SIGNATURE_SIZE, aucCertificate + (usCertificateLength - SIGNATURE_SIZE), SIGNATURE_SIZE);
					}
					catch (exception& e)
					{
						LOG_EXT(LEVEL_ERROR, "Error verifying ID certificate (error code 0x" << hex << (short)returnValue << "): " << e.what() << ".");
						ok = false;
					}

					if (!ok)
					{
						returnValue = INVALID_CERTIFICATE_SIGNATURE;
						LOG_EXT(LEVEL_ERROR, "Error verifying ID certificate (error code 0x" << hex << (short)returnValue << ").");
						return returnValue;
					}
				}

				LOG_EXT(LEVEL_INFO, "Writing certificate into cartridge #"<< (unsigned short)ucCartridgeNum << " ...");
				returnValue = m_tagAdapter->WriteCertificate(aucCertificate, usCertificateLength);
				if (returnValue != IDTLIB_SUCCESS)
				{
					LOG_EXT(LEVEL_ERROR, "Error writing certificate into cartridge #"<< (unsigned short)ucCartridgeNum << " (error code 0x" << hex << (short)returnValue << ").");
					return returnValue;
				}
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

	int MaterialMonitor::EraseCartridge(unsigned char ucCartridgeNum)
	{
		return BurnIDC(ucCartridgeNum, NULL, 0, NULL, NULL);
	}

	int MaterialMonitor::ModifyIDC(unsigned char ucCartridgeNum, const unsigned char *aucMaterialInfo, unsigned short usMaterialInfoLength, const unsigned char *aucPrvKS, const unsigned char *aucPubKS)
	{
		m_bCorrupt = true;
		BurnIDC(ucCartridgeNum, aucMaterialInfo, usMaterialInfoLength, aucPrvKS, aucPubKS);
		m_bCorrupt = false;
		return 0;
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
	int MaterialMonitor::AuthenticateCartridge(unsigned char ucCartridgeNum, const unsigned char *aucPubKS, unsigned char *aucIdd, unsigned short *usIddLength, unsigned int *uiCurrentVolume)
	{
		UniqueLock uniqueLock(m_mtx);

		int returnValue = IDTLIB_SUCCESS;
		if (aucPubKS == NULL && m_pubKS == NULL)
		{
			returnValue = INVALID_HOST_KEY;
			LOG_EXT(LEVEL_ERROR, "Invalid host key (error code 0x" << hex << (short)returnValue << ").");
			return returnValue;
		}

		if (aucIdd == NULL || usIddLength == NULL || uiCurrentVolume == NULL)
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

			LOG_EXT(LEVEL_INFO, "Reading ID certificate...");
			unsigned char aucBuffer[CERTIFICATE_SIZE];
			memset(aucBuffer, 0, sizeof(aucBuffer));
			returnValue = ReadIDCertificate(aucBuffer);
			if (returnValue != IDTLIB_SUCCESS)
			{
				LOG_EXT(LEVEL_ERROR, "Error reading ID certificate (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			bool isEmpty = m_tagAdapter->IsCertificateEmpty(aucBuffer);
			if (isEmpty)
			{
				returnValue = EMPTY_CERTIFICATE_FILE;
				LOG_EXT(LEVEL_ERROR, "Tag is blank (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			LOG_EXT(LEVEL_DEBUG, "Extracting cartridge #" << (unsigned short)ucCartridgeNum << " public key...");
			IDCertificate *certificate = new IDCertificate();
			unsigned short certificateSize;
			try
			{
				certificateSize = (unsigned short)certificate->Decode(aucBuffer);
			}
			catch (exception& e)
			{
				delete certificate;
				returnValue = INVALID_CERTIFICATE_FILE;
				LOG_EXT(LEVEL_ERROR, "Invalid certificate (error code 0x" << hex << (short)returnValue << "): " << e.what() << ".");
				return returnValue;
			}

			*usIddLength = certificateSize - SIGNATURE_SIZE - PUBLIC_KEY_SIZE;
			memcpy(aucIdd, aucBuffer, *usIddLength);

			ECDSA<EC2N, SHA256>::PublicKey* pubKS;
			if (aucPubKS != NULL)
			{
				pubKS = LoadPublicKey(aucPubKS);
				if (pubKS == NULL)
				{
					delete certificate;
					return INVALID_HOST_KEY;
				}
			}
			else
			{
				pubKS = m_pubKS;
			}

			LOG_EXT(LEVEL_INFO, "Verifying ID certificate...");
			SetVerifier(pubKS);
			pthread_t verifierThread;
			VerifyParameters* verifyParameters = CreateVerifyParameters(
				m_veriferS,
				aucBuffer,
				certificateSize - SIGNATURE_SIZE,
				aucBuffer + (certificateSize - SIGNATURE_SIZE),
				SIGNATURE_SIZE);
			CreateVerifierThread(&verifierThread, verifyParameters);

			LOG_EXT(LEVEL_INFO, "Sending random token for IDT to sign...");
			byte aucHostChallenge[TOKEN_SIZE];
			GenerateToken(aucHostChallenge, TOKEN_SIZE);

			byte aucDeviceChallenge[64]; // expected device challenge length
			byte aucSignedDeviceChallenge[SIGNATURE_SIZE];
			unsigned short usDeviceChallengeLength = sizeof(aucDeviceChallenge);
			unsigned short usSignedDeviceChallengeLength = sizeof(aucSignedDeviceChallenge);
			returnValue = ChallengeHostDevice(ucCartridgeNum, aucHostChallenge, TOKEN_SIZE, aucDeviceChallenge, &usDeviceChallengeLength, aucSignedDeviceChallenge, &usSignedDeviceChallengeLength);

			void* status;
			pthread_join(verifierThread, &status);
			delete verifyParameters;
			if (!m_bVerified)
			{
				returnValue = INVALID_CERTIFICATE_SIGNATURE;
				LOG_EXT(LEVEL_ERROR, "Error verifying ID certificate (error code 0x" << hex << (short)returnValue << ").");
				delete certificate;
				return returnValue;
			}

			if (returnValue != IDTLIB_SUCCESS)
			{
				LOG_EXT(LEVEL_ERROR, "Error sending random token to IDT (error code 0x" << hex << (short)returnValue << ").");
				delete certificate;
				return returnValue;
			}

			LOG_EXT(LEVEL_INFO, "Verifying IDT signature...");
			verifyParameters = CreateVerifyParameters(
				certificate->IDD.GetVerifier(),
				aucDeviceChallenge,
				usDeviceChallengeLength,
				aucSignedDeviceChallenge,
				usSignedDeviceChallengeLength);
			CreateVerifierThread(&verifierThread, verifyParameters);

			LOG_EXT(LEVEL_INFO, "Getting current volume...");
			returnValue = GetCurrentVolume(ucCartridgeNum, uiCurrentVolume);

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
			LOG_EXT(LEVEL_INFO, "Cartridge #" << (unsigned short)ucCartridgeNum << " authentication ended successfully. Current weight: " << *uiCurrentVolume << " milligrams.");
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

	/// <summary>
	/// Gets the IDT certificate.
	/// </summary>
	/// <param name="ucCartridgeNum">The cartridge number.</param>
	/// <param name="certificate">The certificate (outout parameter).</param>
	/// <returns>0 on success, non-zero for failures.</returns>
	int MaterialMonitor::ReadIDCertificate(unsigned char *aucCertificate)
	{
		int returnValue = m_tagAdapter->ReadIDCertificate(aucCertificate);
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
	int MaterialMonitor::ChallengeHostDevice(unsigned char ucCartridgeNum, const unsigned char *aucHostChallenge, unsigned short usHostChallengeLength, byte *aucDeviceChallenge, unsigned short *pusDeviceChallengeLength, byte *aucSignedDeviceChallenge, unsigned short *pusSignedDeviceChallengeLength)
	{
		int returnValue = m_tagAdapter->ChallengeHostDevice(ucCartridgeNum, aucHostChallenge, usHostChallengeLength, aucDeviceChallenge, pusDeviceChallengeLength, aucSignedDeviceChallenge, pusSignedDeviceChallengeLength);
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
	int MaterialMonitor::UpdateConsumption(unsigned char ucCartridgeNum, unsigned int uiComsumption, unsigned int *uiNewVolume)
	{
		UniqueLock uniqueLock(m_mtx);

		int returnValue = IDTLIB_SUCCESS;
		if (uiNewVolume == NULL)
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

			if (m_certificates == NULL ||
				m_certificates[ucCartridgeNum] == NULL)
			{
				returnValue = CARTRIDGE_NOT_AUTHENTICATED;
				LOG_EXT(LEVEL_ERROR, "Cartridge #"<< (unsigned short)ucCartridgeNum << " not authenticated (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			LOG_EXT(LEVEL_INFO, "Decreasing volume by " << uiComsumption << "...");
			CounterResponse counterResponse;
			returnValue = DecreaseVolume(ucCartridgeNum, uiComsumption, &counterResponse);
			if (returnValue != IDTLIB_SUCCESS)
			{
				LOG_EXT(LEVEL_ERROR, "Error decreasing volume (error code 0x" << hex << (short)returnValue << ").");
				if (returnValue == MATERIAL_OVERCONSUMPTION)
				{
					DecreaseVolume(ucCartridgeNum, 0, &counterResponse);
					*uiNewVolume = counterResponse.GetCounterValue();
				}

				return returnValue;
			}

			LOG_EXT(LEVEL_INFO, "Verifying signature...");
			bool ok = VerifyVolume(m_certificates[ucCartridgeNum]->IDD.GetVerifier(), counterResponse);
			if (!ok)
			{
				returnValue = INVALID_VOLUME_SIGNATURE;
				LOG_EXT(LEVEL_ERROR, "Error verifying signature (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			*uiNewVolume = counterResponse.GetCounterValue();
			LOG_EXT(LEVEL_INFO, "Cartridge #" << (unsigned short)ucCartridgeNum << " material consumption ended successfully. Current weight: " << *uiNewVolume << " milligrams.");
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
	int MaterialMonitor::GetCurrentVolume(unsigned char ucCartridgeNum, unsigned int *puiCurrentVolume)
	{
		int returnValue = m_tagAdapter->GetCurrentVolume(ucCartridgeNum, puiCurrentVolume);
		return returnValue;
	}

	/// <summary>
	/// Decreases material volume of a cartridge.
	/// </summary>
	/// <param name="ucCartridgeNum">The cartridge number.</param>
	/// <param name="consumption">The material volume to consume.</param>
	/// <param name="newSignedVolume">The new material volume, signed by cartridge private key.</param>
	/// <returns>0 on success, non-zero for failures.</returns>
	int MaterialMonitor::DecreaseVolume(unsigned char ucCartridgeNum, unsigned int uiConsumption, CounterResponse *newSignedVolume)
	{
		int returnValue = m_tagAdapter->DecreaseConsumption(ucCartridgeNum, uiConsumption, newSignedVolume);
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
		bool ok = verifier->VerifyMessage(
			signedVolume.GetRandomMessage(),
			RND_LENGTH,
			signedVolume.GetSignature(),
			signedVolume.GetSignatureLength());

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
