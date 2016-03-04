/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib related utility.                                   *
 * Module: QLib Mini Debugger                                       *
 * Module Description: This module implement a run-time objects     *
 *                     inspector / debugger. It can show in run time*
 *                     all the registered QComponent objects and    *
 *                     their associated methods and properties.     *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 22/07/2001                                           *
 * Last upate: 02/08/2001                                           *
 ********************************************************************/

#include <vcl.h>
#pragma hdrstop

#include "MiniDebugger.h"

#include "QComponent.h"
#include "QObjectsRoster.h"
#include "QUtils.h"
#include "QTypes.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TMiniDebuggerForm *MiniDebuggerForm;

__fastcall TMiniDebuggerForm::TMiniDebuggerForm(TComponent* Owner)
        : TForm(Owner)
{
}

// Refresh the display (return the current selected item or -1 if no objects found)
int TMiniDebuggerForm::RefreshObjectsView(void)
{
  // Remember the total amount of items and the currently selected item
  int CurrentItem = ObjectsListBox->ItemIndex;
  int TotalItemsCount = ObjectsListBox->Items->Count;

  ObjectsListBox->Items->Clear();

  // Get a pointer to the objects roster
  CQObjectsRoster *Roster = CQObjectsRoster::Instance();

  if(Roster == NULL)
    return -1;

  // Add all the registered components to the list
  for(TQComponentList::iterator i = Roster->GetComponentList()->begin(); i != Roster->GetComponentList()->end(); i++)
    ObjectsListBox->Items->Add((*i)->Name().c_str());

  // If the number has changed, set the selected item on the first item
  if(TotalItemsCount != ObjectsListBox->Items->Count)
    ObjectsListBox->ItemIndex = 0;
  else
    ObjectsListBox->ItemIndex = CurrentItem;

  return ObjectsListBox->ItemIndex;
}

void __fastcall TMiniDebuggerForm::RefreshButtonClick(TObject *Sender)
{
  int CurrentItem;

  // Refresh objects view and reposition the marker on the current object
  if((CurrentItem = RefreshObjectsView()) != -1)
    UpdateSpecificObjectView(CurrentItem);
}

void __fastcall TMiniDebuggerForm::FormCreate(TObject *Sender)
{
  m_MiniSequencer = new CMiniSequencer;
}

void __fastcall TMiniDebuggerForm::FormDestroy(TObject *Sender)
{
  Q_SAFE_DELETE(m_MiniSequencer);
}

void __fastcall TMiniDebuggerForm::EvaluateButtonClick(TObject *Sender)
{
  try {
    int CurrentItem;

    ResultEdit->Text = m_MiniSequencer->EvaluateExpression(ExpressionEdit->Text.c_str()).c_str();

    // Refresh objects view and reposition the marker on the current object
    if((CurrentItem = RefreshObjectsView()) != -1)
      UpdateSpecificObjectView(CurrentItem);

  } catch(EQException &e) {
      ShowMessage(e.GetErrorMsg().c_str());
    }
}

void __fastcall TMiniDebuggerForm::FormShow(TObject *Sender)
{
  int CurrentItem = RefreshObjectsView();

  // On show, update item 0
  if(CurrentItem != -1)
    UpdateSpecificObjectView(CurrentItem);
}

void __fastcall TMiniDebuggerForm::Button1Click(TObject *Sender)
{
  Close();
}

void __fastcall TMiniDebuggerForm::ObjectsListBoxClick(TObject *Sender)
{
  int CurrentItem;
  
  if((CurrentItem = RefreshObjectsView()) != -1)
    UpdateSpecificObjectView(CurrentItem);
}

// Convert an integer to QString
QString FixStrangeCompilerBug(void)
{
  TStrStream StrStream;

  StrStream << 0;

  return StrStream.str();
}


