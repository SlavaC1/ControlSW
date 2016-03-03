
#ifndef _UV_REPLACEMENT_WIZARD_H_
#define _UV_REPLACEMENT_WIZARD_H_

#include "Q2RTWizard.h"
#include "WizardPages.h"
#include "HeatersDefs.h"
#include "GlobalDefs.h"
#include "stdio.h"

// For the test part
#include <vcl.h>
#include <FileCtrl.hpp>

typedef enum {ReplacementAndAdjustmentMode,
              UVAdjustmentOnlyMode,
              IntensityEvaluationMode} TUVAdjustMode;

class CUVReplacementWizard : public CQ2RTAutoWizard {
private:
    ///// Variables...

    bool               m_IsLampReplaced[NUMBER_OF_UV_LAMPS];
    bool               m_CleanUpRequired;
    TUVAdjustMode      m_WizardMode;
    int                m_QualityModeIndex;
    int                m_OperationModeIndex;
    bool               m_RunUVCalibrationWizard;
    TTimer*            m_UVONCompletionPageTimer;

    ///// Functions...
    void ProcessResult(int DesiredPercentage, int ActualPercentage, int CurrPSLevel, QString &OutputRemark, bool &OutputResult, int Lamp);
    void FillInResults(QString* LampOutputRemarkArray,
                       bool*    LampOutputResultArray);

    bool UVLampIgnition();

    void EnableAllAxesAndHome();

    void CleanUp();

    // Start the wizard session event
    void StartEvent();
    // End the wizard session event
    void EndEvent();
    // Cancel the wizard session event
    void CancelEvent(CWizardPage *WizardPages);
    // Display Wizard Help event
    void HelpEvent(CWizardPage *WizardPage);

    void __fastcall UVONCompletionPageTimerEvent(TObject *Sender);
    void TurnUVLamps(bool Enable);
    void RestartUVONTimer();

    void ResetUVCalibrationPageLeave       (CWizardPage *WizardPage);
    void ChooseModePageLeave               (CWizardPage *WizardPage);

    void ChooseModePageEnter               (CWizardPage *WizardPage);
    void MoveBlockToReplacementPosPageEnter(CWizardPage *WizardPage);
    void ReplaceLampPageEnter              (CWizardPage *WizardPage);
    void AdjustUVLampsPageEnter            (CWizardPage *WizardPage);
    void StatusPageEnter                   (CWizardPage *WizardPage);
    void LampIgnition1PageEnter            (CWizardPage *WizardPage);
    void MoveBlockToSensorPos1PageEnter    (CWizardPage *WizardPage);

public:

    CUVReplacementWizard();
    ~CUVReplacementWizard();

    void PageLeave(CWizardPage *Page, TWizardPageLeaveReason LeaveReason);
    void PageEnter(CWizardPage *Page);
    bool RunUVCalibrationWizard(void);
};

#endif




