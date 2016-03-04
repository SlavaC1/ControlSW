//---------------------------------------------------------------------------

#ifndef OHDBTesterDlgH
#define OHDBTesterDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

#include "EdenProtocolEngine.h"
#include "OHDBProtocolClient.h"
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include "QTimer.h"
#include "A2DDisplayThread.h"
#include "HeadParameters.h"
#include <ImgList.hpp>

//---------------------------------------------------------------------------

#define WM_OHDB_A2D_READINGS_DISPLAY  WM_USER+4
#define WM_OHDB_HEADS_VOLTAGES        WM_USER+5
#define WM_OHDB_BUMPER_NOTIFICATION   WM_USER+6
#define WM_OHDB_OTHER_NOTIFICATION    WM_USER+7
#define WM_OHDB_HEADS_HEATERS         WM_USER+8

const int NUM_OF_ANALOG_INPUTS              = 24;
const int NUMBER_OF_HEATERS                 = 12;
const int MAX_NUM_OF_LAYER_THICKNESS_VALUES = 15;

const int NUMBER_OF_HEADS                   = 8;

#pragma pack(push,1)

struct TXilinxReadMessage{
    BYTE MessageID;
    BYTE Address;
    };

#pragma pack(pop)



class TOHDBTesterForm : public TForm
{
__published:	// IDE-managed Components
  TBitBtn *BitBtn1;
  TOpenDialog *MacroOpenDialog;
  TSaveDialog *MacroSaveDialog;
  TOpenDialog *LoopbackOpenDialog;
  TTimer *FIFOStatusTimer;
        TImageList *BumperLedsImageList;
        TPageControl *BumperTabSheet;
        TTabSheet *XilinxTabSheet;
        TGroupBox *GroupBox3;
        TMemo *MacroMemo;
        TButton *MacroLoadButton;
        TButton *MacroSaveButton;
        TCheckBox *MacroRecordCheckBox;
        TButton *MacroExecuteButton;
        TGroupBox *GroupBox4;
        TLabel *Label1;
        TLabel *Label2;
        TEdit *XilinxAddressEdit;
        TEdit *XilinxDataEdit;
        TButton *XilinxReadButton;
        TButton *XilinxWriteButton;
        TTabSheet *E2promTabSheet;
        TGroupBox *GroupBox38;
        TLabel *Label3;
        TLabel *Label4;
        TLabel *Label7;
        TEdit *E2PROMAddressEdit;
        TEdit *E2PROMDataEdit;
        TComboBox *E2PROMComboBox;
        TButton *E2PROMWriteButton;
        TButton *E2PROMReadButton;
        TTabSheet *PotentiometerTabSheet;
        TGroupBox *GroupBox39;
        TLabel *Label47;
        TLabel *Label6;
        TComboBox *PotentiometerComboBox;
        TEdit *PotenmtrDataEdit;
        TButton *PotenmtrWriteButton;
        TTabSheet *A2DTabSheet;
        TGroupBox *GroupBox41;
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
        TTabSheet *HeatersTabSheet;
        TGroupBox *SetHeadsHeatersGroupBox;
        TLabel *Label27;
        TLabel *Label28;
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
        TLabel *Label58;
        TEdit *ModelHead0TempEdit;
        TEdit *ModelHead1TempEdit;
        TEdit *ModelHead2TempEdit;
        TEdit *ModelHead3TempEdit;
        TEdit *SupportHead0TempEdit;
        TEdit *SupportHead1TempEdit;
        TEdit *SupportHead2TempEdit;
        TEdit *SupportHead3TempEdit;
        TEdit *SupportBlockFrontTempEdit;
        TEdit *SupportBlockRearTempEdit;
        TEdit *ModelBlockRearTempEdit;
        TEdit *ModelBlockFrontTempEdit;
        TButton *SetHeatersTempButton;
        TEdit *ExternalLiquidTempEdit;
        TGroupBox *GroupBox2;
        TLabel *Label39;
        TPanel *TemperatureStatusPanel;
        TButton *IsHeadTempOkButton;
        TGroupBox *GroupBox5;
        TButton *TurnOffHeatersButton;
        TGroupBox *GroupBox11;
        TRadioButton *SetHeatersOffRadioButton;
        TRadioButton *SetHeatersOnRadioButton;
        TGroupBox *GroupBox13;
        TCheckBox *Head1CheckBox;
        TCheckBox *Head2CheckBox;
        TCheckBox *Head3CheckBox;
        TCheckBox *Head4CheckBox;
        TCheckBox *Head5CheckBox;
        TCheckBox *Head6CheckBox;
        TCheckBox *Head7CheckBox;
        TCheckBox *Head8CheckBox;
        TCheckBox *BlockHead1CheckBox;
        TCheckBox *BlockHead2CheckBox;
        TCheckBox *BlockHead3CheckBox;
        TCheckBox *BlockHead4CheckBox;
        TCheckBox *ExternalLiquidCheckBox;
        TTabSheet *TabSheet4;
        TGroupBox *HeadsVoltagesGroupBox;
        TLabel *Label48;
        TLabel *Label49;
        TLabel *Label50;
        TLabel *Label51;
        TLabel *Label52;
        TLabel *Label53;
        TLabel *Label54;
        TLabel *Label55;
        TLabel *Label56;
        TLabel *Label57;
        TEdit *PotentiometerS3Edit;
        TEdit *PotentiometerS2Edit;
        TEdit *PotentiometerS1Edit;
        TEdit *PotentiometerS0Edit;
        TEdit *PotentiometerM3Edit;
        TEdit *PotentiometerM2Edit;
        TEdit *PotentiometerM1Edit;
        TEdit *PotentiometerM0Edit;
        TPanel *S3VoltageA2DPanel;
        TPanel *S2VoltageA2DPanel;
        TPanel *S1VoltageA2DPanel;
        TPanel *S0VoltageA2DPanel;
        TPanel *M3VoltageA2DPanel;
        TPanel *M2VoltageA2DPanel;
        TPanel *M1VoltageA2DPanel;
        TPanel *M0VoltageA2DPanel;
        TButton *SetPotnmtrValuesButton;
        TTabSheet *TabSheet5;
        TGroupBox *GroupBox14;
        TLabel *Label74;
        TButton *SetHeadDataButton;
        TButton *GetHeadDataButton;
        TComboBox *HeadNumComboBox;
        TGroupBox *GroupBox15;
        TLabel *Label64;
        TLabel *Label65;
        TLabel *Label66;
        TLabel *Label67;
        TLabel *Label68;
        TLabel *Label69;
        TEdit *Support10KhzAEdit;
        TEdit *Model20KhzAEdit;
        TEdit *Support20KhzAEdit;
        TEdit *Support20KhzBEdit;
        TEdit *Support10KhzBEdit;
        TEdit *Model10KhzAEdit;
        TEdit *Model20KhzBEdit;
        TEdit *Model10KhzBEdit;
        TGroupBox *GroupBox16;
        TLabel *Label70;
        TLabel *Label73;
        TLabel *Label63;
        TEdit *XOffsetEdit;
        TDateTimePicker *ProductionDateTimePicker;
        TEdit *SerialNumberEdit;
        TGroupBox *GroupBox17;
        TLabel *Label71;
        TLabel *Label72;
        TLabel *Label75;
        TLabel *Label76;
        TLabel *Label77;
        TLabel *Label78;
        TLabel *Label79;
        TEdit *A2DValuefor80CEdit;
        TEdit *A2DValuefor60CEdit;
        TGroupBox *GroupBox40;
        TLabeledEdit *HeadsDataCheckSumEdit;
        TLabeledEdit *HeadsDataValidityEdit;
        TButton *VerifyHeadDataButton;
        TTabSheet *TabSheet1;
        TGroupBox *GroupBox6;
        TButton *SetRollerOnOffButton;
        TRadioButton *RollerOffRadioButton;
        TRadioButton *RollerOnRadioButton;
        TGroupBox *GroupBox7;
        TLabel *Label40;
        TButton *setRollerSpeedButton;
        TEdit *RollerSpeedEdit;
        TGroupBox *GroupBox8;
        TLabel *Label41;
        TLabel *Label42;
        TButton *GetRollerStatusButton;
        TPanel *RollerStatusPanel;
        TPanel *RollerSpeedPanel;
        TTabSheet *TabSheet2;
        TButton *LoopbackLoadButton;
        TBitBtn *LoopbackGoBitBtn;
        TBitBtn *LoopbackStopBitBtn;
        TCheckBox *ContinuousCheckBox;
        TCheckBox *EnableOHDBCheckBox;
        TGroupBox *GroupBox9;
        TLabel *Label43;
        TLabel *Label44;
        TProgressBar *FIFOStatusProgressBar;
        TTabSheet *PrintingTabSheet;
        TGroupBox *GroupBox28;
        TGroupBox *GroupBox29;
        TLabeledEdit *StartPegParamEdit;
        TLabeledEdit *EndPegParamEdit;
        TLabeledEdit *NumberOfFireParamEdit;
        TGroupBox *GroupBox30;
        TLabeledEdit *BumperStartPegParamEdit;
        TLabeledEdit *BumperEndPegParamEdit;
        TButton *SetPrintingPassParametersButton;
        TGroupBox *GroupBox31;
        TButton *EnablePrintingCircuitButton;
        TRadioButton *EnablePrintingCircuitRadioButton;
        TRadioButton *DisablePrintingCircuitRadioButton;
        TGroupBox *GroupBox32;
        TButton *ResetDriverCircuitButton;
        TCheckBox *ResetDriverCircuitCheckBox;
        TCheckBox *ResetDriverStateMachineCheckBox;
        TCheckBox *DefaultPrintParamsCheckBox;
        TGroupBox *GroupBox33;
        TGroupBox *GroupBox34;
        TButton *FireAllButton;
        TLabeledEdit *FireAllFrequencyEdit;
        TLabeledEdit *FireAllTimeEdit;
        TGroupBox *GroupBox35;
        TButton *GetFireAllStatusButton;
        TLabeledEdit *FireAllStatusEdit;
        TLabeledEdit *FireAllCounterEdit;
        TTabSheet *TabSheet8;
        TGroupBox *GroupBox18;
        TRadioButton *SetBumperOnRadioButton;
        TRadioButton *SetBumperOffRadioButton;
        TButton *SetBumperOnOffButton;
        TGroupBox *GroupBox36;
        TButton *SetBumperSensitivityButton;
        TLabeledEdit *SetBumperSensitivityEdit;
        TLabeledEdit *SetBumperResetTimeEdit;
        TLabeledEdit *SetBumperImpactCounterEdit;
        TGroupBox *GroupBox37;
        TImage *BumperOnOffLedImage;
        TLabel *BumperStatusLabel;
        TButton *BumperStatusResetButton;
        TGroupBox *BumperPeg;
        TRadioButton *SetBumperPegOnRadioButton;
        TRadioButton *SetBumperPegOffRadioButton;
        TButton *SetBumperPegOnOffButton;
        TTabSheet *GeneralTabSheet;
        TGroupBox *GroupBox21;
        TGroupBox *GroupBox22;
        TLabeledEdit *SetVacuumModelHighValueEdit;
        TLabeledEdit *SetVacuumModelLowValueEdit;
        TButton *SetVacuumModelValueButton;
        TGroupBox *GroupBox23;
        TButton *GetVacuumStatusButton;
        TLabeledEdit *GetVacuumStatusEdit;
        TGroupBox *GroupBox24;
        TButton *GetVacuumSensorStatusButton;
        TEdit *GetVacuumSensorStatusEdit;
        TGroupBox *GroupBox25;
        TGroupBox *GroupBox19;
        TLabel *Model;
        TLabel *Edit80;
        TButton *GetMaterialLevelStatusButton;
        TEdit *GetSupportMaterialStatusLevelEdit;
        TEdit *GetModelMaterialStatusLevelEdit;
        TGroupBox *GroupBox20;
        TRadioButton *SetMaterialCoolingFanOnBtn;
        TRadioButton *SetMaterialCoolingFanOffBtn;
        TButton *SetMaterialCoolingFanButton;
        TLabeledEdit *SetMaterialCoolingFanEdit;
        TUpDown *SetMaterialCoolingFanUpDown;
        TGroupBox *GroupBox26;
        TButton *GetAmbientTemperatureButton;
        TLabeledEdit *GetAmbientTemperatureA2DEdit;
        TLabeledEdit *GetAmbientTemperatureEdit;
        TGroupBox *GroupBox27;
        TButton *GetPowerSuppliesVoltagesButton;
        TLabeledEdit *VppPowerSupplyEdit;
        TLabeledEdit *Heater24VPowerSupplyEdit;
        TLabeledEdit *VDDPowerSupplyEdit;
        TLabeledEdit *VCCPowerSupplyEdit;
        TButton *SoftwareResetButton;
        TTabSheet *TabSheet3;
        TGroupBox *GroupBox10;
        TLabel *Label45;
        TPanel *SoftwareVersionPanel;
        TButton *GetSoftwareVersionButton;
        TGroupBox *GroupBox12;
        TLabel *Label46;
        TPanel *HardwareVersionPanel;
        TButton *GetHardwareVersionButton;
        TEdit *LowThresholdTempEdit;
        TEdit *HighThresholdTempEdit;
        TLabel *Label80;
        TLabel *Label81;
        TEdit *GetModelHead0TempEdit;
        TEdit *GetModelHead1TempEdit;
        TEdit *GetModelHead2TempEdit;
        TEdit *GetModelHead3TempEdit;
        TEdit *GetSupportHead0TempEdit;
        TEdit *GetSupportHead1TempEdit;
        TEdit *GetSupportHead2TempEdit;
        TEdit *GetSupportHead3TempEdit;
        TEdit *GetModelBlockFrontTempEdit;
        TEdit *GetModelBlockRearTempEdit;
        TEdit *GetSupportBlockFrontTempEdit;
        TEdit *GetSupportBlockRearTempEdit;
        TEdit *GetExternalLiquidTempEdit;
        TLabel *Label82;
        TUpDown *PotentiometerM0UpDown;
        TUpDown *PotentiometerM1UpDown;
        TUpDown *PotentiometerM2UpDown;
        TUpDown *PotentiometerM3UpDown;
        TUpDown *PotentiometerS0UpDown;
        TUpDown *PotentiometerS1UpDown;
        TUpDown *PotentiometerS2UpDown;
        TUpDown *PotentiometerS3UpDown;
        TLabel *Label83;
        TEdit *VppPowerSupplyCEdit;
        TEdit *Heater24VPowerSupplyCEdit;
        TEdit *VDDPowerSupplyCEdit;
        TEdit *VCCPowerSupplyCEdit;
        TLabel *Label84;
        TPanel *M0VoltagePanel;
        TPanel *M1VoltagePanel;
        TPanel *M2VoltagePanel;
        TPanel *M3VoltagePanel;
        TPanel *S0VoltagePanel;
        TPanel *S1VoltagePanel;
        TPanel *S2VoltagePanel;
        TPanel *S3VoltagePanel;
        TLabel *Label85;
  void __fastcall FormDestroy(TObject *Sender);
  void __fastcall XilinxWriteButtonClick(TObject *Sender);
  void __fastcall XilinxReadButtonClick(TObject *Sender);
  void __fastcall E2PROMWriteButtonClick(TObject *Sender);
  void __fastcall E2PROMReadButtonClick(TObject *Sender);
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall PotenmtrWriteButtonClick(TObject *Sender);
  void __fastcall DataEditClick(TObject *Sender);
  void __fastcall SetHeatersTempButtonClick(TObject *Sender);
  void __fastcall IsHeadTempOkButtonClick(TObject *Sender);
  void __fastcall MacroLoadButtonClick(TObject *Sender);
  void __fastcall MacroSaveButtonClick(TObject *Sender);
  void __fastcall MacroExecuteButtonClick(TObject *Sender);
  void __fastcall TurnOffHeatersButtonClick(TObject *Sender);
  void __fastcall SetRollerOnOffButtonClick(TObject *Sender);
  void __fastcall setRollerSpeedButtonClick(TObject *Sender);
  void __fastcall GetRollerStatusButtonClick(TObject *Sender);
  void __fastcall LoopbackLoadButtonClick(TObject *Sender);
  void __fastcall LoopbackGoBitBtnClick(TObject *Sender);
  void __fastcall FormShow(TObject *Sender);
  void __fastcall LoopbackStopBitBtnClick(TObject *Sender);
  void __fastcall FIFOStatusTimerTimer(TObject *Sender);
  void __fastcall GetSoftwareVersionButtonClick(TObject *Sender);
  void __fastcall GetHardwareVersionButtonClick(TObject *Sender);
  void __fastcall SetPotnmtrValuesButtonClick(TObject *Sender);
  void __fastcall HeatersMaskCheckBoxClick(TObject *Sender);
  void __fastcall SetHeadDataButtonClick(TObject *Sender);
        void __fastcall GetMaterialLevelStatusButtonClick(TObject *Sender);
        void __fastcall SetMaterialCoolingFanOnBtnClick(TObject *Sender);
        void __fastcall SetMaterialCoolingFanOffBtnClick(TObject *Sender);
        void __fastcall SetMaterialCoolingFanButtonClick(TObject *Sender);
        void __fastcall SetVacuumModelValueButtonClick(TObject *Sender);
        void __fastcall GetVacuumStatusButtonClick(TObject *Sender);
        void __fastcall GetVacuumSensorStatusButtonClick(TObject *Sender);
        void __fastcall GetAmbientTemperatureButtonClick(TObject *Sender);
        void __fastcall GetPowerSuppliesVoltagesButtonClick(
          TObject *Sender);
        void __fastcall SoftwareResetButtonClick(TObject *Sender);
        void __fastcall ResetDriverCircuitButtonClick(TObject *Sender);
        void __fastcall SetPrintingPassParametersButtonClick(
          TObject *Sender);
        void __fastcall EnablePrintingCircuitButtonClick(TObject *Sender);
        void __fastcall FireAllButtonClick(TObject *Sender);
        void __fastcall GetFireAllStatusButtonClick(TObject *Sender);
        void __fastcall SetBumperSensitivityButtonClick(TObject *Sender);
        void __fastcall SetBumperOnOffButtonClick(TObject *Sender);
        void __fastcall BumperStatusResetButtonClick(TObject *Sender);
        void __fastcall SetBumperPegOnOffButtonClick(TObject *Sender);
        void __fastcall VerifyHeadDataButtonClick(TObject *Sender);
        void __fastcall GetHeadDataButtonClick(TObject *Sender);
private:	// User declarations

