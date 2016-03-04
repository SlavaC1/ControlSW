/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#include "vaultic_common.h"
#include "vaultic_spi_cheetah.h"
#include "cheetah.h"
#include <stdio.h>

#if( VLT_PLATFORM == VLT_WINDOWS )
    #pragma warning(disable : 4996)
#endif

/*
* Error Codes
*/
#define ECHEINITNULLPARAMS   VLT_ERROR( VLT_CHEETAH, 0 )
#define ECHESNDNULLPARAMS    VLT_ERROR( VLT_CHEETAH, 1 )
#define ECHERCVNULLPARAMS    VLT_ERROR( VLT_CHEETAH, 2 )
#define ECHENOCHEETAH        VLT_ERROR( VLT_CHEETAH, 3 )
#define ECHEINVLDSERNUM      VLT_ERROR( VLT_CHEETAH, 4 )
#define ECHECHEETAHINUSE     VLT_ERROR( VLT_CHEETAH, 5 )
#define ECHEINITOPEN         VLT_ERROR( VLT_CHEETAH, 6 )
#define ECHECLOSECHEETCLS    VLT_ERROR( VLT_CHEETAH, 7 )
#define ECHEINITTGTPWRBOTH   VLT_ERROR( VLT_CHEETAH, 8 )
#define ECHETRANSCLEARQUEUE  VLT_ERROR( VLT_CHEETAH, 9 )
#define ECHETRANSOUTENABLE   VLT_ERROR( VLT_CHEETAH, 10 )
#define ECHETRANSSLVSLCTEN   VLT_ERROR( VLT_CHEETAH, 11 )
#define ECHETRANSSLVSLCTDIS  VLT_ERROR( VLT_CHEETAH, 12 )
#define ECHETRANSOUTDISABLE  VLT_ERROR( VLT_CHEETAH, 13 )
#define ECHETRANSBATCHSHIFT  VLT_ERROR( VLT_CHEETAH, 14 )
#define ECHEINITCONFIG       VLT_ERROR( VLT_CHEETAH, 15 )
#define ECHETRANSQUEUEBYTE   VLT_ERROR( VLT_CHEETAH, 16 )
#define ECHERCVREADATTMPTS   VLT_ERROR( VLT_CHEETAH, 17 )
#define ECHERCVCAPTOOLOW     VLT_ERROR( VLT_CHEETAH, 18 )
#define ECHEUPDBITLNULLPTR   VLT_ERROR( VLT_CHEETAH, 19 )
#define ECHEUNSUPPIOCTLID    VLT_ERROR( VLT_CHEETAH, 20 )
#define ECHETOOMANYCHEET     VLT_ERROR( VLT_CHEETAH, 21 )
#define EDTCHERSIVLDPARAM    VLT_ERROR( VLT_CHEETAH, 22 )
#define EDTCHERSIVLDSTR      VLT_ERROR( VLT_CHEETAH, 23 )
#define EDTCHDRSEXDMXM       VLT_ERROR( VLT_CHEETAH, 24 )
#define EDTCHDRSINSFLEN      VLT_ERROR( VLT_CHEETAH, 25 )
#define ECHESTBTRTNULLPARAMS VLT_ERROR( VLT_CHEETAH, 26 )
#define ECHESNDDELARRYNULL   VLT_ERROR( VLT_CHEETAH, 27 )
#define ECHETRANSDATANULL    VLT_ERROR( VLT_CHEETAH, 28 )
#define ECHECLOSEPWRNONE     VLT_ERROR( VLT_CHEETAH, 29 )

/*
* Defines
*/
#define SPI_POLLING_BYTE      (VLT_U8)0xC0
#define MAX_NO_OF_CHEETAHS    (VLT_U8)0x0A
#define DEFAULT_BIT_RATE      (VLT_U8)125
#define NANO_SECS_IN_USEC     (VLT_U32)1000
#define XML_CH_HEADER_SZ      (VLT_U8)26
#define XML_CH_DATA_SZ        (VLT_U8)34
#define XML_CH_FOOTER_SZ      (VLT_U8)12

#define WRITE_MODE            (VLT_U8)0x00
#define READ_MODE             (VLT_U8)0x01

/*
* External variables
*/
extern int iHandle;
extern VLT_U8 bFirstReceive;
extern VLT_SPI_PARAMS theSpiParams;

