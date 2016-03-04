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
		: BaseMaterialMonitor(), m_bVerified(false)
	{
		m_tagAdapter = IDTagAdapterFactory::CreateIDTagAdapter();
		m_ucCartridgesCount = 0;

		hasp_status_t status = HASP_SYS_ERR;

		/* login and initlialize vm engine */
		status = openAoC(&m_AoC_handle);
		if (status != HASP_STATUS_OK)
		{
			LOG_EXT(LEVEL_FATAL, "Error initializing VM engine (error code 0x" << hex << (short)status << ").");
		}


		LOG_EXT(LEVEL_INFO, "IDT-Srv initialized.");
	}

	/// <summary>
	/// Finalizes an instance of the <see cref="MaterialMonitor"/> class.
	/// </summary>
	MaterialMonitor::~MaterialMonitor()
	{
		delete m_tagAdapter;

		/* close VM and logout from HASP */
		closeAoC(m_AoC_handle);
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
			}

			LOG_EXT(LEVEL_DEBUG, "Status: 0x" << setfill('0') << setw(4) << hex << *pStatus << ".");
		}
		else
		{
			LOG_EXT(LEVEL_ERROR, "Error getting in-place status (error code 0x" << hex << (short)returnValue << ").");
		}

		return returnValue;
	}

	int MaterialMonitor::ResetCartridgeInfo(unsigned char ucCartridgeNum)
	{
		if (ucCartridgeNum >= m_ucCartridgesCount)
		{
			return FCB_SELECT_CHANNEL_FAILED;
		}

		return IDTLIB_SUCCESS;
	}

	/// <summary>
	/// Authenticates cartridge by verifying its certificate and its signature of a random token.
	/// </summary>
	/// <param name="ucCartridgeNum">The cartridge number.</param>
	/// <param name="isActive">Active or not tag.</param>
	/// <param name="aucIdd">The tag identification data (output parameter).</param>
	/// <param name="usIddLength">The tag identification data length (output parameter).</param>
	/// <param name="uiCurrentVolume">The tag current volume (output parameter).</param>
	/// <returns>0 on success, non-zero for failures.</returns>
	int MaterialMonitor::AuthenticateCartridge(unsigned char ucCartridgeNum, bool isActive, unsigned char *aucIdd, unsigned short* pusIddLength, unsigned int* puiCurrentVolume)
	{
		UniqueLock uniqueLock(m_mtx);
		hasp_status_t safenet_status;
		int result = 0;


		int returnValue = IDTLIB_SUCCESS;

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

			unsigned char aucBuffer[255];
			unsigned short usCertificateLength;

			m_tagAdapter->GenerateCertificate(aucIdd, *pusIddLength, aucBuffer, &usCertificateLength);

			const int messageSize = usCertificateLength; 
			const int signatureSizeOffset = messageSize;
			const int signatureSize = 72;
			const int signatureOffset = signatureSizeOffset + 2;

			memcpy(aucBuffer + signatureOffset, aucSignature, usSignatureLength);

			aucBuffer[signatureSizeOffset + 1] = signatureSize >> 8 & 0xFF;
			aucBuffer[signatureSizeOffset] = signatureSize & 0xFF;

			LOG_EXT(LEVEL_INFO, "Verifying ID certificate...");
			pthread_t verifierThread;
			unsigned char random[8];
			memset(random, 0, sizeof(random));

			// the following verifies: certificate generated by tag + receiving random for the next step
			VerifyParameters* verifyParameters = CreateVerifyParameters(
				m_AoC_handle,
				aucBuffer,
				usCertificateLength + usSignatureLength + 2,  
				isActive,
				ucCartridgeNum,
				runVerifyTag, 
				random); // output parameter returned from the dongle verifyTag

			ExecuteVerifyMessage(verifyParameters);

			if (!m_bVerified)
			{
				returnValue = INVALID_CERTIFICATE_SIGNATURE;
				LOG_EXT(LEVEL_ERROR, "Error verifying ID certificate (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			byte aucHostChallenge[TOKEN_SIZE];
			memcpy(aucHostChallenge, verifyParameters->random, TOKEN_SIZE);

			delete verifyParameters;

			LOG_EXT(LEVEL_INFO, "Sending random token for IDT to sign...");

			byte aucDeviceChallenge[64]; 
			byte aucSignedDeviceChallenge[SIGNATURE_SIZE];
			unsigned short usDeviceChallengeLength = sizeof(aucDeviceChallenge);
			unsigned short usSignedDeviceChallengeLength = sizeof(aucSignedDeviceChallenge);
			returnValue = ChallengeHostDevice(aucHostChallenge, TOKEN_SIZE, aucDeviceChallenge, &usDeviceChallengeLength, aucSignedDeviceChallenge, &usSignedDeviceChallengeLength);

			if (returnValue != IDTLIB_SUCCESS)
			{
				LOG_EXT(LEVEL_ERROR, "Error sending random token to IDT (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			unsigned char signedChallengeBuffer[255];
			const int mSize = usDeviceChallengeLength;  
			const int sigSiOffset = mSize;
			const int sigSize = 72;
			const int sigOffset = sigSiOffset + 2;

			memcpy(signedChallengeBuffer, aucDeviceChallenge, usDeviceChallengeLength);
			memcpy(signedChallengeBuffer + sigOffset, aucSignedDeviceChallenge, usSignedDeviceChallengeLength);

			signedChallengeBuffer[sigSiOffset + 1] = sigSize >> 8 & 0xFF;
			signedChallengeBuffer[sigSiOffset] = sigSize & 0xFF;

			LOG_EXT(LEVEL_INFO, "Verifying IDT signature...");

			// the following verfies: challenge from tag
			verifyParameters = CreateVerifyParameters(
				m_AoC_handle,
				signedChallengeBuffer,
				usDeviceChallengeLength+sigSize+2,
				isActive,
				ucCartridgeNum,
				runVerifyChallenge,   
				aucHostChallenge);

			ExecuteVerifyMessage(verifyParameters);

			delete verifyParameters;
			if (!m_bVerified)
			{
				returnValue = INVALID_TOKEN_SIGNATURE;
				LOG_EXT(LEVEL_ERROR, "Invalid signature (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			LOG_EXT(LEVEL_INFO, "Getting current volume...call UpdateConsumptionRedInteg for setting current volume to dongle ");

			returnValue = UpdateConsumptionRedInteg(ucCartridgeNum, 0, puiCurrentVolume);

			if (returnValue != IDTLIB_SUCCESS)
			{
				LOG_EXT(LEVEL_ERROR, "Error getting current volume (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			LOG_EXT(LEVEL_INFO, "Cartridge #" << (unsigned short)ucCartridgeNum << " authentication ended successfully. Current weight: " << *puiCurrentVolume << " milligrams.");
			return returnValue;
		}
		catch (exception& e)
		{
			LOG_EXT(LEVEL_ERROR, "Exception caught: " << e.what() << ".");
			return EXCEPTION_CAUGHT;
		}
	}

	/// <summary>
	/// Updates the consumption with safenet.
	/// </summary>
	/// <param name="ucCartridgeNum">The uc cartridge number.</param>
	/// <param name="consumption">The consumption.</param>
	/// <param name="puiCurrentVolume">The pui current volume (output parameter).</param>
	/// <returns></returns>
	int MaterialMonitor::UpdateConsumptionRedInteg(unsigned char ucCartridgeNum, int consumption, unsigned int *puiCurrentVolume)
	{
		hasp_status_t safenet_status;
		unsigned char random[8];
		int result=0;
		int returnValue = CARTRIDGE_NOT_AUTHENTICATED;
		*puiCurrentVolume = 0;

		memset(random, 0, sizeof(random));
		
		safenet_status = updateConsumption(m_AoC_handle, ucCartridgeNum, true, consumption, random, &result);

		if(safenet_status == HASP_STATUS_OK && result == 1)
		{
			byte signedWeight[102];  
			memset(signedWeight, 0, sizeof(signedWeight));

			CounterResponse counterResponse;

			returnValue = DecreaseVolume(consumption, &counterResponse, random);
			if (returnValue != IDTLIB_SUCCESS)
			{
				LOG_EXT(LEVEL_ERROR, "Error decreasing volume (error code 0x" << hex << (short)returnValue << ").");
				if (returnValue == MATERIAL_OVERCONSUMPTION)
				{
					DecreaseVolume(0, &counterResponse);
					*puiCurrentVolume = counterResponse.GetCounterValue();
				}
				return returnValue;
			}

			BuildSignedWeight(&counterResponse,signedWeight);

			safenet_status = verifyWeight(m_AoC_handle, ucCartridgeNum, true, signedWeight, sizeof(signedWeight), &result);
			if(safenet_status == HASP_STATUS_OK && result == 1)
			{
				returnValue = IDTLIB_SUCCESS;
				*puiCurrentVolume = counterResponse.GetCounterValue();
			}
		}

		return returnValue;
	}

	/// <summary>
	/// Builds the signed weight in the following structure:28 bytes data from tag + 2 bytes length + 72 bytes signature
	/// </summary>
	/// <param name="newSighedVolume">The new sighed volume.</param>
	/// <param name="signedWeight">The output signed weight. (output parameter)</param>
	void MaterialMonitor::BuildSignedWeight(CounterResponse *newSighedVolume,byte *signedWeight)
	{
		const int signatureLen = 72;
		const int signatureLenOffset = RND_LENGTH;
		const int signatureOffset = signatureLenOffset + 2;

		memcpy(signedWeight, newSighedVolume->GetRandomMessage(), RND_LENGTH);
		/* signatureLen is always 72 in 16 bit little endian */
		signedWeight[signatureLenOffset + 1] = (byte)(signatureLen >> 8);
		signedWeight[signatureLenOffset] = (byte)signatureLen;
		memcpy(signedWeight+signatureOffset, newSighedVolume->GetSignature(), signatureLen);
	}


	VerifyParameters* MaterialMonitor::CreateVerifyParameters(hasp_handle_t handle, const unsigned char *aucMessage, unsigned short usMessageLength,
		bool isActive, int tagNo, RunningMethod method, unsigned char* random)
	{
		VerifyParameters* verifyParameters = new VerifyParameters;

		verifyParameters->handle = handle;
		verifyParameters->Message = aucMessage;
		verifyParameters->MessageLength = usMessageLength;
		verifyParameters->Verified = &(this->m_bVerified);
		verifyParameters->isActive = isActive;
		verifyParameters->tagNo = tagNo;
		verifyParameters->method = method;
		verifyParameters->random = random;

		return verifyParameters;
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
		UniqueLock uniqueLock(m_mtx);
		int returnValue = IDTLIB_SUCCESS;
		if (puiNewVolume == NULL)
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

			LOG_EXT(LEVEL_INFO, "Decreasing volume by " << uiComsumption << "...");

			returnValue = UpdateConsumptionRedInteg(ucCartridgeNum, uiComsumption, puiNewVolume);

			if (returnValue == IDTLIB_SUCCESS)
			{
				LOG_EXT(LEVEL_INFO, "Cartridge #" << (unsigned short)ucCartridgeNum << " material consumption ended successfully. Current weight: " << *puiNewVolume << " milligrams.");
			}
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
	/// <param name="uiConsumption">The material volume to consume.</param>
	/// <param name="newSignedVolume">The new material volume, signed by cartridge private key.</param>
	/// <param name="random">Random array from Safenet</param>
	/// <returns>0 on success, non-zero for failures.</returns>
	int MaterialMonitor::DecreaseVolume(unsigned int uiConsumption, CounterResponse *newSignedVolume, unsigned char *random)
	{
		int returnValue = m_tagAdapter->DecreaseConsumption(uiConsumption, newSignedVolume, random);
		return returnValue;
	}

	/// <summary>
	/// Executes the verifyTag or verifyChallenge method.
	/// </summary>
	/// <param name="args">The arguments.</param>
	/// <returns></returns>
	static void *ExecuteVerifyMessage(void* args)
	{
		VerifyParameters* executeVerifyParameters = new VerifyParameters;
		*executeVerifyParameters = *((VerifyParameters*)args);

		hasp_status_t status = HASP_SYS_ERR;
		int result;

		try
		{
			if(executeVerifyParameters->method == runVerifyTag)
			{
				status = verifyTag(
					executeVerifyParameters->handle, 
					executeVerifyParameters->tagNo, 
					executeVerifyParameters->isActive,					
					executeVerifyParameters->Message,
					executeVerifyParameters->MessageLength,
					executeVerifyParameters->random,
					&result
					);
			}
			else if(executeVerifyParameters->method == runVerifyChallenge)
			{
				status = verifyChallenge(
					executeVerifyParameters->handle, 
					executeVerifyParameters->tagNo, 
					executeVerifyParameters->isActive,
					executeVerifyParameters->Message,
					executeVerifyParameters->MessageLength,
					&result
					);
			}

			if(status == HASP_STATUS_OK && result == 1)
			{
				*executeVerifyParameters->Verified = true;
			}
			else
			{
				*executeVerifyParameters->Verified = false;
			}
		}
		catch (exception& e)
		{
			LOG_EXT(LEVEL_ERROR, "Error verifying message: " << e.what() << ".");
			*executeVerifyParameters->Verified = false;
		}

		delete executeVerifyParameters;
		return NULL;
	}

	/// <summary>
	/// Resets cartridge information.
	/// </summary>
	/// <param name="ucCartridgeNum">The cartridge number.</param>
	/// <returns>0 on success, non-zero for failures.</returns>
	int MaterialMonitor::RemoveCartridge(unsigned char ucCartridgeNum)
	{
		hasp_status_t status;
		int returnValue;
		int result;

		returnValue = BaseMaterialMonitor::RemoveCartridge(ucCartridgeNum);

		if(returnValue == IDTLIB_SUCCESS)
		{
			status = removeTag(m_AoC_handle, ucCartridgeNum, &result);
			if(status == HASP_STATUS_OK && result == 1)
			{
				LOG_EXT(LEVEL_INFO, "Cartridge #" << (unsigned short)ucCartridgeNum << " was removed successfully from SafeNet.");
			}
			else
			{
				LOG_EXT(LEVEL_ERROR, "Cannot remove cartridge #" << (unsigned short)ucCartridgeNum << " from SafeNet. Cartridges count is " << (unsigned short)m_ucCartridgesCount << ".");
			}
		}
		return returnValue;
	}

	//ITAMAR RACCOON
	int MaterialMonitor::callCNOF(int SW, int SR, int LHO1200, int SRX, int IEF)
	{
		hasp_status_t status;
		int result;


		status = CNOF(m_AoC_handle, SW, SR, LHO1200, SRX, IEF, &result);
		if(status == HASP_STATUS_OK)
		{
			LOG_EXT(LEVEL_INFO, "CNOF executed.");
		}
		else
		{
			LOG_EXT(LEVEL_ERROR, "CNOF not executed");
		}
		return result;

	}


	int MaterialMonitor::callCEOP(int SOP, int SW, int SR, int LHO1200, int AF, int IEF)
	{
		hasp_status_t status;
		int result;

		status = CEOP(m_AoC_handle, SOP, SW, SR, LHO1200, AF, IEF, &result);
		if(status == HASP_STATUS_OK)
		{
			LOG_EXT(LEVEL_INFO, "CEOP executed.");
		}
		else
		{
			LOG_EXT(LEVEL_ERROR, "CEOP not executed");
		}
		return result;

	}

	int MaterialMonitor::callCNOP(int SH, int SST, int SYO, int HPW, int SPEO, int CH)
	{
		hasp_status_t status;
		int result;

		status = CNOP(m_AoC_handle, SH, SST, SYO, HPW, SPEO, CH, &result);
		if(status == HASP_STATUS_OK)
		{
			LOG_EXT(LEVEL_INFO, "CNOP executed.");
		}
		else
		{
			LOG_EXT(LEVEL_ERROR, "CNOP not executed");
		}
		return result;

	}

	int MaterialMonitor::callCYSO(int SH, int SST, int SYO, int HPW, int NGIP, int NOP, int SPEO, int YINOP, int YSINOP)
	{
		hasp_status_t status;
		int result;

		status = CYSO(m_AoC_handle, SH, SST, SYO, HPW, NGIP, NOP, SPEO, YINOP, YSINOP, &result);
		if(status == HASP_STATUS_OK)
		{
			LOG_EXT(LEVEL_INFO, "CYSO executed.");
		}
		else
		{
			LOG_EXT(LEVEL_ERROR, "CYSO not executed");
		}
		return result;

	}
}
	
IMaterialMonitor* CreateMaterialMonitor()
{
	return new StratasysIDTSrv::MaterialMonitor();
}

