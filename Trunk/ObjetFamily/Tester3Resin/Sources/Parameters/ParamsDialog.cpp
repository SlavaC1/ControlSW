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
 * Last upate: 03/06/2000                                           *
 ********************************************************************/

#include <vcl.h>
#pragma hdrstop

#include <typeinfo.h>
#include "ParamsDialog.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

const INT_EDIT_WIDTH = 72;
const REAL_EDIT_WIDTH = 87;
const GENERIC_EDIT_WIDTH = 87;
const PARAM_TITLE_TO_FIELD_SPACE = 3;
const CORNER_SPACING = 10;
const PANE_SPACING = 8;
const PARAMS_RAW_SPACING = 110;
const PARAMS_RAW_THRSHLD = 10;
const PANE_WIDTH_SPACING = 10;
const UP_DOWN_WIDTH = 16;
const UP_DOWN_HEIGHT = 21;
const MIN_CHECK_BOX_WIDTH = 20;


#define CHANGE_HIGHLIGHT_COLOR clRed
#define NORMAL_TEXT_COLOR      clBlack

TParamsForm *ParamsForm;
//---------------------------------------------------------------------------
__fastcall TParamsForm::TParamsForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------


// Help function to generate a parameter title string
AnsiString __fastcall TParamsForm::BuildParamTitleString(CNameParamBase *ParamPtr)
{
  AnsiString s(ParamPtr->Name());

  if((ParamPtr->GetLimitsMode()) && m_ShowLimitsMode)
    s = s + " (" + ParamPtr->MinLimitToString() + " - " + ParamPtr->MaxLimitToString() + ")";

  return s;
}

// Build a pane for integer (short,long, etc.) parameter
bool __fastcall TParamsForm::AddIntegerPane(TWinControl *Parent,int PaneLeft,int PaneTop,int MaxParentHeight,
                                            CNameParamBase *ParamPtr,int& PaneWidth,int& PaneHeight,
                                            bool ReadOnly)
{
  TLabel *TmpLabel;
  TEdit *TmpEdit;
  TUpDown *TmpUpDown;
  TIntegerPaneInfo *IntegerPaneInfo;

  // Build the label
  TmpLabel = new TLabel(this);
  TmpLabel->Caption = BuildParamTitleString(ParamPtr);
  TmpLabel->Left = PaneLeft;
  TmpLabel->Top = PaneTop;
  TmpLabel->Parent = Parent;
  TmpLabel->PopupMenu = SpecificParamPopupMenu;
  TmpLabel->Hint = BuildParamHintString(ParamPtr);
  TmpLabel->ShowHint = true;

  // Build the edit and the up/down control
  TmpEdit = new TEdit(this);
  TmpEdit->Width = INT_EDIT_WIDTH;
  TmpEdit->Left = PaneLeft;
  TmpEdit->Top = PaneTop + TmpLabel->Height + PARAM_TITLE_TO_FIELD_SPACE;
  TmpEdit->Parent = Parent;
  TmpEdit->Enabled = ReadOnly;

  TmpUpDown = new TUpDown(this);
  TmpUpDown->Parent = Parent;
  TmpUpDown->Width = UP_DOWN_WIDTH;
  TmpUpDown->Height = UP_DOWN_HEIGHT;
  TmpUpDown->Left = TmpEdit->Left + TmpEdit->Width;
  TmpUpDown->Top = TmpEdit->Top;
  TmpUpDown->Wrap = true;
  TmpUpDown->Enabled = ReadOnly;
  TmpUpDown->Min = std::numeric_limits<short>::min();
  TmpUpDown->Max = std::numeric_limits<short>::max();

  // Set limits and initial values
  TmpEdit->Text = ParamPtr->ValueToString();

  // Allocate an integer pane info structure
  IntegerPaneInfo = new TIntegerPaneInfo;
  IntegerPaneInfo->Min = StrToInt(ParamPtr->MinLimitToString());
  IntegerPaneInfo->Max = StrToInt(ParamPtr->MaxLimitToString());
  IntegerPaneInfo->Value = StrToInt(TmpEdit->Text);
  IntegerPaneInfo->EditFieldPtr = TmpEdit;

  TmpUpDown->Tag = reinterpret_cast<int>(IntegerPaneInfo);

  PaneWidth = TmpEdit->Width + TmpUpDown->Width;

  // The total width is set according to the label width
  PaneWidth = max(PaneWidth,TmpLabel->Width);

  PaneHeight = TmpLabel->Height + PARAM_TITLE_TO_FIELD_SPACE + TmpEdit->Height;

  // Check if the param pane overflow the parent bottom
  if(PaneTop + PaneHeight + PARAMS_RAW_THRSHLD > MaxParentHeight) {
    // Delete the temporary pane components
    TmpLabel->Parent = NULL;
		Q_SAFE_DELETE(TmpLabel);

    TmpEdit->Parent = NULL;
		Q_SAFE_DELETE(TmpEdit);

    TmpUpDown->Parent = NULL;
		Q_SAFE_DELETE(TmpUpDown);

		Q_SAFE_DELETE(IntegerPaneInfo);

    return false;
  }

  // Add an entry in the parameter pane list
  TParamPaneItem *ParamPaneItem = new TParamPaneItem(TmpUpDown,ParamPtr);
  m_ParamPaneList.push_back(ParamPaneItem);

  // Attach the pane pointer to the tag property
  TmpEdit->Tag = reinterpret_cast<int>(ParamPaneItem);
  TmpLabel->Tag = reinterpret_cast<int>(ParamPaneItem);

  TmpUpDown->OnChangingEx = UpDownChangingEvent;
  TmpEdit->OnChange = PaneControlChangingEvent;

  return true;
}

