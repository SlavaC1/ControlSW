//---------------------------------------------------------------------------

#ifndef LicenseManagerH
#define LicenseManagerH

#include "QComponent.h"
#include "tinyxml2.h"
#include <vector>
#include "LicenseManagerDefs.h"
#include "QLogFile.h"

using namespace tinyxml2;

//---------------------------------------------------------------------------

class CLicenseManager : public CQComponent
{
private:

	BYTE *m_FileBuffer;

	QString DecryptDatabaseFile(QString FilePath);
	unsigned int ReadFileIntoBuffer(QString FilePath);

	QString ShortModeNameToFullName(QString ShortName);
	TMaintenanceMode ShortModeNameToTag(QString ShortName);

	void ParseMaterialsList(XMLElement* MaterialsNode, TLicensedMaterialsList &MaterialsList, const QString &PackageName);
	void ParseTrayProperties(XMLElement* TrayPropertiesNode, const QString &PackageName);
	void ParseOperationModes(XMLElement* OperationModesNode, const QString &PackageName);
	void ParseAdditionalHostParams(XMLElement* AdditionalHostParamsNode, const QString &PackageName);

protected:

	TPackagesList             m_PackagesList;
	TLicensedMaterialsList    m_LicensedMaterialsList;
	TLicensedMaterialsList    m_LicensedDigitalMaterialsList;
	TLicensedModesList        m_LicensedModesList;
	TTrayPropertiesList       m_TrayPropertiesList;
	TAdditionalHostParamsList m_AdditionalHostParamsList;

	QString m_PackagesDatabaseVersion;
	QString m_MachineFamily;

	TLogFileTag m_LogTag;

	virtual void ParsePackagesXML();
	virtual TPackageType GetPackageType(int PackageTypeNum);

public:
	
	CLicenseManager(const QString &Name, TLogFileTag LogTag);
	CLicenseManager(const QString &Name, QString MachineFamily, TLogFileTag LogTag);
	virtual ~CLicenseManager();

	virtual bool IsMaterialLicensed(QString MaterialName);
	virtual bool IsModeLicensed(QString ModeName);

	TPackagesList             GetLicensedPackages();
	TLicensedMaterialsList    GetLicensedMaterials();
	TLicensedMaterialsList    GetLicensedDigitalMaterials();
	TLicensedModesList        GetLicensedModes();
	TTrayPropertiesList       GetLicensedTrayProperties();
	TAdditionalHostParamsList GetLicensedAdditionalHostParams();

	virtual QString GetPackagesDatabaseVersion();
	virtual QString GetLicensedPackagesAsString();

    // Relevant to Alaris machines only
	virtual bool IsProMachine();
};

class CLicenseManagerDummy : public CLicenseManager
{
private:

	void ParsePackagesXML();
	
public:

	CLicenseManagerDummy(const QString &Name, TLogFileTag LogTag);
	virtual ~CLicenseManagerDummy();
	bool IsMaterialLicensed(QString MaterialName);
	bool IsModeLicensed(QString ModeName);
	QString GetPackagesDatabaseVersion();
	QString GetLicensedPackagesAsString();
	bool IsProMachine();
};

//---------------------------------------------------------------------------
#endif
