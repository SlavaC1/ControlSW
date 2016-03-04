/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_iso7816_protocol.h
 * 
 * \brief ISO 7816 Protocol interface for the VaultIC API.
 * 
 * \par Description:
 * TBD.
 */

#ifndef VAULTIC_ISO7816_PROTOCOL_H
#define VAULTIC_ISO7816_PROTOCOL_H

#if(VLT_ENABLE_ISO7816 == VLT_ENABLE )
#ifdef __APPLE__
#include <PCSC/wintypes.h>
#include <PCSC/pcsclite.h>
#include <PCSC/winscard.h>
#endif
#endif

VLT_STS VltIso7816SelectCard( SCARDHANDLE hScard , SCARDCONTEXT hCxt, DWORD dwProtocol);

VLT_STS VltIso7816PtclInit( VLT_INIT_COMMS_PARAMS* pInitCommsParams, 
    VLT_MEM_BLOB *pOutData,
    VLT_MEM_BLOB *pInData,
    VLT_PU16 pu16MaxSendSize,
    VLT_PU16 pu16MaxReceiveSize );

VLT_STS VltIso7816PtclClose( void );

VLT_STS VltIso7816PtclSendReceiveData( VLT_MEM_BLOB *pOutData, 
    VLT_MEM_BLOB *pInData );

VLT_STS VltIso7816PtclCardEvent(VLT_PU8 pu8ReaderName, DWORD dwTimeout,PDWORD pdwEventState);
VLT_STS VltIso7816PtclDetectReaders( VLT_PU32 pSize, VLT_PU8 pXmlReaderString);

#endif /*VAULTIC_ISO7816_PROTOCOL_H*/
