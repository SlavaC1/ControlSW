#ifndef _SHR_WELCOME_WIZRAD_PAGE_
#define _SHR_WELCOME_WIZRAD_PAGE_

#include "WizardPages.h"

const TWizardPageType wptAgreementPage = wptCustom + 1;

class CAgreementWizardPage : public CWizardPage {
    bool m_Agreement;
  public:
    // Constructor
    CAgreementWizardPage(const QString& Title,int ImageID = -1,
                       TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
      CWizardPage(Title,ImageID,PageAttributes)
    {
      Label1  = "BY CLICKING ON THE 'I AGREE' BUTTON, YOU ARE ACCEPTING THE FOLLOWING CONDITION \
                 FOR THE SINGLE HEAD REPLACEMENT ('SHR').  IF YOU DO NOT AGREE TO THE FOLLOWING CONDITION, \
                 CLICK THE 'I DO NOT AGREE' OR THE 'CANCEL' BUTTON AND CONTACT CUSTOMER SUPPORT.";
      Label2  = "IMPORTANT: PLEASE READ CAREFULLY";
      Label5  = "By checking the 'I agree', you indicate your acceptance of the conditions and compliance \
                 with the guidelines and instructions, specified in the User Guide."; 
      m_Agreement = true;
    }

    // Return the example page type
    TWizardPageType GetPageType(void)
    {
      return wptAgreementPage;
    }

    void SetAgreement(bool Agreement)
    {
      m_Agreement = Agreement;
    }
    
    bool GetAgreement(void)
    {
      return m_Agreement; 
    }
    
    QString Label1,Label2,Label3,Label4,Label5;
};

#endif

 