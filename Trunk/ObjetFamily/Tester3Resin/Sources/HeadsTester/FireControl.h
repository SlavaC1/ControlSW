//---------------------------------------------------------------------------

#ifndef FireControlH
#define FireControlH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include "GenFourTesterDefs.h"
#include "OHDBCommDefs.h"
#include "OHDBProtocolClient.h"
#include <ImgList.hpp>

#define WM_GEN4_GET_HEADS_VOLTAGES WM_USER + 70

struct TFireParams
{
    enum TFireControlType
	{
		fcContinuous,
		fcDutyCycle,
		fcSingleBurst
	};

	enum TCountinuousFireType
	{
		cftInfinite,
		cftTime
	};

	TFireParams() : OnFires(0), OffFires(0), Cycles(0), TimeDuration(0), FireControlType(fcContinuous), CountinuousFireType(cftInfinite) {}

	TFireControlType     FireControlType;
	TCountinuousFireType CountinuousFireType;

	unsigned long OnFires;
	unsigned long OffFires;
	int Cycles;
	int TimeDuration;
};

//----------------------------------------------------------------------------------------

struct TPulseParams
{
    enum TPulseType
	{
		pulSingle,
		pulDouble
	};

	TPulseType PulseType;
	double Frequency;

	// Parameters for double pulse
	double FirstPulseFallTime [HEAD_VOLTAGES_NUM];
	double FirstPulseDwellTime[HEAD_VOLTAGES_NUM];
	double SecondPulseWidth   [HEAD_VOLTAGES_NUM];
	double DelayToSecondPulse [HEAD_VOLTAGES_NUM];
};

//---------------------------------------------------------------------------
class TFireControlFrame : public TFrame
{
__published:	// IDE-managed Components
	TGroupBox *FireControlGroupbox;
	TPageControl *FireOptionsPagecontrol;
	TTabSheet *ContinuousTab;
	TTabSheet *DutyCycleTab;
	TTabSheet *SingleBurstTab;
	TEdit *PulseWidthEdit;
	TEdit *FireFreqEdit;
	TLabel *Label2;
	TLabel *Label3;
	TEdit *FiresOnEdit;
	TEdit *FiresOffEdit;
	TEdit *CyclesEdit;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *Label7;
	TRadioGroup *ContinuousRadioGroup;
	TEdit *TimeDurationEdit;
	TEdit *SingleBurstFiresEdit;
	TLabel *Label8;
	TGroupBox *ActivationGroupbox;
	TLabel *ProgressLabel;
	TBitBtn *StartButton;
	TTimer *ClockTimer;
	TPageControl *FireParamsPagecontrol;
	TTabSheet *TabSheet1;
	TTabSheet *TabSheet2;
	TPanel *Panel1;
	TLabel *Label4;
	TLabel *Label11;
	TLabel *Label12;
	TLabel *Label13;
	TEdit *P1_WidthEdit;
	TEdit *P2_WidthEdit;
	TEdit *VoltageProcEdit;
	TImageList *IconsImageList;
	TTimer *CountdownTimer;
	TEdit *IntervalEdit;
	TLabel *Label9;
	TEdit *PropDelayEdit;
	TLabel *Label1;
	TEdit *SlewRateEdit;
	TLabel *Label10;
	void __fastcall EditKeyPressInt(TObject *Sender, char &Key);
	void __fastcall EditKeyPressFloat(TObject *Sender, char &Key);
	void __fastcall StartButtonClick(TObject *Sender);
	void __fastcall StopButtonClick(TObject *Sender);
	void __fastcall ClockTimerTimer(TObject *Sender);
	void __fastcall CountdownTimerTimer(TObject *Sender);
	
private:    // User declarations

	enum TIconTtype
	{
		icoPlayEnabled,
		icoPlayDisabled,
		icoStop
    };

	COHDBProtocolClient *m_OHDBProtocolClient;

	unsigned int m_PrintTimeCounter;

	WORD m_HeadsVolts[HEAD_VOLTAGES_NUM];

	TPulseParams m_PulseParams;
	TFireParams  m_FireParams;

	void WriteToFPGA(BYTE Address, WORD Data);
	void ReadVoltagesFromHeads();
	static void ReadVoltagesHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
	void SaveParams();

public:		// User declarations

	__fastcall TFireControlFrame(TComponent* Owner);
	void LoadFromParams();
	TPulseParams GetPulseParams();
	TFireParams  GetFireParams();
	void SetOHDBProtocolClient(COHDBProtocolClient *OHDBProtocolClient);
	int  ConvertTimeToFPGAClockCounts(double Time, double Clock);
};
//---------------------------------------------------------------------------
extern PACKAGE TFireControlFrame *FireControlFrame;
//---------------------------------------------------------------------------
#endif
