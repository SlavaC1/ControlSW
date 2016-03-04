/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_twi_peripheral.h
 * 
 * \brief TWI Peripheral interface for the VaultIC API.
 * 
 * \par Description:
 * TBD.
 */

#ifndef VAULTIC_TWI_PERIPHERAL_H
#define VAULTIC_TWI_PERIPHERAL_H

#include "vaultic_peripheral.h"

VLT_STS VltTwiPeripheralInit( VLT_INIT_COMMS_PARAMS* pInitCommsParams );
VLT_STS VltTwiPeripheralClose( void );
VLT_STS VltTwiPeripheralIoctl( VLT_U32 u32Id, void* pConfigData );
VLT_STS VltTwiPeripheralSendData( VLT_MEM_BLOB *pOutData,
    VLT_U8 u8DelayArraySz,
    VLT_DELAY_PAIRING* pDelayPairing  );

VLT_STS VltTwiPeripheralReceiveData( VLT_MEM_BLOB *pInData );

#endif /*VAULTIC_TWI_PERIPHERAL_H*/
