//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FansSpeedDlg.h"
#include "FrontEndControlIDs.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFansSpeedDlg *FansSpeedDlg;

template <typename T>
void ControlConvertToArrayByTag(T* UIArr, TPanel* UIControl) {
	for (int i=0; i < UIControl->ControlCount; ++i)
	{
		T obj = dynamic_cast<T>(UIControl->Controls[i]);
		if (obj) {
		   UIArr[obj->Tag] = obj;
		}
	}
}

//---------------------------------------------------------------------------
__fastcall TFansSpeedDlg::TFansSpeedDlg(TComponent* Owner)
	: TForm(Owner)
{
	ControlConvertToArrayByTag<TEdit*> (FansSpeedTextBox, pnlFansSpeedTextBoxes);
	m_ParamsMgr = CAppParams::Instance();
}

// Update status on the frame (integer version)
void TFansSpeedDlg::UpdateStatus(int ControlID,int Status)
{
  PostMessage(Handle,WM_STATUS_UPDATE,ControlID,Status);
}
//---------------------------------------------------------------------------
// Update status on the frame (float version)
void TFansSpeedDlg::UpdateStatus(int ControlID,float Status)
{
  PostMessage(Handle,WM_STATUS_UPDATE,ControlID,*((int *)&Status));
}
//---------------------------------------------------------------------------
// Update status on the frame (string version)
void TFansSpeedDlg::UpdateStatus(int ControlID,QString Status)
{
  PostMessage(Handle,WM_STATUS_UPDATE,ControlID,reinterpret_cast<int>(strdup(Status.c_str())));
}

// Message handler for the WM_STATUS_UPDATE message
void TFansSpeedDlg::HandleStatusUpdateMessage(TMessage &Message)
{
	if (Message.WParam >= FE_FANS_SPEED_BASE && Message.WParam < FE_FANS_SPEED_BASE + FANS_DATA_LENGTH )
	{
		int Fan = Message.WParam - FE_FANS_SPEED_BASE;
		if (0 <= Message.LParam)
		{
			FansSpeedTextBox[Fan]->Text = IntToStr( Message.LParam );
		}
		else if (OFF == Message.LParam)
		{
			FansSpeedTextBox[Fan]->Text = "OFF";
		}
		else
		{
        	FansSpeedTextBox[Fan]->Text = "NA";
        }
		
	}
}

/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/

void __fastcall TFansSpeedDlg::chkMainFanSpeedDontShowWarningMessageAgainClick(
      TObject *Sender)
{
	m_ParamsMgr->MainFanSpeedDontShowWarningMessageAgainFlag = (cbChecked == chkMainFanSpeedDontShowWarningMessageAgain->State) ? 1:0;
	m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->MainFanSpeedDontShowWarningMessageAgainFlag);
}
//---------------------------------------------------------------------------
  
//---------------------------------------------------------------------------
/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
