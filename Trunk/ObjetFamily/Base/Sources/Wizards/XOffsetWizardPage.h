
#ifndef _X_OFFSET_WIZARD_PAGE_H_
#define _X_OFFSET_WIZARD_PAGE_H_

#include "WizardPages.h"

const TWizardPageType wptXOffsetWizardPage = wptCustom + 5;

class CXOffsetWizardPage : public CWizardPage {
  public:
    // Constructor
    CXOffsetWizardPage(const QString& Title,
                       int ImageID = -1,
                       TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
                       CWizardPage(Title,ImageID,PageAttributes) {}

    int ChosenPrintLine;

    // Return the example page type
    TWizardPageType GetPageType(void) {
      return wptXOffsetWizardPage;
    }
};


#endif
