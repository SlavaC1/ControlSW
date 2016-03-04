/********************************************************************
 *                       Parameters Dialog Box                      *
 *                       ---------------------                      *
 * Module description: This module implement a dialog box with      *
 *                     automatic layout building according to the   *
 *                     content of a parameters container.           *
 *                                                                  *
 * Compilation: Standard C++ / BCB.                                 *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 26/03/2000                                           *
 * Last upate: 22/08/2002                                           *
 ********************************************************************/

#ifndef ParamsDialogH
#define ParamsDialogH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ActnList.hpp>
#include <Buttons.hpp>
#include <ImgList.hpp>
#include <SyncObjs.hpp>
#include <Menus.hpp>
#include <list>
#include <vector>
#include <map>
#include "QTypes.h"
#include "QParameter.h"
#include "QStringList.h"

class CQParamsContainer;
class CQParameterBase;
class CQEnumParameter;

typedef void (*TParamsFormDestructionCallback)(bool, TGenericCockie);

// Type for a list of custom page viewers
typedef std::map<AnsiString,int> TParamsTabsList;

// The parameters dialog can be activated with Ok and Cancel buttons
// or with Apply and Close buttons.
typedef enum {pbOkCancel,pbApplyClose} TParamDialogButtonsMode;

// Type for an item in the tabs list
typedef struct {
  TTabSheet *TabSheet;
  int CurX,CurY,MaxWidth;

} TTabsListItem;

// Tabs list type used in the dialog layout building
typedef std::vector<TTabsListItem> TTabsList;

// Type for an item in the parameter pane list
struct TParamPaneItem {
  TControl *m_PaneControl;
  CQParameterBase *m_ParamPtr;
  TControl *m_ParamLabel;

  // Constructor
  TParamPaneItem(TControl *PaneControl,CQParameterBase *ParamPtr,TControl *Label)
  {
    m_PaneControl = PaneControl;
    m_ParamPtr = ParamPtr;
    m_ParamLabel = Label;
  }

  ~TParamPaneItem()
  {
    if(m_PaneControl)
    {
      delete(m_PaneControl);
      m_PaneControl = NULL;
    }

/*    if(m_ParamLabel)
    {
      delete(m_ParamLabel);
      m_ParamLabel = NULL;
    }
*/  }
};

// Structure for extended info of an integer parameter pane
typedef struct {
  int Value,Min,Max;
  bool LimitsMode;
  TEdit *EditFieldPtr;

} TIntegerPaneInfo;

// Param pane list type
typedef std::list<TParamPaneItem *> TParamPaneList;

// Types for alternative parameter access routines
typedef AnsiString __fastcall (__closure *TParamGetEvent)(AnsiString ParamName);
typedef void __fastcall (__closure *TParamSetEvent)(AnsiString ParamName,AnsiString NewValue);

