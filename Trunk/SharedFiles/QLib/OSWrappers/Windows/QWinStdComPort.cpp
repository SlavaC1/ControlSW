/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (ComPort - standrad)                    *
 * Module Description: Windows implementation for the standard      *
 *                     serial com port class.                       *
 *                                                                  *
 * Compilation: Standard C++, Win32                                 *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 05/08/2001                                           *
 * Last upate: 20/08/2001                                           *
 ********************************************************************/

#include <stdio.h>
#include "QUtils.h"
#include "QStdComPort.h"


// Default constructor
CQStdComPort::CQStdComPort(void)
{
  m_Handle = INVALID_HANDLE_VALUE;
  m_ComNum = m_BaudRate = m_Parity  = m_DataBits = m_StopBits = 0;
}

// Initialization constructor
CQStdComPort::CQStdComPort(int ComNum,int BaudRate,char Parity,int DataBits,int StopBits)
{
  m_Handle = INVALID_HANDLE_VALUE;
  m_ComNum = m_BaudRate = m_Parity  = m_DataBits = m_StopBits = 0;
  Init(ComNum,BaudRate,Parity,DataBits,StopBits);
}

// Destructor
CQStdComPort::~CQStdComPort(void)
{
  DeInit();
}

// Write data to the com port
ULONG CQStdComPort::Write(void *Data,unsigned DataLength)
{
  ULONG ActualBytesWrittenCount;

  m_WriteOverlapped.Internal = 0;
  m_WriteOverlapped.InternalHigh = 0;
  m_WriteOverlapped.Offset = 0;
  m_WriteOverlapped.OffsetHigh = 0;
  m_WriteOverlapped.hEvent = m_WriteEvent.Handle();

  if(!::WriteFile(m_Handle,Data,DataLength,&ActualBytesWrittenCount,&m_WriteOverlapped))
  {
    DWORD err = GetLastError();

    if(err != ERROR_IO_PENDING)
      throw EQComPort("Can not write com port COM" + QIntToStr(m_ComNum),err);
  }

  // Wait for read completion
  if(!GetOverlappedResult(m_Handle,&m_WriteOverlapped,&ActualBytesWrittenCount,TRUE))
    throw EQComPort("Can not write com port COM" + QIntToStr(m_ComNum),GetLastError());

  // Return the actual number of bytes read
  return ActualBytesWrittenCount;
}

// Read data from the com port
ULONG CQStdComPort::Read(void *Data,unsigned DataLength)
{
  ULONG ActualBytesReadCount;

  // Prepare the read overlapped structure
  m_ReadOverlapped.Internal = 0;
  m_ReadOverlapped.InternalHigh = 0;
  m_ReadOverlapped.Offset = 0;
  m_ReadOverlapped.OffsetHigh = 0;
  m_ReadOverlapped.hEvent = m_ReadEvent.Handle();

  if(!::ReadFile(m_Handle,Data,DataLength,&ActualBytesReadCount,&m_ReadOverlapped))
  {
    DWORD err = GetLastError();

    if(err != ERROR_IO_PENDING)
      throw EQComPort("Can not read com port COM" + QIntToStr(m_ComNum),err);
  }

  // Wait for read completion
  if(!GetOverlappedResult(m_Handle,&m_ReadOverlapped,&ActualBytesReadCount,TRUE))
    throw EQComPort("Can not read com port COM" + QIntToStr(m_ComNum),GetLastError());

  // Return the actual number of bytes read
  return ActualBytesReadCount;
}

// Set the com port read timeout in ms
void CQStdComPort::SetReadTimeout(ULONG TimeoutInMs)
{
  COMMTIMEOUTS NewTimeouts;

  NewTimeouts.ReadTotalTimeoutConstant = TimeoutInMs;
  NewTimeouts.ReadTotalTimeoutMultiplier = 0;
  NewTimeouts.WriteTotalTimeoutMultiplier = 0;
  NewTimeouts.WriteTotalTimeoutConstant = 0;
  ::SetCommTimeouts(m_Handle,&NewTimeouts);
}

