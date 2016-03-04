/*===========================================================================
 *   FILENAME       : Eden protocol {EdenProtocol.h}  
 *   PURPOSE        : Eden serial communication protocol header file  
 *   DATE CREATED   : 11/09/2003
 *   PROGRAMMER     : Nir Sade 
 *   PROCEDURES     : 
 *===========================================================================*/

#ifndef _EDEN_PROTOCOL_H_
#define _EDEN_PROTOCOL_H_

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


#define EDEN_ID		1
#define OCB_ID		3
#define OHDB_ID		4


#define EDEN_OCB_PROTOCOL 		0
#define OCB_OHDB_PROTOCOL			1

#define PAYLOAD_LOW_LENGTH_INDEX   0
#define PAYLOAD_HIGH_LENGTH_INDEX  1

#ifdef OCB_SIMULATOR
#define EDEN_DEST_ID					1
#endif
// Function Prototype 
// ====================

// Initialization of the eden protocol                                        
void EdenProtocolInit();

// build a message (header + footer) and send it by the UART
EDEN_PROTOCOL_STATUS EdenProtocolSend(BYTE *Data, WORD DataLength, BYTE DestId, BYTE SourceId,BYTE Transaction, BOOL Reply, BYTE Protocol);
#ifdef OCB_SIMULATOR
EDEN_PROTOCOL_STATUS OHDBEdenProtocolSend(BYTE *Data, WORD DataLength, BYTE DestId ,BYTE Transaction, BOOL Reply);
#endif
// This task checks if a new byte has arrived and decodes it                                        
void EdenProtocolDecodeTask(BYTE Arg);

// Decode a byte from a message
void EdenProtocolDecode(BYTE RData, BYTE Protocol);
// Returns a pointer to the message buffer
RBUF_MsgRingBuffer *EdenProtocolGetMsgBuff(BYTE Protocol);
#ifdef OCB_SIMULATOR
RBUF_MsgRingBuffer *OHDBEdenProtocolGetMsgBuff(void);
#endif

// Enable / diasble the communication status task
void EnableDisableCommStatusTask(BOOL Enable);


#endif	