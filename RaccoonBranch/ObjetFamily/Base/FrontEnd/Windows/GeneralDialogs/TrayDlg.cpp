//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TrayDlg.h"
#include "TrayHeater.h"
#include "AppLogFile.h"
#include "Q2RTApplication.h"
#include "MachineSequencer.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TTrayInsertDlg *TrayInsertDlg;
//---------------------------------------------------------------------------
__fastcall TTrayInsertDlg::TTrayInsertDlg(TComponent* Owner)
        : TForm(Owner)
{
}

//---------------------------------------------------------------------------
void __fastcall TTrayInsertDlg::Timer1Timer(TObject *Sender)
{
   CTrayBase *Tray = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetTrayInstance();

   if( Tray->IsTrayInserted() )
   {
     TrayInsertDlg->Close();
   }
}
//---------------------------------------------------------------------------

void TTrayInsertDlg::EnableDisableTimer(bool enable)
{
  Timer1->Enabled = enable;
}
//---------------------------------------------------------------------------
