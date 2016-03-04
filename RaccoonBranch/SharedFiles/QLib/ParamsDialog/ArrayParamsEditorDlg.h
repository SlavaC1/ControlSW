//---------------------------------------------------------------------------

#ifndef ArrayParamsEditorDlgH
#define ArrayParamsEditorDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <Grids.hpp>
//---------------------------------------------------------------------------

#include "QTypes.h"

class CQArrayParameterBase;

class TArrayParamsEditorForm : public TForm
{
__published:	// IDE-managed Components
        TStringGrid *StringGrid1;
        TBitBtn *OkButton;
        TBitBtn *CancelButton;
        void __fastcall FormShow(TObject *Sender);
        void __fastcall OkButtonClick(TObject *Sender);
        void __fastcall StringGrid1SelectCell(TObject *Sender, int ACol,
          int ARow, bool &CanSelect);
private:	// User declarations

  CQArrayParameterBase *m_Param;
  QString m_CurrentValue;

  AnsiString m_ResultStr;

  void SetParameter(CQArrayParameterBase *Param,AnsiString CurrentValue);

  // Update the string list component
  void UpdateStringList(void);

  // Count how many cell are in an array string
  int CountArrayCells(QString Str);

public:		// User declarations
        __fastcall TArrayParamsEditorForm(TComponent* Owner);

  // Open the array parameters editor with a given parameter
  static bool Execute(CQArrayParameterBase *Param,AnsiString CurrentValue,AnsiString& ResultStr);
};

#endif
