//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "GeneralDevicesDlg.h"
#include "FrontEndParams.h"
#include "FrontEnd.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "EdenGeneralDeviceFrame"
#pragma resource "*.dfm"
TGeneralDevicesForm *GeneralDevicesForm;

//---------------------------------------------------------------------------
__fastcall TGeneralDevicesForm::TGeneralDevicesForm(TComponent* Owner)
  : TForm(Owner)
{}

//---------------------------------------------------------------------------
TGeneralDeviceFrame* TGeneralDevicesForm::GetUIFrame()
{
  return GeneralDeviceFrame;
}

void __fastcall TGeneralDevicesForm::FormClose(TObject *Sender,TCloseAction &Action)
{
  CFrontEndParams *FrontEndParams = CFrontEndParams::Instance();
  
  FrontEndParams->DevicesDlgLeftPosition = Left;
  FrontEndParams->DevicesDlgTopPosition = Top;
  FrontEndParams->DevicesDlgAlwaysOnTop = (FormStyle == fsStayOnTop);
}
//---------------------------------------------------------------------------

void __fastcall TGeneralDevicesForm::FormShow(TObject *Sender)
{  
  CAppParams *ParamsMgr = CAppParams::Instance();
  if(ParamsMgr->TrayHeaterEnabled)
  {
	 GeneralDeviceFrame->Width = GeneralDeviceFrame->TrayHeaterGroupBox->Left + GeneralDeviceFrame->TrayHeaterGroupBox->Width + 10;
	 this->Width               = GeneralDeviceFrame->Width + 5;
  }
  else if(ParamsMgr->EvacuationIndicatorEnabled)
  {
	 GeneralDeviceFrame->Width = GeneralDeviceFrame->EvacuationIndicatorGroupBox->Left + GeneralDeviceFrame->EvacuationIndicatorGroupBox->Width + 10;
	 this->Width               = GeneralDeviceFrame->Width + 5;
  }

  CFrontEndParams *FrontEndParams = CFrontEndParams::Instance();

  AlwaysOnTopAction->Checked = (FormStyle == fsStayOnTop);

  // Set from position
  if(FrontEndParams->DevicesDlgLeftPosition != -1)
    Left = FrontEndParams->DevicesDlgLeftPosition;
  else
    Left = (Screen->Width - Width) / 2;

  if (FrontEndParams->DevicesDlgTopPosition != -1)
    Top = FrontEndParams->DevicesDlgTopPosition;
  else
    Top = (Screen->Height - Height) / 2;

  GeneralDeviceFrame->ReqRollerSpeedUpDown->Min = CAppParams::Instance()->RollerVelocity.MinLimit();
  GeneralDeviceFrame->ReqRollerSpeedUpDown->Max = CAppParams::Instance()->RollerVelocity.MaxLimit();

  GeneralDeviceFrame->DisplayCurrentParameters();
  GeneralDeviceFrame->UpdateDevicesStatus();
  GeneralDeviceFrame->UpdateUVDimming();
  FrontEndInterface->RefreshStatus();
}
//---------------------------------------------------------------------------

void __fastcall TGeneralDevicesForm::AlwaysOnTopActionExecute(TObject *Sender)
{
  AlwaysOnTopAction->Checked = !AlwaysOnTopAction->Checked;

  // Temporarly disable the OnShow event 
  TNotifyEvent TmpEvent = OnShow;
  OnShow = NULL;
  FormStyle = AlwaysOnTopAction->Checked ? fsStayOnTop : fsNormal;
  OnShow = TmpEvent;
}
//---------------------------------------------------------------------------

void __fastcall TGeneralDevicesForm::CloseDialogActionExecute(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------


void __fastcall TGeneralDevicesForm::GeneralDeviceFrameTurnRollerButtonClick(
      TObject *Sender)
{
  GeneralDeviceFrame->RollerOnActionExecute(Sender);

}
//---------------------------------------------------------------------------

