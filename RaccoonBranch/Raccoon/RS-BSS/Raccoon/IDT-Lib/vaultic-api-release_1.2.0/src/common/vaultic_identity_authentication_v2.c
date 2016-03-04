/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_identity_authentication_v2.c
 * 
 * \brief Interface to identity authentication version 2.
 * 
 * \par Description:
 * Provides the host with the ability to authenticate users who have the 
 * following authentication methods:
 * Secure Channel 02.
 * Secure Channel 03.
 * Microsoft Card Minidriver.
 * Strong Authentication.
 * This interface replaces the old interface as it is extendable.
 */

#include "vaultic_common.h"
#include "vaultic_identity_authentication_v2.h"

#if( ( VLT_ENABLE_SCP02 == VLT_ENABLE ) || ( VLT_ENABLE_SCP03 == VLT_ENABLE ) )
    #include "vaultic_secure_channel.h"
#endif /* ( ( VLT_ENABLE_SCP02 == VLT_ENABLE ) || ( VLT_ENABLE_SCP03 == VLT_ENABLE ) ) */

#if( VLT_ENABLE_MS_AUTH == VLT_ENABLE )
    #include "vaultic_ms_auth.h"
#endif /* ( VLT_ENABLE_MS_AUTH == VLT_ENABLE ) */

#include "vaultic_strong_authentication.h"
#include "vaultic_api.h"




/*
* Defines
*/
#define SMAC_STATIC_KEY_INDEX 0
#define SENC_STATIC_KEY_INDEX 1
#define UNDEFINED_STATE 255


#if( VLT_ENABLE_IDENTITY_AUTH_V2 == VLT_ENABLE )
/*
 * Private Variable Definitions
 */
static VLT_U8 u8AuthState = UNDEFINED_STATE;
#endif /* #if( VLT_ENABLE_IDENTITY_AUTH_V2 == VLT_ENABLE ) */



VLT_STS VltAuthInit( VLT_IDENTITY_AUTH* pAuthParams )
{
    VLT_STS status = VLT_FAIL;
#if( VLT_ENABLE_IDENTITY_AUTH_V2 == VLT_ENABLE )    
    
    VLT_U8 i = 0;
    
    /*
     * Check the input parameter is valid.
     */    
    if( NULL == pAuthParams )
    {
        return( EID2INITNULLPARAM );
    }

    
    /* 
     * Close the currently active login. 
     */
    status = VltAuthClose( );


    if ( VLT_OK == status )
    {
        /*
         * Determine which type of channel the user has request.
         */
        switch ( pAuthParams->u8AuthMethod )
        {
        #if( ( VLT_ENABLE_SCP02 == VLT_ENABLE ) || ( VLT_ENABLE_SCP03 == VLT_ENABLE ) )
            case VLT_LOGIN_SCP02:
            case VLT_LOGIN_SCP03:
                
                /*
                 * Check the keys are valid before making the call, as the ScpInit
                 * method does not take a KEY_BLOB_ARRAY so the KEY_BLOBs held 
                 * within must be checked to prevent using a NULL ptr.
                 */
                for( i = 0; i < pAuthParams->data.SecureChannelAuth.keys.u8ArraySize; i++ )
                {
                    if( NULL == pAuthParams->data.SecureChannelAuth.keys.pKeys[i] )
                    {
                        return( EID2NULLBLOBBASE + i );
                    }                
                }

                            
                /* 
                 * Call the secure channel init method
                 */
                status = VltScpInit( pAuthParams->data.SecureChannelAuth.u8UserID, 
                    pAuthParams->data.SecureChannelAuth.u8RoleID, 
                    pAuthParams->data.SecureChannelAuth.u8ChannelLevel,
                    pAuthParams->data.SecureChannelAuth.keys.pKeys[SMAC_STATIC_KEY_INDEX],
                    pAuthParams->data.SecureChannelAuth.keys.pKeys[SENC_STATIC_KEY_INDEX] );

                
                /* 
                 * If the channel has been established, change the internal
                 * state variable to equal the type of secure channel. 
                 */
                if ( VLT_OK == status )
                {
                    u8AuthState = pAuthParams->u8AuthMethod;
                }

                break;
        #endif /* #if( ( VLT_ENABLE_SCP02 == VLT_ENABLE ) || ( VLT_ENABLE_SCP03 == VLT_ENABLE ) ) */
        
        #if ( VLT_ENABLE_MS_AUTH == VLT_ENABLE )
                case VLT_LOGIN_MS:
                
                /* 
                 * Call the Microsoft Card Minidriver init method.
                 */
                status = VltMsInit( pAuthParams->data.MicrosoftAuth.u8UserID, 
                    pAuthParams->data.MicrosoftAuth.u8RoleID, 
                    &pAuthParams->data.MicrosoftAuth.key );


                /*
                 * If the channel has been established, change the internal
                 * state variable to equal VLT_LOGIN_MS. 
                 */
                if ( VLT_OK == status )
                {
                    u8AuthState = pAuthParams->u8AuthMethod;
                }

                break;
        #endif /* #if ( VLT_ENABLE_MS_AUTH == VLT_ENABLE ) */

        
        #if ( VLT_ENABLE_STRONG_AUTH == VLT_ENABLE )
            case VLT_LOGIN_STRONG:
            
                /* 
                 * Call Strong Authentication 
                 */
                status = VltStrongInit( &pAuthParams->data.StrongAuth );

                /*
                 * If the user was authenticated set the
                 * state variable to equal VLT_LOGIN_STRONG. 
                 */
                if ( VLT_OK == status )
                {
                    u8AuthState = pAuthParams->u8AuthMethod;
                }

                break;
        #endif /* #if ( VLT_ENABLE_STRONG_AUTH == VLT_ENABLE ) */

        default:
            
            /* 
             * The requested authentication method is not supported. 
             */
            status = EID2AUTHMETHODNS;
        }    
    }    
    return( status );
#else
    status = EMETHODNOTSUPPORTED;
#endif /* #if( VLT_ENABLE_IDENTITY_AUTH_V2 == VLT_ENABLE ) */

    return ( status );
}



