/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_peripheral.h
 * 
 * \brief Peripheral interface for the VaultIC API.
 * 
 * \par Description:
 * TBD.
 */

#ifndef VAULTIC_PERIPHERAL_H
#define VAULTIC_PERIPHERAL_H

#include "vaultic_common.h"

/*
* Function Pointer Definitions
*/
typedef VLT_STS (*pfnVltPtclInit)( VLT_INIT_COMMS_PARAMS* pInitCommsParams );

typedef VLT_STS (*pfnVltPtclClose)( void );

typedef VLT_STS (*pfnVltPeripheralIoctl)( VLT_U32 u32Id, void* pConfigData );

typedef VLT_STS (*pfnVltPeripheralSendData)( VLT_MEM_BLOB* pOutData, 
    VLT_U8 u8DelayArraySz,
    VLT_DELAY_PAIRING* pDelayPairing );

typedef VLT_STS (*pfnVltPeripheralReceiveData)( VLT_MEM_BLOB* pInData );

/**
 * \struct _VltPeripheral
 *
 * \brief Structure of function pointers used by the comms peripherals.
 */
typedef struct _VltPeripheral
{
    pfnVltPtclInit PeripheralInit;
    pfnVltPtclClose PeripheralClose;
    pfnVltPeripheralIoctl PeripheralIoctl;
    pfnVltPeripheralSendData PeripheralSendData;
    pfnVltPeripheralReceiveData PeripheralReceiveData;
} VltPeripheral;

/**
 * Defines
 */
#define VLT_AWAIT_DATA        (VLT_U32)0x00000000
#define VLT_RESET_PROTOCOL    (VLT_U32)0x00000001
#define VLT_UPDATE_BITRATE    (VLT_U32)0x00000002

#endif /*VAULTIC_PERIPHERAL_H*/
