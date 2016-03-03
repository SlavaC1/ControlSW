//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ContinousWriteDlg.h"
#include "FIFOPCI.h"
#include "EdenPCISys.h"
#include "FIFOPCI.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"


//const int FIFO_WRITE_REG = 0x100;
//const int FIFO_GO_REG    = 0x1C;

TContinousWriteForm *ContinousWriteForm;
//---------------------------------------------------------------------------
__fastcall TContinousWriteForm::TContinousWriteForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TContinousWriteForm::StopButtonClick(TObject *Sender)
{
  m_StopFlag = true;
  Close();        
}
//---------------------------------------------------------------------------

void __fastcall TContinousWriteForm::Timer1Timer(TObject *Sender)
{
  m_StopFlag = false;
  Timer1->Enabled = false;

  DWORD WriteValue = 0;

  EdenPCI_WriteDWORD(AD_PCI_BAR2,FIFO_GO_REG,1);

  EdenPCISystem_WriteWS(FIFO_WS);

  do
  {
    unsigned LoopCycleCounter = 0;

    EdenPCISystem_WriteWS(INITIAL_WS);
    unsigned Status = FIFOPCI_ReadStatus();

    if((Status != FIFO_AF_TO_FULL) && (Status != FIFO_FULL))
      EdenPCISystem_WriteWS(FIFO_WS);

    while((Status != FIFO_AF_TO_FULL) && (Status != FIFO_FULL))
    {
      EdenPCI_WriteDWORD(AD_PCI_BAR2,FIFO_WRITE_REG,WriteValue++);

      EdenPCISystem_WriteWS(INITIAL_WS);
      Status = FIFOPCI_ReadStatus();
      EdenPCISystem_WriteWS(FIFO_WS);

      if(LoopCycleCounter++ == 1024)
        break;
    }

    Application->ProcessMessages();

  } while(!m_StopFlag);

  // Return the WS back
  EdenPCISystem_WriteWS(INITIAL_WS);
}
//---------------------------------------------------------------------------

void __fastcall TContinousWriteForm::FormShow(TObject *Sender)
{
  Timer1->Enabled = true;
}
//---------------------------------------------------------------------------
