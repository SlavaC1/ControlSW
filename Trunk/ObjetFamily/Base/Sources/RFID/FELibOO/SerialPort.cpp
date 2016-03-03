//#include "stdafx.h"
#include <windows.h>
#include <memory.h>
#include "SerialPort.h"


//-- DLL Main ----------------------------------------------------------------
CSerialPort::CSerialPort()
{
    memset( &sBuf[0], '\0', sizeof(sBuf));
	for ( int i = 0; i < 255; i++ )
		SerialPortProperties.sPort[i] = 0;
	InitSerialPortProperties();
}

void CSerialPort::InitSerialPortProperties()
{
    // Initialize SerialPortProperties struct
	strcpy( SerialPortProperties.sPort, "COM1" );
	SerialPortProperties.BaudRate = CBR_9600;
	SerialPortProperties.ParityCheck = FALSE;
	SerialPortProperties.CTS_FlowControl = FALSE;
	SerialPortProperties.DSR_FlowControl = FALSE;
	SerialPortProperties.DTR_FlowControl = DTR_CONTROL_ENABLE;
	SerialPortProperties.DSR_Sensitivity = FALSE;
	SerialPortProperties.RTS_FlowControl = RTS_CONTROL_ENABLE;
	SerialPortProperties.ByteSize = 8;
	SerialPortProperties.Parity = NOPARITY;
	SerialPortProperties.StopBits = ONESTOPBIT; 

	handleComPort = NULL;
	nReadTimeOut = 1000;
	nWriteTimeOut = 1000;
}



//----------------------------------------------------------------------------
// Implementation of Exported Functions
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
DWORD CSerialPort::SP_Init( char* sComPort )
//----------------------------------------------------------------------------
{	// create handle to com port
	SP_Exit();
	strcpy( SerialPortProperties.sPort , sComPort );



	handleComPort = CreateFileA(	(LPCSTR)SerialPortProperties.sPort,			// pointer to name of the file
  								GENERIC_READ | GENERIC_WRITE,		// access (read-write) mode
								0,									// share mode
								NULL,								// pointer to security attributes
								OPEN_EXISTING,						// how to create
								FILE_FLAG_OVERLAPPED,				// file attributes
								NULL								// handle to file with attributes to copy
							  );	
	if( handleComPort == INVALID_HANDLE_VALUE )
	{	// error opening com port
		handleComPort = NULL;
		return( GetLastError() );
	}
	// Set TimeOuts
	int status = SP_SetComTimeOuts( nReadTimeOut, nWriteTimeOut );
	if( status != ERROR_SUCCESS )
	{	return( status );
	}
	// Setup comm, define size of inbuffer and outbuffer
	SetupComm( handleComPort, 256, 256 );
	// Abort all outstandig r/w operations
	PurgeComm( handleComPort, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR );
	// create DCB
	DCB	dcb = {0};	
	if( !GetCommState( handleComPort, &dcb ) )
	{	CloseHandle( handleComPort );
		handleComPort = NULL;
		return( GetLastError() );
	}
	dcb.DCBlength = sizeof( dcb );									// sizeof(DCB) 
    dcb.BaudRate = SerialPortProperties.BaudRate;					// baud rate     
	dcb.fBinary = TRUE;												// binary mode, no EOF check 
    dcb.fParity = SerialPortProperties.ParityCheck;					// enable parity checking 
    dcb.fOutxCtsFlow = FALSE;//SerialPortProperties.CTS_FlowControl;		// CTS output flow control 
    dcb.fOutxDsrFlow = FALSE;//SerialPortProperties.DSR_FlowControl;		// DSR output flow control 
    dcb.fDtrControl = DTR_CONTROL_DISABLE;//SerialPortProperties.DTR_FlowControl;			// DTR flow control type 
    dcb.fDsrSensitivity = FALSE;//SerialPortProperties.DSR_Sensitivity;		// DSR sensitivity 
	/////////////////////////////////
	// new block
	dcb.fTXContinueOnXoff = FALSE; //new
	dcb.fOutX = FALSE;
	dcb.fInX = FALSE;
	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	//dcb.fRtsControl = SerialPortProperties.RTS_FlowControl;			// RTS flow control 
	/////////////////////////////////
    //DWORD fTXContinueOnXoff:1;									// XOFF continues Tx 
	//DWORD fOutX: 1;												// XON/XOFF out flow control 
    //DWORD fInX: 1;												// XON/XOFF in flow control 
    //DWORD fErrorChar: 1;											// enable error replacement 
    //DWORD fNull: 1;												// enable null stripping 
    
    //DWORD fAbortOnError:1;										// abort reads/writes on error 
    //DWORD fDummy2:17;												// reserved 
    //WORD wReserved;												// not currently used 
    //WORD XonLim;													// transmit XON threshold 
    //WORD XoffLim;													// transmit XOFF threshold 
    dcb.ByteSize = SerialPortProperties.ByteSize;					// number of bits/byte, 4-8 
    dcb.Parity = SerialPortProperties.Parity;						// 0-4=no,odd,even,mark,space 
    dcb.StopBits = SerialPortProperties.StopBits;					// 0,1,2 = 1, 1.5, 2 
    //char XonChar;													// Tx and Rx XON character 
    //char XoffChar;												// Tx and Rx XOFF character 
    //char ErrorChar;												// error replacement character 
    //char EofChar;													// end of input character 
    //char EvtChar;													// received event character 
    //WORD wReserved1;												// reserved; do not use 	
	// setup comm state
	if( !SetCommState( handleComPort, &dcb ) )
	{	CloseHandle( handleComPort );
		handleComPort = NULL;
		return( GetLastError() );
	}
	// Com port successfully opened and init
	return( ERROR_SUCCESS );
}



