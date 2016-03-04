//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MainUnit.h"
#include "ParamsLevelExposure.h"
#include "AppParams.h"
#include "QThreadUtils.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"


TParametersExposureLevelForm *ParametersExposureLevelForm;
//---------------------------------------------------------------------------
__fastcall TParametersExposureLevelForm::TParametersExposureLevelForm(TComponent* Owner)
        : TForm(Owner)
{}
//---------------------------------------------------------------------------
void __fastcall TParametersExposureLevelForm::CancelButtonClick(TObject *Sender)
{

  PasswordMaskEdit->Clear(); 
  Close();        
}
//---------------------------------------------------------------------------
void __fastcall TParametersExposureLevelForm::OkButtonClick(TObject *Sender)
{

  time_t rawtime;
  struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  int res = timeinfo->tm_mday*timeinfo->tm_mday+(timeinfo->tm_mon+1)*(timeinfo->tm_mon+1)+timeinfo->tm_year;
  QString str = "EMPM" + QIntToStr(res);
  if(strcmp(str.c_str(),PasswordMaskEdit->Text.c_str())==0)
  {
	  PasswordMaskEdit->Clear();
	  IncorrectPass->Visible = false;
	  MainForm->SetCurrentParamLevel(SUPER_USER_LEVEL);
	  MainForm->SetValidPassword(true);
	  Close();
	  MainForm->OpenParamsDialogActionExecute(Sender);
  }
  else
  {
	  IncorrectPass->Visible = true;
	  MainForm->SetCurrentParamLevel(SERVICE_LEVEL);
	  MainForm->SetValidPassword(false);
  }

}
//---------------------------------------------------------------------------
void __fastcall TParametersExposureLevelForm::FormShow(TObject *Sender)
{
  TRadioButton *MyRadio;
  int CurrentLevel = -1;

  CurrentLevel = MainForm->GetCurrentParamLevel();

  for (int i = 0; i < ChooseUserGroupBox->ControlCount; i++)
  {
    MyRadio = dynamic_cast<TRadioButton *>(ChooseUserGroupBox->Controls[i]);
    if (MyRadio != NULL)
      if(MyRadio->Tag == CurrentLevel)
      {
        MyRadio->Checked = true;
        break;
      }
  }
  Timer1->Enabled = true;

  IncorrectPass->Visible = false;  
}
//---------------------------------------------------------------------------
void __fastcall TParametersExposureLevelForm::Timer1Timer(TObject *Sender)
{
  while( GetTopWindow(NULL) != this->Handle )
  {
     Timer1->Enabled = false;
     SetWindowPos(this->Handle,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
  }
}
//---------------------------------------------------------------------------

void __fastcall TParametersExposureLevelForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
  Timer1->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TParametersExposureLevelForm::FormPaint(TObject *Sender)
{
  Timer1->Enabled = false;
}
//---------------------------------------------------------------------------

