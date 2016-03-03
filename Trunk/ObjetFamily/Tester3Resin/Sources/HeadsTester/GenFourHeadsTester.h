//---------------------------------------------------------------------------

#ifndef GenFourHeadsTesterH
#define GenFourHeadsTesterH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <ImgList.hpp>

#include "NozzlesMap.h"
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include "FireControl.h"
#include "HeadsControl.h"
#include "GenFourTesterDefs.h"
#include "EdenProtocolEngine.h"
#include "OHDBProtocolClient.h"
#include "OCBProtocolClient.h"
#include "A2DDisplayThread.h"
#include "OCBDisplayThread.h"
#include "OCBCommDefs.h"
#include <Graphics.hpp>
#include <Grids.hpp>
#include <ValEdit.hpp>

#define WM_GEN4_ACTUATORS           WM_USER + 60
#define WM_GEN4_THERMISTORS         WM_USER + 61
#define WM_GEN4_STROBE_PULSE_DELAY  WM_USER + 62
#define WM_GEN4_ERROR               WM_USER + 63
#define WM_GEN4_POWER               WM_USER + 64

//---------------------------------------------------------------------------

class TGenFourHeadsTesterForm : public TForm
{
__published:	// IDE-managed Components
	TGroupBox *HeadsDataGroupbox;
	TGroupBox *DataPatternGroupbox;
	TGroupBox *ForAllHeadsGroupbox;
	TBitBtn *SetAllButton;
	TBitBtn *ClearAllButton;
	TBitBtn *LoadPatternButton;
	TBitBtn *SavePatternButton;
	TImageList *ButtonsImageList;
	TLabeledEdit *StartNozzleEdit;
	TLabeledEdit *EndNozzleEdit;
	TLabeledEdit *OnNozzleEdit;
	TLabeledEdit *OffNozzleEdit;
	TBitBtn *SetButton;
	TPageControl *NozzleMapPageControl;
	TOpenDialog *LoadPatternDialog;
	TSaveDialog *SavePatternDialog;
	TPanel *FireControlPanel;
	TFireControlFrame *FireControlFrame1;
	TGroupBox *DownloadGroupbox;
	TBitBtn *DownloadParamsButton;
	TProgressBar *DownloadProgressBar;
	TGroupBox *StrobeGroupbox;
	TButton *StrobeOnButton;
	TPanel *HeadsControlPanel;
	THeadsControlFrame *HeadsControlFrame1;
	TEdit *StrobePulseWidthEdit;
	TLabel *Label1;
	TPanel *HolderPanel1;
	TGroupBox *ActuatorsGroupbox;
	TButton *ActVacuumButton;
	TLabel *Label9;
	TImage *ActVacuumImg;
	TButton *ActAirValveButton;
	TImage *ActAirValveImg;
	TLabel *Label10;
	TButton *ActLedButton;
	TLabel *Label11;
	TImage *ActLedImg;
	TButton *ActWasteButton;
	TLabel *Label12;
	TImage *ActWasteImg;
	TGroupBox *StrobePulseDelayGroupbox;
	TTrackBar *StrobePulseDelayTracker;
	TPanel *StrobePulseDelayValue;
	TLabel *Label2;
	TLabel *MinStrobeLabel;
	TLabel *MaxStrobeLabel;
	TPanel *HeadFillingPanel;
	TGroupBox *FillingGroupbox;
	TPanel *Panel29;
	TPanel *Panel30;
	TPanel *Panel31;
	TPanel *Panel32;
	TPanel *Panel33;
	TPanel *Panel34;
	TPanel *Panel35;
	TPanel *Panel36;
	TPanel *Panel37;
	TPanel *Panel38;
	TPanel *Panel39;
	TPanel *Panel40;
	TButton *MonitorOnOffButton;
	TGroupBox *ThermLevelGroupbox;
	TGroupBox *ThermCalGroupbox;
	TEdit *Edit1;
	TEdit *Edit2;
	TEdit *Edit3;
	TEdit *Edit4;
	TEdit *Edit5;
	TEdit *Edit6;
	TEdit *Edit7;
	TEdit *Edit8;
	TEdit *Edit9;
	TEdit *Edit10;
	TEdit *Edit11;
	TEdit *Edit12;
	TButton *SetFillingParamsButton;
	TGroupBox *BclockOperationSelectionGroupbox;
	TGroupBox *FillingMonitorGroupbox;
	TEdit *FillingTimeoutEdit;
	TLabel *Label3;
	TGroupBox *GroupBox1;
	TEdit *PumpDutyOnEdit;
	TLabel *Label4;
	TEdit *PumpDutyOffEdit;
	TLabel *Label5;
	TImage *FillingMonitorStatusImg;
	TLabel *lbl1;
	TButton *btn1;
	TImage *img1;
	TLabel *lbl2;
	TButton *btn2;
	TImage *img2;
	TLabel *lbl3;
	TImage *img3;
	TButton *btn3;
	TLabel *lbl4;
	TImage *img4;
	TButton *btn4;
	TLabel *lbl5;
	TImage *img5;
	TButton *btn5;
	TLabel *lbl6;
	TImage *img6;
	TButton *btn6;
	TLabel *lbl7;
	TImage *img7;
	TButton *btn7;
	TLabel *lbl8;
	TButton *btn8;
	TImage *img8;
	TLabel *lbl9;
	TLabel *lbl10;
	TButton *btn9;
	TButton *btn10;
	TImage *img9;
	TImage *img10;
	TLabel *lbl11;
	TLabel *lbl12;
	TButton *btn11;
	TButton *btn12;
	TImage *img11;
	TImage *img12;
	TLabel *lbl13;
	TButton *btn13;
	TImage *img13;
	TLabel *lbl14;
	TButton *btn14;
	TImage *img14;
	TLabel *lbl15;
	TButton *btn15;
	TImage *img15;
	TLabel *lbl16;
	TButton *btn16;
	TImage *img16;
	TPanel *pnl1;
	TGroupBox *TankAsignGroupbox;
	TGroupBox *ActiveThermsGroupbox;
	TCheckBox *CheckBox1;
	TCheckBox *CheckBox2;
	TCheckBox *CheckBox3;
	TCheckBox *CheckBox4;
	TCheckBox *CheckBox5;
	TCheckBox *CheckBox6;
	TCheckBox *CheckBox7;
	TCheckBox *CheckBox8;
	TCheckBox *CheckBox9;
	TCheckBox *CheckBox10;
	TCheckBox *CheckBox11;
	TCheckBox *CheckBox12;
	TImage *Image1;
	TRadioGroup *S_Selection_GB;
	TRadioGroup *M4_Selection_GB;
	TRadioGroup *M7_Selection_GB;
	TRadioGroup *M1_Selection_GB;
	TRadioGroup *M5_Selection_GB;
	TRadioGroup *M3_Selection_GB;
	TRadioGroup *M2_Selection_GB;
	TRadioGroup *M6_Selection_GB;
	TButton *StrobeOffButton;
	TBitBtn *DownloadDataButton;
	TPanel *FPGAPanel;
	TGroupBox *FPGARegistersGroupbox;
	TEdit *FPGARegDataEdit;
	TLabel *Label6;
	TButton *ReadFPGARegButton;
	TEdit *FPGARegAddressEdit;
	TLabel *Label7;
	TButton *WriteFPGARegButton;
	TMemo *FPGARegMacroMemo;
	TButton *FPGARegLoadMacroButton;
	TButton *FPGARegSaveMacroButton;
	TButton *FPGARegExecuteMacroButton;
	TCheckBox *FPGARegRecordMacroCheckbox;
	TSaveDialog *FPGAMacroSaveDialog;
	TOpenDialog *FPGAMacroOpenDialog;
	TButton *FPGARegMacroClearButton;
	TGroupBox *PowerGroupbox;
	TButton *PowerButton;
	TImage *PowerStatusImg;
	void __fastcall EditKeyPress(TObject *Sender, char &Key);
	void __fastcall SetButtonClick(TObject *Sender);
	void __fastcall SetAllButtonClick(TObject *Sender);
	void __fastcall ClearAllButtonClick(TObject *Sender);
	void __fastcall LoadPatternButtonClick(TObject *Sender);
	void __fastcall SavePatternButtonClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall DownloadParamsButtonClick(TObject *Sender);
	void __fastcall DownloadDataButtonClick(TObject *Sender);
	void __fastcall StrobePulseDelayTrackerChange(TObject *Sender);
	void __fastcall TurnFillingMonitorOnButtonClick(TObject *Sender);
	void __fastcall TurnFillingMonitorOffButtonClick(TObject *Sender);
	void __fastcall SetFillingParamsButtonClick(TObject *Sender);
	void __fastcall ActiveThermSelectorCheckBoxClick(TObject *Sender);
	void __fastcall StrobeOnButtonClick(TObject *Sender);
	void __fastcall StrobeOffButtonClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall LedOnClick(TObject *Sender);
	void __fastcall LedOffClick(TObject *Sender);
	void __fastcall FilterByteKeyPress(TObject *Sender, char &Key);
	void __fastcall FilterWordKeyPress(TObject *Sender, char &Key);
	void __fastcall FPGARegLoadMacroButtonClick(TObject *Sender);
	void __fastcall FPGARegSaveMacroButtonClick(TObject *Sender);
	void __fastcall WriteFPGARegButtonClick(TObject *Sender);
	void __fastcall ReadFPGARegButtonClick(TObject *Sender);
	void __fastcall FPGARegMacroClearButtonClick(TObject *Sender);
	void __fastcall FPGARegExecuteMacroButtonClick(TObject *Sender);
  	void __fastcall TurnPowerOnButtonClick(TObject *Sender);
	void __fastcall TurnPowerOffButtonClick(TObject *Sender);

private:	// User declarations