/*
* Local functions
*/
#if( VLT_ENABLE_SPI == VLT_ENABLE )
    static VLT_STS VltSpiCheetahAwaitData( void );

    static VLT_STS VltSpiCheetahSetBitRate( VLT_PU16 pu16BitRate );

    static VLT_STS VltSpiCheetahDataTransfer( VLT_U8 u8Mode,
        VLT_U16 u16Len,
        VLT_PU8 pu8Data,
        VLT_U8 u8DelayArraySz,
        VLT_DELAY_PAIRING* pDelayPairing);
#endif

VLT_STS VltSpiCheetahInit( VLT_INIT_COMMS_PARAMS* pInitCommsParams )
{
#if( VLT_ENABLE_SPI == VLT_ENABLE )
    VLT_STS status = VLT_FAIL;
    VLT_U16 au16Dev[MAX_NO_OF_CHEETAHS];
    VLT_U32 au32Ids[MAX_NO_OF_CHEETAHS];
    VLT_U32 u32NumFoundCheetahs = 0;
    VLT_U32 u32Idx = 0;

    /*
    * Validate the input parameters
    */
    if( NULL == pInitCommsParams )
    {
        return ECHEINITNULLPARAMS;
    }

    /*
    * Check that a cheetah can be found
    */
    if( 0 >= ( u32NumFoundCheetahs = ch_find_devices_ext( MAX_NO_OF_CHEETAHS, 
        &au16Dev[0], 
        MAX_NO_OF_CHEETAHS, 
        (u32*)&au32Ids[0] ) ) )
    {
        status = ECHENOCHEETAH;
    }
    else if(MAX_NO_OF_CHEETAHS < u32NumFoundCheetahs)
    {
        status = ECHETOOMANYCHEET;
    }
    else
    {
        status = VLT_OK;
    }

    if( VLT_OK == status )
    {
        for( u32Idx = 0; u32Idx < u32NumFoundCheetahs; u32Idx++ )
        {
            if( pInitCommsParams->Params.VltBlockProtocolParams.VltPeripheralParams.u32SerialNo ==
                au32Ids[u32Idx] )
            {
                break;
            }
        }

        /*
        * If the index is equal to the number of cheetahs found we didn't get
        * a match on the serial number so set the appropriate status
        */
        if( u32Idx == u32NumFoundCheetahs )
        {
            status = ECHEINVLDSERNUM;
        }
    }

    /*
    * Open the Cheetah
    */
    if( VLT_OK == status )
    {
        if( CH_PORT_NOT_FREE == ( CH_PORT_NOT_FREE & au16Dev[u32Idx] ) )
        {
            status = ECHECHEETAHINUSE;
        }
        else
        {
            /*
            * Open the Cheetah adapter and get back a handle
            */
            iHandle = ch_open( au16Dev[u32Idx] );
            
            if ( 0 > iHandle )
            {
                status = ECHEINITOPEN;
            }
        }
    }

    /*
    * Set the target power
    */
    if( VLT_OK == status )
    {
        if ( CH_TARGET_POWER_ON != ch_target_power( iHandle, CH_TARGET_POWER_ON ) )
        {
            status = ECHEINITTGTPWRBOTH;
        }
    }

    /*
    * Set bitrate
    */
    if( VLT_OK == status )
    {
        VLT_U16 u16BitRate = DEFAULT_BIT_RATE;
        status = VltSpiCheetahSetBitRate ( &u16BitRate );
    }

    /*
    * Configure the polarity, phase, endianess and slave select polarity
    */
    if( VLT_OK == status )
    {
        if( CH_OK != ch_spi_configure( iHandle, 
            CH_SPI_POL_RISING_FALLING, 
            CH_SPI_PHASE_SAMPLE_SETUP,
            CH_SPI_BITORDER_MSB, 
            0 ) )
        {
            status = ECHEINITCONFIG;
        }
    }

    if( VLT_OK != status )
    {
        /*
        * Close the Cheetah adapter
        */
        ch_close( iHandle );
    }

    return( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}

VLT_STS VltSpiCheetahClose( void )
{
#if( VLT_ENABLE_SPI == VLT_ENABLE )
    VLT_STS status = VLT_FAIL;

    
    /*
    * Disable the target power pins
    */
    if( CH_TARGET_POWER_OFF != 
        ch_target_power( iHandle, CH_TARGET_POWER_OFF ) )
    {
        status = ECHECLOSEPWRNONE;
    }
    else
    {
        status = VLT_OK;
    }

    
    /*
    * Close the cheetah adapter
    */
    if( 1 != ch_close( iHandle ) )
    {
        status = ECHECLOSECHEETCLS;
    }
    else
    {
        status = VLT_OK;
    }

    return ( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}

VLT_STS VltSpiCheetahIoctl( VLT_U32 u32Id, void* pConfigData )
{
#if( VLT_ENABLE_SPI == VLT_ENABLE )
    VLT_STS status = VLT_FAIL;
    VLT_U16 u16BitRate = DEFAULT_BIT_RATE;

    switch( u32Id )
    {
    case VLT_AWAIT_DATA:
        status = VltSpiCheetahAwaitData( );
        break;
        
    case VLT_RESET_PROTOCOL:
        status = VltSpiCheetahSetBitRate( (VLT_U16*)&u16BitRate );
        break;

        case VLT_UPDATE_BITRATE:
        if( NULL == pConfigData )
        {
            status = ECHEUPDBITLNULLPTR;
        }
        else
        {
            status = VltSpiCheetahSetBitRate( (VLT_U16*)pConfigData );
        }
        
        break;
        
    default:
        status = ECHEUNSUPPIOCTLID;
        break;
    }

    return( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}

VLT_STS VltSpiCheetahSendData( VLT_MEM_BLOB *pOutData,
    VLT_U8 u8DelayArraySz,
    VLT_DELAY_PAIRING* pDelayPairing )
{
#if( VLT_ENABLE_SPI == VLT_ENABLE )
    VLT_STS status = VLT_FAIL;

    /*
    * Validate the input parameters
    */
    if( NULL == pOutData )
    {
        return ECHESNDNULLPARAMS;
    }

    if( ( 0 != u8DelayArraySz) && ( NULL == pDelayPairing ) )
    {
        return ECHESNDDELARRYNULL;
    }

    status = VltSpiCheetahDataTransfer( WRITE_MODE,
        pOutData->u16Len, 
        pOutData->pu8Data,
        u8DelayArraySz,
        pDelayPairing );

    if( VLT_OK == status )
    {
        /*
        * Flag that a call to Receive will be the first since the send
        */
        bFirstReceive = TRUE;
    }
    
    return( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}

VLT_STS VltSpiCheetahReceiveData( VLT_MEM_BLOB *pInData )
{
#if( VLT_ENABLE_SPI == VLT_ENABLE )
    VLT_STS status = VLT_FAIL;
    VLT_U32 u32ReadAttempts = 0;
    VLT_U16 u16Len = pInData->u16Len;
    VLT_PU8 pu8DataPos = &(pInData->pu8Data[0]);
    VLT_DELAY_PAIRING delayPairing;

    /*
    * Validate the input parameters
    */
    if( NULL == pInData )
    {
        return ECHERCVNULLPARAMS;
    }

    if( pInData->u16Len > pInData->u16Capacity )
    {
        status = ECHERCVCAPTOOLOW;
    }
    else
    {
        status = VLT_OK;
    }

    if( VLT_OK == status)
    {
        /*
        * If this is the first receive since a send need to check for
        * polling bytes
        */
        if( TRUE == bFirstReceive )
        {
            /*
            * Poll the SPI until we stop getting 0xC0 or we have tried
            * too many times
            */
            *(pu8DataPos) = SPI_POLLING_BYTE;

            /*
            * Sleep before attempting to get first poll byte
            */
            delayPairing.u16BytePos = 0;
            delayPairing.u32DelayTime = theSpiParams.u32FstPollByteDelay; 

            do
            {
                status = VltSpiCheetahDataTransfer( READ_MODE, 1, pu8DataPos, 1, &delayPairing );

                u32ReadAttempts++;

                /*
                * Sleep briefly before reading again
                */
                delayPairing.u32DelayTime = theSpiParams.u32IntPollByteDelay; 

            }while( ( theSpiParams.u32PollMaxRetries > u32ReadAttempts ) && 
                ( SPI_POLLING_BYTE == *pu8DataPos ) );

            /*
            * Check if the read was successful
            */
            if( theSpiParams.u32PollMaxRetries == u32ReadAttempts)
            {
                status = ECHERCVREADATTMPTS;
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
    }

    if( VLT_OK == status )
    {
        /*
        * Receive the data
        */
        status = VltSpiCheetahDataTransfer( READ_MODE, u16Len, pu8DataPos, 0, NULL );
    }
    
    return( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}

#if( VLT_ENABLE_SPI == VLT_ENABLE )
static VLT_STS VltSpiCheetahAwaitData( void )
{
    /*
    * Set the flag to poll for data past the padding byte
    */
    bFirstReceive = TRUE;

    return ( VLT_OK );
}

static VLT_STS VltSpiCheetahSetBitRate( VLT_PU16 pu16BitRate )
{
    /*
    * Check the pointer
    */
    if( NULL == pu16BitRate )
    {
        return( ECHESTBTRTNULLPARAMS );
    }

    /*
    * Set bitrate
    */
    *pu16BitRate = ch_spi_bitrate( iHandle, *pu16BitRate );

    return ( VLT_OK );
}

static VLT_STS VltSpiCheetahDataTransfer( VLT_U8 u8Mode,
    VLT_U16 u16Len,
    VLT_PU8 pu8Data,
    VLT_U8 u8DelayArraySz,
    VLT_DELAY_PAIRING* pDelayPairing )
{
    VLT_STS status = VLT_FAIL;
    VLT_U16 u16DelayIndex = 0;

    if( ( 0 != u8DelayArraySz) && ( NULL == pDelayPairing ) )
    {
        return ECHETRANSDATANULL;
    }

    /*
    * Clear the SPI Queue
    */
    if( CH_OK != ch_spi_queue_clear( iHandle ) )
    {
        status = ECHETRANSCLEARQUEUE;
    }
    else
    {
        status = VLT_OK;
    }

    /*
    * Check the array index array to see if a delay has to be inserted before
    * the first byte.  If so do it before enabling the slave select
    */
    if( ( 0 != u8DelayArraySz ) && 
        ( 0 == pDelayPairing[u16DelayIndex].u16BytePos ) )
    {
        ch_spi_queue_delay_ns( iHandle, 
            pDelayPairing[u16DelayIndex].u32DelayTime * NANO_SECS_IN_USEC );

        u16DelayIndex++;
    }

    /*
    * Queue the enable of the Cheetah's Output 
    */
    if( VLT_OK == status )
    {
        if( CH_OK != ch_spi_queue_oe( iHandle, 1 ) )
        {
            status = ECHETRANSOUTENABLE;
        }
    }

    /*
    * Queue the enable of slave select
    */
    if( VLT_OK == status )
    {
        if( CH_OK == ch_spi_queue_ss( iHandle, 1 ) )
        {
            if( WRITE_MODE == u8Mode )
            {
                /*
                * Add the Slave Select delay passed in the comms init params
                */
                ch_spi_queue_delay_ns( iHandle, 
                    theSpiParams.u32SlaveSelectDelay * NANO_SECS_IN_USEC );
            }
        }
        else
        {
            status = ECHETRANSSLVSLCTEN;
        }
    }

    /*
    * Queue the data to be sent
    */
    if( VLT_OK == status )
    {
        VLT_U16 u16Idx = 0;
        VLT_U8 u8SendByte = SPI_POLLING_BYTE;

        for( u16Idx = 0; u16Idx < u16Len; u16Idx++ )
        {
            /*
            * If we are sending data set the appropriate byte to be sent.
            * If we are reading data send the polling byte (0xC0)
            */
            if( WRITE_MODE == u8Mode )
            {
                u8SendByte = pu8Data[u16Idx];
            }

            /*
            * Check if we have a unique interbyte delay to add
            */
            if( ( u8DelayArraySz != 0 ) && ( u16DelayIndex < u8DelayArraySz ) )
            {
                if( pDelayPairing[u16DelayIndex].u16BytePos == u16Idx )
                {
                    /*
                    * We have been given specific interbyte delays for some values
                    */
                    ch_spi_queue_delay_ns( iHandle, 
                        pDelayPairing[u16DelayIndex].u32DelayTime * NANO_SECS_IN_USEC );

                    u16DelayIndex++;
                }
            }
            else
            {
                /*
                * Insert the default interbyte delay
                */
                ch_spi_queue_delay_ns( iHandle, 
                    theSpiParams.u32IntByteDelay * NANO_SECS_IN_USEC );
            }

            /*
            * Send each byte
            */
            if( 1 != ch_spi_queue_byte( iHandle, 1, u8SendByte ) )
            {
                status = ECHETRANSQUEUEBYTE;
                break;
            }
        }
    }

    /*
    * Queue the disable of slave select
    */
    if( VLT_OK == status )
    {
        if( CH_OK != ch_spi_queue_ss( iHandle, 0 ) )
        {
            status = ECHETRANSSLVSLCTDIS;
        }
    }

    /*
    * Queue the disable of the Cheetah's Output 
    */
    if( VLT_OK == status )
    {
        if( CH_OK != ch_spi_queue_oe( iHandle, 0 ) )
        {
            status = ECHETRANSOUTDISABLE;
        }
    }

    /*
    * Send Accumulated commands 
    */
    if( VLT_OK == status )
    {
        VLT_U16 u16RcvLen = 0;
        VLT_PU8 pu8RcvData = NULL;

        if( READ_MODE == u8Mode )
        {
            u16RcvLen = u16Len;
            pu8RcvData = pu8Data;
        }
        if( u16Len != ch_spi_batch_shift( iHandle, u16RcvLen, pu8RcvData ) )
        {
            status = ECHETRANSBATCHSHIFT;
        }
    }

    return( status );
}

//
// If the pXmlReaderString is null and pSize not null and a value of zero it 
// means return to the pSize the total number of bytes required to read the
// entire XML string.
//
//
VLT_STS VltCheetahDetectReaders( VLT_PU32 pSize, VLT_PU8 pXmlReaderString )
{
    VLT_STS status = VLT_FAIL;  
    // Has to be a signed integer to account for the library missing.
    int numOfReaders = 0;
    VLT_U16 index = 0;
    VLT_U16 count = 0;   
    const char* pXmlHeader = "<interface type=\"cheetah\">";   // 26, 0x1A
    const char* pXmlData = "<peripheral idx=\"%02d\">%10lu</peripheral>"; //34 , 0x22 
    const char* pXmlFooter = "</interface>"; //12 - 0x0C
    VLT_U16 au16Dev[MAX_NO_OF_CHEETAHS];
    VLT_U32 au32Ids[MAX_NO_OF_CHEETAHS];
            
    if( NULL == pSize )
    {
        return( EDTCHERSIVLDPARAM );
    }

    if( ( *pSize != 0 ) && ( NULL == pXmlReaderString ) )
    {
        return( EDTCHERSIVLDSTR );
    }

    /*
     * If the number of devices found is more than we can handle then we return an 
     * appropriate error. Otherwise if the number returned is negative the cheetah.dll
     * is not in the specified path so we coerse the numOfReaders to 0. However, 
     * if the number is between 0 and the MAX_NO_OF_CHEETAHS then we accept that and 
     * we build the appropriate device strings.
     */
    if( MAX_NO_OF_CHEETAHS < ( numOfReaders = ch_find_devices_ext( MAX_NO_OF_CHEETAHS, 
        &au16Dev[0], 
        MAX_NO_OF_CHEETAHS, 
        (u32*)&au32Ids[0] ) ) )
    {        
        return( EDTCHDRSEXDMXM );
    }
    else
    {
        if( 0 > numOfReaders )
        {
            numOfReaders = 0;
        }
    }

    /*
     * Total String count is:
     * Xml Header Size + ( Number of Readers * Xml Data Size) + Xml Footer Size
     */
    count = ( XML_CH_HEADER_SZ + ( numOfReaders * XML_CH_DATA_SZ ) + XML_CH_FOOTER_SZ );
    /*
     * Each entry is a 10 digit serial number
     */
    count += numOfReaders * 10 ; 

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
            status = EDTCHDRSINSFLEN;
        }
        else
        {
            *pSize = 0;
            index = 0;

            /*
             * Add the XML header
             */
            *pSize = sprintf( (char*)&pXmlReaderString[*pSize], "%s", pXmlHeader );

            /*
             * Add the XML Data
             */
            while( index < numOfReaders )
            {
                *pSize += sprintf( (char*)&pXmlReaderString[*pSize], (const char*)pXmlData, index, (unsigned long)au32Ids[index] );
                ++index;
            }

            /*
             * Add the XML Footer
             */
            *pSize += sprintf( (char*)&pXmlReaderString[*pSize], "%s", pXmlFooter );
        }
    }

    return( VLT_OK );
}

#endif /* ( VLT_ENABLE_SPI == VLT_ENABLE ) */