class TParamsForm : public TForm
{
__published:	// IDE-managed Components
        TPageControl *PageControl1;
        TActionList *ActionList1;
        TAction *OkAction;
        TAction *CancelAction;
        TAction *ApplyAction;
        TAction *CloseAction;
        TBitBtn *Button1;
        TBitBtn *Button2;
        TImageList *ImageList1;
        TBitBtn *DefaultsButton;
        TPopupMenu *SpecificParamPopupMenu;
        TMenuItem *RestoretoDefaultMenuItem;
        TMenuItem *RevertChangesMenuItem;
        TAction *FindAction;
        TBitBtn *BitBtn1;
        TAction *SearchAgainAction;
        TAction *RefreshAction;
        TListBox *ParamTabsListBox;
        TAction *ChangeExposureLevelAction;
        TLabel *WidthTestLabel;
        TButton *LogParametersButton;
        void __fastcall OkActionExecute(TObject *Sender);
        void __fastcall CancelActionExecute(TObject *Sender);
        void __fastcall ApplyActionExecute(TObject *Sender);
        void __fastcall CloseActionExecute(TObject *Sender);
        void __fastcall DefaultsButtonClick(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall RestoretoDefaultMenuItemClick(TObject *Sender);
        void __fastcall SpecificParamPopupMenuPopup(TObject *Sender);
        void __fastcall RevertChangesMenuItemClick(TObject *Sender);
        void __fastcall FindActionExecute(TObject *Sender);
        void __fastcall SearchAgainActionExecute(TObject *Sender);
        void __fastcall RefreshActionExecute(TObject *Sender);
        void __fastcall ParamTabsListBoxClick(TObject *Sender);
        void __fastcall ChangeExposureLevelActionExecute(TObject *Sender);
        void __fastcall LogParametersButtonClick(TObject *Sender);
private:	// User declarations

  TTabsList m_TabsList;
  TParamPaneList m_ParamPaneList;
  TParamsTabsList m_ParamsTabsList;

  // Critical section is used for protecting parametsr during apply
  TCriticalSection *m_CriticalSection;

  // Dialog modes
  bool m_ShowLimitsMode;
  bool m_SaveWhenApply;
  TNotifyEvent m_OnApply;

  bool m_checksumValid; //TRIPLEX_MACHINE

  TParamPaneList::iterator m_LastSearchResult;
  AnsiString m_SearchString;

  static bool m_RestartRequired;

  // Alternative access routines for a parameter
  TParamGetEvent m_ParamGetEvent;
  TParamSetEvent m_ParamSetEvent;

  // Build a "hint" string for a specific parameter
  AnsiString BuildParamHintString(CQParameterBase *ParamPtr);

  // Build a pane for integer (short,long, etc.) parameter
  bool AddIntegerPane(TWinControl *Parent,int PaneLeft,int PaneTop,int MaxParentHeight,
                                 CQParameterBase *ParamPtr,int& PaneWidth,int& PaneHeight);

  // Build a pane for real (floating point) parameter
  bool AddRealPane(TWinControl *Parent,int PaneLeft,int PaneTop,int MaxParentHeight,
                              CQParameterBase *ParamPtr,int& PaneWidth,int& PaneHeight);

  // Build a pane for boolean parameter
  bool AddBoolPane(TWinControl *Parent,int PaneLeft,int PaneTop,int MaxParentHeight,
                              CQParameterBase *ParamPtr,int& PaneWidth,int& PaneHeight);

  // Build a pane for generic parameter
  bool AddGenericPane(TWinControl *Parent,int PaneLeft,int PaneTop,int MaxParentHeight,
                                 CQParameterBase *ParamPtr,int& PaneWidth,int& PaneHeight);

  // Build a pane for enumerated parameter
  bool AddEnumPane(TWinControl *Parent,int PaneLeft,int PaneTop,int MaxParentHeight,
                              CQEnumParameter *ParamPtr,int& PaneWidth,int& PaneHeight);

  // Build a pane for an array parameter
  bool AddArrayPane(TWinControl *Parent,int PaneLeft,int PaneTop,int MaxParentHeight,
                    CQParameterBase *ParamPtr,int& PaneWidth,int& PaneHeight);

  // Verify the content of a real (floating point) field
  bool CheckFloatField(CQParameterBase *ParamPtr,AnsiString FloatText);

  // Get an integer field value with limits and values checking
  AnsiString GetIntegerFieldValue(TIntegerPaneInfo *IntegerPaneInfo);

  // Assign all the values from the GUI to the parameters
  void AssignToParams(void);

  void ActivateDesiredTab();

  // Assign a value from a single parameter pane objet to the GUI element
  void AssignFromParamPaneObjet(TParamPaneItem *ParamPtr,bool AssignDefaultValue,bool ChangeToNormalColor);

  // Assign values from the params to the parameters pane
  void AssignFromParams(bool AssignDefaultValue);

  static void RestartRequiredParamObserver(CQParameterBase */*Param*/,TGenericCockie /*Cockie*/);

  // Help function to generate a parameter title string
  AnsiString BuildParamTitleString(CQParameterBase *ParamPtr);

  bool AddParamControlPane(TWinControl *Parent,int PaneLeft,int PaneTop,int MaxParentHeight,
                           CQParameterBase *ParamPtr,int& PaneWidth,int& PaneHeight);

  void __fastcall UpDownChangingEvent(TObject *Sender,bool &AllowChange,
                                      short NewValue, TUpDownDirection Direction);

  void __fastcall PaneControlChangingEvent(TObject *Sender);

  void __fastcall ArrayParameterOpenEventHandler(TObject *Sender);

  // Implement search for a given parameter
  void DoParameterSearch(AnsiString SearchStr,TParamPaneList::iterator StartItem);

  // Help function for matching search string
  bool MatchSearchStrings(AnsiString Str,AnsiString SearchStr);

  void CheckRestartRequired(void);

  // Access functions to a get/set a parameter value
  void SetParamValue(CQParameterBase *ParamPtr,AnsiString Str);
  AnsiString GetParamValue(CQParameterBase *ParamPtr);

  // Change the color of the parameter label according to its attributes
  void SetParameterLabelColor(CQParameterBase *ParamPtr,TControl *LabelControl);
  
  TParamsFormDestructionCallback m_ParamsFormDestructionCallback;
  TGenericCockie                 m_Cockie;

public:		// User declarations
  __fastcall TParamsForm(TComponent* Owner);
  __fastcall TParamsForm(TParamsForm* Other);

  void SetDialogFeatures(TParamDialogButtonsMode ButtonsMode,TNotifyEvent OnApply);
  void BuildDialogLayout(CQParamsContainer *ParamsContainer,int ExposureLevel, bool ShowLimits, CQStringList* HideTabs, bool passwordValid = false);

  // Return true if a program restart is required
  bool IsRestartRequired(void) {
    return m_RestartRequired;
  }

  bool IsParametersChecksumValid() { return m_checksumValid; }

  // Set an alternative set/get functions for a parameter
  void SetAlternativeParameterAccess(TParamGetEvent GetRoutine,TParamSetEvent SetRoutine);
  
  void SetParamsFormDestructionCallback(TParamsFormDestructionCallback ParamsFormDestructionCallback, TGenericCockie Cockie);
};

// Type for list of containers
typedef std::vector<CQParamsContainer *> TParamContainerList;

class TParamsDialog : public TObject {
  private:
    TParamsForm *m_ParamsForm;

