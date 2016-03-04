/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_aes.h
 * 
 * \brief Implementation of the AES Cipher based on the common cipher interface. 
 * 
 * \par Description:
 * This file declares the specific AES cipher methods that Implement the AES 
 * cipher based on the common cipher interface. \see vaultic_cipher
 */
#ifndef VAULTIC_AES_H
#define VAULTIC_AES_H

#ifdef __cplusplus
    extern "C"
    {
#endif

    /**
     * \fn AesInit( VLT_U8 opMode, const KEY_BLOB* pKey, VLT_PU8 pParams )
     *
     * \brief Initialises the AES cipher.
     *
     * \par Description:
     * This method is the AES concrete implementation of the CipherInit method of the
     * common cipher interface. \see CipherInit.
     *
     * \return Status
     */
    VLT_STS AesInit( VLT_U8 opMode, const KEY_BLOB* pKey, VLT_PU8 pParams );

    /**
     * \fn AesClose( void )
     *
     * \brief Releases resources used by the AES Cipher.
     *
     * \par Description:
     * This method is the AES concrete implementation of the CipherClose method of the
     * common cipher interface. \see CipherClose.
     *
     * \return Status
     */
    VLT_STS AesClose( void );

    /**
     * \fn AesDoFinal( VLT_PU8 pDataIn, VLT_PU32 pDataInLen, VLT_PU8 pDataOut, VLT_PU32 pDataOutLen ) 
     *
     * \brief Performs the last step of the encryption/decryption process for the AES cipher.
     *
     * \par Description:
     * This method is the AES concrete implementation of the CipherDoFinal method of the
     * common cipher interface. \see CipherDoFinal.
     *
     * \return Status
     */
    VLT_STS AesDoFinal( VLT_PU8 pDataIn, VLT_PU32 pDataInLen, VLT_PU8 pDataOut, VLT_PU32 pDataOutLen );

    /**
     * \fn AesUpdate( VLT_PU8 pDataIn, VLT_PU32 pDataInLen, VLT_PU8 pDataOut, VLT_PU32 pDataOutLen )
     *
     * \brief Performs part of the encryption/decryption process for the AES cipher.
     *
     * \par Description:
     * This method is the AES concrete implementation of the CipherUpdate method of the
     * common cipher interface. \see CipherUpdate.
     *
     * \return Status
     */
    VLT_STS AesUpdate( VLT_PU8 pDataIn, VLT_PU32 pDataInLen, VLT_PU8 pDataOut, VLT_PU32 pDataOutLen );

    /**
     * \fn AesGetBlockSize
     *
     * \brief Returns the AES cipher block size.
     *
     * \par Description:
     * This method is the AES concrete implementation of the CipherGetBlockSize method of the
     * common cipher interface. \see CipherGetBlockSize.
     *
     * \return Status
     */
    VLT_U16 AesGetBlockSize( void );
    VLT_STS AesTest( void );

#ifdef __cplusplus
    };
#endif


#endif//VAULTIC_AES_H
