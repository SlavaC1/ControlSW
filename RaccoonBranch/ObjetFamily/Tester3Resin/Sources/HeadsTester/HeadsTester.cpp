//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "HeadsTester.h"
#include "AppParams.h"
#include "EdenProtocolClient.h"
#include "EdenProtocolClientIDs.h"
#include "OHDBCommDefs.h"
#include "QMonitor.h"
#include <limits>
using namespace std;


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#define SW0 0
#define SW1 1
#define SW2 2
#define SW3 3
#define SW4 4
#define SW5 5
#define SW6 6
#define SW7 7
#define CLR 0
#define SET 1

// Constants for the creation of the nozzle displaying
const int C_LEFT_OFFSET     = 485;
const int LABEL_TOP_OFFSET  = 644;
const int BTN_TOP_OFFSET    = 659;
const int NUMBER_OF_HEADS   = 8;
const int COLUMNS_IN_HEAD   = 3;
const int TEMP_LEFT_OFFSET  = 15;
const int NOZZLES_IN_HEAD   = 32;

//

const int MAX_TIME_FOR_NO_OF_FIRES = 5000;
const int MILISECONDS_IN_SECONDS   = 1000;

const int XILINX_CLOCK = 33333333;
const int CONTINOUOUS_FIRE_BIT = 0x10;
const int ENABLE_DIAGNOSTICS_SIMULATORS = 0x0307;

// for specific num of fire both the contionous fire bit and the specific num of fire bit must be set
const int SPECIFIC_NUM_OF_FIRE_BIT = 0x30;

THeadsTesterForm *HeadsTesterForm;

DWORD Ref[] = {0x80000000,0x40000000,0x20000000,0x10000000,0x8000000,0x4000000,0x2000000,0x1000000,
              0x800000,  0x400000,0x200000,0x100000,0x80000,0x40000,0x20000,0x10000,
              0x8000,0x4000,0x2000,0x1000,0x800,0x400,0x200,0x100,
              0x80,0x40,0x20,0x10,0x8,0x4,0x2,0x1};


//---------------------------------------------------------------------------

void THeadsTesterForm::Open(CEdenProtocolEngine *ProtocolEngine)
{
  if (m_ProtocolClient == NULL)
  {
    m_ProtocolEngine = ProtocolEngine;
    COHDBProtocolClient::Init(m_ProtocolEngine);
    m_ProtocolClient = COHDBProtocolClient::Instance();
  }


  Show();
}
//---------------------------------------------------------------------------


__fastcall THeadsTesterForm::THeadsTesterForm(TComponent* Owner)
        : TForm(Owner)
{

}
//---------------------------------------------------------------------------



void __fastcall THeadsTesterForm::DownloadButtonClick(TObject *Sender)
{
  try
  {
    DownloadingProgressBar->Position = 0;
    DownloadingProgressBar->Visible  = true;
    HeadsTesterStatusBar->SimpleText = "Downloading Data to Xilinx";

    CAppParams *ParamsMgr = CAppParams::GetInstance();

    //Update Param manager for new values
    UpdateFields();

    ParamsMgr->FireFrquency   = StrToInt(FireFrequency->Text);
    ParamsMgr->PulseWidth     = StrToFloat(PulseWidth->Text);
    ParamsMgr->PulseDelay     = StrToFloat(PulseDelay->Text);
    ParamsMgr->FireTime       = StrToInt(FireTimeEdit->Text);
    ParamsMgr->NumOfFires     = StrToInt(NumOfFiresEdit->Text);
    ParamsMgr->CycleOnTime    = StrToInt(CycleOnTimeEdit->Text);
    ParamsMgr->CycleOffTime   = StrToInt(CycleOffTimeEdit->Text);
    ParamsMgr->CycleTotalTime = StrToInt(CycleTotalTimeEdit->Text);
    ParamsMgr->PostPulseDelay = StrToInt("$" + PostPulseEdit->Text);

    // Saving parameters
    ParamsMgr->SaveAll();
    
    // send the print parameters to the xilinx
    for (int i = 0; i < PRINT_COMMAND_LENGTH; i++)
    {
      WriteDataToXilinx(m_PrintCommand[i].Address, m_PrintCommand[i].Data);
      DownloadingProgressBar->Position ++;
    }

    HeadsTesterStatusBar->SimpleText = "Ready";
    DownloadingProgressBar->Visible  = false;
    PrintAction->Enabled = true;
  }
  catch(Exception& e)
  {
    MessageDlg(e.Message,mtError,TMsgDlgButtons() << mbOK,0);
  }

}
//---------------------------------------------------------------------------

