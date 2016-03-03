//---------------------------------------------------------------------------

#ifndef SystemFileDlgH
#define SystemFileDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <FileCtrl.hpp>
#include "QTypes.h"

#define SYSTEM_DIRECTORY_NAME   ":\\Tester"
//---------------------------------------------------------------------------
class TSystemFileForm : public TForm
{
__published:	// IDE-managed Components
  TBitBtn *ApproveBitBtn;
  TBitBtn *BitBtn2;
  TLabel *Label1;
  TLabel *Label2;
  TDriveComboBox *DriveComboBox;
  TDirectoryListBox *DirectoryListBox;
  void __fastcall FormShow(TObject *Sender);
  void __fastcall DriveComboBoxChange(TObject *Sender);
  void __fastcall ApproveBitBtnClick(TObject *Sender);
private:	// User declarations
  bool LookForSystemDir();

  // the direcory the user selected
  QString m_SelectedDiretory;

  // the last working directory
  AnsiString m_LastWorkingDir;
public:		// User declarations
  __fastcall TSystemFileForm(TComponent* Owner);

  // returns the directory the user selected
  QString GetSelectedDirectory();

  // show the form with last working directory (if exists)
  bool Open(AnsiString LastWorkingDirectory);
};
//---------------------------------------------------------------------------
extern PACKAGE TSystemFileForm *SystemFileForm;
//---------------------------------------------------------------------------
#endif
