//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MainUnit.h"
#include "SystemFileDlg.h"
#include "SetupDlg.h"
#include "AppParams.h"
#include "OCBSimulatorDlg.h"
#include "OHDBTesterDlg.h"
#include "OCBTesterDlg.h"
#include "MCBSimulatorDlg.h"
#include "HeadsTester.h"
#include "ReliabilityTest.h"
#include "EdenProtocolEngine.h"
#include "RFIDTester.h"
#include "QFileWithChecksumParamsStream.h"
#include "GenFourHeadsTester.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMainForm *MainForm;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
    : TForm(Owner)
{
	m_HeadTypeSelectionResult = static_cast<THeadTypeSelectionResult>(-1);

	StatusBar->Panels[0][0]->Text = __DATE__;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormCreate(TObject *Sender)
{
    CTesterApplication::CreateInstance();
    m_TesterApplication = CTesterApplication::GetInstance();
    m_AppParams = CAppParams::GetInstance();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormDestroy(TObject *Sender)
{
    CTesterApplication::DeleteInstance();
    Q_SAFE_DELETE(m_SystemParam);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ActivateParamsDialogActionExecute(
    TObject *Sender)
{
//
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::OpenSetupActionExecute(TObject *Sender)
{
    if(SystemFileForm->Open(m_AppParams->LastWorkingDirectory.Value().c_str()))
    {
        // save the last working directory
        m_AppParams->LastWorkingDirectory.Value() = SystemFileForm->GetSelectedDirectory().c_str();

        // Create a system parameters manager object
        if (m_SystemParam)  // if the object exists, delete it
            Q_SAFE_DELETE(m_SystemParam);
        m_SystemParam = new CSystemParams((SystemFileForm->GetSelectedDirectory() + SYSTEM_CONFIG_FILE_NAME).c_str());

        // Open the setup form with the new system parameters object
        if (SetupForm->Open(m_SystemParam))
        {
            // If returned with Ok, apply the selected setup
            // open the com ports etc.

            if (m_SystemParam->EmbeddedConnectionKind == DIRECT)    // todo -oNobody -cNone: handle the indirect modes
                m_TesterApplication->InitComPort(m_SystemParam->EmbeddedComNum);
            if (m_SystemParam->ContainersConnectionKind == DIRECT)
                m_TesterApplication->InitComPort(m_SystemParam->ContainersComNum);
            if (m_SystemParam->LCDConnectionKind == DIRECT)
                m_TesterApplication->InitComPort(m_SystemParam->LCDComNum);
            if (m_SystemParam->MCBConnectionKind == DIRECT)
                m_TesterApplication->InitComPort(m_SystemParam->MCBComNum);
            if (m_SystemParam->OHDBConnectionKind == DIRECT)
                m_TesterApplication->InitComPort(m_SystemParam->OHDBComNum);
            if (m_SystemParam->OCBConnectionKind == DIRECT)
                m_TesterApplication->InitComPort(m_SystemParam->OCBComNum);
            if (m_SystemParam->OCBSimConnectionKind == DIRECT)
                m_TesterApplication->InitComPort(m_SystemParam->OCBSimComNum);

            // enable the menu items
            OCBsimulator->Enabled = true;
            OCBTester->Enabled = true;
            OHDBTester->Enabled = true;
            MCBSimulator->Enabled = true;
            HeadsTester->Enabled = true;
            ReliabilityTest->Enabled = true;
        }


    }
}
//---------------------------------------------------------------------------


// Get the instance of the system parametres manager
CSystemParams* TMainForm::GetSystemParamsIntance()
{
    return m_SystemParam;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::OCBSimulatorActionExecute(TObject *Sender)
{
    if (m_SystemParam->OCBSimConnectionKind == NONE)
        MessageDlg("The OCB simulator is not associated with a com port",mtError,TMsgDlgButtons() << mbOK,0);
    else
    {
        CEdenProtocolEngine *ProtocolEngine = m_TesterApplication->GetProtocolEngine(m_SystemParam->OCBSimComNum);

        OCBSimulatorForm->Open(ProtocolEngine);
    }
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::OHDBTesterClick(TObject *Sender)
{
	CEdenProtocolEngine *ProtocolEngine = GetOHDBProtocolEngine();
	if(ProtocolEngine)
		OHDBTesterForm->Open(ProtocolEngine);
}

CEdenProtocolEngine* TMainForm::GetOHDBProtocolEngine()
{
	CEdenProtocolEngine *ProtocolEngine = NULL;
	
	switch(m_SystemParam->OHDBConnectionKind)
	{
		case DIRECT:  // direct connection to a com port
			ProtocolEngine = m_TesterApplication->GetProtocolEngine(m_SystemParam->OHDBComNum);
			break;

		case THROUGH_EMBEDDED:  // connect through the com port embedded use
			if (m_SystemParam->EmbeddedConnectionKind == NONE)
			{
				MessageDlg("Embedded is not associated with a com port",mtError,TMsgDlgButtons() << mbOK,0);
				break;
			}
			ProtocolEngine = m_TesterApplication->GetProtocolEngine(m_SystemParam->EmbeddedComNum);
			break;

		case THROUGH_OCB:
			if (m_SystemParam->OCBConnectionKind == NONE)
			{
				MessageDlg("OCB is not associated with a com port",mtError,TMsgDlgButtons() << mbOK,0);
				break;
			}
			ProtocolEngine = m_TesterApplication->GetProtocolEngine(m_SystemParam->OCBComNum);
			break;

		case NONE:
			MessageDlg("OHDB is not associated with a com port",mtError,TMsgDlgButtons() << mbOK,0);
			break;
	}

	return ProtocolEngine;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::OCBTesterClick(TObject *Sender)
{
	CEdenProtocolEngine *ProtocolEngine = GetOCBProtocolEngine();
	if(ProtocolEngine)
		OCBTesterForm->Open(ProtocolEngine);
}

CEdenProtocolEngine* TMainForm::GetOCBProtocolEngine()
{
	CEdenProtocolEngine *ProtocolEngine = NULL;
	switch(m_SystemParam->OCBConnectionKind)
    {
		case DIRECT:  // direct connection to a com port
			ProtocolEngine = m_TesterApplication->GetProtocolEngine(m_SystemParam->OCBComNum);
			break;

		case THROUGH_EMBEDDED:  // connect through the com port embedded use
			if (m_SystemParam->EmbeddedConnectionKind == NONE)
			{
				MessageDlg("Embedded is not associated with a com port",mtError,TMsgDlgButtons() << mbOK,0);
				break;
			}
			ProtocolEngine = m_TesterApplication->GetProtocolEngine(m_SystemParam->EmbeddedComNum);
			break;

		case NONE:
			MessageDlg("OCB is not associated with a com port",mtError,TMsgDlgButtons() << mbOK,0);
			break;
	}
	return ProtocolEngine;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::MCBSimulatorClick(TObject *Sender)
{
    CEdenProtocolEngine *ProtocolEngine;
    switch(m_SystemParam->MCBConnectionKind)
    {
    case DIRECT:  // direct connection to a com port
        ProtocolEngine = m_TesterApplication->GetProtocolEngine(m_SystemParam->MCBComNum);
        break;
    case THROUGH_EMBEDDED:  // connect through the com port embedded use
        if (m_SystemParam->EmbeddedConnectionKind == NONE)
        {
            MessageDlg("Embedded is not associated with a com port",mtError,TMsgDlgButtons() << mbOK,0);
            return;
        }
        ProtocolEngine = m_TesterApplication->GetProtocolEngine(m_SystemParam->EmbeddedComNum);
        break;
    case NONE:
        MessageDlg("MCB is not associated with a com port",mtError,TMsgDlgButtons() << mbOK,0);
        return;
    }

    MCBSimulatorForm->Open(ProtocolEngine);

}
//---------------------------------------------------------------------------


void __fastcall TMainForm::HeadsTesterClick(TObject *Sender)
{
	CEdenProtocolEngine *OHDBProtocolEngine = GetOHDBProtocolEngine();
	CEdenProtocolEngine *OCBProtocolEngine  = GetOCBProtocolEngine();

	GenFourHeadsTesterForm->Open(OCBProtocolEngine, OHDBProtocolEngine);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    CanClose = (MessageDlg("Are you sure you want to exit?",mtConfirmation,TMsgDlgButtons() << mbYes << mbNo,0) == mrYes);

    if( CanClose )
    {
        m_AppParams->DeInit();
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ReliabilityTestClick(TObject *Sender)
{
    CEdenProtocolEngine *OCBProtocolEngine, *OHDBProtocolEngine;
    switch(m_SystemParam->OHDBConnectionKind)
    {
    case DIRECT:  // direct connection to a com port
        OHDBProtocolEngine = m_TesterApplication->GetProtocolEngine(m_SystemParam->OHDBComNum);
        break;
    case THROUGH_EMBEDDED:  // connect through the com port embedded use
        if (m_SystemParam->EmbeddedConnectionKind == NONE)
        {
            MessageDlg("Embedded is not associated with a com port",mtError,TMsgDlgButtons() << mbOK,0);
            return;
        }
        OHDBProtocolEngine = m_TesterApplication->GetProtocolEngine(m_SystemParam->EmbeddedComNum);
        break;
    case THROUGH_OCB:
        if (m_SystemParam->OCBConnectionKind == NONE)
        {
            MessageDlg("OCB is not associated with a com port",mtError,TMsgDlgButtons() << mbOK,0);
            return;
        }
        OHDBProtocolEngine = m_TesterApplication->GetProtocolEngine(m_SystemParam->OCBComNum);
        break;
    case NONE:
        MessageDlg("OHDB is not associated with a com port",mtError,TMsgDlgButtons() << mbOK,0);
        return;
    }

    switch(m_SystemParam->OCBConnectionKind)
    {
    case DIRECT:  // direct connection to a com port
        OCBProtocolEngine = m_TesterApplication->GetProtocolEngine(m_SystemParam->OCBComNum);
        break;
    case THROUGH_EMBEDDED:  // connect through the com port embedded use
        if (m_SystemParam->EmbeddedConnectionKind == NONE)
        {
            MessageDlg("Embedded is not associated with a com port",mtError,TMsgDlgButtons() << mbOK,0);
            return;
        }
        OCBProtocolEngine = m_TesterApplication->GetProtocolEngine(m_SystemParam->EmbeddedComNum);
        break;
    case NONE:
        MessageDlg("OCB is not associated with a com port",mtError,TMsgDlgButtons() << mbOK,0);
        return;
    }

    ReliabilityTestForm->Open(OHDBProtocolEngine, OCBProtocolEngine);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::RFIDClick(TObject *Sender)
{
    RFIDForm->Open();
}
void __fastcall TMainForm::ChecksumGenMenuItemClick(TObject *Sender)
{
    OpenFileDialog->Filter = "Configuration files (*.cfg)|*.cfg|";
    if(OpenFileDialog->Execute())
    {
        CQParamsFileWithChecksumStream ParamsStream(OpenFileDialog->FileName.c_str());
        if(!ParamsStream.SaveFileWithChecksum())
        {
            MessageDlg("Checksum was not created, please check file!",mtError,TMsgDlgButtons() << mbOK,0);
            return;
        }
        else
        {
            MessageDlg("New checksum was generated.",mtInformation,TMsgDlgButtons() << mbOK,0);
            return;
        }
    }
}
//---------------------------------------------------------------------------



