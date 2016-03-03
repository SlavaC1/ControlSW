/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Q2RT Front-End interface.                                *
 *                                                                  *
 * Compilation: BCB.                                                *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 28/01/2002                                           *
 * Last upate: 04/08/2003                                           *
 ********************************************************************/

#include "Q2RTApplication.h"
#include "FrontEnd.h"
#include "EdenMainUIFrame.h"
#include "RemoteFrontEndInterface.h"
#include "QMonitor.h"
#include "QErrors.h"
#include "EdenGeneralDeviceFrame.h"
#include "GeneralDevicesDlg.h"
#include "HeadsControlDlg.h"
#include "MotorsControlDlg.h"
#include "ActuatorsControlDlg.h"
#include "CartridgeErrorDlg.h"
#include "TankIdentificationNotice.h"
#include "MaterialWarningForm.h"
#include "OcbOhdbControlDlg.h"
#include "AutoWizardDlg.h"
#include "MachineManager.h"  //track Wizards' status
#include "FansSpeedDlg.h"

// Constructor
CFrontEndInterface::CFrontEndInterface(TMainUIFrame *UIFrame) : CQComponent("FrontEndInterface"),
	m_tidComm(0)
{
  INIT_VAR_PROPERTY(RemoteModeEnabled,false);
  INIT_METHOD(CFrontEndInterface,RefreshStatus);
  INIT_METHOD(CFrontEndInterface,YieldUIThread);
  INIT_METHOD(CFrontEndInterface,HideTankIDNoticeDlg);

  m_RemoteFrontEndInterface = new CRemoteFrontEndInterface;
  m_UIFrame = UIFrame;

  // Remember the GUI main thread ID
  m_GUIMainThread = GetCurrentThreadId();

  m_DisplayMaterialWarningEnable = true;
}

// Destructor
CFrontEndInterface::~CFrontEndInterface(void)
{
  delete m_RemoteFrontEndInterface;
}

// Update a status in the front end (integer version)
void CFrontEndInterface::DoUpdateStatus(int ControlID,int Status)
{
  UpdateShadow(ControlID,Status);
  m_UIFrame->UpdateStatus(ControlID,Status);

  if(GeneralDevicesForm && GeneralDevicesForm->Visible)
    GeneralDevicesForm->GetUIFrame()->UpdateStatus(ControlID,Status);

  if(HeadsControlForm && HeadsControlForm->Visible)
	HeadsControlForm->GetUIFrame()->UpdateStatus(ControlID,Status);

  if(MotorsControlForm && MotorsControlForm->Visible)
    MotorsControlForm->UpdateStatus(ControlID,Status);

  if(ActuatorsControlForm && ActuatorsControlForm->Visible)
    ActuatorsControlForm->UpdateStatus(ControlID,Status);
  
  if(OcbOhdbControlForm && OcbOhdbControlForm->Visible)
	OcbOhdbControlForm->UpdateStatus(ControlID,Status);

  if(FansSpeedDlg && FansSpeedDlg->Visible)
	FansSpeedDlg->UpdateStatus(ControlID,Status);

  if(RemoteModeEnabled)
  try
  {
	m_RemoteFrontEndInterface->UpdateStatus(ControlID,Status);
  } catch(...)
	{
	  RemoteModeEnabled = false;
	}
}

// Update a status in the front end (float version)
void CFrontEndInterface::DoUpdateStatus(int ControlID,float Status)
{
  UpdateShadow(ControlID,Status);
  m_UIFrame->UpdateStatus(ControlID,Status);

  if(GeneralDevicesForm && GeneralDevicesForm->Visible)
	GeneralDevicesForm->GetUIFrame()->UpdateStatus(ControlID,Status);

  if(HeadsControlForm && HeadsControlForm->Visible)
	HeadsControlForm->GetUIFrame()->UpdateStatus(ControlID,Status);

  if(MotorsControlForm && MotorsControlForm->Visible)
	MotorsControlForm->UpdateStatus(ControlID,Status);

  if(ActuatorsControlForm && ActuatorsControlForm->Visible)
	ActuatorsControlForm->UpdateStatus(ControlID,Status);

  if(OcbOhdbControlForm && OcbOhdbControlForm->Visible)
	OcbOhdbControlForm->UpdateStatus(ControlID,Status);

  if(FansSpeedDlg && FansSpeedDlg->Visible)
	FansSpeedDlg->UpdateStatus(ControlID,Status);

  if(RemoteModeEnabled)
  try
  {
	m_RemoteFrontEndInterface->UpdateStatus(ControlID,Status);
  } catch(...)
	{
	  RemoteModeEnabled = false;
	}
}

