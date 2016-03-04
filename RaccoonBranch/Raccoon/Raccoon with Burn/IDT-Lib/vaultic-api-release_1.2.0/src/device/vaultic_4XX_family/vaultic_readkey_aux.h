/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_readkey_aux.h
 * 
 * \brief Auxiliary functions for key management.
 * 
 * \par Description:
 * This file declares functions for key management.
 */

#ifndef VAULTIC_READKEY_AUX_H
#define VAULTIC_READKEY_AUX_H

#include "vaultic_typedefs.h"


/**
 * \fn ReadKeyInitCrc( void )
 *
 * \brief Initialises CRC for reading a key
 *
 * \return The current CRC.
 */
void ReadKeyInitCrc( void );

/**
 * \fn VltReadKeyCommand(VLT_MEM_BLOB *command,
 *         VLT_MEM_BLOB *response,
 *         VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,    
 *         VLT_PSW pSW)
 *
 * \brief Calls VltCommand to read a key, or part of a key.
 *
 * \par Description:
 * A hoisted out common call to VltCommand which copes with re-issuing the
 * command when VLT_STATUS_REISSUE is received.
 *
 * \param[in]  command    Command blob.
 * \param[in]  response   Response blob.
 * \param[in]  u8KeyGroup Key Group index.
 * \param[in]  u8KeyIndex Key index.
 * \param[out] pSW        Status word.
 *
 * \return Status.
 */
VLT_STS VltReadKeyCommand(VLT_MEM_BLOB *command,
    VLT_MEM_BLOB *response,
    VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,    
    VLT_PSW pSW);


/**
 * \fn VltReadKey_Raw(VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,
 *         VLT_KEY_OBJ_RAW* keyObj,
 *         VLT_PSW pSW)
 * \brief Exports a Raw key from the internal Key Ring.
 * \par Description:
 * See VltReadKey().
 */
VLT_STS VltReadKey_Raw(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_RAW* keyObj,
    VLT_PSW pSW);

#ifdef VLT_ENABLE_KEY_SECRET

/**
 * \fn VltReadKey_Secret( VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,
 *         VLT_KEY_OBJ_SECRET* keyObj,
 *         VLT_PSW pSW )
 * \brief Exports a secret key from the internal Key Ring.
 * \par Description:
 * See VltReadKey().
 */
VLT_STS VltReadKey_Secret( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_SECRET* keyObj,
    VLT_PSW pSW );

#endif /* VLT_ENABLE_KEY_SECRET */


#ifdef VLT_ENABLE_KEY_HOTP

/**
 * \fn VltReadKey_Hotp(VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,
 *         VLT_KEY_OBJ_HOTP* keyObj,
 *         VLT_PSW pSW)
 * \brief Exports an HOTP key from the internal Key Ring.
 * \par Description:
 * See VltReadKey().
 */
VLT_STS VltReadKey_Hotp(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_HOTP* keyObj,
    VLT_PSW pSW);

#endif /* VLT_ENABLE_KEY_HOTP */


#ifdef VLT_ENABLE_KEY_TOTP

/**
 * \fn VltReadKey_Totp(VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,
 *         VLT_KEY_OBJ_TOTP* keyObj,
 *         VLT_PSW pSW)
 * \brief Exports an TOTP key from the internal Key Ring.
 * \par Description:
 * See VltReadKey().
 */
VLT_STS VltReadKey_Totp(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_TOTP* keyObj,
    VLT_PSW pSW);

#endif /* VLT_ENABLE_KEY_TOTP */


#ifdef VLT_ENABLE_KEY_RSA

/**
 * \fn VltReadKey_RsaPublic(VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,
 *         VLT_KEY_OBJ_RSA_PUB* keyObj,
 *         VLT_PSW pSW)
 * \brief Exports an RSA public key from the internal Key Ring.
 * \par Description:
 * See VltReadKey().
 */
VLT_STS VltReadKey_RsaPublic(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_RSA_PUB* keyObj,
    VLT_PSW pSW);
/**
 * \fn VltReadKey_RsaPrivate(VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,
 *         VLT_KEY_OBJ_RSA_PRIV* keyObj,
 *         VLT_PSW pSW)
 * \brief Exports an RSA private key from the internal Key Ring.
 * \par Description:
 * See VltReadKey().
 */
VLT_STS VltReadKey_RsaPrivate(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_RSA_PRIV* keyObj,
    VLT_PSW pSW);
/**
 * \fn VltReadKey_RsaPrivateCrt(VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,
 *         VLT_KEY_OBJ_RSA_PRIV_CRT* keyObj,
 *         VLT_PSW pSW)
 * \brief Exports an RSA private CRT key from the internal Key Ring.
 * \par Description:
 * See VltReadKey().
 */
