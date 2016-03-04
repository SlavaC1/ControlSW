/********************************************************************
*  HEADER MODULE: I2C_DRV.h
*
*  Serial Data Interface Module Headers
*
*  DESCRIPTION: Prototypes for I2C Serial Interface for the C8051F022
*               and other Silabs chips
*   
*  
*   Author : Elad Hazan
*********************************************************************/
#ifndef _I2C_DRV_H_
#define _I2C_DRV_H_

//#define DEBUG_SEND_DEBUG_MESSAGE_TO_PC

//-----------------------------------------------------------------------------   
// Global CONSTANTS   
//-----------------------------------------------------------------------------   
   
#define  SYSCLK              4000000L       // System clock frequency in Hz   
   
#define  SMB_FREQUENCY       10000L         // Target SCL clock rate   
                                       // This example supports between 10kHz   
                                       // and 100kHz   
   

#define ESMB0		          0x02		   // SMB interrupt enable bit in sfr EIE1
#define  WRITE                0x00           // WRITE direction bit   
#define  READ                 0x01           // READ direction bit   
   
                                         
#define  SMB_BUS_ERROR  	  0x00           // (all modes) BUS ERROR   
#define  SMB_START            0x08           // (MT & MR) START transmitted   
#define  SMB_RP_START   	  0x10           // (MT & MR) repeated START   
#define  SMB_MTADDACK         0x18           // (MT) Slave address + W transmitted; ACK received                                         //       
#define  SMB_MTADDNACK  	  0x20           // (MT) Slave address + W transmitted; NACK received                                        //       
#define  SMB_MTDBACK    	  0x28           // (MT) data byte transmitted; ACK received        
#define  SMB_MTDBNACK   	  0x30           // (MT) data byte transmitted; NACK received                                              
#define  SMB_MTARBLOST  	  0x38           // (MT) arbitration lost   
#define  SMB_MRADDACK   	  0x40           // (MR) Slave address + R transmitted; ACK received                                                
#define  SMB_MRADDNACK  	  0x48           // (MR) Slave address + R transmitted; NACK received                                               
#define  SMB_MRDBACK    	  0x50           // (MR) data byte rec'vd; ACK transmitted                    
#define  SMB_MRDBNACK         0x58           // (MR) data byte rec'vd; NACK transmitted  
#define  NUM_BYTES_WR         1              // Number of bytes to write Master -> Slave              
#define  NUM_INCOMING_BYTES   19	          // Number of bytes to read
#define  FIRST_TWO_BYTES      2
#define  LAST_BYTE            10                                          
#define  NUM_OF_MSC_CARDS     2
#define  BYTES_TO_WORDS       11
#define  ON_OFF_DELAY		  250 //Delay for I2C_NO_SLAVE Message

// Device addresses (7 bits, lsb is R/W)     
#define  FIRST_CHIP_SELECT    0xA4 //First MSC card address  
#define  SECOND_CHIP_SELECT   0xAA //Second MSC card address
//-----------------------------------------------------------------------------   
// Analog Inputs Decleration   
//-----------------------------------------------------------------------------
#define SWITCHES			    0
#define	PUMPS_STATUS			1
#define	LOAD_CELL_1_STATUS		2
#define	LOAD_CELL_2_STATUS		3
#define	LOAD_CELL_3_STATUS	    4
#define	LOAD_CELL_4_STATUS	    5
#define	_24V_STATUS			    6
#define	_7V_STATUS			    7
#define	_5V_STATUS			    8
#define	_3_3V_STATUS		    9
//-----------------------------------------------------------------------------   
// Function PROTOTYPES   
//-----------------------------------------------------------------------------   


enum  {FIRSTMSCCARD=0, SECONDMSCCARD};
enum  {I2C_WRITE_OP=0, I2C_READ_OP};
enum  {BUS_FREE=0, BUS_BUSY};
enum{  
  I2C_NO_ERROR,
  MSC1_NO_SLAVE,
  MSC1_HW_ERROR,
  MSC1_COM_ERROR,
  MSC2_NO_SLAVE,
  MSC2_HW_ERROR,
  MSC2_COM_ERROR,
  I2C_ERROR  //ISR is not called, I2C error
  };
   
void Port_Init(void);   
void SMBus_Init(void);   
//void Timer3_Init(void);   
   
void SMBus_ISR(void);   
void Timer3_ISR(void);   
   
void SMB_Write (void);   
void SMB_Read (void);
#ifndef OCB_SIMULATOR
BYTE Incoming_Bytes_To_Words(BYTE FirstSecondMSC);
void I2C_Status_Task(BYTE Arg);
void I2CPrepareStatusMsg();
BYTE CheckForError(BYTE TargetToCheck);
#endif
BYTE I2CWriteByte( BYTE address, BYTE dat);
WORD I2CA2D_GetReading(BYTE AnalogInNum,BYTE FirstSecondMSC);
void I2CA2D_GetBlockReading(WORD *state, BYTE FirstSecondMSC);

void I2C_SetOnOff(BOOL CurrPowerOnOffReq);
/*********************************/
#ifdef DEBUG_SEND_DEBUG_MESSAGE_TO_PC
void BeginDebugMessage();
void EndDebugMessage();
void AppendToDebugMessage(BYTE DataReg);
#endif
#endif    