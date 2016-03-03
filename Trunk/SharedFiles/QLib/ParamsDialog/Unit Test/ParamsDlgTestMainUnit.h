//---------------------------------------------------------------------------

#ifndef ParamsDlgTestMainUnitH
#define ParamsDlgTestMainUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "QTypes.h"

class TParamsDialog;
class CQParameterBase;

//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
        TButton *Button1;
        TRadioGroup *RadioGroup1;
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall Button1Click(TObject *Sender);
private:	// User declarations
  TParamsDialog *m_ParamsDlg;

  static void MyObserver1(const int& NewTemperature,TGenericCockie Cockie);
  static void MyObserver2(const int& NewTemperature,TGenericCockie Cockie);

  static void MyGenericObserver(CQParameterBase *Param,TGenericCockie Cockie);


public:		// User declarations
        __fastcall TMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
