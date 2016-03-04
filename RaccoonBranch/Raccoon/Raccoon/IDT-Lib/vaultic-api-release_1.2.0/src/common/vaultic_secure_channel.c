/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_secure_channel.h
 * 
 * \brief Interface to secure channel.
 * 
 * \par Description:
 * TBD.
 */

#include "vaultic_common.h"
#include "vaultic_secure_channel.h"
#include "vaultic_api.h"
#include "vaultic_scp02.h"
#include "vaultic_scp03.h"

#if( ( VLT_ENABLE_SCP02 == VLT_ENABLE ) || ( VLT_ENABLE_SCP03 == VLT_ENABLE ) )
    /*
    * Function Pointer Defines
    */
    typedef VLT_STS (*pfnScpInit)( VLT_U8 u8ChannelLevel, 
        KEY_BLOB* pSMac, 
        KEY_BLOB* pSEnc, 
        VLT_PU8 pu8HostChal,
        VLT_U8 u8HostChalLen,
        VLT_INIT_UPDATE* pInitUpRsp );

    typedef VLT_STS (*pfnScpClose)( void );

    typedef VLT_STS (*pfnScpWrap)( VLT_MEM_BLOB *pCmd );

    typedef VLT_STS (*pfnScpUnwrap)( VLT_MEM_BLOB *pRsp );

    typedef VLT_STS (*pfnScpGetChannelOverhead)( VLT_U8 u8Mode, VLT_PU8 pu8Overhead );


    /*
    * Local Variables used by both SCP02 and SCP03
    */
    VLT_U8 u8SecureChannelLevel = VLT_NO_CHANNEL;
    VLT_U8 u8SecureChannelState = VLT_USER_NOT_AUTHENTICATED;

    VLT_U8 au8CMacKey[SCPXX_MAX_SESSION_KEY_LEN];
    VLT_U8 au8RMacKey[SCPXX_MAX_SESSION_KEY_LEN];
    VLT_U8 au8CEncKey[SCPXX_MAX_SESSION_KEY_LEN];

    VLT_U8 au8CMac[SCPXX_MAX_CMAC_LEN];
    VLT_U8 au8RMac[SCPXX_MAX_RMAC_LEN];

    typedef struct _secureChannel
    {
        pfnScpInit SecureChannelInit;
        pfnScpClose SecureChannelClose;
        pfnScpWrap SecureChannelWrap;
        pfnScpUnwrap SecureChannelUnwrap;
        pfnScpGetChannelOverhead SecureChannelGetChannelOverhead;
    } SecureChannel;

static SecureChannel theSecureChannel = 
{
#if( VLT_ENABLE_SCP02 == VLT_ENABLE )
    VltScp02Init,
    VltScp02Close,
    VltScp02Wrap,
    VltScp02Unwrap,
    VltScp02GetChannelOverhead
#elif( VLT_ENABLE_SCP03 == VLT_ENABLE )
    VltScp03Init,
    VltScp03Close,
    VltScp03Wrap,
    VltScp03Unwrap,
    VltScp03GetChannelOverhead
#endif/*( VLT_ENABLE_SCP02 == VLT_ENABLE )*/
};
#endif

/*
* Defines
*/

#define HOST_CHALLENGE_LEN    (VLT_U8)0x08

