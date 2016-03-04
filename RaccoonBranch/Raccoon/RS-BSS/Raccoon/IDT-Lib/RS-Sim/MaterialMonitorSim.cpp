#include "stdafx.h"
#include "MaterialMonitorSim.h"
#include "IDCertificate.h"
#include "RSCommonDefinitions.h"
#include "osrng.h"
#include "oids.h"
#include "BehaviorParser.h"
#include "UniqueLock.h"
#include "Utils.h"
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <math.h>
#include <errno.h>
#endif

#include <string>
#include <iomanip>
#include <time.h>
#include <sys/timeb.h>
#include <sys/types.h>

using namespace CryptoPP;
using namespace RSCommon;

namespace RSSim
{

#define SIMULATOR_FOLDER	"Simulator"
#define BEHAVIORS_FOLDER	"Behaviors"
#define TIMELINE_FILE		"timeline.xml"
#define CERTIFICATE_FOLDER	"Certificates"
#define CERTIFICATE_FILE	"cert.cer"
#define COUNTER_FILE		"counter.txt"

	void *LoadBehaviorTimeline(void* arg)
	{
		MaterialMonitorSim* materialMonitorSim = (MaterialMonitorSim*)arg;

		try
		{
			LOG_EXT(LEVEL_INFO, "Loading behavior timeline '.\\" << materialMonitorSim->GetTimelineFile() << "'...");

			XMLDocument doc;
			doc.LoadFile(materialMonitorSim->GetTimelineFile().c_str());

			XMLElement* root = doc.RootElement();
			if (root != NULL)
			{
				XMLElement* behaviorEl = root->FirstChildElement("Behavior");
				XMLElement* lastBehaviorEl = root->LastChildElement("Behavior");
				while (behaviorEl != NULL)
				{
					float interval = 0;
					behaviorEl->QueryFloatAttribute("Interval", &interval);
					LOG_EXT(LEVEL_INFO, "Waiting for behavior for " << interval << " seconds...");
					if (interval > 0)
					{
						// Wait for a specified interval by acquiring a lock;
						// if lock was acquired successfully, destructor was called - shutdown thread.
						timespec ts;
						timeb now;
						ftime(&now);
						ts.tv_sec = now.time + (int)floorf(interval);
						long nsec = now.millitm * 1000000 + (long)((interval - floorf(interval)) * 1000000000);
						ts.tv_sec += nsec / 1000000000;
						ts.tv_nsec = nsec % 1000000000;

						pthread_mutex_t simLifetimeMutex = materialMonitorSim->GetSimLifetimeMutex();
						pthread_cond_t simLifetimeCV = materialMonitorSim->GetSimLifetimeCV();
						pthread_mutex_lock(&simLifetimeMutex);
						int cvStatus = pthread_cond_timedwait(&simLifetimeCV, &simLifetimeMutex, &ts);
						bool timeout;
#ifdef WIN32
						timeout = (cvStatus == WSAETIMEDOUT);
#else
						timeout = (cvStatus == ETIMEDOUT);
#endif

						pthread_mutex_unlock(&simLifetimeMutex);
						if (!timeout)
						{
							break;
						}
					}
					LoadBehavior(materialMonitorSim, behaviorEl->GetText());

					behaviorEl = (behaviorEl == lastBehaviorEl) ? NULL : behaviorEl->NextSiblingElement();
				}

				LOG_EXT(LEVEL_INFO, "Behavior timeline loaded successfully.");
			}
			else
			{
				LOG_EXT(LEVEL_WARN, "Cannot find file root.");
			}
		}
		catch (...)
		{
			LOG_EXT(LEVEL_WARN, "Invalid timeline.");
		}

		return NULL;
	}

	void LoadBehavior(void* arg, const char* fileName)
	{
		MaterialMonitorSim* materialMonitorSim = (MaterialMonitorSim*)arg;
		UniqueLock(materialMonitorSim->GetMutex());

		string path(SIMULATOR_FOLDER);
		path.append(1, PATH_SEPARATOR);
		path.append(BEHAVIORS_FOLDER);
		path.append(1, PATH_SEPARATOR);
		path.append(fileName);
		Behavior* behavior = BehaviorParser::ParseBehavior(path.c_str());
		materialMonitorSim->SetBehavior(behavior);
		if (behavior != NULL)
		{
			LOG_EXT(LEVEL_INFO, "Behavior '" << fileName << "' was loaded.");
		}
		else
		{
			LOG_EXT(LEVEL_WARN, "Error loading behavior '" << fileName << "': invalid file.");
		}
	}

