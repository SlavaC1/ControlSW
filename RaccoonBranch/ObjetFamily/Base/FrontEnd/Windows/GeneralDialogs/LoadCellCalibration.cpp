//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "LoadCellCalibration.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TLoadCellCalibrationCheckBoxFrame *LoadCellCalibrationCheckBoxFrame;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void __fastcall CLoadCellCalibrationFramePageViewer::CheckBoxOnClick(TObject *Sender)
{
   TCheckBox* CheckBox = dynamic_cast<TCheckBox *>(Sender);

  // Update only if the origin is Checkbox
  if (CheckBox)
  {
	if(CheckBox->Name == "Model1LeftCHBX")
		m_TLoadCellCalibration->CheckBoxArray[TYPE_TANK_MODEL1] = CheckBox->Checked;
	else if(CheckBox->Name == "Model2LeftCHBX")
		m_TLoadCellCalibration->CheckBoxArray[TYPE_TANK_MODEL3] = CheckBox->Checked;
	else if (CheckBox->Name == "Model3LeftCHBX")
		m_TLoadCellCalibration->CheckBoxArray[TYPE_TANK_MODEL5] = CheckBox->Checked;
	else if (CheckBox->Name == "SupportLeftCHBX")
		m_TLoadCellCalibration->CheckBoxArray[TYPE_TANK_SUPPORT1] = CheckBox->Checked;
	else if(CheckBox->Name == "Model1RightCHBX")
		m_TLoadCellCalibration->CheckBoxArray[TYPE_TANK_MODEL2] = CheckBox->Checked;
	else if(CheckBox->Name == "Model2RightCHBX")
		m_TLoadCellCalibration->CheckBoxArray[TYPE_TANK_MODEL4] = CheckBox->Checked;
	else if(CheckBox->Name == "Model3RightCHBX")
		m_TLoadCellCalibration->CheckBoxArray[TYPE_TANK_MODEL6] = CheckBox->Checked;
	else if(CheckBox->Name == "SupportRightCHBX")
		m_TLoadCellCalibration->CheckBoxArray[TYPE_TANK_SUPPORT2] = CheckBox->Checked;
	else if(CheckBox->Name == "WasteCHBX")
		m_TLoadCellCalibration->CheckBoxArray[TYPE_TANK_WASTE]= CheckBox->Checked;
	 if (CheckBox->Checked)
	 {
	   m_NoOfCheckedCheckboxes++;
	 }
	 else
	 {
	   m_NoOfCheckedCheckboxes--;
	 }

    GetParentWizard()->EnableDisableNext(m_NoOfCheckedCheckboxes > 0? true : false);
	GetActivePage()->NotifyEvent(0,0);
  }
}

 __fastcall TLoadCellCalibrationCheckBoxFrame::TLoadCellCalibrationCheckBoxFrame(TComponent* Owner)
        : TFrame(Owner)
{
  for(int i = 0;i<NUM_OF_CHECKBOX;i++)
     CheckBoxArray[i] = false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void TLoadCellCalibrationCheckBoxFrame::GetCheckedBoxes (bool OutputArray[])
{
  for(int i = 0;i<NUM_OF_CHECKBOX;i++)
     OutputArray[i] = CheckBoxArray[i];
}

//---------------------------------------------------------------------------
// Setting the checkboxes according to the input
void TLoadCellCalibrationCheckBoxFrame::SetCheckedBoxes (bool CheckedArray[])
{
   for(int i = 0;i<NUM_OF_CHECKBOX;i++)
	 CheckBoxArray[i] = CheckedArray[i];
}
//---------------------------------------------------------------------------

