#include "stdafx.h"
#include "TestManager.h"
#include "TestParser.h"
#include "IMaterialMonitor.h"
#include "IDCertificate.h"
#include "Utils.h"
#include "UniqueLock.h"

#include "oids.h"
#include "osrng.h"

#ifdef SIMULATOR
#include "RS-Sim/MaterialMonitorSim.h"
#else
#include "IDT-Srv/MaterialMonitor.h"
#endif

#include "log4cplus/configurator.h"
#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

#include <fstream>
#include <iostream>
#include <time.h>
#include <vector>
#include <pthread.h>
#include <sstream>
#include <sys/timeb.h>
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

using namespace std;
using namespace RSCommon;
using namespace log4cplus;

namespace RSTesterBL
{

#define LOGS_DIRECTORY			LOG4CPLUS_TEXT("Logs")

	static void *ExecuteMethod(void* args);
	static void LogMessage(const char* message);

	static clock_t start;

	IMaterialMonitor *materialMonitor = NULL;

	TestManager::TestManager()
		: m_testsFolder(TESTS_FOLDER), m_status(new int), m_volumes(new unsigned int[IDT_COUNT]), m_isBusy(false), m_bStop(false)
	{
		memset(m_volumes, 0, IDT_COUNT * sizeof(unsigned int));

		SetLogMessageFunction(&LogMessage);
		pthread_mutex_init(&m_mtx, NULL);

		start = clock();
	}

	TestManager::~TestManager()
	{
		pthread_mutex_destroy(&m_mtx);

		delete[] m_volumes;
		delete m_status;
		delete materialMonitor;
	}

	bool TestManager::Init()
	{
		materialMonitor = CreateMaterialMonitor();

		if (materialMonitor == NULL)
		{
			Logger logger = Logger::getInstance(TESTER_LOGGER_NAME);
			LOG4CPLUS_ERROR(logger, "Error loading material monitor.");
			return false;
		}

		Logger logger = Logger::getInstance(TESTER_LOGGER_NAME);
		logger.removeAllAppenders();

		unsigned char ucCartridgesCount;
		int returnValue = materialMonitor->InitHW(&ucCartridgesCount);

		*m_status = 0;

		return returnValue == IDTLIB_SUCCESS;
	}

	void TestManager::SetTestsFolder(const char* testsFolder)
	{
		m_testsFolder = testsFolder;
	}

	const char* TestManager::GetTestsFolder() const
	{
		return m_testsFolder.c_str();
	}

	bool TestManager::RunTest(const char* path)
	{
		bool ok;
		TestParser testParser;
		Test *test = testParser.ParseTest(path);
		if (test != NULL)
		{
			// making log directory within tests directory:
			string testDirectory(path);
			testDirectory = testDirectory.substr(0, testDirectory.rfind(PATH_SEPARATOR) + 1);
			testDirectory.append(LOGS_DIRECTORY);
#ifdef _WIN32
			_mkdir(testDirectory.c_str());
#else
			mkdir(testDirectory.c_str(), S_IRWXU);
#endif

			// setting log file name:
			tstring fileName(LOG4CPLUS_TEXT(path));
			fileName.append(LOG4CPLUS_TEXT(".log"));
			size_t lastSeparatorIndex = fileName.rfind(PATH_SEPARATOR);
			fileName.insert(lastSeparatorIndex, 1, PATH_SEPARATOR);
			fileName.insert(lastSeparatorIndex + 1, LOGS_DIRECTORY);

			// configuring logger:
			PropertyConfigurator baseConfig(TESTER_LOG_CONFIG);
			helpers::Properties baseProperties = baseConfig.getProperties();
			helpers::Properties* properties = new helpers::Properties();
			vector<tstring> propertyNames = baseProperties.propertyNames();
			for (vector<string>::iterator it = propertyNames.begin(); it != propertyNames.end(); it++)
			{
				tstring key("log4cplus.");
				key.append(it->c_str());
				properties->setProperty(key, baseProperties.getProperty(it->c_str()));
			}

			properties->setProperty(LOG4CPLUS_TEXT("log4cplus.appender.RaccoonTestFileAppender.File"), fileName);

			Logger logger = Logger::getInstance(TESTER_LOGGER_NAME);
			SharedAppenderPtrList list = logger.getAllAppenders();
			PropertyConfigurator config(*properties);
			config.configure();
			for (SharedAppenderPtrList::iterator it = list.begin(); it != list.end(); it++)
			{
				logger.addAppender(*it);
			}

			LOG4CPLUS_INFO(logger, "Testing " << path << "...");
			ok = RunTest(*test);
			delete test;
			delete properties;
			logger.removeAppender(FILE_APPENDER_NAME);
		}
		else
		{
			ok = false;
		}

		return ok;
	}

