/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#include "vaultic_common.h"
#include "vaultic_block_protocol.h"
#include "vaultic_crc16.h"
#include "vaultic_peripheral.h"
#if ( VLT_ENABLE_SPI == VLT_ENABLE )
    #include "vaultic_spi_peripheral.h"
#endif
#if ( VLT_ENABLE_TWI == VLT_ENABLE )
    #include "vaultic_twi_peripheral.h"
#endif
#include "vaultic_mem.h"
#include "vaultic_apdu.h"
#include "vaultic_timer_delay.h"

/*
* Error Codes
*/
#define EBLKPTCLINITNULLPARAM VLT_ERROR( VLT_BLKPTCL, 0 )
#define EBLKPTCLINVLDCOMMSMD  VLT_ERROR( VLT_BLKPTCL, 1 )
#define EBLKPTCLADDNULLPARAM  VLT_ERROR( VLT_BLKPTCL, 2 )
#define EBLKPTCLREMNULLPARAM  VLT_ERROR( VLT_BLKPTCL, 3 )
#define EBLKPTCLIOSNDRCVNULL  VLT_ERROR( VLT_BLKPTCL, 4 )
#define EBLKPTCLSBLOCKSNDNULL VLT_ERROR( VLT_BLKPTCL, 5 )
#define EBLKPTCLINVLDSTATE    VLT_ERROR( VLT_BLKPTCL, 6 )
#define EBLKPTCLINITCHKMODE   VLT_ERROR( VLT_BLKPTCL, 7 )
#define EBLKPTCLSUM8NOSUPP    VLT_ERROR( VLT_BLKPTCL, 8 )
#define EBLKPTCLCRC16NOSUPP   VLT_ERROR( VLT_BLKPTCL, 9 )
#define EBLKPTCLINVLDCHKSUMMD VLT_ERROR( VLT_BLKPTCL, 10 )
#define EBLKPTCLINVLDBITRATE  VLT_ERROR( VLT_BLKPTCL, 11 )
#define EBLKPTCLUNSUPBITRATE  VLT_ERROR( VLT_BLKPTCL, 12 )
#define EBLKPTCLMAXRSYNC      VLT_ERROR( VLT_BLKPTCL, 13 )

/*
* Defines
*/
#define BLK_PTCL_BLOCK_TYPE_MASK     (VLT_U8)0xC0
#define BLK_PTCL_IBLOCK_MASK         (VLT_U8)0x00
#define BLK_PTCL_SBLOCK_MASK         (VLT_U8)0x40
#define BLK_PTCL_RBLOCK_MASK         (VLT_U8)0x80

#define BLK_PTCL_GET_PARAMS_MASK     (VLT_U8)0x00
#define BLK_PTCL_SET_PARAMS_MASK     (VLT_U8)0x02
#define BLK_PTCL_GET_IDENTITY_MASK   (VLT_U8)0x04
#define BLK_PTCL_RESYNCH_MASK        (VLT_U8)0x06
#define BLK_PTCL_MORE_TIME           (VLT_U8)0x08

#define BLCK_PTCL_MASTER_SEND_MASK   (VLT_U8)0x00
#define BLCK_PTCL_SLAVE_SEND_MASK    (VLT_U8)0x01

#define BLK_PTCL_MAX_ERR_CNT         (VLT_U8)0x03

#define MAX_BIT_RATE_SUPPORT_MASK    (VLT_U8)0x20
#define MAX_BUFFER_SIZE_SUPPORT_MASK (VLT_U8)0x10
#define CRC16_SUPPORT_MASK           (VLT_U8)0x02
#define SUM8_SUPPORT_MASK            (VLT_U8)0x01

#define SET_PARAMS_DATA_POS          (VLT_U8)0x03
#define GET_PARAMS_RESP_LEN          (VLT_U8)0x05
#define GET_PARAMS_TWI_RESP_LEN      (VLT_U8)0x06

#define MAX_BUF_SIZE_MSB_DATA_OFFSET (VLT_U8)0x01
#define MAX_BUF_SIZE_LSB_DATA_OFFSET (VLT_U8)0x02
#define MAX_BIT_RATE_MSB_DATA_OFFSET (VLT_U8)0x03
#define MAX_BIT_RATE_LSB_DATA_OFFSET (VLT_U8)0x04
#define BLK_PTCL_TEMP_BUFFER_SIZE    (VLT_U8)0x03

#define ST_NORMAL                    (VLT_U8)0x00
#define ST_RESEND_DATA               (VLT_U8)0x01
#define ST_RCVD_DATA_ERROR           (VLT_U8)0x02
#define ST_RESYNCH                   (VLT_U8)0x04
#define ST_MORE_TIME                 (VLT_U8)0x08
#define ST_RESYNCH_DEFAULT           (VLT_U8)0x10

#define MAX_RESYNC_ATTEMPTS          (VLT_U8)0x0A



#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X )
    #define DELAY_ARRAY_HDR_PAIRING_SIZE    3

    #define DEL_ARR_IDX_INT_BLOCK        (VLT_U8)0x00
    #define DEL_ARR_IDX_AFTR_HDR_BYTE_1  (VLT_U8)0x01
    #define DEL_ARR_IDX_AFTR_HDR_BYTE_2  (VLT_U8)0x02

    #define DEL_ARR_IDX_AFTER_HDR        (VLT_U8)0x00
