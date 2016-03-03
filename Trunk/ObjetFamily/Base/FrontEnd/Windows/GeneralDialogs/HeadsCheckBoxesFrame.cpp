//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "HeadsCheckBoxesFrame.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

THeadsCheckBoxFrame* HeadsCheckBoxFrame;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
__fastcall THeadsCheckBoxFrame::THeadsCheckBoxFrame(TComponent* Owner)
        : TFrame(Owner)
{
    int SpaceX = (ChooseHeadsGroupBox->Width - TOTAL_NUMBER_OF_HEADS/2*17/*Default checkBox Width*/)/ (TOTAL_NUMBER_OF_HEADS+1);
    int DefaultCheckBoxWidth = 50;
    FOR_ALL_DOUBLE_HEADS(i)
    {
	CheckBoxArray[i]           = new TCheckBox(this);
	LabelArray[i]              = new TLabel(this);
	CheckBoxArray[i]->Parent   = ChooseHeadsGroupBox;
	CheckBoxArray[i]->Tag      = i;
	CheckBoxArray[i]->TabOrder = i;
	LabelArray[i]->Left        = SpaceX + (SpaceX + DefaultCheckBoxWidth)*i;
	CheckBoxArray[i]->Left     = LabelArray[i]->Left + 15;
    CheckBoxArray[i]->Width    = 20;
	CheckBoxArray[i]->Top      = 40;
//fixme	          CheckBoxArray[i]->Action   ;

	LabelArray[i]->Parent      = ChooseHeadsGroupBox;
	LabelArray[i]->Width       = CheckBoxArray[i]->Width;
   //	LabelArray[i]->Left        = CheckBoxArray[i]->Left;
	LabelArray[i]->Top         = CheckBoxArray[i]->Top-20;
	LabelArray[i]->Caption     = GetDoubleHeadName(i).c_str();
	LabelArray[i]->AutoSize    = false;
	LabelArray[i]->Alignment   = taCenter;
    }
}
//---------------------------------------------------------------------------

// Setting the checkboxes according to the input
void THeadsCheckBoxFrame::SetCheckedBoxes (bool CheckedArray[])
{
  FOR_ALL_DOUBLE_HEADS(i)
     CheckBoxArray[i]->Checked = CheckedArray[i];
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void THeadsCheckBoxFrame::GetCheckedBoxes (bool OutputArray[])
{
  FOR_ALL_DOUBLE_HEADS(i)
	 OutputArray[i] = CheckBoxArray[i]->Checked;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void __fastcall CHeadsCheckBoxesPageViewer::CheckBoxOnClick(TObject *Sender)
{
   TCheckBox* CheckBox = dynamic_cast<TCheckBox *>(Sender);

   	#ifdef OBJET_MACHINE_KESHET
 //	for(int i = 0; i < TOTAL_NUMBER_OF_HEATERS; i++)
//	{
//		if((i%2 != 0)&&(i<TOTAL_NUMBER_OF_HEADS))
//		{
 //		m_HeadsCheckBoxFrame->CheckBoxArray[i]->Checked =  m_HeadsCheckBoxFrame->CheckBoxArray[i-1]->Checked;
 //		}
//	}
	#endif
  // Update only if the origin is Checkbox
  if (CheckBox)
  {
     if (CheckBox->Checked)
       m_NoOfCheckedCheckboxes++;
     else
       m_NoOfCheckedCheckboxes--;

    GetParentWizard()->EnableDisableNext(m_NoOfCheckedCheckboxes > 0? true : false);
    GetActivePage()->NotifyEvent(0,0);
  }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void CHeadsCheckBoxesPageViewer::Prepare(TWinControl *PageWindow,CWizardPage *WizardPage)
{
  m_NoOfCheckedCheckboxes = 0;

  m_HeadsCheckBoxFrame         = new THeadsCheckBoxFrame(PageWindow);
  m_HeadsCheckBoxFrame->Parent = PageWindow;

  CHeadsCheckboxesWizardPage *Page = dynamic_cast<CHeadsCheckboxesWizardPage *>(WizardPage);
  m_HeadsCheckBoxFrame->SubTitleLabel->Caption = Page->SubTitle.c_str();

  m_HeadsCheckBoxFrame->SetCheckedBoxes(Page->CheckedArray);
  FOR_ALL_DOUBLE_HEADS(i)
  {
      m_HeadsCheckBoxFrame->CheckBoxArray[i]->OnClick = CheckBoxOnClick;
	  if (m_HeadsCheckBoxFrame->CheckBoxArray[i]->Checked)
		 m_NoOfCheckedCheckboxes++;
  	#ifdef OBJET_MACHINE_KESHET
  //	 if((i%2 != 0)&&(i<TOTAL_NUMBER_OF_HEADS))
  //	   m_HeadsCheckBoxFrame->CheckBoxArray[i]->Enabled = false;
	 #endif
  }

}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void CHeadsCheckBoxesPageViewer::Refresh(TWinControl *PageWindow,CWizardPage *WizardPage)
{}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void CHeadsCheckBoxesPageViewer::Leave(TWinControl *PageWindow,CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason)
{
  if (LeaveReason != lrGoNext)
     return;

  CHeadsCheckboxesWizardPage *Page = dynamic_cast<CHeadsCheckboxesWizardPage *>(WizardPage);
  m_HeadsCheckBoxFrame->GetCheckedBoxes(Page->CheckedArray);

  if (m_HeadsCheckBoxFrame)
  {
    delete m_HeadsCheckBoxFrame;
    m_HeadsCheckBoxFrame = NULL;
  }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
