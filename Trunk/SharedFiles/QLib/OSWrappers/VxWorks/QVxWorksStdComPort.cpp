/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (ComPort - standrad)                    *
 * Module Description: VxWorks implementation for the standard      *
 *                     serial com port class.                       *
 *                                                                  *
 * Compilation: Standard C++, VxWorks                               *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 05/08/2001                                           *
 * Last upate: 22/08/2001                                           *
 ********************************************************************/

#include <stdio.h>
#include <ioLib.h>
#include "QUtils.h"
#include "QStdComPort.h"

#include "QThreadUtils.h"

// Default constructor
CQStdComPort::CQStdComPort(void)
{
  m_Handle = ERROR;
  m_ComNum = m_BaudRate = m_Parity  = m_DataBits = m_StopBits = 0;
}

// Initialization constructor
CQStdComPort::CQStdComPort(int ComNum,int BaudRate,char Parity,int DataBits,int StopBits)
{
  m_Handle = ERROR;
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
  int ActualBytesWrittenCount;

  // Write to the com port file  
  ActualBytesWrittenCount = write(m_Handle,(PCHAR)Data,DataLength);

  if(ActualBytesWrittenCount == ERROR)
    throw EQComPort("Can not write com port COM" + QIntToStr(m_ComNum),errno);

  // Return the actual number of bytes read
  return (ULONG)ActualBytesWrittenCount;
}

// Read data from the com port
ULONG CQStdComPort::Read(void *Data,unsigned DataLength)
{
  int ActualBytesReadCount = 0;

  // If the read timeout is different then infinite time, wind the timeout callback timer
  if(m_ReadTimeout != Q_INFINITE_TIME)
    m_ReadTimeoutTimer.TriggerTimer(m_ReadTimeout,ReadTimeoutCallback,reinterpret_cast<TGenericCockie>(this));

  // Read from the com port file
  while(ActualBytesReadCount < DataLength) 
    ActualBytesReadCount += read(m_Handle,((PCHAR)Data + ActualBytesReadCount),DataLength - ActualBytesReadCount);
  
  // Check if error
  if(ActualBytesReadCount == ERROR)
    throw EQComPort("Can not read com port COM" + QIntToStr(m_ComNum),errno);

  // Return the actual number of bytes read
  return (ULONG)ActualBytesReadCount;
}

// Set the com port read timeout in ms
void CQStdComPort::SetReadTimeout(ULONG TimeoutInMs)
{
  m_ReadTimeout = TimeoutInMs;
}

// Flush input and output buffers
void CQStdComPort::Flush(void)
{
  if((ioctl(m_Handle,FIOFLUSH,0) == ERROR))
    throw EQComPort("Can not flush com port COM" + QIntToStr(m_ComNum),errno);
}

// Init function for the common properties
void CQStdComPort::Init(int ComNum,int BaudRate,char Parity ,int DataBits,int StopBits)
{
  // Create the com port file name
  QString ComStr = "/tyCo/" + QIntToStr(ComNum - 1);

  // Open the com port file
  m_Handle = open(ComStr.c_str(),O_RDWR,0);

  // Check if the port opened Ok
  if(m_Handle == ERROR)
    throw EQComPort("Can not open com port COM" + QIntToStr(ComNum),errno);

  // Configure the port settings
  if((ioctl(m_Handle,FIOBAUDRATE,BaudRate) == ERROR) ||
     (ioctl(m_Handle,FIOSETOPTIONS,OPT_RAW) == ERROR))
    throw EQComPort("Can not setup com port COM" + QIntToStr(ComNum),errno);

  // Remember the com port settings
  m_ComNum   = ComNum;
  m_BaudRate = BaudRate;
  m_Parity   = Parity;
  m_DataBits = DataBits;
  m_StopBits = StopBits;

  // Set default timeouts
  SetReadTimeout(Q_INFINITE_TIME);
}

// Close the port (deinit)
void CQStdComPort::DeInit(void)
{
  // If currently open, close the port
  if(m_Handle != ERROR)
  {
    int TmpComNum = m_ComNum;
    m_ComNum = m_BaudRate = m_Parity  = m_DataBits = m_StopBits = 0;

    // Close the com port file and check if error
    if(close(m_Handle) == ERROR)
    {
      m_Handle = ERROR;
      throw EQComPort("Can not close com port COM" + QIntToStr(TmpComNum),errno);
    }

    m_Handle = ERROR;
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

  return (m_Handle != ERROR);
}

// Cancel read operation
void CQStdComPort::CancelRead(void)
{
  ioctl(m_Handle,FIOCANCEL,0);
}

void CQStdComPort::ReadTimeoutCallback(TGenericCockie Cockie)
{
  // Get a pointer to the calling instance
  CQStdComPort *InstancePtr = reinterpret_cast<CQStdComPort *>(Cockie);

  // Cancel the current read operation
  InstancePtr->CancelRead();
}