// Build a pane for real (floating point) parameter
bool __fastcall TParamsForm::AddRealPane(TWinControl *Parent,int PaneLeft,int PaneTop,int MaxParentHeight,
                                         CNameParamBase *ParamPtr,int& PaneWidth,int& PaneHeight,
                                         bool ReadOnly)
{
  TLabel *TmpLabel;
  TEdit *TmpEdit;

  // Build the label
  TmpLabel = new TLabel(this);
  TmpLabel->Caption = BuildParamTitleString(ParamPtr);
  TmpLabel->Left = PaneLeft;
  TmpLabel->Top = PaneTop;
  TmpLabel->Parent = Parent;
  TmpLabel->PopupMenu = SpecificParamPopupMenu;
  TmpLabel->Hint = BuildParamHintString(ParamPtr);
  TmpLabel->ShowHint = true;

  // Build the edit field
  TmpEdit = new TEdit(this);
  TmpEdit->Width = REAL_EDIT_WIDTH;
  TmpEdit->Left = PaneLeft;
  TmpEdit->Top = PaneTop + TmpLabel->Height + PARAM_TITLE_TO_FIELD_SPACE;
  TmpEdit->Text = ParamPtr->ValueToString();
  TmpEdit->Parent = Parent;
  TmpEdit->Enabled = ReadOnly;

  PaneWidth = TmpEdit->Width;

  // The total width is set according to the label width
  PaneWidth = max(PaneWidth,TmpLabel->Width);

  PaneHeight = TmpLabel->Height + PARAM_TITLE_TO_FIELD_SPACE + TmpEdit->Height;

  TmpEdit->OnChange = PaneControlChangingEvent;

  // Check if the param pane overflow the parent bottom
  if(PaneTop + PaneHeight + PARAMS_RAW_THRSHLD > MaxParentHeight) {
    // Delete the temporary pane components
    TmpLabel->Parent = NULL;
		Q_SAFE_DELETE(TmpLabel);

    TmpEdit->Parent = NULL;
		Q_SAFE_DELETE(TmpEdit);

    return false;
  }

  // Add an entry in the parameter pane list
  TParamPaneItem *ParamPaneItem = new TParamPaneItem(TmpEdit,ParamPtr);
  m_ParamPaneList.push_back(ParamPaneItem);

  TmpEdit->Tag = reinterpret_cast<int>(ParamPaneItem);
  TmpLabel->Tag = reinterpret_cast<int>(ParamPaneItem);

  return true;
}

