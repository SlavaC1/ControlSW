//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "RFIDTester.h"
#include "ReaderDfn.h"
#include <map>

// Define DISABLE_PUMPTIME_ACCESS for versions that are going "public", since it allows reseting PumpTime:
#define DISABLE_PUMPTIME_ACCESS

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TRFIDForm *RFIDForm;
//---------------------------------------------------------------------------
__fastcall TRFIDForm::TRFIDForm(TComponent* Owner)
    : TForm(Owner)
{
	for(int i = 0; i < NUM_OF_RF_READERS * 4; i++)
		RFChannelBox->Items->Add(TankNames[i]);
}
//---------------------------------------------------------------------------

enum
{
    TypeEditBox,
	TypeDropList
};

void TRFIDForm::Open()
{
    try
    {
        CRFID::Init();
        m_RFIDInstance = CRFID::Instance();
        m_RFIDInstance->Start();
    }
    catch(ERFID &err)
    {
        QMonitor.ErrorMessage(err.GetErrorMsg().c_str());
    }

	for(int i = 0; i < NUM_OF_RF_READERS; i++)
        for(int j = 1; j < 5; j++)  // 4 containers for each reader
            m_RFIDInstance->EnableSampling(i,j);

    m_resinTypeIdArr[FULLCURE980] = FULLCURE980_TYPEID;
    m_resinTypeIdArr[FULLCURE930] = FULLCURE930_TYPEID;
    m_resinTypeIdArr[FULLCURE720] = FULLCURE720_TYPEID;
    m_resinTypeIdArr[FULLCURE630] = FULLCURE630_TYPEID;
    m_resinTypeIdArr[FULLCURE640] = FULLCURE640_TYPEID;
    m_resinTypeIdArr[FULLCURE660] = FULLCURE660_TYPEID;
    m_resinTypeIdArr[FULLCURE680] = FULLCURE680_TYPEID;
    m_resinTypeIdArr[FULLCURE430] = FULLCURE430_TYPEID;
	m_resinTypeIdArr[VEROGREY]    = VEROGREY_TYPEID;
	m_resinTypeIdArr[VEROBLACK]   = VEROBLACK_TYPEID;
	m_resinTypeIdArr[VEROBLUE]    = VEROBLUE_TYPEID;
	m_resinTypeIdArr[VEROWHITE]   = VEROWHITE_TYPEID;
	m_resinTypeIdArr[VEROCLEAR]   = VEROCLEAR_TYPEID;
	m_resinTypeIdArr[TANGOBLACK]  = TANGOBLACK_TYPEID;
	m_resinTypeIdArr[TANGOGREY]   = TANGOGREY_TYPEID;
	m_resinTypeIdArr[RGD851]      = RGD851_TYPEID;
	m_resinTypeIdArr[RGD841]      = RGD841_TYPEID;
    m_resinTypeIdArr[RGD843]      = RGD843_TYPEID;
	m_resinTypeIdArr[RGD836]      = RGD836_TYPEID;
	m_resinTypeIdArr[RGD535]      = RGD535_TYPEID;
	m_resinTypeIdArr[RGD525]      = RGD525_TYPEID;
	m_resinTypeIdArr[RGD515]      = RGD515_TYPEID;
	m_resinTypeIdArr[RGD531]      = RGD531_TYPEID;
	m_resinTypeIdArr[RGD545]      = RGD545_TYPEID;
    m_resinTypeIdArr[RGD546]      = RGD546_TYPEID;

    m_resinTypeIdArr[FULLCURE705] = FULLCURE705_TYPEID;
    m_resinTypeIdArr[MCLEANSER] = MCLEANSER_TYPEID;
    m_resinTypeIdArr[SCLEANSER] = SCLEANSER_TYPEID;

    for(int i = FIRST_INDEX_OF_MATERIALS; i < NUM_OF_MATERIALS; i++)
        m_resinTypeIdMap[m_resinTypeIdArr[i]] = MaterialTypeBox->Items->Strings[i].c_str();

    m_Channel = 0;
	m_rdrNum  = 0;
	
	for(int i = 0; i < NUM_OF_RF_READERS; i++)
		m_TagAccess[i] = new CContainerTag(i, m_Channel);

    ProtocolNumberGroup->Visible = false;
    //ProtocolNumberGroup->Enabled = false;
    TagVersionEdit->Visible = false;

    ErrorCodeGroup->Visible = false;
    ErrorCodeEdit->Visible = false;

    MFGDateGroupBox->Visible = false;
    MFGEdit->Visible = false;

    SecurityGroup->Visible = false;
    SecurityGroup->Enabled = false;

    SettingWeightLabel->Visible = false;
    InitialWeightEdit->Visible  = false;
    InitialWeightEdit->Enabled  = false;
    InitialWeightGroup->Visible = false;
    InitialWeightGroup->Enabled = false;

    MaterialTypeInput = TypeDropList;

    Show();
}

