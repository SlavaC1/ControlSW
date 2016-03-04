//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "QPythonTestMainUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#include "QApplication.h"
#include "QMonitor.h"

#include "QPythonIntegrator.h"
#include "PythonConsole.h"
#include "QException.h"


class CMyComponent : public CQComponent {
  public:

    DEFINE_VAR_PROPERTY(int,Ran);
    DEFINE_METHOD_1(CMyComponent,int,f1,int);
    DEFINE_METHOD_1(CMyComponent,int,f2,int);
    DEFINE_METHOD_1(CMyComponent,int,f3,int);

    CMyComponent(const QString& Name) : CQComponent(Name) {
      INIT_VAR_PROPERTY(Ran,15);
      INIT_METHOD(CMyComponent,f1);
      INIT_METHOD(CMyComponent,f2);
      INIT_METHOD(CMyComponent,f3);
    }
};

int CMyComponent::f1(int p)
{
  return (p + 10);
}

int CMyComponent::f2(int p)
{
  return (p * 2);
}

int CMyComponent::f3(int p)
{
  return (p + 1);
}

CMyComponent AObject("AObject"),BObject("BObject");


TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormCreate(TObject *Sender)
{
  m_App = new CQApplication;
  m_App->Init();

  CQObjectsRoster *Roster = CQObjectsRoster::Instance();
  Roster->RegisterComponent(&AObject);
  Roster->RegisterComponent(&BObject);

  TPythonConsoleForm::SetDefaultString("from QLib import *");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormDestroy(TObject *Sender)
{
  m_App->Stop();
  delete m_App;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormShow(TObject *Sender)
{
  m_App->Start();

  CQObjectsRoster::Instance()->RegisterComponent(&QMonitor);

  QPythonIntegratorDM->Init();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button1Click(TObject *Sender)
{
  TPythonConsoleForm::CreateNewConsoleWindow();
}
//---------------------------------------------------------------------------

