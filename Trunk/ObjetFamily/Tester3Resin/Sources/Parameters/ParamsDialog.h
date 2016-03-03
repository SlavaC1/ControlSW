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
 * Last upate: 20/06/2000                                           *
 ********************************************************************/

#ifndef _PARAMETERS_DIALOG_H_
#define _PARAMETERS_DIALOG_H_

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
#include "ParamsContainer.h"


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
  CNameParamBase *m_ParamPtr;

  // Constructor
  TParamPaneItem(TControl *PaneControl,CNameParamBase *ParamPtr) {
    m_PaneControl = PaneControl;
    m_ParamPtr = ParamPtr;
  }
};

// Structure for extended info of an integer parameter pane
typedef struct {
  int Value,Min,Max;
  TEdit *EditFieldPtr;

} TIntegerPaneInfo;

// Param pane list type
typedef std::list<TParamPaneItem *> TParamPaneList;


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
private:	// User declarations

  TTabsList m_TabsList;
  TParamPaneList m_ParamPaneList;

  // Critical section is used for protecting parametsr during apply
  TCriticalSection *m_CriticalSection;

  // Dialog modes
  bool m_ShowLimitsMode;
  bool m_SaveWhenApply;
  TNotifyEvent m_OnApply;

  TParamPaneList::iterator m_LastSearchResult;
  AnsiString m_SearchString;

  // Build a "hint" string for a specific parameter
  AnsiString __fastcall BuildParamHintString(CNameParamBase *ParamPtr);

  // Build a pane for integer (short,long, etc.) parameter
  bool __fastcall AddIntegerPane(TWinControl *Parent,int PaneLeft,int PaneTop,int MaxParentHeight,
                                 CNameParamBase *ParamPtr,int& PaneWidth,int& PaneHeight,
                                 bool ReadOnly);

  // Build a pane for real (floating point) parameter
  bool __fastcall AddRealPane(TWinControl *Parent,int PaneLeft,int PaneTop,int MaxParentHeight,
                              CNameParamBase *ParamPtr,int& PaneWidth,int& PaneHeight,
                              bool ReadOnly);

  // Build a pane for boolean parameter
  bool __fastcall AddBoolPane(TWinControl *Parent,int PaneLeft,int PaneTop,int MaxParentHeight,
                              CNameParamBase *ParamPtr,int& PaneWidth,int& PaneHeight,
                              bool ReadOnly);

  // Build a pane for generic parameter
  bool __fastcall AddGenericPane(TWinControl *Parent,int PaneLeft,int PaneTop,int MaxParentHeight,
                                 CNameParamBase *ParamPtr,int& PaneWidth,int& PaneHeight,
                                 bool ReadOnly);

  // Verify the content of a real (floating point) field
  bool __fastcall CheckFloatField(CNameParamBase *ParamPtr,AnsiString FloatText);

  // Get an integer field value with limits and values checking
  AnsiString __fastcall GetIntegerFieldValue(TIntegerPaneInfo *IntegerPaneInfo);

  // Assign all the values from the GUI to the parameters
  void __fastcall AssignToParams(void);

  // Assign a value from a single parameter pane objet to the GUI element
  void __fastcall AssignFromParamPaneObjet(TParamPaneItem *ParamPtr,bool AssignDefaultValue,bool ChangeToNormalColor);

  // Assign values from the params to the parameters pane
  void __fastcall AssignFromParams(bool AssignDefaultValue);

  // Help function to generate a parameter title string
  AnsiString __fastcall BuildParamTitleString(CNameParamBase *ParamPtr);

  bool __fastcall AddParamControlPane(TWinControl *Parent,int PaneLeft,int PaneTop,int MaxParentHeight,
                                      CNameParamBase *ParamPtr,int& PaneWidth,int& PaneHeight,
                                      bool ReadOnly);

  void __fastcall UpDownChangingEvent(TObject *Sender,bool &AllowChange,
                                      short NewValue, TUpDownDirection Direction);

  void __fastcall PaneControlChangingEvent(TObject *Sender);

  // Implement search for a given parameter
  void DoParameterSearch(AnsiString SearchStr,TParamPaneList::iterator StartItem);

  // Help function for matching search string
  bool MatchSearchStrings(AnsiString Str,AnsiString SearchStr);


public:		// User declarations
  __fastcall TParamsForm(TComponent* Owner);

  void __fastcall SetDialogFeatures(TParamDialogButtonsMode ButtonsMode,TNotifyEvent OnApply);
  void __fastcall BuildDialogLayout(CParamsContainer *ParamsContainer,bool ShowLimits);
};

// Type for list of containers
typedef std::vector<CParamsContainer *> TParamContainerList;

class TParamsDialog : public TObject {
  private:
    TParamsForm *m_ParamsForm;

    // Internal variables
    TParamContainerList m_ContainesList;
    bool m_SaveWhenApply;
    AnsiString m_Caption;
    TNotifyEvent m_OnApply;

    // Current dialog buttons settings
    TParamDialogButtonsMode m_CurrentButtonsMode;

  public:
    // Constructor
    TParamsDialog(void);

    // Dialog caption
    __property AnsiString Caption = {read=m_Caption,write=m_Caption};

    // Buttons mode
    __property TParamDialogButtonsMode ButtonsMode = {read=m_CurrentButtonsMode,write=m_CurrentButtonsMode};

    // Notification for apply event
    __property TNotifyEvent OnApply = {read=m_OnApply,write=m_OnApply};

    // Add a container to the dialog layout
    void AddParamsContainer(CParamsContainer *ParamsContainer,bool ShowLimits = false);

    // Open the dialog
    bool Execute(void);
};

#endif
