#if defined _WIN32
	#pragma comment(lib, "..\\lib\\libhasp_windows_92199_32.lib")
	#if defined _DEBUG
		#pragma comment(lib, "..\\lib\\Debug\\32\\libhasp_cpp_windows_mt_msc10_d.lib")
	#else
		#pragma comment(lib, "..\\lib\\Release\\32\\libhasp_cpp_windows_mt_msc10.lib")
	#endif
#else
	#pragma comment(lib, "..\\lib\\libhasp_windows_92199_64.lib")
	#if defined _DEBUG
		#pragma comment(lib, "..\\lib\\Debug\\64\\libhasp_cpp_windows_x64_mt_msc10_d.lib")
	#else
		#pragma comment(lib, "..\\lib\\Release\\64\\libhasp_cpp_windows_x64_mt_msc10.lib")
	#endif
#endif



#include "LicenseManager.h"
#include <iostream>

void main()
{
	CLicenseManager LM;

	TLMStatus Status = LM.ReadDatabase("C:\\SVN\\Trunk\\Utils\\AdvancedLicensingDLL_ForOS\\VS2013NonDLLProject\\AL_OS.dat");

	if (Status != LM_STATUS_OK)
	{
		std::cout << LM.Status2String(Status) << std::endl;
		return;
	}

	bool VW = LM.IsMaterialLicensed("VeroTrue");

	TMaterialsList MaterialsList;
	LM.GetLicensedMaterials(&MaterialsList);

	int i = MaterialsList.MaterialsNum;

	bool propTest = LM.IsPropertyLicensed(propVoid);

	double Version = LM.GetPackagesDatabaseVersion();
}