/*===========================================================================
 *   FILENAME       : Ring Buffer {RingBuff.C}
 *   PURPOSE        : Communication ring buffer procedures
 *   DATE CREATED   : 02/Sep/2001
 *   PROGRAMMER     : Juval Izhaki
 *   Description    : This package handles the SIUB_SW RBUF sequence.
 *                    The package provides the following:
 *            	      - Ring buffers initialization
 *      		      - Create RING buffer
 *		              - Insert element to RING buffer
 *		              - Remove element to RING buffer
 *   Include        : RingBuff.h
 *===========================================================================*/
 
#ifdef C8051F206_CPU
#include "c8051F200.h"
#elif defined(C8051F005_CPU)
#include "c8051F000.h"
#elif defined(C8051F023_CPU)
#include "c8051F020.h"
#endif

#include "Define.h"
#include "RingBuff.h"
#ifdef OCB_SIMULATOR
#include "EdenProtocol.h"
#else
#include "EdenProtocolOHDB.h"
#endif
 
 #define  _RBUF_
 bit      CharInSBuff;

/*===========================================================================
 *   FILENAME       : Ring Buffer {RingBuff.C}
 *   PURPOSE        : Communication ring buffer procedures
 *   DATE CREATED   : 02/Sep/2001
 *   PROGRAMMER     : Juval Izhaki
 *   Procedure Name : RBUF_GetElementCount()
 *   Description    : Returns Element count.
 *   Input          : RBUF_RingBuffer	RingBuffer
 *   Return         : element count
 *===========================================================================*/
  unsigned char	RingBuffGetElementCount (RBUF_RingBuffer	*RingBuffer)
	{
	 	return (RingBuffer->ElementCount);
	}

/*===========================================================================
 *   FILENAME       : Ring Buffer {RingBuff.C}
 *   PURPOSE        : Communication ring buffer procedures
 *   DATE CREATED   : 02/Sep/2001
 *   PROGRAMMER     : Juval Izhaki
 *   Procedure Name : RBUF_Init()
 *   Description    : Initialize ring buffer according to data elements +
 *            		  header: Length, Head, Tail, Data [no of elements].
 *			          Initialize Size of buffer, Head and tail index.
 *           		  Return pointer to alloctaed ring buffer.
 *   Input          : unsinged short Element Size
 *                    - The length of one element in the ring buffer.
 *			          unsinged short NumberOfElements
 *                    - Number of elements in the ring buffer.
 *
 *===========================================================================*/
void  RingBuffInit(RBUF_RingBuffer *RingBuffer)
{
  // Initialize the ring buffer
	RingBuffer->MaxHeadIndex     = NUMBER_OF_ELEMENTS -1;
	RingBuffer->HeadIndex        = 0x0;
	RingBuffer->TailIndex        = 0x0;
	RingBuffer->ElementCount     = 0x0;
  RingBuffer->DecodeState      = E_MSG_Header;
  RingBuffer->RcvCharIndex     = 0x0;
  RingBuffer->CheckSum         = 0x0;
	RingBuffer->CheckSumErrors   = 0x0;
}//End RBUF_Init
/*===========================================================================
 *   FILENAME       : Ring Buffer {RingBuff.C}
 *   PURPOSE        : Communication ring buffer procedures
 *   DATE CREATED   : 02/Sep/2001
 *   PROGRAMMER     : Juval Izhaki
 *   Procedure Name : RBUF_Flush()
 *   Description    : Resets the ring buffer, frees the allocated areas and zeroes the Head and
 *       	          Tail variables.
 *		              Returns error code.
 *   Input          : RBUF_RingBuffer	RingBuffer
 *   Return         : RBUF_ErrorType	*ErrorCode
 *===========================================================================
void RingBuffFlush (RBUF_RingBuffer	*RingBuffer)
{
// Zero the Head and Tail of the ring buffer
	RingBuffer->HeadIndex = 0;
	RingBuffer->TailIndex = 0;
	RingBuffer->ElementCount = 0;
}
/*===========================================================================
 *   FILENAME       : Ring Buffer {RingBuff.C}
 *   PURPOSE        : Communication ring buffer procedures
 *   DATE CREATED   : 02/Sep/2001
 *   PROGRAMMER     : Juval Izhaki
 *   Procedure Name : RBUF_Insert()
 *   Description    : Verifies free space in the ring buffer,
 *				      Allocates memory for the input Element,
 *		 		      Updates the Head variable and copies the input Element
 *				      to the allocated element area.
 *				      Returns error code.
 *   Input          : RBUF_RingBuffer	RingBuffer
 *				      unsigned char	    *Element
 *   Return         : RBUF_ErrorType	*ErrorCode
 *===========================================================================*/
