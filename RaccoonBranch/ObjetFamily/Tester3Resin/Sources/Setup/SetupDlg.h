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

//---------------------------------------------------------------------------


#include <vector>
#include "SystemParams.h"

const int MAX_NUM_COM_PORTS = 2;  // todo -oNobody -cNone: change to 8
   
typedef struct {
  int ComNum;
  bool Used;

} TComPortItemInfo;

typedef std::vector<TComPortItemInfo> TComPortItemInfoList;


class TSetupForm : public TForm
{
__published:	// IDE-managed Components
  TGroupBox *GroupBox1;
  TLabel *Date;
  TMaskEdit *DateMaskEdit;
  TMaskEdit *TimeMaskEdit;
  TLabel *Time;
  TLabel *Label1;
  TEdit *OperNameEdit;
  TLabel *Label2;
  TEdit *System_SNEdit;
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
  TEdit *Containers_SNEdit;
  TLabel *Label3;
  TEdit *LCD_SNEdit;
  TLabel *Label4;
  TEdit *MCB_SNEdit;
  TLabel *Label5;
  TEdit *OCB_SNEdit;
  TEdit *OHDB_SNEdit;
  TLabel *Label6;
  TLabel *Label7;
  TBitBtn *BitBtn1;
  TGroupBox *GroupBox8;
  TComboBox *OCBSimComboBox;
  TRadioGroup *OCBSimRadioGroup;
        TGroupBox *GroupBox9;
        TRadioGroup *RFIDRadioGroup;
        TComboBox *RFIDComboBox;
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


public:		// User declarations
  __fastcall TSetupForm(TComponent* Owner);
  bool Open(CSystemParams* SystemParams);
};
//---------------------------------------------------------------------------
extern PACKAGE TSetupForm *SetupForm;
//---------------------------------------------------------------------------
#endif
