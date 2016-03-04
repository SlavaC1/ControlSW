// RS-TesterBLApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "RSTesterBLWrapper.h"
#include <stdlib.h>
#include <string>
#include <iostream>
#include <signal.h>
#include "../IDT-Srv/MaterialMonitor.h"
#include <ctime>


#ifndef _WIN32
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#endif

#include "RSCommonDefinitions.h"

#ifdef _WIN32
#define PATH_LENGTH MAX_PATH
#else
#define PATH_LENGTH PATH_MAX
#endif

using namespace std;

int GetRunningDirectory(char* pBuf)
{
	int bytes;

#if defined(WIN32) || defined(_WIN32)
	bytes = GetModuleFileName(NULL, pBuf, PATH_LENGTH);
	if(bytes == 0)
	{
		bytes = -1;
	}
#else
	char szTmp[32];
	sprintf(szTmp, "/proc/%d/exe", getpid());
	bytes = readlink(szTmp, pBuf, PATH_LENGTH);
	if(bytes >= 0)
	{
		pBuf[bytes] = '\0';
	}
#endif

	return bytes;
}

int SetWorkingDirectory(const char* directory)
{
#if defined(WIN32) || defined(_WIN32)
	return SetCurrentDirectory(directory);
#else
	return chdir(directory);
#endif
}

