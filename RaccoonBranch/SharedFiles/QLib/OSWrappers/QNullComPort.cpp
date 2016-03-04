/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (ComPort - empty implementation)        *
 * Module Description: Empty implementation of a serial com port    *
 *                     intended mainly for debug.                   *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 22/08/2001                                           *
 * Last upate: 02/09/2001                                           *
 ********************************************************************/

#include "QUtils.h"
#include "QMonitor.h"
#include "QNullComPort.h"


// Default constructor
CQNullComPort::CQNullComPort(void)
{
  m_Handle = false;
  m_ComNum = m_BaudRate = m_Parity  = m_DataBits = m_StopBits = 0;
  m_SendOutputToMonitor = true;
  m_ReadTimeoutInMs = Q_INFINITE_TIME;
}

// Initialization constructor
CQNullComPort::CQNullComPort(int ComNum,int BaudRate,char Parity,int DataBits,int StopBits)
{
  m_Handle = false;
  m_ComNum = m_BaudRate = m_Parity  = m_DataBits = m_StopBits = 0;
  Init(ComNum,BaudRate,Parity,DataBits,StopBits);
  m_SendOutputToMonitor = true;
  m_ReadTimeoutInMs = (ULONG)Q_INFINITE_TIME;
}

// Destructor
CQNullComPort::~CQNullComPort(void)
{
  m_TimeoutEvent.SetEvent();
  DeInit();
}

// Write data to the com port
ULONG CQNullComPort::Write(void *Data,unsigned DataLength)
{
  if(!m_Handle)
    throw EQComPort("Can not write to NULL com port COM" + QIntToStr(m_ComNum));

  // Output to monitor (if enabled)
  if(m_SendOutputToMonitor)
  {
    // Prepare a string for output on the monitor
    QString Str = QFormatStr("NullCom %d: ",m_ComNum);

    for(unsigned i = 0; i < DataLength; i++)
      Str += QFormatStr("%.2X ",static_cast<BYTE>(((PCHAR)Data)[i]));

    QMonitor.Print(Str.c_str());
  }

  return DataLength;
}

// Read data from the com port
ULONG CQNullComPort::Read(void */*Data*/,unsigned /*DataLength*/)
{
  if(!m_Handle)
    throw EQComPort("Can not read from NULL com port COM" + QIntToStr(m_ComNum));

  m_TimeoutEvent.WaitFor(m_ReadTimeoutInMs);

  // Alwyas zero
  return 0;
}

// Set the com port read timeout in ms
void CQNullComPort::SetReadTimeout(ULONG TimeoutInMs)
{
  if(!m_Handle)
    throw EQComPort("Can not set read timeouts in NULL com port COM" + QIntToStr(m_ComNum));

  m_ReadTimeoutInMs = TimeoutInMs;
}

// Flush input and output buffers
void CQNullComPort::Flush(void)
{
  if(!m_Handle)
    throw EQComPort("Can not flush NULL com port COM" + QIntToStr(m_ComNum));
}

// Init function for the common properties
void CQNullComPort::Init(int ComNum,int BaudRate,char Parity ,int DataBits,int StopBits)
{
  // "Initialize"
  m_Handle = true;

  // Remember the com port settings
  m_ComNum   = ComNum;
  m_BaudRate = BaudRate;
  m_Parity   = Parity;
  m_DataBits = DataBits;
  m_StopBits = StopBits;
}

// Close the port (deinit)
void CQNullComPort::DeInit(void)
{
  m_Handle = false;
}

// Get the current com port settings, return true if the com port is open
bool CQNullComPort::GetComSettings(int& ComNum,int& BaudRate,char& Parity,int& DataBits,int& StopBits)
{
  ComNum   = m_ComNum;
  BaudRate = m_BaudRate;
  Parity   = m_Parity;
  DataBits = m_DataBits;
  StopBits = m_StopBits;

  return m_Handle;
}