void TMiniDebuggerForm::UpdateSpecificObjectView(int ObjectIndex)
{
  // Get a pointer to the objects roster
  CQObjectsRoster *Roster = CQObjectsRoster::Instance();

  if(Roster == NULL)
    return;

  // Update the methods and properties lists
  CQComponent *Component = Roster->FindComponent(ObjectIndex);

  int i;

  MethodsListBox->Items->Clear();
  ArgsNumListBox->Items->Clear();

  for(i = 0; i < Component->MethodCount(); i++)
  {
    int n = Component->Methods[i]->GetMethodArgumentsCount();
    QString ArgNum = QIntToStr(n);
    ArgsNumListBox->Items->Add(ArgNum.c_str());
    MethodsListBox->Items->Add(Component->Methods[i]->Name().c_str());
  }

  PropertiesListBox->Items->Clear();
  ValuesListBox->Items->Clear();

  for(i = 0; i < Component->PropertyCount(); i++) {
    PropertiesListBox->Items->Add(Component->Properties[i]->Name().c_str());
    ValuesListBox->Items->Add(Component->Properties[i]->ValueAsString().c_str());
  }
}
void __fastcall TMiniDebuggerForm::ExpressionEditDragOver(TObject *Sender,
      TObject *Source, int X, int Y, TDragState State, bool &Accept)
{
  // Enable dragging only if the priodic refresh is false
  Accept = !PeriodicRefreshCheckBox->Checked;
}

void __fastcall TMiniDebuggerForm::ExpressionEditDragDrop(TObject *Sender,
      TObject *Source, int X, int Y)
{
  TListBox *MyListBox = dynamic_cast<TListBox *>(Source);
  AnsiString s;

  // Just for safety (this should not happened)
  if(ObjectsListBox->ItemIndex == -1)
    return;

  s = ObjectsListBox->Items->Strings[ObjectsListBox->ItemIndex] + "." + MyListBox->Items->Strings[MyListBox->ItemIndex];

  // If the dragged item is a method with no parameters add '()' to the string
  if(MyListBox->Tag == 1) {
    if(ArgsNumListBox->Items->Strings[MyListBox->ItemIndex] == "0")
      s += "()";
  }

  ExpressionEdit->Text = s;
}

void __fastcall TMiniDebuggerForm::PeriodicRefreshCheckBoxClick(
      TObject *Sender)
{
  RefreshTimer->Enabled = PeriodicRefreshCheckBox->Checked;
}

void __fastcall TMiniDebuggerForm::RefreshTimerTimer(TObject *Sender)
{
  int CurrentItem;

  // Refresh objects view and reposition the marker on the current object
  if((CurrentItem = RefreshObjectsView()) != -1)
    UpdateSpecificObjectView(CurrentItem);
}

void __fastcall TMiniDebuggerForm::SampleRateMenuItemClick(TObject *Sender)
{
  // The sender is a menu item
  TMenuItem *MenuItem = dynamic_cast<TMenuItem *>(Sender);

  // Set the sender to be checked
  MenuItem->Checked = true;

  // Set the timer period according to the sender Tag property
  RefreshTimer->Interval = MenuItem->Tag;
}

void __fastcall TMiniDebuggerForm::EvaluateScriptButtonClick(
      TObject *Sender)
{
  if(OpenDialog1->Execute())
  {
    CQStringList *SList = new CQStringList(OpenDialog1->FileName.c_str());

    m_MiniSequencer->ExecuteSequence(SList,true);
  }
}
//---------------------------------------------------------------------------

void __fastcall TMiniDebuggerForm::MethodsListBoxDblClick(TObject *Sender)
{
  AnsiString s;

  // Just for safety (this should not happened)
  if(ObjectsListBox->ItemIndex == -1)
    return;

  s = ObjectsListBox->Items->Strings[ObjectsListBox->ItemIndex] + "." + MethodsListBox->Items->Strings[MethodsListBox->ItemIndex];

  if(ArgsNumListBox->Items->Strings[MethodsListBox->ItemIndex] == "0")
    s += "()";

  ExpressionEdit->Text = s;
}
//---------------------------------------------------------------------------

