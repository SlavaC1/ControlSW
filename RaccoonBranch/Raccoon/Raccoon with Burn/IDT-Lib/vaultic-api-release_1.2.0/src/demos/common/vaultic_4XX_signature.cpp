// SampleCode.cpp : Defines the entry point for the console application.
//

#include "common.h"

#define PREDIGESTED							(VLT_U8)0x80
#define	INITIAL_USER_PIN					(const VLT_U8*)"12345678"
#define INITIAL_USER_PIN_LEN				(VLT_U8)8

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
	VAULTIC_AUTH* theAuthSvr = 0;
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
	unsigned char ucManufPasswordLength = (unsigned char)strManufPassword.size();

	//
	// Check the supplied password is a valid length.
	//
	if ( ( ucManufPasswordLength < ucMinPasswordLength ) ||
		( ucManufPasswordLength > ucMaxPasswordLength ) )
	{
		std::cout << "Please ensure the manufacturer's password is between "
			"4 characters and 32 characters:" << endl;

		return( 1 );
	}

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
			CloseAndExit(hMod,status,"Get Library info failed"); 
		}
		else
		{
			std::cout << "==========================================================" << endl;
			std::cout << "                  Library Information                     " << endl;
			std::cout << "==========================================================" << endl;
			std::cout << "Library version : " << libInfo.pVersion << endl;

			std::cout << "    Put/Read Secret Key Support Enabled : " << boolalpha << 
				SUPPORTED( libInfo.capabilities, VLT_CPB_ENABLE_KEY_SECRET ) << endl;
			std::cout << "    Put/Read HOTP Key Support Enabled: " << boolalpha << 
				SUPPORTED( libInfo.capabilities, VLT_CPB_ENABLE_KEY_HOTP ) << endl;
			std::cout << "    Put/Read TOTP Key Support Enabled: " << boolalpha << 
				SUPPORTED( libInfo.capabilities, VLT_CPB_ENABLE_KEY_TOTP ) << endl;
			std::cout << "    Put/Read RSA Key Support Enabled : " << boolalpha << 
				SUPPORTED( libInfo.capabilities, VLT_CPB_ENABLE_KEY_RSA ) << endl;
			std::cout << "    Put/Read DSA Key Support Enabled : " << boolalpha << 
				SUPPORTED( libInfo.capabilities,  VLT_CPB_ENABLE_KEY_DSA ) << endl;
			std::cout << "    Put/Read ECDSA Key Support Enabled : " << boolalpha << 
				SUPPORTED( libInfo.capabilities,  VLT_CPB_ENABLE_KEY_ECDSA ) << endl;
			std::cout << "    Cipher Self Tests Support Enabled : " << boolalpha << 
				SUPPORTED( libInfo.capabilities, VLT_CPB_ENABLE_CIPHER_TESTS ) << endl;
			std::cout << "    Identity Authentication Service Support Enabled: " << boolalpha <<
				SUPPORTED( libInfo.capabilities,  VLT_CPB_ENABLE_IDENTITY_AUTH )<< endl; 
			std::cout << "    SCP02 Secure Channel Support Enabled: " << boolalpha << 
				SUPPORTED( libInfo.capabilities,  VLT_CPB_ENABLE_SCP02 ) << endl;
			std::cout << "    SCP03 Secure Channel Support Enabled: " << boolalpha << 
				SUPPORTED( libInfo.capabilities,  VLT_CPB_ENABLE_SCP03 ) << endl;
			std::cout << "    Microsoft Minicard Driver Support Enabled: " << boolalpha << 
				SUPPORTED( libInfo.capabilities,  VLT_CPB_ENABLE_MS_AUTH ) << endl;
			std::cout << "    Block Protocol Support Enabled: " << boolalpha << 
				SUPPORTED( libInfo.capabilities,  VLT_CPB_ENABLE_BLOCK_PROTOCOL ) << endl;
			std::cout << "    TWI Interface Support Enabled: " << boolalpha <<             
				SUPPORTED( libInfo.capabilities,  VLT_CPB_ENABLE_TWI ) << endl;
			std::cout << "    SPI Interface Support Enabled: " << boolalpha <<
				SUPPORTED( libInfo.capabilities,  VLT_CPB_ENABLE_SPI ) << endl;
			std::cout << "    ISO7816 Protocol Support Enabled: " << boolalpha <<             
				SUPPORTED( libInfo.capabilities,  VLT_CPB_ENABLE_ISO7816 ) << endl;           
			std::cout << "    AES(128, 192, 256) Support Enabled: " << boolalpha <<
				SUPPORTED( libInfo.capabilities,  VLT_CPB_ENABLE_CIPHER_AES ) << endl;
			std::cout << "    TDES(EEE& EDE modes) Support Enabled: " << boolalpha <<
				SUPPORTED( libInfo.capabilities,  VLT_CPB_ENABLE_CIPHER_TDES ) << endl;
			std::cout << "    DES Support Enabled: " << boolalpha <<
				SUPPORTED( libInfo.capabilities,  VLT_CPB_ENABLE_CIPHER_DES ) << endl;
			std::cout << "    Fast CRC16 CCIT Support Enabled: " << boolalpha <<
				SUPPORTED( libInfo.capabilities,  VLT_CPB_ENABLE_FAST_CRC16CCIT ) << endl;
			std::cout << "    Key Wrapping Service Support Enabled: " << boolalpha <<
				SUPPORTED( libInfo.capabilities,  VLT_CPB_ENABLE_KEY_WRAPPING ) << endl;
			std::cout << "    Put/Read Host/Device ID Key Support Enabled: " << boolalpha <<
				SUPPORTED( libInfo.capabilities,  VLT_ENABLE_KEY_HOST_DEVICE_ID ) << endl;
			std::cout << "    File System Service Support Enabled: " << boolalpha <<
				SUPPORTED( libInfo.capabilities,  VLT_ENABLE_FILE_SYSTEM ) << endl;
			std::cout << "    Aardvark SPI Write Error suppresion support Enabled: " << boolalpha <<
				SUPPORTED( libInfo.capabilities,  VLT_ENABLE_AARDVK_SPPRSS_ERR ) << endl;


			if( libInfo.fwCompatibilityVersionId == 1 )
			{
				std::cout << "This Library is compiled for VaultIC version: v1.0.2" << endl;
			}
			if( libInfo.fwCompatibilityVersionId == 2 )
			{
				std::cout << "This Library is compiled for VaultIC version: v1.1.x" << endl;
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
		//
		if( VLT_OK != VltInitLibrary( &commsParams ) )
			CloseAndExit(hMod,VLT_INIT_LIB_FAILED,"InitLibrary failed");

		//
		// Get the Api Interface.
		//
		if ( 0 == ( theBaseApi = VltGetApi( ) ) )
			CloseAndExit(hMod,VLT_GET_API_FAILED,"Get API failed");

		//
		// Get the Scp Interface.
		//
		if ( 0 == ( theAuthSvr = VltGetAuth( ) ) )
			CloseAndExit(hMod,VLT_GET_AUTH_FAILED,"Get Auth failed");


		//---------------------------------------------------------------------
		// The Api is setup, its ok to call interface funtions
		//---------------------------------------------------------------------
		// 1: Login as the manufacturer.
		//---------------------------------------------------------------------
		if ( VLT_OK != (usActualSW = theBaseApi->VltSubmitPassword(VLT_USER7,
			VLT_MANUFACTURER,
			ucManufPasswordLength,
			(VLT_U8*)strManufPassword.c_str() ) ) )
			CloseAndExit(hMod,usActualSW,"Submit manufacturer password failed");

		theBaseApi->VltSetStatus( 
			VLT_OPERATIONAL_ACTIVE );

		if (VLT_OK != ( usActualSW = theBaseApi->VltSetStatus( 
			VLT_CREATION ) ) )
			CloseAndExit(hMod,usActualSW,"Set status (CREATION) failed");

		//---------------------------------------------------------------------
		// 2: Delete User 0 if exist 
		//---------------------------------------------------------------------
		VLT_MANAGE_AUTH_DATA deluserSetup;
		// Assign the manage authentication Operation IDs, more in "vaultic_consts.c" 
		deluserSetup.u8OperationID = VLT_DELETE_USER;
		// Assign the first user ID.
		deluserSetup.u8UserID = VLT_USER0;
		theBaseApi->VltManageAuthenticationData( 
			&deluserSetup );

		//---------------------------------------------------------------------
		// 3: Create a non approved user 
		//---------------------------------------------------------------------
		VLT_MANAGE_AUTH_DATA structAuthSetup;
		// Assign the manage authentication Operation IDs, more in "vaultic_consts.c" 
		structAuthSetup.u8OperationID = VLT_CREATE_USER;
		// Assign the first user ID.
		structAuthSetup.u8UserID = VLT_USER0;
		// Utilise SCP03 authentication method (uses SCP service).
		structAuthSetup.u8Method = VLT_LOGIN_PASSWORD;
		// Make the user an approved user.
		structAuthSetup.u8RoleID = VLT_NON_APPROVED_USER;
		// Make the minimum require secure channel level the highest level possible.
		structAuthSetup.u8ChannelLevel = 0x00;
		// Enable delete all user files if the user locks their account.
		structAuthSetup.u8SecurityOption = 0x00;
		// Assign a small try count to perevent an attack (account locked after 3 failed authentication attempts).
		structAuthSetup.u8TryCount = 3;
		// Assign the static S-Mac and S-Enc keys.
		structAuthSetup.data.cleartext.u8PasswordLength = INITIAL_USER_PIN_LEN;
		// Set Password value
		memset(structAuthSetup.data.cleartext.u8Password,0x00,sizeof(structAuthSetup.data.cleartext.u8Password));
		memcpy(structAuthSetup.data.cleartext.u8Password,(VLT_PU8)INITIAL_USER_PIN,INITIAL_USER_PIN_LEN);


		if (VLT_OK != ( usActualSW = theBaseApi->VltManageAuthenticationData( 
			&structAuthSetup ) ) )
			CloseAndExit(hMod,usActualSW,"User creation failed");

		//---------------------------------------------------------------------
		// 5: Put B-163 domain param
		//---------------------------------------------------------------------

		VLT_FILE_PRIVILEGES structKeyPrivilages;

		//
		// The key shall only be used by User 0.
		structKeyPrivilages.u8Read = 0x01;
		structKeyPrivilages.u8Write = 0x01;
		structKeyPrivilages.u8Delete = 0x01;
		structKeyPrivilages.u8Execute = 0x01;


		VLT_U8 q[] = { 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC9};
		VLT_U8 x[] = { 0x00, 0x00, 0x00, 0x03, 0xF0, 0xEB, 0xA1, 0x62, 0x86, 0xA2, 0xD5, 0x7E, 0xA0, 0x99, 0x11, 0x68, 0xD4, 0x99, 0x46, 0x37, 0xE8, 0x34, 0x3E, 0x36};
		VLT_U8 y[] = { 0x00, 0x00, 0x00, 0x00, 0xD5, 0x1F, 0xBC, 0x6C, 0x71, 0xA0, 0x09, 0x4F, 0xA2, 0xCD, 0xD5, 0x45, 0xB1, 0x1C, 0x5C, 0x0C, 0x79, 0x73, 0x24, 0xF1};
		VLT_U8 z[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
		VLT_U8 a[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
		VLT_U8 b[] = { 0x00, 0x00, 0x00, 0x02, 0x0A, 0x60, 0x19, 0x07, 0xB8, 0xC9, 0x53, 0xCA, 0x14, 0x81, 0xEB, 0x10, 0x51, 0x2F, 0x78, 0x74, 0x4A, 0x32, 0x05, 0xFD};
		VLT_U8 N[] = { 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x92, 0xFE, 0x77, 0xE7, 0x0C, 0x12, 0xA4, 0x23, 0x4C, 0x33};
		//VLT_U8 H[] = 00000002

		VLT_KEY_OBJECT b163domainParams;
		b163domainParams.u8KeyID = VLT_KEY_ECDSA_DOMAIN_PARMS;
		b163domainParams.data.EcdsaParamsKey.u16NLen = sizeof(N);
		b163domainParams.data.EcdsaParamsKey.u16QLen = sizeof(q);
		b163domainParams.data.EcdsaParamsKey.pu8A = a;
		b163domainParams.data.EcdsaParamsKey.pu8B = b;
		b163domainParams.data.EcdsaParamsKey.pu8Gx = x;
		b163domainParams.data.EcdsaParamsKey.pu8Gy = y;
		b163domainParams.data.EcdsaParamsKey.pu8Gz = z;
		b163domainParams.data.EcdsaParamsKey.pu8N = N;
		b163domainParams.data.EcdsaParamsKey.pu8Q = q;
		b163domainParams.data.EcdsaParamsKey.u32H = 2;

		if ( VLT_OK != ( usActualSW = theBaseApi->VltPutKey(0xE0,0x01,&structKeyPrivilages,&b163domainParams)))
			CloseAndExit(hMod,usActualSW,"Put domain params failed");

		//---------------------------------------------------------------------
		// 6: Put ECDSA B-163 keys
		//---------------------------------------------------------------------
		VLT_U8 pubx[] = { 0x00, 0x00, 0x00, 0x02, 0x16, 0x8A, 0xA5, 0xB0, 0x4F, 0xCA, 0x58, 0xE3, 0x1D, 0x98, 0x6A, 0x7B, 0xA8, 0x76, 0xE1, 0xB8, 0x5B, 0x79, 0x0C, 0x63};
		VLT_U8 puby[] = { 0x00, 0x00, 0x00, 0x07, 0x5D, 0x08, 0x59, 0x47, 0x1D, 0x32, 0x0C, 0xE3, 0x4A, 0xA1, 0xF6, 0x01, 0x4D, 0x20, 0xCA, 0x91, 0x67, 0xE1, 0x09, 0x04};
		VLT_U8 priv[] = { 0x00, 0x00, 0x00, 0x00, 0x66, 0xA0, 0x1D, 0x79, 0xDB, 0x96, 0x89, 0xAF, 0x42, 0x69, 0x9F, 0xFD, 0x80, 0x25, 0x40, 0x09, 0xF2, 0x59, 0x72, 0xD9};

		VLT_KEY_OBJECT b163pub;
		b163pub.u8KeyID = VLT_KEY_ECC_PUB;
		b163pub.data.EcdsaPubKey.pu8Qx = pubx;
		b163pub.data.EcdsaPubKey.pu8Qy = puby;
		b163pub.data.EcdsaPubKey.u16QLen = sizeof(pubx);
		b163pub.data.EcdsaPubKey.u8DomainParamsGroup = 0xE0;
		b163pub.data.EcdsaPubKey.u8DomainParamsGroup = 0x01;

		if ( VLT_OK != ( usActualSW = theBaseApi->VltPutKey(0xD0,0x01,&structKeyPrivilages,&b163pub)))
			CloseAndExit(hMod,usActualSW,"Put public key failed");

		VLT_KEY_OBJECT priveccKey;
		priveccKey.u8KeyID = VLT_KEY_ECC_PRIV;
		priveccKey.data.EcdsaPrivKey.u16DLen = sizeof(priv);
		priveccKey.data.EcdsaPrivKey.pu8D = priv;
		priveccKey.data.EcdsaPrivKey.u8DomainParamsGroup = 0xE0;
		priveccKey.data.EcdsaPrivKey.u8DomainParamsIndex = 0x01;
		priveccKey.data.EcdsaPrivKey.u8PublicKeyGroup = 0xD0;
		priveccKey.data.EcdsaPrivKey.u8PublicKeyIndex = 0x01;
		priveccKey.data.EcdsaPrivKey.u8Mask = 0x00;

		if ( VLT_OK != ( usActualSW = theBaseApi->VltPutKey(0xC0,0x01,&structKeyPrivilages,&priveccKey)))
			CloseAndExit(hMod,usActualSW,"Put domain params failed");

		//---------------------------------------------------------------------
		// 7: Transition to the operational active state.
		//---------------------------------------------------------------------
		if (VLT_OK != ( usActualSW = theBaseApi->VltSetStatus( 
			VLT_OPERATIONAL_ACTIVE ) ) )
			CloseAndExit(hMod,usActualSW,"Set status failed");

		//---------------------------------------------------------------------
		// 8: Log the manufacturer out of the VaultIC Secure Object.
		//---------------------------------------------------------------------
		if (VLT_OK != ( usActualSW = theBaseApi->VltCancelAuthentication(  ) ) )
			CloseAndExit(hMod,usActualSW,"Cancel authentication failed");


		//---------------------------------------------------------------------
		// 9: Login as the new user 0, using password 
		//---------------------------------------------------------------------
		std::string pwd((const char*)INITIAL_USER_PIN);

		if ( VLT_OK != (usActualSW = theBaseApi->VltSubmitPassword(VLT_USER0,
			VLT_NON_APPROVED_USER,
			INITIAL_USER_PIN_LEN,
			(VLT_U8*)pwd.c_str() ) ) )
			CloseAndExit(hMod,usActualSW,"Submit user password failed");

		//---------------------------------------------------------------------
		// 10: Init the vaultIC Firmware to perform ECDSA signature.
		//---------------------------------------------------------------------
		VLT_ALGO_PARAMS strctAlgoParms;
		strctAlgoParms.u8AlgoID = VLT_ALG_SIG_ECDSA_GF2M;

		strctAlgoParms.data.EcdsaDsa.u8Digest = PREDIGESTED | VLT_ALG_DIG_SHA256;

		if ( VLT_OK != ( usActualSW = theBaseApi->VltInitializeAlgorithm(
			0xc0, 
			0x01,
			VLT_SIGN_MODE,
			&strctAlgoParms) ) )
			CloseAndExit(hMod,usActualSW,"Initialize algo failed");

		//---------------------------------------------------------------------
		// 11: Perform the signature
		//---------------------------------------------------------------------	
		unsigned char dataToSign[] = {0xF0, 0x39, 0x3F, 0xEB, 0xE8, 0xBA, 0xAA, 0x55, 0xE3, 0x2F, 0x7B, 0xE2,
			0xA7, 0xCC, 0x18, 0x0B, 0xF3, 0x4E, 0x52, 0x13, 0x7D, 0x99, 0xE0, 0x56, 
			0xC8, 0x17, 0xA9, 0xC0, 0x7B, 0x8F, 0x23, 0x9A};


		VLT_U16 ulsignatureLen = 0x100;
		VLT_PU8 pSignature = (VLT_PU8)malloc(ulsignatureLen);

		if ( VLT_OK != ( usActualSW = theBaseApi->VltGenerateSignature(sizeof(dataToSign),dataToSign,&ulsignatureLen,pSignature )))
			CloseAndExit(hMod,usActualSW,"Encryption failed");

		//
		// Print out all of the cipher text values
		std::cout << "The signature is:" << endl;
		if (true != DisplayBuffer( ulsignatureLen, pSignature ) )
		{
			std::cout << "Failed to log signature text, check buffer is not NULL." 
				<< endl;
		}


		//---------------------------------------------------------------------
		// 12: Log the manufacturer out of the VaultIC Secure Object.
		//---------------------------------------------------------------------
		if (VLT_OK != ( usActualSW = theBaseApi->VltCancelAuthentication(  ) ) )
			CloseAndExit(hMod,usActualSW,"De-authentication failed");


		//---------------------------------------------------------------------
		// 13: Log back in as the manufacturer.  
		//---------------------------------------------------------------------
		if ( VLT_OK != (usActualSW = theBaseApi->VltSubmitPassword(VLT_USER7,
			VLT_MANUFACTURER,
			ucManufPasswordLength,
			(VLT_U8*)strManufPassword.c_str() ) ) )
			CloseAndExit(hMod,usActualSW,"Manufacturer authentication failed");



		//---------------------------------------------------------------------
		// 14: Transition to the creation state, wipes the file system.
		//---------------------------------------------------------------------
		if (VLT_OK != ( usActualSW = theBaseApi->VltSetStatus( 
			VLT_CREATION ) ) )
			CloseAndExit(hMod,usActualSW,"Set status (CREATION) failed");


		//---------------------------------------------------------------------
		// 15: Log back in as the manufacturer, to delete user 0.  
		//---------------------------------------------------------------------
		if ( VLT_OK != (usActualSW = theBaseApi->VltSubmitPassword(VLT_USER7,
			VLT_MANUFACTURER,
			ucManufPasswordLength,
			(VLT_U8*)strManufPassword.c_str() ) ) )
			CloseAndExit(hMod,usActualSW,"Manufacturer authentication failed");


		//---------------------------------------------------------------------
		// 16: Delete user 0, which should leave the VaultIC in a virgin state.
		//---------------------------------------------------------------------
		structAuthSetup.u8OperationID = VLT_DELETE_USER;
		// Assign the first user ID.
		structAuthSetup.u8UserID = VLT_USER0;

		if (VLT_OK != ( usActualSW = theBaseApi->VltManageAuthenticationData( 
			&structAuthSetup ) ) )
			CloseAndExit(hMod,usActualSW,"Delete User 0 failed");


		//---------------------------------------------------------------------
		// 17: Log the manufacturer out of the VaultIC Secure Object.
		//---------------------------------------------------------------------
		if (VLT_OK != ( usActualSW = theBaseApi->VltCancelAuthentication(  ) ) )
			CloseAndExit(hMod,usActualSW,"De-authentication failed");

		std::cout << "Example code complete, VaultIC file system wiped and users "
			"deleted (not manufacturer)." << endl;                  
	}
	catch(...)
	{
		std::cout << "Unknown Exception caught." << endl;              
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
			std::cout << "0x" << hex << setw(2) << setfill('0') 
				<< static_cast<int>( pucBuffer[i] ) << " ";
		}
		// new line
		std::cout << endl;
	}

	return true;
}