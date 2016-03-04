/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_apdu.h
 * 
 * \brief Constants needed for all modules having knowledge of APDu commands.
 * 
 * \par Description:
 * This file declares the constants used by all modules that make use of APDU
 * commands.
 */

#ifndef VAULTIC_APDU_H
#define VAULTIC_APDU_H

#define VLT_APDU_MIN_HEADER_SZ       (VLT_U8)4
#define VLT_APDU_TYPICAL_HEADER_SZ   (VLT_U8)5


#define VLT_APDU_CLASS_OFFSET        (VLT_U8)0
#define VLT_APDU_INS_OFFSET          (VLT_U8)1
#define VLT_APDU_P1_OFFSET           (VLT_U8)2
#define VLT_APDU_P2_OFFSET           (VLT_U8)3
#define VLT_APDU_P3_OFFSET           (VLT_U8)4
#define VLT_APDU_DATA_OFFSET         (VLT_U8)5

#define VLT_APDU_CASE1_CMD           (VLT_U8)1
#define VLT_APDU_CASE2_CMD           (VLT_U8)2
#define VLT_APDU_CASE3_CMD           (VLT_U8)3
#define VLT_APDU_CASE4_CMD           (VLT_U8)4

#define VLT_HEADER_SIZE              (VLT_U8)5
#define VLT_SW_SIZE                  (VLT_U8)2

#define VLT_MAX_APDU_SND_DATA_SZ     (VLT_U8)240
#define VLT_MAX_APDU_RCV_DATA_SZ     (VLT_U16)250

#define VLT_MAX_APDU_SND_TRANS_SIZE  (VLT_U16)(VLT_MAX_APDU_SND_DATA_SZ + VLT_HEADER_SIZE)
#define VLT_MAX_APDU_RCV_TRANS_SIZE  (VLT_U16)(VLT_MAX_APDU_RCV_DATA_SZ + VLT_SW_SIZE)

#endif /*VAULTIC_APDU_H*/
