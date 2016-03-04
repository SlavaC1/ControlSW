/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#include "vaultic_common.h"
#include "vaultic_comms.h"
#include "vaultic_protocol.h"
#include "vaultic_apdu.h"

#if( ( VLT_ENABLE_SCP02 == VLT_ENABLE ) || ( VLT_ENABLE_SCP03 == VLT_ENABLE ) )
    #include "vaultic_secure_channel.h"
#endif /* ( ( VLT_ENABLE_SCP02 == VLT_ENABLE ) || ( VLT_ENABLE_SCP03 == VLT_ENABLE ) ) */

#if( ( VLT_ENABLE_SPI == VLT_ENABLE ) || ( VLT_ENABLE_TWI == VLT_ENABLE ) || ( VLT_ENABLE_ISO7816 == VLT_ENABLE ))
    #include "vaultic_block_protocol.h"
#endif /* ( ( VLT_ENABLE_SPI == VLT_ENABLE ) || ( VLT_ENABLE_TWI == VLT_ENABLE ) ) */

/*
* Error Codes
*/
#define EINITNULLPARAM      VLT_ERROR( VLT_COMMS, 0 )
#define EDISPZEROLEN        VLT_ERROR( VLT_COMMS, 1 )
#define EDISPNULLIDATA      VLT_ERROR( VLT_COMMS, 2 )
#define EDISPNULLODATA      VLT_ERROR( VLT_COMMS, 3 )
#define EDISPNULLRSPDATA    VLT_ERROR( VLT_COMMS, 4 )

/*
* Defines
*/
#define MIN_CMD_SZ                (VLT_U8) 5
#define APDU_COMMAND_OFFSET       (VLT_U8) VLT_BLOCK_PROTOCOL_HDR_SZ
#define VLT_COMMS_CMD_BUFFER_SIZE (VLT_U16)(VLT_BLOCK_PROTOCOL_OH + VLT_MAX_APDU_SND_TRANS_SIZE)
#define VLT_COMMS_RSP_BUFFER_SIZE (VLT_U16)(VLT_BLOCK_PROTOCOL_OH + VLT_MAX_APDU_RCV_TRANS_SIZE) 

/*
* Private Data
*/
static VLT_U8 commsBuffer[VLT_COMMS_CMD_BUFFER_SIZE];
static VLT_U16 u16MaxSendSize = VLT_MAX_APDU_SND_DATA_SZ;
static VLT_U16 u16MaxReceiveSize = VLT_MAX_APDU_RCV_DATA_SZ;

VLT_STS VltCommsInit( VLT_INIT_COMMS_PARAMS* pInitCommsParams,
    VLT_MEM_BLOB *Command, 
    VLT_MEM_BLOB *Response )
{
    VLT_STS status = VLT_FAIL;
    VLT_MEM_BLOB outData;
    VLT_MEM_BLOB inData;
    
    /*
    * Check input params
    */
    if( NULL == pInitCommsParams )
    {
        return EINITNULLPARAM;
    }

    /*
    * Setup the MEM_BLOBs passed for Command and Response to use the commsBuffer
    * Data should be added and removed from the APDU_COMMAND_OFFSET so that
    * any Block Protocol data is not seen at the API level
    */
    Command->pu8Data = &commsBuffer[APDU_COMMAND_OFFSET];
    Command->u16Capacity = VLT_MAX_APDU_SND_TRANS_SIZE;
    Command->u16Len = 0;

    Response->pu8Data = &commsBuffer[APDU_COMMAND_OFFSET];;
    Response->u16Capacity = VLT_MAX_APDU_RCV_TRANS_SIZE;
    Response->u16Len = 0;

    /*
    * The actual Comms Buffer has extra space in it.  The peripherals need 
    * to know about this
    */
    outData.pu8Data = &commsBuffer[0];
    outData.u16Len = 0;
    outData.u16Capacity = VLT_COMMS_CMD_BUFFER_SIZE;

    inData.pu8Data = &commsBuffer[0];
    inData.u16Len = 0;
    inData.u16Capacity = VLT_COMMS_RSP_BUFFER_SIZE;

    status = VltPtclInit( pInitCommsParams, &outData, &inData, &u16MaxSendSize, &u16MaxReceiveSize );

    return( status );
}

