//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "GenFourHeadsTester.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "FireControl"
#pragma link "HeadsControl"
#pragma resource "*.dfm"

#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"

#include "QUtils.h"
#include "QMonitor.h"
#include "OHDBCommDefs.h"
#include "FIFOPci.h"
#include "EdenPCISys.h"
#include "AppLogFile.h"

TGenFourHeadsTesterForm *GenFourHeadsTesterForm;

const int NOZZLES_PAGE_CONTROL_HEIGHT = 560;
const int FORM_HEIGHT                 = 920;
const int FORM_WIDTH                  = 1517;
const int THEMISTOR_THRESHOLD_DELTA   = 20;

const BYTE NOZZLE_BIT_LOOKUP[] = {0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80};

//---------------------------------------------------------------------------

__fastcall TGenFourHeadsTesterForm::TGenFourHeadsTesterForm(TComponent* Owner)
	: TForm(Owner)
{
	m_OCBProtocolEngine  = NULL;
	m_OHDBProtocolEngine = NULL;

	m_OCBProtocolClient  = NULL;
	m_OHDBProtocolClient = NULL;

	m_OHDBDisplayThread = NULL;
	m_OCBDisplayThread  = NULL;

	this->Height = FORM_HEIGHT;
	this->Width  = FORM_WIDTH;

	NozzleMapPageControl->Height = NOZZLES_PAGE_CONTROL_HEIGHT;

	FireControlPanel->BevelOuter = bvNone;
	FireControlPanel->Top        = HeadsDataGroupbox->Top + HeadsDataGroupbox->Height - FireControlPanel->Height;

	HolderPanel1->BevelOuter      = bvNone;
	HolderPanel1->Top             = HeadsDataGroupbox->Top;
	HolderPanel1->Height          = FireControlPanel->Top - HolderPanel1->Top;

	HeadsControlPanel->BevelOuter = bvNone;
	HeadsControlPanel->Height     = HeadsDataGroupbox->Height;

	HeadFillingPanel->BevelOuter = bvNone;
	HeadFillingPanel->Height     = HeadsDataGroupbox->Height - FPGAPanel->Height;

	FPGAPanel->BevelOuter = bvNone;
	FPGAPanel->Top        = HeadFillingPanel->Top + HeadFillingPanel->Height;


	MinStrobeLabel->Caption = StrobePulseDelayTracker->Min;
	MaxStrobeLabel->Caption = StrobePulseDelayTracker->Max;

	// Set buttons icons
	ButtonsImageList->GetBitmap(icoSet,      SetAllButton->Glyph);
	ButtonsImageList->GetBitmap(icoClear,    ClearAllButton->Glyph);
	ButtonsImageList->GetBitmap(icoSave,     SavePatternButton->Glyph);
	ButtonsImageList->GetBitmap(icoLoad,     LoadPatternButton->Glyph);
	ButtonsImageList->GetBitmap(icoSet,      SetButton->Glyph);
	ButtonsImageList->GetBitmap(icoDownload, DownloadParamsButton->Glyph);
	ButtonsImageList->GetBitmap(icoDownload, DownloadDataButton->Glyph);

	ButtonsImageList->GetBitmap(icoLedGrey,   FillingMonitorStatusImg->Picture->Bitmap);
	ButtonsImageList->GetBitmap(icoLedGrey,   PowerStatusImg->Picture->Bitmap);

	// Create nozzle map frames with checkboxes
	for(int i = 0; i < HEADS_NUM; i++)
	{
		TTabSheet *Tab   = new TTabSheet(NozzleMapPageControl);
		Tab->PageControl = NozzleMapPageControl;
		Tab->Caption     = "Head " + IntToStr(i + 1);
		Tab->Name        = "Head"  + IntToStr(i) + "Tab";
		Tab->Tag         = i;

		m_NozzleMapFrames[i]         = new TNozzlesMapFrame(Tab);
		m_NozzleMapFrames[i]->Name   = "Head" + IntToStr(i) + "NozzlesMapFrame";
		m_NozzleMapFrames[i]->Align  = alClient;
		m_NozzleMapFrames[i]->Parent = Tab;
		m_NozzleMapFrames[i]->Tag    = i;
	}

	for(int i = 0; i < ActuatorsGroupbox->ControlCount; i++)
	{
		TImage *img = dynamic_cast<TImage *>(ActuatorsGroupbox->Controls[i]);
		if(img)
			ButtonsImageList->GetBitmap(icoLedGrey, img->Picture->Bitmap);
	}

	for(int i = 0; i < ThermLevelGroupbox->ControlCount; i++)
	{
		TPanel *Panel = dynamic_cast<TPanel *>(ThermLevelGroupbox->Controls[i]);

		if(Panel)
		{
            Panel->ParentBackground = false;
			m_FillingThermPanels[Panel->Tag] = Panel;
		}
	}

	for(int i = 0; i < ThermCalGroupbox->ControlCount; i++)
	{
		TEdit *Edit = dynamic_cast<TEdit *>(ThermCalGroupbox->Controls[i]);
		if(Edit) m_FillingThermInputs[Edit->Tag] = Edit;                    
	}

	for(int i = 0; i < ActiveThermsGroupbox->ControlCount; i++)
	{
		TCheckBox *chbx = dynamic_cast<TCheckBox *>(ActiveThermsGroupbox->Controls[i]);
		if(chbx)
		{
            chbx->OnClick = ActiveThermSelectorCheckBoxClick;
			m_ActiveThermSelectors[chbx->Tag] = chbx;
		}
	}

	for(int i = 0; i < TankAsignGroupbox->ControlCount; i++)
	{
		TRadioGroup *rg = dynamic_cast<TRadioGroup *>(TankAsignGroupbox->Controls[i]);
		if(rg) m_ContainerSelectors[rg->Tag] = rg;
	}

	m_FillingMonitorStatus = false;
	m_AtAppStartup         = true;
	m_PowerStatus          = false; 

	for(int i = 0; i < ACTUATORS_NUM; i++)
		m_ActuatorsValues[i] =  m_PrevActuatorsValues[i] = false;

	m_FPGAData = 0;	
}
//---------------------------------------------------------------------------

