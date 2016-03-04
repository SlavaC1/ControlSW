/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * --------------------------------------------------------------------------*/

#include "vaultic_config.h"
#include "vaultic_common.h"
#include "vaultic_api.h"
#include "vaultic_lib.h"
#include "vaultic_identity_authentication.h"
#include "vaultic_key_wrapping.h"
#include "vaultic_file_system.h"
#include "vaultic_crc16.h"
#include <stdio.h>

#if( VLT_PLATFORM == VLT_WINDOWS )
    #pragma warning(disable : 4996)
#endif


static VAULTIC_API Api;
static VAULTIC_AUTH Auth;
static VAULTIC_KEY_WRAPPING KeyWrapping;
static VAULTIC_FILE_SYSTEM FileSystem;
static VAULTIC_CRC16 Crc16;
/*
* Defines
*/
#define XML_LB_HEADER_SZ      (VLT_U8)52
#define XML_LB_FOOTER_SZ      (VLT_U8)10


/*
 * External references to the reader detection methods that are called by the 
 * VltFindDevices method
 */
#if(VLT_ENABLE_ISO7816 == VLT_ENABLE )
extern VLT_STS VltIso7816PtclDetectReaders( VLT_PU32 pSize, VLT_PU8 pXmlReaderString );
#endif
#if(VLT_ENABLE_AARDVARK == VLT_ENABLE )
extern VLT_STS VltAardvarkDetectReaders( VLT_PU32 pSize, VLT_PU8 pXmlReaderString );
#endif
#if(VLT_ENABLE_CHEETAH == VLT_ENABLE )
extern VLT_STS VltCheetahDetectReaders( VLT_PU32 pSize, VLT_PU8 pXmlReaderString );
#endif


_API_ VLT_STS VltGetLibraryInfo( VLT_LIBRARY_INFO* pLibraryInfo )
{
    return( VltGetLibInfo( pLibraryInfo ) );
}

_API_ VAULTIC_API* VltGetApi( void )
{
    if( ( 0 != Api.VltBeginTransaction ) &&
        ( 0 != Api.VltCancelAuthentication ) &&
        ( 0 != Api.VltComputeMessageDigest ) &&
        ( 0 != Api.VltCreateFile ) &&
        ( 0 != Api.VltCreateFolder ) &&
        ( 0 != Api.VltDecrypt ) &&
        ( 0 != Api.VltDeleteFile ) &&
        ( 0 != Api.VltDeleteFolder ) &&
        ( 0 != Api.VltDeleteKey ) &&
        ( 0 != Api.VltEncrypt ) &&
        ( 0 != Api.VltEndTransaction ) &&
        ( 0 != Api.VltExternalAuthenticate ) &&
        ( 0 != Api.VltGenerateAssuranceMessage ) &&
        ( 0 != Api.VltGenerateKeyPair ) &&
        ( 0 != Api.VltGenerateRandom ) &&
        ( 0 != Api.VltGenerateSignature ) &&
#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_2_1 )
		( 0 != Api.VltDeriveKey ) &&
		( 0 != Api.VltConstructDHAgreement ) &&
#endif
        ( 0 != Api.VltGenericExternalAuthenticate ) &&
        ( 0 != Api.VltGenericInternalAuthenticate ) &&
        ( 0 != Api.VltGetAuthenticationInfo ) &&
        ( 0 != Api.VltGetChallenge ) &&
        ( 0 != Api.VltGetInfo ) &&
        ( 0 != Api.VltInitializeAlgorithm ) &&
        ( 0 != Api.VltInitializeUpdate ) &&
        ( 0 != Api.VltListFiles ) &&
        ( 0 != Api.VltManageAuthenticationData ) &&
        ( 0 != Api.VltPutKey ) &&
        ( 0 != Api.VltReadFile ) &&
        ( 0 != Api.VltReadGpio ) &&
        ( 0 != Api.VltReadKey ) &&
        ( 0 != Api.VltSeekFile ) &&
        ( 0 != Api.VltSelectFileOrDirectory ) &&
        ( 0 != Api.VltSelfTest ) &&
        ( 0 != Api.VltSetAttributes ) &&
        ( 0 != Api.VltSetConfig ) &&
        ( 0 != Api.VltSetGpioDirection ) &&
        ( 0 != Api.VltSetPrivileges ) &&
        ( 0 != Api.VltSetStatus ) &&
        ( 0 != Api.VltSubmitPassword ) &&
        ( 0 != Api.VltTestCase1 ) &&
        ( 0 != Api.VltTestCase2 ) &&
        ( 0 != Api.VltTestCase3 ) &&
        ( 0 != Api.VltTestCase4 ) &&
        ( 0 != Api.VltVerifySignature ) &&
        ( 0 != Api.VltWriteFile ) &&
        ( 0 != Api.VltWriteGpio ) )
    {
        return( &Api );
    }

    return( NULL );
}

