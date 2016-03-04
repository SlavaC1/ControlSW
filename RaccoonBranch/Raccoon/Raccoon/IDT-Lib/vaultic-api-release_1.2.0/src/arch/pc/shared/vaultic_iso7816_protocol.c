/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#include "vaultic_common.h"
#include "vaultic_apdu.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if( VLT_PLATFORM == VLT_LINUX )
    #include <reader.h>
#endif

#ifdef WIN32
#include <winscard.h>
#else
#include "PCSC/wintypes.h"
#endif

#ifndef SCARD_E_NO_READERS_AVAILABLE
#define SCARD_E_NO_READERS_AVAILABLE    0x8010002E
#endif

#ifndef SCARD_ATTR_VALUE
#define SCARD_ATTR_VALUE(Class, Tag) ((((ULONG)(Class)) << 16) | ((ULONG)(Tag)))
#endif

#ifndef SCARD_CLASS_IFD_PROTOCOL
#define SCARD_CLASS_IFD_PROTOCOL     8   /**< Interface Device Protocol Options */
#endif

#ifndef SCARD_ATTR_CURRENT_IFSD
#define SCARD_ATTR_CURRENT_IFSD SCARD_ATTR_VALUE(SCARD_CLASS_IFD_PROTOCOL, 0x0208)
#endif

#if( VLT_PLATFORM == VLT_WINDOWS )
    #pragma warning(disable : 4996)
#endif

/*
* Error Codes
*/
#define ENULLCOMMSPARAMS     VLT_ERROR( VLT_ISO7816, 0 )
#define EINVLDPRTCL          VLT_ERROR( VLT_ISO7816, 1 )
#define EINVLDSCARDCONTEXT   VLT_ERROR( VLT_ISO7816, 2 )
#define ELISTREADERSIZE      VLT_ERROR( VLT_ISO7816, 3 )
#define ELISTREADERS         VLT_ERROR( VLT_ISO7816, 4 )
#define EINVLDSCARDCONNT0    VLT_ERROR( VLT_ISO7816, 5 )
#define EINVLDSCARDCONNT1    VLT_ERROR( VLT_ISO7816, 6 )
#define ESCARDNOTCONNECTED   VLT_ERROR( VLT_ISO7816, 7 )
#define ESCARDEXCEPTION      VLT_ERROR( VLT_ISO7816, 8 )
#define EFAILEDSCARDTRANS    VLT_ERROR( VLT_ISO7816, 9 )
#define ESCARDGETATTRIB1     VLT_ERROR( VLT_ISO7816, 10 )
#define ESCARDGETATTRIB2     VLT_ERROR( VLT_ISO7816, 11 )
#define ESCARDDISCONNECT     VLT_ERROR( VLT_ISO7816, 12 )
#define ESCARDRELEASECONTEXT VLT_ERROR( VLT_ISO7816, 13 )
#define EDTCRDRSIVLDCARDCTX  VLT_ERROR( VLT_ISO7816, 14 )
#define EDTCRDRSLSTRDRSZ     VLT_ERROR( VLT_ISO7816, 15 )
#define EDTCRDRSMALLOC       VLT_ERROR( VLT_ISO7816, 16 )
#define EDTCRDRSLSTRDRSTR    VLT_ERROR( VLT_ISO7816, 17 )
#define EDTCRDRSRLSCTX       VLT_ERROR( VLT_ISO7816, 18 )
#define EDTCRDRSIVLDPARAM    VLT_ERROR( VLT_ISO7816, 19 )
#define EDTCRDRSINSFLEN      VLT_ERROR( VLT_ISO7816, 20 )
#define EDTCRDRSIVLDSTR      VLT_ERROR( VLT_ISO7816, 21 )
#define EFAILEDTIMEOUT		 VLT_ERROR( VLT_ISO7816, 22 )
/*
* Defines
*/
#define XML_PCSC_HEADER_SZ      (VLT_U8)23
#define XML_PCSC_DATA_SZ        (VLT_U8)34
#define XML_PCSC_FOOTER_SZ      (VLT_U8)12