void __fastcall THeadsTesterForm::ClearClick(TObject *Sender)
{

  TCheckBox *MyCheckbox;
  TButton   *MyButton;
	TComponent *MyComp;
	int i;

  for(i = 0; i < Panel1->ComponentCount; i++)
  {
    MyComp = Panel1->Components[i];

    if((MyCheckbox = dynamic_cast<TCheckBox *>(MyComp)) != 0)
     MyCheckbox->Checked = false;
    if((MyButton = dynamic_cast<TButton *>(MyComp)) != 0)
     if(MyButton->Caption == "CLR")
      MyButton->Caption = "SET" ;

  }
	NozzelsName->Caption ="001";

	for(i=0; i<NUMBER_OF_HEADS; i++) {
		ColumnIndex[i] = 0;
	}

}
//---------------------------------------------------------------------------

void __fastcall THeadsTesterForm::SetButtonEvent(TObject *Sender)
{

  int SW = dynamic_cast<TButton *>(Sender)->Tag;
  int start_i = 0;

//ShowMessage(dynamic_cast<TButton *>(Sender)->Caption + " pressed");

  ColumnIndex[SW]++;
  ColumnIndex[SW] %= (COLUMNS_IN_HEAD + 2);

  if( ColumnIndex[SW] == 0 )
  {
    dynamic_cast<TButton *>(Sender)->Caption ="SET";
    EditCheckbox(CLR, PTRN_SIZE * SW, (PTRN_SIZE * (SW+1)) - 1 );
  }
  else if( ColumnIndex[SW] == COLUMNS_IN_HEAD + 1 )
  {
    dynamic_cast<TButton *>(Sender)->Caption ="CLR";
    EditCheckbox(SET, PTRN_SIZE * SW, (PTRN_SIZE * (SW+1)) - 1 );
  }
  else
  {
    dynamic_cast<TButton *>(Sender)->Caption ="SET";
    EditCheckbox(CLR, PTRN_SIZE * SW, (PTRN_SIZE * (SW+1)) - 1 );
    start_i = (PTRN_SIZE * SW) + (NOZZLES_IN_HEAD*(ColumnIndex[SW]-1));
    EditCheckbox(SET, start_i, start_i + NOZZLES_IN_HEAD - 1 );
  }


}

//---------------------------------------------------------------------------

