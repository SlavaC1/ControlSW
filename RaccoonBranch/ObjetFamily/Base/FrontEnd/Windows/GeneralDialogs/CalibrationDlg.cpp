//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CalibrationDlg.h"
#include "Q2RTApplication.h"
#include "MachineManager.h"
#include "MachineSequencer.h"
#include "BackEndInterface.h"
#include "QThreadUtils.h"
#include "QMonitor.h"
#include "AppParams.h"
#include "FrontEndParams.h"
#include "AppLogFile.h"
#include "values.h" // needed for MAXFLOAT
#include "math.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#define CONVERT_HEAD_VPP_A2D_TO_VOLT(x) (16 * 2.4 * (x) / 1024)

const int   FULL_TANK_WEIGHT              = 4000; //grams
const int   VOLTAGES_STABILIZATION_TIME   = 150;

// Theoretical distance in Y axis in um of four nozzles
const int   FOUR_NOZZLES_DISTANCE         = 681;
const char  EOL_HELP_FILENAME[]           = "WeightSensorCalibration.chm";
const char  Y_STEPS_PER_PIXEL_HELP_FILE_NAME[] = "YStepsPerPixel.chm";
// this is for to 300DPI. which is the only Y res. used today. (Y-600DPI is done "across two layers")
const float PIXEL_PER_MM                  = 298 / MM_PER_INCH; // 298  = pixels per inch
const float VOLTAGE_TOLERANCE             = 1.0; // Any calibrated value within this tolerance is acceptable. (but see voltage_toleance for finer tunning)
const int CALIBRATION_FAIL_RETRIES        = 3; // number of retries for calibrating head voltage.
const int FORCED_RETRIES                  = 1; // number of times we force the calibration process to "cross" the calibration point.

TCalibrationForm *CalibrationForm;
//---------------------------------------------------------------------------
__fastcall TCalibrationForm::TCalibrationForm(TComponent* Owner)
  : TForm(Owner)
{
  m_HeadsForVoltageCalibration      = 0xFF;
  m_SaveNeededForVoltageCalibration = false;

  FOR_ALL_HEADS(i)
  {
	 m_HeadsCheckBoxArray[i]            = new TLabel(this);
	 m_HeadsCheckBoxArray[i]->Parent    = HeadsVoltagesGroupBox;
     m_HeadsCheckBoxArray[i]->Tag       = i;
	 m_HeadsCheckBoxArray[i]->Left      = ReqVoltLabel->Left+ReqVoltLabel->Width+46*i+5;/*18;*/
	 m_HeadsCheckBoxArray[i]->Top       = ReqVoltLabel->Top+(ReqVoltLabel->Top-CurVoltLabel->Top);/*32*(i+1);*/
	 m_HeadsCheckBoxArray[i]->Caption   = GetHeadName(i).c_str();
	 m_HeadsCheckBoxArray[i]->Width     = 42;
     m_HeadsCheckBoxArray[i]->Height     = 21;

     m_HeadReqVoltEdits[i]              = new TEdit(this);
     m_HeadReqVoltEdits[i]->Parent      = HeadsVoltagesGroupBox;
	 m_HeadReqVoltEdits[i]->Left        = m_HeadsCheckBoxArray[i]->Left;
	 m_HeadReqVoltEdits[i]->Top         = ReqVoltLabel->Top;
	 m_HeadReqVoltEdits[i]->Width       = 42;
	 m_HeadReqVoltEdits[i]->Tag         = i;
     m_HeadReqVoltEdits[i]->Height      = m_HeadsCheckBoxArray[i]->Height;

     m_HeadCurVoltPanels[i]             = new TPanel(this);
     m_HeadCurVoltPanels[i]->Parent     = HeadsVoltagesGroupBox;
	 m_HeadCurVoltPanels[i]->Left       = m_HeadsCheckBoxArray[i]->Left;
	 m_HeadCurVoltPanels[i]->Width      = 42;
     m_HeadCurVoltPanels[i]->Height     = m_HeadsCheckBoxArray[i]->Height;
	 m_HeadCurVoltPanels[i]->Top        = CurVoltLabel->Top;
     m_HeadCurVoltPanels[i]->BevelOuter = bvLowered;
     m_HeadCurVoltPanels[i]->Tag        = i;
  }

  FOR_ALL_HEADS(i)
  {
     m_HeadReqVoltEdits[i]->TabOrder    = i;
     //m_HeadsCheckBoxArray[i]->TabOrder  = i+TOTAL_NUMBER_OF_HEADS;
  }
  #ifdef OBJET_MACHINE
  {
	m_CartridgeImage = new Graphics::TBitmap;
	LOAD_BITMAP(m_CartridgeImage, IDB_CARTIDGE_IMAGE);
	Image9->Picture->Assign(m_CartridgeImage);
  }
  #endif
}

