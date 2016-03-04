
#ifndef _X_OFFSET_WIZARD_H_
#define _X_OFFSET_WIZARD_H_

#include "Q2RTWizard.h"
#include "WizardPages.h"
#include "HeatersDefs.h"
#include "GlobalDefs.h"

class CXOffsetWizard : public CQ2RTAutoWizard
{
private:
    QString            m_WizardSummary;

    // Array of 'Line Selection'
    int  m_BestPrint[TOTAL_NUMBER_OF_HEADS_HEATERS];
    // Heads which need to be calibrated
    int  m_CurrentHead;

    void EndEvent();// End the wizard session event
    void CancelEvent(CWizardPage *WizardPages);// Cancel the wizard session event
    void HelpEvent(CWizardPage *WizardPage);
    void UpdateParameters();// Update the parameters according to the process

    bool PrintTransparencyPageEnter(CWizardPage *WizardPage);

    int GetNextHead();
    int GetPreviousHead();

public:
    CXOffsetWizard();
    void PageLeave(CWizardPage *Page, TWizardPageLeaveReason LeaveReason);
    void PageEnter(CWizardPage *Page);
    int  GetContinuePageNumber();

private:
};


#endif

 