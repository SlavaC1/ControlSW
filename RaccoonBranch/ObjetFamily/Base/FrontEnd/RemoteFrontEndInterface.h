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
 * Last upate: 04/02/2002                                           *
 ********************************************************************/

#ifndef _REMOTE_FRONT_END_INTERFACE_
#define _REMOTE_FRONT_END_INTERFACE_

#include "QObject.h"


class CRemoteFrontEndInterface : public CQObject {
  public:
    // Update a status in the front end (3 versions)
    void UpdateStatus(int ControlID,int Status);
    void UpdateStatus(int ControlID,float Status);
    void UpdateStatus(int ControlID,QString Status);

    // Enable/Disable a front-end UI control
    void EnableDisableControl(int ControlID,bool Enable);

    // Print a message on the monitor
    void MonitorPrint(QString Msg);

    // Show notification message
    void NotificationMessage(QString Msg);

    // Show warning message
    void WarningMessage(QString Msg);

    // Show error message
    void ErrorMessage(QString Msg);
};

#endif
