/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT.                                                   *
 * Module: Licesne Manager                                          *
 * Module Description: Advanced Licensing implementation.           *
 *                                                                  *
 * Compilation: Standard C++ , BCB                                  *
 *                                                                  *
 * Author: Slava Chuhovich                                          *
 * Start date: 02/02/2011                                           *
 * Last upate:                                                      *
 ********************************************************************/

#pragma hdrstop
#include "LicenseManager.h"
#pragma package(smart_init)

#include "Q2RTApplication.h"
#include "HaspDefs.h"
#include "hasp_api_cpp_.h"
#include "QFile.h"
#include "GlobalDefs.h"

TMaintenanceMode AllModesTags [NUMBER_OF_QUALITY_MODES][NUM_OF_OPERATION_MODES];

CLicenseManager::CLicenseManager(const QString &Name, TLogFileTag LogTag)
	: CQComponent(Name)
{
	m_LogTag = LogTag;
	m_FileBuffer = NULL;
}

CLicenseManager::CLicenseManager(const QString &Name, QString MachineFamily, TLogFileTag LogTag)
	: CQComponent(Name)
{
	m_MachineFamily = MachineFamily;

	m_PackagesList.clear();
	m_LicensedMaterialsList.clear();
	m_LicensedDigitalMaterialsList.clear();
	m_LicensedModesList.clear();
	m_TrayPropertiesList.clear();
	m_AdditionalHostParamsList.clear();

	m_LogTag = LogTag;

	AllModesTags[HS_INDEX][SINGLE_MATERIAL_OPERATION_MODE]  = mmHighSpeedMode;
	AllModesTags[HS_INDEX][DIGITAL_MATERIAL_OPERATION_MODE] = mmDigitalMaterialMode;
	AllModesTags[HQ_INDEX][DIGITAL_MATERIAL_OPERATION_MODE]  = mmHighQualityMode;

    // This check is for the developer - log file must be initialized in application before License Manager
	if(! CQLog::IsInitialized())
		throw ELicenseManager(LM_ERR_LOG_INIT);

	try
	{
		ParsePackagesXML();

		CQLog::Write(m_LogTag, QFormatStr("LicenseManager: Packages: %s, Database version: %s",GetLicensedPackagesAsString().c_str(), m_PackagesDatabaseVersion.c_str()));
		CQEncryptedLog::Write(m_LogTag, QFormatStr("LicenseManager: Packages: %s, Database version: %s",GetLicensedPackagesAsString().c_str(), m_PackagesDatabaseVersion.c_str()));
	}
	catch(ELicenseManager &Err)
	{
		CQLog::Write(m_LogTag, QFormatStr("LicenseManager: %s", Err.GetErrorMsg().c_str()));
		throw ELicenseManager(Err.GetErrorMsg().c_str());
    }
	catch(...)
	{
		CQLog::Write(m_LogTag, QFormatStr("LicenseManager: %s", LM_ERR_OTHER));
		throw ELicenseManager(LM_ERR_OTHER);
	}
}

CLicenseManager::~CLicenseManager()
{
    m_PackagesList.clear();
	m_LicensedMaterialsList.clear();
	m_LicensedDigitalMaterialsList.clear();
	m_LicensedModesList.clear();
	m_TrayPropertiesList.clear();
	m_AdditionalHostParamsList.clear();

	Q_SAFE_DELETE_ARRAY(m_FileBuffer);
}

QString CLicenseManager::DecryptDatabaseFile(QString FilePath)
{
	unsigned int FileSize = ReadFileIntoBuffer(FilePath.c_str());
	if(FileSize == 0)
		throw ELicenseManager(LM_ERR_OPEN_FILE);

	int Feature = 0;
	Chasp hasp(ChaspFeature::fromFeature(Feature));
	try
	{
		haspStatus status = hasp.login(HASP_OBJET_VENDOR_CODE, HASP_LOCAL_SCOPE);
		CQLog::Write(m_LogTag, QFormatStr("LicenseManager: login, line: %d, status: %d", __LINE__, status));

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
			throw ELicenseManager(LM_ERR_LOGIN);
	}
	__finally
	{
		hasp.logout();
	}

	return "";
}

