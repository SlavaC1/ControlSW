#ifndef _LOAD_CELL_WELCOME_WIZARD_PAGE_
#define _LOAD_CELL_WELCOME_WIZARD_PAGE_

#include "WizardPages.h"

const TWizardPageType wptLoadCellWellcomePage = wptCustom + 56;

class CLoadCellWellcomeWizardPage : public CWizardPage {
  public:
    // Constructor
    CLoadCellWellcomeWizardPage(const QString& Title,int ImageID = -1,
                       TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
      CWizardPage(Title,ImageID,PageAttributes)
    {
	  m_imageIndex = 0;
	}

    // Return the example page type
    TWizardPageType GetPageType(void)
    {
      return wptLoadCellWellcomePage;
    }


	int m_imageIndex;
};

#endif

