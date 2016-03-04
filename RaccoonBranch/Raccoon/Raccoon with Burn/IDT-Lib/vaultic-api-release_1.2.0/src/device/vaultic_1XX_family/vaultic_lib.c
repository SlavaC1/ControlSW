/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * --------------------------------------------------------------------------*/

#include "vaultic_common.h"
#include "vaultic_api.h"
#include "vaultic_lib.h"
#include "vaultic_crc16.h"
#include "vaultic_ecdsa_strong_authentication.h"
#include "vaultic_mem.h"
#include "vaultic_ecdsa_signer.h"
#include <stdio.h>


#if( VLT_PLATFORM == VLT_WINDOWS )
    #pragma warning(disable : 4996)
#endif 

/*
 * Declare static instances of the required components parts of the VaultIC API.
 */
static VAULTIC_API Api;
static VAULTIC_STRONG_AUTHENTICATION StrongAuthentication;
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
extern VLT_STS VltAardvarkDetectReaders( VLT_PU32 pSize, VLT_PU8 pXmlReaderString );
extern VLT_STS VltIso7816PtclDetectReaders( VLT_PU32 pSize, VLT_PU8 pXmlReaderString );

_API_ VLT_STS VltGetLibraryInfo( VLT_LIBRARY_INFO* pLibraryInfo )
{
    return( VltGetLibInfo( pLibraryInfo ) );
}

_API_ VAULTIC_API* VltGetApi( void )
{
    if( ( 0 != Api.VltCancelAuthentication ) &&											
        ( 0 != Api.VltComputeMessageDigest ) &&											
        ( 0 != Api.VltGenerateAssuranceMessage ) &&										
        ( 0 != Api.VltVerifyAssuranceMessage ) &&										
        ( 0 != Api.VltGenerateKeyPair ) &&												
        ( 0 != Api.VltGenerateRandom ) &&												
        ( 0 != Api.VltGenerateSignature ) &&											
        ( 0 != Api.VltExternalAuthenticate ) &&											
        ( 0 != Api.VltInternalAuthenticate ) &&											
        ( 0 != Api.VltGetAuthenticationInfo ) &&										
        ( 0 != Api.VltGetInfo ) &&														
        ( 0 != Api.VltInitializeAlgorithm ) &&											
        ( 0 != Api.VltManageAuthenticationData ) &&										
        ( 0 != Api.VltPutKey ) &&														
        ( 0 != Api.VltReadFile ) &&														
        ( 0 != Api.VltReadKey ) &&														
        ( 0 != Api.VltSeekFile ) &&														
        ( 0 != Api.VltSelectFile ) &&													
        ( 0 != Api.VltSelfTest ) &&														
        ( 0 != Api.VltSetConfig ) &&													
        ( 0 != Api.VltSetStatus ) &&													
        ( 0 != Api.VltSubmitPassword ) &&												
        ( 0 != Api.VltTestCase1 ) &&													
        ( 0 != Api.VltTestCase2 ) &&													
        ( 0 != Api.VltTestCase3 ) &&													
        ( 0 != Api.VltTestCase4 ) &&													
        ( 0 != Api.VltVerifySignature ) &&												
        ( 0 != Api.VltWriteFile ) &&	
		( 0 != Api.VltUpdateSignature ) &&	
		( 0 != Api.VltComputeSignatureFinal ) &&
		( 0 != Api.VltUpdateVerify ) &&
		( 0 != Api.VltComputeVerifyFinal ) &&
		( 0 != Api.VltUpdateMessageDigest ) &&
		( 0 != Api.VltComputeMessageDigestFinal) 
#if (VLT_ENABLE_SECURE_COUNTERS == VLT_ENABLE)
		&&( 0 != Api.VltIncrementCounter ) &&
		( 0 != Api.VltDecrementCounter) &&
		( 0 != Api.VltSetSecureCounters) 
#endif
#if (VLT_ENABLE_ISO7816 == VLT_ENABLE)
		&& ( 0 != Api.VltCardEvent ) &&	
		( 0 != Api.VltSelectCard )
#endif
#if(VLT_ENABLE_CHIP_DEACTIVATION == VLT_ENABLE)
	 && ( 0 != Api.VltDeactivate) &&
		( 0 != Api.VltGetChallenge) &&
		( 0 != Api.VltActivate)
#endif
		)														
    {																					
        return( &Api );																	
    }																					
																						
    return( NULL );																		
}																						
																						
																						

