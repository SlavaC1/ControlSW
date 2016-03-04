/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_raw_key_aux.h
 *
 * \brief Auxiliary functions for key management.
 *
 * \par Description:
 * This file declares functions for key management.
 */

#ifndef VAULTIC_RAW_KEY_AUX_H
#define VAULTIC_RAW_KEY_AUX_H

#include "vaultic_typedefs.h"

/**
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
    
#endif /* VAULTIC_RAW_KEY_AUX_H */
