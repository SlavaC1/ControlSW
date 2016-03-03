//---------------------------------------------------------------------------

#include <vcl.h>

#include "AppLogFile.h"
#include "FrontEnd.h"


#pragma hdrstop

#include "DoorDlg.h"
#include "BackEndInterface.h"
#include <string.h>


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
void __fastcall TDoorCloseDlg::ChangeDoorsGuiLabels(int StringIndex)
{
	if (
              (   0 <= StringIndex) &&( StringIndex < ARRAY_SIZR)
             )
	 {
	   Label1->Caption = DoorArray[StringIndex];
	   if(  StringIndex < REAR_EMERGENCY_STOP_BUTTON_IS_PRESSED) //if 0 <= StringIndex <5
	   {
		Label2->Caption = ToContinueArray[0];
	   }
	   else
	   {
		 Label2->Caption = ToContinueArray[1];
	   }
   }

}
//---------------------------------------------------------------------------


/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
void __fastcall TDoorCloseDlg::DoorCancelBitBtn1Click(TObject *Sender)
{
  CBackEndInterface::Instance()->CancelDoorLockWait();        
}
/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
