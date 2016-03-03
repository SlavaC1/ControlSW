//---------------------------------------------------------------------------

#ifndef HeadsControlH
#define HeadsControlH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>

#include "GenFourTesterDefs.h"
#include <Buttons.hpp>
#include "OHDBCommDefs.h"
#include "OHDBProtocolClient.h"
#include "OCBCommDefs.h"
#include "OCBProtocolClient.h"

#define WM_GEN4_HEADS_VOLTAGES WM_USER + 50
#define WM_GEN4_HEADS_HEATERS  WM_USER + 51
#define WM_GEN4_PS_VOLTAGES    WM_USER + 52
#define WM_GEN4_VACUUM         WM_USER + 53

struct THeadVoltRange
{
	double Max;
	double Min;
};

//---------------------------------------------------------------------------
class THeadsControlFrame : public TFrame
{
__published:	// IDE-managed Components
	TGroupBox *TemperatureGroupbox;
	TGroupBox *VoltagesGroupbox;
	TEdit *Edit1;
	TEdit *Edit2;
	TEdit *Edit3;
	TEdit *Edit4;
	TEdit *Edit5;
	TEdit *Edit6;
	TEdit *Edit7;
	TPanel *Panel1;
	TPanel *Panel2;
	TPanel *Panel3;
	TPanel *Panel4;
	TPanel *Panel5;
	TPanel *Panel6;
	TPanel *Panel7;
	TPanel *Panel8;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *Label7;
	TLabel *Label9;
	TLabel *Label10;
	TLabel *Label11;
	TEdit *Edit8;
	TPanel *Panel9;
	TLabel *Label12;
	TEdit *Edit9;
	TEdit *Edit12;
	TEdit *Edit11;
	TEdit *Edit10;
	TLabel *Label13;
	TLabel *Label14;
	TLabel *Label15;
	TPanel *Panel10;
	TPanel *Panel11;
	TPanel *Panel12;
	TEdit *Edit14;
	TEdit *Edit15;
	TEdit *Edit16;
	TEdit *Edit17;
	TEdit *Edit18;
	TEdit *Edit19;
	TEdit *Edit20;
	TEdit *Edit21;
	TLabel *Label8;
	TLabel *Label16;
	TLabel *Label17;
	TPanel *Panel13;
	TPanel *Panel14;
	TPanel *Panel15;
	TPanel *Panel16;
	TLabel *Label18;
	TLabel *Label19;
	TLabel *Label20;
	TLabel *Label21;
	TLabel *Label22;
	TLabel *Label23;
	TLabel *Label24;
	TLabel *Label25;
	TPanel *Panel17;
	TPanel *Panel18;
	TPanel *Panel19;
	TPanel *Panel20;
	TPanel *Panel21;
	TPanel *Panel22;
	TPanel *Panel23;
	TPanel *Panel24;
	TPanel *Panel25;
	TPanel *Panel26;
	TPanel *Panel27;
	TPanel *Panel28;
	TGroupBox *GroupBox1;
	TEdit *LowThresholdEdit;
	TEdit *HighThresholdEdit;
	TLabel *Label26;
	TLabel *Label27;
	TButton *SetTempValuesButton;
	TPanel *VppVoltagePanel;
	TLabel *Label28;
	TButton *CalibrateVoltageButton;
	TGroupBox *VppGroupbox;
	TButton *HeatingOnButton;
	TButton *HeatingOffButton;
	TGroupBox *PurgeGroupbox;
	TLabel *Label29;
	TLabel *Label30;
	TPanel *VacuumReading;
	TEdit *PurgeDurationEdit;
	TButton *PurgeButton;
	TButton *SetInputVoltageRangeButton;
	TLabel *Label31;
	TEdit *Edit13;
	TPanel *Panel29;
	TEdit *HeatingRateEdit;
	TLabel *HeatingRateLabel;
	TCheckBox *SameValueForHeadHalvesCheckbox;

