//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <Dialogs.hpp>
#include <Menus.hpp>
#include <ActnList.hpp>
#include <ActnMan.hpp>
#include <StdActns.hpp>

//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
        TMemo *Memo1;
        TOpenDialog *OpenDialog1;
        TMainMenu *MainMenu1;
        TMenuItem *File1;
        TMenuItem *Open1;
        TMenuItem *Exit1;
        TMenuItem *Edit1;
        TMenuItem *Find1;
        TMenuItem *Help1;
        TMenuItem *About1;
        TActionManager *ActionManager1;
        TSearchFindNext *SearchFindNext1;
        TSearchFind *SearchFind1;
        TMenuItem *FindNext1;
        void __fastcall Open1Click(TObject *Sender);
        void __fastcall Exit1Click(TObject *Sender);
        void __fastcall About1Click(TObject *Sender);
        void __fastcall SearchFind1FindDialogFind(TObject *Sender);
private:	// User declarations
 FILE *m_LogFile;
 long m_key[4];
 AnsiString buff;
 
public:		// User declarations
        __fastcall TForm1(TComponent* Owner);
        void Decode(long* v);
        void ReadFile();
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