    // Internal variables
    TParamContainerList m_ContainesList;
    bool m_SaveWhenApply;
    AnsiString m_Caption;
    TNotifyEvent m_OnApply;
	bool m_ApplyEnabled;

    void __fastcall SetOnApplyEnabled(bool NewMode);

    // Current dialog buttons settings
    TParamDialogButtonsMode m_CurrentButtonsMode;
	bool m_ParamsFormDestroyed;

  public:
    // Constructor
    TParamsDialog(void);
    TParamsDialog(TParamsDialog& Other);
    __fastcall ~TParamsDialog();

    // Dialog caption
    __property AnsiString Caption = {read=m_Caption,write=m_Caption};

    // Buttons mode
    __property TParamDialogButtonsMode ButtonsMode = {read=m_CurrentButtonsMode,write=m_CurrentButtonsMode};

    // Notification for apply event
    __property TNotifyEvent OnApply = {read=m_OnApply,write=m_OnApply};

    __property bool ApplyEnabled = {read=m_ApplyEnabled,write=SetOnApplyEnabled};

    // Add a container to the dialog layout
    void AddParamsContainer(CQParamsContainer *ParamsContainer,int ExposureLevel = RD_LEVEL, bool ShowLimits = false, CQStringList* HideTabs = NULL, bool password = false);
    void RefreshDisplay();
    
    // Open the dialog
	bool Execute(void);

	bool IsRestartRequired(void)
	{
      return m_ParamsForm->IsRestartRequired();
    }

    // Set an alternative set/get functions for a parameter
    void SetAlternativeParameterAccess(TParamGetEvent GetRoutine,TParamSetEvent SetRoutine);
    
	static void ParamsFormDestructionCallback(bool ParamsFormDestroyed, TGenericCockie Cockie);
};

#endif