	int MaterialMonitorSim::ReadCounter(unsigned char ucCartridgeNum, unsigned int *uiCurrentVolume)
	{
		//*uiCurrentVolume = rand() % 1000;
		//return IDTLIB_SUCCESS;
		string sCounterFileName = GetCartridgeFileName(ucCartridgeNum, COUNTER_FILE);

		// read counter
		FILE *pCounterFile = fopen(sCounterFileName.c_str(), "r");
		if (pCounterFile == NULL)
		{
			return INVALID_VOLUME_SIGNATURE;
		}

		fseek(pCounterFile , 0 , SEEK_END);
		size_t fileSize = (size_t)ftell(pCounterFile);
		rewind(pCounterFile);
		char acCurrentVolume[10];
		fgets(acCurrentVolume, 10, pCounterFile);

		char* pcEnd;
		*uiCurrentVolume = (unsigned int)strtol(acCurrentVolume, &pcEnd, 10);
		int returnValue = (*pcEnd == 0) ? IDTLIB_SUCCESS : INVALID_VOLUME_SIGNATURE;

		return returnValue;
	}

	MaterialMonitorSim::MaterialMonitorSim(void)
		: BaseMaterialMonitor(), m_authenticated(0), m_behavior(NULL)
	{
		pthread_mutex_init(&m_simLifetimeMutex, NULL);
		pthread_cond_init(&m_simLifetimeCV, NULL);

		m_timelineFile = SIMULATOR_FOLDER;
		m_timelineFile.append(1, PATH_SEPARATOR);
		m_timelineFile.append(BEHAVIORS_FOLDER);
		m_timelineFile.append(1, PATH_SEPARATOR);
		m_timelineFile.append(TIMELINE_FILE);

		pthread_attr_init(&m_thLoadBehaviorTimelineAttr);
		pthread_attr_setdetachstate(&m_thLoadBehaviorTimelineAttr, PTHREAD_CREATE_JOINABLE);
		pthread_create(&m_thLoadBehaviorTimeline, NULL, LoadBehaviorTimeline, this);

		LOG_EXT(LEVEL_INFO, "RS-Sim initialized.");
	}

	MaterialMonitorSim::~MaterialMonitorSim(void)
	{
		pthread_cond_signal(&m_simLifetimeCV);

		pthread_attr_destroy(&m_thLoadBehaviorTimelineAttr);
		void* status;
		pthread_join(m_thLoadBehaviorTimeline, &status);
		pthread_cond_destroy(&m_simLifetimeCV);
		pthread_mutex_destroy(&m_simLifetimeMutex);

		delete m_behavior;
	}

	int MaterialMonitorSim::InitHW(char* pcHsmTarget, unsigned short pcHsmTargetLength, unsigned char *pucCartridgesCount)
	{
		int returnValue = BaseMaterialMonitor::InitHW(useHsmSimulator, pucCartridgesCount);
		if (returnValue != IDTLIB_SUCCESS)
		{
			return returnValue;
		}

		UniqueLock uniqueLock(m_mtx);

		m_bInitialized = false;
		if (m_behavior == NULL)
		{
			m_ucCartridgesCount = MAX_CARTRIDGES_COUNT;
		}
		else
		{
			m_ucCartridgesCount = m_behavior->CartridgesCount;
		}

		*pucCartridgesCount = m_ucCartridgesCount;
		m_bInitialized = true;
		LOG_EXT(LEVEL_INFO, "HW initialized successfully. Number of cartridges: " << (unsigned short)(*pucCartridgesCount) << ".");
		return IDTLIB_SUCCESS;
	}

	int MaterialMonitorSim::GetInPlaceStatus(int *pStatus)
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

		LOG_EXT(LEVEL_INFO, "Getting in-place status:");
		if (m_behavior == NULL)
		{
			*pStatus = (1 << m_ucCartridgesCount) - 1;
		}
		else
		{
			*pStatus = m_behavior->Status;
		}

		LOG_EXT(LEVEL_INFO, "Status: 0x" << setfill('0') << setw(8) << hex << *pStatus << ".");
		return returnValue;
	}

