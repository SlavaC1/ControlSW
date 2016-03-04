/*===========================================================================
 *   FILENAME       : Eden protocol {EdenProtocol.c}  
 *   PURPOSE        : Eden serial communication protocol  
 *   DATE CREATED   : 11/Nov/2001
 *   PROGRAMMER     : Nir Saadon 
 *   PROCEDURES     : 
 *===========================================================================*/
//#pragma ROM(COMPACT)
#include "EdenProtocolOHDB.h"
#include "RingBuff.h"
#include "Uartdrv.h"
#include "MiniScheduler.h"
#include "Watchdog.h"


// Constants
// =========
#define EDEN_PROTOCOL_MAX_MSG_LENGTH	60
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

#define OHDB_SOURCE_ID		4



#define  C_1stHeaderByte				0x55
#define  C_2ndHeaderByte				0x33
#define  C_FillerByte			   		0xaa
#define  C_LenFillerByte				0x00
#define  C_LenghtBytes_Size    	0x04     // four Length bytes  
#define  C_HeaderSize          	0x06     // two Sync bytes (0X55, 0x33) and four Length bytes 
#define  C_ChkSumSize          	0x01
#define  C_SourceDestTrans     	0x06




// Type definitions
// ================

	
 
// Local routines
// ==============


// Module variables
// ================
	
	
	BYTE xdata EdenProtocolTransactionId;
  RBUF_RingBuffer xdata *RcvRingBufferPtr;
	RBUF_MsgRingBuffer xdata MsgRingBuff;
	BYTE xdata EdenProtocolBuffer[EDEN_PROTOCOL_BUFFER_SIZE];

// Extern variables
// ================
	



// Exported routines
// =================


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
	RcvRingBufferPtr  = UartGetReceiveBuffer();
	RingBuffInit(RcvRingBufferPtr);
}



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
EDEN_PROTOCOL_STATUS OHDBEdenProtocolSend(BYTE *Data, WORD DataLength, BYTE DestId ,BYTE Transaction, BOOL Reply)
{
	BYTE xdata BufIndex, i, CheckSum = 0, TransactionId;

	if (Reply)
		TransactionId = Transaction;
 	else
		TransactionId = EdenProtocolTransactionId;
	
// check if message length > 30
// ----------------------------
	if (DataLength > EDEN_PROTOCOL_MAX_MSG_LENGTH)
		return EDEN_PROTOCOL_MSG_TOO_LONG;

// build the msg header
// --------------------
	EdenProtocolBuffer[MSG_SYNC_1_INDEX] = MSG_SYNC_1;
	EdenProtocolBuffer[MSG_SYNC_2_INDEX] = MSG_SYNC_2;
	
// insert the source id, destination id and transaction id to the buffer
// add filler un case one of them equals to the header byte (0x55)
// ---------------------------------------------------------------
	BufIndex = SOURCE_ID_INDEX;
	EdenProtocolBuffer[BufIndex++] = OHDB_SOURCE_ID;
	if (OHDB_SOURCE_ID == MSG_SYNC_1)
		EdenProtocolBuffer[BufIndex++] = MSG_FILLER;
	EdenProtocolBuffer[BufIndex++] = DestId;
	if (DestId == MSG_SYNC_1)
		EdenProtocolBuffer[BufIndex++] = MSG_FILLER;
	EdenProtocolBuffer[BufIndex++] = TransactionId;	
	if (TransactionId == MSG_SYNC_1)
		EdenProtocolBuffer[BufIndex++] = MSG_FILLER;
	

	if (!Reply)
	{
		if (++EdenProtocolTransactionId == 0)
			EdenProtocolTransactionId = MIN_TRASACTION_ID;
	}

// check if there are header bytes (0x55) in the msg and add filler for every
// header byte found
// -----------------
  for (i = 0;i < DataLength ; i++)
  {
		EdenProtocolBuffer[BufIndex++] = Data[i];
		if (Data[i] == MSG_SYNC_1)
			EdenProtocolBuffer[BufIndex++] = MSG_FILLER;
		
	}
	
// insert the length to the msg after adding all the fillers
// ---------------------------------------------------------
	EdenProtocolBuffer[MSG_LENGTH_LSB_INDEX] = (BufIndex - EDEN_HEADER_SIZE) & LSB_MASK;
	EdenProtocolBuffer[MSG_LENGTH_FILLER_1_INDEX] = MSG_LENGTH_FILLER;
	EdenProtocolBuffer[MSG_LENGTH_MSB_INDEX] = ((BufIndex - EDEN_HEADER_SIZE) & MSB_MASK) >> 8;
	EdenProtocolBuffer[MSG_LENGTH_FILLER_2_INDEX] = MSG_LENGTH_FILLER;

// calculate the check sum and add it to the msg
// ---------------------------------------------
	for(i = SOURCE_ID_INDEX; i < BufIndex; i++)
  	CheckSum += EdenProtocolBuffer[i];

	EdenProtocolBuffer[BufIndex++] = CheckSum;

	
// send the msg through the uart
// -----------------------------
	if (UartSend(EdenProtocolBuffer, BufIndex) != UART_NO_ERROR)
		return EDEN_PROTOCOL_SEND_FAILED;

  return EDEN_PROTOCOL_NO_ERROR;

}