	// Protocol engines
	CEdenProtocolEngine *m_OCBProtocolEngine;
	CEdenProtocolEngine *m_OHDBProtocolEngine;

	// Protocol clients
	COCBProtocolClient  *m_OCBProtocolClient;
	COHDBProtocolClient *m_OHDBProtocolClient;

    // Status update threads
	COHDBDisplayThread *m_OHDBDisplayThread;
	COCBDisplayThread  *m_OCBDisplayThread;

	TNozzlesMapFrame *m_NozzleMapFrames[HEADS_NUM];

	TPanel *m_FillingThermPanels [NUM_OF_CHAMBERS_THERMISTORS];
	USHORT  m_FillingThermsLevels[NUM_OF_CHAMBERS_THERMISTORS];

	TEdit       *m_FillingThermInputs  [NUM_OF_CHAMBERS_THERMISTORS];
	TCheckBox   *m_ActiveThermSelectors[NUM_OF_CHAMBERS_THERMISTORS];
	TRadioGroup *m_ContainerSelectors  [NUM_OF_DM_CHAMBERS_THERMISTORS];

	bool m_FillingMonitorStatus;
	bool m_AtAppStartup;
	bool m_PowerStatus;

	bool m_ActuatorsValues    [ACTUATORS_NUM];
	bool m_PrevActuatorsValues[ACTUATORS_NUM];

