//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "HeadsControl.h"
#include "QThreadUtils.h"
#include "AppLogFile.h"
#undef MAXINT
#include "values.h"


#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"

#pragma package(smart_init)
#pragma resource "*.dfm"

// Voltage calibration constants 
const int VOLTAGES_STABILIZATION_TIME = 150;
const int MIN_ALLOWED_VOLTAGE         = 14;
const int MAX_ALLOWED_VOLTAGE         = 33;
const double VOLTAGE_TOLERANCE        = 1.0; // Any calibrated value within this tolerance is acceptable. (but see VoltTolerances for finer tuning)
const int CALIBRATION_FAIL_RETRIES    = 3;   // Number of retries for calibrating head voltage.
const int FORCED_RETRIES              = 1;   // Number of times we force the calibration process to "cross" the calibration point.

THeadsControlFrame *HeadsControlFrame;

//---------------------------------------------------------------------------
__fastcall THeadsControlFrame::THeadsControlFrame(TComponent* Owner)
	: TFrame(Owner)
{
	m_OHDBProtocolClient = NULL;
	m_DuringHeating      = false;
	
	// All controls, with VoltagesGroupbox as a Parent
	for(int i = 0; i < VoltagesGroupbox->ControlCount; i++)
	{
		TEdit  *Edit  = dynamic_cast<TEdit *>(VoltagesGroupbox->Controls[i]);
		TPanel *Panel = dynamic_cast<TPanel *>(VoltagesGroupbox->Controls[i]);

		if(Edit)
		{
			Edit->OnKeyPress = EditKeyPressFloat;
			Edit->ShowHint   = true;
			m_VoltagesInputValues[Edit->Tag] = Edit;
		}

		if(Panel)
		{
			if(Panel->Tag < 20) // To differentiate between A2D and Volts panels
				m_VoltageReadingA2DPanels[Panel->Tag] = Panel;
			else
                m_VoltageReadingVoltsPanels[Panel->Tag - 20] = Panel;
		}
	}

	// All controls, with TemperatureGroupbox as a Parent
	for(int i = 0; i < TemperatureGroupbox->ControlCount; i++)
	{
		TEdit     *Edit  = dynamic_cast<TEdit *>(TemperatureGroupbox->Controls[i]);
		TPanel    *Panel = dynamic_cast<TPanel *>(TemperatureGroupbox->Controls[i]);

		if(Edit)
		{
			Edit->OnKeyPress = EditKeyPressInt;
			m_HeatersInputValues[Edit->Tag] = Edit;
		}

		if(Panel)
		{
            Panel->ParentBackground            = false;
			m_HeaterReadingsPanels[Panel->Tag] = Panel;
		}
	}

	// Allowed range for input voltages. Currently we use static input voltage range values,
    // but the calculation of dynamic range is still here for the future use
	SetInputVoltageRangeButton->Visible = false;
	for(int i = 0; i < HEAD_VOLTAGES_NUM; i++)
	{
		m_HeadsVoltageRange[i].Min = MIN_ALLOWED_VOLTAGE;
		m_HeadsVoltageRange[i].Max = MAX_ALLOWED_VOLTAGE;

		m_VoltagesInputValues[i]->Hint = "Range: " + FloatToStrF(m_HeadsVoltageRange[i].Min, ffFixed, 4, 2) + 
										 " < X < " + FloatToStrF(m_HeadsVoltageRange[i].Max, ffFixed, 4, 2);
	}

	SameValueForHeadHalvesCheckbox->Checked = false;
}
//--------------------------------------------------------------------------- 

// Allow only ints at input
void __fastcall THeadsControlFrame::EditKeyPressInt(TObject *Sender, char &Key)
{
	if((Key >= '0') && (Key <= '9'))
		return;

	if(Key == VK_ESCAPE || Key == VK_BACK || Key == VK_RETURN)
		return;

	Key = 0;
}

