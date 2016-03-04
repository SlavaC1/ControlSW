//---------------------------------------------------------------------------

#ifndef OCBTesterDlgH
#define OCBTesterDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include "QSimpleQueue.h"
#include "QSafeList.h"
#include "OCBProtocolClient.h"
#include "EdenProtocolEngine.h"
#include "TimeoutThread.h"
#include "CommStatusThread.h"
#include <ImgList.hpp>
#include <Menus.hpp>
#include <ActnList.hpp>
#include "OCBDisplayThread.h"
#include "GlobalDefs.h"

#define WM_OCB_A2D_READINGS_DISPLAY             WM_USER+3
#define WM_OCB_ACTUATORS_READINGS_DISPLAY       WM_USER+4
#define WM_OCB_INPUTS_READINGS_DISPLAY          WM_USER+5
#define WM_OCB_POWER_STATUS_DISPLAY             WM_USER+6
#define WM_OCB_CURRENT_TRAY_TEMP_DISPLAY        WM_USER+7
#define WM_OCB_IS_TRAY_IN_DISPLAY               WM_USER+8
#define WM_OCB_TRAY_TEMP_IN_RANGE_DISPLAY       WM_USER+9
#define WM_OCB_UV_LAMPS_STATUS_DISPLAY          WM_USER+10
#define WM_OCB_FILLING_READINGS_DISPLAY         WM_USER+11
#define WM_OCB_PURGE_STATUS_DISPLAY             WM_USER+12
#define WM_OCB_SW_VERSION_DISPLAY               WM_USER+13
#define WM_OCB_DOOR_STATUS_DISPLAY              WM_USER+14
#define WM_OCB_LIQUID_TANKS_STATUS_DISPLAY      WM_USER+15
#define WM_OCB_LIQUID_TANKS_WEIGHTS_DISPLAY     WM_USER+16

#define WM_OCB_UV_LAMPS_ARE_ON_OFF              WM_USER+17
#define WM_OCB_POWER_IS_ON_OFF                  WM_USER+18
#define WM_OCB_POWER_ERROR                      WM_USER+19
#define WM_OCB_DOOR_LOCK_NOTIFICATION           WM_USER+20
#define WM_OCB_UV_LAMPS_ERROR                   WM_USER+21
#define WM_OCB_PURGE_END                        WM_USER+22
#define WM_OCB_PURGE_ERROR                      WM_USER+23
#define WM_OCB_HEADS_FILLING_ERROR              WM_USER+24
#define WM_OCB_RESET_WAKE_UP                    WM_USER+25
#define WM_OCB_LIQUID_TANK_STATUS_CHANGED       WM_USER+26
#define WM_OCB_HEADS_FILLING_PUMPS              WM_USER+27
#define WM_OCB_HEADS_FILLING_NOTIFICATION       WM_USER+28

const int MAX_OCB_MSG_LENGTH       = 1024;
const int MAX_NUM_0F_OCB_MESSAGES  = 100;
const int LAST_OCB_MSG_ID          = 237;
const int FIRST_OCB_MSG_ID         = 200;

const int MATERIAL_TYPE_MODEL_ID   = 0;
const int MATERIAL_TYPE_SUPPORT_ID = 1;
const int MATERIAL_TYPE_BOTH_ID    = 3;

//const int NO_ERROR = 0;
const int TOO_HIGH = 1;
const int TOO_LOW  = 2;
const int TIME_OUT = 3;

const int OCB_NUM_OF_ANALOG_INPUTS      = 32;
const int NUMBER_OF_INPUTS    = 24;

const int NUMBER_OF_ACTUATORS = 48;

struct TMessageHandlerInfo{
  QString Format;
  int Tag;
  };


