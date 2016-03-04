/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_comms.h
 * 
 * \brief Interface to the VaultIC API communications layer.
 * 
 * \par Description:
 * This file declares the functions and constants used by the VaultIC API
 * communications layer.
 */

#ifndef VAULTIC_COMMS_H
#define VAULTIC_COMMS_H

#if ( VLT_ENABLE_ISO7816 == VLT_ENABLE )
#ifndef WIN32
#include "PCSC/wintypes.h"
#endif
#endif

VLT_STS VltCommsInit( VLT_INIT_COMMS_PARAMS* pInitCommsParams,
    VLT_MEM_BLOB *Command, 
    VLT_MEM_BLOB *Response );

VLT_STS VltCommsClose( void );

VLT_STS VltCommsDispatchCommand( VLT_MEM_BLOB *Command, VLT_MEM_BLOB *Response );

VLT_U16 VltCommsGetMaxSendSize( void );

VLT_U16 VltCommsGetMaxReceiveSize( void );

#if(VLT_ENABLE_ISO7816 == VLT_ENABLE )
VLT_STS VltCommsCardEvent(VLT_PU8 pu8ReaderName, DWORD dwTimeout,PDWORD pdwEventState);

VLT_STS VltCommsSelectCard( SCARDHANDLE hScard , SCARDCONTEXT hCxt, DWORD dwProtocol);
#endif

#endif /*VAULTIC_COMMS_H*/