// Update a status in the front end (string version)
void CFrontEndInterface::DoUpdateStatus(int ControlID,QString Status)
{
  UpdateShadow(ControlID,Status);
  m_UIFrame->UpdateStatus(ControlID,Status);

  if(GeneralDevicesForm && GeneralDevicesForm->Visible)
	GeneralDevicesForm->GetUIFrame()->UpdateStatus(ControlID,Status);

  if(HeadsControlForm && HeadsControlForm->Visible)
	HeadsControlForm->GetUIFrame()->UpdateStatus(ControlID,Status);

  if(MotorsControlForm && MotorsControlForm->Visible)
	MotorsControlForm->UpdateStatus(ControlID,Status);

  if(ActuatorsControlForm && ActuatorsControlForm->Visible)
	ActuatorsControlForm->UpdateStatus(ControlID,Status);

  if(OcbOhdbControlForm && OcbOhdbControlForm->Visible)
	OcbOhdbControlForm->UpdateStatus(ControlID,Status);

  if(WizardForm && WizardForm->Visible)
	WizardForm->UpdateStatus(ControlID,Status);

  if(FansSpeedDlg && FansSpeedDlg->Visible)
	FansSpeedDlg->UpdateStatus(ControlID,Status);

  if(RemoteModeEnabled)
  try
  {
    m_RemoteFrontEndInterface->UpdateStatus(ControlID,Status);
  } catch(...)
    {
      RemoteModeEnabled = false;
    }
}

// Enable/Disable a front-end UI control
void CFrontEndInterface::DoEnableDisableControl(int ControlID,bool Enable)
{
  UpdateShadow(ControlID,Enable);
  m_UIFrame->EnableDisableControl(ControlID,Enable);

  if(RemoteModeEnabled)
  try
  {
    m_RemoteFrontEndInterface->EnableDisableControl(ControlID,Enable);
  } catch(...)
    {
      RemoteModeEnabled = false;
    }
}
// Update a status in the front end (integer version)
void CFrontEndInterface::RemoveStatus(int ControlID)
{
    // Check if value is different then current value
	TIntShadow::iterator i = m_IntShadow.find(ControlID);
	if(i != m_IntShadow.end())
		m_IntShadow.erase(i);
}

void CFrontEndInterface::UpdateStatus(int ControlID)
{
  m_UIFrame->UpdateStatus(ControlID,0);
}

// Update a status in the front end (integer version)
void CFrontEndInterface::UpdateStatus(int ControlID,int Status,bool ForceRefresh)
{
  bool DoUpdate = true;

  if(!ForceRefresh)
  {
    // Check if value is different then current value
	TIntShadow::iterator i = m_IntShadow.find(ControlID);

    // If the item can not be found
    if(i == m_IntShadow.end())
      DoUpdate = true;
    else
      DoUpdate = (*i).second != Status;
  }

  if(DoUpdate)
  {
    UpdateShadow(ControlID,Status);
    DoUpdateStatus(ControlID,Status);
  }
}

// Update a status in the front end (float version)
void CFrontEndInterface::UpdateStatus(int ControlID,float Status,bool ForceRefresh)
{
  bool DoUpdate = true;

  if(!ForceRefresh)
  {
    // Check if value is different then current value
    TFloatShadow::iterator i = m_FloatShadow.find(ControlID);

    // If the item can not be found
    if(i == m_FloatShadow.end())
      DoUpdate = true;
    else
      DoUpdate = (*i).second != Status;
  }

  if(DoUpdate)
  {
    UpdateShadow(ControlID,Status);
    DoUpdateStatus(ControlID,Status);
  }
}

// Update a status in the front end (string version)
void CFrontEndInterface::UpdateStatus(int ControlID,QString Status,bool ForceRefresh)
{
  bool DoUpdate = true;

  if(!ForceRefresh)
  {
    // Check if value is different then current value
    TStringShadow::iterator i = m_StringShadow.find(ControlID);

    // If the item can not be found
    if(i == m_StringShadow.end())
      DoUpdate = true;
    else
      DoUpdate = (*i).second != Status;
  }

  if(DoUpdate)
  {
    UpdateShadow(ControlID,Status);
    DoUpdateStatus(ControlID,Status);
  }
}