	std::vector<TFPGACommand> m_PrintCommandsVector;

	WORD m_StrobePulseDelay;

	QString m_ErrorStr;

	WORD m_FPGAData;
	BYTE m_FPGAddress;

	bool ValidatePatternValues(int StartNozzle, int EndNozzle, int OnCount, int OffCount);
	void SetAllNozzleCheckboxes(bool State);

	void __fastcall ActuatorOnClick (TObject *Sender);
	void __fastcall ActuatorOffClick(TObject *Sender);

	void SetActuator(int ActuatorID, int State);
	void GetNozzlesPattern(BYTE *NozzlesPattern);
	void SetNozzlesPattern(BYTE *NozzlesPattern);
	void ActivateFillingMonitor(bool State);
	void LoadFromParams();
	void TurnPower(bool Status);
	void CollectFireParamsSinglePulse(TPulseParams &PulseParams);
	void CollectFireParamsDoublePulse(TPulseParams &PulseParams);
	void CollectFireOperationCommands(TFireParams &FireParams);
	void SendFireParams();

	void OCBSendNotificationAck (int RespondedMessageID);
	void OHDBSendNotificationAck(int RespondedMessageID);

public:		// User declarations
	__fastcall TGenFourHeadsTesterForm(TComponent* Owner);

	void Open(CEdenProtocolEngine *OCBProtocolEngine, CEdenProtocolEngine *OHDBProtocolEngine);
	void WriteToFPGA(BYTE Address, WORD Data);
	void ReadFromFPGA(BYTE Address);
	
	static void ActuatorReceiveHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
	static void FillingReceiveHandler (int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
	static void OCBErrorHandler       (int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
	static void OHDBErrorHandler      (int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
	static void FPGAReadReceiveHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
	static void PowerReceiveHandler   (int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);

	// Display actuators
	MESSAGE void ActuatorsDisplay       (TMessage& Message);
	MESSAGE void FillingDisplay         (TMessage &Message);
	MESSAGE void StrobePulseDelayUpdate (TMessage &Message);
	MESSAGE void NotifyError            (TMessage& Message);
	MESSAGE void PowerStatus            (TMessage& Message);

	BEGIN_MESSAGE_MAP
		MESSAGE_HANDLER(WM_GEN4_ACTUATORS,           TMessage, ActuatorsDisplay);
		MESSAGE_HANDLER(WM_GEN4_THERMISTORS,         TMessage, FillingDisplay);
		MESSAGE_HANDLER(WM_GEN4_STROBE_PULSE_DELAY,  TMessage, StrobePulseDelayUpdate);
		MESSAGE_HANDLER(WM_GEN4_ERROR,               TMessage, NotifyError);
		MESSAGE_HANDLER(WM_GEN4_POWER,               TMessage, PowerStatus);
	END_MESSAGE_MAP(TForm);
};


//---------------------------------------------------------------------------
extern PACKAGE TGenFourHeadsTesterForm *GenFourHeadsTesterForm;
//---------------------------------------------------------------------------

#endif
