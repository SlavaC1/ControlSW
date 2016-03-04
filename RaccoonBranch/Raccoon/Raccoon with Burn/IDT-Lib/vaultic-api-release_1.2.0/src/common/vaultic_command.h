/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_command.h
 * 
 * \brief Functions for issuing the VaultIC commands.
 * 
 * \par Description:
 * This file declares functions used to issue commands to the VaultIC.
 */

#ifndef VAULTIC_COMMAND_H
#define VAULTIC_COMMAND_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \fn VltCommand(VLT_MEM_BLOB *command, 
 *         VLT_MEM_BLOB *response,
 *         VLT_U16 u16Send, 
 *         VLT_U16 u16Require, 
 *         VLT_PSW pSW)
 *
 * \brief Issues a VaultIC command.
 *
 * \par Description:
 * This issues a command to the VaultIC and deals with calling Get Response
 * where appropriate.
 *
 * \param[in]  command    Command blob.
 * \param[in]  response   Response blob.
 * \param[in]  u16Send    Amount of data to send.
 * \param[in]  u16Require Amount of data to require. Will return an error if
 *                        this amount is not returned.
 * \param[out] pSW        Status word.
 *
 * \return Status.
 */
VLT_STS VltCommand(VLT_MEM_BLOB *command, 
    VLT_MEM_BLOB *response,
    VLT_U16 u16Send, 
    VLT_U16 u16Require, 
    VLT_PSW pSW);

/**
 * \fn VltCase4(VLT_U8 u8Ins,
 *         VLT_U8 u8P2,
 *         VLT_U32 u32SrcLen,
 *         const VLT_U8 *pu8Src,
 *         VLT_PU32 pu32DstLen,
 *         VLT_PU8 pu8Dst,
 *         VLT_PSW pSW)
 *
 * \brief Common implementation for Case 4 commands.
 *
 * \par Description:
 * This provides a common implementation for case 4 commands.
 *
 * \param[in]  u8Ins      INS
 * \param[in]  u8P2       P2
 * \param[in]  u32SrcLen  Bytes to send.
 * \param[in]  pu8Src     Buffer to send bytes from.
 * \param[out] pu32DstLen On entry this holds the maximum size of the receive
 *                        buffer. On exit it is set to the amount of receive
 *                        buffer used.
 * \param[out] pu8Dst     Buffer to receive bytes.
 * \param[out] pSW        Status word
 *
 * \return Status.
 */
VLT_STS VltCase4(VLT_U8 u8Ins,
    VLT_U8 u8P2,
    VLT_U32 u32SrcLen,
    const VLT_U8 *pu8Src,
    VLT_PU32 pu32DstLen,
    VLT_PU8 pu8Dst,
    VLT_PSW pSW);

#ifdef __cplusplus
}
#endif

#endif /* VAULTIC_COMMAND_H */
