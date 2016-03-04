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
  this->Height = GeneralDeviceFrame->DisplayValuesModeCheckBox->Top + GeneralDeviceFrame->DisplayValuesModeCheckBox->Height + 50;

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
  FrontEndInterface->RefreshStatus(); //this line makes all the right tanks to be the active tanks
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

