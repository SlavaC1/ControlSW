
#ifndef _PRINTING_POINTS_POSITIONS_WIZARD_H_
#define _PRINTING_POINTS_POSITIONS_WIZARD_H_

#include "Q2RTWizard.h"
#include "WizardPages.h"
#include "HeatersDefs.h"

#define PRINTING_POSITION_HELP_FILE_NAME LOAD_STRING(IDS_PP_WIZARD_HELP_FILE)

class CPrintingPositionWizard : public CQ2RTAutoWizard
{
private:
    TMachineState      m_PrevMachineState;
    bool               m_RepeatTest;
    float              m_PrevTestPatternXPos;
    float              m_PrevTestPatternYPos;

private:

    void StartEvent(); // Start the wizard session event
    void EndEvent(); // End the wizard session event
    void CancelEvent(CWizardPage *WizardPages); // Cancel the wizard session event
    void HelpEvent(CWizardPage *WizardPage);
    void CleanUp();
    bool DoPatternTest();

    void PrintPatternTestPageEnter(CWizardPage *WizardPage);
    void IsResultGoodPageLeave(CWizardPage *WizardPage);
    void PositionCorrectionPageLeave(CWizardPage *WizardPage);

public:
    CPrintingPositionWizard();
    void PageLeave(CWizardPage *Page, TWizardPageLeaveReason LeaveReason);
    void PageEnter(CWizardPage *Page);
    int  GetContinuePageNumber();

};

#endif