//---------------------------------------------------------------------------
class TOCBTesterForm : public TForm
{
__published:	// IDE-managed Components
        TPageControl *bh;
  TStatusBar *StatusBar1;
  TTimer *LedTimer;
  TImageList *LEDsImageList;
        TTabSheet *TabSheet1;
        TTabSheet *TabSheet2;
        TActionList *juh;
        TAction *PowerOnAction;
        TAction *PowerOffAction;
        TGroupBox *Just_a_GroupBox;
        TGroupBox *A2DGroupBox;
        TLabel *Label19;
        TLabel *Label20;
        TLabel *Label21;
        TLabel *Label22;
        TLabel *Label23;
        TLabel *Label24;
        TLabel *Label25;
        TLabel *Label26;
        TLabel *Label59;
        TLabel *Label60;
        TLabel *Label61;
        TLabel *Label62;
        TLabel *Label8;
        TLabel *Label9;
        TLabel *Label5;
        TLabel *Label11;
        TLabel *Label10;
        TLabel *Label12;
        TLabel *Label13;
        TLabel *Label14;
        TLabel *Label15;
        TLabel *Label16;
        TLabel *Label17;
        TLabel *Label18;
        TPanel *A2DPanel24;
        TPanel *A2DPanel23;
        TPanel *A2DPanel22;
        TPanel *A2DPanel21;
        TPanel *A2DPanel20;
        TPanel *A2DPanel19;
        TPanel *A2DPanel18;
        TPanel *A2DPanel17;
        TPanel *A2DPanel16;
        TPanel *A2DPanel15;
        TPanel *A2DPanel14;
        TPanel *A2DPanel13;
        TPanel *A2DPanel12;
        TPanel *A2DPanel11;
        TPanel *A2DPanel10;
        TPanel *A2DPanel9;
        TPanel *A2DPanel8;
        TPanel *A2DPanel7;
        TPanel *A2DPanel6;
        TPanel *A2DPanel5;
        TPanel *A2DPanel4;
        TPanel *A2DPanel3;
        TPanel *A2DPanel2;
        TPanel *A2DPanel1;
        TGroupBox *A2D_CPU_GroupBox;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *Label4;
        TLabel *Label6;
        TLabel *Label7;
        TLabel *Label27;
        TLabel *Label28;
        TPanel *Panel1;
        TPanel *Panel2;
        TPanel *Panel3;
        TPanel *Panel4;
        TPanel *Panel5;
        TPanel *Panel6;
        TPanel *Panel7;
        TPanel *Panel8;
        TGroupBox *ActuatorsGroupBox;
        TImage *Image1;
        TLabel *Label29;
        TLabel *Label30;
        TLabel *Label31;
        TLabel *Label32;
        TLabel *Label33;
        TLabel *Label34;
        TLabel *Label35;
        TLabel *Label36;
        TLabel *Label37;
        TLabel *Label38;
        TLabel *Label39;
        TLabel *Label40;
        TLabel *Label41;
        TLabel *Label42;
        TLabel *Label43;
        TImage *Image3;
        TImage *Image4;
        TImage *Image5;
        TImage *Image6;
        TImage *Image7;
        TImage *Image8;
        TImage *Image9;
        TImage *Image10;
        TImage *Image11;
        TImage *Image12;
        TImage *Image13;
        TImage *Image14;
        TImage *Image15;
        TImage *Image16;
        TLabel *Label44;
        TImage *Image25;
        TButton *Button1;
        TButton *Button2;
        TButton *Button3;
        TButton *Button4;
        TButton *Button5;
        TButton *Button6;
        TButton *Button7;
        TButton *Button8;
        TButton *Button9;
        TButton *Button10;
        TButton *Button11;
        TButton *Button12;
        TButton *Button13;
        TButton *Button14;
        TButton *Button15;
        TButton *Button16;
        TButton *Button17;
        TButton *Button18;
        TButton *Button19;
        TButton *Button20;
        TButton *Button21;
        TButton *Button22;
        TButton *Button23;
        TButton *Button24;
        TButton *Button25;
        TButton *Button26;
        TButton *Button27;
        TButton *Button28;
        TButton *Button29;
        TButton *Button30;
        TButton *Button31;
        TButton *Button32;
        TTabSheet *TabSheet3;
        TGroupBox *SensorsGroupBox;
        TLabel *Label45;
        TLabel *Label46;
        TLabel *Label47;
        TLabel *Label48;
        TLabel *Label49;
        TImage *Image2;
        TLabel *Label51;
        TLabel *Label52;
        TImage *Image17;
        TImage *Image18;
        TImage *Image19;
        TImage *Image20;
        TImage *Image21;
        TImage *Image22;
        TLabel *Label56;
        TLabel *Label57;
        TLabel *Label58;
        TLabel *Label63;
        TLabel *Label64;
        TLabel *Label65;
        TButton *Button33;
        TButton *Button34;
        TImage *Image26;
        TImage *Image27;
        TButton *Button35;
        TButton *Button36;
        TImage *Image28;
        TButton *Button37;
        TButton *Button38;
        TButton *Button39;
        TButton *Button40;
        TImage *Image29;
        TImage *Image30;
        TButton *Button41;
        TButton *Button42;
        TButton *Button43;
        TButton *Button44;
        TImage *Image31;
        TImage *Image32;
        TButton *Button45;
        TButton *Button46;
        TLabel *Label66;
        TLabel *Label67;
        TButton *Button47;
        TButton *Button48;
        TImage *Image33;
        TImage *Image34;
        TButton *Button49;
        TButton *Button50;
        TLabel *Label68;
        TLabel *Label69;
        TButton *Button51;
        TButton *Button52;
        TImage *Image35;
        TImage *Image36;
        TButton *Button53;
        TButton *Button54;
        TLabel *Label70;
        TLabel *Label71;
        TButton *Button55;
        TButton *Button56;
        TImage *Image37;
        TImage *Image38;
        TButton *Button57;
        TButton *Button58;
        TLabel *Label72;
        TLabel *Label73;
        TButton *Button59;
        TButton *Button60;
        TImage *Image39;
        TImage *Image40;
        TButton *Button61;
        TButton *Button62;
        TLabel *Label74;
        TLabel *Label75;
        TButton *Button63;
        TButton *Button64;
        TImage *Image41;
        TLabel *Label76;
        TImage *Image42;
        TLabel *Label77;
        TImage *Image43;
        TLabel *Label78;
        TImage *Image44;
        TImage *Image24;
        TLabel *Label53;
        TLabel *Label54;
        TImage *Image45;
        TImage *Image46;
        TLabel *Label55;
        TTabSheet *TabSheet4;
        TAction *HeatingOnAction;
        TAction *HeatingOffAction;
        TAction *UVLampsOn;
        TAction *UVLampsOff;
        TGroupBox *GroupBox2;
        TGroupBox *GroupBox3;
        TGroupBox *GroupBox1;
        TLabel *Label99;
        TImage *RightUVLampImage;
        TLabel *Label91;
        TImage *LeftUVLampImage;
        TLabel *Label86;
        TButton *UVLampsButton;
        TEdit *IgnitionTimeoutEdit;
        TButton *SetUVLampsParameterButton;
        TGroupBox *TrayHeaterGroupBox;
        TLabel *Label79;
        TLabel *Label80;
        TLabel *Label81;
        TLabel *Label82;
        TLabel *Label83;
        TImage *TrayInsertedImage;
        TLabel *Label84;
        TButton *HeatingBottun;
        TEdit *SetTrayTempEdit;
        TEdit *SetTrayMarginEdit;
        TEdit *SetTrayTimeoutEdit;
        TPanel *CurrentTemperaturePanel;
        TButton *SetTrayParametersButton;
        TPanel *TemperatureOKPanel;
        TGroupBox *PowerGroupBox;
        TImage *PowerImage;
        TLabel *Label85;
        TButton *PowerButton;
        TLabeledEdit *PowerOffDelayEdit;
        TLabeledEdit *PowerOnDelayEdit;
        TGroupBox *ModelHeadsGroupBox;
        TEdit *TimeoutFillingEdit;
        TEdit *Model1LevelThermistorHighEdit;
        TEdit *Model1LevelThermistorLowEdit;
        TEdit *PumpOnEdit;
        TButton *SetHeadsParametersButton;
        TEdit *PumpOffEdit;
        TLabel *Label142;
        TLabel *Label143;
        TLabel *Label156;
        TLabel *Label162;
        TLabel *Label169;
        TAction *HeadsFillingOn;
        TGroupBox *GeneralHeadsGroupBox;
        TButton *HeadsFillingButton;
        TPanel *Model1ThermistorLevelPanel;
        TLabel *Label90;
        TPanel *SupportThermistorLevelPanel;
        TLabel *Label93;
        TLabel *Label89;
        TImage *FillingImage;
        TAction *HeadsFillingOff;
        TGroupBox *PurgeBox;
        TEdit *PerformPurge_TimeEdit;
        TLabel *Label219;
        TButton *PerformPurgeButton;
        TLabel *Label100;
        TImage *PurgeStatusImage;
        TGroupBox *SoftwareGroupBox;
        TEdit *ExternalVersionEdit;
        TLabel *Label246;
        TEdit *InternalVersionEdit;
        TLabel *Label247;
        TButton *SWResetButton;
        TButton *GetOCBSoftwareVersion;
        TGroupBox *DoorGroupBox;
        TButton *DoorButton;
        TAction *LockDoorAction;
        TAction *UnLockDoorAction;
        TEdit *DoorStatusEdit;
        TLabel *Label101;
        TTabSheet *TabSheet5;
        TGroupBox *GroupBox4;
        TGroupBox *GroupBox5;
	TEdit *ModelTank1LeftExistEdit;
	TEdit *ModelTank2LeftExistEdit;
	TEdit *ModelTank1RightExistEdit;
        TLabel *Label102;
        TLabel *Label103;
        TLabel *Label104;
        TLabel *Label105;
        TEdit *SupportTank1ExistEdit;
        TLabel *Label106;
        TEdit *SupportTank2ExistEdit;
        TGroupBox *GroupBox6;
	TPanel *Model1LeftPanel;
	TPanel *Model1RightPanel;
	TPanel *Model2LeftPanel;
        TLabel *Label108;
        TLabel *Label109;
        TLabel *Label110;
        TPanel *Support1Panel;
        TPanel *Support2Panel;
	TPanel *Model2RightPanel;
        TLabel *Label111;
        TLabel *Label112;
	TLabel *Model4Support3Lbl;
        TPanel *WastePanel;
        TLabel *Label114;
        TEdit *PostIgnitionTimeoutEdit;
        TLabel *Label115;
        TCheckBox *SensorBypassCheckBox;
        TLabel *Label116;
        TEdit *SupportLevelThermistorLowEdit;
        TEdit *SupportLevelThermistorHighEdit;
        TLabel *Label95;
        TLabel *Label94;
        TGroupBox *GroupBox7;
        TButton *SetActivePumpsButton;
        TGroupBox *Model1ActivePumpsGroupBox;
	TRadioButton *Moedl1LeftPumpRadioButton;
	TRadioButton *Moedl1RightPumpRadioButton;
        TGroupBox *SupportActivePumpsGroupBox;
        TRadioButton *SupportPump1RadioButton;
        TRadioButton *SupportPump2RadioButton;
        TTabSheet *HeadsTab;
        TTabSheet *TabSheet6;
        TGroupBox *GroupBox8;
        TButton *OdourOnOffButton;
        TEdit *OdourValueEdit;
        TAction *OdourOn;
        TAction *OdourOff;
        TLabel *Label92;
        TTabSheet *TabSheet7;
        TGroupBox *GroupBox9;
        TGroupBox *GroupBox10;
        TButton *SetD2AValueDevice1Button;
        TButton *SetD2AValueDevice2Button;
        TEdit *SetD2AValueDevice1Edit;
        TEdit *SetD2AValueDevice2Edit;
        TLabel *Label96;
        TLabel *Label97;
        TGroupBox *ActiveLampsMaskGroupBox;
        TRadioButton *RadioButton1;
        TRadioButton *RadioButton2;
        TRadioButton *RadioButton3;
        TRadioButton *RadioButton4;
        TLabel *Label87;
        TLabel *Label88;
        TLabel *Label98;
        TLabel *Label117;
        TLabel *Label118;
        TPanel *Model2ThermistorLevelPanel;
        TEdit *Model2LevelThermistorLowEdit;
        TEdit *Model2LevelThermistorHighEdit;
        TLabel *Label119;
        TLabel *Label120;
        TPanel *Panel10;
        TLabel *Label121;
        TGroupBox *Model2ActivePumpsGroupBox;
	TRadioButton *Moedl2LeftPumpRadioButton;
	TRadioButton *Moedl2RightPumpRadioButton;
	TPanel *Model3LeftPanel;
	TLabel *Model5Lbl;
	TLabel *Model6Lbl;
	TPanel *Model3RightPanel;
	TLabel *Model5ExistLbl;
	TEdit *ModelTank3LeftExistEdit;
	TEdit *ModelTank3RightExistEdit;
	TLabel *Model6ExistLbl;
	TEdit *ModelTank2RightExistEdit;
	TLabel *Model4ExistLbl;
	TLabel *Label107;
	TPanel *Model3ThermistorLevelPanel;
	TLabel *Label113;
	TEdit *Model3LevelThermistorHighEdit;
	TLabel *Label122;
	TEdit *Model3LevelThermistorLowEdit;
	TGroupBox *Model3ActivePumpsGroupBox;
	TRadioButton *Moedl3LeftPumpRadioButton;
	TRadioButton *Moedl3RightPumpRadioButton;
	TLabel *Label123;
	TEdit *ModelLevelThermistorHighEdit;
	TEdit *SLevelThermistorHighEdit;
	TLabel *Label124;
	TEdit *ModelLevelThermistorLowEdit;
	TLabel *Label125;
	TEdit *SLevelThermistorLowEdit;
	TLabel *Label126;
	TPanel *ModelThermistorLevelPanel;
	TPanel *SThermistorLevelPanel;
	TLabel *Label127;
	TLabel *Label128;
	TLabel *Label129;
	TImage *Image47;
	TLabel *Label130;
	TImage *Image48;
	TLabel *Label131;
	TImage *Image49;
	TCheckBox *MHighThCheckBox;
	TCheckBox *SHighThCheckBox;
	TCheckBox *SLowThCheckBox;
	TCheckBox *M1LowThCheckBox;
	TCheckBox *M2LowThCheckBox;
	TCheckBox *M3LowThCheckBox;
  void __fastcall StatusBar1DrawPanel(TStatusBar *StatusBar,
          TStatusPanel *Panel, const TRect &Rect);
  void __fastcall LedTimerTimer(TObject *Sender);
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall FormDestroy(TObject *Sender);
        void __fastcall PowerOnActionExecute(TObject *Sender);
        void __fastcall PowerOffActionExecute(TObject *Sender);
        void __fastcall ActuatorOffClick(TObject *Sender);
        void __fastcall ActuatorOnClick(TObject *Sender);
        void __fastcall SetTrayParametersButtonClick(TObject *Sender);
        void __fastcall HeatingOnClick(TObject *Sender);
        void __fastcall HeatingOffClick(TObject *Sender);
        void __fastcall UVLampsOnExecute(TObject *Sender);
        void __fastcall UVLampsOffExecute(TObject *Sender);
        void __fastcall SetUVLampsParameterButtonClick(TObject *Sender);
        void __fastcall HeadsFillingOnExecute(TObject *Sender);
        void __fastcall HeadsFillingOffExecute(TObject *Sender);
        void __fastcall SetHeadsParametersButtonClick(
          TObject *Sender);
        void __fastcall PerformPurgeButtonClick(TObject *Sender);
        void __fastcall SWResetButtonClick(TObject *Sender);
        void __fastcall GetOCBSoftwareVersionClick(TObject *Sender);
        void __fastcall LockDoorExecute(TObject *Sender);
        void __fastcall UnLockDoorExecute(TObject *Sender);
        void __fastcall Model3PumpSelectExecte(TObject *Sender);
		void __fastcall Model2PumpSelectExecte(TObject *Sender);
		void __fastcall Model1PumpSelectExecte(TObject *Sender);     
        void __fastcall SupportPumpSelectExecute(TObject *Sender);
        void __fastcall SetActivePumpsButtonClick(TObject *Sender);
        void __fastcall HeadsTabExecute(TObject *Sender);
        void __fastcall OdourOffExecute(TObject *Sender);
        void __fastcall OdourOnExecute(TObject *Sender);
        void __fastcall SetD2AValueDevice1ButtonClick(TObject *Sender);
        void __fastcall SetD2AValueDevice2ButtonClick(TObject *Sender);
private:	// User declarations