VLT_STS VltReadKey_RsaPrivateCrt(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_RSA_PRIV_CRT* keyObj,
    VLT_PSW pSW);

#endif /* VLT_ENABLE_KEY_RSA */


#ifdef VLT_ENABLE_KEY_DSA

/**
 * \fn VltReadKey_DsaPublic(VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,
 *         VLT_KEY_OBJ_DSA_PUB* keyObj,
 *         VLT_PSW pSW)
 * \brief Exports a DSA public key from the internal Key Ring.
 * \par Description:
 * See VltReadKey().
 */
VLT_STS VltReadKey_DsaPublic(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_DSA_PUB* keyObj,
    VLT_PSW pSW);

/**
 * \fn VltReadKey_DsaPrivate(VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,
 *         VLT_KEY_OBJ_DSA_PRIV* keyObj,
 *         VLT_PSW pSW)
 * \brief Exports a DSA private key from the internal Key Ring.
 * \par Description:
 * See VltReadKey().
 */
VLT_STS VltReadKey_DsaPrivate(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_DSA_PRIV* keyObj,
    VLT_PSW pSW);

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)

/**
 * \fn VltReadKey_DsaParams(VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,
 *         VLT_KEY_OBJ_DSA_PARAMS* keyObj,
 *         VLT_PSW pSW)
 * \brief Exports a DSA Params key from the internal Key Ring.
 * \par Description:
 * See VltReadKey().
 */
VLT_STS VltReadKey_DsaParams( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_DSA_PARAMS* keyObj,
    VLT_PSW pSW);

#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */ 

#endif /* VLT_ENABLE_KEY_DSA */


#ifdef VLT_ENABLE_KEY_ECDSA

/**
 * \fn VltReadKey_EcdsaPublic(VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,
 *         VLT_KEY_OBJ_ECDSA_PUB* keyObj,
 *         VLT_PSW pSW)
 * \brief Exports an ECDSA public key from the internal Key Ring.
 * \par Description:
 * See VltReadKey().
 */
VLT_STS VltReadKey_EcdsaPublic(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_ECDSA_PUB* keyObj,
    VLT_PSW pSW);

/**
 * \fn VltReadKey_EcdsaPrivate(VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,
 *         VLT_KEY_OBJ_ECDSA_PRIV* keyObj,
 *         VLT_PSW pSW)
 * \brief Exports an ECDSA private key from the internal Key Ring.
 * \par Description:
 * See VltReadKey().
 */
VLT_STS VltReadKey_EcdsaPrivate(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_ECDSA_PRIV* keyObj,
    VLT_PSW pSW);

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)
/**
 * \fn VltReadKey_EcdsaParams(VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,
 *         VLT_KEY_OBJ_ECDSA_PARAMS* keyObj,
 *         VLT_PSW pSW)
 * \brief Exports an ECDSA Params key from the internal Key Ring.
 * \par Description:
 * See VltReadKey().
 */
VLT_STS VltReadKey_EcdsaParams( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_ECDSA_PARAMS* keyObj,
    VLT_PSW pSW );
#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */ 

#endif /* VLT_ENABLE_KEY_ECDSA */

#ifdef VLT_ENABLE_KEY_ECDH
#if (VAULT_IC_VERSION == VAULTIC_VERSION_1_2_1)
/**
 * \fn VltReadKey_EcdhPublic(VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,
 *         VLT_KEY_OBJ_ECDH_PUB* keyObj,
 *         VLT_PSW pSW)
 * \brief Exports an ECDH public key from the internal Key Ring.
 * \par Description:
 * See VltReadKey().
 */
VLT_STS VltReadKey_EcdhPublic(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_ECDH_PUB* keyObj,
    VLT_PSW pSW);

/**
 * \fn VltReadKey_EcdhPrivate(VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,
 *         VLT_KEY_OBJ_ECDH_PRIV* keyObj,
 *         VLT_PSW pSW)
 * \brief Exports an ECDH private key from the internal Key Ring.
 * \par Description:
 * See VltReadKey().
 */
VLT_STS VltReadKey_EcdhPrivate(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_ECDH_PRIV* keyObj,
    VLT_PSW pSW);
#endif /* (VAULT_IC_VERSION == VAULTIC_VERSION_1_2_1) */
#endif

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)

/**
 * \fn VltReadKey_IdKey( VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,
 *         VLT_KEY_OBJ_ID* keyObj,
 *         VLT_PSW pSW)
 * \brief Exports a Host or Device ID key from the internal Key Ring.
 * \par Description:
 * See VltReadKey().
 */
VLT_STS VltReadKey_IdKey( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJ_ID* keyObj,
    VLT_PSW pSW );

#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */ 

#endif /* VAULTIC_READKEY_AUX_H */
