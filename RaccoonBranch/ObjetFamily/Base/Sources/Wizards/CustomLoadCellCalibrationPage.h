

#ifndef _CUSTOM_LOAD_CELL_PAGE_H_
#define _CUSTOM_LOAD_CELL_PAGE_H_

#include "WizardPages.h"
#include "GlobalDefs.h"
#define NUM_OF_CHECKBOX  9
#define MCB_DELTA_PARAMETER_DELTA (CAppParams::Instance()->WC_TDelta)

const TWizardPageType wptCustomLoadCellCalibrationPage = wptCustom +55;

class CCustomLoadCellCalibrationPage : public CWizardPage
{
private:
public:
   bool m_CheckedArray[NUM_OF_CHECKBOX];
   bool m_ActiveCheckBoxes[NUM_OF_CHECKBOX] ;
   String m_SubTitle ;
   CCustomLoadCellCalibrationPage(const QString& Title,int ImageID = -1,TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
     CWizardPage(Title,ImageID,PageAttributes)
   {
       ResetCheckBoxView(true);
   }

   //void Reset(){}

   TWizardPageType GetPageType(void)
   {
   	  return wptCustomLoadCellCalibrationPage;
   }
   void SetCartridgesArray(bool CheckedArray[])
   {
      for(int i = 0;i<NUM_OF_CHECKBOX;i++)
		 m_CheckedArray[i] = CheckedArray[i];
   }
   void GetCartridgesArray (bool OutputArray[])
	{
		for(int i = 0;i<NUM_OF_CHECKBOX;i++)
		  OutputArray[i] = m_CheckedArray[i];
	}
	void GetActiveCheckBoxes (bool OutputArray[])
    {
		for(int i = 0;i<NUM_OF_CHECKBOX;i++)
		  OutputArray[i] = m_ActiveCheckBoxes[i];
	}
	void ResetCheckBoxView(bool active)
	{
		for(int i = 0;i<NUM_OF_CHECKBOX;i++)
			m_ActiveCheckBoxes[i] = active;
		m_SubTitle = "";
	}
	String GetSubTitle()
	{
        return m_SubTitle;
    }
};


#endif