/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#include "vaultic_common.h"
#include "vaultic_spi_peripheral.h"

VLT_STS VltSpiPeripheralInit( VLT_INIT_COMMS_PARAMS* pInitCommsParams )
{
#if( VLT_ENABLE_SPI == VLT_ENABLE )    
    /*
    * Implementation code should be added here
    */
    return ( VLT_FAIL );
#else
    return( EMETHODNOTSUPPORTED );
#endif      
}

VLT_STS VltSpiPeripheralClose( void )
{
#if( VLT_ENABLE_SPI == VLT_ENABLE )  
    /*
    * Implementation code should be added here
    */
    return ( VLT_FAIL );
#else
    return( EMETHODNOTSUPPORTED );
#endif  
}

VLT_STS VltSpiPeripheralIoctl( VLT_U32 u32Id, void* pConfigData )
{
#if( VLT_ENABLE_SPI == VLT_ENABLE )  
    /*
    * Implementation code should be added here
    */
    return ( VLT_FAIL );
#else
    return( EMETHODNOTSUPPORTED );
#endif  
}

VLT_STS VltSpiPeripheralSendData( VLT_MEM_BLOB *pOutData,
    VLT_U8 u8DelayArraySz,
    VLT_DELAY_PAIRING* pDelayPairing )
{
#if( VLT_ENABLE_SPI == VLT_ENABLE )  
    /*
    * Implementation code should be added here
    */
    return ( VLT_FAIL );
#else
    return( EMETHODNOTSUPPORTED );
#endif 
}

VLT_STS VltSpiPeripheralReceiveData( VLT_MEM_BLOB *pInData )
{
#if( VLT_ENABLE_SPI == VLT_ENABLE )  
    /*
    * Implementation code should be added here
    */
    return ( VLT_FAIL );
#else
    return( EMETHODNOTSUPPORTED );
#endif  
}