/*
* Private Data
*/
#if( VLT_ENABLE_ISO7816 == VLT_ENABLE )
    static SCARDHANDLE   hVaultIC = 0; /* VaultIC Handle */
    static SCARDCONTEXT  hContext = 0; /* PC/SC system handle */
    static VLT_U32 u32SCardProtocol = SCARD_PROTOCOL_T0;
#endif

/*
* Defines
*/
#define IFSD_SIZE      (VLT_U8)4

VLT_STS VltIso7816PtclInit( VLT_INIT_COMMS_PARAMS* pInitCommsParams,
    VLT_MEM_BLOB *pOutData,
    VLT_MEM_BLOB *pInData,
    VLT_PU16 pu16MaxSendSize,
    VLT_PU16 pu16MaxReceiveSize)
{
#if( VLT_ENABLE_ISO7816 == VLT_ENABLE )
    VLT_STS status = VLT_FAIL;
    DWORD       activeProt = 0;
    SCARDHANDLE*    pSlot = 0;
    DWORD sCardStatus = SCARD_S_SUCCESS;
    const char* pszReaderString = 0;

    /*
    * Check that the input parameter pointer is valid
    */
    if( ( NULL == pInitCommsParams ) ||
        ( NULL == pOutData ) || 
        ( NULL == pInData ) ||
        ( NULL == pu16MaxSendSize ) ||
        ( NULL == pu16MaxReceiveSize ) )
    {
        return ENULLCOMMSPARAMS;
    }
    else
    {
        /*
        * Input parameter was OK so setup the communictaions parameters
        */
        switch(pInitCommsParams->u8CommsProtocol)
        {
            /*
            * ISO T0 and USB both use T0
            */
            case VLT_ISO_T0_COMMS:
            case VLT_USB_COMMS:
                u32SCardProtocol = SCARD_PROTOCOL_T0;
                break;
            case VLT_ISO_T1_COMMS:
                u32SCardProtocol = SCARD_PROTOCOL_T1;
                break;
			case VLT_ISO_T0_OR_T1_COMMS:
				u32SCardProtocol = SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1;
				break;
            default:
                return EINVLDPRTCL;
                break;
        }

        pszReaderString = (char*)pInitCommsParams->Params.VltPcscInitParams.pu8ReaderString;
    }

#if( VLT_ENABLE_MULTI_SLOT == VLT_ENABLE )
	hVaultIC = pInitCommsParams->Params.VltPcscInitParams.hCard;

	if (hVaultIC)
	{
		*pu16MaxSendSize = VLT_MAX_APDU_SND_DATA_SZ;
		*pu16MaxReceiveSize = VLT_MAX_APDU_RCV_DATA_SZ;
		return VLT_OK;
	}
	else
	{
		/*
		* Gain access to the smart card manager PC/SC
		*/
		if( SCARD_S_SUCCESS != 
			SCardEstablishContext( SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext ) )
		{
			status = EINVLDSCARDCONTEXT;
		}
		else
		{
			status = VLT_OK;
		}


		if( VLT_OK == status )
		{
			if( 0 == hVaultIC )
			{
				pSlot = (SCARDHANDLE*)&hVaultIC;

				/*
				* Always connect using T0 in the first instance
				*/
				sCardStatus = SCardConnect( hContext,
					pszReaderString,
					SCARD_SHARE_SHARED,
					u32SCardProtocol,
					pSlot,
					&activeProt );
				if (sCardStatus != SCARD_S_SUCCESS)
				{
					status = EINVLDSCARDCONNT0;
				}
				else
				{
					u32SCardProtocol = activeProt;
					pInitCommsParams->Params.VltPcscInitParams.hCard = *pSlot;
					pInitCommsParams->Params.VltPcscInitParams.hContext = hContext;
					pInitCommsParams->Params.VltPcscInitParams.u32Protocol = activeProt;
					u32SCardProtocol = activeProt;
					*pu16MaxSendSize = VLT_MAX_APDU_SND_DATA_SZ;
					*pu16MaxReceiveSize = VLT_MAX_APDU_RCV_DATA_SZ;
					status = VLT_OK;
				}
			}
		}
	}
	return( status );
#else
	/*
	* Gain access to the smart card manager PC/SC
	*/
	if( SCARD_S_SUCCESS != 
		SCardEstablishContext( SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext ) )
	{
		status = EINVLDSCARDCONTEXT;
	}
	else
	{
		status = VLT_OK;
	}

	if( VLT_OK == status )
	{
		if( 0 == hVaultIC )
		{
			pSlot = (SCARDHANDLE*)&hVaultIC;

			/*
			* Always connect using T0 in the first instance
			*/
			sCardStatus = SCardConnect( hContext,
				pszReaderString,
				SCARD_SHARE_SHARED,
				u32SCardProtocol,
				pSlot,
				&activeProt );
			if (sCardStatus != SCARD_S_SUCCESS)
			{
				status = EINVLDSCARDCONNT0;
			}
			else
			{
				pInitCommsParams->Params.VltPcscInitParams.hCard = *pSlot;
				pInitCommsParams->Params.VltPcscInitParams.u32Protocol = activeProt;
				u32SCardProtocol = activeProt;
				*pu16MaxSendSize = VLT_MAX_APDU_SND_DATA_SZ;
				*pu16MaxReceiveSize = VLT_MAX_APDU_RCV_DATA_SZ;
				status = VLT_OK;
			}
		}
	}
	return( status );
#endif
#else
    return( EMETHODNOTSUPPORTED );
#endif
}

