/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#ifndef VAULTIC_CIPHER_H
#define VAULTIC_CIPHER_H
/**
 * 
 * \brief The common cipher interface. 
 * 
 * \par Description:
 * This file declares the common cipher interface, all ciphers supported by 
 * the Vault IC adhere to this interface. It allows further ciphers to be added
 * without affecting existing code.
 */


/**
 * \struct CIPHER_PARAMS
 * 
 * \brief The cipher parameters. 
 * 
 * \par Description:
 * The _cipherParams structure is used to configure the cipher. Its members are:
 * \li AlignLeft algoID Specifies which cipher will be used.
 * \li AlignLeft paddingScheme Specifies which padding mode will be used.
 * \li AlignLeft chainMode Specifies which chain mode will be use.
 * \li AlignLeft pIV An array of characters used as the Initialisation Vector.
 */
/*typedef struct _cipherParams
{
    VLT_U8 algoID;
    VLT_U8 paddingScheme;
    VLT_U8 chainMode;
    VLT_PU8 pIV;
} CIPHER_PARAMS;*/
typedef struct _VltWrapParams CIPHER_PARAMS;


#ifdef __cplusplus
    extern "C"
    {
#endif

    /**
     * \fn CipherInit( VLT_U8 opMode, const KEY_BLOB* pKey, VLT_PU8 pParams )
     *
     * \brief Initialises the underlying cipher.
     *
     * \par Description:
     * This method initialises the underlying cipher for use in the mode specified by
     * the opMode parameter, using the secret key specified by the pKey parameter and 
     * configured by the pParams parameter.
     * Once a cipher has been initialised in can only be used in the mode specified, 
     * e.g. if the cipher was initialised for encryption then only encrypt operations 
     * can be carried out. To use the cipher for decryption the CipherInit method with
     * the appropriate parameters should be called before decryption operations can 
     * take place.
     *
     * \param[in]  opMode   The Operational mode VLT_CIPHER_ENCRYPT for encryption  
     *                      otherwise VLT_CIPHER_DECRYPT for decryption.
     * 
     * \param[in]  pKey     The key used to initialise the cipher see KEY_BLOB 
     *                      structure for further information.
     * 
     * \param[in]  pParams  This parameter configures the cipher to operate in a 
     *                      required mode. This parameter should be treated as opaque  
     *                      CIPHER_PARAMS should be passed casted to a VLT_PU8.
     *
     * \return Status
     */
    VLT_STS CipherInit( VLT_U8 opMode, const KEY_BLOB* pKey, VLT_PU8 pParams );

    /**
     * \fn CipherClose( void )
     *
     * \brief Releases resources used.
     *
     * \par Description:
     * This method clears any residual data used by the underlying cipher. 
     * It must be called at the end of an encrypt or decrypt sequence.
     *
     * \return Status
     */
    VLT_STS CipherClose( void );

    /**
     * \fn CipherDoFinal( 
     *         VLT_PU8 pDataIn, 
     *         VLT_U32 DataInLen, 
     *         VLT_U32 dataInCapacity, 
     *         VLT_PU8 pDataOut, 
     *         VLT_PU32 pDataOutLen, 
     *         VLT_U32 dataOutCapacity )
     *
     * \brief Performs the last step of the encryption/decryption process.
     *
     * \par Description:
     * This method performs the last step of the encryption or decryption depending
     * on the mode of operation specified during a call to the CipherInit method. 
     * Once the call to the CipherDoFinal has completed no more encryption/decryption
     * operations can take place until a further call to the CipherInit method has 
     * been issued.
     * Depending on how the cipher was configured during the call to the CipherInit
     * method, if a the padding method selected was PADDING_NONE and the block of 
     * data passed in was not multiple of the block size of the underlying cipher the
     * the call will fail with the appropriate error code.
     *
     * \param[in]  pDataIn         The array of data to be encrypted/decrypted.
     * 
     * \param[in]  DataInLen       The length of the array of data to be encrypted/decrypted.
     * 
     * \param[in]  dataInCapacity  The capacity of the array of data to be encrypted/decrypted.
     *
     * \param[out] pDataOut        The array of data that will hold the result of the 
     *                             encryption/decryption.
     * 
     * \param[out] pDataOutLen     The length of the array of data that holds the result of the 
     *                             encryption/decryption.
     *
     * \param[in]  dataOutCapacity The capacity of the array of data that holds the result of the 
     *                             encryption/decryption.
     *
     * \return Status
     */ 
    VLT_STS CipherDoFinal( 
        VLT_PU8 pDataIn, 
        VLT_U32 DataInLen, 
        VLT_U32 dataInCapacity, 
        VLT_PU8 pDataOut, 
        VLT_PU32 pDataOutLen, 
        VLT_U32 dataOutCapacity );

    /**
     * \fn CipherUpdate( 
     *         VLT_PU8 pDataIn, 
     *         VLT_U32 DataInLen, 
     *         VLT_U32 dataInCapacity, 
     *         VLT_PU8 pDataOut, 
     *         VLT_PU32 pDataOutLen, 
     *         VLT_U32 dataOutCapacity )
     *
     * \brief Performs part of the encryption/decryption process.
     *
     * \par Description:
     * This method performs part of the encryption/decryption process, this method is provided
     * in aid of the scenario where the data to be processed are not contiguous but need 
     * to be processed in smaller chunks due to target memory storage constraints. This method
     * can only be called once a call to the CipherInit method has completed successfully.
     * This method can be called multiple times.
     * Please note, this method only process data of length equal or multiple of the underlying 
     * cipher block size. The block size can be determined by making a call to the CipherGetBlockSize
     * method. If a the DataInLen specified isn't equal or multiple of the block size then the 
     * method will return the appropriate error code.
     *
     * \param[in]  pDataIn         The array of data to be encrypted/decrypted.
     * 
     * \param[in]  DataInLen       The length of the array of data to be encrypted/decrypted.
     * 
     * \param[in]  dataInCapacity  The capacity of the array of data to be encrypted/decrypted.
     *
     * \param[out] pDataOut        The array of data that will hold the result of the 
     *                             encryption/decryption.
     * 
     * \param[out] pDataOutLen     The length of the array of data that holds the result of the 
     *                             encryption/decryption.
     *
     * \param[in]  dataOutCapacity The capacity of the array of data that holds the result of the 
     *                             encryption/decryption.
     *
     * \return Status
     */ 
    VLT_STS CipherUpdate( 
        VLT_PU8 pDataIn, 
        VLT_U32 DataInLen, 
        VLT_U32 dataInCapacity, 
        VLT_PU8 pDataOut, 
        VLT_PU32 pDataOutLen, 
        VLT_U32 dataOutCapacity );

    /**
     * \fn CipherGetBlockSize
     *
     * \brief Returns the underlying cipher block size.
     *   
     * \return The underlying cipher block size.
     */
    VLT_U16 CipherGetBlockSize( void );

#ifdef __cplusplus
    };
#endif


#endif//VAULTIC_CIPHER_H
