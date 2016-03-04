/* --------------------------------------------------------------------------
* VaultIC API
* Copyright (C) Inside Secure, 2011. All Rights Reserved.
* -------------------------------------------------------------------------- */

#include "vaultic_common.h"
#include "vaultic_peripheral.h"
#include "aardvark_peripheral.h"
#include "IDTHWInf.h"
#include <stdio.h>

#if( VLT_PLATFORM == VLT_WINDOWS )
#pragma warning(disable : 4996)
#endif

/*
* Error Codes
*/
#define ETWISNDNULLPARAMS       VLT_ERROR( VLT_TWI, 0 )
#define ETWIRCVNULLPARAMS       VLT_ERROR( VLT_TWI, 1 )
#define ETWINOAARDVARK          VLT_ERROR( VLT_TWI, 2 )
#define ETWIAARDVARKNIUSE       VLT_ERROR( VLT_TWI, 3 )
#define ETWIINITOPEN            VLT_ERROR( VLT_TWI, 4 )
#define ETWIINITCONFIG          VLT_ERROR( VLT_TWI, 5 )
#define ETWISLAVEDISABLE        VLT_ERROR( VLT_TWI, 6 )
#define ETWIINITBITRATE         VLT_ERROR( VLT_TWI, 7 )
#define ETWIINITTGTPWRBOTH      VLT_ERROR( VLT_TWI, 8 )
#define ETWICLOSEPWRNONE        VLT_ERROR( VLT_TWI, 9 )
#define ETWICLOSEAARDCLS        VLT_ERROR( VLT_TWI, 10 )
#define ETWIINITFREEBUS         VLT_ERROR( VLT_TWI, 11 )
#define ETWIINITPULLUP          VLT_ERROR( VLT_TWI, 12 )
#define ETWISNDFAILED           VLT_ERROR( VLT_TWI, 13 )
#define ETWIRCVTRUNCATED        VLT_ERROR( VLT_TWI, 14 )
#define ETWIRCVFAILED           VLT_ERROR( VLT_TWI, 15 )
#define ETWIINITBUSTIMEOUT      VLT_ERROR( VLT_TWI, 16 )
#define ETWISNDTRUNCATED        VLT_ERROR( VLT_TWI, 17 )
#define ETWIUNSUPPIOCTLID       VLT_ERROR( VLT_TWI, 18 )
#define ETWIINITNULLPARAMS      VLT_ERROR( VLT_TWI, 19 )
#define ETWIINVLDSERNUM         VLT_ERROR( VLT_TWI, 20 )
#define ETWIUPDBITLNULLPTR      VLT_ERROR( VLT_TWI, 21 )
#define ETWISTBTRTNULLPARAMS    VLT_ERROR( VLT_TWI, 22 )
#define ETWIBUSWAKEUP           VLT_ERROR( VLT_TWI, 23 )
#define ETWISNDDELARRYNULL      VLT_ERROR( VLT_TWI, 24 )

/*
* Local functions
*/
#if( VLT_ENABLE_TWI == VLT_ENABLE )
static VLT_STS VltTwiPeripheralSetBitRate( VLT_PU16 pu16BitRate );
#endif

/*
* Private Data
*/
#if( VLT_ENABLE_TWI == VLT_ENABLE )
static int iHandle = 0;
VLT_TWI_PARAMS theTwiParams;
static VLT_BOOL useAardvark = FALSE;
#endif

