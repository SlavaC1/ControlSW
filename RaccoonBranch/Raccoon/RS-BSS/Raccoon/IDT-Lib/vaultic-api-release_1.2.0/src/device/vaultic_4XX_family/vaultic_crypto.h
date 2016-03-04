/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#ifndef VAULTIC_CRYPTO_H
#define VAULTIC_CRYPTO_H

#ifdef __cplusplus
    extern "C"
    {
#endif


/**
 * \fn CryptoInit( const CRYPTO_SERVICE_KEY_OBJECT* pKey, 
 *      VLT_U8 u8OpMode, 
 *      const CRYPTO_SERVICE_ALGO_PARAMS* pAlgoParams );
 *
 * \brief Initialises the VaultIC API Crypto Service.
 * \par Description:
 *
 * The CryptoInit() method provides a software implementation for crypto 
 * service initialisation for the crypto library. 
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
VLT_STS CryptoInit( const CRYPTO_SERVICE_KEY_OBJECT* pKey, 
    VLT_U8 u8OpMode, 
    const CRYPTO_SERVICE_ALGO_PARAMS* pAlgoParams );


/**
 * \fn CryptoClose( void );
 *
 * \brief Closes the VaultIC API Crypto Service.
 * \par Description:
 *
 * The CryptoClose() method provides a software implementation for crypto 
 * service closeure and crypto resource cleanup. 
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned.
 *  
 */
VLT_STS CryptoClose( void );

    VLT_STS CryptoDoFinal( 
        VLT_PU8 pu8Message, 
        VLT_U32 u32messageLen, 
        VLT_U32 u32messageCapacity, 
        VLT_PU8 pu8Output, 
        VLT_PU32 pu32OutputLen, 
        VLT_U32 u32OutPutCapacity );


/**
 * \fn CryptoDoFinal(
 *      VLT_PU8 pu8Message, 
 *      VLT_U32 u32messageLen, 
 *      VLT_U32 u32messageCapacity, 
 *      VLT_PU8 pu8Output, 
 *      VLT_PU32 pu32OutputLen, 
 *      VLT_U32 u32OutPutCapacity );
 *
 * \brief performs the final operation for an initialised crypto service.
 * \par Description:
 *
 * The CryptoDoFinal() method provides a software implementation for crypto 
 * service final operation. 
 * Upon successful completion the answer will be returned from the crypto 
 * library.
 *
 * \param pu8Message         [IN]  pointer to a message buffer the crypto 
 *                                 service will use.
 *
 * \param u32messageLen      [IN]  The length of the message.
 * \param u32messageCapacity [IN]  The capacity of the message buffer.
 * \param pu8Output          [OUT] pointer to a result buffer the crypto 
 *                                 service will use.
 * \param pu32OutputLen      [OUT] The length of the output.
 * \param u32OutPutCapacity  [IN]  The capacity of the output buffer.
 *  
 * \return Upon successful completion a VLT_OK status will be returned 
 * otherwise the appropriate error code will be returned.
 *  
 */ 
VLT_STS CryptoDoFinal(
    VLT_PU8 pu8Message, 
    VLT_U32 u32messageLen, 
    VLT_U32 u32messageCapacity, 
    VLT_PU8 pu8Output, 
    VLT_PU32 pu32OutputLen, 
    VLT_U32 u32OutPutCapacity );



/**
 * \fn CryptoUpdate( VLT_PU8 pu8Message, 
 *      VLT_U32 u32MessageLen, 
 *      VLT_U32 u32MessageCapacity );
 *
 * \brief performs the update operation for an initialised crypto service.
 * \par Description:
 *
 * The CryptoUpdate() method provides a software implementation for crypto 
 * service operation. 
 * Upon successful completion an answer will be returned and stored 
 * interally for use on the next update or do final on the crypto service.
 *
 * \param pu8Message         [IN]  pointer to a message buffer the crypto 
 *                                 service will use.
 *
 * \param u32messageLen      [IN]  The length of the message.
 * \param u32messageCapacity [IN]  The capacity of the message buffer.
 * \param pu8Output          [OUT] pointer to a result buffer the crypto 
 *                                 service will use.
 * \param pu32OutputLen      [OUT] The length of the output.
 * \param u32OutPutCapacity  [IN]  The capacity of the output buffer.
 *  
 * \return Upon successful completion a VLT_OK status will be returned 
 * otherwise the appropriate error code will be returned.
 *  
 */
VLT_STS CryptoUpdate( VLT_PU8 pu8Message,
    VLT_U32 u32MessageLen, 
    VLT_U32 u32MessageCapacity,
    VLT_PU8 pu8Output, 
    VLT_PU32 pu32OutputLen, 
    VLT_U32 u32OutPutCapacity );


#ifdef __cplusplus
    };
#endif

#endif//VAULTIC_CRYPTO_H