	void TestManager::SetStop(bool value)
	{
		UniqueLock uniqueLock(m_mtx);
		m_bStop = value;
	}

	bool TestManager::GetStop() const
	{
//		UniqueLock uniqueLock(m_mtx);
		return m_bStop;
	}

	bool TestManager::RunTest(const Test &test)
	{
		UniqueLock uniqueLock(m_mtx);
		Logger logger = Logger::getInstance(TESTER_LOGGER_NAME);
		LOG4CPLUS_INFO(logger, "Running test...");

		m_bStop = false;
		bool ok = true;
		srand((unsigned int)time(NULL));
		for (size_t i = 0; i < test.ScenariosCount; i++)
		{
			ok &= RunScenario(test.Scenarios[i], test.DefaultParameters, test.DefaultParametersCount);
		}

		if (ok)
		{
			LOG4CPLUS_INFO(logger, "Test passed.\r\n");
		}
		else
		{
			LOG4CPLUS_ERROR(logger, "Test failed.\r\n");
		}

		return ok;
	}

	bool TestManager::RunScenario(const Scenario *scenario, pair<string, string>* defaultParameters, size_t defaultParametersCount)
	{
		bool ok = true;

		pthread_t *executeMethodThreads = new pthread_t[scenario->MethodsCount];
		pthread_attr_t executeMethodThreadAttr;
		pthread_attr_init(&executeMethodThreadAttr);
		pthread_attr_setdetachstate(&executeMethodThreadAttr, PTHREAD_CREATE_JOINABLE);

		ExecuteMethodParameters* executeMethodParameters = new ExecuteMethodParameters[scenario->MethodsCount];

		for (size_t i = 0; i < scenario->MethodsCount; i++)
		{
			executeMethodParameters[i].TestMngr = this;
			executeMethodParameters[i].DefaultParameters = defaultParameters;
			executeMethodParameters[i].DefaultParametersCount = defaultParametersCount;
			executeMethodParameters[i].Mtd = scenario->Methods[i];
			pthread_create(&executeMethodThreads[i], &executeMethodThreadAttr, ExecuteMethod, (void *)&executeMethodParameters[i]);
		}

		pthread_attr_destroy(&executeMethodThreadAttr);
		for (size_t i = 0; i < scenario->MethodsCount; i++)
		{
			void* status;
			pthread_join(executeMethodThreads[i], &status);
			ok &= scenario->Methods[i]->Status;
		}

		delete[] executeMethodParameters;
		delete[] executeMethodThreads;
		return ok;
	}

