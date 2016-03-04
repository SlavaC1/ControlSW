/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_scp03.h
 * 
 * \brief Interface to secure channel SCP03.
 * 
 * \par Description:
 * TBD.
 */

#ifndef __VAULTIC_SCP03_H__
#define __VAULTIC_SCP03_H__

VLT_STS VltScp03Init( VLT_U8 u8ChannelLevel, 
    KEY_BLOB* pSMac, 
    KEY_BLOB* pSEnc,
    VLT_PU8 pu8HostChal,
    VLT_U8 u8HostChalLen,
    VLT_INIT_UPDATE* pInitUpRsp );

VLT_STS VltScp03Close( void );
VLT_STS VltScp03Wrap( VLT_MEM_BLOB *pCmd );
VLT_STS VltScp03Unwrap( VLT_MEM_BLOB *pRsp );
VLT_STS VltScp03GetChannelOverhead( VLT_U8 u8Mode, VLT_PU8 pu8Overhead );

#endif /*__VAULTIC_SCP02_H__*/