  // the protocol engine
  CEdenProtocolEngine *m_ProtocolEngine;

  // the protocol client
  COCBProtocolClient *m_ProtocolClient;


  // a bitmap object for the current led
  Graphics::TBitmap *m_LedBMP;

  // list of timeouts for ack or status messages for messages sent
  TTimeoutList m_TimeoutList;

  // the thread that handles the timeouts for ack or status messages for messages sent
  CTimeoutThread *m_TimeoutThread;

  // install the handler for all the notifications
  void InstallNotificationHandlers();

  // General ACK message.
  void SendAck (int RespondedMessageID);
  void SendNotificationAck (int RespondedMessageID);

  // the thread that checks the communication status
  CCommStatusThread *m_CommStatusThread;

  // the communication status
  bool m_CommStatus;

  // the state of the led
  bool m_LedOn;

  bool IsFirstInputsUpdate;
  bool IsFirstActuatorsUpdate;

  COCB_DisplayThread *m_DisplayThread;

  // Chosen Model/Support Pumps
  BYTE m_Model1_Container;
  BYTE m_Model2_Container;
  BYTE m_Model3_Container;
  BYTE m_SupportContainer;

  CQEvent m_ActuatorsDisplayEvent;
  //DWORD   m_ActuatorsValues;
  //DWORD   m_PrevActuatorsValues;
  bool m_ActuatorsValues[NUMBER_OF_ACTUATORS];
  bool m_PrevActuatorsValues[NUMBER_OF_ACTUATORS];

