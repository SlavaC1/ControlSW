/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_spi_peripheral.h
 *
 * \brief SPI Peripheral interface for the VaultIC API.
 *
 * \par Description:
 * TBD.
 */

#ifndef VAULTIC_SPI_PERIPHERAL_H
#define VAULTIC_SPI_PERIPHERAL_H

#include "vaultic_peripheral.h"

VLT_STS VltSpiPeripheralInit( VLT_INIT_COMMS_PARAMS* pInitCommsParams );
VLT_STS VltSpiPeripheralClose( void );
VLT_STS VltSpiPeripheralIoctl( VLT_U32 u32Id, void* pConfigData );
VLT_STS VltSpiPeripheralSendData( VLT_MEM_BLOB *pOutData,
    VLT_U8 u8DelayArraySz,
    VLT_DELAY_PAIRING* pDelayPairing );

VLT_STS VltSpiPeripheralReceiveData( VLT_MEM_BLOB *pInData );

#endif /*VAULTIC_SPI_PERIPHERAL_H*/