/****************************************************************************
 *
 *  NAME        : EdenProtocolDecodeTask
 *
 *  DESCRIPTION : This task checks if a new byte has arrived and decodes it.                                        
 *
 ****************************************************************************/
void EdenProtocolDecodeTask(BYTE Arg)
{
	BYTE xdata Data;

	if(RingBuffRemove(RcvRingBufferPtr,&Data)== E_RBUF_RING_BUFFER_OK)
  {
  	OHDBEdenProtocolDecode(Data); 
  }

	SchedulerLeaveTask(Arg);
  
}


/****************************************************************************
 *
 *  NAME        : EdenProtocolDecode
 *
 *  INPUT       : Data byte
 *
 *  OUTPUT      : None.
 *
 *  DESCRIPTION : Decode a byte from a message.                                        
 *
 ****************************************************************************/
void OHDBEdenProtocolDecode(BYTE RData)
{
	int xdata MsgHandle;
  BYTE xdata *Message;

	MsgHandle = GetAvailableMsgHandle(&MsgRingBuff);
	if (MsgHandle == -1)
		return;
	
	Message = GetMsgPtrByHandle(&MsgRingBuff, MsgHandle);
	if (Message == NULL)
		return;

  switch (RcvRingBufferPtr->DecodeState)
  {
  	case E_MSG_Header:
		// When Bytes is the first header byte
		// -----------------------------------
    	if (RData == C_1stHeaderByte)
    		RcvRingBufferPtr->DecodeState = E_MSG_InHeader;
    	break;

    case E_MSG_InHeader:
		// When Bytes is the second header byte move to detect the length
		// --------------------------------------------------------------
   		if (RData== C_2ndHeaderByte)
      	RcvRingBufferPtr->DecodeState = E_MSG_LengthLow;
  	//When any other case go back to search header
		// -------------------------------------------
      else
      	RcvRingBufferPtr->DecodeState = E_MSG_Header;
      break;

    case E_MSG_LengthLow:
    	RcvRingBufferPtr->MassegeLen        =    RData;
      RcvRingBufferPtr->PreviosByteIsSync =    (RData == C_1stHeaderByte);
      RcvRingBufferPtr->DecodeState       =    E_MSG_LengthFiller;
      RcvRingBufferPtr->NextState         =    E_MSG_LengthHigh;
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
        		RcvRingBufferPtr->DecodeState = E_MSG_Header;                 
        }
        else
        {    
          //Error state : reset state
          RcvRingBufferPtr->DecodeState = E_MSG_Header;
          RcvRingBufferPtr->PreviosByteIsSync = CLEAR; 
				}
       }
       break;

    case E_MSG_LengthHigh  :
    	// Keep flag in case this byte is A sync byte 
			// ------------------------------------------
      RcvRingBufferPtr->PreviosByteIsSync =    (RData == C_1stHeaderByte);  

      // Save high nibble of length
			// --------------------------
      RcvRingBufferPtr->MassegeLen = RcvRingBufferPtr->MassegeLen | ((RData) << 8);

      // Verify the length validity
			// --------------------------
      if (RcvRingBufferPtr->MassegeLen > 0 && RcvRingBufferPtr->MassegeLen <= EDEN_PROTOCOL_MAX_MSG_LENGTH)
      {
      	// Reset the buffer index
				// ----------------------
        RcvRingBufferPtr->BytesRcv = 0;
        RcvRingBufferPtr->DataBytesOnly = 0;

        //Reset the CheckSum byte
				// ----------------------
        RcvRingBufferPtr->CheckSum = 0;
        
		    RcvRingBufferPtr->NextState = E_MSG_SourceId;
        RcvRingBufferPtr->DecodeState     = E_MSG_LengthFiller;
      }
      else
      {
      	if(RData == C_1stHeaderByte)
        	RcvRingBufferPtr->DecodeState = E_MSG_InHeader;  
        else
        	// Wait for HEADER again
					// ---------------------
          RcvRingBufferPtr->DecodeState  = E_MSG_Header;
      }
      break;

    case E_MSG_SourceId:
			//When Byte is the SYNC HEADER byte
			if (RData== C_1stHeaderByte)
      {
      	RcvRingBufferPtr->DecodeState     = E_MSG_Filler;
        RcvRingBufferPtr->NextState = E_MSG_DestinationId;
      }
      else
      	RcvRingBufferPtr->DecodeState  = E_MSG_DestinationId;

      RcvRingBufferPtr->BytesRcv++;
      RcvRingBufferPtr->DataBytesOnly++;

			//Calculate CheckSum
      RcvRingBufferPtr->CheckSum = RcvRingBufferPtr->CheckSum + RData;

			// insert the source id to the msg buffer 
			InsertByte(&MsgRingBuff,MsgHandle,RData);
      break;

    case E_MSG_DestinationId:
			// When Byte is the SYNC HEADER byte
			// ---------------------------------
      if (RData== C_1stHeaderByte)
      {
      	RcvRingBufferPtr->DecodeState     = E_MSG_Filler;
        RcvRingBufferPtr->NextState = E_MSG_TransactionId;
      }
      else
      	RcvRingBufferPtr->DecodeState     = E_MSG_TransactionId;
        
      RcvRingBufferPtr->BytesRcv++;
      RcvRingBufferPtr->DataBytesOnly++;

			// Calculate CheckSum
   		// ------------------
      RcvRingBufferPtr->CheckSum = RcvRingBufferPtr->CheckSum+RData;

			// insert the destination id to the msg buffer 
			// -------------------------------------------
			InsertByte(&MsgRingBuff,MsgHandle,RData);
      break;

    case E_MSG_TransactionId:
			// When Byte is the SYNC HEADER byte
			// ---------------------------------
      if (RData== C_1stHeaderByte)
      {
      	RcvRingBufferPtr->DecodeState     = E_MSG_Filler;
        RcvRingBufferPtr->NextState = E_MSG_Data;
      }
      else
      	RcvRingBufferPtr->DecodeState    = E_MSG_Data;
        
      RcvRingBufferPtr->BytesRcv++;
      RcvRingBufferPtr->DataBytesOnly++;

			// Calculate CheckSum
			// ------------------
      RcvRingBufferPtr->CheckSum =RcvRingBufferPtr->CheckSum + RData;

			// insert the transaction id to the msg buffer 
			// -------------------------------------------
			InsertByte(&MsgRingBuff,MsgHandle,RData);
			break;

   case E_MSG_Data:
		// When Byte is the SYNC HEADER byte
		// ---------------------------------
    if (RData== C_1stHeaderByte)
    {
    	RcvRingBufferPtr->DecodeState     = E_MSG_Filler;
      RcvRingBufferPtr->NextState = E_MSG_Data;
    }

    InsertByte(&MsgRingBuff,MsgHandle,RData);

		// Calculate CheckSum
		// ------------------
    RcvRingBufferPtr->CheckSum =RcvRingBufferPtr->CheckSum + RData;

		// Advance counters
		// ----------------
    RcvRingBufferPtr->DataBytesOnly++;
    RcvRingBufferPtr->BytesRcv++;

		// When all bytes received EXCEPT CheckSum byte
		// --------------------------------------------
    if (RcvRingBufferPtr->BytesRcv >= RcvRingBufferPtr->MassegeLen)
    {
			// Move to CheckSum State
			// ----------------------
      RcvRingBufferPtr->DecodeState = E_MSG_CheckSum;
    }
    break;

  case E_MSG_CheckSum:
  	// When Checksum is correct
		// ------------------------
    if ((RcvRingBufferPtr->CheckSum & 0xff) != RData)
			RcvRingBufferPtr->CheckSumErrors++;
		else
 	  {
			// If checksum is ok then inc to the next buffer 
			// ---------------------------------------------
			MarkMsgAsReady(&MsgRingBuff);

			// We don't ResetCommunicationTimeout() here, but in the MessageDecode(), to make sure
			// We Reset only on messages accepted from EDEN Application, and not on messages accepted from OCB.
			// ResetCommunicationTimeout();
    }
    //And wait for HEADER again
    RcvRingBufferPtr->DecodeState  = E_MSG_Header;
    break;

       /*  to be used in the decode function 
             if (Message_Buffer.MsgBufFlage < MAX_MSG_BUF_SELECT)     
                 Message_Buffer.MsgBufIndex++                 
             else 
                 Message_Buffer.MsgBufFlage = 0;
      */
  case E_MSG_Filler:
		// When the byte is the filler byte
		// --------------------------------
    if (RData == C_FillerByte)
    {
    	if (RcvRingBufferPtr->BytesRcv + 1 >= RcvRingBufferPtr->MassegeLen)
      	RcvRingBufferPtr->DecodeState = E_MSG_CheckSum;
      else
      	RcvRingBufferPtr->DecodeState = RcvRingBufferPtr->NextState;

			// When in DATA state count the filler byte also
			// ---------------------------------------------	
	    if ((RcvRingBufferPtr->NextState == E_MSG_Data) ||
	        (RcvRingBufferPtr->NextState == E_MSG_DestinationId ) ||
	         (RcvRingBufferPtr->NextState == E_MSG_TransactionId ))
	    {
				// Advance counter
				// ---------------
	      RcvRingBufferPtr->BytesRcv++;
	
				//Calculate CheckSum
				RcvRingBufferPtr->CheckSum = RcvRingBufferPtr->CheckSum + RData;
	     }
    }
		else
		{
			// When the byte is the second Sync byte (0x33) then
			// maybe it is a sync header, moving to detect source
			//--------------------------------------------------
      if (RData == C_2ndHeaderByte)
      	RcvRingBufferPtr->DecodeState = E_MSG_LengthLow;
  		else
				// When any other case Filler error - try to Resync
				// ------------------------------------------------
      	RcvRingBufferPtr->DecodeState = E_MSG_Header;
    } 
    RcvRingBufferPtr->RcvCharIndex++;        
    break;
  }
}



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
RBUF_MsgRingBuffer *OHDBEdenProtocolGetMsgBuff(void)
{
  return &MsgRingBuff; 
}

