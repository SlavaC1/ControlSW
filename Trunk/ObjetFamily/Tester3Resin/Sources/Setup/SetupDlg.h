//---------------------------------------------------------------------------

#ifndef SetupDlgH
#define SetupDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Mask.hpp>
#include <Buttons.hpp>
#include <Menus.hpp>
#include <ExtCtrls.hpp>
#include "GenFourTesterDefs.h"   
#include <vector>
#include "SystemParams.h"

const int MAX_NUM_COM_PORTS = 2;  // todo -oNobody -cNone: change to 8

typedef struct
{
	int ComNum;
	bool Used;

} TComPortItemInfo;

typedef std::vector<TComPortItemInfo> TComPortItemInfoList;


class TSetupForm : public TForm
{
__published:	// IDE-managed Components
	TBitBtn *OKBitBtn;
	TGroupBox *GroupBox7;
	TComboBox *ContainersComboBox;
	TRadioGroup *ContainersRadioGroup;
	TGroupBox *GroupBox2;
	TComboBox *LCDComboBox;
	TRadioGroup *LCDRadioGroup;
	TGroupBox *GroupBox3;
	TComboBox *MCBComboBox;
	TRadioGroup *MCBRadioGroup;
	TGroupBox *GroupBox4;
	TComboBox *OCBComboBox;
	TRadioGroup *OCBRadioGroup;
	TGroupBox *GroupBox5;
	TComboBox *OHDBComboBox;
	TRadioGroup *OHDBRadioGroup;
	TGroupBox *GroupBox6;
	TComboBox *EmbeddedComboBox;
	TRadioGroup *EmbeddedRadioGroup;
	TBitBtn *BitBtn1;
	TGroupBox *GroupBox8;
	TComboBox *OCBSimComboBox;
	TRadioGroup *OCBSimRadioGroup;
	TGroupBox *GroupBox9;
	TRadioGroup *RFIDRadioGroup;
	TComboBox *RFIDComboBox;
	TGroupBox *LogGroupbox;
	void __fastcall ComComboBoxDropDown(TObject *Sender);
	void __fastcall EmbeddedRadioGroupClick(TObject *Sender);
	void __fastcall MCBRadioGroupClick(TObject *Sender);
	void __fastcall LCDRadioGroupClick(TObject *Sender);
	void __fastcall ContainersRadioGroupClick(TObject *Sender);
	void __fastcall OCBRadioGroupClick(TObject *Sender);
	void __fastcall OHDBRadioGroupClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall OKBitBtnClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall OCBSimRadioGroupClick(TObject *Sender);
	void __fastcall RFIDRadioGroupClick(TObject *Sender);


private:	// User declarations

	// pointer to the instance of the parametrs manager
	CSystemParams *m_SystemParams;

	// find all the com ports that are currently available
	void FindAllAvailableComPorts();

	// Update the comm list of the new state of a comm port
	void UpdateCommList(int ComNum, bool Used);

	// check if a com port is in the available port list
	bool ComPortIsAvailable(int ComNum);

	TComPortItemInfoList m_ComPortInfoList;

	TCheckBox *m_LogTagCheckbox[JS_LOG_TAGS_NUM];


public:		// User declarations
	__fastcall TSetupForm(TComponent* Owner);
	bool Open(CSystemParams* SystemParams);
};
//---------------------------------------------------------------------------
extern PACKAGE TSetupForm *SetupForm;
//---------------------------------------------------------------------------
#endif
