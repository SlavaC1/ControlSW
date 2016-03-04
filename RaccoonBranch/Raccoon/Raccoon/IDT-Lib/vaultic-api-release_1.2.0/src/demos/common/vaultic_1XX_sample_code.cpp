// -----------------------------------------------------------------------------
// vaultic_1XX_sample_code.cpp : 
// -----------------------------------------------------------------------------
// This sample code project is intended to show how to call the VAULTIC_API, 
// and VAULTIC_STRONG_AUTHENTICATION interface methods.
//
// The code does not work, do not try to run it!
//
// There are working example code projects for FIPS mode and NON FIPS mode.
// Decide if you want to define your own custom ECDSA curve (use NON FIPS 
// mode), or use one of the preloaded curves in FIPS mode.
//
// FIPS mode Samples:
// VaultIC_100_FipsPersonaliseSampleCode.exe - will personalise
// the device with keys for the B283-R1 curve, run only once.
// The VaultIC_100_FipsAuthenticationSampleCode.exe provides a working 
// example of FIPS mutual authentication, and an example of a message 
// digest - SHA-256.
//
// NON FIPS Mode - custom curve:
// Run the VaultIC_100_PersonaliseSampleCode.exe - this will personalise
// the device with keys for the B283-R1 curve.  This curve example will
// store the domain parameters for this curve in the device EEPROM.
// This sample is intended to have the B-283-R1 curve replaced with a 
// customer curve, run only once.
// The VaultIC_100_AuthenticationSampleCode.exe provides a working 
// example of mutual authentication, and an example of a message 
// digest - SHA-256.
//------------------------------------------------------------------------------
#include "common.h"
#include "vaultic_1XX_sample_code.h"
#include "B283_R1_Curve.h"

/* Key Group and Index Defines. */
enum enDeviceKeyTypes
{
	enDeviceKeyGroup = 1, 
	enHostKeyGroup = 2,
	enPublicKeyIndex = 1, 
	enPrivateKeyIndex = 2,
	enDomainParamsKeyIndex = 3,
	enHostDeviceIDKeyIndex = 4,    
};

/*******************************************************
VaultIC SDK DLL Methods:
*******************************************************/
typedef VLT_STS (*pfnInitLibrary)(VLT_INIT_COMMS_PARAMS* initCommsParams);
typedef VLT_STS (*pfnCloseLibrary)(void);
typedef VAULTIC_API* (*pfnGetApi)(void);
typedef VLT_STS (*pfnGetLibraryInfo)( VLT_LIBRARY_INFO* pLibraryInfo );
typedef VLT_STS (*pfnFindDevices)( VLT_PU32 pSize, VLT_PU8 pXmlReaderString );
typedef VAULTIC_STRONG_AUTHENTICATION* (*pfnGetStrongAuthentication)(void);

using namespace std;

#define SUPPORTED( flags, value )   static_cast<bool>(( flags & value ) == value )

