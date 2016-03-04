//---------------------------------------------------------------------------

#ifndef TrayDlgH
#define TrayDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TTrayInsertDlg : public TForm
{
__published:	// IDE-managed Components
        TTimer *Timer1;
        TLabel *Label1;
        TLabel *Label2;
        void __fastcall Timer1Timer(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TTrayInsertDlg(TComponent* Owner);
        void EnableDisableTimer(bool enable);
};
//---------------------------------------------------------------------------
extern PACKAGE TTrayInsertDlg *TrayInsertDlg;
//---------------------------------------------------------------------------
#endif
