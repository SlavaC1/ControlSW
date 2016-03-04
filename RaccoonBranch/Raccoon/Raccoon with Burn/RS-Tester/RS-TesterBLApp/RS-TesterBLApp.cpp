// RS-TesterBLApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "RSTesterBLWrapper.h"
#include <stdlib.h>
#include <string>
#include <iostream>
#include <signal.h>

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
