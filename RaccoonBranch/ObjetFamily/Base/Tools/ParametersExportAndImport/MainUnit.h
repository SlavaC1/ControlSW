//---------------------------------------------------------------------------

#ifndef ParamsExportDlgH
#define ParamsExportDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
#include <Menus.hpp>
#include <ActnList.hpp>
//---------------------------------------------------------------------------

#include <vector>

class CQApplication;
class CQParameterBase;
class CAppParams;

class TMainForm : public TForm
{
__published:	// IDE-managed Components
        TListBox *AllParamsListBox;
        TListBox *ForExportListBox;
        TLabel *Label1;
        TLabel *Label2;
        TButton *CloseButton;
        TSaveDialog *SaveDialog1;
        TPopupMenu *PopupMenu1;
        TMenuItem *Clear1;
        TMenuItem *N1;
        TMenuItem *Export1;
        TActionList *ActionList1;
        TAction *OpenParametersDialogAction;
        void __fastcall CloseButtonClick(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
private:	// User declarations

  CQApplication *m_MyQApp;
  CAppParams    *m_TempParamsMgr;
  CAppParams    *m_ParamsMgr;

  bool           m_Silent;  

  void AddParamForExport(CQParameterBase *Param);
  void SelectParamByName(AnsiString ParamName);

  void RefreshModesList(void);

  void ExportFile(const AnsiString FileName);

public:		// User declarations
        __fastcall TMainForm(TComponent* Owner);

};

// Import parameters from a given INI file, return true if application restart is required
bool ImportParameters(AnsiString FileName);

//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
