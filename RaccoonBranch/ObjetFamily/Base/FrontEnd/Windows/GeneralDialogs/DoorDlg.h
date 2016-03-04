//---------------------------------------------------------------------------

#ifndef DoorDlgH
#define DoorDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TDoorCloseDlg : public TForm
{
__published:	// IDE-managed Components
        TLabel *Label1;
        TBitBtn *DoorCancelBitBtn1;
        TLabel *Label2;
        TLabel *Label3;
        void __fastcall DoorCancelBitBtn1Click(TObject *Sender);
private:	// User declarations

public:		// User declarations
        __fastcall TDoorCloseDlg(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TDoorCloseDlg *DoorCloseDlg;
//---------------------------------------------------------------------------
#endif
