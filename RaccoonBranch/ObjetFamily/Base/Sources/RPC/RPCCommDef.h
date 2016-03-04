/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: OCB related defintions.                                  *
 * Module Description: RPC communication types, constants and       *
 *                     structures.                                  *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 24/09/2001                                           *
 * Last upate: 30/01/2002                                           *
 ********************************************************************/

#ifndef _RPC_COMM_H_
#define _RPC_COMM_H_

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

// Maximum and minimum number of transaction for outgoing messages
const int MIN_RPC_OUTGOING_TRANSACTION = 1;
const int MAX_RPC_OUTGOING_TRANSACTION = 150;

// Unsolicited transactions range
const int MIN_RPC_UNSOLICITED_TRANSACTION = 151;
const int MAX_RPC_UNSOLICITED_TRANSACTION = 255;


// Messages from RPC to EdenRT
const int RPC_PING           = 0x01;
const int RPC_RPC_PROP_READ  = 0x02;
const int RPC_RPC_PROP_WRITE = 0x03;
const int RPC_RPC_INVOKE     = 0x04;

// Messages from EdenRT to tester
const int RPC_ACK       = 0xC0;
const int RPC_RPC_REPLY = 0xC1;

// Ack status values
const int RPC_ACK_STATUS_OK = 0;
const int RPC_ACK_STATUS_NOT_OK = 1;

// RPC status values
const int RPC_OK               = 0;
const int RPC_OBJECT_NOT_FOUND = 1;
const int RPC_PROP_NOT_FOUND   = 2;
const int RPC_METHOD_NOT_FOUND = 3;
const int RPC_INVALID_VALUE    = 4;
const int RPC_INVALID_MESSAGE  = 5;
const int RPC_INVOKATION_ERR   = 6;

// Generic tester message (data should be appended after this structure)
struct TRPCGenericMessage {
  BYTE MessageID;
  BYTE ArgsNum;   // Required only for methods invocation
} STRUCT_ATTRIBUTE;

// Ack message
struct TRPCAck {
  BYTE MessageID;
  BYTE RespondedMessageID;
  BYTE AckStatus;
} STRUCT_ATTRIBUTE;

// RPC reply message
struct TRPCReply {
  BYTE MessageID;
  BYTE Status;
} STRUCT_ATTRIBUTE;

// Restore previous align settings
#ifdef OS_WINDOWS
#pragma pack(pop)
#endif

#endif
