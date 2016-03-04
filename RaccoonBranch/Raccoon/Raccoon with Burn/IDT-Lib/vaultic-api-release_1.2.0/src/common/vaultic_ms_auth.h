/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_ms_auth.h
 * 
 * \brief Interface to Microsoft Card Minidriver Authentication.
 * 
 * \par Description:
 * TBD.
 */

#ifndef VAULTIC_MS_AUTH_H
#define VAULTIC_MS_AUTH_H


/**
 * \fn VltMsInit( VLT_U8 u8UserID, 
        VLT_U8 u8RoleID, 
        KEY_BLOB* pKeyTDes3k )
 *
 * \brief Initialise the Secure Channel.
 *
 * \return Status.
 */
VLT_STS VltMsInit( VLT_U8 u8UserID, 
    VLT_U8 u8RoleID, 
    KEY_BLOB* pKeyTDes3k );

/**
 * \fn VltMsClose( void )
 *
 * \brief Log out the current MS authenticated user.
 *
 * \return Status.
 */
VLT_STS VltMsClose( void );


#endif /*VAULTIC_MS_AUTH_H*/
