
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "AppLogFile.h"
#include "AppParams.h"
#include "BackEndInterface.h"
#include "Configuration.h"
#include "DoorDlg.h"
#include "EdenPCISys.h"
#include "FrontEnd.h"
#include "FrontEndControlIDs.h"
#include "FrontEndParams.h"
#include "MotorDefs.h"
#include "MotorsControlDlg.h"
#include "Q2RTApplication.h"
#include "QErrors.h"
#include "QMonitor.h"
#include "QThreadUtils.h"
#include "FEResources.h"
#include <limits>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZAxisFrame"
#pragma link "XYAxisFrame"
#pragma resource "*.dfm"

TMotorsControlForm *MotorsControlForm;

const long Y_AXIS_MOVE_SIZE = 500;

const bool LOCK   = true;
const bool UNLOCK = false;

const long RCSpeedLookup[6] = {X_AXIS_SPEED,X_AXIS_SPEED,Y_AXIS_SPEED,Y_AXIS_SPEED,Z_AXIS_SPEED,Z_AXIS_SPEED};
const int  RCAxesLookup[6] = {AXIS_X,AXIS_X,AXIS_Y,AXIS_Y,AXIS_Z,AXIS_Z};
//---------------------------------------------------------------------------
__fastcall TMotorsControlForm::TMotorsControlForm(TComponent* Owner)
  : TForm(Owner)
{
  memset(m_RCPositionLookup, 0, sizeof(m_RCPositionLookup));
  m_OldMoveSpeed = 0;
  m_OldMotorStatus = 0;
  m_OldYMotorStatus = 0;
  m_OldYMoveSpeed = 0;
  m_ContinueYMoving = false;
  m_StopMoving = false;
  m_DontPerformMouseUp = false;

  m_MotorLimitLeftLedLastState = -1;
  m_MotorLimitRightLedLastState = -1;
  m_TabSheetLastState = 0;
  LOAD_BITMAP(LockDoorButton->Glyph,IDB_UNLOCK_SMALL);
  TZAxisFrame1->ZSlowModeCheckBox->Checked = SlowModeCheckBox->Checked;
  TXYAxisFrame1->XYSlowModeCheckBox->Checked = SlowModeCheckBox->Checked;  
}
//---------------------------------------------------------------------------

