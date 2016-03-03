//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FireControl.h"
#include "AppParams.h"

#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

// Pulse builder constants

TFireControlFrame *FireControlFrame;

//---------------------------------------------------------------------------
__fastcall TFireControlFrame::TFireControlFrame(TComponent* Owner)
	: TFrame(Owner)
{
	FireOptionsPagecontrol->ActivePageIndex = TFireParams::fcContinuous;
	FireParamsPagecontrol->ActivePageIndex  = TPulseParams::pulSingle;
//	StartButton->Enabled = false;

	IconsImageList->GetBitmap(icoPlayEnabled, StartButton->Glyph);

	m_PrintTimeCounter = 0;
}
//---------------------------------------------------------------------------

// Allow only ints at input
void __fastcall TFireControlFrame::EditKeyPressInt(TObject *Sender, char &Key)
{
	if((Key >= '0') && (Key <= '9'))
		return;

	if(Key == VK_ESCAPE || Key == VK_BACK || Key == VK_RETURN)
		return;

	Key = 0;
}

// Allow only floats at input
void __fastcall TFireControlFrame::EditKeyPressFloat(TObject *Sender, char &Key)
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


void TFireControlFrame::SetOHDBProtocolClient(COHDBProtocolClient *OHDBProtocolClient)
{
	m_OHDBProtocolClient = OHDBProtocolClient;
}

void TFireControlFrame::LoadFromParams()
{
	//  Load fire values from the parameters
	CAppParams *ParamsMgr = CAppParams::GetInstance();

	PulseWidthEdit->Text = FloatToStr(ParamsMgr->PulseWidth);
	FireFreqEdit->Text   = IntToStr(ParamsMgr->FireFrquency);

	// Continuous - time duration
	TimeDurationEdit->Text = IntToStr(ParamsMgr->FireTime);

	// Duty cycle
	FiresOnEdit->Text  = IntToStr(ParamsMgr->CycleOnTime);
	FiresOffEdit->Text = IntToStr(ParamsMgr->CycleOffTime);
	CyclesEdit->Text   = IntToStr(ParamsMgr->NumOfCycles);

	// Single burst
	SingleBurstFiresEdit->Text = IntToStr(ParamsMgr->NumOfFires);

	IntervalEdit->Text  = FloatToStr(ParamsMgr->GenFourInterval);
	PropDelayEdit->Text = IntToStr(ParamsMgr->GenFourPropagationDelay);
	SlewRateEdit->Text  = FloatToStr(ParamsMgr->GenFourSlewRate);

	P1_WidthEdit->Text    = FloatToStr(ParamsMgr->GenFourDoublePulse_Width_P1);
	P2_WidthEdit->Text    = FloatToStr(ParamsMgr->GenFourDoublePulse_Width_P2);
	VoltageProcEdit->Text = IntToStr(ParamsMgr->GenFourVoltagePercentage);
}