	void __fastcall EditKeyPressInt(TObject *Sender, char &Key);
	void __fastcall EditKeyPressFloat(TObject *Sender, char &Key);

	void __fastcall SetHeatersValuesButtonClick(TObject *Sender);
	void __fastcall TurnHeatersOnButtonClick(TObject *Sender);
	void __fastcall TurnHeatersOffButtonClick(TObject *Sender);
	void __fastcall CalibrateVoltageButtonClick(TObject *Sender);
	void __fastcall SetInputVoltageRangeButtonClick(TObject *Sender);
	void __fastcall PurgeButtonClick(TObject *Sender);
	void __fastcall SameValueForHeadHalvesCheckboxClick(TObject *Sender);

private:	// User declarations

	TEdit     *m_HeatersInputValues  [HEATERS_NUM];
	TPanel    *m_HeaterReadingsPanels[HEATERS_NUM];

	TEdit     *m_VoltagesInputValues      [HEAD_VOLTAGES_NUM];
	TPanel    *m_VoltageReadingVoltsPanels[HEAD_VOLTAGES_NUM];
	TPanel    *m_VoltageReadingA2DPanels  [HEAD_VOLTAGES_NUM];

	WORD           m_HeadsVoltsForRangeCalc[HEAD_VOLTAGES_NUM];
	THeadVoltRange m_HeadsVoltageRange     [HEAD_VOLTAGES_NUM];
	CQEvent        m_SetVoltageRangeEvent;

	WORD m_HeadsVoltages[HEAD_VOLTAGES_NUM];
	WORD m_HeadsHeaters [HEATERS_NUM];

	WORD m_VppPowerSupplyValue;
	WORD m_VacuumSensorValue;

	BYTE m_HeadsForVoltageCalibration;

	COHDBProtocolClient *m_OHDBProtocolClient;
	COCBProtocolClient  *m_OCBProtocolClient;

	bool m_DuringHeating;

	void   ActivateHeaters       (bool    State);
	void   SetPotentiometerValues();
	void   SendPotValuesToHeads  (int    *PotValues);
	void   ReadVoltagesFromHeads ();
	void   VoltsToPotsCheckRange (int    *Pots);
	bool   CheckInputVoltageRange(double  Value,   int HeadIndex);
	void   MarkHeadForVoltCal    (int     HeadNum, bool On);
	bool   IsHeadForVoltCal      (int     HeadNum); 
	void   CalibrateVoltages     ();
	void   CalcVoltageRange      ();

public:		// User declarations
	__fastcall THeadsControlFrame(TComponent* Owner);

	void SetOHDBProtocolClient(COHDBProtocolClient *OHDBProtocolClient);
	void SetOCBProtocolClient (COCBProtocolClient  *OCBProtocolClient);
	void LoadFromParams();

	static void StatusHandler         (int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
	static void SetVoltageRangeHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);

	MESSAGE void HeadsVoltagesDisplay(TMessage& Message);
	MESSAGE void HeadsHeatersDisplay (TMessage& Message);
	MESSAGE void VppDisplay          (TMessage& Message);
	MESSAGE void VacuumDisplay       (TMessage& Message);

	BEGIN_MESSAGE_MAP
		MESSAGE_HANDLER(WM_GEN4_HEADS_VOLTAGES, TMessage, HeadsVoltagesDisplay);
		MESSAGE_HANDLER(WM_GEN4_HEADS_HEATERS,  TMessage, HeadsHeatersDisplay);
		MESSAGE_HANDLER(WM_GEN4_PS_VOLTAGES,    TMessage, VppDisplay);
		MESSAGE_HANDLER(WM_GEN4_VACUUM,         TMessage, VacuumDisplay);
	END_MESSAGE_MAP(TFrame);
};
//---------------------------------------------------------------------------
extern PACKAGE THeadsControlFrame *HeadsControlFrame;
//---------------------------------------------------------------------------
#endif
