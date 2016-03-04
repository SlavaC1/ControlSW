//---------------------------------------------------------------------------

#ifndef MaterialWarningFormH
#define MaterialWarningFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <jpeg.hpp>
#include "Q2RTErrors.h"
#include <Buttons.hpp>

#define WM_SHOW_DIALOG          WM_USER

//---------------------------------------------------------------------------
class TMaterialWarningForm : public TForm
{
__published:	// IDE-managed Components
        TLabel *TextLabel;
        TButton *OKButton;
        void __fastcall FormShow(TObject *Sender);
        void __fastcall OKButtonClick(TObject *Sender);

private:	// User declarations
        MESSAGE void HandleShowDialogMessage(TMessage &Message);

public:		// User declarations
        __fastcall TMaterialWarningForm(TComponent* Owner);
        TQErrCode ShowDialog(int Value);

BEGIN_MESSAGE_MAP
  MESSAGE_HANDLER(WM_SHOW_DIALOG,TMessage,HandleShowDialogMessage);
END_MESSAGE_MAP(TForm);

};
//---------------------------------------------------------------------------
extern PACKAGE TMaterialWarningForm *MaterialWarningForm;
//---------------------------------------------------------------------------
#endif
