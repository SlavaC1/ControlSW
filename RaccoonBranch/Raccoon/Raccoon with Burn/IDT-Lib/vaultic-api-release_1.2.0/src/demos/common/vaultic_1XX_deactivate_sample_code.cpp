// -----------------------------------------------------------------------------
// VaultIC_100_AuthenticationSampleCode.cpp : 
// -----------------------------------------------------------------------------
// Defines a procedure for demonstrating how to authenticate the VaultIC 100
// approved user in NON FIPS mode.  NON FIPS mode offers the 
// host the ability to define a custom ECDSA curve up to 303 bits.  
// The NON FIPS mode also offers the ability to transition back to the creation 
// state, wiping the keys and file system contents.
//
// Prerequisites: Run the VaultIC_100_PersonaliseSampleCode.exe first.
//
// The example uses B-283-R1 Curve for illustration purposes.  This curve should
// be replaced with a user defined curve.
//
// -----------------------------------------------------------------------------
#include "common.h"
#include "B283_R1_Curve.h"
#include "vaultic_1XX_sample_code.h"

/*******************************************************
 VaultIC SDK DLL Methods:
*******************************************************/
typedef VLT_STS (*pfnInitLibrary)(VLT_INIT_COMMS_PARAMS* initCommsParams);
typedef VLT_STS (*pfnCloseLibrary)(void);
typedef VAULTIC_API* (*pfnGetApi)(void);
typedef VLT_STS (*pfnGetLibraryInfo)( VLT_LIBRARY_INFO* pLibraryInfo );
typedef VLT_STS (*pfnFindDevices)( VLT_PU32 pSize, VLT_PU8 pXmlReaderString );
typedef VAULTIC_STRONG_AUTHENTICATION* (*pfnGetStrongAuthentication)(void);


