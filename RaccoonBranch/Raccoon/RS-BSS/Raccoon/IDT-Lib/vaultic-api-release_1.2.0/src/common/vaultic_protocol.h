/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_protocol.h
 * 
 * \brief Protocol interface for the VaultIC API.
 * 
 * \par Description:
 * TBD.
 */

#ifndef VAULTIC_PROTOCOL_H
#define VAULTIC_PROTOCOL_H

#if ( VLT_ENABLE_ISO7816 == VLT_ENABLE )
#ifndef WIN32
#include "PCSC/wintypes.h"
#endif
#endif

VLT_STS VltPtclInit( VLT_INIT_COMMS_PARAMS* pInitCommsParams,
    VLT_MEM_BLOB *pOutData,
    VLT_MEM_BLOB *pInData,
    VLT_PU16 pu16MaxSendSize,
    VLT_PU16 pu16MaxReceiveSize);

VLT_STS VltPtclClose( void );

VLT_STS VltPtclSendReceiveData( VLT_MEM_BLOB *pOutData, VLT_MEM_BLOB *pInData );

#if(VLT_ENABLE_ISO7816 == VLT_ENABLE )
VLT_STS VltPtclCardEvent(VLT_PU8 pu8ReaderName, DWORD dwTimeout,PDWORD pdwEventState);

VLT_STS VltPtclSelectCard( SCARDHANDLE hScard , SCARDCONTEXT hCxt, DWORD dwProtocol);
#endif

/*
* Function Pointer Definitions
*/
typedef VLT_STS (*pfnVltPtclInit)( VLT_INIT_COMMS_PARAMS* pInitCommsParams, 
    VLT_MEM_BLOB *pOutData,
    VLT_MEM_BLOB *pInData,
    VLT_PU16 pu16MaxSendSize,
    VLT_PU16 pu16MaxReceiveSize );

typedef VLT_STS (*pfnVltPtclClose)( void );

typedef VLT_STS (*pfnVltPtclSendReceiveData)( VLT_MEM_BLOB *pOutData, 
    VLT_MEM_BLOB *pInData );

#if(VLT_ENABLE_ISO7816 == VLT_ENABLE )
typedef VLT_STS (*pfnVltPtclCardEvent)(VLT_PU8 pu8ReaderName, DWORD dwTimeout,PDWORD pdwEventState);

typedef VLT_STS (*pfnVltPtclSelectCard)(SCARDHANDLE hScard, SCARDCONTEXT hCxt, DWORD dwProtocol);
#endif
/**
 * \struct _VltPtcl
 *
 * \brief Structure of function pointers used by the comms peripherals.
 */
typedef struct _VltPtcl
{
    pfnVltPtclInit PtclInit;
    pfnVltPtclClose PtclClose;
    pfnVltPtclSendReceiveData PtclSendReceiveData;
#if(VLT_ENABLE_ISO7816 == VLT_ENABLE )
    pfnVltPtclCardEvent PtclCardEvent;
	pfnVltPtclSelectCard PtclSelectCard;
#endif
} VltPtcl;

#endif /*VAULTIC_PROTOCOL_H*/
