//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop


#include "XOffsetChooseBestPrintFrame.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
#include "FEResources.h"

TXOffsetChooseBestPrintFram *XOffsetChooseBestPrintFram;


//---------------------------------------------------------------------------
__fastcall TXOffsetChooseBestPrintFram::TXOffsetChooseBestPrintFram(TComponent* Owner)
        : TFrame(Owner)
{
   SubTitleLabel->Caption = LOAD_STRING(IDS_XO_CHOOSE_BEST);
   Refresh();
}
//---------------------------------------------------------------------------
int TXOffsetChooseBestPrintFram::GetSelectedPrint()
{
  TRadioButton *MyButton = NULL;
  TComponent   *MyComp   = NULL;

  int CheckedButton = -1;

  for(int i = 0; i < ChooseBestPrintGroupBox->ControlCount; i++)
  {
    MyComp = ChooseBestPrintGroupBox->Controls[i];

    if((MyButton = dynamic_cast<TRadioButton *>(MyComp)) != 0)
    {
      if(MyButton->Checked)
      {
        CheckedButton = MyButton->Tag;
      }
    }
  }

  return CheckedButton;
}
//---------------------------------------------------------------------------
void TXOffsetChooseBestPrintFram::SetSelection(int Selection)
{
  TRadioButton *MyButton = NULL;
  TComponent   *MyComp   = NULL;

  for(int i = 0; i < ChooseBestPrintGroupBox->ControlCount; i++)
  {
    MyComp = ChooseBestPrintGroupBox->Controls[i];

    // In case this component is a RadioButton
    if((MyButton = dynamic_cast<TRadioButton *>(MyComp)) != 0)
    {
      // In case this RadioButton is the right one...
      if(MyButton->Tag == Selection)
      {
        MyButton->Checked = true;
      }
    }
  }
}
//---------------------------------------------------------------------------
void CCustomXOffsetSelectPrintWizardPageViewer::Prepare(TWinControl *PageWindow,CWizardPage *WizardPage)
{
  m_XOffsetChooseBestPrintFrame = new TXOffsetChooseBestPrintFram(PageWindow);
  m_XOffsetChooseBestPrintFrame->Parent = PageWindow;

  CXOffsetWizardPage* Page = dynamic_cast<CXOffsetWizardPage *>(WizardPage);
  m_XOffsetChooseBestPrintFrame->SetSelection(Page->ChosenPrintLine);
}
//---------------------------------------------------------------------------


/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
 void CCustomXOffsetSelectPrintWizardPageViewer::Refresh(TWinControl *PageWindow,CWizardPage *WizardPage)
{
  CXOffsetWizardPage* Page = dynamic_cast<CXOffsetWizardPage *>(WizardPage);
  m_XOffsetChooseBestPrintFrame->SetSelection(Page->ChosenPrintLine);
}
//---------------------------------------------------------------------------
void CCustomXOffsetSelectPrintWizardPageViewer::Leave(TWinControl *PageWindow,CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason)
{
  if (LeaveReason == lrCanceled)
     return;

  CXOffsetWizardPage* Page = dynamic_cast<CXOffsetWizardPage *>(WizardPage);
  Page->ChosenPrintLine = m_XOffsetChooseBestPrintFrame->GetSelectedPrint();

  if (m_XOffsetChooseBestPrintFrame)
  {
    delete m_XOffsetChooseBestPrintFrame;
    m_XOffsetChooseBestPrintFrame = NULL;
  }
}
/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