unsigned char RingBuffInsert(RBUF_RingBuffer	*RingBuffer) using 2
{
	
  if(RingBuffer->HeadIndex == RingBuffer->TailIndex)  
  {
    if(RingBuffer->ElementCount != 0)
	    return(E_RBUF_RING_BUFFER_EMPTY);
	}

  // Insert the element data
#ifdef C8051F023_CPU
  RingBuffer->RxBuf[RingBuffer->HeadIndex]= SBUF0;
#else
  RingBuffer->RxBuf[RingBuffer->HeadIndex]= SBUF;
#endif

  // Check and increment the element count 
	if(RingBuffer->HeadIndex < RingBuffer->MaxHeadIndex)
  {
	  (RingBuffer->HeadIndex)++;             // Increment the element counter 
  }
	else
  { 
	  RingBuffer->HeadIndex = 0;          // If Pass over the tail reset HeadIndex
	}

  (RingBuffer->ElementCount)++;

  return(E_RBUF_RING_BUFFER_OK);
}//End


/*===========================================================================
 *   FILENAME       : Ring Buffer {RingBuff.C}
 *   PURPOSE        : Communication ring buffer procedures
 *   DATE CREATED   : 30/Sep/2001
 *   PROGRAMMER     : Juval Izhaki
 *   Procedure Name : RingBuffRemove()
 *   Description    : This Function return the index of the Active message 
 *            		  Buffer 
 *   Input          : pointer to alloctaed Message ring buffer.
 *   Company        : OBJET Geometries INC 
 *===========================================================================*/
unsigned char  RingBuffRemove(RBUF_RingBuffer *RingBuffer,unsigned char *Element)
{
  bit IntSataus ; 
  
  if(RingBuffer->ElementCount == 0)
  {
    return (E_RBUF_RING_BUFFER_EMPTY);
	}	
	else
	{
#ifdef C8051F023_CPU
  IntSataus = ES0;  
  ES0 = DISABLE;
#else
  IntSataus = ES;  
  ES = DISABLE;
#endif

    *Element  =  RingBuffer->RxBuf[RingBuffer->TailIndex];

    // Update Tail and message count
		if(RingBuffer->TailIndex < RingBuffer->MaxHeadIndex)
     	 (RingBuffer->TailIndex)++;
		else
      RingBuffer->TailIndex = 0;

    (RingBuffer->ElementCount)--;
#ifdef C8051F023_CPU
  ES0 = IntSataus;
#else
  ES = IntSataus;
#endif
	}
  
  return (E_RBUF_RING_BUFFER_OK);
}//End



/*===========================================================================
 *   FILENAME       : Ring Buffer {RingBuff.C}
 *   PURPOSE        : Communication ring buffer procedures
 *   DATE CREATED   : 23/Sep/2001
 *   PROGRAMMER     : Juval Izhaki
 *   Procedure Name : MsgBuffInit()
 *   Description    : Initialize massege buffer according to data elements +
 *   Input          : None 
 *===========================================================================
void MsgBuffInit ( RBUF_MessageBuffer *MsgBuffer)
{
  // Initialize the Massege buffer
   MsgBuffer->MsgBuffIndex         = CLEAR;                             //* Index to the Msg buffer 
   MsgBuffer->MsgBuffInUse         = CLEAR;                             //* Number of char inseted into the Msg Buffer  
   MsgBuffer->MsgBufReadyToUse     = CLEAR;                             //* Number of char extructed from the Msg Buffer  

  //MsgBuffer->MsgBufIndex = MSG_BUF_1_SELECT;      //* Indecate first buffer 
}//End MsgBuffInit


/*===========================================================================
 *   FILENAME       : Ring Buffer {RingBuff.C}
 *   PURPOSE        : Communication ring buffer procedures
 *   DATE CREATED   : 30/Sep/2001
 *   PROGRAMMER     : Juval Izhaki
 *   Procedure Name : MsgRingBuffInit()
 *   Description    : Initialize the message ring buffer according to data elements +
 *            		  header: Length, Head, Tail, Data [no of elements].
 *			          Initialize Size of buffer, Head and tail index.
 *   Input          : pointer to alloctaed Message ring buffer.
 *   Company        : OBJET Geometries INC 
 *===========================================================================*/