_API_ VAULTIC_AUTH* VltGetAuth( void )
{
    if( ( 0 != Auth.VltAuthClose ) &&
        ( 0 != Auth.VltAuthGetState ) &&
        ( 0 != Auth.VltAuthInit ) )
    {
        return( &Auth );
    }

    return( NULL );
}

_API_ VAULTIC_KEY_WRAPPING* VltGetKeyWrapping( void )
{
    if( ( 0 != KeyWrapping.VltKeyWrappingClose )  &&
        ( 0 != KeyWrapping.VltKeyWrappingInit )  &&
        ( 0 != KeyWrapping.VltUnwrapKey )  &&
        ( 0 != KeyWrapping.VltWrapKey ) )
    {
        return( &KeyWrapping );
    }

    return( NULL );
}

_API_ VAULTIC_FILE_SYSTEM* VltGetFileSystem( void )
{
    if( ( 0 != FileSystem.VltFsCloseFile ) && 
        ( 0 != FileSystem.VltFsCreate ) &&
        ( 0 != FileSystem.VltFsDelete ) &&
        ( 0 != FileSystem.VltFsListFiles ) &&
        ( 0 != FileSystem.VltFsOpenFile ) &&
        ( 0 != FileSystem.VltFsReadFile ) &&
        ( 0 != FileSystem.VltFsSetAttributes ) &&
        ( 0 != FileSystem.VltFsSetPrivileges ) &&
        ( 0 != FileSystem.VltFsWriteFile ) )
    {
        return( &FileSystem );
    }
         
    return( NULL );
}

_API_ VAULTIC_CRC16* VltGetCrc16( void )
{
    if( 0 != Crc16.VltCrc16 )
    {
        return( &Crc16 );
    }
   
    return( NULL );   
}

_API_ VLT_STS VltCloseLibrary( void )
{
    return( VltApiClose() );
}

