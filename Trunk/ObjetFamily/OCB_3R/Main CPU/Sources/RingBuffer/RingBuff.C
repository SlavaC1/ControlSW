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
 
#include "Define.h"
#include "RingBuff.h"
#include "EdenProtocol.h"
#include "UartDrv.h"
#ifdef OCB_SIMULATOR
#include "c8051F120.h"
#elif defined OCB2
#include "c8051F120.h"
#else
#include "c8051F020.h"
#endif
 
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
#ifdef DEBUG
unsigned char	RingBuffGetElementCount (RBUF_RingBuffer	*RingBuffer)
{
  return (RingBuffer->ElementCount);
}
#endif

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

/****************************************************************************
 *
 *  NAME        : RingBuffInsert_U1
 *
 *  DESCRIPTION :                                         
 *
 ****************************************************************************/
unsigned char RingBuffInsert(RBUF_RingBuffer	*RingBuffer, BYTE Data) using 2
{
	
  if(RingBuffer->HeadIndex == RingBuffer->TailIndex)  
  {
    if(RingBuffer->ElementCount != 0)
	    return(E_RBUF_RING_BUFFER_EMPTY);
	}

  // Insert the element data
  RingBuffer->RxBuf[RingBuffer->HeadIndex]= Data;

  // Check and increment the element count 
	if(RingBuffer->HeadIndex < RingBuffer->MaxHeadIndex)
  {
    // Increment the element counter
	  (RingBuffer->HeadIndex)++;              
  }
	else
  { 
    // If Pass over the tail reset HeadIndex
	  RingBuffer->HeadIndex = 0;          
	}

  (RingBuffer->ElementCount)++;

  return(E_RBUF_RING_BUFFER_OK);
}



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
unsigned char  RingBuffRemove(RBUF_RingBuffer *RingBuffer,unsigned char *Element, BYTE Protocol)
{
  BYTE xdata PrevIntStatus ; 
  
  if(RingBuffer->ElementCount == 0)
  {
    return (E_RBUF_RING_BUFFER_EMPTY);
	}	
	else
	{
	  // Disable the UART interrupt and save the previous interrupt state
		if (Protocol == EDEN_OCB_PROTOCOL)
  	  PrevIntStatus = Uart0DisableInterrupt();
		else
      PrevIntStatus = Uart1DisableInterrupt();

    *Element  =  RingBuffer->RxBuf[RingBuffer->TailIndex];

    // Update Tail and message count
		if(RingBuffer->TailIndex < RingBuffer->MaxHeadIndex)
     	 (RingBuffer->TailIndex)++;
		else
      RingBuffer->TailIndex = 0;

    (RingBuffer->ElementCount)--;
		
	  if (PrevIntStatus)
		{
		  if (Protocol == EDEN_OCB_PROTOCOL)
    	  Uart0EnableInterrupt();
		  else
        Uart1EnableInterrupt();
		}
	}
  
  return (E_RBUF_RING_BUFFER_OK);
}


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

}



/****************************************************************************
 *
 *  NAME        : GetAvailableMsgHandle
 *
 *  DESCRIPTION : Get a handle to a free message buffer 
 ****************************************************************************/
int GetAvailableMsgHandle(RBUF_MsgRingBuffer *MsgRingBuffer)
{
// Check if there are no available message buffers
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
// Check the validity of the handle
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
// Increment the head index to the next buffer
	if(++(MsgRingBuffer->HeadIndex) == MsgRingBuffer->Size)
  	MsgRingBuffer->HeadIndex = 0;             

	MsgRingBuffer->MsgCharIndex = 0;

// Increment the number of ready messages
  MsgRingBuffer->ElementCount++;
}


/****************************************************************************
 *
 *  NAME        : GetReadyMsgsNumber
 *
 *  DESCRIPTION : Get the number of ready messages  
 ****************************************************************************/
#ifdef DEBUG
int GetReadyMsgsNumber(RBUF_MsgRingBuffer *MsgRingBuffer)
{
	return MsgRingBuffer->ElementCount;
}
#endif

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

// Increment the tail index to the next buffer
	if(++(MsgRingBuffer->TailIndex) == MsgRingBuffer->Size)
  	MsgRingBuffer->TailIndex = 0;             

// Decrement the number of ready messages
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
