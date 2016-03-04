//---------------------------------------------------------------------------

#ifndef CalibrationDlgH
#define CalibrationDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include "FEResources.h"

#include "HeatersDefs.h"
#include "GlobalDefs.h"
#include <Graphics.hpp>

//---------------------------------------------------------------------------
class TCalibrationForm : public TForm
{
__published:	// IDE-managed Components
  TBitBtn *CloseBitBtn;
  TTimer *SensorsSamplingTimer;
  TPageControl *PageControl1;
        TTabSheet *HeadsVoltagesTabSheet;
  TBitBtn *VoltageCalibrateBitBtn;
  TBitBtn *VoltageCalibrationSaveBitBtn;
  TBitBtn *VoltageCalibrationStopBitBtn;
  TGroupBox *GroupBox1;
  TPanel *MainVppPanel;
  TTabSheet *WeightSensorsTabSheet;
	TGroupBox *Panel1;
  TLabel *Label9;
  TLabel *Label10;
  TLabel *Label14;
  TLabel *Label15;
  TButton *FullWeightButton;
  TButton *EmptyWeightButton;
  TComboBox *WeightSensorsComboBox;
        TButton *WeightSensorsSaveButton;
  TEdit *FullWeightEdit;
  TPanel *FullWeightPanel;
  TPanel *EmptyWeightPanel;
  TPanel *CurrA2DWeightPanel;
        TTabSheet *YStepsPerPixelTabSheet;
	TGroupBox *Panel2;
        TEdit *CalculateYStepsPerPixelEdit;
        TButton *CalculateYStepsPerPixelButton;
        TLabel *Label3;
        TGroupBox *HeadsVoltagesGroupBox;
        TLabel *ReqVoltLabel;
        TLabel *CurVoltLabel;
	TSpeedButton *WeightSensorsHelpSpeedButton;
	TImage *Image9;
	TImage *Image1;
	TImage *Image2;
	TSpeedButton *YStepsPerPixelSpeedButton;


  void __fastcall FullWeightButtonClick(TObject *Sender);
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall EmptyWeightButtonClick(TObject *Sender);
  void __fastcall WeightSensorsSaveButtonClick(TObject *Sender);
  void __fastcall CloseBitBtnClick(TObject *Sender);
  void __fastcall SensorsSamplingTimerTimer(TObject *Sender);
  void __fastcall FormShow(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall VoltageCalibrateBitBtnClick(TObject *Sender);
  void __fastcall VoltageCalibrationStopBitBtnClick(TObject *Sender);
  void __fastcall VoltageCalibrationSaveBitBtnClick(TObject *Sender);
  void __fastcall HeadVoltageCheckBoxClick(TObject *Sender);
  void __fastcall WeightSensorsComboBoxChange(TObject *Sender);
        void __fastcall WeightSensorsHelpSpeedButtonClick(TObject *Sender);
        void __fastcall CalculateYStepsPerPixelButtonClick(TObject *Sender);
	void __fastcall YStepsPerPixelSpeedButtonClick(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall FullWeightEditKeyPress(TObject *Sender, char &Key);
private:	// User declarations

  
  TLabel* m_HeadsCheckBoxArray[TOTAL_NUMBER_OF_HEADS];
  TEdit*     m_HeadReqVoltEdits  [TOTAL_NUMBER_OF_HEADS];
  TPanel*    m_HeadCurVoltPanels [TOTAL_NUMBER_OF_HEADS];

  Graphics::TBitmap *m_CartridgeImage;

  // Weight sensors calibration
  int   m_FullWeight    [TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE];
  float m_A2DFullWeight [TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE];
  float m_A2DEmptyWeight[TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE];

  // Flags for enabling the save button
  bool m_FullWeightCalibrated;
  bool m_EmptyWeightCalibrated;

  int m_PrevWeightSensorsComboBoxItemIndex;

  // The heads the user selected for voltage calibration
  BYTE m_HeadsForVoltageCalibration;

  // The heads that were successfully calibrated
  BYTE m_VoltageCalibratedHeads;

  // The potentiometer value for each head
  int m_PotentiometerValues[TOTAL_NUMBER_OF_HEADS_HEATERS];

  // Has the new potentiometer values been saved
  bool m_SaveNeededForVoltageCalibration;

  // Flag for stoping the voltage calibration when the user clicks on 'Stop'
  bool m_CancelVoltageCalibration;

  // An array of the requested voltages
  float m_ReqVoltagesModel[TOTAL_NUMBER_OF_HEADS_HEATERS];
  float m_ReqVoltagesSupport[TOTAL_NUMBER_OF_HEADS_HEATERS];
  // Mark that the user selected to calibrate the voltage of this head
  void MarkHeadForVoltageCalibration(int HeadNum, bool On);

  // Check if the user selected to calibrate the voltage of this head
  bool IsHeadForVoltageCalibration(int HeadNum);

  // Display the current voltages
  void DisplayCurrentVoltages(float *Voltages);

  // save the the potentiometer values
  void SavePotentiometrValues();


public:		// User declarations
  __fastcall TCalibrationForm(TComponent* Owner);

};
//---------------------------------------------------------------------------
extern PACKAGE TCalibrationForm *CalibrationForm;
//---------------------------------------------------------------------------
#endif
