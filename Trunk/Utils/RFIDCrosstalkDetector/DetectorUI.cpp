//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "DetectorUI.h"
#include "About.h"
#include "ReportFile.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TDetectorForm *DetectorForm;
//---------------------------------------------------------------------------
__fastcall TDetectorForm::TDetectorForm(TComponent* Owner)
	: TForm(Owner)
{
	CyclesEdit->OnKeyPress   = DigitsFilter;
	PassFailEdit->OnKeyPress = DigitsFilter;

	m_ReaderThread = new CReaderThread();
}

//---------------------------------------------------------------------------
void __fastcall TDetectorForm::Exit1Click(TObject *Sender)
{
	Close();
}

//---------------------------------------------------------------------------
void __fastcall TDetectorForm::FormDestroy(TObject *Sender)
{
    m_ReaderThread->Terminate();
	Q_SAFE_DELETE(m_ReaderThread);
}
//---------------------------------------------------------------------------

void __fastcall TDetectorForm::DigitsFilter(TObject *Sender, char &Key)
{
    if ((Key >= '0') && (Key <= '9'))
        return;

    if (Key == VK_BACK)
        return;

    if (Key == VK_RETURN)
        return;

    Key = 0;
}

void __fastcall TDetectorForm::StartButtonClick(TObject *Sender)
{
	TTestData TestData;
	TestData.TestingCycles     = CyclesEdit->Text.ToInt();
	TestData.PassFailCriteria  = PassFailEdit->Text.ToInt();
	TestData.SelectedCOM       = COMPortCombo->ItemIndex + 1;
	TestData.SelectedContainer = static_cast<TContainer>(ContainerSelectorCombo->ItemIndex);

    ProgressBar->Position = 0;
	ProgressBar->Max      = TestData.TestingCycles;

    ResultsGrid->Clear();
	EnableDisableControls(false);
	
	m_ReaderThread->SetTestData(TestData);
	m_ReaderThread->Resume();
}
//---------------------------------------------------------------------------

void __fastcall TDetectorForm::FormShow(TObject *Sender)
{
    // Populate the combo boxes
	for(int i = 0; i < NUM_OF_CONTAINERS; i++)
		ContainerSelectorCombo->AddItem(ContainerNames[i].c_str(), NULL);
	ContainerSelectorCombo->ItemIndex = 0;

	for(int i = 0; i < NUM_OF_COM_PORTS; i++)
		COMPortCombo->AddItem(COMPorts[i].c_str(), NULL);
    COMPortCombo->ItemIndex = 0;
}
//---------------------------------------------------------------------------

void TDetectorForm::UpdateProgress(int Percent)
{
	PostMessage(Handle, WM_UPDATE_PROGRESS, 0, Percent);
}

void TDetectorForm::ReportError(QString Error)
{
	PostMessage(Handle, WM_REPORT_ERROR, 0, reinterpret_cast<int>(strdup(Error.c_str())));
}

void TDetectorForm::MessageHandler(TMessage &Message)
{
	switch(Message.Msg)
	{
		case WM_UPDATE_PROGRESS:
			ProgressBar->Position = Message.LParam;
			break;

		case WM_REPORT_ERROR:
			char *StrPtr = reinterpret_cast<char *>(Message.LParam);
			Application->MessageBoxA(StrPtr, "Error", MB_ICONERROR);
			free(StrPtr);
			StartButton->Enabled = true;
			break;
    }
}

void TDetectorForm::ShowResults(TTestData &TestData)
{
	ResultsMsgLabel->Caption     = "Test result: OK";
	ResultsMsgLabel->Font->Color = clGreen;

	for(int i = 0; i < NUM_OF_CONTAINERS; i++)
	{
		TListItem *ListItem = ResultsGrid->Items->Add();
		ListItem->Caption   = ContainerNames[i].c_str();

		TResultsTable::iterator it;
		bool FirstLine = true;

		for(it = TestData.TestResults.ResultsPerTag[i].begin(); it != TestData.TestResults.ResultsPerTag[i].end(); ++it)
		{
			if(FirstLine)
			{
            	ListItem->SubItems->Add(it->first);
				ListItem->SubItems->Add(it->second);

				FirstLine = false;
			}
			else
			{
				TListItem *SubItem = ResultsGrid->Items->Add();
				SubItem->Caption   = "";
				SubItem->SubItems->Add(it->first);
				SubItem->SubItems->Add(it->second);
			}
        }

		// Initial implementation:
		// ----------------------------------------------------------------------------------
		// Calculating pass / fail percentage using only the counts of the first Tag ID found
		// assuming it is the right one. Must be redesigned after "true" requirements will be issued
		
		it = TestData.TestResults.ResultsPerTag[i].begin();
		if(i != TestData.SelectedContainer && it->second != 0)
		{
			int Percentage = (double)it->second / (double)TestData.TestingCycles * 100;
			TestData.TestResults.Percentage[i] = Percentage;
			ListItem->SubItems->Add(Percentage);
			if(Percentage > TestData.PassFailCriteria)
			{
				ResultsMsgLabel->Caption     = "Test result: FAILED";
				ResultsMsgLabel->Font->Color = clRed;
				TestData.TestResults.Passed  = false;
			}
		}
	}

	if(GenerateReportCheckBox->Checked)
		GenerateReport(TestData);

	EnableDisableControls(true);
}

void TDetectorForm::EnableDisableControls(bool Enable)
{
	StartButton->Enabled     = Enable;
	ResultsMsgLabel->Visible = Enable;
}

void __fastcall TDetectorForm::About1Click(TObject *Sender)
{
	AboutBox->ShowModal();	
}
//---------------------------------------------------------------------------

void TDetectorForm::GenerateReport(const TTestData &TestData)
{
	CReportBase *Report;
	try
	{
		Report = new CReportTXT(ExtractFilePath(Application->ExeName).c_str() + REPORT_FILE_NAME_TXT, TestData);
	}
	__finally
	{
    	Q_SAFE_DELETE(Report);
    }
}

