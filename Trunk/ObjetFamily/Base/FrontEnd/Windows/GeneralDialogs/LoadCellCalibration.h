#ifndef LoadCellCalibrationH
#define LoadCellCalibrationH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "WizardViewer.h"
#include "CustomLoadCellCalibrationPage.h"
#include <Graphics.hpp>
#include "AppParams.h"
#include <ComCtrls.hpp>

//---------------------------------------------------------------------------
class TLoadCellCalibrationCheckBoxFrame : public TFrame
{
__published:	// IDE-managed Components
	TLabel *SubTitle;
	TGroupBox *GroupBox1;
	TImage *Image1;
	TImage *Image3;
	TLabel *Label1;
	TLabel *Label2;
private:	// User declarations
		int m_numOfActualContainers;
public:		// User declarations
		TCheckBox* m_containersChecks[TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE];

		__fastcall TLoadCellCalibrationCheckBoxFrame(TComponent* Owner);
		bool CheckBoxArray[TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE];
		void GetCheckedBoxes (bool OutputArray[]);
		void SetCheckedBoxes (bool CheckedArray[]);
		void SetSubTitle(String subTytle);
};
//---------------------------------------------------------------------------
extern PACKAGE TLoadCellCalibrationCheckBoxFrame* LoadCellCalibrationCheckBoxFrame;
class CLoadCellCalibrationFramePageViewer : public CCustomWizardPageViewer {
  private:
	TLoadCellCalibrationCheckBoxFrame *m_TLoadCellCalibration;
	int m_NoOfCheckedCheckboxes;
	 void __fastcall CheckBoxOnClick(TObject *Sender);

  public:
	 CLoadCellCalibrationFramePageViewer()
	 {
     	m_TLoadCellCalibration = NULL;
     	m_NoOfCheckedCheckboxes = 0 ;
     }
	void Prepare(TWinControl *PageWindow,CWizardPage *WizardPage)
	{
	  m_TLoadCellCalibration         = new TLoadCellCalibrationCheckBoxFrame(PageWindow);
	  m_TLoadCellCalibration->Parent = PageWindow;
	  GetParentWizard()->EnableDisableNext(false);
	  m_NoOfCheckedCheckboxes = 0;

	  for(int i = 0; i < TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE; i++)
	  {
		m_TLoadCellCalibration->m_containersChecks[i]->OnClick = CheckBoxOnClick;
	  }
	  
	  Refresh(PageWindow,WizardPage);
	}

	void Refresh(TWinControl *PageWindow,CWizardPage *WizardPage)
	{
	  CCustomLoadCellCalibrationPage* Page = dynamic_cast<CCustomLoadCellCalibrationPage*>(WizardPage);
	  bool activeCheckBoxes[TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE];
	  
	  Page->GetActiveCheckBoxes(activeCheckBoxes);
	
	  //m_TLoadCellCalibration->SetCheckedBoxes(activeCheckBoxes);
	  String subTytle = Page->GetSubTitle();
	  m_TLoadCellCalibration->SubTitle->Caption = subTytle;

	  for (int i = 0; i < TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE; i++)
	  {
         m_TLoadCellCalibration->m_containersChecks[i]->Enabled = activeCheckBoxes[i];
         m_TLoadCellCalibration->m_containersChecks[i]->Visible = activeCheckBoxes[i];
	  }
      if (!(CAppParams::Instance()->DualWasteEnabled))
      { // override second waste
          m_TLoadCellCalibration->m_containersChecks[TYPE_TANK_WASTE_RIGHT]->Visible = false;
      }
      PageWindow->Repaint();
	}

	void Leave(TWinControl *PageWindow,CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason)
	{
	  if (m_TLoadCellCalibration)
	  {
		CCustomLoadCellCalibrationPage* Page = dynamic_cast<CCustomLoadCellCalibrationPage*>(WizardPage);
		bool OutputArray[TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE];
		m_TLoadCellCalibration->GetCheckedBoxes(OutputArray);
		Page->SetCartridgesArray(OutputArray);
	  //	delete m_TLoadCellCalibration;
    //    m_TLoadCellCalibration = NULL;
      }
	}
	void SetSubTytle(String subTytle)
	{
		m_TLoadCellCalibration->SetSubTitle(subTytle);
	}
};

#endif