void GetFilesInDirectory(std::vector<string> &out, const string &directory)
{
#if defined(WIN32) || defined(_WIN32)
	HANDLE dir;
	WIN32_FIND_DATA file_data;

	if ((dir = FindFirstFile((directory + PATH_SEPARATOR + "*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
		return; /* No files found */

	do {
		const string file_name = file_data.cFileName;
		const string full_file_name = directory + PATH_SEPARATOR + file_name;
		const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

		if (file_name[0] == '.')
			continue;

		if (is_directory)
			continue;

		out.push_back(full_file_name);
	} while (FindNextFile(dir, &file_data));

	FindClose(dir);
#else
	DIR *dp;
	struct dirent *dirp;
	if ((dp  = opendir(directory.c_str())) == NULL)
	{
		return;
	}

	while ((dirp = readdir(dp)) != NULL)
	{
		out.push_back(string(dirp->d_name));
	}

	closedir(dp);
#endif
}

int filesStartIndex = 1;
size_t filesListCount = 0;
char** filesList = NULL;
char* address = NULL;
char directory[PATH_LENGTH];

RSTesterBLApp::RSTesterBLWrapper rsTesterBLWrapper;

// Define the function to be called when ctrl-c (SIGINT) signal is sent to process
void SignalCallbackHandler(int signum)
{
	rsTesterBLWrapper.StopTest();
}

int RunMyTest()
{
	int status = 0;
	IMaterialMonitor *pMaterialMonitor = NULL;
	unsigned char ucCartridgesCount;

	cout << "Run my tests " << endl;

	pMaterialMonitor = CreateMaterialMonitor();

	if(pMaterialMonitor != NULL)
	{
		cout << "Create Material Monitor successfully " << endl;

		int returnValue = pMaterialMonitor->InitHW(&ucCartridgesCount);
		if(returnValue == IDTLIB_SUCCESS)
			cout << "HW initialized successfully. Number of cartridges: "<< (unsigned short)(ucCartridgesCount) << endl;
		returnValue = pMaterialMonitor->GetInPlaceStatus(&status);
		cout << "GetInPlaceStatus = " << status << endl;

		unsigned char aucIdd[128];
		unsigned short usIddLength;
		unsigned int currentVolume=0;

		returnValue = pMaterialMonitor->AuthenticateCartridge(0, false, aucIdd, &usIddLength, &currentVolume);
		cout << "AuthenticateCartridge currentVolume = " << currentVolume << endl;
		currentVolume=0;
		returnValue = pMaterialMonitor->AuthenticateCartridge(0, true, aucIdd, &usIddLength, &currentVolume);
		cout << "AuthenticateCartridge currentVolume = " << currentVolume << endl;

		clock_t startTime = clock();
		returnValue = pMaterialMonitor->AuthenticateCartridge(0, true, aucIdd, &usIddLength, &currentVolume);
		clock_t endTime = clock();

		clock_t clockTicksTaken = endTime - startTime;
		double timeInSeconds = clockTicksTaken / (double) CLOCKS_PER_SEC;
		cout << "AuthenticateCartridge finished in " << timeInSeconds << " seconds" << endl;

		cout << "AuthenticateCartridge currentVolume = " << currentVolume << endl;

		unsigned int volume=0;
		startTime = clock();
		returnValue = pMaterialMonitor->UpdateConsumption(0, 10, &volume);
		endTime = clock();
		clockTicksTaken = endTime - startTime;
		timeInSeconds = clockTicksTaken / (double) CLOCKS_PER_SEC;
		cout << "UpdateConsumption finished in " << timeInSeconds << " seconds" << endl;

		cout << "UpdateConsumption returnValue = " << returnValue << "  volume = " << volume << endl;

		returnValue = pMaterialMonitor->RemoveCartridge(0);
		cout << "RemoveCartridge returnValue = " << returnValue <<endl;

//		returnValue = pMaterialMonitor->RemoveCartridge(1);
//		cout << "RemoveCartridge returnValue = " << returnValue <<endl;

		volume=0;
		returnValue = pMaterialMonitor->UpdateConsumption(0, 10, &volume);
		cout << "UpdateConsumption returnValue = " << returnValue << "  volume = " << volume << endl;

		currentVolume=0;
		DWORD timeS = GetTickCount();
		returnValue = pMaterialMonitor->AuthenticateCartridge(0, true, aucIdd, &usIddLength, &currentVolume);
		DWORD timeE = GetTickCount();
		cout << "AuthenticateCartridge finished in " << timeE-timeS << " milliseconds " << endl;

		cout << "AuthenticateCartridge currentVolume = " << currentVolume << endl;

		volume=0;
		returnValue = pMaterialMonitor->UpdateConsumption(0, 10, &volume);
		cout << "UpdateConsumption returnValue = " << returnValue << "  volume = " << volume << endl;

		volume=0;
		returnValue = pMaterialMonitor->UpdateConsumption(1, 10, &volume);
		cout << "UpdateConsumption returnValue = " << returnValue << "  volume = " << volume << endl;

		delete pMaterialMonitor;
	}

	return 0; 
}

int main(int argc, char** argv)
{
	string str = TESTS_FOLDER;
	strcpy(directory, str.c_str());
	char appName[PATH_LENGTH];
	GetRunningDirectory(appName);

	string sAppName = appName;
	string appFolder = sAppName.substr(0, sAppName.rfind(PATH_SEPARATOR));
	SetWorkingDirectory(appFolder.c_str());

	vector<string> filesVector;

	const char* NetworkPrefix = "tcp://";
	const char* AddressFlag = "-a";
	const char* DirFlag = "-d";
	const char* FilesFlag = "-f";

	return RunMyTest();

	bool ok = rsTesterBLWrapper.Init();
	if (!ok)
	{
		cout << "Press any key to continue...\n";
		char c;
		cin >> c;
		return 1;
	}

	if (argc > filesStartIndex && (strcmp(argv[filesStartIndex], AddressFlag) == 0))
	{
		if (filesStartIndex + 1 < argc)
		{
			string addressStr = NetworkPrefix;
			addressStr.append(argv[filesStartIndex + 1]);
			address = new char[addressStr.length() + 1];
			strcpy(address, addressStr.c_str());
		}

		filesStartIndex += 2;
	}

	bool findFiles =
		filesStartIndex >= argc ||
		strcmp(argv[filesStartIndex], DirFlag) == 0;

	if (findFiles)
	{
		if (filesStartIndex + 1 < argc)
		{
			strcpy(directory, argv[filesStartIndex + 1]);
			rsTesterBLWrapper.SetTestsFolder(directory);
		}

		GetFilesInDirectory(filesVector, directory);
		filesListCount = filesVector.size();
		if (filesListCount > 0)
		{
			filesList = new char*[filesListCount];
			for (size_t i = 0; i < filesListCount; i++)
			{
				filesList[i] = new char[filesVector[i].size() + 1];
				strcpy(filesList[i], filesVector[i].c_str());
				filesList[i][filesVector[i].size()] = '\0';
			}
		}
	}
	else if (strcmp(argv[filesStartIndex], FilesFlag) == 0)
	{
		filesStartIndex++;
		filesList = argv + filesStartIndex;
		filesListCount = argc - filesStartIndex;
	}


	if (address != NULL)
	{
		signal(SIGINT, SignalCallbackHandler);
		signal(SIGABRT, SignalCallbackHandler);
		signal(SIGTERM, SignalCallbackHandler);
		rsTesterBLWrapper.RunRemoteTests(address, filesList, filesListCount);
	}
	else
	{
		rsTesterBLWrapper.RunTests(filesList, filesListCount);
	}

	if (findFiles)
	{
		for (size_t i = 0; i < filesListCount; i++)
		{
			delete[] filesList[i];
		}

		delete[] filesList;
	}

	if (address != NULL)
	{
		delete[] address;
	}

	return 0;
}