#if ( VLT_ENABLE_IDENTITY_AUTH_V2 == VLT_ENABLE )
_API_ VAULTIC_STRONG_AUTHENTICATION* VltGetStrongAuthentication( void )
{
    if( ( 0 != StrongAuthentication.VltStrongAuthenticate ) &&
        ( 0 != StrongAuthentication.VltStrongSetCryptoParams ) &&
        ( 0 != StrongAuthentication.VltStrongGetState ) &&
        ( 0 != StrongAuthentication.VltStrongClose ) &&
		( 0 != StrongAuthentication.EcdsaSignerClose ) &&
		( 0 != StrongAuthentication.EcdsaSignerDoFinal ) &&
		( 0 != StrongAuthentication.EcdsaSignerInit ) &&
		( 0 != StrongAuthentication.EcdsaSignerUpdate ) )
    {
        return( &StrongAuthentication );
    }
    return( NULL );
}
#endif /* #if ( VLT_ENABLE_IDENTITY_AUTH_V2 == VLT_ENABLE ) */





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
    Api.VltManageAuthenticationData = VltManageAuthenticationData;
    Api.VltGetAuthenticationInfo = VltGetAuthenticationInfo;
    Api.VltCancelAuthentication = VltCancelAuthentication;
    Api.VltInternalAuthenticate = VltInternalAuthenticate;
    Api.VltExternalAuthenticate = VltExternalAuthenticate;
    Api.VltInitializeAlgorithm = VltInitializeAlgorithm;
    Api.VltPutKey = VltPutKey;
    Api.VltReadKey = VltReadKey;
    Api.VltGenerateAssuranceMessage = VltGenerateAssuranceMessage;
    Api.VltVerifyAssuranceMessage = VltVerifyAssuranceMessage;
    Api.VltGenerateSignature = VltGenerateSignature;
    Api.VltVerifySignature = VltVerifySignature;
    Api.VltComputeMessageDigest = VltComputeMessageDigest;
    Api.VltGenerateRandom = VltGenerateRandom;
    Api.VltGenerateKeyPair = VltGenerateKeyPair;
    Api.VltSelectFile = VltSelectFile;
    Api.VltWriteFile = VltWriteFile;
    Api.VltReadFile = VltReadFile;
    Api.VltSeekFile = VltSeekFile;
    Api.VltGetInfo = VltGetInfo;
    Api.VltSelfTest = VltSelfTest;
    Api.VltSetStatus = VltSetStatus;
    Api.VltSetConfig = VltSetConfig;
    Api.VltTestCase1 = VltTestCase1;
    Api.VltTestCase2 = VltTestCase2;
    Api.VltTestCase3 = VltTestCase3;
    Api.VltTestCase4 = VltTestCase4;
	Api.VltUpdateMessageDigest = VltUpdateMessageDigest;
	Api.VltUpdateSignature = VltUpdateSignature;
	Api.VltUpdateVerify = VltUpdateVerify;
	Api.VltComputeMessageDigestFinal = VltComputeMessageDigestFinal;
	Api.VltComputeVerifyFinal= VltComputeVerifyFinal;
	Api.VltComputeSignatureFinal = VltComputeSignatureFinal;
#if (VLT_ENABLE_SECURE_COUNTERS == VLT_ENABLE)
	Api.VltIncrementCounter = VltIncrementCounter;
	Api.VltDecrementCounter = VltDecrementCounter;
	Api.VltSetSecureCounters = VltSetSecureCounters;
#endif
#if (VLT_ENABLE_ISO7816 == VLT_ENABLE)
	Api.VltCardEvent = VltCardEvent;
	Api.VltSelectCard = VltSelectCard;
#endif
#if(VLT_ENABLE_CHIP_DEACTIVATION == VLT_ENABLE)
	Api.VltDeactivate = VltDeactivate;
	Api.VltGetChallenge = VltGetChallenge;
	Api.VltActivate = VltActivate;
#endif
    /**
     * Initialisation of all the function pointers of the 
     * VAULTIC_STRONG_AUTHENTICATION structure.
	 * These are set-up to point directly to the ECDSA
	 * implementation becuase that's the only method 
	 * supported on VaultIC100
     */
	host_memset((VLT_PU8)&StrongAuthentication, 0, sizeof(StrongAuthentication));
    StrongAuthentication.VltStrongAuthenticate = VltEcdsaStrongAuthenticate;
    StrongAuthentication.VltStrongSetCryptoParams = VltEcdsaStrongSetCryptoParams;
    StrongAuthentication.VltStrongClose = VltEcdsaStrongClose;
    StrongAuthentication.VltStrongGetState = VltEcdsaStrongGetState;
	StrongAuthentication.EcdsaSignerClose = EcdsaSignerClose;
	StrongAuthentication.EcdsaSignerDoFinal = EcdsaSignerDoFinal;
	StrongAuthentication.EcdsaSignerInit = EcdsaSignerInit;
	StrongAuthentication.EcdsaSignerUpdate = EcdsaSignerUpdate;
 
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
#if(VLT_ENABLE_AARDVARK == VLT_ENABLE )  
    VLT_U32 strAardvarkSize = 0;
#endif
    VLT_U32 strCardReaderSize = 0;
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
             * Add the PCSC xml segment
             */
            if( VLT_OK == ( status = VltIso7816PtclDetectReaders( &strCardReaderSize , 
                &pXmlReaderString[count] ) ) )
            {
                count += strCardReaderSize;
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

#endif /*#if( VLT_PLATFORM == VLT_EMBEDDED )*/

}