void __fastcall TCalibrationForm::FormCreate(TObject *Sender)
{
  for(int i = FIRST_TANK_INCLUDING_WASTE_TYPE; i < LAST_TANK_INCLUDING_WASTE_TYPE; i++)
      WeightSensorsComboBox->Items->Add(TankToStr((TTankIndex)i).c_str());
  WeightSensorsComboBox->ItemIndex = TYPE_TANK_MODEL1;
}//FormCreate

void __fastcall TCalibrationForm::FormShow(TObject *Sender)
{
  CBackEndInterface* BackEnd        = CBackEndInterface::Instance();
  CFrontEndParams*   FrontEndParams = CFrontEndParams::Instance();
  FOR_ALL_QUALITY_MODES(qm)
      FOR_ALL_OPERATION_MODES(om)
          if (GetModeAccessibility(qm, om) == true)
             BackEnd->EnableConfirmationBypass(PER_MACHINE_MODE[qm][om], MACHINE_QUALITY_MODES_DIR(qm,om));

  SensorsSamplingTimer->Enabled = true;

  int   ADuCurrVoltages        [TOTAL_NUMBER_OF_HEADS_HEATERS];
  int   CurrPotentiometerValues[TOTAL_NUMBER_OF_HEADS_HEATERS];
  float CurrVoltages           [TOTAL_NUMBER_OF_HEADS_HEATERS];
  float ReqVoltages            [TOTAL_NUMBER_OF_HEADS_HEATERS];
  m_CancelVoltageCalibration = false;
  m_FullWeightCalibrated     = false;
  m_EmptyWeightCalibrated    = false;

  BackEnd->GetHeadsPotentiometrValues(CurrPotentiometerValues);
  BackEnd->SetHeadsVoltages(CurrPotentiometerValues, TOTAL_NUMBER_OF_HEADS_HEATERS);
  // Wait for the stabilization of the voltages
  QSleep(VOLTAGES_STABILIZATION_TIME);

  BackEnd->GetHeadsA2DVoltages(ADuCurrVoltages, TOTAL_NUMBER_OF_HEADS_HEATERS);
  for(int i = 0; i < TOTAL_NUMBER_OF_HEADS_HEATERS; i++)
    CurrVoltages[i] = CONVERT_HEAD_VPP_A2D_TO_VOLT(ADuCurrVoltages[i]);
  DisplayCurrentVoltages(CurrVoltages);
  CAppParams *ParamManager = CAppParams::Instance();
 // BackEnd->GetHeadRequestedVoltagesModel(ReqVoltages);
  FOR_ALL_HEADS(i)
  {
  if(ParamManager->IsModelHead(i))
		ReqVoltages[i]           = ParamManager->RequestedHeadVoltagesModel[i];
	else
		ReqVoltages[i]         = ParamManager->RequestedHeadVoltagesSupport[i];
  }
  FOR_ALL_HEADS(i)
  {
  	m_HeadReqVoltEdits[i]->Text = FloatToStrF(ReqVoltages[i],ffFixed,4,2);
  }
  MainVppPanel->Caption = FloatToStrF(BackEnd->GetVppPowerSupply(),ffFixed,4,2);

  WeightSensorsComboBox->ItemIndex     = TYPE_TANK_MODEL1;
  m_PrevWeightSensorsComboBoxItemIndex = -1;

  FullWeightPanel->Caption  = FloatToStrF(FrontEndParams->FullA2DSetupWeightArray[TYPE_TANK_MODEL1],ffFixed,4,1);
  EmptyWeightPanel->Caption = FloatToStrF(FrontEndParams->EmptyA2DSetupWeightArray[TYPE_TANK_MODEL1],ffFixed,4,1);
  FullWeightEdit->Text = "";
  FullWeightButton->Enabled = false;

}
//---------------------------------------------------------------------------

