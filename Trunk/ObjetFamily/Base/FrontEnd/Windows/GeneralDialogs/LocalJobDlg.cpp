//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "LocalJobDlg.h"
#include "FrontEndParams.h"
#include "AppParams.h"
#include "BackEndInterface.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"


TLocalJobForm *LocalJobForm;
//---------------------------------------------------------------------------
__fastcall TLocalJobForm::TLocalJobForm(TComponent* Owner)
    : TForm(Owner)
{
    for (int i =0 ; i < NUMBER_OF_QUALITY_MODES ; ++i)
    {
        for(  int j = 0; j < NUM_OF_OPERATION_MODES ; ++j)
        {
            m_ModesRadioButton[i][j] = NULL;
        }
    }
}
//---------------------------------------------------------------------------
QString ExtractLongPathName(LPCTSTR lpszShortPath)
{
    char lpszLongPath[_MAX_PATH];
    GetLongPathName(lpszShortPath, lpszLongPath, _MAX_PATH);
    QString ret = lpszLongPath;
    return ret;
}
//---------------------------------------------------------------------------
void __fastcall TLocalJobForm::Prepare(TOperationModeIndex OperationMode)
{
    CFrontEndParams* FEParamsMgr       = CFrontEndParams::Instance();
    NoOfSlices1UpDown->Position        = FEParamsMgr->LastNumberOfSlices1;
    NoOfSlices2UpDown->Position        = FEParamsMgr->LastNumberOfSlices2;
    TypeSelector1ComboBox->Items->Clear();
    TypeSelector2ComboBox->Items->Clear();
    ListView1->Items->Clear();
    ListView2->Items->Clear();

    QString FName;

    for(int i = FIRST_CHAMBER_TYPE; i < OperationModeToBitmapsNums(OperationMode); i++)
    {
        TypeSelector1ComboBox->Items->Add(ChamberToStr((TChamberIndex)i, (OperationMode == 0), OperationMode).c_str());
        FName = FEParamsMgr->LastFileNamesArray1[i];
        FName = ( (FName.compare("?") == 0) ? "" : ExtractLongPathName(FName.c_str()).c_str() );
        strcpy((TBMPFileName)m_FileNamesArray1 + i*MAX_FILE_PATH, FName.c_str());

        TListItem  *ListItem1;
        ListItem1 = ListView1->Items->Add();
        ListItem1->Caption = ChamberToStr((TChamberIndex)i, (OperationMode == 0), OperationMode).c_str();
        ListItem1->SubItems->Add(FName.c_str());

        TypeSelector2ComboBox->Items->Add(ChamberToStr((TChamberIndex)i, (OperationMode == 0), OperationMode).c_str());
        FName = FEParamsMgr->LastFileNamesArray2[i];
        FName = ( (FName.compare("?") == 0) ? "" : ExtractLongPathName(FName.c_str()).c_str() );
        strcpy((TBMPFileName)m_FileNamesArray2 + i*MAX_FILE_PATH, FName.c_str());

        TListItem  *ListItem2;
        ListItem2 = ListView2->Items->Add();
        ListItem2->Caption = ChamberToStr((TChamberIndex)i, (OperationMode == 0), OperationMode).c_str();
        ListItem2->SubItems->Add(FName.c_str());
    }

    TypeSelector1ComboBox->ItemIndex = TYPE_CHAMBER_MODEL1;
    m_PrevItemIndex1                 = TYPE_CHAMBER_MODEL1;
    FileName1Edit->Text              = (TBMPFileName)m_FileNamesArray1 + TYPE_CHAMBER_MODEL1*MAX_FILE_PATH;

    TypeSelector2ComboBox->ItemIndex = TYPE_CHAMBER_MODEL1;
    m_PrevItemIndex2                 = TYPE_CHAMBER_MODEL1;
    FileName2Edit->Text              = (TBMPFileName)m_FileNamesArray2 + TYPE_CHAMBER_MODEL1*MAX_FILE_PATH;
}
//---------------------------------------------------------------------------
bool TLocalJobForm::IsPrintDoubleBMPEnabled()
{
    return PrintDoubleBMPCheckBox->Checked;
}
//---------------------------------------------------------------------------
// Get the print job settings
void TLocalJobForm::GetJobSettings(TFileNamesArray    FileNames,
                                   int&               BitmapResolution,
                                   int&               QualityMode,
                                   int&               OperationMode,
                                   int&               SliceNum)
{
	CFrontEndParams   *FrontendParams = CFrontEndParams::Instance();
	//CAppParams        *ParamsMgr      = CAppParams::Instance();
	CBackEndInterface *BackEnd        = CBackEndInterface::Instance();
	
	memset(FileNames, 0, TFileNamesArraySIZE);
	QString FName;

	int LastIndex = OperationModeToBitmapsNums(FrontendParams->LastOperationMode);

	for(int i = FIRST_CHAMBER_TYPE; i < LastIndex; i++)
	{
		FName = FrontendParams->LastFileNamesArray1[i];
		FName = ((FName.compare("?") == 0) ? "" : ExpandFileName(FName.c_str()).c_str());

		strcpy((TBMPFileName)FileNames + MAX_FILE_PATH * i, FName.c_str());
	}

	QualityMode   = FrontendParams->LastQualityMode;
	OperationMode = FrontendParams->LastOperationMode;

	// Enter selected mode to get the desired X resolution
	BackEnd->GotoDefaultMode();
	BackEnd->EnterMode(PRINT_MODE[QualityMode][OperationMode], MACHINE_QUALITY_MODES_DIR(QualityMode, OperationMode));

	// Assuming that the resolution of loaded bitmap is always 600 DPI
	BitmapResolution = 600;
    SliceNum         = NoOfSlices1UpDown->Position;
}
//---------------------------------------------------------------------------
void TLocalJobForm::GetJobSettings(TFileNamesArray    FileNames1,
                                   TFileNamesArray    FileNames2,
                                   int&               BitmapResolution,
                                   int&               QualityMode,
                                   int&               OperationMode,
                                   int&               SliceNum1,
                                   int&               SliceNum2)
{
    GetJobSettings(FileNames1, BitmapResolution, QualityMode, OperationMode, SliceNum1);

    CFrontEndParams* ParamsMgr = CFrontEndParams::Instance();
    memset(FileNames2, 0, TFileNamesArraySIZE);
    QString FName;
    int     LastIndex = OperationModeToBitmapsNums(ParamsMgr->LastOperationMode);
    for(int i = FIRST_CHAMBER_TYPE; i < LastIndex; i++)
    {
        FName = ParamsMgr->LastFileNamesArray2[i];
        FName = ( (FName.compare("?") == 0) ? "" : ExpandFileName(FName.c_str()).c_str() );
        strcpy((TBMPFileName)FileNames2 + MAX_FILE_PATH*i, FName.c_str());
    }
    SliceNum2 = NoOfSlices2UpDown->Position;
}
//---------------------------------------------------------------------------
void __fastcall TLocalJobForm::TypeSelectorComboBoxChange(TObject *Sender)
{
    TComponent*     Component = dynamic_cast<TComponent*>(Sender);
    TComboBox*      TypeSelectorComboBox;
    TChamberIndex*  PrevItemIndex;
    TEdit*          FileNameEdit;
    TFileNamesArray FileNamesArray;
    if (Component->Tag == 1)
    {
        TypeSelectorComboBox = TypeSelector1ComboBox;
        PrevItemIndex  = &m_PrevItemIndex1;
        FileNameEdit   = FileName1Edit;
        FileNamesArray = m_FileNamesArray1;
    }
    else
    {
        TypeSelectorComboBox = TypeSelector2ComboBox;
        PrevItemIndex = &m_PrevItemIndex2;
        FileNameEdit  = FileName2Edit;
        FileNamesArray = m_FileNamesArray2;
    }

    if (*PrevItemIndex == TypeSelectorComboBox->ItemIndex)
        return;

    strcpy((TBMPFileName)FileNamesArray + (*PrevItemIndex)*MAX_FILE_PATH, FileNameEdit->Text.c_str());
    FileNameEdit->Text = (TBMPFileName)FileNamesArray + TypeSelectorComboBox->ItemIndex*MAX_FILE_PATH;
    *PrevItemIndex = (TChamberIndex)TypeSelectorComboBox->ItemIndex;
}
//---------------------------------------------------------------------------
void __fastcall TLocalJobForm::OpenFileSelection1ButtonClick(TObject *Sender)
{
    TComponent* Component    = dynamic_cast<TComponent*>(Sender);
    TEdit*      FileNameEdit = ((Component->Tag == 1) ? FileName1Edit : FileName2Edit);
    TListView* ListView = ((Component->Tag == 1) ? ListView1 : ListView2);
    TComboBox* TypeSelectorComboBox = ((Component->Tag == 1) ? TypeSelector1ComboBox : TypeSelector2ComboBox);
    if(OpenDialog->Execute())
    {
        FileNameEdit->Text = OpenDialog->FileName;
        TListItem  *ListItem1;
        ListItem1 = ListView->Items->Item[(TChamberIndex)TypeSelectorComboBox->ItemIndex];
        if(ListItem1!=NULL)
        {
            ListItem1->SubItems->Clear();
            ListItem1->SubItems->Add(FileNameEdit->Text.c_str());
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TLocalJobForm::ModeRadioButtonsClick(TObject *Sender)
{
//   strcpy((TBMPFileName)m_FileNamesArray1 + TypeSelector1ComboBox->ItemIndex*MAX_FILE_PATH,
//          FileName1Edit->Text.c_str());
//   strcpy((TBMPFileName)m_FileNamesArray2 + TypeSelector2ComboBox->ItemIndex*MAX_FILE_PATH,
//          FileName2Edit->Text.c_str());

    CFrontEndParams* FEParamsMgr  = CFrontEndParams::Instance();
    TRadioButton* ModeRadioButton = dynamic_cast<TRadioButton *>(Sender);
    if (!ModeRadioButton)
        return;
    int qm,om;
    TAG_TO_MODE(ModeRadioButton->Tag, qm, om);
    FEParamsMgr->LastOperationMode = om;
    FEParamsMgr->LastQualityMode   = qm;
    FEParamsMgr->SaveSingleParameter(&FEParamsMgr->LastOperationMode);
    FEParamsMgr->SaveSingleParameter(&FEParamsMgr->LastQualityMode);
    Prepare(static_cast<TOperationModeIndex>(om));
}








/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
void __fastcall TLocalJobForm::FormCreate(TObject *Sender)
{
    m_FileNamesArray1 = (TFileNamesArray)malloc(TFileNamesArraySIZE);
    m_FileNamesArray2 = (TFileNamesArray)malloc(TFileNamesArraySIZE);

    memset(m_FileNamesArray1, 0, TFileNamesArraySIZE);
    memset(m_FileNamesArray2, 0, TFileNamesArraySIZE);

    int i = 0, AccessibleModeCount = 0;
    FOR_ALL_QUALITY_MODES(qm)
    FOR_ALL_OPERATION_MODES(om)
    if (GetModeAccessibility(qm, om))
        AccessibleModeCount++;

    FOR_ALL_QUALITY_MODES(qm)
    {
        FOR_ALL_OPERATION_MODES(om)
        {
            if (GetModeAccessibility(qm, om) == false)
                m_ModesRadioButton[qm][om] = NULL;
            else
            {
                m_ModesRadioButton[qm][om]              = new TRadioButton(this);
                m_ModesRadioButton[qm][om]->Parent      = OkButton->Parent;
                MODE_TO_TAG(m_ModesRadioButton[qm][om]->Tag, qm, om);
                m_ModesRadioButton[qm][om]->TabOrder    = qm*NUMBER_OF_QUALITY_MODES + om;
                m_ModesRadioButton[qm][om]->Left        = FirstBMPGroupBox->Width/AccessibleModeCount*i + FirstBMPGroupBox->Left;
                i++;
                m_ModesRadioButton[qm][om]->Top         = 265;
                m_ModesRadioButton[qm][om]->Caption     = GetModeStr(qm, om).c_str();
                m_ModesRadioButton[qm][om]->OnClick     = ModeRadioButtonsClick;
            }
        }
    }
    PrintDoubleBMPCheckBox->Checked = false;
}
//---------------------------------------------------------------------------
void __fastcall TLocalJobForm::OkButtonClick(TObject *Sender)
{
    // Save current settings for next time
    CFrontEndParams* FEParamsMgr  = CFrontEndParams::Instance();
    TBMPFileName     iFileName    = NULL;
    QString          FName;
    TListItem  *ListItem1;
    TListItem  *ListItem2;

    strcpy((TBMPFileName)m_FileNamesArray1 + TypeSelector1ComboBox->ItemIndex*MAX_FILE_PATH,
           FileName1Edit->Text.c_str());
    strcpy((TBMPFileName)m_FileNamesArray2 + TypeSelector2ComboBox->ItemIndex*MAX_FILE_PATH,
           FileName2Edit->Text.c_str());

    for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
    {
		iFileName = (TBMPFileName) m_FileNamesArray1 + i*MAX_FILE_PATH;
        FName     = ExtractShortPathName(iFileName).c_str();
        if((FName.compare("") == 0))
        {
            FEParamsMgr->LastFileNamesArray1[i] = "?";
            ListItem1 = ListView1->Items->Item[i];
            if(ListItem1!=NULL)
            {
                ListItem1->SubItems->Clear();
                ListItem1->SubItems->Add(FName.c_str());
			}
        }
		else
		{
			FEParamsMgr->LastFileNamesArray1[i] = iFileName;
        }
        //FEParamsMgr->LastFileNamesArray1[i] = ( (FName.compare("") == 0) ? "?" : FName.c_str() );

        iFileName = (TBMPFileName) m_FileNamesArray2 + i*MAX_FILE_PATH;
        FName     = ExtractShortPathName(iFileName).c_str();
        if((FName.compare("") == 0))
        {
            FEParamsMgr->LastFileNamesArray2[i] = "?";
            ListItem2 = ListView2->Items->Item[i];
            if(ListItem2!=NULL)
            {
                ListItem2->SubItems->Clear();
                ListItem2->SubItems->Add(FName.c_str());
            }
        }
        else
        {
            FEParamsMgr->LastFileNamesArray2[i] = FName.c_str();
        }
        //FEParamsMgr->LastFileNamesArray2[i] = ( (FName.compare("") == 0) ? "?" : FName.c_str() );
    }

	FName = AnsiString((TBMPFileName) m_FileNamesArray1 + TypeSelector1ComboBox->ItemIndex * MAX_FILE_PATH).c_str();
    FEParamsMgr->LastFileNamesArray1[TypeSelector1ComboBox->ItemIndex] = ((FName.compare("") == 0) ? "?" : FName.c_str());

	FName = AnsiString((TBMPFileName) m_FileNamesArray2 + TypeSelector2ComboBox->ItemIndex * MAX_FILE_PATH).c_str();
	FEParamsMgr->LastFileNamesArray2[TypeSelector2ComboBox->ItemIndex] = ((FName.compare("") == 0) ? "?" : FName.c_str());

    FEParamsMgr->LastNumberOfSlices1 = NoOfSlices1UpDown->Position;
    FEParamsMgr->LastNumberOfSlices2 = NoOfSlices2UpDown->Position;

    // Save for next time
    FEParamsMgr->SaveSingleParameter(&FEParamsMgr->LastNumberOfSlices1);
	FEParamsMgr->SaveSingleParameter(&FEParamsMgr->LastNumberOfSlices2);

	FEParamsMgr->SaveAll();
}
//---------------------------------------------------------------------------
void __fastcall TLocalJobForm::FormDestroy(TObject *Sender)
{

    TypeSelector1ComboBox->Items->Clear();
    TypeSelector2ComboBox->Items->Clear();

    FOR_ALL_QUALITY_MODES(qm)
    {
        FOR_ALL_OPERATION_MODES(om)
        {
            if (GetModeAccessibility(qm, om) == false)
                m_ModesRadioButton[qm][om]=NULL;
            else
            {
                m_ModesRadioButton[qm][om]->Checked = false;
                delete m_ModesRadioButton[qm][om];
                m_ModesRadioButton[qm][om] = NULL;
            }
        }
    }

    if(m_FileNamesArray1)
    {
        free(m_FileNamesArray1);
        m_FileNamesArray1 = NULL;
    }
    if(m_FileNamesArray2)
    {
        free(m_FileNamesArray2);
        m_FileNamesArray2 = NULL;
    }
}
//---------------------------------------------------------------------------
void __fastcall TLocalJobForm::FormShow(TObject *Sender)
{
    CFrontEndParams* FEParamsMgr       = CFrontEndParams::Instance();
    if (GetModeAccessibility(FEParamsMgr->LastQualityMode, FEParamsMgr->LastOperationMode) == false)
        return;
    m_ModesRadioButton[FEParamsMgr->LastQualityMode][FEParamsMgr->LastOperationMode]->Checked = true;
}
//---------------------------------------------------------------------------
void __fastcall TLocalJobForm::PrintDoubleBMPCheckBoxClick(TObject *Sender)
{
    SecondBMPGroupBox->Visible = PrintDoubleBMPCheckBox->Checked;
    ListView2->Visible = PrintDoubleBMPCheckBox->Checked;
    int Diff = SecondBMPGroupBox->Height;
    if (SecondBMPGroupBox->Visible == false)
        Diff *= -1;
    PrintDoubleBMPCheckBox->Top += Diff;
    OkButton->Top += Diff;
    CancelButton->Top += Diff;
    FOR_ALL_QUALITY_MODES(qm)
    FOR_ALL_OPERATION_MODES(om)
    if (GetModeAccessibility(qm, om) == true)
        m_ModesRadioButton[qm][om]->Top += Diff;
    Height += Diff;
}
/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/




