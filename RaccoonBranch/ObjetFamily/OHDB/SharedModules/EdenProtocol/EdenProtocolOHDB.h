/*===========================================================================
 *   FILENAME       : Eden protocol {EdenProtocol.h}  
 *   PURPOSE        : Eden serial communication protocol header file  
 *   DATE CREATED   : 11/Nov/2001
 *   PROGRAMMER     : Nir Saadon 
 *   PROCEDURES     : 
 *===========================================================================*/

#ifndef _EDEN_PROTOCOL_OHDB_H_
#define _EDEN_PROTOCOL_OHDB_H_

#include "Define.h"
#include "RingBuff.h"


typedef enum{
	EDEN_PROTOCOL_NO_ERROR = 0,
	EDEN_PROTOCOL_MSG_TOO_LONG,
	EDEN_PROTOCOL_SEND_FAILED
}EDEN_PROTOCOL_STATUS;

#define	E_MSG_Header 				0x00  
#define	E_MSG_InHeader				0x01
#define	E_MSG_LengthLow			0x02
#define E_MSG_LengthFiller		0x03
#define	E_MSG_LengthHigh			0x04
#define	E_MSG_SourceId				0x05
#define	E_MSG_DestinationId	0x06
#define E_MSG_TransactionId	0x07
#define	E_MSG_Data						0x08
#define	E_MSG_Filler					0x0A
#define E_MSG_CheckSum				0x0B


#define EDEN_DEST_ID					1


// Function Prototype 
// ====================

// Initialization of the eden protocol                                        
// -----------------------------------
void EdenProtocolInit();

// build a message (header + footer) and send it by the UART
// ---------------------------------------------------------
EDEN_PROTOCOL_STATUS OHDBEdenProtocolSend(BYTE *Data, WORD DataLength, BYTE DestId ,BYTE Transaction, BOOL Reply);

// This task checks if a new byte has arrived and decodes it                                        
// ---------------------------------------------------------
void EdenProtocolDecodeTask(BYTE Arg);

// Decode a byte from a message
// ----------------------------
void OHDBEdenProtocolDecode(BYTE RData);

// Returns a pointer to the message buffer
// ---------------------------------------
RBUF_MsgRingBuffer *OHDBEdenProtocolGetMsgBuff(void);


#endif	