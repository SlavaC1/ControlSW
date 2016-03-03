//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "HeadsButtonsEntryFrame.h"
#include "AppParams.h"
#include "AppLogFile.h"
#include "QMonitor.h"
#include "FEResources.h"
#include "HeadsButtonsEntryPage.h"
#include "BackEndInterface.h"
#include "QThreadUtils.h"
#include <math.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#define IS_PAGE_DONE(_steps_) ((_steps_) >= TOTAL_NUMBER_OF_HEADS * NUMBER_OF_VOLTAGE_MODES * NUMBER_OF_QUALITY_MODES)

int XPrintOrder[NUMBER_OF_VOLTAGE_MODES][NUMBER_OF_QUALITY_MODES];
int YPrintOrder[NUMBER_OF_VOLTAGE_MODES][NUMBER_OF_QUALITY_MODES];
int MaxXPosition=0;
int MaxYPosition=0;

enum
{
    MISSING_NOZZLES  = 0
   ,WEIGHTS
   ,NUMBER_OF_INPUTS
};

enum
{
    E_QUALITY_MODE  = 0
   ,E_VOLTAGE
   ,E_HEAD
   ,E_OPERATION_MODE
   ,NUMBER_OF_TAGS
};

#define QUALITY_MODE(x)   ((x)->Tags[E_QUALITY_MODE])
#define VOLTAGE(x)        ((x)->Tags[E_VOLTAGE])
#define HEAD(x)           ((x)->Tags[E_HEAD])
#define OPERATION_MODE(x) ((x)->Tags[E_OPERATION_MODE]) /*ADD USAGE FOR THIS*/

//---------------------
// For fixing the consideration of Carpet weight in Max_Weight calculations in the sanity of entered weights in HSW
// (These are taken from 'HeadServiceWizard.cpp')
#define CARPET_MODE_QUALITY_MODE   HQ_INDEX
#define CARPET_VOLTAGE             HIGH_VOLTAGE
#define NUMBER_OF_CARPET_SLICES	   6
//---------------------

