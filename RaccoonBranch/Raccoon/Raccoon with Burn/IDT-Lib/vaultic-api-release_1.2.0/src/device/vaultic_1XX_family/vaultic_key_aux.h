/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_dkey_aux.h
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


#endif /* VAULTIC_KEY_AUX_H */