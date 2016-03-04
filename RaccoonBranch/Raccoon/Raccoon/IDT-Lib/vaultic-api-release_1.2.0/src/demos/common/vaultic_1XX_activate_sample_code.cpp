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
    if ( 2 > argc )
    {
        std::cout << "Please ensure the key data are provided in parameters" << endl;
	
        return( 1 );
    }

	std::string strKeyData(argv[1]);


	if ( strKeyData.length()/2 != 32 ) 
    {
        cout << "Please ensure the keydata is 32 "
            "bytes(64 char) in length." << endl;
	
        return( 1 );
    }
	unsigned char aucKeyData[32];

	HexStringToByteArray(strKeyData,aucKeyData);

	unsigned char ucKeyDataLength = (unsigned char)strKeyData.size()/2;
    
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
			CloseAndExit(hMod,VLT_FIND_DEVS_MALLOC_FAIL,"Malloc"); 
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
			CloseAndExit(hMod, VLT_NO_READER_CONNECTED, "No interface found\n");  
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
			CloseAndExit(hMod, VLT_GET_API_FAILED, "VltGetApi");  
        }

	    //
	    // Get the Strong Authentication Interface.
        if ( 0 == ( theStrongAuth = VltGetStrongAuthentication( ) ) )
        {
            CloseAndExit(hMod, VLT_GET_AUTH_FAILED, "VltGetStrongAuthentication");  
        }

		//---------------------------------------------------------------------
        // 1: Get Info
        //---------------------------------------------------------------------
		VLT_TARGET_INFO chipInfo;
		if ( VLT_OK != ( usActualSW = theBaseApi->VltGetInfo(&chipInfo)))
        {
            CloseAndExit(hMod, usActualSW, "VltGetInfo");  
        }

		printf("Chip Serial Number before activation: ");
		for (unsigned int i = 0; i < sizeof(chipInfo.au8Serial); i++)
			printf("%02X", chipInfo.au8Serial[i]/*, ((i + 1) % 16 )*/);
		printf("\n");

		//---------------------------------------------------------------------
        // 2: Get Challenge
        //---------------------------------------------------------------------
		VLT_U8 challenge[CHALLENGE_SIZE];
		VLT_U8 challengeLen = CHALLENGE_SIZE;

		if ( VLT_OK != ( usActualSW = theBaseApi->VltGetChallenge(challenge,&challengeLen) ))
        {
            CloseAndExit(hMod, usActualSW, "VltGetChallenge");  
        }

		//---------------------------------------------------------------------
        // 3: Activate chip
        //---------------------------------------------------------------------
		if ( VLT_OK != ( usActualSW = theBaseApi->VltActivate(challenge,challengeLen,aucKeyData,ucKeyDataLength) ))
        {
            CloseAndExit(hMod, usActualSW, "VltActivate");  
        }

		//---------------------------------------------------------------------
        // 4: Get Info
        //---------------------------------------------------------------------
		if ( VLT_OK != ( usActualSW = theBaseApi->VltGetInfo(&chipInfo)))
        {
            CloseAndExit(hMod, usActualSW, "VltGetInfo");      
        }

		printf("Chip Serial Number after activation: ");
		for (unsigned int i = 0; i < sizeof(chipInfo.au8Serial); i++)
			printf("%02X", chipInfo.au8Serial[i]/*, ((i + 1) % 16 )*/);
		printf("\n");


        //---------------------------------------------------------------------
        // 5: Log out user.  
        // Note using the theApi->VltCancelAuthentication( ) 
        // method will leave the strong authentication service intalised.
        //---------------------------------------------------------------------
        if ( VLT_OK != ( usActualSW = theStrongAuth->VltStrongClose( ) ) )
        {
			CloseAndExit(hMod, usActualSW, "VltStrongClose");        
        }
		CloseAndExit(hMod, usActualSW, "Example code complete, VaultIC Activated.");                        
    }
	catch(...)
	{
       CloseAndExit(hMod, usActualSW, "Unknown Exception caught.");              
    }

    return 0;
}