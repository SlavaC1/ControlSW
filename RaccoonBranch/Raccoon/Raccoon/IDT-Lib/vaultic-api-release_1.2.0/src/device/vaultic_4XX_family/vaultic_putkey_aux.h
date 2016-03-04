/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_putkey_aux.h
 *
 * \brief Auxiliary functions for key management.
 *
 * \par Description:
 * This file declares functions for key management.
 */

#ifndef VAULTIC_KEY_AUX_H
#define VAULTIC_KEY_AUX_H

#include "vaultic_typedefs.h"

/**
 * * \file vaultic_putkey_aux.h
 * \fn VltPutKey_Raw(VLT_U8 u8KeyGroup,
           VLT_U8 u8KeyIndex,
           const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
           const VLT_KEY_OBJ_RAW* pKeyObj,
           VLT_PSW pSW)
 *
 * \brief Imports a raw key into the internal Key Ring.
 * \par Description:
 * See VltPutKey().
 */
VLT_STS VltPutKey_Raw(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
    const VLT_KEY_OBJ_RAW* pKeyObj,
    VLT_PSW pSW);

/**
 * \fn VltPutKey_Secret(VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,
 *         const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
 *         VLT_U8 u8KeyID,
 *         const VLT_KEY_OBJ_SECRET* pKeyObj,
 *         VLT_PSW pSW)
 *
 * \brief Imports a secret key into the internal Key Ring.
 * \par Description:
 * See VltPutKey().
 */
VLT_STS VltPutKey_Secret(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
    VLT_U8 u8KeyID,
    const VLT_KEY_OBJ_SECRET* pKeyObj,
    VLT_PSW pSW);

/**
 * \fn VltPutKey_Hotp(VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,
 *         const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
 *         VLT_U8 u8KeyID,
 *         const VLT_KEY_OBJ_HOTP* pKeyObj,
 *         VLT_PSW pSW)
 *
 * \brief Imports an HOTP key into the internal Key Ring.
 * \par Description:
 * See VltPutKey().
 */
VLT_STS VltPutKey_Hotp(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
    VLT_U8 u8KeyID,
    const VLT_KEY_OBJ_HOTP* pKeyObj,
    VLT_PSW pSW);

/**
 * \fn VltPutKey_Totp(VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,
 *         const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
 *         VLT_U8 u8KeyID,
 *         const VLT_KEY_OBJ_TOTP* pKeyObj,
 *         VLT_PSW pSW)
 *
 * \brief Imports an TOTP key into the internal Key Ring.
 * \par Description:
 * See VltPutKey().
 */
VLT_STS VltPutKey_Totp(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
    VLT_U8 u8KeyID,
    const VLT_KEY_OBJ_TOTP* pKeyObj,
    VLT_PSW pSW);

/**
 * \fn VltPutKey_RsaPublic(VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,
 *         const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
 *         VLT_U8 u8KeyID,
 *         const VLT_KEY_OBJ_RSA_PUB* pKeyObj,
 *         VLT_PSW pSW)
 *
 * \brief Imports an RSA public key into the internal Key Ring.
 * \par Description:
 * See VltPutKey().
 */
VLT_STS VltPutKey_RsaPublic(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
    VLT_U8 u8KeyID,
    const VLT_KEY_OBJ_RSA_PUB* pKeyObj,
    VLT_PSW pSW);

/**
 * \fn VltPutKey_RsaPrivate(VLT_U8 u8KeyGroup,
 *          VLT_U8 u8KeyIndex,
 *         const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
 *         VLT_U8 u8KeyID,
 *         const VLT_KEY_OBJ_RSA_PRIV* pKeyObj,
 *         VLT_PSW pSW)
 *
 * \brief Imports an RSA private key into the internal Key Ring.
 * \par Description:
 * See VltPutKey().
 */
VLT_STS VltPutKey_RsaPrivate(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
    VLT_U8 u8KeyID,
    const VLT_KEY_OBJ_RSA_PRIV* pKeyObj,
    VLT_PSW pSW);

/**
 * \fn VltPutKey_RsaPrivateCrt(VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,
 *         const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
 *         VLT_U8 u8KeyID,
 *         const VLT_KEY_OBJ_RSA_PRIV_CRT* pKeyObj,
 *         VLT_PSW pSW)
 *
 * \brief Imports an RSA private CRT key into the internal Key Ring.
 * \par Description:
 * See VltPutKey().
 */