VLT_STS VltAuthClose( void )
{
    #if( VLT_ENABLE_IDENTITY_AUTH_V2 == VLT_ENABLE )    
        VLT_STS status = VLT_FAIL;

    /* Check the internal state variable to determine what 
     * type of authentication is established. */
    switch ( u8AuthState )
    {
    #if( ( VLT_ENABLE_SCP02 == VLT_ENABLE ) || ( VLT_ENABLE_SCP03 == VLT_ENABLE ) )
        case VLT_LOGIN_SCP02:
        case VLT_LOGIN_SCP03:
            
            /* Log out the secure channel authenticated user. */
            status = VltScpClose( );
            
            break;
    #endif /* #if( ( VLT_ENABLE_SCP02 == VLT_ENABLE ) || ( VLT_ENABLE_SCP03 == VLT_ENABLE ) ) */

    #if( VLT_ENABLE_MS == VLT_ENABLE )
        case VLT_LOGIN_MS:
            /* Log out the MS authenticated user.*/
            status = VltMsClose( );
            
            break;
    #endif /* #if( VLT_ENABLE_MS == VLT_ENABLE ) */

    #if( VLT_ENABLE_STRONG_AUTH == VLT_ENABLE )
        case VLT_LOGIN_STRONG:
                
            /* Call Strong Authentication */
            status = VltStrongClose( );

            break;
    #endif /* #if( VLT_ENABLE_STRONG_AUTH == VLT_ENABLE ) */

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
#endif /* #if( VLT_ENABLE_IDENTITY_AUTH_V2 == VLT_ENABLE ) */
}

VLT_STS VltAuthGetState( VLT_PU8 pu8State )
{
#if( VLT_ENABLE_IDENTITY_AUTH_V2 == VLT_ENABLE )
    VLT_STS status = VLT_FAIL;

    if ( NULL == pu8State )
    {
        return ( EIDSTATENULLPARAM );
    }

    /* Check the internal state variable to determine what 
     * type of authentication is established. */
    switch ( u8AuthState )
    {
    #if( ( VLT_ENABLE_SCP02 == VLT_ENABLE ) || ( VLT_ENABLE_SCP03 == VLT_ENABLE ) )
        case VLT_LOGIN_SCP02:
        case VLT_LOGIN_SCP03:
            /* Get the state of the secure channel authenticated user. */
            status = VltScpGetState( pu8State );
            
            break;
    #endif /* #if( ( VLT_ENABLE_SCP02 == VLT_ENABLE ) || ( VLT_ENABLE_SCP03 == VLT_ENABLE ) ) */
    
    #if( VLT_ENABLE_MS == VLT_ENABLE )
        case VLT_LOGIN_MS:
            /* MS has no state so return, user authenticated. */
            *pu8State = VLT_USER_AUTHENTICATED;
            status = VLT_OK;
            
            break;
    #endif /* #if( ( VLT_ENABLE_MS == VLT_ENABLE ) */

    #if( VLT_ENABLE_STRONG_AUTH == VLT_ENABLE )
        case VLT_LOGIN_STRONG:
            /* Strong Authentication has no state so return, user authenticated. */
            *pu8State = VLT_USER_AUTHENTICATED;
            status = VLT_OK;

            break;
    #endif /* #if( ( VLT_ENABLE_STRONG_AUTH == VLT_ENABLE ) */

    default:
        /* Not authenticated */
        *pu8State =  VLT_USER_NOT_AUTHENTICATED;
        status = VLT_OK;

        break;
    }

    return ( status ); 
#else
    return( EMETHODNOTSUPPORTED );
#endif /* #if( VLT_ENABLE_IDENTITY_AUTH_V2 == VLT_ENABLE ) */
}