enum enDeviceKeyTypes
{
    enDeviceKeyGroup = 1, 
    enHostKeyGroup = 2,
    enPublicKeyIndex = 1, 
    enPrivateKeyIndex = 2,
    enDomainParamsKeyIndex = 3,
    enHostDeviceIDKeyIndex = 4,    
} ;

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
    pfnGetApi VltGetApi;
    pfnInitLibrary VltInitLibrary;
    pfnCloseLibrary VltCloseLibrary;
    pfnGetLibraryInfo VltGetLibraryInfo;
    pfnFindDevices VltFindDevices;
    pfnGetStrongAuthentication VltGetStrongAuthentication;
    string strXMLDevs;
    unsigned short usActualSW = 0;
    long lPrefferedInterface = 0;
    
    //
    // Check the command line arguments are valid
    //
    if ( 5 > argc )
    {
        std::cout << "Please ensure the keyid and key data are provided in parameters" << endl;
	
        return( 1 );
    }

	std::string strcurve(argv[1]);
	std::string strhostPriv(argv[2]);
	std::string strKeyId(argv[3]);
	std::string strKeyData(argv[4]);

	//
    // Check the supplied password is a valid length.
    //
    if ( strKeyId.length()/2 != 2 ) 
    {
        cout << "Please ensure the keyId is 2 "
            "bytes(4 char) in length." << endl;
	
        return( 1 );
    }

	if ( strKeyData.length()/2 != 32 ) 
    {
        cout << "Please ensure the keydata is 32 "
            "bytes(64 char) in length." << endl;
	
        return( 1 );
    }

	unsigned char ucKeyIdLength = (unsigned char)strKeyId.size()/2;
	unsigned char ucKeyDataLength = (unsigned char)strKeyData.size()/2;
	unsigned char ucHostPrivLength = (unsigned char)strhostPriv.size()/2;

	unsigned char aucKeyId[2];
	unsigned char aucKeyData[32];
	unsigned char aucHostPriv[0x50];

	HexStringToByteArray(strKeyId,aucKeyId);
	HexStringToByteArray(strKeyData,aucKeyData);
	HexStringToByteArray(strhostPriv,aucHostPriv);

    
	if (6 == argc)
	{
		lPrefferedInterface = strtol(argv[5], NULL, 10);
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
     if (!readerFound)
		{
			CloseAndExit(hMod, VLT_NO_READER_CONNECTED, "Invalid selection\n"); 
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
            CloseAndExit(hMod, VLT_GET_API_FAILED, "VltInitLibrary"); 
        }

	    //
	    // Get the Strong Authentication Interface.
        if ( 0 == ( theStrongAuth = VltGetStrongAuthentication( ) ) )
        {
            CloseAndExit(hMod, VLT_GET_AUTH_FAILED, "VltGetStrongAuthentication"); 
        }


		//---------------------------------------------------------------------
		// 1: Perform non FIPS Mutual Authentication.
		//---------------------------------------------------------------------
		//B233
		VLT_U8 primeQ_B233[] = {0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01}; 
		VLT_U8 xPoint_B233[] = {0x00,0x00,0x00,0xFA,0xC9,0xDF,0xCB,0xAC,0x83,0x13,0xBB,0x21,0x39,0xF1,0xBB,0x75,0x5F,0xEF,0x65,0xBC,0x39,0x1F,0x8B,0x36,0xF8,0xF8,0xEB,0x73,0x71,0xFD,0x55,0x8B}; 
		VLT_U8 yPoint_B233[] = {0x00,0x00,0x01,0x00,0x6A,0x08,0xA4,0x19,0x03,0x35,0x06,0x78,0xE5,0x85,0x28,0xBE,0xBF,0x8A,0x0B,0xEF,0xF8,0x67,0xA7,0xCA,0x36,0x71,0x6F,0x7E,0x01,0xF8,0x10,0x52}; 
		VLT_U8 zPoint_B233[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01}; 
		VLT_U8 coeffA_B233[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};
		VLT_U8 coeffB_B233[] = {0x00,0x00,0x00,0x66,0x64,0x7E,0xDE,0x6C,0x33,0x2C,0x7F,0x8C,0x09,0x23,0xBB,0x58,0x21,0x3B,0x33,0x3B,0x20,0xE9,0xCE,0x42,0x81,0xFE,0x11,0x5F,0x7D,0x8F,0x90,0xAD}; 
		VLT_U8 orderN_B233[] = {0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x13,0xE9,0x74,0xE7,0x2F,0x8A,0x69,0x22,0x03,0x1D,0x26,0x03,0xCF,0xE0,0xD7}; 
		VLT_U32 cofactorH_B233 = 0x00000002;

		//K233
		VLT_U8 primeQ_K233[] = {0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01}; 
		VLT_U8 xPoint_K233[] = {0x00,0x00,0x01,0x72,0x32,0xBA,0x85,0x3A,0x7E,0x73,0x1A,0xF1,0x29,0xF2,0x2F,0xF4,0x14,0x95,0x63,0xA4,0x19,0xC2,0x6B,0xF5,0x0A,0x4C,0x9D,0x6E,0xEF,0xAD,0x61,0x26}; 
		VLT_U8 yPoint_K233[] = {0x00,0x00,0x01,0xDB,0x53,0x7D,0xEC,0xE8,0x19,0xB7,0xF7,0x0F,0x55,0x5A,0x67,0xC4,0x27,0xA8,0xCD,0x9B,0xF1,0x8A,0xEB,0x9B,0x56,0xE0,0xC1,0x10,0x56,0xFA,0xE6,0xA3}; 
		VLT_U8 zPoint_K233[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01}; 
		VLT_U8 coeffA_K233[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; 
		VLT_U8 coeffB_K233[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01}; 
		VLT_U8 orderN_K233[] = {0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x9D,0x5B,0xB9,0x15,0xBC,0xD4,0x6E,0xFB,0x1A,0xD5,0xF1,0x73,0xAB,0xDF}; 
		VLT_U32 cofactorH_K233 = 0x00000004; 

		//B283
		VLT_U8 primeQ_B283[] = {0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0xA1}; 
		VLT_U8 xPoint_B283[] = {0x05,0xF9,0x39,0x25,0x8D,0xB7,0xDD,0x90,0xE1,0x93,0x4F,0x8C,0x70,0xB0,0xDF,0xEC,0x2E,0xED,0x25,0xB8,0x55,0x7E,0xAC,0x9C,0x80,0xE2,0xE1,0x98,0xF8,0xCD,0xBE,0xCD,0x86,0xB1,0x20,0x53}; 
		VLT_U8 yPoint_B283[] = {0x03,0x67,0x68,0x54,0xFE,0x24,0x14,0x1C,0xB9,0x8F,0xE6,0xD4,0xB2,0x0D,0x02,0xB4,0x51,0x6F,0xF7,0x02,0x35,0x0E,0xDD,0xB0,0x82,0x67,0x79,0xC8,0x13,0xF0,0xDF,0x45,0xBE,0x81,0x12,0xF4}; 
		VLT_U8 zPoint_B283[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01}; 
		VLT_U8 coeffA_B283[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01}; 
		VLT_U8 coeffB_B283[] = {0x02,0x7B,0x68,0x0A,0xC8,0xB8,0x59,0x6D,0xA5,0xA4,0xAF,0x8A,0x19,0xA0,0x30,0x3F,0xCA,0x97,0xFD,0x76,0x45,0x30,0x9F,0xA2,0xA5,0x81,0x48,0x5A,0xF6,0x26,0x3E,0x31,0x3B,0x79,0xA2,0xF5}; 
		VLT_U8 orderN_B283[] = {0x03,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xEF,0x90,0x39,0x96,0x60,0xFC,0x93,0x8A,0x90,0x16,0x5B,0x04,0x2A,0x7C,0xEF,0xAD,0xB3,0x07}; 
		VLT_U32 cofactorH_B283 = 0x00000002;

		//K283
		VLT_U8 primeQ_K283[] = {0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0xA1}; 
		VLT_U8 xPoint_K283[] = {0x05, 0x03, 0x21, 0x3F, 0x78, 0xCA, 0x44, 0x88, 0x3F, 0x1A, 0x3B, 0x81, 0x62, 0xF1, 0x88, 0xE5, 0x53, 0xCD, 0x26, 0x5F, 0x23, 0xC1, 0x56, 0x7A, 0x16, 0x87, 0x69, 0x13, 0xB0, 0xC2, 0xAC, 0x24, 0x58, 0x49, 0x28, 0x36}; 
		VLT_U8 yPoint_K283[] = {0x01, 0xCC, 0xDA, 0x38, 0x0F, 0x1C, 0x9E, 0x31, 0x8D, 0x90, 0xF9, 0x5D, 0x07, 0xE5, 0x42, 0x6F, 0xE8, 0x7E, 0x45, 0xC0, 0xE8, 0x18, 0x46, 0x98, 0xE4, 0x59, 0x62, 0x36, 0x4E, 0x34, 0x11, 0x61, 0x77, 0xDD, 0x22, 0x59}; 
		VLT_U8 zPoint_K283[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}; 
		VLT_U8 coeffA_K283[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; 
		VLT_U8 coeffB_K283[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}; 
		VLT_U8 orderN_K283[] = {0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE9, 0xAE, 0x2E, 0xD0, 0x75, 0x77, 0x26, 0x5D, 0xFF, 0x7F, 0x94, 0x45, 0x1E, 0x06, 0x1E, 0x16, 0x3C, 0x61}; 
		VLT_U32 cofactorH_K283 = 0x00000004;

        VLT_SA_CRYPTO_PARAMS cryptoParams;
        cryptoParams.u8AlgID = VLT_ALG_SIG_ECDSA_GF2M;
    
		if (strcurve.compare("B233") == 0)
		{
			cryptoParams.params.ecdsa.domainParams.u16QLen = sizeof(primeQ_B233);
			cryptoParams.params.ecdsa.domainParams.pu8Q = primeQ_B233;
			cryptoParams.params.ecdsa.domainParams.pu8A = coeffA_B233;
			cryptoParams.params.ecdsa.domainParams.pu8B = coeffB_B233;
			cryptoParams.params.ecdsa.domainParams.pu8Gx = xPoint_B233;
			cryptoParams.params.ecdsa.domainParams.pu8Gy = yPoint_B233;
			cryptoParams.params.ecdsa.domainParams.pu8Gz = zPoint_B233;
			cryptoParams.params.ecdsa.domainParams.u16NLen = sizeof(orderN_B233);
			cryptoParams.params.ecdsa.domainParams.pu8N = orderN_B233;        
			cryptoParams.params.ecdsa.domainParams.u32H = cofactorH_B233;
		}
		else if (strcurve.compare("K233") == 0)
		{
			cryptoParams.params.ecdsa.domainParams.u16QLen = sizeof(primeQ_K233);
			cryptoParams.params.ecdsa.domainParams.pu8Q = primeQ_K233;
			cryptoParams.params.ecdsa.domainParams.pu8A = coeffA_K233;
			cryptoParams.params.ecdsa.domainParams.pu8B = coeffB_K233;
			cryptoParams.params.ecdsa.domainParams.pu8Gx = xPoint_K233;
			cryptoParams.params.ecdsa.domainParams.pu8Gy = yPoint_K233;
			cryptoParams.params.ecdsa.domainParams.pu8Gz = zPoint_K233;
			cryptoParams.params.ecdsa.domainParams.u16NLen = sizeof(orderN_K233);
			cryptoParams.params.ecdsa.domainParams.pu8N = orderN_K233;        
			cryptoParams.params.ecdsa.domainParams.u32H = cofactorH_K233;
		}
		else if (strcurve.compare("B283") == 0)
		{
			cryptoParams.params.ecdsa.domainParams.u16QLen = sizeof(primeQ_B283);
			cryptoParams.params.ecdsa.domainParams.pu8Q = primeQ_B283;
			cryptoParams.params.ecdsa.domainParams.pu8A = coeffA_B283;
			cryptoParams.params.ecdsa.domainParams.pu8B = coeffB_B283;
			cryptoParams.params.ecdsa.domainParams.pu8Gx = xPoint_B283;
			cryptoParams.params.ecdsa.domainParams.pu8Gy = yPoint_B283;
			cryptoParams.params.ecdsa.domainParams.pu8Gz = zPoint_B283;
			cryptoParams.params.ecdsa.domainParams.u16NLen = sizeof(orderN_B283);
			cryptoParams.params.ecdsa.domainParams.pu8N = orderN_B283;        
			cryptoParams.params.ecdsa.domainParams.u32H = cofactorH_B283;
		}
		else if (strcurve.compare("K283") == 0)
		{
			cryptoParams.params.ecdsa.domainParams.u16QLen = sizeof(primeQ_K283);
			cryptoParams.params.ecdsa.domainParams.pu8Q = primeQ_K283;
			cryptoParams.params.ecdsa.domainParams.pu8A = coeffA_K283;
			cryptoParams.params.ecdsa.domainParams.pu8B = coeffB_K283;
			cryptoParams.params.ecdsa.domainParams.pu8Gx = xPoint_K283;
			cryptoParams.params.ecdsa.domainParams.pu8Gy = yPoint_K283;
			cryptoParams.params.ecdsa.domainParams.pu8Gz = zPoint_K283;
			cryptoParams.params.ecdsa.domainParams.u16NLen = sizeof(orderN_K283);
			cryptoParams.params.ecdsa.domainParams.pu8N = orderN_K283;        
			cryptoParams.params.ecdsa.domainParams.u32H = cofactorH_K283;
		}
		else
		{
			 cout << "Curve param must be B233 or K233 or B283 or K283" << endl;
		}

        //
        // Setup the host's private key
        cryptoParams.params.ecdsa.hostPrivateKey.u16DLen = ucHostPrivLength;
		cryptoParams.params.ecdsa.hostPrivateKey.pu8D = aucHostPriv;
        
		VLT_KEY_OBJECT devpubKey;
		devpubKey.u8KeyID = VLT_KEY_ECDSA_GF2M_PUB;
		devpubKey.data.EcdsaPubKey.pu8Qx = (VLT_PU8)malloc(0x50);
		devpubKey.data.EcdsaPubKey.pu8Qy = (VLT_PU8)malloc(0x50);

		if( VLT_OK != ( usActualSW = theBaseApi->VltReadKey(0x01,0x01,&devpubKey) ) )
        {
           CloseAndExit(hMod, usActualSW, "VltReadKey VLT_KEY_ECDSA_GF2M_PUB");  
        }

        //
        // Setup the device's public key
		cryptoParams.params.ecdsa.devicePublicKey.u16QLen = devpubKey.data.EcdsaPubKey.u16QLen;
		cryptoParams.params.ecdsa.devicePublicKey.pu8Qx = devpubKey.data.EcdsaPubKey.pu8Qx;
        cryptoParams.params.ecdsa.devicePublicKey.pu8Qy = devpubKey.data.EcdsaPubKey.pu8Qy;
        

		VLT_KEY_OBJECT devID;
		devID.u8KeyID = VLT_KEY_HOST_DEVICE_ID;
		devID.data.HostDeviceIdKey.pu8StringId = (VLT_PU8)malloc(VLT_HOST_DEV_ID_STRING_LENGTH);
		devID.data.HostDeviceIdKey.u16StringLen = VLT_HOST_DEV_ID_STRING_LENGTH;
		if( VLT_OK != ( usActualSW = theBaseApi->VltReadKey(0x01,0x04,&devID) ) )
        {
           CloseAndExit(hMod, usActualSW, "VltReadKey VLT_KEY_HOST_DEVICE_ID");  
        }

		VLT_KEY_OBJECT hostID;
		hostID.u8KeyID = VLT_KEY_HOST_DEVICE_ID;
		hostID.data.HostDeviceIdKey.pu8StringId = (VLT_PU8)malloc(VLT_HOST_DEV_ID_STRING_LENGTH);
		hostID.data.HostDeviceIdKey.u16StringLen = VLT_HOST_DEV_ID_STRING_LENGTH;
		if( VLT_OK != ( usActualSW = theBaseApi->VltReadKey(0x02,0x04,&hostID) ) )
        {
			CloseAndExit(hMod, usActualSW, "VltReadKey VLT_KEY_HOST_DEVICE_ID");  
        }

        //
        // Setup the authentication protocol parameters.
        VLT_SA_PROTOCOL_PARAMS protocolParams;
        protocolParams.u8AuthModeID = VLT_AUTH_MUTUAL_NON_FIPS;
        protocolParams.u8UserID = VLT_USER;
        protocolParams.u8RoleID = VLT_APPROVED_USER;
        protocolParams.u8DeviceIdLength = VLT_HOST_DEV_ID_STRING_LENGTH;
        protocolParams.pu8DeviceId = devID.data.HostDeviceIdKey.pu8StringId;
        protocolParams.u8HostIdLength = VLT_HOST_DEV_ID_STRING_LENGTH;
        protocolParams.pu8HostId = hostID.data.HostDeviceIdKey.pu8StringId;
        
        //
        // Perform FIPS Mutual Authentication
		usActualSW = theStrongAuth->VltStrongAuthenticate( &cryptoParams, 
            &protocolParams );

		free(devpubKey.data.EcdsaPubKey.pu8Qx);
		free(devpubKey.data.EcdsaPubKey.pu8Qy);
		free(hostID.data.HostDeviceIdKey.pu8StringId);
		free(devID.data.HostDeviceIdKey.pu8StringId);

        if( VLT_OK != ( usActualSW ) )
        {
            CloseAndExit(hMod, usActualSW, "VltStrongAuthenticate");  
        }


        //---------------------------------------------------------------------
        // 2: Check the state of the authentication service.
        //---------------------------------------------------------------------
        unsigned char ucState = 0;

        if ( VLT_OK != ( usActualSW = theStrongAuth->VltStrongGetState( &ucState ) ) )
        {
             CloseAndExit(hMod, usActualSW, "VltStrongGetState");  
        }

        if ( VLT_USER_AUTHENTICATED != ucState )
        {
            CloseAndExit(hMod, usActualSW, "VLT_USER_AUTHENTICATED");  
        }

		//---------------------------------------------------------------------
        // 3: Deactivate chip
        //---------------------------------------------------------------------
		if ( VLT_OK != ( usActualSW = theBaseApi->VltDeactivate(aucKeyId,ucKeyIdLength,aucKeyData,ucKeyDataLength) ))
        {
            CloseAndExit(hMod, usActualSW, "VltDeactivate");     
        }

		//---------------------------------------------------------------------
        // 4: Get Info
        //---------------------------------------------------------------------
		VLT_TARGET_INFO chipInfo;
		if ( VLT_OK != ( usActualSW = theBaseApi->VltGetInfo(&chipInfo)))
        {
           CloseAndExit(hMod, usActualSW, "VltGetInfo");     
        }

		printf("Chip Serial Number: ");
		for (unsigned int i = 0; i < sizeof(chipInfo.au8Serial); i++)
			printf("%02X", chipInfo.au8Serial[i]/*, ((i + 1) % 16 )*/);
		printf("\n");


        CloseAndExit(hMod, usActualSW, "Example code SUCCESSFULLY complete, VaultIC file system wiped.");     
	}

	catch(...)
	{
		CloseAndExit(hMod, usActualSW, "Unknown Exception caught.");     
	}

    return 0;
}