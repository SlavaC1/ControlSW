
#ifndef HaspDefsH
#define HaspDefsH

#include "QTypes.h"

// Objet vendor license key
const unsigned char HASP_OBJET_VENDOR_CODE[] =
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

// DemoMA vendor license key
/*
const unsigned char HASP_OBJET_VENDOR_CODE[] =
	"AzIceaqfA1hX5wS+M8cGnYh5ceevUnOZIzJBbXFD6dgf3tBkb9cvUF/Tkd/iKu2fsg9wAysYKw7RMAsV"
	"vIp4KcXle/v1RaXrLVnNBJ2H2DmrbUMOZbQUFXe698qmJsqNpLXRA367xpZ54i8kC5DTXwDhfxWTOZrB"
	"rh5sRKHcoVLumztIQjgWh37AzmSd1bLOfUGI0xjAL9zJWO3fRaeB0NS2KlmoKaVT5Y04zZEc06waU2r6"
	"AU2Dc4uipJqJmObqKM+tfNKAS0rZr5IudRiC7pUwnmtaHRe5fgSI8M7yvypvm+13Wm4Gwd4VnYiZvSxf"
	"8ImN3ZOG9wEzfyMIlH2+rKPUVHI+igsqla0Wd9m7ZUR9vFotj1uYV0OzG7hX0+huN2E/IdgLDjbiapj1"
	"e2fKHrMmGFaIvI6xzzJIQJF9GiRZ7+0jNFLKSyzX/K3JAyFrIPObfwM+y+zAgE1sWcZ1YnuBhICyRHBh"
	"aJDKIZL8MywrEfB2yF+R3k9wFG1oN48gSLyfrfEKuB/qgNp+BeTruWUk0AwRE9XVMUuRbjpxa4YA67SK"
	"unFEgFGgUfHBeHJTivvUl0u4Dki1UKAT973P+nXy2O0u239If/kRpNUVhMg8kpk7s8i6Arp7l/705/bL"
	"Cx4kN5hHHSXIqkiG9tHdeNV8VYo5+72hgaCx3/uVoVLmtvxbOIvo120uTJbuLVTvT8KtsOlb3DxwUrwL"
	"zaEMoAQAFk6Q9bNipHxfkRQER4kR7IYTMzSoW5mxh3H9O8Ge5BqVeYMEW36q9wnOYfxOLNw6yQMf8f9s"
	"JN4KhZty02xm707S7VEfJJ1KNq7b5pP/3RjE0IKtB2gE6vAPRvRLzEohu0m7q1aUp8wAvSiqjZy7FLaT"
	"tLEApXYvLvz6PEJdj4TegCZugj7c8bIOEqLXmloZ6EgVnjQ7/ttys7VFITB3mazzFiyQuKf4J6+b/a/Y";  */

// Hasp login scope for local PC only
/*const char* HASP_LOCAL_SCOPE =
	"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
	"<haspscope>"
	"	<hasp type=\"HASP-HL\" >"
	"    	<license_manager hostname =\"localhost\" />"
	"	</hasp>"
	"</haspscope>";   */

const char* HASP_LOCAL_SCOPE =
   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
"<haspscope>"
"<hasp configuration=\"sentinelhl.driverless\">"
"<license_manager hostname=\"localhost\" />"
"</hasp>"
"</haspscope> "   ;


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
	
	QString HostName;
	QString PlugID;
	QString PlugModel;
	THaspDate  ProductionDate;
	QString FeatureID;
	QString IsFeatureUsable;
	QString LicenseType;
	THaspDate  ExpirationDate; // for 'expiration' type license only
	bool       UptodateDriver; 	
};

enum THaspConnectionStatus
{
	hsConnected = 0,
	hsDisconnected,
	hsExpired,
	hsInvalid
};

const int TIME_BETWEEN_EACH_HASP_LOGIN_ATTEMPT = 1000; 	
const int TIME_BETWEEN_EACH_HASP_LICENSE_CHECK = 30000; 	
  
#endif
