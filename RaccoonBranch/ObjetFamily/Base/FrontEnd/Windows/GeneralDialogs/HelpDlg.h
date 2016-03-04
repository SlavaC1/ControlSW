//---------------------------------------------------------------------------

#ifndef HelpDlgH
#define HelpDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "SHDocVw_OCX.h"
#include <OleCtrls.hpp>
//---------------------------------------------------------------------------
class TShowHelpForm : public TForm
{
__published:	// IDE-managed Components
        TCppWebBrowser *HelpWebBrowser;
private:	// User declarations
public:		// User declarations
        __fastcall TShowHelpForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TShowHelpForm *ShowHelpForm;
//---------------------------------------------------------------------------
#endif
