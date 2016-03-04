/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_block_protocol.h
 * 
 * \brief Interface to block protocol.
 * 
 * \par Description:
 * TBD.
 */

#ifndef VAULTIC_BLOCK_PROTOCOL_H
#define VAULTIC_BLOCK_PROTOCOL_H

#define VLT_BLOCK_PROTOCOL_HDR_SZ (VLT_U8)0x03
#define VLT_BLOCK_PROTOCOL_TRL_SZ (VLT_U8)0x02
#define VLT_BLOCK_PROTOCOL_OH     (VLT_U16)VLT_BLOCK_PROTOCOL_HDR_SZ + VLT_BLOCK_PROTOCOL_TRL_SZ

#define BLK_PTCL_BLOCK_TYPE_OFFSET   (VLT_U8)0
#define BLK_PTCL_LEN_MSB_OFFSET      (VLT_U8)1
#define BLK_PTCL_LEN_LSB_OFFSET      (VLT_U8)2

VLT_STS VltBlkPtclInit( VLT_INIT_COMMS_PARAMS* pInitCommsParams,
    VLT_MEM_BLOB *pOutData,
    VLT_MEM_BLOB *pInData,
    VLT_PU16 pu16MaxSendSize,
    VLT_PU16 pu16MaxReceiveSize );

VLT_STS VltBlkPtclClose( void );

VLT_STS VltBlkPtclSendReceiveData( VLT_MEM_BLOB *pOutData, VLT_MEM_BLOB *pInData );

#endif /*VAULTIC_BLOCK_PROTOCOL_H*/
