/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (TCP/IP ComPort)                        *
 * Module Description: This class emulate serial communication using*
 *                     TCP/IP client.                               *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 27/02/2002                                           *
 * Last upate: 27/02/2002                                           *
 ********************************************************************/

#include "QTcpIpClientComPort.h"
#include "QTcpIpClient.h"
#include "QThreadUtils.h"


const int SERIAL_TCPIP_PORT_NUMBER = 12;
const int READ_RELEASE_TIMEOUT = 2000;
const int CONNECT_RETRY_DELAY = 3000;


// Initialization constructor
CQTcpIpClientComPort::CQTcpIpClientComPort(const QString& ServerAddress)
{
  // Create the TCP/IP client component
  m_TcpIpClient = new CQTcpIpClient(ServerAddress,SERIAL_TCPIP_PORT_NUMBER);

  // Set default read timeout
  m_ReadTimeoutInTicks = Q_INFINITE_TIME;

  TerminateFlag = false;
}

// Destructor
CQTcpIpClientComPort::~CQTcpIpClientComPort(void)
{
  // Set termination flag
  TerminateFlag = true;

  // Close socket
  m_TcpIpClient->Disconnect();

  // Wait for read release
  m_ReadReleaseEvent.WaitFor(QMsToTicks(READ_RELEASE_TIMEOUT));

  delete m_TcpIpClient;
}

// Implementation functions
// ------------------------

// Write data to the com port
ULONG CQTcpIpClientComPort::Write(void *Data,unsigned DataLength)
{
  // If currently not connected, establish connection
  if(!m_TcpIpClient->IsConnected())
    if(!m_TcpIpClient->Connect())
      throw EQComPort("TCP/IP com port can not connect to server");

  if(m_TcpIpClient->Write(Data,DataLength) != tcOK)
    return 0L;

  return DataLength;
}

// Read data from the com port
ULONG CQTcpIpClientComPort::Read(void *Data,unsigned DataLength)
{
  while(!m_TcpIpClient->IsConnected())
  {
    if(TerminateFlag)
    {
      m_ReadReleaseEvent.SetEvent();
      return 0L;
    }

    if(!m_TcpIpClient->Connect())
    {
      //throw EQComPort("TCP/IP com port can not connect to server");
      QSleep(CONNECT_RETRY_DELAY);
    }
  }

  // Read data
  if(m_TcpIpClient->Read(Data,DataLength,m_ReadTimeoutInTicks) != tcOK)
  {
    if(TerminateFlag)
    {
      m_ReadReleaseEvent.SetEvent();
      return 0L;
    } else
        throw EQComPort("TCP/IP com port read error");
  }

  return DataLength;
}

// Set the com port read timeout in ms
void CQTcpIpClientComPort::SetReadTimeout(ULONG TimeoutInMs)
{
  m_ReadTimeoutInTicks = QMsToTicks(TimeoutInMs);
}

