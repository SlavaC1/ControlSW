
#ifndef _VACUUM_CALIBRATION_WIZARD_H_
#define _VACUUM_CALIBRATION_WIZARD_H_

#include "Q2RTWizard.h"
#include "WizardPages.h"
#include "HeatersDefs.h"
#include "stdio.h"

// For the test part
#include <fstream>
#include <vcl.h>
#include <FileCtrl.hpp>

class CVacuumCalibrationWizard : public CQ2RTAutoWizard
{
  private:
    // Variables.....
    float m_CurrVacuumValue;

    int m_VacuumLineGain;
    float m_VacuumLineOffset;

    bool m_NextWasPressed;
    bool m_IsRestartRequired;

    QString m_WizardResults;

    // Functions.....
    void CleanUp();
    void UpdateParams();
    bool PurgeCycle(int NumberOfPurgeCycles);

    // Start the wizard session event
    void StartEvent();
    // End the wizard session event
    void EndEvent();
    // Cancel the wizard session event
    void CancelEvent(CWizardPage *WizardPages);
    // Display Wizard Help event
    void HelpEvent(CWizardPage *WizardPage);

  public:
    CVacuumCalibrationWizard();

    bool IsRestartRequired();

    void PageLeave(CWizardPage *Page, TWizardPageLeaveReason LeaveReason);
    void PageEnter(CWizardPage *Page);
    void PageEventsHandler(CWizardPage *WizardPage,int Param1,int Param2);

};


#endif

