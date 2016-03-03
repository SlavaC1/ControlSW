
#ifndef _VACUUM_CALIBRATION_WIZARD_PAGE_H_
#define _VACUUM_CALIBRATION_WIZARD_PAGE_H_

#include "WizardPages.h"

const TWizardPageType wptVacuumStatusWizardPage = wptCustom + 10;

class CVacuumCalibrationWizardPage : public CWizardPage {
  public:
    // Constructor
    CVacuumCalibrationWizardPage(const QString& Title,
                                 int ImageID = -1,
                                 TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
                                 CWizardPage(Title,ImageID,PageAttributes) { CurrentVacuumValue = 0;
								 AverageVacuumValue = 0 ;}

    int CurrentVacuumValue;
    int AverageVacuumValue;

    // Return the example page type
    TWizardPageType GetPageType(void) {
      return wptVacuumStatusWizardPage;
    }
};


#endif
