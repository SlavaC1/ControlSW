#ifndef GlobalDefsH
#define GlobalDefsH

#include <string>
#include <vector>

typedef std::string QString;

// XML node names
const char* N_ROOT          = "Project";
const char* N_MACHINE       = "Machine";
const char* N_PACKAGE       = "Package";
const char* N_MATERIALS     = "Materials";
const char* N_DM_MATERIALS  = "DigitalMaterials";
const char* N_MODES         = "OperationModes";
const char* N_MATERIAL_ITEM = "Material";
const char* N_MODE_ITEM     = "Mode";

// XML attribute names
const char* A_MACHINE_MODEL = "model";
const char* A_PACK_FEATURE  = "feature";
const char* A_PACK_NAME     = "name";  

// Objet vendor license key
const unsigned char HASP_OBJET_VENDOR_CODE[] =
  "jq7BZNARd9Jn+quB7ygHVmxDOLZ84KpMsnydBRtHHtLx9JLzHKtTTme9cQ9N6c5HHQu/S4VJHFIFHg"
  "CocxTQFKdL86Z41vcu31BrzeClxRGactl10LmjQSDYx+fnGvE12x1ET98oXahjmGLEAZlmbcmrQc94"
  "/c94zVC2fZrt4sdgzJXHauX42q51Pz2fIjQZ0pe3LDNPbQI5ROuKxXcCHtyeRZL7lW+L1G3C1SYgoi"
  "A+3oUmrIKqr/iJZGwjPg8w0AmFUuuCq2CO6SOtWo4KWIZNVfNceuJjbzETeHXWK4pxkket1tHNGERz"
  "sJ9h6A0gI+Og4vR7JxPjuUEwyRaLhBjwKP7V8+dqrN09QbUrJrFQJMrAt8IUiV/MSrPDIEB+QrPqkx"
  "TR3cYIQDHJ6GAyPooTQqXtyFrEKS9L0HZ309qQBK8Wo5noyTE+RmpklAW8xemW6Oh2uFuTCNwwZsZT"
  "15iT1JBe6TmZtfVP35ycdCgvdUa07TShfZpHHB1khN/brd6XKwMapvtoP+agxW1cMoRCXCx155oIZV"
  "gqb3P03QsereK23VyFPc2Vag3+Zh1W6CnFyd+7kn92gSy7JfKOmNveVPj94pdl9SUg6cO0FulSL2dN"
  "flYtkk7RLGObISPyagrIlXFyppD3dqMnQANEpkulsGcTERxICcve5l+bKlU+E7IBW5Px2NJb6T4pd+"
  "7Rpop5f1ROCw9caMTj7JrVeJc/zJ6yPh/Lydb5u7bQ1xWMoHay5P9hZbWVEz4JRu1oxp6E1vqjTwV7"
  "Wh6N81sCs9fZdLM73irCylAJOGc2uDI5ezeJFlJ7p7GHRKl4u5sDCXdLOOSmzC94NuIxu9nt5kO8ae"
  "PPeu/1KLo+IAtZsIZTUNNXBs4yKLUGBI47nJG54aqbUMUKRNw1vuZOuBZ4TJDvI765qx2O+M1ndnjO"
  "IWU=";

// Hasp login scope for local PC only   
const char HASP_LOCAL_SCOPE[] =
    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
	"<haspscope>"
	"	<hasp type=\"HASP-HL\" >"
	"    	<license_manager hostname =\"localhost\" />"
	"	</hasp>"
	"</haspscope>";  

struct TLicensedModeItem
{
	QString PackageName;
	QString ModeName;
};

struct TLicensedMaterialItem
{
	QString PackageName;
	QString MaterialName;
};

typedef std::vector<TLicensedMaterialItem> TLicensedMaterialsList;
typedef std::vector<TLicensedModeItem>     TLicensedModesList;

struct TLicensedPackage
{
	QString PackageName;
	int     HaspFeature;
	TLicensedMaterialsList PackageMaterialsList;
	TLicensedMaterialsList PackageDigitalMaterialsList;
	TLicensedModesList     PackageModesList;
};

typedef std::vector<TLicensedPackage> TPackagesList;

struct TLicensedMachine
{
	QString 	           MachineModelName;
	TPackagesList          PackagesList;
	TLicensedMaterialsList MachineMaterialsList;
	TLicensedMaterialsList MachineDigitalMaterialsList;
	TLicensedModesList     MachineModesList;
};

typedef std::vector<TLicensedMachine> TMachinesList;


#endif