// Flush input and output buffers
void CQStdComPort::Flush(void)
{
  if((PurgeComm(m_Handle,PURGE_RXABORT | PURGE_RXCLEAR) == 0) ||
     (PurgeComm(m_Handle,PURGE_TXABORT | PURGE_TXCLEAR) == 0))
    throw EQComPort("Can not flush com port COM" + QIntToStr(m_ComNum),GetLastError());
}

// Init function for the common properties
void CQStdComPort::Init(int ComNum,int BaudRate,char Parity ,int DataBits,int StopBits)
{
  // If already open, close first
  if(m_Handle != INVALID_HANDLE_VALUE)
    DeInit();

  DCB dcbCommPort;

  // Create the com port file name
  QString ComStr = "COM" + QIntToStr(ComNum);

  // Open the com port file
  m_Handle = ::CreateFile(ComStr.c_str(),GENERIC_READ | GENERIC_WRITE,0,0,
                         OPEN_EXISTING,FILE_FLAG_OVERLAPPED,0);

  // Check if the port opened Ok
  if(m_Handle == INVALID_HANDLE_VALUE)
    throw EQComPort("Can not open com port " + ComStr,GetLastError());

  // Remember the com port settings
  m_ComNum   = ComNum;
  m_BaudRate = BaudRate;
  m_Parity   = Parity;
  m_DataBits = DataBits;
  m_StopBits = StopBits;

  // Set default timeouts
  SetReadTimeout(Q_INFINITE_TIME);

  // Prepare the port configuration string
  char SettingsStr[100];
  sprintf(SettingsStr,"%d,%c,%d,%d",BaudRate,Parity,DataBits,StopBits);

  // BuildCommDCB does not affect all the DCB fields, so we need to initialize these fields with
  // correct values.
  ::GetCommState(m_Handle,&dcbCommPort);

  // Build the configuration structure
  if(!::BuildCommDCB(SettingsStr,&dcbCommPort))
    throw EQComPort("Can not setup com port " + ComStr,GetLastError());

  dcbCommPort.fDtrControl = DTR_CONTROL_DISABLE;
  dcbCommPort.fDsrSensitivity = FALSE;
  dcbCommPort.fOutxDsrFlow = FALSE;
  dcbCommPort.fOutxCtsFlow = FALSE;
  dcbCommPort.fTXContinueOnXoff = FALSE;
  dcbCommPort.fOutX = FALSE;
  dcbCommPort.fInX = FALSE;

  // Assign the new DCB to the com port
  if(::SetCommState(m_Handle, &dcbCommPort) == 0)
    throw EQComPort("Can not setup port " + ComStr,GetLastError());
}

// Close the port (deinit)
void CQStdComPort::DeInit(void)
{
  // If currently open, close the port
  if(m_Handle != INVALID_HANDLE_VALUE)
  {
    int TmpComNum = m_ComNum;
    m_ComNum = m_BaudRate = m_Parity  = m_DataBits = m_StopBits = 0;

    // Close the com port file and check if error
    if(!::CloseHandle(m_Handle))
    {
      m_Handle = INVALID_HANDLE_VALUE;
      throw EQComPort("Can not close com port COM" + QIntToStr(TmpComNum),GetLastError());
    }

    m_Handle = INVALID_HANDLE_VALUE;
  }
}

// Get the current com port settings, return true if the com port is open
bool CQStdComPort::GetComSettings(int& ComNum,int& BaudRate,char& Parity,int& DataBits,int& StopBits)
{
  ComNum   = m_ComNum;
  BaudRate = m_BaudRate;
  Parity   = m_Parity;
  DataBits = m_DataBits;
  StopBits = m_StopBits;

  return (m_Handle != INVALID_HANDLE_VALUE);
}

