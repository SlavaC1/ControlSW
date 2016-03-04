// ECDSA.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CertGenerator.h"
#include <time.h>

using namespace std;

#ifdef _WIN32
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char** argv)
#endif
{
	char appName[MAX_PATH];
	GetModuleFileName(NULL, appName, MAX_PATH);

	string sAppName = appName;
	string appFolder = sAppName.substr(0, sAppName.rfind('\\'));
	SetCurrentDirectory(appFolder.c_str());

	CertGen::CertGenerator cg;
	cg.TestCrypto();
	return 0;    
}

extern "C" __declspec(dllexport) void GenerateCertificate(const unsigned char *idd, unsigned short iddLength)
{
	CertGen::CertGenerator cg;
	cg.GenerateCertificate(idd, iddLength);
}