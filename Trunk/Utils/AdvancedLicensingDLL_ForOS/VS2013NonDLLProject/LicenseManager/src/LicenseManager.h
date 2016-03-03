#pragma once 

#include "tinyxml2.h"
#include <vector>
#include <exception>
#include "hasp_api_cpp_.h"
#include "LicenseManagerDefs.h"

using namespace tinyxml2;

// Holder objects
class CHaspHolder
{
private:
	Chasp m_Hasp;
public:
	CHaspHolder(const Chasp &hasp);
	~CHaspHolder();
};

class CFileHolder
{
private:
	FILE *m_File;
public:
	CFileHolder(FILE *file);
	~CFileHolder();
};

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
class CLicenseManager
{
private:

	unsigned char* m_FileBuffer;

	std::string m_DatabasePath;
	
	TPackagesList          m_PackagesList;
	TLicensedMaterialsList m_LicensedMaterialsList;
	TPropertiesList        m_PropertiesList;

	double m_PackagesDatabaseVersion;

	std::string  DecryptDatabaseFile(const std::string &FilePath);
	unsigned int ReadFileIntoBuffer (const std::string &FilePath);
	void 		 ParseMaterialsList (XMLElement* MaterialsNode,      TLicensedMaterialsList &MaterialsList, const std::string &PackageName);
	void 		 ParseProperties    (XMLElement* TrayPropertiesNode, const std::string &PackageName);
	TLMStatus    ParsePackagesXML   ();

public:

	CLicenseManager();
	~CLicenseManager();

	TLMStatus ReadDatabase              (const std::string &DatabasePath);
	bool      IsMaterialLicensed        (const std::string &MaterialName);
	bool      IsPropertyLicensed        (const TPropertyID PropertyID);
	double    GetPackagesDatabaseVersion();
	void      GetLicensedMaterials      (TMaterialsList *MaterialsList);
	
	std::string Status2String(TLMStatus Status);
};
