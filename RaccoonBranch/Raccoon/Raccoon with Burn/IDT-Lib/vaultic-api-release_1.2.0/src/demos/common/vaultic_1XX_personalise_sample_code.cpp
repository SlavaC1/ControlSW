// -----------------------------------------------------------------------------
// VaultIC_100_PersonaliseSampleCode.cpp : 
// -----------------------------------------------------------------------------
// Defines a procedure for demonstrating how to personalise the VaultIC 100
// for authentication in NON FIPS mode.  NON FIPS mode offers the 
// host the ability to define a custom ECDSA curve up to 303 bits.  
// The NON FIPS mode also offers the ability to transition back to the creation 
// state, wiping the keys and file system contents.
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

    VAULTIC_API* theBaseApi = 0;
    VAULTIC_STRONG_AUTHENTICATION* theStrongAuth;
    pfnGetApi VltGetApi;
    pfnInitLibrary VltInitLibrary;
    pfnCloseLibrary VltCloseLibrary = NULL;
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
			 CloseAndExit(hMod, VLT_NO_READER_CONNECTED, "VltFindDevices");
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
			printf("Invalid selection\n");
			 CloseAndExit(hMod, VLT_NO_READER_CONNECTED, "VltFindDevice");

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
        // The Api is setup, its ok to call interface funtions
        //---------------------------------------------------------------------
        // 1: Login as the manufacturer.
        //---------------------------------------------------------------------
        if ( VLT_OK != (usActualSW = theBaseApi->VltSubmitPassword( ucManufPasswordLength,
                aucManufPassword) ) )
        {
              CloseAndExit(hMod, usActualSW, "VltSubmitPassword Manuf password");
        }

		//---------------------------------------------------------------------
        // 2: Transition to the CREATION state.
        //---------------------------------------------------------------------
		if (VLT_OK != ( usActualSW = theBaseApi->VltSetStatus( VLT_CREATION) ) )
        {
             CloseAndExit(hMod, usActualSW, "VltSetStatus CREATION");
        }

        //---------------------------------------------------------------------
        // 3. Select the User Defined Params File
        //---------------------------------------------------------------------
        unsigned char ucCurve = VLT_ECDSA_PARAMS_USER_DEF_CURVE_CHOICE;

        if ( VLT_OK != ( usActualSW = theBaseApi->VltSetConfig( VLT_ECDSA_DOMAIN_PARAM_CHOICE, 
            VLT_ECDSA_PARAMS_CHOICE_SZ, &ucCurve ) ) )
        {
             CloseAndExit(hMod, usActualSW, "VltSetConfig VLT_ECDSA_DOMAIN_PARAM_CHOICE");
        }

        //---------------------------------------------------------------------
        // 4. B-283-R1 is used as an example curve, curves upto 303 bits
        // can be used, if this is desired, disable FIPS mode.
        //---------------------------------------------------------------------
		//Be careful with FIPS mode, if activated the manufacturer role will be unavailable

        unsigned char ucFipsMode = 0;

        if ( VLT_OK != ( usActualSW = theBaseApi->VltSetConfig( VLT_FIPS_MODE, 
                VLT_FIPS_MODE_SZ, &ucFipsMode ) ) )
        {
           CloseAndExit(hMod, usActualSW, "VltSetConfig FIPS MODE OFF");
        }

        //---------------------------------------------------------------------
        // 5. Store the Host ID Key in the Secure Object file system.
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
        // 6. Store the Device ID Key in the Secure Object file system.
        //---------------------------------------------------------------------
        KeyObj.data.HostDeviceIdKey.pu8StringId = aucDeviceID;

        if ( VLT_OK != ( usActualSW = theBaseApi->VltPutKey( (VLT_U8)enDeviceKeyGroup,
                (VLT_U8)enHostDeviceIDKeyIndex,
                &KeyPrivilages,
                &KeyObj ) ) )
        {
            CloseAndExit(hMod, usActualSW, "VltPutKey device ID");
        }

        //---------------------------------------------------------------------
        // 7: Store the Host defined domain parameters key in the Secure 
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
            CloseAndExit(hMod, usActualSW, "VltPutKey Domain param");
        }

        //---------------------------------------------------------------------
        // 8: Store the Host's Public Key in the Secure Object file system.
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
            CloseAndExit(hMod, usActualSW, "VltPutKey host public key");
        }

        //---------------------------------------------------------------------
        // 9: Store the Device's Private Key in the Secure Object file system.
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
			CloseAndExit(hMod, usActualSW, "VltPutKey device private key");
        }

        //---------------------------------------------------------------------
        // 10: Store the Device's Public Key in the Secure Object file system.
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
            CloseAndExit(hMod, usActualSW, "VltPutKey device public key");
        }

        //---------------------------------------------------------------------
        // 11: Transition to the ACTIVE state.
        //---------------------------------------------------------------------
        if (VLT_OK != ( usActualSW = theBaseApi->VltSetStatus( VLT_OPERATIONAL_ACTIVE ) ) )
        {
            CloseAndExit(hMod, usActualSW, "VltSetStatus OPERATIONAL ACTIVE");
        }

        //---------------------------------------------------------------------
        // 12: Log the manufacturer out of the VaultIC Secure Object.
        //---------------------------------------------------------------------
        if (VLT_OK != ( usActualSW = theBaseApi->VltCancelAuthentication(  ) ) )
        {
             CloseAndExit(hMod, usActualSW, "VltCancelAuthentication");
        }
		CloseAndExit(hMod, usActualSW, "Example code complete, VaultIC set to OPERATIONAL ACTIVE");             
    }
	catch(...)
	{
		CloseAndExit(hMod, usActualSW, "Unknown Exception caught.");         
    }
    return 0;
}
