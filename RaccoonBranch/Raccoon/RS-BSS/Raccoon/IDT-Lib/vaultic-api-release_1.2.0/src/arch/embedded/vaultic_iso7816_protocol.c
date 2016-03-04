/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#include "vaultic_common.h"
#include "vaultic_iso7816_protocol.h"

VLT_STS VltIso7816PtclInit( VLT_INIT_COMMS_PARAMS* pInitCommsParams,
    VLT_MEM_BLOB *pOutData,
    VLT_MEM_BLOB *pInData,
    VLT_PU16 pu16MaxSendSize,
    VLT_PU16 pu16MaxReceiveSize )
{
#if( VLT_ENABLE_ISO7816 == VLT_ENABLE )  
    /*
    * Implementation code should be added here
    */
    return ( VLT_FAIL );
#else
    return( EMETHODNOTSUPPORTED );
#endif   
}

VLT_STS VltIso7816PtclClose( void )
{
#if( VLT_ENABLE_ISO7816 == VLT_ENABLE )  
    /*
    * Implementation code should be added here
    */
    return ( VLT_FAIL );
#else
    return( EMETHODNOTSUPPORTED );
#endif   
}

VLT_STS VltIso7816PtclSendReceiveData( VLT_MEM_BLOB *pOutData, VLT_MEM_BLOB *pInData )
{
#if( VLT_ENABLE_ISO7816 == VLT_ENABLE )  
    /*
    * Implementation code should be added here
    */
    return ( VLT_FAIL );
#else
    return( EMETHODNOTSUPPORTED );
#endif   
}


