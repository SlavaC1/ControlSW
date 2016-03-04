/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#include "vaultic_common.h"
#include "vaultic_peripheral.h"
#include <stdio.h>
#if(VLT_ENABLE_AARDVARK == VLT_ENABLE )
#include "vaultic_spi_aardvark.h"
#include "aardvark.h"
#endif
#if(VLT_ENABLE_CHEETAH == VLT_ENABLE )
#include "vaultic_spi_cheetah.h"
#endif

/*
* Error Codes
*/
#define ESPIINITNULLPARAMS   VLT_ERROR( VLT_SPI, 0 )
#define ESPISNDNULLPARAMS    VLT_ERROR( VLT_SPI, 1 )
#define ESPIRCVNULLPARAMS    VLT_ERROR( VLT_SPI, 2 )
#define ESPIUNSUPPADAPT      VLT_ERROR( VLT_SPI, 3 )

/*
* Private Data
*/
#if( VLT_ENABLE_SPI == VLT_ENABLE )
    int iHandle = 0;
    VLT_U8 bFirstReceive = TRUE;
    VLT_SPI_PARAMS theSpiParams;

    static VltPeripheral theSpi = 
    {
        0,
        0,
        0,
        0,
        0,
    };
#endif

VLT_STS VltSpiPeripheralInit( VLT_INIT_COMMS_PARAMS* pInitCommsParams )
{
#if( VLT_ENABLE_SPI == VLT_ENABLE )
    VLT_STS status = VLT_FAIL;

    /*
    * Check that the input parameters are valid
    */
    if( NULL == pInitCommsParams )
    {
        return ESPIINITNULLPARAMS;
    }

    /*
    * Setup the function pointers
    */
    switch( pInitCommsParams->Params.VltBlockProtocolParams.VltPeripheralParams.PeriphParams.VltSpiParams.u8AdapterType )
    {
#if(VLT_ENABLE_AARDVARK == VLT_ENABLE )
    case VLT_SPI_AARDVARK_ADAPTER:
        theSpi.PeripheralInit = VltSpiAardvarkInit;
        theSpi.PeripheralClose = VltSpiAardvarkClose;
        theSpi.PeripheralIoctl = VltSpiAardvarkIoctl;
        theSpi.PeripheralSendData = VltSpiAardvarkSendData;
        theSpi.PeripheralReceiveData = VltSpiAardvarkReceiveData;
        status = VLT_OK;
        break;
#endif
#if(VLT_ENABLE_CHEETAH == VLT_ENABLE )
    case VLT_SPI_CHEETAH_ADAPTER:
        theSpi.PeripheralInit  = VltSpiCheetahInit;
        theSpi.PeripheralClose= VltSpiCheetahClose;
        theSpi.PeripheralIoctl = VltSpiCheetahIoctl;
        theSpi.PeripheralSendData = VltSpiCheetahSendData;
        theSpi.PeripheralReceiveData = VltSpiCheetahReceiveData;
        status = VLT_OK;
        break;
#endif
    default:
        status = ESPIUNSUPPADAPT;
        break;
    }


    if( VLT_OK == status )
    {
        /*
        * Store the SPI params
        */
        theSpiParams = pInitCommsParams->Params.VltBlockProtocolParams.VltPeripheralParams.PeriphParams.VltSpiParams;

        /*
        * Delegate the call to the appropriate adapter
        */
        status = theSpi.PeripheralInit( pInitCommsParams );
    }

    return( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}

VLT_STS VltSpiPeripheralClose( void )
{
#if( VLT_ENABLE_SPI == VLT_ENABLE )
    return theSpi.PeripheralClose( );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}

VLT_STS VltSpiPeripheralSendData( VLT_MEM_BLOB *pOutData,
    VLT_U8 u8DelayArraySz,
    VLT_DELAY_PAIRING* pDelayPairing)
{
#if( VLT_ENABLE_SPI == VLT_ENABLE )
    /*
    * Check the input parameter is valid
    */
    if( NULL == pOutData )
    {
        return ESPISNDNULLPARAMS;
    }

    /*
    * Delegate the call to the appropriate adapter
    */
    return theSpi.PeripheralSendData( pOutData, u8DelayArraySz, pDelayPairing );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}

VLT_STS VltSpiPeripheralReceiveData( VLT_MEM_BLOB *pInData )
{
#if( VLT_ENABLE_SPI == VLT_ENABLE )
    /*
    * Check the input parameters are valid
    */
    if( NULL == pInData )
    {
        return ESPIRCVNULLPARAMS;
    }

    /*
    * Delegate the call to the appropriate adapter
    */
    return theSpi.PeripheralReceiveData( pInData );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}

VLT_STS VltSpiPeripheralIoctl( VLT_U32 u32Id, void* pConfigData )
{
#if( VLT_ENABLE_SPI == VLT_ENABLE )
    /*
    * Delegate the call to the appropriate adapter
    */
    return theSpi.PeripheralIoctl( u32Id, pConfigData );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}