// Build a pane for boolean parameter
bool __fastcall TParamsForm::AddBoolPane(TWinControl *Parent,int PaneLeft,int PaneTop,int MaxParentHeight,
                                         CNameParamBase *ParamPtr,int& PaneWidth,int& PaneHeight,
                                         bool ReadOnly)
{
  TLabel *TmpLabel;
  TEdit *TmpEdit;
  TCheckBox *TmpCheckBox;

  // Build the label
  TmpLabel = new TLabel(this);
  TmpLabel->Caption = "";
  TmpLabel->Left = PaneLeft;
  TmpLabel->Top = PaneTop;
  TmpLabel->Parent = Parent;

  // Build a checkbox
  TmpCheckBox = new TCheckBox(this);

  TmpCheckBox->Caption = BuildParamTitleString(ParamPtr);
  TmpCheckBox->Left = PaneLeft;
  TmpCheckBox->Top = PaneTop + TmpLabel->Height;
  TmpCheckBox->Parent = Parent;
  TmpCheckBox->Checked = (ParamPtr->ValueToString() == "1");
  TmpCheckBox->Enabled = ReadOnly;
  TmpCheckBox->PopupMenu = SpecificParamPopupMenu;
  TmpCheckBox->Hint = BuildParamHintString(ParamPtr);
  TmpCheckBox->ShowHint = true;

  // Use a dirty trick to find the size of a checkbox according to it's caption length:
  // we use the form canvas in order to use the function TextWidth(). We must assume that
  // the form and the checkbox has the same font.
  TmpCheckBox->Width = MIN_CHECK_BOX_WIDTH + Canvas->TextWidth(TmpCheckBox->Caption);

  PaneWidth = TmpCheckBox->Width;

  // The total width is set according to the label width
  PaneWidth = max(PaneWidth,TmpLabel->Width);

  PaneHeight = TmpLabel->Height + PARAM_TITLE_TO_FIELD_SPACE + TmpCheckBox->Height;

  TmpCheckBox->OnClick = PaneControlChangingEvent;

  // Check if the param pane overflow the parent bottom
  if(PaneTop + PaneHeight + PARAMS_RAW_THRSHLD > MaxParentHeight) {
    // Delete the temporary pane components
    TmpLabel->Parent = NULL;
		Q_SAFE_DELETE(TmpLabel);

    TmpCheckBox->Parent = NULL;
		Q_SAFE_DELETE(TmpCheckBox);

    return false;
  }

  // Add an entry in the parameter pane list
  TParamPaneItem *ParamPaneItem = new TParamPaneItem(TmpCheckBox,ParamPtr);
  m_ParamPaneList.push_back(ParamPaneItem);

  TmpCheckBox->Tag = reinterpret_cast<int>(ParamPaneItem);

  return true;
}

// Build a pane for generic parameter
bool __fastcall TParamsForm::AddGenericPane(TWinControl *Parent,int PaneLeft,int PaneTop,int MaxParentHeight,
                                            CNameParamBase *ParamPtr,int& PaneWidth,int& PaneHeight,
                                            bool ReadOnly)
{
  TLabel *TmpLabel;
  TEdit *TmpEdit;

  // Build the label
  TmpLabel = new TLabel(this);
  TmpLabel->Caption = BuildParamTitleString(ParamPtr);
  TmpLabel->Left = PaneLeft;
  TmpLabel->Top = PaneTop;
  TmpLabel->Parent = Parent;
  TmpLabel->PopupMenu = SpecificParamPopupMenu;
  TmpLabel->Hint = BuildParamHintString(ParamPtr);
  TmpLabel->ShowHint = true;

  // Build the edit field
  TmpEdit = new TEdit(this);
  TmpEdit->Width = GENERIC_EDIT_WIDTH;
  TmpEdit->Left = PaneLeft;
  TmpEdit->Top = PaneTop + TmpLabel->Height + PARAM_TITLE_TO_FIELD_SPACE;
  TmpEdit->Text = ParamPtr->ValueToString();
  TmpEdit->Parent = Parent;
  TmpEdit->Enabled = ReadOnly;

  PaneWidth = TmpEdit->Width;

  // The total width is set according to the label width
  PaneWidth = max(PaneWidth,TmpLabel->Width);

  PaneHeight = TmpLabel->Height + PARAM_TITLE_TO_FIELD_SPACE + TmpEdit->Height;

  TmpEdit->OnChange = PaneControlChangingEvent;

  // Check if the param pane overflow the parent bottom
  if(PaneTop + PaneHeight + PARAMS_RAW_THRSHLD > MaxParentHeight) {
    // Delete the temporary pane components
    TmpLabel->Parent = NULL;
		Q_SAFE_DELETE(TmpLabel);

    TmpEdit->Parent = NULL;
		Q_SAFE_DELETE(TmpEdit);

    return false;
  }

  // Add an entry in the parameter pane list
  TParamPaneItem *ParamPaneItem = new TParamPaneItem(TmpEdit,ParamPtr);
  m_ParamPaneList.push_back(ParamPaneItem);

  TmpEdit->Tag = reinterpret_cast<int>(ParamPaneItem);
  TmpLabel->Tag = reinterpret_cast<int>(ParamPaneItem);

  return true;
}

