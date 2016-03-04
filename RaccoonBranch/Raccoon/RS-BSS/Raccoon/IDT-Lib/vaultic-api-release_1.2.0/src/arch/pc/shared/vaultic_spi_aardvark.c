/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#include "vaultic_common.h"
#include "vaultic_spi_aardvark.h"
#include "aardvark_peripheral.h"
#include "vaultic_timer_delay.h"
/*
* Defines
*/
#define SPI_POLLING_BYTE      (VLT_U8)0xC0

/*
* External variables
*/
#if( VLT_ENABLE_SPI == VLT_ENABLE )
extern int iHandle;
extern VLT_U8 bFirstReceive;
extern VLT_SPI_PARAMS theSpiParams;
#endif /* #if( VLT_ENABLE_SPI == VLT_ENABLE ) */

/*
* Local functions
*/
#if( VLT_ENABLE_SPI == VLT_ENABLE )
    static VLT_STS VltSpiAardvarkAwaitData( void );
    static VLT_STS VltSpiAardvarkSetBitRate( VLT_PU16 pu16BitRate );
#endif

VLT_STS VltSpiAardvarkInit( VLT_INIT_COMMS_PARAMS* pInitCommsParams )
{
#if( VLT_ENABLE_SPI == VLT_ENABLE )
    VLT_STS status = VLT_FAIL;
    VLT_U16 au16Dev[MAX_NO_OF_AARDVARKS];
    VLT_U32 au32Ids[MAX_NO_OF_AARDVARKS];
    VLT_U32 u32NumFoundAardvarks = 0;
    VLT_U32 u32Idx = 0;

    /*
    * Validate the input parameters
    */
    if( NULL == pInitCommsParams )
    {
        return EAARINITNULLPARAMS;
    }

    /*
    * Check that an aardvark can be found
    */
    if( 0 >= ( u32NumFoundAardvarks = aa_find_devices_ext( MAX_NO_OF_AARDVARKS, 
        &au16Dev[0], 
        MAX_NO_OF_AARDVARKS, 
        (u32*)&au32Ids[0] ) ) )
    {
        status = EAARNOAARDVARK;
    }
    else if(MAX_NO_OF_AARDVARKS < u32NumFoundAardvarks)
    {
        status = EAARTOOMANYAARDV;
    }
    else
    {
        status = VLT_OK;
    }

    if( VLT_OK == status )
    {
        for( u32Idx = 0; u32Idx < u32NumFoundAardvarks; u32Idx++ )
        {
            if( pInitCommsParams->Params.VltBlockProtocolParams.VltPeripheralParams.u32SerialNo ==
                au32Ids[u32Idx] )
            {
                break;
            }
        }

        /*
        * If the index is equal to the number of aardvarks found we didn't get
        * a match on the serial number so set the appropriate status
        */
        if( u32Idx == u32NumFoundAardvarks )
        {
            status = EAARINVLDSERNUM;
        }
    }

    if( VLT_OK == status )
    {

        if( AA_PORT_NOT_FREE == ( AA_PORT_NOT_FREE & au16Dev[u32Idx] ) )
        {
            status = EAARDVARKINUSE;
        }
        else
        {
            /*
            * Open the Aardvark adapter and get back a handle
            */
            iHandle = aa_open( au16Dev[u32Idx] );
            
            if ( 0 > iHandle )
            {
                status = EAARINITOPEN;
            }
        }
    }

	//Like the manager
	aa_i2c_pullup(iHandle, (unsigned char)0);
	aa_gpio_pullup(iHandle, (unsigned char)0);
	aa_target_power(iHandle, AA_TARGET_POWER_NONE);
	aa_configure(iHandle, AA_CONFIG_GPIO_ONLY);
    aa_gpio_set(iHandle,(unsigned char)0x3F);
    aa_gpio_direction(iHandle,(unsigned char)0);

	aa_gpio_direction(iHandle, AA_GPIO_SDA);
	aa_gpio_set(iHandle,(aa_gpio_get(iHandle)&(~AA_GPIO_SDA)));

	/*
    * Set the target power
    */
    if( VLT_OK == status )
    {
		u08 u8targetPowerResult = aa_target_power( iHandle, AA_TARGET_POWER_BOTH );
        if ( AA_TARGET_POWER_BOTH != u8targetPowerResult )
        {
            status = EAARINITTGTPWRBOTH;
        }
    }

	/*
	* Wait for aardvark initialization
	*/
	if( VLT_OK == status )
    {
		VltSleep(500000);
	}
	
	if (aa_gpio_direction(iHandle, 0x00) != AA_OK)
	{
		status = EAARINITCONFIGSPI;
	}

	if( VLT_OK == status )
    {
		if (aa_configure(iHandle, AA_CONFIG_SPI_GPIO) != AA_CONFIG_SPI_GPIO)
		{
            status = EAARINITCONFIGSPI;
        }
	}

	if( VLT_OK == status )
	{
        if ( AA_OK != aa_spi_configure( iHandle, 
            AA_SPI_POL_RISING_FALLING, 
            AA_SPI_PHASE_SAMPLE_SETUP,
            AA_SPI_BITORDER_MSB ) )
        {
            status = EAARINITCONFIGSPI;
        }
    }

    /*
    * Select master mode
    */
    if( VLT_OK == status )
    {
        if ( AA_OK != aa_spi_slave_disable( iHandle ) )
        {
            status = EAARSLAVEDISABLE;
        }
    }
    
    /*
    * Set bitrate
    */
    if( VLT_OK == status )
    {
        VLT_U16 u16BitRate = DEFAULT_BIT_RATE;
        status = VltSpiAardvarkSetBitRate ( &u16BitRate );
    }

    if( VLT_OK != status )
    {
        /*
        * Close the aardvark adapter
        */
        aa_close( iHandle );
    }
    return( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}

VLT_STS VltSpiAardvarkClose( void )
{
#if( VLT_ENABLE_SPI == VLT_ENABLE )
    VLT_STS status = VLT_FAIL;

    /*
    * Disable the target power pins
    */
    if( AA_TARGET_POWER_NONE != 
        aa_target_power( iHandle, AA_TARGET_POWER_NONE ) )
    {
        status = EAARCLOSEPWRNONE;
    }
    else
    {
        status = VLT_OK;
    }

    /*
    * Close the aardvark adapter
    */
    if( 1 != aa_close( iHandle ) )
    {
        status = EAARCLOSEAARDCLS;
    }

    return ( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}

VLT_STS VltSpiAardvarkIoctl( VLT_U32 u32Id, void* pConfigData )
{
#if( VLT_ENABLE_SPI == VLT_ENABLE )
    VLT_STS status = VLT_FAIL;
    VLT_U16 u16BitRate = DEFAULT_BIT_RATE;

    switch( u32Id )
    {
    case VLT_AWAIT_DATA:
        status = VltSpiAardvarkAwaitData( );
        break;
        
    case VLT_RESET_PROTOCOL:
        status = VltSpiAardvarkSetBitRate( (VLT_U16*)&u16BitRate );
        break;

        case VLT_UPDATE_BITRATE:
        if( NULL == pConfigData )
        {
            status = EAARUPDBITLNULLPTR;
        }
        else
        {
            status = VltSpiAardvarkSetBitRate( (VLT_U16*)pConfigData );
        }
        
        break;
        
    default:
        status = EAARUNSUPPIOCTLID;
        break;
    }

    return( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}

VLT_STS VltSpiAardvarkSendData( VLT_MEM_BLOB *pOutData,
    VLT_U8 u8DelayArraySz,
    VLT_DELAY_PAIRING* pDelayPairing )
{
#if( VLT_ENABLE_SPI == VLT_ENABLE )
    VLT_STS status = VLT_FAIL;
    int iRet = 0;

    /*
    * Check the input parameter is valid
    */
    if( NULL == pOutData )
    {
        return EAARSNDNULLPARAMS;
    }

    if( ( 0 != u8DelayArraySz) && ( NULL == pDelayPairing ) )
    {
        return EAARSNDDELARRYNULL;
    }

    if( 0 != u8DelayArraySz )
    {
        /*
        * Delay before sending the bytes.  Only one delay can be added by the 
        * aardvark so it will be the first in the array passed in
        */
        aa_sleep_ms( pDelayPairing[0].u32DelayTime / VLT_MICRO_SECS_IN_MSEC );
    }
	
    /*
    * Send the Data
    */
    iRet = aa_spi_write( iHandle,
        pOutData->u16Len,
        &(pOutData->pu8Data[0]),
        0,
        &(pOutData->pu8Data[0]) );

    /*
    * Check that the send happened
    */    
    if( AA_SPI_WRITE_ERROR == iRet )
    {
#if( VLT_ENABLE_AARDVK_SPPRSS_ERR == VLT_ENABLE )
        /*
        * Flag that a call to Receive will be the first since the send
        */
        bFirstReceive = TRUE;

        status = VLT_OK;
#else /*( VLT_ENABLE_AARDVK_SPPRSS_ERR == VLT_DISABLE ) */
        status = EAARDSPIWRITE;
#endif /* ( VLT_ENABLE_AARDVK_SPPRSS_ERR == VLT_ENABLE ) */        
    }
    else if( pOutData->u16Len != iRet )
    {
        status = EAARSNDFAIL;
    }
    else
    {
        /*
        * Flag that a call to Receive will be the first since the send
        */
        bFirstReceive = TRUE;

        status = VLT_OK;
    }

    return( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}

VLT_STS VltSpiAardvarkReceiveData( VLT_MEM_BLOB *pInData )
{
#if( VLT_ENABLE_SPI == VLT_ENABLE )
    VLT_STS status = VLT_FAIL;
    int iRet = 0;
    VLT_U32 u32ReadAttempts = 0;
    VLT_U16 u16Len = pInData->u16Len;
    VLT_PU8 pu8DataPos = &(pInData->pu8Data[0]);

    /*
    * Check the input parameters are valid
    */
    if( NULL == pInData )
    {
        return EAARRCVNULLPARAMS;
    }

    if( 0 == u16Len )
    {
        /*
        * No need to attempt to get data as none is being requested
        */
        status = VLT_OK;
    }
    else if( pInData->u16Len > pInData->u16Capacity )
    {
        status = EAARRCVCAPTOOLOW;
    }
    else
    {
        if( bFirstReceive )
        {
            /*
            * Poll the SPI until we stop getting 0xC0 or we have tried
            * too many times
            */
            *pu8DataPos = SPI_POLLING_BYTE;

            /*
            * Sleep before attempting to get first poll byte
            */
            aa_sleep_ms( theSpiParams.u32FstPollByteDelay / VLT_MICRO_SECS_IN_MSEC );

            do
            {
                iRet = aa_spi_write( iHandle,
                            1,
                            pu8DataPos,
                            1,
                            pu8DataPos );

                u32ReadAttempts++;

                /*
                * Sleep briefly before reading again
                */
                aa_sleep_ms( theSpiParams.u32IntPollByteDelay / VLT_MICRO_SECS_IN_MSEC );

            }while( ( theSpiParams.u32PollMaxRetries > u32ReadAttempts ) && 
                ( SPI_POLLING_BYTE == *pu8DataPos ) );

            /*
            * Check if the read was successful
            */
            if( theSpiParams.u32PollMaxRetries == u32ReadAttempts)
            {
                status = EAARREADATTMPTS;
            }
            else
            {
                /*
                * Successfully recieved the first byte of data.  Update the 
                * position in the buffer where the remaining data should be
                * placed and reduce the length of the read by 1.  Also clear
                * the bFirstReceive flag so that further receives before 
                * another send won't poll for the padding byte
                */
                pu8DataPos++;
                u16Len--;
                bFirstReceive = FALSE;
                status = VLT_OK;
            }
        }
        else
        {
            status = VLT_OK;
        }

        if( VLT_OK == status )
        {
            /*
            * Receive the data
            */
            iRet = aa_spi_write( iHandle,
                    u16Len,
                    pu8DataPos,
                    u16Len,
                    pu8DataPos );

            if( u16Len != iRet )
            {
                /*
                * There was a problem receiving the data
                */
                status = EAARRCVFAIL;
            }
        }
    }

    return( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}

#if( VLT_ENABLE_SPI == VLT_ENABLE )
static VLT_STS VltSpiAardvarkAwaitData( void )
{
    /*
    * Set the flag to poll for data past the padding byte
    */
    bFirstReceive = TRUE;

    return ( VLT_OK );
}

static VLT_STS VltSpiAardvarkSetBitRate( VLT_PU16 pu16BitRate )
{
    /*
    * Check the pointer
    */
    if( NULL == pu16BitRate )
    {
        return( EAADSTBTRTNULLPARAMS );
    }

    /*
    * Set bitrate
    */
    *pu16BitRate = aa_spi_bitrate( iHandle, *pu16BitRate );

    return ( VLT_OK );
}
#endif /* ( VLT_ENABLE_SPI == VLT_ENABLE ) */

