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
        TLabel *Label41;
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
        TImage *Image2;
        TLabel *Label51;
	TLabel *InterlockLbl;
	TImage *InterlockImg;
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
        TLabel *Label52;
        TImage *Image17;
        TLabel *Label31;
        TButton *Button3;
        TImage *Image3;
        TLabel *Label71;
        TButton *Button55;
        TImage *Image37;
	TLabel *TrayInsertedLabel;
	TImage *TrayInsertedImg;
        TLabel *RollerSuctionSystemLabel;
        TBevel *RollerSuctionSystemBevel;
        TLabel *RollerSuctionValveLabel;
        TLabel *RollerSuctionPumpLabel;
        TButton *RollerSuctionValveButton;
        TButton *RollerSuctionPumpButton;
        TImage *RollerSuctionPumpImage;
        TImage *RollerSuctionValveImage;
	TLabel *WasteRightLabel;
	TButton *WasteRightBtn;
	TImage *WasteRightImg;
	TLabel *SignalTowerLabel;
	TBevel *SignalTowerBevel;
	TLabel *RedLightLabel;
	TButton *RedLightButton;
	TImage *RedLightImage;
	TLabel *GreenLightLabel;
	TButton *GreenLightButton;
	TImage *GreenLightImage;
	TLabel *YellowLightLabel;
	TButton *YellowLightButton;
	TImage *YellowLightImage;
	TLabel *AmbientHeater1Label;
	TButton *AmbientHeater1Button;
	TImage *AmbientHeater1Image;
	TLabel *AmbientHeater2Label;
	TButton *AmbientHeater2Button;
	TImage *AmbientHeater2Image;
	TImage *serviceDoorImg;
	TImage *FrontRightDoorImg;
	TImage *FrontLeftDoorImg;
	TImage *ServiceKeyDoorImg;
	TMainMenu *MainMenu1;
	TLabel *lblServiceDoorLabel;
	TLabel *FrontRightDoorLabel;
	TLabel *FrontLeftDoorLabel;
	TLabel *ServiceKeyDoorLabel;
	TImage *EmergencyRearImg;
	TImage *EmergencyFrontImg;
	TLabel *EmergencyRearlbl;
	TLabel *EmergencyFrontlbl;
    TLabel *PurgeBathLabel;
    TImage *PurgeBathImg;
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
  void InitDoorGuiParams();

  void SetActuatorStatus(UINT64 ActuatorNum, bool Status);

  bool GetActuatorStatus(UINT64 ActuatorNum);

  bool GetInputStatus(int InputNum);

  void SetInputStatus(int InputNum, bool Status);

  void DisplayEmulationState();
  void ChangeLedsEmulation(bool OnOff, int LedID);

  void InitPumps();
  void InitRSS();
  void InitSignalTower();
  void InitTrayHeater();
  void InitAmbientHeater();
  void InitDualWaste();
  void InitRemovableTray();
  void InitPurgeBath();

  CAppParams *m_ParamMgr;
  UINT64 m_ActuatorsStatus;
  unsigned long m_InputsStatus;
  int m_PumpTankArr[MAX_ACTUATOR_ID]; //mapping between the pump and its tank
  
//#ifdef OBJET_MACHINE_KESHET
	TLabel*  ModelPumpLbl[NUMBER_OF_PUMPS];
	TButton* ModelPumpBtn[NUMBER_OF_PUMPS];
	TImage*  ModelPumpImg[NUMBER_OF_PUMPS];
	
	TLabel*  TankMSLbl[TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE];
	TImage*  TankMSImg[TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE];
//#endif

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
