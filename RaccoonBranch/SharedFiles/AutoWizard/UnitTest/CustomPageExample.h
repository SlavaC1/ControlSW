// Example for a wizard custom page

#ifndef _CUSTOM_PAGE_EXAMPLE_
#define _CUSTOM_PAGE_EXAMPLE_

#include "WizardPages.h"

const TWizardPageType wptCustomPageExample = wptCustom;

class CCustomPageExample : public CWizardPage {
  public:
    // Constructor
    CCustomPageExample(const QString& Title,int ImageID = -1,
                       TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
      CWizardPage(Title,ImageID,PageAttributes) {}

    QString Str1,Str2;

    // Return the example page type
    TWizardPageType GetPageType(void) {
      return wptCustomPageExample;
    }
};

#endif