VLT_STS VltScpInit( VLT_U8 u8UserID,
    VLT_U8 u8RoleID, 
    VLT_U8 u8ChannelLevel, 
    KEY_BLOB* pSMac, 
    KEY_BLOB* pSEnc )
{
#if( ( VLT_ENABLE_SCP02 == VLT_ENABLE ) || ( VLT_ENABLE_SCP03 == VLT_ENABLE ) )
    VLT_STS status = VLT_FAIL;
    VLT_U16 au16HostChallenge[HOST_CHALLENGE_LEN];
    VLT_INIT_UPDATE respData;
    
    /*
    * Check that the input parameters are valid
    */
    if( ( NULL == pSMac ) || 
        ( NULL == pSEnc ) || 
        ( NULL == pSMac->keyValue) || 
        ( NULL == pSEnc->keyValue) )
    {
        return ESCPINITNULLPARAM;
    }

    /*
    * Ensure the channel is closed
    */
    status = VltScpClose();

    /*
    * Generate Host Challenge
    */
    status = VltGenerateRandom(
        HOST_CHALLENGE_LEN, 
        &au16HostChallenge[0] );

   
    /*
    * Check that the call to Generate Random was successful
    */
    if( VLT_OK != status )
    {
        return ( ESCPGENRANDFAIL );
    }

    
    /*
    * Initialise Update
    */
    status = VltInitializeUpdate( u8UserID, 
        u8RoleID,
        HOST_CHALLENGE_LEN,
        (VLT_PU8)&au16HostChallenge[0],
        &respData );
    
    
    /*
    * Check that the call to Initialize Update was successful
    */
    if( VLT_OK != status )
    {
        return ( ESCPINITUPDTFAIL );
    }

    /*
    * Check that the response has given either SCP02 or SCP03 data
    */
    switch( respData.u8LoginMethodID )
    {

    #if( VLT_ENABLE_SCP02 == VLT_ENABLE )   
        case VLT_LOGIN_SCP02:
            theSecureChannel.SecureChannelInit = VltScp02Init;
            theSecureChannel.SecureChannelClose = VltScp02Close;
            theSecureChannel.SecureChannelWrap = VltScp02Wrap;
            theSecureChannel.SecureChannelUnwrap = VltScp02Unwrap;
            theSecureChannel.SecureChannelGetChannelOverhead = 
                VltScp02GetChannelOverhead;
            break;
    #endif /* ( VLT_ENABLE_SCP02 == VLT_ENABLE ) */

    #if( VLT_ENABLE_SCP03 == VLT_ENABLE )   
        case VLT_LOGIN_SCP03:
            theSecureChannel.SecureChannelInit = VltScp03Init;
            theSecureChannel.SecureChannelClose = VltScp03Close;
            theSecureChannel.SecureChannelWrap = VltScp03Wrap;
            theSecureChannel.SecureChannelUnwrap = VltScp03Unwrap;
            theSecureChannel.SecureChannelGetChannelOverhead = 
                VltScp03GetChannelOverhead;
            break;
    #endif /* ( VLT_ENABLE_SCP03 == VLT_ENABLE ) */

        default:
             status = ESCPINITUPDTMODE;
    }

    if ( VLT_OK == status ) 
    {
        status = theSecureChannel.SecureChannelInit(u8ChannelLevel, 
            pSMac, 
            pSEnc, 
            (VLT_PU8)&au16HostChallenge[0], 
            HOST_CHALLENGE_LEN,
            &respData );

        
        if ( VLT_OK != status )
        {
            /* Needed to ensure further attempts to start authentication,
               does not result in Initialise Update returning an error (6988h). */
            VltCancelAuthentication( );
        }
    }
    
    return( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}

VLT_STS VltScpClose( void )
{
#if( ( VLT_ENABLE_SCP02 == VLT_ENABLE ) || ( VLT_ENABLE_SCP03 == VLT_ENABLE ) )
    return theSecureChannel.SecureChannelClose();
#else
    return( EMETHODNOTSUPPORTED );
#endif/* ( ( VLT_ENABLE_SCP02 == VLT_ENABLE ) ||( VLT_ENABLE_SCP03 == VLT_ENABLE ) ) */
}

VLT_STS VltScpGetState( VLT_PU8 pu8State )
{
#if( ( VLT_ENABLE_SCP02 == VLT_ENABLE ) || ( VLT_ENABLE_SCP03 == VLT_ENABLE ) )
    /*
    * Check the validity of the input parameter
    */
    if( NULL == pu8State)
    {
        return SCPGETSTATENULLPARAM;
    }

    *pu8State = u8SecureChannelState;

    return( VLT_OK );
#else
    return( EMETHODNOTSUPPORTED );
#endif/* ( ( VLT_ENABLE_SCP02 == VLT_ENABLE ) ||( VLT_ENABLE_SCP03 == VLT_ENABLE ) ) */    
}

VLT_STS VltScpWrap( VLT_MEM_BLOB *pCmd )
{    
#if( ( VLT_ENABLE_SCP02 == VLT_ENABLE ) || ( VLT_ENABLE_SCP03 == VLT_ENABLE ) )
    return theSecureChannel.SecureChannelWrap( pCmd );
#else
    return( EMETHODNOTSUPPORTED );
#endif/* ( ( VLT_ENABLE_SCP02 == VLT_ENABLE ) ||( VLT_ENABLE_SCP03 == VLT_ENABLE ) ) */     
}

VLT_STS VltScpUnwrap( VLT_MEM_BLOB *pRsp )
{    
#if( ( VLT_ENABLE_SCP02 == VLT_ENABLE ) || ( VLT_ENABLE_SCP03 == VLT_ENABLE ) )
    return theSecureChannel.SecureChannelUnwrap( pRsp );
#else
    return( EMETHODNOTSUPPORTED );
#endif/* ( ( VLT_ENABLE_SCP02 == VLT_ENABLE ) ||( VLT_ENABLE_SCP03 == VLT_ENABLE ) ) */ 
}

VLT_STS VltScpGetChannelOverhead( VLT_U8 u8Mode, VLT_PU8 pu8Overhead )
{    
#if( ( VLT_ENABLE_SCP02 == VLT_ENABLE ) || ( VLT_ENABLE_SCP03 == VLT_ENABLE ) )
    return theSecureChannel.SecureChannelGetChannelOverhead( u8Mode, pu8Overhead );
#else
    return( EMETHODNOTSUPPORTED );
#endif/* ( ( VLT_ENABLE_SCP02 == VLT_ENABLE ) ||( VLT_ENABLE_SCP03 == VLT_ENABLE ) ) */
}
