/*===========================================================================
 *   FILENAME       : Eden protocol {EdenProtocol.c}  
 *   PURPOSE        : Eden serial communication protocol  
 *   DATE CREATED   : 11/09/2003
 *   PROGRAMMER     : Nir Sade 
 *   PROCEDURES     : 
 *===========================================================================*/
#include "EdenProtocol.h"
#include "UartDrv.h"
#include "MiniScheduler.h"
//#include "Watchdog.h"


// Constants
// =========
#define EDEN_PROTOCOL_MAX_MSG_LENGTH	70
#define EDEN_PROTOCOL_BUFFER_SIZE		(EDEN_PROTOCOL_MAX_MSG_LENGTH + 3)*2+6+1 // (msg length + src,dst,transaction)*2+headrer+checksum 
#define MSG_SYNC_1	0x55
#define MSG_SYNC_2	0x33
#define MSG_FILLER 0xaa
#define MSG_LENGTH_FILLER	0x00

#define MSG_SYNC_1_INDEX					0
#define MSG_SYNC_2_INDEX					1
#define MSG_LENGTH_LSB_INDEX			2
#define MSG_LENGTH_FILLER_1_INDEX	3
#define MSG_LENGTH_MSB_INDEX			4
#define MSG_LENGTH_FILLER_2_INDEX	5
#define SOURCE_ID_INDEX						6
#define EDEN_HEADER_SIZE					6
#define MIN_TRASACTION_ID					151


#define  C_1stHeaderByte				0x55
#define  C_2ndHeaderByte				0x33
#define  C_FillerByte			   		0xaa
#define  C_LenFillerByte				0x00
#define  C_LenghtBytes_Size    	0x04     // four Length bytes  
#define  C_HeaderSize          	0x06     // two Sync bytes (0X55, 0x33) and four Length bytes 
#define  C_ChkSumSize          	0x01
#define  C_SourceDestTrans     	0x06

#define COMMUNICATION_LOSS_TIMEOUT   10000

// Type definitions
// ================

	
 
// Local routines
// ==============


// Module variables
// ================
	
	
BYTE xdata EdenProtocolTransactionId;
BYTE xdata OHDBProtocolTransactionId;
RBUF_RingBuffer xdata *EdenRcvRingBufferPtr;
RBUF_RingBuffer xdata *OHDBRcvRingBufferPtr;
RBUF_MsgRingBuffer xdata EdenMsgRingBuff;
RBUF_MsgRingBuffer xdata OHDBMsgRingBuff;
BYTE xdata EdenProtocolBuffer[EDEN_PROTOCOL_BUFFER_SIZE];
BYTE xdata OHDBProtocolBuffer[EDEN_PROTOCOL_BUFFER_SIZE];



/****************************************************************************
 *
 *  NAME        : EdenProtocolInit
 *
 *  DESCRIPTION : Initialization of the eden protocol                                        
 *
 ****************************************************************************/
void EdenProtocolInit()
{
	EdenProtocolTransactionId = MIN_TRASACTION_ID;
  OHDBProtocolTransactionId = MIN_TRASACTION_ID;
	EdenRcvRingBufferPtr = UartGetReceiveBuffer(UART_0);
	OHDBRcvRingBufferPtr = UartGetReceiveBuffer(UART_1);
	RingBuffInit(EdenRcvRingBufferPtr);
	RingBuffInit(OHDBRcvRingBufferPtr);
}


/*for simulator use - OHDB*/
#ifdef OCB_SIMULATOR
EDEN_PROTOCOL_STATUS OHDBEdenProtocolSend(BYTE *Data, WORD DataLength, BYTE DestId ,BYTE Transaction, BOOL Reply)
{
	return EdenProtocolSend(Data,DataLength,DestId ,OHDB_ID ,Transaction,Reply, EDEN_OCB_PROTOCOL);
}
#endif
/****************************************************************************
 *
 *  NAME        : EdenProtocolSend
 *
 *  INPUT       : Data buffer, data length, transaction id, reply/solicited message
 *
 *  OUTPUT      : EDEN_PROTOCOL_STATUS.
 *
 *  DESCRIPTION : build a message (header + footer) and send it by the UART.                                        
 *
 ****************************************************************************/
