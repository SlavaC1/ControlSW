/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_identity_authentication2.h
 * 
 * \defgroup VaultIcIdentityAuthentication Vault IC Identity Authentication.
 *
 * \brief Interface to identity authentication2.
 * 
 * \par Description:
 * Provides the host with a flexable method for user authentication 
 *  who have the following authentication methods:
 * - Secure Channel 02.
 * - Secure Channel 03.
 * - Microsoft Card Minidriver.
 * - Strong Authentication (NEW) 
 *
 * Version 2 provides a flexible interface that is extensible for future 
 * versions of the VaultIC and new authentication methods.  
 * Please note version 1, and version 2 of this interface can not be used
 * at the same time. 
 */
 /*@{*/

#ifndef VAULTIC_IDENTITY_AUTHENTICATION_V2_H
#define VAULTIC_IDENTITY_AUTHENTICATION_V2_H


/**
 * \fn VltAuthInit( VLT_IDENTITY_AUTH* pAuthParams )
 *
 * \brief Initialise the identity authentication service.
 *
 * \par Description:
 *
 * This service shall be used exclusively to authenticated users who have 
 * one of the following authentication methods:
 * Secure Channel 02.
 * Secure Channel 03.
 * Microsoft Card Minidriver.
 * Strong Authentication. 
 * On success, the user will be authenticated (logged into) the vaultic
 * target, providing access to most methods.
 *
 * \param AuthParams [IN] Identity Authentication parameters structure.
 */
VLT_STS VltAuthInit( VLT_IDENTITY_AUTH* pAuthParams );

/**
 * \fn VltAuthClose( void )
 *
 * \brief Close the identity authentication service.
 *
 * \par Description:
 *
 * Loggs the current user out of the vaultic and closes an open secure channel.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned.
 */
VLT_STS VltAuthClose( void );

/**
 * \fn VltAuthGetState( VLT_PU8 pu8State )
 *
 * \brief Returns the state of the identity authentication service.
 *
 * \par Description:
 * 
 * provides the host with the authentication (login) state of the 
 * the current user.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned.
 */
VLT_STS VltAuthGetState( VLT_PU8 pu8State );

/*@}*/
#endif /*VAULTIC_IDENTITY_AUTHENTICATION2_H*/