  struct TA2DTimeoutCockie{
      TOHDBTesterForm *FormInstance;
      int TransactionHandle;
      };

  // the protocol engine
   CEdenProtocolEngine *m_ProtocolEngine;

   // the protocol client
   COHDBProtocolClient *m_ProtocolClient;

   CA2DDisplayThread *m_A2DDisplayThread;

  TMemoryStream *m_LoopbackFile;
  int m_LoopBackCompareIndex;
  bool m_StopLoopbackTest;
  bool m_PCIStatus;

  CQEvent m_XilinxEvent;
  CQEvent m_E2PROMEvent;
  CQEvent m_HeadTemperatureEvent;
  CQEvent m_XilinxWriteEvent;
  CQEvent m_PotentiometerEvent;
  CQEvent m_E2PROMWriteEvent;
  CQEvent m_RollerOnOffEvent;
  CQEvent m_RollerSpeedEvent;
  CQEvent m_GetRollerStatusEvent;
  CQEvent m_StartLoopBackEvent;
  CQEvent m_A2DDisplayEvent;
  CQEvent m_SetHeatersOnOffEvent;
  CQEvent m_GetHardwareVersionEvent;
  CQEvent m_GetSoftwareVersionEvent;
  CQEvent m_SetHeadsVoltagesEvent;
  CQEvent m_GetHeadsVoltagesEvent;
  CQEvent m_A2DHeatersDisplayEvent;
  CQEvent m_SetHeadDataEvent;
  CQEvent m_GetHeadDataEvent;
  CQEvent m_GetMaterialLevelEvent;
  CQEvent m_MaterialCoolingFanEvent;
  CQEvent m_SetVacuumValuesEvent;
  CQEvent m_GetVacuumStatusEvent;
  CQEvent m_GetVacuumSensorStatusEvent;
  CQEvent m_GetAmbientTemperatureEvent;
  CQEvent m_GetPowerSuppliesVoltagesEvent;
  CQEvent m_ResetSoftwareEvent;
  CQEvent m_ResetDriverCircuitEvent;
  CQEvent m_ResetDriverStateMachineEvent;
  CQEvent m_ApplyDefaultPrintingParamsEvent;
  CQEvent m_SetPrintingPassParamsEvent;
  CQEvent m_EnablePrintingCircuitEvent;
  CQEvent m_DisablePrintingCircuitEvent;
  CQEvent m_FireAllEvent;
  CQEvent m_FireAllStatusEvent;
  CQEvent m_SetBumperSensitivityEvent;
  CQEvent m_SetBumperOnOffEvent;
  CQEvent m_SetBumperPegOnOffEvent;
  CQEvent m_SetHeatersTempEvent;

