
#ifndef _UV_LAMPS_STATUS_PAGE_H_
#define _UV_LAMPS_STATUS_PAGE_H_

#include "WizardPages.h"
#include "GlobalDefs.h"

const TWizardPageType wptUVLampsStatusWizardPage = wptCustom + 9;

class CUVLampsStatusWizardPage : public CWizardPage {
protected:
    QString SubTitle;
  public:
    // Constructor
    CUVLampsStatusWizardPage(const QString& Title,
                             int ImageID = -1,
                             TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
                             CWizardPage(Title,ImageID,PageAttributes) {}

    QString UVStatusLabel [NUMBER_OF_UV_LAMPS][NUMBER_OF_QUALITY_MODES][NUM_OF_OPERATION_MODES];
    bool    LampStatus    [NUMBER_OF_UV_LAMPS][NUMBER_OF_QUALITY_MODES][NUM_OF_OPERATION_MODES];

    // Return the example page type
    TWizardPageType GetPageType(void)
    {
      return wptUVLampsStatusWizardPage;
    }
    
};


#endif



