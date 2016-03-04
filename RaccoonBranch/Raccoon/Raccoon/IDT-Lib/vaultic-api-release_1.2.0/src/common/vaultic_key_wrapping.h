/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_key_wrapping.h
 * 
 * \defgroup VaultIcKeyWrapping Vault IC Key Wrapping/Unwrapping
 *
 * \brief Interface functions to VaultIC Key Wrapping/Unwrapping Service.
 * 
 * \par Description:
 *
 * This file declares the interface functions for the VaultIC Key 
 * Wrapping/Unwrapping service.  The service provides the ability to
 * wrap keys before they are sent to the VaultIC or to unwrap keys
 * read from the VaultIC.  Use of the service is as follows:
 * 
 * \par
 * 1. Call VltKeyWrappingInit() to initialise the service with the necessary
 * parameters to allow the service to wrap/unwrap keys.
 * \par
 * 2a. Call VltWrapKey() supplying a pointer to the key object to be wrapped
 * and the file privileges for the key, or
 \par
 * 2b. Call VltUnwrapKey() supplying a pointer to a key object with the
 * appropriate space to place the key to be unwrapped.
 * \par
 * 3. Once complete call VltKeyWrappingClose() to close the Key 
 * Wrapping/Unwrapping service.
 *
 * \par
 * It should be noted that the Key Wrapping Service cannot be used over a
 * Secure Channel
 */
/*@{*/

#ifndef VAULTIC_KEY_WRAPPING_H
#define VAULTIC_KEY_WRAPPING_H

/**
 * \fn VltKeyWrappingInit( VLT_U8 u8KTSKeyGroup,
    VLT_U8 u8KTSKeyIndex,
    WRAP_PARAMS* pWrapParams,
    VLT_KEY_OBJECT* pKTSKey );
 *
 * \brief Used to initialise the Key Wrapping/Unwrapping Service.
 *
 * \par Description:
 *
 * This method is used to initialise the key wrapping/unwrapping service.
 * The #WRAP_PARAMS structure passed in should be populated with the 
 * appropriate values to select the algorithm used to wrap/unwrap the key
 *
 * \param u8KTSKeyGroup   [IN]  Key Group index of the Key Transport Scheme.
 * \param u8KTSKeyIndex   [IN]  Key index of the Key Transport Scheme.
 * \param pWrapParams     [IN]  The parameters used to wrap/unwrap the key.
 * \param pKTSKey         [IN]  The KTS key used to encrypt/decrypt the key
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 *
 */
VLT_STS VltKeyWrappingInit( VLT_U8 u8KTSKeyGroup,
    VLT_U8 u8KTSKeyIndex,
    WRAP_PARAMS* pWrapParams,
    VLT_KEY_OBJECT* pKTSKey );

/**
 * \fn VltUnWrapKey( VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,
 *         const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
 *         const VLT_KEY_OBJ_RAW* pKeyObj )
 *
 * \brief Used to wrap the key data.
 *
 * \par Description:
 *
 * This method is used to wrap the key data.
 *
 * \param u8KeyGroup         [IN]  Key Group index.
 * \param u8KeyIndex         [IN]  Key index.
 * \param pKeyFilePrivileges [IN]  Pointer to the privileges for the key being 
 *                                 put down to the VaultIC.
 * \param pKeyObj            [IN]  Pointer to the key object to pbe put down to
 *                                 the VaultIC
 * 
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 *
 */
VLT_STS VltUnwrapKey( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
    const VLT_KEY_OBJ_RAW* pKeyObj );

/**
 * \fn VltWrapKey( VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,
 *         VLT_KEY_OBJ_RAW* pKeyObj )
 *
 * \brief Used to unwrap the key data.
 *
 * \par Description:
 *
 * This method is used to unwrap the key data.
 *
 * \param u8KeyGroup         [IN]  Key Group index.
 * \param u8KeyIndex         [IN]  Key index.
 * \param pKeyObj            [IN]  Pointer to a key object to be filled with
 *                                 the key being read from the VaultIC
 * 
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 *
 */
VLT_STS VltWrapKey( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_RAW* pKeyObj );

/**
 * \fn VltKeyWrappingClose( void );
 *
 * \brief Used to close the Key Wrapping/Unwrapping Service.
 *
 * \par Description:
 *
 * This method is used to close the Key Wrapping/Unwrapping Service
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 *
 */
VLT_STS VltKeyWrappingClose( void );


/*@}*/
#endif /*VAULTIC_KEY_WRAPPING_H*/

