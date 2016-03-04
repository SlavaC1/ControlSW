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

#ifndef MiniDebuggerH
#define MiniDebuggerH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

#include "MiniSequencer.h"
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>

//---------------------------------------------------------------------------
class TMiniDebuggerForm : public TForm
{
__published:	// IDE-managed Components
        TButton *Button1;
        TListBox *ObjectsListBox;
        TLabel *Label1;
        TListBox *MethodsListBox;
        TLabel *Label2;
        TListBox *PropertiesListBox;
        TLabel *Label3;
        TGroupBox *GroupBox1;
        TLabel *Label4;
        TEdit *ExpressionEdit;
        TEdit *ResultEdit;
        TLabel *Label5;
        TButton *RefreshButton;
        TButton *EvaluateButton;
        TListBox *ValuesListBox;
        TCheckBox *PeriodicRefreshCheckBox;
        TTimer *RefreshTimer;
        TListBox *ArgsNumListBox;
        TPopupMenu *SampleRatePopupMenu;
        TMenuItem *Every250ms1;
        TMenuItem *Every500ms1;
        TMenuItem *Every1sec1;
        TMenuItem *Every2Sec1;
        TMenuItem *Every4sec1;
        TButton *EvaluateScriptButton;
        TOpenDialog *OpenDialog1;
        void __fastcall RefreshButtonClick(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall EvaluateButtonClick(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall ObjectsListBoxClick(TObject *Sender);
        void __fastcall ExpressionEditDragOver(TObject *Sender,
          TObject *Source, int X, int Y, TDragState State, bool &Accept);
        void __fastcall ExpressionEditDragDrop(TObject *Sender,
          TObject *Source, int X, int Y);
        void __fastcall PeriodicRefreshCheckBoxClick(TObject *Sender);
        void __fastcall RefreshTimerTimer(TObject *Sender);
        void __fastcall SampleRateMenuItemClick(TObject *Sender);
        void __fastcall EvaluateScriptButtonClick(TObject *Sender);
        void __fastcall MethodsListBoxDblClick(TObject *Sender);
private:	// User declarations

  CMiniSequencer *m_MiniSequencer;

  // Refresh the display
  int RefreshObjectsView(void);

  void UpdateSpecificObjectView(int ObjectIndex);

public:		// User declarations
        __fastcall TMiniDebuggerForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMiniDebuggerForm *MiniDebuggerForm;
//---------------------------------------------------------------------------
#endif