EDEN_PROTOCOL_STATUS EdenProtocolSend(BYTE *Data, WORD DataLength, BYTE DestId , BYTE SourceId ,BYTE Transaction, BOOL Reply, BYTE Protocol)
{
	BYTE xdata BufIndex, i, CheckSum = 0, TransactionId;
	BYTE xdata *ProtocolBuffer, *ProtocolTransactionId;

	if (Protocol == EDEN_OCB_PROTOCOL)
	{
		ProtocolBuffer = &EdenProtocolBuffer;
		ProtocolTransactionId = &EdenProtocolTransactionId;
	}
	else
	{
		ProtocolBuffer = &OHDBProtocolBuffer;
		ProtocolTransactionId = &OHDBProtocolTransactionId;
	}
		
	if (Reply)
		TransactionId = Transaction;
 	else
		TransactionId = *ProtocolTransactionId;
	
// check if message length is graeter than the max length
	if (DataLength > EDEN_PROTOCOL_MAX_MSG_LENGTH)
		return EDEN_PROTOCOL_MSG_TOO_LONG;

// Build the msg header
	ProtocolBuffer[MSG_SYNC_1_INDEX] = MSG_SYNC_1;
	ProtocolBuffer[MSG_SYNC_2_INDEX] = MSG_SYNC_2;
	
// Insert the source id, destination id and transaction id to the buffer
// add filler un case one of them equals to the header byte (0x55)
	BufIndex = SOURCE_ID_INDEX;
	ProtocolBuffer[BufIndex++] = SourceId;
	if (SourceId == MSG_SYNC_1)
		ProtocolBuffer[BufIndex++] = MSG_FILLER;
	ProtocolBuffer[BufIndex++] = DestId;
	if (DestId == MSG_SYNC_1)
		ProtocolBuffer[BufIndex++] = MSG_FILLER;
	ProtocolBuffer[BufIndex++] = TransactionId;	
	if (TransactionId == MSG_SYNC_1)
		ProtocolBuffer[BufIndex++] = MSG_FILLER;
	

	if (!Reply)
	{
		if (++(*ProtocolTransactionId) == 0)
			*ProtocolTransactionId = MIN_TRASACTION_ID;
	}

// Check if there are header bytes (0x55) in the msg and add filler for every
// header byte found
  for (i = 0;i < DataLength ; i++)
  {
		ProtocolBuffer[BufIndex++] = Data[i];
		if (Data[i] == MSG_SYNC_1)
			ProtocolBuffer[BufIndex++] = MSG_FILLER;	
	}
	
// Insert the length to the msg after adding all the fillers
	ProtocolBuffer[MSG_LENGTH_LSB_INDEX] = (BufIndex - EDEN_HEADER_SIZE) & LSB_MASK;
	ProtocolBuffer[MSG_LENGTH_FILLER_1_INDEX] = MSG_LENGTH_FILLER;
	ProtocolBuffer[MSG_LENGTH_MSB_INDEX] = ((BufIndex - EDEN_HEADER_SIZE) & MSB_MASK) >> 8;
	ProtocolBuffer[MSG_LENGTH_FILLER_2_INDEX] = MSG_LENGTH_FILLER;

// Calculate the check sum and add it to the msg
	for(i = SOURCE_ID_INDEX; i < BufIndex; i++)
  	CheckSum += ProtocolBuffer[i];

	ProtocolBuffer[BufIndex++] = CheckSum;

	
// Send the msg through the uart
	if (UartSend(ProtocolBuffer, BufIndex, Protocol == EDEN_OCB_PROTOCOL? UART_0 : UART_1) != UART_NO_ERROR)
		return EDEN_PROTOCOL_SEND_FAILED;

  return EDEN_PROTOCOL_NO_ERROR;
}


/****************************************************************************
 *
 *  NAME        : EdenProtocolDecodeTask
 *
 *  DESCRIPTION : This task checks if new bytes have arrived and decodes them.                                        
 *
 ****************************************************************************/
void EdenProtocolDecodeTask(BYTE Arg)
{
	BYTE xdata Data;


  if (GetAvailableMsgHandle(&EdenMsgRingBuff) != -1)
  {
	  while(RingBuffRemove(EdenRcvRingBufferPtr,&Data, EDEN_OCB_PROTOCOL)== E_RBUF_RING_BUFFER_OK)
    {
  	  EdenProtocolDecode(Data,EDEN_OCB_PROTOCOL); 
    }
  }

  if (GetAvailableMsgHandle(&OHDBMsgRingBuff) != -1)
  {
  	while(RingBuffRemove(OHDBRcvRingBufferPtr,&Data, OCB_OHDB_PROTOCOL)== E_RBUF_RING_BUFFER_OK)
    {
  	  EdenProtocolDecode(Data,OCB_OHDB_PROTOCOL); 
    }
  }

 	SchedulerLeaveTask(Arg);

}
/****************************************************************************
 *
 *  NAME        : EdenProtocolDecode
 *
 *  INPUT       : Data byte, Protocol (EDEN-OCB or OCB-OHDB)
 *
 *  OUTPUT      : None.
 *
 *  DESCRIPTION : Decode a byte from EDEN-OCB protocol or OCB-OHDB protocol                                        
 *
 ****************************************************************************/
