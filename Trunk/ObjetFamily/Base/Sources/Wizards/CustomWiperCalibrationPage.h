

#ifndef _CALIBRATE_XYT_POSITION_PAGE_H_
#define _CALIBRATE_XYT_POSITION_PAGE_H_

#include "WizardPages.h"
#include "GlobalDefs.h"

#define MCB_DELTA_PARAMETER_DELTA (CAppParams::Instance()->WC_TDelta)

const TWizardPageType wptCustomWiperCalibrationPage = wptCustom +25;

class CCustomWiperCalibrationPage : public CWizardPage
{
private:
public:

   CCustomWiperCalibrationPage(const QString& Title,int ImageID = -1,TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
     CWizardPage(Title,ImageID,PageAttributes)
   {}

   //void Reset(){}

   TWizardPageType GetPageType(void)
   {
   	  return wptCustomWiperCalibrationPage;
   }

};


#endif
