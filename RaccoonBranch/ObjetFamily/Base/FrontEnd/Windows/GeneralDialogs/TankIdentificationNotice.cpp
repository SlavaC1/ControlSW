//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Q2RTApplication.h"
#include "TankIdentificationNotice.h"
#include "Container.h"
#include "FrontEndControlIDs.h"
#include "BackEndInterface.h"
#include "QUtils.h"
#include "QMutex.h"
#include "ChooseSilentMRWMode.h"
#include "GlobalDefs.h"
#include "AppParams.h"
#include "FrontEnd.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TTankIDNotice *TankIDNotice;

// Initialize the static Forms:
TTankIDNotice *TTankIDNotice::m_TankIDNoticeArray[TOTAL_NUMBER_OF_CONTAINERS] = {NULL};
TTankIDNoticeTester *TTankIDNotice::m_Tester = NULL;

// Following const strings are the message text according to each dialog type:

 const char PIPE_NO_MATCH_TEXT[] = "\
The inserted cartridge contains material that does not match \n\
the material previously used in this compartment.\n\
To prevent the printing of models with remains \n\
of the previous material, run the Material Replacement Wizard \n\
for this compartment.";

 const char MACHINE_NO_MATCH_TEXT[] = "\
The inserted cartridge contains material that does not match \n\
the material in the print block.\n\
To prevent the printing of models with remains \n\
of the previous material, run the Material Replacement Wizard \n\
before using this cartridge.";

 const char MACHINE_NO_MATCH_MSPRINTING[] = "\
The inserted cartridge contains material that does not match\n\
the material in the print block.\n\
Therefore, the cartridge will not be used \n\
for the current print job.\n\
NOTE: The material type cannot be changed during printing.";

 const char PIPE_NO_MATCH_MSPRINTING[] = "\
The inserted cartridge contains material \n\
that does not match the material previously used \n\
in this compartment. Therefore, the cartridge \n\
will not be used for the current print job. \n\
NOTE: The material type cannot be changed during printing.";

 const char UNSUPPORTED_RESIN_TEXT[] = "\
The inserted cartridge contains material \n\
that is not compatible with the printer. \n\
Replace it with a compatible cartridge.  \n\
NOTE: You may need to install a patch to use this material. \n\
To check and download available material patches, go to \n\
www.stratasys.com.";

 const char MODEL_IN_SUPPORT_MISPLACE_TEXT[] = "\
The system identified this Support cartridge as a Model\n\
cartridge.\n\
Remove the inserted cartridge and inform your local\n\
Stratasys Ltd. representative.";

 const char SUPPORT_IN_MODEL_MISPLACE_TEXT[] = "\
The system identified this Model cartridge as a Support\n\
cartridge.\n\
Remove the inserted cartridge and inform your local\n\
Stratasys Ltd. representative.";

 const char IDENTIFICATION_DISQUALIFED_TEXT[] = "\
A material identification error has occured. \n\
Please re-insert tank to retry.";

 const char RESIN_EXPIRED_TEXT[] = "\
The inserted cartridge is passed its expiration date.\n\
For operation and system safety - The system will\n\
ignore this cartridge";

 const char NO_TAG_TEXT[] = "\
This cartridge cannot be identified.\n\
To ensure model quality and maximize print-head life,\n\
replace the cartridge.\n\
For further assistance, contact your Stratasys service provider.";

 const char SERVICE_IN_REGULAR_TEXT[] = "\
The inserted cartridge contains material \n\
that is not suitable for printing.";

 const char REGULAR_IN_SERVICE_TEXT[] = "\
The printer is filled with cleaning fluid, \n\
which must be flushed before using this cartridge. \n\
To flush the cleaning fluid, \n\
run the Material Replacement Wizard.";

#ifdef OBJET_MACHINE
const char CHECK_CONTAINER_REFILL_TEXT[] = "\
Load cell has a large weight deviation. \n\
Replace the cartridge and try again, \n\
and run the Load Cell Calibration Wizard if necessary. \n\
If the problem persists, contact your Stratasys service provider.";

const char CONTAINER_CALIBRATION_PROBLEM_TEXT[] = "\
The cartridge weight is not stabilizing. \n\
Remove the cartridge and insert it again. \n\
If the problem persists, contact your Stratasys service provider";

const char INVALID_TAG_VERSION_TEXT[] = "\
The cartridge has an invalid tag version.";

const char MATERIAL_CONSUMPTION_PROBLEM_TEXT[] = "\
Load cell not measuring material usage accurately. \n\
Run the Load Cell Calibration Wizard. \n\
If the problem persists, contact your Stratasys service provider.";

const char READING_TOO_LOW_PROBLEM_TEXT[] = "\
Load cell reading too low. \n\
Run the Load Cell Calibration Wizard. \n\
If the problem persists, contact your Stratasys service provider.";
#endif

