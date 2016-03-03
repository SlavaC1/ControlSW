/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Tester.                                                  *
 * Module Description: QLib RPC client implementation.              *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 24/09/2001                                           *
 * Last upate: 30/01/2002                                           *
 ********************************************************************/

#ifndef _RPC_CLIENT_H_
#define _RPC_CLIENT_H_

#include "EdenProtocolClient.h"
#include "EdenProtocolEngine.h"
#include "QMutex.h"


// Exception class for the CRPCClient class
class ERPCClient : public EQException {
  public:
    ERPCClient(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

const int RPC_REPLY_TIMEOUT_IN_MS = 5000;
const int NO_RPC_TIMEOUT = 0;

class CRPCClient : public CEdenProtocolClient {
  private:
    // Scratch buffer for temporary data
    BYTE m_WorkBuffer[MAX_SERIAL_PACKET_LENGTH];

    CQEvent m_SyncEvent;

    // Status code for the last transaction
    int m_Status;

    // Operation result string
    QString m_ResultStr;

    int Send(PVOID Data,unsigned DataLength);

    void CheckRPCStatus(int Status);

    static void PacketReceivedHandler(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);

  public:

    // Constructor
    CRPCClient(CEdenProtocolEngine *ProtocolEngine);

    // Destructor
    ~CRPCClient(void);

    // Invoke a remote method
    QString InvokeMethod(const QString& ComponentName,const QString& MethodName,
                         const QString *Args,unsigned ArgsNum,int Timeout = RPC_REPLY_TIMEOUT_IN_MS);

    // Set/Get a remote property value
    QString SetProperty(const QString& ComponentName,const QString& PropertyName,
                     const QString& PropertyValue,int Timeout = RPC_REPLY_TIMEOUT_IN_MS);

    QString GetProperty(const QString& ComponentName,const QString& PropertyName,
                        int Timeout = RPC_REPLY_TIMEOUT_IN_MS);
};

#endif

