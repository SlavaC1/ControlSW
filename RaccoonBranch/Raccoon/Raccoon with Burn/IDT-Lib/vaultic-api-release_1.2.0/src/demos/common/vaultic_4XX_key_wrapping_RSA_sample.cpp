// vaultic_4XX_key_wrapping_sample_RSA.cpp : Defines the entry point for the console application.
//

#include "common.h"

#define USER_PIN_ID			0x00

#ifndef USE_STATIC_API
/*******************************************************
 VaultIC SDK DLL Methods:
*******************************************************/
typedef VLT_STS (*pfnInitLibrary)(VLT_INIT_COMMS_PARAMS* initCommsParams);
typedef VLT_STS (*pfnCloseLibrary)(void);
typedef VAULTIC_API* (*pfnGetApi)(void);
typedef VAULTIC_AUTH* (*pfnGetAuth)( void );
typedef VAULTIC_KEY_WRAPPING* (*pfnGetKeyWrapping)( void );
typedef VLT_STS (*pfnGetLibraryInfo)( VLT_LIBRARY_INFO* pLibraryInfo );
typedef VLT_STS (*pfnFindDevices)( VLT_PU32 pSize, VLT_PU8 pXmlReaderString );
#endif

/*******************************************************
 Local function prototypes:
*******************************************************/
bool DisplayBuffer(const unsigned long& ulCipherTextSize,
                   const unsigned char* pucBuffer);


using namespace std;

#define SUPPORTED( flags, value )   static_cast<bool>(( flags & value ) == value )