unsigned int CLicenseManager::ReadFileIntoBuffer(QString FilePath)
{
	QOSFileCreator OSFile;

	OSFile.CreateFile(FilePath,"rb");
	FILE *File = OSFile.toSTDIO();

	try
	{
		if(File != NULL)
		{
			fseek(File, 0, SEEK_END);
			long FileSize = ftell(File);
			rewind(File);

			m_FileBuffer = new BYTE[FileSize + 1];

			return fread(m_FileBuffer,sizeof(BYTE),FileSize,File);
		}
	}
	__finally
	{
		if(File != NULL)
		{
			fclose(File);
		}
    }

	return 0;
}

void CLicenseManager::ParsePackagesXML()
{
	tinyxml2::XMLDocument XMLDoc;

	QString XML = DecryptDatabaseFile(Q2RTApplication->AppFilePath.Value() + XML_DATABASE_PATH);

	XMLDoc.Parse(XML.c_str());

	XMLElement* RootNode = XMLDoc.RootElement();
	
	m_PackagesDatabaseVersion = RootNode->Attribute("version");

	XMLElement* MachineFamilyNode = RootNode->FirstChildElement(m_MachineFamily.c_str());
	XMLElement* MachineModelNode  = MachineFamilyNode->FirstChildElement("Machine");

	QString MachineModel = MachineModelNode->Attribute("model");

	// Find our machine model(type)
	while(Q2RTApplication->GetMachineTypeAsString().compare(MachineModel.c_str()) != 0)
	{
		MachineModelNode = MachineModelNode->NextSiblingElement("Machine");
		
		if(! MachineModelNode)
			throw ELicenseManager(LM_ERR_NO_PACKAGE);
			
		MachineModel = MachineModelNode->Attribute("model");
	}
	
	XMLElement* PackageNode = MachineModelNode->FirstChildElement("Package");

	while(PackageNode)
	{
		// Get package name information
		QString PackageFeature = PackageNode->Attribute("feature");
		QString PackageName    = PackageNode->Attribute("name");
		QString PackageType    = PackageNode->Attribute("type");
		
		// Feature check in HASP dongle
		int Feature = QStrToInt(PackageFeature.c_str());
		Chasp hasp(ChaspFeature::fromFeature(Feature));
		try
		{
			haspStatus status = hasp.login(HASP_OBJET_VENDOR_CODE, HASP_LOCAL_SCOPE);
			CQLog::Write(m_LogTag, QFormatStr("LicenseManager: login, line: %d, status: %d", __LINE__, status));
		
			if(! HASP_SUCCEEDED(status))
			{
                // If package is not licensed, continue to next package
				PackageNode = PackageNode->NextSiblingElement("Package");
				continue;
			}
		}				
		__finally
		{
			hasp.logout();
		}

		// Add package to the list
		TLicensedPackage LicensedPackage;
		LicensedPackage.PackageType = GetPackageType(QStrToInt(PackageType.c_str()));
		LicensedPackage.PackageName = PackageName.c_str();
		LicensedPackage.HaspFeature = Feature;
		m_PackagesList.push_back(LicensedPackage);

		// Get materials list
		XMLElement* MaterialsNode = PackageNode->FirstChildElement("Materials");
		ParseMaterialsList(MaterialsNode, m_LicensedMaterialsList, PackageName.c_str());

		// Get digital materials list
		XMLElement* DigitalMaterialsNode = PackageNode->FirstChildElement("DigitalMaterials");
		ParseMaterialsList(DigitalMaterialsNode, m_LicensedDigitalMaterialsList, PackageName.c_str());

		// Get tray properties list
		XMLElement* TrayPropertiesNode = PackageNode->FirstChildElement("TrayProperties");
        ParseTrayProperties(TrayPropertiesNode, PackageName.c_str());

		// Get operation modes list
		XMLElement* OperationModesNode = PackageNode->FirstChildElement("OperationModes");
        ParseOperationModes(OperationModesNode, PackageName.c_str());

		// Get additional host parameters as XML string (for future use)
		XMLElement* AdditionalHostParamsNode = PackageNode->FirstChildElement("AdditionalHostParams");
		ParseAdditionalHostParams(AdditionalHostParamsNode, PackageName.c_str());
		
		// Check for next package
		PackageNode = PackageNode->NextSiblingElement("Package");
	}
}

void CLicenseManager::ParseAdditionalHostParams(XMLElement* AdditionalHostParamsNode, const QString &PackageName)
{
	if(AdditionalHostParamsNode)
	{
		TAdditionalHostParams AdditionalHostParams;

		AdditionalHostParams.PackageName             = PackageName.c_str();
		AdditionalHostParams.AdditionalHostParamsXML = AdditionalHostParamsNode->GetText();

		m_AdditionalHostParamsList.push_back(AdditionalHostParams);
	}
}