VLT_STS VltPutKey_RsaPrivateCrt(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
    VLT_U8 u8KeyID,
    const VLT_KEY_OBJ_RSA_PRIV_CRT* pKeyObj,
    VLT_PSW pSW);

/**
 * \fn VltPutKey_DsaPublic(VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,
 *         const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
 *         VLT_U8 u8KeyID,
 *         const VLT_KEY_OBJ_DSA_PUB* pKeyObj,
 *         VLT_PSW pSW)
 *
 * \brief Imports a DSA public key into the internal Key Ring.
 * \par Description:
 * See VltPutKey().
 */
VLT_STS VltPutKey_DsaPublic(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
    VLT_U8 u8KeyID,
    const VLT_KEY_OBJ_DSA_PUB* pKeyObj,
    VLT_PSW pSW);

/**
 * \fn VltPutKey_DsaPrivate(VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,
 *         const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
 *         VLT_U8 u8KeyID,
 *         const VLT_KEY_OBJ_DSA_PRIV* pKeyObj,
 *         VLT_PSW pSW)
 *
 * \brief Imports a DSA private key into the internal Key Ring.
 * \par Description:
 * See VltPutKey().
 */
VLT_STS VltPutKey_DsaPrivate(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
    VLT_U8 u8KeyID,
    const VLT_KEY_OBJ_DSA_PRIV* pKeyObj,
    VLT_PSW pSW);

/**
 * \fn VltPutKey_EcdsaPublic(VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,
 *         const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
 *         VLT_U8 u8KeyID,
 *         const VLT_KEY_OBJ_ECDSA_PUB* pKeyObj,
 *         VLT_PSW pSW)
 *
 * \brief Imports an ECDSA public key into the internal Key Ring.
 * \par Description:
 * See VltPutKey().
 */
VLT_STS VltPutKey_EcdsaPublic(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
    VLT_U8 u8KeyID,
    const VLT_KEY_OBJ_ECDSA_PUB* pKeyObj,
    VLT_PSW pSW);

/**
 * \fn VltPutKey_EcdsaPrivate(VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,
 *         const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
 *         VLT_U8 u8KeyID,
 *         const VLT_KEY_OBJ_ECDSA_PRIV* pKeyObj,
 *         VLT_PSW pSW)
 *
 * \brief Imports an ECDSA private key into the internal Key Ring.
 * \par Description:
 * See VltPutKey().
 */
VLT_STS VltPutKey_EcdsaPrivate(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
    VLT_U8 u8KeyID,
    const VLT_KEY_OBJ_ECDSA_PRIV* pKeyObj,
    VLT_PSW pSW);

/**
 * \fn VltPutKey_DsaParams(VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,
 *         const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
 *         VLT_U8 u8KeyID,
 *         const VLT_KEY_OBJ_DSA_PARAMS* pKeyObj,
 *         VLT_PSW pSW)
 *
 * \brief Imports a DSA Parameters key into the internal Key Ring.
 * \par Description:
 * See VltPutKey().
 */
#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)

VLT_STS VltPutKey_DsaParams( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
    VLT_U8 u8KeyID,
    const VLT_KEY_OBJ_DSA_PARAMS* pKeyObj,
    VLT_PSW pSW );

VLT_STS VltPutKey_EcdsaParams( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
    VLT_U8 u8KeyID,
    const VLT_KEY_OBJ_ECDSA_PARAMS* pKeyObj,
    VLT_PSW pSW );

VLT_STS VltPutKey_IdKey( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
    VLT_U8 u8KeyID,
    const VLT_KEY_OBJ_ID* pKeyObj,
    VLT_PSW pSW );

VLT_STS VltPutKey_EcdhPublic(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
    VLT_U8 u8KeyID,
	const VLT_KEY_OBJ_ECDH_PUB* pKeyObj,
    VLT_PSW pSW);

VLT_STS VltPutKey_EcdhPrivate(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
    VLT_U8 u8KeyID,
	const VLT_KEY_OBJ_ECDH_PRIV* pKeyObj,
    VLT_PSW pSW);

#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */ 

#endif /* VAULTIC_KEY_AUX_H */
