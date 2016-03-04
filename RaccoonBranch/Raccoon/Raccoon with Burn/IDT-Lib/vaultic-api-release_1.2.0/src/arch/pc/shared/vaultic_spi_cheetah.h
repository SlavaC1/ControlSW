/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */
/**
 * \file vaultic_spi_cheetah.h
 *
 * \brief SPI Cheetah interface for the VaultIC API.
 *
 * \par Description:
 * TBD.
 */

#ifndef VAULTIC_SPI_CHEETAH_H
#define VAULTIC_SPI_CHEETAH_H

#include "vaultic_peripheral.h"

VLT_STS VltSpiCheetahInit( VLT_INIT_COMMS_PARAMS* pInitCommsParams );
VLT_STS VltSpiCheetahClose( void );
VLT_STS VltSpiCheetahIoctl( VLT_U32 u32Id, void* pConfigData );
VLT_STS VltSpiCheetahSendData( VLT_MEM_BLOB *pOutData,
    VLT_U8 u8DelayArraySz,
    VLT_DELAY_PAIRING* pDelayPairing );

VLT_STS VltSpiCheetahReceiveData( VLT_MEM_BLOB *pInData );

#endif /*VAULTIC_SPI_CHEETAH_H*/