#if( VLT_PLATFORM == VLT_WINDOWS )
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char** argv)
#endif
{ 
#if( VLT_PLATFORM == VLT_WINDOWS )
    HMODULE hMod = NULL;
#else
    void* hMod = NULL;
#endif

#ifndef USE_STATIC_API   
    pfnGetApi VltGetApi;
    pfnGetAuth VltGetAuth;
	pfnGetKeyWrapping VltGetKeyWrapping;
    pfnInitLibrary VltInitLibrary;
    pfnCloseLibrary VltCloseLibrary;
    pfnGetLibraryInfo VltGetLibraryInfo;
    pfnFindDevices VltFindDevices;
#endif    
    
    string strXMLDevs;
    const unsigned char ucMinPasswordLength = 4;
    const unsigned char ucMaxPasswordLength = 32;
    const unsigned short usStartupDelay_1_0_X = 15000;
    const unsigned short usStartupDelay_1_1_0 = 2500;
    unsigned short usActualSW = 0;
	long lPrefferedInterface = 0;

    VAULTIC_API* theBaseApi = 0;
	VAULTIC_KEY_WRAPPING* theKeyWrappingApi = 0;
    VAULTIC_AUTH* theAuthSvr = 0;

    //
    // Check the command line arguments are valid
    //
    if ( 2 > argc )
    {
        cout << "Please ensure the manufacturer's password has "
            "been passed in using the command line:" << endl;
	
        return( 1 );
    }

    std::string strManufPassword(argv[1]);
    unsigned char ucManufPasswordLength = (unsigned char)strManufPassword.size();

    //
    // Check the supplied password is a valid length.
    //
    if ( ( ucManufPasswordLength < ucMinPasswordLength ) ||
         ( ucManufPasswordLength > ucMaxPasswordLength ) )
		 CloseAndExit(hMod,1,"Please ensure the manufacturer's password is between 4 characters and 32 characters"); 
    
	if (3 == argc)
	{
		lPrefferedInterface = strtol(argv[2], NULL, 10);
	}
    try
    {
#ifndef USE_STATIC_API 
       //
        // The path to the library is stored in the project settings.
        hMod = OpenLibrary( LIB_PATH_4XX );
        
        if (hMod == NULL)
        {
    	    return ( VLT_GETPROC_INIT_LIB_FAILED );
        }   
       
        //
        // Get the function pointer to the VltInitLibrary method.  
        // This method is used to initialise the entire API, it may 
        // allocated resources as required.
        if( NULL == ( VltInitLibrary = (pfnInitLibrary)GetLibSymbol( hMod,
            "VltInitLibrary" ) ) )
			CloseAndExit(hMod,VLT_GETPROC_INIT_LIB_FAILED,"GetLibSymbol (VltInitLibrary) failed");       
        
        //
        // Get the function pointer to the VltCloseLibrary method.  This method
        // is used release resources allocated by previous API calls.
        if( NULL == ( VltCloseLibrary = (pfnCloseLibrary)GetLibSymbol( hMod,
            "VltCloseLibrary" ) ) )
			CloseAndExit(hMod,VLT_GETPROC_ClOSE_LIB_FAILED,"GetLibSymbol (VltCloseLibrary) failed");          

        //    
        // Get the function pointer to the VltGetApi method.  This method
        // is used to obtain a pointer to the API Interface 
        if( NULL == ( VltGetApi = (pfnGetApi)GetLibSymbol( hMod, "VltGetApi" ) ) )
			CloseAndExit(hMod,VLT_GETPROC_GET_API_FAILED,"GetLibSymbol (VltGetApi) failed");  

		// Get the function pointer to the VltGetKeyWrapping method.  This method
        // is used to obtain a pointer to the API Interface 
        if( NULL == ( VltGetKeyWrapping = (pfnGetKeyWrapping)GetLibSymbol( hMod, "VltGetKeyWrapping" ) ) )
			CloseAndExit(hMod,VLT_GETPROC_GET_KEYWRAPPING_FAILED,"GetLibSymbol (VltGetKeyWrapping) failed"); 
        
        //    
        // Get the function pointer to the VltGetScp method.  This method
        // is used to obtain a pointer to structure of Secure Channel Protocol
        // service Interface.
        if( NULL == ( VltGetAuth = (pfnGetAuth)GetLibSymbol( hMod, "VltGetAuth" ) ) )
			CloseAndExit(hMod,VLT_GETPROC_GET_AUTH_FAILED,"GetLibSymbol (VltGetAuth) failed"); 
      
        //    
        // Get the function pointer to the VltGetLibraryInfo method. This method
        // is used to obtain information about the library.
        if( NULL == ( VltGetLibraryInfo = (pfnGetLibraryInfo)GetLibSymbol( hMod, "VltGetLibraryInfo" ) ) )
			CloseAndExit(hMod,VLT_GETPROC_GET_LIBINFO_FAILED,"GetLibSymbol (VltGetLibraryInfo) failed"); 
        
        //    
        // Get the function pointer to the VltGetLibraryInfo method. This method
        // is used to obtain information about the library.
        if( NULL == ( VltFindDevices = (pfnFindDevices)GetLibSymbol( hMod, "VltFindDevices" ) ) )
			CloseAndExit(hMod,VLT_GETPROC_GET_FINDDEVS_FAILED,"GetLibSymbol (VltFindDevices) failed"); 
  #endif 

        VLT_LIBRARY_INFO libInfo;
        VLT_STS status = VLT_FAIL;

        //
        // Determine the library capabilities and print those out in the 
        // standard output. 
        //
        if( VLT_OK != ( status = VltGetLibraryInfo( &libInfo ) ) )
        {
#ifndef USE_STATIC_API
            CloseLibrary( hMod );
#endif
            return( status );
        }

        else
        {
            cout << "==========================================================" << endl;
            cout << "                  Library Information                     " << endl;
            cout << "==========================================================" << endl;
            cout << "Library version : " << libInfo.pVersion << endl;
                        
            cout << "    Put/Read Secret Key Support Enabled : " << boolalpha << 
                SUPPORTED( libInfo.capabilities, VLT_CPB_ENABLE_KEY_SECRET ) << endl;
            cout << "    Put/Read HOTP Key Support Enabled: " << boolalpha << 
                SUPPORTED( libInfo.capabilities, VLT_CPB_ENABLE_KEY_HOTP ) << endl;
            cout << "    Put/Read TOTP Key Support Enabled: " << boolalpha << 
                SUPPORTED( libInfo.capabilities, VLT_CPB_ENABLE_KEY_TOTP ) << endl;
            cout << "    Put/Read RSA Key Support Enabled : " << boolalpha << 
                SUPPORTED( libInfo.capabilities, VLT_CPB_ENABLE_KEY_RSA ) << endl;
            cout << "    Put/Read DSA Key Support Enabled : " << boolalpha << 
                SUPPORTED( libInfo.capabilities,  VLT_CPB_ENABLE_KEY_DSA ) << endl;
            cout << "    Put/Read ECDSA Key Support Enabled : " << boolalpha << 
                SUPPORTED( libInfo.capabilities,  VLT_CPB_ENABLE_KEY_ECDSA ) << endl;
            cout << "    Cipher Self Tests Support Enabled : " << boolalpha << 
                SUPPORTED( libInfo.capabilities, VLT_CPB_ENABLE_CIPHER_TESTS ) << endl;
            cout << "    Identity Authentication Service Support Enabled: " << boolalpha <<
                SUPPORTED( libInfo.capabilities,  VLT_CPB_ENABLE_IDENTITY_AUTH )<< endl; 
            cout << "    SCP02 Secure Channel Support Enabled: " << boolalpha << 
                SUPPORTED( libInfo.capabilities,  VLT_CPB_ENABLE_SCP02 ) << endl;
            cout << "    SCP03 Secure Channel Support Enabled: " << boolalpha << 
                SUPPORTED( libInfo.capabilities,  VLT_CPB_ENABLE_SCP03 ) << endl;
            cout << "    Microsoft Minicard Driver Support Enabled: " << boolalpha << 
                SUPPORTED( libInfo.capabilities,  VLT_CPB_ENABLE_MS_AUTH ) << endl;
            cout << "    Block Protocol Support Enabled: " << boolalpha << 
                SUPPORTED( libInfo.capabilities,  VLT_CPB_ENABLE_BLOCK_PROTOCOL ) << endl;
            cout << "    TWI Interface Support Enabled: " << boolalpha <<             
                SUPPORTED( libInfo.capabilities,  VLT_CPB_ENABLE_TWI ) << endl;
            cout << "    SPI Interface Support Enabled: " << boolalpha <<
                SUPPORTED( libInfo.capabilities,  VLT_CPB_ENABLE_SPI ) << endl;
            cout << "    ISO7816 Protocol Support Enabled: " << boolalpha <<             
                SUPPORTED( libInfo.capabilities,  VLT_CPB_ENABLE_ISO7816 ) << endl;           
            cout << "    AES(128, 192, 256) Support Enabled: " << boolalpha <<
                SUPPORTED( libInfo.capabilities,  VLT_CPB_ENABLE_CIPHER_AES ) << endl;
            cout << "    TDES(EEE& EDE modes) Support Enabled: " << boolalpha <<
                SUPPORTED( libInfo.capabilities,  VLT_CPB_ENABLE_CIPHER_TDES ) << endl;
            cout << "    DES Support Enabled: " << boolalpha <<
                SUPPORTED( libInfo.capabilities,  VLT_CPB_ENABLE_CIPHER_DES ) << endl;
            cout << "    Fast CRC16 CCIT Support Enabled: " << boolalpha <<
                SUPPORTED( libInfo.capabilities,  VLT_CPB_ENABLE_FAST_CRC16CCIT ) << endl;
            cout << "    Key Wrapping Service Support Enabled: " << boolalpha <<
                SUPPORTED( libInfo.capabilities,  VLT_CPB_ENABLE_KEY_WRAPPING ) << endl;
            cout << "    Put/Read Host/Device ID Key Support Enabled: " << boolalpha <<
                SUPPORTED( libInfo.capabilities,  VLT_ENABLE_KEY_HOST_DEVICE_ID ) << endl;
            cout << "    File System Service Support Enabled: " << boolalpha <<
                SUPPORTED( libInfo.capabilities,  VLT_ENABLE_FILE_SYSTEM ) << endl;
            cout << "    Aardvark SPI Write Error suppresion support Enabled: " << boolalpha <<
                SUPPORTED( libInfo.capabilities,  VLT_ENABLE_AARDVK_SPPRSS_ERR ) << endl;
            

            if( libInfo.fwCompatibilityVersionId == 1 )
            {
                cout << "This Library is compiled for VaultIC version: v1.0.2" << endl;
            }
            if( libInfo.fwCompatibilityVersionId == 2 )
            {
                cout << "This Library is compiled for VaultIC version: v1.1.x" << endl;
            }
        }

		//
        // Determine the size of the xml string that will be returned on the second call, 
        // set the stringSize parameter to zero and the string to NULL. On return the 
        // stringSize will be set to the actual number of characters of the XML string 
        // that contains the list of devices.
        //
        VLT_U32 stringSize = 0;
        if( VLT_OK != ( status = VltFindDevices( &stringSize, NULL ) ) )
        	CloseAndExit(hMod,status,"Find device failed"); 

        //
        // Allocate enough memory to hold the xml string with the list of
        // devices.
        //
        VLT_PU8 pXmlString = (VLT_PU8)NULL;
        if( NULL == ( pXmlString = (VLT_PU8)malloc(stringSize ) ) ) 
			CloseAndExit(hMod,VLT_FIND_DEVS_MALLOC_FAIL,"Memory allocation failed"); 

        //
        // Now call the VltFindDevices again with all the appropriate parameters
        // On return the pXmlString will be filled with the device list.
        //
        if( VLT_OK != ( status = VltFindDevices( &stringSize, pXmlString ) ) )
        {
            if( NULL != pXmlString )
            {
                free( pXmlString );
                pXmlString = NULL;
            }

			CloseAndExit(hMod,status,"Find device failed (malformated XML)");
        }
        else
        {
            std::cout << pXmlString << endl;
            
            //
            // Assign the XML string to a standard string
            //
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
        //
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
#if(VLT_ENABLE_CHEETAH == VLT_ENABLE )
					//
		// Check if the XML string includes an entry for PC/SC readers
		//
		startPos = strXMLDevs.find("<interface type=\"cheetah\">");
		if( string::npos != startPos )
		{
			endPos = strXMLDevs.find("</interface>");
			if( string::npos == endPos )
				CloseAndExit(hMod,VLT_MALFORMED_XML_CHEETH_INTFC,"Find device failed (malformated XML)"); 

			string strPCSC( strXMLDevs.substr( startPos, ( endPos - startPos ) ) );

			printf("\nList of detected cheetah interfaces:");
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
						CloseAndExit(hMod, VLT_MALFORMED_XML_CHEETH_PERIPH, "Find device failed (malformated XML)");

					//
					// Get the reader name
					//
					strReaderName = strPCSC.substr(startPos, (endPos - startPos));
					
					// Display the reader name
					printf("Interface #%d, Cheetah SPI Serial Number = %s\n", item++ + INTERFACE_CHEETAH_SPI, strReaderName.c_str());

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
			CloseAndExit(hMod,VLT_NO_READER_CONNECTED,"No reader connected");
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

#if(VLT_ENABLE_CHEETAH == VLT_ENABLE )  
		case INTERFACE_CHEETAH_SPI:
			startPos = strXMLDevs.find("<interface type=\"cheetah\">");
			startPos = strXMLDevs.find(pXmlData, startPos);
			startPos += strlen(pXmlData);
			endPos = strXMLDevs.find("</peripheral>", startPos);

			// Get the reader name
			strReaderName = strXMLDevs.substr(startPos, (endPos - startPos));

			commsParams.u8CommsProtocol = VLT_SPI_COMMS;

			commsParams.Params.VltBlockProtocolParams.u16BitRate = 8800;
			commsParams.Params.VltBlockProtocolParams.u8CheckSumMode = BLK_PTCL_CHECKSUM_SUM8;
			commsParams.Params.VltBlockProtocolParams.u32AfterHdrDelay = 6;
			commsParams.Params.VltBlockProtocolParams.u32InterBlkDelay = 160;
			commsParams.Params.VltBlockProtocolParams.u32InterHdrByteDelay = 3;

			commsParams.Params.VltBlockProtocolParams.VltPeripheralParams.PeriphParams.VltSpiParams.u8AdapterType = VLT_SPI_CHEETAH_ADAPTER;
			commsParams.Params.VltBlockProtocolParams.VltPeripheralParams.u32SerialNo = strtoul( strReaderName.c_str(), NULL, 10 );
			commsParams.Params.VltBlockProtocolParams.VltPeripheralParams.PeriphParams.VltSpiParams.u32FstPollByteDelay = 6;
			commsParams.Params.VltBlockProtocolParams.VltPeripheralParams.PeriphParams.VltSpiParams.u32IntPollByteDelay = 5;
			commsParams.Params.VltBlockProtocolParams.VltPeripheralParams.PeriphParams.VltSpiParams.u32IntByteDelay = 0;
			commsParams.Params.VltBlockProtocolParams.VltPeripheralParams.PeriphParams.VltSpiParams.u32PollMaxRetries = 800000;
			commsParams.Params.VltBlockProtocolParams.VltPeripheralParams.PeriphParams.VltSpiParams.u32SlaveSelectDelay = 4;

			//
			// Add a delay to allow self tests to run before establishing comms.
			//
			if( libInfo.fwCompatibilityVersionId == 1 )
			{
				commsParams.Params.VltBlockProtocolParams.u16msSelfTestDelay = usStartupDelay_1_0_X;
			}
			if( libInfo.fwCompatibilityVersionId == 2 )
			{
				commsParams.Params.VltBlockProtocolParams.u16msSelfTestDelay = usStartupDelay_1_1_0;
			}
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
		case INTERFACE_AARDVARK_SPI:
			startPos = strXMLDevs.find("<interface type=\"aardvark\">");
			startPos = strXMLDevs.find(pXmlData, startPos);
			startPos += strlen(pXmlData);
			endPos = strXMLDevs.find("</peripheral>", startPos);

			// Get the reader name
			strReaderName = strXMLDevs.substr(startPos, (endPos - startPos));

			// Setup communications using TWI over Aarvark
			commsParams.u8CommsProtocol = VLT_SPI_COMMS;
			commsParams.Params.VltBlockProtocolParams.u16BitRate		= 125;
			commsParams.Params.VltBlockProtocolParams.u8CheckSumMode	= BLK_PTCL_CHECKSUM_SUM8;
			commsParams.Params.VltBlockProtocolParams.u32AfterHdrDelay	= 1000;	
			commsParams.Params.VltBlockProtocolParams.u32InterBlkDelay	= 1000;	
			commsParams.Params.VltBlockProtocolParams.u32InterHdrByteDelay = 1000;
			commsParams.Params.VltBlockProtocolParams.VltPeripheralParams.PeriphParams.VltSpiParams.u8AdapterType = VLT_SPI_AARDVARK_ADAPTER;
			commsParams.Params.VltBlockProtocolParams.VltPeripheralParams.u32SerialNo = strtoul( strReaderName.c_str(), NULL, 10 );
			commsParams.Params.VltBlockProtocolParams.VltPeripheralParams.PeriphParams.VltSpiParams.u32FstPollByteDelay = 3000;
			commsParams.Params.VltBlockProtocolParams.VltPeripheralParams.PeriphParams.VltSpiParams.u32IntPollByteDelay = 3000;
			commsParams.Params.VltBlockProtocolParams.VltPeripheralParams.PeriphParams.VltSpiParams.u32IntByteDelay = 1000; //NO effect useless for aardvark
			commsParams.Params.VltBlockProtocolParams.VltPeripheralParams.PeriphParams.VltSpiParams.u32PollMaxRetries = 1000;
			commsParams.Params.VltBlockProtocolParams.VltPeripheralParams.PeriphParams.VltSpiParams.u32SlaveSelectDelay = 1000; //NO effect useless for aardvark
			//
			// Add a delay to allow self tests to run before establishing comms.
			//
			commsParams.Params.VltBlockProtocolParams.u16msSelfTestDelay = 4000;

			break;
#endif

		default:
			CloseAndExit(hMod,VLT_NO_READER_CONNECTED,"Invalid selection\n");
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
		// Get the Scp Interface.
		//
		if ( 0 == ( theAuthSvr = VltGetAuth( ) ) )
		{
			CloseAndExit(hMod, VLT_GET_AUTH_FAILED, "VltGetAuth");
		}

		//
	    // Get the Key Wrapping Interface.
        //
	    if ( 0 == ( theKeyWrappingApi = VltGetKeyWrapping( ) ) )
        {
			CloseAndExit(hMod, VLT_GETPROC_GET_KEYWRAPPING_FAILED, "VltGetKeyWrapping");
        }

        //---------------------------------------------------------------------
        // The Api is setup, its ok to call interface funtions
        //---------------------------------------------------------------------
        // 1: Login as the manufacturer.
        //---------------------------------------------------------------------
        if ( VLT_OK != (usActualSW = theBaseApi->VltSubmitPassword(VLT_USER7,
                VLT_MANUFACTURER,
                ucManufPasswordLength,
                (VLT_U8*)strManufPassword.c_str() ) ) )
			CloseAndExit(hMod,usActualSW,"Logging as manufacturer failed"); 

		//---------------------------------------------------------------------
        // 2: Transition to the creation  state.
        //---------------------------------------------------------------------
		theBaseApi->VltSetStatus(VLT_OPERATIONAL_ACTIVE);
		theBaseApi->VltSetStatus(VLT_CREATION );
		
        //---------------------------------------------------------------------
        // 3: Create an user with password authentication method.
        //---------------------------------------------------------------------
		VLT_U8 userPin[] = {0x30, 0x31,0x30, 0x31,0x30, 0x31};

        VLT_MANAGE_AUTH_DATA structAuthSetup;
        // Assign the manage authentication Operation IDs, more in "vaultic_consts.c" 
        structAuthSetup.u8OperationID = VLT_CREATE_USER;
        // Assign the first user ID.
        structAuthSetup.u8UserID = USER_PIN_ID;
        // Utilise SCP03 authentication method (uses SCP service).
		structAuthSetup.u8Method = VLT_LOGIN_PASSWORD;
        // Make the user an approved user.
		structAuthSetup.u8RoleID = VLT_NON_APPROVED_USER;
        // Make the minimum require secure channel level the highest level possible.
        structAuthSetup.u8ChannelLevel = VLT_NO_CHANNEL;
        // Enable delete all user files if the user locks their account.
        structAuthSetup.u8SecurityOption = 0x00;
        // Assign a small try count to perevent an attack (account locked after 3 failed authentication attempts).
        structAuthSetup.u8TryCount = 3;

		// Set Password value
		memcpy(&structAuthSetup.data.cleartext.u8Password,userPin,sizeof(userPin));
		structAuthSetup.data.cleartext.u8PasswordLength = (VLT_U8)sizeof(userPin);

		 if (VLT_OK != ( usActualSW = theBaseApi->VltManageAuthenticationData( 
            &structAuthSetup ) ) )
			CloseAndExit(hMod,usActualSW,"Create user failed"); 

		//---------------------------------------------------------------------
        // 4: Download wrapping RSA keypair.
        //---------------------------------------------------------------------
		VLT_U8 N[]  = {0x8B,0x61,0x6E,0xDE,0x87,0x0E,0x4D,0x96,0xB5,0xC8,0x72,0xE0,0x06,0xE6,0xDE,0x5B,0x75,0x2B,0x53,0x69,0xEF,0xAA,0x8E,0x60,0x8D,0xF7,0xC3,0x64,0xCA,0xA2,0xE0,0x99,0xA0,0x5F,0x39,0x05,0x91,0xAD,0x33,0x73,0x15,0xA5,0x02,0x06,0x8B,0xA3,0xF5,0xAC,0xC1,0x86,0xEC,0x71,0x12,0x04,0x34,0xD3,0x71,0xF5,0x77,0x95,0x73,0x5F,0xD1,0xB4,0xE5,0x21,0xC4,0xDF,0x88,0x46,0x51,0x10,0xA4,0x70,0xD4,0x6B,0xC8,0x48,0xDA,0xE6,0xF4,0x52,0x13,0x37,0x22,0x58,0xD2,0x5B,0xCD,0x3F,0x79,0x10,0x8F,0x5F,0x90,0x54,0x6A,0x78,0x74,0xCF,0x84,0x10,0x1D,0x5A,0xF7,0x6E,0x77,0xD4,0x98,0xC3,0x91,0x7F,0xB4,0x76,0xF4,0xB4,0xAC,0xBA,0xE6,0x04,0xCD,0x04,0xC9,0x07,0xDC,0x0C,0xB6,0xE5};
		VLT_U8 E[]  = {0x00,0x01,0x00,0x01};

		VLT_U8 P[]  = {0xCB,0x07,0xBC,0x78,0xC8,0x28,0xA2,0x9D,0x52,0x03,0x21,0x10,0xD3,0x7F,0x0B,0x02,0xEB,0xCD,0x37,0x8B,0x91,0x33,0xD0,0x02,0x5B,0xCE,0x0F,0xF7,0x1D,0x78,0xCB,0x9F,0xBD,0xB1,0xF4,0x81,0x86,0x9A,0x66,0xB2,0x27,0xF7,0xC3,0x75,0xAB,0xE9,0x27,0xC2,0xD3,0xA0,0x8C,0xA6,0x12,0x7E,0x91,0x78,0xA5,0x03,0x67,0x3E,0x16,0x0E,0xCA,0xD7};
		VLT_U8 Q[]  = {0xAF,0xBE,0x93,0xE3,0xC4,0x12,0x5F,0x0F,0x35,0x7B,0xE5,0x5D,0xAD,0x86,0x82,0x72,0x4F,0x3B,0xEF,0x74,0xEC,0x88,0x7E,0xE9,0x8B,0xFD,0xF4,0xCC,0x77,0x51,0xA9,0x48,0x03,0x89,0x01,0x68,0xA8,0x47,0x7C,0xB7,0x24,0xA6,0xA7,0xD6,0x44,0xC6,0x56,0x18,0xCC,0x49,0x0A,0x4C,0xCC,0xEC,0xA3,0x58,0x73,0x34,0xF7,0x17,0x8A,0x48,0xF0,0xA3};
		VLT_U8 Dp[] = {0xAC,0xF0,0x9A,0x0A,0x07,0xF5,0x15,0x0F,0x0C,0x9C,0xBD,0xBD,0xA7,0x69,0xBF,0xE7,0x6D,0x14,0x11,0x0C,0x41,0x52,0xF1,0x0C,0x11,0x47,0x25,0x32,0x49,0x58,0x6B,0x9A,0x55,0xEB,0x0F,0x6A,0x85,0x4D,0x5F,0xCC,0xC1,0x5E,0xBA,0x8F,0xB1,0x2F,0x99,0x3E,0xCB,0x25,0x26,0xA9,0x0F,0x77,0xE0,0x51,0xF3,0xBD,0x42,0x64,0x10,0x45,0xE9,0x61};
		VLT_U8 Dq[] = {0x83,0x9B,0x9E,0x38,0x74,0xFE,0xA4,0xFD,0x48,0xBA,0x2D,0x7B,0xC7,0x94,0xF1,0x5F,0x15,0x02,0xF7,0x41,0x82,0x58,0x7D,0x61,0xFC,0x1A,0x04,0x16,0x92,0x4C,0x2F,0x0E,0xE4,0xC6,0xD6,0xAD,0x3F,0x47,0xB9,0x97,0x94,0xF4,0xD0,0xBF,0x5E,0xE5,0x80,0x58,0x35,0xB3,0x84,0xE9,0x1A,0x95,0x23,0x85,0xFB,0x50,0x70,0xB1,0xCA,0x36,0xF1,0x2D};
		VLT_U8 IP[] = {0x31,0xF9,0x74,0xF9,0x7F,0x8F,0xBF,0xE8,0xD9,0xF5,0x50,0xE5,0x63,0x1B,0x48,0xE9,0x57,0x8D,0x99,0xCB,0xCC,0x44,0x9C,0x60,0x66,0xF5,0x62,0xF8,0xDE,0xF8,0xE8,0x19,0xBF,0xA2,0x11,0x0B,0x61,0x69,0x94,0x01,0x2B,0xB6,0x8E,0x1C,0x35,0x2E,0x2A,0x6F,0xEF,0xDE,0xA8,0xC6,0x77,0x61,0x0C,0x7F,0xC1,0xB1,0x9E,0xFC,0x7E,0x8B,0x5A,0xDA};

		VLT_U8 pubKeyGroup = 0x20;
		VLT_U8 pubKeyIndex = 0x01;

		VLT_KEY_OBJECT rsaPubKeyObj;
		rsaPubKeyObj.u8KeyID = VLT_KEY_RSAES_PUB;
		rsaPubKeyObj.data.RsaPubKey.u16NLen			= 0x80;
		rsaPubKeyObj.data.RsaPubKey.pu8N			= N;
		rsaPubKeyObj.data.RsaPubKey.u16ELen			= 0x04;
		rsaPubKeyObj.data.RsaPubKey.pu8E			= E;

		VLT_U8 privKeyGroup = 0x20;
		VLT_U8 privKeyIndex = 0x02;

		VLT_KEY_OBJECT rsaPrivKeyObj;
		rsaPrivKeyObj.u8KeyID = VLT_KEY_RSAES_PRIV_CRT;
		rsaPrivKeyObj.data.RsaPrivCrtKey.u8Mask				= 0x00;
		rsaPrivKeyObj.data.RsaPrivCrtKey.u16PLen			= 0x40;
		rsaPrivKeyObj.data.RsaPrivCrtKey.pu8P				= P;
		rsaPrivKeyObj.data.RsaPrivCrtKey.pu8Q				= Q;
		rsaPrivKeyObj.data.RsaPrivCrtKey.pu8Dp				= Dp;
		rsaPrivKeyObj.data.RsaPrivCrtKey.pu8Dq				= Dq;
		rsaPrivKeyObj.data.RsaPrivCrtKey.pu8Ip				= IP;
		rsaPrivKeyObj.data.RsaPrivCrtKey.u8PublicKeyGroup	= pubKeyGroup;
		rsaPrivKeyObj.data.RsaPrivCrtKey.u8PublicKeyIndex	= pubKeyIndex;

		VLT_FILE_PRIVILEGES pubAccess;
		pubAccess.u8Read		= 0xFF;
		pubAccess.u8Execute		= (1 << USER_PIN_ID);
		pubAccess.u8Write		= (1 << USER_PIN_ID);
		pubAccess.u8Delete		= (1 << USER_PIN_ID);

		VLT_FILE_PRIVILEGES privAccess;
		privAccess.u8Read		= (1 << USER_PIN_ID);
		privAccess.u8Execute	= (1 << USER_PIN_ID);
		privAccess.u8Write		= (1 << USER_PIN_ID);
		privAccess.u8Delete		= (1 << USER_PIN_ID);

		usActualSW = theBaseApi->VltPutKey(pubKeyGroup,pubKeyIndex,&pubAccess,&rsaPubKeyObj);
		
		if (VLT_OK != usActualSW)
			CloseAndExit(hMod,usActualSW,"Put RSA Public key failed"); 

		usActualSW = theBaseApi->VltPutKey(privKeyGroup,privKeyIndex,&privAccess,&rsaPrivKeyObj);
		
		if (VLT_OK != usActualSW)
			CloseAndExit(hMod,usActualSW,"Put RSA Public key failed"); 


		//---------------------------------------------------------------------
        // 5: Transition to the operational activated state.
        //---------------------------------------------------------------------
        if (VLT_OK != ( usActualSW = theBaseApi->VltSetStatus( 
			VLT_OPERATIONAL_ACTIVE ) ) )
			CloseAndExit(hMod,usActualSW,"Set status as operational active failed"); 

		//---------------------------------------------------------------------
        // 6: Log the manufacturer out of the VaultIC Secure Object.
        //---------------------------------------------------------------------
        if (VLT_OK != ( usActualSW = theBaseApi->VltCancelAuthentication(  ) ) )
			CloseAndExit(hMod,usActualSW,"Logout failed"); 

		//---------------------------------------------------------------------
        // 7: Log as user.  
        //---------------------------------------------------------------------
        if ( VLT_OK != (usActualSW = theBaseApi->VltSubmitPassword(VLT_USER0,
			VLT_NON_APPROVED_USER,
                sizeof(userPin),
                (VLT_U8*)userPin ) ) )
				CloseAndExit(hMod,usActualSW,"Logging as user failed");


		//---------------------------------------------------------------------
        // 8: Download wrapped key (using RSA_OAEP)
        //---------------------------------------------------------------------
		VLT_U8 key_AES256_Plaintext[] = {VLT_KEY_AES_256,0x00,0x00,0x20,0x97,0x54,0x93,0xD6,0x12,0x86,0x24,0x4A,0x00,0xC1,0x36,0xAA,0xFC,0x47,0x96,0x04,0x0D,0x9F,0x69,0x90,0xFE,0x89,0x4D,0xE9,0x89,0x55,0xC3,0x11,0xCE,0x07,0xD0,0xAD};
		VLT_U8 key_AES256_Encrypted[] = {0x27,0x32,0x3F,0x11,0x4C,0x95,0xAD,0x80,0xE7,0xAE,0xAD,0x56,0x29,0xE7,0x86,0x09,0xEA,0x7C,0x6B,0x1D,0xFB,0xDC,0xF9,0xD3,0x5F,0xE0,0x8C,0x62,0xEF,0xEA,0x63,0xE0,0x6D,0xFF,0x4B,0x8E,0x4C,0x97,0xC4,0xF3,0x1A,0x2C,0xB8,0x07,0xCA,0x9F,0xA8,0x85,0x7B,0x27,0x0F,0x37,0xFC,0x22,0x17,0x4A,0x05,0x8B,0xF2,0x21,0x23,0x7B,0x69,0x53,0x44,0x32,0xC0,0x6B,0xE2,0x11,0xC9,0xC6,0x17,0x55,0x7E,0xA0,0xB2,0x22,0x60,0xBC,0x3C,0x14,0x56,0x8B,0x0D,0x47,0xDA,0xE1,0x69,0x6C,0x65,0xF8,0xD6,0x7E,0xD5,0x2F,0x8A,0x9C,0xC8,0x54,0x9E,0xF2,0x63,0x42,0x83,0x5E,0x5C,0x35,0x7E,0xB2,0x9A,0xFD,0x1E,0x5D,0xA9,0xB0,0xBA,0xB7,0x2E,0x50,0x3F,0x7E,0xC2,0x1B,0x8F,0x9D,0x07,0xDE};
		VLT_U16 crc = 0x9A0E;
		VLT_U16 clearkeylen	= sizeof(key_AES256_Plaintext);
		VLT_U16 enckeylen	= sizeof(key_AES256_Encrypted);
		
		VLT_ALGO_PARAMS wrappingAlgoParams;
		wrappingAlgoParams.u8AlgoID = VLT_ALG_KTS_RSA_OAEP_BASIC;
		wrappingAlgoParams.data.RsaesOaep.u8DigestOaep	= VLT_ALG_DIG_SHA1;
		wrappingAlgoParams.data.RsaesOaep.u8DigestMgf1	= VLT_ALG_DIG_SHA1;
		wrappingAlgoParams.data.RsaesOaep.u16LLen		= 0;
		wrappingAlgoParams.data.RsaesOaep.pu8Label		= 0;

		usActualSW = theBaseApi->VltInitializeAlgorithm(privKeyGroup,privKeyIndex,VLT_UNWRAP_KEY_MODE,&wrappingAlgoParams);

		if (VLT_OK != usActualSW)
			CloseAndExit(hMod,usActualSW,"Init Algo for Key Unwrapping failed"); 		

		VLT_KEY_OBJECT encSecretKeyObj;
		encSecretKeyObj.u8KeyID								= VLT_KEY_RAW;
		encSecretKeyObj.data.RawKey.isEncryptedKey			= TRUE;
		encSecretKeyObj.data.RawKey.pu16ClearKeyObjectLen	= &clearkeylen;
		encSecretKeyObj.data.RawKey.pu16EncKeyObjectLen		= &enckeylen;
		encSecretKeyObj.data.RawKey.pu8KeyObject			= key_AES256_Encrypted;
		encSecretKeyObj.data.RawKey.u16Crc					= crc;

		VLT_U8 secretKeyGroup = 0x20;
		VLT_U8 secretKeyIndex = 0x03;

		VLT_FILE_PRIVILEGES secretAccess;
		secretAccess.u8Read		= (1 << USER_PIN_ID);
		secretAccess.u8Execute	= (1 << USER_PIN_ID);
		secretAccess.u8Write	= (1 << USER_PIN_ID);
		secretAccess.u8Delete	= (1 << USER_PIN_ID);

		usActualSW = theBaseApi->VltPutKey(secretKeyGroup,secretKeyIndex,&secretAccess,&encSecretKeyObj);

		if (VLT_OK != usActualSW)
			CloseAndExit(hMod,usActualSW,"Put wrapped key failed");


		//---------------------------------------------------------------------
        // 9: Try it
        //---------------------------------------------------------------------
        VLT_ALGO_PARAMS strctAlgoParms;
        strctAlgoParms.u8AlgoID = VLT_ALG_CIP_AES;
        // Setup the Algo Params Structure with padding method 2.
        strctAlgoParms.data.SymCipher.u8Padding = PADDING_ISO9797_METHOD2;
        // Setup the Algo Params Structure with block cipher block chaining mode.
        strctAlgoParms.data.SymCipher.u8Mode = BLOCK_MODE_CBC;
        // Assign the 61 byte AES initalisation vector - IV.
        unsigned char aucIv[] = {0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,
									0x9,0xA,0xB,0xC,0xD,0xE,0xF};
        strctAlgoParms.data.SymCipher.u8IvLength = sizeof(aucIv);
        // The Algo Params Structure contains a buffer to hold the IV.
        memcpy(strctAlgoParms.data.SymCipher.u8Iv, aucIv, 
            strctAlgoParms.data.SymCipher.u8IvLength );        

        if ( VLT_OK != ( usActualSW = theBaseApi->VltInitializeAlgorithm(
            secretKeyGroup, 
            secretKeyIndex,
            VLT_ENCRYPT_MODE,
            &strctAlgoParms) ) )
			CloseAndExit(hMod,usActualSW,"Initialize algo failed");

        //---------------------------------------------------------------------
        // 10: Encrypt a buffer of data using AES Encryption 
        // CBC mode and padding method 2.
        //---------------------------------------------------------------------
        unsigned char aucPlainText[510];
        memset( aucPlainText, 0x55, sizeof( aucPlainText ) );
        unsigned char aucCipherText[512]; // 2 extra bytes for padding.
        unsigned long ulCipherTextSize = sizeof( aucCipherText );

        if ( VLT_OK != ( usActualSW = theBaseApi->VltEncrypt(
	        sizeof( aucPlainText ), 
            aucPlainText, 
            &ulCipherTextSize, 
            aucCipherText ) ) )
			CloseAndExit(hMod,usActualSW,"Encryption failed");

        //
        // Print the Cipher text size
        stringstream sstr;
        sstr << "The Cipher text length should be 512 bytes long, the actual "
            "length is: " << (int)ulCipherTextSize;
        std::cout << sstr.str( ) << endl;
	    sstr.str("");

        //
        // Print out all of the cipher text values
        std::cout << "The cipher text is:" << endl;
        if (true != DisplayBuffer( ulCipherTextSize, aucCipherText ) )
        {
            std::cout << "Failed to log cipher text, check buffer is not NULL." 
                << endl;
        }



		//---------------------------------------------------------------------
        // 11: Read the AES secret key using Key Wrapping
        //---------------------------------------------------------------------
		usActualSW = theBaseApi->VltInitializeAlgorithm(pubKeyGroup,pubKeyIndex,VLT_WRAP_KEY_MODE,&wrappingAlgoParams);
		
		if (VLT_OK != usActualSW)
			CloseAndExit(hMod,usActualSW,"Init Algo for Key Wrapping failed"); 

		usActualSW = theBaseApi->VltReadKey(secretKeyGroup,secretKeyIndex,&encSecretKeyObj);

		if (VLT_OK != usActualSW)
			CloseAndExit(hMod,usActualSW,"Read wrapped key failed");

        //---------------------------------------------------------------------
        // 12: Log back in as the manufacturer.  
        //---------------------------------------------------------------------
        if ( VLT_OK != (usActualSW = theBaseApi->VltSubmitPassword(VLT_USER7,
                VLT_MANUFACTURER,
                ucManufPasswordLength,
                (VLT_U8*)strManufPassword.c_str() ) ) )
			CloseAndExit(hMod,usActualSW,"Log as manufacturer failed");

        //---------------------------------------------------------------------
        // 13: Transition to the creation state, wipes the file system.
        //---------------------------------------------------------------------
        if (VLT_OK != ( usActualSW = theBaseApi->VltSetStatus( 
            VLT_CREATION ) ) )
			CloseAndExit(hMod,usActualSW,"Set status as CREATION failed");

        //---------------------------------------------------------------------
        // 14: Log back in as the manufacturer, to delete user 0.  
        //---------------------------------------------------------------------
        if ( VLT_OK != (usActualSW = theBaseApi->VltSubmitPassword(VLT_USER7,
                VLT_MANUFACTURER,
                ucManufPasswordLength,
                (VLT_U8*)strManufPassword.c_str() ) ) )
			CloseAndExit(hMod,usActualSW,"Log as manufacturer failed");

        //---------------------------------------------------------------------
        // 15: Delete user 0, which should leave the VaultIC in a virgin state.
        //---------------------------------------------------------------------
        structAuthSetup.u8OperationID = VLT_DELETE_USER;
        // Assign the first user ID.
        structAuthSetup.u8UserID = VLT_USER0;
                
        if (VLT_OK != ( usActualSW = theBaseApi->VltManageAuthenticationData( 
            &structAuthSetup ) ) )
			CloseAndExit(hMod,usActualSW,"Delete user failed");

        //---------------------------------------------------------------------
        // 16: Log the manufacturer out of the VaultIC Secure Object.
        //---------------------------------------------------------------------
        if (VLT_OK != ( usActualSW = theBaseApi->VltCancelAuthentication(  ) ) )
			CloseAndExit(hMod,usActualSW,"Logout failed");

        cout << "Example code complete, VaultIC file system wiped and users "
            "deleted (not manufacturer)." << endl;                  
    }
	catch(...)
	{
        cout << "Unknown Exception caught." << endl;              
    }

	CloseAndExit(hMod,0,"Sample execution successfull !!!");
}


bool DisplayBuffer(const unsigned long& ulCipherTextSize,
                   const unsigned char* pucBuffer)
{
    const unsigned char ucLineLength = 8;

    if ( 0 == pucBuffer )
    {
        return false;
    }
    
    unsigned long ulRoundedNumberOfChars = ( ulCipherTextSize + 
        ( ulCipherTextSize % ucLineLength ) );

    for ( unsigned int i = 0; i < ulRoundedNumberOfChars; i++ )
    {
        for ( unsigned int j = 0; j < ucLineLength; j++, i++ )
        {
            if ( i >= ulCipherTextSize )
            {
                break;
            }
            // print each character
            cout << "0x" << hex << setw(2) << setfill('0') 
                << static_cast<int>( pucBuffer[i] ) << " ";
        }
        // new line
        cout << endl;
    }
    
    return true;
}