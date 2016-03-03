#ifndef LicenseManagerInterfaceH
#define LicenseManagerInterfaceH

#if defined(LIBRARY_EXPORT) 
#   define LIBAPI     __declspec(dllexport)
#   define CALLINGAPI __stdcall
#else
#   define LIBAPI     __declspec(dllimport)
#   define CALLINGAPI
#endif

const int MAX_MATERIALS_NUM      = 100;
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

struct ILicenseManager
{
public:

	virtual TLMStatus   __stdcall ReadDatabase              (const char* DatabasePath)      = 0;
	virtual bool        __stdcall IsMaterialLicensed        (const char* MaterialName)      = 0;
	virtual bool        __stdcall IsPropertyLicensed        (const TPropertyID PropertyID)  = 0;
	virtual float       __stdcall GetPackagesDatabaseVersion()                              = 0;
	virtual void        __stdcall GetLicensedMaterials      (TMaterialsList *MaterialsList) = 0;
};

extern "C" LIBAPI ILicenseManager* CALLINGAPI CreateLicenseManager(); 
extern "C" LIBAPI void             CALLINGAPI DeleteLicenseManager();

#endif
