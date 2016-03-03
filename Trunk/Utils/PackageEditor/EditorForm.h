//---------------------------------------------------------------------------

#ifndef EditorFormH
#define EditorFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
#include <ComCtrls.hpp>
#include "PackageReaderWriter.h"
#include <ImgList.hpp>
#include <ExtCtrls.hpp>
#include <list>

//---------------------------------------------------------------------------
class TPackageEditorForm : public TForm
{
__published:	// IDE-managed Components
	TMainMenu *MainMenu1;
	TMenuItem *FileMenu;
	TMenuItem *HelpMenu;
	TMenuItem *OpenPackagesMenu;
	TMenuItem *SavePackagesMenu;
	TMenuItem *ExitMenu;
	TMenuItem *AboutMenu;
	TOpenDialog *OpenPackageDialog;
	TSaveDialog *SavePackageDialog;
	TMenuItem *NewPackagesMenu;
	TStatusBar *ConnectionStatusBar;
	TImageList *LedsImageList;
	TTimer *DongleConnectionTimer;
	void __fastcall ExitMenuClick(TObject *Sender);
	void __fastcall OpenPackagesMenuClick(TObject *Sender);
	void __fastcall SavePackagesMenuClick(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall ConnectionStatusBarDrawPanel(TStatusBar *StatusBar,
          TStatusPanel *Panel, const TRect &Rect);
	void __fastcall DongleConnectionTimerTimer(TObject *Sender);
	void __fastcall AboutMenuClick(TObject *Sender);
	void __fastcall PageControlAddNewTab(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y, int HitTest, TMouseActivate &MouseActivate);

private:	// User declarations

	bool m_DongleIsConnected;
	TPageControl *m_PageControl;
	CPackageReaderWriter *m_PackageReaderWriter;

	void AddImportedPackages();
	void AddPackagesTree(TTabSheet *MachineTab, TMachinesList::iterator &MachinesIt);
	void AddPackagesEditor(TTabSheet *MachineTab, TMachinesList::iterator &MachinesIt);
	QString ShortModeNameToFullName(QString ShortName);

public:		// User declarations
	__fastcall TPackageEditorForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TPackageEditorForm *PackageEditorForm;
//---------------------------------------------------------------------------
#endif