#if( VLT_PLATFORM == VLT_WINDOWS )
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char** argv)
#endif
{
#if( VLT_PLATFORM == VLT_WINDOWS )
	HMODULE hMod;
#else
	void* hMod;
#endif

	VAULTIC_API* theBaseApi = 0;
	VAULTIC_STRONG_AUTHENTICATION* theStrongAuth;
	unsigned short usActualSW = 0;
	pfnGetApi VltGetApi;
	pfnInitLibrary VltInitLibrary;
	pfnCloseLibrary VltCloseLibrary = NULL;;
	pfnGetLibraryInfo VltGetLibraryInfo;
	pfnFindDevices VltFindDevices;
	pfnGetStrongAuthentication VltGetStrongAuthentication;
	string strXMLDevs;
	long lPrefferedInterface = 0;

	//
	// Check the command line arguments are valid
	//
	if ( 2 > argc )
	{
		std::cout << "Please ensure the manufacturer's password has "
			"been passed in using the command line:" << endl;

		return( 1 );
	}

	std::string strManufPassword(argv[1]);
	//
	// Check the supplied password is a valid length.
	//
	if ( strManufPassword.length()/2 != VLT_REQUIRED_MANUF_PWD_LENGTH ) 
	{
		cout << "Please ensure the manufacturer's password is 32 "
			"bytes(64 char) in length." << endl;

		return( 1 );
	}

	unsigned char aucManufPassword[VLT_REQUIRED_MANUF_PWD_LENGTH];

	HexStringToByteArray(strManufPassword,aucManufPassword);

	unsigned char ucManufPasswordLength = (unsigned char)strManufPassword.size()/2;

	if (3 == argc)
	{
		lPrefferedInterface = strtol(argv[2], NULL, 10);
	}

	try
	{
		//
		// The path to the library is stored in the project settings.
		hMod = OpenLibrary( LIB_PATH_1XX );

		if ( 0 == hMod )
		{
			cout << "Failed to load the library, check "
				"the path is correct." << endl;
			return 1;
		}

		cout << "Library loaded: " << LIB_PATH_1XX << endl;

		//
		// Get the function pointer to the VltInitLibrary method.  
		// This method is used to initialise the entire API, it may 
		// allocated resources as required.
		if( NULL == ( VltInitLibrary = (pfnInitLibrary)GetLibSymbol( hMod,
			"VltInitLibrary" ) ) )
		{
			CloseAndExit(hMod,VLT_GETPROC_INIT_LIB_FAILED,"VltInitLibrary");
		}

		//
		// Get the function pointer to the VltCloseLibrary method.  This method
		// is used release resources allocated by previous API calls.
		if( NULL == ( VltCloseLibrary = (pfnCloseLibrary)GetLibSymbol( hMod,
			"VltCloseLibrary" ) ) )
		{
			CloseAndExit(hMod,VLT_GETPROC_ClOSE_LIB_FAILED,"VltCloseLibrary");
		}

		//    
		// Get the function pointer to the VltGetApi method.  This method
		// is used to obtain a pointer to the API Interface 
		if( NULL == ( VltGetApi = (pfnGetApi)GetLibSymbol( hMod, "VltGetApi" ) ) )
		{
			CloseAndExit(hMod,VLT_GETPROC_GET_API_FAILED,"VltGetApi");
		}

		//    
		// Get the function pointer to the VltGetLibraryInfo method. This method
		// is used to obtain information about the library.
		if( NULL == ( VltGetLibraryInfo = (pfnGetLibraryInfo)GetLibSymbol( hMod, "VltGetLibraryInfo" ) ) )
		{
			CloseAndExit(hMod,VLT_GETPROC_GET_API_FAILED,"VltGetLibraryInfo");
		}

		//    
		// Get the function pointer to the VltGetLibraryInfo method. This method
		// is used to obtain a list of the connected devices (aardvark devices).
		if( NULL == ( VltFindDevices = (pfnFindDevices)GetLibSymbol( hMod, "VltFindDevices" ) ) )
		{
			CloseAndExit(hMod,VLT_GETPROC_GET_FINDDEVS_FAILED,"VltFindDevices");
		}

		//    
		// Get the function pointer to the VltGetStrongAuthentication method. This method
		// is used to obtain a pointer to the VAULTIC_STRONG_AUTHENTICATION Interface.
		if( NULL == ( VltGetStrongAuthentication = (pfnGetStrongAuthentication)GetLibSymbol( hMod, "VltGetStrongAuthentication" ) ) )
		{
			CloseAndExit(hMod,VLT_GETPROC_GET_FINDDEVS_FAILED,"VltGetStrongAuthentication");
		}

		VLT_STS status = VLT_FAIL;


		//
		// Determine the size of the xml string that will be returned on the second call, 
		// set the stringSize parameter to zero and the string to NULL. On return the 
		// stringSize will be set to the actual number of characters of the XML string 
		// that contains the list of devices.
		VLT_U32 stringSize = 0;
		if( VLT_OK != ( status = VltFindDevices( &stringSize, NULL ) ) )
		{
			CloseAndExit(hMod,status,"VltFindDevices");
		}

		//
		// Allocate enough memory to hold the xml string with the list of
		// devices.
		VLT_PU8 pXmlString = (VLT_PU8)NULL;
		if( NULL == ( pXmlString = (VLT_PU8)malloc(stringSize ) ) ) 
		{
			CloseAndExit(hMod,VLT_FIND_DEVS_MALLOC_FAIL,"malloc");
		}


		//
		// Now call the VltFindDevices again with all the appropriate parameters
		// On return the pXmlString will be filled with the device list.
		if( VLT_OK != ( status = VltFindDevices( &stringSize, pXmlString ) ) )
		{
			if( NULL != pXmlString )
			{
				free( pXmlString );
				pXmlString = NULL;
			}

			CloseAndExit(hMod,status,"VltFindDevices");
		}
		else
		{
			cout << pXmlString << endl;

			//
			// Assign the XML string to a standard string
			strXMLDevs =(const char*)pXmlString;
		}

		if( NULL != pXmlString )
		{
			free( pXmlString );
			pXmlString = NULL;
		}

		VLT_INIT_COMMS_PARAMS commsParams;

		//
		// Check if there are any PC/SC readers connected
		size_t startPos = 0;
		size_t endPos = 0;
		string strReaderName("");

		bool readerFound = false;
		unsigned char item = 0;
		long selection = 0;

#if(VLT_ENABLE_ISO7816 == VLT_ENABLE )  
		//
		// Check if the XML string includes an entry for PC/SC readers
		//
		startPos = strXMLDevs.find("<interface type=\"pcsc\">");
		if( string::npos != startPos )
		{
			endPos = strXMLDevs.find("</interface>");
			if( string::npos == endPos )
				CloseAndExit(hMod,VLT_MALFORMED_XML_PCSC_INTFC,"Find device failed (malformated XML)"); 

			string strPCSC( strXMLDevs.substr( startPos, ( endPos - startPos ) ) );

			printf("\nList of detected PC/SC interfaces:");
			printf("\n==================================\n\n");

			// Reset start pointer & item number
			startPos = 0;
			item = 0;

			do
			{
				//
				// It does so check for entries for a reader
				//
				startPos = strPCSC.find("<peripheral idx=", startPos);
				if (string::npos == startPos)
				{
					// No more reader in the list = end of loop
					break;
				}
				{
					//
					// Start of the reader name is after the opening tag
					//
					startPos += strlen("<peripheral idx=\"00\">");

					//
					// End position is at beginning of the closing tag
					//
					endPos = strPCSC.find("</peripheral>", startPos);
					if (string::npos == endPos)
						CloseAndExit(hMod, VLT_MALFORMED_XML_PCSC_PERIPH, "Find device failed (malformated XML)");

					//
					// Get the reader name
					//
					strReaderName = strPCSC.substr(startPos, (endPos - startPos));

					// Display the reader name
					printf("Interface #%d, SmartCard Reader Name = %s\n", item++ + INTERFACE_PCSC, strReaderName.c_str());

					readerFound = true;
				} 
			} while (true);
		}
#endif

#if(VLT_ENABLE_AARDVARK == VLT_ENABLE )  

		startPos = strXMLDevs.find( "<interface type=\"aardvark\">" );
		if( string::npos != startPos )
		{
			endPos = strXMLDevs.find( "</interface>" , startPos);
			if( string::npos == endPos)
				CloseAndExit(hMod,VLT_MALFORMED_XML_AARDVARK_INTFC,"Find device failed (malformated XML)");

			string strAardvark( strXMLDevs.substr( startPos, ( endPos - startPos ) ) );

			printf("\nList of detected Aardvark interfaces:");
			printf("\n=====================================\n\n");

			// Reset start pointer & item number
			startPos = 0;
			item = 0;

			do
			{
				//
				// It does so check for entries for a reader
				//
				startPos = strAardvark.find("<peripheral idx=", startPos);
				if (string::npos == startPos)
				{
					// No more reader in the list = end of loop
					break;
				}
				else
				{
					//
					// Start of the reader name is after the opening tag
					//
					startPos += strlen("<peripheral idx=\"00\">");

					//
					// End position is at beginning of the closing tag
					//
					endPos = strAardvark.find("</peripheral>", startPos);
					if (string::npos == endPos)
						CloseAndExit(hMod, VLT_MALFORMED_XML_AARDVARK_PERIPH, "Find device failed (malformated XML)");

					//
					// Get the Aardwark serial number
					//
					strReaderName = strAardvark.substr(startPos, (endPos - startPos));

					// Display the reader name
					printf("Interface #%d; Aardvark SPI Serial Number = %s\n", INTERFACE_AARDVARK_SPI + item,	strReaderName.c_str());
					printf("Interface #%d; Aardvark TWI Serial Number = %s\n", INTERFACE_AARDVARK_TWI + item++, strReaderName.c_str());

					readerFound = true;
				}
			} while (true);
		}
#endif

		if (!readerFound)
		{
			printf("No interface found\n");
			CloseAndExit(hMod,VLT_NO_READER_CONNECTED,"No interface found");
		} 

		// If no preferred interface is provide
		if (lPrefferedInterface == 0)
		{
			// -> Prompt the user to select the communication interface
			printf("\n\nPlease enter the interface number, followed by ENTER: ");
			scanf("%02d", &selection); getchar();
			printf("\n\n");
		}
		else
		{
			// -> Automatically select the preferred interface
			printf("\n\nThe interface #%02d is automatically selected.\n", lPrefferedInterface);
			printf("\n\n");
			selection = lPrefferedInterface;
		}

		char pXmlData[30];

		unsigned char protocol = (selection / 10) * 10;
		unsigned char idx = selection % 10;

		sprintf((char*)pXmlData, (const char*)"<peripheral idx=\"%02d\">", idx);

		switch (protocol)
		{

#if(VLT_ENABLE_ISO7816 == VLT_ENABLE )  
		case INTERFACE_PCSC:
			startPos = strXMLDevs.find("<interface type=\"pcsc\">");
			startPos = strXMLDevs.find(pXmlData, startPos);
			startPos += strlen(pXmlData);
			endPos = strXMLDevs.find("</peripheral>", startPos);

			// Get the reader name
			strReaderName = strXMLDevs.substr(startPos, (endPos - startPos));

			// Setup communications using PCSC T=0
			commsParams.Params.VltPcscInitParams.hCard = 0;
			commsParams.Params.VltPcscInitParams.hContext = 0;
			commsParams.Params.VltPcscInitParams.pu8ReaderString = (VLT_PU8)strReaderName.c_str();

			commsParams.u8CommsProtocol = VLT_ISO_T0_OR_T1_COMMS;

			break;
#endif


#if(VLT_ENABLE_AARDVARK == VLT_ENABLE )  
		case INTERFACE_AARDVARK_TWI:
			startPos = strXMLDevs.find("<interface type=\"aardvark\">");
			startPos = strXMLDevs.find(pXmlData, startPos);
			startPos += strlen(pXmlData);
			endPos = strXMLDevs.find("</peripheral>", startPos);

			// Get the reader name
			strReaderName = strXMLDevs.substr(startPos, (endPos - startPos));

			// Setup communications using TWI over Aarvark
			commsParams.u8CommsProtocol = VLT_TWI_COMMS;
			commsParams.Params.VltBlockProtocolParams.VltPeripheralParams.PeriphParams.VltTwiParams.u16BusTimeout = 250;
			commsParams.Params.VltBlockProtocolParams.VltPeripheralParams.PeriphParams.VltTwiParams.u32msTimeout = 4000;
			commsParams.Params.VltBlockProtocolParams.VltPeripheralParams.PeriphParams.VltTwiParams.u8Address = 0x5F;
			commsParams.Params.VltBlockProtocolParams.u16BitRate = 100;
			commsParams.Params.VltBlockProtocolParams.u8CheckSumMode = BLK_PTCL_CHECKSUM_SUM8;
			commsParams.Params.VltBlockProtocolParams.u32AfterHdrDelay = 1000;
			commsParams.Params.VltBlockProtocolParams.u32InterBlkDelay = 1000;
			commsParams.Params.VltBlockProtocolParams.u32InterHdrByteDelay = 1000;
			commsParams.Params.VltBlockProtocolParams.u16msSelfTestDelay = 2500;
			commsParams.Params.VltBlockProtocolParams.VltPeripheralParams.u32SerialNo = strtoul(strReaderName.c_str(), NULL, 10);

			break;
#endif

		default:
			CloseAndExit(hMod, VLT_NO_READER_CONNECTED, "Invalid selection\n");

		}

		//
		// Initialise the API, if it fails we should 
		// exit gracefully.
		if( VLT_OK != VltInitLibrary( &commsParams ) )
		{
			CloseAndExit(hMod, VLT_INIT_LIB_FAILED, "VltInitLibrary");
		}


		//
		// Get the Api Interface.
		if ( 0 == ( theBaseApi = VltGetApi( ) ) )
		{
			CloseAndExit(hMod, VLT_GET_API_FAILED, "VltGetApi");
		}

		//
		// Get the Strong Authentication Interface.
		if ( 0 == ( theStrongAuth = VltGetStrongAuthentication( ) ) )
		{
			CloseAndExit(hMod, VLT_GET_AUTH_FAILED, "VltGetStrongAuthentication");
		}


		//---------------------------------------------------------------------
		// The Api is NOT setup, it is NOT ok to call interface funtions.
		// Code for illustration purposes only!
		//---------------------------------------------------------------------
		// 1: Get the device information
		//---------------------------------------------------------------------
		VLT_TARGET_INFO targetInfo;

		if ( VLT_OK != (usActualSW = theBaseApi->VltGetInfo( &targetInfo ) ) )
		{
			CloseAndExit(hMod, usActualSW, "VltGetInfo");
		}

		//
		// Report the device name.
		std::string strTarget((const char*)targetInfo.au8Firmware, 
			VLT_FIRMWARE_VERSION_LENGTH);

		cout << "The Target Name: " << strTarget << endl;



		//---------------------------------------------------------------------
		// Login as the manufacturer
		//---------------------------------------------------------------------
		// 2: Login as the manufacturer.
		//---------------------------------------------------------------------
		if ( VLT_OK != (usActualSW = theBaseApi->VltSubmitPassword( ucManufPasswordLength,
			aucManufPassword) ) )
		{
			CloseAndExit(hMod, usActualSW, "VltSubmitPassword");
		}

		// change password and trycount parameters
		VLT_MANAGE_AUTH_DATA authenSetup;
		authenSetup.u8RoleID=VLT_MANUFACTURER;  
		authenSetup.u8Method=VLT_LOGIN_PASSWORD;
		authenSetup.u8UserID=VLT_CREATOR;
		authenSetup.u8TryCount=127;
		authenSetup.cleartext.u8PasswordLength=ucManufPasswordLength;
		memcpy( authenSetup.cleartext.u8Password,aucManufPassword,ucManufPasswordLength);


		if (VLT_OK != ( usActualSW = theBaseApi->VltSetStatus( VLT_CREATION) ) )
		{
			CloseAndExit(hMod, usActualSW, "VltSetStatus VLT_CREATION");
		}


		if ( VLT_OK != (usActualSW = theBaseApi->VltManageAuthenticationData(&authenSetup)))
		{
			CloseAndExit(hMod, usActualSW, "VltManageAuthenticationData");
		}

		//---------------------------------------------------------------------
		// 3. Modify the users failed authentication try count to 1.
		//---------------------------------------------------------------------
		VLT_AUTH_INFO authInfo;
		if ( VLT_OK != (usActualSW = theBaseApi->VltGetAuthenticationInfo(VLT_CREATOR,  &authInfo) ) )
		{
			CloseAndExit(hMod, usActualSW, "VltGetAuthenticationInfo");
		}

		//
		// Print out details of the authenticated user.
		printf( "The Authenticated Role is: 0x%.2X\n",authInfo.u8Roles);
		printf( "The Authentication Method is: 0x%.2X\n",authInfo.u8AuthMethod);
		printf( "The Role's Remaing Try Count is: 0x%.2X of: 0x%.2X\n",authInfo.u8RemainingTryCount,authInfo.u8MaxTries);


		//---------------------------------------------------------------------
		// 4: Transition to the CREATION state.
		//---------------------------------------------------------------------
		if (VLT_OK != ( usActualSW = theBaseApi->VltSetStatus( VLT_CREATION) ) )
		{
			CloseAndExit(hMod, usActualSW, "VltSetStatus VLT_CREATION");
		}

		//---------------------------------------------------------------------
		// 5. Modify the users failed authentication try count to 1.
		//---------------------------------------------------------------------
		VLT_MANAGE_AUTH_DATA authSetup;

		authSetup.u8Method = VLT_LOGIN_STRONG;
		authSetup.u8UserID = VLT_USER;
		authSetup.u8RoleID = VLT_APPROVED_USER;
		authSetup.u8TryCount = 1;

		if ( VLT_OK != (usActualSW = theBaseApi->VltManageAuthenticationData( 
			&authSetup ) ) )
		{
			CloseAndExit(hMod, usActualSW, "VltManageAuthenticationData");
		}

		//---------------------------------------------------------------------
		// 6. Select the User Defined Params File
		//---------------------------------------------------------------------
		unsigned char ucCurve = VLT_ECDSA_PARAMS_USER_DEF_CURVE_CHOICE;

		if ( VLT_OK != ( usActualSW = theBaseApi->VltSetConfig( VLT_ECDSA_DOMAIN_PARAM_CHOICE, 
			VLT_ECDSA_PARAMS_CHOICE_SZ, &ucCurve ) ) )
		{
			CloseAndExit(hMod, usActualSW, "VltSetConfig VLT_ECDSA_DOMAIN_PARAM_CHOICE");
		}

		//---------------------------------------------------------------------
		// 7. B-283-R1 is used as an example curve, curves upto 303 bits
		// can be used, if this is desired, disable FIPS mode.
		//---------------------------------------------------------------------
		unsigned char ucFipsMode = 0;

		if ( VLT_OK != ( usActualSW = theBaseApi->VltSetConfig( VLT_FIPS_MODE, 
			VLT_FIPS_MODE_SZ, &ucFipsMode ) ) )
		{
			CloseAndExit(hMod, usActualSW, "VltSetConfig VLT_FIPS_MODE");
		}

		//---------------------------------------------------------------------
		// 8. Store the Host ID Key in the Secure Object file system.
		//---------------------------------------------------------------------
		VLT_KEY_OBJECT KeyObj;
		VLT_FILE_PRIVILEGES KeyPrivilages = { 0x03, 0x00, 0x0, 0x2 };

		KeyObj.u8KeyID = VLT_KEY_HOST_DEVICE_ID;
		KeyObj.data.HostDeviceIdKey.u16StringLen = ucHostDevIdLength;
		KeyObj.data.HostDeviceIdKey.pu8StringId = aucHostID; 

		if ( VLT_OK != ( usActualSW = theBaseApi->VltPutKey( (VLT_U8)enHostKeyGroup,
			(VLT_U8)enHostDeviceIDKeyIndex,
			&KeyPrivilages,
			&KeyObj ) ) )
		{
			CloseAndExit(hMod, usActualSW, "VltPutKey Host ID");
		}

		//---------------------------------------------------------------------
		// 9. Store the Device ID Key in the Secure Object file system.
		//---------------------------------------------------------------------
		KeyObj.data.HostDeviceIdKey.pu8StringId = aucDeviceID;

		if ( VLT_OK != ( usActualSW = theBaseApi->VltPutKey( (VLT_U8)enDeviceKeyGroup,
			(VLT_U8)enHostDeviceIDKeyIndex,
			&KeyPrivilages,
			&KeyObj ) ) )
		{
			CloseAndExit(hMod, usActualSW, "VltPutKey Device ID");
		}

		//---------------------------------------------------------------------
		// 10: Store the Host defined domain parameters key in the Secure 
		// Object file system (Replace .
		//---------------------------------------------------------------------
		VLT_KEY_OBJECT DomainParams;
		DomainParams.u8KeyID = VLT_KEY_ECDSA_DOMAIN_PARMS;
		DomainParams.data.EcdsaParamsKey.u16QLen = ucBinaryFieldLength;
		DomainParams.data.EcdsaParamsKey.pu8Q = auc_Q;
		DomainParams.data.EcdsaParamsKey.pu8Gx = auc_Gx;
		DomainParams.data.EcdsaParamsKey.pu8Gy = auc_Gy;
		DomainParams.data.EcdsaParamsKey.pu8Gz = auc_Gz;
		DomainParams.data.EcdsaParamsKey.pu8A = auc_A;
		DomainParams.data.EcdsaParamsKey.pu8B = auc_B;
		DomainParams.data.EcdsaParamsKey.u16NLen = ucBinaryFieldLength;
		DomainParams.data.EcdsaParamsKey.pu8N = auc_N;
		DomainParams.data.EcdsaParamsKey.u32H = us_H;
		DomainParams.data.EcdsaParamsKey.u8Assurance = 0;

		if (VLT_OK != ( usActualSW = theBaseApi->VltPutKey( (VLT_U8)enDeviceKeyGroup,
			(VLT_U8)enDomainParamsKeyIndex,
			&KeyPrivilages,
			&DomainParams ) ) )
		{
			CloseAndExit(hMod, usActualSW, "VltPutKey domain parameters ");
		}

		//---------------------------------------------------------------------
		// 11: Store the Host's Public Key in the Secure Object file system.
		//---------------------------------------------------------------------
		KeyObj.u8KeyID = VLT_KEY_ECDSA_GF2M_PUB;
		KeyObj.data.EcdsaPubKey.u16QLen = ucBinaryFieldLength;
		KeyObj.data.EcdsaPubKey.pu8Qx = auc_Host_Qx;
		KeyObj.data.EcdsaPubKey.pu8Qy = auc_Host_Qy;
		KeyObj.data.EcdsaPubKey.u8DomainParamsGroup = (VLT_U8)enDeviceKeyGroup; 
		KeyObj.data.EcdsaPubKey.u8DomainParamsIndex = (VLT_U8)enDomainParamsKeyIndex; 
		KeyObj.data.EcdsaPubKey.u8Assurance = 0;

		if (VLT_OK != ( usActualSW = theBaseApi->VltPutKey( (VLT_U8)enHostKeyGroup, 
			(VLT_U8)enPublicKeyIndex, 
			&KeyPrivilages, 
			&KeyObj ) ) )
		{
			CloseAndExit(hMod, usActualSW, "VltPutKey Host's Public Key");
		}

		//---------------------------------------------------------------------
		// 12: Generate the Device's Public and Private Key pair 
		// in the Secure Object file system.
		//---------------------------------------------------------------------
		VLT_KEY_GEN_DATA keyGenData;
		keyGenData.u8AlgoID = VLT_ALG_KPG_ECDSA_GF2M;
		keyGenData.EcdsaKeyGenObj.u8DomainParamsGroup = enDeviceKeyGroup;
		keyGenData.EcdsaKeyGenObj.u8DomainParamsIndex = enDomainParamsKeyIndex;

		if (VLT_OK != ( usActualSW = theBaseApi->VltGenerateKeyPair(
			(VLT_U8)enDeviceKeyGroup, 
			(VLT_U8)enPublicKeyIndex, 
			(VLT_U8)enDeviceKeyGroup, 
			(VLT_U8)enPrivateKeyIndex, 
			&keyGenData ) ) )
		{
			CloseAndExit(hMod, usActualSW, "VltGenerateKeyPair");
		}

		//---------------------------------------------------------------------
		// 13: Read the generate device public key!
		//---------------------------------------------------------------------
		unsigned char aucRead_Qx[36]; // Field length of B-183-R1.
		unsigned char aucRead_Qy[36]; // Field length of B-183-R1.

		VLT_KEY_OBJECT read_key;
		read_key.u8KeyID = VLT_KEY_ECDSA_GF2M_PUB;
		read_key.data.EcdsaPubKey.u16QLen = static_cast<unsigned short>( sizeof( aucRead_Qx ) );
		read_key.data.EcdsaPubKey.pu8Qx = aucRead_Qx;
		read_key.data.EcdsaPubKey.pu8Qy = aucRead_Qy;


		if (VLT_OK != ( usActualSW = theBaseApi->VltReadKey((VLT_U8)enDeviceKeyGroup, 
			(VLT_U8)enPublicKeyIndex,
			&read_key) ) )
		{
			CloseAndExit(hMod, usActualSW, "VltReadKey Device's Public Key");
		}

		//---------------------------------------------------------------------
		// 14: Overwrite the Device's Private Key in the Secure Object file system.
		//---------------------------------------------------------------------
		KeyObj.u8KeyID = VLT_KEY_ECDSA_GF2M_PRIV;
		KeyObj.data.EcdsaPrivKey.u16DLen = ucBinaryFieldLength;
		KeyObj.data.EcdsaPrivKey.u8Mask = 0x00;
		KeyObj.data.EcdsaPrivKey.pu8D = auc_Device_D;
		KeyObj.data.EcdsaPrivKey.u8PublicKeyGroup = (VLT_U8)enDeviceKeyGroup;
		KeyObj.data.EcdsaPrivKey.u8PublicKeyIndex = (VLT_U8)enPublicKeyIndex;
		KeyObj.data.EcdsaPrivKey.u8DomainParamsGroup = (VLT_U8)enDeviceKeyGroup;
		KeyObj.data.EcdsaPrivKey.u8DomainParamsIndex = (VLT_U8)enDomainParamsKeyIndex;

		if (VLT_OK != ( usActualSW = theBaseApi->VltPutKey( (VLT_U8)enDeviceKeyGroup, 
			(VLT_U8)enPrivateKeyIndex, 
			&KeyPrivilages, 
			&KeyObj  ) ) )
		{
			CloseAndExit(hMod, usActualSW, "VltPutKey Device's Private Key");
		}

		//---------------------------------------------------------------------
		// 15: Overwrite the Device's Public Key in the Secure Object file system.
		//---------------------------------------------------------------------
		KeyObj.u8KeyID = VLT_KEY_ECDSA_GF2M_PUB;
		KeyObj.data.EcdsaPubKey.u16QLen = ucBinaryFieldLength;
		KeyObj.data.EcdsaPubKey.pu8Qx = auc_Device_Qx;
		KeyObj.data.EcdsaPubKey.pu8Qy = auc_Device_Qy;
		KeyObj.data.EcdsaPubKey.u8DomainParamsGroup = (VLT_U8)enDeviceKeyGroup;
		KeyObj.data.EcdsaPubKey.u8DomainParamsIndex = (VLT_U8)enDomainParamsKeyIndex;
		KeyObj.data.EcdsaPubKey.u8Assurance = 0;

		if (VLT_OK != ( usActualSW = theBaseApi->VltPutKey( (VLT_U8)enDeviceKeyGroup, 
			(VLT_U8)enPublicKeyIndex, 
			&KeyPrivilages, 
			&KeyObj  ) ) )
		{
			CloseAndExit(hMod, usActualSW, "VltPutKey Device's Public Key");
		}

		//---------------------------------------------------------------------
		// 16: Select the devices certificate file.
		//---------------------------------------------------------------------
		const char* certFile = "/cert/.cer";
		VLT_SELECT respData;

		if (VLT_OK != ( usActualSW = theBaseApi->VltSelectFile( (VLT_U8*)certFile, 
			(VLT_U8)strlen(certFile), 
			&respData ) ) )
		{
			CloseAndExit(hMod, usActualSW, "VltSelectFile (Certificate)");
		}

		//---------------------------------------------------------------------
		// 17: Write some certificate data into the file.
		//---------------------------------------------------------------------
		unsigned char aucCertificateData[VLT_CERTIFICATE_FILE_SIZE];
		memset(aucCertificateData, 0xAA, VLT_CERTIFICATE_FILE_SIZE);

		//
		// Write file can only write 255 bites in one write transaction.
		if (VLT_OK != ( usActualSW = theBaseApi->VltWriteFile( aucCertificateData, 
			MAX_WRITE_SIZE ) ) )
		{
			CloseAndExit(hMod, usActualSW, "VltWriteFile (Certificate)");
		}

		//---------------------------------------------------------------------
		// 18: Set the file system popinter back to the start of the file.
		//---------------------------------------------------------------------
		if (VLT_OK != ( usActualSW = theBaseApi->VltSeekFile( 0 ) ) )
		{
			CloseAndExit(hMod, usActualSW, "VltSeekFile");
		}   

		//---------------------------------------------------------------------
		// 19: Write some certificate data into the file.
		//---------------------------------------------------------------------
		unsigned char aucReadCertificateData[VLT_CERTIFICATE_FILE_SIZE];
		memset(aucReadCertificateData, 0xAA, VLT_CERTIFICATE_FILE_SIZE);
		unsigned short usReadSize = MAX_WRITE_SIZE;

		//
		// Read file can only read 255 bites in one read transaction.
		if (VLT_OK != ( usActualSW = theBaseApi->VltReadFile( &usReadSize, aucReadCertificateData ) ) )
		{
			CloseAndExit(hMod, usActualSW, "VltReadFile (Certificate)");
		}

		if ( 0 != (memcmp( aucReadCertificateData, aucCertificateData, usReadSize) ) )
		{
			cout << "The data written to the the cert file is not the same as the data read." << endl;
		}

		//---------------------------------------------------------------------
		// 20: Run the self tests.
		//---------------------------------------------------------------------
		if (VLT_OK != ( usActualSW = theBaseApi->VltSelfTest(  ) ) )
		{
			CloseAndExit(hMod, usActualSW, "VltSelfTest");
		}

		//---------------------------------------------------------------------
		// 21: Transition to the ACTIVE state.
		//---------------------------------------------------------------------
		if (VLT_OK != ( usActualSW = theBaseApi->VltSetStatus( VLT_OPERATIONAL_ACTIVE ) ) )
		{
			CloseAndExit(hMod, usActualSW, "VltSetStatus");
		}

		//---------------------------------------------------------------------
		// 22: Log the manufacturer out of the VaultIC Secure Object.
		//---------------------------------------------------------------------
		if (VLT_OK != ( usActualSW = theBaseApi->VltCancelAuthentication(  ) ) )
		{
			CloseAndExit(hMod, usActualSW, "VltCancelAuthentication");
		}

		//---------------------------------------------------------------------
		// 23: Perform FIPS Mutual Authentication.
		//---------------------------------------------------------------------
		VLT_SA_CRYPTO_PARAMS cryptoParams;
		cryptoParams.u8AlgID = VLT_ALG_SIG_ECDSA_GF2M;

		//
		// Domain parameters setup for eeprom (replace the B-283_R1 
		// with your a user defined curve).
		cryptoParams.params.ecdsa.domainParams.u16QLen = ucBinaryFieldLength;
		cryptoParams.params.ecdsa.domainParams.pu8Q = auc_Q;
		cryptoParams.params.ecdsa.domainParams.pu8A = auc_A;
		cryptoParams.params.ecdsa.domainParams.pu8B = auc_B;
		cryptoParams.params.ecdsa.domainParams.pu8Gx = auc_Gx;
		cryptoParams.params.ecdsa.domainParams.pu8Gy = auc_Gy;
		cryptoParams.params.ecdsa.domainParams.pu8Gz = auc_Gz;
		cryptoParams.params.ecdsa.domainParams.u16NLen = ucBinaryFieldLength;
		cryptoParams.params.ecdsa.domainParams.pu8N = auc_N;        
		cryptoParams.params.ecdsa.domainParams.u32H = us_H;

		//
		// Setup the host's public key
		cryptoParams.params.ecdsa.hostPrivateKey.u16DLen = ucBinaryFieldLength;
		cryptoParams.params.ecdsa.hostPrivateKey.pu8D = auc_Host_D;

		//
		// Setup the device's public key
		cryptoParams.params.ecdsa.devicePublicKey.u16QLen = ucBinaryFieldLength;
		cryptoParams.params.ecdsa.devicePublicKey.pu8Qx = auc_Device_Qx;
		cryptoParams.params.ecdsa.devicePublicKey.pu8Qy = auc_Device_Qy;

		//
		// Setup the authentication protocol parameters.
		VLT_SA_PROTOCOL_PARAMS protocolParams;
		protocolParams.u8AuthModeID = VLT_AUTH_MUTUAL_NON_FIPS;
		protocolParams.u8UserID = VLT_USER;
		protocolParams.u8RoleID = VLT_APPROVED_USER;
		protocolParams.u8DeviceIdLength = VLT_HOST_DEV_ID_STRING_LENGTH;
		protocolParams.pu8DeviceId = aucDeviceID;
		protocolParams.u8HostIdLength = VLT_HOST_DEV_ID_STRING_LENGTH;
		protocolParams.pu8HostId = aucHostID;

		//
		// Perform FIPS Mutual Authentication
		if( VLT_OK != ( usActualSW = theStrongAuth->VltStrongAuthenticate( &cryptoParams, 
			&protocolParams ) ) )
		{
			CloseAndExit(hMod, usActualSW, "VltStrongAuthenticate");
		}

		//---------------------------------------------------------------------
		// 24: Check the state of the authentication service.
		//---------------------------------------------------------------------
		unsigned char ucState = 0;

		if ( VLT_OK != ( usActualSW = theStrongAuth->VltStrongGetState( &ucState ) ) )
		{
			CloseAndExit(hMod, usActualSW, "VltStrongGetState");
		}

		if ( VLT_USER_AUTHENTICATED != ucState )
		{
			CloseAndExit(hMod, usActualSW, "VltStrongGetState");
		}

		//---------------------------------------------------------------------
		// 25: Check we are logged in by accessing a restricted command:
		// VltInitializeAlgorithm (sets up the vault ic crypto engine).
		//---------------------------------------------------------------------
		VLT_ALGO_PARAMS algorithm;
		algorithm.u8AlgoID = VLT_ALG_DIG_SHA256;

		if ( VLT_OK != (usActualSW = theBaseApi->VltInitializeAlgorithm(
			0, 0, VLT_DIGEST_MODE, &algorithm )))
		{
			CloseAndExit(hMod, usActualSW, "VltInitializeAlgorithm");
		}

		//---------------------------------------------------------------------
		// 26: Check the vaultic crypto engine was setup for a SHA-256 bit hash.
		//---------------------------------------------------------------------
		unsigned char aucMessage[16];

		// Generate a random number from the VaultIC
		if (VLT_OK != (usActualSW = theBaseApi->VltGenerateRandom(sizeof(aucMessage), aucMessage)))
		{
			CloseAndExit(hMod, usActualSW, "VltGenerateRandom");
		}

		//
		// Print out the random bytes
		std::string strRnd((const char*)aucMessage, (size_t)sizeof(aucMessage));
		cout << "The returned random bytes are: " << strRnd << endl;

		unsigned char aucHash[32];
		unsigned char ucSize = sizeof(aucHash);

		if ( VLT_OK != ( usActualSW = theBaseApi->VltComputeMessageDigest(
			sizeof(aucMessage),
			aucMessage,
			&ucSize,
			aucHash )))
		{
			CloseAndExit(hMod, usActualSW, "VltComputeMessageDigest");
		}

		//
		// Print out the hash
		std::string strHash((const char*)aucHash, (size_t)ucSize);
		cout << "The SHA-256 of the random bytes is: " << strHash << endl;

		//---------------------------------------------------------------------
		// 27: Check we are logged in by accessing a restricted command:
		// VltInitializeAlgorithm (set up the vault ic crypto engine for ECDSA
		// signature operation).
		//---------------------------------------------------------------------
		algorithm.u8AlgoID = VLT_ALG_SIG_ECDSA_GF2M;
		algorithm.EcdsaDsa.u8Digest = VLT_ALG_DIG_SHA256;

		if ( VLT_OK != (usActualSW = theBaseApi->VltInitializeAlgorithm(
			(VLT_U8)enDeviceKeyGroup,
			(VLT_U8)enPrivateKeyIndex, 
			VLT_SIGN_MODE, 
			&algorithm )))
		{
				CloseAndExit(hMod, usActualSW, "VltInitializeAlgorithm");
		}

		//---------------------------------------------------------------------
		// 28: Sign a message using the device's private key.
		//---------------------------------------------------------------------
		ucSize = sizeof(aucMessage);
		unsigned char aucSignature[72]; // Twice the field length of B-283-R1
		unsigned short usSignatureSize = sizeof(aucSignature);
		if ( VLT_OK != (usActualSW = theBaseApi->VltGenerateSignature(ucSize,
			aucMessage,
			&usSignatureSize,
			aucSignature) ) )
		{
			CloseAndExit(hMod, usActualSW, "VltGenerateSignature");
		}

		//---------------------------------------------------------------------
		// 29: Check we are logged in by accessing a restricted command:
		// VltInitializeAlgorithm (set up the vault ic crypto engine for ECDSA
		// verify operation).
		//---------------------------------------------------------------------
		if ( VLT_OK != (usActualSW = theBaseApi->VltInitializeAlgorithm(
			(VLT_U8)enDeviceKeyGroup,
			(VLT_U8)enPublicKeyIndex, 
			VLT_VERIFY_MODE, 
			&algorithm )))
		{
			CloseAndExit(hMod, usActualSW, "VltInitializeAlgorithm");
		}

		//---------------------------------------------------------------------
		// 30: Verify a signature using the device's public key.
		//---------------------------------------------------------------------
		if ( VLT_OK != (usActualSW = theBaseApi->VltVerifySignature(ucSize,
			aucMessage,
			usSignatureSize,
			aucSignature) ) )
		{
				CloseAndExit(hMod, usActualSW, "VltVerifySignature");
		}

		//---------------------------------------------------------------------
		// 30bis: Verify the signature using the device's public key
		//			AND a third party cryptographic engine
		//			(i.e. signature verification done from host side)
		//---------------------------------------------------------------------
		/*
		* Perform the verify operation using the vaultic's public key.
		*/
		usActualSW = theStrongAuth->EcdsaSignerInit(
			&cryptoParams.params.ecdsa.domainParams,
			NULL,
			&cryptoParams.params.ecdsa.devicePublicKey,
			VLT_VERIFY_MODE);

		/* Check the crypto service was constructed correctly */
		if (VLT_OK == usActualSW)
		{
			/*
			* Pass the message to the crypto to be signed and verified.
			* There will be no output (verify) so it is ok to pass
			* the signature buffer for the output.
			*/
			VLT_U32 u32SignatureSize = usSignatureSize;
			usActualSW = theStrongAuth->EcdsaSignerDoFinal(
				aucMessage,
				sizeof(aucMessage),
				sizeof(aucMessage),
				aucSignature,
				&u32SignatureSize,
				u32SignatureSize);

			if (VLT_OK != usActualSW)
			{
				/* Failed the verify operation, don't continue, return an error. */
				usActualSW = EINTERNALAUTHVERFAILED;
			}
		}

		/* clear down and free signer resources */
		theStrongAuth->EcdsaSignerClose();

		if (VLT_OK != usActualSW)
		{
			CloseAndExit(hMod, usActualSW, "EcdsaSignerClose");
		}

		//---------------------------------------------------------------------
		// 31: Log out user.  
		// Note using the theApi->VltCancelAuthentication( ) 
		// method will leave the strong authentication service intalised.
		//---------------------------------------------------------------------
		if ( VLT_OK != ( usActualSW = theStrongAuth->VltStrongClose( ) ) )
		{
			CloseAndExit(hMod, usActualSW, "VltStrongClose");
		}
  
		CloseAndExit(hMod, usActualSW, "Example code SUCCESSFULLY complete, VaultIC file system wiped.");     
	}

	catch(...)
	{
		CloseAndExit(hMod, usActualSW, "Unknown Exception caught.");     
	}

	return 0;
}