//----------------------------------------------------------------------------
DWORD CSerialPort::SP_Exit( void )
//----------------------------------------------------------------------------
{	

	if( handleComPort == NULL ) 
	{	SetLastError( ERROR_INVALID_HANDLE );
		return( GetLastError() );
	}

	if( !CloseHandle( handleComPort ) )
	{	return( GetLastError() );
	}

	handleComPort = NULL;
	// Com port successfully closed
	SetLastError( ERROR_SUCCESS );

	return( GetLastError() );
}

//----------------------------------------------------------------------------
DWORD CSerialPort::SP_SetRTS( BOOL bSet )
//----------------------------------------------------------------------------
{	if( handleComPort == NULL ) 
	{	SetLastError( ERROR_INVALID_HANDLE );
		return( ERROR_INVALID_HANDLE );
	}
	BOOL	status = FALSE;
	if( bSet )
	{	status = EscapeCommFunction( handleComPort, SETRTS );
	}else
	{	status = EscapeCommFunction( handleComPort, CLRRTS );
	}
	if( !status )
	{	return( GetLastError() );
	}
	SetLastError( ERROR_SUCCESS );
	return( GetLastError() );
}



//----------------------------------------------------------------------------
DWORD CSerialPort::SP_SetDTR( BOOL bSet )
//----------------------------------------------------------------------------
{	if( handleComPort == NULL ) 
	{	SetLastError( ERROR_INVALID_HANDLE );
		return( GetLastError() );
	}
	BOOL	status = FALSE;
	if( bSet )
	{	status = EscapeCommFunction( handleComPort, SETDTR );
	}else
	{	status = EscapeCommFunction( handleComPort, CLRDTR );
	}
	if( !status )
	{	return( GetLastError() );
	}
	SetLastError( ERROR_SUCCESS );
	return( GetLastError() );
}



//----------------------------------------------------------------------------
DWORD CSerialPort::SP_SetBaudrate( unsigned long nBaudrate )
//----------------------------------------------------------------------------
{	if( handleComPort == NULL ) 
	{	SetLastError( ERROR_INVALID_HANDLE );
		return( GetLastError() );
	}
    //
	DCB dcb;
	if( !GetCommState( handleComPort, &dcb ) )
	{	return( GetLastError() );
	}
    dcb.BaudRate = nBaudrate;
    //
	SetCommState( handleComPort, &dcb );
	return( GetLastError() );
}



//----------------------------------------------------------------------------
DWORD CSerialPort::SP_Exchange( unsigned char* sSendData, unsigned char nSendDataLen,
                             unsigned char* sRecvData, unsigned char* nRecvDataLen,
                             BOOL bUseFirstRecvByteAsMaxRecvLengthInfo /*= FALSE*/ )
//----------------------------------------------------------------------------
{	if( handleComPort == NULL ) 
	{	SetLastError( ERROR_INVALID_HANDLE );
		return( GetLastError() );
	}
    //
	unsigned long written;
	SendCommand( sSendData, nSendDataLen, written );

        #pragma warn -8012 // Diasable warning Number 8012 "Comparing signed to unsigned values"
	if( written != nSendDataLen )
	{	SetLastError( ERROR_WRITE_FAULT );
		return( GetLastError() );
	}
        #pragma warn .8012 // Enable warning Number 8012 "Comparing signed to unsigned values"
    //
    return( SP_Receive( sRecvData, nRecvDataLen, 
                        bUseFirstRecvByteAsMaxRecvLengthInfo ) );
}