// Allow only floats at input
void __fastcall THeadsControlFrame::EditKeyPressFloat(TObject *Sender, char &Key)
{
	TEdit *Edit = dynamic_cast<TEdit *>(Sender);

	if(Key == DecimalSeparator)
    {
		if(Edit->Text.Pos(DecimalSeparator) != 0)
			Key = 0;

        return;
	}

	EditKeyPressInt(Sender, Key);
}

void THeadsControlFrame::StatusHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
    THeadsControlFrame *InstancePtr = reinterpret_cast<THeadsControlFrame *>(Cockie);

    // Build the read data message
    TOHDBGenericMessage *Response = static_cast<TOHDBGenericMessage *>(Data);

    switch(Response->MessageID)
    {
		case OHDB_PRINTING_HEADS_VOLTAGES:
		{
			TOHDBPrintingHeadsVoltagesResponse *HeadsVoltages = static_cast<TOHDBPrintingHeadsVoltagesResponse *>(Data);
			memcpy(InstancePtr->m_HeadsVoltages, HeadsVoltages->HeadsVoltages, HEAD_VOLTAGES_NUM * sizeof(WORD));
			PostMessage(InstancePtr->Handle, WM_GEN4_HEADS_VOLTAGES, reinterpret_cast<int>(0), reinterpret_cast<int>(0));
			break;
		}

		case OHDB_HEADS_TEMPERATURE_STATUS:
		{
			TOHDBHeadsTemperatureStatusResponse *HeadsHeaters = static_cast<TOHDBHeadsTemperatureStatusResponse *>(Data);
			memcpy(InstancePtr->m_HeadsHeaters, HeadsHeaters->CurrentElements.HeaterTable, HEATERS_NUM * sizeof(WORD));
			PostMessage(InstancePtr->Handle, WM_GEN4_HEADS_HEATERS, reinterpret_cast<int>(0), reinterpret_cast<int>(0));
			break;
		}

		case OHDB_POWER_SUPPLIES_VOLTAGES:
		{
			TOHDBPowerSuppliesVoltagesMessage *PowerSuppliesVoltagesResponse = static_cast<TOHDBPowerSuppliesVoltagesMessage *>(Data);
			InstancePtr->m_VppPowerSupplyValue = PowerSuppliesVoltagesResponse->VppPowerSupply;
			PostMessage(InstancePtr->Handle, WM_GEN4_PS_VOLTAGES, reinterpret_cast<int>(0), reinterpret_cast<int>(0));
			break;
		}

		case OHDB_VACUUM_STATUS:
		{
			TOHDBHeadsVacuumStatusResponse *HeadsVacuumStatusResponse = static_cast<TOHDBHeadsVacuumStatusResponse *>(Data);
			InstancePtr->m_VacuumSensorValue = HeadsVacuumStatusResponse->ModelVacuumSensorValue;
			PostMessage(InstancePtr->Handle, WM_GEN4_VACUUM, reinterpret_cast<int>(0), reinterpret_cast<int>(0));
			break;
		}
				
		default:
			return;
    }
}

// Display the heads voltages
MESSAGE void THeadsControlFrame::HeadsVoltagesDisplay(TMessage& Message)
{
	for(int i = 0; i < HEAD_VOLTAGES_NUM; i++)
	{
		m_VoltageReadingA2DPanels[i]->Caption   = m_HeadsVoltages[i];
		m_VoltageReadingVoltsPanels[i]->Caption = FloatToStrF(CONVERT_HEAD_VPP_A2D_TO_VOLT(m_HeadsVoltages[i]), ffFixed, 4, 2);
	}
}