// Add a new parameter "pane" to the parent control
bool __fastcall TParamsForm::AddParamControlPane(TWinControl *Parent,int PaneLeft,int PaneTop,int MaxParentHeight,
                                                 CNameParamBase *ParamPtr,int& PaneWidth,int& PaneHeight,
                                                 bool ReadOnly)
{
  bool Ret = false;

  switch(ParamPtr->GetTypeID()) {
    case ptInt:
      Ret = AddIntegerPane(Parent,PaneLeft,PaneTop,MaxParentHeight,ParamPtr,PaneWidth,PaneHeight,ReadOnly);
      break;

    case ptReal:
      Ret = AddRealPane(Parent,PaneLeft,PaneTop,MaxParentHeight,ParamPtr,PaneWidth,PaneHeight,ReadOnly);
      break;

    case ptBool:
      Ret = AddBoolPane(Parent,PaneLeft,PaneTop,MaxParentHeight,ParamPtr,PaneWidth,PaneHeight,ReadOnly);
      break;

    case ptGeneric:
      Ret = AddGenericPane(Parent,PaneLeft,PaneTop,MaxParentHeight,ParamPtr,PaneWidth,PaneHeight,ReadOnly);
      break;
  }

  return Ret;
}

// Build a parameter dialog layout according to the container content.
// Note: This function can be called several times with different containers.
void __fastcall TParamsForm::BuildDialogLayout(CParamsContainer *ParamsContainer,bool ShowLimits)
{
  int PaneWidth,PaneHeight;
  TTabSheet *CurrentTabSheet;

  m_ShowLimitsMode = ShowLimits;

  TNameParamList PList = ParamsContainer->GetParamList();

  for(TNameParamList::iterator i = PList.begin(); i != PList.end();) {

    // If the parameter is hidden, continue to next parameter
    if((*i)->GetAttributes() & paHidden) {
      ++i;
      continue;
    }

    TTabsList::iterator CurrentTabItem = NULL;
    TScrollBox *ScrollBox;

    // Search for the parameters section in the tabs list
    for(TTabsList::iterator TabsIterator = m_TabsList.begin(); TabsIterator != m_TabsList.end(); ++TabsIterator)
      if((*TabsIterator).TabSheet->Caption == (*i)->SectionName()) {
        CurrentTabItem = TabsIterator;
        ScrollBox = reinterpret_cast<TScrollBox *>((*TabsIterator).TabSheet->Tag);
        break;
      }

    // If no tab could be found, create a new one
    if(CurrentTabItem == NULL) {
      TTabSheet *CurrentTabSheet;

      CurrentTabSheet = new TTabSheet(this);
      CurrentTabSheet->PageControl = PageControl1;
      CurrentTabSheet->Caption = (*i)->SectionName();

      TTabsListItem TabListItem = {CurrentTabSheet,CORNER_SPACING,CORNER_SPACING};

      m_TabsList.push_back(TabListItem);
      CurrentTabItem = &m_TabsList[m_TabsList.size() - 1];
      CurrentTabItem->MaxWidth = 0;

      // Create a scroll box on the tab
      ScrollBox = new TScrollBox(this);
      ScrollBox->Parent = CurrentTabSheet;
      ScrollBox->BorderStyle = bsNone;
      ScrollBox->Align = alClient;
      ScrollBox->VertScrollBar->Visible = false;
      ScrollBox->HorzScrollBar->Tracking = true;

      CurrentTabSheet->Tag = reinterpret_cast<int>(ScrollBox);
    }

    // Check if the parameter is a read-only parameter
    bool ReadOnlyFlag = !((*i)->GetAttributes() & paRDOnly);

    try {
      if(AddParamControlPane(ScrollBox,CurrentTabItem->CurX,CurrentTabItem->CurY,
                             CurrentTabItem->TabSheet->Height,*i,PaneWidth,PaneHeight,
                             ReadOnlyFlag)) {

        CurrentTabItem->CurY += PaneHeight + PANE_SPACING;

        if(PaneWidth > CurrentTabItem->MaxWidth)
          CurrentTabItem->MaxWidth = PaneWidth;

        ++i;
      } else {
          // The last parameter pane overflow the bottom of the window, so we need to go to the
          // next row.
          CurrentTabItem->CurY = CORNER_SPACING;
          CurrentTabItem->CurX += max(PARAMS_RAW_SPACING,CurrentTabItem->MaxWidth + PANE_WIDTH_SPACING);

          // Reset the maximum pane width for the current row
          CurrentTabItem->MaxWidth = 0;
        }
    } catch(...) {
        // In case of an error in AddParamControlPane() we skip the parameter and continue to the next one
        ++i;
      }
  }
}

