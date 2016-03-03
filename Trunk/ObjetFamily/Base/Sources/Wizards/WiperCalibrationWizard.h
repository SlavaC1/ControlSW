
#ifndef _WIPER_CALIBRATION_WIZARD_H_
#define _WIPER_CALIBRATION_WIZARD_H_

#include "Q2RTWizard.h"

class CWiperCalibrationWizard : public CQ2RTAutoWizard
{

private:
    float PrevMotorsPurgeXStartPosition,PrevMotorPurgeYActPosition;
    int PrevMotorWipeTActPositionSteps;
    int FirstMotorWipeTActDelta,PrevMotorWipeTActDelta,FirstCalculatedMaxTPositionStep;
    int PrevMaxTPositionStep;
    long m_PreviousXSpeed,m_PreviousYSpeed;
    bool m_IsModelOnTheTray;
    

    void StartEvent();
    void EndEvent();
    void CancelEvent(CWizardPage *WizardPages);
    void HelpEvent(CWizardPage *WizardPage);
    void CleanUp();

    bool PositionAxisPageEnter(CWizardPage *WizardPage);
    bool CloseDoorPageEnter(CWizardPage *WizardPage);
    void HomeAxisPageEnter(CWizardPage *WizardPage);
    void CalibrateTMaxPositionPageEnter(CWizardPage *WizardPage);
    void PurgeWipePageEnter(CWizardPage *WizardPage, TMachineState checkState, TMachineState doneState, TNotifyEvent FuncPtr);
    bool ResultCheckPositionPageEnter(CWizardPage *WizardPage);

    void IsResultGoodPageLeave(CWizardPage *WizardPage);
    void CalibratePositionPageLeave(CWizardPage *WizardPage);

    int MCB55DiagRead();
    void MCB55DiagWrite(int Value);

    void __fastcall DoPurgeSequence(TObject *Sender=NULL);
    void __fastcall DoWipe(TObject *Sender=NULL);

public:
    CWiperCalibrationWizard();

    void PageLeave(CWizardPage *Page, TWizardPageLeaveReason LeaveReason);
    void PageEnter(CWizardPage *Page);



private:

};


#endif

 