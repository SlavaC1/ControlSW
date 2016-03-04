//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ParamsDlgTestMainUnit.h"
#include "ParamsDialog.h"
#include "QParamsContainer.h"
#include "QFileParamsStream.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"


class CAppParams : public CQParamsContainer {
  public:
    DEFINE_PARAM(int,Temperature);
    DEFINE_PARAM(float,ModelHeight);
    DEFINE_PARAM(bool,OffLineMode);
    DEFINE_PARAM(QString,Test);
    DEFINE_ARRAY_PARAM(int,4,MyArray);
    DEFINE_ENUM_PARAM(ScatterAlgorithm);

    CAppParams(CQParamsStream *ParamsStream,const QString& Name) : CQParamsContainer(ParamsStream,Name) {
      INIT_PARAM(Temperature,85,"Temperature");
      Temperature.SetAttributes(paNoSave);

      INIT_PARAM_WITH_LIMITS(ModelHeight,10.6f,5.0f,20.0f,"Test");
      INIT_PARAM(ModelHeight,10.6f,"Test");
      ModelHeight.SetAttributes(paLimitsException);

      INIT_PARAM(OffLineMode,false,"Options");
      OffLineMode.SetAttributes(paNoSave);

      INIT_PARAM(Test,"Ran","Options");

      INIT_PARAM(MyArray,"2,5,10,8","Test");
      MyArray.SetDescription("This is a test array");
      MyArray.SetCellDescription(1,"Ran peleg");
      MyArray.SetCellDescription(3,"Nir");

      INIT_PARAM_WITH_LIMITS(ScatterAlgorithm,3,0,3,"Process");
      ScatterAlgorithm.SetDescription("0 - Sequential Order, 1 - Random Order, 2 - Smart Order, 3 - File scatter");
      ScatterAlgorithm.AddValue("Sequential Order",0);
      ScatterAlgorithm.AddValue("Random Order",1);
      ScatterAlgorithm.AddValue("Smart Order",2);
      ScatterAlgorithm.AddValue("Smart Random",3);
    }

    ~CAppParams() {
      SaveAll();
    }
};


CQParamsFileStream CfgFile("Test1.cfg");
CAppParams MyParams(&CfgFile,"ConfigParams");



TMainForm *MainForm;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormCreate(TObject *Sender)
{
  MyParams.LoadAll();

  m_ParamsDlg = new TParamsDialog;
  m_ParamsDlg->AddParamsContainer(&MyParams);

  MyParams.Temperature.AddObserver(MyObserver1);
  MyParams.Temperature.AddObserver(MyObserver2);
  MyParams.Temperature.AddGenericObserver(MyGenericObserver);
  
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormDestroy(TObject *Sender)
{
  MyParams.SaveAll();
  delete m_ParamsDlg;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Button1Click(TObject *Sender)
{
  if(RadioGroup1->ItemIndex == 0)
    m_ParamsDlg->ButtonsMode = pbOkCancel;
  else
    m_ParamsDlg->ButtonsMode = pbApplyClose;

  m_ParamsDlg->Execute();
}
//---------------------------------------------------------------------------

void TMainForm::MyObserver1(const int& NewTemperature,TGenericCockie Cockie)
{
  ShowMessage(NewTemperature);
}

void TMainForm::MyObserver2(const int& NewTemperature,TGenericCockie Cockie)
{
  ShowMessage(NewTemperature);
}

void TMainForm::MyGenericObserver(CQParameterBase *Param,TGenericCockie Cockie)
{
  ShowMessage(Param->ValueAsString().c_str());
}
