//---------------------------------------------------------------------------

#ifndef ContinousWriteDlgH
#define ContinousWriteDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TContinousWriteForm : public TForm
{
__published:	// IDE-managed Components
        TButton *StopButton;
        TLabel *Label1;
        TTimer *Timer1;
        void __fastcall StopButtonClick(TObject *Sender);
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
private:	// User declarations
  bool m_StopFlag;

public:		// User declarations
        __fastcall TContinousWriteForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TContinousWriteForm *ContinousWriteForm;
//---------------------------------------------------------------------------
#endif