#endif /* ( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X ) */

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)
    #define DELAY_ARRAY_PAIRING_SIZE     (VLT_U16)0x04

    #define DEL_ARR_IDX_INT_BLOCK        (VLT_U8)0x00
    #define DEL_ARR_IDX_AFTR_HDR_BYTE_1  (VLT_U8)0x01
    #define DEL_ARR_IDX_AFTR_HDR_BYTE_2  (VLT_U8)0x02
    #define DEL_ARR_IDX_AFTER_HDR        (VLT_U8)0x03
#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */



/*
* Private Data
*/
#if( ( VLT_ENABLE_SPI == VLT_ENABLE ) || ( VLT_ENABLE_TWI == VLT_ENABLE ) )
    /*
    * Structure for the parameters returned from the Get Parameters S-Block
    */
    typedef struct _vltBlkPtclParams
    {
        VLT_U16 u16MaxBufferSize;
        VLT_U16 u16MaxBitRate;
        VLT_U8  bCrc16Support;
        VLT_U8  bSum8Support;
    }VltBlkPtclParams;

    static VltBlkPtclParams theVltBlkPtclParams = 
    {
        0,
        0,
        TRUE,
        TRUE
    };

    /*
    * Function pointers to the specified peripheral
    */
    static VltPeripheral theVltPeripheral = 
    {
        0,
        0,
        0,
        0,
        0,
    };

    /*
    * Parameters set by the Init method
    */
    static VLT_U32 u32AfterHdrDelay = 0;
    static VLT_U32 u32InterBlkDelay = 0;
    static VLT_U16 u16BitRate = 0;
    static VLT_U8 u8CheckMode = BLK_PTCL_CHECKSUM_SUM8;
    /*
    * The Mem Blobs used to add the Block Protocol Header info to
    */
    static VLT_MEM_BLOB Command;
    static VLT_MEM_BLOB Response;
    /*
    * Error counter for the number of R-Blocks received
    */
    static VLT_U8 u8ErrorCount = 0;
    static VLT_U8 bSendSBlock = FALSE;
    /*
    * Variables which track the state of the Block Protocol
    */
    static VLT_U8 u8CurrentState = ST_NORMAL;
    static VLT_U8 u8PreviousState = ST_NORMAL;
    /*
    * Temporary buffer used to backup command data that could be lost as a 
    * result of S-Blocks being sent/received between I-Blocks
    */
    static VLT_U8 u8CmdBackupBuf[BLK_PTCL_TEMP_BUFFER_SIZE];
    /*
    * Rescynch Send Count
    */
    static VLT_U8 u8ResynchSendCnt = 0;

    /*
    * Array used for delays by the peripheral
    */
#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X )
    static VLT_DELAY_PAIRING delayArrayHdr[DELAY_ARRAY_HDR_PAIRING_SIZE] =
    {
        { DEL_ARR_IDX_INT_BLOCK, 0 },
        { DEL_ARR_IDX_AFTR_HDR_BYTE_1, 0 },
        { DEL_ARR_IDX_AFTR_HDR_BYTE_2, 0 },
    };

    static VLT_DELAY_PAIRING delayData[1] =
    {
        { DEL_ARR_IDX_AFTER_HDR, 0 }
    };
#endif /*( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X ) */

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)
    static VLT_DELAY_PAIRING delayArray[DELAY_ARRAY_PAIRING_SIZE] =
    {
        { DEL_ARR_IDX_INT_BLOCK, 0 },
        { DEL_ARR_IDX_AFTR_HDR_BYTE_1, 0 },
        { DEL_ARR_IDX_AFTR_HDR_BYTE_2, 0 },
        { DEL_ARR_IDX_AFTER_HDR, 0 }
    };
#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */

    /*
    * Static Method Prototypes
    */
    static VLT_STS VltBlkPtclAddIBlockHeader( VLT_MEM_BLOB *pOutData );

    static VLT_STS VltBlkPtclHandleResponse( VLT_MEM_BLOB* pInData );

    static void VltBlkPtclHandleSBlockRsp( void );

    static void VltBlkPtclHandleGetParamsRsp( void );

    static VLT_U8 CalculateSum8Checksum( VLT_PU8 pu8Data, VLT_U16 u16Len );

    static void AddCommandCheckSum( void );

    static VLT_STS ConstructSBlockSend( VLT_U8 u8SBlockCmdMask, 
        VLT_U8 u8Len, 
        VLT_PU8 pu8Data );

    static void ConstructSendRBlock( void );

    static void UpdateState( VLT_U8 u8State );

#endif /* ( ( VLT_ENABLE_SPI == VLT_ENABLE ) || ( VLT_ENABLE_TWI == VLT_ENABLE ) ) */

/*
* Public Methods
*/

