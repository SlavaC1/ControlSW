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

#include <vcl.h>
#pragma hdrstop

#include <typeinfo.h>
#include "ParamsDialog.h"
#include "QParamsContainer.h"
#include "ArrayParamsEditorDlg.h"
#include "ParamsLevelExposure.h"
#include "BackEndInterface.h"
//TRIPLEX_MACHINE
#include "Q2RTApplication.h" 
#include "QMonitor.h"      
#include "QFileWithCheckSum.h"  
#include "QErrors.h" 
//
#include <limits>

#pragma package(smart_init)
#pragma resource "*.dfm"


const INT_EDIT_WIDTH = 72;
const REAL_EDIT_WIDTH = 87;
const GENERIC_EDIT_WIDTH = 87;
const COMBO_EXTRA_WIDTH = 30;
const PARAM_TITLE_TO_FIELD_SPACE = 3;
const CORNER_SPACING = 10;
const PANE_SPACING = 8;
const PARAMS_RAW_SPACING = 110;
const PARAMS_RAW_THRSHLD = 10;
const PANE_WIDTH_SPACING = 10;
const UP_DOWN_WIDTH = 16;
const UP_DOWN_HEIGHT = 21;
const MIN_CHECK_BOX_WIDTH = 20;
const MAX_COMBO_TEXT_WIDTH = 100;

const TAB_HEIGHT = 350;

const int REAR_FRONT_PADDING   = 10;
const int ITEMS_WITHOUT_SCROLL = 27;
const int SCROLLBAR_PADDING    = 20;

const int CONTROLS_RELOCATION_TAG = -1;
//TRIPLEX_MACHINE
const char BASIC_PARAMETERS_FILE_NAME[]   = "Configs\\BasicPMLevel.cfg";

#define CHANGE_HIGHLIGHT_COLOR         clRed
#define NORMAL_TEXT_COLOR              clBlack
#define PARAM_LABEL_NO_SAVE_COLOR      clBlue
#define PARAM_FROM_MATERIAL_MODE_COLOR clTeal

bool TParamsForm::m_RestartRequired = false;

TParamsForm *ParamsForm;
//---------------------------------------------------------------------------
__fastcall TParamsForm::TParamsForm(TComponent* Owner)
        : TForm(Owner)
{
	m_ParamsFormDestructionCallback = NULL;
	m_Cockie                        = 0;
}

__fastcall TParamsForm::TParamsForm(TParamsForm* Other)
        : TForm(Other->Owner)
{
  m_ShowLimitsMode = Other->m_ShowLimitsMode;
  m_SaveWhenApply = Other->m_SaveWhenApply;
  m_OnApply = Other->m_OnApply;
  m_SearchString = Other->m_SearchString;
  m_ParamGetEvent = Other->m_ParamGetEvent;
  m_ParamSetEvent = Other->m_ParamSetEvent;

  OkAction->Enabled = Other->OkAction->Enabled;
  CancelAction->Enabled = Other->CancelAction->Enabled;
  ApplyAction->Enabled = Other->ApplyAction->Enabled;
  OkAction->Enabled = Other->OkAction->Enabled;
  CloseAction->Enabled = Other->CloseAction->Enabled;
  
  m_ParamsFormDestructionCallback = Other->m_ParamsFormDestructionCallback;
  m_Cockie                        = Other->m_Cockie; 
}

//---------------------------------------------------------------------------


// Help function to generate a parameter title string
AnsiString TParamsForm::BuildParamTitleString(CQParameterBase *ParamPtr)
{
  QString s(ParamPtr->Name());

  if((ParamPtr->GetLimitsMode()) && m_ShowLimitsMode)
    s = s + " (" + ParamPtr->MinLimitAsString() + " - " + ParamPtr->MaxLimitAsString() + ")";

  return s.c_str();
}

// Build a pane for integer (short,long, etc.) parameter
bool TParamsForm::AddIntegerPane(TWinControl *Parent,int PaneLeft,int PaneTop,int MaxParentHeight,
                                 CQParameterBase *ParamPtr,int& PaneWidth,int& PaneHeight)
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
  TmpEdit->Enabled = ! ((ParamPtr->GetAttributes() & paRDOnly) || (ParamPtr->GetAttributes() & paFromMaterialMode)); 

  TmpUpDown = new TUpDown(this);
  TmpUpDown->Parent = Parent;
  TmpUpDown->Width = UP_DOWN_WIDTH;
  TmpUpDown->Height = UP_DOWN_HEIGHT;
  TmpUpDown->Left = TmpEdit->Left + TmpEdit->Width;
  TmpUpDown->Top = TmpEdit->Top;
  TmpUpDown->Wrap = true;
  TmpUpDown->Enabled = ! ((ParamPtr->GetAttributes() & paRDOnly) || (ParamPtr->GetAttributes() & paFromMaterialMode));

  // Set limits
  TmpUpDown->Min = std::numeric_limits<short>::min();
  TmpUpDown->Max = std::numeric_limits<short>::max();

  // Set initial values
  TmpEdit->Text = GetParamValue(ParamPtr);

  // Allocate an integer pane info structure
  IntegerPaneInfo = new TIntegerPaneInfo;
  IntegerPaneInfo->Min = QStrToInt(ParamPtr->MinLimitAsString());
  IntegerPaneInfo->Max = QStrToInt(ParamPtr->MaxLimitAsString());
  IntegerPaneInfo->Value = StrToInt(TmpEdit->Text);
  IntegerPaneInfo->EditFieldPtr = TmpEdit;
  IntegerPaneInfo->LimitsMode = ParamPtr->GetLimitsMode();

  TmpUpDown->Tag = reinterpret_cast<int>(IntegerPaneInfo);

  PaneWidth = TmpEdit->Width + TmpUpDown->Width;

  // The total width is set according to the label width
  PaneWidth = max(PaneWidth,TmpLabel->Width);

  PaneHeight = TmpLabel->Height + PARAM_TITLE_TO_FIELD_SPACE + TmpEdit->Height;

  // Check if the param pane overflow the parent bottom
  if(PaneTop + PaneHeight + PARAMS_RAW_THRSHLD > MaxParentHeight) {
    // Delete the temporary pane components
    TmpLabel->Parent = NULL;
    delete TmpLabel;

    TmpEdit->Parent = NULL;
    delete TmpEdit;

    TmpUpDown->Parent = NULL;
    delete TmpUpDown;

    delete IntegerPaneInfo;

    return false;
  }

  // Add an entry in the parameter pane list
  TParamPaneItem *ParamPaneItem = new TParamPaneItem(TmpUpDown,ParamPtr,TmpLabel);
  m_ParamPaneList.push_back(ParamPaneItem);

  // Attach the pane pointer to the tag property
  TmpEdit->Tag = reinterpret_cast<int>(ParamPaneItem);
  TmpLabel->Tag = reinterpret_cast<int>(ParamPaneItem);

  TmpUpDown->OnChangingEx = UpDownChangingEvent;
  TmpEdit->OnChange = PaneControlChangingEvent;

  return true;
}