void TRFIDForm::RFTagsChangeCallBack(TGenericCockie Cockie, CRFReader* the_reader, int rdr_num, int channel_num, TTagsLastChange TagsCondition, unsigned __int64 tag_id)
{

}
void __fastcall TRFIDForm::ResetPumpTimeButtonClick(TObject *Sender)
{
    DWORD pumptime = 0;

    if ((m_TagAccess[m_rdrNum]->TagWritePumpingTime(pumptime)) != Q_NO_ERROR)
    {
    }
}
//---------------------------------------------------------------------------

void __fastcall TRFIDForm::SetMaterialTypeButtonClick(TObject *Sender)
{
    int   Material = 0;
    WORD toTag    = 0;

    if (TypeEditBox == MaterialTypeInput)
    {
        toTag = (WORD)StrToInt(MaterialIndex->Text.c_str());
        if ((m_TagAccess[m_rdrNum]->TagWriteResinType(toTag)) != Q_NO_ERROR)
        { }
        return;
    }

    toTag = m_resinTypeIdArr[MaterialTypeBox->ItemIndex];

    if ((m_TagAccess[m_rdrNum]->TagWriteResinType(toTag)) != Q_NO_ERROR)
    {
    }

}
//---------------------------------------------------------------------------

void __fastcall TRFIDForm::RFChannelBoxChange(TObject *Sender)
{
    // Channel num is "One-based" here:
    int itemindex = RFChannelBox->ItemIndex;

	m_rdrNum  = RFChannelBox->ItemIndex / 4;
    m_Channel = (RFChannelBox->ItemIndex % 4) + 1;

    if (m_TagAccess[m_rdrNum])
        Q_SAFE_DELETE(m_TagAccess[m_rdrNum]);

    m_TagAccess[m_rdrNum] = new CContainerTag(m_rdrNum, m_Channel);
}
//---------------------------------------------------------------------------

void __fastcall TRFIDForm::GetTagButtonClick(TObject *Sender)
{
    int num_of_tags = 0;
    unsigned __int64 tagsList[MAX_TAGS];

    //m_RFIDInstance->EnableSampling(m_rdrNum,m_Channel);

    // Get the last read tags list:
    m_RFIDInstance->GetTags( m_rdrNum
                             , m_Channel
                             , tagsList
                             , &num_of_tags);

    if (num_of_tags != 1)
        return;

    m_TagAccess[m_rdrNum]->SetTagID(tagsList[0]);

    QString TagStr = m_RFIDInstance->GetTagIDAsString(tagsList[0]);
    TagIDPanel->Caption = TagStr.c_str();
}
//---------------------------------------------------------------------------


__fastcall TRFIDForm::~TRFIDForm()
{
    //CRFID::DeInit();
    for(int i = 0; i < NUM_OF_RF_READERS; i++)
    {
        if(m_TagAccess[i])
            Q_SAFE_DELETE(m_TagAccess[i]);
    }
}
//---------------------------------------------------------------------------