TPulseParams TFireControlFrame::GetPulseParams()
{
	// Updates m_HeadsVolts[]
	ReadVoltagesFromHeads();

	m_PulseParams.Frequency = FPGA_MASTER_CLOCK / (FireFreqEdit->Text.ToInt() * 16);

	double Interval  = IntervalEdit->Text.ToDouble();
	double PropDelay = PropDelayEdit->Text.ToDouble() / 1000;

	switch(FireParamsPagecontrol->ActivePageIndex)
	{
		case TPulseParams::pulSingle:
		{
			m_PulseParams.PulseType = TPulseParams::pulSingle;

			double PulseWidth = PulseWidthEdit->Text.ToDouble();

			// If pulse type is Single, second pulse is not used
			for(int i = 0; i < HEAD_VOLTAGES_NUM; i++)
			{
				m_PulseParams.FirstPulseDwellTime[i] = 0;
				m_PulseParams.FirstPulseFallTime[i]  = ConvertTimeToFPGAClockCounts(PulseWidth + PropDelay, FPGA_MASTER_CLOCK);
			}

			// Check max frequency sanity
			if(FireFreqEdit->Text.ToInt() > 1 / (PulseWidth + Interval) * 1000000)
				throw Exception(QFormatStr("Frequency %d Hz is above maximal allowed", FireFreqEdit->Text.ToInt()).c_str());
		}
		break;

		case TPulseParams::pulDouble:
		{
   			m_PulseParams.PulseType = TPulseParams::pulDouble;

			double P1_Width = P1_WidthEdit->Text.ToDouble();
			double P2_Width = P2_WidthEdit->Text.ToDouble();
			double VoltProc = VoltageProcEdit->Text.ToDouble() / 100;
			double SlewRate = SlewRateEdit->Text.ToDouble();

			for(int i = 0; i < HEAD_VOLTAGES_NUM; i++)
			{
				// TDelay calculation by the formula: Tdelay = Voltage x % of voltage / slew rate
				double P1_TDelay = CONVERT_HEAD_VPP_A2D_TO_VOLT(m_HeadsVolts[i]) * VoltProc / SlewRate;

				m_PulseParams.FirstPulseDwellTime[i] = ConvertTimeToFPGAClockCounts(P1_Width  - P1_TDelay, FPGA_MASTER_CLOCK);
				m_PulseParams.FirstPulseFallTime[i]  = ConvertTimeToFPGAClockCounts(P1_TDelay + PropDelay, FPGA_MASTER_CLOCK);
				m_PulseParams.SecondPulseWidth[i]    = ConvertTimeToFPGAClockCounts(P2_Width  + PropDelay, FPGA_MASTER_CLOCK);
				m_PulseParams.DelayToSecondPulse[i]  = ConvertTimeToFPGAClockCounts(Interval  + P1_TDelay, FPGA_MASTER_CLOCK);

				// Check max frequency sanity
				if(FireFreqEdit->Text.ToInt() > 2 / (P1_Width + P2_Width + Interval) * 1000000)
					throw Exception(QFormatStr("Frequency %d Hz is above maximal allowed", FireFreqEdit->Text.ToInt()).c_str());
			}
		}
		break;
	}

	return m_PulseParams;
}

int TFireControlFrame::ConvertTimeToFPGAClockCounts(double Time, double Clock)
{
	return (Time / (1 /(Clock / 1000000)));
}

TFireParams TFireControlFrame::GetFireParams()
{
	m_FireParams.FireControlType = static_cast<TFireParams::TFireControlType>(FireOptionsPagecontrol->ActivePageIndex);

	switch(FireOptionsPagecontrol->ActivePageIndex)
	{
		case TFireParams::fcContinuous:
		{
			m_FireParams.CountinuousFireType = static_cast<TFireParams::TCountinuousFireType>(ContinuousRadioGroup->ItemIndex);
			m_FireParams.TimeDuration        = TimeDurationEdit->Text.ToInt();
		}
		break;

		case TFireParams::fcDutyCycle:
		{
			m_FireParams.OnFires  = strtoul(FiresOnEdit->Text.c_str(),  NULL, 0);
			m_FireParams.OffFires = strtoul(FiresOffEdit->Text.c_str(), NULL, 0);
			m_FireParams.Cycles   = CyclesEdit->Text.ToInt();
		}
		break;

		case TFireParams::fcSingleBurst:
        {
			m_FireParams.OnFires  = strtoul(SingleBurstFiresEdit->Text.c_str(), NULL, 0);
			m_FireParams.OffFires = 0;
			m_FireParams.Cycles   = 1; // We treat it as a single cycle 
		}
		break;
	}

	return m_FireParams;	
}