_API_ VLT_STS VltInitLibrary( VLT_INIT_COMMS_PARAMS* pInitCommsParams )
{

    VLT_U32 size = 0;
    VLT_PU8 data = NULL;
    VLT_U8 arrayofData[1000];
    /**
     * Initialisation of all the function pointers of the 
     * VAULTIC_API structure.
     */
    Api.VltSubmitPassword = VltSubmitPassword;                       
    Api.VltInitializeUpdate = VltInitializeUpdate;
    Api.VltExternalAuthenticate = VltExternalAuthenticate;
    Api.VltManageAuthenticationData = VltManageAuthenticationData;
    Api.VltGetAuthenticationInfo = VltGetAuthenticationInfo;
    Api.VltCancelAuthentication = VltCancelAuthentication;
    Api.VltGetChallenge = VltGetChallenge;
    Api.VltGenericInternalAuthenticate = VltGenericInternalAuthenticate;
    Api.VltGenericExternalAuthenticate = VltGenericExternalAuthenticate;
    Api.VltInitializeAlgorithm = VltInitializeAlgorithm;
	Api.VltUnInitializeAlgorithm = VltUnInitializeAlgorithm;
    Api.VltPutKey = VltPutKey;
    Api.VltReadKey = VltReadKey;
    Api.VltDeleteKey = VltDeleteKey;
    Api.VltEncrypt = VltEncrypt;
    Api.VltDecrypt = VltDecrypt;
    Api.VltGenerateAssuranceMessage = VltGenerateAssuranceMessage;
    Api.VltGenerateSignature = VltGenerateSignature;
    Api.VltVerifySignature = VltVerifySignature;
    Api.VltComputeMessageDigest = VltComputeMessageDigest;
    Api.VltGenerateRandom = VltGenerateRandom;
    Api.VltGenerateKeyPair = VltGenerateKeyPair;
    Api.VltBeginTransaction = VltBeginTransaction;
    Api.VltEndTransaction = VltEndTransaction;
    Api.VltSelectFileOrDirectory = VltSelectFileOrDirectory;
    Api.VltListFiles = VltListFiles;
    Api.VltCreateFile = VltCreateFile;
    Api.VltCreateFolder = VltCreateFolder;
    Api.VltDeleteFile = VltDeleteFile;
    Api.VltDeleteFolder = VltDeleteFolder;
    Api.VltWriteFile = VltWriteFile;
    Api.VltReadFile = VltReadFile;
    Api.VltSeekFile = VltSeekFile;
    Api.VltSetPrivileges = VltSetPrivileges;
    Api.VltSetAttributes = VltSetAttributes;
    Api.VltGetInfo = VltGetInfo;
    Api.VltSelfTest = VltSelfTest;
    Api.VltSetStatus = VltSetStatus;
    Api.VltSetConfig = VltSetConfig;
    Api.VltSetGpioDirection = VltSetGpioDirection;
    Api.VltWriteGpio = VltWriteGpio;
    Api.VltReadGpio = VltReadGpio;
    Api.VltTestCase1 = VltTestCase1;
    Api.VltTestCase2 = VltTestCase2;
    Api.VltTestCase3 = VltTestCase3;
    Api.VltTestCase4 = VltTestCase4;
	Api.VltUpdateSignature = VltUpdateSignature;
    Api.VltComputeSignatureFinal = VltComputeSignatureFinal;
    Api.VltUpdateVerify = VltUpdateVerify;
	Api.VltComputeVerifyFinal = VltComputeVerifyFinal;
    Api.VltUpdateMessageDigest = VltUpdateMessageDigest;
	Api.VltComputeMessageDigestFinal = VltComputeMessageDigestFinal;
#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_2_1 )
	Api.VltDeriveKey = VltDeriveKey;
	Api.VltConstructDHAgreement = VltConstructDHAgreement;
#endif

    /**
     * Initialisation of all the function pointers of the 
     * VAULTIC_SCP structure.
     */
    Auth.VltAuthInit = VltAuthInit;
    Auth.VltAuthClose = VltAuthClose;
    Auth.VltAuthGetState = VltAuthGetState;

    /**
     * Initialisation of all the function pointers of the 
     * VAULTIC_KEY_WRAPPING structure.
     */
    KeyWrapping.VltKeyWrappingInit = VltKeyWrappingInit;
    KeyWrapping.VltKeyWrappingClose = VltKeyWrappingClose;
    KeyWrapping.VltWrapKey = VltWrapKey;
    KeyWrapping.VltUnwrapKey = VltUnwrapKey;

    /**
     * Initialisation of all the function pointers of the 
     * VAULTIC_FILE_SYSTEM structure.
     */
    FileSystem.VltFsOpenFile = VltFsOpenFile;
    FileSystem.VltFsCloseFile = VltFsCloseFile;
    FileSystem.VltFsCreate = VltFsCreate;
    FileSystem.VltFsDelete = VltFsDelete;
    FileSystem.VltFsReadFile = VltFsReadFile;
    FileSystem.VltFsWriteFile = VltFsWriteFile;
    FileSystem.VltFsListFiles = VltFsListFiles;
    FileSystem.VltFsSetPrivileges = VltFsSetPrivileges;
    FileSystem.VltFsSetAttributes = VltFsSetAttributes;
    
    /**
     * Initialisation of all the function pointer of the 
     * VAULTIC_CRC16 structure.
     */    
    Crc16.VltCrc16 = VltCrc16;


    VltFindDevices( &size, NULL );
    data = arrayofData;
    VltFindDevices( &size, data );
    
    return( VltApiInit( pInitCommsParams ) );
}

