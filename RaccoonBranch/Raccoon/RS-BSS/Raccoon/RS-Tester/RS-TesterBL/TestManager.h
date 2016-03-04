#pragma once

#include "stdafx.h"
#include "Test.h"
#include <time.h>
#include <pthread.h>

namespace RSTesterBL
{
#define IDT_COUNT				32
#define MATERIAL_INFO_LENGTH	25
#define TESTS_FOLDER			"Tests"
#define KEYS_FOLDER				"Keys"
#define MATERIAL_INFO_FOLDER	"MaterialInfo"
#define DEFAULT_PUBKS_FILE		"pubKS.key"
#define TESTER_LOGGER_NAME		LOG4CPLUS_TEXT("RaccoonTest")
#define TESTER_LOG_CONFIG		LOG4CPLUS_TEXT("log4cplusRaccoonTest.prop")
#define FILE_APPENDER_NAME		LOG4CPLUS_TEXT("RaccoonTestFileAppender")

	typedef struct
    {
        pair<string, string>* Parameters;
        size_t ParametersCount;
        pair<string, string>* DefaultParameters;
        size_t DefaultParametersCount;
        int ExpectedResult;
        int Result;
    } TestParameters;

	class RSTESTER_BL_LIB TestManager
	{
	public:
		TestManager();
		~TestManager();

		bool Init();
		bool RunTest(const char* path);
		bool RunTest(const Test &test);
		void SetStop(bool value);
		bool GetStop() const;
		bool RunScenario(const Scenario *scenario, pair<string, string>* defaultParameters, size_t defaultParametersCount);
		bool RunMethod(const Method *method, pair<string, string>* defaultParameters, size_t defaultParametersCount);
		bool IsBusy() const;

		void SetTestsFolder(const char* testsFolder);
		const char* GetTestsFolder() const;

	private:
		bool RunGetInPlaceStatus(TestParameters* testParameters) const;
		bool RunResetCartridge(TestParameters* testParameters) const;
		bool RunBurnSignedIdd(TestParameters* testParameters) const;
		bool RunRemoveCartridge(TestParameters* testParameters) const;
		bool RunAuthenticateCartridge(TestParameters* testParameters) const;
		bool RunUpdateConsumption(TestParameters* testParameters) const;
		bool RunInPlaceStatusModified(TestParameters* testParameters) const;

		static void ReadFileContent(string basePath, string path, unsigned char* buffer, size_t* length);
		static string GetValue(const char* parameterName, TestParameters* testParameters);
		static time_t GetUnixTimeValue(const char* parameterName, TestParameters* testParameters);

	private:
		//RSCommon::IMaterialMonitor *m_materialMonitor;
		string m_testsFolder;
		int* m_status;
		unsigned int* m_volumes;
		bool m_isBusy;
		pthread_mutex_t m_mtx;
		bool m_bStop;
	};

    typedef struct
    {
        TestManager* TestMngr;
        Method* Mtd;
        pair<string, string>* DefaultParameters;
        size_t DefaultParametersCount;
    } ExecuteMethodParameters;
}
