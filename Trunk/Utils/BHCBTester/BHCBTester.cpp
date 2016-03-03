//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "BHCBTester.h"
#include "stdio.h"
#include <vector>
#pragma package(smart_init)
#pragma resource "*.dfm"
#include "QThreadUtils.h"

const int MAX_NUM_COM_PORTS = 10;

TBHCBTesterForm *BHCBTesterForm;

//---------------------------------------------------------------------------
__fastcall TBHCBTesterForm::TBHCBTesterForm(TComponent* Owner)
	: TForm(Owner)
{
	m_Reader = NULL;

	Application->OnException = AppException;

	EnumerateCommPorts();
}

void TBHCBTesterForm::EnumerateCommPorts()
{
	CQStdComPort ComPort;
	
	for(int ComNum = 1; ComNum <= MAX_NUM_COM_PORTS; ComNum++)
	{
		try
		{
			ComPort.Init(ComNum);
			ComPort.DeInit();
			CommPortComboBox->Items->Add("Com " + IntToStr(ComNum));
		}
		catch(...)
		{
			continue; // the current com port does not exist or in use
		}
	}

	CommPortComboBox->ItemIndex = 0;
}

//---------------------------------------------------------------------------

// Input only Hex values BYTE
void __fastcall TBHCBTesterForm::FilterByteKeyPress(TObject *Sender, char &Key)
{
	TEdit *Edit = dynamic_cast<TEdit *>(Sender);

	if(! Edit)
		return;

    if(Key == VK_ESCAPE || Key == VK_BACK || Key == VK_RETURN)
		return;
		
	if(Edit->Text.Length() > 1)
	{
		if(Edit->SelStart != 0)
		{
			Key = 0;
			return;
        }
	}

	if((Key >= '0') && (Key <= '9'))
		return;

	if((Key >= 'a') && (Key <= 'f'))
	{
		Key = UpCase(Key);
		return;
	}

	if((Key >= 'A') && (Key <= 'F'))
		return;

    Key = 0;	
}

// Input only Hex values WORD
void __fastcall TBHCBTesterForm::FilterWordKeyPress(TObject *Sender, char &Key)
{
	TEdit *Edit = dynamic_cast<TEdit *>(Sender);

	if(! Edit)
		return;

    if(Key == VK_ESCAPE || Key == VK_BACK || Key == VK_RETURN)
		return;
		
	if(Edit->Text.Length() > 3)
	{
		if(Edit->SelStart != 0)
		{
			Key = 0;
			return;
        }
	}

	if((Key >= '0') && (Key <= '9'))
		return;

	if((Key >= 'a') && (Key <= 'f'))
	{
		Key = UpCase(Key);
		return;
	}

	if((Key >= 'A') && (Key <= 'F'))
		return;

    Key = 0;	
}

void __fastcall TBHCBTesterForm::FPGARegLoadMacroButtonClick(TObject *Sender)
{
	if(FPGAMacroOpenDialog->Execute())
		FPGARegMacroMemo->Lines->LoadFromFile(FPGAMacroOpenDialog->FileName);
}
//---------------------------------------------------------------------------

void __fastcall TBHCBTesterForm::FPGARegSaveMacroButtonClick(TObject *Sender)
{
    if(FPGAMacroSaveDialog->Execute())
        FPGARegMacroMemo->Lines->SaveToFile(FPGAMacroSaveDialog->FileName);
}

void __fastcall TBHCBTesterForm::WriteFPGARegButtonClick(TObject *Sender)
{
	int Address = StrToInt("$" + FPGARegAddressEdit->Text);
	int Data    = StrToInt("$" + FPGARegDataEdit->Text);

	m_Reader->WriteToFPGA((WORD)Address, (WORD)Data);
	FPGARegDataEdit->Font->Color = clBlack;

	if(FPGARegRecordMacroCheckbox->Checked)
		FPGARegMacroMemo->Lines->Add(IntToHex(Address, 4) + " " + IntToHex(Data, 4));
}
//---------------------------------------------------------------------------

void __fastcall TBHCBTesterForm::ReadFPGARegButtonClick(TObject *Sender)
{
	WORD data = m_Reader->ReadFromFPGA(StrToInt("$" + FPGARegAddressEdit->Text));
	FPGARegDataEdit->Text = IntToHex(data, 4);
    FPGARegDataEdit->Font->Color = clMaroon;
}
//---------------------------------------------------------------------------

void __fastcall TBHCBTesterForm::FPGARegMacroClearButtonClick(TObject *Sender)
{
	FPGARegMacroMemo->Clear();
}
//---------------------------------------------------------------------------

void __fastcall TBHCBTesterForm::FPGARegExecuteMacroButtonClick(TObject *Sender)
{
	DWORD Adr, Data;

	for(int i = 0; i < FPGARegMacroMemo->Lines->Count; i++)
	{
		if(sscanf(FPGARegMacroMemo->Lines->Strings[i].c_str(), "%x %x", &Adr, &Data) != 2)
		{
			MessageDlg("Invalid macro format", mtError, TMsgDlgButtons() << mbOK,0);
			return;
		}

		m_Reader->WriteToFPGA((WORD)Adr, (WORD)Data);

		if(WriteDelayCheckBox->Checked)
			QSleep(WriteDelayEdit->Text.ToInt());

		Application->ProcessMessages();
	}
}

void __fastcall TBHCBTesterForm::ConnectButtonClick(TObject *Sender)
{
	TButton *btn = dynamic_cast<TButton *>(Sender);
	if(! btn)
		return;
		
	if(CommPortComboBox->Text == "")
		return;

	int	ComNum = StrToInt(CommPortComboBox->Text.SubString(5, 1));

	m_Reader = new CReader(ComNum);

	CommPortComboBox->Enabled = false;
	btn->Enabled              = false;
}

void __fastcall TBHCBTesterForm::FormDestroy(TObject *Sender)
{ 
	Q_SAFE_DELETE(m_Reader);
}

void __fastcall TBHCBTesterForm::AppException(TObject *Sender, Exception *E)
{
	Application->ShowException(E);
}