// Enable/Disable a front-end UI control
void CFrontEndInterface::EnableDisableControl(int ControlID,bool Enable,bool ForceRefresh)
{
  bool DoUpdate = true;

  if(!ForceRefresh)
  {
    // Check if value is different then current value
    TEnableDisableShadow::iterator i = m_EnableDisableShadow.find(ControlID);

    // If the item can not be found
    if(i == m_EnableDisableShadow.end())
      DoUpdate = true;
    else
      DoUpdate = (*i).second != Enable;
  }

  if(DoUpdate)
  {
    UpdateShadow(ControlID,Enable);
    DoEnableDisableControl(ControlID,Enable);
  }
}

void CFrontEndInterface::MonitorPrint(QString Msg)
{
  QMonitor.Print(Msg.c_str());

  if(RemoteModeEnabled)
  try
  {
    m_RemoteFrontEndInterface->MonitorPrint(Msg);
  } catch(...)
    {
      RemoteModeEnabled = false;
    }
}
bool CFrontEndInterface::IsWizardRunning(void)
{
	return Q2RTApplication->GetMachineManager()->IsWizardRunning();
}

void CFrontEndInterface::CancelWizard(QString Msg)
{
	if(IsWizardRunning)
	{
		CQLog::Write(LOG_TAG_GENERAL, QFormatStr("Canceling running wizard: %s", Msg.c_str()));
		FrontEndInterface->UpdateStatus(FEOP_CANCEL_WIZARD,Msg,true);
    }
}


// Show notification message
// [@Assumption] IMPORTANT ! This message will interrupt (GUI Thread) a Wizard
// in doing some background work, e.g. during actual pumping in MRW 
void CFrontEndInterface::NotificationMessage(QString Msg)
{
  //To reduce communications errors being popped up during automated testing (within VMs),
  //mask the notification messages (QA only)
  if ((true == CAppParams::Instance()->SimulatorMode) && Q2RTApplication->GetQATestsAllowed())
  {
	 if (0 == m_tidComm)
		m_tidComm = Q2RTApplication->GetCommThreadId();

	 if (GetCurrentThreadId() == m_tidComm)
	  	return;
  }

  if(!RemoteModeEnabled)
    QMonitor.NotificationMessage(Msg);
  else
    try
    {
      m_RemoteFrontEndInterface->NotificationMessage(Msg);
    } catch(...)
      {
        RemoteModeEnabled = false;
      }
}

// Show warning message
void CFrontEndInterface::WarningMessage(QString Msg)
{
  if(!RemoteModeEnabled)
    QMonitor.WarningMessage(Msg);
  else
    try
    {
      m_RemoteFrontEndInterface->WarningMessage(Msg);
    } catch(...)
      {
        RemoteModeEnabled = false;
      }
}

// Show error message
void CFrontEndInterface::ErrorMessage(QString Msg)
{
  if(!RemoteModeEnabled)
    QMonitor.ErrorMessage(Msg);
  else
    try
    {
	  m_RemoteFrontEndInterface->ErrorMessage(Msg);
    } catch(...)
	  {
        RemoteModeEnabled = false;
      }
}

// Refresh all the statuses
TQErrCode CFrontEndInterface::RefreshStatus(void)
{
  for(TIntShadow::iterator i = m_IntShadow.begin(); i != m_IntShadow.end(); ++i)
    DoUpdateStatus((*i).first,(*i).second);

  for(TFloatShadow::iterator i = m_FloatShadow.begin(); i != m_FloatShadow.end(); ++i)
    DoUpdateStatus((*i).first,(*i).second);

  for(TStringShadow::iterator i = m_StringShadow.begin(); i != m_StringShadow.end(); ++i)
    DoUpdateStatus((*i).first,(*i).second);

  for(TEnableDisableShadow::iterator i = m_EnableDisableShadow.begin(); i != m_EnableDisableShadow.end(); ++i)
    DoEnableDisableControl((*i).first,(*i).second);

  return Q_NO_ERROR;
}


// Show the 'Cartridge error' dialog
TQErrCode CFrontEndInterface::ShowCartridgeError(int DialogType, int Cartridge, bool Modal)
{
  return CartridgeErrorForm->ShowDialog(DialogType, Cartridge, Modal);
}

TQErrCode CFrontEndInterface::ShowMaterialWarning(int Value)
{
  if (!m_DisplayMaterialWarningEnable)
     return Q_NO_ERROR;
  return MaterialWarningForm->ShowDialog(Value);
}