// Build a pane for real (floating point) parameter
bool TParamsForm::AddRealPane(TWinControl *Parent,int PaneLeft,int PaneTop,int MaxParentHeight,
                              CQParameterBase *ParamPtr,int& PaneWidth,int& PaneHeight)
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
  TmpEdit->Text = GetParamValue(ParamPtr);
  TmpEdit->Parent = Parent;
  TmpEdit->Enabled = ! ((ParamPtr->GetAttributes() & paRDOnly) || (ParamPtr->GetAttributes() & paFromMaterialMode));

  PaneWidth = TmpEdit->Width;

  // The total width is set according to the label width
  PaneWidth = max(PaneWidth,TmpLabel->Width);

  PaneHeight = TmpLabel->Height + PARAM_TITLE_TO_FIELD_SPACE + TmpEdit->Height;

  TmpEdit->OnChange = PaneControlChangingEvent;

  // Check if the param pane overflow the parent bottom
  if(PaneTop + PaneHeight + PARAMS_RAW_THRSHLD > MaxParentHeight) {
    // Delete the temporary pane components
    TmpLabel->Parent = NULL;
    delete TmpLabel;

    TmpEdit->Parent = NULL;
    delete TmpEdit;

    return false;
  }

  // Add an entry in the parameter pane list
  TParamPaneItem *ParamPaneItem = new TParamPaneItem(TmpEdit,ParamPtr,TmpLabel);
  m_ParamPaneList.push_back(ParamPaneItem);

  TmpEdit->Tag = reinterpret_cast<int>(ParamPaneItem);
  TmpLabel->Tag = reinterpret_cast<int>(ParamPaneItem);

  return true;
}

// Build a pane for boolean parameter
bool TParamsForm::AddBoolPane(TWinControl *Parent,int PaneLeft,int PaneTop,int MaxParentHeight,
                              CQParameterBase *ParamPtr,int& PaneWidth,int& PaneHeight)
{
  TLabel *TmpLabel;
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
  TmpCheckBox->Checked = (GetParamValue(ParamPtr) == "1");
  TmpCheckBox->Enabled = ! ((ParamPtr->GetAttributes() & paRDOnly) || (ParamPtr->GetAttributes() & paFromMaterialMode));
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
    delete TmpLabel;

    TmpCheckBox->Parent = NULL;
    delete TmpCheckBox;

    return false;
  }

  // Add an entry in the parameter pane list
  TParamPaneItem *ParamPaneItem = new TParamPaneItem(TmpCheckBox,ParamPtr,TmpCheckBox);
  m_ParamPaneList.push_back(ParamPaneItem);

  TmpCheckBox->Tag = reinterpret_cast<int>(ParamPaneItem);

  return true;
}

// Build a pane for generic parameter
bool TParamsForm::AddGenericPane(TWinControl *Parent,int PaneLeft,int PaneTop,int MaxParentHeight,
                                 CQParameterBase *ParamPtr,int& PaneWidth,int& PaneHeight)
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
  TmpEdit->Text = GetParamValue(ParamPtr);
  TmpEdit->Parent = Parent;
  TmpEdit->Enabled = ! ((ParamPtr->GetAttributes() & paRDOnly) || (ParamPtr->GetAttributes() & paFromMaterialMode));

  PaneWidth = TmpEdit->Width;

  // The total width is set according to the label width
  PaneWidth = max(PaneWidth,TmpLabel->Width);

  PaneHeight = TmpLabel->Height + PARAM_TITLE_TO_FIELD_SPACE + TmpEdit->Height;

  TmpEdit->OnChange = PaneControlChangingEvent;

  // Check if the param pane overflow the parent bottom
  if(PaneTop + PaneHeight + PARAMS_RAW_THRSHLD > MaxParentHeight) {
    // Delete the temporary pane components
    TmpLabel->Parent = NULL;
    delete TmpLabel;

    TmpEdit->Parent = NULL;
    delete TmpEdit;

    return false;
  }

  // Add an entry in the parameter pane list
  TParamPaneItem *ParamPaneItem = new TParamPaneItem(TmpEdit,ParamPtr,TmpLabel);
  m_ParamPaneList.push_back(ParamPaneItem);

  TmpEdit->Tag = reinterpret_cast<int>(ParamPaneItem);
  TmpLabel->Tag = reinterpret_cast<int>(ParamPaneItem);

  return true;
}