void __fastcall TCalibrationForm::FormClose(TObject *Sender,TCloseAction &Action)
{
  CBackEndInterface* BackEnd = CBackEndInterface::Instance();

  FOR_ALL_QUALITY_MODES(qm)
      FOR_ALL_OPERATION_MODES(om)
          if (GetModeAccessibility(qm, om) == true)
             BackEnd->DisableConfirmationBypass(PER_MACHINE_MODE[qm][om], MACHINE_QUALITY_MODES_DIR(qm,om));

  SensorsSamplingTimer->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TCalibrationForm::FullWeightButtonClick(TObject *Sender)
{
  CFrontEndParams*   FrontEndParams = CFrontEndParams::Instance();
  CBackEndInterface* BackEnd        = CBackEndInterface::Instance();

  int Index                = WeightSensorsComboBox->ItemIndex;
  m_A2DFullWeight[Index]   = BackEnd->GetTankWeightInA2D(Index);
  FullWeightPanel->Caption = FloatToStrF(m_A2DFullWeight[Index],ffFixed,4,1);
  m_FullWeight[Index]      = StrToInt(FullWeightEdit->Text);

  FrontEndParams->FullA2DSetupWeightArray[Index] = m_A2DFullWeight[Index];
  FrontEndParams->FullSetupWeightArray[Index]    = m_FullWeight[Index];

  FrontEndParams->SaveSingleParameter(&FrontEndParams->FullA2DSetupWeightArray);
  FrontEndParams->SaveSingleParameter(&FrontEndParams->FullSetupWeightArray);

  // Enable the save button only if full and empty calibration have been done
  m_FullWeightCalibrated = true;
  if (m_FullWeightCalibrated && m_EmptyWeightCalibrated)
    WeightSensorsSaveButton->Enabled = true;
}


void __fastcall TCalibrationForm::EmptyWeightButtonClick(TObject *Sender)
{
  CBackEndInterface* BackEnd        = CBackEndInterface::Instance();

  int Index                 = WeightSensorsComboBox->ItemIndex;
  m_A2DEmptyWeight[Index]   = BackEnd->GetTankWeightInA2D(Index);
  EmptyWeightPanel->Caption = FloatToStrF(m_A2DEmptyWeight[Index],ffFixed,4,1);

  CFrontEndParams*   FrontEndParams = CFrontEndParams::Instance();
  FrontEndParams->EmptyA2DSetupWeightArray[Index] = m_A2DEmptyWeight[Index];
  FrontEndParams->SaveSingleParameter(&FrontEndParams->EmptyA2DSetupWeightArray);
  
  // Enable the save button only if full and empty calibration have been done
  m_EmptyWeightCalibrated = true;
  if (m_FullWeightCalibrated && m_EmptyWeightCalibrated)
    WeightSensorsSaveButton->Enabled = true;
}//EmptyWeightButtonClick

void __fastcall TCalibrationForm::WeightSensorsSaveButtonClick(TObject *Sender)
{
  CBackEndInterface* BackEnd = CBackEndInterface::Instance();
  int                Index   = WeightSensorsComboBox->ItemIndex;
  float              Gain    = m_FullWeight[Index] / (m_A2DFullWeight[Index] - m_A2DEmptyWeight[Index]);

  BackEnd->SetWeightSensorGain  (Index, Gain);
  BackEnd->SetWeightSensorOffset(Index, - Gain * m_A2DEmptyWeight[Index]);
}//WeightSensorsSaveButtonClick
//---------------------------------------------------------------------------


void __fastcall TCalibrationForm::WeightSensorsComboBoxChange(TObject *Sender)
{
  int              Index          = WeightSensorsComboBox->ItemIndex;

  if (m_PrevWeightSensorsComboBoxItemIndex == Index)
    return;

  m_PrevWeightSensorsComboBoxItemIndex = Index;

  CFrontEndParams* FrontEndParams = CFrontEndParams::Instance();
  FullWeightPanel->Caption  = FloatToStrF(FrontEndParams->FullA2DSetupWeightArray[Index],ffFixed,4,1);
  EmptyWeightPanel->Caption = FloatToStrF(FrontEndParams->EmptyA2DSetupWeightArray[Index],ffFixed,4,1);

  // After a change in the cartridge, the save button is disabled again
  m_FullWeightCalibrated           = false;
  m_EmptyWeightCalibrated          = false;
  WeightSensorsSaveButton->Enabled = false;
}//WeightSensorsComboBoxChange
//---------------------------------------------------------------------------


void __fastcall TCalibrationForm::CloseBitBtnClick(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TCalibrationForm::SensorsSamplingTimerTimer(TObject *Sender)
{
  CBackEndInterface* BackEnd = CBackEndInterface::Instance();
  int                Index   = WeightSensorsComboBox->ItemIndex;

  CurrA2DWeightPanel->Caption = FloatToStrF(BackEnd->GetTankWeightInA2D(Index),ffFixed,4,1);
}//SensorsSamplingTimerTimer
//---------------------------------------------------------------------------

void __fastcall TCalibrationForm::VoltageCalibrateBitBtnClick(TObject *Sender)
{
  CBackEndInterface* BackEnd = CBackEndInterface::Instance();
  CAppParams *ParamManager = CAppParams::Instance();

  if (CHECK_EMULATION(ParamManager->OHDB_Emulation))
  {
	  return;
  }

  float CurrVoltages           [TOTAL_NUMBER_OF_HEADS_HEATERS];
  int   ADuCurrVoltages        [TOTAL_NUMBER_OF_HEADS_HEATERS];
  float PrevVoltages           [TOTAL_NUMBER_OF_HEADS_HEATERS];
  float DisplayVoltages        [TOTAL_NUMBER_OF_HEADS_HEATERS];
  int   PrevPotentiometerValues[TOTAL_NUMBER_OF_HEADS_HEATERS];
  float PrevDeviations         [TOTAL_NUMBER_OF_HEADS_HEATERS];
  float deviations             [TOTAL_NUMBER_OF_HEADS_HEATERS];
  float bestDeviations         [TOTAL_NUMBER_OF_HEADS_HEATERS];
  int CalibrationPointCrossingsCounters[TOTAL_NUMBER_OF_HEADS_HEATERS];
  float voltage_tolerances     [TOTAL_NUMBER_OF_HEADS_HEATERS];
  bool HeadCalibrated          [TOTAL_NUMBER_OF_HEADS_HEATERS];

  try
  {
    // Disable the save button
    VoltageCalibrationSaveBitBtn->Enabled = false;
    VoltageCalibrationStopBitBtn->Enabled = true;
    m_CancelVoltageCalibration            = false;
    m_VoltageCalibratedHeads              = 0;
    m_HeadsForVoltageCalibration          = 0;
    //Set Head min voltages
    BackEnd->B_End_SetHeadMinVoltage();
    // Get the previuos potentiomemter values
    BackEnd->GetHeadsPotentiometrValues(PrevPotentiometerValues);
    FOR_ALL_HEADS(i)
    {
        if (m_HeadReqVoltEdits[i]->Enabled)
        {
          try
          {                           
            float Value = StrToFloat(m_HeadReqVoltEdits[i]->Text);


            float min_limit = ParamManager->HSW_MinVoltageArray[i].Value();
	    float max_limit = ParamManager->HSW_MaxVoltageArray[i].Value();
            CQLog::Write(LOG_TAG_HEAD_HEATERS,"EEPROM Info: The value of Head Num %d is %.2f, the value should be between %.2f to %.2f", i, Value,min_limit ,max_limit);
            CHECK_REQUESTED_VOLTAGE_LIMITS(Value, EQException, i, min_limit,max_limit);

            m_ReqVoltagesModel[i] = Value;
            MarkHeadForVoltageCalibration(i, true);
	  }
          catch (Exception &exception)
          {
            throw EQException("The value entered is not valid");
          }
        }
        else
        {
          m_PotentiometerValues[i] = PrevPotentiometerValues[i];
        }

        // Calculate the potentiometer values
        if (IsHeadForVoltageCalibration(i))
	{
        		m_PotentiometerValues[i] = BackEnd->CalculatePotValue(i,m_ReqVoltagesModel[i]);
        }
		
		PrevDeviations[i]     = MAXFLOAT;
		PrevVoltages[i]       = 0.0;
		deviations[i]         = 0.0;
		bestDeviations[i]     = 0.0;			
		voltage_tolerances[i] = 0.0;
		HeadCalibrated[i]     = false;
			
		CalibrationPointCrossingsCounters[i] = 0;
    }

    while (m_HeadsForVoltageCalibration !=  0)
    {
      if (m_CancelVoltageCalibration)
        return;

      BackEnd->SetHeadsVoltages(m_PotentiometerValues, TOTAL_NUMBER_OF_HEADS_HEATERS);

      // Wait for the stabilization of the voltages
      QSleep(VOLTAGES_STABILIZATION_TIME);

      // Get the current voltage
      BackEnd->GetHeadsA2DVoltages(ADuCurrVoltages, TOTAL_NUMBER_OF_HEADS_HEATERS);

      for(BYTE i = 0; i < TOTAL_NUMBER_OF_HEADS_HEATERS; i++)
      {
        if (IsHeadForVoltageCalibration(i))
        {
			CurrVoltages[i] = DisplayVoltages[i] = CONVERT_HEAD_VPP_A2D_TO_VOLT(ADuCurrVoltages[i]);

			if (0.0 == PrevVoltages[i]) // on first iteration initialize PrevVoltages[]:
				PrevVoltages[i] = CurrVoltages[i];

			voltage_tolerances[i]  = 2 * fabs((BackEnd->EstimateHeadVoltage(i, m_PotentiometerValues[i]) -  BackEnd->EstimateHeadVoltage(i, m_PotentiometerValues[i] + 1)));

			deviations[i] = fabs(m_ReqVoltagesModel[i] - CurrVoltages[i]);

			if (((m_ReqVoltagesModel[i] - CurrVoltages[i]) * (m_ReqVoltagesModel[i] - PrevVoltages[i])) <= 0) // Stopping condition: if we have just passed the m_ReqVoltages[i] point.
			{
				bestDeviations[i] = deviations[i];
				
				if (PrevDeviations[i] < deviations[i])							
					bestDeviations[i] = PrevDeviations[i];
				
				if (bestDeviations[i] > voltage_tolerances[i]) // Detect potential HW *minor* problems.
				{							
					// Detect potential HW *major* problems: could not reach a voltage deviation below HEAD_VOLTAGE_TOLERANCE
					if (CalibrationPointCrossingsCounters[i] > CALIBRATION_FAIL_RETRIES)
					{
						if (bestDeviations[i] > VOLTAGE_TOLERANCE)
						{						
							QString Output = QFormatStr("Retry number: %s. Could't calibrate the %s head: could't reach a voltage deviation below %s V",IntToStr(CalibrationPointCrossingsCounters[i]).c_str(), 
																																						GetHeadName(i).c_str(), 
																																						FloatToStr(VOLTAGE_TOLERANCE).c_str());						
							
							CQLog::Write(LOG_TAG_GENERAL, Output);
							throw EQException(Output);
						}
						else
						{
							 // "not so Successfully" finished calibrating this head's voltage, but still.
							HeadCalibrated[i] = true;							
						}
					}							
				}
				else			
				{		
					// Successfully finished calibrating this head's voltage.
					if (CalibrationPointCrossingsCounters[i] >= FORCED_RETRIES)
						HeadCalibrated[i] = true;								 
				}
				
				if (HeadCalibrated[i])
				{
					if (PrevDeviations[i] < deviations[i])
					{
						m_PotentiometerValues[i] = PrevPotentiometerValues[i];
						BackEnd->SetHeadsVoltages(m_PotentiometerValues, TOTAL_NUMBER_OF_HEADS_HEATERS);
						DisplayVoltages[i] = PrevVoltages[i];							
					}							

					DisplayCurrentVoltages(DisplayVoltages);
					MarkHeadForVoltageCalibration(i,false);
					m_VoltageCalibratedHeads |= (1 << i);
												
					continue;							
				}
			
			CalibrationPointCrossingsCounters[i]++;
			} // if - stopping condition								
				
		PrevDeviations[i] = deviations[i];

		PrevPotentiometerValues[i] = m_PotentiometerValues[i];
		PrevVoltages[i] = CurrVoltages[i];					
		
		if (m_ReqVoltagesModel[i] < CurrVoltages[i])
		{
			m_PotentiometerValues[i]--;
			
			// if trying to put negative value...
			if(m_PotentiometerValues[i] < 0)
                m_PotentiometerValues[i] = 0;
				//throw EQException(QFormatStr("Couldn't calibrate the %s head: Potentiometer value reached its low limit.", GetHeadName(i).c_str()));
		}
		else
		{
			m_PotentiometerValues[i]++;
		
			// if trying to put larger then BYTE values...
			if(m_PotentiometerValues[i] > 255)
				throw EQException(QFormatStr("Couldn't calibrate the %s head: Potentiometer value reached its high limit.", GetHeadName(i).c_str()));
		}				
		
		DisplayCurrentVoltages(DisplayVoltages);
				} //if for calibration
			} //for
			
			DisplayCurrentVoltages(DisplayVoltages);
		} //while

    // Enable the save button
    VoltageCalibrationSaveBitBtn->Enabled = true;
    m_SaveNeededForVoltageCalibration     = true;
    VoltageCalibrationStopBitBtn->Enabled = false;

  }
  catch(EQException& Exception)
  {
    QMonitor.ErrorMessage(Exception.GetErrorMsg());
  }

}
//---------------------------------------------------------------------------

void __fastcall TCalibrationForm::VoltageCalibrationStopBitBtnClick(TObject *Sender)
{
  m_CancelVoltageCalibration = true;
  VoltageCalibrationStopBitBtn->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TCalibrationForm::VoltageCalibrationSaveBitBtnClick(TObject *Sender)
{
  try
  {
    VoltageCalibrationSaveBitBtn->Enabled = false;
    VoltageCalibrationStopBitBtn->Enabled = false;
    SavePotentiometrValues();
  }
  catch(EQException& Err)
  {
    QMonitor.ErrorMessage(Err.GetErrorMsg());
  }
}
//---------------------------------------------------------------------------


void __fastcall TCalibrationForm::HeadVoltageCheckBoxClick(TObject *Sender)
{
  TCheckBox* CheckBox = dynamic_cast<TCheckBox*> (Sender);
  if (CheckBox)
  {
      m_HeadReqVoltEdits[CheckBox->Tag]->Enabled  = CheckBox->Checked;
      m_HeadCurVoltPanels[CheckBox->Tag]->Enabled = CheckBox->Checked;
  }
}
//---------------------------------------------------------------------------

void TCalibrationForm::SavePotentiometrValues()
{
  CAppParams *ParamManager = CAppParams::Instance();

  try
  {
    for (int i = 0; i < TOTAL_NUMBER_OF_HEADS_HEATERS; i++)
    {
      if (m_VoltageCalibratedHeads & (1 << i))
      {
        ParamManager->PotentiometerValues[i] = m_PotentiometerValues[i];
 //		ParamManager->RequestedHeadVoltagesModel[i] = m_ReqVoltagesModel[i];
    	 if(ParamManager->IsModelHead(i))
	 {
         	ParamManager->RequestedHeadVoltagesModel[i] = m_ReqVoltagesModel[i];
         }
         else
	 {
             ParamManager->RequestedHeadVoltagesSupport[i] = m_ReqVoltagesModel[i];
         }

      }
    }
	ParamManager->SaveSingleParameter(&ParamManager->PotentiometerValues);
	ParamManager->SaveSingleParameter(&ParamManager->RequestedHeadVoltagesModel);
    ParamManager->SaveSingleParameter(&ParamManager->RequestedHeadVoltagesSupport);
    m_SaveNeededForVoltageCalibration  = false;
  }
  catch(EQException& Exception)
  {
    QMonitor.ErrorMessage(Exception.GetErrorMsg());
  }
}

// Display the current voltages
void TCalibrationForm::DisplayCurrentVoltages(float *Voltages)
{
  FOR_ALL_HEADS(i)
    if(m_HeadCurVoltPanels[i]->Enabled)
     m_HeadCurVoltPanels[i]->Caption = FloatToStrF(Voltages[i],ffFixed,4,2);
  Application->ProcessMessages();
}

// Mark that the user selected to calibrate this head
void TCalibrationForm::MarkHeadForVoltageCalibration(int HeadNum, bool On)
{
  if (On)
    m_HeadsForVoltageCalibration |= (1 << HeadNum);
  else
    m_HeadsForVoltageCalibration &= ~(1 << HeadNum);
}

// Check if the user selected to calibrate this head
bool TCalibrationForm::IsHeadForVoltageCalibration(int HeadNum)
{
  return (m_HeadsForVoltageCalibration & (1 << HeadNum));
}

void __fastcall TCalibrationForm::WeightSensorsHelpSpeedButtonClick(TObject *Sender)
{
   Application->HelpFile = (Q2RTApplication->AppFilePath.Value() + LOAD_QSTRING(IDS_HELP_DIR) + EOL_HELP_FILENAME).c_str();
   Application->HelpContext(10);
}
//---------------------------------------------------------------------------

void __fastcall TCalibrationForm::CalculateYStepsPerPixelButtonClick(TObject *Sender)
{
  try
  {
    CAppParams *ParamManager = CAppParams::Instance();
    float Delta     = StrToFloat(CalculateYStepsPerPixelEdit->Text) - FOUR_NOZZLES_DISTANCE;
    float Factor    = Delta / MICRON_IN_HEAD_Y;
    float CurrValue = ParamManager->YStepsPerPixel;
    float NewValue  = CurrValue - (CurrValue * Factor);

    if (QMonitor.AskYesNo(QFormatStr("Current value: %.4f , New value: %.4f.\r\nSave parameter?", CurrValue, NewValue)))
    {
      // Assign new value to the parameter and save it
      ParamManager->YStepsPerPixel = NewValue;
      ParamManager->SaveSingleParameter(&ParamManager->YStepsPerPixel);

      // Read back the YStepsPerPixel back from PM, since it is truncated, and we want the StepsPerMM to match it.
      NewValue = ParamManager->YStepsPerPixel;

      // Also update StepsPerMM[Axis Y] according to the new value.
      ParamManager->StepsPerMM[AXIS_Y] = NewValue * PIXEL_PER_MM;
      ParamManager->SaveSingleParameter(&ParamManager->StepsPerMM);
    }
  }
  catch(...)
  {
    QMonitor.ErrorMessage("Invalid value inserted (must be a number)");
  }
}
//---------------------------------------------------------------------------



void __fastcall TCalibrationForm::YStepsPerPixelSpeedButtonClick(
      TObject *Sender)
{
  Application->HelpFile = (Q2RTApplication->AppFilePath.Value() + LOAD_QSTRING(IDS_HELP_DIR) + Y_STEPS_PER_PIXEL_HELP_FILE_NAME).c_str();
  Application->HelpContext(10);
}
//---------------------------------------------------------------------------

void __fastcall TCalibrationForm::FormDestroy(TObject *Sender)
{
	Q_SAFE_DELETE(m_CartridgeImage);	
}
//---------------------------------------------------------------------------

void __fastcall TCalibrationForm::FullWeightEditKeyPress(TObject *Sender,
      char &Key)
{
  FullWeightButton->Enabled = true;
}
//---------------------------------------------------------------------------

