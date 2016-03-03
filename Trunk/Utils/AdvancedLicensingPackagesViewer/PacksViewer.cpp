//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "PacksViewer.h"
#include "PackageReader.h"
#include "About.h"        
#include "hasp_api_cpp_.h"
#include <shellapi.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TPacksViewerForm *PacksViewerForm;

//---------------------------------------------------------------------------
__fastcall TPacksViewerForm::TPacksViewerForm(TComponent* Owner)
	: TForm(Owner)
{
    BuildEditorComponent();

	m_DongleIsConnected = false;
	
	OpenPackageDialog->Filter  = "DAT files (*.dat)|*.DAT|Any file (*.*)|*.*";
	SaveFileDialog->Filter     = "TXT files (*.txt)|*.TXT|Any file (*.*)|*.*";
	SaveFileDialog->DefaultExt = "txt";

	int Feature = 1001;
	Chasp hasp(ChaspFeature::fromFeature(Feature));
	try
	{
		haspStatus status = hasp.login(HASP_OBJET_VENDOR_CODE, HASP_LOCAL_SCOPE);
		if(! HASP_SUCCEEDED(status))
		{
			MessageDlg("Objet QA HASP feature was not found", mtWarning, TMsgDlgButtons() << mbOK, 0);
			m_QAFeatureFound = false;
		}
		else
		{
			m_QAFeatureFound = true;
        }                           
    }
	__finally
	{
		hasp.logout();
	}
}

void TPacksViewerForm::BuildEditorComponent()
{
	m_PackageContentEdit = new TSynEdit(this);
	m_XMLHighlighter     = new TSynXMLSyn(this);

	m_XMLHighlighter->ElementAttri->Foreground        = clDkGray;
	m_XMLHighlighter->ElementAttri->Style             = TFontStyles();
	m_XMLHighlighter->AttributeValueAttri->Foreground = clTeal;
	m_XMLHighlighter->TextAttri->Foreground           = clNavy; 

	m_PackageContentEdit->Parent                    = this;
	m_PackageContentEdit->Align                     = alClient;
	m_PackageContentEdit->ScrollBars                = ssVertical;
	m_PackageContentEdit->Gutter->ShowLineNumbers   = true;
	m_PackageContentEdit->Gutter->LeftOffset        = 0;
	m_PackageContentEdit->Gutter->Gradient          = true;
	m_PackageContentEdit->RightEdge                 = 200;
	m_PackageContentEdit->RightEdgeColor            = clWindow;
	m_PackageContentEdit->SelectionMode             = smLine;
	m_PackageContentEdit->SelectedColor->Background = clSkyBlue;
	m_PackageContentEdit->Highlighter               = m_XMLHighlighter;
	m_PackageContentEdit->ReadOnly                  = true;
}


//---------------------------------------------------------------------------
void __fastcall TPacksViewerForm::AboutMenuClick(TObject *Sender)
{
	AboutForm->Left = (this->Left + this->Width / 2) - (AboutForm->Width / 2);
	AboutForm->Top  = (this->Top + this->Height / 2) - (AboutForm->Height / 2);
	AboutForm->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TPacksViewerForm::ExitMenuClick(TObject *Sender)
{
	Close();	
}
//---------------------------------------------------------------------------

void __fastcall TPacksViewerForm::OpenMenuClick(TObject *Sender)
{
	if(! m_DongleIsConnected)
	{
		MessageDlg("HASP dongle is not connected", mtWarning, TMsgDlgButtons() << mbOK, 0);
		return;
	}

	if(OpenPackageDialog->Execute())
	{
		if (FileExists(OpenPackageDialog->FileName))
		{
			CPackageReader *PackageReader = new CPackageReader();

			try
			{
				m_PackageContentEdit->Text = PackageReader->ReadDatabaseFile(OpenPackageDialog->FileName.c_str()).c_str();
			}
			__finally
			{
				delete PackageReader;
			}
		}
	}
}
//--------------------------------------------------------------------------- 

void __fastcall TPacksViewerForm::ExportXMLMenuClick(TObject *Sender)
{
	if(m_PackageContentEdit->Lines->Count == 0)
	{
		MessageDlg("No open packages", mtWarning, TMsgDlgButtons() << mbOK, 0);
		return;
	}

	if(SaveFileDialog->Execute())
		m_PackageContentEdit->Lines->SaveToFile(SaveFileDialog->FileName.c_str());
}
//---------------------------------------------------------------------------

void __fastcall TPacksViewerForm::ConnectionStatusBarDrawPanel(
      TStatusBar *StatusBar, TStatusPanel *Panel, const TRect &Rect)
{
	AnsiString Text;
	int LedIconIndex;

	if(m_DongleIsConnected)
	{
		Text = "HASP dongle is connected";
		LedIconIndex = 0;
	}
	else
	{
		Text = "HASP dongle is not connected";
		LedIconIndex = 1;
    }

	StatusBar->Canvas->TextRect(Rect,10 + LedsImageList->Width + Rect.Left, 2 + Rect.Top, Text);
	LedsImageList->Draw(ConnectionStatusBar->Canvas, Rect.Left, Rect.Top, LedIconIndex);	
}
//---------------------------------------------------------------------------

void __fastcall TPacksViewerForm::DongleConnectionTimerTimer(TObject *Sender)
{
	int Feature = 0;
	Chasp hasp(ChaspFeature::fromFeature(Feature));
	try
	{
		haspStatus status = hasp.login(HASP_OBJET_VENDOR_CODE, HASP_LOCAL_SCOPE);
		m_DongleIsConnected = HASP_SUCCEEDED(status) ? true : false;
		ConnectionStatusBar->Refresh();
    }
	__finally
	{
		hasp.logout();
	}	
}
//---------------------------------------------------------------------------

void __fastcall TPacksViewerForm::FormShow(TObject *Sender)
{
	if(! m_QAFeatureFound)
		Close();
}
//---------------------------------------------------------------------------

void __fastcall TPacksViewerForm::FormCreate(TObject *Sender)
{
	DragAcceptFiles(Handle, true);	
}
//---------------------------------------------------------------------------


void __fastcall TPacksViewerForm::WmDropFiles(TWMDropFiles& Message)
{
    if(! m_DongleIsConnected)
	{
		MessageDlg("HASP dongle is not connected", mtWarning, TMsgDlgButtons() << mbOK, 0);
		return;
	}

	char buff[MAX_PATH];
	HDROP hDrop = (HDROP)Message.Drop;
	int numFiles = DragQueryFile(hDrop, -1, NULL, NULL);

	if(numFiles > 1)
	{
    	MessageDlg("Only one packages database file could be opened at a time", mtWarning, TMsgDlgButtons() << mbOK, 0);
		return;
	}

	m_PackageContentEdit->Lines->Clear();
	CPackageReader *PackageReader = new CPackageReader();

	try
	{
		DragQueryFile(hDrop, 0, buff, sizeof(buff));
		m_PackageContentEdit->Text = PackageReader->ReadDatabaseFile(QString(buff).c_str()).c_str();
	}
	__finally
	{
		delete PackageReader;
		DragFinish(hDrop);
	}
	
}