// Build a pane for enumerated parameter
bool TParamsForm::AddEnumPane(TWinControl *Parent,int PaneLeft,int PaneTop,int MaxParentHeight,
                              CQEnumParameter *ParamPtr,int& PaneWidth,int& PaneHeight)
{
  TLabel *TmpLabel;
  TComboBox *TmpCombo;

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
  TmpCombo = new TComboBox(this);
  TmpCombo->Left = PaneLeft;
  TmpCombo->Top = PaneTop + TmpLabel->Height + PARAM_TITLE_TO_FIELD_SPACE;
  TmpCombo->Parent = Parent;
  TmpCombo->Enabled = ! ((ParamPtr->GetAttributes() & paRDOnly) || (ParamPtr->GetAttributes() & paFromMaterialMode));

  // Add items to the combo
  CQStringList ValueStringList = ParamPtr->GetValueStringList();
  for(unsigned i = 0; i < ValueStringList.Count(); i++)
    TmpCombo->Items->Add(ValueStringList[i].c_str());

  TmpCombo->Text = ParamPtr->GetBestValueStr(StrToInt(GetParamValue(ParamPtr))).c_str();

  int ComboTextWidth = Canvas->TextWidth(ParamPtr->GetMaxLengthString().c_str());
  int ComboMaxTextWidth = max(GENERIC_EDIT_WIDTH - COMBO_EXTRA_WIDTH,min(ComboTextWidth,MAX_COMBO_TEXT_WIDTH));

  TmpCombo->Width = ComboMaxTextWidth + COMBO_EXTRA_WIDTH;

  PaneWidth = TmpCombo->Width;

  // The total width is set according to the label width
  PaneWidth = max(PaneWidth,TmpLabel->Width);

  PaneHeight = TmpLabel->Height + PARAM_TITLE_TO_FIELD_SPACE + TmpCombo->Height;

  TmpCombo->OnChange = PaneControlChangingEvent;

  // Check if the param pane overflow the parent bottom
  if(PaneTop + PaneHeight + PARAMS_RAW_THRSHLD > MaxParentHeight) {
    // Delete the temporary pane components
    TmpLabel->Parent = NULL;
    delete TmpLabel;

    TmpCombo->Parent = NULL;
    delete TmpCombo;

    return false;
  }

  // Add an entry in the parameter pane list
  TParamPaneItem *ParamPaneItem = new TParamPaneItem(TmpCombo,ParamPtr,TmpLabel);
  m_ParamPaneList.push_back(ParamPaneItem);

  TmpCombo->Tag = reinterpret_cast<int>(ParamPaneItem);
  TmpLabel->Tag = reinterpret_cast<int>(ParamPaneItem);

  return true;
}

// Build a pane for an array parameter
bool TParamsForm::AddArrayPane(TWinControl *Parent,int PaneLeft,int PaneTop,int MaxParentHeight,
                               CQParameterBase *ParamPtr,int& PaneWidth,int& PaneHeight)
{
  TLabel *TmpLabel;
  TEdit *TmpEdit;
  TButton *TmpButton;

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
  TmpEdit->Text = GetParamValue(ParamPtr);
  TmpEdit->Parent = Parent;
  TmpEdit->Enabled = ! ((ParamPtr->GetAttributes() & paRDOnly) || (ParamPtr->GetAttributes() & paFromMaterialMode));

  // Build the "open editor" button
  TmpButton = new TButton(this);
  TmpButton->Width = TmpEdit->Height - 2;
  TmpButton->Height = TmpButton->Width;
  TmpButton->Left = TmpEdit->Left + TmpEdit->Width;
  TmpButton->Top = TmpEdit->Top + 1;
  TmpButton->Caption = "...";
  TmpButton->Parent = Parent;
  TmpButton->Enabled = ! ((ParamPtr->GetAttributes() & paRDOnly) || (ParamPtr->GetAttributes() & paFromMaterialMode));
  TmpButton->Hint = "Open array parameter editor";
  TmpButton->ShowHint = true;
  TmpButton->OnClick = ArrayParameterOpenEventHandler;

  PaneWidth = TmpEdit->Width + TmpButton->Width;

  // The total width is set according to the label width
  PaneWidth = max(PaneWidth,TmpLabel->Width);

  PaneHeight = TmpLabel->Height + PARAM_TITLE_TO_FIELD_SPACE + TmpEdit->Height;

  TmpEdit->OnChange = PaneControlChangingEvent;

  // Check if the param pane overflow the parent bottom
  if(PaneTop + PaneHeight + PARAMS_RAW_THRSHLD > MaxParentHeight) {
    // Delete the temporary pane components
    TmpLabel->Parent = NULL;
    delete TmpLabel;

    TmpEdit->Parent = NULL;
    delete TmpEdit;

    TmpButton->Parent = NULL;
    delete TmpButton;

    return false;
  }

  // Add an entry in the parameter pane list
  TParamPaneItem *ParamPaneItem = new TParamPaneItem(TmpEdit,ParamPtr,TmpLabel);
  m_ParamPaneList.push_back(ParamPaneItem);

  TmpEdit->Tag = reinterpret_cast<int>(ParamPaneItem);
  TmpLabel->Tag = reinterpret_cast<int>(ParamPaneItem);
  TmpButton->Tag = reinterpret_cast<int>(ParamPaneItem);

  return true;
}

void TParamsDialog::RefreshDisplay()
{
  m_ParamsForm->RefreshActionExecute(NULL);
}

// Add a new parameter "pane" to the parent control
bool TParamsForm::AddParamControlPane(TWinControl *Parent,int PaneLeft,int PaneTop,int MaxParentHeight,
                                      CQParameterBase *ParamPtr,int& PaneWidth,int& PaneHeight)
{
  bool Ret = false;

  switch(ParamPtr->GetTypeID()) {
    case ptInt:
      Ret = AddIntegerPane(Parent,PaneLeft,PaneTop,MaxParentHeight,ParamPtr,PaneWidth,PaneHeight);
      break;

    case ptReal:
      Ret = AddRealPane(Parent,PaneLeft,PaneTop,MaxParentHeight,ParamPtr,PaneWidth,PaneHeight);
      break;

    case ptBool:
      Ret = AddBoolPane(Parent,PaneLeft,PaneTop,MaxParentHeight,ParamPtr,PaneWidth,PaneHeight);
      break;

    case ptGeneric:
      Ret = AddGenericPane(Parent,PaneLeft,PaneTop,MaxParentHeight,ParamPtr,PaneWidth,PaneHeight);
      break;

    case ptEnum:
      Ret = AddEnumPane(Parent,PaneLeft,PaneTop,MaxParentHeight,dynamic_cast<CQEnumParameter *>(ParamPtr),
                        PaneWidth,PaneHeight);
      break;

    case ptArray:
      Ret = AddArrayPane(Parent,PaneLeft,PaneTop,MaxParentHeight,ParamPtr,PaneWidth,PaneHeight);
      break;

    default:
      Abort();
  }

  return Ret;
}

