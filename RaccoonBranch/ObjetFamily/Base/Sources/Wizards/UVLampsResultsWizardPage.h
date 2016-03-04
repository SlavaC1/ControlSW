
#ifndef _UV_LAMPS_RESULTS_PAGE_H_
#define _UV_LAMPS_RESULTS_PAGE_H_

#include "WizardPages.h"
#include "GlobalDefs.h"

const TWizardPageType wptUVLampsResultsWizardPage = wptCustom + 11;

class CUVLampsResultsWizardPage : public CWizardPage {
protected:
    QString SubTitle;
  public:
    // Constructor
    CUVLampsResultsWizardPage(const QString& Title,
                             int ImageID = -1,
                             TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
                             CWizardPage(Title,ImageID,PageAttributes) {}

    QString UVResultsLabel [NUMBER_OF_UV_LAMPS][NUMBER_OF_QUALITY_MODES][NUM_OF_OPERATION_MODES];
    bool    LampResults    [NUMBER_OF_UV_LAMPS][NUMBER_OF_QUALITY_MODES][NUM_OF_OPERATION_MODES];

    // Return the example page type
    TWizardPageType GetPageType(void)
    {
      return wptUVLampsResultsWizardPage;
    }
    
};


#endif