	bool TestManager::RunMethod(const Method *method, pair<string, string>* defaultParameters, size_t defaultParametersCount)
	{
		m_isBusy = true;

		Logger logger = Logger::getInstance(TESTER_LOGGER_NAME);
		if (materialMonitor == NULL)
		{
			m_isBusy = false;
			LOG4CPLUS_ERROR(logger, "Material monitor is not set.");
			return false;
		}

		bool ok;

		TestParameters* testParameters = new TestParameters;
		testParameters->Parameters = method->Parameters;
		testParameters->ParametersCount = method->ParametersCount;
		testParameters->DefaultParameters = defaultParameters;
		testParameters->DefaultParametersCount = defaultParametersCount;

		char* pcEnd;
		testParameters->ExpectedResult = (int)strtol(method->ExpectedResult.c_str(), &pcEnd, 16);
		if (*pcEnd != 0)
		{
			m_isBusy = false;
			LOG4CPLUS_ERROR(logger, "Invalid expected results (" << method->ExpectedResult << ").");
			return false;
		}

		tostringstream toss;
		toss << "Running " << method->Name << "(";
		for (size_t i = 0; i < method->ParametersCount; i++)
		{
			toss << method->Parameters[i].first << " = " << method->Parameters[i].second;
			if (i < method->ParametersCount - 1)
			{
				toss << ", ";
			}
		}

		toss << "), expecting " << hex << testParameters->ExpectedResult << "...";
		LOG4CPLUS_INFO(logger, toss.str());

		try
		{
			if (method->Name.compare("AuthenticateCartridge") == 0)
			{
				ok = RunAuthenticateCartridge(testParameters);
			}
			else if (method->Name.compare("UpdateConsumption") == 0)
			{
				ok = RunUpdateConsumption(testParameters);
			}
			else if (method->Name.compare("ResetCartridge") == 0)
			{
				ok = RunResetCartridge(testParameters);
			}
			else if (method->Name.compare("BurnSignedIdd") == 0)
			{
				ok = RunBurnSignedIdd(testParameters);
			}
			else if (method->Name.compare("RemoveCartridge") == 0)
			{
				ok = RunRemoveCartridge(testParameters);
			}
			else if (method->Name.compare("GetInPlaceStatus") == 0)
			{
				ok = RunGetInPlaceStatus(testParameters);
			}
			else if (method->Name.compare("InPlaceStatusModified") == 0)
			{
				ok = RunInPlaceStatusModified(testParameters);
			}
			else
			{
				ok = false;
			}

			tostringstream endToss;
			endToss << method->Name << " finished, return value: " << ((testParameters->Result == IDTLIB_SUCCESS) ? "" : "0x") << hex << (short)testParameters->Result << ".";
			string str = endToss.str();
			if (ok)
			{
				LOG4CPLUS_INFO(logger, str);
			}
			else
			{
				LOG4CPLUS_ERROR(logger, str);
			}
		}
		catch (exception& e)
		{
			LOG4CPLUS_ERROR(logger, "Exception caught: " << e.what() << ".");
			ok = false;
		}

		delete testParameters;
		m_isBusy = false;
		return ok;
	}

	bool TestManager::RunGetInPlaceStatus(TestParameters* testParameters) const
	{
		testParameters->Result = materialMonitor->GetInPlaceStatus(m_status);
		return testParameters->Result == testParameters->ExpectedResult;
	}

	bool TestManager::IsBusy() const
	{
		return m_isBusy;
	}
	bool TestManager::RunResetCartridge(TestParameters* testParameters) const
	{
		return true;
	}
	bool TestManager::RunBurnSignedIdd(TestParameters* testParameters) const
	{
		return true;
	}

#if (ENABLE_BURNING == 1)

	bool TestManager::RunResetCartridge(TestParameters* testParameters) const
	{
		char* pcEnd;
		string sCartridgeNum = GetValue("CartridgeNum", testParameters);
		unsigned char ucCartridgeNum = (unsigned char)strtol(sCartridgeNum.c_str(), &pcEnd, 10);

		testParameters->Result = materialMonitor->ResetCartridge(ucCartridgeNum);
		return testParameters->Result == testParameters->ExpectedResult;
	}