void CLicenseManager::ParseOperationModes(XMLElement* OperationModesNode, const QString &PackageName)
{
	if(OperationModesNode)
	{
		TLicensedModeItem LicensedModeItem;

		XMLElement* ModeNode = OperationModesNode->FirstChildElement("Mode");
		while(ModeNode)
		{
			QString Mode = ModeNode->GetText();

			LicensedModeItem.PackageName   = PackageName.c_str();
			LicensedModeItem.ModeShortName = Mode.c_str();
			LicensedModeItem.ModeFullName  = ShortModeNameToFullName(Mode.c_str());
			LicensedModeItem.ModeTag       = ShortModeNameToTag(LicensedModeItem.ModeShortName);

			TLicensedModesList::iterator it = m_LicensedModesList.begin();
			for(; it < m_LicensedModesList.end() ; ++it)
				if(it->ModeShortName.compare(LicensedModeItem.ModeShortName) == 0)
					break;

			if(it == m_LicensedModesList.end())
				m_LicensedModesList.push_back(LicensedModeItem);

			ModeNode = ModeNode->NextSiblingElement("Mode");
		}
	}
}

void CLicenseManager::ParseTrayProperties(XMLElement* TrayPropertiesNode, const QString &PackageName)
{
	if(TrayPropertiesNode)
	{
		TTrayProperty TrayProperty;

		XMLElement* PropertyNode = TrayPropertiesNode->FirstChildElement("Property");
		while(PropertyNode)
		{
			QString PropertyName = PropertyNode->Attribute("name");
			QString BitMask      = PropertyNode->Attribute("bitMask");

			TrayProperty.PackageName  = PackageName.c_str();
			TrayProperty.PropertyName = PropertyName.c_str();
			TrayProperty.BitMask      = QStrToInt(BitMask.c_str());

			TTrayPropertiesList::iterator it = m_TrayPropertiesList.begin();
			for(; it < m_TrayPropertiesList.end() ; ++it)
				if(it->PropertyName.compare(TrayProperty.PropertyName) == 0)
					break;

			if(it == m_TrayPropertiesList.end())
				m_TrayPropertiesList.push_back(TrayProperty);

			PropertyNode = PropertyNode->NextSiblingElement("Property");
		}
	}
}

void CLicenseManager::ParseMaterialsList(XMLElement* MaterialsNode, TLicensedMaterialsList &MaterialsList, const QString &PackageName)
{
	if(MaterialsNode)
	{
		TLicensedMaterialItem LicensedMaterialItem;

		XMLElement* MaterialNode = MaterialsNode->FirstChildElement("Material");
		while(MaterialNode)
		{
			QString Material = MaterialNode->GetText();

			LicensedMaterialItem.PackageName  = PackageName.c_str();
			LicensedMaterialItem.MaterialName = Material.c_str();

			TLicensedMaterialsList::iterator it = MaterialsList.begin();
			for(; it < MaterialsList.end() ; ++it)
				if(it->MaterialName.compare(LicensedMaterialItem.MaterialName) == 0)
					break;

			if(it == MaterialsList.end())
				MaterialsList.push_back(LicensedMaterialItem);

			MaterialNode = MaterialNode->NextSiblingElement("Material");
		}
	}
}

bool CLicenseManager::IsMaterialLicensed(QString MaterialName)
{
	for (TLicensedMaterialsList::iterator it = m_LicensedMaterialsList.begin() ; it != m_LicensedMaterialsList.end() ; ++it)
		if(it->MaterialName.compare(MaterialName) == 0)
			return true;

	return false;
}

bool CLicenseManager::IsModeLicensed(QString ModeName)
{
	 for (TLicensedModesList::iterator it = m_LicensedModesList.begin() ; it != m_LicensedModesList.end() ; ++it)
		if(it->ModeShortName.compare(ModeName) == 0)
			return true;

	return false;
}

TPackagesList CLicenseManager::GetLicensedPackages()
{
	return m_PackagesList;
}

QString CLicenseManager::GetLicensedPackagesAsString()
{
	QString Packages = "";

	for (TPackagesList::iterator it = m_PackagesList.begin() ; it != m_PackagesList.end() ; ++it)
		Packages += it->PackageName + ", ";

	if(! Packages.empty())
		Packages.erase(Packages.end() - 2, Packages.end());
	return Packages;
}

TLicensedMaterialsList CLicenseManager::GetLicensedMaterials()
{
	return m_LicensedMaterialsList;
}

