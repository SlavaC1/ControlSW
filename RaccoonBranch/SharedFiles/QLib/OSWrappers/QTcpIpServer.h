/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (TCP/IP socket)                         *
 * Module Description: Cross platform TCP/IP server socket.         *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Nir Saadon.                                              *
 * Start date: 24/10/2001                                           *
 * Last upate: 24/10/2001                                           *
 ********************************************************************/

#ifndef _Q_TCP_IP_SERVER_H_
#define _Q_TCP_IP_SERVER_H_

#include "QObject.h"
#include "QOSWrapper.h"
#include "QEvent.h"
#include "QThread.h"

#ifdef OS_WINDOWS
  #include <winsock.h>
#elif defined(OS_VXWORKS)
  #include "sockLib.h"
  #include "inetLib.h"
#endif

typedef enum {tsOK, tsDisconnect, tsAbort} TQTcpIpServerStatus;

// Exception class for the TCP/IP server class
class EQTcpIpServer : public EQOSWrapper {
  public:
    EQTcpIpServer(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQOSWrapper(ErrMsg,ErrCode) {}
};

// Interface class for the TCP/IP client socket
class CQTcpIpServer : public CQObject {
  private:

    // the server port number
    int m_PortNumber;

    // the server socket  for listening to connection attempts
    // and the client socket that is returned by accept() for communication with one client
  #ifdef OS_WINDOWS
    SOCKET m_ServerSocket;
    SOCKET m_ClientSocket;
  #elif defined(OS_VXWORKS)
    int m_ServerSocket;
    int m_ClientSocket;
  #endif

    CQEvent m_ConnectionErrorEvent;

    // Connection status
    bool m_Connected;

    // struct that holds the loacl connection parameters
    struct sockaddr_in m_Local;

    // struct that holds the remote connection parameters
    struct sockaddr_in m_Remote;

    // set the current status of the connection
    void SetConnected(bool Status);

    // Do several operations required in a case of an error
    void HandleCommError(void);

  public:
    // Constructor
    CQTcpIpServer(int PortNum);

    // Destructor
    ~CQTcpIpServer(void);

    // Initialize connection with the server
    bool Connect(void);

    // Initialize connection with the server
    bool CQTcpIpServer::Reconnect(void);

    // Disconnect current connection
    void Disconnect(void);

    // Return true if currently connected
    bool IsConnected(void);

    // Write data to the TCP/IP socket
    TQTcpIpServerStatus Write(void *Data,unsigned DataLength);

    // Read data from the TCP/IP socket
    TQTcpIpServerStatus Read(void *Data,unsigned DataLength,TQWaitTime TimeoutInTicks = Q_INFINITE_TIME);
};

#endif
