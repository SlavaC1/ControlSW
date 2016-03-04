#pragma once

#include "TestManager.h"

using namespace RSTesterBL;

namespace RSTesterBLApp
{

	class RSTesterBLWrapper
	{
	public:
		RSTesterBLWrapper();
		~RSTesterBLWrapper();

		bool Init();
		void SetTestsFolder(const char* testFolder);
		bool RunTest(const char* path) const;
		bool RunTests(char** filesList, size_t filesListCount) const;
		void RunRemoteTests(const char* address, char** filesList, size_t filesListCount);
		void StopTest();
		bool GetStop() const;

	private:

		TestManager* m_testManager;
		bool m_stop;
		void* m_responder;
		void* m_context;
	};

	typedef struct
	{
		void* Responder;
		const RSTesterBLWrapper* RSTesterBLWrpr;
		char** FilesList;
		size_t FilesListLength;
	} ReceiveFileNamesArgs;
}