	bool TestManager::RunBurnSignedIdd(TestParameters* testParameters) const
	{
		char* pcEnd;
		string sCartridgeNum = GetValue("CartridgeNum", testParameters);
		unsigned char ucCartridgeNum = (unsigned char)strtol(sCartridgeNum.c_str(), &pcEnd, 10);

		IdentificationData idd;
		string sValue = GetValue("TagStructID", testParameters);
		idd.MaterialInfo.TagStructID = (TAG_TYPE__STRUCT_ID)strtol(sValue.c_str(), &pcEnd, 10);
		sValue = GetValue("MaterialType", testParameters);
		idd.MaterialInfo.MaterialType = (TAG_TYPE__MATERIAL_TYPE)strtol(sValue.c_str(), &pcEnd, 10);
		idd.MaterialInfo.MfgDateTime = (TAG_TYPE__MFG_DATE_TIME)GetUnixTimeValue("MfgDateTime", testParameters);
		string sBatchNo = GetValue("BatchNo", testParameters);
		memcpy(idd.MaterialInfo.BatchNo, sBatchNo.c_str(), BATCH_NO_SIZE);
		sValue = GetValue("InitialWeight", testParameters);
		idd.MaterialInfo.InitialWeight = (TAG_TYPE__INITIAL_WEIGHT)strtol(sValue.c_str(), &pcEnd, 10);
		idd.MaterialInfo.ExpirationDate = (TAG_TYPE__MFG_DATE_TIME)GetUnixTimeValue("ExpirationDate", testParameters);
		sValue = GetValue("ExtraWeight", testParameters);
		idd.MaterialInfo.ExtraWeight = (TAG_TYPE__EXTRA_WEIGHT)strtol(sValue.c_str(), &pcEnd, 10);
		unsigned char ucSerialNumberLength;
		testParameters->Result = materialMonitor->GetDeviceSerialNumber(ucCartridgeNum, (unsigned char*)idd.SerialNo, &ucSerialNumberLength);

		if (testParameters->Result == IDTLIB_SUCCESS)
		{
			unsigned char aucPubKC[2 * KEY_COEFF_SIZE];
			unsigned short usKeyLength;
			testParameters->Result = materialMonitor->ReadTagPublicKey(aucPubKC, &usKeyLength);
			if (testParameters->Result == IDTLIB_SUCCESS)
			{
				try
				{
					PolynomialMod2 Qx, Qy;
					Qx.Decode(aucPubKC, usKeyLength / 2);
					Qy.Decode(aucPubKC + usKeyLength / 2, usKeyLength / 2);
					PubKey pubKC;
					pubKC.Initialize(ASN1::sect283k1(), EC2NPoint(Qx, Qy));
					idd.SetCartridgePublicKey(pubKC);
				}
				catch (...)
				{
					testParameters->Result = INVALID_HOST_KEY;
				}

				unsigned char aucIdd[127];
				size_t usIddLength;
				usIddLength = idd.Encode(aucIdd);

				string signature;
				size_t signatureLength = 0;
				try
				{				
					unsigned char aucPrvKS[PRIVATE_KEY_SIZE];
					string sPrvKS = GetValue("PrvKS", testParameters);
					size_t keySize = sizeof(aucPrvKS);
					ReadFileContent(m_testsFolder, sPrvKS, aucPrvKS, &keySize);
					ECDSA<EC2N, SHA256>::PrivateKey prvKS;
					prvKS.Load(StringSource(aucPrvKS, PRIVATE_KEY_SIZE, true).Ref());

					AutoSeededRandomPool prng;
					ECDSA<EC2N, SHA256>::Signer signer(prvKS);
					signatureLength = signer.MaxSignatureLength();
					signature.assign(signatureLength, 0);
					signatureLength = signer.SignMessage(prng, aucIdd, usIddLength, (byte*)signature.data());
					signature.resize(signatureLength);
				}
				catch(...)
				{
					testParameters->Result = INVALID_HOST_KEY;
				}

				if (testParameters->Result == IDTLIB_SUCCESS)
				{
					testParameters->Result = materialMonitor->BurnSignedIdd(ucCartridgeNum, aucIdd, usIddLength - 2 * KEY_COEFF_SIZE, (unsigned char*)signature.data(), signatureLength);
				}
			}
		}

		return testParameters->Result == testParameters->ExpectedResult;
	}

#endif

	bool TestManager::RunRemoveCartridge(TestParameters* testParameters) const
	{
		Logger logger = Logger::getInstance(TESTER_LOGGER_NAME);

		char* pcEnd;
		string sCartridgeNum = GetValue("CartridgeNum", testParameters);
		unsigned char ucCartridgeNum = (unsigned char)strtol(sCartridgeNum.c_str(), &pcEnd, 10);
		if (*pcEnd != 0)
		{
			LOG4CPLUS_ERROR(logger, "Invalid cartridge num (" << sCartridgeNum << ").");
			return false;
		}

		testParameters->Result = materialMonitor->RemoveCartridge(ucCartridgeNum);
		return testParameters->Result == testParameters->ExpectedResult;
	}

