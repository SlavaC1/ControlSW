//---------------------------------------------------------------------------

#ifndef MaintenanceCountersDlgH
#define MaintenanceCountersDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <Menus.hpp>
#include <ExtCtrls.hpp>
#include <ActnList.hpp>
#include <Dialogs.hpp>
#include "QTypes.h"
#include "MaintenanceCountersDefs.h"
#include <vector>
#include "AppParams.h"

//---------------------------------------------------------------------------
class TMaintenanceCountersForm : public TForm
{
__published:	// IDE-managed Components
  TEdit *PrintingHoursEdit;
  TLabel *TotalTimeLabel;
        TLabel *ItemLabel;
        TLabel *DatedLabel;
        TLabel *PrintingHoursLabel;
  TEdit *PrintingDateEdit;
  TBitBtn *CloseBitBtn;
  TMainMenu *MainMenu1;
  TMenuItem *Units1;
  TMenuItem *DaysMenuItem;
  TMenuItem *HoursMenuItem;
  TMenuItem *MinutesMenuItem;
  TMenuItem *SecondsMenuItem;
        TEdit *PrintingWarnEdit;
        TLabel *WarningTimeLabel;
        TBevel *Bevel2;
        TActionList *ActionList1;
        TAction *EnterEditModeAction;
        TAction *SaveValuesAction;
        TPopupMenu *WarnFieldPopupMenu;
        TMenuItem *RestoreDefaultRestoreDefaultMenuItem;
        TLabel *UserWarningLabel;
        TAction *OpenUserWarningAction;
        TSaveDialog *MaintenanceSaveDialog;
        TMenuItem *File1;
        TMenuItem *SaveToFileMenuItem;
        TAction *ApplyWarningTimeAction;
        TScrollBox *CountersScrollBox;
	TBitBtn *btnSave;
  void __fastcall ResetButtonClick(TObject *Sender);
  void __fastcall FormShow(TObject *Sender);
  void __fastcall CloseBitBtnClick(TObject *Sender);
  void __fastcall DaysMenuItemClick(TObject *Sender);
  void __fastcall HoursMenuItemClick(TObject *Sender);
  void __fastcall MinutesMenuItemClick(TObject *Sender);
  void __fastcall SecondsMenuItemClick(TObject *Sender);
  void __fastcall FormCreate(TObject *Sender);
        void __fastcall RestoreDefaultRestoreDefaultMenuItemClick(TObject *Sender);
        void __fastcall EnterEditModeActionExecute(TObject *Sender);
        void __fastcall SaveValuesActionExecute(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall OpenUserWarningActionExecute(TObject *Sender);
        void __fastcall SaveToFileMenuItemClick(TObject *Sender);
        void __fastcall ApplyWarningTimeActionExecute(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
	void __fastcall btnSaveClick(TObject *Sender);
private:	// User declarations

  typedef enum{duDays, duHours, duMinutes, duSeconds}TDisplayUnits;

  TLabel*    NameLabels    [NUM_OF_MAINTENANCE_COUNTERS];
  TEdit*     TotalTimeEdits[NUM_OF_MAINTENANCE_COUNTERS];
  TEdit*     DateEdits     [NUM_OF_MAINTENANCE_COUNTERS];
  TEdit*     WarnTimeEdits [NUM_OF_MAINTENANCE_COUNTERS];
  TButton*   ResetButtons  [NUM_OF_MAINTENANCE_COUNTERS];
  TCheckBox* CheckBoxs     [NUM_OF_MAINTENANCE_COUNTERS];
  std::vector<TBevel*> BevelsVector;
  std::vector<TLabel*> GroupLabelsVector;

  TDisplayUnits m_DispalyUnits;
  bool m_EditMode;
  bool m_destroyed;

  bool m_MaintenanceMode;
  CAppParams *m_ParamManager;
  std::vector<TComponent> m_Components;

  // This array is used to save the counters values (in seconds) bofore they are changed in the "secret" edit mode
  ULONG PrevCountersValue[NUM_OF_MAINTENANCE_COUNTERS];

  void UpdateDisplay();
  void SetDisplayUnitsAtFormShow();

  void SetEditMode(bool EditMode);

  void __fastcall UserWarningActionExecute(bool ToShow);
  AnsiString SecondsToCurrentUnitsStr(long Secs);
  long CurrentUnitsStrToSeconds(AnsiString UnitsStr);
  long SecondsToCurrentUnits(long Secs);
  void HideUserWarningColumn();
  void CreateMaintenanceFile(QString MaintenanceFileName);
  void ShowHideReset(bool ShowReset);
  void SaveChangesToCounters();
  void CreateMaintenanceCountersForm();
  void DestroyMaintenanceCountersForm();
  bool IsCounterExistInDlg(int i);

  QString m_MaintenanceDirName;

public:		// User declarations
  __fastcall TMaintenanceCountersForm(TComponent* Owner);
  void SetMaintenanceMode(bool NewMode)
  {
    m_MaintenanceMode = NewMode;
  }
};
//---------------------------------------------------------------------------
extern PACKAGE TMaintenanceCountersForm *MaintenanceCountersForm;
//---------------------------------------------------------------------------
#endif