  CQEvent m_TrayStatusDisplayEvent;
  USHORT  m_CurrentTrayTemp;

  CQEvent m_InputsDisplayEvent;
  WORD    m_InputsValues[NUMBER_OF_INPUTS];
  WORD    m_PrevInputsValues[NUMBER_OF_INPUTS];

  CQEvent m_FillingDisplayEvent;
  BYTE    m_FillingOnOff;
  USHORT  m_SupportThermistorLevel;
  USHORT  m_Model1ThermistorLevel;
  USHORT  m_Model2ThermistorLevel;
  USHORT  m_Model3ThermistorLevel;

  USHORT  m_SThermistorLevel;
  USHORT  m_ModelThermistorLevel;
               
  CQEvent m_PowerDisplayEvent;
  BYTE    m_PowerStatus;

  CQEvent m_DoorDisplayEvent;
  BYTE    m_DoorStatus;

  CQEvent m_PurgeDisplayEvent;
  BYTE    m_PurgeStatus;

  CQEvent m_UVLampsStatusDisplayEvent;
  BYTE    m_RightUVLampValue;
  BYTE    m_LeftUVLampValue;

  CQEvent m_IsTrayInDisplayEvent;
  BYTE    m_IsTrayIn;

  CQEvent m_A2DDisplayEvent;
  WORD    m_A2DValues[OCB_NUM_OF_ANALOG_INPUTS];