VLT_STS VltCommsClose( void )
{
    VLT_STS status = VLT_FAIL;

    status = VltPtclClose();

    return( status );
}

VLT_STS VltCommsDispatchCommand( VLT_MEM_BLOB *Command, VLT_MEM_BLOB *Response )
{   
    VLT_STS status = VLT_FAIL;

    /*
    * Ensure we a valid Command data pointer
    */
    if( NULL == Command->pu8Data )
    {
        return( EDISPNULLIDATA );
    }
    /*
    * Ensure we have a valid size command
    */
    if( MIN_CMD_SZ > Command->u16Len )
    {
        return( EDISPZEROLEN );
    }

    /*
    * Ensure we have a valid Response pointer
    */
    if( NULL == Response )
    {
        return( EDISPNULLRSPDATA );
    }

    /*
    * Ensure we have a valid Response data pointer
    */
    if( NULL == Response->pu8Data )
    {
        return( EDISPNULLODATA );
    }

    /*
    * The Status Word SW is always implied 
    */
    Response->u16Len += VLT_SW_SIZE;

#if( ( VLT_ENABLE_SCP02 == VLT_ENABLE ) || ( VLT_ENABLE_SCP03 == VLT_ENABLE ) )
    /*
    * Call ScpWrap method to add appropriate security data
    */
    status = VltScpWrap( Command );
#else
    status = VLT_OK;
#endif

    /*
    * Send the Data to the Peripheral and get the Response from it
    */
    if( VLT_OK == status )
    {
        status = VltPtclSendReceiveData( Command, Response );
    }

#if( ( VLT_ENABLE_SCP02 == VLT_ENABLE ) || ( VLT_ENABLE_SCP03 == VLT_ENABLE ) )
    /*
    * Call Secure Channel Unwrap to remove any security data
    */
    if( VLT_OK == status )
    {
        status = VltScpUnwrap ( Response );
    }
#endif

    return( status );
}

VLT_U16 VltCommsGetMaxSendSize( void )
{
#if( ( VLT_ENABLE_SCP02 == VLT_ENABLE ) || ( VLT_ENABLE_SCP03 == VLT_ENABLE ) )
    VLT_U8 u8Overhead = 0;
    /*
    * The call to VltScpGetChannelOverhead will fail if the conditional compilation
    * switch for Secure Channel has not been enabled
    */
    VltScpGetChannelOverhead( SECURE_CHANNEL_SEND, &u8Overhead );

    return u16MaxSendSize - u8Overhead;
#else    
    return u16MaxSendSize;
#endif /* #if( ( VLT_ENABLE_SCP02 == VLT_ENABLE ) || ( VLT_ENABLE_SCP03 == VLT_ENABLE ) ) */
}

VLT_U16 VltCommsGetMaxReceiveSize( void )
{
#if( ( VLT_ENABLE_SCP02 == VLT_ENABLE ) || ( VLT_ENABLE_SCP03 == VLT_ENABLE ) )

    VLT_U8 u8Overhead = 0;

    /*
    * The call to VltScpGetChannelOverhead will fail if the conditional compilation
    * switch for Secure Channel has not been enabled
    */
    VltScpGetChannelOverhead( SECURE_CHANNEL_RECEIVE, &u8Overhead );
    
    return u16MaxReceiveSize - u8Overhead;

#else
    return u16MaxReceiveSize;
#endif /* #if( ( VLT_ENABLE_SCP02 == VLT_ENABLE ) || ( VLT_ENABLE_SCP03 == VLT_ENABLE ) ) */
    
}

#if(VLT_ENABLE_ISO7816 == VLT_ENABLE )
VLT_STS VltCommsCardEvent(VLT_PU8 pu8ReaderName, DWORD dwTimeout,PDWORD pdwEventState)
{
    VLT_STS status = VLT_FAIL;

    status = VltPtclCardEvent(pu8ReaderName,dwTimeout,pdwEventState);

    return( status );
}

VLT_STS VltCommsSelectCard(SCARDHANDLE hScard, SCARDCONTEXT hCxt, DWORD dwProtocol)
{
	 VLT_STS status = VLT_FAIL;

	 status = VltPtclSelectCard(hScard,hCxt,dwProtocol);

    return( status );
}
#endif
