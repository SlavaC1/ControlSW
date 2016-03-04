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

#ifndef VAULTIC_SECURE_CHANNEL_H
#define VAULTIC_SECURE_CHANNEL_H

/*
* Defines
*/

#define SECURE_CHANNEL_SEND       (VLT_U8)0x00
#define SECURE_CHANNEL_RECEIVE    (VLT_U8)0x01

#define SCPXX_MAX_SESSION_KEY_LEN (VLT_U8)0x20
#define SCPXX_MAX_CMAC_LEN        (VLT_U8)0x10
#define SCPXX_MAX_RMAC_LEN        (VLT_U8)0x10

/**
 * \fn VltScpInit( VLT_U8 u8UserID, 
        VLT_U8 u8RoleID, 
        VLT_U8 u8ChannelLevel, 
        KEY_BLOB* pSMac, 
        KEY_BLOB* pSEnc )
 *
 * \brief Initialise the Secure Channel.
 *
 * \return Status.
 */
VLT_STS VltScpInit( VLT_U8 u8UserID, 
    VLT_U8 u8RoleID, 
    VLT_U8 u8ChannelLevel, 
    KEY_BLOB* pSMac, 
    KEY_BLOB* pSEnc );

/**
 * \fn VltScpClose( void )
 *
 * \brief Close the Secure Channel.
 *
 * \return Status.
 */
VLT_STS VltScpClose( void );

/**
 * \fn VltScpGetState( VLT_PU8 pu8State )
 *
 * \brief Returns the state of the Secure Channel.
 *
 * \return state of the Secure Channel.
 */
VLT_STS VltScpGetState( VLT_PU8 pu8State );

/**
 * \fn VltScpWrap( VLT_MEM_BLOB *pCmd )
 *
 * \brief Wrap the command being sent to the VaultIC.
 *
 * \return Status.
 */
VLT_STS VltScpWrap( VLT_MEM_BLOB *pCmd );

/**
 * \fn VltScpUnwrap( VLT_MEM_BLOB *pRsp )
 *
 * \brief Unwrap the response from the VaultIC.
 *
 * \return Status.
 */
VLT_STS VltScpUnwrap( VLT_MEM_BLOB *pRsp );

/**
 * \fn VltScpGetChannelOverhead( VLT_U8 u8Mode, VLT_PU8 pu8Overhead )
 *
 * \brief Get the number of bytes required by the currently active channel.
 *
 * \return Status.
 */
VLT_STS VltScpGetChannelOverhead( VLT_U8 u8Mode, VLT_PU8 pu8Overhead );

#endif /*VAULTIC_SECURE_CHANNEL_H*/