// Build a parameter dialog layout according to the container content.
// Note: This function can be called several times with different containers.
void TParamsForm::BuildDialogLayout(CQParamsContainer *ParamsContainer, int ExposureLevel, bool ShowLimits, CQStringList* HideTabs, bool passwordValid)
{
  int        PaneWidth;
  int        PaneHeight;
  int        TabNameMaxLength = 0;
  AnsiString TabCaption;

  CQStringList StrList;  
  QString FileName = Q2RTApplication->AppFilePath.Value() + BASIC_PARAMETERS_FILE_NAME;

  CQParamsFileWithChecksumStream ParamsStream(FileName);  

  m_checksumValid = ParamsStream.LoadFileIntoStringList(FileName,StrList);
  if(!m_checksumValid)
  {
	//QMonitor.ErrorMessage("The files "+ FileName +" and " + FileName +".bak are both missing or corrupted.\n\r Without these files the Parameter Manager cannot be loaded. \n\r");
	//m_checksumValid = false;
	//return;
	m_checksumValid = true;

  }

  LogParametersButton->Visible = (ExposureLevel <= QA_LEVEL);
  m_ShowLimitsMode             = ShowLimits;

  for(int i = 0; i < ParamsContainer->PropertyCount();)
  {
    CQParameterBase *CurrentParam = dynamic_cast<CQParameterBase *>(ParamsContainer->Properties[i]);

	if(passwordValid) //goto advanced level
	{
	  // remove readOnly attribute (enable writing)
	  if(CurrentParam->GetAttributes() & paRDOnly)
	  {
		CurrentParam->RemoveAttribute(paRDOnly);
	  }
	}
	else //stay/return in basic level
	{
	  int strIndex = StrList.Find(CurrentParam->Name());
	  if(strIndex==-1)  //parameter not on the list
	  {
		CurrentParam->AddAttribute(paRDOnly);
	  }
	}

   // If the parameter is hidden, continue to next parameter
    if(CurrentParam->GetAttributes() & paHidden)
    {
      i++;
      continue;
    }
    TTabsList::iterator CurrentTabItem = NULL;
    TScrollBox *ScrollBox;

    // Search for the parameters section in the tabs list
    for(TTabsList::iterator TabsIterator = m_TabsList.begin(); TabsIterator != m_TabsList.end(); ++TabsIterator)
    {
      if((*TabsIterator).TabSheet->Caption == CurrentParam->GroupName().c_str())
      {
        CurrentTabItem = TabsIterator;
        ScrollBox = reinterpret_cast<TScrollBox *>((*TabsIterator).TabSheet->Tag);
        break;
      }
    }
    
    // If no tab could be found, create a new one
    if(CurrentTabItem == NULL)
    {
      TTabSheet* CurrentTabSheet;

      TabCaption = CurrentParam->GroupName().c_str();

      // Keeping the tab's name max size for later...
      WidthTestLabel->Caption = TabCaption;
      if(TabNameMaxLength < WidthTestLabel->Width)
        TabNameMaxLength = WidthTestLabel->Width;

      ParamTabsListBox->Items->Add(TabCaption);

      CurrentTabSheet = new TTabSheet(this);
      CurrentTabSheet->PageControl = PageControl1;
      CurrentTabSheet->Caption = TabCaption;
      CurrentTabSheet->Height = TAB_HEIGHT;

      m_ParamsTabsList[TabCaption] = CurrentTabSheet->TabIndex;

      TTabsListItem TabListItem = {CurrentTabSheet,CORNER_SPACING,CORNER_SPACING};

      m_TabsList.push_back(TabListItem);
      CurrentTabItem = &m_TabsList[m_TabsList.size() - 1];
      CurrentTabItem->MaxWidth = 0;

      // Create a scroll box on the tab
      ScrollBox = new TScrollBox(this);
      ScrollBox->Parent = CurrentTabSheet;
      ScrollBox->BorderStyle = bsNone;
      ScrollBox->Align = alClient;
      //ScrollBox->VertScrollBar->Visible = false;
      ScrollBox->VertScrollBar->Tracking = true;
      ScrollBox->HorzScrollBar->Tracking = true;
	  ScrollBox->AutoScroll = true;
	  
      CurrentTabSheet->Tag = reinterpret_cast<int>(ScrollBox);
    } // end of ...if(CurrentTabItem == NULL)

    // If the parameter has the "paRestartRequired" flag set, add the special generic observer
    if(CurrentParam->GetAttributes() & paRestartRequired)
       CurrentParam->AddGenericObserver(RestartRequiredParamObserver);

    try
    {

      if(AddParamControlPane(ScrollBox,CurrentTabItem->CurX,CurrentTabItem->CurY,
                             TAB_HEIGHT,CurrentParam,PaneWidth,PaneHeight))
      {

        CurrentTabItem->CurY += PaneHeight + PANE_SPACING;

        if(PaneWidth > CurrentTabItem->MaxWidth)
          CurrentTabItem->MaxWidth = PaneWidth;

        i++;
      }
      else
      {
          // The last parameter pane overflow the bottom of the window, so we need to go to the
          // next row.
          CurrentTabItem->CurY = CORNER_SPACING;
          CurrentTabItem->CurX += max(PARAMS_RAW_SPACING,CurrentTabItem->MaxWidth + PANE_WIDTH_SPACING);

          // Reset the maximum pane width for the current row
          CurrentTabItem->MaxWidth = 0;
      }
    }
    catch(...)
    {
        // In case of an error in AddParamControlPane() we skip the parameter and continue to the next one
        i++;
    }
  }

  // 'Hiding' all the tabs....
  for(int i = 0; i < PageControl1->PageCount; i++)
    PageControl1->Pages[i]->TabVisible = false;

  int ListBoxOldSize = ParamTabsListBox->Width;
  
  int FormShift      = 0;

  ParamTabsListBox->Width = TabNameMaxLength + REAR_FRONT_PADDING;
  // Determine the Tabs list edit's size to the max size of tab's names
  if(ParamTabsListBox->Count > ITEMS_WITHOUT_SCROLL)
    ParamTabsListBox->Width += SCROLLBAR_PADDING;

  FormShift = ParamTabsListBox->Width - ListBoxOldSize;

  this->Width += FormShift;

  for(int i = 0; i < this->ComponentCount; i++)
  {
    TControl *MyComp = (TControl *)this->Components[i];

    if(MyComp != NULL)
      if(MyComp->Tag == CONTROLS_RELOCATION_TAG)
        MyComp->Left += FormShift;
  }

  // Activating the first tab
  ParamTabsListBox->ItemIndex = 0;
  ActivateDesiredTab();
}

