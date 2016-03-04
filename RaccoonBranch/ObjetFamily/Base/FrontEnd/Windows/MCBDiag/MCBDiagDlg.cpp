//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MCBDiagDlg.h"  
#include "QMonitor.h"
#include "BackEndInterface.h"
#include "EdenPCISys.h"
#include "Motor.h"
#include "QThreadUtils.h"
#include "AppParams.h"
#include "MotorDefs.h"
#include "AppLogFile.h"
#include "FrontEnd.h"
#include "Q2RTApplication.h"
#include "MachineSequencer.h"

#pragma package(smart_init)
#pragma resource "*.dfm"
TMCBDiagForm *MCBDiagForm;

#define WIPER_HEIGHT_ADDR      55
#define Y_AXIS_HOME_COUNT_ADDR 63

#define MCB_HELP_FILE          "\\ServiceTools\\MCB Loader\\MCB Loader Instructions.hlp"

__fastcall TMCBDiagForm::TMCBDiagForm(TComponent* Owner)
        : TForm(Owner)
{}

void __fastcall TMCBDiagForm::FormCreate(TObject *Sender)
{
   m_BackEndInterface = CBackEndInterface::Instance();
   m_ParamsMgr        = CAppParams::Instance(); 
	 m_bCommOK          = true;
}

void __fastcall TMCBDiagForm::ReadButtonClick(TObject *Sender)
{
  int Parameter = StrToInt(DiagParameterEdit->Text);

  if(Parameter < MCB_SW_TABLE_FirstElement ||
     Parameter > m_ParamsMgr->MaxMCBSWParameter)
  {
     QMonitor.ErrorMessage("Parameter out of range");
     return;
  }

  try
  {
     int Value           = m_BackEndInterface->MCBDiagRead(Parameter);
     DiagValueEdit->Text = Value;
     QString DiagString  = QFormatStr("Read Parameter[%d] = %d",Parameter,Value);
     DiagText->Lines->Add(DiagString.c_str());
  }
  catch(EQException& E)
  {
     QMonitor.ErrorMessage(E.GetErrorMsg());
  }
        
}

void __fastcall TMCBDiagForm::ReadAllButtonClick(TObject *Sender)
{
  try
  {
     int     Value;
     QString DiagString;
     for (int i = MCB_SW_TABLE_FirstElement; i <= m_ParamsMgr->MaxMCBSWParameter; i++)
     {
        Value      = m_BackEndInterface->MCBDiagRead(i);
        DiagString = QFormatStr("Read Parameter[%d] = %d",i,Value);
        DiagText->Lines->Add(DiagString.c_str());
        //DiagText->Lines->Clear();
        Application->ProcessMessages();
     }
  }
  catch(EQException& E)
  {
    QMonitor.ErrorMessage(E.GetErrorMsg());
  }
}

void __fastcall TMCBDiagForm::WriteButtonClick(TObject *Sender)
{
  int Parameter,Value;
  try
  {
     Parameter = StrToInt(DiagParameterEdit->Text);
     if(Parameter < MCB_SW_TABLE_FirstElement ||
        Parameter > m_ParamsMgr->MaxMCBSWParameter)
     {
       QMonitor.ErrorMessage("Parameter out of range");
       return;
     }
     Value = StrToInt(DiagValueEdit->Text);

     int     PreviousValue = m_BackEndInterface->MCBDiagRead(Parameter);
     QString DiagString    = QFormatStr("Previous Parameter[%d] = %d",Parameter,PreviousValue);
     DiagText->Lines->Add(DiagString.c_str());

     m_BackEndInterface->MCBDiagWrite(Parameter,Value);
     DiagString = QFormatStr("Write Parameter[%d] = %d",Parameter,Value);
     DiagText->Lines->Add(DiagString.c_str());
  }
  catch (Exception &exception)
  {
    QMonitor.ErrorMessage("Error writing diag parameter");
  }
}

void __fastcall TMCBDiagForm::CloseButtonClick(TObject *Sender)
{
  Close();
}

void __fastcall TMCBDiagForm::FormClose(TObject *Sender, TCloseAction &Action)
{
  try
  {
		 if(m_bCommOK) {
			 DiagText->Lines->Add(" ");
			 DiagText->Lines->Add("Closing. Please wait...");
       m_BackEndInterface->MCBReset();
     }
     CQLog::Write(LOG_TAG_MOTORS,"MCB Diag Mode exit");
     //m_BackEndInterface->WaitForEndOfSWReset(10);
  }
  catch(EQException& E)
  {
     QMonitor.ErrorMessage(E.GetErrorMsg());
  }
}

void __fastcall TMCBDiagForm::FormShow(TObject *Sender)
{

	Edit1->Text = "<empty>";
	DiagParameterEdit->Text = "";
	DiagValueEdit->Text = "";
	DiagText->Lines->Clear();

  try
  {
      m_BackEndInterface->EnableMotor(false, AXIS_ALL);
      m_BackEndInterface->MCBEnterDiag();
      CQLog::Write(LOG_TAG_MOTORS,"MCB Diag Mode enter");
  }
  catch(EQException& E)
  {
		 DiagText->Lines->Add("No communication with the MCB!");
		 m_bCommOK = false;
		 ReadButton->Enabled = false;
		 ReadAllButton->Enabled = false;
		 WriteButton->Enabled = false;
		 Browse->Enabled = false;
     QMonitor.ErrorMessage(E.GetErrorMsg());
  }
}

