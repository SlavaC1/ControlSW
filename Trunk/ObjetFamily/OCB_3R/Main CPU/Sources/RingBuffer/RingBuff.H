/*===========================================================================
 *   FILENAME       : Ring Buffer {RingBuff.h}  
 *   PURPOSE        : This file contains the definitions of all functions and data types
 *		              that are exported by the RBUF package.
 *   DATE CREATED   : 02/Sep/2001
 *   PROGRAMMER     : Juval Izhaki 
 *   Description    : Ring Buffer H File 
 *   Include        : Define.h
 *===========================================================================*/
#ifndef _RING_BUFF_H_
#define _RING_BUFF_H_

#include "Define.h"

#ifdef OCB_SIMULATOR
#define BUFF_SIM_RATIO	  0x04
#else
#define BUFF_SIM_RATIO	  0x01
#endif


#define SIZE_OF_DECODE_MASSEGE 		 70
#define NUMBER_OF_ELEMENTS         150    /*30 byte max message length */
#define TX_QUEUE_SIZE              0x1E    /*30 byte max message length */
#define NUMBER_OF_BUFFERS          0x02 * BUFF_SIM_RATIO  /*The number of msg buffers */
#define LAST_MSG_BUF               0x01    /*The number of least msg buff */

//   Ring Buffer data structure ,this data structure Holds all 
//   incomming data from receive interrupt routine.  
//==================================================================================
typedef struct 
{
	unsigned char 		RcvCharIndex;	      	         	 //* The location of the last received char in the buffer  			  
	unsigned char 		HeadIndex;				         	 //* The ring buffer head  							  
	unsigned char 		TailIndex;				             //* The ring buffer tail  							  
	unsigned char 		MaxHeadIndex;				         //* The ring buffer Max index  
	unsigned char 		DecodeState;			 	         //* The current state  								  
	unsigned char 		ElementCount;			             //* The current number of elements in the ring buffer 
	unsigned char 		NextState;                           //* Next state machine  
	unsigned char 		MassegeLen;                          //* Next state machine
	unsigned char 		BytesRcv;                            //* Number of byte received 
	unsigned char 		DataBytesOnly;                       //* Number of raw data byte received 
	unsigned char     CheckSum ;     	 	                 //* Check sum 
	unsigned char    	CheckSumErrors ;     	 	         //* Number of Check sum ERROR's 
  unsigned char     PreviosByteIsSync;                   //* Was the next comming byte is A header one 
  unsigned char		  RxBuf [NUMBER_OF_ELEMENTS];	 	     //* The ring buffer itself 							  
} RBUF_RingBuffer;



typedef struct 
{
	unsigned char 		MsgCharIndex;	      	         	 //* The location of the last received char in the buffer  			  
	unsigned char 		TailIndex;				             //* The ring buffer tail  							  
	unsigned char 		HeadIndex;				         	 //* The ring buffer head  							  
	unsigned char 		Size;				         //* The ring buffer Max index  
	unsigned char 		ElementCount;			             //* The current number of elements in the ring buffer 
  unsigned char     ElementSize;                         //* Element size in byte                                          
  unsigned char	    MsgBuf[NUMBER_OF_BUFFERS][SIZE_OF_DECODE_MASSEGE];      //* The Message buffer himself 							  
} RBUF_MsgRingBuffer;

// Function Prototype 
// ====================
// This file contains the constant definitions of the RBUF package.
#define   E_RBUF_RING_BUFFER_FULL      	0x00
#define   E_RBUF_RING_BUFFER_OK   			0x01
#define   C_RBUF_RING_BUFFER_USED				0x02
#define   E_RBUF_RING_BUFFER_EMPTY     	0x03
#define   FIRST_MSG_BUF                	0x00 
#define   MAX_MSG_BUFF_ALOC            	0x03
//This file contains the enumeration type definitions for the RBUF package


void RingBuffInit (RBUF_RingBuffer *RingBuffer);
					
unsigned char RingBuffInsert (RBUF_RingBuffer	*RingBuffer, BYTE Data);
#ifdef DEBUG						
unsigned char RingBuffGetElementCount (RBUF_RingBuffer *RingBuffer);
// Get the number of ready messages
int GetReadyMsgsNumber(RBUF_MsgRingBuffer *MsgRingBuffer);										
#endif
void MsgRingBuffInit (RBUF_MsgRingBuffer *MsgRingBuffer);

unsigned char RingBuffRemove (RBUF_RingBuffer *RingBuffer,unsigned char *Element, BYTE Protocol);


// Get a handle to a free message buffer
int GetAvailableMsgHandle(RBUF_MsgRingBuffer *MsgRingBuffer);

// Get a pointer to a message buffer by a handle
BYTE* GetMsgPtrByHandle(RBUF_MsgRingBuffer *MsgRingBuffer, int Handle);

// Mark message as ready after all the message bytes have recieved
// Move the poniter to the available message to the next buffer
void MarkMsgAsReady(RBUF_MsgRingBuffer *MsgRingBuffer);

// Get a handle to the ready message
int GetReadyMsgHandle(RBUF_MsgRingBuffer *MsgRingBuffer);

// Remove a message when it is no longer needed
void RemoveMsg(RBUF_MsgRingBuffer *MsgRingBuffer);

// push a byte to a message buffer
void InsertByte(RBUF_MsgRingBuffer *MsgRingBuffer, int Handle, BYTE Data);

#endif 
