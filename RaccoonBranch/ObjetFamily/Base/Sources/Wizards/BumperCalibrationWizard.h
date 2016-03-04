
#ifndef _BUMPER_CALIBRATION_WIZARD_H_
#define _BUMPER_CALIBRATION_WIZARD_H_

#include "Q2RTWizard.h"
#include "WizardPages.h"

class CBumperCalibrationWizard : public CQ2RTAutoWizard
{
private:
    int                m_SensitivityThreshold;
    float              m_RequiredTrayOffset;
    float              m_CurrTrayOffset;
    int                m_BumperSensitivity;
    bool               m_PrevBumperStopAfterImpact;
    bool               m_PrevBumperBypassParamValue;
    int                m_MaterialPermissiveCount;

    // Start the wizard session event
    void StartEvent();
    void CleanUp();
    // End the wizard session event
    void EndEvent();
    // Cancel the wizard session event
    void CancelEvent(CWizardPage *WizardPages);

    void PageLeave(CWizardPage *Page, TWizardPageLeaveReason LeaveReason);
    void PageEnter(CWizardPage *Page);
    void HelpEvent(CWizardPage *WizardPage);

    int FindThresholdValue(int LowValue, int HighValue);
    int FindSensitivityValue(int LowValue, int HighValue);

    void ThresholdCalibrationPageEnter(CWizardPage *Page);
    bool PrintCubePageEnter(CWizardPage *Page);
    bool CalibrateSensitivityPageEnter(CWizardPage *Page);

public:
    CBumperCalibrationWizard();

};


#endif