void  MsgRingBuffInit(RBUF_MsgRingBuffer *MsgRingBuffer)
{
	// Initialize the ring buffer
	MsgRingBuffer->Size = NUMBER_OF_BUFFERS;
	MsgRingBuffer->HeadIndex = 0;
	MsgRingBuffer->TailIndex = 0;
	MsgRingBuffer->ElementCount = 0;
  MsgRingBuffer->MsgCharIndex = 0;

}//End MsgRingBuffInit



/****************************************************************************
 *
 *  NAME        : GetAvailableMsgHandle
 *
 *  DESCRIPTION : Get a handle to a free message buffer 
 ****************************************************************************/
int GetAvailableMsgHandle(RBUF_MsgRingBuffer *MsgRingBuffer)
{
// check if there are no available message buffers
// -----------------------------------------------
	if (MsgRingBuffer->ElementCount == MsgRingBuffer->Size)
		return -1;

	return MsgRingBuffer->HeadIndex;
}


/****************************************************************************
 *
 *  NAME        : GetMsgPtrByHandle
 *
 *  DESCRIPTION : Get a pointer to a message buffer by a handle 
 ****************************************************************************/
BYTE* GetMsgPtrByHandle(RBUF_MsgRingBuffer *MsgRingBuffer, int Handle)
{
// check the validity of the handle
// --------------------------------
	if ((Handle > (MsgRingBuffer->Size - 1)) || (Handle < 0))
		return NULL;
	
	return MsgRingBuffer->MsgBuf[Handle];

}

/****************************************************************************
 *
 *  NAME        : MarkMsgAsReady
 *
 *  DESCRIPTION : Mark message as ready after all the message bytes have recieved
 *                Move the poniter to the available message to the next buffer
 ****************************************************************************/
void MarkMsgAsReady(RBUF_MsgRingBuffer *MsgRingBuffer)
{
// increment the head index to the next buffer
// ------------------------------------------- 
	if(++(MsgRingBuffer->HeadIndex) == MsgRingBuffer->Size)
  	MsgRingBuffer->HeadIndex = 0;             

	MsgRingBuffer->MsgCharIndex = 0;

// increment the number of ready messages
// -------------------------------------- 
  MsgRingBuffer->ElementCount++;
}


/****************************************************************************
 *
 *  NAME        : GetReadyMsgsNumber
 *
 *  DESCRIPTION : Get the number of ready messages  
 ****************************************************************************/
int GetReadyMsgsNumber(RBUF_MsgRingBuffer *MsgRingBuffer)
{
	return MsgRingBuffer->ElementCount;
}


/****************************************************************************
 *
 *  NAME        : GetReadyMsgHandle
 *
 *  DESCRIPTION : Get a handle to the ready message 
 ****************************************************************************/
int GetReadyMsgHandle(RBUF_MsgRingBuffer *MsgRingBuffer)
{
	if (MsgRingBuffer->ElementCount == 0)
		return -1;

	return MsgRingBuffer->TailIndex;
}

/****************************************************************************
 *
 *  NAME        : RemoveMsg
 *
 *  DESCRIPTION : Remove a message when it is no longer needed 
 ****************************************************************************/
void RemoveMsg(RBUF_MsgRingBuffer *MsgRingBuffer)
{
	if (MsgRingBuffer->ElementCount == 0)
		return;

// increment the tail index to the next buffer
// ------------------------------------------- 
	if(++(MsgRingBuffer->TailIndex) == MsgRingBuffer->Size)
  	MsgRingBuffer->TailIndex = 0;             

// decrement the number of ready messages
// -------------------------------------- 
  (MsgRingBuffer->ElementCount)--;	
}


/****************************************************************************
 *
 *  NAME        : InsertByte
 *
 *  DESCRIPTION : push a byte to a message buffer 
 ****************************************************************************/
void InsertByte(RBUF_MsgRingBuffer *MsgRingBuffer, int Handle, BYTE Data)
{
	if (MsgRingBuffer->MsgCharIndex >= SIZE_OF_DECODE_MASSEGE)
		return;

	MsgRingBuffer->MsgBuf[Handle][MsgRingBuffer->MsgCharIndex] = Data;
	(MsgRingBuffer->MsgCharIndex)++;

}
