//---------------------------------------------------------------------------

#ifndef MotorsControlDlgH
#define MotorsControlDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ActnList.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include "QTypes.h"
#include <ImgList.hpp>
#include <Menus.hpp>
#include "MotorDefs.h"
#include <Graphics.hpp>
#include "ZAxisFrame.h"
#include "XYAxisFrame.h"


// Define private windows message for status updates
#define WM_STATUS_UPDATE WM_USER
#define WM_MOVE_Y_AXIS WM_USER+1

#define MOTOR_LIMIT_TIMER_INVERVAL 500 // ms
//---------------------------------------------------------------------------
class TMotorsControlForm : public TForm
{
__published:	// IDE-managed Components
  TBevel *Bevel1;
  TBevel *Bevel2;
  TLabel *Label5;
  TPageControl *PageControl1;
        TTabSheet *XAxisTabSheet;
  TLabel *Label1;
  TCheckBox *XEnableCheckBox;
  TPanel *XPositionPanel;
  TButton *XHomeButton;
        TEdit *XMoveEdit;
  TCheckBox *XAbsValueCheckBox;
  TButton *XMoveButton;
  TButton *XKillMotionButton;
        TTabSheet *YAxisTabSheet;
  TLabel *Label6;
  TCheckBox *YEnableCheckBox;
  TPanel *YPositionPanel;
  TButton *YHomeButton;
  TButton *YKillMotionButton;
  TButton *YMoveButton;
  TEdit *YMoveEdit;
  TCheckBox *YAbsValueCheckBox;
        TTabSheet *ZAxisTabSheet;
  TLabel *Label7;
  TCheckBox *ZEnableCheckBox;
  TPanel *ZPositionPanel;
  TButton *ZHomeButton;
  TButton *ZKillMotionButton;
  TButton *ZMoveButton;
  TEdit *ZMoveEdit;
  TCheckBox *ZAbsValueCheckBox;
        TTabSheet *TAxisTabSheet;
  TLabel *Label8;
  TCheckBox *TEnableCheckBox;
  TPanel *TPositionPanel;
  TButton *THomeButton;
  TButton *TKillMotionButton;
  TButton *TMoveButton;
  TEdit *TMoveEdit;
  TCheckBox *TAbsValueCheckBox;
  TBitBtn *CloseBitBtn;
  TActionList *ActionList1;
  TAction *ShowRemoteControlAction;
  TButton *RemoteControlButton;
  TAction *HideRemoteControlAction;
  TButton *XLoopButton;
        TButton *TWipePositionButton;
        TButton *TPurgePositionButton;
        TButton *TrayOutButton;
        TAction *LockDoorAction;
        TAction *UnlockDoorAction;
        TSpeedButton *LockDoorButton;
        TButton *KillAllMotionButton;
        TButton *HomeAllButton;
        TButton *XStopButton;
        TButton *YStopButton;
        TButton *ZStopButton;
        TButton *TStopButton;
        TButton *GoZDownButton;
        TButton *EncoderButton;
        TPanel *EncoderPanel;
        TPopupMenu *PopupMenu1;
        TAction *AlwaysOnTopAction;
        TAction *CloseDialogAction;
        TMenuItem *Alwaysontop1;
        TMenuItem *N1;
        TMenuItem *Close1;
        TGroupBox *ZStartGroupBox;
        TButton *GoToZStartPositionButton1;
        TButton *SaveZStartPositionButton;
        TEdit *ZSaveStartEdit;
        TComboBox *UnitsComboBox;
        TLabel *Label9;
        TTimer *Timer1;
        TLabel *Label10;
        TImage *Image1;
        TLabel *Label11;
        TImage *Image3;
        TLabel *Label12;
        TImage *Image6;
        TLabel *Label13;
        TImage *Image7;
        TLabel *Label14;
        TImage *Image2;
        TLabel *Label15;
        TImage *Image4;
        TLabel *Label16;
        TImage *Image5;
        TLabel *Label17;
        TImage *Image8;
    TZAxisFrame *TZAxisFrame1;
    TXYAxisFrame *TXYAxisFrame1;
    TCheckBox *SlowModeCheckBox;
  void __fastcall FormShow(TObject *Sender);
  void __fastcall CloseBitBtnClick(TObject *Sender);
  void __fastcall EnableCheckBoxClick(TObject *Sender);
  void __fastcall HomeButtonClick(TObject *Sender);
  void __fastcall KillMotionButtonClick(TObject *Sender);
  void __fastcall XMoveButtonClick(TObject *Sender);
  void __fastcall ShowRemoteControlActionExecute(TObject *Sender);
  void __fastcall HideRemoteControlActionExecute(TObject *Sender);
  void __fastcall YMoveButtonClick(TObject *Sender);
  void __fastcall ZMoveButtonClick(TObject *Sender);
  void __fastcall TMoveButtonClick(TObject *Sender);
  void __fastcall ZStartButtonClick(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall XLoopButtonClick(TObject *Sender);
        void __fastcall TWipePositionButtonClick(TObject *Sender);
        void __fastcall TPurgePositionButtonClick(TObject *Sender);
        void __fastcall TrayOutButtonClick(TObject *Sender);
        void __fastcall LockDoorActionExecute(TObject *Sender);
        void __fastcall UnlockDoorActionExecute(TObject *Sender);
        void __fastcall KillAllMotionButtonClick(TObject *Sender);
        void __fastcall HomeAllButtonClick(TObject *Sender);
        void __fastcall StopButtonClick(TObject *Sender);
        void __fastcall GoZDownButtonClick(TObject *Sender);
        void __fastcall YButtonMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall YButtonMouseUp(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall EncoderButtonClick(TObject *Sender);
        void __fastcall AlwaysOnTopActionExecute(TObject *Sender);
        void __fastcall CloseDialogActionExecute(TObject *Sender);
        void __fastcall SaveButtonClick(TObject *Sender);
        void __fastcall UnitsComboBoxChange(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall SlowModeCheckBoxClick(TObject *Sender);

private:	// User declarations

  long m_OldMoveSpeed;
  bool m_OldMotorStatus;
  long m_RCPositionLookup[6];

  bool m_OldYMotorStatus;
  long m_OldYMoveSpeed;

  //For axis Y eden special moving
  bool m_ContinueYMoving;

  bool m_StopMoving;
  bool m_DontPerformMouseUp;

  int m_MotorLimitLeftLedLastState;
  int m_MotorLimitRightLedLastState;
  int m_TabSheetLastState;

  TMotorUnits m_CurrUnits;

  // Message handler for the WM_STATUS_UPDATE message
  void HandleStatusUpdateMessage(TMessage &Message);
  void AxisHomeDoneHandle(TMotorAxis Axis, TWinControl* AxisTabSheet, TButton* HomeButtn, TPanel* PositionPanel, TCheckBox* EnableCheckBox, bool HomeDone, TSpeedButton* Button1=NULL, TSpeedButton* Button2=NULL);
  void CurrentAxisPositionHandle(TMotorAxis Axis, TWinControl* AxisTabSheet, TButton* HomeButton, TPanel* PositionPanel, TCheckBox* EnableCheckBox, bool Enable);

  // Enable/disable all the child controls (with tag = -1) on a control
  void EnableControls(TWinControl *Parent, bool Enable);
  void MarkHomeIsNeeded(Axis);

  void HandleMoveYAxisMessage(TMessage &Message);

  void CloseDoor(bool);

  AnsiString StepsToUnitsStr(TMotorAxis Axis,long PosUnitsInSteps,TMotorUnits Units);
  long UnitsStrToSteps(TMotorAxis Axis,AnsiString UnitsStr,TMotorUnits Units);

  // Translate one type of units to another
  float TranslateUnits(TMotorAxis Axis,TMotorUnits OldUnits,TMotorUnits NewUnits,float Value);

public:		// User declarations
  __fastcall TMotorsControlForm(TComponent* Owner);

    // Update status on the frame (integer version)
  void UpdateStatus(int ControlID,int Status);

  // Update status on the frame (float version)
  void UpdateStatus(int ControlID,float Status);

  // Update status on the frame (string version)
  void UpdateStatus(int ControlID,QString Status);

BEGIN_MESSAGE_MAP
  MESSAGE_HANDLER(WM_STATUS_UPDATE,TMessage,HandleStatusUpdateMessage);
  MESSAGE_HANDLER(WM_MOVE_Y_AXIS,TMessage,HandleMoveYAxisMessage);
END_MESSAGE_MAP(TForm);
};
//---------------------------------------------------------------------------
extern PACKAGE TMotorsControlForm *MotorsControlForm;
//---------------------------------------------------------------------------
#endif
