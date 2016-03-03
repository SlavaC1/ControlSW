#ifndef LicenseManagerDefsH
#define LicenseManagerDefsH

#include "HaspDefs.h"

const std::string LM_ERR_OPEN_FILE  = "Cannot find or open packages file. Contact your service provider.";
const std::string LM_ERR_LOGIN      = "Cannot decrypt packages file. HASP dongle is missing.";
const std::string LM_ERR_NO_PACKAGE = "Printing is disabled because license information is missing from HASP,"
									  " or is incompatible\nwith your current package file. Contact your "
							          "service provider.";
const std::string LM_ERR_OTHER      = "Error occured while parsing packages database file";

struct TLicensedProperty
{
	std::string PackageName;
	std::string PropertyName;
	int         ID;
	bool        State; 
};

struct TLicensedMaterialItem
{
	std::string PackageName;
	std::string MaterialName;
};

struct TLicensedPackage
{
	std::string  PackageName;
	int          HaspFeature;
};

typedef std::vector<TLicensedPackage>      TPackagesList;
typedef std::vector<TLicensedMaterialItem> TLicensedMaterialsList;
typedef std::vector<TLicensedProperty>     TPropertiesList;


#endif