VLT_STS VltBlkPtclInit( VLT_INIT_COMMS_PARAMS* pInitCommsParams,
    VLT_MEM_BLOB *pOutData,
    VLT_MEM_BLOB *pInData,
    VLT_PU16 pu16MaxSendSize,
    VLT_PU16 pu16MaxReceiveSize )
{
#if( ( VLT_ENABLE_SPI == VLT_ENABLE ) || ( VLT_ENABLE_TWI == VLT_ENABLE ) )
    VLT_STS status = VLT_FAIL;
    VLT_U8 u8ReqCheckMode = BLK_PTCL_CHECKSUM_SUM8;
    /*
    * Check that the pointer to the Block Protocol Header Position Buffer
    * is valid
    */
    if( ( NULL == pInitCommsParams ) ||
        ( NULL == pOutData ) || 
        ( NULL == pInData ) ||
        ( NULL == pu16MaxSendSize ) ||
        ( NULL == pu16MaxReceiveSize ) )
    {
        return EBLKPTCLINITNULLPARAM;
    }

    switch( pInitCommsParams->u8CommsProtocol )
    {
#if( VLT_ENABLE_SPI == VLT_ENABLE )
        case VLT_SPI_COMMS:
            theVltPeripheral.PeripheralInit = VltSpiPeripheralInit;
            theVltPeripheral.PeripheralClose = VltSpiPeripheralClose;
            theVltPeripheral.PeripheralIoctl = VltSpiPeripheralIoctl;
            theVltPeripheral.PeripheralSendData = VltSpiPeripheralSendData;
            theVltPeripheral.PeripheralReceiveData = VltSpiPeripheralReceiveData;
            break;
#endif /* ( VLT_ENABLE_SPI == VLT_ENABLE ) */

#if( VLT_ENABLE_TWI == VLT_ENABLE )
        case VLT_TWI_COMMS:
            theVltPeripheral.PeripheralInit = VltTwiPeripheralInit;
            theVltPeripheral.PeripheralClose = VltTwiPeripheralClose;
            theVltPeripheral.PeripheralIoctl = VltTwiPeripheralIoctl;
            theVltPeripheral.PeripheralSendData = VltTwiPeripheralSendData;
            theVltPeripheral.PeripheralReceiveData = VltTwiPeripheralReceiveData;
            break;
#endif /* ( VLT_ENABLE_TWI == VLT_ENABLE ) */

        default:
            return EBLKPTCLINVLDCOMMSMD;
    }

    /*
    * Check that the checksum mode is valid
    */
    if( ( BLK_PTCL_CHECKSUM_SUM8 != pInitCommsParams->Params.VltBlockProtocolParams.u8CheckSumMode ) &&
        ( BLK_PTCL_CHECKSUM_CRC16 != pInitCommsParams->Params.VltBlockProtocolParams.u8CheckSumMode ) )
    {
        return EBLKPTCLINVLDCHKSUMMD;
    }
    /*
    * Store the checksum mode
    */
    u8ReqCheckMode = pInitCommsParams->Params.VltBlockProtocolParams.u8CheckSumMode;
    
    /*
    * Store the Configurable delay times
    */
    u32AfterHdrDelay = pInitCommsParams->Params.VltBlockProtocolParams.u32AfterHdrDelay;
    u32InterBlkDelay = pInitCommsParams->Params.VltBlockProtocolParams.u32InterBlkDelay;

    /*
    * Store the input MEM BLOBs that should be used to append the Block
    * Protocol data
    */
    Command.pu8Data = pOutData->pu8Data;
    Command.u16Capacity = pOutData->u16Capacity;

    Response.pu8Data = pInData->pu8Data;
    Response.u16Capacity = pInData->u16Capacity;

    /*
    * The Block protocol has no further overhead on the comms buffer
    */
    *pu16MaxSendSize = VLT_MAX_APDU_SND_DATA_SZ;
    *pu16MaxReceiveSize = VLT_MAX_APDU_RCV_DATA_SZ;

    /*
    * Store the Bit Rate that is the target to use
    */
    u16BitRate = pInitCommsParams->Params.VltBlockProtocolParams.u16BitRate;

    /*
    * Update the Delay Array with the appropriate values
    */
#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X )
    delayArrayHdr[DEL_ARR_IDX_INT_BLOCK].u32DelayTime =
        pInitCommsParams->Params.VltBlockProtocolParams.u32InterBlkDelay;

    delayArrayHdr[DEL_ARR_IDX_AFTR_HDR_BYTE_1].u32DelayTime = 
        pInitCommsParams->Params.VltBlockProtocolParams.u32InterHdrByteDelay;

    delayArrayHdr[DEL_ARR_IDX_AFTR_HDR_BYTE_2].u32DelayTime = 
        pInitCommsParams->Params.VltBlockProtocolParams.u32InterHdrByteDelay;


    delayData[0].u32DelayTime = 
        pInitCommsParams->Params.VltBlockProtocolParams.u32AfterHdrDelay;
#endif /*( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X ) */


#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)
    delayArray[DEL_ARR_IDX_INT_BLOCK].u32DelayTime =
        pInitCommsParams->Params.VltBlockProtocolParams.u32InterBlkDelay;

    delayArray[DEL_ARR_IDX_AFTR_HDR_BYTE_1].u32DelayTime = 
        pInitCommsParams->Params.VltBlockProtocolParams.u32InterHdrByteDelay;

    delayArray[DEL_ARR_IDX_AFTR_HDR_BYTE_2].u32DelayTime = 
        pInitCommsParams->Params.VltBlockProtocolParams.u32InterHdrByteDelay;

    delayArray[DEL_ARR_IDX_AFTER_HDR].u32DelayTime = 
        pInitCommsParams->Params.VltBlockProtocolParams.u32AfterHdrDelay;

#endif /*( VAULT_IC_VERSION == VAULTIC_VERSION_1_2_X ) */

    /*
    * Call init on the Block Protocol Peripheral
    */
    status = 
        theVltPeripheral.PeripheralInit( pInitCommsParams );

    /*
    * SDVAULTICWRAP-58 - Introduce a delay from when bus power is applied
    * to sending first command to allow the self tests to complete
    */
    VltSleep( pInitCommsParams->Params.VltBlockProtocolParams.u16msSelfTestDelay * 
        VLT_MICRO_SECS_IN_MSEC );

    if( VLT_OK == status )
    {    
        /*
        * Force a Resynch
        */
        u8ResynchSendCnt = 0;
        u8CurrentState = ST_RESYNCH_DEFAULT;
        u8PreviousState = ST_NORMAL;
        status = VltBlkPtclSendReceiveData( &Command, &Response );
    }

    if( VLT_OK == status )
    {
        /*
        * Get the current parameters
        */
        ConstructSBlockSend( BLK_PTCL_GET_PARAMS_MASK, 0, NULL );
        status = VltBlkPtclSendReceiveData( &Command, &Response );
    }

    /*
    * Check if the Checksum mode is supported
    */
    if( (BLK_PTCL_CHECKSUM_SUM8 == u8ReqCheckMode) && 
        ( FALSE == theVltBlkPtclParams.bSum8Support) )
    {
        status = EBLKPTCLSUM8NOSUPP;
    }

    if( (BLK_PTCL_CHECKSUM_CRC16 == u8ReqCheckMode) && 
        ( FALSE == theVltBlkPtclParams.bCrc16Support) )
    {
        status = EBLKPTCLCRC16NOSUPP;
    }

    if( VLT_OK == status )
    {
        /*
        * If the current checksum mode differs from the request then change it
        */
        if( u8CheckMode != u8ReqCheckMode )
        {
            /*
            * Set up the buffer to send the SetParameters method to 
            * change the checksum method
            */
            ConstructSBlockSend( BLK_PTCL_SET_PARAMS_MASK, 
                sizeof(VLT_U8),
                &u8ReqCheckMode );

            status = VltBlkPtclSendReceiveData( &Command, &Response );

            if( VLT_OK == status )
            {
                /*
                * The value of u8CheckMode will have been updated by
                * VltBlkPtclSendReceiveData
                */
                if ( u8CheckMode != u8ReqCheckMode )
                {
                    status = EBLKPTCLINITCHKMODE;
                }
            }
        }
    }

    /*
    * Attempt to update the bit rate used for comms
    */
    if( VLT_OK == status )
    {
        if( u16BitRate <= theVltBlkPtclParams.u16MaxBitRate )
        {
            status = theVltPeripheral.PeripheralIoctl( VLT_UPDATE_BITRATE , 
                (void*)&u16BitRate );

            /*
            * Check that the bit rate has been set correctly, if not update to show the
            * value that was set and return an error
            */
            if( pInitCommsParams->Params.VltBlockProtocolParams.u16BitRate != u16BitRate )
            {
                pInitCommsParams->Params.VltBlockProtocolParams.u16BitRate = u16BitRate;

                status = EBLKPTCLUNSUPBITRATE;
            }
        }
        else
        {
            /*
            * The specified bit rate is higher than the maximum that the Vault
            * IC supports.  Update the bit rate to tell the user what the max
            * supported bit rate is
            */
            pInitCommsParams->Params.VltBlockProtocolParams.u16BitRate = 
                theVltBlkPtclParams.u16MaxBitRate;

            status = EBLKPTCLINVLDBITRATE;
        }
    }

    return ( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif    
}

VLT_STS VltBlkPtclClose( void )
{
#if( ( VLT_ENABLE_SPI == VLT_ENABLE ) || ( VLT_ENABLE_TWI == VLT_ENABLE ) )
    return theVltPeripheral.PeripheralClose( );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}

VLT_STS VltBlkPtclSendReceiveData( VLT_MEM_BLOB *pOutData, VLT_MEM_BLOB *pInData )
{
#if( ( VLT_ENABLE_SPI == VLT_ENABLE ) || ( VLT_ENABLE_TWI == VLT_ENABLE ) )
    VLT_STS status = VLT_FAIL;
    VLT_MEM_BLOB TempBlob;

    if( ( NULL == pOutData ) || ( NULL == pInData ) )
    {
        return EBLKPTCLIOSNDRCVNULL;
    }

    do
    {
        /*
        * Add the appropriate header info
        */
        switch( u8CurrentState )
        {
        case ST_RESEND_DATA:
            /*
            * Restore the corrupted command data from the last command
            */
            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memcpy( &(Command.pu8Data[VLT_BLOCK_PROTOCOL_HDR_SZ]), 
                &u8CmdBackupBuf[0],
                BLK_PTCL_TEMP_BUFFER_SIZE );
            /* fall through */
        case ST_NORMAL:
            if( FALSE == bSendSBlock )
            {
                /*
                * Send/Resend the I-Block
                */
                status = VltBlkPtclAddIBlockHeader( pOutData );
            }
            else
            {
                status = VLT_OK;
            }
            break;

        case ST_RCVD_DATA_ERROR:
            /*
            * Need to send an R-Block to request a resend
            */
            ConstructSendRBlock( );
            status = VLT_OK;
            break;

        case ST_RESYNCH:
        case ST_RESYNCH_DEFAULT:
            u8CurrentState = ST_RESYNCH; 
            u8CheckMode = BLK_PTCL_CHECKSUM_SUM8;
            ConstructSBlockSend( BLK_PTCL_RESYNCH_MASK, 0, NULL );
            u8ResynchSendCnt++;
            status = VLT_OK;
            break;

        case ST_MORE_TIME:
            status = VLT_OK;
            break;

        default:
            status = EBLKPTCLINVLDSTATE;
            break;
        }

        /*
        * Send the Block Protocol command and receive the response
        */
        if( VLT_OK == status )
        {
            /*
            * If more time has been requested don't send anything
            * just try to receive
            */
            if( ST_MORE_TIME != u8CurrentState )
            {
#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X )
                /*
                * Send the header.  Setup the Temporary Mem Blob for this
                */
                TempBlob.pu8Data = &(Command.pu8Data[0]);
                TempBlob.u16Len = VLT_BLOCK_PROTOCOL_HDR_SZ;
                TempBlob.u16Capacity = VLT_BLOCK_PROTOCOL_HDR_SZ;

                status = theVltPeripheral.PeripheralSendData( &TempBlob, 
                    DELAY_ARRAY_HDR_PAIRING_SIZE,
                    &(delayArrayHdr[0]) );

                if( VLT_OK == status )
                {
                    /*
                    * Send the data
                    */
                    TempBlob.pu8Data = &(Command.pu8Data[VLT_BLOCK_PROTOCOL_HDR_SZ]);
                    TempBlob.u16Len = Command.u16Len - VLT_BLOCK_PROTOCOL_HDR_SZ;
                    TempBlob.u16Capacity = Command.u16Capacity - VLT_BLOCK_PROTOCOL_HDR_SZ;

                    status = theVltPeripheral.PeripheralSendData( &TempBlob, 
                        1,
                        &(delayData[0]) );
                }
#endif /*( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X )*/

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) 

                status = theVltPeripheral.PeripheralSendData( &Command, 
                    DELAY_ARRAY_PAIRING_SIZE,
                    &(delayArray[0]) );

#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)*/
            }
            else
            {
                status = theVltPeripheral.PeripheralIoctl( VLT_AWAIT_DATA, NULL );
            }

            if( VLT_OK == status )
            {
                /*
                * Receive the header.  Setup the Temporary Mem Blob for this
                */
                TempBlob.pu8Data = &Response.pu8Data[0];
                TempBlob.u16Len = VLT_BLOCK_PROTOCOL_HDR_SZ;
                TempBlob.u16Capacity = VLT_BLOCK_PROTOCOL_HDR_SZ;

                status = theVltPeripheral.PeripheralReceiveData ( &TempBlob );

                if( VLT_OK == status)
                {
                    /*
                    * Receive the data.  Setup the Temporary Mem Blob for this
                    */
                    TempBlob.pu8Data = &Response.pu8Data[VLT_BLOCK_PROTOCOL_HDR_SZ];
                    TempBlob.u16Len = ( Response.pu8Data[BLK_PTCL_LEN_MSB_OFFSET] << 8 ) + 
                        Response.pu8Data[BLK_PTCL_LEN_LSB_OFFSET];
                    TempBlob.u16Capacity = Response.u16Capacity - VLT_BLOCK_PROTOCOL_HDR_SZ;

                    /*
                    * Adjust the requested length to include the checksum byte(s)
                    */
                    if( BLK_PTCL_CHECKSUM_SUM8 == u8CheckMode )
                    {
                        TempBlob.u16Len += sizeof(VLT_U8);
                    }
                    else
                    {
                        TempBlob.u16Len += sizeof(VLT_U16);
                    }

                    status = 
                        theVltPeripheral.PeripheralReceiveData( &TempBlob );

                    if( VLT_OK == status )
                    {
                        /*
                        * Update the length of the Response Buffer
                        */
                        Response.u16Len = TempBlob.u16Len + VLT_BLOCK_PROTOCOL_HDR_SZ;
                    }
                }
                else
                {
                    if( ST_RESYNCH == u8CurrentState )
                    {
                        /*
                        * Reset the protocol, update the state to show that we are
                        * resynching at default parameters and set the status as VLT_OK
                        * to allow an attempt to resync at default parameters
                        */
                        theVltPeripheral.PeripheralIoctl( VLT_RESET_PROTOCOL, NULL );
                        u8CurrentState = ST_RESYNCH_DEFAULT;
                        status = VLT_OK;
                    }
                }
            }

            if( MAX_RESYNC_ATTEMPTS == u8ResynchSendCnt )
            {
                status = EBLKPTCLMAXRSYNC;
                u8ResynchSendCnt = 0;
            }
        }

        /*
        * Handle the response
        */
        if( ( VLT_OK == status ) && ( ST_RESYNCH_DEFAULT != u8CurrentState) )
        {
            status = VltBlkPtclHandleResponse( pInData );
        }
    }while( (ST_NORMAL != u8CurrentState) && ( VLT_OK == status ) );

    return ( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}

#if( ( VLT_ENABLE_SPI == VLT_ENABLE ) || ( VLT_ENABLE_TWI == VLT_ENABLE ) )
static VLT_STS VltBlkPtclAddIBlockHeader( VLT_MEM_BLOB *pOutData )
{
    /*
    * Check that the Command Buffer passed in is valid
    */
    if( NULL == pOutData )
    {
        return EBLKPTCLADDNULLPARAM;
    }

    /*
    * All blocks that will have the Block Protocol data added from this
    * interface will be I-Blocks from the Master to the Slave
    */
    Command.pu8Data[BLK_PTCL_BLOCK_TYPE_OFFSET] = 
        BLK_PTCL_IBLOCK_MASK | BLCK_PTCL_MASTER_SEND_MASK;

    /*
    * Set the Length to be the length of the command buffer which specifies
    * the complete length of the APDU command
    */
    Command.pu8Data[BLK_PTCL_LEN_MSB_OFFSET] = 
        (VLT_U8)( pOutData->u16Len >> 8 ) & 0xFF;
    Command.pu8Data[BLK_PTCL_LEN_LSB_OFFSET] = (VLT_U8)pOutData->u16Len & 0xFF;
    Command.u16Len = pOutData->u16Len + VLT_BLOCK_PROTOCOL_HDR_SZ;

    /*
    * Calculate the specified checksum and add it after the APDU command data
    */
    AddCommandCheckSum( );
    
    /*
    * If any S or R-Block need to be sent they could corrupt some of the I-Block
    * Data.  Back up the bytes that could be corrupted in case they are needed.
    * Get Parameters and Get Identity won't be sent after Application commands have
    * commenced so the buffer doesn't need to be large enough to accomodate them
    */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( &u8CmdBackupBuf[0],
        &(Command.pu8Data[VLT_BLOCK_PROTOCOL_HDR_SZ]),
        BLK_PTCL_TEMP_BUFFER_SIZE );

    return ( VLT_OK );
}

static VLT_STS VltBlkPtclHandleResponse( VLT_MEM_BLOB* pInData )
{
    VLT_U8 u8BlockType = 0;
    VLT_U16 u16Length = 0;
    VLT_U16 u16RcvdCheckSum = 0;
    VLT_U16 u16CalcCheckSum = 0;

    /*
    * Check that the ResponseBuffer passed in is valid
    */
    if( NULL == pInData )
    {
        return EBLKPTCLREMNULLPARAM;
    }

    /*
    * Check that the direction identifier within the Block Type is correct
    */
    if( BLCK_PTCL_SLAVE_SEND_MASK != 
            (Response.pu8Data[BLK_PTCL_BLOCK_TYPE_OFFSET] & BLCK_PTCL_SLAVE_SEND_MASK ) )
    {
        /*
        * The direction identifier is incorrect
        */
        switch( u8CurrentState )
        {
        case ST_RESYNCH:
        case ST_RESYNCH_DEFAULT:
            break;

        case ST_NORMAL:
        case ST_MORE_TIME:
        case ST_RESEND_DATA:
            UpdateState( ST_RCVD_DATA_ERROR );
            /*
            * Fall through
            */
       case ST_RCVD_DATA_ERROR:
           u8ErrorCount++;
           break;

        default:
            UpdateState( ST_RCVD_DATA_ERROR );
            u8ErrorCount++;
            break;
        }
    }
    else
    {
        /*
        * Store the length
        */
        u16Length = ( Response.pu8Data[BLK_PTCL_LEN_MSB_OFFSET] << 8 )
            + Response.pu8Data[BLK_PTCL_LEN_LSB_OFFSET];

        /*
        * Validate the Checksum
        */
        if( BLK_PTCL_CHECKSUM_SUM8 == u8CheckMode )
        {
            /*
            * SUM 8 Checksum
            */
            u16RcvdCheckSum = Response.pu8Data[Response.u16Len - sizeof(VLT_U8)];

            u16CalcCheckSum = 
                CalculateSum8Checksum( Response.pu8Data, Response.u16Len - sizeof(VLT_U8) ); 
        }
        else
        {
            /*
            * CRC-16 Checksum
            */
            u16RcvdCheckSum = ( Response.pu8Data[Response.u16Len - sizeof(VLT_U16)] << 8 )
                + Response.pu8Data[Response.u16Len - sizeof(VLT_U8)];

            u16CalcCheckSum = 
                VltCrc16Block( VLT_CRC16_CCITT_INIT_Fs,  
                &(Response.pu8Data[0]), 
                Response.u16Len - sizeof(VLT_U16) );
        }

        /*
        * Check that the Checksums match
        */
        if( u16CalcCheckSum != u16RcvdCheckSum )
        {
            /*
            * The Checksums don't match so increment the error count
            */
            u8ErrorCount++;
            if( ST_RCVD_DATA_ERROR != u8CurrentState )
            {
                UpdateState( ST_RCVD_DATA_ERROR );
            }
        }
        else
        {
            /*
            * Check the block type byte to determine the type of information received
            */
            u8BlockType = 
                Response.pu8Data[BLK_PTCL_BLOCK_TYPE_OFFSET] & 
                BLK_PTCL_BLOCK_TYPE_MASK;

            switch( u8BlockType )
            {
            case BLK_PTCL_IBLOCK_MASK:
                /*
                * Adjust the Response Buffer to remove the Block Protocol Info
                * This requires that the data point back at the APDU command
                * and that the length remove the Block Protocol Data length
                */
                pInData->pu8Data = &(Response.pu8Data[VLT_BLOCK_PROTOCOL_HDR_SZ]);
                pInData->u16Len = u16Length;

                /*
                * Clear the error counter and update the state
                */
                u8ErrorCount = 0;
                UpdateState( ST_NORMAL );
                break;

            case BLK_PTCL_SBLOCK_MASK:
                VltBlkPtclHandleSBlockRsp( );
                /*
                * Clear the error counter.  The state has been updated within
                * VltBlkPtclHandleSBlockRsp()
                */
                u8ErrorCount = 0;
                break;

            case BLK_PTCL_RBLOCK_MASK:
            default:
                /*
                * Increment the error counter and set the status to show that the
                * last command didn't transmit correctly
                */
                u8ErrorCount++;
                UpdateState( ST_RESEND_DATA );
                break;
            }
        }
    }

    if( BLK_PTCL_MAX_ERR_CNT == u8ErrorCount )
    {
        UpdateState( ST_RESYNCH );
        u8ErrorCount = 0;
    }

    return ( VLT_OK );
}

static void VltBlkPtclHandleSBlockRsp( void )
{
    VLT_U8 u8SBlkCmd = 0;

    /*
    * Check the block type byte to determine the type of information received
    */
    u8SBlkCmd = 
        Response.pu8Data[BLK_PTCL_BLOCK_TYPE_OFFSET] & 
        (~ (BLK_PTCL_BLOCK_TYPE_MASK | BLCK_PTCL_SLAVE_SEND_MASK ) );

    switch( u8SBlkCmd )
    {
    case BLK_PTCL_GET_PARAMS_MASK:
        VltBlkPtclHandleGetParamsRsp( );
        UpdateState( ST_NORMAL );
        break;

    case BLK_PTCL_SET_PARAMS_MASK:
        u8CheckMode = Response.pu8Data[SET_PARAMS_DATA_POS];
        /* fall through */
    case BLK_PTCL_GET_IDENTITY_MASK:
        UpdateState( ST_NORMAL );
        break;

    case BLK_PTCL_RESYNCH_MASK:
        /*
        * Reset the protocol
        */
        theVltPeripheral.PeripheralIoctl( VLT_RESET_PROTOCOL, NULL );
        u8CheckMode = BLK_PTCL_CHECKSUM_SUM8;
        UpdateState ( u8PreviousState );
        u8ResynchSendCnt = 0;
        break;

    case BLK_PTCL_MORE_TIME:
        UpdateState( ST_MORE_TIME );
        break;

    default:
        break;
    }

    /*
    * Clear the flag to say that an S-Block has been built in the command buffer
    * as S-Block has been dealt with
    */
    bSendSBlock = FALSE;
}

static void VltBlkPtclHandleGetParamsRsp( void )
{
    VLT_U8 u8DataPos = VLT_BLOCK_PROTOCOL_HDR_SZ;

    /*
    * Check the first byte for support for params.  First Max Buffer Rate
    * Field Present
    */
    if( MAX_BUFFER_SIZE_SUPPORT_MASK == 
        (MAX_BUFFER_SIZE_SUPPORT_MASK & Response.pu8Data[u8DataPos] ) )
    {
        theVltBlkPtclParams.u16MaxBufferSize = 
            ( Response.pu8Data[u8DataPos + MAX_BUF_SIZE_MSB_DATA_OFFSET] << 8 ) + 
            ( Response.pu8Data[u8DataPos + MAX_BUF_SIZE_LSB_DATA_OFFSET] );
    }

    /*
    * Max Bit Rate Support
    */
    if( MAX_BIT_RATE_SUPPORT_MASK == 
        (MAX_BIT_RATE_SUPPORT_MASK & Response.pu8Data[u8DataPos] ) )
    {
        /*
        * Older versions of the Vault IC Firmware return 6 bytes rather than 5
        * for the Get Parameters data using TWI (See JIRA Issue SDAT98FW-295)
        * To get around this check the length returned and act accordingly
        */
        if(GET_PARAMS_RESP_LEN == Response.pu8Data[BLK_PTCL_LEN_LSB_OFFSET] )
        {
            theVltBlkPtclParams.u16MaxBitRate = 
                ( Response.pu8Data[u8DataPos + MAX_BIT_RATE_MSB_DATA_OFFSET] << 8 ) + 
                ( Response.pu8Data[u8DataPos + MAX_BIT_RATE_LSB_DATA_OFFSET] );
        }
        else/* if(GET_PARAMS_TWI_RESP_LEN == Response.pu8Data[BLK_PTCL_LEN_MSB_OFFSET] */
        {
            theVltBlkPtclParams.u16MaxBitRate = 
                ( Response.pu8Data[u8DataPos + MAX_BIT_RATE_MSB_DATA_OFFSET + 1] << 8 ) + 
                ( Response.pu8Data[u8DataPos + MAX_BIT_RATE_LSB_DATA_OFFSET + 1] );
        }
    }
    

    /*
    * Check for CRC-16 Support
    */
    if( CRC16_SUPPORT_MASK == 
        (CRC16_SUPPORT_MASK & Response.pu8Data[u8DataPos] ) )
    {
        theVltBlkPtclParams.bCrc16Support = TRUE;
    }
    else
    {
        theVltBlkPtclParams.bCrc16Support = FALSE;
    }

    /*
    * Check for SUM8 Support
    */
    if( SUM8_SUPPORT_MASK == 
        (SUM8_SUPPORT_MASK & Response.pu8Data[u8DataPos] ) )
    {
        theVltBlkPtclParams.bSum8Support = TRUE;
    }
    else
    {
        theVltBlkPtclParams.bSum8Support = FALSE;
    }
}

static VLT_U8 CalculateSum8Checksum( VLT_PU8 pu8Data, VLT_U16 u16Len )
{
    VLT_U16 u16Pos = 0;
    VLT_U8 u8Sum8 = 0;

    for( u16Pos = 0; u16Pos < u16Len; u16Pos++ )
    {
        u8Sum8 += pu8Data[u16Pos];
    }

    return ( u8Sum8 );
}

static void AddCommandCheckSum( void )
{
    VLT_U16 u16CalcCheckSum = 0;

    /*
    * Calculate the specified checksum and add it after the APDU command data
    */
    if( BLK_PTCL_CHECKSUM_SUM8 == u8CheckMode )
    {
        u16CalcCheckSum = 
            CalculateSum8Checksum ( Command.pu8Data, Command.u16Len );

        Command.pu8Data[Command.u16Len] = (VLT_U8)u16CalcCheckSum;
        Command.u16Len += sizeof( VLT_U8 );
    }
    else
    {
        u16CalcCheckSum =
            VltCrc16Block( VLT_CRC16_CCITT_INIT_Fs,
            &(Command.pu8Data[0]),
            Command.u16Len );

        Command.pu8Data[Command.u16Len] = 
            (VLT_U8)( ( u16CalcCheckSum >> 8 ) & 0xFF );

        Command.pu8Data[Command.u16Len + 1] = 
            (VLT_U8)( u16CalcCheckSum & 0xFF );
        Command.u16Len += sizeof( VLT_U16 );
    }
}

static void ConstructSendRBlock( void )
{
    /*
    * Setup the type and the length of 0
    */
    Command.pu8Data[BLK_PTCL_BLOCK_TYPE_OFFSET] = 
        BLK_PTCL_RBLOCK_MASK | BLCK_PTCL_MASTER_SEND_MASK;

    Command.pu8Data[BLK_PTCL_LEN_MSB_OFFSET] = 0x00;
    Command.pu8Data[BLK_PTCL_LEN_LSB_OFFSET] = 0x00;

    Command.u16Len = VLT_BLOCK_PROTOCOL_HDR_SZ;

    /*
    * Add the checksum
    */
    AddCommandCheckSum( );
}

static VLT_STS ConstructSBlockSend( VLT_U8 u8SBlockCmdMask, 
    VLT_U8 u8Len, 
    VLT_PU8 pu8Data )
{
    VLT_U8 u8BufPos = 0;

    /*
    * Check Input Parameters.  The pointer to the data can be NULL as long as
    * the length passed for the data is 0
    */
    if( ( 0 < u8Len) && ( NULL == pu8Data ) )
    {
        return EBLKPTCLSBLOCKSNDNULL;
    }

    /*
    * Setup the Type Byte
    */
    Command.pu8Data[BLK_PTCL_BLOCK_TYPE_OFFSET] = 
        BLK_PTCL_SBLOCK_MASK | u8SBlockCmdMask | BLCK_PTCL_MASTER_SEND_MASK;

    /*
    * Set the Length.  No S-Block is bigger than a VLT_U8
    */
    Command.pu8Data[BLK_PTCL_LEN_MSB_OFFSET] = 0x00;
    Command.pu8Data[BLK_PTCL_LEN_LSB_OFFSET] = u8Len;

    /*
    * Add the Data
    */
    for(u8BufPos = 0; u8BufPos < u8Len; u8BufPos++)
    {
        Command.pu8Data[u8BufPos + VLT_BLOCK_PROTOCOL_HDR_SZ] = 
            pu8Data[u8BufPos];
    }

    Command.u16Len = VLT_BLOCK_PROTOCOL_HDR_SZ + u8Len;

    /*
    * There is no data so add Checksum
    */
    AddCommandCheckSum( );

    /*
    * Set the flag to say that an S-Block has been built in the command buffer
    */
    bSendSBlock = TRUE;

    return ( VLT_OK );
}

static void UpdateState( VLT_U8 u8State )
{
    u8PreviousState = u8CurrentState;
    u8CurrentState = u8State;

    if( ST_RESYNCH == u8PreviousState )
    {
        /*
        * If the previous state has been set to resync change it to normal
        * as we don't want to restore the state to resync
        */
        u8PreviousState = ST_NORMAL;
    } 
}
#endif /*( ( VLT_ENABLE_SPI == VLT_ENABLE ) || ( VLT_ENABLE_TWI == VLT_ENABLE ) ) */
