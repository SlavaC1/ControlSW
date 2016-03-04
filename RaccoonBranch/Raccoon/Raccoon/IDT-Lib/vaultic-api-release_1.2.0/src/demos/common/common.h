#include "vaultic_common.h"
#include "vaultic_lib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <sstream>

#if( VLT_PLATFORM == VLT_WINDOWS )
    #include <tchar.h>
#endif

#ifdef USE_STATIC_API
    #include "vaultic_ApiLib.h"
#else
#if( VLT_PLATFORM == VLT_LINUX ) || ( VLT_PLATFORM == VLT_MAC_OS )
    #include <dlfcn.h>
#endif
#endif

#if(VLT_ENABLE_AARDVARK == VLT_ENABLE )
#include "aardvark.h"
#endif

#define FREE(a)		if (a) \
					{ \
						free(a); \
						a = NULL; \
					}
/*******************************************************
 Error Codes:
*******************************************************/
#define VLT_GETPROC_INIT_LIB_FAILED			-1
#define VLT_GETPROC_ClOSE_LIB_FAILED		-2
#define VLT_GETPROC_GET_API_FAILED			-3
#define VLT_INIT_LIB_FAILED					-5
#define VLT_GET_API_FAILED					-6
#define VLT_GETPROC_GET_AUTH_FAILED			-7 
#define VLT_GETPROC_GET_LIBINFO_FAILED		-8
#define VLT_GETPROC_GET_FINDDEVS_FAILED		-9
#define VLT_FIND_DEVS_MALLOC_FAIL			-10
#define VLT_NO_PCSC_XML_TAG					-11
#define VLT_NO_CHEETAHS_CONNECTED			-12
#define VLT_MALFORMED_XML_PCSC_INTFC		-13 
#define VLT_MALFORMED_XML_PCSC_PERIPH		-14
#define VLT_MALFORMED_XML_CHEETH_INTFC		-15
#define VLT_MALFORMED_XML_CHEETH_PERIPH		-16 
#define VLT_MALFORMED_XML_AARDVARK_INTFC	-17
#define VLT_MALFORMED_XML_AARDVARK_PERIPH	-18 
#define VLT_NO_AARDVARK_CONNECTED			-19
#define VLT_NO_READER_CONNECTED				-20
#define VLT_GETPROC_CRC16_FAILED			-21
#define VLT_GETPROC_GET_KEYWRAPPING_FAILED  -22

#define VLT_GET_AUTH_FAILED             -30

#ifdef USE_STATIC_API
#define CloseAndExit(hmod,status,exitMessage)		{printf("%s\n",exitMessage); \
	VltCloseLibrary(); \
	system("pause"); \
	return( status ); }					
#else
#define CloseAndExit(hmod,status,exitMessage) {printf("%s (0x%.4X)\n",exitMessage,status); \
												CloseLibrary( hMod ); \
														system("pause"); \
														return( status ); }	
#endif

#ifndef USE_STATIC_API
#if( VLT_PLATFORM == VLT_WINDOWS )
    #define GetLibSymbol(mod, procName) GetProcAddress(mod, procName)
    #define OpenLibrary(libPath) LoadLibrary(libPath)
    #define CloseLibrary(mod) FreeLibrary(mod)
#else
    #define GetLibSymbol(mod, procName) dlsym(mod, procName)
    #define OpenLibrary(libPath) dlopen(libPath, RTLD_LAZY)
    #define CloseLibrary(mod) dlclose(mod)
#endif

#if( VLT_PLATFORM == VLT_WINDOWS )
        #define LIB_PATH_4XX    ".\\VaultIC_API_4XX.dll"
		#define LIB_PATH_1XX    ".\\VaultIC_API_1XX.dll"
#endif

#if( VLT_PLATFORM == VLT_LINUX )
        #define LIB_PATH_4XX    "./libVaultIC_API_4XX.so"
        #define LIB_PATH_1XX    "./libVaultIC_API_1XX.so"
#endif //( VLT_PLATFORM == VLT_LINUX )

#if( VLT_PLATFORM == VLT_MAC_OS )
        #define LIB_PATH_4XX    "./libVaultIC_API_4XX.dylib"
        #define LIB_PATH_1XX    "./libVaultIC_API_1XX.dylib"
#endif //( VLT_PLATFORM == VLT_MAC_OS )
#endif

enum enInterfaceTypes
{
	INTERFACE_PCSC = 10,
	INTERFACE_AARDVARK_SPI = 20,
	INTERFACE_AARDVARK_TWI = 30,
	INTERFACE_CHEETAH_SPI = 40,
};