  CQEvent m_SWVersionDisplayEvent;
  BYTE    m_InternalVersion;
  BYTE    m_ExternalVersion;

  CQEvent m_LiquidTanksDisplayEvent;

  BYTE m_ModelTank1Status;
  BYTE m_ModelTank2Status;
  BYTE m_ModelTank3Status;
  BYTE m_SupportTank1Status;
  BYTE m_SupportTank2Status;
#ifdef OBJET_MACHINE
  BYTE m_ModelTank4Status;
  BYTE m_ModelTank5Status;
  BYTE m_ModelTank6Status;
#else
  BYTE m_SupportTank3Status;
#endif

  CQEvent m_LiquidTanksWeightsDisplayEvent;
  float m_ModelTank1Weight;
  float m_ModelTank2Weight;
  float m_ModelTank3Weight;
  float m_SupportTank1Weight;
  float m_SupportTank2Weight;
#ifdef OBJET_MACHINE
  float m_ModelTank4Weight;
  float m_ModelTank5Weight;
  float m_ModelTank6Weight;
#else
  float m_SupportTank3Weight;
#endif
  float m_WasteTankWeight;
  

  bool m_IsUVLampsAreOn;

  bool m_IsSWResetInUse;
  bool m_IsPowerOnInUse;
  bool m_IsHeatingOnInUse;
  bool m_IsTrayParametersInUse;
  bool m_IsUVLampsInUse;
  bool m_IsUVLampsParameterInUse;
  bool m_IsPerformPurgeInUse;
  bool m_IsSWVersionInUse;
  bool m_IsLockDoorInUse;
  bool m_IsActivePumpsInUse;
  bool m_IsHeadParamsInUse;
  bool m_IsFillingInUse;
  bool m_IsOdourInUse;
  bool m_IsA2DDeviceInUse;

