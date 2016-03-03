//---------------------------------------------------------------------------

#ifndef DetectorUIH
#define DetectorUIH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include "ReaderThread.h"

#define WM_UPDATE_PROGRESS WM_USER + 1
#define WM_REPORT_ERROR    WM_USER + 2

const QString REPORT_FILE_NAME_TXT = "RFCrosstalkDetectorReport.txt";
const QString REPORT_FILE_NAME_XML = "RFCrosstalkDetectorReport.xml";

//---------------------------------------------------------------------------
class TDetectorForm : public TForm
{
__published:	// IDE-managed Components
	TMainMenu *MainMenu1;
	TMenuItem *File1;
	TMenuItem *Help1;
	TMenuItem *About1;
	TMenuItem *Exit1;
	TButton *StartButton;
	TGroupBox *SettingsGroupBox;
	TProgressBar *ProgressBar;
	TGroupBox *ResultsGroupbox;
	TLabel *ContainerSelectorLabel;
	TComboBox *ContainerSelectorCombo;
	TEdit *CyclesEdit;
	TLabel *CyclesLabel;
	TEdit *PassFailEdit;
	TLabel *PassFailLabel;
	TListView *ResultsGrid;
	TLabel *Label1;
	TComboBox *COMPortCombo;
	TLabel *Label2;
	TLabel *ResultsMsgLabel;
	TCheckBox *GenerateReportCheckBox;
	void __fastcall Exit1Click(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall StartButtonClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall About1Click(TObject *Sender);
private:	// User declarations

	CReaderThread *m_ReaderThread;

	void __fastcall DigitsFilter(TObject *Sender, char &Key);
	void EnableDisableControls(bool Enable);
	void GenerateReport(const TTestData &TestData);
	
public:		// User declarations
	__fastcall TDetectorForm(TComponent* Owner);

	void UpdateProgress(int Percent);
	void ShowResults(TTestData &TestData);
	void ReportError(QString Error);
	
	MESSAGE void MessageHandler(TMessage &Message);

BEGIN_MESSAGE_MAP
	MESSAGE_HANDLER(WM_UPDATE_PROGRESS, TMessage, MessageHandler);
	MESSAGE_HANDLER(WM_REPORT_ERROR,    TMessage, MessageHandler);
END_MESSAGE_MAP(TForm);
};
//---------------------------------------------------------------------------
extern PACKAGE TDetectorForm *DetectorForm;
//---------------------------------------------------------------------------
#endif
