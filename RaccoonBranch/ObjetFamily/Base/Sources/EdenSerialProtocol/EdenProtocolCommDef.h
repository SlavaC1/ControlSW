/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: OCB related defintions.                                  *
 * Module Description: Eden protocol related types, constants and   *
 *                     structures.                                  *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 01/10/2001                                           *
 * Last upate: 01/10/2001                                           *
 ********************************************************************/

#ifndef _EDEN_PROTOCOL_COMM_DEF_
#define _EDEN_PROTOCOL_COMM_DEF_

#include "QTypes.h"

// Align all structures to byte boundry
#ifdef OS_WINDOWS

// The following pragma is for BCB only
#ifndef __BCPLUSPLUS__
#error Q2RT: This module contain pragma statements for Borland C++ Builder
#endif

#pragma pack(push,1)

#endif

#ifdef OS_VXWORKS
#define STRUCT_ATTRIBUTE __attribute__ ((packed))
#elif defined(OS_WINDOWS)
#define STRUCT_ATTRIBUTE
#endif


// Generic tester message (data should be appended after this structure)
struct TEdenProtocolPacketHeader {
  BYTE SourceID;
  BYTE DestID;
  BYTE TransactionID;

} STRUCT_ATTRIBUTE;

// Restore previous align settings
#ifdef OS_WINDOWS
#pragma pack(pop)
#endif

#endif
