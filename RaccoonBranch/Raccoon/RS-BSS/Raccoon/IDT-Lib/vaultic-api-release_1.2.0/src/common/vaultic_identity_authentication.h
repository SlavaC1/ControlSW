/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_identity_authentication.h
 * 
 * \defgroup VaultIcIdentityAuthentication Vault IC Identity Authentication.
 *
 * \brief Interface to identity authentication.
 * 
 * \par Description:
 * Provides the host with the ability to authenticate users who have the 
 * following authentication methods:
 * - Secure Channel 02.
 * - Secure Channel 03.
 * - Microsoft Card Minidriver.
 */
/*@{*/

#ifndef VAULTIC_IDENTITY_AUTHENTICATION_H
#define VAULTIC_IDENTITY_AUTHENTICATION_H


/**
 * \fn VltAuthInit( VLT_U8 u8AuthMethod, VLT_U8 u8UserID, VLT_U8 u8RoleID, VLT_U8 u8ChannelLevel, KEY_BLOB_ARRAY keys )
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
 * On success, the user will be authenticated (logged into) the vaultic
 * target, providing access to most methods.
 *
 * \param u8AuthMethod [IN] Authentication Method, possible values are:
 * - #VLT_LOGIN_SCP02
 * - #VLT_LOGIN_SCP03
 * - #VLT_LOGIN_MS
 *
 * \param u8UserID [IN]  Operator ID (0..7). Possible values are: 
 * - #VLT_USER0 
 * - #VLT_USER1 
 * - #VLT_USER2 
 * - #VLT_USER3 
 * - #VLT_USER4 
 * - #VLT_USER5 
 * - #VLT_USER6 
 * - #VLT_USER7 
 *
 * \param u8RoleID [IN]  Role ID. Possible values are:
 * - #VLT_APPROVED_USER     
 * - #VLT_NON_APPROVED_USER 
 * - #VLT_MANUFACTURER      
 * - #VLT_ADMINISTRATOR
 * - #VLT_NON_APPROVED_ADMINISTRATOR
 * - #VLT_EVERYONE            
 *
 * \param u8ChannelLevel [IN] Secure Channel Level (valid for SCP02 and SCP03) 
 *   possible values are:
 * - #VLT_NO_CHANNEL
 * - #VLT_CMAC
 * - #VLT_CMAC_CENC
 * - #VLT_CMAC_RMAC
 * - #VLT_CMAC_CENC_RMAC
 * - #VLT_CMAC_CENC_RMAC_RENC 
 *
 * \param keys [IN] a structure containing a the number of keys and a pointer 
 *   to an array of type KEY_BLOB.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned.
 *  
 * \par Example:
 * \code 
 *  // This example is for an approved user who’s auth method is SCP03.
 *  VLT_STS status = VLT_FAIL
 *  KEY_BLOB keys[2] =
 *  {
 *      { VLT_KEY_AES_256, AES_256_KEY_SIZE, &aucSMacKey[0] },
 *      { VLT_KEY_AES_256, AES_256_KEY_SIZE, &aucSEncKey[0] }
 *  };
 *
 *  KEY_BLOB_ARRAY arrayOfKeys = 
 *  {
 *      2, // The number of keys
 *      &keys[0], // KEY_BLOB 1
 *      &keys[1]  // KEY_BLOB 2
 *  };
 * 
 *  
 *  // The "auth" pointer is a pointer to VAULTIC_AUTH
 *  status = auth->VltAuthInit(VLT_LOGIN_SCP03, 
 *      VLT_USER0, 
 *      VLT_APPROVED_USER, 
 *      VLT_CMAC_CENC_RMAC_RENC, 
 *      arrayOfKeys );
 *  
 *  if( VLT_OK != status )
 *  {
 *      return( VltApiClose() );
 *  }
 * \endcode
 */
VLT_STS VltAuthInit( VLT_U8 u8AuthMethod,
    VLT_U8 u8UserID, 
    VLT_U8 u8RoleID, 
    VLT_U8 u8ChannelLevel, 
    KEY_BLOB_ARRAY keys );

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
#endif /*VAULTIC_IDENTITY_AUTHENTICATION_H*/