const char NOT_LICENSED_CARTRIDGE_TEXT[] = "\
The cartridge loaded is not compatible with the terms\n\
of your license. Replace it and click OK.\n\
To upgrade your license, contact your Stratasys dealer.";

const char TANK_IS_INACTIVE_TEXT[] = "\
The cartridge was inserted in a disabled compartment. \n\n\
This compartment is disabled because the printer is currenly \n\
configured for a printing mode that does not use it. \n\n\
To use this cartridge, either insert it into a different \n\
compartment, or run the Material Replacement Wizard.";

const char AUTHENTICATION_FAILED_TEXT[] = "\
Cartridge cannot be identified";

const char OVERCONSUMPTION_TEXT[] = "\
Cartridge seems to be empty. \n\
If this is not the case, please contact customer support.";


// Help file names
const char RF_HELP_FILE_1_NAME[] = "RF_Help_App1.hlp";
const char RF_HELP_FILE_2_NAME[] = "RF_Help_App2.hlp";
const char RF_HELP_FILE_3_NAME[] = "RF_Help_App3.hlp";
const char RF_HELP_FILE_4_NAME[] = "RF_Help_App4.hlp";
const char RF_HELP_FILE_5_NAME[] = "RF_Help_App5.hlp";
const char RF_HELP_FILE_6_NAME[] = "RF_Help_App6.hlp";
const char RF_HELP_FILE_7_NAME[] = "RF_Help_App7.hlp";
const char RF_HELP_FILE_8_NAME[] = "RF_Help_App8.hlp";
const char RF_HELP_FILE_9_NAME[] = "RF_Help_App9.hlp";

void ExternalAppExecute(const char *FileName,const char *CmdLineParam);

