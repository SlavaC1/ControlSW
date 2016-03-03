#ifndef MaterialReplacementDonePageH
#define MaterialReplacementDonePageH

#include "WizardPages.h"
#include "GlobalDefs.h"

const TWizardPageType wptMaterialReplacementDonePage = wptCustom + 30;

class CMaterialReplacementDonePage : public CWizardPage
{
private:
public:

	QString m_Title;

   CMaterialReplacementDonePage(const QString& Title,int ImageID = -1,TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
	 CWizardPage("",ImageID,PageAttributes)
   {
   		m_Title = Title;
   }

   TWizardPageType GetPageType(void)
   {
	  return wptMaterialReplacementDonePage;
   }

};


#endif