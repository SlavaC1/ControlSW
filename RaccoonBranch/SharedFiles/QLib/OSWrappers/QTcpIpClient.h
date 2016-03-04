/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (TCP/IP socket)                         *
 * Module Description: Cross platform TCP/IP client socket.         *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Nir Saadon.                                              *
 * Start date: 24/10/2001                                           *
 * Last upate: 24/10/2001                                           *
 ********************************************************************/

#ifndef _Q_TCP_IP_CLIENT_H_
#define _Q_TCP_IP_CLIENT_H_

#include "QObject.h"
#include "QOSWrapper.h"

#ifdef OS_WINDOWS
  #include "winsock.h"
#elif defined(OS_VXWORKS)
  #include "sockLib.h"
  #include "inetLib.h"
 
#endif

typedef enum {tcOK, tcDisconnect,tcAbort} TQTcpIpClientStatus;

// Exception class for the TCP/IP client class
class EQTcpIpClient : public EQOSWrapper {
  public:
    EQTcpIpClient(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQOSWrapper(ErrMsg,ErrCode) {}
};

// Interface class for the TCP/IP client socket
class CQTcpIpClient : public CQObject {
  private:

  // the server port number
  int m_PortNumber;

  // the server addres
	QString m_ServerAddress;

  // the socket
  #ifdef OS_WINDOWS
  SOCKET m_ServerSocket;
  #elif defined(OS_VXWORKS)
  int m_ServerSocket;
  #endif


  // Connection status
  bool m_Connected;

  // struct that holds the connection parameters
  struct sockaddr_in m_Destination;

   // set the current status of the connection
   void SetConnected(bool Status);

  public:
    // Constructor
    CQTcpIpClient(const QString& ServerAddress,int PortNum);

    // Destructor
    ~CQTcpIpClient(void);

    // Initialize connection with the server
    bool Connect(void);

    // Disconnect current connection
    void Disconnect(void);

    // Return true if currently connected
    bool IsConnected(void);

    // Write data to the TCP/IP socket
    TQTcpIpClientStatus Write(void *Data,unsigned DataLength);

    // Read data from the TCP/IP socket
    TQTcpIpClientStatus Read(void *Data,unsigned DataLength,TQWaitTime TimeoutInTicks = Q_INFINITE_TIME);


};

#endif
