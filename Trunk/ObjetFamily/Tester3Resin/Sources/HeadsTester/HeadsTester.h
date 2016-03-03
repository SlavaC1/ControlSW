//---------------------------------------------------------------------------

#ifndef HeadsTesterH
#define HeadsTesterH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include "EdenProtocolEngine.h"
#include "OHDBProtocolClient.h"
#include <ActnList.hpp>
#include <ImgList.hpp>
#include <Dialogs.hpp>

#define	PTRN_SIZE       96
#define NUM_OF_HEADS    8
#define PTRN_STR_SIZE   (3 * PTRN_SIZE)
//---------------------------------------------------------------------------

struct TXilinxCommand{
    BYTE Address;
    WORD Data;
};


class THeadsTesterForm : public TForm
{
__published:	// IDE-managed Components
  TPanel *Panel1;
  TLabel *Label4;
  TPanel *NozzelsName;
  TGroupBox *GroupBox1;
  TRadioButton *NumOfFiresRadioButton;
  TRadioButton *FireTimeRadioButton;
  TRadioButton *ContinuousFireRadioButton;
  TRadioButton *FireCycleRadioButton;
  TEdit *NumOfFiresEdit;
  TEdit *FireTimeEdit;
  TEdit *CycleOnTimeEdit;
  TEdit *CycleOffTimeEdit;
  TEdit *CycleTotalTimeEdit;
  TLabel *Label6;
  TLabel *Label7;
  TGroupBox *GroupBox2;
  TLabel *Label1;
  TEdit *PulseDelay;
  TLabel *Label2;
  TEdit *PulseWidth;
  TLabel *Label3;
  TEdit *FireFrequency;
  TPanel *Panel2;
  TImageList *ImageList1;
  TActionList *ActionList1;
  TAction *PrintAction;
  TAction *StopAction;
  TLabel *Label5;
  TEdit *PostPulseEdit;
        TOpenDialog *LoadPatternOpenDialog;
        TSaveDialog *SavePatternSaveDialog;
        TTimer *PrintTimeTimer;
        TPanel *Panel3;
        TPanel *Panel4;
        TSpeedButton *PrintButton;
        TBitBtn *ClearAllButton;
        TBitBtn *SetAllButton;
        TBitBtn *SavePatternButton;
        TBitBtn *LoadPatternButton;
        TBitBtn *DownloadButton;
        TStatusBar *HeadsTesterStatusBar;
        TProgressBar *DownloadingProgressBar;
        TPanel *PrintTimePanel;
        TLabeledEdit *PrintTimeEdit;
        TLabel *Label8;
        TLabel *Label9;
  void __fastcall DownloadButtonClick(TObject *Sender);
  void __fastcall ClearClick(TObject *Sender);
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall FormShow(TObject *Sender);
  void __fastcall PrintActionExecute(TObject *Sender);
  void __fastcall StopActionExecute(TObject *Sender);
  void __fastcall FireOptionsRadioButtonClick(TObject *Sender);
        void __fastcall SavePatternButtonClick(TObject *Sender);
        void __fastcall LoadPatternButtonClick(TObject *Sender);
        void __fastcall PrintTimeTimerTimer(TObject *Sender);
        void __fastcall SetAllButtonClick(TObject *Sender);
        void __fastcall ParameterChange(TObject *Sender);
private:	// User declarations

  // The structure of the the nozzles test algorithm
  enum PRINT_COMMAND{
    ZERO_CTRL_REG_INDEX = 0,
    ZERO_DIAG_REG_INDEX = 1,
    HEAD_DELAY_1_INDEX = 2,
    HEAD_DELAY_2_INDEX = 3,
    HEAD_DELAY_3_INDEX = 4,
    HEAD_DELAY_4_INDEX = 5,
    HEAD_DELAY_5_INDEX = 6,
    HEAD_DELAY_6_INDEX = 7,
    HEAD_DELAY_7_INDEX = 8,
    HEAD_DELAY_8_INDEX = 9,
    PRE_PULSER_INDEX = 10,
    POST_PULSER_INDEX = 11,
    PULSE_WITDH_DELAY_INDEX = 12,
    FIRE_FREQUENCY_INDEX = 13,
    NUM_OF_FIRES_INDEX = 14,
    POLARITY_INDEX = 15,
    DIAG_REG_INDEX = 16,
    HW_RESET_INDEX = 17,
    STATE_MACHINE_RESET_INDEX = 18,
    START_PEG_INDEX = 19,
    END_PEG_INDEX = 20,
    CONFIG_REG_INDEX = 21,
    PATTERN_TEST_INDEX = 22,
    PRINT_COMMAND_LENGTH = PATTERN_TEST_INDEX + PTRN_SIZE /*+ 1*/,
    };

  enum TFireOptions {foNumberOfFires, foFireTime, foContinuousFire, foFireCycle};

  TFireOptions m_FireOption;

  void __fastcall SetButtonEvent(TObject *Sender);
  void __fastcall SetCheckboxEvent(TObject *Sender);
  void __fastcall EditCheckbox(bool,int,int);
  void  UpdateFields();
  void WriteDataToXilinx(BYTE Address, WORD Data);
  void InitPrintCommand();
  TXilinxCommand XilinxAddrData(BYTE Addr,WORD Data);



  // the protocol engine
  CEdenProtocolEngine *m_ProtocolEngine;

  // the protocol client
  COHDBProtocolClient *m_ProtocolClient;

  CQEvent m_XilinxWriteEvent;

  TXilinxCommand m_PrintCommand[PRINT_COMMAND_LENGTH];

  bool m_StopPrinting;

  void LoadingUpdateFields ();

//---------------------------------------------------------------------------


public:		// User declarations
  __fastcall THeadsTesterForm(TComponent* Owner);
  void Open(CEdenProtocolEngine *ProtocolEngine);

 static void AckHandler(int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie);

  BYTE PatternArray [PTRN_SIZE];
	int m_PrintTime;

  int ColumnIndex[NUM_OF_HEADS];

};
//---------------------------------------------------------------------------
extern PACKAGE THeadsTesterForm *HeadsTesterForm;
//---------------------------------------------------------------------------
#endif
