/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Q2RT remote (RPC based) Front-End interface              *
 *         implementation.                                          *
 *                                                                  *
 * Compilation: BCB.                                                *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 30/01/2002                                           *
 * Last upate: 30/01/2002                                           *
 ********************************************************************/

#include "Q2RTApplication.h"
#include "RPCClient.h"
#include "RemoteFrontEndInterface.h"


const int FRONT_END_RPC_TIMEOUT = NO_RPC_TIMEOUT;
const int FRONT_END_RPC_LONG_TIMEOUT = 20000;


// Update a status in the front end (integer version)
void CRemoteFrontEndInterface::UpdateStatus(int ControlID,int Status)
{
  CRPCClient *RPCClient = Q2RTApplication->GetRPCClient();

  QString Args[]={QIntToStr(ControlID),QIntToStr(Status)};
  RPCClient->InvokeMethod("FrontEndProxi","UpdateStatusInt",Args,2,FRONT_END_RPC_TIMEOUT);
}

// Update a status in the front end (float version)
void CRemoteFrontEndInterface::UpdateStatus(int ControlID,float Status)
{
  CRPCClient *RPCClient = Q2RTApplication->GetRPCClient();

  QString Args[]={QIntToStr(ControlID),QFloatToStr(Status)};
  RPCClient->InvokeMethod("FrontEndProxi","UpdateStatusFloat",Args,2,FRONT_END_RPC_TIMEOUT);
}

// Update a status in the front end (string version)
void CRemoteFrontEndInterface::UpdateStatus(int ControlID,QString Status)
{
  CRPCClient *RPCClient = Q2RTApplication->GetRPCClient();

  QString Args[]={QIntToStr(ControlID),Status};
  RPCClient->InvokeMethod("FrontEndProxi","UpdateStatusStr",Args,2,FRONT_END_RPC_TIMEOUT);
}

// Enable/Disable a front-end UI control
void CRemoteFrontEndInterface::EnableDisableControl(int ControlID,bool Enable)
{
  CRPCClient *RPCClient = Q2RTApplication->GetRPCClient();

  QString Args[]={QIntToStr(ControlID),Enable ? "1" : "0"};
  RPCClient->InvokeMethod("FrontEndProxi","EnableDisableControl",Args,2,FRONT_END_RPC_TIMEOUT);
}

void CRemoteFrontEndInterface::MonitorPrint(QString Msg)
{
  CRPCClient *RPCClient = Q2RTApplication->GetRPCClient();

  RPCClient->InvokeMethod("FrontEndProxi","MonitorPrint",&Msg,1,FRONT_END_RPC_TIMEOUT);
}

// Show notification message
void CRemoteFrontEndInterface::NotificationMessage(QString Msg)
{
  CRPCClient *RPCClient = Q2RTApplication->GetRPCClient();

  RPCClient->InvokeMethod("FrontEndProxi","NotificationMessage",&Msg,1,FRONT_END_RPC_TIMEOUT);
}

// Show warning message
void CRemoteFrontEndInterface::WarningMessage(QString Msg)
{
  CRPCClient *RPCClient = Q2RTApplication->GetRPCClient();

  RPCClient->InvokeMethod("FrontEndProxi","WarningMessage",&Msg,1,FRONT_END_RPC_TIMEOUT);
}

// Show error message
void CRemoteFrontEndInterface::ErrorMessage(QString Msg)
{
  CRPCClient *RPCClient = Q2RTApplication->GetRPCClient();

  RPCClient->InvokeMethod("FrontEndProxi","ErrorMessage",&Msg,1,FRONT_END_RPC_TIMEOUT);
}

