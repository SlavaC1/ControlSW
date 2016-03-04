
#ifndef _SINGLE_POINT_WEIGHT_CALIBRATION_WIZARD_H_
#define _SINGLE_POINT_WEIGHT_CALIBRATION_WIZARD_H_

#include "Q2RTWizard.h"
#include "WizardPages.h"
#include "GlobalDefs.h"

class CSinglePointWeightCalibrationWizard : public CQ2RTAutoWizard {
  private:

    bool m_SensorsToCalibrate[TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE];
    bool m_TwoTanksPerMaterial;

    int m_CartridgeIndex;
	bool m_needToResetData;
    // Start the wizard session event
    void StartEvent();

    void PageLeave(CWizardPage *Page, TWizardPageLeaveReason LeaveReason);

    void PageEnter(CWizardPage *Page);

    void HelpEvent(CWizardPage *WizardPage);

	void FindNextToCalibrate(void);
	void SetSensorCalibrateArrayAccordingToEnum(bool OutputArray[]);

  public:
    CSinglePointWeightCalibrationWizard();

};


#endif



