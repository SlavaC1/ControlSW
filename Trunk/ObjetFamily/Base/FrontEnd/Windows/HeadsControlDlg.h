//---------------------------------------------------------------------------

#ifndef HeadsControlDlgH
#define HeadsControlDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "EdenHeadsFrame.h"
#include <ActnList.hpp>
#include <Menus.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class THeadsControlForm : public TForm
{
__published:	// IDE-managed Components
  THeadsFrame *HeadsFrame;
        TActionList *ActionList1;
        TAction *AlwaysOnTopAction;
        TAction *CloseDialogAction;
        TPopupMenu *PopupMenu1;
        TMenuItem *Alwaysontop1;
        TMenuItem *N1;
        TMenuItem *Close1;
  void __fastcall FormShow(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall AlwaysOnTopActionExecute(TObject *Sender);
        void __fastcall CloseDialogActionExecute(TObject *Sender);
        void __fastcall FormPaint(TObject *Sender);
private:	// User declarations
public:		// User declarations
  __fastcall THeadsControlForm(TComponent* Owner);

  THeadsFrame* GetUIFrame();

};
//---------------------------------------------------------------------------
extern PACKAGE THeadsControlForm *HeadsControlForm;
//---------------------------------------------------------------------------
#endif
