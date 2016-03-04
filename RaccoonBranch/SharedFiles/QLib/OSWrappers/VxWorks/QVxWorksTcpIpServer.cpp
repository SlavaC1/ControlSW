/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (TCP/IP socket)                         *
 * Module Description: VxWorks implementation for TCP/IP server     *
 *                     socket.                                      *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Nir Saadon.                                              *
 * Start date: 24/10/2001                                           *
 * Last upate: 14/02/2002                                           *
 ********************************************************************/

#include <ioLib.h>
#include "QTcpIpServer.h"
#include "QThreadUtils.h"	//for debug
#include "AllQlib.h"       // For Degug  

// Constructor
CQTcpIpServer::CQTcpIpServer(int PortNum)
{
  SetConnected(false);

  // zero the local and remot structs
  bzero((char*) &m_Local, sizeof(struct sockaddr_in));
  bzero((char*) &m_Remote, sizeof(struct sockaddr_in));

  m_PortNumber = PortNum;
  m_Local.sin_family = AF_INET;
  m_Local.sin_addr.s_addr = htonl (INADDR_ANY);
  m_Local.sin_port = htons(m_PortNumber);

}

// Destructor
CQTcpIpServer::~CQTcpIpServer(void)
{
  Disconnect();
}

// Initialize connection with the server
bool CQTcpIpServer::Connect(void)
{
  if (IsConnected())
    throw EQTcpIpServer("Tcp/Ip already connected");

  static bool flag = true;

// create the socket
  if((m_ServerSocket = socket (AF_INET, SOCK_STREAM, 0)) == ERROR)
    return false;

// bind the socket to the port number
  if (bind (m_ServerSocket,(struct sockaddr *) &m_Local,sizeof (m_Local)) == ERROR)
  {
    close(m_ServerSocket);
    QMonitor.Printf("Bind - error Server Socket No. %d", m_ServerSocket );

    return false;
  }

// listen to client connection attempts
  if (listen (m_ServerSocket, 1) == ERROR)
  {
    close (m_ServerSocket);
    return false;
  }

// accept a client connection
  int length = sizeof (m_Remote);
  m_ClientSocket = accept (m_ServerSocket,(struct sockaddr *) &m_Remote,(int *) &length);

  if (m_ClientSocket == ERROR)
  {
    close (m_ServerSocket);
    return false;
  }

// set the connection status
  SetConnected(true);

  return true;
}

// Disconnect current connection
void CQTcpIpServer::Disconnect(void)
{
  shutdown(m_ClientSocket,2);
  shutdown(m_ServerSocket,2);
  SetConnected(false);
  close (m_ServerSocket);
  close (m_ClientSocket);
}

// Return true if currently connected
bool CQTcpIpServer::IsConnected(void)
{
  return m_Connected;
}

// Write data to the TCP/IP socket
TQTcpIpServerStatus CQTcpIpServer::Write(void *Data,unsigned DataLength)
{
  int RetVal;

  if (!IsConnected())
    return tsDisconnect;

  RetVal = send (m_ClientSocket, (char*)Data, DataLength, 0);
  if( RetVal == ERROR)
  {
    HandleCommError();
    throw EQTcpIpServer("Send error",errno);
  }

  return tsOK;
}

// Read data from the TCP/IP socket
TQTcpIpServerStatus CQTcpIpServer::Read(void *Data,unsigned DataLength,TQWaitTime TimeoutInTicks)
{
  if(!IsConnected())
    return tsDisconnect;

  // Set read timeout

  int DataCounter = 0;

  while((unsigned)DataCounter < DataLength)
  {
    int RetVal = recv(m_ClientSocket,(char *)Data + DataCounter,DataLength - DataCounter,0);
    
    // Gracefully disconnection by the client
    if(RetVal == 0)
    {
      HandleCommError();

      return tsDisconnect;
    }

    // If error
    if(RetVal == ERROR)
    {

      switch(errno)
      {
        case ECONNRESET:
          HandleCommError();
          return tsDisconnect;

        case ECONNABORTED:
        case EINTR:
          HandleCommError();
          return tsAbort;
      }

      throw EQTcpIpServer("Receive error",errno);
    }

    DataCounter += RetVal;
  }

  return tsOK;
}

// set the current status of the connection
void CQTcpIpServer::SetConnected(bool Status)
{
  m_Connected = Status;
}

// Do several operations required in a case of an error
void CQTcpIpServer::HandleCommError(void)
{
 SetConnected(false);
 close(m_ServerSocket);
 close(m_ClientSocket);
}

