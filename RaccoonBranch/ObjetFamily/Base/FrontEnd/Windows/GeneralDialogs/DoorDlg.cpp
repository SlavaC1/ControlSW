//---------------------------------------------------------------------------

#include <vcl.h>

#include "AppLogFile.h"
#include "FrontEnd.h"


#pragma hdrstop

#include "DoorDlg.h"
#include "BackEndInterface.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TDoorCloseDlg *DoorCloseDlg;
//---------------------------------------------------------------------------
__fastcall TDoorCloseDlg::TDoorCloseDlg(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------


void __fastcall TDoorCloseDlg::DoorCancelBitBtn1Click(TObject *Sender)
{
  CBackEndInterface::Instance()->CancelDoorLockWait();        
}
//---------------------------------------------------------------------------

