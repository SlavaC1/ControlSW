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
#define NUM_OF_CHECKBOX  9
//---------------------------------------------------------------------------
class TLoadCellCalibrationCheckBoxFrame : public TFrame
{
__published:	// IDE-managed Components
	TCheckBox *Model1LeftCHBX;
	TCheckBox *Model2LeftCHBX;
	TCheckBox *Model3LeftCHBX;
	TCheckBox *SupportLeftCHBX;
	TCheckBox *Model1RightCHBX;
	TCheckBox *Model2RightCHBX;
	TCheckBox *Model3RightCHBX;
	TCheckBox *SupportRightCHBX;
	TCheckBox *WasteCHBX;
	TPanel *LoadCellCalibrationCheckbox;
	TImage *Image1;
	TLabel *SubTitle;

private:	// User declarations
public:		// User declarations

		__fastcall TLoadCellCalibrationCheckBoxFrame(TComponent* Owner);
		bool CheckBoxArray[NUM_OF_CHECKBOX];
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
	 CLoadCellCalibrationFramePageViewer(){
	 }
	void Prepare(TWinControl *PageWindow,CWizardPage *WizardPage)
	{
	  m_TLoadCellCalibration         = new TLoadCellCalibrationCheckBoxFrame(PageWindow);
	  m_TLoadCellCalibration->Parent = PageWindow;
	  GetParentWizard()->EnableDisableNext(false);
	  m_NoOfCheckedCheckboxes = 0;
	  m_TLoadCellCalibration->Model1LeftCHBX->OnClick = CheckBoxOnClick;
	  m_TLoadCellCalibration->Model2LeftCHBX->OnClick = CheckBoxOnClick;
	  m_TLoadCellCalibration->Model3LeftCHBX->OnClick = CheckBoxOnClick;
	  m_TLoadCellCalibration->SupportLeftCHBX->OnClick = CheckBoxOnClick;
	  m_TLoadCellCalibration->Model1RightCHBX->OnClick = CheckBoxOnClick;
	  m_TLoadCellCalibration->Model2RightCHBX->OnClick = CheckBoxOnClick;
	  m_TLoadCellCalibration->Model3RightCHBX->OnClick = CheckBoxOnClick;
	  m_TLoadCellCalibration->SupportRightCHBX->OnClick = CheckBoxOnClick;
	  m_TLoadCellCalibration->WasteCHBX->OnClick = CheckBoxOnClick;
	  Refresh(PageWindow,WizardPage);
	}

	void Refresh(TWinControl *PageWindow,CWizardPage *WizardPage)
	{
		CCustomLoadCellCalibrationPage* Page = dynamic_cast<CCustomLoadCellCalibrationPage*>(WizardPage);
	  bool activeCheckBoxes[NUM_OF_CHECKBOX];
	  String subTytle = "";
	  Page->GetActiveCheckBoxes(activeCheckBoxes);
	  subTytle = Page->GetSubTitle();
	  m_TLoadCellCalibration->SubTitle->Caption = subTytle;
	  m_TLoadCellCalibration->Model1LeftCHBX->Enabled = activeCheckBoxes[TYPE_TANK_MODEL1];
	  m_TLoadCellCalibration->Model2LeftCHBX->Enabled = activeCheckBoxes[TYPE_TANK_MODEL3];
	  m_TLoadCellCalibration->Model3LeftCHBX->Enabled = activeCheckBoxes[TYPE_TANK_MODEL5];
	  m_TLoadCellCalibration->SupportLeftCHBX->Enabled = activeCheckBoxes[TYPE_TANK_SUPPORT1];
	  m_TLoadCellCalibration->Model1RightCHBX->Enabled = activeCheckBoxes[TYPE_TANK_MODEL2];
	  m_TLoadCellCalibration->Model2RightCHBX->Enabled = activeCheckBoxes[TYPE_TANK_MODEL4];
	  m_TLoadCellCalibration->Model3RightCHBX->Enabled = activeCheckBoxes[TYPE_TANK_MODEL6];
	  m_TLoadCellCalibration->SupportRightCHBX->Enabled = activeCheckBoxes[TYPE_TANK_SUPPORT2];
	  m_TLoadCellCalibration->WasteCHBX->Enabled = activeCheckBoxes[TYPE_TANK_WASTE];
      PageWindow->Repaint();
	}

	void Leave(TWinControl *PageWindow,CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason)
	{
	  if (m_TLoadCellCalibration)
	  {
		CCustomLoadCellCalibrationPage* Page = dynamic_cast<CCustomLoadCellCalibrationPage*>(WizardPage);
		bool OutputArray[NUM_OF_CHECKBOX];
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
