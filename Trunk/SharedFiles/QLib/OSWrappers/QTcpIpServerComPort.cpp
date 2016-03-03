/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (TCP/IP ComPort)                        *
 * Module Description: This class emulate serial communication using*
 *                     TCP/IP server.                               *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 27/02/2002                                           *
 * Last upate: 11/03/2002                                           *
 ********************************************************************/

#include "QTcpIpServerComPort.h"
#include "QTcpIpServer.h"


const int SERIAL_TCPIP_PORT_NUMBER = 12;

// Initialization constructor
CQTcpIpServerComPort::CQTcpIpServerComPort(/*int ComNum,int BaudRate,char Parity,int DataBits,int StopBits*/)
{
  // Create the TCP/IP server component
  m_TcpIpServer = new CQTcpIpServer(SERIAL_TCPIP_PORT_NUMBER);

  // Set default read timeout
  m_ReadTimeoutInTicks = Q_INFINITE_TIME;

  TerminateFlag = false;
}

// Destructor
CQTcpIpServerComPort::~CQTcpIpServerComPort(void)
{
  // Set termination flag
  TerminateFlag = true;

  // Close socket
  m_TcpIpServer->Disconnect();

  delete m_TcpIpServer;
}

// Implementation functions
// ------------------------

// Write data to the com port
ULONG CQTcpIpServerComPort::Write(void *Data,unsigned DataLength)
{
  if(m_TcpIpServer->Write(Data,DataLength) != tsOK)
    return 0L;

  return DataLength;
}

// Read data from the com port
ULONG CQTcpIpServerComPort::Read(void *Data,unsigned DataLength)
{
  // If already terminated, do nothing
  if(TerminateFlag)
    return 0L;

  for(;;)
  {
    // If currently not connected wait for connect
    while(!m_TcpIpServer->IsConnected())
    {
      if(!m_TcpIpServer->Connect())
      {
        // Check for connection failed
        if(TerminateFlag)
          return 0L;
      }
    }

    // Read data
    if(m_TcpIpServer->Read(Data,DataLength/*,m_ReadTimeoutInTicks*/) != tsOK)
    {
      if(TerminateFlag)
        return 0L;
        
    } else
        break;
  }

  return DataLength;
}

// Set the com port read timeout in ms
void CQTcpIpServerComPort::SetReadTimeout(ULONG TimeoutInMs)
{
  m_ReadTimeoutInTicks = QMsToTicks(TimeoutInMs);
}

