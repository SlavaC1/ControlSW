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

// typdef for the task function callback
typedef void (*TFuncPtr)(void);
typedef void (*TErrorFuncPtr)(BYTE);

//#define DEBUG_SEND_DEBUG_MESSAGE_TO_PC

//-----------------------------------------------------------------------------   
// Global CONSTANTS   
//-----------------------------------------------------------------------------   

#define  SYSCLK              4000000L  // System clock frequency in Hz   

#define  SMB_FREQUENCY       10000L   // Target SCL clock rate   
// This example supports between 10kHz and 100kHz  

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
#define  BYTES_TO_WORDS       11
#define  ON_OFF_DELAY		  150 //Delay for I2C_NO_SLAVE Message

enum  
{
	I2C_WRITE_OP = 0, 
	I2C_READ_OP
};

enum
{
	BUS_FREE = 0,
	BUS_BUSY
};

enum
{
	I2C_ERROR = 0,
	I2C_NO_SLAVE,
	I2C_NO_ERROR
};

void Port_Init(void);   
void SMBus_Init(void);         
void SMBus_ISR(void);

BYTE I2CWrite(BYTE ChipAddress, BYTE* Data, BYTE Size);   
BYTE I2CRead (BYTE ChipAddress, BYTE* Data, BYTE Size, TFuncPtr callback, TErrorFuncPtr OnMSCFailCallback);
void I2C_SetOnOff(BOOL CurrPowerOnOffReq);
BYTE I2CBusBusy();

void BeginDebugMessage();
void EndDebugMessage();
void AppendToDebugMessage(BYTE DataReg);
#endif 
