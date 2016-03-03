
#ifndef _UV_CALIBRATION_WIZARD_H_
#define _UV_CALIBRATION_WIZARD_H_

#include "Q2RTWizard.h"
#include "GlobalDefs.h"
#include "WizardPages.h"
#include "HeatersDefs.h"
#include "stdio.h"

// For the test part
#include <vcl.h>
#include <FileCtrl.hpp>

typedef enum {PowerTestMode,
              CalibrationMode} TUVWizardMode;

class CUVCalibrationWizard : public CQ2RTAutoWizard
{
private:
    //Variables...

    int                 m_QualityModeIndex;
    int                 m_OperationModeIndex;
    int                 m_UVLampIndex;
    TUVWizardMode       m_WizardMode;
    QString             m_WizardModeStr;
    
    bool    m_CleanUpRequired;
    QString m_UVStatusLabel        [NUMBER_OF_UV_LAMPS][NUMBER_OF_QUALITY_MODES][NUM_OF_OPERATION_MODES];
    bool    m_LampTotalStatus      [NUMBER_OF_UV_LAMPS][NUMBER_OF_QUALITY_MODES][NUM_OF_OPERATION_MODES];
    int     m_ValuePercentage      [NUMBER_OF_UV_LAMPS];
    float   m_LastValue            [NUMBER_OF_UV_LAMPS];
    USHORT  m_PSValue;

    bool    m_LampStatus          [NUMBER_OF_UV_LAMPS];
    int     m_AdditionalStatus    [NUMBER_OF_UV_LAMPS];

    int     m_CurPSIndex;
    int     m_DelayTime;

    ///// Functions...
    void CleanUp();
    bool XLoop(CProgressWizardPage* WizardPage, int X1, int X2, int Cycles);
    bool GetNextPSValue(bool NeedMoreCapacity, int PercentDiff, int LampID);
    void InitIndices();
    void CommentLampStatusToUVHistoryFile(int LampID);
    void CommentLampStatusToStatusPage   (int LampID);
    
    // Start the wizard session event
    void StartEvent();
    // End the wizard session event
    void EndEvent();
    // Cancel the wizard session event
    void CancelEvent(CWizardPage *WizardPages);
    // Display Wizard Help event
    void HelpEvent(CWizardPage *WizardPage);

    bool PreparingPageEnter             (CWizardPage* WizardPage);
    bool LampsOnPageEnter               (CWizardPage* WizardPage);
    bool ResetSensorPageEnter           (CWizardPage* WizardPage);
    bool LampScanPageEnter              (CWizardPage* WizardPage);
    bool InsertLampValuePageEnter       (CWizardPage* WizardPage);
    bool DisplayLampCapacityPageEnter   (CWizardPage* WizardPage);
    bool StatusPageEnter                (CWizardPage* WizardPage);
    bool HomeAxisPageEnter              (CWizardPage* WizardPage);


public:

    CUVCalibrationWizard();

    void PageLeave        (CWizardPage *Page, TWizardPageLeaveReason LeaveReason);
    void PageEnter        (CWizardPage *Page);
};

#endif




