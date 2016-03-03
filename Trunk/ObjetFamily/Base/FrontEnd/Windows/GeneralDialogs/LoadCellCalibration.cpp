//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "FEResources.h"
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
		m_TLoadCellCalibration->CheckBoxArray[CheckBox->Tag] = CheckBox->Checked;

		if (CheckBox->Checked)
			m_NoOfCheckedCheckboxes++;
		else
			m_NoOfCheckedCheckboxes--;

		GetParentWizard()->EnableDisableNext(m_NoOfCheckedCheckboxes > 0 ? true : false);
		GetActivePage()->NotifyEvent(0, 0);
	}
}

__fastcall TLoadCellCalibrationCheckBoxFrame::TLoadCellCalibrationCheckBoxFrame(TComponent* Owner)
	: TFrame(Owner)
{
	#if defined(OBJET_1000)
	for(int i = 0; i < TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE; i++)
	{
		CheckBoxArray[i] = false;

	   m_containersChecks[i]                = new TCheckBox(this);
	   m_containersChecks[i]->Parent        = GroupBox1;
	   m_containersChecks[i]->Tag           = i;
	   m_containersChecks[i]->TabOrder      = i;
	   m_containersChecks[i]->Width         = 50;
	   m_containersChecks[i]->Left          = INT_FROM_RESOURCE_ARRAY(i,IDN_LOAD_CELL_CALIBRATION_CHBX_LEFT);
	   m_containersChecks[i]->Top           = INT_FROM_RESOURCE_ARRAY(i,IDN_LOAD_CELL_CALIBRATION_CHBX_TOP);
	   m_containersChecks[i]->Checked       = false;
	   m_containersChecks[i]->Caption       = TankToStr((TTankIndex)i).c_str();
	}
	pgc1->Pages[1]->TabVisible = false; /*no need to show second tab*/
	#else
		for(int i = 0; i < TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE; i++)
	{
	  m_containersChecks[i]                = new TCheckBox(this);
	   m_containersChecks[i]->Parent        = GroupBox1;
	   m_containersChecks[i]->Tag           = i;
	   m_containersChecks[i]->TabOrder      = i;
	   m_containersChecks[i]->Width         = 10;
	   m_containersChecks[i]->Left          = INT_FROM_RESOURCE_ARRAY(i,IDN_LOAD_CELL_CALIBRATION_CHBX_LEFT);
	   m_containersChecks[i]->Top           = INT_FROM_RESOURCE_ARRAY(i,IDN_LOAD_CELL_CALIBRATION_CHBX_TOP);
	   m_containersChecks[i]->Checked       = false;
	   m_containersChecks[i]->Caption       = TankToStr((TTankIndex)i).c_str();
	}

	m_containersChecks[TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE-2]->Width         = 50;
	m_containersChecks[TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE-2]->Caption       = TankToStr((TTankIndex)(TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE-2)).c_str();
	
	#endif
	if (CAppParams::Instance()->NumOfMaterialCabinets == 0)
	{
		Image1->Visible = false;
	}

	if (CAppParams::Instance()->DualWasteEnabled)
	{
		m_numOfActualContainers = TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE;
	}
	else
	{
    	m_containersChecks[TYPE_TANK_WASTE_RIGHT]->Visible = false;
        m_numOfActualContainers = TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE-1;
    }


}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void TLoadCellCalibrationCheckBoxFrame::GetCheckedBoxes (bool OutputArray[])
{
	for(int i = 0; i < m_numOfActualContainers; i++)
		OutputArray[i] = CheckBoxArray[i];
}

//---------------------------------------------------------------------------
// Setting the checkboxes according to the input
void TLoadCellCalibrationCheckBoxFrame::SetCheckedBoxes (bool CheckedArray[])
{
	for(int i = 0; i < m_numOfActualContainers; i++)
	{
		//m_containersChecks[i]->Enabled = CheckedArray[i];
		CheckBoxArray[i] = CheckedArray[i];
    }
}
//---------------------------------------------------------------------------

