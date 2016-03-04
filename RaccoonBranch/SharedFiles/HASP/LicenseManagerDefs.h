#ifndef LicenseManagerDefsH
#define LicenseManagerDefsH

class ELicenseManager : public EQException
{
  public:
    ELicenseManager(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

const QString LM_ERR_OPEN_FILE  = "Cannot find or open packages file. Contact your Objet service provider.";
const QString LM_ERR_LOGIN      = "Cannot decrypt packages file. HASP dongle is missing.";
const QString LM_ERR_LOG_INIT   = "Log file not initialized.";
const QString LM_ERR_NO_PACKAGE = "Printing is disabled because license information is missing from HASP,"
							      " or is incompatible\nwith your current package file. Contact your "
							      "Objet service provider.";
const QString LM_ERR_OTHER      = "Error occured while parsing packages database file";

const QString XML_DATABASE_PATH = "Configs\\Packages.dat";

enum TMaintenanceMode
{
	mmDefaultMode,
	mmHighQualityMode,
	mmHighSpeedMode,
	mmDigitalMaterialMode
};

enum TModeType
{
	mdtOperationMode,
	mdtAuxiliaryMode
};

enum TPackageType
{
	ptBasic,
	ptPro
};

struct TLicensedModeItem
{
	QString PackageName;
	QString ModeShortName;
	QString ModeFullName;
	TMaintenanceMode ModeTag;
};

struct TLicensedMaterialItem
{
	QString PackageName;
	QString MaterialName;
};

struct TTrayProperty
{
	QString PackageName;
	QString PropertyName;
	int     BitMask;
};

struct TAdditionalHostParams
{
	QString PackageName;
	QString AdditionalHostParamsXML;
};

struct TLicensedPackage
{
	TPackageType PackageType;
	QString      PackageName;
	int          HaspFeature;
};

typedef std::vector<TLicensedPackage>      TPackagesList;
typedef std::vector<TLicensedMaterialItem> TLicensedMaterialsList;
typedef std::vector<TLicensedMaterialItem> TLicensedDigitalMaterialsList;
typedef std::vector<TLicensedModeItem>     TLicensedModesList;
typedef std::vector<TTrayProperty>         TTrayPropertiesList;
typedef std::vector<TAdditionalHostParams> TAdditionalHostParamsList;


#endif
