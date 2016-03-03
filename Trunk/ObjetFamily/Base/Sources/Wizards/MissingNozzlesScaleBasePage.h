#ifndef _MISSING_NOZZLES_SCALE_PAGES_H_
#define _MISSING_NOZZLES_SCALE_PAGES_H_

#include "WizardPages.h"
#include "GlobalDefs.h"

typedef void (*FuncPtr)( void* );


const TWizardPageType wptMissingNozzlesWizardPage = wptCustom +57;

class CMissingNozzlesScaleBasePage : public CWizardPage
{
private:
	std::vector<int> m_missingNozzles;

public:

   CMissingNozzlesScaleBasePage(const QString& Title,int ImageID = -1,TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
	 CWizardPage(Title,ImageID,PageAttributes | wpUserButton1Disabled |wpUserButton2Disabled)
   {}



   bool PreEnter(void)
   {
	  return CWizardPage::PreEnter();
   }

   void Reset()
   {

   }
	TWizardPageType GetPageType(void)
   {
	  return wptMissingNozzlesWizardPage;
   }
   void SetMissingNozzles(std::vector<int> missingNozzlesVector)
   {
	m_missingNozzles =  missingNozzlesVector;
   }
   void GetMissingNozzles(std::vector<int> &missingNozzlesVector)
   {
	 missingNozzlesVector = m_missingNozzles;
   }
};


#endif