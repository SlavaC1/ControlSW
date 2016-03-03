/********************************************************************
 *                        Stratasys LTD.                            *
 *                        ---------------------                     *
 * Project: Advanced Licensing library for Objet Studio.            *
 * Module: Licesne Manager                                          *
 * Module Description: Advanced Licensing implementation.           *
 *                                                                  *
 * Compilation: Standard C++                                        *
 *                                                                  *
 * Author: Slava Chuhovich                                          *
 * Start date: 26/02/2014                                           *
 * Last upate: 08/12/2015                                           *
 ********************************************************************/

#pragma warning (disable : 4996) // Disable fopen and strcpy deprecation warning

#include "LicenseManager.h"
#include <stdio.h>

const std::string ErrorsList[] =
{
	LM_ERR_OPEN_FILE,
	LM_ERR_LOGIN,
	LM_ERR_NO_PACKAGE,
	LM_ERR_OTHER
};

ELicenseManager::ELicenseManager(const char* Msg, const int ErrCode) :
	m_Msg(Msg), m_ErrCode(ErrCode)
{}

ELicenseManager::ELicenseManager(const std::string &Msg, const int ErrCode) :
	m_Msg(Msg), m_ErrCode(ErrCode)
{}

const char* ELicenseManager::what() const throw()
{
	return m_Msg.c_str();
}

const int ELicenseManager::GetErrorCode() const throw()
{
	return m_ErrCode;
}

// -------------------------------------------------------------------------------------------------------

CHaspHolder::CHaspHolder(const Chasp &hasp) : m_Hasp(hasp)
{}

CHaspHolder::~CHaspHolder()
{
	m_Hasp.logout();
}

CFileHolder::CFileHolder(FILE *file) : m_File(file)
{}

CFileHolder::~CFileHolder()
{
	if (m_File)
		fclose(m_File);
}

// -------------------------------------------------------------------------------------------------------

CLicenseManager::CLicenseManager()
{
	m_PackagesList.clear();
	m_LicensedMaterialsList.clear();
	m_PropertiesList.clear();

	m_PackagesDatabaseVersion = 0.0;
}

CLicenseManager::~CLicenseManager()
{
	m_PackagesList.clear();
	m_LicensedMaterialsList.clear();
	m_PropertiesList.clear();   

	delete[] m_FileBuffer;
}

TLMStatus CLicenseManager::ReadDatabase(const std::string &DatabasePath)
{
	m_PackagesList.clear();
	m_LicensedMaterialsList.clear();
	m_PropertiesList.clear();

	m_DatabasePath = DatabasePath;
	
	try
	{  
		return ParsePackagesXML();
	}
	catch(ELicenseManager &Err)
	{
		return static_cast<TLMStatus>(Err.GetErrorCode());
	}
	catch(...)
	{
		return LM_STATUS_OTHER_ERR;
	}
}

std::string CLicenseManager::DecryptDatabaseFile(const std::string &FilePath)
{
	unsigned int FileSize = ReadFileIntoBuffer(FilePath.c_str());
	if(FileSize == 0)
		throw ELicenseManager(LM_ERR_OPEN_FILE, LM_STATUS_OPEN_FILE_ERR);

	int Feature = 0;
	Chasp hasp(ChaspFeature::fromFeature(Feature));
	CHaspHolder HaspHolder(hasp);
	
	haspStatus status = hasp.login(HASP_SSYS_VENDOR_CODE, HASP_LOCAL_SCOPE);

	if(HASP_SUCCEEDED(status))
	{
		status = hasp.decrypt(m_FileBuffer, FileSize);
		if(HASP_SUCCEEDED(status))
		{
			m_FileBuffer[FileSize] = '\0';	
			return reinterpret_cast<const char*>(m_FileBuffer);
		}
	}
	else
		throw ELicenseManager(LM_ERR_LOGIN, LM_STATUS_LOGIN_ERR);	

	return "";
}

unsigned int CLicenseManager::ReadFileIntoBuffer(const std::string &FilePath)
{
	FILE *File = fopen(FilePath.c_str(),"rb");
	CFileHolder FileHolder(File);
	
	if(File != NULL)
	{
		fseek(File, 0, SEEK_END);
		long FileSize = ftell(File);
		rewind(File);

		m_FileBuffer = new unsigned char[FileSize + 1];

		return fread(m_FileBuffer, sizeof(BYTE), FileSize, File);
	}	

	return 0;
}