void TGenFourHeadsTesterForm::Open(CEdenProtocolEngine *OCBProtocolEngine, CEdenProtocolEngine *OHDBProtocolEngine)
{
	if(! m_OCBProtocolClient)
	{
		if(OCBProtocolEngine)
		{
			m_OCBProtocolEngine = OCBProtocolEngine;
			COCBProtocolClient::Init(m_OCBProtocolEngine);
			m_OCBProtocolClient = COCBProtocolClient::Instance();

			m_OCBDisplayThread = new COCBDisplayThread(m_OCBProtocolClient,
													   reinterpret_cast<TGenericCockie>(this),
													   &ActuatorReceiveHandler, NULL, NULL, NULL, NULL,
													   &FillingReceiveHandler,  NULL, NULL,
													   &PowerReceiveHandler);

			HeadsControlFrame1->SetOCBProtocolClient(m_OCBProtocolClient);

			m_OCBProtocolClient->InstallMessageHandler(OCB_HEADS_FILLING_ERROR,     OCBErrorHandler,     reinterpret_cast<TGenericCockie>(this));
			m_OCBProtocolClient->InstallMessageHandler(OCB_MSC_STATUS_NOTIFICATION, OCBErrorHandler,     reinterpret_cast<TGenericCockie>(this));
		}
	}

	if(! m_OHDBProtocolClient)
	{
		if(OHDBProtocolEngine)
		{
			m_OHDBProtocolEngine = OHDBProtocolEngine;
			COHDBProtocolClient::Init(m_OHDBProtocolEngine);
			m_OHDBProtocolClient = COHDBProtocolClient::Instance();

			m_OHDBDisplayThread = new COHDBDisplayThread(m_OHDBProtocolClient, reinterpret_cast<TGenericCockie>(HeadsControlFrame1),
														 NULL,
														 &(HeadsControlFrame1->StatusHandler),
														 &(HeadsControlFrame1->StatusHandler),
														 &(HeadsControlFrame1->StatusHandler),
														 &(HeadsControlFrame1->StatusHandler));

			 HeadsControlFrame1->SetOHDBProtocolClient(m_OHDBProtocolClient);
			 FireControlFrame1->SetOHDBProtocolClient(m_OHDBProtocolClient);

			 m_OHDBProtocolClient->InstallMessageHandler(OHDB_HEADS_TEMPERATURE_ERROR, OHDBErrorHandler, reinterpret_cast<TGenericCockie>(this));
		}
	}

	Show();

	if(m_OCBDisplayThread)
		if(! m_OCBDisplayThread->Running)
			m_OCBDisplayThread->Resume();

	if(m_OHDBDisplayThread)
		if(! m_OHDBDisplayThread->Running)
			m_OHDBDisplayThread->Resume();

	DWORD dwStatus = EdenPCISystem_Init();
	if(dwStatus != WD_STATUS_SUCCESS)
		throw Exception(QFormatStr("Data PCI not initialized. Status: %s", Stat2Str(dwStatus)).c_str());
	FIFOPCI_Init();
}

void __fastcall TGenFourHeadsTesterForm::FormDestroy(TObject *Sender)
{
	if(m_OHDBDisplayThread)
	{
		m_OHDBDisplayThread->Terminate();
		m_OHDBDisplayThread->WaitFor();
		Q_SAFE_DELETE(m_OHDBDisplayThread);
	}

	if(m_OCBDisplayThread)
	{
		m_OCBDisplayThread->Terminate();
		m_OCBDisplayThread->WaitFor();
		Q_SAFE_DELETE(m_OCBDisplayThread);
	}
}

// Allow only digits at input 
void __fastcall TGenFourHeadsTesterForm::EditKeyPress(TObject *Sender, char &Key)
{
	if ((Key >= '0') && (Key <= '9'))
        return;

	if (Key == VK_ESCAPE || Key == VK_BACK || Key == VK_RETURN)
		return;

    Key = 0;	
}

// Fill the selected head checkboxes with defined pattern
void __fastcall TGenFourHeadsTesterForm::SetButtonClick(TObject *Sender)
{
	int StartNozzle = StartNozzleEdit->Text.ToInt();
	int EndNozzle	= EndNozzleEdit->Text.ToInt();
	int OnCount     = OnNozzleEdit->Text.ToInt();
	int OffCount    = OffNozzleEdit->Text.ToInt();

	if(! ValidatePatternValues(StartNozzle, EndNozzle, OnCount, OffCount))
		return;

	TNozzlesMapFrame *Frame = m_NozzleMapFrames[NozzleMapPageControl->ActivePageIndex];

	for(int i = StartNozzle - 1; i < EndNozzle;)
	{
		Frame->SetCheckboxesInRange(i,           i + OnCount,            true);
		Frame->SetCheckboxesInRange(i + OnCount, i + OnCount + OffCount, false);

		i += OnCount + OffCount;

		if(i > EndNozzle)
			return;
    }  
}

// Validate the input values for the pattern creation
bool TGenFourHeadsTesterForm::ValidatePatternValues(int StartNozzle, int EndNozzle, int OnCount, int OffCount)
{
	if(StartNozzle < 1 || StartNozzle > NOZZLES_IN_GEN4_HEAD)
	{
		QMonitor.ErrorMessage("Invalid start nozzle value");
		return false;
	}

	if(EndNozzle < 1 || EndNozzle > NOZZLES_IN_GEN4_HEAD)
	{
		QMonitor.ErrorMessage("Invalid end nozzle value");
		return false;
	}

	if(EndNozzle - StartNozzle < 1)
	{
		QMonitor.ErrorMessage("Invalid range");
		return false;
	}

	if(EndNozzle - StartNozzle < OnCount || EndNozzle - StartNozzle < OffCount)
	{
		QMonitor.ErrorMessage("Invalid count range");
		return false;
	}

	if(OnCount < 1 || OnCount > NOZZLES_IN_GEN4_HEAD)
	{
		QMonitor.ErrorMessage("Invalid on count value");
		return false;
	}

	if(OffCount < 1 || OffCount > NOZZLES_IN_GEN4_HEAD)
	{
		QMonitor.ErrorMessage("Invalid off count value");
		return false;
	}

	return true;
}

void __fastcall TGenFourHeadsTesterForm::SetAllButtonClick(TObject *Sender)
{
	SetAllNozzleCheckboxes(false);
}

void __fastcall TGenFourHeadsTesterForm::ClearAllButtonClick(TObject *Sender)
{
	SetAllNozzleCheckboxes(true);
}

// Set a state for all checkboxes for all heads
void TGenFourHeadsTesterForm::SetAllNozzleCheckboxes(bool State)
{
	for(int i = 0; i < HEADS_NUM; i++)
	{
		m_NozzleMapFrames[i]->SetAllState(State);
		m_NozzleMapFrames[i]->SetAllButtonClick(NULL);
	}
}

void __fastcall TGenFourHeadsTesterForm::LoadPatternButtonClick(TObject *Sender)
{
	CQParamsFileStream *FileStream = NULL;
	CAppParams         *ParamsMgr  = CAppParams::GetInstance();

	LoadPatternDialog->Filter     = "Config files (*.cfg)|*.cfg|All files (*.*)|*.*";
	LoadPatternDialog->DefaultExt = "cfg";

	if(LoadPatternDialog->Execute())
	{
        try
        {
			FileStream = new CQParamsFileStream(LoadPatternDialog->FileName.c_str());
			ParamsMgr->Import(FileStream, false);

			BYTE Nozzles[NOZZLES_PATTERN_SIZE];
			memset(Nozzles, 0, NOZZLES_PATTERN_SIZE);

			for(int i = 0; i < NOZZLES_PATTERN_SIZE; i++)
				Nozzles[i] = ParamsMgr->GenFourNozzlePatternValues[i];

			SetNozzlesPattern(Nozzles);
		}
		__finally
		{
			Q_SAFE_DELETE(FileStream);
		}
	}
}