	bool TestManager::RunAuthenticateCartridge(TestParameters* testParameters) const
	{
		Logger logger = Logger::getInstance(TESTER_LOGGER_NAME);

		char* pcEnd;
		string sCartridgeNum = GetValue("CartridgeNum", testParameters);
		unsigned char ucCartridgeNum = (unsigned char)strtol(sCartridgeNum.c_str(), &pcEnd, 10);
		if (*pcEnd != 0)
		{
			LOG4CPLUS_ERROR(logger, "Invalid cartridge num (" << sCartridgeNum << ").");
			return false;
		}

		unsigned char idd[640];
		unsigned short iddLength;
		memset(idd, 0, 640);

		testParameters->Result = materialMonitor->AuthenticateCartridge(ucCartridgeNum, false, idd, &iddLength, &m_volumes[ucCartridgeNum]);
		return testParameters->Result == testParameters->ExpectedResult;
	}

	bool TestManager::RunUpdateConsumption(TestParameters* testParameters) const
	{
		Logger logger = Logger::getInstance(TESTER_LOGGER_NAME);

		char* pcEnd;
		string sCartridgeNum = GetValue("CartridgeNum", testParameters);
		unsigned char ucCartridgeNum = (unsigned char)strtol(sCartridgeNum.c_str(), &pcEnd, 10);
		if (*pcEnd != 0)
		{
			LOG4CPLUS_ERROR(logger, "Invalid cartridge num (" << sCartridgeNum << ").");
			return false;
		}

		string sConsumption = GetValue("Consumption", testParameters);
		string sMaxConsumption;
		size_t idx = sConsumption.find("..");
		if (idx != string::npos)
		{
			sMaxConsumption = sConsumption.substr(idx + 2, sConsumption.length() - (idx + 2));
			sConsumption = sConsumption.substr(0, idx);
		}
		else
		{
			sMaxConsumption = sConsumption;
		}

		unsigned int uiConsumption = (unsigned int)strtol(sConsumption.c_str(), &pcEnd, 10);
		if (*pcEnd != 0)
		{
			LOG4CPLUS_ERROR(logger, "Invalid consumption (" << sConsumption << ").");
			return false;
		}

		unsigned int uiMaxConsumption = (unsigned int)strtol(sMaxConsumption.c_str(), &pcEnd, 10);
		if (*pcEnd != 0)
		{
			LOG4CPLUS_ERROR(logger, "Invalid consumption (" << sMaxConsumption << ").");
			return false;
		}

		uiConsumption += (rand() % (uiMaxConsumption - uiConsumption + 1));

		unsigned int originalVolume = m_volumes[ucCartridgeNum];

		string sSkip = GetValue("Skip", testParameters);
		if (sSkip.empty())
		{
			testParameters->Result = materialMonitor->UpdateConsumption(ucCartridgeNum, uiConsumption, &m_volumes[ucCartridgeNum]);
		}
		else
		{
			unsigned char ucSkip = (unsigned char)strtol(sSkip.c_str(), &pcEnd, 10);
			if (*pcEnd != 0)
			{
				LOG4CPLUS_ERROR(logger, "Invalid skip value (" << sSkip << ").");
				return false;
			}

			testParameters->Result = materialMonitor->UpdateConsumption(ucCartridgeNum, uiConsumption, &m_volumes[ucCartridgeNum]);
		}

		if (testParameters->Result != IDTLIB_SUCCESS)
		{
			m_volumes[ucCartridgeNum] = originalVolume;
		}

		return testParameters->Result == testParameters->ExpectedResult;
	}

	bool TestManager::RunInPlaceStatusModified(TestParameters* testParameters) const
	{
		int returnValue, origStatus, diffStatus;
		do
		{
			origStatus = *m_status;
			returnValue = materialMonitor->GetInPlaceStatus(m_status);
			diffStatus = origStatus ^ *m_status;
			CPSleep(1000);
		} while (
			(returnValue == IDTLIB_SUCCESS) &&
			(diffStatus == 0) &&
			!m_bStop);

		testParameters->Result = IDTLIB_SUCCESS;
		for (unsigned char ucCartridgeNum = 0; ucCartridgeNum < IDT_COUNT; ucCartridgeNum++)
		{
			if ((diffStatus & (1 << ucCartridgeNum)) == 0)
			{
				continue;
			}

			int result;
			if ((*m_status & (1 << ucCartridgeNum)) == 0)
			{
				result = materialMonitor->RemoveCartridge(ucCartridgeNum);
			}
			else
			{
				unsigned char aucIdd[128];
				unsigned short usIddLength;
				unsigned int currentVolume;
				result = materialMonitor->AuthenticateCartridge(ucCartridgeNum, true, aucIdd, &usIddLength, &currentVolume);
			}

			if (result != IDTLIB_SUCCESS)
			{
				testParameters->Result = result;
			}
		}

		return testParameters->Result == testParameters->ExpectedResult;
	}

