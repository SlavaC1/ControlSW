
#ifndef _UV_LAMPS_CALIBRATION_PAGE_H_
#define _UV_LAMPS_CALIBRATION_PAGE_H_

#include "WizardPages.h"

const TWizardPageType wptUVLampsCalibrationWizardPage = wptCustom + 10;

class CUVLampsCalibrationWizardPage : public CWizardPage {
  public:
    // Constructor
    CUVLampsCalibrationWizardPage(const QString& Title,
                             int ImageID = -1,
                             TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
                             CWizardPage(Title,ImageID,PageAttributes) {}

//    QString SubTitle;
//    QString HQ_RightUVStatusLabel;
//    QString HS_RightUVStatusLabel;
//    QString HQ_LeftUVStatusLabel;
//    QString HS_LeftUVStatusLabel;
//    bool HQ_RightLampStatus;
//    bool HS_RightLampStatus;
//    bool HQ_LeftLampStatus;
//    bool HS_LeftLampStatus;
	int LampStabProgress;
	int MeasureUVProgress;
	QString CurrentLampString;
	QString CurrentModeString;
	bool DuringLampStab;
	bool DuringMeasureUV;
	QString LampStabTime;
	QString MeasureUVCycles;
	int OverallProgress;


    // Return the example page type
    TWizardPageType GetPageType(void) {
      return wptUVLampsCalibrationWizardPage;
    }
};


#endif