#if (VLT_ENABLE_ISO7816 == VLT_ENABLE)
VLT_STS VltIso7816SelectCard( SCARDHANDLE hScard , SCARDCONTEXT hCxt, DWORD dwProtocol)
{
#if( VLT_ENABLE_MULTI_SLOT == VLT_ENABLE)
	hVaultIC = hScard;
	hContext = hCxt;
	u32SCardProtocol = (VLT_U32)dwProtocol;
	return (VLT_OK);
#else
    return( EMETHODNOTSUPPORTED );
#endif
}
#endif

VLT_STS VltIso7816PtclClose( void )
{
#if( VLT_ENABLE_ISO7816 == VLT_ENABLE )
    VLT_STS status = VLT_FAIL;

    /*
    * Disconnect from the Vault IC
    */
    if( 0 != hVaultIC )
    {
        LONG lRet = SCardDisconnect( hVaultIC, SCARD_UNPOWER_CARD );
        if( SCARD_S_SUCCESS == lRet || SCARD_E_INVALID_HANDLE == lRet)
        {
            status = VLT_OK;
        }
        else
        {
            status = ESCARDDISCONNECT;
        }
        hVaultIC = 0;
    }
    else
    {
        status = VLT_OK;
    }

    /*
    * Release the Smart Card context
    */
    if( 0 != hContext )
    {
        if( SCARD_S_SUCCESS == SCardReleaseContext( hContext ) )
        {
            hContext = 0;
        }
        else
        {
            /*
            * Only update the status if there is no error code currently
            */
            if( VLT_OK == status )
            {
                status = ESCARDRELEASECONTEXT;
            }
        }
    }

    return ( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}

VLT_STS VltIso7816PtclSendReceiveData( const VLT_MEM_BLOB *pOutData, VLT_MEM_BLOB *pInData )
{
#if( VLT_ENABLE_ISO7816 == VLT_ENABLE )
    VLT_STS status = VLT_FAIL;
    VLT_U32 lReturn = 0;
    VLT_U32 pOutDataLen = pOutData->u16Len;
    VLT_U32 inDataLen = VLT_MAX_APDU_RCV_TRANS_SIZE;
    SCARD_IO_REQUEST sendPciReq;
    SCARD_IO_REQUEST recPciReq;

    if( 0 == hVaultIC )
    {
        return ESCARDNOTCONNECTED;
    }

#ifdef WIN32
    __try 
#endif
    {
		if(SCARD_PROTOCOL_T0 == u32SCardProtocol)
        {
            sendPciReq = *SCARD_PCI_T0;
            recPciReq = *SCARD_PCI_T0;
        }
		else 
        {
            sendPciReq = *SCARD_PCI_T1;
            recPciReq = *SCARD_PCI_T1;
        }
        lReturn = SCardTransmit( hVaultIC,
            &sendPciReq,
            pOutData->pu8Data,
            pOutDataLen,
            &recPciReq,
            pInData->pu8Data,
            (LPDWORD)&inDataLen);
    }
#ifdef WIN32    
    __except (EXCEPTION_EXECUTE_HANDLER )
    {
        status = ESCARDEXCEPTION;   
    }
#endif

    if ( SCARD_S_SUCCESS != lReturn )
    {
        hVaultIC = 0;
		if (SCARD_E_COMM_DATA_LOST == lReturn)
			status = EFAILEDTIMEOUT;
		else
        status =  EFAILEDSCARDTRANS;
    }
    else
    {
        pInData->u16Len = (VLT_U16)inDataLen;
        status = VLT_OK;
    }

    return( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}

//
// If the pXmlReaderString is null and pSize not null and a value of zero it 
// means return to the pSize the total number of bytes required to read the
// entire XML string.
//
//
VLT_STS VltIso7816PtclDetectReaders( VLT_PU32 pSize, VLT_PU8 pXmlReaderString )
{
#if (VLT_ENABLE_ISO7816 == VLT_ENABLE)
    VLT_STS status = VLT_FAIL;
    VLT_U32 cchReaders = (VLT_U32)0;
    VLT_PU8 pReadersString = NULL;
    SCARDCONTEXT hCtx = (SCARDCONTEXT)0; /* PC/SC system handle */
    VLT_U16 numOfReaders = 0;
    VLT_U16 index = 0;
    VLT_U16 count = 0;
    const char* pXmlHeader = "<interface type=\"pcsc\">"; // 23, 0x17
    const char* pXmlData = "<peripheral idx=\"%02d\">%s</peripheral>"; //34 , 0x22     
    const char* pXmlFooter = "</interface>"; //12 - 0x0C  
    LONG lStatus = 0;
    
    if( NULL == pSize )
    {
        return( EDTCRDRSIVLDPARAM );
    }

    if( ( *pSize != 0 ) && ( NULL == pXmlReaderString ) )
    {
        return( EDTCRDRSIVLDSTR );
    }

    /*
     * Gain access to the smart card manager PC/SC
     */
    if( SCARD_S_SUCCESS != SCardEstablishContext( SCARD_SCOPE_SYSTEM, 
        NULL, NULL, &hCtx ) )
    {
        status = EDTCRDRSIVLDCARDCTX;
        hCtx = (SCARDCONTEXT)0;
    }
    else
    {
        status = VLT_OK;
    }

    if( VLT_OK == status )
    {
        /*
         * Determine the size of the string for the readers.
         */
        lStatus = SCardListReaders( hCtx, NULL, NULL, (DWORD*)&cchReaders );
        if( SCARD_E_NO_READERS_AVAILABLE == lStatus )
        {
            numOfReaders = 0;
            status = VLT_OK;
            
        } 
        else 
        {   
            if( lStatus != SCARD_S_SUCCESS )
            {
                if (hCtx)
                    SCardReleaseContext( hCtx );
                return( EDTCRDRSLSTRDRSZ );
            }
        }

        if( NULL == ( pReadersString = (VLT_PU8)malloc( cchReaders ) ) )
        {
            status = EDTCRDRSMALLOC;
        }

        /*
         * Retrieve the complete readers string 
         */
        if( ( VLT_OK == status ) && ( SCARD_E_NO_READERS_AVAILABLE != lStatus ) )
        {
            if( ( SCARD_S_SUCCESS != SCardListReaders( hCtx, NULL, (LPSTR)pReadersString, (DWORD*)&cchReaders ) ) )
            {
                free( (void*)pReadersString );
                status = EDTCRDRSLSTRDRSTR;
            }
        }
    }

    if( VLT_OK == status )
    {
        index = 0;        

        /*
         * Determine the number of readers in the list.
         */
        if( SCARD_E_NO_READERS_AVAILABLE != lStatus )
        {
            while( 0 != ( count = (VLT_U16)strlen( (char*)&pReadersString[index] ) ) )
            {
                ++numOfReaders;            
                index += ( count + 1 ); // skip the null terminator.
            }
        }        

        /*
         * Total String count is:
         */
        count = ( XML_PCSC_HEADER_SZ + ( numOfReaders * XML_PCSC_DATA_SZ) + XML_PCSC_FOOTER_SZ );
        /*
         * Remove the null terminator per reader, plus the last of the double one.
         */
        count += ( (VLT_U16)cchReaders - numOfReaders);

        if( SCARD_E_NO_READERS_AVAILABLE != lStatus )
        {
            count -= 1;
        }

        if( NULL == pXmlReaderString  )
        {
            /*
             * Return to the caller the amount of space required to 
             * extract the reader string.
             */
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
                status = EDTCRDRSINSFLEN;
            }
            else
            {
                index = 0;
                *pSize = 0;
                numOfReaders = 0;

                /*
                 * Add the XML header
                 */
                *pSize = sprintf( (LPSTR)&pXmlReaderString[*pSize], "%s", pXmlHeader );

                /*
                 * Add the XML Data
                 */
                if( SCARD_E_NO_READERS_AVAILABLE != lStatus )
                {
                    while( 0 != ( count = (VLT_U16)strlen( (char*)&pReadersString[index] ) ) )
                    {                 
                        *pSize += sprintf( (char*)&pXmlReaderString[*pSize], (const char*)pXmlData, 
                            numOfReaders, &pReadersString[index] );

                        index += ( count + 1 ); // skip the null terminator.
                        ++numOfReaders; 
                    }
                }

                /*
                 * Add the XML Footer
                 */
                *pSize += sprintf( (LPSTR)&pXmlReaderString[*pSize], "%s", pXmlFooter );
            }            
		}

		/*
		* Release the readers string memory if it has been allocated.
		*/
		if( NULL != pReadersString )
		{
			free( (void*)pReadersString ); 
			pReadersString = NULL;
		}
	}

    /*
     * Release the card context if one has been established.
     */
    if( (SCARDCONTEXT)0 != hCtx )
    {
        if( SCARD_S_SUCCESS != SCardReleaseContext( hCtx ) )
        {
            status = EDTCRDRSRLSCTX;
        }
    }   

    return( status );
#else
	return( EMETHODNOTSUPPORTED );
#endif
}

VLT_STS VltIso7816PtclCardEvent(VLT_PU8 pu8ReaderName, DWORD dwTimeout,PDWORD pdwEventState)
{
#if( VLT_ENABLE_ISO7816 == VLT_ENABLE )
	SCARD_READERSTATE rgReaderStates[1];
	
	if (pu8ReaderName)
	{
		rgReaderStates[0].szReader = (char *)pu8ReaderName;
		rgReaderStates[0].dwCurrentState = *pdwEventState;
		{
			SCARDCONTEXT hSC;
			LONG lRet = SCardEstablishContext(SCARD_SCOPE_USER,
                                NULL,
                                NULL,
                                &hSC );
			if ( SCARD_S_SUCCESS == lRet )
			{
				lRet = SCardGetStatusChange( hSC,dwTimeout, rgReaderStates,1);
				if( SCARD_S_SUCCESS == lRet )
				{
					*pdwEventState = rgReaderStates->dwEventState;
					return VLT_OK;
				}
			}
			SCardReleaseContext(hSC);
		}
	}
	*pdwEventState = SCARD_STATE_UNAWARE;
	return VLT_FAIL;
#else
	return( EMETHODNOTSUPPORTED );
#endif
}