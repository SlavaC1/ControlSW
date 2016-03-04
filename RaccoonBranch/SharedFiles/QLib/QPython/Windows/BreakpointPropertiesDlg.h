//---------------------------------------------------------------------------

#ifndef BreakpointPropertiesDlgH
#define BreakpointPropertiesDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TBreakpointPropertiesForm : public TForm
{
__published:	// IDE-managed Components
        TLabeledEdit *BrkNameEdit;
        TLabeledEdit *BrkConEdit;
        TBevel *Bevel1;
        TBitBtn *BitBtn1;
        TBitBtn *BitBtn2;
private:	// User declarations
public:		// User declarations
        __fastcall TBreakpointPropertiesForm(TComponent* Owner);

  void SetBreakpointProps(const AnsiString BrkName,const AnsiString BrkCon);
  void GetBreakpointProps(AnsiString& BrkName,AnsiString& BrkCon);
};
//---------------------------------------------------------------------------
extern PACKAGE TBreakpointPropertiesForm *BreakpointPropertiesForm;
//---------------------------------------------------------------------------
#endif
