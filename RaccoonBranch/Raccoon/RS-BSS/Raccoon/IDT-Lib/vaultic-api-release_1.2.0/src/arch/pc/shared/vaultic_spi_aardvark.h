/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_spi_aardvark.h
 *
 * \brief SPI Aardvark interface for the VaultIC API.
 *
 * \par Description:
 * TBD.
 */

#ifndef VAULTIC_SPI_AARDVARK_H
#define VAULTIC_SPI_AARDVARK_H

#include "vaultic_peripheral.h"

VLT_STS VltSpiAardvarkInit( VLT_INIT_COMMS_PARAMS* pInitCommsParams );
VLT_STS VltSpiAardvarkClose( void );
VLT_STS VltSpiAardvarkIoctl( VLT_U32 u32Id, void* pConfigData );
VLT_STS VltSpiAardvarkSendData( VLT_MEM_BLOB *pOutData,
    VLT_U8 u8DelayArraySz,
    VLT_DELAY_PAIRING* pDelayPairing);

VLT_STS VltSpiAardvarkReceiveData( VLT_MEM_BLOB *pInData );

#endif /*VAULTIC_SPI_AARDVARK_H*/