void __fastcall TMCBDiagForm::MCBSWLoadStartClick(TObject *Sender)
{

	MCBSWLoadStart->Enabled = false;
	CloseButton->Enabled = false;
	Browse->Enabled = false;

	MCBLoadInit();

	for(int i = 0; i< 10; i++) {
		FrontEndInterface->YieldUIThread();
		QSleep(500);
	}

	MCBLoadProcess();

	for(int i = 0; i< 10; i++) {
		FrontEndInterface->YieldUIThread();
		QSleep(500);
	}

	MCBLoadFinalize();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void TMCBDiagForm::MCBLoadInit()
{
//	CheckMotorCommunication(); // Ping

//	Start Mode Diag - not necessary after FormShow()

	DiagText->Lines->Add("Initializing MCB firmware download process...");

	try
	{
			// diag mode  zzz can be removed because the form-show sends this
			//m_BackEndInterface->MCBEnterDiag();
			//CQLog::Write(LOG_TAG_MOTORS,"MCB Diag Mode enter");

			// save wiper height param for later
			m_WiperHeightRestore = m_BackEndInterface->MCBDiagRead(WIPER_HEIGHT_ADDR);
			Application->ProcessMessages();
			m_YAxisHomeCountRestore = m_BackEndInterface->MCBDiagRead(Y_AXIS_HOME_COUNT_ADDR);
			Application->ProcessMessages();

			// download mode
			m_BackEndInterface->MCBDownload();
			CQLog::Write(LOG_TAG_MOTORS,"MCB download start");
	}
	catch(EQException& E)
	{
		 QMonitor.ErrorMessage(E.GetErrorMsg());
	}
}

void TMCBDiagForm::MCBLoadProcess()
{

	QString sProgress = "";

	int new_percentage = 0;

	DiagText->Lines->Add("Downloading MCB firmware file... [0%]");

	try
	{

		CQStringList StrList;

		StrList.LoadFromFile( Edit1->Text.c_str() );

		CQLog::Write(LOG_TAG_MOTORS,"MCB SW download from ROM file");

		int n = StrList.Count();

		for(int i = 0; i < n ; i++) {

			// download new SW for the MCB from a ROM file
			m_BackEndInterface->MCBSendRomFileLine( StrList[i] );

			new_percentage = (100 * i) / n;

			// change the progress only when the percentage updates (to reduce flickering)
			if( new_percentage > ProgressBar1->Position )
			{
				ProgressBar1->Position = new_percentage;

				sProgress = QFormatStr("Downloading MCB firmware file... [%d\%]", ProgressBar1->Position);
				DiagText->Lines->Strings[DiagText->Lines->Count - 1] = sProgress.c_str();
			}

			FrontEndInterface->YieldUIThread();
		}
	}
	catch(EQException& E)
	{
		 QMonitor.ErrorMessage(E.GetErrorMsg());
	}

	DiagText->Lines->Strings[DiagText->Lines->Count - 1] = "Downloading MCB firmware file... [100%]";
}

void TMCBDiagForm::MCBLoadFinalize()
{

	DiagText->Lines->Add("Finalizing MCB download process...");

	try
	{
		 // Reset
		 m_BackEndInterface->MCBReset();
		 CQLog::Write(LOG_TAG_MOTORS,"MCB SW Reset");

		 // Diag start
		 //m_BackEndInterface->EnableMotor(false, AXIS_ALL);
		 m_BackEndInterface->MCBEnterDiag();
		 CQLog::Write(LOG_TAG_MOTORS,"MCB Diag Mode enter");

		 // Diag Initiate Data Log Counter
		 m_BackEndInterface->DiagInitiateDataLogCounter(1); // 1 == DIAG_INIT_DataSoftSW, as it is called in the 'Tornado' application
		 CQLog::Write(LOG_TAG_MOTORS,"MCB Diag initiate data log counter");

		 // restore wiper height param
		 try
		 {
			 m_BackEndInterface->MCBDiagWrite(WIPER_HEIGHT_ADDR, m_WiperHeightRestore);
			 Application->ProcessMessages();
			 m_BackEndInterface->MCBDiagWrite(Y_AXIS_HOME_COUNT_ADDR, m_YAxisHomeCountRestore);
			 Application->ProcessMessages();
		 }
		 catch (Exception &exception)
		 {
			 QMonitor.ErrorMessage("Error restoring wiper height parameter");
		 }

		 // Reset
		 //m_BackEndInterface->MCBReset();
		 //CQLog::Write(LOG_TAG_MOTORS,"MCB SW Reset");
	}
	catch(EQException& E)
	{
		 QMonitor.ErrorMessage(E.GetErrorMsg());
	}

	ProgressBar1->Position = 0;

	DiagText->Lines->Add("MCB download process completed successfully.");
	DiagText->Lines->Add(" ");

	CloseButton->Enabled = true;
	Browse->Enabled = true;
}


void __fastcall TMCBDiagForm::BrowseClick(TObject *Sender)
{

	OpenDialog1->Filter = "MCB firmware file (*.rom)|*.rom|";

	if( OpenDialog1->Execute() ) {

		Edit1->Text = OpenDialog1->FileName;

		MCBSWLoadStart->Enabled = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TMCBDiagForm::Button1Click(TObject *Sender)
{
  CMachineSequencer *MachineSequencer = Q2RTApplication->GetMachineManager()->GetMachineSequencer();

  MachineSequencer->DispatchHelp(10, MCB_HELP_FILE);
}
//---------------------------------------------------------------------------

