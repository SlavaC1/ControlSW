//---------------------------------------------------------------------------

#ifndef FansSpeedDlgH
#define FansSpeedDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "GlobalDefs.h"
#include "OHDBCommDefs.h"
//---------------------------------------------------------------------------

//#define NUM_OF_FANS 10
// Define private windows message for status updates
#define WM_STATUS_UPDATE WM_USER

class TFansSpeedDlg : public TForm
{
__published:	// IDE-managed Components
	TPanel *pnlFansSpeedTextBoxes;
	TEdit *edtLeftUvFan1;
	TEdit *edtLeftUvFan2;
	TEdit *edtRightUvFan1;
	TEdit *edtRightUvFan2;
	TEdit *edtLeftMaterialFan1;
	TEdit *edtLeftMaterialFan2;
	TEdit *edtLeftMaterialFan3;
	TEdit *edtRightMaterialFan1;
	TEdit *edtRightMaterialFan2;
	TEdit *edtMiddleFan;
	TCheckBox *chkMainFanSpeedDontShowWarningMessageAgain;
	TLabel *lbl1;
	TLabel *lbl2;
	TLabel *lbl3;
	TLabel *lbl4;
	TLabel *lbl5;
	TLabel *lbl6;
	TLabel *lbl7;
	TLabel *lbl8;
	TLabel *lbl9;
	TLabel *lbl10;
	TLabel *lbl11;
	TLabel *lbl12;
	TLabel *lbl13;
	TLabel *lbl14;
	TLabel *lbl15;
	TLabel *lbl16;
	TLabel *lbl17;
	TLabel *lbl18;
	TLabel *lbl19;
	TLabel *lbl20;
	void __fastcall chkMainFanSpeedDontShowWarningMessageAgainClick(
          TObject *Sender);
private:	// User declarations
	CAppParams *m_ParamsMgr;
	// Message handler for the WM_STATUS_UPDATE message
	void HandleStatusUpdateMessage(TMessage &Message);
public:		// User declarations
	__fastcall TFansSpeedDlg(TComponent* Owner);

public:
	TEdit *FansSpeedTextBox[NUM_OF_FANS];

	// Update status on the frame (integer version)
	void UpdateStatus(int ControlID,int Status);
	// Update status on the frame (float version)
	void UpdateStatus(int ControlID,float Status);
	// Update status on the frame (string version)
  	void UpdateStatus(int ControlID,QString Status);

BEGIN_MESSAGE_MAP
  MESSAGE_HANDLER(WM_STATUS_UPDATE,TMessage,HandleStatusUpdateMessage);
END_MESSAGE_MAP(TForm);

};
//---------------------------------------------------------------------------
extern PACKAGE TFansSpeedDlg *FansSpeedDlg;
//---------------------------------------------------------------------------
#endif