TLMStatus CLicenseManager::ParsePackagesXML()
{
	tinyxml2::XMLDocument XMLDoc;

	std::string XML = DecryptDatabaseFile(m_DatabasePath);

	XMLDoc.Parse(XML.c_str());

	XMLElement* RootNode = XMLDoc.RootElement();

	std::string DatabaseVersion = RootNode->Attribute("version");
	m_PackagesDatabaseVersion   = atof(DatabaseVersion.c_str());
	
	XMLElement* PackageNode = RootNode->FirstChildElement("Package");

	while(PackageNode)
	{
		// Get package name information
		std::string PackageFeature = PackageNode->Attribute("feature");
		std::string PackageName    = PackageNode->Attribute("name");
		
		// Feature check in HASP dongle
		int Feature = atoi(PackageFeature.c_str());
		Chasp hasp(ChaspFeature::fromFeature(Feature));
		CHaspHolder HaspHolder(hasp);
		
		haspStatus status = hasp.login(HASP_SSYS_VENDOR_CODE, HASP_LOCAL_SCOPE);
		
		if(! HASP_SUCCEEDED(status))
		{
			// If package is not licensed, continue to next package
			PackageNode = PackageNode->NextSiblingElement("Package");
			continue;
		}		

		// Add package to the list
		TLicensedPackage LicensedPackage;
		LicensedPackage.PackageName = PackageName.c_str();
		LicensedPackage.HaspFeature = Feature;
		m_PackagesList.push_back(LicensedPackage);

		// Get materials list
		XMLElement* MaterialsNode = PackageNode->FirstChildElement("Materials");
		ParseMaterialsList(MaterialsNode, m_LicensedMaterialsList, PackageName.c_str());

		// Get properties list
		XMLElement* PropertiesNode = PackageNode->FirstChildElement("Properties");
		ParseProperties(PropertiesNode, PackageName.c_str());
		
		// Check for next package
		PackageNode = PackageNode->NextSiblingElement("Package");
	}

	if(m_PackagesList.empty())
		return LM_STATUS_NO_PACKAGE_ERR;
	else
		return LM_STATUS_OK;	
}

void CLicenseManager::ParseProperties(XMLElement* PropertiesNode, const std::string &PackageName)
{
	if(PropertiesNode)
	{
		TLicensedProperty Property;

		XMLElement* PropertyNode = PropertiesNode->FirstChildElement("Property");
		while(PropertyNode)
		{
			std::string PropertyID   = PropertyNode->Attribute("ID");
			std::string PropertyName = PropertyNode->Attribute("name");
			std::string State        = PropertyNode->Attribute("state");

			Property.PackageName  = PackageName.c_str();
			Property.ID           = atoi(PropertyID.c_str());
			Property.PropertyName = PropertyName.c_str();
			Property.State        = (State == "true") ? true : false;

			// Find if property already in the list
			TPropertiesList::iterator it = m_PropertiesList.begin();
			for(; it < m_PropertiesList.end() ; ++it)
				if(it->PropertyName.compare(Property.PropertyName) == 0)
					break;

			 // Add property if it's not already present
			if(it == m_PropertiesList.end())
				m_PropertiesList.push_back(Property);

			PropertyNode = PropertyNode->NextSiblingElement("Property");
		}
	}
}

void CLicenseManager::ParseMaterialsList(XMLElement* MaterialsNode, TLicensedMaterialsList &MaterialsList, const std::string &PackageName)
{
	if(MaterialsNode)
	{
		TLicensedMaterialItem LicensedMaterialItem;

		XMLElement* MaterialNode = MaterialsNode->FirstChildElement("Material");
		while(MaterialNode)
		{
			std::string Material = MaterialNode->GetText();

			LicensedMaterialItem.PackageName  = PackageName.c_str();
			LicensedMaterialItem.MaterialName = Material.c_str();

			// Find if a material already in the list
			TLicensedMaterialsList::iterator it = MaterialsList.begin();
			for(; it < MaterialsList.end() ; ++it)
				if(it->MaterialName.compare(LicensedMaterialItem.MaterialName) == 0)
					break;

			// Add the material if it's not already present
			if(it == MaterialsList.end())
				MaterialsList.push_back(LicensedMaterialItem);

			MaterialNode = MaterialNode->NextSiblingElement("Material");
		}
	}
}

bool CLicenseManager::IsMaterialLicensed(const std::string &MaterialName)
{
	// If all materials are allowed, mark any material as 'licensed'
	if(IsPropertyLicensed(propAllMatAllowed))
		return true;

	for(TLicensedMaterialsList::const_iterator it = m_LicensedMaterialsList.begin() ; it != m_LicensedMaterialsList.end() ; ++it)
		if(it->MaterialName.compare(MaterialName) == 0)
			return true;

	return false;
}

bool CLicenseManager::IsPropertyLicensed(const TPropertyID PropertyID)
{
	if(PropertyID >= propSupport && PropertyID < LICENSED_PROPS_NUM)
		for(TPropertiesList::const_iterator it = m_PropertiesList.begin() ; it != m_PropertiesList.end() ; ++it)
			if(it->ID == PropertyID && it->State)
				return true;

	return false;
}

double CLicenseManager::GetPackagesDatabaseVersion()
{
	return m_PackagesDatabaseVersion;
}

void CLicenseManager::GetLicensedMaterials(TMaterialsList *MaterialsList)
{
	MaterialsList->MaterialsNum = m_LicensedMaterialsList.size();

	for(TLicensedMaterialsList::iterator it = m_LicensedMaterialsList.begin(); it != m_LicensedMaterialsList.end(); ++it)
	{
		int index = std::distance(m_LicensedMaterialsList.begin(), it);

		strcpy(MaterialsList->LicensedMaterial[index].MaterialName, it->MaterialName.c_str());

		MaterialsList->LicensedMaterial[index].NameLength = it->MaterialName.size();
	}
}

std::string CLicenseManager::Status2String(TLMStatus Status)
{
	if (Status < LM_ERRORS_NUM)	
		return ErrorsList[Status];	
	else	
		return "";	
}
