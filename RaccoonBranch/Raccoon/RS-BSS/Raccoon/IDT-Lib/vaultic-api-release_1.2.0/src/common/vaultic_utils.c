/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#include "vaultic_common.h"
#include "vaultic_mem.h"
#include "vaultic_utils.h"
#include "vaultic_apdu.h"

VLT_U16 VltEndianReadPU16(const VLT_U8 *p)
{
    return (p[0] << 8) | p[1];
}

VLT_U32 VltEndianReadPU32(const VLT_U8 *p)
{
    return ((VLT_U32) p[0] << 24) |
           ((VLT_U32) p[1] << 16) |
           ((VLT_U32) p[2] <<  8) |
           ((VLT_U32) p[3] <<  0);
}

VLT_U16 NumBytesInBuffer( VLT_U16 u16Idx )
{
    return ( u16Idx - VLT_APDU_DATA_OFFSET );
}

VLT_U16 NumBufferBytesAvail( VLT_U16 u16MaxBytes, VLT_U16 u16Idx )
{
    return u16MaxBytes - NumBytesInBuffer( u16Idx );
}