class EHeadsButtonsEntryWizard : public EQException
{
  public:
	EHeadsButtonsEntryWizard(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

class ECheckingLowVsHighVoltage : public EQException
{
  public:
	ECheckingLowVsHighVoltage(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

class EOverMaxExist : public EQException
{
  public:
	EOverMaxExist(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};           

class EOverMaxAllowed : public EQException
{
  public:
	EOverMaxAllowed(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

THeadsButtonsEntryFrame *HeadsButtonsEntryFrame;
/*******************************************************************************
* C'tor
*******************************************************************************/
__fastcall THeadsButtonsEntryFrame::~THeadsButtonsEntryFrame()
{
  FOR_ALL_QUALITY_MODES(qm)
     FOR_ALL_VOLTAGE_MODES(v)
        FOR_ALL_HEADS(h)
           Q_SAFE_DELETE(m_Buttons[h][v][qm]);

  if (MaxXPosition == 0)
     FOR_ALL_HEADS(h)
        Q_SAFE_DELETE(m_NameLabels[h]);

  for (int i=0; i < NUMBER_OF_INPUTS; i++)
  {
     Q_SAFE_DELETE(InputLabels[i]);
     Q_SAFE_DELETE(InputEdits[i]);
  }
  Q_SAFE_DELETE_ARRAY(InputEdits);
  Q_SAFE_DELETE_ARRAY(InputLabels);
}

__fastcall THeadsButtonsEntryFrame::THeadsButtonsEntryFrame(TComponent* Owner, float*** aWeights, int*** aMissingNozzles, int& aStepsNum, int& aQualityIndex, int& aVoltageIndex, int& aHeadIndex, bool* aHeadsToBeSkipped, bool*** aMarkHeadsToBeReplaced)
	: TFrame(Owner),StepsNum(aStepsNum), Weights(aWeights), MissingNozzles(aMissingNozzles), QualityIndex(aQualityIndex), VoltageIndex(aVoltageIndex), HeadIndex(aHeadIndex), HeadsToBeSkipped(aHeadsToBeSkipped), MarkHeadsToBeReplaced(aMarkHeadsToBeReplaced)
{
  MaxXPosition=0;
  MaxYPosition=0;
  int Nibble = 4;
  int ValueH = 28;
  int Top = 3*Nibble;

  FOR_ALL_QUALITY_MODES(qm)
      FOR_ALL_VOLTAGE_MODES(v)
      {
         if (MaxXPosition < XPrintOrder[v][qm])
            MaxXPosition = XPrintOrder[v][qm];
         if (MaxYPosition < YPrintOrder[v][qm])
            MaxYPosition = YPrintOrder[v][qm];
      }

  if (MaxXPosition == 0)
  {
    FOR_ALL_HEADS(h)
    {
      m_NameLabels[h]              = new TLabel(this);
      m_NameLabels[h]->AutoSize    = false;
      m_NameLabels[h]->Parent      = TrayMapGroupBox;
      m_NameLabels[h]->Top         = Top;
      m_NameLabels[h]->Caption     = GetHeadName(h).c_str();
      m_NameLabels[h]->Font->Style = TFontStyles()<< fsBold;
      m_NameLabels[h]->Alignment   = taCenter;
      m_NameLabels[h]->Transparent = true;

    }
    Top+=m_NameLabels[FIRST_HEAD]->Height;
  }
  else
  {
     TrayMapGroupBox->Width  += TrayMapGroupBox->Left - EnterGroupBox->Left;
     TrayMapGroupBox->Left   = EnterGroupBox->Left;
     EnterGroupBox->Width    = TrayMapGroupBox->Width;
     Top+=Nibble;
  }
  
  int ValueW = TrayMapGroupBox->Width/((MaxXPosition+1)*TOTAL_NUMBER_OF_HEADS);
  
  //The width should be bigger that nibble
  if (ValueW < Nibble*2)
     Nibble = max(1,ValueW/3);
  ValueW -= Nibble;

  FOR_ALL_HEADS(h)
  {
     FOR_ALL_QUALITY_MODES(qm)
     {
           FOR_ALL_VOLTAGE_MODES(v)
           {
               m_Buttons[h][v][qm]                = new TSpeedButton(this);
               m_Buttons[h][v][qm]->Parent        = TrayMapGroupBox;
               m_Buttons[h][v][qm]->Width         = ValueW;
               m_Buttons[h][v][qm]->Height        = ValueH;
               m_Buttons[h][v][qm]->Left          = Nibble + (ValueW + Nibble)*CalcCol(h, v, qm);
               m_Buttons[h][v][qm]->Top           = Top + CalcRow(/*h,*/ v, qm)*(ValueH+Nibble);
               m_Buttons[h][v][qm]->OnClick       = ButtonClick;
               m_Buttons[h][v][qm]->NumGlyphs     = 2;/*Enable+Disable*/
           }
      }
      if (MaxXPosition == 0)
      {
        m_NameLabels[h]->Left  = m_Buttons[h][FIRST_VOLTAGE_MODE][FIRST_QUALITY_MODE]->Left;
        m_NameLabels[h]->Width = ValueW;
      }
  }

  InputEdits  = new TMultiGridEdit*[NUMBER_OF_INPUTS];
  InputLabels = new TLabel*[NUMBER_OF_INPUTS];
  for (int i=0; i < NUMBER_OF_INPUTS; i++)
  {
     InputLabels[i]              = new TLabel(this);
     InputLabels[i]->AutoSize    = true;
     InputLabels[i]->Parent      = EnterGroupBox;
     InputLabels[i]->Transparent = true;
     InputLabels[i]->Width       = max(40,EnterGroupBox->Width/(NUMBER_OF_INPUTS+1/*ApplyButton*/));
     InputLabels[i]->WordWrap    = true;

     InputEdits[i]               = new TMultiGridEdit(EnterGroupBox, NUMBER_OF_TAGS);
     InputEdits[i]->Parent       = EnterGroupBox;
     InputEdits[i]->Height       = 21;
     InputEdits[i]->TabOrder     = i;
     InputEdits[i]->Width        = 40;
     InputEdits[i]->CellTag      = i;
  }

  InputLabels[WEIGHTS]->Caption           = "Weight";
  InputEdits[WEIGHTS]->OnKeyPress         = WeightEditKeyPress;

  InputLabels[MISSING_NOZZLES]->Caption   = "Missing Nozzles";
  InputEdits[MISSING_NOZZLES]->OnExit     = MainMissingNozzlesEditExit;
  InputEdits[MISSING_NOZZLES]->OnKeyPress = MissingNozzlesEditKeyPress;

  int StartNibble = 6*Nibble;
  if (MaxXPosition != 0)
  {
     TrayMapGroupBox->Height = Top + (MaxYPosition+1)*(ValueH+Nibble)+Nibble;
     EnterGroupBox->Top = TrayMapGroupBox->Top + TrayMapGroupBox->Height + Nibble;
     EnterGroupBox->Height = TrayMapGroupBox->Height;

     ValueW = EnterGroupBox->Width/(NUMBER_OF_INPUTS+1)-Nibble;
     for (int i=0; i < NUMBER_OF_INPUTS; i++)
     {
        InputLabels[i]->Top = StartNibble;
        InputLabels[i]->Alignment   = taLeftJustify;
        InputLabels[i]->Left = Nibble + (Nibble+ValueW)*i;
        InputLabels[i]->Width = ValueW-2*Nibble;

        InputEdits[i]->Top = InputLabels[0]->Top + InputLabels[0]->Height;
        InputEdits[i]->Left = InputLabels[i]->Left;
        InputEdits[i]->Width = InputLabels[i]->Width;
     }
     ApplyButton->Top = InputEdits[0]->Top;
     ApplyButton->Width = InputEdits[0]->Width;
     ApplyButton->Left = Nibble + (Nibble+ValueW)*NUMBER_OF_INPUTS;
  }
  else
  {
     ValueH = EnterGroupBox->Height/(NUMBER_OF_INPUTS+1)-Nibble;
     for (int i=0; i < NUMBER_OF_INPUTS; i++)
     {
        InputLabels[i]->Top =  StartNibble + (Nibble+ValueH)*i;
        InputLabels[i]->Alignment   = taCenter;
        InputLabels[i]->Left = Nibble;
        if(InputLabels[i]->Width > EnterGroupBox->Width-2*Nibble)
        {
           InputLabels[i]->AutoSize = false;
           InputLabels[i]->Height *= 2;
           InputLabels[i]->Width = EnterGroupBox->Width-2*Nibble;
        }

        InputEdits[i]->Top = InputLabels[i]->Top + InputLabels[i]->Height;
        InputEdits[i]->Left = InputLabels[i]->Left;
        InputEdits[i]->Width = InputLabels[i]->Width;
     }
     ApplyButton->Top = StartNibble + (Nibble+ValueH)*NUMBER_OF_INPUTS;
     ApplyButton->Width = InputEdits[0]->Width;
     ApplyButton->Left = InputEdits[0]->Left;
  }
  UpdateTags();
  RefreshButtons();
}

/*******************************************************************************
* RefreshButtons
*******************************************************************************/
void THeadsButtonsEntryFrame::RefreshButtons()
{
  UpdateTags();
  FOR_ALL_QUALITY_MODES(qm)
      FOR_ALL_VOLTAGE_MODES(v)
           FOR_ALL_HEADS(h)
           {
               if(UNINITIALIZED == Weights[h][v][qm] || UNINITIALIZED == MissingNozzles[h][v][qm])
                  continue;
               if ( HeadsToBeSkipped[h] )
                  ButtonMarkSkipped(m_Buttons[h][v][qm]);
               else
                  ButtonClear(m_Buttons[h][v][qm]);
           }
  ButtonMark(m_Buttons[HeadIndex][VoltageIndex][QualityIndex]);
}

/*******************************************************************************
* ButtonClick
*******************************************************************************/
/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
void __fastcall THeadsButtonsEntryFrame::ButtonClick(TObject *Sender)
{
  //fixme ApplyButton->Click();
}

/*******************************************************************************
* MissingNozzlesEditKeyPress
*******************************************************************************/
void __fastcall THeadsButtonsEntryFrame::MissingNozzlesEditKeyPress(TObject *Sender, char &Key)
{
    if ((Key >= '0') && (Key <= '9'))
        return;

    if (Key == VK_ESCAPE)
        return;

    if (Key == VK_BACK)
        return;

    if (Key == VK_RETURN)
        return;

    Key = 0;
}
//---------------------------------------------------------------------------


void CHeadsButtonsEntryFrameWizardPageViewer::Leave(TWinControl *PageWindow,CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason)
{
  switch(LeaveReason)
  {
     case lrCanceled:
          m_HeadsButtonsEntryFrame->m_EditWTValuesDlg->CancelButton->Click();
          break;

     case lrGoPrevious:
          m_HeadsButtonsEntryFrame->UpdateIndexedToPrev();
          break;

     case lrGoNext:
          m_HeadsButtonsEntryFrame->UpdateIndexedToNext();
          break;

  }
  Q_SAFE_DELETE(m_HeadsButtonsEntryFrame);
}

/*******************************************************************************
* ApplyButtonClick //this function shouldn't be reentrant
*******************************************************************************/
void __fastcall CHeadsButtonsEntryFrameWizardPageViewer::ApplyButtonClick(TObject *Sender)
{
  try
  {
     int h = m_HeadsButtonsEntryFrame->HeadIndex;
     int v = m_HeadsButtonsEntryFrame->VoltageIndex;
     int qm = m_HeadsButtonsEntryFrame->QualityIndex;

     //If the head marked to be replaced don't check anything
     if (m_HeadsButtonsEntryFrame->MissingNozzlesEditExitEvent(m_HeadsButtonsEntryFrame->InputEdits[MISSING_NOZZLES]))
     {
        if (m_HeadsButtonsEntryFrame->MainWeightEditExit(m_HeadsButtonsEntryFrame->InputEdits[WEIGHTS]))
        {
           m_HeadsButtonsEntryFrame->Weights[h][v][qm] = StrToFloat(m_HeadsButtonsEntryFrame->InputEdits[WEIGHTS]->Text);
           m_HeadsButtonsEntryFrame->MissingNozzles[h][v][qm] = m_HeadsButtonsEntryFrame->MissingNozzlesStrToInt();

           int StepsNum = m_HeadsButtonsEntryFrame->StepsNum;
           int QualityIndex = m_HeadsButtonsEntryFrame->QualityIndex;
           int VoltageIndex = m_HeadsButtonsEntryFrame->VoltageIndex;
           int HeadIndex = m_HeadsButtonsEntryFrame->HeadIndex;

           m_HeadsButtonsEntryFrame->UpdateIndexedToNext(HeadIndex, VoltageIndex, QualityIndex, StepsNum, false);

           if (IS_PAGE_DONE(StepsNum))
           {
              m_HeadsButtonsEntryFrame->EnterGroupBox->Visible = false;
              m_HeadsButtonsEntryFrame->InputEdits[WEIGHTS]->Visible = false;
              m_HeadsButtonsEntryFrame->InputEdits[MISSING_NOZZLES]->Visible = false;
              m_HeadsButtonsEntryFrame->ButtonClear(m_HeadsButtonsEntryFrame->m_Buttons[h][v][qm]);
              GetParentWizard()->EnableDisableNext(true);
           }
           else
              UpdateGotoNextIndex();
        }
     }
  }
  catch(ECheckingLowVsHighVoltage &ErrMsg)
  {
     QMonitor.ErrorMessage(ErrMsg.GetErrorMsg(),ORIGIN_WIZARD_PAGE);
     m_HeadsButtonsEntryFrame->InputEdits[WEIGHTS]->Text = "";
     m_HeadsButtonsEntryFrame->InputEdits[WEIGHTS]->SetFocus();
  }
}


void CHeadsButtonsEntryFrameWizardPageViewer::Refresh(TWinControl *PageWindow,CWizardPage *WizardPage)
{
  //SetNextButtonStatus according ti Edit's value
  GetParentWizard()->EnableDisablePrevious(/*CheckPreviousButtonStatus(WizardPage)*/false);
  if (m_HeadsButtonsEntryFrame->EnterGroupBox->Visible)
  {
     m_HeadsButtonsEntryFrame->RefreshButtons();
     m_HeadsButtonsEntryFrame->InputEdits[MISSING_NOZZLES]->SetFocus();
  }
}
/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
/*******************************************************************************
* CheckPreviousButtonStatus
*******************************************************************************/
bool THeadsButtonsEntryFrame::CheckPreviousButtonStatus(void)
{
   return (StepsNum != 0);
}

/*******************************************************************************
* EnterEditMode
*******************************************************************************/
void THeadsButtonsEntryFrame::EnterEditMode(void* obj)
{
  THeadsButtonsEntryFrame* pThis = (THeadsButtonsEntryFrame*)obj;
  int MaxWidth=0,MaxHeigth=0;
  for (int i=0; i < NUMBER_OF_INPUTS; i++)
  {
     pThis->m_EditWTValuesDlg->SetRowLabelCaption(QString(pThis->InputLabels[i]->Caption.c_str()), i);
     pThis->m_EditWTValuesDlg->SetKeyPressEvent(pThis->InputEdits[i]->OnKeyPress, i);
     pThis->m_EditWTValuesDlg->SetEditExitEvent(pThis->InputEdits[i]->OnExit, i);
     MaxWidth = max(MaxWidth,pThis->InputLabels[i]->Width);
     MaxHeigth = max(MaxHeigth,pThis->InputLabels[i]->Height);
  }
  pThis->m_EditWTValuesDlg->SetEditExitEvent(pThis->MissingNozzlesEditExit, MISSING_NOZZLES);
  pThis->m_EditWTValuesDlg->SetEditExitEvent(pThis->WeightEditExit, WEIGHTS);
  pThis->m_EditWTValuesDlg->SetRowLabelWidth(MaxWidth);
  pThis->m_EditWTValuesDlg->SetRowLabelHeight(MaxHeigth);

  if (MaxXPosition == 0)
  {
    FOR_ALL_HEADS(h)
      pThis->m_EditWTValuesDlg->SetColumnLabelCaption(h, GetHeadName(h));
    pThis->m_EditWTValuesDlg->SetColumnLabelHeight(15);
    pThis->m_EditWTValuesDlg->SetColumnWidth(40);
  }
  else
    pThis->m_EditWTValuesDlg->SetColumnLabelHeight(0);
    
  pThis->m_EditWTValuesDlg->Clear();

  int qm = LAST_QUALITY_MODE-1;
  int v = LAST_VOLTAGE_MODE-1;
  int h = LAST_HEAD-1;
  for(int Tab=0; Tab < NUMBER_OF_QUALITY_MODES*NUMBER_OF_VOLTAGE_MODES*TOTAL_NUMBER_OF_HEADS; Tab++)
  {
     for (int i=0; i < NUMBER_OF_INPUTS; i++)
        pThis->m_EditWTValuesDlg->SetEditTab(Tab*NUMBER_OF_INPUTS + i,
                                             pThis->CalcRow(/*h,*/ v, qm),
                                             i,
                                             pThis->CalcCol(h, v, qm));
     pThis->UpdateIndexedToNext(h, v, qm);
  }

  bool MarkHeadsToBeReplaced[TOTAL_NUMBER_OF_HEADS][NUMBER_OF_VOLTAGE_MODES][NUMBER_OF_QUALITY_MODES];

  FOR_ALL_QUALITY_MODES(qmI)
     FOR_ALL_VOLTAGE_MODES(vI)
        FOR_ALL_HEADS(hI)
           MarkHeadsToBeReplaced[hI][vI][qmI] = pThis->MarkHeadsToBeReplaced[hI][vI][qmI];

  pThis->CopyToGrid();
  if (pThis->m_EditWTValuesDlg->ShowModal() == mrOk)
  {
      pThis->CopyFromGrid();
  }
  else
  {
    FOR_ALL_QUALITY_MODES(qmI)
       FOR_ALL_VOLTAGE_MODES(vI)
          FOR_ALL_HEADS(hI)
             pThis->MarkHeadsToBeReplaced[hI][vI][qmI] = MarkHeadsToBeReplaced[hI][vI][qmI];
  }
}

/*******************************************************************************
* CopyFromGrid
*******************************************************************************/
int THeadsButtonsEntryFrame::CalcRow(/*int HeadI,*/ int VoltageI, int QualityI)
{
   return YPrintOrder[VoltageI][QualityI];
}

int THeadsButtonsEntryFrame::CalcCol(int HeadI, int VoltageI, int QualityI)
{
   return TOTAL_NUMBER_OF_HEADS*XPrintOrder[VoltageI][QualityI]+HeadI;
}

void THeadsButtonsEntryFrame::CopyFromGrid(void)
{
  int Row, Col;
  FOR_ALL_HEADS(h)
  {
     FOR_ALL_QUALITY_MODES(qm)
     {
        FOR_ALL_VOLTAGE_MODES(v)
        {
             Col = CalcCol(h, v, qm);
             Row = CalcRow(/*h,*/ v, qm);
             if ( m_EditWTValuesDlg->GetEditValue(Row, WEIGHTS, Col) == "")
                continue;

             Weights[h][v][qm]= QStrToFloat(m_EditWTValuesDlg->GetEditValue(Row, WEIGHTS, Col));
             MissingNozzles[h][v][qm]= QStrToInt(m_EditWTValuesDlg->GetEditValue(Row, MISSING_NOZZLES, Col));
        }
     }
  }
  Col = CalcCol(HeadIndex, VoltageIndex, QualityIndex);
  Row = CalcRow(/*HeadIndex,*/ VoltageIndex, QualityIndex);
  for (int i=0; i < NUMBER_OF_INPUTS; i++)
     InputEdits[i]->Text = m_EditWTValuesDlg->GetEditValue(Row, i,Col).c_str();
}

/*******************************************************************************
* CopyToGrid
*******************************************************************************/
void THeadsButtonsEntryFrame::CopyToGrid(void)
{
  m_EditWTValuesDlg->Caption = "Edit entered values";
  int Row, Col;
  FOR_ALL_QUALITY_MODES(qm)
  {
     FOR_ALL_VOLTAGE_MODES(v)
     {
        FOR_ALL_HEADS(h)
        {
               Col = CalcCol(h, v, qm);
               Row = CalcRow(/*h,*/ v, qm);

               m_EditWTValuesDlg->SetEditTag(qm, E_QUALITY_MODE, Row, -1, Col);
               m_EditWTValuesDlg->SetEditTag(v,  E_VOLTAGE,      Row, -1, Col);
               m_EditWTValuesDlg->SetEditTag(h,  E_HEAD,         Row, -1, Col);

               //Current is not applyed yet, so we copy this later manually
               if((h == HeadIndex) && (qm == QualityIndex) && (v == VoltageIndex))
                  continue;

               if(UNINITIALIZED == Weights[h][v][qm])
                  continue;

               m_EditWTValuesDlg->SetEditValue(QIntToStr(MissingNozzles[h][v][qm]), Row, MISSING_NOZZLES, Col);
               m_EditWTValuesDlg->SetEditValue(QFloatToStr(Weights[h][v][qm]), Row, WEIGHTS, Col);

               m_EditWTValuesDlg->SetEditEnabled(!HeadsToBeSkipped[h], Row, -1, Col);
         }
     }
  }
  for (int i=0; i < NUMBER_OF_INPUTS; i++)
     m_EditWTValuesDlg->SetEditValue(InputEdits[i]->Text.c_str(),
                                     CalcRow(/*HeadIndex,*/ VoltageIndex, QualityIndex),
                                     i,
                                     CalcCol(HeadIndex, VoltageIndex, QualityIndex));
}

/*******************************************************************************
* ButtonClear
*******************************************************************************/
void THeadsButtonsEntryFrame::ButtonClear(TSpeedButton* Button)
{
   LOAD_BITMAP(Button->Glyph,IDB_HSW_OK_BUTTON);
   Button->Refresh();
}

/*******************************************************************************
* ButtonMarkSkipped
*******************************************************************************/
void THeadsButtonsEntryFrame::ButtonMarkSkipped(TSpeedButton* Button)
{
   LOAD_BITMAP(Button->Glyph,IDB_HSW_SKIPPED_BUTTON);
   Button->Refresh();
}

/*******************************************************************************
* ButtonMark
*******************************************************************************/
void THeadsButtonsEntryFrame::ButtonMark(TSpeedButton* Button)
{
   LOAD_BITMAP(Button->Glyph,IDB_HSW_CURRENT);
   Button->Refresh();
}

/*******************************************************************************
* UpdateTags
*******************************************************************************/
void THeadsButtonsEntryFrame::UpdateTags(void)
{
  for (int i=0; i < NUMBER_OF_INPUTS; i++)
  {
    QUALITY_MODE(InputEdits[i])= QualityIndex;
    VOLTAGE(InputEdits[i])     = VoltageIndex;
    HEAD(InputEdits[i])        = HeadIndex;
    InputEdits[i]->RowTag  = CalcRow(/*HeadIndex,*/ VoltageIndex, QualityIndex);
    InputEdits[i]->ColTag  = CalcCol(HeadIndex, VoltageIndex, QualityIndex);
    InputEdits[i]->CellTag = i;
  }
}

/*******************************************************************************
* UpdateIndexedToNext
*******************************************************************************/

void THeadsButtonsEntryFrame::UpdateIndexedToNext(void)
{
   UpdateIndexedToNext(HeadIndex, VoltageIndex, QualityIndex, StepsNum, false);
   RefreshButtons();
}

void THeadsButtonsEntryFrame::UpdateIndexedToNext(int& HeadI, int& VoltageI, int& QualityI, int& StepsNumI, bool UpdateGUI)
{
   StepsNumI++;
   if (UpdateGUI && VALIDATE_HEAD(HeadI) && VALIDATE_VOLTAGE_MODE(VoltageI) && VALIDATE_QUALITY_MODE(QualityI))
       ButtonClear(m_Buttons[HeadI][VoltageI][QualityI]);

   UpdateIndexedToNext(HeadI, VoltageI, QualityI);
   if (UpdateGUI) UpdateTags();

   if ( HeadsToBeSkipped[HeadI] )
   {
      if (UpdateGUI) ButtonMarkSkipped(m_Buttons[HeadI][VoltageI][QualityI]);
      UpdateIndexedToNext(HeadI, VoltageI, QualityI, StepsNumI, UpdateGUI);
   }
   else
      if (UpdateGUI) ButtonMark(m_Buttons[HeadI][VoltageI][QualityI]);

}

void THeadsButtonsEntryFrame::UpdateIndexedToNext(int& HeadI, int& VoltageI, int& QualityI)
{
    --VoltageI;
    if (!VALIDATE_VOLTAGE_MODE(VoltageI))
    {
       VoltageI = LAST_VOLTAGE_MODE-1;
       --HeadI;
       if (!VALIDATE_HEAD(HeadI))
       {
          HeadI = LAST_HEAD-1;

          --QualityI;
          if (!VALIDATE_QUALITY_MODE(QualityI))
             QualityI = LAST_QUALITY_MODE-1;
       }
    }
}

/*******************************************************************************
* UpdateIndexedToPrev
*******************************************************************************/
void THeadsButtonsEntryFrame::UpdateIndexedToPrev(void)
{
    StepsNum--;
    if (VALIDATE_HEAD(HeadIndex) && VALIDATE_VOLTAGE_MODE(VoltageIndex) && VALIDATE_QUALITY_MODE(QualityIndex))
        ButtonClear(m_Buttons[HeadIndex][VoltageIndex][QualityIndex]);

   ++VoltageIndex;
   if (!VALIDATE_VOLTAGE_MODE(VoltageIndex))
   {
       VoltageIndex = FIRST_VOLTAGE_MODE;
       ++HeadIndex;
       if (!VALIDATE_HEAD(HeadIndex))
       {
          HeadIndex = FIRST_HEAD;
          ++QualityIndex;
          if (!VALIDATE_QUALITY_MODE(QualityIndex))
             QualityIndex = FIRST_QUALITY_MODE;

          UpdateTags();   

       }
    }
    if ( HeadsToBeSkipped[HeadIndex] )
    {
       ButtonMarkSkipped(m_Buttons[HeadIndex][VoltageIndex][QualityIndex]);
       UpdateIndexedToPrev();
       return;
    }
    else
       ButtonMark(m_Buttons[HeadIndex][VoltageIndex][QualityIndex]);
  RefreshButtons();
}

//---------------------------------------------------------------------------



/*******************************************************************************
* WeightEditKeyPress
*******************************************************************************/
void __fastcall THeadsButtonsEntryFrame::WeightEditKeyPress(TObject *Sender, char &Key)
{
    TEdit *Edit = dynamic_cast<TEdit *>(Sender);

    if (!Edit)
       return;
        
    if (Key == DecimalSeparator)
    {
        if ((Edit->Text).Pos(DecimalSeparator) != 0)
            Key = 0;

        return;
    }
    MissingNozzlesEditKeyPress(Sender,Key);
}

/*******************************************************************************
* DoMissingNozzlesSanityCheck
*******************************************************************************/
void THeadsButtonsEntryFrame::DoMissingNozzlesSanityCheck(TMultiGridEdit* Edit)
{
  if (!Edit)
     return;
  if (Edit->Text == "")
     return;
  if (MarkHeadsToBeReplaced[HEAD(Edit)][VOLTAGE(Edit)][QUALITY_MODE(Edit)])
     return;

  int MissingNozzles = QStrToInt(Edit->Text.c_str());
  CAppParams* ParamMgr = CAppParams::Instance();

  if(MissingNozzles > NOZZLES_PER_HEAD)
	throw EOverMaxExist(QFormatStr(LOAD_STRING(IDS_ENTERED_NOZZLES_OVER_MAX_EXIST),MissingNozzles, NOZZLES_PER_HEAD));
  else if(MissingNozzles > ParamMgr->HSW_MaxMissingNozzles)
  	throw EOverMaxAllowed(QFormatStr(LOAD_STRING(IDS_ENTERED_OVER_MAX_ALLOWED_D), InputLabels[MISSING_NOZZLES]->Caption.c_str(), MissingNozzles, (int)ParamMgr->HSW_MaxMissingNozzles));
}

/*******************************************************************************
* WeightEditExit
*******************************************************************************/
void __fastcall THeadsButtonsEntryFrame::WeightEditExit(TObject *Sender)
{
  TMultiGridEdit* Edit = dynamic_cast<TMultiGridEdit *>(Sender);
  float LowVoltageWeight,HighVoltageWeight;
  int qm = QUALITY_MODE(Edit);
  int h = HEAD(Edit);

  if (!TryStrToFloat(m_EditWTValuesDlg->GetEditValue(CalcRow(/*h, */HIGH_VOLTAGE, qm), WEIGHTS, CalcCol(h, HIGH_VOLTAGE, qm)).c_str(), HighVoltageWeight))
     HighVoltageWeight = UNINITIALIZED;
  if (!TryStrToFloat(m_EditWTValuesDlg->GetEditValue(CalcRow(/*h,*/ LOW_VOLTAGE, qm), WEIGHTS, CalcCol(h, LOW_VOLTAGE, qm)).c_str(), LowVoltageWeight))
     LowVoltageWeight = UNINITIALIZED;

  WeightEditExitEvent(Sender, HighVoltageWeight, LowVoltageWeight);
}

bool __fastcall THeadsButtonsEntryFrame::MainWeightEditExit(TObject *Sender)
{
  TMultiGridEdit* Edit = dynamic_cast<TMultiGridEdit *>(Sender);
  int VoltageIndex = VOLTAGE(Edit);

  float HighVoltageWeight = Weights[HeadIndex][HIGH_VOLTAGE][QualityIndex];
  float LowVoltageWeight  = Weights[HeadIndex][LOW_VOLTAGE][QualityIndex];

  float* pCurrentVoltageWeight = ((VoltageIndex == HIGH_VOLTAGE) ? &HighVoltageWeight : &LowVoltageWeight);

  if (!TryStrToFloat(Edit->Text, *pCurrentVoltageWeight))
     *pCurrentVoltageWeight = UNINITIALIZED;
      
  return WeightEditExitEvent(Sender, HighVoltageWeight, LowVoltageWeight);
}
/*******************************************************************************
* WeightEditExitEvent
*******************************************************************************/
bool __fastcall THeadsButtonsEntryFrame::WeightEditExitEvent(TObject *Sender, float HighVoltageWeight, float LowVoltageWeight)
{
  bool ret = true;
  TMultiGridEdit* Edit = dynamic_cast<TMultiGridEdit *>(Sender);
  try
  {
     DoWeightSanityCheck(Edit, HighVoltageWeight, LowVoltageWeight);
  }
  catch(ECheckingLowVsHighVoltage &ErrMsg)
  {
     if (m_EditWTValuesDlg->Visible)
     {
	 QMonitor.ErrorMessage(ErrMsg.GetErrorMsg(),ORIGIN_WIZARD_PAGE);
		 Edit->SetFocus();
     }
     else
       throw;
     ret = false;
  }
  catch(EHeadsButtonsEntryWizard &ErrMsg)
  {
     ret = OnError(Edit,ErrMsg.GetErrorMsg());
  }
  catch(EQException &ErrMsg)
  {
     ret = OnError(Edit,"",true);
  }
  return ret;
}

/*******************************************************************************
* DoWeightSanityCheck
*******************************************************************************/
void THeadsButtonsEntryFrame::DoWeightSanityCheck(TMultiGridEdit* Edit, float HighVoltageWeight, float LowVoltageWeight)
{
  if (!Edit)
     return;

  /* Formulas by Meir Bar Nathan:
     a_min 	-	MinLayerHeight (HS/HQ)
     a_max	-	MaxLayerHeight (HS/HQ)
     b		-   ProductLT_m
     c		-	ProductLT_n
     d		-	# of nozzles_ head
     e		-	# of missing nozzles in head
     S.G    -   Specific Gravity
     mv     -   MaxVoltage M/S
     tvl    -   HeadTestV_M/S_Low
     tvh    -   HeadTestV_M/S_High
     gg     -   GenericGain_HQ/HS

				((a_min + b) * c - (mv - tvl) * gg) * 0.66 * (d-e) * S.G * Height*Width * # of slices
Min_Weight =    -------------------------------------------------------------------------------------  // Formula 6
										10000 * d

				((a_max + b) * c  + (mv - tvh) * gg) * 1.5 * S.G * Height*Width * # of slices
Max_Weight =	 ------------------------------------------------------------------------------------  // Formula 7

											10000
*/



  float Weight = ((Edit->Text == "") ? 0 : QStrToFloat(Edit->Text.c_str()));
  int   QualityIndex = QUALITY_MODE(Edit);
  int   OperationIndex = HSW_OPERATION_MODE;
  int   HeadIndex    = HEAD(Edit);
  int   VoltageIndex = VOLTAGE(Edit);
  int   ChamberIndex = GetHeadsChamber(HeadIndex,OperationIndex);

  CAppParams * ParamMgr = CAppParams::Instance();
  CBackEndInterface::Instance()->EnterMode(PRINT_MODE[QualityIndex][OperationIndex],
                                           MACHINE_QUALITY_MODES_DIR(QualityIndex,OperationIndex));

  int MissingNozzlesValue = MissingNozzles[HeadIndex][VoltageIndex][QualityIndex];
  if (MissingNozzlesValue == UNINITIALIZED)
     MissingNozzlesValue = 0;

  float HeightBmp = ParamMgr->HSW_BmpHeight;
  float WidthBmp  = ParamMgr->HSW_BmpWidth;
  float MaxLayerHeightGross = ParamMgr->HSW_MaxLayerHeightArray[QualityIndex];
  float MinLayerHeightGross = ParamMgr->HSW_MinLayerHeightArray[QualityIndex];
  float ProductLT_m = ParamMgr->ProductLT_m;
  float ProductLT_n = ParamMgr->ProductLT_n;

  float MaxVoltage      = ParamMgr->HSW_MaxVoltageArray[ChamberIndex];
  float HeadTestV_Low   = ParamMgr->HSW_HeadTestV_LowArray[ChamberIndex];
  float HeadTestV_High  = ParamMgr->HSW_HeadTestV_HighArray[ChamberIndex];

  float MinWeight = ( ( ( MinLayerHeightGross + ProductLT_m ) * ProductLT_n -
                        ( MaxVoltage - HeadTestV_Low) * FLOAT_FROM_RESOURCE_ARRAY(QualityIndex, IDN_GENERIC_GAIN) ) *
						0.66 * (NOZZLES_PER_HEAD - MissingNozzlesValue) *
                        FLOAT_FROM_RESOURCE(IDN_SPECIFIC_GRAVITY) *
		        HeightBmp * WidthBmp * ParamMgr->HSW_NumberOfSlicesArray[QualityIndex])
					/(10000 * NOZZLES_PER_HEAD);
					
    //---------------------------
    // Calculations for Formula 8

    float CarpetForSupportWeight = 0,WeightModelHeadsCarpetPrint  = 0;
    for(int h = FIRST_MODEL_HEAD; h < LAST_MODEL_HEAD; h++)
	 //WeightModelHeadsCarpetPrint += WeightResults[h][CARPET_VOLTAGE][CARPET_MODE_QUALITY_MODE];
	 WeightModelHeadsCarpetPrint += Weights[h][CARPET_VOLTAGE][CARPET_MODE_QUALITY_MODE];

    CarpetForSupportWeight = ParamMgr->HSW_NumberOfSlicesArray[CARPET_MODE_QUALITY_MODE];
    CarpetForSupportWeight *= NUMBER_OF_MODEL_HEADS;
    CarpetForSupportWeight = WeightModelHeadsCarpetPrint / CarpetForSupportWeight;
    CarpetForSupportWeight *= NUMBER_OF_CARPET_SLICES;
	//---------------------------

  float MaxWeight = ( ( ( MaxLayerHeightGross + ProductLT_m) * ProductLT_n +
                        ( MaxVoltage - HeadTestV_High) * FLOAT_FROM_RESOURCE_ARRAY(QualityIndex, IDN_GENERIC_GAIN)) *
                	1.5 * FLOAT_FROM_RESOURCE(IDN_SPECIFIC_GRAVITY) *
                        HeightBmp * WidthBmp * ParamMgr->HSW_NumberOfSlicesArray[QualityIndex])
                        /10000;
						
	if(!ParamMgr->IsModelHead(HeadIndex) ) // carpet weight in case of support
	{
		//QMonitor.Print( QFormatStr( "CarpetWeight = %.3f" , CarpetForSupportWeight ) );
		CQLog::Write(LOG_TAG_GENERAL, QFormatStr("CarpetWeight = %.3f", CarpetForSupportWeight));
		MaxWeight += CarpetForSupportWeight;
	}

  CBackEndInterface::Instance()->GoOneModeBack();

  if(Weight < MinWeight)
  	throw EHeadsButtonsEntryWizard(QFormatStr(LOAD_STRING(IDS_ENTERED_BELOW_MIN), InputLabels[WEIGHTS]->Caption.c_str(), Weight, MinWeight));

  if(Weight > MaxWeight)
  	throw EHeadsButtonsEntryWizard(QFormatStr(LOAD_STRING(IDS_ENTERED_OVER_MAX_ALLOWED_F), InputLabels[WEIGHTS]->Caption.c_str(), Weight, MaxWeight));

  if (UNINITIALIZED == LowVoltageWeight)
     return;
  if (UNINITIALIZED == HighVoltageWeight)
     return;
     
  if(LowVoltageWeight > HighVoltageWeight)
  	throw ECheckingLowVsHighVoltage(QFormatStr("The weight you entered (%.2f) conflicts with the weight of the previous sample (%.2f)."\
  					                           "\n1. Weigh this sample again." \
  					                           "\n2. If the weight is the same as before, weigh the previous sample again." \
  					                           "\n3. To correct the previous weight, click 'Edit Data'.", Weight, ((Weight == LowVoltageWeight) ? HighVoltageWeight : LowVoltageWeight)));
}//DoWeightSanityCheck

/*******************************************************************************
* MissingNozzlesEditExit
*******************************************************************************/
void __fastcall THeadsButtonsEntryFrame::MissingNozzlesEditExit(TObject *Sender)
{
  MissingNozzlesEditExitEvent(Sender);
}

/*******************************************************************************
* MainMissingNozzlesEditExit
*******************************************************************************/
void __fastcall THeadsButtonsEntryFrame::MainMissingNozzlesEditExit(TObject *Sender)
{
  TMultiGridEdit* Edit = dynamic_cast<TMultiGridEdit *>(Sender);
  if (Edit->Text == "")
     Edit->Text = "0";
}

/*******************************************************************************
* OnError
*******************************************************************************/
bool THeadsButtonsEntryFrame::OnError(TMultiGridEdit* Edit, QString Msg, bool Silent)
{
   if (!Silent)
     QMonitor.ErrorMessage(Msg,ORIGIN_WIZARD_PAGE);

   MarkHeadsToBeReplaced[HEAD(Edit)][VOLTAGE(Edit)][QUALITY_MODE(Edit)] = false;
   Edit->Text = "";
   Edit->SetFocus();
   return false;
}

bool THeadsButtonsEntryFrame::YesNoOnError(TMultiGridEdit* Edit, QString Msg, bool Silent, bool SilentAnswer)
{
  bool ret = true;
  if ( (Silent) ? SilentAnswer : QMonitor.AskYesNo(Msg) )
  {
    MarkHeadsToBeReplaced[HEAD(Edit)][VOLTAGE(Edit)][QUALITY_MODE(Edit)] = true;
    MissingNozzles[HEAD(Edit)][VOLTAGE(Edit)][QUALITY_MODE(Edit)] = StrToFloat(InputEdits[MISSING_NOZZLES]->Text);
    ret = true;
  }
  else
    ret = OnError(Edit,Msg, true);
  return ret;
}

float THeadsButtonsEntryFrame::GetCurrentWeight(void)
{ 
    return Weights[HeadIndex][VoltageIndex][QualityIndex];
}

int THeadsButtonsEntryFrame::GetCurrentMissingNozzlesValue()
{
    return MissingNozzles[HeadIndex][VoltageIndex][QualityIndex];
}

/*******************************************************************************
* IsHeadMarkedReplaced
*******************************************************************************/
bool THeadsButtonsEntryFrame::IsHeadMarkedReplaced(int HeadIndex)
{
   if (!VALIDATE_HEAD(HeadIndex))
      throw "IsHeadMarkedReplaced : Wrong parameter value";
   FOR_ALL_QUALITY_MODES(qm)
      FOR_ALL_VOLTAGE_MODES(v)
          if (MarkHeadsToBeReplaced[HeadIndex][v][qm])
             return true;
   return false;
}

/*******************************************************************************
* MissingNozzlesEditExitEvent
*******************************************************************************/
bool __fastcall THeadsButtonsEntryFrame::MissingNozzlesEditExitEvent(TObject *Sender/*, bool Silent*/)
{
  bool ret = true;
  TMultiGridEdit* Edit = dynamic_cast<TMultiGridEdit *>(Sender);
  try
  {
     DoMissingNozzlesSanityCheck(Edit);
     MarkHeadsToBeReplaced[HEAD(Edit)][VOLTAGE(Edit)][QUALITY_MODE(Edit)] = false;
  }                                                                   

  catch(EOverMaxAllowed &ErrMsg)
  {
     ret = YesNoOnError(Edit,ErrMsg.GetErrorMsg() + ",indicating that the head should be replaced." \
                                                    "Make sure the number is correct before continuing." \
                                                    "\nDo you want to continue, using the number you entered ?",
                                                    IsHeadMarkedReplaced(HEAD(Edit)),
                                                    true);
  }
  catch(EOverMaxExist &ErrMsg)
  {
     YesNoOnError(Edit,ErrMsg.GetErrorMsg() + "\nDo you want to continue, using the total number of nozzles?");
     ret = false;
  }
  return ret;
}



/*******************************************************************************
* MissingNozzlesStrToInt
*******************************************************************************/
int THeadsButtonsEntryFrame::MissingNozzlesStrToInt()
{
   int res = 0;
   if (InputEdits[MISSING_NOZZLES]->Text != "")
       res = QStrToInt(InputEdits[MISSING_NOZZLES]->Text.c_str());
   if (res > NOZZLES_PER_HEAD)
       res = NOZZLES_PER_HEAD;
   return res;
}



CHeadsButtonsEntryFrameWizardPageViewer::CHeadsButtonsEntryFrameWizardPageViewer() :
m_HeadsButtonsEntryFrame(NULL)
{}

CHeadsButtonsEntryFrameWizardPageViewer::~CHeadsButtonsEntryFrameWizardPageViewer()
{}

#define VALUE_TO_EDIT_TEXT(_func_, _val_) ((UNINITIALIZED == _val_) ? "" : _func_(_val_).c_str())

void CHeadsButtonsEntryFrameWizardPageViewer::UpdateGotoNextIndex(void)
{
   m_HeadsButtonsEntryFrame->UpdateIndexedToNext();
   GotoNextIndex();
}

void CHeadsButtonsEntryFrameWizardPageViewer::GotoNextIndex(void)
{
   if ( m_HeadsButtonsEntryFrame->HeadsToBeSkipped[m_HeadsButtonsEntryFrame->HeadIndex] )
   {
       UpdateGotoNextIndex();
       return;
   }
   GetParentWizard()->EnableDisableUserButton1(m_HeadsButtonsEntryFrame->StepsNum == 0);
   GetParentWizard()->EnableDisableUserButton1(true);
   GetParentWizard()->EnableDisableNext(false);
   m_HeadsButtonsEntryFrame->InputEdits[WEIGHTS]->Text = VALUE_TO_EDIT_TEXT(QFloatToStr,m_HeadsButtonsEntryFrame->GetCurrentWeight());
   m_HeadsButtonsEntryFrame->InputEdits[MISSING_NOZZLES]->Text = VALUE_TO_EDIT_TEXT(QIntToStr,m_HeadsButtonsEntryFrame->GetCurrentMissingNozzlesValue());
   m_HeadsButtonsEntryFrame->InputEdits[MISSING_NOZZLES]->SetFocus();
}

void CHeadsButtonsEntryFrameWizardPageViewer::Prepare(TWinControl *PageWindow,CWizardPage *WizardPage)
{
	CHeadsButtonsEntryPage* Page = dynamic_cast<CHeadsButtonsEntryPage *>(WizardPage);
	m_HeadsButtonsEntryFrame = new THeadsButtonsEntryFrame(PageWindow,
                                                           Page->GetWeightsPtr(),
                                                           Page->GetMissingNozzlesPtr(),
                                                           Page->GetStepsNum(),
                                                           Page->GetQualityIndex(),
                                                           Page->GetVoltageIndex(),
                                                           Page->GetHeadIndex(),
                                                           Page->GetHeadsToSkipped(),
                                                           Page->GetMarkHeadsToReplace());

    m_HeadsButtonsEntryFrame->m_EditWTValuesDlg = new TMultiGridForm(PageWindow,(MaxXPosition+1)*TOTAL_NUMBER_OF_HEADS,MaxYPosition+1,
    /*
                                           TOTAL_NUMBER_OF_HEADS*INT_FROM_RESOURCE(IDN_HSW_MAX_PRINT_NUM_IN_AXIS_X),
                                           NUMBER_OF_QUALITY_MODES*NUMBER_OF_VOLTAGE_MODES/INT_FROM_RESOURCE(IDN_HSW_MAX_PRINT_NUM_IN_AXIS_X),
    */                                           
                                           NUMBER_OF_INPUTS,
                                           NUMBER_OF_TAGS);

    m_HeadsButtonsEntryFrame->Parent = PageWindow;
    m_HeadsButtonsEntryFrame->ApplyButton->OnClick = ApplyButtonClick;
    Page->EnterEditModeEventPtr = &(m_HeadsButtonsEntryFrame->EnterEditMode);
    Page->EnterEditModeEventArg = m_HeadsButtonsEntryFrame;
    GotoNextIndex();
}





bool CHeadsButtonsEntryFrameWizardPageViewer::CheckPreviousButtonStatus(CWizardPage *WizardPage)
{
   bool IsPreviousDisabled = (WizardPage->GetAttributes() & wpPreviousDisabled);
   return (!IsPreviousDisabled | m_HeadsButtonsEntryFrame->CheckPreviousButtonStatus());
}
//---------------------------------------------------------------------------



