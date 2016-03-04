//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "OHDBOptionsDlg.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#include <stdio.h>
#include "QMonitor.h"
#include "BackEndInterface.h"
#include "EdenPCISys.h"

TOHDBOptionsForm *OHDBOptionsForm;

//---------------------------------------------------------------------------
__fastcall TOHDBOptionsForm::TOHDBOptionsForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TOHDBOptionsForm::FormCreate(TObject *Sender)
{
  m_BackEndInterface = CBackEndInterface::Instance();
}
//---------------------------------------------------------------------------

void __fastcall TOHDBOptionsForm::Button3Click(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TOHDBOptionsForm::Button2Click(TObject *Sender)
{
  BYTE Address;
  WORD Data;

  try
  {
    Address = (BYTE) StrToInt("$"+XilinxAddressEdit->Text);
    Data = (WORD) StrToInt("$"+XilinxDataEdit->Text);
    if (MacroRecordCheckBox->Checked)
      AddToMacroRecorder(Address, Data);
  }
  catch (Exception &exception)
  {
    MessageDlg("The value entered is not valid",mtError,TMsgDlgButtons() << mbOK,0);
  }

  try
  {
    m_BackEndInterface->WriteDataToOHDBXilinx(Address,Data);
  } catch(EQException& E)
    {
      QMonitor.ErrorMessage(E.GetErrorMsg());
    }
}
//---------------------------------------------------------------------------

void __fastcall TOHDBOptionsForm::Button4Click(TObject *Sender)
{
  if(OpenDialog1->Execute())
    MacroMemo->Lines->LoadFromFile(OpenDialog1->FileName);
}
//---------------------------------------------------------------------------

void __fastcall TOHDBOptionsForm::Button5Click(TObject *Sender)
{
  if(SaveDialog1->Execute())
    MacroMemo->Lines->SaveToFile(SaveDialog1->FileName);
}
//---------------------------------------------------------------------------

void __fastcall TOHDBOptionsForm::ExecuteButtonClick(TObject *Sender)
{
  DWORD Adr;
  DWORD Data;

  for(int i = 0; i < MacroMemo->Lines->Count; i++)
  {

    if(sscanf(MacroMemo->Lines->Strings[i].c_str(),"%x %x",&Adr,&Data) != 2)
    {
      MessageDlg("Invalid macro format",mtError,TMsgDlgButtons() << mbOK,0);
      return;
    }
    m_BackEndInterface->WriteDataToOHDBXilinx((BYTE)Adr,(WORD)Data);
  }
}
//---------------------------------------------------------------------------

void TOHDBOptionsForm::AddToMacroRecorder(BYTE Address, WORD Data)
{
  MacroMemo->Lines->Add(IntToHex(Address,2) + " " + IntToHex(Data,4));
}

void __fastcall TOHDBOptionsForm::Button1Click(TObject *Sender)
{
  BYTE Address = (BYTE) StrToInt("$"+XilinxAddressEdit->Text);

  try
  {
    XilinxDataEdit->Text = IntToHex(m_BackEndInterface->ReadDataFromOHDBXilinx(Address),4);
  } catch(EQException& E)
    {
      QMonitor.ErrorMessage(E.GetErrorMsg());
    }
}
//---------------------------------------------------------------------------