TLicensedMaterialsList CLicenseManager::GetLicensedDigitalMaterials()
{
	return m_LicensedDigitalMaterialsList;
}

TTrayPropertiesList CLicenseManager::GetLicensedTrayProperties()
{
	return m_TrayPropertiesList;
}

TAdditionalHostParamsList CLicenseManager::GetLicensedAdditionalHostParams()
{
	return m_AdditionalHostParamsList;
}

TLicensedModesList CLicenseManager::GetLicensedModes()
{
	return m_LicensedModesList;
}

QString CLicenseManager::GetPackagesDatabaseVersion()
{
    return m_PackagesDatabaseVersion;
}

TPackageType CLicenseManager::GetPackageType(int PackageTypeNum)
{
	TPackageType type = ptBasic;

	switch(PackageTypeNum)
	{
		case 0:
			type = ptBasic;
			break;

		case 1:
			type = ptPro;
			break;

		default:
			type = ptBasic;
			break;
	}

	return type;
}

bool CLicenseManager::IsProMachine()
{
    for (TPackagesList::iterator it = m_PackagesList.begin() ; it != m_PackagesList.end() ; ++it)
		if(it->PackageType == ptPro)
			return true;
	return false;
}

QString CLicenseManager::ShortModeNameToFullName(QString ShortName)
{
	if(ShortName.compare("Emulation") == 0)
		return "Emulation";

	int QualityMode, OperationMode;
	GetModeFromShortStr(ShortName, QualityMode, OperationMode);
	return GetModeStr(QualityMode, OperationMode);
}

TMaintenanceMode CLicenseManager::ShortModeNameToTag(QString ShortName)
{
	int QualityMode, OperationMode;
	GetModeFromShortStr(ShortName, QualityMode, OperationMode);
	return AllModesTags[QualityMode][OperationMode];
}

//---------------------------------------------------------------------------

CLicenseManagerDummy::CLicenseManagerDummy(const QString &Name, TLogFileTag LogTag)
	: CLicenseManager(Name, LogTag)
{
    // This check is for the developer - log file must be initialized in application before License Manager
	if(! CQLog::IsInitialized())
		throw ELicenseManager(LM_ERR_LOG_INIT);
		
	ParsePackagesXML();
}

CLicenseManagerDummy::~CLicenseManagerDummy()
{
}

void CLicenseManagerDummy::ParsePackagesXML()
{
	TLicensedPackage 	  LicensedPackage;
	TLicensedMaterialItem LicensedMaterialItem;
	TLicensedMaterialItem LicensedDigitalMaterialItem;
	TLicensedModeItem     LicensedModeItem;
	TTrayProperty         TrayProperty;
	TAdditionalHostParams AdditionalHostParams;
	
	LicensedPackage.PackageName = "Emulation";
	LicensedPackage.HaspFeature = -1;	
	
	LicensedMaterialItem.PackageName  = "Emulation";
	LicensedMaterialItem.MaterialName = "Emulation";

	LicensedDigitalMaterialItem.PackageName  = "Emulation";
	LicensedDigitalMaterialItem.MaterialName = "Emulation";

	LicensedModeItem.PackageName   = "Emulation";
	LicensedModeItem.ModeShortName = "Emulation";
	LicensedModeItem.ModeFullName  = "Emulation";

	TrayProperty.PackageName  = "Emulation";
	TrayProperty.PropertyName = "Emulation";

	AdditionalHostParams.PackageName             = "Emulation";
	AdditionalHostParams.AdditionalHostParamsXML = "Emulation";

	m_PackagesList.push_back(LicensedPackage);
	m_LicensedMaterialsList.push_back(LicensedMaterialItem);
	m_LicensedDigitalMaterialsList.push_back(LicensedDigitalMaterialItem);	
	m_LicensedModesList.push_back(LicensedModeItem);
	m_TrayPropertiesList.push_back(TrayProperty);
	m_AdditionalHostParamsList.push_back(AdditionalHostParams);
}

bool CLicenseManagerDummy::IsMaterialLicensed(QString MaterialName)
{
    MaterialName = "";
	return true;
}

bool CLicenseManagerDummy::IsModeLicensed(QString ModeName)
{
    ModeName = "";
	return true;
}

QString CLicenseManagerDummy::GetPackagesDatabaseVersion()
{
	return "Emulation";
}

QString CLicenseManagerDummy::GetLicensedPackagesAsString()
{
	return "Emulation";
}

bool CLicenseManagerDummy::IsProMachine()
{
    return false;
}