  WORD m_XilinxData;
  BYTE m_E2PROMData;
  BYTE m_PotenmtrData;

  BYTE m_XilinxAddress;
  WORD m_E2PROMAddress;

  BYTE m_HeadTempraturesStatus;
  BYTE m_RollerStatus;
  WORD m_RollerSpeed;
  BYTE m_HardwareVersion;
  BYTE m_InternalSoftwareVersion;
  BYTE m_ExternalSoftwareVersion;
  WORD m_A2DValues[NUM_OF_ANALOG_INPUTS];
  WORD m_HeadsVoltages[NUMBER_OF_HEADS];
  WORD m_HeadsHeaters [NUMBER_OF_HEATERS];
  WORD m_HeatersMask;
  WORD m_ModelFullLevel;
  WORD m_ModelHalfFullLevel;
  WORD m_SupportFullLevel;
  WORD m_SupportlHalfFullLevel;
  BYTE m_HeadsVacuumOK;
  WORD m_ModelVacuumSensorValue;
  WORD m_AmbientTemperatureValue;
  WORD m_VppPowerSupplyValue;
  WORD m_Heater24VPowerSupplyValue;
  WORD m_VDDPowerSupplyValue;
  WORD m_VCCPowerSupplyValue;
  BYTE m_FireAllStatus;
  BYTE m_FireAllCounter;
  WORD m_HeadDataCheckSum;
  BYTE m_HeadDataValidity;


