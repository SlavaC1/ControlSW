//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "PCIOptionsDlg.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#include <stdio.h>
#include "QMonitor.h"
#include "BackEndInterface.h"
#include "EdenPCISys.h"

TPCIOptionsForm *PCIOptionsForm;

//---------------------------------------------------------------------------
__fastcall TPCIOptionsForm::TPCIOptionsForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TPCIOptionsForm::FormCreate(TObject *Sender)
{
  m_BackEndInterface = CBackEndInterface::Instance();
}
//---------------------------------------------------------------------------

void __fastcall TPCIOptionsForm::Button3Click(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------

DWORD TPCIOptionsForm::GetBinaryNumber(void)
{
  DWORD Result;

  for(int i=0; i < BinaryNumberPanel->ControlCount; i++)
  {
    TCheckBox *TmpCheckbox = dynamic_cast<TCheckBox *>(BinaryNumberPanel->Controls[i]);

    if(TmpCheckbox != NULL)
      if(TmpCheckbox->Checked)
        Result += 1 << TmpCheckbox->Tag;
  }

  return Result;
}

void TPCIOptionsForm::SetBinaryNumber(DWORD Number)
{
  for(int i = 0; i < 32; i++)
  {
    TCheckBox *TmpCheckbox;

    for(int j=0; j < BinaryNumberPanel->ControlCount; j++)
    {
      TmpCheckbox = dynamic_cast<TCheckBox *>(BinaryNumberPanel->Controls[j]);

      if(TmpCheckbox != NULL)
        if(TmpCheckbox->Tag == i)
          break;
    }

    if(TmpCheckbox)
      TmpCheckbox->Checked = (bool)((1 << i) & Number);
  }
}

void __fastcall TPCIOptionsForm::CheckBox17Click(TObject *Sender)
{
  DataEdit->Text = IntToHex((int)GetBinaryNumber(),8);
}
//---------------------------------------------------------------------------

void __fastcall TPCIOptionsForm::Button2Click(TObject *Sender)
{
  DWORD Data = StrToInt("$" + DataEdit->Text);
  DWORD Address = StrToInt("$" + AddrressEdit->Text);
  int BarNum = BarRadioGroup1->ItemIndex;

  SetBinaryNumber(Data);

  try
  {
    m_BackEndInterface->PCIWriteDWORD(BarNum,Address,Data);

    if(RecordCheckBox->Checked)
      AddToScript(BarNum,Address,Data);

  } catch(EQException& E)
    {
      QMonitor.ErrorMessage(E.GetErrorMsg());
    }
}
//---------------------------------------------------------------------------

void __fastcall TPCIOptionsForm::Button1Click(TObject *Sender)
{
  try
  {
    DWORD Value = m_BackEndInterface->PCIReadDWORD(BarRadioGroup1->ItemIndex,StrToInt("$" + AddrressEdit->Text));
    SetBinaryNumber(Value);    
  } catch(EQException& E)
    {
      QMonitor.ErrorMessage(E.GetErrorMsg());
    }
}
//---------------------------------------------------------------------------

void __fastcall TPCIOptionsForm::Button4Click(TObject *Sender)
{
  if(OpenDialog1->Execute())
    IOWriteMemo->Lines->LoadFromFile(OpenDialog1->FileName);
}
//---------------------------------------------------------------------------

void __fastcall TPCIOptionsForm::Button5Click(TObject *Sender)
{
  if(SaveDialog1->Execute())
    IOWriteMemo->Lines->SaveToFile(SaveDialog1->FileName);
}
//---------------------------------------------------------------------------

void __fastcall TPCIOptionsForm::ExecuteButtonClick(TObject *Sender)
{
  int BarNum;
  DWORD Address,Data;

  for(int i = 0; i < IOWriteMemo->Lines->Count; i++)
  {
    if(sscanf(IOWriteMemo->Lines->Strings[i].c_str(),"%x,%lx,%lx",&BarNum,&Address,&Data) != 3)
    {
      MessageDlg("Invalid script format",mtError,TMsgDlgButtons() << mbOK,0);
      return;
    }

    m_BackEndInterface->PCIWriteDWORD(BarNum,Address,Data);
  }
}
//---------------------------------------------------------------------------

void TPCIOptionsForm::AddToScript(int BarNum,DWORD Address,DWORD Data)
{
  IOWriteMemo->Lines->Add(IntToStr(BarNum) + "," + IntToHex((int)Address,8) + "," + IntToHex((int)Data,8));
}

void __fastcall TPCIOptionsForm::FormClose(TObject *Sender, TCloseAction &Action)
{
//  EdenPCI_FIFOSetReadMode(FF_READ_SERIAL);
}
//---------------------------------------------------------------------------

void __fastcall TPCIOptionsForm::FormShow(TObject *Sender)
{
//  EdenPCI_FIFOSetReadMode(FF_READ_MANUAL);        
}
//---------------------------------------------------------------------------