VLT_STS VltTwiPeripheralInit( VLT_INIT_COMMS_PARAMS* pInitCommsParams )
{
#if( VLT_ENABLE_TWI == VLT_DISABLE )
	return( EMETHODNOTSUPPORTED );
#endif

#if( VLT_ENABLE_AARDVARK == VLT_ENABLE )
	if (pInitCommsParams != NULL &&
		pInitCommsParams->Params.VltBlockProtocolParams.VltPeripheralParams.u32SerialNo != 0)
	{
		useAardvark = TRUE;
	}
#endif

	if (useAardvark)
	{
		VLT_STS status = VLT_FAIL;
		VLT_U16 au16Dev[MAX_NO_OF_AARDVARKS];
		VLT_U32 au32Ids[MAX_NO_OF_AARDVARKS];
		VLT_U16 u16BusLockTimeout = 0;
		VLT_U32 u32NumFoundAardvarks = 0;
		VLT_U32 u32Idx = 0;

		/*
		* Validate the input parameter
		*/
		if( NULL == pInitCommsParams )
		{
			return ETWIINITNULLPARAMS;
		}

		/*
		* Store the TWI Parameters
		*/
		theTwiParams = 
			pInitCommsParams->Params.VltBlockProtocolParams.VltPeripheralParams.PeriphParams.VltTwiParams;

		/*
		* Check that an aardvark can be found
		*/
		if( 0 >= ( u32NumFoundAardvarks = aa_find_devices_ext( MAX_NO_OF_AARDVARKS, 
			&au16Dev[0], 
			MAX_NO_OF_AARDVARKS, 
			(u32*)&au32Ids[0] ) ) ) 
		{
			status =  ETWINOAARDVARK;
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
				status = ETWIINVLDSERNUM;
			}
		}

		if( VLT_OK == status )
		{
			if( AA_PORT_NOT_FREE == ( AA_PORT_NOT_FREE & au16Dev[u32Idx] ) )
			{
				status = ETWIAARDVARKNIUSE;
			}
			else
			{
				/*
				* Open the Aardvark adapter and get back a handle
				*/

				iHandle = aa_open( au16Dev[u32Idx] );

				if ( 0 > iHandle )
				{
					status = ETWIINITOPEN;
				}
			}
		}


		/*
		* Enter TWI Mode
		*/
		if( VLT_OK == status )
		{
			if ( AA_CONFIG_SPI_I2C != aa_configure( iHandle, AA_CONFIG_SPI_I2C ) )
			{
				status = ETWIINITCONFIG;
			}
		}

		/*
		* Pull up the resistors
		*/
		if( VLT_OK == status )
		{
			if( AA_INCOMPATIBLE_DEVICE ==
				aa_i2c_pullup( iHandle, AA_I2C_PULLUP_BOTH ) )
			{
				status = ETWIINITPULLUP;
			}
		}

		/*
		* Enable the target power
		*/
		if( VLT_OK == status )
		{
			if( AA_TARGET_POWER_BOTH != 
				aa_target_power( iHandle, AA_TARGET_POWER_BOTH ) )
			{
				status = ETWIINITTGTPWRBOTH;
			}
		}

		/*
		* Select master mode
		*/
		if( VLT_OK == status )
		{
			if( AA_OK != aa_i2c_slave_disable( iHandle ) )
			{
				status = ETWISLAVEDISABLE;
			}
		}

		/*
		* Set bitrate
		*/
		if( VLT_OK == status )
		{
			VLT_U16 u16BitRate = DEFAULT_BIT_RATE;
			status = VltTwiPeripheralSetBitRate( &u16BitRate );
		}

		/*
		* Set the Bus Lock Timeout
		*/
		if( VLT_OK == status )
		{
			u16BusLockTimeout = aa_i2c_bus_timeout( iHandle, theTwiParams.u16BusTimeout );

			if( theTwiParams.u16BusTimeout != u16BusLockTimeout )
			{
				status = ETWIINITBUSTIMEOUT;
			}

		}

		if( VLT_OK != status )
		{
			/*
			* Close the aardvark adapter
			*/
			aa_close( iHandle );
		}

		return( status );
	}
	else
	{
		int cartridgesCount;
		int status = IdtInit(&cartridgesCount);
		return (status == TRUE) ? VLT_OK : ETWIINITCONFIG;
	}
}

VLT_STS VltTwiPeripheralClose( void )
{
#if( VLT_ENABLE_TWI == VLT_DISABLE )
	return( EMETHODNOTSUPPORTED );
#endif

	if (useAardvark)
	{
		VLT_STS status = VLT_FAIL;

		/*
		* Disable the target power pins
		*/
		if( AA_INCOMPATIBLE_DEVICE == 
			aa_target_power( iHandle, AA_TARGET_POWER_NONE ) )
		{
			status = ETWICLOSEPWRNONE;
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
			status = ETWICLOSEAARDCLS;
		}

		return ( status );
	}
	else
	{
		int status = IdtClose();
		return (status == TRUE) ? VLT_OK : ETWICLOSEPWRNONE;
	}
}

VLT_STS VltTwiPeripheralIoctl( VLT_U32 u32Id, void* pConfigData )
{
#if( VLT_ENABLE_TWI == VLT_DISABLE )
	return( EMETHODNOTSUPPORTED );
#endif

	if (useAardvark)
	{
		VLT_STS status = VLT_FAIL;
		VLT_U16 u16BitRate = DEFAULT_BIT_RATE;

		switch( u32Id )
		{
		case VLT_AWAIT_DATA:
			/*
			* No need to do anything
			*/
			status = VLT_OK;
			break;

		case VLT_RESET_PROTOCOL:
			status = VltTwiPeripheralSetBitRate( (VLT_U16*)&u16BitRate );
			break;

		case VLT_UPDATE_BITRATE:
			if( NULL == pConfigData )
			{
				status = ETWIUPDBITLNULLPTR;
			}
			else
			{
				status = VltTwiPeripheralSetBitRate( (VLT_U16*)pConfigData );
			}

			break;

		default:
			status = ETWIUNSUPPIOCTLID;
			break;
		}

		return( status );
	}
	else
	{
		return VLT_OK;
	}
}