void __fastcall THeadsTesterForm::SetCheckboxEvent(TObject *Sender)
{
   NozzelsName->Caption = IntToStr(dynamic_cast<TCheckBox *>(Sender)->Tag + 1);
   PrintAction->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall THeadsTesterForm::FormCreate(TObject *Sender)
{
  int TmpLeftOffset = C_LEFT_OFFSET;
  BYTE Patterns[PTRN_SIZE];

  TCheckBox *MyCheckbox;

  for (int TmpHead = 0; TmpHead < NUMBER_OF_HEADS; TmpHead++)
  {
		TmpLeftOffset = C_LEFT_OFFSET - (TmpHead * (COLUMNS_IN_HEAD * 15 + 15));
    for (int TmpCol = 0; TmpCol < COLUMNS_IN_HEAD; TmpCol ++)
    {
      TmpLeftOffset -= TEMP_LEFT_OFFSET;
      for(int TmpCheckBox = 0; TmpCheckBox < NOZZLES_IN_HEAD; TmpCheckBox++)
      {
        MyCheckbox = new TCheckBox(Panel1);
        //Use the Tag to transfer information via components member (int type)
				MyCheckbox->Tag     = ((TmpHead * PTRN_SIZE) + (TmpCol * 32) + TmpCheckBox);
        MyCheckbox->Left    = TmpLeftOffset;
        MyCheckbox->Top     = 624 - TmpCheckBox * 20;
        MyCheckbox->Width   = 15;
        MyCheckbox->Parent  = Panel1;
        MyCheckbox->OnClick = SetCheckboxEvent;

      }
    }

    TButton *SetBtn = new TButton(Panel1);
    SetBtn->Parent  = Panel1;
    SetBtn->Left    = TmpLeftOffset +7  ;
    SetBtn->Top     = BTN_TOP_OFFSET;
    SetBtn->Height  = 18;
    SetBtn->Width   = 30;
    SetBtn->Caption = "SET";
    SetBtn->Tag     = TmpHead;
    SetBtn->OnClick = SetButtonEvent;

    AnsiString Label;
    switch (TmpHead)
    {
      case 0:
        Label = "M0";
        break;
      case 1:
        Label = "M1";
        break;
      case 2:
        Label = "M2";
        break;
      case 3:
        Label = "M3";
        break;
      case 4:
        Label = "S0";
        break;
      case 5:
        Label = "S1";
        break;
      case 6:
        Label = "S2";
        break;
      case 7:
        Label = "S3";
        break;
    }

    TLabel *SetLbl = new TLabel(Panel1);
    SetLbl->Parent  = Panel1;
    SetLbl->Left    = TmpLeftOffset + 15;
    SetLbl->Top     = LABEL_TOP_OFFSET;
    SetLbl->Height  = 18;
    SetLbl->Width   = 30;
    SetLbl->Caption = Label;
		SetLbl->Tag     = TmpHead;

		ColumnIndex[TmpHead] = 0;
	}
  
  InitPrintCommand();

  HeadsTesterStatusBar->SimpleText = "Ready";
  
}
//---------------------------------------------------------------------------



void __fastcall THeadsTesterForm::EditCheckbox(bool Stat,int First,int Last)
{
  TCheckBox *MyCheckbox;
  TComponent *MyComp;
  for(int i = 0; i < Panel1->ComponentCount; i++)
  {
    MyComp = Panel1->Components[i];

    if((MyCheckbox = dynamic_cast<TCheckBox *>(MyComp)) != 0)
    //Check if in eranage
     if ((MyCheckbox->Tag >= First)&& (MyCheckbox->Tag <= Last))
    //Set OR Reset
      if(Stat)
       MyCheckbox->Checked = true;
      else
       MyCheckbox->Checked = false;

  }

}
/*******************************************************************************/

void THeadsTesterForm::UpdateFields()
{
  union
  {
    BYTE Patterns[PTRN_SIZE];
    DWORD DWPatterns[PTRN_SIZE / 4];
  };

  char tmps[PTRN_SIZE];
  int i;
  memset(Patterns,0,PTRN_SIZE);
  TCheckBox *MyCheckbox;
  TComponent *MyComp;

  for(int i = 0; i < Panel1->ComponentCount; i++)
  {
    MyComp = Panel1->Components[i];

    if((MyCheckbox = dynamic_cast<TCheckBox *>(MyComp)) != 0)
    {
     //Check if in range
		 if ((MyCheckbox->Tag >= 0)&& (MyCheckbox->Tag <= PTRN_SIZE * NUMBER_OF_HEADS))
       if (MyCheckbox->Checked)
       {
        DWPatterns[MyCheckbox->Tag / 32] |= Ref[MyCheckbox->Tag % 32];
       }

    }
  }
  memcpy (PatternArray, Patterns, PTRN_SIZE);

  WORD PulseWitdhAndDelay = (BYTE)( StrToFloat(PulseDelay->Text)/0.12);
  PulseWitdhAndDelay = PulseWitdhAndDelay << 8;
  PulseWitdhAndDelay |= (BYTE) (StrToFloat(PulseWidth->Text)/0.12);
  m_PrintCommand[PULSE_WITDH_DELAY_INDEX] = XilinxAddrData(0x08,PulseWitdhAndDelay);
  m_PrintCommand[POST_PULSER_INDEX] = XilinxAddrData(0x51,StrToInt("$"+PostPulseEdit->Text));
  m_PrintCommand[FIRE_FREQUENCY_INDEX] = XilinxAddrData(0x35, XILINX_CLOCK/((StrToInt(FireFrequency->Text) * 16)));
  WORD DiagReg = ENABLE_DIAGNOSTICS_SIMULATORS;
  if (NumOfFiresRadioButton->Checked)
  {
    DiagReg |= SPECIFIC_NUM_OF_FIRE_BIT;
    m_PrintCommand[NUM_OF_FIRES_INDEX] = XilinxAddrData(0x36,StrToInt(NumOfFiresEdit->Text));
  }
  else
  {
    DiagReg |= CONTINOUOUS_FIRE_BIT;
    m_PrintCommand[NUM_OF_FIRES_INDEX] = XilinxAddrData(0x36,0);
  }

  m_PrintCommand[DIAG_REG_INDEX] = XilinxAddrData(0x04,DiagReg);
  for (i = 0; i < PTRN_SIZE; i++)
     m_PrintCommand[PATTERN_TEST_INDEX /*+ 1*/ + i] = XilinxAddrData(0x2a,Patterns[i]);

}



//---------------------------------------------------------------------------

void __fastcall THeadsTesterForm::FormShow(TObject *Sender)
{
  LoadingUpdateFields ();
}
//---------------------------------------------------------------------------


void THeadsTesterForm::WriteDataToXilinx(BYTE Address, WORD Data)
{
  TOHDBXilinxWriteMessage Msg;
  Msg.MessageID = OHDB_WRITE_DATA_TO_XILINX;
  Msg.Address = Address;
  Msg.Data = Data;

  // (zohar) int TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TOHDBXilinxWriteMessage));
  // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

  // Wait for reply
  // (zohar) TQWaitResult WaitResult = m_XilinxWriteEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

  if (m_ProtocolClient->SendInstallWaitReply(&Msg,
                           sizeof(TOHDBXilinxWriteMessage),
                           AckHandler,
                           reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

  {
     // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
		 throw Exception("Tester did not recieve Ack");
  }

}
//---------------------------------------------------------------------------
              

void THeadsTesterForm::InitPrintCommand()
{
  m_PrintCommand[ZERO_CTRL_REG_INDEX]       = XilinxAddrData(0x03,0);
  m_PrintCommand[ZERO_DIAG_REG_INDEX]       = XilinxAddrData(0x04,0);
  m_PrintCommand[HEAD_DELAY_1_INDEX]        = XilinxAddrData(0x20,0x15);
  m_PrintCommand[HEAD_DELAY_2_INDEX]        = XilinxAddrData(0x21,0x15);
  m_PrintCommand[HEAD_DELAY_3_INDEX]        = XilinxAddrData(0x22,0x15);
  m_PrintCommand[HEAD_DELAY_4_INDEX]        = XilinxAddrData(0x23,0x15);
  m_PrintCommand[HEAD_DELAY_5_INDEX]        = XilinxAddrData(0x24,0x15);
  m_PrintCommand[HEAD_DELAY_6_INDEX]        = XilinxAddrData(0x25,0x15);
  m_PrintCommand[HEAD_DELAY_7_INDEX]        = XilinxAddrData(0x26,0x15);
  m_PrintCommand[HEAD_DELAY_8_INDEX]        = XilinxAddrData(0x27,0x15);
  m_PrintCommand[PRE_PULSER_INDEX]          = XilinxAddrData(0x50,0x01);
  m_PrintCommand[POST_PULSER_INDEX]         = XilinxAddrData(0x51,0x08);
  m_PrintCommand[POLARITY_INDEX]            = XilinxAddrData(0x07,0xff);
  m_PrintCommand[HW_RESET_INDEX]            = XilinxAddrData(0x10,0x01);
  m_PrintCommand[STATE_MACHINE_RESET_INDEX] = XilinxAddrData(0x11,0x01);
  m_PrintCommand[START_PEG_INDEX]           = XilinxAddrData(0x01,0x50);
  m_PrintCommand[END_PEG_INDEX]             = XilinxAddrData(0x02,0x3000);
  m_PrintCommand[CONFIG_REG_INDEX]          = XilinxAddrData(0x05,0x02);
//  m_PrintCommand[PATTERN_TEST_INDEX]      = XilinxAddrData(0x2a,0xff);


}
//-------------------------------------------------------------------------

TXilinxCommand THeadsTesterForm::XilinxAddrData(BYTE Addr,WORD Data)
{
  TXilinxCommand Cmd = {Addr,Data};

  return Cmd;
}

//---------------------------------------------------------------------------

void THeadsTesterForm::AckHandler(int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
   THeadsTesterForm *InstancePtr = reinterpret_cast<THeadsTesterForm *>(Cockie);

   // Build the read data message
   TOHDBAck *Msg = static_cast<TOHDBAck *>(Data);

   if(Msg->MessageID != OHDB_ACK)
    return;

   switch (Msg->RespondedMessageID)
   {
      case OHDB_WRITE_DATA_TO_XILINX:
        if (Msg->AckStatus == OHDB_Ack_Success)
          InstancePtr->m_XilinxWriteEvent.SetEvent();
        break;

      default:
        return;
	 }

   InstancePtr->m_ProtocolClient->UnInstallReceiveHandler(TransactionId);

}
//-------------------------------------------------------------------------


void __fastcall THeadsTesterForm::PrintActionExecute(TObject *Sender)
{

  try
  {
    m_StopPrinting = false;
    PrintButton->Glyph = NULL;
    PrintButton->Action = StopAction;
    unsigned long EndPrintTime;


    WORD DiagReg = ENABLE_DIAGNOSTICS_SIMULATORS;

    // Enabling Timer
    m_PrintTime = 0;

    PrintTimeTimer->Enabled = true;
    PrintTimeEdit->Text     = "0:00";
    PrintTimePanel->Visible = true;

    HeadsTesterStatusBar->SimpleText = "Printing";

    CAppParams *ParamsMgr = CAppParams::GetInstance();

    //Update Param manager for new values
    ParamsMgr->FireTime       = StrToInt(FireTimeEdit->Text);
    ParamsMgr->NumOfFires     = StrToInt(NumOfFiresEdit->Text);
    ParamsMgr->CycleOnTime    = StrToInt(CycleOnTimeEdit->Text);
    ParamsMgr->CycleOffTime   = StrToInt(CycleOffTimeEdit->Text);
    ParamsMgr->CycleTotalTime = StrToInt(CycleTotalTimeEdit->Text);

    // Saving parameters
    ParamsMgr->SaveAll();
    
    switch(m_FireOption)
    {
      case foNumberOfFires:
        DiagReg |= SPECIFIC_NUM_OF_FIRE_BIT;
        WriteDataToXilinx(0x36,StrToInt(NumOfFiresEdit->Text));
        WriteDataToXilinx(0x04,DiagReg);

        // reset the 'GO' register
        WriteDataToXilinx(0x03,0x00);

        // start the printing (it will stop automatically)
        WriteDataToXilinx(0x03,0x01);
        break;

      case foFireTime:
      {
        // Starting time measure
        unsigned long PrintFireTime      = StrToInt(FireTimeEdit->Text);
        unsigned int  FireFrequencyValue = StrToInt(FireFrequency->Text);
        unsigned int  NumberOfFires      = (PrintFireTime * FireFrequencyValue) / MILISECONDS_IN_SECONDS;

        EndPrintTime = GetTickCount() + PrintFireTime;
        // In case time is smaller than 5 secs - do it with number of fires
        if (PrintFireTime < MAX_TIME_FOR_NO_OF_FIRES)
        {
          WriteDataToXilinx(0x36,NumberOfFires);

          DiagReg = ENABLE_DIAGNOSTICS_SIMULATORS;
          DiagReg |= SPECIFIC_NUM_OF_FIRE_BIT;
          WriteDataToXilinx(0x04,DiagReg);

          // reset the 'GO' register
          WriteDataToXilinx(0x03,0x00);

          // start the printing (it will stop automatically)
          WriteDataToXilinx(0x03,0x01);

          while (GetTickCount() < EndPrintTime)
          {
            Application->ProcessMessages();
          }

        } else
        {
          DiagReg |= CONTINOUOUS_FIRE_BIT;
          WriteDataToXilinx(0x36,0);
          WriteDataToXilinx(0x04,DiagReg);

          // start the printing
          WriteDataToXilinx(0x03,0x01);
          while (GetTickCount() < EndPrintTime)
          {
            Application->ProcessMessages();
            if (m_StopPrinting)
              break;
          }
          // stop the printing
          WriteDataToXilinx(0x03,0x00);
        }
        break;
       }
      case foContinuousFire:
        DiagReg |= CONTINOUOUS_FIRE_BIT;
        WriteDataToXilinx(0x36,0);
        WriteDataToXilinx(0x04,DiagReg);

        // start the printing
        WriteDataToXilinx(0x03,0x01);
        EndPrintTime = std::numeric_limits<unsigned long>::max() - 1000;
        while (GetTickCount() < EndPrintTime)
        {
          Application->ProcessMessages();
          if (m_StopPrinting)
            break;
        }
        // stop the printing
        WriteDataToXilinx(0x03,0x00);
        break;

      case foFireCycle:
      {
        DiagReg |= CONTINOUOUS_FIRE_BIT;
        WriteDataToXilinx(0x36,0);
        WriteDataToXilinx(0x04,DiagReg);

        unsigned long CurrTime, CurrCycleEndTime;
        bool OnCycle = true;
        unsigned long EndTotalCycleTime;
        int CycleOffTime = StrToInt(CycleOffTimeEdit->Text);
        int CycleOnTime  = StrToInt(CycleOnTimeEdit->Text);
        CurrCycleEndTime = GetTickCount() + CycleOnTime;

        // In case "-1" was inserted - make it run 'Forever'
        if ((StrToInt(CycleTotalTimeEdit->Text)) == -1)
        {
          EndTotalCycleTime = std::numeric_limits<unsigned long>::max() - 1000;
        } else
        {
          EndTotalCycleTime = GetTickCount() + StrToInt(CycleTotalTimeEdit->Text);
        }


        // start the printing
        WriteDataToXilinx(0x03,0x01);

        while( (CurrTime = GetTickCount()) < EndTotalCycleTime)
        {
          if (m_StopPrinting)
            break;

          if (CurrTime >= CurrCycleEndTime)
          {
            if (OnCycle)
            {
              // stop the printing
              WriteDataToXilinx(0x03,0x00);
              OnCycle = false;
              CurrCycleEndTime = CurrTime + CycleOffTime;
            }
            else
            {
              // resume the printing
              WriteDataToXilinx(0x03,0x01);
              OnCycle = true;
              CurrCycleEndTime = CurrTime + CycleOnTime;
            }
          }
          Application->ProcessMessages();
        }
        // stop the printing
        WriteDataToXilinx(0x03,0x00);
        break;
      }
    }

    PrintButton->Glyph = NULL;
    PrintButton->Action = PrintAction;
    PrintButton->Down = false;
  }
  catch (Exception& e)
  {
    PrintButton->Down = false;
    MessageDlg(e.Message,mtError,TMsgDlgButtons() << mbOK,0);
  }

    // Disabling Timer
    PrintTimeTimer->Enabled = false;

    HeadsTesterStatusBar->SimpleText = "Ready";
}
//---------------------------------------------------------------------------

void __fastcall THeadsTesterForm::StopActionExecute(TObject *Sender)
{
  m_StopPrinting = true;
  PrintButton->Glyph = NULL;
  PrintButton->Action = PrintAction;

    // Disabling Timer
    PrintTimeTimer->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall THeadsTesterForm::FireOptionsRadioButtonClick(
      TObject *Sender)
{
  TRadioButton *RadioButton = dynamic_cast<TRadioButton *> (Sender);

  if (RadioButton == NULL)
    return;

  m_FireOption = RadioButton->Tag;
}
//---------------------------------------------------------------------------



void __fastcall THeadsTesterForm::SavePatternButtonClick(TObject *Sender)
{
  CQParamsFileStream *FileStream = NULL;
  try
  {
    CAppParams *ParamsMgr = CAppParams::GetInstance();

    //Update Param manager for new values
    UpdateFields();

    ParamsMgr->FireFrquency   = StrToInt(FireFrequency->Text);
    ParamsMgr->PulseWidth     = StrToFloat(PulseWidth->Text);
    ParamsMgr->PulseDelay     = StrToFloat(PulseDelay->Text);
    ParamsMgr->FireTime       = StrToInt(FireTimeEdit->Text);
    ParamsMgr->NumOfFires     = StrToInt(NumOfFiresEdit->Text);
    ParamsMgr->CycleOnTime    = StrToInt(CycleOnTimeEdit->Text);
    ParamsMgr->CycleOffTime   = StrToInt(CycleOffTimeEdit->Text);
    ParamsMgr->CycleTotalTime = StrToInt(CycleTotalTimeEdit->Text);
    ParamsMgr->PostPulseDelay = StrToInt("$" + PostPulseEdit->Text);

    for (int i = 0; i < PTRN_SIZE; i++)
    {
      ParamsMgr->NozzlePatternValue [i] = PatternArray [i];
    }

    // Saving Parameters & Pattern to a wanted file (*.cfg)
    SavePatternSaveDialog->Filter = "Config files (*.cfg)|*.CFG";
    if (SavePatternSaveDialog->Execute())
    {
      // Try to delete the current file (in case it's already exists
      DeleteFile(SavePatternSaveDialog->FileName.c_str());

      // Create a file params stream
      QString s = SavePatternSaveDialog->FileName.c_str();

      FileStream = new CQParamsFileStream(s);

      // Saving parameters
      ParamsMgr->SaveSingleParameter(&ParamsMgr->FireFrquency      , FileStream);
      ParamsMgr->SaveSingleParameter(&ParamsMgr->PulseWidth        , FileStream);
      ParamsMgr->SaveSingleParameter(&ParamsMgr->PulseDelay        , FileStream);
      ParamsMgr->SaveSingleParameter(&ParamsMgr->FireTime          , FileStream);
      ParamsMgr->SaveSingleParameter(&ParamsMgr->NumOfFires        , FileStream);
      ParamsMgr->SaveSingleParameter(&ParamsMgr->CycleOnTime       , FileStream);
      ParamsMgr->SaveSingleParameter(&ParamsMgr->CycleOffTime      , FileStream);
      ParamsMgr->SaveSingleParameter(&ParamsMgr->CycleTotalTime    , FileStream);
      ParamsMgr->SaveSingleParameter(&ParamsMgr->PostPulseDelay    , FileStream);
      ParamsMgr->SaveSingleParameter(&ParamsMgr->NozzlePatternValue, FileStream);
    }

  }
/*  catch(Exception& e)
  {
    MessageDlg(e.Message,mtError,TMsgDlgButtons() << mbOK,0);
  }*/ __finally
    {
			Q_SAFE_DELETE(FileStream);
    }
}
//---------------------------------------------------------------------------

void __fastcall THeadsTesterForm::LoadPatternButtonClick(TObject *Sender)
{
  CQParamsFileStream *FileStream = NULL;
  CAppParams         *ParamsMgr  = CAppParams::GetInstance();

  // Setting Filter.
  LoadPatternOpenDialog->Filter = "Config files (*.cfg)|*.CFG|All files (*.*)|*.*";
  if(LoadPatternOpenDialog->Execute())
  {

    // Prepare a parameters file stream and import the stream
    try
    {
      FileStream = new CQParamsFileStream(LoadPatternOpenDialog->FileName.c_str());
      ParamsMgr->Import(FileStream,false);

      // Updating the fields
      LoadingUpdateFields ();
    }
    catch(EQException& Err)
    {
    }

  Q_SAFE_DELETE(FileStream);
  }
}
//---------------------------------------------------------------------------

void __fastcall THeadsTesterForm::PrintTimeTimerTimer(TObject *Sender)
{
  int PrintTimeMinutes;
  int PrintTimeSeconds;
  int PrintTime10Seconds;

  m_PrintTime ++;

  PrintTimeMinutes   = m_PrintTime / 60;
  PrintTimeSeconds   = m_PrintTime % 10;
  PrintTime10Seconds = (m_PrintTime - (PrintTimeMinutes * 60)) / 10;

  PrintTimeEdit->Text = IntToStr (PrintTimeMinutes) + ":" + IntToStr (PrintTime10Seconds) + IntToStr (PrintTimeSeconds);
}
//---------------------------------------------------------------------------

void __fastcall THeadsTesterForm::SetAllButtonClick(TObject *Sender)
{

  TCheckBox *MyCheckbox;
  TButton   *MyButton;
	TComponent *MyComp;
	int i;

	for(i = 0; i < Panel1->ComponentCount; i++)
  {
    MyComp = Panel1->Components[i];

    if((MyCheckbox = dynamic_cast<TCheckBox *>(MyComp)) != 0)
     MyCheckbox->Checked = true;
    if((MyButton = dynamic_cast<TButton *>(MyComp)) != 0)
     if(MyButton->Caption == "SET")
      MyButton->Caption = "CLR" ;

  }
	NozzelsName->Caption ="001";

	for(i=0; i<NUMBER_OF_HEADS; i++) {
		ColumnIndex[i] = COLUMNS_IN_HEAD + 1;
	}

}
//---------------------------------------------------------------------------

void THeadsTesterForm::LoadingUpdateFields()
{
  TCheckBox  *MyCheckbox;
  TComponent *MyComp;

  CAppParams *ParamsMgr  = CAppParams::GetInstance();

  union
  {
    BYTE Patterns[PTRN_SIZE];
    DWORD DWPatterns[PTRN_SIZE / 4];
  };


  //  Update Form with new values
  //  ---------------------------
  FireFrequency->Text      = IntToStr((int)ParamsMgr->FireFrquency);
  PulseWidth->Text         = FloatToStr(ParamsMgr->PulseWidth);
  PulseDelay->Text         = FloatToStr((int)ParamsMgr->PulseDelay);
  FireTimeEdit->Text       = FloatToStr(ParamsMgr->FireTime);
  NumOfFiresEdit->Text     = IntToStr((int)ParamsMgr->NumOfFires);
  CycleOnTimeEdit->Text    = IntToStr((int)ParamsMgr->CycleOnTime);
  CycleOffTimeEdit->Text   = IntToStr((int)ParamsMgr->CycleOffTime);
  CycleTotalTimeEdit->Text = IntToStr((int)ParamsMgr->CycleTotalTime);
  PostPulseEdit->Text      = IntToHex((int)ParamsMgr->PostPulseDelay,2);

  // Copying the heads data to the union
  for (int i = 0; i < PTRN_SIZE; i++)
  {
    Patterns [i] = ParamsMgr->NozzlePatternValue [i];
  }

  // For every CheckBox in the panel - set it's value
  for(int i = 0; i < Panel1->ComponentCount; i++)
  {
    MyComp = Panel1->Components[i];

    // Only components which are TCheckBox proceed
    if((MyCheckbox = dynamic_cast<TCheckBox *>(MyComp)) != 0)
    {
      //Check if in range
      if ((MyCheckbox->Tag >= 0)&& (MyCheckbox->Tag<=PTRN_SIZE * NUMBER_OF_HEADS))
      {
        MyCheckbox->Checked = ((DWPatterns[MyCheckbox->Tag/32] & Ref[MyCheckbox->Tag%32]) != 0);
      }
    }
  }
}

//---------------------------------------------------------------------------

void __fastcall THeadsTesterForm::ParameterChange(TObject *Sender)
{
  PrintAction->Enabled = false;
}
//---------------------------------------------------------------------------

