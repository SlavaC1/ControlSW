//---------------------------------------------------------------------------

#ifndef RFIDTesterH
#define RFIDTesterH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "rfid.h"
#include "ContainerTag.h"
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Mask.hpp>

enum
{
     FULLCURE980 = 0
	,FIRST_INDEX_OF_MATERIALS = FULLCURE980
	,FULLCURE930
    ,FULLCURE720
    ,FULLCURE630
    ,FULLCURE640
    ,FULLCURE660
    ,FULLCURE680
    ,FULLCURE430
    ,VEROGREY
    ,VEROBLACK
    ,VEROBLUE
    ,VEROWHITE
    ,VEROCLEAR
    ,RGD851
	,RGD841
	,RGD843
    ,RGD836
    ,TANGOBLACK
    ,TANGOGREY
    ,RGD515
    ,RGD525
    ,RGD535
    ,RGD531
    ,RGD545
    ,RGD546
    ,FULLCURE705
    ,MCLEANSER
    ,SCLEANSER
    ,NUM_OF_MATERIALS
};

enum
{
     FULLCURE980_TYPEID = 980
	,FULLCURE930_TYPEID = 930
	,FULLCURE720_TYPEID = 720
	,FULLCURE630_TYPEID = 630
	,FULLCURE640_TYPEID = 640
	,FULLCURE660_TYPEID = 660
	,FULLCURE680_TYPEID = 680
	,FULLCURE430_TYPEID = 430
	,VEROGREY_TYPEID    = 850
	,VEROBLACK_TYPEID   = 870
	,VEROBLUE_TYPEID    = 840
	,VEROWHITE_TYPEID   = 837
	,VEROCLEAR_TYPEID   = 810
	,RGD851_TYPEID      = 851
	,RGD841_TYPEID      = 841
	,RGD843_TYPEID      = 843
	,RGD836_TYPEID      = 836
	,TANGOBLACK_TYPEID  = 970
	,TANGOGREY_TYPEID   = 950
	,RGD535_TYPEID      = 535
	,RGD525_TYPEID      = 525
	,RGD515_TYPEID      = 515
	,RGD531_TYPEID      = 531
	,RGD545_TYPEID      = 545
	,RGD546_TYPEID      = 546
	,FULLCURE705_TYPEID = 705
	,MCLEANSER_TYPEID   = 10
	,SCLEANSER_TYPEID   = 11
};

const char* TankNames[NUM_OF_RF_READERS * 4] =
{
	"Support Left",
	"Support Right",
	"M1 L", "M1 R",
	"M2 L", "M2 R",
	"M3 L", "M3 R",
	"M4 L", "M4 R",
	"M5 L", "M5 R",
	"M6 L", "M6 R",
	"M7 L", "M7 R"
};

//---------------------------------------------------------------------------
class TRFIDForm : public TForm
{
__published:	// IDE-managed Components
    TButton *Button2;
    TPageControl *PageControl1;
    TTabSheet *TabSheet1;
    TGroupBox *GroupBox1;
    TLabel *Label1;
    TComboBox *RFChannelBox;
    TPanel *TagIDPanel;
    TButton *GetTagButton;
    TGroupBox *GroupBox2;
    TLabel *Label2;
    TComboBox *MaterialTypeBox;
    TButton *SetMaterialTypeButton;
    TGroupBox *SecurityGroup;
    TLabel *Label6;
    TEdit *PumpTimeEdit;
    TButton *SetPumpTimeButton;
    TButton *ResetPumpTimeButton;
    TGroupBox *GroupBox4;
    TLabel *Label3;
    TEdit *ExpirationDateEdit;
    TButton *SetExpirationButton;
    TCheckBox *DatePlusTimeCheck;
    TGroupBox *GroupBox5;
    TButton *ReadTagButton;
    TMemo *TagDataMemo;
    TGroupBox *InitialWeightGroup;
    TLabel *SettingWeightLabel;
    TButton *SetWeightButton;
    TEdit *InitialWeightEdit;
    TTabSheet *TabSheet2;
    TMemo *TagDataAsciiMemo;
    TMemo *TagDataHexMemo;
    TLabel *Label5;
    TLabel *Label7;
    TButton *ReadTag;
    TEdit *MaterialIndex;
    TGroupBox *CurrentWeightGroupBox;
    TButton *SetCurrentWeightBtn;
    TGroupBox *GroupBox3;
    TButton *EnterPassword;
    TLabel *Label4;
    TMaskEdit *PasswordMaskEdit;
    TLabel *IncorrectPass;
    TGroupBox *ErrorCodeGroup;
    TLabel *Label8;
    TButton *ErrorCodeBtn;
    TGroupBox *ProtocolNumberGroup;
    TLabel *Label10;
    TButton *TagVersionBtn;
    TEdit *ErrorCodeEdit;
    TEdit *TagVersionEdit;
    TEdit *CurrentWeightEdit;
    TLabel *Label9;
    TGroupBox *MFGDateGroupBox;
    TLabel *Label11;
    TButton *MFGBtn;
    TEdit *MFGEdit;
    TCheckBox *DateTimeCheckBox;
    void __fastcall ResetPumpTimeButtonClick(TObject *Sender);
    void __fastcall SetMaterialTypeButtonClick(TObject *Sender);
    void __fastcall RFChannelBoxChange(TObject *Sender);
    void __fastcall GetTagButtonClick(TObject *Sender);
    void __fastcall Button2Click(TObject *Sender);
    void __fastcall ReadTagButtonClick(TObject *Sender);
    void __fastcall SetPumpTimeButtonClick(TObject *Sender);
    void __fastcall SetExpirationButtonClick(TObject *Sender);
    void __fastcall SetWeightButtonClick(TObject *Sender);
    void __fastcall ReadTagClick(TObject *Sender);
    void __fastcall MaterialIndexChange(TObject *Sender);
    void __fastcall MaterialTypeBoxChange(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall SetCurrentWeightBtnClick(TObject *Sender);
    void __fastcall EnterPasswordClick(TObject *Sender);
    void __fastcall ErrorCodeBtnClick(TObject *Sender);
    void __fastcall TagVersionBtnClick(TObject *Sender);
    void __fastcall MFGBtnClick(TObject *Sender);
private:	// User declarations
    CRFIDBase*     m_RFIDInstance;
    CContainerTag* m_TagAccess[NUM_OF_RF_READERS];
    int            m_Channel;
    int 		   m_rdrNum;
    int            MaterialTypeInput;
    int m_resinTypeIdArr[NUM_OF_MATERIALS];
    map<WORD,QString> m_resinTypeIdMap;
    bool m_validPass;

public:		// User declarations
    void Open();
    static void RFTagsChangeCallBack(TGenericCockie Cockie, CRFReader* the_reader, int rdr_num, int channel_num, TTagsLastChange TagsCondition, unsigned __int64 tag_id);
    __fastcall TRFIDForm(TComponent* Owner);
    __fastcall ~TRFIDForm();
};
//---------------------------------------------------------------------------
extern PACKAGE TRFIDForm *RFIDForm;
//---------------------------------------------------------------------------
#endif
