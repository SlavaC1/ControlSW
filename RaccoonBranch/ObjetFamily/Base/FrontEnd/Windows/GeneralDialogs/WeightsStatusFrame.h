
#ifndef WeightsStatusFrameH
#define WeightsStatusFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "WizardViewer.h"
#include "WeightsStatusWizardPage.h"
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
class TWeightsStatusFrame : public TFrame
{
__published:	// IDE-managed Components
		TCheckBox *ValuesStableCheckBox;
		TPanel *Model1LeftPNL;
		TPanel *Model2LeftPNL;
		TPanel *Model3LeftPNL;
		TPanel *SupportLeftPNL;
		TPanel *Model1RightPNL;
		TPanel *Model2RightPNL;
		TPanel *Model3RightPNL;
		TPanel *SupportRightPNL;
		TGroupBox *WeightsStatusGroupBox;
		TPanel *WastePNL;
	TImage *Image1;
	TLabel *Caption;
		//---------------------------------------------------------------------------

private:	// User declarations
public:		// User declarations
		__fastcall TWeightsStatusFrame(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TWeightsStatusFrame *WeightsStatusFrame;
//---------------------------------------------------------------------------

class CWeightsStatusPageViewer : public CCustomWizardPageViewer {
  private:
	TWeightsStatusFrame *m_WeightsStatusFrame;

    void __fastcall CheckBoxOnClick(TObject *Sender)
    {
      TCheckBox *MyCheckBox = dynamic_cast<TCheckBox *>(Sender);

      // Update only if the origin is Checkbox
      if (MyCheckBox)
      {
        GetParentWizard()->EnableDisableNext(MyCheckBox->Checked? true : false);
        GetActivePage()->NotifyEvent(0,0);
      }
    };


  public:
	void Prepare(TWinControl *PageWindow,CWizardPage *WizardPage)
    {
	  m_WeightsStatusFrame         = new TWeightsStatusFrame(PageWindow);
	  m_WeightsStatusFrame->Parent = PageWindow;

	  m_WeightsStatusFrame->ValuesStableCheckBox->OnClick = CheckBoxOnClick;

	  CWeightsStatusWizardPage *Page = dynamic_cast<CWeightsStatusWizardPage *>(WizardPage);
	  m_WeightsStatusFrame->Caption->Caption = Page->GetSubTitle().c_str();
	}

    void Refresh(TWinControl *PageWindow,CWizardPage *WizardPage)
    {
      int PanelLeftPos;
      int PanelWidth;
      int UnitsLabelWidth;
      int UnitsLabelNewPos;
	  CWeightsStatusWizardPage *Page = dynamic_cast<CWeightsStatusWizardPage *>(WizardPage);
	  if(m_WeightsStatusFrame == NULL)
	  	return;
	//  m_WeightsStatusFrame->UnitsLabel->Caption           = Page->UnitsCaption.c_str();
#ifdef OBJET_MACHINE
	  m_WeightsStatusFrame->ValuesStableCheckBox->Caption = "The active values above have stabilized.";
#else
	  m_WeightsStatusFrame->ValuesStableCheckBox->Caption = "Values are stable";
#endif

	  m_WeightsStatusFrame->Model1LeftPNL->Caption = Page->m_WeightsStatusWizardPageData[TYPE_TANK_MODEL1]->m_CurrValue;
	  if(Page->m_WeightsStatusWizardPageData[TYPE_TANK_MODEL1]->m_enable)
		m_WeightsStatusFrame->Model1LeftPNL->Font->Color = clGray;
	  else
		m_WeightsStatusFrame->Model1LeftPNL->Font->Color = clWindowText;

	  m_WeightsStatusFrame->Model2LeftPNL->Caption = Page->m_WeightsStatusWizardPageData[TYPE_TANK_MODEL3]->m_CurrValue;
	  if(Page->m_WeightsStatusWizardPageData[TYPE_TANK_MODEL3]->m_enable)
		m_WeightsStatusFrame->Model2LeftPNL->Font->Color = clGray;
	  else
		m_WeightsStatusFrame->Model2LeftPNL->Font->Color = clWindowText;

	  m_WeightsStatusFrame->Model3LeftPNL->Caption = Page->m_WeightsStatusWizardPageData[TYPE_TANK_MODEL5]->m_CurrValue;
	  if(Page->m_WeightsStatusWizardPageData[TYPE_TANK_MODEL5]->m_enable)
		m_WeightsStatusFrame->Model3LeftPNL->Font->Color = clGray;
	  else
		m_WeightsStatusFrame->Model3LeftPNL->Font->Color = clWindowText;

	  m_WeightsStatusFrame->SupportLeftPNL->Caption = Page->m_WeightsStatusWizardPageData[TYPE_TANK_SUPPORT1]->m_CurrValue;
	  if(Page->m_WeightsStatusWizardPageData[TYPE_TANK_SUPPORT1]->m_enable)
		m_WeightsStatusFrame->SupportLeftPNL->Font->Color = clGray;
	  else
		m_WeightsStatusFrame->SupportLeftPNL->Font->Color = clWindowText;

	  m_WeightsStatusFrame->Model1RightPNL->Caption = Page->m_WeightsStatusWizardPageData[TYPE_TANK_MODEL2]->m_CurrValue;
	  if(Page->m_WeightsStatusWizardPageData[TYPE_TANK_MODEL2]->m_enable)
		m_WeightsStatusFrame->Model1RightPNL->Font->Color = clGray;
	  else
		m_WeightsStatusFrame->Model1RightPNL->Font->Color = clWindowText;

	  m_WeightsStatusFrame->Model2RightPNL->Caption = Page->m_WeightsStatusWizardPageData[TYPE_TANK_MODEL4]->m_CurrValue;
	  if(Page->m_WeightsStatusWizardPageData[TYPE_TANK_MODEL4]->m_enable)
		m_WeightsStatusFrame->Model2RightPNL->Font->Color = clGray;
	  else
		m_WeightsStatusFrame->Model2RightPNL->Font->Color = clWindowText;

	  m_WeightsStatusFrame->Model3RightPNL->Caption = Page->m_WeightsStatusWizardPageData[TYPE_TANK_MODEL6]->m_CurrValue;
	   if(Page->m_WeightsStatusWizardPageData[TYPE_TANK_MODEL6]->m_enable)
		m_WeightsStatusFrame->Model3RightPNL->Font->Color = clGray;
	  else
		m_WeightsStatusFrame->Model3RightPNL->Font->Color = clWindowText;

	  m_WeightsStatusFrame->SupportRightPNL->Caption = Page->m_WeightsStatusWizardPageData[TYPE_TANK_SUPPORT2]->m_CurrValue;
	  if(Page->m_WeightsStatusWizardPageData[TYPE_TANK_SUPPORT2]->m_enable)
		m_WeightsStatusFrame->SupportRightPNL->Font->Color = clGray;
	  else
		m_WeightsStatusFrame->SupportRightPNL->Font->Color = clWindowText;

	  m_WeightsStatusFrame->WastePNL->Caption = Page->m_WeightsStatusWizardPageData[TYPE_TANK_WASTE]->m_CurrValue;
       if(Page->m_WeightsStatusWizardPageData[TYPE_TANK_WASTE]->m_enable)
		m_WeightsStatusFrame->WastePNL->Font->Color = clGray;
	  else
		m_WeightsStatusFrame->WastePNL->Font->Color = clWindowText;
	}

    void Leave(TWinControl *PageWindow,CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason)
    {
	  if (m_WeightsStatusFrame)
      {
		delete m_WeightsStatusFrame;
		m_WeightsStatusFrame = NULL;
      }
    }
};

//---------------------------------------------------------------------------
#endif
