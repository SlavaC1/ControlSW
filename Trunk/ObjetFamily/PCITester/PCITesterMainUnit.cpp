//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <stdio.h>
#include "PCITesterMainUnit.h"
#include "EdenPCI.h"
#include "EdenPCISys.h"
#include "FIFOPCI.h"
#include "ContinousWriteDlg.h"
#include "HelpDlg.h"
#include "QUtils.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

const int F_LED    = 0;
const int AFF_LED  = 1;
const int HFAF_LED = 2;
const int AEHF_LED = 3;
const int EAE_LED  = 4;
const int E_LED    = 5;

//const int FIFO_WRITE_REG = 0x100;
//const int FIFO_GO_REG    = 0x1C;
const int FIFO_INTR_TEST_BLOCK_SIZE = 1024;
const int FIFO_INTR_TEST_BLOCKS_NUM = 16;


// Exception class for the name parameter
class EPCITester : public Exception
{
public:
    __fastcall EPCITester(const AnsiString& Msg) : Exception(Msg) {}
};

TMainForm *MainForm;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
    : TForm(Owner)
{
	IconImageList->GetBitmap(0, SaveDataButton->Glyph);
	IconImageList->GetBitmap(1, LoadDataButton->Glyph);
	IconImageList->GetBitmap(2, ClearMemoButton->Glyph);

	StatusBar->Panels[0][0]->Text = __DATE__;
}
//---------------------------------------------------------------------------

DWORD TMainForm::GetBinaryNumber(void)
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

