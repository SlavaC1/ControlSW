//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "HeadsControlDlg.h"
#include "EdenHeadsFrame.h"
#include "FrontEndParams.h"
#include "FrontEnd.h"
#include "BackEndInterface.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "EdenHeadsFrame"
#pragma resource "*.dfm"
THeadsControlForm *HeadsControlForm;


//---------------------------------------------------------------------------
__fastcall THeadsControlForm::THeadsControlForm(TComponent* Owner)
  : TForm(Owner)
{}
//---------------------------------------------------------------------------
THeadsFrame* THeadsControlForm::GetUIFrame()
{
  return HeadsFrame;
}
//---------------------------------------------------------------------------


/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
void __fastcall THeadsControlForm::FormShow(TObject *Sender)
{  
  CFrontEndParams*   FrontEndParams = CFrontEndParams::Instance();
  CBackEndInterface* BackEnd        = CBackEndInterface::Instance();

  FOR_ALL_QUALITY_MODES(qm)
    	FOR_ALL_OPERATION_MODES(om)
          if (GetModeAccessibility(qm, om) == true)
             BackEnd->EnableConfirmationBypass(PER_MACHINE_MODE[qm][om], MACHINE_QUALITY_MODES_DIR(qm,om));

  AlwaysOnTopAction->Checked = (FormStyle == fsStayOnTop);

  if (FrontEndParams->HeadsDlgLeftPosition != -1)
    Left = FrontEndParams->HeadsDlgLeftPosition;
  else
    Left = (Screen->Width - Width) / 2;

  if (FrontEndParams->HeadsDlgTopPosition != -1)
    Top = FrontEndParams->HeadsDlgTopPosition;
  else
    Top = (Screen->Height - Height) / 2;

  HeadsFrame->DisplayCurrentParameters();
  HeadsFrame->UpdateDevicesStatus();

  FrontEndInterface->RefreshStatus();
}
//---------------------------------------------------------------------------
void __fastcall THeadsControlForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
  CFrontEndParams*   FrontEndParams = CFrontEndParams::Instance();
  CBackEndInterface* BackEnd        = CBackEndInterface::Instance();

  FOR_ALL_QUALITY_MODES(qm)
   	 FOR_ALL_OPERATION_MODES(om)
          if (GetModeAccessibility(qm, om) == true)
             BackEnd->DisableConfirmationBypass(PER_MACHINE_MODE[qm][om], MACHINE_QUALITY_MODES_DIR(qm,om));

  FrontEndParams->HeadsDlgLeftPosition = Left;
  FrontEndParams->HeadsDlgTopPosition  = Top;
  FrontEndParams->HeadsDlgAlwaysOnTop  = (FormStyle == fsStayOnTop);
}
//---------------------------------------------------------------------------
void __fastcall THeadsControlForm::AlwaysOnTopActionExecute(TObject *Sender)
{
  AlwaysOnTopAction->Checked = !AlwaysOnTopAction->Checked;
  FormStyle = AlwaysOnTopAction->Checked ? fsStayOnTop : fsNormal;
}
//---------------------------------------------------------------------------
void __fastcall THeadsControlForm::CloseDialogActionExecute(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------
void __fastcall THeadsControlForm::FormPaint(TObject *Sender)
{
//  if (!Visible)
//	 return;
	 
//  HeadsFrame->DisplayCurrentParameters();
//  HeadsFrame->UpdateDevicesStatus();
}
//---------------------------------------------------------------------------
/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
