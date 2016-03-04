/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (TCP/IP socket)                         *
 * Module Description: Windows implementation for TCP/IP server     *
 *                     socket.                                      *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Nir Saadon.                                              *
 * Start date: 24/10/2001                                           *
 * Last upate: 12/05/2003                                           *
 ********************************************************************/

#include "QTcpIpServer.h"
#include "QLogFile.h"

const unsigned MAX_BUFFER_SIZE = 8192; //8k

// Constructor
CQTcpIpServer::CQTcpIpServer(int PortNum)
{
  SetConnected(false);
  m_PortNumber = PortNum;
  m_Local.sin_family = AF_INET;
  m_Local.sin_addr.s_addr = htonl (INADDR_ANY);
  m_Local.sin_port = htons(m_PortNumber);
  m_ServerSocket = INVALID_SOCKET;
}

// Destructor
CQTcpIpServer::~CQTcpIpServer(void)
{
  Disconnect();
}

// Initialize connection with the server
bool CQTcpIpServer::Connect(void)
{
  WSADATA WSAData;

  if(IsConnected())
    throw EQTcpIpServer("Tcp/Ip already connected");

  if(WSAStartup (MAKEWORD(1,1), &WSAData) != 0)
    return false;

  // create the socket
  m_ServerSocket = socket (AF_INET, SOCK_STREAM, 0);

  if(m_ServerSocket == INVALID_SOCKET)
    return false;

  // bind the socket to the port number
  if (bind (m_ServerSocket,(struct sockaddr *) &m_Local,sizeof (m_Local)) == SOCKET_ERROR)
  {
    closesocket (m_ServerSocket);
    return false;
  }

  // listen to client connection attempts
  if (listen (m_ServerSocket,1) == SOCKET_ERROR)
  {
    closesocket(m_ServerSocket);
    m_ServerSocket = INVALID_SOCKET;
    return false;
  }

  // accept a client connection
  int length = sizeof (m_Remote);
  m_ClientSocket = accept (m_ServerSocket,(struct sockaddr *) &m_Remote,(int *) &length);

  closesocket(m_ServerSocket);
  m_ServerSocket = INVALID_SOCKET;

  if(m_ClientSocket == SOCKET_ERROR)
    return false;

// set the connection status
  SetConnected(true);

  return true;
}

// Disconnect current connection
void CQTcpIpServer::Disconnect(void)
{
  shutdown(m_ClientSocket,/*SD_BOTH*/0x02);

  SetConnected(false);
  closesocket (m_ClientSocket);

  if(m_ServerSocket != INVALID_SOCKET)
  {
    closesocket (m_ServerSocket);
    m_ServerSocket = INVALID_SOCKET;
  }
}

// Return true if currently connected
bool CQTcpIpServer::IsConnected(void)
{
  return m_Connected;
}

// Write data to the TCP/IP socket
TQTcpIpServerStatus CQTcpIpServer::Write(void *Data,unsigned DataLength)
{
  if (!IsConnected())
    return tsDisconnect;

  int nLeft, nWritten;
  PBYTE pBuf = (PBYTE)Data;
  nLeft = DataLength;

  CQLog::Write(2,"CQTcpIpServer::Write() cmd = 0x%X, len = %d", static_cast<unsigned char*>(Data)[1], DataLength);

  while (nLeft > 0)
  {
    nWritten = send (m_ClientSocket, (char*)pBuf, nLeft, 0);

    if (nWritten == SOCKET_ERROR)
    {
      int res = WSAGetLastError(); //bug 6029
      HandleCommError();
      throw EQTcpIpServer("Send error",res);
    }

    nLeft -= nWritten;
    pBuf += nWritten;
  }

  CQLog::Write(2,"CQTcpIpServer::Write() nWritten = %d", nWritten);
  return tsOK;
}

// Read data from the TCP/IP socket
TQTcpIpServerStatus CQTcpIpServer::Read(void *Data,unsigned DataLength,TQWaitTime TimeoutInTicks)
{
  if(!IsConnected())
    return tsDisconnect;

  // Set read timeout

  int DataCounter = 0;
  unsigned BufferLength = MAX_BUFFER_SIZE; //6778
  if(DataLength<MAX_BUFFER_SIZE)
    BufferLength = DataLength;

  while((unsigned)DataCounter < DataLength)
  {
    if((DataLength - DataCounter)<BufferLength) //don't read more than requested DataLength
      BufferLength = DataLength - DataCounter;
    int RetVal = recv(m_ClientSocket,(char *)Data + DataCounter,BufferLength,0);//6790

    // Gracefully disconnection by the client
    if(RetVal == 0)
    {
      HandleCommError();
      return tsDisconnect;
    }

    // If error
    if(RetVal == SOCKET_ERROR)
    {

      int err = WSAGetLastError();

      switch(err)
      {
        case WSAECONNRESET:
          HandleCommError();
          return tsDisconnect;

        case WSAECONNABORTED:
        case WSAEINTR:
          HandleCommError();
          return tsAbort;
      }

      throw EQTcpIpServer("Receive error",err);
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
  closesocket(m_ClientSocket);
}