// Display the heads heaters
MESSAGE void THeadsControlFrame::HeadsHeatersDisplay(TMessage& Message)
{
	QString str = "Heaters: ";

	// Update the heads heaters display
	for(int i = 0; i < HEATERS_NUM; i++)
	{
		m_HeaterReadingsPanels[i]->Caption = m_HeadsHeaters[i];
		str += QIntToStr(m_HeadsHeaters[i]) + ", ";

		if(m_DuringHeating)
		{
            // Lower value means higher temperature
			int LowValue  = m_HeatersInputValues[i]->Text.ToInt() + LowThresholdEdit->Text.ToInt();
			int HighValue = m_HeatersInputValues[i]->Text.ToInt() - HighThresholdEdit->Text.ToInt();

			if(m_HeadsHeaters[i] < LowValue && m_HeadsHeaters[i] > HighValue)
			{
				m_HeaterReadingsPanels[i]->Color       = clLime;
				m_HeaterReadingsPanels[i]->Font->Color = clBlack;
			}
			else
			{
				m_HeaterReadingsPanels[i]->Color       = clRed;
				m_HeaterReadingsPanels[i]->Font->Color = clWhite;
            }
		}
		else
		{
			m_HeaterReadingsPanels[i]->Color       = clBtnFace;
			m_HeaterReadingsPanels[i]->Font->Color = clBlack;
		}
	}

	DEL_LAST_2_CHARS(str); 
	CQLog::Write(JS_LOG_TAG_HEATING, str.c_str());
}

// Display Vpp
MESSAGE void THeadsControlFrame::VppDisplay(TMessage& Message)
{
	float VoltVpp = CONVERT_VPP_A2D_TO_VOLT(m_VppPowerSupplyValue);
	VppVoltagePanel->Caption = FloatToStrF(VoltVpp, ffFixed, 4, 2);
}

// Display Vacuum
MESSAGE void THeadsControlFrame::VacuumDisplay(TMessage& Message)
{
	VacuumReading->Caption = IntToStr(m_VacuumSensorValue);
}

void THeadsControlFrame::SetOHDBProtocolClient(COHDBProtocolClient *OHDBProtocolClient)
{
	m_OHDBProtocolClient = OHDBProtocolClient;
}

void THeadsControlFrame::SetOCBProtocolClient(COCBProtocolClient *OCBProtocolClient)
{
	m_OCBProtocolClient = OCBProtocolClient;
}

// Heads heaters control
// ----------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------