  bool   m_ChamberCurrentPumpState[NUM_OF_CHAMBERS_THERMISTORS];
  USHORT m_ChamberLiquidLevel[NUM_OF_CHAMBERS_THERMISTORS];

  // Filling error variables
  int m_FillingErrorType;

public:		// User declarations

  void Open(CEdenProtocolEngine *ProtocolEngine);
  __fastcall TOCBTesterForm(TComponent* Owner);

  void UpdateTimeoutFlag (int MsgID);

  static void AckHandler(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);
  void RemoveFromTimeOutList(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);

  static void A2DReceiveHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
  static void ActuatorReceiveHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
  static void InputsReceiveHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
  static void PowerStatusHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
  static void TrayStatusReceiveHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
  static void IsTrayInsertedReceiveHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
  static void GetUVLampsStatusReceiveHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
  static void FillingReceiveHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
  static void PurgeStatusReceiveHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
  static void SWVersionHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
  static void DoorReceiveHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
  static void LiquidTanksStatusHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
  static void LiquidTanksWeightsHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
  static void HeadsFillingActivePumpsHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);

  static void TrayTemperatureIsOKHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
  static void PowerErrorHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
  static void PowerIsOnOffHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
  static void UVLampsAreOnOffHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
  static void UVLampsErrorHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
  static void PurgeEndHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
  static void PurgeErrorHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
  static void HeadsFillingErrorHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
  static void DoorIsLockedHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
  static void ResetWakeUpHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
  static void LiquidTanksStatusChangesHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
  static void HeadsFillingHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);

  void SetCommStatus(bool Status);
  bool GetCommStatus();

  MESSAGE void DisplayActuatorsReading(TMessage& Message);
  MESSAGE void DisplayInputsReading(TMessage& Message);
  MESSAGE void DisplayA2DReading(TMessage& Message);
  MESSAGE void DisplayPowerStatus(TMessage& Message);
  MESSAGE void DisplayCurrentTrayTemp(TMessage& Message);
  MESSAGE void DisplayIsTrayInserted(TMessage& Message);
  MESSAGE void DisplayTrayTempInRange(TMessage& Message);
  MESSAGE void DisplayUVLampsStatus(TMessage& Message);
  MESSAGE void DisplayFillingStatus(TMessage& Message);
  MESSAGE void DisplayPurgeStatus(TMessage& Message);
  MESSAGE void DisplaySWVersion(TMessage& Message);
  MESSAGE void DisplayDoorStatus(TMessage& Message);
  MESSAGE void DisplayLiquidTanksStatus(TMessage& Message);
  MESSAGE void DisplayLiquidTanksWeights(TMessage& Message);
  MESSAGE void DisplayHeadsFillingPumps(TMessage& Message);
  MESSAGE void DisplayHeadsFilling(TMessage& Message);

  MESSAGE void NotifyLampsAreOnOff(TMessage& Message);
  MESSAGE void NotifyPowerIsOnOff(TMessage& Message);
  MESSAGE void NotifyPowerError(TMessage& Message);
  MESSAGE void NotifyDoorLock(TMessage& Message);
  MESSAGE void NotifyUVLampsError(TMessage& Message);
  MESSAGE void NotifyPurgeEnd(TMessage& Message);
  MESSAGE void NotifyPurgeError(TMessage& Message);
  MESSAGE void NotifyFillingError(TMessage& Message);
  MESSAGE void NotifyResetWakeUp(TMessage& Message);
  MESSAGE void NotifyLiquidTanksStatusChanged(TMessage& Message);

  BEGIN_MESSAGE_MAP
  // Statuses
  MESSAGE_HANDLER(WM_OCB_A2D_READINGS_DISPLAY        ,TMessage ,DisplayA2DReading);
  MESSAGE_HANDLER(WM_OCB_ACTUATORS_READINGS_DISPLAY  ,TMessage ,DisplayActuatorsReading);
  MESSAGE_HANDLER(WM_OCB_INPUTS_READINGS_DISPLAY     ,TMessage ,DisplayInputsReading);
  MESSAGE_HANDLER(WM_OCB_POWER_STATUS_DISPLAY        ,TMessage ,DisplayPowerStatus);
  MESSAGE_HANDLER(WM_OCB_CURRENT_TRAY_TEMP_DISPLAY   ,TMessage ,DisplayCurrentTrayTemp);
  MESSAGE_HANDLER(WM_OCB_IS_TRAY_IN_DISPLAY          ,TMessage ,DisplayIsTrayInserted);
  MESSAGE_HANDLER(WM_OCB_TRAY_TEMP_IN_RANGE_DISPLAY  ,TMessage ,DisplayTrayTempInRange);
  MESSAGE_HANDLER(WM_OCB_UV_LAMPS_STATUS_DISPLAY     ,TMessage ,DisplayUVLampsStatus);
  MESSAGE_HANDLER(WM_OCB_FILLING_READINGS_DISPLAY    ,TMessage ,DisplayFillingStatus);
  MESSAGE_HANDLER(WM_OCB_PURGE_STATUS_DISPLAY        ,TMessage ,DisplayPurgeStatus);
  MESSAGE_HANDLER(WM_OCB_SW_VERSION_DISPLAY          ,TMessage ,DisplaySWVersion);
  MESSAGE_HANDLER(WM_OCB_DOOR_STATUS_DISPLAY         ,TMessage ,DisplayDoorStatus);
  MESSAGE_HANDLER(WM_OCB_LIQUID_TANKS_STATUS_DISPLAY ,TMessage ,DisplayLiquidTanksStatus);
  MESSAGE_HANDLER(WM_OCB_LIQUID_TANKS_WEIGHTS_DISPLAY,TMessage ,DisplayLiquidTanksWeights);
  MESSAGE_HANDLER(WM_OCB_HEADS_FILLING_PUMPS         ,TMessage ,DisplayHeadsFillingPumps);
  MESSAGE_HANDLER(WM_OCB_HEADS_FILLING_NOTIFICATION  ,TMessage ,DisplayHeadsFilling);

  // Notifications
  MESSAGE_HANDLER(WM_OCB_UV_LAMPS_ARE_ON_OFF,       TMessage ,NotifyLampsAreOnOff);
  MESSAGE_HANDLER(WM_OCB_POWER_IS_ON_OFF,           TMessage ,NotifyPowerIsOnOff);
  MESSAGE_HANDLER(WM_OCB_POWER_ERROR,               TMessage ,NotifyPowerError);
  MESSAGE_HANDLER(WM_OCB_DOOR_LOCK_NOTIFICATION,    TMessage ,NotifyDoorLock);
  MESSAGE_HANDLER(WM_OCB_UV_LAMPS_ERROR,            TMessage ,NotifyUVLampsError);
  MESSAGE_HANDLER(WM_OCB_PURGE_END,                 TMessage ,NotifyPurgeEnd);
  MESSAGE_HANDLER(WM_OCB_PURGE_ERROR,               TMessage ,NotifyPurgeError);
  MESSAGE_HANDLER(WM_OCB_HEADS_FILLING_ERROR,       TMessage ,NotifyFillingError);
  MESSAGE_HANDLER(WM_OCB_RESET_WAKE_UP,             TMessage ,NotifyResetWakeUp);
  MESSAGE_HANDLER(WM_OCB_LIQUID_TANK_STATUS_CHANGED,TMessage ,NotifyLiquidTanksStatusChanged);
  END_MESSAGE_MAP(TForm);
};


//---------------------------------------------------------------------------
extern PACKAGE TOCBTesterForm *OCBTesterForm;
//---------------------------------------------------------------------------
#endif

