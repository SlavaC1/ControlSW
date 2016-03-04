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
 * Decleares the API interface relating to strong authentication
 */

#ifndef VAULTIC_STRONG_AUTHENTICATION_H
#define VAULTIC_STRONG_AUTHENTICATION_H

/*
* Defines
*/

/*
 * \fn VLT_STS VltStrongAuthenticate( 
	VLT_SA_CRYPTO_PARAMS* pCrytoParams, 
	VLT_SA_PROTOCOL_PARAMS* pProtocolParams);
 *
 * \brief performs the requested authentication protocol using the specified algorithm
 * parameters
 *
 * \par Description:
 *
 * This function will perform a strong authentication using the requested
 * cryptographic functions and protocol. The implementation can perform unilateral 
 * or mutual authentication and supports the FIPS required private key possession
 * assurance phase. Refer to the VaultIC device datasheet and the sample code for
 * more details
 *
 * \param pCryptoParams         [IN]  Cryptographic parameters
 * \param pProtocolParams       [IN]  protocol parameters
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltStrongAuthenticate( 
	VLT_SA_CRYPTO_PARAMS* pCryptoParams, 
	VLT_SA_PROTOCOL_PARAMS* pProtocolParams);

/**
  \fn VLT_STS VltStrongSetCryptoParams( 
	VLT_U8 id;
	VLT_SA_CRYPTO_PARAMS* pCrytoParams);
 *
 * \brief Initialises the cryptographic parameters structure with pre-defined settings
 *
 * \par Description:
 *
 * This function will is used to pre-initialise a VLT_SA_CRYPTO_PARAMS structure
 * with one of a set of pre-defined values.
 *
 * \param id			[IN]  identifer indicating which parameter set is required
 *							
 *  For ECDSA based strong authentication:
 *
 *	VLT_ECDSA_CURVE_B233	- 233 bit binary curve as defined in ANS9.62-2005
 *	VLT_ECDSA_CURVE_K233    - 233 bit Kobilitz curve as defined in ANS9.62-2005
 *	VLT_ECDSA_CURVE_B283	- 283 bit binary curve as defined in ANS9.62-2005
 *  VLT_ECDSA_CURVE_K283 	- 283 bit Koblitz curve as defined in ANS9.62-2005
 * 
 * \param pCryptoParams [IN]  pointer to a user allocated structure
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 */
VLT_STS VltStrongSetCryptoParams( 
	VLT_U8 paramId,
	VLT_SA_CRYPTO_PARAMS* pCryptoParams);

/**
 * \fn VltStrongClose( void )
 *
 * \brief Logs out the currently authenticated user.
 *
 * \return Status.
 */
VLT_STS VltStrongClose( void );

/**
 * \fn VltStrongGetState( VLT_PU8 pu8State )
 *
 * \brief Returns the state of the authenticated user.
 *
 * \return state of the authenticated user.
 */
VLT_STS VltStrongGetState( VLT_PU8 pu8State );


#endif /* VAULTIC_STRONG_AUTHENTICATION_H */
