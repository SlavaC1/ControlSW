

// Stratasys vendor license key
const unsigned char HASP_SSYS_VENDOR_CODE[] =
	"xSXE7L1aWtkKTbnPkh/u2qqMeZ3edBFNvSz1jKxNiIQn4XlHPgKdt+vjy3j49baPpiWsTAmwKaeE4Jjk"
	"hUfciCC6O/wlRyr8p8Jh9y4TN3SsPZ2eSmYve7yBXMS+6K3yWMtBclwnnt1EjCzFrawj/x0Ymi7TRdfy"
	"E8ia6wyjeFRH1R7ARWCeBvmg9vKCc6VPwwQWjQmwXw5WtVsExXVJKuG5B5Ha5M+tE/P/ssd3q0RZqAb6"
	"i4Z6ecYPvKhrPcwNTBp3gUzqXSX2tuu1V6zl7THxSz5TX+JWR85k5GgTJup9e0E42KVVMOWoxqp3ehLA"
	"+1IJhwVXSMbeeu0x8Aqn6jN0jV+3uEOrJhuCawSP38/VyTUlS+26E7RUjTT4bej5U7dO8s6+tdkn331E"
	"G7rU+EeTB9RgGxoP3hc3FW1fbrrH4PfDEXm5bJ3XOzpndtDzMIdHSvm2h+XZIJEH4Giptz+hIMqQ+N4A"
	"7imY4Ut3BgiI32WNjKDnZgpQTH313QgtPs3BemwhtsGNS3Wpvzwl6IuuN70iOodO8HpZqoRG8Wq9+vc5"
	"G2nwYIadc7wsJ9BrPW/XDFX6VfQM6uWHg7z2hRIAQB4wv/6DKKXIr+jZjbAKkYPS3PDERADA0szG1N1A"
	"eNuIsAZD+rZyZpyOItY4gy6RrLrxK+ybRWpmvB8nC5CO4AhTTWcmgR8OPbbsS+U4/zurEOw3EgSUbn9e"
	"FdZL4T2YDFhtcsL/03Ci2quQrmGS24d3bgFhDeQDkiFQWcjLXQlDVartVQXtF5Y/txmoJkk7rgzlNcRr"
	"Y+Qi2uY3Bb3anfqxEkQ7EQbx0TgL3WhBczeaW2X0J4UStqIrflksdwTjfq9tcs9lCCYwHEE8G+tHN5Sw"
	"dZOa9hHEXvGzbmhir99TKfAqVUR6P2NMPpdomqh0o6yxD2nJyV6dNoYKuMjdSHOSh2dYGY8S/2mZp18u"
	"cPoVvdxzEvfxnVqx3mOE5w==";

// Hasp login scope for local PC only   
const std::string HASP_LOCAL_SCOPE =
	"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
	"<haspscope>"
	"	<hasp configuration=\"sentinelhl.driverless\">"
	"    	<license_manager hostname =\"localhost\" />"
	"	</hasp>"
	"</haspscope>";

const std::string LM_ERR_OPEN_FILE  = "Cannot find or open packages file. Contact your service provider.";
const std::string LM_ERR_LOGIN      = "Cannot decrypt packages file. HASP dongle is missing.";
const std::string LM_ERR_NO_PACKAGE = "Printing is disabled because license information is missing from HASP,"
									  " or is incompatible\nwith your current package file. Contact your "
							          "service provider.";
const std::string LM_ERR_OTHER      = "Error occurred while parsing packages database file";

const int LM_ERRORS_NUM = 4;


const int MAX_MATERIALS_NUM = 100;
const int MAX_MATERIAL_NAME_SIZE = 20;

struct TMaterialsList
{
	TMaterialsList() : MaterialsNum(0){}

	struct TMaterial
	{
		TMaterial() : NameLength(0){}

		int  NameLength;
		char MaterialName[MAX_MATERIAL_NAME_SIZE];
	};

	int       MaterialsNum;
	TMaterial LicensedMaterial[MAX_MATERIALS_NUM];
};

enum TPropertyID
{
	propSupport,
	propVoid,
	propAllMatAllowed,
	propImportBMP,
	propConnectivityPFF,
	LICENSED_PROPS_NUM
};

enum TLMStatus
{
	LM_STATUS_OPEN_FILE_ERR,
	LM_STATUS_LOGIN_ERR,
	LM_STATUS_NO_PACKAGE_ERR,
	LM_STATUS_OTHER_ERR,
	LM_STATUS_OK
};

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

