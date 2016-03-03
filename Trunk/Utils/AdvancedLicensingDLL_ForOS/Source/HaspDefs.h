
#ifndef HaspDefsH
#define HaspDefsH

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
const char* HASP_LOCAL_SCOPE =
    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
	"<haspscope>"
	"	<hasp type=\"HASP-HL\" >"
	"    	<license_manager hostname =\"localhost\" />"
	"	</hasp>"
	"</haspscope>";

// HL plug information gathering format (can be extended if needed) 	
const char* HASP_FORMAT =
	"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
	"<haspformat root=\"hasp_info\">"
	"	 <license_manager>"
    " 		 <element name=\"hostname\" />"    		 
	"	 </license_manager>"
	"    <hasp>"
	"        <attribute name=\"id\" />"
	"        <attribute name=\"key_model\" />"
	"		 <attribute name =\"production_date\" />"	
	"    </hasp>"
	"    <feature>"
    "	     <attribute name=\"id\" />"
	"		 <attribute name=\"usable\" />"
    "        <element name=\"license\" />"
	"    </feature>"
	"</haspformat>";

// HASP format to get the API library version number	
const char* HASP_API_VER_FORMAT = 
	"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
	"<haspformat root=\"haspscope\">"
	"<session>"
	"	<attribute name=\"apiversion\" />"
	"</session>"
	"</haspformat>";

// HASP session information format
const char* HASP_SESSION_FORMAT = 
	"<haspformat root=\"hasp_info\">"
	"    <feature>"
	"       <attribute name=\"id\" />"	
	"       <attribute name=\"expired\" />"	
	"    </feature>"
	"</haspformat>";	

struct THaspDate	
{
	THaspDate() : Day(0), Month(0), Year(0), Hour(0), Minute(0) {}

	unsigned Day;
	unsigned Month;
	unsigned Year;
    unsigned Hour;
    unsigned Minute;
};

struct THaspPlugInfo
{
	THaspPlugInfo() : UptodateDriver(true){} 
	
	std::string HostName;
	std::string PlugID;
	std::string PlugModel;
	THaspDate  ProductionDate;
	std::string FeatureID;
	std::string IsFeatureUsable;
	std::string LicenseType;
	THaspDate  ExpirationDate; // for 'expiration' type license only
	bool       UptodateDriver; 	
}; 
  
#endif
