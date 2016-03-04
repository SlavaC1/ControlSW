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
#include "ModesDefs.h"
#include "QTypes.h"

//---------------------------------------------------------------------------

#include <vector>

class CQParameterBase;
class CAppParams;

class TParamsExportForm : public TForm
{
__published:	// IDE-managed Components
        TListBox *AllParamsListBox;
        TListBox *ForExportListBox;
        TLabel *Label1;
        TLabel *Label2;
        TSpeedButton *AddToSelectionButton;
        TGroupBox *GroupBox1;
        TButton *AllButton;
        TButton *NoneButton;
        TButton *SaveAsButton;
        TButton *CloseButton;
        TSpeedButton *RemoveFromSelectionButton;
        TBevel *Bevel1;
        TSaveDialog *SaveDialog1;
        TPopupMenu *PopupMenu1;
        TMenuItem *Clear1;
        TMenuItem *N1;
        TMenuItem *Export1;
        TComboBox *ModesComboBox;
        TLabel *Label3;
        TButton *EditCollectionButton;
        TActionList *ActionList1;
        TAction *OpenParametersDialogAction;
        TButton *SaveButton;
        TOpenDialog *OpenDialog1;
        void __fastcall CloseButtonClick(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall AllButtonClick(TObject *Sender);
        void __fastcall NoneButtonClick(TObject *Sender);
        void __fastcall AddToSelectionButtonClick(TObject *Sender);
        void __fastcall RemoveFromSelectionButtonClick(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall ForExportListBoxDragOver(TObject *Sender,
          TObject *Source, int X, int Y, TDragState State, bool &Accept);
        void __fastcall ForExportListBoxDragDrop(TObject *Sender,
          TObject *Source, int X, int Y);
        void __fastcall SaveAsButtonClick(TObject *Sender);
        void __fastcall AllParamsListBoxDblClick(TObject *Sender);
        void __fastcall Clear1Click(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall EditCollectionButtonClick(TObject *Sender);
        void __fastcall ModesComboBoxClick(TObject *Sender);
        void __fastcall OpenParametersDialogActionExecute(TObject *Sender);
        void __fastcall SaveButtonClick(TObject *Sender);
private:	// User declarations
  CAppParams *m_TempParamsMgr;

  TQualityModeIndex   m_MaintenanceMode;
  TOperationModeIndex m_OperationMode;
  QString m_ModesDirName;

  void AddParamForExport(CQParameterBase *Param);
  void SelectParamByName(AnsiString ParamName);

  void RefreshModesList(void);

  void ExportFile(const AnsiString FileName);

public:		// User declarations
  void       SetMaintenanceMode (TQualityModeIndex QualityMode, TOperationModeIndex OperationMode);

        __fastcall TParamsExportForm(TComponent* Owner);
};

// Import parameters from a given INI file, return true if application restart is required
bool ImportParameters(AnsiString FileName);

//---------------------------------------------------------------------------
extern PACKAGE TParamsExportForm *ParamsExportForm;
//---------------------------------------------------------------------------
#endif