#if (ENABLE_BURNING == 1)

	///
	/// Sets a certificate into a specified IDT.
	/// This method is available on test only.
	///
	int MaterialMonitorSim::BurnIDC(unsigned char ucCartridgeNum, const unsigned char *aucMaterialInfo, unsigned short usMaterialInfoLength, unsigned char* aucCertificate, unsigned short* usCertificateLength)
	{
		UniqueLock uniqueLock(m_mtx);

		try
		{
			int returnValue = 0;
			int behaviorReturnValue = GetBurnIDCBehavior(ucCartridgeNum, aucMaterialInfo, usMaterialInfoLength);

			LOG_EXT(LEVEL_INFO, "Setting ID certificate for cartridge #" << (unsigned short)ucCartridgeNum << " started:");
			LOG_EXT(LEVEL_INFO, "Switching to cartridge #"<< (unsigned short)ucCartridgeNum << "...");
			if ((m_behavior != NULL && !IsCartridgeOn(ucCartridgeNum, m_behavior->Status)) ||
				behaviorReturnValue == FCB_SELECT_CHANNEL_FAILED)
			{
				returnValue = FCB_SELECT_CHANNEL_FAILED;
				LOG_EXT(LEVEL_ERROR, "Error switching to cartridge #"<< (unsigned short)ucCartridgeNum << " (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			LOG_EXT(LEVEL_INFO, "Signing material information...");

			// Decode material info:
			unsigned char* ucCertificate = new unsigned char[CERTIFICATE_SIZE];
			MaterialInformation materialInfo;
			materialInfo.Decode(aucMaterialInfo);

			// Add serial number
			IdentificationData idd;
			idd.MaterialInfo = materialInfo;
			memset(idd.SerialNo, 0, SERIAL_NO_SIZE);
			idd.SerialNo[0] = ucCartridgeNum;

			// Generate public key and append public element:
			AutoSeededRandomPool prng;
			ECDSA<EC2N, SHA256>::PrivateKey prvKC;
			prvKC.Initialize(prng, ASN1::sect283k1());
			ECDSA<EC2N, SHA256>::PublicKey pubKC;
			prvKC.MakePublicKey(pubKC);
			idd.SetCartridgePublicKey(pubKC);

			size_t offset = idd.Encode(ucCertificate);

			unsigned char signature[SIGNATURE_SIZE];
			size_t signatureLength = m_hsmClient->SignMessage(ucCertificate, offset, signature);
			if (signatureLength == -1)
			{
				returnValue = INVALID_HOST_KEY;
			}			

			if (returnValue == INVALID_HOST_KEY || behaviorReturnValue == INVALID_HOST_KEY)
			{
				LOG_EXT(LEVEL_ERROR, "Invalid host key (error code 0x" << hex << (short)returnValue << ").");
				delete ucCertificate;
				return returnValue;
			}

			memcpy(ucCertificate + offset, signature, signatureLength);
			offset += signatureLength;

			string sCertificateFileName = GetCartridgeFileName(ucCartridgeNum, CERTIFICATE_FILE);

			// create directory:
			string sCertDirectory = sCertificateFileName.substr(0, sCertificateFileName.rfind(PATH_SEPARATOR) + 1);
#ifdef _WIN32
			_mkdir(sCertDirectory.c_str());
#else
			mkdir(sCertDirectory.c_str(), S_IRWXU);
#endif
			FILE* certFile = fopen(sCertificateFileName.c_str(), "wb");
			fwrite(ucCertificate, sizeof(char), offset, certFile);
			fclose(certFile);

			// create counter file:
			LOG_EXT(LEVEL_INFO, "Writing counter file...");
			WriteCounter(ucCartridgeNum, idd.MaterialInfo.InitialWeight);
			delete[] ucCertificate;

			returnValue = behaviorReturnValue;
			if (returnValue == 0)
			{
				LOG_EXT(LEVEL_INFO, "Setting ID certificate for cartridge #" << (unsigned short)ucCartridgeNum << " ended successfully.");
			}
			else
			{
				LOG_EXT(LEVEL_ERROR, "Error setting ID certificate (error code 0x" << hex << (short)returnValue << ").");
			}

			return returnValue;
		}
		catch (exception& e)
		{
			LOG_EXT(LEVEL_ERROR, "Exception caught: " << e.what() << ".");
			return EXCEPTION_CAUGHT;
		}
	}

	int MaterialMonitorSim::GetBurnIDCBehavior(unsigned char ucCartridgeNum, const unsigned char *aucMaterialInfo, unsigned short usMaterialInfoLength)
	{
		if (m_behavior == NULL)
		{
			return 0;
		}

		vector< pair<string, string> > parameters;
		ostringstream ossCartridgeNum;
		ossCartridgeNum << (unsigned short)ucCartridgeNum;
		parameters.push_back(pair<string, string>("CartridgeNum", ossCartridgeNum.str()));

		char *buffer = new char[2 * usMaterialInfoLength + 1];
		ByteArrayToHexString(aucMaterialInfo, usMaterialInfoLength, buffer);
		parameters.push_back(pair<string, string>("MaterialInfo", buffer));

		ostringstream ossMaterialInfoLength;
		ossMaterialInfoLength << usMaterialInfoLength;
		parameters.push_back(pair<string, string>("MaterialInfoLength", ossMaterialInfoLength.str()));
		delete[] buffer;

		return m_behavior->GetReturnValue("BurnIDC", parameters);
	}

#endif

	int MaterialMonitorSim::ResetCartridgeInfo(unsigned char ucCartridgeNum)
	{
		if (ucCartridgeNum >= m_ucCartridgesCount)
		{
			LOG_EXT(LEVEL_ERROR, "Cannot select cartridge #" << (unsigned short)ucCartridgeNum << ": cartridges count is " << (unsigned short)m_ucCartridgesCount << ".");
			return FCB_SELECT_CHANNEL_FAILED;
		}

		m_authenticated &= ~(1 << ucCartridgeNum);
		LOG_EXT(LEVEL_INFO, "Cartridge #" << (unsigned short)ucCartridgeNum << " was removed successfully.");
		return IDTLIB_SUCCESS;
	}

	///
	/// Authenticates cartridge.
	///
	int MaterialMonitorSim::AuthenticateCartridge(unsigned char ucCartridgeNum, const unsigned char *aucPubKS, unsigned char *aucIdd, unsigned short *usIddLength, unsigned int *uiCurrentVolume)
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
			int behaviorReturnValue = GetAuthenticateCartridgeBehavior(ucCartridgeNum, aucPubKS, aucIdd, usIddLength, uiCurrentVolume);

			LOG_EXT(LEVEL_INFO, "Cartridge #" << (unsigned short)ucCartridgeNum << " authentication started:");
			LOG_EXT(LEVEL_INFO, "Switching to cartridge #"<< (unsigned short)ucCartridgeNum << "...");
			if (((m_behavior != NULL) && !IsCartridgeOn(ucCartridgeNum, m_behavior->Status)) ||
				behaviorReturnValue == FCB_SELECT_CHANNEL_FAILED)
			{
				returnValue = FCB_SELECT_CHANNEL_FAILED;
				LOG_EXT(LEVEL_ERROR, "Error switching to cartridge #"<< (unsigned short)ucCartridgeNum << " (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			ResetCartridgeInfo(ucCartridgeNum);

			// read certificate file
			LOG_EXT(LEVEL_INFO, "Getting ID certificate...");
			string sCertificateFileName = GetCartridgeFileName(ucCartridgeNum, CERTIFICATE_FILE);

			FILE *pCertificateFile = fopen(sCertificateFileName.c_str(), "rb");
			if (pCertificateFile == NULL || behaviorReturnValue == INVALID_CERTIFICATE_FILE)
			{
				returnValue = INVALID_CERTIFICATE_FILE;
				LOG_EXT(LEVEL_ERROR, "Error getting ID certificate (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			fseek(pCertificateFile , 0 , SEEK_END);
			size_t fileSize = (size_t)ftell(pCertificateFile);
			rewind(pCertificateFile);
			fread(aucIdd, sizeof(unsigned char), fileSize, pCertificateFile);

			IDCertificate idc;
			*usIddLength = (unsigned short)idc.Decode(aucIdd) - SIGNATURE_SIZE;

			ECDSA<EC2N, SHA256>::PublicKey* pubKS;
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

			if (returnValue == INVALID_HOST_KEY || behaviorReturnValue == INVALID_HOST_KEY)
			{
				returnValue = INVALID_HOST_KEY;
				LOG_EXT(LEVEL_ERROR, "Invalid host key (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			// verify signature:
			ECDSA<EC2N, SHA256>::Verifier verifier(*pubKS);
			bool ok;
			try
			{
				ok = verifier.VerifyMessage(aucIdd, *usIddLength, aucIdd + *usIddLength, SIGNATURE_SIZE);
			}
			catch (exception& e)
			{
				LOG_EXT(LEVEL_ERROR, "Error verifying ID certificate (error code 0x" << hex << (short)returnValue << "):" << e.what() << ".");
				ok = false;
			}

			if (!ok || behaviorReturnValue == INVALID_CERTIFICATE_SIGNATURE)
			{
				returnValue = INVALID_CERTIFICATE_SIGNATURE;
				LOG_EXT(LEVEL_ERROR, "Error verifying ID certificate (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			// read counter
			returnValue = ReadCounter(ucCartridgeNum, uiCurrentVolume);
			if (returnValue != IDTLIB_SUCCESS)
			{
				if (returnValue == INVALID_VOLUME_SIGNATURE || behaviorReturnValue == INVALID_VOLUME_SIGNATURE)
				{
					LOG_EXT(LEVEL_ERROR, "Error verifying signature (error code 0x" << hex << (short)returnValue << ").");
				}
				else
				{
					LOG_EXT(LEVEL_ERROR, "Error reading counter (error code 0x" << hex << (short)returnValue << ").");
				}

				return returnValue;
			}

			returnValue = behaviorReturnValue;
			if (returnValue == IDTLIB_SUCCESS)
			{
				m_authenticated |= (1 << ucCartridgeNum);
				LOG_EXT(LEVEL_INFO, "Cartridge #" << (unsigned short)ucCartridgeNum << " authentication ended successfully. Current volume: " << *uiCurrentVolume << ".");
			}
			else
			{
				LOG_EXT(LEVEL_ERROR, "Error authenticating cartridge (error code 0x" << hex << (short)returnValue << ").");
			}

			return returnValue;
		}
		catch (exception& e)
		{
			LOG_EXT(LEVEL_ERROR, "Exception caught: " << e.what() << ".");
			return EXCEPTION_CAUGHT;
		}
	}

	int MaterialMonitorSim::GetAuthenticateCartridgeBehavior(unsigned char ucCartridgeNum, const unsigned char *aucPubKS, unsigned char *aucIdd, unsigned short *usIddLength, unsigned int *uiCurrentVolume)
	{
		if (m_behavior == NULL)
		{
			return IDTLIB_SUCCESS;
		}

		vector< pair<string, string> > parameters;
		ostringstream oss;
		oss << ucCartridgeNum;
		parameters.push_back(pair<string, string>("CartridgeNum", oss.str()));

		char *buffer = new char[2 * PUBLIC_KEY_SIZE + 1];
		ByteArrayToHexString(aucPubKS, PUBLIC_KEY_SIZE, buffer);
		parameters.push_back(pair<string, string>("PubKS", buffer));
		delete[] buffer;

		return m_behavior->GetReturnValue("AuthenticateCartridge", parameters);
	}

	///
	/// Verifies material consumption.
	//
	int MaterialMonitorSim::UpdateConsumption(unsigned char ucCartridgeNum, unsigned int uiComsumption, unsigned int *uiNewVolume)
	{
		UniqueLock uniqueLock(m_mtx);

		try
		{
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

			int behaviorReturnValue = GetUpdateConsumptionBehavior(ucCartridgeNum, uiComsumption, uiNewVolume);

			LOG_EXT(LEVEL_INFO, "Cartridge #" << (unsigned short)ucCartridgeNum << " material consumption started...");
			if (!IsCartridgeOn(ucCartridgeNum, m_authenticated) ||
				behaviorReturnValue == CARTRIDGE_NOT_AUTHENTICATED)
			{
				returnValue = CARTRIDGE_NOT_AUTHENTICATED;
				LOG_EXT(LEVEL_ERROR, "Cartridge #"<< (unsigned short)ucCartridgeNum << " not authenticated (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			LOG_EXT(LEVEL_INFO, "Switching to cartridge #"<< (unsigned short)ucCartridgeNum << "...");
			if (((m_behavior != NULL) && !IsCartridgeOn(ucCartridgeNum, m_behavior->Status)) ||
				behaviorReturnValue == FCB_SELECT_CHANNEL_FAILED)
			{
				returnValue = FCB_SELECT_CHANNEL_FAILED;
				LOG_EXT(LEVEL_ERROR, "Error switching to cartridge #"<< (unsigned short)ucCartridgeNum << " (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			LOG_EXT(LEVEL_INFO, "Getting current volume...");
			returnValue = ReadCounter(ucCartridgeNum, uiNewVolume);
			if (returnValue != IDTLIB_SUCCESS)
			{
				if (returnValue == INVALID_VOLUME_SIGNATURE || behaviorReturnValue == INVALID_VOLUME_SIGNATURE)
				{
					LOG_EXT(LEVEL_ERROR, "Error verifying signature (error code 0x" << hex << (short)returnValue << ").");
				}
				else
				{
					LOG_EXT(LEVEL_ERROR, "Error reading counter (error code 0x" << hex << (short)returnValue << ").");
				}

				return returnValue;
			}

			if (*uiNewVolume < uiComsumption || behaviorReturnValue == MATERIAL_OVERCONSUMPTION)
			{
				returnValue = MATERIAL_OVERCONSUMPTION;
				LOG_EXT(LEVEL_ERROR, "Invalid consumption (error code 0x" << hex << (short)returnValue << ").");
				return returnValue;
			}

			returnValue = behaviorReturnValue;
			if (returnValue == IDTLIB_SUCCESS)
			{
				LOG_EXT(LEVEL_INFO, "Decreasing volume by " << uiComsumption << "...");
				*uiNewVolume -= uiComsumption;
				WriteCounter(ucCartridgeNum, *uiNewVolume);
				LOG_EXT(LEVEL_INFO, "Cartridge #" << (unsigned short)ucCartridgeNum << " material consumption ended successfully. Current volume: " << *uiNewVolume << ".");
			}
			else
			{
				LOG_EXT(LEVEL_ERROR, "Error updating consumption (error code 0x" << hex << (short)returnValue << ").");
			}

			return returnValue;
		}
		catch (exception& e)
		{
			LOG_EXT(LEVEL_ERROR, "Exception caught: " << e.what() << ".");
			return EXCEPTION_CAUGHT;
		}
	}

	string MaterialMonitorSim::GetTimelineFile() const
	{
		return m_timelineFile;
	}

	pthread_cond_t MaterialMonitorSim::GetSimLifetimeCV() const
	{
		return m_simLifetimeCV;
	}

	pthread_mutex_t MaterialMonitorSim::GetSimLifetimeMutex() const
	{
		return m_simLifetimeMutex;
	}

	Behavior* MaterialMonitorSim::GetBehavior() const
	{
		return m_behavior;
	}

	void MaterialMonitorSim::SetBehavior(Behavior* behavior)
	{
		if (m_behavior != NULL)
		{
			delete m_behavior;
		}

		m_behavior = behavior;
	}

	int MaterialMonitorSim::GetUpdateConsumptionBehavior(unsigned char ucCartridgeNum, unsigned int uiComsumption, unsigned int *uiNewVolume)
	{
		if (m_behavior == NULL)
		{
			return IDTLIB_SUCCESS;
		}

		vector< pair<string, string> > parameters;
		ostringstream oss;
		oss << ucCartridgeNum;
		parameters.push_back(pair<string, string>("CartridgeNum", oss.str()));
		oss.clear();
		oss << uiComsumption;
		parameters.push_back(pair<string, string>("Consumption", oss.str()));

		return m_behavior->GetReturnValue("UpdateConsumption", parameters);
	}

	int MaterialMonitorSim::WriteCounter(unsigned char ucCartridgeNum, unsigned int uiNewVolume)
	{
		string sCounterFileName = GetCartridgeFileName(ucCartridgeNum, COUNTER_FILE);

		FILE* pCounterFile = fopen(sCounterFileName.c_str(), "w");

		ostringstream oss;
		oss << uiNewVolume;
		string sNewVolume = oss.str();
		fputs(sNewVolume.c_str(), pCounterFile);
		fclose(pCounterFile);

		return IDTLIB_SUCCESS;
	}

	string MaterialMonitorSim::GetCartridgeFileName(unsigned char ucCartridgeNum, const char* aucBaseFileName)
	{
		string sCartridgeFileName(SIMULATOR_FOLDER);
		sCartridgeFileName.append(1, PATH_SEPARATOR);
		sCartridgeFileName.append(CERTIFICATE_FOLDER);
		sCartridgeFileName.append(1, PATH_SEPARATOR);
		sCartridgeFileName.append(aucBaseFileName);

		ostringstream oss;
		oss << ucCartridgeNum;
		sCartridgeFileName.insert(sCartridgeFileName.rfind('.'), oss.str());

		return sCartridgeFileName;
	}

	bool MaterialMonitorSim::IsCartridgeOn(unsigned char ucCartridgeNum, int status)
	{
		return (status & (1 << ucCartridgeNum)) == (1 << ucCartridgeNum);
	}
}

IMaterialMonitor* CreateMaterialMonitor()
{
	return new RSSim::MaterialMonitorSim();
}