void TMainForm::SetBinaryNumber(DWORD Number)
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
void __fastcall TMainForm::BinaryNumCheckboxClick(TObject *Sender)
{
    DataEdit->Text = IntToHex((int)GetBinaryNumber(),8);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::WriteButtonClick(TObject *Sender)
{
    DWORD Data = StrToInt("$" + DataEdit->Text);
    DWORD Address = StrToInt("$" + AddrressEdit->Text);
    int BarNum = BarRadioGroup1->ItemIndex;

    SetBinaryNumber(Data);
    EdenPCI_WriteDWORD(BarNum,Address,Data);

    if(RecordCheckBox->Checked)
        AddToScript(BarNum,Address,Data);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ReadButtonClick(TObject *Sender)
{
    DWORD Value = EdenPCI_ReadDWORD(BarRadioGroup1->ItemIndex,StrToInt("$" + AddrressEdit->Text));

    SetBinaryNumber(Value);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormCreate(TObject *Sender)
{
    DWORD err = EdenPCISystem_Init();

    if(err != WD_STATUS_SUCCESS)
        MessageDlg(QFormatStr("Data PCI initialization error: %s", Stat2Str(err)).c_str(), mtError, TMsgDlgButtons() << mbOK, 0);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormDestroy(TObject *Sender)
{
    EdenPCI_DeInit();
}
//---------------------------------------------------------------------------

void TMainForm::SetLed(int LedNum,bool State)
{
    TShape *TmpShape = NULL;

    switch(LedNum)
    {
    case F_LED:
        TmpShape = FLed;
        break;

    case AFF_LED:
        TmpShape = AFFLed;
        break;

    case HFAF_LED:
        TmpShape = HFAFLed;
        break;

    case AEHF_LED:
        TmpShape = AEHFLed;
        break;

    case EAE_LED:
        TmpShape = EAELed;
        break;

    case E_LED:
        TmpShape = ELed;
        break;
    }

    TmpShape->Brush->Color = State ? clRed : clWhite;
}

void TMainForm::UpdateFIFOLeds(void)
{
    unsigned Status = FIFOPCI_ReadStatus();

    SetLed(E_LED,false);
    SetLed(EAE_LED,false);
    SetLed(AEHF_LED,false);
    SetLed(HFAF_LED,false);
    SetLed(AFF_LED,false);
    SetLed(F_LED,false);
    CapacityErrorLabel->Visible = false;

    switch(Status)
    {
    case FIFO_EMPTY:
        SetLed(E_LED,true);
        break;

    case FIFO_1_TO_AE:
        SetLed(EAE_LED,true);
        break;

    case FIFO_AE_TO_HF:
        SetLed(AEHF_LED,true);
        break;

    case FIFO_HF_TO_AF:
        SetLed(HFAF_LED,true);
        break;

    case FIFO_AF_TO_FULL:
        SetLed(AFF_LED,true);
        break;

    case FIFO_FULL:
        SetLed(F_LED,true);
        break;

    case FIFO_CAPACITY_ERROR:
        CapacityErrorLabel->Visible = true;
        break;

    default:
        throw EPCITester("Invalid FIFO status value");
    }
}

void __fastcall TMainForm::ContinousWriteGoButtonClick(TObject *Sender)
{
    FIFOPCI_MasterReset();

    ContinousWriteForm->Show();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Button4Click(TObject *Sender)
{
    if(OpenDialog1->Execute())
        IOWriteMemo->Lines->LoadFromFile(OpenDialog1->FileName);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Button5Click(TObject *Sender)
{
    if(SaveDialog1->Execute())
        IOWriteMemo->Lines->SaveToFile(SaveDialog1->FileName);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ExecuteButtonClick(TObject *Sender)
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

        EdenPCI_WriteDWORD(BarNum,Address,Data);
    }
}
//---------------------------------------------------------------------------

void TMainForm::AddToScript(int BarNum,DWORD Address,DWORD Data)
{
    IOWriteMemo->Lines->Add(IntToStr(BarNum) + "," + IntToHex((int)Address,8) + "," + IntToHex((int)Data,8));
}

void __fastcall TMainForm::FIFOManualWriteButtonClick(TObject *Sender)
{
    int Count = StrToInt(FIFOWriteCountEdit->Text);

    // Allocate write buffer
    DWORD *BufferPtr = new DWORD[Count];

    // Fill buffer with values
    DWORD v = StrToInt("$" + FIFOManuaDataWriteEdit->Text);

    for(int i = 0; i < Count; i++)
        BufferPtr[i] = v;

    // Write to FIFO
    FIFOPCI_WriteDWORD(BufferPtr,Count);
    delete []BufferPtr;

    UpdateFIFOLeds();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FIFOManualReadButtonClick(TObject *Sender)
{
    int Count = StrToInt(FIFOReadCountEdit->Text);

    if(Count <= 0)
        return;

    EdenPCI_FIFOSetReadMode(FF_READ_MANUAL);

    // Allocate read buffer
    DWORD *BufferPtr = new DWORD[Count];

    int Err = FIFOPCI_ReadDWORD(BufferPtr,Count);

    if(Err != -1)
        FIFOManuaDataReadEdit->Text = IntToHex((int)BufferPtr[Count - 1],8);

    delete []BufferPtr;

    UpdateFIFOLeds();

    EdenPCI_FIFOSetReadMode(FF_READ_SERIAL);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::RefreshStatusButtonClick(TObject *Sender)
{
    UpdateFIFOLeds();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ResetFIFOButtonClick(TObject *Sender)
{
    FIFOPCI_MasterReset();
    UpdateFIFOLeds();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::NonInterruptTestButtonClick(TObject *Sender)
{
    int i;
    DWORD ReadValue,WriteValue = 0;
    unsigned PrevStatus;
    int StatusChangesCount = 0;

    FIFOPCI_MasterReset();

    PrevStatus = FIFOPCI_ReadStatus();

    // Check if empty
    if(PrevStatus != FIFO_EMPTY)
        throw EPCITester("FIFO should be empty ater reset");

    // WS 0  -
    EdenPCISystem_WriteWS(FIFO_WS);

    // Fill the fifo with data, and check for status flag changes
    for(i = 0; i < MAX_FIFO_SIZE; i++)
    {
        // Write one dword
        EdenPCI_WriteDWORD(AD_PCI_BAR2,FIFO_WRITE_REG,WriteValue);
        WriteValue++;

        unsigned Status = FIFOPCI_ReadStatus();
        if(Status != PrevStatus)
        {
            StatusChangesCount++;
            CheckFIFOExpectedStatus(StatusChangesCount,Status);
            PrevStatus = Status;
        }
    }

    Sleep(1);

    // Return the WS back
    EdenPCISystem_WriteWS(INITIAL_WS);

    if(StatusChangesCount != 5)
        throw EPCITester("Not all FIFO status signals has changed during FIFO write.\n\r Last signal = "
                         + IntToStr(StatusChangesCount));

    EdenPCI_FIFOSetReadMode(FF_READ_MANUAL);

    WriteValue = 0;

    // Read from the fifo and check for status flag changes
    for(i = 0; i < MAX_FIFO_SIZE; i++)
    {
        // Write one dword
        if(FIFOPCI_ReadDWORD(&ReadValue,1) == -1)
        {
            EdenPCI_FIFOSetReadMode(FF_READ_SERIAL);
            throw EPCITester("FIFO read error");
        }

        if(ReadValue != WriteValue)
        {
            EdenPCI_FIFOSetReadMode(FF_READ_SERIAL);
            throw EPCITester("Data read from the FIFO does not match written data.\n\r" \
                             "failed after reading " + IntToStr(WriteValue) + " dwords.");
        }

        WriteValue++;

        unsigned Status = FIFOPCI_ReadStatus();
        if(Status != PrevStatus)
        {
            StatusChangesCount--;
            CheckFIFOExpectedStatus(StatusChangesCount,Status);
            PrevStatus = Status;
        }
    }

    EdenPCI_FIFOSetReadMode(FF_READ_SERIAL);

    if(StatusChangesCount != 0)
        throw EPCITester("Not all FIFO status signals has changed during FIFO read.\n\r Last signal = " + IntToStr(StatusChangesCount));

    MessageDlg("Test pass Ok",mtInformation,TMsgDlgButtons() << mbOK,0);
}
//---------------------------------------------------------------------------

// Check a FIFO flag according to a step number
void TMainForm::CheckFIFOExpectedStatus(int StepNum,unsigned Status)
{
    int NextStep;

    if(Status == FIFO_CAPACITY_ERROR)
        throw EPCITester("FIFO capacity error");

    switch(StepNum)
    {
    case 0:
        if(Status != FIFO_EMPTY)
            throw EPCITester("Status should be FIFO_EMPTY");
        break;

    case 1:
        if(Status != FIFO_1_TO_AE)
            throw EPCITester("Status should be FIFO_1_TO_AE");
        break;

    case 2:
        if(Status != FIFO_AE_TO_HF)
            throw EPCITester("Status should be FIFO_AE_TO_HF");
        break;

    case 3:
        if(Status != FIFO_HF_TO_AF)
            throw EPCITester("Status should be FIFO_HF_TO_AF");
        break;

    case 4:
        if(Status != FIFO_AF_TO_FULL)
            throw EPCITester("Status should be FIFO_AF_TO_FULL");
        break;

    case 5:
        if(Status != FIFO_FULL)
            throw EPCITester("Status should be FIFO_FULL");
        break;
    }
}

void __fastcall TMainForm::InterruptTestButtonClick(TObject *Sender)
{
    EdenPCI_FIFOSetReadMode(FF_READ_MANUAL);

	// Allocate buffer for the test
	DWORD *WriteBuffer = new DWORD[FIFO_INTR_TEST_BLOCK_SIZE * FIFO_INTR_TEST_BLOCKS_NUM];
    DWORD *ReadBuffer = new DWORD[FIFO_INTR_TEST_BLOCK_SIZE];

	// Fill the buffer
	for(int i = 0; i < FIFO_INTR_TEST_BLOCK_SIZE * FIFO_INTR_TEST_BLOCKS_NUM; i++)
		WriteBuffer[i] = i;

	FIFOPCI_WriteAsync((PBYTE)WriteBuffer,FIFO_INTR_TEST_BLOCK_SIZE * FIFO_INTR_TEST_BLOCKS_NUM);

    DWORD CompareValue = 0;

    for(int i = 0; i < FIFO_INTR_TEST_BLOCKS_NUM; i++)
    {
        // Give the interrupt some time for writing the data
        Sleep(250);

        // Disabel interrupts and clear go flag for read back operation
        EdenPCISystem_INTRDisable(LOCAL_FIFO);
        EdenPCI_WriteDWORD(AD_PCI_BAR2,FIFO_GO_REG,0);

        // Read back from FIFO
        if(FIFOPCI_ReadDWORD(ReadBuffer,FIFO_INTR_TEST_BLOCK_SIZE) == -1)
        {
            delete []WriteBuffer;
            delete []ReadBuffer;
            EdenPCI_FIFOSetReadMode(FF_READ_SERIAL);
            throw EPCITester("FIFO read error");
        }

        EdenPCISystem_INTREnable(LOCAL_FIFO);
        EdenPCI_WriteDWORD(AD_PCI_BAR2,FIFO_GO_REG,1);

        // Check read buffer content
        for(int j = 0; j < FIFO_INTR_TEST_BLOCK_SIZE; j++,CompareValue++)
            if(ReadBuffer[j] != CompareValue)
            {
                delete []WriteBuffer;
                delete []ReadBuffer;
                EdenPCI_FIFOSetReadMode(FF_READ_SERIAL);
                throw EPCITester("Data read from the FIFO does not match written data.\n\r" \
                                 "failed after reading " + IntToStr(CompareValue) + " dwords.");
            }
    }

    EdenPCI_FIFOSetReadMode(FF_READ_SERIAL);

    delete []WriteBuffer;
    delete []ReadBuffer;

    MessageDlg("Test pass Ok",mtInformation,TMsgDlgButtons() << mbOK,0);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::EEPROMWriteButtonClick(TObject *Sender)
{
    EdenPCI_WriteEEPROM_DWORD(StrToInt("$" + EEPROMAddrEdit->Text),StrToInt("$" + EEPROMDataEdit->Text));
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::EEPROMReadButtonClick(TObject *Sender)
{
    EEPROMDataEdit->Text = IntToHex((int)EdenPCI_ReadEEPROM_DWORD(StrToInt("$" + EEPROMAddrEdit->Text)),8);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::HelpButtonClick(TObject *Sender)
{
    HelpForm->ShowModal();
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::GoButtonClick(TObject *Sender)
{
	QString InputStr;

    // Get all data into one string
	for(int i = 0; i < DataPathDataInput->Lines->Count; i++)
		InputStr += DataPathDataInput->Lines->Strings[i].c_str();

    // Pad with zeroes for multiplies of DWORDs
	int Padding = 8 - InputStr.size() % 8;
	if(Padding != 0 && Padding != 8)
		for(int i = 0; i < Padding; i++)
			InputStr += "0";

    // Data size in DWORDs
	int DataSize = InputStr.size() / 8;

    // Allocate buffer for the test
	DWORD *WriteBuffer;

	try
	{
		WriteBuffer = new DWORD[DataSize];
		
		memset(WriteBuffer, 0, sizeof(DWORD) * DataSize);

		for(int i = 0; i < DataSize; i++)
		{
			QString str    = InputStr.substr(i * 8, 8);
			WriteBuffer[i] = StrToInt("$" + AnsiString(str.c_str())); // Convert to Hex

			Application->ProcessMessages();
		}

		FIFOPCI_ResetBufferAddress();
		FIFOPCI_NoGo();

		FIFOPCI_WriteAsync((PBYTE)WriteBuffer, DataSize);

		// Go
		EdenPCI_WriteDWORD(AD_PCI_BAR2, FIFO_GO_REG, 0x01);
	}
	__finally
	{
		Q_SAFE_DELETE_ARRAY(WriteBuffer);
	}
}
//---------------------------------------------------------------------------

// Input only Hex values
void __fastcall TMainForm::MemoKeyPress(TObject *Sender, char &Key)
{
	if((Key >= '0') && (Key <= '9'))
		return;

	if((Key >= 'a') && (Key <= 'f'))
	{
		Key = UpCase(Key);
		return;
	}

	if((Key >= 'A') && (Key <= 'F'))
		return;

	if(Key == VK_ESCAPE || Key == VK_BACK || Key == VK_RETURN)
		return;

    Key = 0;	
}


void __fastcall TMainForm::ClearMemoButtonClick(TObject *Sender)
{
	DataPathDataInput->Clear();	
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::SaveDataButtonClick(TObject *Sender)
{
	if(DataFileSaveDialog->Execute())
		DataPathDataInput->Lines->SaveToFile(DataFileSaveDialog->FileName);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::LoadDataButtonClick(TObject *Sender)
{
	if(DataFileOpenDialog->Execute())
		DataPathDataInput->Lines->LoadFromFile(DataFileOpenDialog->FileName);
}
//---------------------------------------------------------------------------

