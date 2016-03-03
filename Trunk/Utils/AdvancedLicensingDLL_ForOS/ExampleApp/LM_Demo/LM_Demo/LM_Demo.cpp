
#include "LicenseManagerInterface.h"


void main()
{
	ILicenseManager *LM = CreateLicenseManager(); 
	
	TLMStatus Status = LM->ReadDatabase("C:\\SVN\\Trunk\\Utils\\AdvancedLicensingDLL_ForOS\\ExampleApp\\AL_OS.dat");	

	if(Status != LM_STATUS_OK)
		return;	
	
	bool VW = LM->IsMaterialLicensed("VeroTrue");

	TMaterialsList MaterialsList;
	LM->GetLicensedMaterials(&MaterialsList);
	
	int i = MaterialsList.MaterialsNum;
	
	bool propTest = LM->IsPropertyLicensed(propVoid);

	float Version = LM->GetPackagesDatabaseVersion();

	DeleteLicenseManager();
}

