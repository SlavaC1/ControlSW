/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#ifndef VAULTIC_ECDSA_SIGNER_H
#define VAULTIC_ECDSA_SIGNER_H

#ifdef __cplusplus
    extern "C"
    {
#endif

/**
 * \fn EcdsaSignerInit( const CRYPTO_SERVICE_KEY_OBJECT* pKey, 
 *      VLT_U8 u8OpMode, 
 *      const CRYPTO_SERVICE_ALGO_PARAMS* pAlgoParams );
 *
 * \brief Initialises the VaultIC API ECDSA Signer Service.
 * \par Description:
 *
 * The EcdsaSignerInit() method provides a software implementation for ECDSA 
 * signing and verifying service initialisation. 
 * Upon successful completion a number of internal system resources would be 
 * allocated and used.  These resources will remain in use until a call to the 
 * VltApiClose() method is made.
 *
 * \param pKey     [IN] Key structure used to pass different key types.
 * \param u8OpMode [IN] The operation identifier, supported values:
 * -#VLT_SIGN_MODE
 * -#VLT_VERIFY_MODE
 *  
 * \param pAlgoParams [IN] algorithm initialization parameters.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned.
 *  
 */
VLT_STS EcdsaSignerInit(
	const VLT_ECDSA_DOMAIN_PARAMS* pDomainParams, 
	const VLT_ECDSA_PRIVATE_KEY* pPrivateKey, 
	const VLT_ECDSA_PUBLIC_KEY* pPublicKey, 
    VLT_U8 u8OpMode);

/**
 * \fn EcdsaSignerClose( void );
 *
 * \brief Closes the VaultIC API ECDSA Signing Service.
 * \par Description:
 *
 * The EcdsaSignerClose() method provides a software implementation for ECDSA 
 * signing service closeure and resource cleanup.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned.
 *  
 */
VLT_STS EcdsaSignerClose( void );


/**
 * \fn EcdsaSignerDoFinal(
 *      VLT_PU8 pu8Message, 
 *      VLT_U32 u32messageLen, 
 *      VLT_U32 u32messageCapacity, 
 *      VLT_PU8 pu8Output, 
 *      VLT_PU32 pu32OutputLen, 
 *      VLT_U32 u32OutPutCapacity );
 *
 * \brief performs a signing operation using ECDSA algorithm.
 * \par Description:
 *
 * The EcdsaSignerDoFinal() method provides a software implementation for ECDSA 
 * GF2M algorithm. The method can only be called once with the whole message or
 * the final block of the message. EcdsaSignerInit must be called first.
 *
 * \param pu8Message         [IN]  pointer to a message buffer the ECDSA signer 
 *                                 will use.
 *
 * \param u32messageLen      [IN]  The length of the message.
 * \param u32messageCapacity [IN]  The capacity of the message buffer.
 * \param pu8Signature       [OUT] pointer to a signature buffer the ECDSA 
 *                                 signer will use.
 * \param pu32SignatureLen   [OUT] The length of the signature.
 * \param u32SignatureCapacity[IN] The capacity of the signature buffer.
 *  
 * \return Upon successful completion a VLT_OK status will be returned 
 * otherwise the appropriate error code will be returned.
 *  
 */ 
VLT_STS EcdsaSignerDoFinal( 
    VLT_PU8 pu8Message, 
    VLT_U32 u32messageLen, 
    VLT_U32 u32messageCapacity, 
    VLT_PU8 pu8Signature, 
    VLT_PU32 pu32SignatureLen, 
    VLT_U32 u32SignatureCapacity );


/**
 * \fn EcdsaSignerUpdate( VLT_PU8 pu8Message, 
 *      VLT_U32 u32MessageLen, 
 *      VLT_U32 u32MessageCapacity );
 *
 * \brief performs a signing operation using ECDSA algorithm.
 * \par Description:
 *
 * The EcdsaSignerUpdate() method provides a software implementation for ECDSA 
 * GF2M algorithm. The method can be called multiple times with blocks of the
 * message to be signed. Call the do final on the last block to obtain the 
 * ECDSA signature.
 *
 * \param pu8Message         [IN]  pointer to a message buffer the ECDSA signer 
 *                                 will use.
 *
 * \param u32messageLen      [IN]  The length of the message.
 * \param u32messageCapacity [IN]  The capacity of the message buffer.
 *  
 * \return Upon successful completion a VLT_OK status will be returned 
 * otherwise the appropriate error code will be returned.
 *  
 */
VLT_STS EcdsaSignerUpdate( VLT_PU8 pu8Message, 
    VLT_U32 u32MessageLen, 
    VLT_U32 u32MessageCapacity );


#ifdef __cplusplus
    };
#endif

#endif // VAULTIC_ECDSA_SIGNER_H
