
#ifndef _EIGHT_HEADS_CHECKBOXES_WIZARD_PAGE_H_
#define _EIGHT_HEADS_CHECKBOXES_WIZARD_PAGE_H_

#include "WizardPages.h"

const TWizardPageType wptHeadsCheckboxesWizardPage = wptCustom + 7;

class CHeadsCheckboxesWizardPage : public CWizardPage {
  public:
    // Constructor
    CHeadsCheckboxesWizardPage(const QString& Title,
                                    int ImageID = -1,
                                    TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
                                    CWizardPage(Title,ImageID,PageAttributes) { CheckedArray = NULL;}

    QString SubTitle;
    bool *CheckedArray;

    // Return the example page type
    TWizardPageType GetPageType(void) {
      return wptHeadsCheckboxesWizardPage;
    }
};


#endif
 