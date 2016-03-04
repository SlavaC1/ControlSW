/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#include "vaultic_common.h"
#include "vaultic_protocol.h"
#if( VLT_ENABLE_ISO7816 == VLT_ENABLE )
    #include "vaultic_iso7816_protocol.h"
#endif
#include "vaultic_block_protocol.h"

/*
* Error Codes
*/
#define EPTCLINITNULLPARAMS       VLT_ERROR( VLT_PTCL, 0 )
#define EPTCLINVLDCOMMSTYPE       VLT_ERROR( VLT_PTCL, 1 )

/*
* Private Data
*/
static VltPtcl theVltPtcl = 
{
    0,
    0,
    0,
#if(VLT_ENABLE_ISO7816 == VLT_ENABLE )
	0,
    0
#endif
};

/*
* Public Methods
*/
VLT_STS VltPtclInit( VLT_INIT_COMMS_PARAMS* pInitCommsParams, 
    VLT_MEM_BLOB *pOutData,
    VLT_MEM_BLOB *pInData,
    VLT_PU16 pu16MaxSendSize,
    VLT_PU16 pu16MaxReceiveSize )
{
    VLT_STS status = VLT_FAIL;

    /*
    * Check that the supplied pointer is valid
    */
    if( ( NULL == pInitCommsParams ) || 
        ( NULL == pOutData ) ||
        ( NULL == pInData ) ||
        ( NULL == pu16MaxSendSize ) ||
        ( NULL == pu16MaxReceiveSize ) )
    {
        return EPTCLINITNULLPARAMS;
    }

    /*
    * Setup routing of calls to the appropraite protocol
    */
    switch(pInitCommsParams->u8CommsProtocol)
    {
#if( VLT_ENABLE_ISO7816 == VLT_ENABLE )    
		case VLT_ISO_T0_OR_T1_COMMS:
        case VLT_ISO_T0_COMMS:
        case VLT_ISO_T1_COMMS:
        case VLT_USB_COMMS:
            theVltPtcl.PtclInit = VltIso7816PtclInit;
            theVltPtcl.PtclClose = VltIso7816PtclClose;
            theVltPtcl.PtclSendReceiveData = VltIso7816PtclSendReceiveData;
            theVltPtcl.PtclCardEvent = VltIso7816PtclCardEvent;
			theVltPtcl.PtclSelectCard = VltIso7816SelectCard;
            status = VLT_OK;
            break;
#endif /*( VLT_ENABLE_ISO7816 == VLT_ENABLE ) */ 

#if( ( VLT_ENABLE_SPI == VLT_ENABLE ) || ( VLT_ENABLE_TWI == VLT_ENABLE ) )
            /*
            * Both SPI and TWI use the Block Protocol
            */
        case VLT_SPI_COMMS:
        case VLT_TWI_COMMS:
            theVltPtcl.PtclInit = VltBlkPtclInit;
            theVltPtcl.PtclClose = VltBlkPtclClose;
            theVltPtcl.PtclSendReceiveData = VltBlkPtclSendReceiveData;
            status = VLT_OK;
            break;
#endif

        default:
            /*
            * Invalid comms type
            */
            status = EPTCLINVLDCOMMSTYPE;
            break;
    }

    if( VLT_OK == status )
    {
        /*
        * Delegate the call to the appropriate protocol
        */
        status = theVltPtcl.PtclInit( pInitCommsParams, 
            pOutData, 
            pInData, 
            pu16MaxSendSize,
            pu16MaxReceiveSize );
    }

    return( status );
}

VLT_STS VltPtclClose( void )
{
    /*
    * Delegate the call to the appropriate protocol
    */
    return theVltPtcl.PtclClose( );
}

VLT_STS VltPtclSendReceiveData( VLT_MEM_BLOB *pOutData, VLT_MEM_BLOB *pInData )
{   
    /*
    * Delegate the call to the appropriate protocol
    */
    return theVltPtcl.PtclSendReceiveData( pOutData, pInData );
}

#if(VLT_ENABLE_ISO7816 == VLT_ENABLE )
VLT_STS VltPtclCardEvent(VLT_PU8 pu8ReaderName, DWORD dwTimeout,PDWORD pdwEventState)
{
    if (theVltPtcl.PtclCardEvent)
		return theVltPtcl.PtclCardEvent(pu8ReaderName,dwTimeout,pdwEventState);
    return VLT_OK;
}

VLT_STS VltPtclSelectCard( SCARDHANDLE hScard , SCARDCONTEXT hCxt, DWORD dwProtocol)
{
    if (theVltPtcl.PtclSelectCard)
		return theVltPtcl.PtclSelectCard(hScard,hCxt,dwProtocol);
    return VLT_OK;
}
#endif