VLT_STS VltTwiPeripheralSendData( VLT_MEM_BLOB *pOutData,
								 VLT_U8 u8DelayArraySz,
								 VLT_DELAY_PAIRING* pDelayPairing)
{
#if( VLT_ENABLE_TWI == VLT_DISABLE )
	return( EMETHODNOTSUPPORTED );
#endif

	if (useAardvark)
	{
		VLT_STS status = VLT_FAIL;
		VLT_U16 u16BytesWritten = 0;
		int iRet = AA_OK;

		/*
		* Check the input parameter is valid
		*/
		if( NULL == pOutData )
		{
			return ETWISNDNULLPARAMS;
		}

		if( ( 0 != u8DelayArraySz) && ( NULL == pDelayPairing ) )
		{
			return ETWISNDDELARRYNULL;
		}

		/*
		* Wakeup the bus
		*/
		iRet = aa_i2c_read( iHandle,
			0x00,
			AA_I2C_NO_STOP,
			0x00,
			NULL );

		if( 0 != u8DelayArraySz )
		{
			/*
			* Delay before sending the bytes.  Only one delay can be added by the 
			* aardvark so it will be the first in the array passed in
			*/
			aa_sleep_ms( pDelayPairing[0].u32DelayTime / VLT_MICRO_SECS_IN_MSEC );
		}

		if( AA_OK == iRet )
		{  
			/*
			* Write command
			*/
			iRet = aa_i2c_write_ext( iHandle,
				theTwiParams.u8Address,
				AA_I2C_NO_FLAGS,
				pOutData->u16Len,
				&(pOutData->pu8Data[0]),
				&u16BytesWritten);

			if( AA_I2C_WRITE_ERROR == iRet )
			{
				status = ETWISNDFAILED;
			}
			else if( u16BytesWritten != pOutData->u16Len )
			{
				status = ETWISNDTRUNCATED;
			}
			else
			{
				status = VLT_OK;
			}
		}
		else
		{
			status = ETWIBUSWAKEUP;
		}

		return( status );
	}
	else
	{
		int status = VLT_FAIL;
		if (pOutData == NULL)
		{
			return ETWISNDNULLPARAMS;
		}

		status = IdtSendData(pOutData->u16Len, (char*)pOutData->pu8Data);
		return (status == TRUE) ? VLT_OK : ETWISNDFAILED;
	}
}

VLT_STS VltTwiPeripheralReceiveData( VLT_MEM_BLOB *pInData )
{ 
#if( VLT_ENABLE_TWI == VLT_DISABLE )
	return( EMETHODNOTSUPPORTED );
#endif

	if (useAardvark)
	{
		VLT_STS status = VLT_FAIL;
		VLT_U16 u16BytesRead = 0;
		int iRet = AA_OK;
#if (VAULT_IC_TARGET == VAULTIC100)
		VLT_U32 nCount = 0;
#endif

		/*
		* Check the input parameter is valid
		*/
		if( NULL == pInData )
		{
			return ETWIRCVNULLPARAMS;
		}


#if (VAULT_IC_TARGET == VAULTIC100)

		/*
		// VaultIC100 will NACK read requests until data is available
		*/
		for (;;)
		{
			iRet = aa_i2c_read_ext( iHandle,
				theTwiParams.u8Address,
				AA_I2C_NO_FLAGS,
				pInData->u16Len,
				&(pInData->pu8Data[0]),
				&u16BytesRead );

			if (AA_OK == iRet)
				break;

			/* check for timeout expiry */
			nCount++;
			aa_sleep_ms(1);
			aa_i2c_free_bus( iHandle );
			if (nCount > theTwiParams.u32msTimeout)
			{
				return ETWIRCVFAILED;
			}
		}

#else 
		iRet = aa_i2c_read_ext( iHandle,
			theTwiParams.u8Address,
			AA_I2C_NO_FLAGS,
			pInData->u16Len,
			&(pInData->pu8Data[0]),
			&u16BytesRead );

		/* 
		* VaultIC460 will hold SCL (clock) low until data is available 
		*/
		if( AA_OK != iRet )
		{
			iRet = aa_i2c_free_bus( iHandle );

			/*
			* First attempt at the read failed.  Sleep
			* before retrying.
			*/
			aa_sleep_ms( theTwiParams.u32msTimeout );

			iRet = aa_i2c_read_ext( iHandle,
				theTwiParams.u8Address,
				AA_I2C_NO_FLAGS,
				pInData->u16Len,
				&(pInData->pu8Data[0]),
				&u16BytesRead );
		}
#endif

		/*
		* Check that the read was OK
		*/
		if( AA_OK != iRet ) 
		{
			status = ETWIRCVFAILED;
		}
		else if ( pInData->u16Len != u16BytesRead )
		{
			status = ETWIRCVTRUNCATED;
		}
		else
		{
			status = VLT_OK;
		}

		return( status);
	}
	else
	{
		int status = VLT_FAIL;
		if (pInData == NULL)
		{
			return ETWIRCVNULLPARAMS;
		}

		status = IdtReceiveData(pInData->u16Len, (char*)pInData->pu8Data);
		return (status == TRUE) ? VLT_OK : ETWIRCVFAILED;
	}
}

#if( VLT_ENABLE_TWI == VLT_ENABLE )
static VLT_STS VltTwiPeripheralSetBitRate( VLT_PU16 pu16BitRate )
{
	/*
	* Check the pointer
	*/
	if( NULL == pu16BitRate )
	{
		return( ETWISTBTRTNULLPARAMS );
	}

	/*
	* Set bitrate
	*/
	*pu16BitRate = aa_i2c_bitrate( iHandle, *pu16BitRate );

	return ( VLT_OK );
}
#endif /* ( VLT_ENABLE_TWI == VLT_ENABLE ) */



