/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_identity_authentication.c
 * 
 * \brief Interface to identity authentication.
 * 
 * \par Description:
 * Provides the host with the ability to authenticate users who have the 
 * following authentication methods:
 * Secure Channel 02.
 * Secure Channel 03.
 * Microsoft Card Minidriver.
 */

#include "vaultic_common.h"
#include "vaultic_identity_authentication.h"
#include "vaultic_secure_channel.h"
#include "vaultic_ms_auth.h"
#include "vaultic_api.h"



/*
* Defines
*/
#define SMAC_STATIC_KEY_INDEX 0
#define SENC_STATIC_KEY_INDEX 1
#define UNDEFINED_STATE 255


#if( VLT_ENABLE_IDENTITY_AUTH == VLT_ENABLE )
/*
 * Private Variable Definitions
 */
static VLT_U8 u8AuthState = UNDEFINED_STATE;
#endif



VLT_STS VltAuthInit( VLT_U8 u8AuthMethod,
    VLT_U8 u8UserID, 
    VLT_U8 u8RoleID, 
    VLT_U8 u8ChannelLevel,
    KEY_BLOB_ARRAY keys )
{
#if( VLT_ENABLE_IDENTITY_AUTH == VLT_ENABLE )    
    VLT_STS status = VLT_FAIL;
    VLT_U8 i = 0;
        
    /*
    * Check the input parameters are valid.
    */    
    if( NULL == keys.pKeys )
    {
        return( EIDINITNULLPARAM );
    }

    /* Check the key size is within range, 1 key for MS and 2 for SCP. */
    if( ( keys.u8ArraySize <= 0 ) || ( keys.u8ArraySize > 2 ) )
    {
        return( EIDNUMKEYS );
    }

    /* Check the key blob pointer at index i is not null, 
     * and the key value in the key blob structure is not null.
     */
    for( i = 0; i < keys.u8ArraySize; i++ )
    {
        if( NULL == keys.pKeys[i] )
        {
            return( EIDNULLBLOBBASE + i );
        }
        if( NULL == keys.pKeys[i]->keyValue )
        {
            return( EIDNULLKEYBASE + i );
        }
    }

    /* Close the currently active login. */
    status = VltAuthClose( );

    if ( VLT_OK == status )
    {
    
        /*
         * Determine which type of channel the user has request.
         */
        switch ( u8AuthMethod )
        {
        case VLT_LOGIN_SCP02:
        case VLT_LOGIN_SCP03:
            
            /* 
             * Call the secure channel init method
             */
            status = VltScpInit( u8UserID, 
                u8RoleID, 
                u8ChannelLevel,
                keys.pKeys[SMAC_STATIC_KEY_INDEX],
                keys.pKeys[SENC_STATIC_KEY_INDEX] );

            /* If the channel has been established, change the internal
               state variable to equal the type of secure channel. */
            if ( VLT_OK == status )
            {
                u8AuthState = u8AuthMethod;
            }

            break;
        case VLT_LOGIN_MS:
            
            /* 
             * Call the Microsoft Card Minidriver init method.
             */
            status = VltMsInit( u8UserID, 
                u8RoleID, 
                keys.pKeys[SMAC_STATIC_KEY_INDEX] );

            /* If the channel has been established, change the internal
               state variable to equal VLT_LOGIN_MS. */
            if ( VLT_OK == status )
            {
                u8AuthState = u8AuthMethod;
            }

            break;
        default:
            /* Any other value than SCP02, SCP03 or MS will result in an error */
            status = EIDINITNULLPARAM;
        }    
    }
    
    return( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}

VLT_STS VltAuthClose( void )
{
#if( VLT_ENABLE_IDENTITY_AUTH == VLT_ENABLE )    
    VLT_STS status = VLT_FAIL;

    /* Check the internal state variable to determine what 
     * type of authentication is established. */
    switch ( u8AuthState )
    {
    case VLT_LOGIN_SCP02:
    case VLT_LOGIN_SCP03:
        
        /* Log out the secure channel authenticated user. */
        status = VltScpClose( );

        break;
    case VLT_LOGIN_MS:
        
        /* Log out the MS authenticated user.*/
        status = VltMsClose( );

        break;
    default:
        /* Call cancel authentication, nothing else can be done. */
        status = VltCancelAuthentication( );
        break;
    }
    
    /* Clear the state tracking variable. */
    u8AuthState = UNDEFINED_STATE;

    return ( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}

VLT_STS VltAuthGetState( VLT_PU8 pu8State )
{
#if( VLT_ENABLE_IDENTITY_AUTH == VLT_ENABLE )
    VLT_STS status = VLT_FAIL;

    if ( NULL == pu8State )
    {
        return ( EIDSTATENULLPARAM );
    }

    /* Check the internal state variable to determine what 
     * type of authentication is established. */
    switch ( u8AuthState )
    {
    case VLT_LOGIN_SCP02:
    case VLT_LOGIN_SCP03:
        
        /* Get the state of the secure channel authenticated user. */
        status = VltScpGetState( pu8State );

        break;
    case VLT_LOGIN_MS:
        
        /* MS has no state so return, user authenticated.*/
        *pu8State = VLT_USER_AUTHENTICATED;
        status = VLT_OK;

        break;
    default:
        /* Not authenticated */
        *pu8State =  VLT_USER_NOT_AUTHENTICATED;
        status = VLT_OK;
        break;
    }

    return ( status ); 
#else
    return( EMETHODNOTSUPPORTED );
#endif
}