//---------------------------------------------------------------------------
__fastcall TTankIDNotice::TTankIDNotice(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void TTankIDNotice::SetContainerName(QString name)
{
  m_ContainerName = name;
  CartridgeTypeLabel->Caption = m_ContainerName.c_str();
}

TQErrCode TTankIDNotice::CreateTankIDNoticeForms()
{
  // create a tester object to register it in Roster:
  m_Tester = new TTankIDNoticeTester;
      for(int i = LAST_TANK_TYPE-1; i >= FIRST_TANK_TYPE; i--)
      {
        m_TankIDNoticeArray[i] = new TTankIDNotice(Application);
        m_TankIDNoticeArray[i]->SetContainerName(TankToStr((TTankIndex)i));
      }
  return Q_NO_ERROR;
}

TQErrCode TTankIDNotice::DestroyTankIDNoticeForms()
{
  // Note that there is no need to delete TForm. Just assign it to NULL.
     for(int i = LAST_TANK_TYPE-1; i >= FIRST_TANK_TYPE; i--)
        m_TankIDNoticeArray[i] = NULL;

  if (m_Tester)
    delete m_Tester;

  return Q_NO_ERROR;
}

TQErrCode TTankIDNotice::ShowDialog(int DialogType, TTankIndex Cartridge, bool Modal)
{
  HANDLE hdl = NULL;
  TTankIDNotice* CurrentForm = NULL;
  TTankIDNotice::GetDialog(Cartridge, &CurrentForm, &hdl);

  // It may happen that the TTankIDNotice form is not yet registered (During system starting). We ignore the function in this case.
  if (!CurrentForm)
    return Q_NO_ERROR;

  CurrentForm->m_ShowDialogPending = true;
      
  // If we are requested to show the currently shown form, with same params, then we need do nothing:
  if ((CurrentForm->Visible) && (CurrentForm->m_DlgType == DialogType) && (CurrentForm->m_Cartridge == Cartridge))
  {
    return Q_NO_ERROR;
  }

  if (FE_TANK_ID_NOTICE_DLG_RECENT_DLG_TYPE != DialogType)
    CurrentForm->m_DlgType   = DialogType;

  CurrentForm->m_Cartridge = Cartridge;
  CurrentForm->m_Modal     = Modal;

  CurrentForm->m_BackEndInterface = CBackEndInterface::Instance();
  CurrentForm->m_ChooseModeModalResult = mrNone;

  if (Modal)
  {
    SendMessage(hdl,WM_SHOW_DIALOG, CurrentForm->m_DlgType, CurrentForm->m_Cartridge);

    if (CurrentForm->ModalResult == mrCancel)
      return Q2RT_CARTRIDGE_ERR_DLG_STOP;
    else
      return Q_NO_ERROR;
  }
  PostMessage(hdl,WM_SHOW_DIALOG, CurrentForm->m_DlgType, CurrentForm->m_Cartridge);
  return Q_NO_ERROR;
}

void TTankIDNotice::HideDialog(int Cartridge)
{
  HANDLE hdl = NULL;
  TTankIDNotice* CurrentForm = NULL;

  TTankIDNotice::GetDialog(Cartridge, &CurrentForm, &hdl);

  // It may happen that the TTankIDNotice form is not yet registered (During system starting).
  // We ignore the function in this case.
  if (!CurrentForm)
    return;

  CurrentForm->m_ShowDialogPending = false;
  SendMessage(hdl,WM_HIDE_DIALOG,0,0);

  // make sure that the related ChooseSilentMRWModeDLG, if exists, will also close when this dialog is hidden.
  if (ChooseSilentMRWModeDLG->IsRelatedDialogPending((TTankIndex)Cartridge))
    ChooseSilentMRWModeDLG->AbortDialog();
}

void TTankIDNotice::DlgSendMessage(int Cartridge, int Msg, bool Blocking)
{
  HANDLE hdl = NULL;
  TTankIDNotice* CurrentForm = NULL;

  TTankIDNotice::GetDialog(Cartridge, &CurrentForm, &hdl);

  // It may happen that the TTankIDNotice form is not yet registered (During system starting).
  // We ignore the function in this case.
  if (!CurrentForm)
    return;

  if (Blocking)
  {
    SendMessage(hdl,Msg,0,0);
    return;
  }
  PostMessage(hdl,Msg,0,0);
}

void TTankIDNotice::SuppressDialogPopups(int Cartridge)
{
  HANDLE hdl = NULL;
  TTankIDNotice* CurrentForm = NULL;

  TTankIDNotice::GetDialog(Cartridge, &CurrentForm, &hdl);

  // It may happen that the TTankIDNotice form is not yet registered (During system starting).
  // We ignore the function in this case.
  if (!CurrentForm)
    return;

  SendMessage(hdl,WM_SUPPRESS_DIALOG_POPUPS,0,0);
}

void TTankIDNotice::AllowDialogPopups(int Cartridge)
{
  HANDLE hdl = NULL;
  TTankIDNotice* CurrentForm = NULL;

  TTankIDNotice::GetDialog(Cartridge, &CurrentForm, &hdl);

  // It may happen that the TTankIDNotice form is not yet registered (During system starting).
  // We ignore the function in this case.
  if (!CurrentForm)
    return;

  SendMessage(hdl,WM_ALLOW_DIALOG_POPUPS,0,0);
}


int TTankIDNotice::OpenDialog(int DialogType, int Cartridge)
{
  m_dlgText = "";

  if (!this->Visible)
  {
      Visible = true;
  }

  WarningTextMemo->Clear();

  // Base Controls Settings: (changed later according to dialog type)
  OptionsGroupBox->Visible            = true;
  OKButton->Enabled                   = true;
  Color                               = clBtnFace;
  BorderStyle                         = bsDialog;
  RunResinReplacementRadio->Visible   = true;
  RunResinReplacementRadio->Enabled   = true;
  RunResinReplacementRadio->Checked   = false;
  RunResinReplacementRadio->Caption   = "Continue to Material Replacement Wizard.";
  IAcceptRadio->Enabled               = true;
  IAcceptRadio->Checked               = false;
  IAcceptRadio->Caption               = "Use without Material Replacement Wizard.";
  IAcceptRadio->Font->Color           = clBlack;  
  NotRecommendedLabel->Visible        = true;
  NotRecommendedLabel->Enabled        = true;    
  IDontAcceptRadio->Enabled           = true;
  IDontAcceptRadio->Checked           = true;
  IDontAcceptRadio->Caption           = "Do not use this cartridge for now.";
  IDontAcceptRadio->Font->Color       = clBlack;
  Notice->Visible                     = true;
  WarningTextMemo->Visible            = false;
  InsertedResinGroupBox->Visible      = true;
  MachineResinTypeGroupBox->Visible   = true;
  ExpirationDateGroupBox->Visible     = false;
  MachineResinTypeLabel->Visible      = true;
  PipeResinTypeLabel->Visible         = false;
  NotificationTitleLabel->Font->Color = clBlack;
  CartridgeTypeLabel->Font->Color     = clBlack;
  CartridgeLabel->Font->Color         = clBlack;
  TagIDValueLabel->Font->Color        = clBlack;
  TagIdLabel->Font->Color             = clBlack;
  OptionsGroupBox->Font->Color        = clBlack;

  // Show the Tag ID in the dialog:
  TagIDValueLabel->Caption = m_BackEndInterface->GetContainerTagID(m_Cartridge).c_str();

  // Set the Tank resin type:
  InsertedResinTypeLabel->Caption = m_BackEndInterface->GetTankMaterialType(m_Cartridge).c_str();

  // Set the Pipe resin type:
  PipeResinTypeLabel->Caption = m_BackEndInterface->GetPipeMaterialType(m_Cartridge).c_str();

  // Set the machine resin type in dialog
  MachineResinTypeLabel->Caption = m_BackEndInterface->GetMachineMaterialType(m_Cartridge).c_str();

  // Arrange the dialog appearance according to dialog type:
  switch (DialogType)
  {
     case FE_TANK_ID_NOTICE_DLG_SERVICE_IN_REGULAR:
       NotificationTitleLabel->Caption = "Non-Printing Cartridge";
       m_dlgText                       = SERVICE_IN_REGULAR_TEXT;
       OptionsGroupBox->Visible        = false;
       break;

     case FE_TANK_ID_NOTICE_DLG_REGULAR_IN_SERVICE:
       NotificationTitleLabel->Caption = "Cleaning Fluid in Printer";
       m_dlgText                       = REGULAR_IN_SERVICE_TEXT;
       IAcceptRadio->Enabled           = false;
       NotRecommendedLabel->Enabled    = false;
       break;

     case FE_TANK_ID_NOTICE_DLG_PIPE_NO_MATCH:
       NotificationTitleLabel->Caption = "Material Mismatch in Compartment";
       m_dlgText                       = PIPE_NO_MATCH_TEXT;
       MachineResinTypeLabel->Visible  = false;
       PipeResinTypeLabel->Visible     = true;
       break;

	 case FE_TANK_ID_NOTICE_DLG_PIPE_NO_MATCH_SUPPORT: 
	   NotificationTitleLabel->Caption 		= "Material Mismatch in Compartment";
	   m_dlgText                         	= PIPE_NO_MATCH_TEXT;
	   MachineResinTypeLabel->Visible    	= false;
	   PipeResinTypeLabel->Visible       	= true;
	   IAcceptRadio->Enabled             	= false;
	   NotRecommendedLabel->Enabled      	= false;
	   break;

     case FE_TANK_ID_NOTICE_DLG_MACHINE_NO_MATCH:
       NotificationTitleLabel->Caption = "Material Mismatch in Printer";
       m_dlgText                         = MACHINE_NO_MATCH_TEXT;
       break;

	 case FE_TANK_ID_NOTICE_DLG_MACHINE_NO_MATCH_SUPPORT: 
	   NotificationTitleLabel->Caption = "Material Mismatch in Compartment";
	   m_dlgText                       = MACHINE_NO_MATCH_TEXT;
	   IAcceptRadio->Enabled           = false;
	   NotRecommendedLabel->Enabled    = false;
	   break;

     case FE_TANK_ID_NOTICE_DLG_PIPE_NO_MATCH_MSPRINTING:
       NotificationTitleLabel->Caption   = "Material Mismatch in Compartment";
       m_dlgText                           = PIPE_NO_MATCH_MSPRINTING;
       MachineResinTypeLabel->Visible    = false;
       PipeResinTypeLabel->Visible       = true;
       RunResinReplacementRadio->Enabled = false;
       IAcceptRadio->Enabled             = false;
       NotRecommendedLabel->Enabled      = false;
       IDontAcceptRadio->Enabled         = false;
       break;

     case FE_TANK_ID_NOTICE_DLG_MACHINE_NO_MATCH_MSPRINTING:
       NotificationTitleLabel->Caption   = "Material Mismatch in Printer";
       m_dlgText                         = MACHINE_NO_MATCH_MSPRINTING;
       RunResinReplacementRadio->Enabled = false;
       IAcceptRadio->Enabled             = false;
       NotRecommendedLabel->Enabled      = false;
       IDontAcceptRadio->Enabled         = false;
       break;

     case FE_TANK_ID_NOTICE_DLG_UNSUPPORTED_RESIN:
       NotificationTitleLabel->Caption = "Incompatible Material";
       m_dlgText                       = UNSUPPORTED_RESIN_TEXT;
       OptionsGroupBox->Visible        = false;
       break;

     case FE_TANK_ID_NOTICE_DLG_MODEL_IN_SUPPORT_MISPLACE:
       NotificationTitleLabel->Caption = "Incorrect Cartridge Registered";
       m_dlgText                       = MODEL_IN_SUPPORT_MISPLACE_TEXT;
       OptionsGroupBox->Visible        = false;
       break;

     case FE_TANK_ID_NOTICE_DLG_SUPPORT_IN_MODEL_MISPLACE:
       NotificationTitleLabel->Caption = "Incorrect Cartridge Registered";
       m_dlgText                       = SUPPORT_IN_MODEL_MISPLACE_TEXT;
       OptionsGroupBox->Visible        = false;
       break;

     case FE_TANK_ID_NOTICE_DLG_RESIN_EXPIRED:
       NotificationTitleLabel->Caption = "Expiration Date Exceeded";
       m_dlgText                       = RESIN_EXPIRED_TEXT;
       OptionsGroupBox->Visible        = false;
       ExpirationDateGroupBox->Visible = true;
       ExpirationDateValueLabel->Caption = QDateToStr(m_BackEndInterface->GetContainerExpirationDate(m_Cartridge)).c_str();
       break;

     case FE_TANK_ID_NOTICE_DLG_NO_TAG:  
       NotificationTitleLabel->Caption    = "Unidentified Cartridge";
       m_dlgText                          = NO_TAG_TEXT;
       BorderStyle                        = bsNone;
       Position                           = poDesktopCenter;
       RunResinReplacementRadio->Visible  = false;
       NotRecommendedLabel->Visible       = false;
       IAcceptRadio->Caption              = "Use this cartridge (not recommended).";
       IDontAcceptRadio->Caption          = "Replace the cartridge.";
       MachineResinTypeGroupBox->Visible  = false;
       InsertedResinGroupBox->Visible     = false;
	   break;
	   
#ifdef OBJET_MACHINE //refill req.
     case FE_TANK_ID_NOTICE_DLG_CHECK_CONTAINER_REFILL:
	   NotificationTitleLabel->Caption    = "Cartridge error";
	   m_dlgText                          = CHECK_CONTAINER_REFILL_TEXT;
	   OptionsGroupBox->Visible        = false;
	   MachineResinTypeGroupBox->Visible  = false;
	   break;

	 case FE_TANK_ID_NOTICE_DLG_CALIBRATION_PROBLEM:
	   NotificationTitleLabel->Caption    = "Cartridge error";
	   m_dlgText                          = CONTAINER_CALIBRATION_PROBLEM_TEXT;
	   OptionsGroupBox->Visible        = false;
	   MachineResinTypeGroupBox->Visible  = false;
	   break;

	 case FE_TANK_ID_NOTICE_DLG_INVALID_TAG_VERSION:
	   NotificationTitleLabel->Caption    = "Cartridge error";
	   m_dlgText                          = INVALID_TAG_VERSION_TEXT;
	   OptionsGroupBox->Visible        = false;
	   MachineResinTypeGroupBox->Visible  = false;
	   break;

	 case FE_TANK_ID_MATERIAL_CONSUMPTION_PROBLEM:
	   NotificationTitleLabel->Caption    = "Cartridge error";
	   m_dlgText                          = MATERIAL_CONSUMPTION_PROBLEM_TEXT;
	   OptionsGroupBox->Visible        = false;
	   MachineResinTypeGroupBox->Visible  = false;
	   break;

	 case FE_TANK_ID_NOTICE_DLG_READING_TOO_LOW:
	   NotificationTitleLabel->Caption    = "Cartridge error";
	   m_dlgText                          = READING_TOO_LOW_PROBLEM_TEXT;
	   OptionsGroupBox->Visible        = false;
	   MachineResinTypeGroupBox->Visible  = false;
	   break;
	   
#endif

	 case FE_TANK_ID_NOTICE_DLG_NOT_LICENSED_CARTRIDGE:
       NotificationTitleLabel->Caption = "Incompatible Cartridge Loaded";
	   m_dlgText                         = NOT_LICENSED_CARTRIDGE_TEXT;
       OptionsGroupBox->Visible        = false;
	   break;

	 case FE_TANK_NOT_ASSOCIATED_WITH_CHAMBER:
	   NotificationTitleLabel->Caption = "Disabled Compartment";
	   m_dlgText                       = TANK_IS_INACTIVE_TEXT;
	   OptionsGroupBox->Visible        = false;
	   break;

	case FE_TANK_ID_NOTICE_DLG_AUTHENTICATION_FAILED:
	   NotificationTitleLabel->Caption = "Disabled Compartment";
	   m_dlgText                       = AUTHENTICATION_FAILED_TEXT;
	   OptionsGroupBox->Visible        = false;
	   break;

	case FE_TANK_ID_NOTICE_DLG_OVERCONSUMPTION:
	   NotificationTitleLabel->Caption = "Disabled Compartment";
	   m_dlgText                       = OVERCONSUMPTION_TEXT;
	   OptionsGroupBox->Visible        = false;
	   break;        

     default:
       NotificationTitleLabel->Caption = "Unknown Error Occured";
       m_dlgText                         = (QString("TankIDNotice received DialogType = ") + QIntToStr(DialogType)).c_str();
       OptionsGroupBox->Visible        = false;
       break;
  }

  IAcceptRadio->Enabled &= !CBackEndInterface::Instance()->ArePipesWithCleanser();
  NotRecommendedLabel->Enabled = IAcceptRadio->Enabled;    
  Notice->Caption = m_dlgText.c_str();

 #ifdef OBJET_MACHINE
  //Overall bypassing of all options presented to user in all situations
  RunResinReplacementRadio->Enabled = false;
  IAcceptRadio->Enabled             = false;

  IDontAcceptRadio->Checked 		= true;

  OptionsGroupBox->Enabled 			= false;
  OptionsGroupBox->Visible 			= false;
#endif

  // m_SuppressDialogPopups is set to 'true' for example, during Resin Replacement Wizard.
  // it blocks only the following message types from popping:
  switch (DialogType)
  {
    case FE_TANK_ID_NOTICE_DLG_PIPE_NO_MATCH:
	case FE_TANK_ID_NOTICE_DLG_MACHINE_NO_MATCH:
	case FE_TANK_NOT_ASSOCIATED_WITH_CHAMBER:
      if (m_SuppressDialogPopups == true)
        return 0;

    default:
      break;
  }

  if (!this->Visible)
  {
      Visible = true;
  }

  return 0;
}

void __fastcall TTankIDNotice::OKButtonClick(TObject *Sender)
{
  m_ChooseModeModalResult = mrOk; // Set a default value. (may be overridden later)

  if (m_BackEndInterface->IsChambersTank(TYPE_CHAMBER_SUPPORT, m_Cartridge))
  {
  // In case we are silently MRWing a support Tank, should't ask which mode DM/500. (always 500)
    m_ChooseModeModalResult = mrSingleMode;
  }
#ifdef OBJET_MACHINE
else if (m_BackEndInterface->GetTankMaterialType(TYPE_TANK_MODEL2) == m_BackEndInterface->GetTankMaterialType(TYPE_TANK_MODEL4)
             || m_BackEndInterface->GetTankMaterialType(TYPE_TANK_MODEL2) == m_BackEndInterface->GetTankMaterialType(TYPE_TANK_MODEL6)
                || m_BackEndInterface->GetTankMaterialType(TYPE_TANK_MODEL4) == m_BackEndInterface->GetTankMaterialType(TYPE_TANK_MODEL6))
  {
  // In case we are silently MRWing a model Tank, and both Tanks are the same material, again, shouldn't ask which mode DM/500. (always 500)
    m_ChooseModeModalResult = mrSingleMode;
  }
#else
  else if (m_BackEndInterface->GetTankMaterialType(TYPE_TANK_MODEL1) == m_BackEndInterface->GetTankMaterialType(TYPE_TANK_MODEL2))
  {
  // In case we are silently MRWing a model Tank, and both Tanks are the same material, again, shouldn't ask which mode DM/500. (always 500)
    m_ChooseModeModalResult = mrSingleMode;
  }
#endif

  else
  {
  	if ((IAcceptRadio->Checked == true) && (FE_TANK_ID_NOTICE_DLG_NO_TAG != m_DlgType))
    {
      ChooseSilentMRWModeDLG->SetRelatedCartridge(m_Cartridge);
      m_ChooseModeModalResult = ChooseSilentMRWModeDLG->ShowModal();

      switch (m_ChooseModeModalResult)
      {
        case mrCancel:
        case mrNone:
          return;

        case mrSingleMode:
        case mrDMMode:
          break;
      }
    }
  }

  ModalResult = mrOk;
  Close();
}

// Message handler for the WM_SUPPRESS_DIALOG_POPUPS message
void TTankIDNotice::HandleSuppressDialogPopupsMessage(TMessage &Message)
{
  // Suppress further dialogs from popping
  m_SuppressDialogPopups = true;

  // And close the current one if Visible:
  if (Visible)
    Hide();
}

// Message handler for the WM_ALLOW_DIALOG_POPUPS message
void TTankIDNotice::HandleAllowDialogPopupsMessage(TMessage &Message)
{
  // Allow further dialogs to popup
  m_SuppressDialogPopups = false;

  // And Show() the current pending dialog if there is a pending dialog:
  if (m_ShowDialogPending)
    if (!Visible)
      Show();
}

// Message handler for the WM_SHOW_DIALOG message
void TTankIDNotice::HandleShowDialogMessage(TMessage &Message)
{
  OpenDialog(Message.WParam, Message.LParam);
}

// Message handler for the WM_SHOW_DIALOG message
void TTankIDNotice::HandleHideDialogMessage(TMessage &Message)
{
  if (Visible)
  {
    Hide();
  }
}

TQErrCode TTankIDNotice::GetDialog(int Cartridge, TTankIDNotice** CurrentForm, HANDLE* hdl)
{
  if (!m_TankIDNoticeArray[Cartridge])
      return Q_NO_ERROR; // object not yet created (on startup)
  *hdl = m_TankIDNoticeArray[Cartridge]->Handle;
  *CurrentForm = m_TankIDNoticeArray[Cartridge];
  return Q_NO_ERROR;
}

//---------------------------------------------------------------------------

void __fastcall TTankIDNotice::FormClose(TObject *Sender,
      TCloseAction &Action)
{
  if (!m_Modal)
  {
    if (m_ChooseModeModalResult == mrNone) // Handle the case where the User has clicked the 'X' botton. (Close button)
      IDontAcceptRadio->Checked = true;

  //  Inform the CContainer about cartridge new status.
    switch (m_DlgType)
    {
      // All of the following Dialog types have only an 'OK' button.
      // (or have other options also, that are grayed-out) - Send a Dialog Result: 'OK'
      // (They don't let the user to choose to continue printing.)
      case FE_TANK_ID_NOTICE_DLG_UNSUPPORTED_RESIN:
      case FE_TANK_ID_NOTICE_DLG_RESIN_EXPIRED:
      case FE_TANK_ID_NOTICE_DLG_SUPPORT_IN_MODEL_MISPLACE:
      case FE_TANK_ID_NOTICE_DLG_MODEL_IN_SUPPORT_MISPLACE:
      case FE_TANK_ID_NOTICE_DLG_PIPE_NO_MATCH_MSPRINTING:      
      case FE_TANK_ID_NOTICE_DLG_MACHINE_NO_MATCH_MSPRINTING:
#ifdef OBJET_MACHINE
	  case FE_TANK_ID_NOTICE_DLG_CHECK_CONTAINER_REFILL:
	  case FE_TANK_ID_NOTICE_DLG_CALIBRATION_PROBLEM:
	  case FE_TANK_ID_NOTICE_DLG_INVALID_TAG_VERSION:
	  case FE_TANK_ID_MATERIAL_CONSUMPTION_PROBLEM:
	  case FE_TANK_ID_NOTICE_DLG_READING_TOO_LOW:
#endif
	  case FE_TANK_ID_NOTICE_DLG_AUTHENTICATION_FAILED:
	  case FE_TANK_ID_NOTICE_DLG_OVERCONSUMPTION:
        m_BackEndInterface->HandleTankIdentificationDlgClose(m_Cartridge,
                                                             FE_TANK_ID_NOTICE_DLG_RESULT_OK);
        break;

      // All of the following Dialogs let the user choose to continue printing with the inserted Tank:
      default:
        if (IDontAcceptRadio->Checked == true)
        {
          m_BackEndInterface->HandleTankIdentificationDlgClose(m_Cartridge,
                                                               FE_TANK_ID_NOTICE_DLG_RESULT_DISABLE_TANK);
        } else
        if (IAcceptRadio->Checked == true)
        {
          if (FE_TANK_ID_NOTICE_DLG_NO_TAG == m_DlgType)
          {
              m_BackEndInterface->HandleTankIdentificationDlgClose(m_Cartridge,
                                                               FE_TANK_ID_NOTICE_DLG_RESULT_ENABLE_TANK_NO_TAG);
          }
          else
          {
            switch (m_ChooseModeModalResult)
            {
              case mrSingleMode:
                m_BackEndInterface->HandleTankIdentificationDlgClose(m_Cartridge,
                                                               FE_TANK_ID_NOTICE_DLG_RESULT_ENABLE_TANK_SINGLEMODE);
              break;

              case mrDMMode:
                m_BackEndInterface->HandleTankIdentificationDlgClose(m_Cartridge,
                                                               FE_TANK_ID_NOTICE_DLG_RESULT_ENABLE_TANK_DMMODE);
              break;
            }
            }
          } else
        if (RunResinReplacementRadio->Checked == true)
        {
          m_BackEndInterface->HandleTankIdentificationDlgClose(m_Cartridge,
                                                               FE_TANK_ID_NOTICE_DLG_RESULT_DISABLE_TANK);
        }
        break;
    }
	
	// Do Silent MRW via the MRW Wizard
	if ((IAcceptRadio->Checked == true) && (FE_TANK_ID_NOTICE_DLG_NO_TAG != m_DlgType))
	{
		CAppParams* ParamsMgr = CAppParams::Instance();

	// First, prepare the parameters used by the MRW, then launch the MRW.

    // If we're MRWing a support Tank, don't change RR_SelectedOperationMode.
    if (!m_BackEndInterface->IsChambersTank(TYPE_CHAMBER_SUPPORT, m_Cartridge))
    {
			switch (m_ChooseModeModalResult)
			{
			  case mrSingleMode:
				ParamsMgr->MRW_SelectedOperationMode = SINGLE_MATERIAL_OPERATION_MODE;
				break;
			  case mrDMMode:
				ParamsMgr->MRW_SelectedOperationMode = DIGITAL_MATERIAL_OPERATION_MODE;
				break;
			}
			ParamsMgr->SaveSingleParameter(&ParamsMgr->MRW_SelectedOperationMode);
    }

	// Clear the MR_TanksOptionArray parameter, and set only the Current Tank (m_Cartridge) - to be MRW'ed
		for(int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; i++)
    {
			ParamsMgr->MRW_TanksOptionArray[i] = false; // reset.

      // Note that it is needed for the Silent MRW to have an updated MR_NewResinArray information regarding ALL tanks, since sometimes even if only one tanks is MRW'ed, decisions are made according to the other Tank also. 
		  ParamsMgr->MRW_NewResinArray[i] = m_BackEndInterface->GetTankMaterialType(static_cast<TTankIndex>(i));
    }

		ParamsMgr->MRW_TanksOptionArray[m_Cartridge] = true;
		ParamsMgr->SaveSingleParameter(&ParamsMgr->MRW_TanksOptionArray);

	// Set the Resin type we are going to MRW to:
		ParamsMgr->SaveSingleParameter(&ParamsMgr->MRW_NewResinArray);

	// Launch the MRW Silently
		FrontEndInterface->UpdateStatus(FEOP_RUN_MRW_WIZARD_SILENTLY,   FE_SILENT_MRW_DONT_UPDATE_SEGMENTS, true /*force*/);
	}
	
    if (RunResinReplacementRadio->Checked == true)
    {
	// Launch the MRW 	
		FrontEndInterface->UpdateStatus(FEOP_RUN_MRW_WIZARD);
	}

  }
}
//---------------------------------------------------------------------------

void __fastcall TTankIDNotice::IDontAcceptRadioClick(TObject *Sender)
{
  FormUILogic();
}
//---------------------------------------------------------------------------

void __fastcall TTankIDNotice::IAcceptRadioClick(TObject *Sender)
{
  FormUILogic();
}
//---------------------------------------------------------------------------


void TTankIDNotice::FormUILogic()
{
/*
  if ((NameEdit->Text == "") && (IAcceptRadio->Checked == true))
    OKButton->Enabled = false;
  else
    OKButton->Enabled = true;
*/
}


void __fastcall TTankIDNotice::FormCreate(TObject *Sender)
{
  // Remember the GUI main thread
  m_GUIMainThread = GetCurrentThreadId();
}
//---------------------------------------------------------------------------

// Class TTankIDNoticeTester
TTankIDNoticeTester::TTankIDNoticeTester()
: CQComponent ("TTankIDNoticeTester")
{
 INIT_METHOD(TTankIDNoticeTester,TankIDNoticeOpenDialog);
}

int TTankIDNoticeTester::TankIDNoticeOpenDialog(int DialogType,int /* TTankIndex */ Cartridge)
{
  return TTankIDNotice::ShowDialog(DialogType, static_cast<TTankIndex>(Cartridge), false);
}




void __fastcall TTankIDNotice::HelpButtonClick(TObject *Sender)
{
  QString FileName = Q2RTApplication->AppFilePath.Value() + "\\Help\\RFID\\";

  switch(m_DlgType)
  {
   case FE_TANK_ID_NOTICE_DLG_PIPE_NO_MATCH:
     FileName += RF_HELP_FILE_1_NAME;
     break;

   case FE_TANK_ID_NOTICE_DLG_MACHINE_NO_MATCH:
     FileName += RF_HELP_FILE_3_NAME;
     break;

   case FE_TANK_ID_NOTICE_DLG_PIPE_NO_MATCH_MSPRINTING:
     FileName += RF_HELP_FILE_2_NAME;
     break;

   case FE_TANK_ID_NOTICE_DLG_MACHINE_NO_MATCH_MSPRINTING:
     FileName += RF_HELP_FILE_4_NAME;
     break;

   case FE_TANK_ID_NOTICE_DLG_UNSUPPORTED_RESIN:
     FileName += RF_HELP_FILE_5_NAME;
     break;

   case FE_TANK_ID_NOTICE_DLG_MODEL_IN_SUPPORT_MISPLACE:
     FileName += RF_HELP_FILE_6_NAME;
     break;

   case FE_TANK_ID_NOTICE_DLG_SUPPORT_IN_MODEL_MISPLACE:
     FileName += RF_HELP_FILE_7_NAME;
     break;

   case FE_TANK_ID_NOTICE_DLG_RESIN_EXPIRED:
     FileName += RF_HELP_FILE_8_NAME;
     break;

   case FE_TANK_ID_NOTICE_DLG_NO_TAG:
     FileName += RF_HELP_FILE_9_NAME;   
     break;

   default:
     break; // Help file not available
  }

  ExternalAppExecute(FileName.c_str(),"");
}
//---------------------------------------------------------------------------

QString TTankIDNotice::GetNoticeText(int Cartridge)
{
  HANDLE hdl = NULL;
  TTankIDNotice* CurrentForm = NULL;

  TTankIDNotice::GetDialog(Cartridge, &CurrentForm, &hdl);

  if (!CurrentForm)
    return QString("");

  return CurrentForm->m_dlgText;
}
//---------------------------------------------------------------------------
void __fastcall TTankIDNotice::FormActivate(TObject *Sender)
{
  // If Another ChooseSilentMRWModeDLG Dialog was ALREADY launched (by another Tank's TTankIdNotice dialog), Then: User must first close the former, and only then he may open a new one. (i.e: We don't allow the user to have simultaniously TWO ChooseSilentMRWModeDLG dialogs.)
  //  - FormActivate is called when this form gets Focus. When this occures, we give the focus back to the ChooseSilentMRWModeDLG.
  if (!m_BackEndInterface->IsChambersTank(TYPE_CHAMBER_SUPPORT, m_Cartridge))
    if (ChooseSilentMRWModeDLG->Visible)
      ChooseSilentMRWModeDLG->ConfirmDialog();

    //Invalidate(); // SetFocus(); could also use: ChooseSilentMRWModeDLG->SetFocus() here.
    //ChooseSilentMRWModeDLG->SetFocus(); // Invalidate(); could also use: Invalidate() here.
}
//---------------------------------------------------------------------------

