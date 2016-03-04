
#ifndef _QUICK_SHR_WIZARD_H_
#define _QUICK_SHR_WIZARD_H_

#include "Q2RTSHRWizardBase.h"    
#include "HeatersDefs.h"
#include "GlobalDefs.h"
#include <fstream>
#include <stdio.h>

class CQuickSHRWizard : public CQ2RTSHRWizardBase
{
private:

    int                 m_PrevHeadPotentiometerValues[TOTAL_NUMBER_OF_HEADS_HEATERS];
    float               m_EnteredWeightValues        [TOTAL_NUMBER_OF_HEADS_HEATERS];
    float               m_PrevHeadsVoltages          [TOTAL_NUMBER_OF_HEADS_HEATERS];
    float               m_TargetLayerThickness;

    bool                m_ResetValuesNeeded;
    int                 m_QualityModeIndex;
    int                 m_OperationModeIndex;
    bool                m_CleanUpRequired;
	int                 m_TotalNumberOfSlices;

    void StartEvent();
    void EndEvent();
    void CancelEvent      (CWizardPage *WizardPage);
    void HelpEvent        (CWizardPage *WizardPage);
    void UserButton2Event (CWizardPage *WizardPage);

    void PageLeave(CWizardPage *Page, TWizardPageLeaveReason LeaveReason);
    void PageEnter(CWizardPage *Page);

    bool DoWeightTest(CWizardPage* Page);


    void CleanUp();
    void SaveWizardSpecificDataToTemporaryFile();
    void ResumeWizardSpecificData();
    void SetResumingPage();

    QString GetResumeWarningString();
    QString GetResumeWarningString(int PageNumber);
    bool    CheckRequiredConditions(void);

    void CloseDoor2PageEnter             (CWizardPage* WizardPage);
    bool CalibratingForPageEnter         (CWizardPage* WizardPage);
    bool WeightTestPageEnter             (CWizardPage* WizardPage);
    void EnterWeightsPageEnter           (CWizardPage* WizardPage);
    void AdjustPotentiometerPageEnter    (CWizardPage* WizardPage);
    void EnterWeightsPageLeave           (CWizardPage* WizardPage);

    void WriteToHistoryFile(QString Str);

protected:
    virtual bool SetHeadsTemperature();

public:
    CQuickSHRWizard();

    int GetContinuePageNumber();
    
};


#endif


