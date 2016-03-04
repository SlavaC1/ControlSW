#include "stdafx.h"

#include <iostream>
#include <pthread.h>

#include "RSTesterBLWrapper.h"
#include "zmq.h"

#include "log4cplus/logger.h"
#include "ZmqAppender.h"

#define LOGGER_DEFAULT_LAYOUT	LOG4CPLUS_TEXT("%D{%d-%b-%y %H:%M:%S.%q} - %-5p: %m%n")

namespace RSTesterBLApp
{

	static void *ReceiveFileNames(void* args);

	RSTesterBLWrapper::RSTesterBLWrapper(void)
		: m_stop(false), m_responder(NULL), m_context(NULL)
	{
		m_testManager = new TestManager();
	}

	RSTesterBLWrapper::~RSTesterBLWrapper(void)
	{
		delete m_testManager;
	}

	bool RSTesterBLWrapper::Init()
	{
		return m_testManager->Init();
	}

	void RSTesterBLWrapper::SetTestsFolder(const char* testFolder)
	{
		m_testManager->SetTestsFolder(testFolder);
	}

	bool RSTesterBLWrapper::RunTest(const char* path) const
	{
		return m_testManager->RunTest(path);
	}

	bool RSTesterBLWrapper::RunTests(char** filesList, size_t filesListCount) const
	{
		bool ok = true;
		for (size_t i = 0; i < filesListCount; i++)
		{
			ok &= RunTest(filesList[i]);
		}

		return ok;
	}

	void RSTesterBLWrapper::RunRemoteTests(const char* address, char** filesList, size_t filesListCount)
	{
		m_context = zmq_ctx_new();
		m_responder = zmq_socket(m_context, ZMQ_REP);
		cout << "Binding to socket " << address << "..." << endl;
		int rc = zmq_bind(m_responder, address);
		if (rc != 0)
		{
			return;
		}

		Logger logger = Logger::getInstance(TESTER_LOGGER_NAME);
		SharedAppenderPtr zmqAppender(new ZmqAppender(m_responder));
		tstring appenderName = TESTER_LOGGER_NAME;
		appenderName.append("ZmqAppender");
		zmqAppender->setName(appenderName);
		zmqAppender->setLayout(std::auto_ptr<Layout>(new PatternLayout(LOGGER_DEFAULT_LAYOUT)));
		logger.addAppender(zmqAppender);

		pthread_t receiveFileNamesThread;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

		ReceiveFileNamesArgs args;
		args.Responder = m_responder;
		args.RSTesterBLWrpr = this;
		args.FilesList = filesList;
		args.FilesListLength = filesListCount;
		pthread_create(&receiveFileNamesThread, &attr, ReceiveFileNames, (void*)&args);
		pthread_attr_destroy(&attr);

		void* status;
		pthread_join(receiveFileNamesThread, &status);

		logger.removeAppender(zmqAppender);
	}

	void RSTesterBLWrapper::StopTest()
	{
		Logger logger = Logger::getInstance(TESTER_LOGGER_NAME);
		m_stop = true;
		m_testManager->SetStop(true);
		zmq_close(m_responder);
		zmq_ctx_destroy(m_context);
	}

	bool RSTesterBLWrapper::GetStop() const
	{
		return m_stop;
	}

	void *ReceiveFileNames(void* args)
	{
		ReceiveFileNamesArgs* receiveFileNamesArgs = (ReceiveFileNamesArgs*)args;
		const char* newLine = "\r\n";
		char buffer[0xffff];
		bool abort = false;

		while (!receiveFileNamesArgs->RSTesterBLWrpr->GetStop())
		{
			cout << "Receiving control byte..." << endl;
			int bytesReceived = zmq_recv(receiveFileNamesArgs->Responder, buffer, 0xffff, 0);
			if (receiveFileNamesArgs->RSTesterBLWrpr->GetStop())
			{
				break;
			}

			if (bytesReceived == 1)
			{
				size_t charsCount = 0;
				for (size_t i = 0; i < receiveFileNamesArgs->FilesListLength; i++)
				{
					charsCount += strlen(receiveFileNamesArgs->FilesList[i]) + 2;
				}

				if (charsCount == 0)
				{
					abort = true;
					break;
				}

				string sFilesList;
				for (size_t i = 0; i < receiveFileNamesArgs->FilesListLength; i++)
				{
					sFilesList.append(receiveFileNamesArgs->FilesList[i]);
					sFilesList.append(newLine);
				}

				zmq_send(receiveFileNamesArgs->Responder, sFilesList.c_str(), charsCount, 0);
				if (receiveFileNamesArgs->RSTesterBLWrpr->GetStop())
				{
					break;
				}

				cout << "Receiving test file names..." << endl;
				zmq_recv(receiveFileNamesArgs->Responder, buffer, charsCount - 1, 0);
				if (receiveFileNamesArgs->RSTesterBLWrpr->GetStop())
				{
					break;
				}
			}

			if (abort)
			{
				break;
			}

			cout << buffer << endl;
			string fileNames(buffer);
			size_t startIndex = 0, endIndex;
			do
			{
				endIndex = fileNames.find(newLine, startIndex);
				if (static_cast<int>(endIndex) == -1)
				{
					endIndex = fileNames.length();
				}

				string fileName = fileNames.substr(startIndex, endIndex - startIndex);
				receiveFileNamesArgs->RSTesterBLWrpr->RunTest(fileName.c_str());
				startIndex = endIndex + 2;
			} while (startIndex < fileNames.length() && !receiveFileNamesArgs->RSTesterBLWrpr->GetStop());

			unsigned char controlByte = 3;
			zmq_send(receiveFileNamesArgs->Responder, &controlByte, 1, 0);
		}

		return NULL;
	}
}
