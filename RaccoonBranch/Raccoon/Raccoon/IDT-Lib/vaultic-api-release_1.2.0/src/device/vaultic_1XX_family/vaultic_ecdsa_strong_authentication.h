/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_strong_authentication.h
 * 
 * \brief Interface to strong authentication.
 * 
 * \par Description:
 * TBD.
 */

#ifndef VAULTIC_ECDSA_STRONG_AUTHENTICATION_H
#define VAULTIC_ECDSA_STRONG_AUTHENTICATION_H

/*
* Defines+
*/

/**
 * \fn VltEcdsaAuthenticationInit ( VLT_IDENTITY_AUTH* pAuthParams )
 *
 * \brief Initialise Strong Authentication.
 *
 * \return Status.
 */
VLT_STS VltEcdsaStrongAuthenticate(
	VLT_SA_CRYPTO_PARAMS* pCryptoParams, 
	VLT_SA_PROTOCOL_PARAMS* pProtocolParams);

VLT_STS VltEcdsaStrongSetCryptoParams( 
	VLT_U8 paramId,
	VLT_SA_CRYPTO_PARAMS* pCryptoParams);


/**
 * \fn VltEcdsaStrongClose( void )
 *
 * \brief Logs out the currently authenticated user.
 *
 * \return Status.
 */
VLT_STS VltEcdsaStrongClose( void );

/**
 * \fn VltEcdsaStrongGetState( VLT_PU8 pu8State )
 *
 * \brief Returns the state of the authenticated user.
 *
 * \return state of the authenticated user.
 */
VLT_STS VltEcdsaStrongGetState( VLT_PU8 pu8State );


#endif /* VAULTIC_STRONG_AUTHENTICATION_H */
