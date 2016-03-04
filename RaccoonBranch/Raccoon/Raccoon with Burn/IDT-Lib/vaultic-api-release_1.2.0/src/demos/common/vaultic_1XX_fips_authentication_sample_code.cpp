// -----------------------------------------------------------------------------
// VaultIC_100_FipsAuthenticationSampleCode.cpp : 
// -----------------------------------------------------------------------------
// Defines a procedure for demonstrating how to authenticate the VaultIC 100
// approved user in FIPS mode.   FIPS mode adds an additional step to the 
// authentication to ensure the host's public key is valid.  The FIPS mode
// also offers increased security: - manufacturers password is disabled in the
// active state. FIPS mode requires one of the preloaded curves 
// (B-233-R1, B-283-R1, K233, K283).
//
// Prerequisites: Run the VaultIC_100_FipsPersonaliseSampleCode.exe first.
//
// -----------------------------------------------------------------------------
#include "common.h"
#include "B283_R1_Curve.h"
#include "vaultic_1XX_sample_code.h"

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

    VLT_STS status = VLT_FAIL;
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
    
    try
    {
		if (2 == argc)
		{
			lPrefferedInterface = strtol(argv[1], NULL, 10);
		}

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
        // 1: Get the Domain parameter values for B-283-R1.
        //---------------------------------------------------------------------
        VLT_SA_CRYPTO_PARAMS cryptoParams;

        cryptoParams.u8AlgID = VLT_ALG_SIG_ECDSA_GF2M;

        if( VLT_OK != ( usActualSW = theStrongAuth->VltStrongSetCryptoParams(
                VLT_ECDSA_CURVE_B283,
                &cryptoParams ) ) )
        {
            CloseAndExit(hMod, usActualSW, "VltStrongSetCryptoParams");
        }



        //---------------------------------------------------------------------
        // 2: Perform FIPS Mutual Authentication.
        //---------------------------------------------------------------------
        VLT_SA_PROTOCOL_PARAMS protocolParams;
        
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
        protocolParams.u8AuthModeID = VLT_AUTH_MUTUAL_FIPS;
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
        // 3: Check the state of the authentication service.
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
        // 4: Check we are logged in by accessing a restricted command:
        // VltInitializeAlgorithm (sets up the vaultic crypto engine).
        //---------------------------------------------------------------------
		VLT_ALGO_PARAMS algorithm;
		algorithm.u8AlgoID = VLT_ALG_DIG_SHA256;

		if ( VLT_OK != (usActualSW = theBaseApi->VltInitializeAlgorithm(
		        0, 0, VLT_DIGEST_MODE, &algorithm )))
        {
            CloseAndExit(hMod, usActualSW, "VltInitializeAlgorithm");
        }


        //---------------------------------------------------------------------
        // 5: Check the vaultic crypto engine was setup for a SHA-256 bit hash.
        //---------------------------------------------------------------------
		unsigned char aucMessage[] = "Hello world!";
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
        cout << "The SHA-256 of \"Hello World!\" is: " << strHash << endl;


 
        //---------------------------------------------------------------------
        // 6: Log out user.  
        // Note using the theApi->VltCancelAuthentication( ) 
        // method will leave the strong authentication service intalised.
        //---------------------------------------------------------------------
        if ( VLT_OK != ( usActualSW = theStrongAuth->VltStrongClose( ) ) )
        {
           CloseAndExit(hMod, usActualSW, "VltStrongClose");
        }
       
		CloseAndExit(hMod, usActualSW, "Example code complete, VaultIC set to OPERATIONAL ACTIVE");             
    }
	catch(...)
	{
		CloseAndExit(hMod, usActualSW, "Unknown Exception caught.");         
    }

    return 0;
}