void CFrontEndInterface::EnableMaterialWarning(bool Enable)
{
  m_DisplayMaterialWarningEnable = Enable;
}

// Show the 'TankIDNotice' dialog
TQErrCode CFrontEndInterface::ShowTankIDNotice(int DialogType, int Cartridge, int Modal)
{
  return TTankIDNotice::ShowDialog(DialogType, static_cast<TTankIndex>(Cartridge), Modal);
}

// Show the 'TankIDNotice' dialog
TQErrCode CFrontEndInterface::SendMsgToTankIDNoticeDlg(int Cartridge, int Msg, bool Blocking)
{
  TTankIDNotice::DlgSendMessage(Cartridge, Msg, Blocking);
  return Q_NO_ERROR;
}

// Show the 'TankIDNotice' dialog
TQErrCode CFrontEndInterface::HideTankIDNoticeDlg(int Cartridge)
{
  TTankIDNotice::HideDialog(Cartridge);
  return Q_NO_ERROR;
}

QString CFrontEndInterface::GetTankIDNoticeText(int Cartridge)
{
  return TTankIDNotice::GetNoticeText(Cartridge);
}

void CFrontEndInterface::SetDisabledIconHint(QString HintText)
{

}

// Suppress the 'TankIDNotice' dialog popups
TQErrCode CFrontEndInterface::SuppressDialogPopupsTankIDNotice(int Cartridge)
{
  TTankIDNotice::SuppressDialogPopups(Cartridge);
  return Q_NO_ERROR;
}

// Allow the 'TankIDNotice' dialog popups
TQErrCode CFrontEndInterface::AllowDialogPopupsTankIDNotice(int Cartridge)
{
  TTankIDNotice::AllowDialogPopups(Cartridge);
  return Q_NO_ERROR;
}
TQErrCode CFrontEndInterface::InformStopToCartridgeDlg()
{
  return CartridgeErrorForm->InformStopToDlg();
}

// Give the UI thread some time to run
TQErrCode CFrontEndInterface::YieldUIThread(void)
{
  // This function should do something only when called from the VCL (main) thread
  if(GetCurrentThreadId() == m_GUIMainThread)
	Application->ProcessMessages();

  return Q_NO_ERROR;
}


void CFrontEndInterface::UpdateShadow(int ControlID,int Status)
{
  if (IS_REFRESHABLE_ID(ControlID))
     m_IntShadow[ControlID] = Status;
}

void CFrontEndInterface::UpdateShadow(int ControlID,float Status)
{
  if (IS_REFRESHABLE_ID(ControlID))
     m_FloatShadow[ControlID] = Status;
}

void CFrontEndInterface::UpdateShadow(int ControlID,QString Status)
{
  if (IS_REFRESHABLE_ID(ControlID))
     m_StringShadow[ControlID] = Status;
}

void CFrontEndInterface::UpdateShadow(int ControlID,bool Status)
{
  if (IS_REFRESHABLE_ID(ControlID))
     m_EnableDisableShadow[ControlID] = Status;
}

// Load 32 bit RGBA bitmap (BMP file with Alpha layer) and calculate blending transparency with underlaying container color
void CFrontEndInterface::LoadBitmap32(/* Graphics::TBitmap */ void *BMP, int ResourceID, /* TColor */ int BackGroundColor)
{
	TColor backCol = (TColor)BackGroundColor;
	Graphics::TBitmap *bmp = (Graphics::TBitmap *)BMP;

	TResourceStream *Res;

	PRGBQuad p;

	try
	{
		Res = new TResourceStream((int)HInstance, ResourceID, RT_RCDATA);
		bmp->LoadFromStream(Res);

		int backR = GetRValue(ColorToRGB(backCol));
		int backG = GetGValue(ColorToRGB(backCol));
		int backB = GetBValue(ColorToRGB(backCol));
        

		for(int row = 0, col; row < bmp->Height; row++)
		{
			col = bmp->Width;
			p   = (PRGBQuad)bmp->ScanLine[row];

			for(;col > 0; p++, col--)
			{
				int alpha = p->rgbReserved;

				p->rgbRed   = (p->rgbRed   * alpha + backR * (0xFF - alpha)) / 0xFF;
				p->rgbGreen = (p->rgbGreen * alpha + backG * (0xFF - alpha)) / 0xFF;
				p->rgbBlue  = (p->rgbBlue  * alpha + backB * (0xFF - alpha)) / 0xFF;
			}
		}
	}
	__finally
	{
		Q_SAFE_DELETE(Res);
    }
}