void TFireControlFrame::WriteToFPGA(BYTE Address, WORD Data)
{
    TOHDBXilinxWriteMessage Msg;
	Msg.MessageID = OHDB_WRITE_DATA_TO_XILINX;
	Msg.Address   = Address;
	Msg.Data      = Data;

	if (m_OHDBProtocolClient->SendInstallWaitReply(&Msg, sizeof(TOHDBXilinxWriteMessage), NULL, reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
		throw Exception("WriteToFPGA: Tester did not recieve Ack");                                                                                                        
}

void TFireControlFrame::ReadVoltagesFromHeads()
{
	TOHDBGetPrintingHeadsVoltagesMessage Msg;
	Msg.MessageID = OHDB_GET_PRINTING_HEADS_VOLTAGES;

	if(m_OHDBProtocolClient->SendInstallWaitReply(&Msg,	sizeof(TOHDBGetPrintingHeadsVoltagesMessage), ReadVoltagesHandler, reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
		throw Exception("ReadVoltagesFromHeads: Tester did not receive Ack");
}

void TFireControlFrame::ReadVoltagesHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
	TFireControlFrame   *InstancePtr = reinterpret_cast<TFireControlFrame *>(Cockie);
	switch(static_cast<TOHDBGenericMessage *>(Data)->MessageID)
    {
		case OHDB_PRINTING_HEADS_VOLTAGES:
		{
			TOHDBPrintingHeadsVoltagesResponse *HeadsVoltages = static_cast<TOHDBPrintingHeadsVoltagesResponse *>(Data);
			memcpy(InstancePtr->m_HeadsVolts, HeadsVoltages->HeadsVoltages, HEAD_VOLTAGES_NUM * sizeof(WORD));
			break;
		}
	}
}  

void __fastcall TFireControlFrame::StartButtonClick(TObject *Sender)
{
	TBitBtn *btn = dynamic_cast<TBitBtn *>(Sender);
	if(! btn) return;

	// Start button becomes Stop button
	btn->Caption = "Stop";
	btn->Glyph   = NULL;
	btn->OnClick = StopButtonClick;
	IconsImageList->GetBitmap(icoStop, btn->Glyph);

	if(m_FireParams.FireControlType == TFireParams::fcContinuous)
	{
		ClockTimer->Enabled = true;
		m_PrintTimeCounter  = 0;

		if(m_FireParams.CountinuousFireType == TFireParams::cftTime)
		{
			CountdownTimer->Interval = TimeDurationEdit->Text.ToInt();
            CountdownTimer->Enabled  = true;
		}
	}

	WriteToFPGA(FPGA_REG_GO, 0); // Reset the 'GO' register
	WriteToFPGA(FPGA_REG_GO, 1); // Start the printing

	SaveParams();
}

void __fastcall TFireControlFrame::StopButtonClick(TObject *Sender)
{
    TBitBtn *btn = dynamic_cast<TBitBtn *>(Sender);
	if(! btn) return;

	WriteToFPGA(FPGA_REG_GO, 0x00); // Reset the 'GO' register

	ClockTimer->Enabled     = false;
	CountdownTimer->Enabled = false;

	btn->Caption = "Start";
	btn->Glyph   = NULL;
	btn->OnClick = StartButtonClick;
	IconsImageList->GetBitmap(icoPlayEnabled, btn->Glyph);
}

// Timer for "progress clock" calculations
void __fastcall TFireControlFrame::ClockTimerTimer(TObject *Sender)
{
	int Min, Sec, Sec10;

	m_PrintTimeCounter++;

	Min   = m_PrintTimeCounter / 60;
	Sec   = m_PrintTimeCounter % 10;
	Sec10 = (m_PrintTimeCounter - (Min * 60)) / 10;

	ProgressLabel->Caption = IntToStr(Min) + ":" + IntToStr(Sec10) + IntToStr(Sec);
}

void __fastcall TFireControlFrame::CountdownTimerTimer(TObject *Sender)
{
	TTimer *timer = dynamic_cast<TTimer *>(Sender);
	if(! timer) return;	

	WriteToFPGA(FPGA_REG_GO, 0x00); // Reset the 'GO' register

	// Run only once
	timer->Enabled      = false;
	ClockTimer->Enabled = false;
}
//---------------------------------------------------------------------------

void TFireControlFrame::SaveParams()
{
 	//  Load fire values from the parameters
	CAppParams *ParamsMgr = CAppParams::GetInstance();

	ParamsMgr->PulseWidth   = PulseWidthEdit->Text.ToDouble();
	ParamsMgr->FireFrquency = FireFreqEdit->Text.ToInt();

	// Continuous - time duration
	ParamsMgr->FireTime = TimeDurationEdit->Text.ToInt();

	// Duty cycle
	ParamsMgr->CycleOnTime  = FiresOnEdit->Text.ToInt();
	ParamsMgr->CycleOffTime = FiresOffEdit->Text.ToInt();
	ParamsMgr->NumOfCycles  = CyclesEdit->Text.ToInt();

	// Single burst
	ParamsMgr->NumOfFires = SingleBurstFiresEdit->Text.ToInt();

	ParamsMgr->GenFourInterval         = IntervalEdit->Text.ToDouble();
	ParamsMgr->GenFourPropagationDelay = PropDelayEdit->Text.ToInt();
	ParamsMgr->GenFourSlewRate         = SlewRateEdit->Text.ToDouble();

	ParamsMgr->GenFourDoublePulse_Width_P1 = P1_WidthEdit->Text.ToDouble();
	ParamsMgr->GenFourDoublePulse_Width_P2 = P2_WidthEdit->Text.ToDouble();
	ParamsMgr->GenFourVoltagePercentage    = VoltageProcEdit->Text.ToInt();

    ParamsMgr->SaveAll();
}
