/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (TCP/IP socket)                         *
 * Module Description: VxWorks implementation for TCP/IP client     *
 *                     socket.                                      *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Nir Saadon.                                              *
 * Start date: 24/10/2001                                           *
 * Last upate: 24/10/2001                                           *
 ********************************************************************/

#include "QTcpIpClient.h"


// Constructor
CQTcpIpClient::CQTcpIpClient(const QString& ServerAddress,int PortNum)
{
  SetConnected(false);
  m_PortNumber = PortNum;
  m_ServerAddress = ServerAddress;
  m_Destination.sin_family = AF_INET;
  m_Destination.sin_addr.s_addr = inet_addr(m_ServerAddress.c_str());
  m_Destination.sin_port = htons (m_PortNumber);
}

// Destructor
CQTcpIpClient::~CQTcpIpClient(void)
{
  Disconnect();
}

// Initialize connection with the server
bool CQTcpIpClient::Connect(void)
{
  if (IsConnected())
    throw EQTcpIpClient("Tcp/Ip already connected");

  // windows sockeet initailization
  WSADATA WSAData;

  if(WSAStartup (MAKEWORD(1,1), &WSAData) != 0)
    return false;

// create the socket
  if((m_ServerSocket = socket (AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    return false;

// try to connect to the server
  if(connect(m_ServerSocket,(struct sockaddr *) &m_Destination,sizeof (m_Destination)) == SOCKET_ERROR)
  {
    closesocket (m_ServerSocket);
    return false;
  }

// set the connection status
  SetConnected(true);

  return true;
}

// Disconnect current connection
void CQTcpIpClient::Disconnect(void)
{
  SetConnected(false);
  closesocket (m_ServerSocket);
}

// Return true if currently connected
bool CQTcpIpClient::IsConnected(void)
{
  return m_Connected;
}

// Write data to the TCP/IP socket
TQTcpIpClientStatus CQTcpIpClient::Write(void *Data,unsigned DataLength)
{
  int RetVal;

  if(!IsConnected())
    throw EQTcpIpClient("Tcp/Ip not connected");

  RetVal = send(m_ServerSocket, (char*)Data, DataLength, 0);

  if(RetVal == SOCKET_ERROR)
  {
    SetConnected(false);

    if (WSAGetLastError() == WSAECONNRESET) // if the connection was disconnected
     return tcDisconnect;

    throw EQTcpIpClient("Send error");
  }

  return tcOK;
}

// Read data from the TCP/IP socket
TQTcpIpClientStatus CQTcpIpClient::Read(void *Data,unsigned DataLength,TQWaitTime TimeoutInTicks)
{
  if (!IsConnected())
    throw EQTcpIpClient("Tcp/Ip not connected");

  // Set read timeout
  

  int DataCounter = 0;

  while((unsigned)DataCounter < DataLength)
  {
    int RetVal = recv (m_ServerSocket,(char *)Data + DataCounter,DataLength - DataCounter,0);

    // Gracefully disconnection
    if(RetVal == 0)
    {
      SetConnected(false);    
      return tcDisconnect;
    }

    // If error
    if(RetVal == SOCKET_ERROR)
    {
      SetConnected(false);

      int err = WSAGetLastError();

      switch(err)
      {
        case WSAECONNRESET:
          return tcDisconnect;

        case WSAECONNABORTED:
          return tcAbort;
      }

      throw EQTcpIpClient("Receive error",err);
    }

    DataCounter += RetVal;
  }

  return tcOK;
}

// set the current status of the connection
void CQTcpIpClient::SetConnected(bool Status)
{
  m_Connected = Status;
}