void EdenProtocolDecode(BYTE RData, BYTE Protocol)
{
	int xdata MsgHandle;
  BYTE xdata *Message;
	RBUF_RingBuffer xdata *RcvRingBufferPtr;
	RBUF_MsgRingBuffer xdata *MsgRingBuff;


	if (Protocol == EDEN_OCB_PROTOCOL)
	{
		RcvRingBufferPtr = EdenRcvRingBufferPtr;		
		MsgRingBuff = &EdenMsgRingBuff;
	}
	else
	{
		RcvRingBufferPtr = OHDBRcvRingBufferPtr;  		
		MsgRingBuff = &OHDBMsgRingBuff;
	}


	MsgHandle = GetAvailableMsgHandle(MsgRingBuff);
	if (MsgHandle == -1)
		return;
	
	Message = GetMsgPtrByHandle(MsgRingBuff, MsgHandle);
	if (Message == NULL)
		return;

  switch (RcvRingBufferPtr->DecodeState)
  {
  	case E_MSG_Header:
  		// When Bytes is the first header byte
    	if (RData == C_1stHeaderByte)
    		RcvRingBufferPtr->DecodeState = E_MSG_InHeader;
    	break;

    case E_MSG_InHeader:
  		// When Bytes is the second header byte move to detect the length
   		if (RData== C_2ndHeaderByte)
      	RcvRingBufferPtr->DecodeState = E_MSG_LengthLow;
    	//When any other case go back to search header
      else
			{
       	RcvRingBufferPtr->DecodeState = E_MSG_Header;
			}
      break;

    case E_MSG_LengthLow:
    	RcvRingBufferPtr->MassegeLen        =    RData;
      RcvRingBufferPtr->PreviosByteIsSync =    (RData == C_1stHeaderByte);
      RcvRingBufferPtr->DecodeState       =    E_MSG_LengthFiller;
      RcvRingBufferPtr->NextState         =    E_MSG_LengthHigh;

			// Insert the low byte of the length to the msg buffer 
    	InsertByte(MsgRingBuff,MsgHandle,RData);

      break;

    case E_MSG_LengthFiller:
      if (RData== C_LenFillerByte)
      	RcvRingBufferPtr->DecodeState = RcvRingBufferPtr->NextState;
      else
      {
      	if(RcvRingBufferPtr->PreviosByteIsSync)
        { 
        	if(RData == C_2ndHeaderByte)
        		RcvRingBufferPtr->DecodeState = E_MSG_LengthLow;
        	else
          {
        		RcvRingBufferPtr->DecodeState = E_MSG_Header;                 
          }
        }
        else
        { 
          //Error state : reset state
          RcvRingBufferPtr->DecodeState = E_MSG_Header;
          RcvRingBufferPtr->PreviosByteIsSync = FALSE;
				}
       }
       break;

    case E_MSG_LengthHigh  :
    	// Keep flag in case this byte is A sync byte 
      RcvRingBufferPtr->PreviosByteIsSync =    (RData == C_1stHeaderByte);  

      // Save high nibble of length
      RcvRingBufferPtr->MassegeLen = RcvRingBufferPtr->MassegeLen | ((RData) << 8);

			// insert the high byte of the length to the msg buffer 
    	InsertByte(MsgRingBuff,MsgHandle,RData);

      // Verify the length validity
      if (RcvRingBufferPtr->MassegeLen > 0 && RcvRingBufferPtr->MassegeLen <= EDEN_PROTOCOL_MAX_MSG_LENGTH)
      {
      	// Reset the buffer index
        RcvRingBufferPtr->BytesRcv = 0;
        RcvRingBufferPtr->DataBytesOnly = 0;

        //Reset the CheckSum byte
        RcvRingBufferPtr->CheckSum = 0;
        
		    RcvRingBufferPtr->NextState = E_MSG_Data;
        RcvRingBufferPtr->DecodeState     = E_MSG_LengthFiller;
      }
      else
      {
	    	if(RData == C_1stHeaderByte)
        	RcvRingBufferPtr->DecodeState = E_MSG_InHeader;  
        else
        {
        	// Wait for HEADER again
          RcvRingBufferPtr->DecodeState  = E_MSG_Header;
        }
      }
      break;

  case E_MSG_Data:
		// When Byte is the SYNC HEADER byte
    if (RData== C_1stHeaderByte)
    {
    	RcvRingBufferPtr->DecodeState     = E_MSG_Filler;
      RcvRingBufferPtr->NextState = E_MSG_Data;
    }

		// insert the data to the msg buffer 
    InsertByte(MsgRingBuff,MsgHandle,RData);

		// Calculate CheckSum
    RcvRingBufferPtr->CheckSum = RcvRingBufferPtr->CheckSum + RData;

		// Advance counters
    RcvRingBufferPtr->DataBytesOnly++;
    RcvRingBufferPtr->BytesRcv++;

		// When all bytes received EXCEPT CheckSum byte
    if (RcvRingBufferPtr->BytesRcv >= RcvRingBufferPtr->MassegeLen)
    {
			// Move to CheckSum State
      RcvRingBufferPtr->DecodeState = E_MSG_CheckSum;
    }
    break;

  case E_MSG_CheckSum:
  	// When Checksum is correct
    if ((RcvRingBufferPtr->CheckSum & 0xff) != RData)
		{
			RcvRingBufferPtr->CheckSumErrors++;
		}
		else
 	  {
			// Insert the payload length instead of the message length
			Message[PAYLOAD_LOW_LENGTH_INDEX] = RcvRingBufferPtr->DataBytesOnly;
			Message[PAYLOAD_HIGH_LENGTH_INDEX] = ((RcvRingBufferPtr->DataBytesOnly) & MSB_MASK) >> 8;

			// If checksum is ok then inc to the next buffer 
			MarkMsgAsReady(MsgRingBuff);
    //	if (Protocol == EDEN_OCB_PROTOCOL)
    //    ResetCommunicationLossTimeout();

    }
    //And wait for HEADER again
    RcvRingBufferPtr->DecodeState  = E_MSG_Header;
    break;

  
  case E_MSG_Filler:
		// When the byte is the filler byte
    if (RData == C_FillerByte)
    {
    	if (RcvRingBufferPtr->BytesRcv + 1 >= RcvRingBufferPtr->MassegeLen)
      	RcvRingBufferPtr->DecodeState = E_MSG_CheckSum;
      else
      	RcvRingBufferPtr->DecodeState = RcvRingBufferPtr->NextState;

			// When in DATA state count the filler byte also
	    if ((RcvRingBufferPtr->NextState == E_MSG_Data) ||
	        (RcvRingBufferPtr->NextState == E_MSG_DestinationId ) ||
	         (RcvRingBufferPtr->NextState == E_MSG_TransactionId ))
	    {
				// Advance counter
	      RcvRingBufferPtr->BytesRcv++;
	
				//Calculate CheckSum
				RcvRingBufferPtr->CheckSum = RcvRingBufferPtr->CheckSum + RData;
	     }
    }
		else
		{
			// When the byte is the second Sync byte (0x33) then
			// maybe it is a sync header, moving to detect source
      if (RData == C_2ndHeaderByte)
      	RcvRingBufferPtr->DecodeState = E_MSG_LengthLow;
  		else
			{
				// When any other case Filler error - try to Resync
      	RcvRingBufferPtr->DecodeState = E_MSG_Header;
			}
    } 
    RcvRingBufferPtr->RcvCharIndex++;        
    break;
  }
}


/*for simulator use - OHDB*/
#ifdef OCB_SIMULATOR
RBUF_MsgRingBuffer * OHDBEdenProtocolGetMsgBuff(void)
{
  return EdenProtocolGetMsgBuff(OCB_OHDB_PROTOCOL);
}
#endif
/****************************************************************************
 *
 *  NAME        : EdenProtocolGetMsgBuff
 *
 *  INPUT       : None.
 *
 *  OUTPUT      : EdenProtocolGetMsgBuff.
 *
 *  DESCRIPTION : Returns a pointer to the message buffer.                                        
 *
 ****************************************************************************/
RBUF_MsgRingBuffer *EdenProtocolGetMsgBuff(BYTE Protocol)
{
	if (Protocol == EDEN_OCB_PROTOCOL)
		return &EdenMsgRingBuff;

  return &OHDBMsgRingBuff;
}





