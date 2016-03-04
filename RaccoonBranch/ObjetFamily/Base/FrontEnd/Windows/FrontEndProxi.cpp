/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Q2RT Front-End proxi class for remote interface with a   *
 *         UI frame.                                                *
 *                                                                  *
 * Compilation: BCB.                                                *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 05/02/2002                                           *
 * Last upate: 05/02/2002                                           *
 ********************************************************************/

#include "FrontEndProxi.h"
#include "EdenMainUIFrame.h"
#include "QMonitor.h"
#include "GeneralDevicesDlg.h"
#include "HeadsControlDlg.h"
#include "MotorsControlDlg.h"
#include "ActuatorsControlDlg.h"
#include "OcbOhdbControlDlg.h"

// Constructor
CFrontEndProxi::CFrontEndProxi(TMainUIFrame *UIFrame) : CQComponent("FrontEndProxi")
{
  m_UIFrame = UIFrame;

  INIT_METHOD(CFrontEndProxi,UpdateStatusInt);
  INIT_METHOD(CFrontEndProxi,UpdateStatusFloat);
  INIT_METHOD(CFrontEndProxi,UpdateStatusStr);
  INIT_METHOD(CFrontEndProxi,EnableDisableControl);
  INIT_METHOD(CFrontEndProxi,MonitorPrint);
  INIT_METHOD(CFrontEndProxi,NotificationMessage);
  INIT_METHOD(CFrontEndProxi,WarningMessage);
  INIT_METHOD(CFrontEndProxi,ErrorMessage);
}

int CFrontEndProxi::UpdateStatusInt(int ControlID,int Status)
{
  m_UIFrame->UpdateStatus(ControlID,Status);

  if(GeneralDevicesForm->Visible)
    GeneralDevicesForm->GetUIFrame()->UpdateStatus(ControlID,Status);

  if(HeadsControlForm->Visible)
    HeadsControlForm->GetUIFrame()->UpdateStatus(ControlID,Status);

  if(MotorsControlForm->Visible)
    MotorsControlForm->UpdateStatus(ControlID,Status);

  if(ActuatorsControlForm->Visible)
    ActuatorsControlForm->UpdateStatus(ControlID,Status);
	
  if(OcbOhdbControlForm>Visible)
    OcbOhdbControlForm->UpdateStatus(ControlID,Status);	

  return 0;
}

int CFrontEndProxi::UpdateStatusFloat(int ControlID,float Status)
{
  m_UIFrame->UpdateStatus(ControlID,Status);

  if(GeneralDevicesForm->Visible)
    GeneralDevicesForm->GetUIFrame()->UpdateStatus(ControlID,Status);

  if(HeadsControlForm->Visible)
    HeadsControlForm->GetUIFrame()->UpdateStatus(ControlID,Status);

  if(MotorsControlForm->Visible)
    MotorsControlForm->UpdateStatus(ControlID,Status);

  if(ActuatorsControlForm->Visible)
    ActuatorsControlForm->UpdateStatus(ControlID,Status);

	
  if(OcbOhdbControlForm>Visible)
    OcbOhdbControlForm->UpdateStatus(ControlID,Status);		

  return 0;
}

int CFrontEndProxi::UpdateStatusStr(int ControlID,QString Status)
{
  m_UIFrame->UpdateStatus(ControlID,Status);

  if(GeneralDevicesForm->Visible)
    GeneralDevicesForm->GetUIFrame()->UpdateStatus(ControlID,Status);

  if(HeadsControlForm->Visible)
    HeadsControlForm->GetUIFrame()->UpdateStatus(ControlID,Status);

  if(MotorsControlForm->Visible)
    MotorsControlForm->UpdateStatus(ControlID,Status);

  if(ActuatorsControlForm->Visible)
    ActuatorsControlForm->UpdateStatus(ControlID,Status);
	
  if(OcbOhdbControlForm>Visible)
    OcbOhdbControlForm->UpdateStatus(ControlID,Status);		

  return 0;
}

int CFrontEndProxi::EnableDisableControl(int ControlID,bool Enable)
{
  m_UIFrame->EnableDisableControl(ControlID,Enable);
  return 0;
}

int CFrontEndProxi::MonitorPrint(QString Msg)
{
  QMonitor.Print(Msg.c_str());
  return 0;
}

int CFrontEndProxi::NotificationMessage(QString Msg)
{
  QMonitor.NotificationMessage(Msg);
  return 0;
}

int CFrontEndProxi::WarningMessage(QString Msg)
{
  QMonitor.WarningMessage(Msg);
  return 0;
}

int CFrontEndProxi::ErrorMessage(QString Msg)
{
  QMonitor.ErrorMessage(Msg);
  return 0;
}