_API_ VLT_STS VltFindDevices( VLT_PU32 pSize, VLT_PU8 pXmlReaderString )
{
#if( VLT_PLATFORM == VLT_EMBEDDED )

    return( EMETHODNOTSUPPORTED );

#else/*#if( VLT_PLATFORM == VLT_EMBEDDED )*/

    VLT_U32 strCardReaderSize = 0;
#if(VLT_ENABLE_AARDVARK == VLT_ENABLE )  
    VLT_U32 strAardvarkSize = 0;
#endif
#if(VLT_ENABLE_CHEETAH == VLT_ENABLE )
    VLT_U32 strCheetahSize = 0;
#endif
    VLT_U32 count = 0;
    VLT_STS status = VLT_FAIL;

    const char* xmlHeader = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?><devices>";    
    const char* xmlFooter = "</devices>";

    if( NULL == pSize )
    {
        return( EGTDEVSIVLDPARAM );
    }

    if( ( *pSize != 0 ) && ( NULL == pXmlReaderString ) )
    {
        return( EGTDEVSIVLDSTR );
    }    
#if(VLT_ENABLE_ISO7816 == VLT_ENABLE )   
    strCardReaderSize = 0;
    if( VLT_OK != ( status = VltIso7816PtclDetectReaders( &strCardReaderSize , NULL ) ) )
    {
        return( status );
    }
    else
    {
        // Remove null string terminator
        count += strCardReaderSize; 
    }
#endif
#if(VLT_ENABLE_AARDVARK == VLT_ENABLE )
    strAardvarkSize = 0;
    if( VLT_OK != ( status = VltAardvarkDetectReaders( &strAardvarkSize , NULL ) ) )
    {
        return( status );
    }
    else
    {
        // Remove null string terminator
        count += strAardvarkSize;
    }
#endif  
#if(VLT_ENABLE_CHEETAH == VLT_ENABLE )
    strCheetahSize = 0;
    if( VLT_OK != ( status = VltCheetahDetectReaders( &strCheetahSize , NULL ) ) )
    {
        return( status );
    }
    else
    {
        count += strCheetahSize;
    }    
#endif
    /**
     * Add thethe header and footer sizes plus the last null terminator.
     */
    count += ( XML_LB_HEADER_SZ + XML_LB_FOOTER_SZ ) + 1;


    /**
     * If we have been called with a NULL pXmlReaderString then it 
     * means that the caller is only interested in the size. Otherwise
     * we must return the string as well.
     */
    if( NULL == pXmlReaderString  )
    {
        *pSize = count;
    }
    else
    {

        /*
         * Ensure the caller has passed us enough space to copy the 
         * readers xml string
         */
        if( *pSize < count )
        {
            return( EGTDEVSINSFLEN );
        }
        else
        {
            count = 0;

            /**
             * Add the xml header
             */
            sprintf( (char*)&pXmlReaderString[count],"%s", xmlHeader );
            count += XML_LB_HEADER_SZ;
#if(VLT_ENABLE_ISO7816 == VLT_ENABLE )
            /**
             * Add the pcsc xml segment
             */
            if( VLT_OK == ( status = VltIso7816PtclDetectReaders( &strCardReaderSize , 
                &pXmlReaderString[count] ) ) )
            {
                count += strCardReaderSize ;
            }
            else
            {
                return( status );
            }
#endif
#if(VLT_ENABLE_AARDVARK == VLT_ENABLE )
            /**
             * Add the aardvark xml segment
             */
            if( VLT_OK == ( status = VltAardvarkDetectReaders( &strAardvarkSize , 
                &pXmlReaderString[count] ) ) )
            {
                count += strAardvarkSize;
            }
            else
            {
                return( status );
            }
#endif
#if(VLT_ENABLE_CHEETAH == VLT_ENABLE )
            /**
             * Add the cheetah xml segment
             */
            if( VLT_OK == ( status = VltCheetahDetectReaders( &strCheetahSize , 
                &pXmlReaderString[count] ) ) )
            {
                count += strCheetahSize;
            }
            else
            {
                return( status );
            }
#endif
            /**
             * Add the xml footer
             */
            sprintf( (char*)&pXmlReaderString[count],"%s", xmlFooter );
            count += XML_LB_FOOTER_SZ;

            /*
             * Return the total size plus the null terminator
             */
            *pSize = ( count + 1 );
        }
    }

    return( VLT_OK );

#endif/*#if( VLT_PLATFORM == VLT_EMBEDDED )*/

}
