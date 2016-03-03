//---------------------------------------------------------------------------

#ifndef PacksViewerH
#define PacksViewerH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include "SynEdit.hpp"
#include "SynHighlighterXML.hpp"

//---------------------------------------------------------------------------
class TPacksViewerForm : public TForm
{
__published:	// IDE-managed Components
	TMainMenu *MainMenu;
	TMenuItem *FileMenu;
	TMenuItem *OpenMenu;
	TMenuItem *HelpMenu;
	TMenuItem *AboutMenu;
	TStatusBar *ConnectionStatusBar;
	TImageList *LedsImageList;
	TMenuItem *ExitMenu;
	TOpenDialog *OpenPackageDialog;
	TMenuItem *ExportXMLMenu;
	TSaveDialog *SaveFileDialog;
	TTimer *DongleConnectionTimer;
	void __fastcall AboutMenuClick(TObject *Sender);
	void __fastcall ExitMenuClick(TObject *Sender);
	void __fastcall OpenMenuClick(TObject *Sender);
	void __fastcall ExportXMLMenuClick(TObject *Sender);
	void __fastcall ConnectionStatusBarDrawPanel(TStatusBar *StatusBar,
          TStatusPanel *Panel, const TRect &Rect);
	void __fastcall DongleConnectionTimerTimer(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);

private:	// User declarations
	bool m_DongleIsConnected;
	bool m_QAFeatureFound;

	TSynEdit   *m_PackageContentEdit;
	TSynXMLSyn *m_XMLHighlighter;

	void __fastcall	WmDropFiles(TWMDropFiles& Message);
	void BuildEditorComponent();
	
public:		// User declarations
	__fastcall TPacksViewerForm(TComponent* Owner);

  BEGIN_MESSAGE_MAP
	MESSAGE_HANDLER(WM_DROPFILES, TWMDropFiles, WmDropFiles)
  END_MESSAGE_MAP(TForm)
};
//---------------------------------------------------------------------------
extern PACKAGE TPacksViewerForm *PacksViewerForm;
//---------------------------------------------------------------------------
#endif
