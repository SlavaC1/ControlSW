//---------------------------------------------------------------------------

#ifndef ActuatorsControlDlgH
#define ActuatorsControlDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <ImgList.hpp>
#include "QTypes.h"
#include <ActnList.hpp>
#include <Menus.hpp>
#include "BackEndInterface.h"

// Define private windows message for status updates
#define WM_STATUS_UPDATE WM_USER

class CAppParams;

//---------------------------------------------------------------------------
class TActuatorsControlForm : public TForm
{
__published:	// IDE-managed Components
  TGroupBox *SensorsGroupBox;
  TGroupBox *ActuatorsGroupBox;
        TActionList *ActionList1;
        TAction *AlwaysOnTopAction;
        TAction *CloseDialogAction;
        TPopupMenu *PopupMenu1;
        TMenuItem *Alwaysontop1;
        TMenuItem *N1;
        TMenuItem *Close1;
        TImage *Image1;
        TLabel *Label29;
	TLabel *Model1LeftPumpLbl;
		TLabel *S1PumpLabel;
        TLabel *Label41;
	TImage *Model1LeftPumpImg;
		TImage *S1PumpImage;
        TImage *Image14;
        TLabel *Label44;
        TImage *Image25;
        TLabel *TrayHeaterLabel;
        TLabel *YZTMotorsPowerLabel;
        TLabel *Label58;
        TLabel *Label63;
        TLabel *Label64;
        TLabel *Label65;
        TImage *TrayHeaterImage;
        TImage *YZTMotorsPowerImage;
        TImage *Image28;
        TImage *Image29;
        TImage *Image30;
        TImage *Image31;
        TImage *Image34;
        TLabel *ACControlLabel;
        TLabel *Label69;
        TImage *Image35;
        TImage *Image36;
        TLabel *Label70;
        TButton *Button1;
	TButton *Model1LeftPumpBtn;
		TButton *S1PumpBtn;
        TButton *Button25;
        TButton *Button31;
        TButton *TrayHeaterButton;
        TButton *YZTMotorsPowerButton;
        TButton *Button38;
        TButton *Button39;
        TButton *Button42;
        TButton *Button43;
        TButton *Button50;
        TButton *Button51;
        TButton *Button54;
        TLabel *Label45;
        TLabel *Label48;
        TImage *Image2;
        TLabel *Label51;
	TImage *Model1LeftMSImg;
	TImage *SupportLeftMSImg;
        TLabel *Label76;
        TImage *Image42;
        TGroupBox *GroupBox1;
        TGroupBox *GroupBox2;
        TLabel *Label1;
        TLabel *Label47;
        TLabel *Label50;
        TImage *Image20;
        TImage *Image23;
        TLabel *Label77;
        TImage *Image43;
        TLabel *Label78;
        TImage *Image44;
        TLabel *Label54;
        TImage *Image45;
        TImage *Image46;
        TLabel *Label55;
        TLabel *Label33;
        TLabel *Label36;
        TLabel *Label37;
        TLabel *Label39;
        TLabel *Label40;
        TImage *Image6;
        TImage *Image9;
        TImage *Image10;
        TImage *Image12;
        TImage *Image13;
        TButton *Button9;
        TButton *Button15;
        TButton *Button17;
        TButton *Button21;
        TButton *Button23;
        TLabel *Label66;
        TButton *Button46;
        TImage *Image32;
        TLabel *Label67;
        TButton *Button47;
        TImage *Image33;
        TLabel *Label2;
        TBevel *Bevel5;
        TLabel *Label4;
        TBevel *Bevel2;
        TLabel *Label5;
        TBevel *Bevel3;
        TLabel *Label6;
        TBevel *Bevel4;
        TBevel *Bevel6;
        TLabel *Label7;
        TLabel *ACControlLabel2;
        TAction *ActuatorOnAction;
        TAction *ActuatorOffAction;
        TLabel *Label53;
        TImage *Image24;
        TLabel *Label52;
        TImage *Image17;
        TLabel *Label31;
        TButton *Button3;
        TImage *Image3;
	TLabel *Model2LeftPumpLbl;
	TButton *Model2LeftPumpBtn;
	TImage *Model2LeftPumpImg;
	TLabel *S2PumpLabel;
	TButton *S2PumpBtn;
	TImage *S2PumpImage;
	TLabel *M2DrainPumpLabel;
	TButton *M2DrainPumpBtn;
	TImage *M2DrainPumpImage;
	TLabel *DrainPumpsLabel;
	TBevel *DrainPumpsBevel;
	TLabel *M1DrainPumpLabel;
	TButton *M1DrainPumpBtn;
	TImage *M1DrainPumpImage;
	TLabel *S1DrainPumpLabel;
	TButton *S1DrainPumpBtn;
	TImage *S1DrainPumpImage;
	TLabel *S2DrainPumpLabel;
	TButton *S2DrainPumpBtn;
	TImage *S2DrainPumpImage;
        TLabel *Label46;
	TImage *Model1RightMSImg;
        TLabel *Label49;
	TImage *SupportRightMSImg;
        TLabel *Label71;
        TButton *Button55;
        TImage *Image37;
	TImage *Model1RightPumpImg;
	TButton *Model1RightPumpBtn;
	TLabel *Model1RightPumpLbl;
	TImage *Model2RightPumpImg;
	TButton *Model2RightPumpBtn;
	TLabel *Model2RightPumpLbl;
	TLabel *Label8;
	TImage *Image4;
        TLabel *RollerSuctionSystemLabel;
        TBevel *RollerSuctionSystemBevel;
        TLabel *RollerSuctionValveLabel;
        TLabel *RollerSuctionPumpLabel;
        TButton *RollerSuctionValveButton;
        TButton *RollerSuctionPumpButton;
        TImage *RollerSuctionPumpImage;
        TImage *RollerSuctionValveImage;
	TLabel *Model3LeftPumpLbl;
	TLabel *Model3RightPumpLbl;
	TButton *Model3LeftPumpBtn;
	TButton *Model3RightPumpBtn;
	TImage *Model3LeftPumpImg;
	TImage *Model3RightPumpImg;
	TImage *Model2LeftMSImg;
	TImage *Model2RightMSImg;
	TImage *Model3LeftMSImg;
	TImage *Model3RightMSImg;
        TLabel *Model3InsertedLabel;
        TLabel *Model4InsertedLabel;
        TLabel *Model5InsertedLabel;
        TLabel *Model6InsertedLabel;
  void __fastcall FormShow(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall AlwaysOnTopActionExecute(TObject *Sender);
        void __fastcall CloseDialogActionExecute(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall ActuatorOffActionExecute(TObject *Sender);
        void __fastcall ActuatorOnActionExecute(TObject *Sender);
private:	// User declarations

  // Message handler for the WM_STATUS_UPDATE message
  void HandleStatusUpdateMessage(TMessage &Message);

  void SetActuatorStatus(UINT64 ActuatorNum, bool Status);

  bool GetActuatorStatus(UINT64 ActuatorNum);

  bool GetInputStatus(int InputNum);

  void SetInputStatus(int InputNum, bool Status);

  void DisplayEmulationState();
  void ChangeLedsEmulation(bool OnOff, int LedID, int SensorID = -1);

  CAppParams *m_ParamMgr;
  UINT64 m_ActuatorsStatus;
  unsigned long m_InputsStatus;
  int m_PumpTankArr[MAX_ACTUATOR_ID]; //mapping between the pump and its tank

  void LEDIndicator(TWinControl *Control,int InputNum,bool OnOff);

public:		// User declarations
  __fastcall TActuatorsControlForm(TComponent* Owner);

    // Update status on the frame (integer version)
  void UpdateStatus(int ControlID,int Status);

  // Update status on the frame (float version)
  void UpdateStatus(int ControlID,float Status);

  // Update status on the frame (string version)
  void UpdateStatus(int ControlID,QString Status);

BEGIN_MESSAGE_MAP
  MESSAGE_HANDLER(WM_STATUS_UPDATE,TMessage,HandleStatusUpdateMessage);
END_MESSAGE_MAP(TForm);

};
//---------------------------------------------------------------------------
extern PACKAGE TActuatorsControlForm *ActuatorsControlForm;
//---------------------------------------------------------------------------
#endif