void __fastcall TParamsForm::OkActionExecute(TObject *Sender)
{
  // Apply and close
  AssignToParams();
  ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TParamsForm::CancelActionExecute(TObject *Sender)
{
  // Cancel
  ModalResult = mrCancel;
}
//---------------------------------------------------------------------------

void __fastcall TParamsForm::ApplyActionExecute(TObject *Sender)
{
  // Assign and get
  AssignToParams();
  AssignFromParams(false);
}
//---------------------------------------------------------------------------

void __fastcall TParamsForm::CloseActionExecute(TObject *Sender)
{
  // Close
  ModalResult = mrNone;
  Close();
}

void __fastcall TParamsForm::SetDialogFeatures(TParamDialogButtonsMode ButtonsMode,TNotifyEvent OnApply)
{
  // Set the dialog button actions according to the current dialog mode
  switch(ButtonsMode) {
    case pbOkCancel:
      Button1->Action = OkAction;
      Button2->Action = CancelAction;
      break;

    case pbApplyClose:
      Button1->Action = ApplyAction;
      Button2->Action = CloseAction;
      break;
  }

  m_OnApply = OnApply;
}

// Check a specific float field for valid values and range
bool __fastcall TParamsForm::CheckFloatField(CNameParamBase *ParamPtr,AnsiString FloatText)
{
  bool r = true;
  long double v,MinV,MaxV;

  try {
    // Convert to float number
    v = StrToFloat(FloatText);

    // Convert the min and max values to float
    MinV = StrToFloat(ParamPtr->MinLimitToString());
    MaxV = StrToFloat(ParamPtr->MaxLimitToString());

    // Check range
    if((v < MinV) || (v > MaxV))
      r = false;

  } catch(...) {
      r = false;
    }

  return r;
}

// Assign values from the GUI to the parameters
void __fastcall TParamsForm::AssignToParams(void)
{
  // Block all other threads during the assignment
  m_CriticalSection->Enter();

  try {

    // Process all the elements in the pane list
    for(TParamPaneList::iterator i = m_ParamPaneList.begin(); i != m_ParamPaneList.end(); ++i) {
      // If the parameter is read only, do nothing
      if((*i)->m_ParamPtr->GetAttributes() & paRDOnly)
        continue;

      // Process each param according to it's type
      switch((*i)->m_ParamPtr->GetTypeID()) {
        case ptInt:
        {
          TUpDown *UpDown = dynamic_cast<TUpDown *>((*i)->m_PaneControl);
          TIntegerPaneInfo *IntegerPaneInfo = reinterpret_cast<TIntegerPaneInfo *>(UpDown->Tag);

          (*i)->m_ParamPtr->StringToValue(GetIntegerFieldValue(IntegerPaneInfo));
        }
        break;

        case ptReal:
        {
          TEdit *TmpEdit = dynamic_cast<TEdit *>((*i)->m_PaneControl);

          // Check value
          if(CheckFloatField((*i)->m_ParamPtr,TmpEdit->Text))
            (*i)->m_ParamPtr->StringToValue(TmpEdit->Text);
        }
        break;

        case ptBool:
        {
          TCheckBox *TmpCheckBox = dynamic_cast<TCheckBox *>((*i)->m_PaneControl);
          (*i)->m_ParamPtr->StringToValue(TmpCheckBox->Checked ? "1" : "0");
        }
        break;

        case ptGeneric:
        {
          TEdit *TmpEdit = dynamic_cast<TEdit *>((*i)->m_PaneControl);
          (*i)->m_ParamPtr->StringToValue(TmpEdit->Text);
        }
        break;
      }
    }
  } __finally {
    // Enable other threads
    m_CriticalSection->Leave();
  }

  // Call the apply event handler
  if(m_OnApply)
    m_OnApply(this);
}

// Assign a value from a single parameter pane objet to the GUI element
void __fastcall TParamsForm::AssignFromParamPaneObjet(TParamPaneItem *ParamPane,bool AssignDefaultValue,bool ChangeToNormalColor)
{
  // Avoid setting the parameter to defaults if the paBlockDefaults option is included
  if(AssignDefaultValue && (ParamPane->m_ParamPtr->GetAttributes() & paBlockDefaults))
    return;

  // Process each param according to it's type
  switch(ParamPane->m_ParamPtr->GetTypeID()) {
    case ptInt:
    {
      TUpDown *UpDown = dynamic_cast<TUpDown *>(ParamPane->m_PaneControl);
      TIntegerPaneInfo *IntegerPaneInfo = reinterpret_cast<TIntegerPaneInfo *>(UpDown->Tag);

      if(AssignDefaultValue)
        IntegerPaneInfo->Value = StrToInt(ParamPane->m_ParamPtr->DefaultValueToString());
      else
        IntegerPaneInfo->Value = StrToInt(ParamPane->m_ParamPtr->ValueToString());

      if(ChangeToNormalColor)
        // Set font color to normal
        IntegerPaneInfo->EditFieldPtr->Font->Color = NORMAL_TEXT_COLOR;

      // Refresh the edit text
      IntegerPaneInfo->EditFieldPtr->Text = IntToStr(IntegerPaneInfo->Value);
    }
    break;

    case ptReal:
    {
      TEdit *TmpEdit = dynamic_cast<TEdit *>(ParamPane->m_PaneControl);

      if(AssignDefaultValue)
        TmpEdit->Text = ParamPane->m_ParamPtr->DefaultValueToString();
      else
        TmpEdit->Text = ParamPane->m_ParamPtr->ValueToString();

      if(ChangeToNormalColor)
        // Set font color to normal
        TmpEdit->Font->Color = NORMAL_TEXT_COLOR;
    }
    break;

    case ptBool:
    {
      TCheckBox *TmpCheckBox = dynamic_cast<TCheckBox *>(ParamPane->m_PaneControl);

      if(AssignDefaultValue)
        TmpCheckBox->Checked = (ParamPane->m_ParamPtr->DefaultValueToString() == "1");
      else
        TmpCheckBox->Checked = (ParamPane->m_ParamPtr->ValueToString() == "1");

      if(ChangeToNormalColor)
        // Set font color to normal
        TmpCheckBox->Font->Color = NORMAL_TEXT_COLOR;
    }
    break;

    case ptGeneric:
    {
      TEdit *TmpEdit = dynamic_cast<TEdit *>(ParamPane->m_PaneControl);
//      ParamPane->m_ParamPtr->StringToValue(TmpEdit->Text);

      if(AssignDefaultValue)
        TmpEdit->Text = ParamPane->m_ParamPtr->DefaultValueToString();
      else
        TmpEdit->Text = ParamPane->m_ParamPtr->ValueToString();

      if(ChangeToNormalColor)
        // Set font color to normal
        TmpEdit->Font->Color = NORMAL_TEXT_COLOR;
    }
    break;
  }
}

// Assign values from the params to the parameters pane
void __fastcall TParamsForm::AssignFromParams(bool AssignDefaultValue)
{
  // Process all the elements in the pane list
  for(TParamPaneList::iterator i = m_ParamPaneList.begin(); i != m_ParamPaneList.end(); ++i)
    AssignFromParamPaneObjet((*i),AssignDefaultValue,true);
}

// Restore defaults
void __fastcall TParamsForm::DefaultsButtonClick(TObject *Sender)
{
  if(MessageDlg("This operation will set all machine parameters to default values.\n\r"   \
                "Press Ok to continue or Cancel to abort.",mtWarning,TMsgDlgButtons() << mbOK << mbCancel,0) == mrOk)
    AssignFromParams(true);
}

void __fastcall TParamsForm::UpDownChangingEvent(TObject *Sender,bool &AllowChange,
                                                 short NewValue,TUpDownDirection Direction)
{
  TUpDown *UpDown = dynamic_cast<TUpDown *>(Sender);
  TIntegerPaneInfo *IntegerPaneInfo = reinterpret_cast<TIntegerPaneInfo *>(UpDown->Tag);

  // Get current value
  int v;
  try {
    v = StrToInt(IntegerPaneInfo->EditFieldPtr->Text);
  } catch(...) {
      v = IntegerPaneInfo->Value;
    }

  // Calculate new value
  switch(Direction) {
    case updUp:
      v++;
      break;

    case updDown:
      v--;
      break;
  }

  // Check limits
  if((v < IntegerPaneInfo->Min) || (v > IntegerPaneInfo->Max))
    v = IntegerPaneInfo->Value;
  else
    // Save current value
    IntegerPaneInfo->Value = v;

  IntegerPaneInfo->EditFieldPtr->Text = IntToStr(v);
}

void __fastcall TParamsForm::PaneControlChangingEvent(TObject *Sender)
{
  AnsiString ValueStr;
  TComponent *SenderComponent = dynamic_cast<TComponent *>(Sender);
  TParamPaneItem *ParamPane = reinterpret_cast<TParamPaneItem *>(SenderComponent->Tag);

  switch(ParamPane->m_ParamPtr->GetTypeID()) {
    case ptInt:
    case ptReal:
    case ptGeneric:
    {
      TEdit *TmpEdit = dynamic_cast<TEdit *>(Sender);

      // Check if current value is different from the parameter value
      if(ParamPane->m_ParamPtr->ValueToString() != TmpEdit->Text)
        TmpEdit->Font->Color = CHANGE_HIGHLIGHT_COLOR;
      else
        TmpEdit->Font->Color = NORMAL_TEXT_COLOR;
    }
    break;

    case ptBool:
    {
      TCheckBox *TmpCheckBox = dynamic_cast<TCheckBox *>(Sender);
      AnsiString s = TmpCheckBox->Checked ? "1" : "0";

      // Check if current value is different from the parameter value
      if(ParamPane->m_ParamPtr->ValueToString() != s)
        TmpCheckBox->Font->Color = CHANGE_HIGHLIGHT_COLOR;
      else
        TmpCheckBox->Font->Color = NORMAL_TEXT_COLOR;
    }
    break;
  }
}

void __fastcall TParamsForm::FormCreate(TObject *Sender)
{
  m_CriticalSection = new TCriticalSection;
}

void __fastcall TParamsForm::FormDestroy(TObject *Sender)
{
	Q_SAFE_DELETE(m_CriticalSection);

  // Delete the integer panes info structures
  for(TParamPaneList::iterator i = m_ParamPaneList.begin(); i != m_ParamPaneList.end(); ++i) {
    if((*i)->m_ParamPtr->GetTypeID() == ptInt) {
      TUpDown *UpDown = dynamic_cast<TUpDown *>((*i)->m_PaneControl);
      TIntegerPaneInfo *IntegerPaneInfo = reinterpret_cast<TIntegerPaneInfo *>(UpDown->Tag);

      // Delete allocated structure
			Q_SAFE_DELETE(IntegerPaneInfo);
    }

    Q_SAFE_DELETE( (*i) );
  }
}

// Get an integer field value with limits and values checking
AnsiString __fastcall TParamsForm::GetIntegerFieldValue(TIntegerPaneInfo *IntegerPaneInfo)
{
  // Get current value
  int v;
  try {
    v = StrToInt(IntegerPaneInfo->EditFieldPtr->Text);
  } catch(...) {
      v = IntegerPaneInfo->Value;
    }

  // Check limits
  if((v < IntegerPaneInfo->Min) || (v > IntegerPaneInfo->Max))
    v = IntegerPaneInfo->Value;

  return IntToStr(v);
}


// Implementation of class TParamsDialog
//---------------------------------------------------------------------------

// Constructor
TParamsDialog::TParamsDialog(void)
{
  m_ParamsForm = new TParamsForm(Application);

  m_CurrentButtonsMode = pbOkCancel;
  m_OnApply = NULL;
}

// Add a container to the dialog layout
void TParamsDialog::AddParamsContainer(CParamsContainer *ParamsContainer,bool ShowLimits)
{
  m_ParamsForm->BuildDialogLayout(ParamsContainer,ShowLimits);
}

// Open the dialog
bool TParamsDialog::Execute(void)
{
  bool RetValue = false;

  // Set dialog caption
  if(m_Caption != "")
    m_ParamsForm->Caption = m_Caption;

  // Set dialog features
  m_ParamsForm->SetDialogFeatures(m_CurrentButtonsMode,m_OnApply);

  // Show the dialog
  if(m_ParamsForm->ShowModal() == mrOk)
    RetValue = true;

  return RetValue;
}

void __fastcall TParamsForm::FormShow(TObject *Sender)
{
  // Update parameter values
  AssignFromParams(false);
}

void __fastcall TParamsForm::RestoretoDefaultMenuItemClick(TObject *Sender)
{
  // Retrieve a pointer to the parameter pane
  TParamPaneItem *ParamPane = reinterpret_cast<TParamPaneItem *>(SpecificParamPopupMenu->PopupComponent->Tag);

  // Restore default and assign
  AssignFromParamPaneObjet(ParamPane,true,false);
}

// Revert changes to this parameter (since last apply)
void __fastcall TParamsForm::RevertChangesMenuItemClick(TObject *Sender)
{
  // Retrieve a pointer to the parameter pane
  TParamPaneItem *ParamPane = reinterpret_cast<TParamPaneItem *>(SpecificParamPopupMenu->PopupComponent->Tag);

  // Restore default and assign
  AssignFromParamPaneObjet(ParamPane,false,true);
}

// This event fires just before the popup of the SpecificParamPopupMenu
void __fastcall TParamsForm::SpecificParamPopupMenuPopup(TObject *Sender)
{
  // Retrieve a pointer to the parameter pane
  TParamPaneItem *ParamPane = reinterpret_cast<TParamPaneItem *>(SpecificParamPopupMenu->PopupComponent->Tag);

  // Set the menu "Enabled" properties according to the parameter read-only flag

  unsigned ParamsAttributes = ParamPane->m_ParamPtr->GetAttributes();

  RestoretoDefaultMenuItem->Enabled = !(ParamsAttributes & paRDOnly) && !(ParamsAttributes & paBlockDefaults);
  RevertChangesMenuItem->Enabled = !(ParamsAttributes & paRDOnly);
}

// Build a "hint" string for a specific parameter
AnsiString __fastcall TParamsForm::BuildParamHintString(CNameParamBase *ParamPtr)
{
  AnsiString Result;

  const char CRLF[] = "\n\r";

  // Check if the paNoDescription attribute is excluded
  if(!(ParamPtr->GetAttributes() & paNoDescription)) {

    // If the description string is not empty
    if(!ParamPtr->Description().IsEmpty())
      Result = ParamPtr->Description() + CRLF;
    else
      // If no description is assigned, used the parameter name as the title of the hint
      Result = ParamPtr->Name() + CRLF;

    // If the parameter has limits, add the limits range
    if(ParamPtr->GetLimitsMode())
      Result += "Range: " + ParamPtr->MinLimitToString() + " - " + ParamPtr->MaxLimitToString() + CRLF;

    // Add the default value string
    Result += "Default: " + ParamPtr->DefaultValueToString();

    // If the parameter is read only, add a notification to the hint
    if(ParamPtr->GetAttributes() & paRDOnly)
      Result += "\n\r- Read Only -";

    // If the parameter has blocked defaults attribute, add a notification to the hint
    if(ParamPtr->GetAttributes() & paBlockDefaults)
      Result += "\n\r- Blocked Default -";
  }

  return Result;
}

// Help function for matching search string
bool TParamsForm::MatchSearchStrings(AnsiString Str,AnsiString SearchStr)
{
  AnsiString s1 = Str.UpperCase();

  if(s1.AnsiPos(SearchStr.UpperCase()) != 0)
    return true;

  return false;
}

// Implement search for a given parameter
void TParamsForm::DoParameterSearch(AnsiString SearchStr,TParamPaneList::iterator StartItem)
{
  // Search for the parameter in the params pane list
  for(TParamPaneList::iterator i = StartItem; i != m_ParamPaneList.end(); ++i)
  {
    if(MatchSearchStrings((*i)->m_ParamPtr->Name(),SearchStr))
    {
      // If there is a match, make sure that the variable is visible and move the mouse cursor near to it
      (*i)->m_PaneControl->Show();
      Mouse->CursorPos = (*i)->m_PaneControl->ClientToScreen(TPoint(5,-2));

      // Remember last search result + next item
      ++i;
      m_LastSearchResult = i;
      return;
    }
  }

  // An iterator value for end() means not found
  m_LastSearchResult = m_ParamPaneList.end();

  MessageDlg("Search string '" + SearchStr + "' not found",mtInformation,TMsgDlgButtons() << mbOK,0);
}


// Find and show a parameter
void __fastcall TParamsForm::FindActionExecute(TObject *Sender)
{
  AnsiString SearchStr = InputBox("Find","Enter parameter name:","");

  // Check if canceled
  if(SearchStr.IsEmpty())
    return;

  DoParameterSearch(SearchStr,m_ParamPaneList.begin());

  // Remember the search string
  m_SearchString = SearchStr;
}

void __fastcall TParamsForm::SearchAgainActionExecute(TObject *Sender)
{
  if(m_SearchString.IsEmpty())
    return;

  DoParameterSearch(m_SearchString,m_LastSearchResult);
}
//---------------------------------------------------------------------------