void __fastcall TRFIDForm::Button2Click(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------

void __fastcall TRFIDForm::ReadTagButtonClick(TObject *Sender)
{
    // Tag types:
    TAG_TYPE__STRUCT_ID              structId = 0;
    TAG_TYPE__MFG_DATE_TIME          mfg_date_time = 0;
    TAG_TYPE__RESIN_TYPE             Material       = 0;
    TAG_TYPE__CURRENT_PUMPING_TIME   PumpTime       = 0;
    TAG_TYPE__EXPIRATION_DATE        ExpirationDate = 0;
    TAG_TYPE__INITIAL_WEIGHT         initialWeight  = 0;
    TAG_TYPE__CURRENT_WEIGHT		   currentWeight  = 0;
    TAG_TYPE__ERROR_CODE			   errorCode	  = 0;

    QString MaterialStr    = "";
    QString MemoText       = "";
    char batchNo[BATCH_NO_SIZE] = "";

    TagDataMemo->Clear();

    m_TagAccess[m_rdrNum]->InvalidateTagData();

    // Get Resing type:
    if ((m_TagAccess[m_rdrNum]->TagReadResinType(Material)) != Q_NO_ERROR)
    {
        TagDataMemo->Lines->Add("Failed to read Resin Type!");
    }
    else
    {
        MaterialStr = m_resinTypeIdMap[Material];
        TagDataMemo->Lines->Add(("Resin is: " + MaterialStr).c_str());
    }

    if ((m_TagAccess[m_rdrNum]->TagReadPumpingTime(PumpTime)) != Q_NO_ERROR)
    {
        TagDataMemo->Lines->Add("Failed to read Pump Time!");
    }
    else
        TagDataMemo->Lines->Add(("Pump Time is: " + QIntToStr(PumpTime)).c_str());

    if ((m_TagAccess[m_rdrNum]->TagReadWeight(currentWeight)) != Q_NO_ERROR)
    {
        TagDataMemo->Lines->Add("Failed to read Current Weight!");
    }
    else
        TagDataMemo->Lines->Add(("Current Weight is: " + QIntToStr(currentWeight)).c_str());

    if ((m_TagAccess[m_rdrNum]->TagReadErrorCode(errorCode)) != Q_NO_ERROR)
    {
        TagDataMemo->Lines->Add("Failed to read Error Code!");
    }
    else
        TagDataMemo->Lines->Add(("Error code is: " + QIntToStr(errorCode)).c_str());

    if ((m_TagAccess[m_rdrNum]->TagReadExpirationDate(ExpirationDate)) != Q_NO_ERROR)
    {
        TagDataMemo->Lines->Add("Failed to read Expiration Date!");
    }
    else
        TagDataMemo->Lines->Add(("Expiration Date is: " + QDateToStr(ExpirationDate)).c_str());

    if ((m_TagAccess[m_rdrNum]->TagReadInitialWeight(initialWeight)) != Q_NO_ERROR)
    {
        TagDataMemo->Lines->Add("Failed to read Initial Weight!");
    }
    else
        TagDataMemo->Lines->Add(("Initial Weight is: " + QIntToStr(initialWeight)).c_str());

    TagDataMemo->Lines->Add("=== 'Priority' Fields ===");

    if ((m_TagAccess[m_rdrNum]->TagReadStructID(structId)) != Q_NO_ERROR)
    {
        TagDataMemo->Lines->Add("Failed to read Tag Struct ID!");
    }
    else
        TagDataMemo->Lines->Add(("Tag Struct ID is: " + QIntToStr(structId)).c_str());

    if ((m_TagAccess[m_rdrNum]->TagReadBatchNo(batchNo)) != Q_NO_ERROR)
    {
        TagDataMemo->Lines->Add("Failed to read Batch Number!");
    }
    else
        TagDataMemo->Lines->Add((QString("Tag Batch Number is: ") + batchNo).c_str());

    if ((m_TagAccess[m_rdrNum]->TagReadMfgDateTime(mfg_date_time)) != Q_NO_ERROR)
    {
        TagDataMemo->Lines->Add("Failed to read Mfg Date Time!");
    }
    else
        TagDataMemo->Lines->Add(("Mfg Date Time is: " + QDateToStr(mfg_date_time)).c_str());

    TagDataMemo->Repaint();
}
//---------------------------------------------------------------------------

void __fastcall TRFIDForm::SetPumpTimeButtonClick(TObject *Sender)
{
    DWORD pumptime = 0;

    pumptime = StrToInt(PumpTimeEdit->Text.c_str());

    if ((m_TagAccess[m_rdrNum]->TagWritePumpingTime(pumptime)) != Q_NO_ERROR)
    {}
}
//---------------------------------------------------------------------------

void __fastcall TRFIDForm::SetExpirationButtonClick(TObject *Sender)
{
    time_t expdate;

// Convert the date to "Seconds elapsed from 1/1/1970" format:
    if (DatePlusTimeCheck->Checked)
    {
        expdate = QStrToDateTime(ExpirationDateEdit->Text.c_str());
    }
    else
    {
        expdate = QStrToDate(ExpirationDateEdit->Text.c_str());
    }
    DWORD tmp = (DWORD)expdate;
    // Write to Tag:
    if ((m_TagAccess[m_rdrNum]->TagWriteExpirationDate(tmp)) != Q_NO_ERROR)
    {}
}
//---------------------------------------------------------------------------

void __fastcall TRFIDForm::SetWeightButtonClick(TObject *Sender)
{
    WORD initialWeight = 0;

    initialWeight = StrToInt(InitialWeightEdit->Text.c_str());

    if ((m_TagAccess[m_rdrNum]->TagWriteInitialWeight(initialWeight)) != Q_NO_ERROR)
    {}
}
//---------------------------------------------------------------------------


void __fastcall TRFIDForm::ReadTagClick(TObject *Sender)
{
    TAG_STRUCT tag_struct;

    if ((m_TagAccess[m_rdrNum]->TagReadAllRaw(tag_struct)) != Q_NO_ERROR)
    {}
}
//---------------------------------------------------------------------------

void __fastcall TRFIDForm::MaterialIndexChange(TObject *Sender)
{
    MaterialTypeInput = TypeEditBox;
}
//---------------------------------------------------------------------------

void __fastcall TRFIDForm::MaterialTypeBoxChange(TObject *Sender)
{
    MaterialTypeInput = TypeDropList;
}
//---------------------------------------------------------------------------


void __fastcall TRFIDForm::FormClose(TObject *Sender, TCloseAction &Action)
{
    CRFID::DeInit();
}
//---------------------------------------------------------------------------


void __fastcall TRFIDForm::SetCurrentWeightBtnClick(TObject *Sender)
{
    int CurrentWeight = 0;

    CurrentWeight = StrToInt(CurrentWeightEdit->Text.c_str());

    if ((m_TagAccess[m_rdrNum]->TagWriteWeight(CurrentWeight)) != Q_NO_ERROR)
    {}
}
//---------------------------------------------------------------------------


void __fastcall TRFIDForm::EnterPasswordClick(TObject *Sender)
{
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    int res = timeinfo->tm_mday*timeinfo->tm_mday+(timeinfo->tm_mon+1)*(timeinfo->tm_mon+1)+timeinfo->tm_year;
    QString str = "TESTER" + QIntToStr(res);
    if(strcmp(str.c_str(),PasswordMaskEdit->Text.c_str())==0)
    {
        PasswordMaskEdit->Clear();
        IncorrectPass->Visible = false;
        m_validPass = true;

        SecurityGroup->Visible = true;
        SecurityGroup->Enabled = true;

        SettingWeightLabel->Visible = true;
        InitialWeightEdit->Visible  = true;
        InitialWeightEdit->Enabled  = true;

        ProtocolNumberGroup->Visible = true;
        //ProtocolNumberGroup->Enabled = true;
        TagVersionEdit->Visible = true;

        ErrorCodeGroup->Visible = true;
        ErrorCodeEdit->Visible = true;

        MFGDateGroupBox->Visible = true;
        MFGEdit->Visible = true;
    }
    else
    {
        IncorrectPass->Visible = true;
        m_validPass = true;
    }
}
//---------------------------------------------------------------------------

void __fastcall TRFIDForm::ErrorCodeBtnClick(TObject *Sender)
{
    int errCode = 0;

    errCode = StrToInt(ErrorCodeEdit->Text.c_str());

    if ((m_TagAccess[m_rdrNum]->TagWriteErrorCode(errCode)) != Q_NO_ERROR)
    {}
}
//---------------------------------------------------------------------------

void __fastcall TRFIDForm::TagVersionBtnClick(TObject *Sender)
{
    TAG_TYPE__STRUCT_ID tagVersion = 0;

    tagVersion = StrToInt(TagVersionEdit->Text.c_str());

    if ((m_TagAccess[m_rdrNum]->TagWriteStructID(tagVersion)) != Q_NO_ERROR)
    {}
}
//---------------------------------------------------------------------------

void __fastcall TRFIDForm::MFGBtnClick(TObject *Sender)
{
    time_t mfgdate;

    // Convert the date to "Seconds elapsed from 1/1/1970" format:
    if (DateTimeCheckBox->Checked)
    {
        mfgdate = QStrToDateTime(MFGEdit->Text.c_str());
    }
    else
    {
        mfgdate = QStrToDate(MFGEdit->Text.c_str());
    }
    DWORD tmp = (DWORD)mfgdate;
    // Write to Tag:
    if ((m_TagAccess[m_rdrNum]->TagWriteMfgDateTime(tmp)) != Q_NO_ERROR)
    {}

    //TagWriteSingleParam(TAG_PARAM__1ST_USAGE_TIME_INSERTION, &first_usage_time);
}
//---------------------------------------------------------------------------

