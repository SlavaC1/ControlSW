#if !defined __SERIAL_PORT_MG_H
#define __SERIAL_PORT_MG_H

#include <windows.h>
//#include <afx.h>
//#include "afxwin.h"
#include "CmdCodes.h"
#include "ErrorCodes.h"




//////////////////////////////////////////////////////////////////////////////
// Define SerialPort Properties Structur
typedef struct tagSerialPortProperties
{	char			sPort[255];			// = "COM1"
	unsigned long	BaudRate;		    // = CBR_9600
	unsigned long	ParityCheck;	    // = 0 = FALSE
	unsigned long	CTS_FlowControl;    // = 0 = FALSE
	unsigned long	DSR_FlowControl;    // = 0 = FALSE
	unsigned long	DTR_FlowControl;    // = DTR_CONTROL_ENABLE
	unsigned long	DSR_Sensitivity;    // = 0 = FALSE
	unsigned long	RTS_FlowControl;    // = RTS_CONTROL_ENABLE
	unsigned char	ByteSize;		    // = 8
	unsigned char	Parity;			    // = NOPARITY
	unsigned char	StopBits;		    // = ONESTOPBIT
	// All data is init with default values ( see above )
	// in DLL_PROCESS_ATTACH path in DllMain( .. )
	// Notes : For more information about the constant values
	// see the chapter "DCB" in the MSDN Help.
}SERIAL_PORT_PROPERTIES;

class CSerialPort
{
public:
	CSerialPort();
void InitSerialPortProperties();


//----------------------------------------------------------------------------
//-- Prototypes --------------------------------------------------------------

//-- Exported Variablen ------------------------------------------------------
SERIAL_PORT_PROPERTIES		SerialPortProperties;

//-- Exported Functions ( for static linkage )--------------------------------

// ---------------------------------------------------------------------------
//  FUNCTION: SP_Init
//        IN: -
//       OUT: -
//    RETURN: WIN32 SDK Error Codes
//   COMMENT: Init the Serial Port with the SerialPortProperties struct
DWORD	SP_Init( char* sComPort );
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//  FUNCTION: SP_Exit
//        IN: -
//       OUT: -
//    RETURN: WIN32 SDK Error Codes
//   COMMENT: Destroy the Handle to Serial Port
DWORD	SP_Exit( void );
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  FUNCTION: SP_SetRTS
//        IN: bSet
//       OUT: -
//    RETURN: WIN32 SDK Error Codes
//   COMMENT: Set or Reset the RTS line
DWORD	SP_SetRTS( BOOL bSet );
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  FUNCTION: SP_SetDTR
//        IN: bSet
//       OUT: -
//    RETURN: WIN32 SDK Error Codes
//   COMMENT: Set or Reset the DTR line
DWORD	SP_SetDTR( BOOL bSet );
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  FUNCTION: SP_SetBaudRate
//        IN: nBaudRate
//       OUT: -
//    RETURN: WIN32 SDK Error Codes
//   COMMENT: Set the baudrate
DWORD	SP_SetBaudrate( unsigned long nBaudrate );
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  FUNCTION: SP_Exchange
//        IN: nSendDataLen, bUseFirstRecvByteAsMaxRecvLengthInfo
//       OUT: sSendData, sRecvData
//    IN/OUT: nRecvDataLen
//    RETURN: WIN32 SDK Error Codes
//   COMMENT: Send the sSendData string to the port
//            and Receive nRecvDataLen Bytes from the port.
//            nRecvDataLen has to init with the max no. of bytes to receive
//            and is also used to store the no. of really received bytes
DWORD	SP_Exchange( unsigned char* sSendData, unsigned char nSendDataLen,
                                           unsigned char* sRecvData, unsigned char* nRecvDataLen,
                                           BOOL bUseFirstRecvByteAsMaxRecvLengthInfo = FALSE );
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  FUNCTION: SP_Send
//        IN: nSendDataLen
//       OUT: sSendData
//    RETURN: WIN32 SDK Error Codes
//   COMMENT: Send the sSendData string to the port
DWORD	SP_Send( unsigned char* sSendData, unsigned char nSendDataLen );
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  FUNCTION: SP_Receive
//        IN: bUseFirstRecvByteAsMaxRecvLengthInfo
//       OUT: sRecvData
//    IN/OUT: nRecvDataLen
//    RETURN: WIN32 SDK Error Codes
//   COMMENT: Receive nRecvDataLen Bytes from the port.
//            nRecvDataLen has to init with the max no. of bytes to receive
//            and is also used to store the no. of really received bytes
DWORD	SP_Receive( unsigned char* sRecvData, unsigned char* nRecvDataLen,
                                          BOOL bUseFirstRecvByteAsMaxRecvLengthInfo = FALSE );
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  FUNCTION: SP_SetComTimeOuts
//        IN: nReadComTimeOut, nWriteComTimeOut
//       OUT: -
//    RETURN: WIN32 SDK Error Codes
//   COMMENT: Set the Timeouts for used Serial Port
DWORD	SP_SetComTimeOuts( unsigned long nReadComTimeOut, unsigned long nWriteComTimeOut );
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  FUNCTION: SP_GetComTimeOuts
//        IN: -
//       OUT: nReadComTimeOut, nWriteComTimeOut
//    RETURN: -
//   COMMENT: Get the Timeouts for used Serial Port
void	    SP_GetComTimeOuts( unsigned long& nReadComTimeOut, unsigned long& nWriteComTimeOut );
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  FUNCTION: SP_GetErrorMessage
//        IN: nError
//       OUT: -
//    RETURN: String that contains the formated error message
//   COMMENT: The returned string is 1024 bytes long !
char*	SP_GetErrorMessage( int nError );
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  FUNCTION: SP_IsCTS
//        IN: -
//       OUT: -
//    RETURN: CTS line status
//   COMMENT: Return TRUE if the line is set or FALSE if it is not
BOOL		SP_IsCTS( void );
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  FUNCTION: SP_IsDSR
//        IN: -
//       OUT: -
//    RETURN: DSR line status
//   COMMENT: Return TRUE if the line is set or FALSE if it is not
BOOL		SP_IsDSR( void );
// ---------------------------------------------------------------------------

protected:
void            SendCommand( unsigned char* sCommand, unsigned char nToWrite, unsigned long& nBytesWritten );
BOOL            GetResponse( unsigned char& cResponse );
unsigned long   ClearPort( void );
unsigned long   PortStatus( void );

//-- Not Exported Variablen --------------------------------------------------
HANDLE			handleComPort;// = NULL;
unsigned long	nReadTimeOut;// = 1000;
unsigned long	nWriteTimeOut;// = 1000;
char			sBuf[1024];


};


#endif // !defined __SERIAL_PORT_MG_H