void __fastcall TGenFourHeadsTesterForm::SavePatternButtonClick(TObject *Sender)
{
	CQParamsFileStream *FileStream = NULL;

	SavePatternDialog->Filter     = "Config files (*.cfg)|*.cfg";
	SavePatternDialog->DefaultExt = "cfg";

	CAppParams *ParamsMgr = CAppParams::GetInstance();

	BYTE NozzlesPattern[NOZZLES_PATTERN_SIZE];
	GetNozzlesPattern(NozzlesPattern);

	for(int i = 0; i < NOZZLES_PATTERN_SIZE; i++)
		ParamsMgr->GenFourNozzlePatternValues[i] = NozzlesPattern[i];

	try
	{
		if(SavePatternDialog->Execute())
		{
			// Try to delete the current file (in case it's already exists
			DeleteFile(SavePatternDialog->FileName.c_str());

			// Create a file params stream
			QString s = SavePatternDialog->FileName.c_str();

			FileStream = new CQParamsFileStream(s);

			ParamsMgr->SaveSingleParameter(&ParamsMgr->GenFourNozzlePatternValues, FileStream);
        }

    }	__finally
    {
        Q_SAFE_DELETE(FileStream);
	}
}

void __fastcall TGenFourHeadsTesterForm::FormShow(TObject *Sender)
{
	FireControlFrame1->LoadFromParams();
	HeadsControlFrame1->LoadFromParams();
	LoadFromParams();
	TurnPower(true);

    // Reset all FPGA registers
	WriteToFPGA(FPGA_REG_RESET_ALL, 1);

	// Reset the led
	ReadFromFPGA(0x90);
	WriteToFPGA(0x90, m_FPGAData & 0xFFFE);

	ActLedButton->Caption = "On";
	ActLedButton->OnClick = LedOnClick;
	ButtonsImageList->GetBitmap(icoLedRed,  ActLedImg->Picture->Bitmap);
}