//----------------------------------------------------------------------------
DWORD CSerialPort::SP_Send( unsigned char* sSendData, unsigned char nSendDataLen )
//----------------------------------------------------------------------------
{	if( handleComPort == NULL ) 
	{	SetLastError( ERROR_INVALID_HANDLE );
		return( GetLastError() );
	}
	// Abort all outstandig r/w operations and clear buffers
	PurgeComm( handleComPort, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR );
    //
	unsigned long written;

	SendCommand( sSendData, nSendDataLen, written );
	if( !FlushFileBuffers( handleComPort ) )
	{	return( GetLastError() );
	}
       #pragma warn -8012 // Diasable warning Number 8012 "Comparing signed to unsigned values"
	if( written != nSendDataLen )
	{	SetLastError( ERROR_WRITE_FAULT );
		return( GetLastError() );
	}
        #pragma warn .8012 // Enable warning Number 8012 "Comparing signed to unsigned values"
	// Send successful
	SetLastError( ERROR_SUCCESS );
	return( GetLastError() );
}



//----------------------------------------------------------------------------
DWORD CSerialPort::SP_Receive( unsigned char* sRecvData, unsigned char* nRecvDataLen,
                            BOOL bUseFirstRecvByteAsMaxRecvLengthInfo /*= FALSE*/ )
//----------------------------------------------------------------------------
{	
	try
	{	
		if( handleComPort == NULL ) 
		{	SetLastError( ERROR_INVALID_HANDLE );
			return( GetLastError() );
		}
		//
		unsigned char	maxLen = *nRecvDataLen;
		*nRecvDataLen = 0;
		while( GetResponse( sRecvData[ *nRecvDataLen ] ) && ( *nRecvDataLen < maxLen ) )
		{	// get hole response
			if( ( bUseFirstRecvByteAsMaxRecvLengthInfo ) && ( *nRecvDataLen == 0 ) )
			{   // use the first received byte to detect the length of the received frame
				if( maxLen > sRecvData[ 0 ] )
				{   maxLen = sRecvData[ 0 ];
				}
			}
			( *nRecvDataLen )++;
		}


		// Exchange successful
		SetLastError( ERROR_SUCCESS );
		// Clear input buffer
		PurgeComm( handleComPort, PURGE_RXCLEAR );
		//
	}
	catch(...)
	{
	}
    return( GetLastError() );
}

//----------------------------------------------------------------------------
char* CSerialPort::SP_GetErrorMessage( int nError )
//----------------------------------------------------------------------------
{	FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM |
				   FORMAT_MESSAGE_IGNORE_INSERTS,
				   NULL,
				   nError,
				   MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
				   ( LPTSTR ) &sBuf,
				   1024,
				   NULL );
	return( sBuf );
}



//----------------------------------------------------------------------------
DWORD CSerialPort::SP_SetComTimeOuts( unsigned long nReadComTimeOut, unsigned long nWriteComTimeOut )
//----------------------------------------------------------------------------
{	nReadTimeOut = nReadComTimeOut; 
	nWriteTimeOut = nWriteComTimeOut;
	
	if( handleComPort == NULL ) 
	{	SetLastError( ERROR_INVALID_HANDLE );
		return( ERROR_INVALID_HANDLE );
	}

	COMMTIMEOUTS TimeOuts;

	if( handleComPort == INVALID_HANDLE_VALUE )
	{	return( GetLastError() );
	}
	
	// set ReadTimeOuts
	TimeOuts.ReadIntervalTimeout = MAXDWORD; 
	TimeOuts.ReadTotalTimeoutMultiplier = MAXDWORD; 
	TimeOuts.ReadTotalTimeoutConstant = nReadTimeOut; 
	// set WriteTimeOuts
	TimeOuts.WriteTotalTimeoutMultiplier = 0; 
	TimeOuts.WriteTotalTimeoutConstant = nWriteTimeOut;

	if( !SetCommTimeouts( handleComPort, &TimeOuts ) )
	{	return( GetLastError() );
	}
	
	// TimeOuts successfully set
	SetLastError( ERROR_SUCCESS );
	return( GetLastError() );
}



//----------------------------------------------------------------------------
void CSerialPort::SP_GetComTimeOuts( unsigned long& nReadComTimeOut, unsigned long& nWriteComTimeOut )
//----------------------------------------------------------------------------
{	
    nReadComTimeOut = nReadTimeOut; 
	nWriteComTimeOut = nWriteTimeOut;
}



//----------------------------------------------------------------------------
BOOL CSerialPort::SP_IsCTS( void )
//----------------------------------------------------------------------------
{	DWORD dwModemStatus;
	if( !GetCommModemStatus( handleComPort, &dwModemStatus ) )
	{	return( GetLastError() );
	} 
	// Check CTS line
	if( MS_CTS_ON & dwModemStatus )
	{	return( TRUE );
	}else
	{	return( FALSE );
	}
}
//----------------------------------------------------------------------------
BOOL CSerialPort::SP_IsDSR( void )
//----------------------------------------------------------------------------
{	DWORD dwModemStatus;
	if( !GetCommModemStatus( handleComPort, &dwModemStatus ) )
	{	return( GetLastError() );
	} 
	// Check DSR line
	if( MS_DSR_ON & dwModemStatus )
	{	return( TRUE );
	}else
	{	return( FALSE );
	}
}