void __fastcall TParamsForm::OkActionExecute(TObject *Sender)
{
  // Apply and close
  AssignToParams();
  ModalResult = mrOk;

  CheckRestartRequired();
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
  try
  {
    Screen->Cursor = crHourGlass;

    // Assign and get
    AssignToParams();
    AssignFromParams(false);

    CheckRestartRequired();
  }
  catch(EQException& E)
  {
    Screen->Cursor = crDefault;
    MessageDlg(E.GetErrorMsg().c_str(),mtError,TMsgDlgButtons() << mbOK,0);
  }

  Screen->Cursor = crDefault;
}
//---------------------------------------------------------------------------

void __fastcall TParamsForm::CloseActionExecute(TObject *Sender)
{
  ModalResult = mrNone;
  Close();
}

void TParamsForm::SetDialogFeatures(TParamDialogButtonsMode ButtonsMode,TNotifyEvent OnApply)
{
  // Set the dialog button actions according to the current dialog mode
  switch(ButtonsMode)
  {
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
bool TParamsForm::CheckFloatField(CQParameterBase *ParamPtr,AnsiString FloatText)
{
  bool r = true;
  long double v,MinV,MaxV;

  try {
    // Convert to float number
    v = StrToFloat(FloatText);

    if(ParamPtr->GetLimitsMode()) {
      // Convert the min and max values to float
      MinV = QStrToFloat(ParamPtr->MinLimitAsString());
      MaxV = QStrToFloat(ParamPtr->MaxLimitAsString());

      // Check range
      if((v < MinV) || (v > MaxV))
        r = false;
    }

  } catch(...) {
      r = false;
    }

  return r;
}

// Assign values from the GUI to the parameters
void TParamsForm::AssignToParams(void)
{
  // Block all other threads during the assignment
  m_CriticalSection->Enter();

  try
  {
    // Process all the elements in the pane list
    for(TParamPaneList::iterator i = m_ParamPaneList.begin(); i != m_ParamPaneList.end(); ++i) {
      // If the parameter is read only, do nothing
      if(((*i)->m_ParamPtr->GetAttributes() & paRDOnly) || ((*i)->m_ParamPtr->GetAttributes() & paFromMaterialMode))
        continue;

      // Process each param according to it's type
      switch((*i)->m_ParamPtr->GetTypeID()) {
        case ptInt:
        {
          TUpDown *UpDown = dynamic_cast<TUpDown *>((*i)->m_PaneControl);
          TIntegerPaneInfo *IntegerPaneInfo = reinterpret_cast<TIntegerPaneInfo *>(UpDown->Tag);

          SetParamValue((*i)->m_ParamPtr,GetIntegerFieldValue(IntegerPaneInfo));
        }
        break;

        case ptReal:
        {
          TEdit *TmpEdit = dynamic_cast<TEdit *>((*i)->m_PaneControl);

          // Check value
          if(CheckFloatField((*i)->m_ParamPtr,TmpEdit->Text))
            SetParamValue((*i)->m_ParamPtr,TmpEdit->Text);
        }
        break;

        case ptBool:
        {
          TCheckBox *TmpCheckBox = dynamic_cast<TCheckBox *>((*i)->m_PaneControl);
          SetParamValue((*i)->m_ParamPtr,TmpCheckBox->Checked ? "1" : "0");
        }
        break;

        case ptEnum:
        {
          TComboBox *TmpComboBox = dynamic_cast<TComboBox *>((*i)->m_PaneControl);

          CQEnumParameter *EnumParam = dynamic_cast<CQEnumParameter *>((*i)->m_ParamPtr);
          SetParamValue(EnumParam,EnumParam->BestValueFromString(TmpComboBox->Text.c_str()).c_str());
        }
        break;

        case ptGeneric:
        case ptArray:        
        {
          TEdit *TmpEdit = dynamic_cast<TEdit *>((*i)->m_PaneControl);
          SetParamValue((*i)->m_ParamPtr,TmpEdit->Text);
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
  {
    m_OnApply(this);

    // Make sure that the dialog has the focus
    SetFocus();
  }
}

// Change the color of the parameter label according to its attributes
void TParamsForm::SetParameterLabelColor(CQParameterBase *ParamPtr,TControl *LabelControl)
{
	TLabel *Label = dynamic_cast<TLabel *>(LabelControl);

	// Is it really a label?
	if(Label != NULL)
	{
		if((ParamPtr->GetAttributes() & paNoSave) || !ParamPtr->IsStackEmpty())
			Label->Font->Color = PARAM_LABEL_NO_SAVE_COLOR;
		else
			Label->Font->Color = NORMAL_TEXT_COLOR;
		
		if(ParamPtr->GetAttributes() & paFromMaterialMode)
			Label->Font->Color = PARAM_FROM_MATERIAL_MODE_COLOR;
	} 
	else
	{
		TCheckBox *ChkBox = dynamic_cast<TCheckBox *>(LabelControl);

		// Is it a checkbox?
		if(ChkBox != NULL)
		{
			if((ParamPtr->GetAttributes() & paNoSave) || !ParamPtr->IsStackEmpty())
				ChkBox->Font->Color = PARAM_LABEL_NO_SAVE_COLOR;
			else
				ChkBox->Font->Color = NORMAL_TEXT_COLOR;
			
			if(ParamPtr->GetAttributes() & paFromMaterialMode)
				ChkBox->Font->Color = PARAM_FROM_MATERIAL_MODE_COLOR;
		}
	}
}

// Assign a value from a single parameter pane objet to the GUI element
void TParamsForm::AssignFromParamPaneObjet(TParamPaneItem *ParamPane,bool AssignDefaultValue,
                                                      bool ChangeToNormalColor)
{
  // Avoid setting the parameter to defaults if the paBlockDefaults option is included
  if(AssignDefaultValue && (ParamPane->m_ParamPtr->GetAttributes() & paBlockDefaults))
    return;

  SetParameterLabelColor(ParamPane->m_ParamPtr,ParamPane->m_ParamLabel);  

  // Process each param according to it's type
  switch(ParamPane->m_ParamPtr->GetTypeID()) {
    case ptInt:
    {
      TUpDown *UpDown = dynamic_cast<TUpDown *>(ParamPane->m_PaneControl);
      TIntegerPaneInfo *IntegerPaneInfo = reinterpret_cast<TIntegerPaneInfo *>(UpDown->Tag);

      if(AssignDefaultValue)
        IntegerPaneInfo->Value = QStrToInt(ParamPane->m_ParamPtr->DefaultValueAsString());
      else
        IntegerPaneInfo->Value = StrToInt(GetParamValue(ParamPane->m_ParamPtr));

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
        TmpEdit->Text = ParamPane->m_ParamPtr->DefaultValueAsString().c_str();
      else
        TmpEdit->Text = GetParamValue(ParamPane->m_ParamPtr);

      if(ChangeToNormalColor)
        // Set font color to normal
        TmpEdit->Font->Color = NORMAL_TEXT_COLOR;
    }
    break;

    case ptBool:
    {
      TCheckBox *TmpCheckBox = dynamic_cast<TCheckBox *>(ParamPane->m_PaneControl);

      if(AssignDefaultValue)
        TmpCheckBox->Checked = (ParamPane->m_ParamPtr->DefaultValueAsString() == "1");
      else
        TmpCheckBox->Checked = (GetParamValue(ParamPane->m_ParamPtr) == "1");

      if(ChangeToNormalColor)
        SetParameterLabelColor(ParamPane->m_ParamPtr,TmpCheckBox);
    }
    break;

    case ptGeneric:
    case ptArray:
    {
      TEdit *TmpEdit = dynamic_cast<TEdit *>(ParamPane->m_PaneControl);

      if(AssignDefaultValue)
        TmpEdit->Text = ParamPane->m_ParamPtr->DefaultValueAsString().c_str();
      else
        TmpEdit->Text = GetParamValue(ParamPane->m_ParamPtr);

      if(ChangeToNormalColor)
        // Set font color to normal
        TmpEdit->Font->Color = NORMAL_TEXT_COLOR;
    }
    break;

    case ptEnum:
    {
      TComboBox *TmpCombo = dynamic_cast<TComboBox *>(ParamPane->m_PaneControl);
      CQEnumParameter *TmpEnumParam = dynamic_cast<CQEnumParameter *>(ParamPane->m_ParamPtr);

      if(AssignDefaultValue)
        TmpCombo->Text = TmpEnumParam->GetBestValueStr(TmpEnumParam->DefaultValue()).c_str();
      else
        TmpCombo->Text = TmpEnumParam->GetBestValueStr(StrToInt(GetParamValue(TmpEnumParam))).c_str();

      if(ChangeToNormalColor)
        // Set font color to normal
        TmpCombo->Font->Color = NORMAL_TEXT_COLOR;
    }
    break;
  }
}

// Assign values from the params to the parameters pane
void TParamsForm::AssignFromParams(bool AssignDefaultValue)
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

  // If we are in limits mode
  if(IntegerPaneInfo->LimitsMode)
  {
    // Check limits
    if((v < IntegerPaneInfo->Min) || (v > IntegerPaneInfo->Max))
      v = IntegerPaneInfo->Value;
    else
      // Save current value
      IntegerPaneInfo->Value = v;
  } else
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
    case ptArray:
    {
      TEdit *TmpEdit = dynamic_cast<TEdit *>(Sender);

      // Check if current value is different from the parameter value
      if(GetParamValue(ParamPane->m_ParamPtr) != TmpEdit->Text)
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
      if(GetParamValue(ParamPane->m_ParamPtr) != s.c_str())
        TmpCheckBox->Font->Color = CHANGE_HIGHLIGHT_COLOR;
      else
        SetParameterLabelColor(ParamPane->m_ParamPtr,TmpCheckBox);
    }
    break;

    case ptEnum:
    {
      TComboBox *TmpCombo = dynamic_cast<TComboBox *>(Sender);
      CQEnumParameter *EnumParam = dynamic_cast<CQEnumParameter *>(ParamPane->m_ParamPtr);

      // Check if current value is different from the parameter value
      if(EnumParam->GetBestValueStr(StrToInt(GetParamValue(EnumParam))) != TmpCombo->Text.c_str())
        TmpCombo->Font->Color = CHANGE_HIGHLIGHT_COLOR;
      else
        TmpCombo->Font->Color = NORMAL_TEXT_COLOR;
    }
    break;
  }
}

// Open the "array parameter" editor
void __fastcall TParamsForm::ArrayParameterOpenEventHandler(TObject *Sender)
{
  TParamPaneItem *ParamPane = reinterpret_cast<TParamPaneItem *>(dynamic_cast<TComponent *>(Sender)->Tag);
  TEdit *TmpEdit = dynamic_cast<TEdit *>(ParamPane->m_PaneControl);

  AnsiString ResultStr;

  if(TArrayParamsEditorForm::Execute(dynamic_cast<CQArrayParameterBase *>(ParamPane->m_ParamPtr),TmpEdit->Text,ResultStr))
    TmpEdit->Text = ResultStr;
}

void __fastcall TParamsForm::FormCreate(TObject *Sender)
{
  m_CriticalSection = new TCriticalSection;
  m_RestartRequired = false;
  m_ParamGetEvent = NULL;
  m_ParamSetEvent = NULL;
}

void __fastcall TParamsForm::FormDestroy(TObject *Sender)
{
	if (m_ParamsFormDestructionCallback)
		(*m_ParamsFormDestructionCallback)(true, m_Cockie);
	
	Q_SAFE_DELETE(m_CriticalSection);

	// Delete the integer panes info structures
	for(TParamPaneList::iterator i = m_ParamPaneList.begin(); i != m_ParamPaneList.end(); ++i)
	{
		if((*i)->m_ParamPtr->GetTypeID() == ptInt)
		{

			TUpDown *UpDown = dynamic_cast<TUpDown *>((*i)->m_PaneControl);
			TIntegerPaneInfo *IntegerPaneInfo = reinterpret_cast<TIntegerPaneInfo *>(UpDown->Tag);

			// Delete allocated structure
			Q_SAFE_DELETE(IntegerPaneInfo);
		}

		Q_SAFE_DELETE((*i));
	}
}

// Get an integer field value with limits and values checking
AnsiString TParamsForm::GetIntegerFieldValue(TIntegerPaneInfo *IntegerPaneInfo)
{
  // Get current value
  int v;
  try {
    v = StrToInt(IntegerPaneInfo->EditFieldPtr->Text);
  } catch(...) {
      v = IntegerPaneInfo->Value;
    }

  // Check limits
  if(IntegerPaneInfo->LimitsMode)
    if((v < IntegerPaneInfo->Min) || (v > IntegerPaneInfo->Max))
      v = IntegerPaneInfo->Value;

  return IntToStr(v);
}

void TParamsForm::SetAlternativeParameterAccess(TParamGetEvent GetRoutine,TParamSetEvent SetRoutine)
{
  m_ParamGetEvent = GetRoutine;
  m_ParamSetEvent = SetRoutine;
}

// Set an alternative set/get functions for a parameter
void TParamsDialog::SetAlternativeParameterAccess(TParamGetEvent GetRoutine,TParamSetEvent SetRoutine)
{
  m_ParamsForm->SetAlternativeParameterAccess(GetRoutine,SetRoutine);
}

void __fastcall TParamsForm::FormShow(TObject *Sender)
{
  m_RestartRequired = false;
  Screen->Cursor = crHourGlass;

  try
  {
    // Update parameter values
    AssignFromParams(false);

  } catch(EQException& E)
    {
      Screen->Cursor = crDefault;
      MessageDlg(E.GetErrorMsg().c_str(),mtError,TMsgDlgButtons() << mbOK,0);
    }

  Screen->Cursor = crDefault;
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

  RestoretoDefaultMenuItem->Enabled = !(ParamsAttributes & paRDOnly) && !(ParamsAttributes & paBlockDefaults) && !(ParamsAttributes & paFromMaterialMode);
  RevertChangesMenuItem->Enabled = !(ParamsAttributes & paRDOnly) && !(ParamsAttributes & paFromMaterialMode);
}

// Build a "hint" string for a specific parameter
AnsiString TParamsForm::BuildParamHintString(CQParameterBase *ParamPtr)
{
  QString Result;

  const char CRLF[] = "\n\r";

  // Check if the paNoDescription attribute is excluded
  if(!(ParamPtr->GetAttributes() & paNoDescription)) {

    // If the description string is not empty
    if(ParamPtr->Description() != "")
      Result = ParamPtr->Description() + CRLF;
    else
      // If no description is assigned, used the parameter name as the title of the hint
      Result = ParamPtr->Name() + CRLF;

    // If the parameter has limits, add the limits range
    if(ParamPtr->GetLimitsMode())
      Result += "Range: " + ParamPtr->LimitRangeAsString() + CRLF;

    // Add the default value string
    Result += "Default: " + ParamPtr->DefaultValueAsString();

    // If the parameter is read only, add a notification to the hint
    if(ParamPtr->GetAttributes() & paRDOnly)
      Result += "\n\r- Read Only -";

    // If the parameter has blocked defaults attribute, add a notification to the hint
    if(ParamPtr->GetAttributes() & paBlockDefaults)
      Result += "\n\r- Blocked Default -";

    // If the parameter has blocked defaults attribute, add a notification to the hint
    if(ParamPtr->GetAttributes() & paNoSave)
      Result += "\n\r- Not Saveable -";
	
	if (ParamPtr->GetAttributes() & paFromMaterialMode)
		Result += "\n\r- Loaded from material mode -";
  }

  return Result.c_str();
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
  AnsiString TabCaption = "";

  // Search for the parameter in the params pane list
  for(TParamPaneList::iterator i = StartItem; i != m_ParamPaneList.end(); ++i)
  {
    if(MatchSearchStrings((*i)->m_ParamPtr->Name().c_str(),SearchStr))
    {
      // If there is a match, make sure that the variable is visible and move the mouse cursor near to it
      (*i)->m_PaneControl->Show();
      //If the matched parameter isn't visible we need to auto-scroll horizontally.
	  //ScrollBy(deltaX,deltaY) doesn't work well.
	  TScrollBox *TabsHScrollBox = reinterpret_cast<TScrollBox *>(PageControl1->ActivePage->Tag);
	  TabsHScrollBox->ScrollInView((*i)->m_ParamLabel);

	  //Point the mouse's cursor next to the matching object
	  Mouse->CursorPos =(*i)->m_ParamLabel->ClientToScreen(TPoint(5,-2));
      TabCaption = PageControl1->ActivePage->Caption;

      for(int i = 0; i < ParamTabsListBox->Count; i++)
      {
        if(ParamTabsListBox->Items->Strings[i] == TabCaption)
        {
          ParamTabsListBox->ItemIndex = i;
          break;
        }
      }

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

void TParamsForm::RestartRequiredParamObserver(CQParameterBase */*Param*/,TGenericCockie /*Cockie*/)
{
  // Mark that a restart is required
  m_RestartRequired = true;
}

void TParamsForm::CheckRestartRequired(void)
{
  if(m_RestartRequired)
  {
    if(MessageDlg("The program must be restarted.\nRestart now?",mtConfirmation,
                  TMsgDlgButtons() << mbYes << mbNo,0) == mrYes)
    {
      m_RestartRequired = true;
      Close();
    }
    else
      m_RestartRequired = false;
  }
}

// Access functions to a get/set a parameter value
void TParamsForm::SetParamValue(CQParameterBase *ParamPtr,AnsiString Str)
{
  // If the alternative event is defined, use it to set the paramter value
  if(m_ParamSetEvent != NULL)
    m_ParamSetEvent(ParamPtr->Name().c_str(),Str);
  else
    // Use default method
    ParamPtr->AssignFromString(Str.c_str());
}

AnsiString TParamsForm::GetParamValue(CQParameterBase *ParamPtr)
{
  // If the alternative event is defined, use it to set the paramter value
  if(m_ParamGetEvent != NULL)
    return m_ParamGetEvent(ParamPtr->Name().c_str());

  // Use default method
  return ParamPtr->ValueAsString().c_str();
}

// Refresh parameters view
void __fastcall TParamsForm::RefreshActionExecute(TObject *Sender)
{
  Screen->Cursor = crHourGlass;

  try
  {
    AssignFromParams(false);
  } catch(EQException& E)
    {
      Screen->Cursor = crDefault;
      MessageDlg(E.GetErrorMsg().c_str(),mtError,TMsgDlgButtons() << mbOK,0);
    }

  Screen->Cursor = crDefault;
}

// Implementation of class TParamsDialog
//---------------------------------------------------------------------------

// Constructor
TParamsDialog::TParamsDialog(void)
{
  m_ParamsFormDestroyed = false;
  m_ParamsForm = new TParamsForm(Application);

  if (m_ParamsForm)
	m_ParamsForm->SetParamsFormDestructionCallback(ParamsFormDestructionCallback, reinterpret_cast<TGenericCockie>(this));  

  m_CurrentButtonsMode = pbOkCancel;
  m_OnApply = NULL;
  m_ApplyEnabled = true;
  m_ParamsForm->ApplyAction->Enabled = m_ApplyEnabled;
}

TParamsDialog::TParamsDialog(TParamsDialog& Other)
{
  m_ParamsForm = new TParamsForm(Other.m_ParamsForm);

  if (m_ParamsForm)
	m_ParamsForm->SetParamsFormDestructionCallback(ParamsFormDestructionCallback, reinterpret_cast<TGenericCockie>(this));
        
  m_SaveWhenApply = Other.m_SaveWhenApply;
  m_Caption = Other.m_Caption;
  m_OnApply = Other.m_OnApply;
  m_ApplyEnabled = Other.m_ApplyEnabled;
  m_ParamsFormDestroyed = Other.m_ParamsFormDestroyed;  
}

__fastcall TParamsDialog::~TParamsDialog()
{
    // Don't delete the form 'manually' when application is closing
    if(m_ParamsFormDestroyed)
       return;

    Q_SAFE_DELETE(m_ParamsForm);
}

// Add a container to the dialog layout
void TParamsDialog::AddParamsContainer(CQParamsContainer *ParamsContainer,int ExposureLevel, bool ShowLimits, CQStringList* HideTabs, bool passwordValid)
{
  m_ParamsForm->BuildDialogLayout(ParamsContainer,ExposureLevel, ShowLimits, HideTabs, passwordValid);
}

// Open the dialog
bool TParamsDialog::Execute(void)
{
  bool RetValue = false;

  //don't show parameter manager, checksum of BasicPMLevel.cfg is invalid
  //if(!m_ParamsForm->IsParametersChecksumValid())
  //	return RetValue; //Doron ugly fix for ALM defect 4174
  // Set dialog caption
  if(m_Caption != "")
    m_ParamsForm->Caption = m_Caption;

  // Set dialog features
  m_ParamsForm->SetDialogFeatures(m_CurrentButtonsMode,m_OnApply);  

  if (m_ApplyEnabled)
  {
     m_ParamsForm->ApplyAction->Enabled               = m_ApplyEnabled;
     m_ParamsForm->OkAction->Enabled                  = m_ApplyEnabled;
     m_ParamsForm->ChangeExposureLevelAction->Enabled = m_ApplyEnabled;
  }

  // Show the dialog
  if(m_ParamsForm->ShowModal() == mrOk)
    RetValue = true;

  return RetValue;
}

void __fastcall TParamsDialog::SetOnApplyEnabled(bool NewMode)
{
  m_ApplyEnabled = NewMode;
  if (NewMode == false)
  {
     m_ParamsForm->ApplyAction->Enabled               = NewMode;
     m_ParamsForm->OkAction->Enabled                  = NewMode;
     m_ParamsForm->ChangeExposureLevelAction->Enabled = NewMode;
  }
}


void __fastcall TParamsForm::ParamTabsListBoxClick(TObject *Sender)
{
  ActivateDesiredTab();
}
//---------------------------------------------------------------------------

void TParamsForm::ActivateDesiredTab()
{
  int TabIndex;
  AnsiString TabName;

  // Get the tab name
  TabIndex = ParamTabsListBox->ItemIndex;

  if(TabIndex >= 0)
  {
    TabName  = ParamTabsListBox->Items->Strings[TabIndex];

    // Activate the suitable tab
    PageControl1->ActivePageIndex = m_ParamsTabsList[TabName];
  }
}



void __fastcall TParamsForm::ChangeExposureLevelActionExecute(
      TObject *Sender)
{
  ParametersExposureLevelForm->Show();
  CloseAction->Execute();
}
//---------------------------------------------------------------------------


void __fastcall TParamsForm::LogParametersButtonClick(TObject *Sender)
{
  CBackEndInterface* BackEnd = CBackEndInterface::Instance();
  char DateTimeStr[32];
  time_t CurrentTime = time(NULL);
  struct tm *LocalTimeTM = localtime(&CurrentTime);
  strftime(DateTimeStr,32,"%d-%m-%y-%H-%M",LocalTimeTM);
  BackEnd->LogParams(QString("Configs\\") + DateTimeStr + ".cfg");
}
//---------------------------------------------------------------------------

void TParamsForm::SetParamsFormDestructionCallback(TParamsFormDestructionCallback ParamsFormDestructionCallback, TGenericCockie Cockie)
{
	m_ParamsFormDestructionCallback = ParamsFormDestructionCallback;
	m_Cockie                        = Cockie;
}

void TParamsDialog::ParamsFormDestructionCallback(bool ParamsFormDestroyed, TGenericCockie Cockie)
{
	TParamsDialog *ParamsDialog = reinterpret_cast<TParamsDialog *>(Cockie);
	ParamsDialog->m_ParamsFormDestroyed = ParamsFormDestroyed;
}
//---------------------------------------------------------------------------

