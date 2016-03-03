
#ifndef _STABLE_VALUE_WIZARD_PAGE_H_
#define _STABLE_VALUE_WIZARD_PAGE_H_

#include "WizardPages.h"

const TWizardPageType wptStableValueWizardPage = wptCustom + 11;

class CStableValueWizardPage : public CWizardPage {
  public:
    // Constructor
    CStableValueWizardPage(const QString& Title,
                           int ImageID = -1,
                           TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
                           CWizardPage(Title,ImageID,PageAttributes) { CurrValue = 0;}

    QString GroupBoxCaption;
    QString UnitsCaption;
    QString CheckBoxText;

    int CurrValue;

    // Return the example page type
    TWizardPageType GetPageType(void) {
      return wptStableValueWizardPage;
    }
};


#endif