	string TestManager::GetValue(const char* parameterName, TestParameters* testParameters)
	{
		string value = "";
		for (size_t i = 0; i < testParameters->ParametersCount; i++)
		{
			if (testParameters->Parameters[i].first == parameterName)
			{
				value = testParameters->Parameters[i].second;
				break;
			}
		}

		if (value.empty())
		{
			for (size_t i = 0; i < testParameters->DefaultParametersCount; i++)
			{
				if (testParameters->DefaultParameters[i].first == parameterName)
				{
					value = testParameters->DefaultParameters[i].second;
					break;
				}
			}
		}

		return value;
	}

	time_t TestManager::GetUnixTimeValue(const char* parameterName, TestParameters* testParameters)
	{
		string value = GetValue(parameterName, testParameters);
		tm tmValue;
		int month, year;
		sscanf(value.c_str(), "%2d/%2d/%4d %2d:%2d:%2d", &tmValue.tm_mday, &month, &year, &tmValue.tm_hour, &tmValue.tm_min, &tmValue.tm_sec);
		tmValue.tm_mon = month - 1;
		tmValue.tm_year = year - 1900;
		return mktime(&tmValue);
	}

	void TestManager::ReadFileContent(string basePath, string path, unsigned char* buffer, size_t* length)
	{
		string fullPath = basePath;
		fullPath.append(1, PATH_SEPARATOR);
		fullPath.append(path);

		FILE *pFile = fopen(fullPath.c_str(), "rb");
		if (pFile != NULL)
		{
			fseek(pFile , 0 , SEEK_END);
			size_t fileSize = (size_t)ftell(pFile);
			rewind(pFile);
			*length = fread(buffer, sizeof(unsigned char), min(fileSize, *length), pFile);
			fclose(pFile);
		}
	}

	static void *ExecuteMethod(void* args)
	{
		ExecuteMethodParameters* executeMethodParameters = new ExecuteMethodParameters;
		*executeMethodParameters = *((ExecuteMethodParameters*)args);

		bool ok = true;
		clock_t start = clock(), elapsed;
		unsigned int counter = 0;
		do
		{
	//		if (!executeMethodParameters->TestMngr->IsBusy())
			{
				counter++;
				ok =  executeMethodParameters->TestMngr->RunMethod(
					executeMethodParameters->Mtd,
					executeMethodParameters->DefaultParameters,
					executeMethodParameters->DefaultParametersCount);
			}

			CPSleep(executeMethodParameters->Mtd->Interval);
			elapsed = clock() - start;
		} while (
			!executeMethodParameters->TestMngr->GetStop() &&
			executeMethodParameters->Mtd->Repeating && 
			ok &&
			(executeMethodParameters->Mtd->Timeout == -1 || elapsed < (executeMethodParameters->Mtd->Timeout * CLOCKS_PER_SEC)));

		bool stopped = executeMethodParameters->TestMngr->GetStop();
		executeMethodParameters->Mtd->Status = (!stopped && ok);
		if (stopped)
		{
			Logger logger = Logger::getInstance(TESTER_LOGGER_NAME);
			LOG4CPLUS_INFO(logger, "Test terminated.");
		}

		delete executeMethodParameters;
		return NULL;
	}

	static void LogMessage(const char* message)
	{
		char* buffer = new char[32];
		clock_t now = clock();
		clock_t diff = now - start;
		ltoa(diff, buffer, 10);

		string formattedMessage = buffer;
		formattedMessage.append(" ");
		formattedMessage.append(message);
		formattedMessage.append("\n");

		cout << formattedMessage.c_str();
		delete[] buffer;

		FILE* fp = fopen("Raccoon.log", "a");
		fwrite(formattedMessage.c_str(), sizeof(char), formattedMessage.length(), fp);
		fclose(fp);
	}
}
