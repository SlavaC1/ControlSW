//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "ChooseSilentMRWMode.h"
#include "BackEndInterface.h"
#include "QMonitor.h"
#include "GlobalDefs.h"
#include "AppParams.h"

//--------------------------------------------------------------------- 
#pragma resource "*.dfm"
TChooseSilentMRWModeDLG *ChooseSilentMRWModeDLG;



//---------------------------------------------------------------------
__fastcall TChooseSilentMRWModeDLG::TChooseSilentMRWModeDLG(TComponent* AOwner)
	: TForm(AOwner)
{
  m_Abort = false;
}
//---------------------------------------------------------------------
void TChooseSilentMRWModeDLG::AbortDialog()
{
    m_Abort = true;
    ModalResult = mrCancel;

    // The TChooseSilentMRWModeDLG::FormCloseQuery may ask a modal QMonitor.NotificationMessageWaitOk question, which also must be terminated: (In which case, we don't need the CancelClick() because the form is already in the process of closing)
    HANDLE MessageFormHDL = NULL;
    MessageFormHDL = FindWindow("TMessageForm", "Information");
    if (MessageFormHDL != NULL)
      SendMessage(MessageFormHDL, WM_CLOSE, 0, 0);
    else
      CancelClick(this);
}
//---------------------------------------------------------------------------
void TChooseSilentMRWModeDLG::ConfirmDialog()
{
    m_Abort = true;

    // !! Don't change ModalResult value, which should be here == mrDMMode
	
    // The TChooseSilentMRWModeDLG::FormCloseQuery may ask a modal QMonitor.NotificationMessageWaitOk question, which also must be terminated: (In which case, we don't need the CancelClick() because the form is already in the process of closing)
    HANDLE MessageFormHDL = NULL;
    MessageFormHDL = FindWindow("TMessageForm", "Information");
    if (MessageFormHDL != NULL)
      SendMessage(MessageFormHDL, WM_CLOSE, 0, 0);
    else
      SetFocus();
}
//---------------------------------------------------------------------------
bool TChooseSilentMRWModeDLG::IsRelatedDialogPending(TTankIndex Cartridge)
{
  if (false == Visible)
    return false;

  return (Cartridge == m_RelatesToCartridge);
}
//---------------------------------------------------------------------------





/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
void __fastcall TChooseSilentMRWModeDLG::OKClick(TObject *Sender)
{
  Timer1->Enabled = false;
  ModalResult = ((DigitalModeRadio->Checked) ? mrDMMode : mrSingleMode);
}
//---------------------------------------------------------------------------
void __fastcall TChooseSilentMRWModeDLG::CancelClick(TObject *Sender)
{
  ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TChooseSilentMRWModeDLG::FormCloseQuery(TObject *Sender,
      bool &CanClose)
{
  CBackEndInterface *BackEnd = CBackEndInterface::Instance();
  CanClose = true;

  if ((ModalResult == mrCancel) || (ModalResult == mrSingleMode))
    return;
//OBJET_MACHINE //itamar objet check!!! adjustment
// In DM mode confirm that both Tanks are inserted:
  if ((BackEnd->IsRelevantTankInserted(TYPE_TANK_MODEL1) && BackEnd->IsRelevantTankInserted(TYPE_TANK_MODEL3))
	|| (BackEnd->IsRelevantTankInserted(TYPE_TANK_MODEL1) && BackEnd->IsRelevantTankInserted(TYPE_TANK_MODEL4))
	|| (BackEnd->IsRelevantTankInserted(TYPE_TANK_MODEL1) && BackEnd->IsRelevantTankInserted(TYPE_TANK_MODEL5))
	|| (BackEnd->IsRelevantTankInserted(TYPE_TANK_MODEL1) && BackEnd->IsRelevantTankInserted(TYPE_TANK_MODEL6))
	|| (BackEnd->IsRelevantTankInserted(TYPE_TANK_MODEL2) && BackEnd->IsRelevantTankInserted(TYPE_TANK_MODEL3))
	|| (BackEnd->IsRelevantTankInserted(TYPE_TANK_MODEL2) && BackEnd->IsRelevantTankInserted(TYPE_TANK_MODEL4))
	|| (BackEnd->IsRelevantTankInserted(TYPE_TANK_MODEL2) && BackEnd->IsRelevantTankInserted(TYPE_TANK_MODEL5))
	|| (BackEnd->IsRelevantTankInserted(TYPE_TANK_MODEL2) && BackEnd->IsRelevantTankInserted(TYPE_TANK_MODEL6))
	|| (BackEnd->IsRelevantTankInserted(TYPE_TANK_MODEL3) && BackEnd->IsRelevantTankInserted(TYPE_TANK_MODEL5))
	|| (BackEnd->IsRelevantTankInserted(TYPE_TANK_MODEL3) && BackEnd->IsRelevantTankInserted(TYPE_TANK_MODEL6))
	|| (BackEnd->IsRelevantTankInserted(TYPE_TANK_MODEL4) && BackEnd->IsRelevantTankInserted(TYPE_TANK_MODEL5))
	||  (BackEnd->IsRelevantTankInserted(TYPE_TANK_MODEL4) && BackEnd->IsRelevantTankInserted(TYPE_TANK_MODEL6)))
    return;
  Timer1->Enabled = true;

  QMonitor.NotificationMessageWait("This mode requires two model-material cartridges.\r\nLoad a cartridge in the other model-material compartment.");

  // Check if dialog was aborted.
  if (m_Abort)
  {
    Timer1->Enabled = false;
    return;
  }

  CanClose = false;
}
//---------------------------------------------------------------------------
void __fastcall TChooseSilentMRWModeDLG::FormShow(TObject *Sender)
{
    m_Abort = false;
}
//---------------------------------------------------------------------------
void __fastcall TChooseSilentMRWModeDLG::Timer1Timer(TObject *Sender)
{
  CBackEndInterface *BackEnd = CBackEndInterface::Instance();
  
  // Reason for having the timer is for the case where user inserts one incompatibe cart. then chooses silent, then chooses DM,
  // then gets "Please insert the other Model ..." and then he inserts a VALID catr. to other housing.
  // Since it is Valid, no TTankDialog is launched that can handle the closing of it.
//OBJET_MACHINE adjustment
// In DM mode confirm that both Tanks are inserted:
  if (BackEnd->IsRelevantTankInserted(TYPE_TANK_MODEL2) && BackEnd->IsRelevantTankInserted(TYPE_TANK_MODEL4)
    || BackEnd->IsRelevantTankInserted(TYPE_TANK_MODEL4) && BackEnd->IsRelevantTankInserted(TYPE_TANK_MODEL6)
    ||  BackEnd->IsRelevantTankInserted(TYPE_TANK_MODEL2) && BackEnd->IsRelevantTankInserted(TYPE_TANK_MODEL6))
             if (BackEnd->GetTankMaterialType(TYPE_TANK_MODEL2) != "" && BackEnd->GetTankMaterialType(TYPE_TANK_MODEL4) != ""
                || BackEnd->GetTankMaterialType(TYPE_TANK_MODEL4) != "" && BackEnd->GetTankMaterialType(TYPE_TANK_MODEL6) != ""
                ||  BackEnd->GetTankMaterialType(TYPE_TANK_MODEL2) != "" && BackEnd->GetTankMaterialType(TYPE_TANK_MODEL6) != "")
    ConfirmDialog();
}
//---------------------------------------------------------------------------

/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