void __fastcall TMotorsControlForm::EnableCheckBoxClick(TObject *Sender)
{
  TCheckBox *EnableCheckBox = dynamic_cast<TCheckBox *>(Sender);

  bool Enable = EnableCheckBox->Checked;
  TMotorAxis Axis = static_cast<TMotorAxis>(EnableCheckBox->Parent->Tag);
  CBackEndInterface* BackEnd = CBackEndInterface::Instance();
  try
  {
    if(Enable != BackEnd->IsMotorEnabled(Axis))
    {
       if(Enable)
          CloseDoor(LOCK);

       BackEnd->EnableMotor(Enable, Axis);
    }
    // find all the controls on this tab and enable/disable them
    if(!Enable)
       EnableControls(EnableCheckBox->Parent, Enable);
    else
       {
       switch (Axis)
          {
          case AXIS_X: XHomeButton->Enabled = true;break;
          case AXIS_Y: YHomeButton->Enabled = true;break;
          case AXIS_Z: ZHomeButton->Enabled = true;break;
          case AXIS_T: THomeButton->Enabled = true;break;
          default:
             break;
          }
       }
  }
  catch(EQException& Exception)
  {
    EnableControls(EnableCheckBox->Parent, false);
    EnableCheckBox->Checked = false;
    QMonitor.ErrorMessage(Exception.GetErrorMsg());
    return;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMotorsControlForm::HomeButtonClick(TObject *Sender)
{
  TButton *HomeButton = dynamic_cast<TButton *>(Sender);
  TMotorAxis Axis = static_cast<TMotorAxis>(HomeButton->Parent->Tag);
  CBackEndInterface* BackEnd = CBackEndInterface::Instance();

  try
  {
     CloseDoor(LOCK);

     if(BackEnd->IsMotorMoving(Axis))
     {
        QMonitor.ErrorMessage("Motor is moving");
        return;
     }

     m_StopMoving = false;
     BackEnd->SetMotorParameters(Axis);
     BackEnd->GoToMotorHomePosition(Axis,false);

   EnableControls(HomeButton->Parent,true);
  }
  catch(EQException& Exception)
  {
    QMonitor.ErrorMessage(Exception.GetErrorMsg());
    return;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMotorsControlForm::KillMotionButtonClick(TObject *Sender)
{
  TButton *KillButton = dynamic_cast<TButton *>(Sender);

  TMotorAxis Axis = static_cast<TMotorAxis>(KillButton->Parent->Tag);
  CBackEndInterface* BackEnd = CBackEndInterface::Instance();

  try
  {
    m_StopMoving=true;
    BackEnd->KillMotorMotion(Axis);

    if(Axis==AXIS_X)
      EncoderButtonClick(Sender);
  }
  catch(EQException& Exception)
  {
    QMonitor.ErrorMessage(Exception.GetErrorMsg());
  }

}
//---------------------------------------------------------------------------


void __fastcall TMotorsControlForm::XMoveButtonClick(TObject *Sender)
{
  try
  {
    CBackEndInterface* BackEnd = CBackEndInterface::Instance();
    if(BackEnd->IsMotorMoving(AXIS_X))
    {
      QMonitor.ErrorMessage("Motor is moving");
      return;
    }

    int Position;
    try
    {
      Position = UnitsStrToSteps(AXIS_X,XMoveEdit->Text,m_CurrUnits);
    }
    catch (Exception &exception)
    {
      throw EQException("The value entered is not valid");
    }
    BackEnd->SetMotorParameters(AXIS_X);
    if (XAbsValueCheckBox->Checked)
      BackEnd->MoveMotorToAbsolutePosition(AXIS_X,Position,true);
    else
      BackEnd->MoveMotorRelative(AXIS_X,Position,true);

    EncoderButtonClick(Sender);
  }
  catch(EQException& Exception)
  {
    QMonitor.ErrorMessage(Exception.GetErrorMsg());
  }

}
//---------------------------------------------------------------------------

void __fastcall TMotorsControlForm::ShowRemoteControlActionExecute(TObject *Sender)
{
  SlowModeCheckBox->Checked = false;
  Width = Bevel1->Left + Bevel2->Left + Bevel2->Width + 6;
  RemoteControlButton->Action = HideRemoteControlAction;
}

//---------------------------------------------------------------------------

void __fastcall TMotorsControlForm::HideRemoteControlActionExecute(TObject *Sender)
{
  Width = Bevel1->Left * 2 + Bevel1->Width + 6;
  RemoteControlButton->Action = ShowRemoteControlAction;
}
//---------------------------------------------------------------------------

void __fastcall TMotorsControlForm::FormShow(TObject *Sender)
{
  CBackEndInterface* BackEnd = CBackEndInterface::Instance();
  CFrontEndParams *FrontEndParams = CFrontEndParams::Instance();

  AlwaysOnTopAction->Checked = (FormStyle == fsStayOnTop);
  SlowModeCheckBox->Checked = false;
  LockDoorButton->Down = false;
  // Enable timer events (for continuously updating Motor Limits)
  Timer1->Enabled = true;

  // Display the dialog in the last position
  if (FrontEndParams->MotorsDlgLeftPosition != -1)
    Left = FrontEndParams->MotorsDlgLeftPosition;
  else
    Left = (Screen->Width - Width) / 2;

  if (FrontEndParams->MotorsDlgTopPosition != -1)
    Top = FrontEndParams->MotorsDlgTopPosition;
  else
    Top = (Screen->Height - Height) / 2;

  HideRemoteControlAction->Execute();

  BackEnd->GetMotorsMinAndMaxPositions(m_RCPositionLookup);

  // Enables/disables the motor control according to the current state of the motor
  XEnableCheckBox->OnClick = NULL;
  YEnableCheckBox->OnClick = NULL;
  ZEnableCheckBox->OnClick = NULL;
  TEnableCheckBox->OnClick = NULL;

  bool Enable = BackEnd->IsMotorEnabled(AXIS_X);
  bool HomeDone = BackEnd->IsMotorHomeDone(AXIS_X);
  XEnableCheckBox->Checked = Enable;
  EnableControls(XAxisTabSheet,HomeDone);
  XHomeButton->Enabled = Enable;
  XPositionPanel->Color = ((HomeDone) ? clBtnFace : clRed);
  XPositionPanel->Caption = StepsToUnitsStr(AXIS_X,BackEnd->GetMotorsPosition(AXIS_X),m_CurrUnits);

  TXYAxisFrame1->XLeftSpeedButton->Enabled  = BackEnd->IsMotorHomeEverDone(AXIS_X);
  TXYAxisFrame1->XRightSpeedButton->Enabled = BackEnd->IsMotorHomeEverDone(AXIS_X);

  Enable = BackEnd->IsMotorEnabled(AXIS_Y);
  HomeDone = BackEnd->IsMotorHomeDone(AXIS_Y);
  YEnableCheckBox->Checked = Enable;
  EnableControls(YAxisTabSheet,HomeDone);
  YHomeButton->Enabled = Enable;
  YPositionPanel->Color = ((HomeDone) ? clBtnFace : clRed);

  YPositionPanel->Caption = StepsToUnitsStr(AXIS_Y,BackEnd->GetMotorsPosition(AXIS_Y),m_CurrUnits);

  TXYAxisFrame1->YUpSpeedButton->Enabled   = BackEnd->IsMotorHomeEverDone(AXIS_Y);
  TXYAxisFrame1->YDownSpeedButton->Enabled = BackEnd->IsMotorHomeEverDone(AXIS_Y);

  Enable = BackEnd->IsMotorEnabled(AXIS_Z);
  HomeDone = BackEnd->IsMotorHomeDone(AXIS_Z);
  ZEnableCheckBox->Checked = Enable;
  EnableControls(ZAxisTabSheet,HomeDone);
  ZHomeButton->Enabled = Enable;
  ZPositionPanel->Color = ((HomeDone) ? clBtnFace : clRed);
  ZPositionPanel->Caption = StepsToUnitsStr(AXIS_Z,BackEnd->GetMotorsPosition(AXIS_Z),m_CurrUnits);

  TZAxisFrame1->ZUpSpeedButton->Enabled   = BackEnd->IsMotorHomeEverDone(AXIS_Z);
  TZAxisFrame1->ZDownSpeedButton->Enabled = BackEnd->IsMotorHomeEverDone(AXIS_Z);

  Enable = BackEnd->IsMotorEnabled(AXIS_T);
  HomeDone = BackEnd->IsMotorHomeDone(AXIS_T);
  TEnableCheckBox->Checked = Enable;
  EnableControls(TAxisTabSheet,HomeDone);
  THomeButton->Enabled = Enable;
  if(HomeDone)
     TPositionPanel->Color = clBtnFace;
  else
     TPositionPanel->Color = clRed;

  TPositionPanel->Caption = StepsToUnitsStr(AXIS_T,BackEnd->GetMotorsPosition(AXIS_T),m_CurrUnits);

  XEnableCheckBox->OnClick = EnableCheckBoxClick;
  YEnableCheckBox->OnClick = EnableCheckBoxClick;
  ZEnableCheckBox->OnClick = EnableCheckBoxClick;
  TEnableCheckBox->OnClick = EnableCheckBoxClick;

  long ZStartPosInSteps = CONFIG_ConvertUnitsToStep(AXIS_Z,CAppParams::Instance()->Z_StartPrintPosition,muMM);
  ZSaveStartEdit->Text = StepsToUnitsStr(AXIS_Z,ZStartPosInSteps,m_CurrUnits);
}
//---------------------------------------------------------------------------

void __fastcall TMotorsControlForm::CloseBitBtnClick(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TMotorsControlForm::YMoveButtonClick(TObject *Sender)
{
  try
  {
    CBackEndInterface* BackEnd = CBackEndInterface::Instance();
    int Position;
    try
       {
       if(BackEnd->IsMotorMoving(AXIS_Y))
          {
          QMonitor.ErrorMessage("Motor is moving");
          return;
          }
      Position = UnitsStrToSteps(AXIS_Y,YMoveEdit->Text,m_CurrUnits);
      }
    catch (Exception &exception)
    {
      throw EQException("The value entered is not valid");
    }
    BackEnd->SetMotorParameters(AXIS_Y);
    if (YAbsValueCheckBox->Checked)
      BackEnd->MoveMotorToAbsolutePosition(AXIS_Y,Position);
    else
      BackEnd->MoveMotorRelative(AXIS_Y,Position);

  }
  catch(EQException& Exception)
  {
    QMonitor.ErrorMessage(Exception.GetErrorMsg());
  }

}
//---------------------------------------------------------------------------

void __fastcall TMotorsControlForm::ZMoveButtonClick(TObject *Sender)
{
  try
  {
    CBackEndInterface* BackEnd = CBackEndInterface::Instance();
    if(BackEnd->IsMotorMoving(AXIS_Z))
    {
      QMonitor.ErrorMessage("Motor is moving");
      return;
    }

    int Position;
    try
    {
      Position = UnitsStrToSteps(AXIS_Z,ZMoveEdit->Text,m_CurrUnits);
    }
    catch (Exception &exception)
    {
      throw EQException("The value entered is not valid");
    }
    BackEnd->SetMotorParameters(AXIS_Z);
    if (ZAbsValueCheckBox->Checked)
      BackEnd->MoveMotorToAbsolutePosition(AXIS_Z,Position);
    else
      BackEnd->MoveMotorRelative(AXIS_Z,Position);

  }
  catch(EQException& Exception)
  {
    QMonitor.ErrorMessage(Exception.GetErrorMsg());
  }

}
//---------------------------------------------------------------------------

void __fastcall TMotorsControlForm::TMoveButtonClick(TObject *Sender)
{
  try
  {
    CBackEndInterface* BackEnd = CBackEndInterface::Instance();
    int Position;
    try
    {
      Position = UnitsStrToSteps(AXIS_T,TMoveEdit->Text,m_CurrUnits);
    }
    catch (Exception &exception)
    {
      throw EQException("The value entered is not valid");
    }
    BackEnd->SetMotorParameters(AXIS_T);
    if (TAbsValueCheckBox->Checked)
      BackEnd->MoveMotorToAbsolutePosition(AXIS_T,Position);
    else
      BackEnd->MoveMotorRelative(AXIS_T,Position);

  }
  catch(EQException& Exception)
  {
    QMonitor.ErrorMessage(Exception.GetErrorMsg());
  }

}

void __fastcall TMotorsControlForm::ZStartButtonClick(TObject *Sender)
{
  CBackEndInterface* BackEnd = CBackEndInterface::Instance();
  CAppParams *ParamsMgr = CAppParams::Instance();

  try
  {
    if(BackEnd->IsMotorMoving(AXIS_Z))
    {
      QMonitor.ErrorMessage("Motor is already moving");
      return;
    }

    long ActualZEdit = UnitsStrToSteps(AXIS_Z,ZSaveStartEdit->Text,m_CurrUnits);
    long ZStartPosInSteps = CONFIG_ConvertUnitsToStep(AXIS_Z,ParamsMgr->Z_StartPrintPosition,muMM);

    AnsiString ZStartPosStr = StepsToUnitsStr(AXIS_Z,ZStartPosInSteps,m_CurrUnits);

    if(ActualZEdit != ZStartPosInSteps)
       if(!QMonitor.AskYesNo(QFormatStr("The saved Z Start Print Position is %s.\r\nContinue moving to the saved position?",
                             ZStartPosStr.c_str())))
       {
         ZSaveStartEdit->Text = ZStartPosStr;
         return;
       }

    ZSaveStartEdit->Text = ZStartPosStr;
    BackEnd->SetMotorParameters(AXIS_Z);
    BackEnd->MoveMotorToZStartPosition();
  }
  catch(EQException& Exception)
  {
    QMonitor.ErrorMessage(Exception.GetErrorMsg());
  }
}

//--------------------------------------------------------------------------
void __fastcall TMotorsControlForm::SaveButtonClick(TObject *Sender)
{
  CAppParams *ParamsMgr = CAppParams::Instance();

  try
  {
    long Position = UnitsStrToSteps(AXIS_Z,ZSaveStartEdit->Text,m_CurrUnits);

    ParamsMgr->Z_StartPrintPosition = CONFIG_ConvertStepToUnits(AXIS_Z,Position,muMM);
    ParamsMgr->SaveSingleParameter(&ParamsMgr->Z_StartPrintPosition);
  }
  catch(EQException& Exception)
  {
    QMonitor.ErrorMessage(Exception.GetErrorMsg());
  }
}

//---------------------------------------------------------------------------


void __fastcall TMotorsControlForm::TrayOutButtonClick(TObject *Sender)
{
  CBackEndInterface* BackEnd = CBackEndInterface::Instance();

  try
  {
    if(BackEnd->IsMotorMoving(AXIS_Z))
    {
      QMonitor.ErrorMessage("Motor is moving");
      return;
    }
    BackEnd->SetMotorParameters(AXIS_Z);
    BackEnd->MoveMotorToZTrayOutPosition();
  }
  catch(EQException& Exception)
  {
    QMonitor.ErrorMessage(Exception.GetErrorMsg());
  }
  
}
//---------------------------------------------------------------------------

// Update status on the frame (integer version)
void TMotorsControlForm::UpdateStatus(int ControlID,int Status)
{
  PostMessage(Handle,WM_STATUS_UPDATE,ControlID,Status);
}

// Update status on the frame (float version)
void TMotorsControlForm::UpdateStatus(int ControlID,float Status)
{
  PostMessage(Handle,WM_STATUS_UPDATE,ControlID,*((int *)&Status));
}

// Update status on the frame (string version)
void TMotorsControlForm::UpdateStatus(int ControlID,QString Status)
{
  PostMessage(Handle,WM_STATUS_UPDATE,ControlID,reinterpret_cast<int>(strdup(Status.c_str())));
}

void TMotorsControlForm::CurrentAxisPositionHandle(TMotorAxis Axis, TWinControl* AxisTabSheet, TButton* HomeButton, TPanel* PositionPanel, TCheckBox* EnableCheckBox, bool Enable)
{
   TNotifyEvent TmpEvent = EnableCheckBox->OnClick;
   EnableCheckBox->OnClick = NULL;
   EnableCheckBox->Checked = Enable;
   EnableCheckBox->OnClick = TmpEvent;
   EnableCheckBox->Checked = Enable;
   if(Enable)
   {
      if(!CBackEndInterface::Instance()->IsMotorHomeDone(Axis))
         EnableControls(AxisTabSheet,false);
      HomeButton->Enabled = true;
   }
   else
   {
      EnableControls(AxisTabSheet,false);
      PositionPanel->Color = clRed;
   }
}

void TMotorsControlForm::AxisHomeDoneHandle(TMotorAxis Axis, TWinControl* AxisTabSheet, TButton* HomeButton, TPanel* PositionPanel, TCheckBox* EnableCheckBox, bool HomeDone, TSpeedButton* Button1, TSpeedButton* Button2)
{
  if(HomeDone)
  {
     PositionPanel->Color = clBtnFace;
     EnableControls(AxisTabSheet,true);
     if (Button1)
        Button1->Enabled = true;
     if (Button2)
        Button2->Enabled = true;
  }
  else
  {
     PositionPanel->Color = clRed;
     EnableControls(AxisTabSheet,false);
     if(CBackEndInterface::Instance()->IsMotorEnabled(Axis))
        HomeButton->Enabled = true;
  }
}

// Message handler for the WM_STATUS_UPDATE message
void TMotorsControlForm::HandleStatusUpdateMessage(TMessage &Message)
{
  switch(Message.WParam)
  {
    case FE_CURRENT_X_AXIS_POSITION:
         XPositionPanel->Caption = StepsToUnitsStr(AXIS_X,Message.LParam,m_CurrUnits);
         break;

    case FE_CURRENT_Y_AXIS_POSITION:
         YPositionPanel->Caption = StepsToUnitsStr(AXIS_Y,Message.LParam,m_CurrUnits);
         break;

    case FE_CURRENT_Z_AXIS_POSITION:
         ZPositionPanel->Caption = StepsToUnitsStr(AXIS_Z,Message.LParam,m_CurrUnits);
         break;

    case FE_CURRENT_T_AXIS_POSITION:
         TPositionPanel->Caption = StepsToUnitsStr(AXIS_T,Message.LParam,m_CurrUnits);
         break;

    case FE_MOTOR_X_AXIS_ENABLE:
         CurrentAxisPositionHandle(AXIS_X, XAxisTabSheet, XHomeButton, XPositionPanel, XEnableCheckBox, bool(Message.LParam)/*Enable*/);
         break;

    case FE_MOTOR_Y_AXIS_ENABLE:
         CurrentAxisPositionHandle(AXIS_Y, YAxisTabSheet, YHomeButton, YPositionPanel, YEnableCheckBox, bool(Message.LParam)/*Enable*/);    
         break;

    case FE_MOTOR_Z_AXIS_ENABLE:
         CurrentAxisPositionHandle(AXIS_Z, ZAxisTabSheet, ZHomeButton, ZPositionPanel, ZEnableCheckBox, bool(Message.LParam)/*Enable*/);
         break;

    case FE_MOTOR_T_AXIS_ENABLE:
         CurrentAxisPositionHandle(AXIS_T, TAxisTabSheet, THomeButton, TPositionPanel, TEnableCheckBox, bool(Message.LParam)/*Enable*/);
         break;

    case FE_MOTOR_X_AXIS_HOME_DONE:
         AxisHomeDoneHandle(AXIS_X, XAxisTabSheet, XHomeButton, XPositionPanel, XEnableCheckBox, bool(Message.LParam)/*HomeDone*/, TXYAxisFrame1->XLeftSpeedButton, TXYAxisFrame1->XRightSpeedButton);
         break;

    case FE_MOTOR_Y_AXIS_HOME_DONE:
         AxisHomeDoneHandle(AXIS_Y, YAxisTabSheet, YHomeButton, YPositionPanel, YEnableCheckBox, bool(Message.LParam)/*HomeDone*/, TXYAxisFrame1->YUpSpeedButton, TXYAxisFrame1->YDownSpeedButton);
         break;

    case FE_MOTOR_Z_AXIS_HOME_DONE:
         AxisHomeDoneHandle(AXIS_Z, ZAxisTabSheet, ZHomeButton, ZPositionPanel, ZEnableCheckBox, bool(Message.LParam)/*HomeDone*/, TZAxisFrame1->ZUpSpeedButton, TZAxisFrame1->ZDownSpeedButton);
         break;

    case FE_MOTOR_T_AXIS_HOME_DONE:
         AxisHomeDoneHandle(AXIS_T, TAxisTabSheet, THomeButton, TPositionPanel, TEnableCheckBox, bool(Message.LParam)/*Enable*/);
         break;

    case FE_CURRENT_DOOR_STATUS:
    
         if (Message.LParam)
         {
            LOAD_BITMAP(LockDoorButton->Glyph,IDB_LOCK_SMALL);
            LockDoorButton->Action = UnlockDoorAction;
            LockDoorButton->Down = true;
         }
         else
         {
            LOAD_BITMAP(LockDoorButton->Glyph,IDB_UNLOCK_SMALL);
            LockDoorButton->Action = LockDoorAction;
            LockDoorButton->Down = false;
         }
         break;

    default:
      break;
  }
}
//---------------------------------------------------------------------------


// Enable/disable all the child controls (with tag = -1) on a control
void TMotorsControlForm::EnableControls(TWinControl *Parent, bool Enable)
{
  TControl *Control;

  for (int i = 0; i < Parent->ControlCount; i++)
  {
    Control = dynamic_cast<TControl *>(Parent->Controls[i]);
    if (Control != NULL && Control->Tag  == -1 )
      Control->Enabled = Enable;
  }
}

// Enable/disable all the child controls (with tag = -1) on a control
void TMotorsControlForm::MarkHomeIsNeeded(int Axis)
{}


void __fastcall TMotorsControlForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
  // Disable timer events (to stop continuously updating Motor Limits)
  Timer1->Enabled = false;

  CFrontEndParams *FrontEndParams = CFrontEndParams::Instance();
  CBackEndInterface* BackEnd = CBackEndInterface::Instance();

  FrontEndParams->MotorsDlgLeftPosition = Left;
  FrontEndParams->MotorsDlgTopPosition = Top;
  FrontEndParams->MotorsDlgAlwaysOnTop = (FormStyle == fsStayOnTop);

  //Verify if you want to disable T axis
  if(BackEnd->IsMotorEnabled(AXIS_T))
     BackEnd->DisableTAxisIfMachineIsInactive();
}
//---------------------------------------------------------------------------

void __fastcall TMotorsControlForm::XLoopButtonClick(TObject *Sender)
{
  try
  {
    CBackEndInterface* BackEnd = CBackEndInterface::Instance();
    long Speed = RCSpeedLookup[AXIS_X];

    // Save the previous speed
    m_OldMoveSpeed =  Speed;

    if(SlowModeCheckBox->Checked)
      Speed /= 3.0;

    if(BackEnd->IsMotorMoving(AXIS_X))
    {
      QMonitor.ErrorMessage("Motor is moving");
      return;
    }
    BackEnd->SetMotorParameters(AXIS_X);

    // Set the speed for this axis
    BackEnd->SetMotorSpeed(AXIS_X,Speed);

    // go to x minimum position
    BackEnd->MoveMotorToAbsolutePosition(AXIS_X, m_RCPositionLookup[0]);
    BackEnd->WaitForEndOfMovement(AXIS_X);

    BackEnd->JogMotor(AXIS_X,m_RCPositionLookup[1], numeric_limits<int>::max());
  }
  catch(EQException& Exception)
  {
    QMonitor.ErrorMessage(Exception.GetErrorMsg());
  }
}
//---------------------------------------------------------------------------


void __fastcall TMotorsControlForm::TWipePositionButtonClick(TObject *Sender)
{
    //send T axis to waste tank wipe position
  CBackEndInterface* BackEnd = CBackEndInterface::Instance();

  try
  {
    if(BackEnd->IsMotorMoving(AXIS_T))
    {
      QMonitor.ErrorMessage("Motor is moving");
      return;
    }

    BackEnd->SetMotorParameters(AXIS_T);
    BackEnd->MoveMotorToTWipePosition();
  }
  catch(EQException& Exception)
  {
    QMonitor.ErrorMessage(Exception.GetErrorMsg());
  }

}
//---------------------------------------------------------------------------


void __fastcall TMotorsControlForm::TPurgePositionButtonClick(TObject *Sender)
{
    //send T axis to waste tank open position
  CBackEndInterface* BackEnd = CBackEndInterface::Instance();

  try
  {
    if(BackEnd->IsMotorMoving(AXIS_T))
    {
      QMonitor.ErrorMessage("Motor is moving");
      return;
    }
    BackEnd->SetMotorParameters(AXIS_T);
    BackEnd->MoveMotorToTPurgePosition();
  }
  catch(EQException& Exception)
  {
    QMonitor.ErrorMessage(Exception.GetErrorMsg());
  }

}
//---------------------------------------------------------------------------

void __fastcall TMotorsControlForm::LockDoorActionExecute(TObject *Sender)
{
 try
  {
    CBackEndInterface::Instance()->LockDoor(LOCK);
	LockDoorButton->Glyph  = NULL; // Need to NULL it here in order to show it properly next time
	LockDoorButton->Action = UnlockDoorAction;
	LOAD_BITMAP(LockDoorButton->Glyph,IDB_LOCK_SMALL);
  }
  catch(EQException& Exception)
  {
    LockDoorButton->Down = false;
    QMonitor.ErrorMessage(Exception.GetErrorMsg());
  }       
}
//---------------------------------------------------------------------------

void __fastcall TMotorsControlForm::UnlockDoorActionExecute(TObject *Sender)
{
  CBackEndInterface* BackEnd = CBackEndInterface::Instance();

  try
  {
    BackEnd->LockDoor(UNLOCK);
	LockDoorButton->Glyph  = NULL;
	LockDoorButton->Action = LockDoorAction;
	LOAD_BITMAP(LockDoorButton->Glyph,IDB_UNLOCK_SMALL);
	BackEnd->EnableMotor(false, AXIS_ALL);
  }
  catch(EQException& Exception)
  {
    QMonitor.ErrorMessage(Exception.GetErrorMsg());
  }
}
//---------------------------------------------------------------------------

void __fastcall TMotorsControlForm::KillAllMotionButtonClick(TObject *Sender)
{
  CBackEndInterface* BackEnd = CBackEndInterface::Instance();

  try
  {
     for(int i=AXIS_X;i<MAX_AXIS;i++)
     {
        if(BackEnd->IsMotorEnabled(static_cast<TMotorAxis>(i))) //Check if is motor moving not enable
           BackEnd->KillMotorMotion(static_cast<TMotorAxis>(i));
     }
  }
  catch(EQException& Exception)
  {
    QMonitor.ErrorMessage(Exception.GetErrorMsg());
  }
}
//---------------------------------------------------------------------------

void __fastcall TMotorsControlForm::HomeAllButtonClick(TObject *Sender)
{
  

  CBackEndInterface* BackEnd = CBackEndInterface::Instance();

  try
	 {
	 bool AxisEnable[MAX_AXIS];

	 CloseDoor(LOCK);
	 for(int i = AXIS_X;i<MAX_AXIS;i++)
        {
        BackEnd->SetMotorParameters(i);
        AxisEnable[i] = BackEnd->IsMotorEnabled(static_cast<TMotorAxis>(i));

        if (!AxisEnable[i])
          BackEnd->EnableMotor(true,static_cast<TMotorAxis>(i));

        if(BackEnd->IsMotorMoving(static_cast<TMotorAxis>(i)))
          {
          QMonitor.ErrorMessage("Motor is moving");
          return;
          }
        }

     XEnableCheckBox->Checked = true;
     YEnableCheckBox->Checked = true;
     ZEnableCheckBox->Checked = true;
     TEnableCheckBox->Checked = true;

     BackEnd->GoToMotorHomePosition(AXIS_ALL,true);

     EnableControls(XAxisTabSheet,true);
     EnableControls(YAxisTabSheet,true);
     EnableControls(ZAxisTabSheet,true);
     EnableControls(TAxisTabSheet,true);
     }
  catch(EQException& Exception)
     {
     QMonitor.ErrorMessage(Exception.GetErrorMsg());
     }


}
//---------------------------------------------------------------------------

void __fastcall TMotorsControlForm::StopButtonClick(TObject *Sender)
{
  TButton *StopButton = dynamic_cast<TButton *>(Sender);

  TMotorAxis Axis = static_cast<TMotorAxis>(StopButton->Parent->Tag);
  CBackEndInterface* BackEnd = CBackEndInterface::Instance();

  try
  {
    m_StopMoving = true;
    BackEnd->StopMotorMotion(Axis);

    if(Axis==AXIS_X)
      EncoderButtonClick(Sender);
  }
  catch(EQException& Exception)
  {
    QMonitor.ErrorMessage(Exception.GetErrorMsg());
  }
}
//---------------------------------------------------------------------------

void __fastcall TMotorsControlForm::GoZDownButtonClick(TObject *Sender)
{
  CBackEndInterface* BackEnd = CBackEndInterface::Instance();

  try
  {
    if(BackEnd->IsMotorMoving(AXIS_Z))
       {
       QMonitor.ErrorMessage("Motor is moving");
       return;
       }
    BackEnd->SetMotorParameters(AXIS_Z);
    BackEnd->MoveZAxisDownToLowestPosition();
  }
  catch(EQException& Exception)
  {
    QMonitor.ErrorMessage(Exception.GetErrorMsg());
  }

}
//---------------------------------------------------------------------------




void __fastcall TMotorsControlForm::YButtonMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{

  CAppParams *ParamsMgr = CAppParams::Instance();
  CBackEndInterface* BackEnd = CBackEndInterface::Instance();
  if(ParamsMgr->OfficeProfessionalType==PROFESSIONAL_MACHINE) //runtime objet
  {
//#if defined EDEN_500 || defined EDEN_350 || defined EDEN_350_V || defined CONNEX_500 || defined CONNEX_350 || defined OBJET_500 || defined OBJET_350 || defined OBJET_1000 //runtime objet
  try
  {
    m_DontPerformMouseUp = false;

    TSpeedButton *SpeedButton = dynamic_cast<TSpeedButton *>(Sender);
    //CBackEndInterface* BackEnd = CBackEndInterface::Instance(); runtime objet

    if(!BackEnd->IsMotorHomeEverDone(AXIS_Y))
    {
      TXYAxisFrame1->YUpSpeedButton->Enabled = false;
      TXYAxisFrame1->YDownSpeedButton->Enabled = false;
      return;
    }

    // Find the speed and axis for the movement
    long Speed = RCSpeedLookup[SpeedButton->Tag];
    long Position = m_RCPositionLookup[SpeedButton->Tag];

    if(BackEnd->IsMotorMoving(AXIS_Y))
    {
       QMonitor.ErrorMessage("Motor is moving");
       return;
    }

    m_OldYMotorStatus = BackEnd->IsMotorEnabled(AXIS_Y);
    if (!m_OldYMotorStatus)
      BackEnd->EnableMotor(true,AXIS_Y);

    // Save the previous speed
    m_OldYMoveSpeed = Speed;

    // We're using the arrows - set speed to 25%
    Speed /= 4.0;

    if(SlowModeCheckBox->Checked)
      Speed /= 3.0;

    if(Shift.Contains(ssShift))
      Speed /= 4.0;

    // Set the speed for this axis
    BackEnd->SetMotorSpeed(AXIS_Y,Speed);

    // Start movement
    BackEnd->MoveMotorToAbsolutePosition(AXIS_Y,Position);
  }
  catch(EQException& Exception)
  {
    QMonitor.ErrorMessage(Exception.GetErrorMsg());
  }
  }
  else //office machine //runtime objet
  {
//#endif
//#if defined EDEN_260 || defined EDEN_250 || defined EDEN_260_V || defined CONNEX_260 || OBJET_260
  //CBackEndInterface* BackEnd = CBackEndInterface::Instance(); runtime objet

  if(!BackEnd->IsMotorHomeEverDone(AXIS_Y))
  {
    TXYAxisFrame1->YUpSpeedButton->Enabled = false;
    TXYAxisFrame1->YDownSpeedButton->Enabled = false;
    return;
  }

  if(BackEnd->IsMotorMoving(AXIS_Y))
  {
    QMonitor.ErrorMessage("Motor is moving");
    return;
  }

  TSpeedButton *SpeedButton = dynamic_cast<TSpeedButton *>(Sender);
  PostMessage(Handle,WM_MOVE_Y_AXIS,SpeedButton->Tag, Shift.Contains(ssShift) ? 1 : 0);
//#endif
  }
}
//---------------------------------------------------------------------------

void __fastcall TMotorsControlForm::YButtonMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  CAppParams *ParamsMgr = CAppParams::Instance();
  //CBackEndInterface* BackEnd = CBackEndInterface::Instance();
  if(ParamsMgr->OfficeProfessionalType==PROFESSIONAL_MACHINE) //runtime objet
  {
//#if defined EDEN_500 || defined EDEN_350 || defined EDEN_350_V || defined CONNEX_500 || defined CONNEX_350 || defined OBJET_500 || defined OBJET_350 || defined OBJET_1000
  try
  {
    if(m_DontPerformMouseUp)
       return;

    CBackEndInterface* BackEnd = CBackEndInterface::Instance();
    int SleepTime = 500;

    // kill current motion
    BackEnd->KillMotorMotion(AXIS_Y);

    Sleep(SleepTime);

    // Get the current position of the motors
    BackEnd->DisplayMotorsPosition();

    // Return to the previous settings of the motor (enabled, speed)
    BackEnd->SetMotorSpeed(AXIS_Y,m_OldYMoveSpeed);

    if (!m_OldYMotorStatus)
      BackEnd->EnableMotor(m_OldYMotorStatus,AXIS_Y);
  }
  catch(EQException& Exception)
  {
    QMonitor.ErrorMessage(Exception.GetErrorMsg());
  }
  }
  else   //office machine //runtime objet
  {
//#endif
//#if defined EDEN_260 || defined EDEN_250 || defined EDEN_260_V || defined CONNEX_260 || defined OBJET_260
    m_ContinueYMoving=false;
//#endif
  }
}
//---------------------------------------------------------------------------


void TMotorsControlForm::HandleMoveYAxisMessage(TMessage &Message)
{
  try
  {
    CBackEndInterface* BackEnd = CBackEndInterface::Instance();
       
    // Find the speed and axis for the movement
    long Speed = RCSpeedLookup[Message.WParam];
    long MaxOrMinPosition = m_RCPositionLookup[Message.WParam];

    m_OldYMotorStatus = BackEnd->IsMotorEnabled(AXIS_Y);
    if (!m_OldYMotorStatus)
      BackEnd->EnableMotor(true,AXIS_Y);

    // Save the previous speed
    m_OldYMoveSpeed =  Speed;

    if(SlowModeCheckBox->Checked)
      Speed /= 3.0;

    //botton Shift pressed
    if(Message.LParam)
      Speed /= 4.0;

    // Set the speed for this axis
    BackEnd->SetMotorSpeed(AXIS_Y,Speed);

    // Start movement
    m_ContinueYMoving=true;
    long Position;
    while(m_ContinueYMoving)
    {
      if(Message.WParam == 2)
      {
        Position = BackEnd->GetMotorsPosition(AXIS_Y) - Y_AXIS_MOVE_SIZE;
        if(Position < MaxOrMinPosition)
        {
          Position = MaxOrMinPosition;
          m_ContinueYMoving=false;
        }
      }
      else
      {
        Position = BackEnd->GetMotorsPosition(AXIS_Y) + Y_AXIS_MOVE_SIZE;
        if(Position > MaxOrMinPosition)
        {
          Position = MaxOrMinPosition;
          m_ContinueYMoving=false;
        }
      }

      Application->ProcessMessages();

      BackEnd->MoveMotorToAbsolutePosition(AXIS_Y,Position,true);
      Application->ProcessMessages();
     }

     // Return to the previous settings of the motor (enabled, speed)
     BackEnd->SetMotorSpeed(AXIS_Y,m_OldYMoveSpeed);

     if (!m_OldYMotorStatus)
        BackEnd->EnableMotor(m_OldYMotorStatus,AXIS_Y);
  }
  catch(EQException& Exception)
  {
    QMonitor.ErrorMessage(Exception.GetErrorMsg());
  }

}

void TMotorsControlForm::CloseDoor(bool Enable)
{
  CBackEndInterface* BackEnd = CBackEndInterface::Instance();

  if(BackEnd->EnableDoor(Enable) != Q_NO_ERROR)
    throw EQException(Enable ? "Failed to close door" : "Failed to open door"); 
}

void __fastcall TMotorsControlForm::EncoderButtonClick(TObject *Sender)
{
  BYTE Address = ENCODER_ADDRESS;

  try
  {
    if(CHECK_EMULATION(CAppParams::Instance()->MCB_Emulation) || CHECK_EMULATION(CAppParams::Instance()->OHDB_Emulation))
       {
       EncoderPanel->Caption = "Emulation";
       return;
       }

    EncoderPanel->Caption = IntToHex(CBackEndInterface::Instance()->ReadDataFromOHDBXilinx(Address),4);
  } catch(EQException& E)
    {
      QMonitor.Printf(E.GetErrorMsg().c_str());
    }

}
//---------------------------------------------------------------------------

void __fastcall TMotorsControlForm::AlwaysOnTopActionExecute(TObject *Sender)
{
  AlwaysOnTopAction->Checked = !AlwaysOnTopAction->Checked;

  // Temporarly disable the OnShow event
  TNotifyEvent TmpEvent = OnShow;
  OnShow = NULL;
  FormStyle = AlwaysOnTopAction->Checked ? fsStayOnTop : fsNormal;
  OnShow = TmpEvent;
}
//---------------------------------------------------------------------------

void __fastcall TMotorsControlForm::CloseDialogActionExecute(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------

AnsiString TMotorsControlForm::StepsToUnitsStr(TMotorAxis Axis,long PosUnitsInSteps,TMotorUnits Units)
{
  float Pos = CONFIG_ConvertStepToUnits(Axis,PosUnitsInSteps,Units);

  switch(Units)
  {
    case muMM:
      return Format("%.3f",ARRAYOFCONST((Pos)));

    case muInch:
      return Format("%.4f",ARRAYOFCONST((Pos)));

    default:
      return IntToStr((int)Pos);
  }
}

long TMotorsControlForm::UnitsStrToSteps(TMotorAxis Axis,AnsiString UnitsStr,TMotorUnits Units)
{
  float Pos;

  if(Units == muSteps)
    Pos = StrToInt(UnitsStr);
  else
    Pos = StrToFloat(UnitsStr);

  long PosInSteps = CONFIG_ConvertUnitsToStep(Axis,Pos,Units);

  return PosInSteps;
}

// Translate one type of units to another
float TMotorsControlForm::TranslateUnits(TMotorAxis Axis,TMotorUnits OldUnits,TMotorUnits NewUnits,float Value)
{
  // Convert current value to steps
  long PosInSteps = CONFIG_ConvertUnitsToStep(Axis,Value,OldUnits);
  return CONFIG_ConvertStepToUnits(Axis,PosInSteps,NewUnits);
}

void __fastcall TMotorsControlForm::UnitsComboBoxChange(TObject *Sender)
{
  TMotorUnits NewUnits = (TMotorUnits)UnitsComboBox->ItemIndex;

  try
  {
    // Convert all position indicators to new units
    AnsiString XPositionPanelCaption  = StepsToUnitsStr(AXIS_X,UnitsStrToSteps(AXIS_X,XPositionPanel->Caption,m_CurrUnits),NewUnits);
    AnsiString YPositionPanelCaption = StepsToUnitsStr(AXIS_Y,UnitsStrToSteps(AXIS_Y,YPositionPanel->Caption,m_CurrUnits),NewUnits);
    AnsiString ZPositionPanelCaption = StepsToUnitsStr(AXIS_Z,UnitsStrToSteps(AXIS_Z,ZPositionPanel->Caption,m_CurrUnits),NewUnits);
    AnsiString TPositionPanelCaption = StepsToUnitsStr(AXIS_T,UnitsStrToSteps(AXIS_T,TPositionPanel->Caption,m_CurrUnits),NewUnits);

    AnsiString XMoveEditText      = StepsToUnitsStr(AXIS_X,UnitsStrToSteps(AXIS_X,XMoveEdit->Text,m_CurrUnits),NewUnits);
    AnsiString YMoveEditText      = StepsToUnitsStr(AXIS_Y,UnitsStrToSteps(AXIS_Y,YMoveEdit->Text,m_CurrUnits),NewUnits);
    AnsiString ZMoveEditText      = StepsToUnitsStr(AXIS_Z,UnitsStrToSteps(AXIS_Z,ZMoveEdit->Text,m_CurrUnits),NewUnits);
    AnsiString TMoveEditText      = StepsToUnitsStr(AXIS_T,UnitsStrToSteps(AXIS_T,TMoveEdit->Text,m_CurrUnits),NewUnits);
    AnsiString ZSaveStartEditText = StepsToUnitsStr(AXIS_Z,UnitsStrToSteps(AXIS_Z,ZSaveStartEdit->Text,m_CurrUnits),NewUnits);

    // Assign new values
    XPositionPanel->Caption = XPositionPanelCaption; 
    YPositionPanel->Caption = YPositionPanelCaption;
    ZPositionPanel->Caption = ZPositionPanelCaption;
    TPositionPanel->Caption = TPositionPanelCaption;
    XMoveEdit->Text      = XMoveEditText;
    YMoveEdit->Text      = YMoveEditText;
    ZMoveEdit->Text      = ZMoveEditText;
    TMoveEdit->Text      = TMoveEditText;
    ZSaveStartEdit->Text = ZSaveStartEditText;

    m_CurrUnits = NewUnits;
  } catch(...)
    {
      // In case of an error, don't cahnge units
      MessageDlg("Invalid position value entered",mtError,TMsgDlgButtons() << mbOK,0);
      UnitsComboBox->ItemIndex = (int)m_CurrUnits;
    }
}
//---------------------------------------------------------------------------

void __fastcall TMotorsControlForm::FormCreate(TObject *Sender)
{
  m_CurrUnits = muSteps;

  // Disable timer events on create (stop continuously updating Motor Limits)
  Timer1->Enabled = false;

  TImage* LedsImageArray[8] = {Image1, Image2, Image3, Image4, Image5, Image6, Image7, Image8};

  for (int i=0; i<8; i++)
    LOAD_BITMAP(LedsImageArray[i]->Picture->Bitmap, IDB_GREY_LED);

  // cause first Invalidate(): (in case we start running within a Motor Limit)
  m_MotorLimitLeftLedLastState = -1;
  m_MotorLimitRightLedLastState = -1;
  m_TabSheetLastState = -1;
}

//---------------------------------------------------------------------------

void __fastcall TMotorsControlForm::Timer1Timer(TObject *Sender)
{

  TImage* LedsImageArray[8] = {Image1, Image2, Image3, Image4, Image5, Image6, Image7, Image8};
  TTabSheet* ActiveTabSheet = NULL;
  CBackEndInterface* BackEnd = CBackEndInterface::Instance();

  // Obtain the currenly active Tab and its tag-index:
  ActiveTabSheet = PageControl1->ActivePage;
  TMotorAxis Axis = static_cast<TMotorAxis>(ActiveTabSheet->Tag);

  BackEnd->MotorGetLimitsStatus(Axis);
  int LeftLimitStatus =  BackEnd->MotorGetAxisLeftLimit(Axis);
  int RightLimitStatus = BackEnd->MotorGetAxisRightLimit(Axis);
   
  if (m_MotorLimitLeftLedLastState != LeftLimitStatus) // limit state changed
    LOAD_BITMAP(LedsImageArray[Axis*2]->Picture->Bitmap, (LeftLimitStatus) ? IDB_RED_LED : IDB_GREY_LED);
  if (m_MotorLimitRightLedLastState != RightLimitStatus) // limit state changed
    LOAD_BITMAP(LedsImageArray[Axis*2+1]->Picture->Bitmap, (RightLimitStatus) ? IDB_RED_LED : IDB_GREY_LED);

  // Invalidate Led Bmp's if needed.
  // m_TabSheetLastState is needed only for the situation where X_Axis and Y_Axis have the same
  // state, X is updated and then we go to Y tab. Y shows the OLD state. So:
  if ((m_TabSheetLastState != Axis) || (m_MotorLimitLeftLedLastState != LeftLimitStatus))
    LedsImageArray[Axis*2]->Invalidate();

  if ((m_TabSheetLastState != Axis) || (m_MotorLimitRightLedLastState != RightLimitStatus))
    LedsImageArray[Axis*2 + 1]->Invalidate();

  m_MotorLimitLeftLedLastState = LeftLimitStatus;
  m_MotorLimitRightLedLastState = RightLimitStatus;
  m_TabSheetLastState = Axis;

  // Set the time interval to elapse until the next OnTimer event:
  Timer1->Interval = MOTOR_LIMIT_TIMER_INVERVAL;
}
//---------------------------------------------------------------------------

void __fastcall TMotorsControlForm::SlowModeCheckBoxClick(TObject *Sender)
{
  TZAxisFrame1->ZSlowModeCheckBox->Checked = SlowModeCheckBox->Checked;
  TXYAxisFrame1->XYSlowModeCheckBox->Checked = SlowModeCheckBox->Checked;  
}
//---------------------------------------------------------------------------


