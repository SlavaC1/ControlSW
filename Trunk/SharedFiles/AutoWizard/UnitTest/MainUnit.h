//---------------------------------------------------------------------------

#ifndef MainUnitH
#define MainUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "WizardViewer.h"
#include <ImgList.hpp>


class CQApplication;

//---------------------------------------------------------------------------
class TTestForm : public TForm
{
__published:	// IDE-managed Components
        TButton *Button2;
        TImageList *ImageList1;
        TLabel *Label1;
        TButton *Button1;
        TEdit *WizardNameEdit;
        TButton *Button3;
        void __fastcall Button2Click(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall Button3Click(TObject *Sender);
private:	// User declarations

  CQApplication *m_WizardUnitTest;

public:		// User declarations
        __fastcall TTestForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TTestForm *TestForm;
//---------------------------------------------------------------------------
#endif
