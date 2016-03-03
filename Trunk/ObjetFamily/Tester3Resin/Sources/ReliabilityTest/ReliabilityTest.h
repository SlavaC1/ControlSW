//---------------------------------------------------------------------------

#ifndef ReliabilityTestH
#define ReliabilityTestH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>

#include "OCBProtocolClient.h"
#include "OHDBProtocolClient.h"
#include "OCBCommDefs.h"
#include "OHDBCommDefs.h"

#include <ActnList.hpp>
//---------------------------------------------------------------------------
class TReliabilityTestForm : public TForm
{
__published:	// IDE-managed Components
  TGroupBox *GroupBox1;
  TGroupBox *GroupBox2;
  TLabel *Label1;
  TEdit *PurgeFrequencyEdit;
  TLabel *Label2;
  TLabel *Label3;
  TPanel *PurgeCyclePanel;
  TLabel *Label4;
  TEdit *LowTempEdit;
  TLabel *Label5;
  TLabel *Label6;
  TEdit *HighTempEdit;
  TLabel *Label7;
  TSpeedButton *PurgeTestButton;
  TImageList *ImageList1;
  TSpeedButton *TemperatueTestButton;
  TLabel *Label8;
  TPanel *TemperatureCyclesPanel;
  TTimer *PurgeTimer;
  TTimer *TemperatureTimer;
  TActionList *ActionList1;
  TAction *PurgeStartAction;
  TAction *PurgeStopAction;
  TAction *TemperatureStartAction;
  TAction *TemperatureStopAction;
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall TemperatureStartActionExecute(TObject *Sender);
  void __fastcall TemperatureStopActionExecute(TObject *Sender);
  void __fastcall TemperatureTimerTimer(TObject *Sender);
  void __fastcall PurgeStartActionExecute(TObject *Sender);
  void __fastcall PurgeStopActionExecute(TObject *Sender);
  void __fastcall PurgeTimerTimer(TObject *Sender);
private:	// User declarations
  TOHDBSetHeatersTemperatureMessage m_UpperLimitTempMsg;
  TOHDBSetHeatersTemperatureMessage m_LowerLimitTempMsg;
  THeaterElements m_CurrentTemp;

  bool m_LowLimitNow;
  int m_LowerLimitTemp;
  int m_UpperLimitTemp;
  int m_TemperatureCycle;

  int m_PurgeCycle;
  BYTE m_PurgeStatus;
  int m_PurgeInterval;

  COHDBProtocolClient* m_OHDBProtocolClient;
  COCBProtocolClient* m_OCBProtocolClient;

  CQEvent m_HeatersStatusEvent;
  CQEvent m_SetHeatersEvent;
  CQEvent m_HeatersOnOffEvent;
  CQEvent m_FansEvent;
  CQEvent m_PurgeStatusEvent;
  CQEvent m_PerformPurgeEvent;
  CQEvent m_HeadFillingEvent;

public:		// User declarations
  __fastcall TReliabilityTestForm(TComponent* Owner);
  void Open(CEdenProtocolEngine *OHDBProtocolEngine, CEdenProtocolEngine *OCBProtocolEngine);
  static void OHDBHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
  static void OCBHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);

};
//---------------------------------------------------------------------------
extern PACKAGE TReliabilityTestForm *ReliabilityTestForm;
//---------------------------------------------------------------------------
#endif
