/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_id_key_aux.h
 *
 * \brief Auxiliary functions for key management.
 *
 * \par Description:
 * This file declares functions for key management.
 */

#ifndef VAULTIC_ID_KEY_AUX_H
#define VAULTIC_ID_KEY_AUX_H

#include "vaultic_typedefs.h"


VLT_STS VltPutKey_IdKey( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
    VLT_U8 u8KeyID,
    const VLT_KEY_OBJ_ID* pKeyObj,
    VLT_PSW pSW );


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
    


#endif /* VAULTIC_KEY_AUX_H */
