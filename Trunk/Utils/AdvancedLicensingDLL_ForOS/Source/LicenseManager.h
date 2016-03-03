//---------------------------------------------------------------------------

#ifndef LicenseManagerH
#define LicenseManagerH             

#include "tinyxml2.h"
#include <vector>
#include <exception>
#include "LicenseManagerInterface.h"
#include "LicenseManagerDefs.h"

using namespace tinyxml2;

// Exception class for license manager
class ELicenseManager : public std::exception
{
protected:
	std::string m_Msg;
    int         m_ErrCode;

public:
	ELicenseManager(const char* Msg, const int ErrCode);
	ELicenseManager(const std::string &Msg, const int ErrCode);
	virtual ~ELicenseManager() throw (){}
	virtual const char* what() const throw();
	virtual const int GetErrorCode() const throw();
};

// License manager class
class CLicenseManager : public ILicenseManager
{
private:

	unsigned char* m_FileBuffer;

	std::string m_DatabasePath;
	
	TPackagesList          m_PackagesList;
	TLicensedMaterialsList m_LicensedMaterialsList;
	TPropertiesList        m_PropertiesList;

	float m_PackagesDatabaseVersion;

	std::string  __stdcall DecryptDatabaseFile(std::string FilePath);
	unsigned int __stdcall ReadFileIntoBuffer (std::string FilePath);
	void 		 __stdcall ParseMaterialsList (XMLElement* MaterialsNode,      TLicensedMaterialsList &MaterialsList, const std::string &PackageName);
	void 		 __stdcall ParseProperties    (XMLElement* TrayPropertiesNode, const std::string &PackageName);
	TLMStatus    __stdcall ParsePackagesXML   ();

public:

	CLicenseManager();
	~CLicenseManager();

	TLMStatus   __stdcall ReadDatabase              (const char*       DatabasePath);
	bool        __stdcall IsMaterialLicensed        (const char*       MaterialName);
	bool        __stdcall IsPropertyLicensed        (const TPropertyID PropertyID);
	float       __stdcall GetPackagesDatabaseVersion();
	void        __stdcall GetLicensedMaterials      (TMaterialsList *MaterialsList);
};

//---------------------------------------------------------------------------
#endif