  THeadParameters m_HeadData;
  int m_HeadNum;

  void AddToMacroRecorder(BYTE Address, WORD Data);
  void WriteDataToXilinx(BYTE Address, WORD Data);
  bool GetAndDisplayHeadsData ();





public:		// User declarations

   void Open(CEdenProtocolEngine *ProtocolEngine);

  __fastcall TOHDBTesterForm(TComponent* Owner);
  static void StatusHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
  static void A2DReceiveHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
  static void AckHandler(int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie);
  static void A2DTimeoutHandler(TGenericCockie Cockie);
  static void LoopBackTestRcvHandler(int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie);
  static void BumperRcvHandler(int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie);
  static void NotificationsRcvHandler(int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie);


  // Display the A2D readings
  MESSAGE void DisplayA2DReading(TMessage& Message);

  // Display the heads voltages
  MESSAGE void HeadsVoltagesDisplay(TMessage& Message);

  // Display a Bumper notification
  MESSAGE void BumperNotificationDisplay(TMessage& Message);

  // Display Other notification
  MESSAGE void OtherNotificationDisplay(TMessage& Message);

  // Display Heads Heaters
  MESSAGE void HeadsHeatersDisplay(TMessage& Message);

  BEGIN_MESSAGE_MAP
  MESSAGE_HANDLER(WM_OHDB_A2D_READINGS_DISPLAY,TMessage ,DisplayA2DReading);
  MESSAGE_HANDLER(WM_OHDB_HEADS_VOLTAGES,TMessage ,HeadsVoltagesDisplay);
  MESSAGE_HANDLER(WM_OHDB_BUMPER_NOTIFICATION,TMessage ,BumperNotificationDisplay);
  MESSAGE_HANDLER(WM_OHDB_OTHER_NOTIFICATION,TMessage ,OtherNotificationDisplay);
  MESSAGE_HANDLER(WM_OHDB_HEADS_HEATERS,TMessage ,HeadsHeatersDisplay);

  END_MESSAGE_MAP(TForm);
};
//---------------------------------------------------------------------------
extern PACKAGE TOHDBTesterForm *OHDBTesterForm;
//---------------------------------------------------------------------------
#endif