void __fastcall THeadsControlFrame::SetHeatersValuesButtonClick(TObject *Sender)
{
	CAppParams *ParamsMgr = CAppParams::GetInstance();

	TOHDBSetHeatersTemperatureMessage Msg;
	Msg.MessageID = OHDB_SET_HEATERS_TEMPERATURE;

	int HeadIndex = 0;

	for(int i = 0; i < HEATERS_NUM; i++)
	{
		// Odd and Even halves of the head get the same value
		if(SameValueForHeadHalvesCheckbox->Checked)
		{
			if(i < HEADS_NUM * NOZZLE_LINES_PER_HEAD)
			{
				if(i % 2 == 0)
					HeadIndex = i;
			}
			else
			{
				HeadIndex = i;
			}
		}
		else
		{
        	HeadIndex = i;
        }

        // Display the same value for the both halves in UI
		m_HeatersInputValues[i]->Text  = m_HeatersInputValues[HeadIndex]->Text;
		m_HeatersInputValues[i]->Refresh();
		
		Msg.SetElements.HeaterTable[i] = ParamsMgr->GenFourHeaters[i] = static_cast<WORD>(m_HeatersInputValues[HeadIndex]->Text.ToInt());
	}

	Msg.LowThreshold       = ParamsMgr->LowThresholdTemp   = static_cast<WORD>(LowThresholdEdit->Text.ToInt());
	Msg.BlockLowThreshold  =                                 static_cast<WORD>(LowThresholdEdit->Text.ToInt());
	Msg.HighThreshold      = ParamsMgr->HighThresholdTemp  = static_cast<WORD>(HighThresholdEdit->Text.ToInt());
	Msg.BlockHighThreshold =                                 static_cast<WORD>(HighThresholdEdit->Text.ToInt());
	Msg.HeatingRate        = ParamsMgr->GenFourHeatingRate = static_cast<WORD>(HeatingRateEdit->Text.ToInt());
	Msg.KeepHeatingEvenOpenOrShortCircuit                  = static_cast<BYTE>(0);
	Msg.HeatingWatchdogSamplingInterval                    = static_cast<BYTE>(ParamsMgr->HeatingWatchdogSamplingInterval.Value());

	if(m_OHDBProtocolClient->SendInstallWaitReply(&Msg, sizeof(TOHDBSetHeatersTemperatureMessage), NULL, reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
		throw Exception("SetActuator: Tester did not recieve Ack");

	ParamsMgr->SaveAll();	
}

void __fastcall THeadsControlFrame::TurnHeatersOnButtonClick(TObject *Sender)
{
	TButton *btn = dynamic_cast<TButton *>(Sender);
	if(! btn) return;

    // In case that user forgot to set the values before turning ON the heaters
	SetTempValuesButton->Click();

	ActivateHeaters(true);
}

void __fastcall THeadsControlFrame::TurnHeatersOffButtonClick(TObject *Sender)
{
    TButton *btn = dynamic_cast<TButton *>(Sender);
	if(! btn) return;

	ActivateHeaters(false);
}

void THeadsControlFrame::ActivateHeaters(bool State)
{
	// Initialize the heaters to be all on
	WORD HeatersMask = 0x1FFF;

	TOHDBSetHeadHeaterOnOffMessage Msg;
    Msg.MessageID   = OHDB_SET_HEAD_HEATER_ON_OFF;
    Msg.OnOff       = (State ? 1 : 0);
	Msg.HeatersMask = HeatersMask;

	if (m_OHDBProtocolClient->SendInstallWaitReply(&Msg, sizeof(TOHDBSetHeatersTemperatureMessage), NULL, reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
		throw Exception("SetActuator: Tester did not receive Ack");

	m_DuringHeating = State;	
}



// Heads voltage control
// ----------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------


void __fastcall THeadsControlFrame::CalibrateVoltageButtonClick(TObject *Sender)
{
	TButton *btn = dynamic_cast<TButton *>(Sender);
	if(! btn) return;

	btn->Enabled = false;

	try
	{
		CalibrateVoltages();
	}
	__finally
	{
		btn->Enabled = true;
	}
}

void THeadsControlFrame::CalibrateVoltages()
{
    CAppParams *ParamsMgr = CAppParams::GetInstance();

	int PotValues        [HEAD_VOLTAGES_NUM] = {0};
	int PrevPotValues    [HEAD_VOLTAGES_NUM] = {0};
	double CurrVoltages  [HEAD_VOLTAGES_NUM] = {0.0};
	double PrevVoltages  [HEAD_VOLTAGES_NUM] = {0.0};
	double VoltTolerances[HEAD_VOLTAGES_NUM] = {0.0};
	double Deviations    [HEAD_VOLTAGES_NUM] = {0.0};
	double BestDeviations[HEAD_VOLTAGES_NUM] = {0.0};	
	double PrevDeviations[HEAD_VOLTAGES_NUM] = {MAXDOUBLE};
	bool   HeadCalibrated[HEAD_VOLTAGES_NUM] = {false};	
	
	int CalibrationPointCrossingsCounters[HEAD_VOLTAGES_NUM] = {0};
	
	m_HeadsForVoltageCalibration = 0xFF;

	for(int i = 0; i < HEAD_VOLTAGES_NUM; i++)
	{
		PrevPotValues[i] = m_VoltageReadingA2DPanels[i]->Caption.ToInt();

        // Do not calibrate disconnected heads
		if(PrevPotValues[i] == 0 || (m_HeadsVoltageRange[i].Max == 0 && m_HeadsVoltageRange[i].Min == 0))
        	MarkHeadForVoltCal(i, false);
	}

	VoltsToPotsCheckRange(PotValues);     // throws
	
	while(m_HeadsForVoltageCalibration != 0)
	{
		SendPotValuesToHeads(PotValues);      // Set initial potentiometer values to heads
		QSleep(VOLTAGES_STABILIZATION_TIME);  // Wait for voltage stabilization

		for(int i = 0; i < HEAD_VOLTAGES_NUM; i++)
		{
			if(! IsHeadForVoltCal(i))
				continue;

			double RequestedVoltage = m_VoltagesInputValues[i]->Text.ToDouble();
			
			CurrVoltages[i] = m_VoltageReadingVoltsPanels[i]->Caption.ToDouble();

			if(0.0 == PrevVoltages[i]) // On first iteration initialize PrevVoltages[]
				PrevVoltages[i] = CurrVoltages[i];

			VoltTolerances[i] = fabs((CONVERT_HEAD_POT_VALUE_TO_VOLTS(PotValues[i]) - CONVERT_HEAD_POT_VALUE_TO_VOLTS(PotValues[i] + 1))) * 2;
			Deviations[i]     = fabs(RequestedVoltage - CurrVoltages[i]);
			
			if(((RequestedVoltage - CurrVoltages[i]) * (RequestedVoltage - PrevVoltages[i])) <= 0) // Stopping condition: if we have just passed the m_ReqVoltages[i] point.
			{
				BestDeviations[i] = Deviations[i];
				
				if(PrevDeviations[i] < Deviations[i])							
					BestDeviations[i] = PrevDeviations[i];						
				
				if(BestDeviations[i] > VoltTolerances[i]) // Detect potential HW *minor* problems.
				{							
					// Detect potential HW *major* problems: could not reach a voltage deviation below HEAD_VOLTAGE_TOLERANCE
					if(CalibrationPointCrossingsCounters[i] > CALIBRATION_FAIL_RETRIES)
					{
						if(BestDeviations[i] > VOLTAGE_TOLERANCE)																								
							throw EQException("Couldn't calibrate the head");					
						else						 
							HeadCalibrated[i] = true; // "Not so successfully" finished calibrating this head's voltage, but still					
					}							
				}
				else			
				{		
					// Successfully finished calibrating this head's voltage.
					if (CalibrationPointCrossingsCounters[i] >= FORCED_RETRIES)
						HeadCalibrated[i] = true;								 
				}
				
				if(HeadCalibrated[i])
				{
					if(PrevDeviations[i] < Deviations[i])
					{
						PotValues[i] = PrevPotValues[i];
						SendPotValuesToHeads(PotValues);												
					}
					
					ParamsMgr->GenFourPotValues[i] = PotValues[i];
					MarkHeadForVoltCal(i, false);												
					continue;							
				}
				
				CalibrationPointCrossingsCounters[i]++;
			} // if - stopping condition

			PrevDeviations[i] = Deviations[i];
			PrevPotValues[i]  = PotValues[i];
			PrevVoltages[i]   = CurrVoltages[i];					
			
			if(RequestedVoltage < CurrVoltages[i])
			{
				PotValues[i]--;
				
				// if trying to put negative value...
				if(PotValues[i] < 0)
					throw Exception(QFormatStr("Couldn't calibrate the head: Potentiometer value reached its low limit.").c_str());						
			}
			else
			{
				PotValues[i]++;
			
				// if trying to put larger then BYTE values...
				if(PotValues[i] > 255)
					throw Exception(QFormatStr("Couldn't calibrate the head: Potentiometer value reached its high limit.").c_str());
			}

			Application->ProcessMessages();						
		}
	}	
	
	ParamsMgr->SaveAll();
}

void THeadsControlFrame::MarkHeadForVoltCal(int HeadNum, bool On)
{
	if(On)
		m_HeadsForVoltageCalibration |=  (1 << HeadNum);
	else
		m_HeadsForVoltageCalibration &= ~(1 << HeadNum);
}

bool THeadsControlFrame::IsHeadForVoltCal(int HeadNum)
{
	return (m_HeadsForVoltageCalibration & (1 << HeadNum));
}

void THeadsControlFrame::VoltsToPotsCheckRange(int *Pots)
{
	bool OutOfRange = false;
	for(int i = 0; i < HEAD_VOLTAGES_NUM; i++)
	{
		if(! IsHeadForVoltCal(i))
			continue;
			
		double Value = m_VoltagesInputValues[i]->Text.ToDouble();

		TColor FontColor;
		if(CheckInputVoltageRange(Value, i))
		{
			FontColor = clBlack;
			Pots[i]   = CONVERT_HEAD_VOLTAGE_TO_POT_VALUE(Value);
		}
		else
		{
			FontColor = clRed;
			OutOfRange = true;
		}

		m_VoltagesInputValues[i]->Font->Color = FontColor;
		m_VoltagesInputValues[i]->Refresh();
	}

	if(OutOfRange)
		throw Exception("Input voltage is out of range or range is not set yet");
}

bool THeadsControlFrame::CheckInputVoltageRange(double Value, int HeadIndex)
{
	if(Value < m_HeadsVoltageRange[HeadIndex].Min || Value > m_HeadsVoltageRange[HeadIndex].Max)
		return false;
	else
		return true;	
}

void THeadsControlFrame::SendPotValuesToHeads(int *PotValues)
{
	TOHDBSetPrintingHeadsVoltagesMessage Msg;

	Msg.MessageID = OHDB_SET_PRINTING_HEADS_VOLTAGES;

	for(int i = 0; i < HEAD_VOLTAGES_NUM; i++)
		Msg.PotentiometerValues[i] = PotValues[i];

	if(m_OHDBProtocolClient->SendInstallWaitReply(&Msg,	sizeof(TOHDBSetPrintingHeadsVoltagesMessage), NULL, 0) != QLib::wrSignaled)
		throw Exception("SendPotValuesToHeads: Tester did not receive Ack");
}

void THeadsControlFrame::ReadVoltagesFromHeads()
{
	TOHDBGetPrintingHeadsVoltagesMessage Msg;
	Msg.MessageID = OHDB_GET_PRINTING_HEADS_VOLTAGES;

	if(m_OHDBProtocolClient->SendInstallWaitReply(&Msg,	sizeof(TOHDBGetPrintingHeadsVoltagesMessage), SetVoltageRangeHandler, reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
		throw Exception("ReadVoltagesFromHeads: Tester did not receive Ack");
}

void THeadsControlFrame::SetVoltageRangeHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
	THeadsControlFrame *InstancePtr = reinterpret_cast<THeadsControlFrame *>(Cockie);

    // Build the read data message
    TOHDBGenericMessage *Response = static_cast<TOHDBGenericMessage *>(Data);

    switch(Response->MessageID)
    {
		case OHDB_PRINTING_HEADS_VOLTAGES:
		{
			TOHDBPrintingHeadsVoltagesResponse *HeadsVoltages = static_cast<TOHDBPrintingHeadsVoltagesResponse *>(Data);
			memcpy(InstancePtr->m_HeadsVoltsForRangeCalc, HeadsVoltages->HeadsVoltages, HEAD_VOLTAGES_NUM * sizeof(WORD));
			InstancePtr->m_SetVoltageRangeEvent.SetEvent();			
			break;
		}

		default: break;
	}
}

// Calculate the allowed range of input voltages  
void THeadsControlFrame::CalcVoltageRange()
{
	int MinPotValues[HEAD_VOLTAGES_NUM] = {0};
	int MaxPotValues[HEAD_VOLTAGES_NUM] = {0};

	memset(MaxPotValues, 255, sizeof(MaxPotValues));

	SendPotValuesToHeads(MinPotValues);
	QSleep(VOLTAGES_STABILIZATION_TIME);

	ReadVoltagesFromHeads();
	m_SetVoltageRangeEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

	for(int i = 0; i < HEAD_VOLTAGES_NUM; i++)
		if(CONVERT_HEAD_VPP_A2D_TO_VOLT(m_HeadsVoltsForRangeCalc[i]))
			m_HeadsVoltageRange[i].Min = CONVERT_HEAD_VPP_A2D_TO_VOLT(m_HeadsVoltsForRangeCalc[i]);

	SendPotValuesToHeads(MaxPotValues);
	QSleep(VOLTAGES_STABILIZATION_TIME);

	ReadVoltagesFromHeads();
	m_SetVoltageRangeEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

	for(int i = 0; i < HEAD_VOLTAGES_NUM; i++)
	{
		if(CONVERT_HEAD_VPP_A2D_TO_VOLT(m_HeadsVoltsForRangeCalc[i]))
			m_HeadsVoltageRange[i].Max = CONVERT_HEAD_VPP_A2D_TO_VOLT(m_HeadsVoltsForRangeCalc[i]);
		m_VoltagesInputValues[i]->Hint = "Range: " + FloatToStrF(m_HeadsVoltageRange[i].Min, ffFixed, 4, 2) + 
										 " < X < " + FloatToStrF(m_HeadsVoltageRange[i].Max, ffFixed, 4, 2);
	}

    // Restore the previous values
	CAppParams *ParamsMgr = CAppParams::GetInstance();
	for(int i = 0; i < HEAD_VOLTAGES_NUM; i++)
		MinPotValues[i] = ParamsMgr->GenFourPotValues[i];	
	SendPotValuesToHeads(MinPotValues);
}

void __fastcall THeadsControlFrame::SetInputVoltageRangeButtonClick(TObject *Sender)
{
	TButton *btn = dynamic_cast<TButton *>(Sender);
	if(! btn) return;

	btn->Enabled = false;

	try
	{
		CalcVoltageRange();
	}
	__finally
	{
        btn->Enabled = true;
    }
}
//---------------------------------------------------------------------------


void THeadsControlFrame::LoadFromParams()
{
	CAppParams *ParamsMgr = CAppParams::GetInstance();

	for(int i = 0; i < HEATERS_NUM; i++)
		m_HeatersInputValues[i]->Text = IntToStr(ParamsMgr->GenFourHeaters[i]);

	LowThresholdEdit->Text  = IntToStr(ParamsMgr->LowThresholdTemp);
	HighThresholdEdit->Text = IntToStr(ParamsMgr->HighThresholdTemp);
	HeatingRateEdit->Text   = IntToStr(ParamsMgr->GenFourHeatingRate);

	for(int i = 0; i < HEAD_VOLTAGES_NUM; i++)
		m_VoltagesInputValues[i]->Text = FloatToStrF(CONVERT_HEAD_POT_VALUE_TO_VOLTS(ParamsMgr->GenFourPotValues[i]), ffFixed, 4, 2);

	PurgeDurationEdit->Text = IntToStr(ParamsMgr->PurgeTime);
}

void __fastcall THeadsControlFrame::PurgeButtonClick(TObject *Sender)
{
	TOCBPerformPurgeMessage Msg;
	Msg.MessageID = OCB_PERFORM_PURGE;
	Msg.PurgeTime = PurgeDurationEdit->Text.ToInt();

	if(m_OCBProtocolClient->SendInstallWaitReply(&Msg,	sizeof(TOCBPerformPurgeMessage), NULL, reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
		throw Exception("Perform purge: Tester did not receive Ack");
}
//---------------------------------------------------------------------------

void __fastcall THeadsControlFrame::SameValueForHeadHalvesCheckboxClick(TObject *Sender)
{
	TCheckBox *box = dynamic_cast<TCheckBox *>(Sender);
	if(! box) return;

	for(int i = 0; i < HEADS_NUM * NOZZLE_LINES_PER_HEAD; i++)
	{
		if(box->Checked)
		{
			if(i % 2 != 0)
			{
				m_HeatersInputValues[i]->Enabled = false;
				m_HeatersInputValues[i]->Color   = clBtnFace;
			}
		}
		else
		{
			m_HeatersInputValues[i]->Enabled = true;
			m_HeatersInputValues[i]->Color   = clWindow;
        }                                          
    }
}
//---------------------------------------------------------------------------

