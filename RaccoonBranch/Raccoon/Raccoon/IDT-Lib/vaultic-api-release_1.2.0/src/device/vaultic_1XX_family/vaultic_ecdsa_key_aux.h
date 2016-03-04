/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_ecdsa_key_aux.h
 *
 * \brief Auxiliary functions for ecdsa key management.
 *
 * \par Description:
 * This file declares functions for key management.
 */

#ifndef VAULTIC_ECDSA_KEY_AUX_H
#define VAULTIC_ECDSA_KEY_AUX_H

#include "vaultic_typedefs.h"



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
 * \fn VltPutKey_EcdsaParams(VLT_U8 u8KeyGroup,
 *         VLT_U8 u8KeyIndex,
 *         const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
 *         VLT_U8 u8KeyID,
 *         const VLT_KEY_OBJ_DSA_PARAMS* pKeyObj,
 *         VLT_PSW pSW)
 *
 * \brief Imports a ECDSA Parameters key into the internal Key Ring.
 * \par Description:
 * See VltPutKey().
 */
VLT_STS VltPutKey_EcdsaParams( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
    VLT_U8 u8KeyID,
    const VLT_KEY_OBJ_ECDSA_PARAMS* pKeyObj,
    VLT_PSW pSW );



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
 



#endif /* VAULTIC_ECDSA_KEY_AUX_H */