//
void CSerialPort::SendCommand( unsigned char* sCommand, unsigned char nToWrite, unsigned long& nBytesWritten )
{	if( ClearPort() || PortStatus() )
	{	// error clear port or at port status
		nBytesWritten = 0;
		return;
	}
	OVERLAPPED	osWrite = {0};
	DWORD		dwWritten = 0;   
	// Create this write operation's OVERLAPPED structure's hEvent.
	osWrite.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	if( osWrite.hEvent == NULL )
	{	// error creating overlapped event handle (Issue write)
		nBytesWritten = 0;
	}else
	{	if( !WriteFile( handleComPort, sCommand, nToWrite, &dwWritten, &osWrite ) ) 
		{	if( GetLastError() != ERROR_IO_PENDING ) 
			{	// WriteFile failed, but isn't delayed. Report error and abort.
			}else
			{	// Write is pending.
				switch( WaitForSingleObject( osWrite.hEvent, nWriteTimeOut ) )
				{	// OVERLAPPED structure's event has been signaled. 
					case WAIT_OBJECT_0:
						if( GetOverlappedResult( handleComPort, &osWrite, &dwWritten, FALSE ) )
						{	// Write operation completed successfully.
							nBytesWritten = dwWritten;
						}
						break;            
					default:
						// An error has occurred in WaitForSingleObject.
						// This usually indicates a problem with the
						// OVERLAPPED structure's event handle.
						break;         
				}      
			}
		}else 
		{	// WriteFile completed immediately.      
		}
		CloseHandle( osWrite.hEvent );   
	}
}



//----------------------------------------------------------------------------
//
BOOL CSerialPort::GetResponse( unsigned char& cResponse )
{	static unsigned char ResponseBuf[2];
	static unsigned long dwBytesRead;
	OVERLAPPED osReader = {0};
	BOOL	fSuccess = FALSE;
	// Create the overlapped event. Must be closed before exiting
	// to avoid a handle leak.
	osReader.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	if( osReader.hEvent == NULL )
	{	// error creating overlapped event handle
	}else
	{	if( !ReadFile( handleComPort, ResponseBuf, 1, &dwBytesRead, &osReader ) ) 
		{	// read not delayed?
			if( GetLastError() != ERROR_IO_PENDING )     
			{	// Error in communications; report it.
			}else  
			{	//wait for completion of read
				switch( WaitForSingleObject( osReader.hEvent, nReadTimeOut ) )
				{	// Read completed.
					case WAIT_OBJECT_0:
						if( !GetOverlappedResult( handleComPort, &osReader, &dwBytesRead, FALSE ) )
						{	break;
						}else
						{	//read successfull
							if( dwBytesRead != 1 )
							{	break;
							}
							cResponse = ResponseBuf[0];
							fSuccess = TRUE;
							break;
						}
					case WAIT_TIMEOUT: //timeout occourred 
						break;
					default:
						// Error in the WaitForSingleObject; abort.
						// This indicates a problem with the OVERLAPPED structure's event handle.
						break;
				}
			}
		}else
		{	if( dwBytesRead != 1 )
			{
			}else
			{	cResponse = ResponseBuf[0];
				fSuccess = TRUE;
			}
		}
	    CloseHandle( osReader.hEvent );
    }
	return( fSuccess );
}



//----------------------------------------------------------------------------
//
unsigned long CSerialPort::ClearPort()
{	COMSTAT comStat;
	DWORD   dwErrors;
	// Get and clear current errors on the port.
    if( !ClearCommError( handleComPort, &dwErrors, &comStat))
	{	return( GetLastError() );
	}  
	// Clear port successful
	SetLastError( ERROR_SUCCESS );
	return( GetLastError() );
}



unsigned long CSerialPort::PortStatus()
{	DWORD dwModemStatus;
	BOOL  fCTS, fDSR, fRING, fRLSD;
	if( !GetCommModemStatus( handleComPort, &dwModemStatus ) )
	{	return( GetLastError() );
	}  
	fCTS = MS_CTS_ON & dwModemStatus;
	fDSR = MS_DSR_ON & dwModemStatus;
	fRING = MS_RING_ON & dwModemStatus;
	fRLSD = MS_RLSD_ON & dwModemStatus;
	if( fCTS || fDSR || fRING || fRLSD )
	{	// Card not inserted || Reader not powered || ??? || ???
		return( 0xFFFFFFFF );
	}
	// port status ok
	SetLastError( ERROR_SUCCESS );
	return( GetLastError() );
}



//----------------------------------------------------------------------------
//