void TGenFourHeadsTesterForm::TurnPower(bool Status)
{
	CQLog::Write(JS_LOG_TAG_GENERAL, QFormatStr("Turning power %s", (Status ? "ON" : "OFF")).c_str());

	TOCBSetPowerOnOffMessage Msg;
	Msg.MessageID  = OCB_SET_POWER_ON_OFF;
	Msg.PowerOnOff = Status ? 1 : 0;

	if(m_OCBProtocolClient)
		if (m_OCBProtocolClient->SendInstallWaitReply(&Msg, sizeof(TOCBSetPowerOnOffMessage), NULL, reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
			throw Exception("TurnPowerOn: Tester did not recieve Ack");

	m_PowerStatus = Status;		
}

void TGenFourHeadsTesterForm::FillingReceiveHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
	TOCBHeadFillingStatusResponse *FillingMsg   = static_cast<TOCBHeadFillingStatusResponse *>(Data);
	TGenFourHeadsTesterForm       *InstancePtr  = reinterpret_cast<TGenFourHeadsTesterForm *>(Cockie);

	if (FillingMsg->MessageID != OCB_HEADS_FILLING_STATUS)
        return;

	InstancePtr->m_FillingMonitorStatus = FillingMsg->HeadFillingControlOnOff;

	for(int i = 0; i < NUM_OF_CHAMBERS_THERMISTORS; i++)
		InstancePtr->m_FillingThermsLevels[i] = FillingMsg->ResinLevel[i];

    PostMessage(InstancePtr->Handle, WM_GEN4_THERMISTORS, reinterpret_cast<int>(0), reinterpret_cast<int>(0));
}

// Display block filling thermistors and "Filling Monitor" status 
MESSAGE void TGenFourHeadsTesterForm::FillingDisplay(TMessage &Message)
{
	CAppParams *ParamsMgr = CAppParams::GetInstance();

	QString str = "Filling thermistors: ";
	
	for(int i = 0; i < NUM_OF_CHAMBERS_THERMISTORS; i++)
	{
		m_FillingThermPanels[i]->Caption = m_FillingThermsLevels[i];
		str += QIntToStr(m_FillingThermsLevels[i]) + ", ";

		if(m_FillingMonitorStatus)
		{
			if(m_FillingThermPanels[i]->Caption.ToInt() > ParamsMgr->GenFourBlockFillingThermsLow[i])
			{
				m_FillingThermPanels[i]->Color       = clLime;
				m_FillingThermPanels[i]->Font->Color = clBlack;
			}
			else
			{
				m_FillingThermPanels[i]->Color       = clRed;
				m_FillingThermPanels[i]->Font->Color = clWhite;
            }
		}
		else
		{
			m_FillingThermPanels[i]->Color       = clBtnFace;
			m_FillingThermPanels[i]->Font->Color = clBlack;
        }   
	}

	if(m_FillingMonitorStatus)
	{
		MonitorOnOffButton->Caption = "Filling OFF";
		MonitorOnOffButton->OnClick = TurnFillingMonitorOffButtonClick;
		ButtonsImageList->GetBitmap(icoLedGreen, FillingMonitorStatusImg->Picture->Bitmap);
	}
	else
	{
		MonitorOnOffButton->Caption = "Filling ON";
		MonitorOnOffButton->OnClick = TurnFillingMonitorOnButtonClick;
		ButtonsImageList->GetBitmap(icoLedRed, FillingMonitorStatusImg->Picture->Bitmap);
	}
	
	FillingMonitorStatusImg->Refresh();

	DEL_LAST_2_CHARS(str); 
	CQLog::Write(JS_LOG_TAG_FILLING, str.c_str());
}

void __fastcall TGenFourHeadsTesterForm::TurnFillingMonitorOnButtonClick(TObject *Sender)
{
	TButton *btn = dynamic_cast<TButton *>(Sender);
	if(! btn) return;

    // Making sure that parameters are set in OHDB before activating filling monitor
	SetFillingParamsButton->Click();

	ActivateFillingMonitor(true);
}

void __fastcall TGenFourHeadsTesterForm::TurnFillingMonitorOffButtonClick(TObject *Sender)
{
	TButton *btn = dynamic_cast<TButton *>(Sender);
	if(! btn) return;

	ActivateFillingMonitor(false);
}

void TGenFourHeadsTesterForm::ActivateFillingMonitor(bool State)
{
	TOCBHeadFillingControlOnOffMessage Msg;
	Msg.MessageID               = OCB_HEADS_FILLING_CONTROL_ON_OFF;
	Msg.HeadFillingControlOnOff = State;

	if (m_OCBProtocolClient->SendInstallWaitReply(&Msg, sizeof(TOCBHeadFillingControlOnOffMessage), NULL, reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
		throw Exception("ActivateFillingMonitor: Tester did not recieve Ack");
}

void TGenFourHeadsTesterForm::ActuatorReceiveHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
    TOCBActuatorsStatusResponse *ActuatorsMsg = static_cast<TOCBActuatorsStatusResponse *>(Data);
    TGenFourHeadsTesterForm     *InstancePtr  = reinterpret_cast<TGenFourHeadsTesterForm *>(Cockie);

    if (ActuatorsMsg->MessageID != OCB_ACTUATOR_STATUS)
        return;

    for(int i = 0, j = 0; j < ACTUATORS_NUM / 8; j++)
    {
        InstancePtr->m_ActuatorsValues[i++] = ((int)ActuatorsMsg->ActuatorsArray[j] & 0x01);
        InstancePtr->m_ActuatorsValues[i++] = ((int)ActuatorsMsg->ActuatorsArray[j] & 0x02);
        InstancePtr->m_ActuatorsValues[i++] = ((int)ActuatorsMsg->ActuatorsArray[j] & 0x04);
        InstancePtr->m_ActuatorsValues[i++] = ((int)ActuatorsMsg->ActuatorsArray[j] & 0x08);
        InstancePtr->m_ActuatorsValues[i++] = ((int)ActuatorsMsg->ActuatorsArray[j] & 0x10);
        InstancePtr->m_ActuatorsValues[i++] = ((int)ActuatorsMsg->ActuatorsArray[j] & 0x20);
        InstancePtr->m_ActuatorsValues[i++] = ((int)ActuatorsMsg->ActuatorsArray[j] & 0x40);
        InstancePtr->m_ActuatorsValues[i++] = ((int)ActuatorsMsg->ActuatorsArray[j] & 0x80);
    }

    // Post a message to the dialog to display the current received reading
    PostMessage(InstancePtr->Handle, WM_GEN4_ACTUATORS, reinterpret_cast<int>(0), reinterpret_cast<int>(0));
}

MESSAGE void TGenFourHeadsTesterForm::ActuatorsDisplay(TMessage &Message)
{
	for(int i = 0; i < ActuatorsGroupbox->ControlCount; i++)
	{
		if(ActuatorsGroupbox->Controls[i]->Tag < 0)
			continue;

		TImage  *img = dynamic_cast<TImage *> (ActuatorsGroupbox->Controls[i]);
		TButton *btn = dynamic_cast<TButton *>(ActuatorsGroupbox->Controls[i]);

		if(img)
		{
            // To avoid UI flickering
			if(m_AtAppStartup || m_ActuatorsValues[img->Tag] != m_PrevActuatorsValues[img->Tag])
			{
				if(m_ActuatorsValues[img->Tag])
					ButtonsImageList->GetBitmap(icoLedGreen, img->Picture->Bitmap);
				else
					ButtonsImageList->GetBitmap(icoLedRed,   img->Picture->Bitmap);
				img->Refresh();

				m_PrevActuatorsValues[img->Tag] = m_ActuatorsValues[img->Tag];
			}
		}

		if(btn)
		{
			if(m_ActuatorsValues[btn->Tag])
			{
				btn->Caption = "Off";
				btn->OnClick = ActuatorOffClick;
			}
			else
			{
				btn->Caption = "On";
				btn->OnClick = ActuatorOnClick;
            }   
        }
	}

	m_AtAppStartup = false;
}

void __fastcall TGenFourHeadsTesterForm::LedOnClick(TObject *Sender)
{
	TButton *btn = dynamic_cast<TButton *>(Sender);
	if(! btn) return;

	// Turn ON the led
	ReadFromFPGA(0x90);
	WriteToFPGA(0x90, m_FPGAData | 1);

	btn->OnClick = LedOffClick;
	btn->Caption = "Off";
	ButtonsImageList->GetBitmap(icoLedGreen, ActLedImg->Picture->Bitmap);
	ActLedImg->Refresh();
}

void __fastcall TGenFourHeadsTesterForm::LedOffClick(TObject *Sender)
{
	TButton *btn = dynamic_cast<TButton *>(Sender);
	if(! btn) return;

	// Reset the led
	ReadFromFPGA(0x90);
	WriteToFPGA(0x90, m_FPGAData & 0xFFFE);

	btn->OnClick = LedOnClick;
	btn->Caption = "On";
	ButtonsImageList->GetBitmap(icoLedRed, ActLedImg->Picture->Bitmap);
	ActLedImg->Refresh();
}

void __fastcall TGenFourHeadsTesterForm::ActuatorOnClick(TObject *Sender)
{
    TButton *btn = dynamic_cast<TButton *>(Sender);
	if(btn)
		SetActuator(btn->Tag, ACT_ON);
}

void __fastcall TGenFourHeadsTesterForm::ActuatorOffClick(TObject *Sender)
{
	TButton *btn = dynamic_cast<TButton *>(Sender);
	if(btn)
		SetActuator(btn->Tag, ACT_OFF);
}

void TGenFourHeadsTesterForm::SetActuator(int ActuatorID, int State)
{
	TOCBSetActuatorOnOffMessage Msg;
	Msg.MessageID  = OCB_SET_ACTUATOR_ON_OFF;
	Msg.ActuatorID = ActuatorID;
	Msg.OnOff      = State;

	if (m_OCBProtocolClient->SendInstallWaitReply(&Msg, sizeof(TOCBSetActuatorOnOffMessage), NULL, reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
		throw Exception("SetActuator: Tester did not recieve Ack");
}

void __fastcall TGenFourHeadsTesterForm::DownloadDataButtonClick(TObject *Sender)
{
	TBitBtn *btn = dynamic_cast<TBitBtn *>(Sender);
	if(! btn) return;

	btn->Enabled                            = false;
	DownloadParamsButton->Enabled           = false;
	FireControlFrame1->StartButton->Enabled = false;

	m_PrintCommandsVector.clear();

	try
	{
		// Add commands for data transfer
		BYTE NozzlesPattern[NOZZLES_PATTERN_SIZE];
		GetNozzlesPattern(NozzlesPattern);

		// Select RS232 as data source
		m_PrintCommandsVector.push_back(TFPGACommand(FPGA_REG_DATA_SOURCE, 1));

		// Reset firing state machine
		m_PrintCommandsVector.push_back(TFPGACommand(FPGA_REG_RESET_SM, 1));

        // Reset FIFO at FPGA
		m_PrintCommandsVector.push_back(TFPGACommand(FPGA_REG_FIFO_RESET, 1));

		FIFOPCI_ResetBufferAddress();
		FIFOPCI_NoGo();
		FIFOPCI_WriteAsync(NozzlesPattern, sizeof(NozzlesPattern) / sizeof(DWORD));

		// Send all fire commands to FPGA
		SendFireParams();
	}
	__finally
	{
		btn->Enabled                            = true;
		DownloadParamsButton->Enabled           = true;
		FireControlFrame1->StartButton->Enabled = true;
		m_PrintCommandsVector.clear();
	}
}


void __fastcall TGenFourHeadsTesterForm::DownloadParamsButtonClick(TObject *Sender)
{
	TBitBtn *btn = dynamic_cast<TBitBtn *>(Sender);
	if(! btn) return;

	btn->Enabled                            = false;
	DownloadDataButton->Enabled             = false;
	FireControlFrame1->StartButton->Enabled = false;

	m_PrintCommandsVector.clear();

	try
	{
		// Select RS232 as data source
		m_PrintCommandsVector.push_back(TFPGACommand(FPGA_REG_DATA_SOURCE, 1));

		// Reset firing state machine
		m_PrintCommandsVector.push_back(TFPGACommand(FPGA_REG_RESET_SM, 1));

		TPulseParams PulseParams = FireControlFrame1->GetPulseParams();
		TFireParams  FireParams  = FireControlFrame1->GetFireParams();

        // Add single/double pulse operation command
		m_PrintCommandsVector.push_back(TFPGACommand(FPGA_REG_PULSE_OP_MODE, PulseParams.PulseType));

        // Add firing frequency
		m_PrintCommandsVector.push_back(TFPGACommand(FPGA_REG_FIRE_FREQ, PulseParams.Frequency));
		
        // Add pulse operation mode commands
		if(PulseParams.PulseType == TPulseParams::pulSingle)
			CollectFireParamsSinglePulse(PulseParams);
		else
			CollectFireParamsDoublePulse(PulseParams);

        // Add fire operation mode commands
		CollectFireOperationCommands(FireParams);

		// Add strobe pulse width
		WORD StrobePulseWidth = static_cast<WORD>(StrobePulseWidthEdit->Text.ToDouble() / (1 / (double)FPGA_MASTER_CLOCK));
		m_PrintCommandsVector.push_back(TFPGACommand(FPGA_REG_STROBE_PULSE_WIDTH, StrobePulseWidth));

        // Send all fire commands to FPGA
		SendFireParams();
	}
	__finally
	{
		btn->Enabled                            = true;
		DownloadDataButton->Enabled             = true;
		FireControlFrame1->StartButton->Enabled = true;
		m_PrintCommandsVector.clear();
	}
}

// Send fire commands to FPGA
void TGenFourHeadsTesterForm::SendFireParams()
{
    DownloadProgressBar->Position = 0;
	DownloadProgressBar->Max      = m_PrintCommandsVector.size();

	for(std::vector<TFPGACommand>::const_iterator it = m_PrintCommandsVector.begin(); it != m_PrintCommandsVector.end(); ++it)
	{
		WriteToFPGA(it->Address, it->Data);
		DownloadProgressBar->Position++;
		Application->ProcessMessages();
    }
}

// Collect fire operation mode and timing commands
void TGenFourHeadsTesterForm::CollectFireOperationCommands(TFireParams &FireParams)
{
	switch(FireParams.FireControlType)
	{
		case TFireParams::fcContinuous:
		{
			WORD DiagValue = DIAG1_ENABLE_SIMS | DIAG1_FIRE_INFINITE;
			m_PrintCommandsVector.push_back(TFPGACommand(FPGA_REG_DIAG1, DiagValue));
		}
		break;

		case TFireParams::fcDutyCycle:
		case TFireParams::fcSingleBurst:
		{
        	WORD DiagValue = DIAG1_ENABLE_SIMS & DIAG1_FIRE_CYCLES;
			m_PrintCommandsVector.push_back(TFPGACommand(FPGA_REG_DIAG1, DiagValue));

			m_PrintCommandsVector.push_back(TFPGACommand(FPGA_REG_FIRES_ON_H,    static_cast<WORD>((FireParams.OnFires & 0xFFFF0000) >> 16)));
			m_PrintCommandsVector.push_back(TFPGACommand(FPGA_REG_FIRES_ON_L,    static_cast<WORD>(FireParams.OnFires)));
			m_PrintCommandsVector.push_back(TFPGACommand(FPGA_REG_FIRES_OFF_H,   static_cast<WORD>((FireParams.OffFires & 0xFFFF0000) >> 16)));
			m_PrintCommandsVector.push_back(TFPGACommand(FPGA_REG_FIRES_OFF_L,   static_cast<WORD>(FireParams.OffFires)));
			m_PrintCommandsVector.push_back(TFPGACommand(FPGA_REG_FIRE_CYCLES_H, static_cast<WORD>((FireParams.Cycles & 0xFFFF0000) >> 16)));
			m_PrintCommandsVector.push_back(TFPGACommand(FPGA_REG_FIRE_CYCLES_L, static_cast<WORD>(FireParams.Cycles)));
		}
		break;
	}
}

// Collect Single Pulse related commands into commands vector before transmitting to FPGA
void TGenFourHeadsTesterForm::CollectFireParamsSinglePulse(TPulseParams &PulseParams)
{
	for(int i = 0; i < HEAD_VOLTAGES_NUM; i++)
	{
		m_PrintCommandsVector.push_back(TFPGACommand(PULSE1_FT_REGS[i],    PulseParams.FirstPulseFallTime[i]));
		m_PrintCommandsVector.push_back(TFPGACommand(PULSE1_DWELL_REGS[i], PulseParams.FirstPulseDwellTime[i]));
	}
}

// Collect Double Pulse related commands into commands vector before transmitting to FPGA
void TGenFourHeadsTesterForm::CollectFireParamsDoublePulse(TPulseParams &PulseParams)
{
	for(int i = 0; i < HEAD_VOLTAGES_NUM; i++)
	{
		m_PrintCommandsVector.push_back(TFPGACommand(PULSE1_FT_REGS[i],    PulseParams.FirstPulseFallTime[i]));
		m_PrintCommandsVector.push_back(TFPGACommand(PULSE1_DWELL_REGS[i], PulseParams.FirstPulseDwellTime[i]));
		m_PrintCommandsVector.push_back(TFPGACommand(PULSE2_WIDTH_REGS[i], PulseParams.SecondPulseWidth[i]));
		m_PrintCommandsVector.push_back(TFPGACommand(PULSE_DELAY_REGS[i],  PulseParams.DelayToSecondPulse[i]));
	}
}

// Set nozzles pattern from Parameters manager, which was previously loaded from file
void TGenFourHeadsTesterForm::SetNozzlesPattern(BYTE *NozzlesPattern)
{
	bool Nozzles[NOZZLES_IN_GEN4_HEAD];

	for(int i = 0; i < HEADS_NUM; i++)
	{
		memset(Nozzles, false, sizeof(Nozzles));
		
		for(int j = 0; j < NOZZLES_IN_GEN4_HEAD; j++)
		{
			if((NozzlesPattern[(i * NOZZLES_IN_GEN4_HEAD + j) / 8] & NOZZLE_BIT_LOOKUP[j % 8]) == NOZZLE_BIT_LOOKUP[j % 8])
            	Nozzles[j] = true;
		}

		m_NozzleMapFrames[i]->SetNozzles(Nozzles);
	}
}

// Get the nozzle pattern (checked checkboxes) from all heads 
void TGenFourHeadsTesterForm::GetNozzlesPattern(BYTE *NozzlesPattern)
{
	memset(NozzlesPattern, 0, NOZZLES_PATTERN_SIZE);

	for(int i = 0; i < HEADS_NUM; i++)
	{
		bool *Nozzles = m_NozzleMapFrames[i]->GetNozzles();

		for(int j = 0; j < NOZZLES_IN_GEN4_HEAD; j++)
			if(Nozzles[j])
				NozzlesPattern[(i * NOZZLES_IN_GEN4_HEAD + j) / 8] |= NOZZLE_BIT_LOOKUP[j % 8];
	}
}

void TGenFourHeadsTesterForm::WriteToFPGA(BYTE Address, WORD Data)
{
    TOHDBXilinxWriteMessage Msg;
	Msg.MessageID = OHDB_WRITE_DATA_TO_XILINX;
	Msg.Address   = Address;
	Msg.Data      = Data;

	if (m_OHDBProtocolClient->SendInstallWaitReply(&Msg, sizeof(TOHDBXilinxWriteMessage), NULL, reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
		throw Exception("WriteToFPGA: Tester did not recieve Ack");                                                                                                        
}

void TGenFourHeadsTesterForm::ReadFromFPGA(BYTE Address)
{
    TOHDBXilinxReadMessage Msg;
	Msg.MessageID = OHDB_READ_FROM_XILINX;
	Msg.Address   = Address;              

	if (m_OHDBProtocolClient->SendInstallWaitReply(&Msg, sizeof(TOHDBXilinxWriteMessage), FPGAReadReceiveHandler, reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
		throw Exception("ReadFromFPGA: Tester did not recieve Ack");                                                                                                        
}

void TGenFourHeadsTesterForm::FPGAReadReceiveHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
	TOHDBXilinxResponse     *XilinxResponse = static_cast<TOHDBXilinxResponse *>(Data);
	TGenFourHeadsTesterForm *InstancePtr    = reinterpret_cast<TGenFourHeadsTesterForm *>(Cockie);

	InstancePtr->m_FPGAddress = XilinxResponse->Address;
	InstancePtr->m_FPGAData   = XilinxResponse->Data;
}

// Populate UI with values from parameters manager
void TGenFourHeadsTesterForm::LoadFromParams()
{
	CAppParams *ParamsMgr = CAppParams::GetInstance();

	for(int i = 0; i < NUM_OF_CHAMBERS_THERMISTORS; i++)
	{
		m_FillingThermInputs[i]->Text      = QIntToStr(ParamsMgr->GenFourBlockFillingThermsLow[i]).c_str();
		m_ActiveThermSelectors[i]->Checked = ParamsMgr->GenFourActiveThermistors[i];
	}

	for(int i = 0; i < NUMBER_OF_CHAMBERS; i++)
		m_ContainerSelectors[i]->ItemIndex = ParamsMgr->GenFourActiveTanksPerChamber[i] % 2;

	FillingTimeoutEdit->Text = QIntToStr(ParamsMgr->TimeoutFilling).c_str();
	PumpDutyOnEdit->Text     = QIntToStr(ParamsMgr->TimePumpON).c_str();
	PumpDutyOffEdit->Text    = QIntToStr(ParamsMgr->TimePumpOFF).c_str();
}

void __fastcall TGenFourHeadsTesterForm::SetFillingParamsButtonClick(TObject *Sender)
{
	CAppParams *ParamsMgr = CAppParams::GetInstance();

	if(StrToInt(PumpDutyOnEdit->Text) < 150 || StrToInt(PumpDutyOffEdit->Text) < 150)
    	throw Exception("Pump duty cycle value is too low");

	// Filling thermistors asignments
	TOCBSetHeadFillingParamsMessage FillingMsg;
	FillingMsg.MessageID = OCB_SET_HEADS_FILLING_PARAMS;

	FillingMsg.TimePumpOn     = ParamsMgr->TimePumpON     = StrToInt(PumpDutyOnEdit->Text);
	FillingMsg.TimePumpOff    = ParamsMgr->TimePumpOFF    = StrToInt(PumpDutyOffEdit->Text);
	FillingMsg.FillingTimeout = ParamsMgr->TimeoutFilling = StrToInt(FillingTimeoutEdit->Text);

	for(int i = 0; i < NUM_OF_CHAMBERS_THERMISTORS; i++)
	{
		FillingMsg.ResinLowThereshold[i] = ParamsMgr->GenFourBlockFillingThermsLow[i]  = StrToInt(m_FillingThermInputs[i]->Text);
		FillingMsg.ResinHighThershold[i] = ParamsMgr->GenFourBlockFillingThermsHigh[i] = StrToInt(m_FillingThermInputs[i]->Text) + THEMISTOR_THRESHOLD_DELTA;
		FillingMsg.ActiveThermistors[i]  = ParamsMgr->GenFourActiveThermistors[i]      = m_ActiveThermSelectors[i]->Checked;
	}
	
	if(m_OCBProtocolClient->SendInstallWaitReply(&FillingMsg, sizeof(TOCBSetHeadFillingParamsMessage), NULL, reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
		throw Exception("SetFillingParamsButtonClick: Tester did not recieve Ack");
	// --------------------------------------------------------------------------------

	// Active tank asignments
	TOCBSetChamberTankMsg TanksMsg;
	TanksMsg.MessageID = OCB_SET_CHAMBERS_TANK;

	for(int i = 0; i < NUMBER_OF_CHAMBERS; i++)
		TanksMsg.TankID[i] = ParamsMgr->GenFourActiveTanksPerChamber[i] = i * 2 + m_ContainerSelectors[i]->ItemIndex; // Convert to TTankIndex. 2 tanks connected to each chamber

	if(m_OCBProtocolClient->SendInstallWaitReply(&TanksMsg, sizeof(TOCBSetChamberTankMsg), NULL, reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
		throw Exception("SetFillingParamsButtonClick: Tester did not recieve Ack");
	// --------------------------------------------------------------------------------

	ParamsMgr->SaveAll();
}
//---------------------------------------------------------------------------

// Active thermistors checkbox logic
void __fastcall TGenFourHeadsTesterForm::ActiveThermSelectorCheckBoxClick(TObject *Sender)
{
	TCheckBox *chbx = dynamic_cast<TCheckBox *>(Sender);

	if(! chbx) return;

	switch(chbx->Tag)
	{
		case SUPPORT_CHAMBER_THERMISTOR:
		case M7_CHAMBER_THERMISTOR:
			if(chbx->Checked)
			{
				m_ActiveThermSelectors[SUPPORT_MODEL_CHAMBER_THERMISTOR]->Checked = false;
				m_ActiveThermSelectors[SUPPORT_CHAMBER_THERMISTOR]->Checked    = true;
				m_ActiveThermSelectors[M7_CHAMBER_THERMISTOR]->Checked         = true;
			}
			break;

		case M5_CHAMBER_THERMISTOR:
		case M6_CHAMBER_THERMISTOR:
			if(chbx->Checked)
			{
				m_ActiveThermSelectors[M5_M6_CHAMBER_THERMISTOR]->Checked = false;
				m_ActiveThermSelectors[M5_CHAMBER_THERMISTOR]->Checked    = true;
				m_ActiveThermSelectors[M6_CHAMBER_THERMISTOR]->Checked    = true;
			}
			break;

		case M3_CHAMBER_THERMISTOR:
		case M4_CHAMBER_THERMISTOR:
			if(chbx->Checked)
			{
				m_ActiveThermSelectors[M3_M4_CHAMBER_THERMISTOR]->Checked = false;
				m_ActiveThermSelectors[M3_CHAMBER_THERMISTOR]->Checked    = true;
				m_ActiveThermSelectors[M4_CHAMBER_THERMISTOR]->Checked    = true;
			}
			break;

		case M1_CHAMBER_THERMISTOR:
		case M2_CHAMBER_THERMISTOR:
			if(chbx->Checked)
			{
				m_ActiveThermSelectors[M1_M2_CHAMBER_THERMISTOR]->Checked = false;
				m_ActiveThermSelectors[M1_CHAMBER_THERMISTOR]->Checked    = true;
				m_ActiveThermSelectors[M2_CHAMBER_THERMISTOR]->Checked    = true;
			}
			break;

		case SUPPORT_MODEL_CHAMBER_THERMISTOR:
			if(chbx->Checked)
			{
				m_ActiveThermSelectors[SUPPORT_CHAMBER_THERMISTOR]->Checked = false;
				m_ActiveThermSelectors[M7_CHAMBER_THERMISTOR]->Checked      = false;
			}
			break;

		case M5_M6_CHAMBER_THERMISTOR:
			if(chbx->Checked)
			{
				m_ActiveThermSelectors[M5_CHAMBER_THERMISTOR]->Checked = false;
				m_ActiveThermSelectors[M6_CHAMBER_THERMISTOR]->Checked = false;
			}
			break;

		case M3_M4_CHAMBER_THERMISTOR:
			if(chbx->Checked)
			{
				m_ActiveThermSelectors[M3_CHAMBER_THERMISTOR]->Checked = false;
				m_ActiveThermSelectors[M4_CHAMBER_THERMISTOR]->Checked = false;
			}
			break;

	   case M1_M2_CHAMBER_THERMISTOR:
			if(chbx->Checked)
			{
				m_ActiveThermSelectors[M1_CHAMBER_THERMISTOR]->Checked = false;
				m_ActiveThermSelectors[M2_CHAMBER_THERMISTOR]->Checked = false;
			}
			break;
    } 	 	
}
//---------------------------------------------------------------------------

void __fastcall TGenFourHeadsTesterForm::StrobeOnButtonClick(TObject *Sender)
{
	ReadFromFPGA(FPGA_REG_DIAG1);

	WORD DiagValue = m_FPGAData | DIAG1_ENABLE_SIMS | DIAG1_STROBE_ON;

	WORD StrobePulseWidth = static_cast<WORD>(FireControlFrame1->ConvertTimeToFPGAClockCounts(StrobePulseWidthEdit->Text.ToDouble(), FPGA_MASTER_CLOCK));
	WORD StrobePulseDelay = static_cast<WORD>(FireControlFrame1->ConvertTimeToFPGAClockCounts(StrobePulseDelayTracker->Position,     FPGA_MASTER_CLOCK));

	WriteToFPGA(FPGA_REG_STROBE_PULSE_WIDTH, StrobePulseWidth);
	WriteToFPGA(FPGA_REG_STROBE_PULSE_DELAY, StrobePulseDelay);

    // Turning the strobe ON
	WriteToFPGA(FPGA_REG_DIAG1, DiagValue);
}

void __fastcall TGenFourHeadsTesterForm::StrobeOffButtonClick(TObject *Sender)
{
	ReadFromFPGA(FPGA_REG_DIAG1);
	
	WORD DiagValue = m_FPGAData & DIAG1_STROBE_OFF;

    // Turning the strobe OFF
	WriteToFPGA(FPGA_REG_DIAG1, DiagValue);
}

void __fastcall TGenFourHeadsTesterForm::StrobePulseDelayTrackerChange(TObject *Sender)
{
	TTrackBar *Slider = dynamic_cast<TTrackBar *>(Sender);

	if(! Slider) return;

	StrobePulseDelayValue->Caption = Slider->Position;

	m_StrobePulseDelay = static_cast<WORD>(FireControlFrame1->ConvertTimeToFPGAClockCounts(Slider->Position, FPGA_MASTER_CLOCK));

	PostMessage(this->Handle, WM_GEN4_STROBE_PULSE_DELAY, reinterpret_cast<int>(0), reinterpret_cast<int>(0));
}

MESSAGE void TGenFourHeadsTesterForm::StrobePulseDelayUpdate(TMessage &Message)
{
	// Send new value to FPGA
	WriteToFPGA(FPGA_REG_STROBE_PULSE_DELAY, m_StrobePulseDelay);
}

//---------------------------------------------------------------------------

void __fastcall TGenFourHeadsTesterForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	// Reset all FPGA registers
	WriteToFPGA(FPGA_REG_RESET_ALL, 1);
}

//---------------------------------------------------------------------------

void TGenFourHeadsTesterForm::OCBErrorHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{	
	TGenFourHeadsTesterForm *InstancePtr = reinterpret_cast<TGenFourHeadsTesterForm *>(Cockie);
	TOCBGenericMessage      *Message     = static_cast<TOCBGenericMessage *>(Data);

	switch(Message->MessageID)
	{
		case OCB_HEADS_FILLING_ERROR:
		{
			TOCBHeadFillingErrorResponse *Msg = static_cast<TOCBHeadFillingErrorResponse *>(Data);
			
			InstancePtr->OCBSendNotificationAck(OCB_HEADS_FILLING_ERROR);
			InstancePtr->m_ErrorStr = QFormatStr("Head filling error: %s", FillingError2Str[static_cast<THeadsFillingError>(Msg->HeadFillingError)]);
        }
		break;

		case OCB_MSC_STATUS_NOTIFICATION:
		{
			TOCBMSCNotificationMessage *Msg = static_cast<TOCBMSCNotificationMessage *>(Data);
			
			InstancePtr->OCBSendNotificationAck(OCB_MSC_STATUS_NOTIFICATION);	
			InstancePtr->m_ErrorStr = QFormatStr("MSC %d error: %s", Msg->CardNum + 1, MSCError2Str[static_cast<TMSCError>(Msg->MSCStatus)]);
		}
		break;
	}
	
	// Post a message to the dialog to display the current received reading
	PostMessage(InstancePtr->Handle, WM_GEN4_ERROR, reinterpret_cast<int>(0), reinterpret_cast<int>(0));
}

void TGenFourHeadsTesterForm::OHDBErrorHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{	
	TGenFourHeadsTesterForm *InstancePtr = reinterpret_cast<TGenFourHeadsTesterForm *>(Cockie);
	TOHDBGenericMessage      *Message     = static_cast<TOHDBGenericMessage *>(Data);

	switch(Message->MessageID)
	{
		case OHDB_HEADS_TEMPERATURE_ERROR:
		{
			TOHDBHeadsTemperatureError *Msg = static_cast<TOHDBHeadsTemperatureError *>(Data);
			InstancePtr->OHDBSendNotificationAck(OHDB_HEADS_TEMPERATURE_ERROR);
			QString str = QFormatStr("Invalid heating rate %d [AD/sec] in heater %d for period of %d seconds. Turning heating off.", Msg->ErrorHeatingRate, Msg->ErrorHead,
																															         CAppParams::GetInstance()->HeatingWatchdogSamplingInterval.Value());
			InstancePtr->m_ErrorStr = str;
        }
		break;
	}
	
	// Post a message to the dialog to display the current received reading
	PostMessage(InstancePtr->Handle, WM_GEN4_ERROR, reinterpret_cast<int>(0), reinterpret_cast<int>(0));
}

void TGenFourHeadsTesterForm::OCBSendNotificationAck(int RespondedMessageID)
{
	// Build the message
	TOCBAck Message;
    Message.MessageID          = OCB_EDEN_ACK;
    Message.RespondedMessageID = RespondedMessageID;
    Message.AckStatus          = SUCCESS;

    // Send the message
    m_OCBProtocolClient->SendNotificationAck(RespondedMessageID, &Message, sizeof(TOCBAck));
}

void TGenFourHeadsTesterForm::OHDBSendNotificationAck(int RespondedMessageID)
{
	// Build the message
	TOHDBAck Message;
    Message.MessageID          = OHDB_EDEN_ACK;
    Message.RespondedMessageID = RespondedMessageID;
    Message.AckStatus          = SUCCESS;

    // Send the message
    m_OHDBProtocolClient->SendNotificationAck(RespondedMessageID, &Message, sizeof(TOHDBAck));
}

MESSAGE void TGenFourHeadsTesterForm::NotifyError(TMessage& Message)
{
	QMonitor.ErrorMessage(m_ErrorStr.c_str());
}

// Input only Hex values BYTE
void __fastcall TGenFourHeadsTesterForm::FilterByteKeyPress(TObject *Sender, char &Key)
{
	TEdit *Edit = dynamic_cast<TEdit *>(Sender);

	if(! Edit)
		return;

    if(Key == VK_ESCAPE || Key == VK_BACK || Key == VK_RETURN)
		return;
		
	if(Edit->Text.Length() > 1)
	{
		if(Edit->SelStart != 0)
		{
			Key = 0;
			return;
        }
	}

	if((Key >= '0') && (Key <= '9'))
		return;

	if((Key >= 'a') && (Key <= 'f'))
	{
		Key = UpCase(Key);
		return;
	}

	if((Key >= 'A') && (Key <= 'F'))
		return;

    Key = 0;	
}

// Input only Hex values WORD
void __fastcall TGenFourHeadsTesterForm::FilterWordKeyPress(TObject *Sender, char &Key)
{
	TEdit *Edit = dynamic_cast<TEdit *>(Sender);

	if(! Edit)
		return;

    if(Key == VK_ESCAPE || Key == VK_BACK || Key == VK_RETURN)
		return;
		
	if(Edit->Text.Length() > 3)
	{
		if(Edit->SelStart != 0)
		{
			Key = 0;
			return;
        }
	}

	if((Key >= '0') && (Key <= '9'))
		return;

	if((Key >= 'a') && (Key <= 'f'))
	{
		Key = UpCase(Key);
		return;
	}

	if((Key >= 'A') && (Key <= 'F'))
		return;

    Key = 0;	
}

void __fastcall TGenFourHeadsTesterForm::FPGARegLoadMacroButtonClick(TObject *Sender)
{
	if(FPGAMacroOpenDialog->Execute())
		FPGARegMacroMemo->Lines->LoadFromFile(FPGAMacroOpenDialog->FileName);
}
//---------------------------------------------------------------------------

void __fastcall TGenFourHeadsTesterForm::FPGARegSaveMacroButtonClick(TObject *Sender)
{
    if(FPGAMacroSaveDialog->Execute())
        FPGARegMacroMemo->Lines->SaveToFile(FPGAMacroSaveDialog->FileName);
}

void __fastcall TGenFourHeadsTesterForm::WriteFPGARegButtonClick(TObject *Sender)
{
	int Address = StrToInt("$" + FPGARegAddressEdit->Text);
	int Data    = StrToInt("$" + FPGARegDataEdit->Text);

	WriteToFPGA(Address, Data);
	FPGARegDataEdit->Font->Color = clBlack;

	if(FPGARegRecordMacroCheckbox->Checked)
		FPGARegMacroMemo->Lines->Add(IntToHex(Address, 2) + " " + IntToHex(Data, 4));
}
//---------------------------------------------------------------------------

void __fastcall TGenFourHeadsTesterForm::ReadFPGARegButtonClick(TObject *Sender)
{
	ReadFromFPGA(StrToInt("$" + FPGARegAddressEdit->Text));
	FPGARegDataEdit->Text = IntToHex(m_FPGAData, 4);
    FPGARegDataEdit->Font->Color = clMaroon;
}
//---------------------------------------------------------------------------

void __fastcall TGenFourHeadsTesterForm::FPGARegMacroClearButtonClick(TObject *Sender)
{
	FPGARegMacroMemo->Clear();
}
//---------------------------------------------------------------------------

void __fastcall TGenFourHeadsTesterForm::FPGARegExecuteMacroButtonClick(TObject *Sender)
{
	DWORD Adr, Data;

	for(int i = 0; i < FPGARegMacroMemo->Lines->Count; i++)
	{
		if(sscanf(FPGARegMacroMemo->Lines->Strings[i].c_str(), "%x %x", &Adr, &Data) != 2)
		{
			MessageDlg("Invalid macro format", mtError, TMsgDlgButtons() << mbOK,0);
			return;
		}

		WriteToFPGA((BYTE)Adr, (WORD)Data);
	}
}
//---------------------------------------------------------------------------


void __fastcall TGenFourHeadsTesterForm::TurnPowerOffButtonClick(TObject *Sender)
{
	TButton *btn = dynamic_cast<TButton *>(Sender);
	if(! btn) return;
	
	btn->Enabled = false;

	try
	{
		TurnPower(false);
	}
	__finally
	{
		btn->Enabled = true;
	}
}

void __fastcall TGenFourHeadsTesterForm::TurnPowerOnButtonClick(TObject *Sender)
{
	TButton *btn = dynamic_cast<TButton *>(Sender);
	if(! btn) return;

	btn->Enabled = false;

	try
	{
		TurnPower(true);
	}
	__finally
	{
		btn->Enabled = true;
	}
}
//---------------------------------------------------------------------------

void TGenFourHeadsTesterForm::PowerReceiveHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
	TOCBPowerStatusResponse *PowerMsg   = static_cast<TOCBPowerStatusResponse *>(Data);
	TGenFourHeadsTesterForm *InstancePtr  = reinterpret_cast<TGenFourHeadsTesterForm *>(Cockie);

	if (PowerMsg->MessageID != OCB_POWER_STATUS)
        return;

	InstancePtr->m_PowerStatus = PowerMsg->PowerOnOff;

    PostMessage(InstancePtr->Handle, WM_GEN4_POWER, reinterpret_cast<int>(0), reinterpret_cast<int>(0));
}

MESSAGE void TGenFourHeadsTesterForm::PowerStatus(TMessage &Message)
{  
	if(m_PowerStatus)
	{
		PowerButton->Caption = "Power OFF";
		PowerButton->OnClick = TurnPowerOffButtonClick;
		ButtonsImageList->GetBitmap(icoLedGreen, PowerStatusImg->Picture->Bitmap);
	}
	else
	{
		PowerButton->Caption = "Power ON";
		PowerButton->OnClick = TurnPowerOnButtonClick;
		ButtonsImageList->GetBitmap(icoLedRed, PowerStatusImg->Picture->Bitmap);
	}

	PowerStatusImg->Refresh();
}
