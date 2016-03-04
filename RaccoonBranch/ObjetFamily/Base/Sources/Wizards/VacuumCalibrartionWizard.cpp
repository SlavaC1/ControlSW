/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Vacuum Calibration.                                      *
 * Module Description: Vacuum Calibration wizard.                   *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Oved Ben Zeev                                            *
 * Start date: 3/3/2004                                             *
 * Last upate: 3/3/2004                                             *
 ********************************************************************/

#include "VacuumCalibrartionWizard.h"
#include "QTimer.h"
#include "AppParams.h"
#include "MotorDefs.h"
#include "HeatersDefs.h"
#include "QErrors.h"
#include "QMonitor.h"
#include "QThreadUtils.h"
#include "AppLogFile.h"
#include "VacuumCalibrationWizardPage.h"
#include "BackEndInterface.h"
#include "Q2RTApplication.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "WizardImages.h"
const char VACUUM_HELP_FILE_NAME[] = "\\Help\\Vacuum Calibration Wizard\\Vacuum_Calibration_Wizard.chm";

#ifdef EDEN_250
  const int VACUUM_MEASURED_VALUE    = 7;  // cmH2O
#else
  const int VACUUM_MEASURED_VALUE    = 6;  // cmH2O
#endif

const int VACUUM_LIMITS_THRESHOLD          = 400;
const int VACUUM_CRITICAL_LIMITS_THRESHOLD = 600;

const int A2D_FILE_MIN_VALUE = 0;
const int A2D_FILE_MAX_VALUE = 11;

const int RESTART_APPLICATION_NOW = 0;

const int NUMBER_OF_PURGES = 2;

const int MIN_SENSOR_REASONABLE_VALUE = 50;
const int MAX_SENSOR_REASONABLE_VALUE = 4000;

char* CHECK_PURGE_PRESSURE_AIR_VALVE_OFF = "Click the 'Air Valve' button below to open and close the air valve.\nAdjust the pressure to 4.0 +/- 0.5 psi.\nFor more informaion, click 'Help'.\n\n AIR VALVE IS OFF.";
char* CHECK_PURGE_PRESSURE_AIR_VALVE_ON = "Click the 'Air Valve' button below to open and close the air valve.\nAdjust the pressure to 4.0 +/- 0.5 psi.\nFor more informaion, click 'Help'.\n\n AIR VALVE IS ON.";

void ExternalAppExecute(const char *FileName,const char *CmdLineParam);

enum {vcWelcomePage,        //   1
      vcIsTrayInsertedPage, //   2
      vcCheckTrayClean,     //   3
      vcCloseDoor,          //   4
      vcHomingAxis,
	  vcGotoPurgePosition,  //   5
      vcConnectMeasDevice,  //   6
      vcCalibrateVacuum,    //   7
      vcDismantleDevice,    //   8
      vcIsTrayInsertedPage2,//   9
	  vcCloseDoor1,         //   10
	  vcPurge,              //   11
      vcUpdateParameters,   //   12
	  vcWizardResults,      //   13
	  vcConnectPurgeDevice, //   14
	  vcCheckPurgePressure, //   15
	  vcDisconnectPurgeDevice, //   16
	  vcCloseDoor2,         //   17
	  vcCompletionPage,     //   19
	  vcLiquidsShortagePage,//   20
	  vcDummyPage,
	  vcSensorProblemPage,
      vcPagesCount
      };


CVacuumCalibrationWizard::CVacuumCalibrationWizard(void) : CQ2RTAutoWizard(IDS_VC_WIZARD,true,IN_PROCESS_IMAGE_ID,false,false,"Air Valve","")
{
  TPagePointers pagesVector(vcPagesCount, 0);
  m_IsRestartRequired = false;
  {
	CMessageWizardPage *pPage = new CMessageWizardPage("Vacuum Calibration Wizard",FIRST_PAGE_IMAGE_ID,wpPreviousDisabled | wpHelpNotVisible);
	pPage->SubTitle = "This wizard guides you through the vacuum and purge panel calibration procedure.\
 Prepare the following:\n\
- vacuum gauge (JIG-00065) filled with water to 6 cm\n\
- air pressure gauge (JIG-04044\n\
- vacuum tubing, at least 50 cm\n\
- digital voltmeter with insulated test clips\n\
- 2-mm and 7-mm screwdriver\n\
- needle-nose pliers\n\
Verify that there are no air bubbles in the gauge, \
and that the water level on the left and right sides is the same.";
	pagesVector[vcWelcomePage] = pPage;
  }
  {
	CInsertTrayPage *pPage = new CInsertTrayPage(this, PREPARATIONS_IMAGE_ID, wpPreviousDisabled | wpHelpNotVisible);
  	pagesVector[vcIsTrayInsertedPage] = pPage;
  }
  {
    CVerifyCleanTrayPage *pPage = new CVerifyCleanTrayPage(this,PREPARATIONS_IMAGE_ID, wpPreviousDisabled | wpHelpNotVisible);
  	pagesVector[vcCheckTrayClean] = pPage;
  }
  {
	CCloseDoorPage *pPage = new CCloseDoorPage(this,CAUTION_IMAGE_ID, wpPreviousDisabled | wpHelpNotVisible);
	pagesVector[vcCloseDoor] = pPage;
  }
  {
	CHomingAxisPage *pPage = new CHomingAxisPage(this,"",wpNextDisabled| wpPreviousDisabled | wpHelpNotVisible | wpHelpNotVisible );
	pagesVector[vcHomingAxis] = pPage;
  }
  {
	CSuspensionPointsStatusPage *pPage = new CSuspensionPointsStatusPage("",-1,wpPreviousDisabled | wpHelpNotVisible);
    pPage->PointsStatusMessage = LOAD_STRING(IDS_PREPARING);
  	pagesVector[vcGotoPurgePosition] = pPage;
  }
  {
	CCheckBoxWizardPage *pPage = new CCheckBoxWizardPage("Check Printer",PREPARATIONS_IMAGE_ID,wpPreviousDisabled);
	pPage->SubTitle = "Confirm before continuing:";
	pPage->Strings.Add("Vacuum measurement device is connected.");
  	pagesVector[vcConnectMeasDevice] = pPage;
  }
  {
	CCheckBoxWizardPage *pPage = new CCheckBoxWizardPage("Check Printer",PREPARATIONS_IMAGE_ID,wpPreviousDisabled | wpNextWhenSelected);
    pPage->SubTitle = "Confirm before continuing:";
	pPage->Strings.Add(QFormatStr("Vacuum level is set to %d cm/H2O.", VACUUM_MEASURED_VALUE));
	pPage->Strings.Add("Vacuum pump frequency is set to 100Hz +/- 5Hz.");
  	pagesVector[vcCalibrateVacuum] = pPage;
  }
  {
	CCheckBoxWizardPage *pPage = new CCheckBoxWizardPage("Check Printer",PREPARATIONS_IMAGE_ID,wpPreviousDisabled);
	pPage->SubTitle = "Confirm before continuing:";
	pPage->Strings.Add("The vacuum measurement device is removed.");
	pagesVector[vcDismantleDevice] = pPage;
  }
  {
	CInsertTrayPage *pPage = new CInsertTrayPage(this, PREPARATIONS_IMAGE_ID, wpPreviousDisabled | wpNextWhenSelected | wpHelpNotVisible);
  	pagesVector[vcIsTrayInsertedPage2] = pPage;
  }
  {
    CCloseDoorPage *pPage = new CCloseDoorPage(this,CAUTION_IMAGE_ID, wpPreviousDisabled | wpHelpNotVisible);
	pagesVector[vcCloseDoor1] = pPage;
  }
  {
	CElapsingTimeWizardPage *pPage = new CElapsingTimeWizardPage("Purging Print Heads",-1,wpPreviousDisabled | wpHelpNotVisible | wpNextDisabled);
  	pagesVector[vcPurge] = pPage;
  }
  {
	CVacuumCalibrationWizardPage *pPage = new CVacuumCalibrationWizardPage("Indicate when Values are Stable",PREPARATIONS_IMAGE_ID, wpPreviousDisabled | wpNextDisabled);
  	pagesVector[vcUpdateParameters] = pPage;
  }
  {
	CMessageWizardPage *pPage = new CMessageWizardPage("Note",SHD_INFORMATION_PICTURE_ID,wpCancelDisabled | wpHelpNotVisible);
	pagesVector[vcWizardResults] = pPage;
  }
  {
	CCheckBoxWizardPage *pPage = new CCheckBoxWizardPage("Check Printer",PREPARATIONS_IMAGE_ID,wpPreviousDisabled);
	pPage->SubTitle = "Confirm before continuing:";
	pPage->Strings.Add("Air pressure gauge is connected.");
	pagesVector[vcConnectPurgeDevice] = pPage;
  }
  {
	CMessageWizardPage *pPage = new CMessageWizardPage("Check Purge Pressure",PREPARATIONS_IMAGE_ID,wpFlipButton1Visible | wpCancelDisabled);
	pPage->SubTitle = CHECK_PURGE_PRESSURE_AIR_VALVE_OFF;
	pagesVector[vcCheckPurgePressure] = pPage;
  }
  {
	CCheckBoxWizardPage *pPage = new CCheckBoxWizardPage("Check Printer",PREPARATIONS_IMAGE_ID,wpPreviousDisabled);
	pPage->SubTitle = "Confirm before continuing:";
	pPage->Strings.Add("The air pressure gauge is removed.");
	pagesVector[vcDisconnectPurgeDevice] = pPage;
  }
  {
    CCloseDoorPage *pPage = new CCloseDoorPage(this,CAUTION_IMAGE_ID, wpPreviousDisabled | wpHelpNotVisible);
	pagesVector[vcCloseDoor2] = pPage;
  }
  {
	CRadioGroupWizardPage *pPage = new CRadioGroupWizardPage(WIZARD_COMPLETED_STR,SUCCESSFULLY_COMPLETED_IMAGE_ID,wpCancelDisabled | wpPreviousDisabled | wpDonePage | wpHelpNotVisible);
	pPage->SubTitle = "To apply changes, restart the printer application.";
    pPage->Strings.Add("Restart the printer application now.");
    pPage->Strings.Add("Do not restart the printer application now.");
    pPage->DefaultOption = RESTART_APPLICATION_NOW;
  	pagesVector[vcCompletionPage] = pPage;
  }
  {
    CCheckBoxWizardPage *pPage = new CLiquidsShortagePage(this,-1,wpNextWhenSelected | wpHelpNotVisible);
  	pagesVector[vcLiquidsShortagePage] = pPage;
  }
  {
    CMessageWizardPage *pPage = new CMessageWizardPage("Dummy Page",FIRST_PAGE_IMAGE_ID,wpPreviousDisabled | wpHelpNotVisible); // Purpose of this dummy page is that the LiquidsShortagePage will not be the last page in pages list. (which will make it a "Done" page by default)
  	pagesVector[vcDummyPage] = pPage;
  }
  {
	CMessageWizardPage *pPage = new CMessageWizardPage("Sensor Problem",-1,wpCancelDisabled | wpPreviousDisabled | wpDonePage | wpHelpNotVisible);
  	pagesVector[vcSensorProblemPage] = pPage;
  }
  for(TPagePointersIterator p = pagesVector.begin(); p != pagesVector.end(); p++)
   	AddPage(*p);
};


// Start the wizard session event
void CVacuumCalibrationWizard::StartEvent()
{
  m_IsRestartRequired = false;

  m_VacuumLineGain   = m_ParamMgr->VacuumLineGain;
  m_VacuumLineOffset = 0;
  m_BackEnd->EnableDisableVacuumErrorHandling(false);
}


// End the wizard session event
void CVacuumCalibrationWizard::EndEvent()
{
  CleanUp();
}


// Cancel the wizard session event
void CVacuumCalibrationWizard::CancelEvent(CWizardPage *WizardPage)
{
  CleanUp();
}

void CVacuumCalibrationWizard::PageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason)
{

  if (LeaveReason != lrGoNext)
    return;

  switch(WizardPage->GetPageNumber())
  {
    case vcGotoPurgePosition:
      m_BackEnd->LockDoor(false);
      while(m_BackEnd->IsDoorLocked())
      {
        YieldAndSleepWizardThread();
        if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
      }
      break;

	case vcUpdateParameters:
	{
      // Retrieve the current vacuum value...
	  m_CurrVacuumValue = m_BackEnd->GetCurrentVacuumValue_A2D();
	  QString ExceptionStr;
	  float avgVacuum  = m_BackEnd->GetAverageVacuumValue_A2D();
	  if (avgVacuum < MIN_SENSOR_REASONABLE_VALUE)
	  {
		 m_NextWasPressed = true;
		 SetNextPage(vcSensorProblemPage);
		 return;
	  }
	  else
	  if (avgVacuum > MAX_SENSOR_REASONABLE_VALUE)
	  {
		 m_NextWasPressed = true;
		 SetNextPage(vcSensorProblemPage);
		 return;
	  }

      UpdateParams();

	  m_NextWasPressed = true;
	  }
	  break;

	case vcWelcomePage:  //bug 6483
	{
	  // Start heating the block to save time
	  StartAsyncHeatingCycle(TOTAL_NUMBER_OF_HEATERS, true);
	}
	  break;
	case vcCheckPurgePressure:
	{
	  m_BackEnd->SetActuatorState(ACTUATOR_ID_AIR_VALVE, false);
	}
	break;
  }
}


void CVacuumCalibrationWizard::PageEnter(CWizardPage *WizardPage)
{
  switch(WizardPage->GetPageNumber())
  {
	case vcSensorProblemPage:
	{
     CMessageWizardPage *Page = dynamic_cast<CMessageWizardPage *>(WizardPage);
      Page->Refresh();
	}
	break;
    case vcGotoPurgePosition:
    {
      CSuspensionPointsStatusPage *Page = dynamic_cast<CSuspensionPointsStatusPage *>(WizardPage);
      Page->PointsStatusMessage = "Goto Purge Position";
      Page->Refresh();

      TQErrCode Err = Q_NO_ERROR;
      TRY_FUNCTION(Err, m_BackEnd->GotoPurgePosition(true, false));

      // Activating the Waste Pump
      m_BackEnd->SetActuatorState(ACTUATOR_ID_WASTE_PUMP,true);

      // Disabling All Axis
      m_BackEnd->EnableMotor(false, AXIS_ALL);

      GotoNextPage();
    }
    break;
	case vcPurge:
    {
	  CElapsingTimeWizardPage *Page = dynamic_cast<CElapsingTimeWizardPage *>(WizardPage);

	  // Heating the block...
	  HeadsHeatingCycle();
      if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
      Page->Refresh();

      // Doing some purge cycles.
		try
		{
		  if (PurgeCycle(NUMBER_OF_PURGES) != true)
		  {
			// Purge timeout
			QMonitor.ErrorMessage(TIMEOUT_REASON(IDS_PURGE),ORIGIN_WIZARD_PAGE);
            throw EQException(TIMEOUT_REASON(IDS_PURGE));
		  }
		}
		catch(EQHeadFillingException& E)
		{
			GotoPage(vcLiquidsShortagePage);
			return;	  
		}

      // Turn heating off

      GotoNextPage();
    }
    break;

    case vcUpdateParameters:
    {
      CVacuumCalibrationWizardPage *Page = dynamic_cast<CVacuumCalibrationWizardPage *>(WizardPage);

      m_NextWasPressed = false;

      // Update the Page...
      while (m_NextWasPressed == false)
      {
        // Retrieve Current & Average vacuum values...
        Page->CurrentVacuumValue = m_BackEnd->GetCurrentVacuumValue_A2D();
		Page->AverageVacuumValue = m_BackEnd->GetAverageVacuumValue_A2D();
        Page->Refresh();

        YieldAndSleepWizardThread();
      }
    }
    break;

    case vcWizardResults:
    {
      CMessageWizardPage *Page = dynamic_cast<CMessageWizardPage *>(WizardPage);
      Page->SubTitle = m_WizardResults;
      Page->Refresh();
    }
    break;

  }
}


void CVacuumCalibrationWizard::HelpEvent(CWizardPage *WizardPage)
{  
	switch(WizardPage->GetPageNumber())
	{
		case vcWelcomePage:			m_BackEnd->DispatchHelp(10, VACUUM_HELP_FILE_NAME); break;
		case vcConnectMeasDevice:	m_BackEnd->DispatchHelp(20, VACUUM_HELP_FILE_NAME); break;
		case vcCalibrateVacuum:		m_BackEnd->DispatchHelp(30, VACUUM_HELP_FILE_NAME); break;
		case vcDismantleDevice:		m_BackEnd->DispatchHelp(40, VACUUM_HELP_FILE_NAME); break;
		case vcUpdateParameters:	m_BackEnd->DispatchHelp(50, VACUUM_HELP_FILE_NAME); break;
		case vcConnectPurgeDevice:	m_BackEnd->DispatchHelp(60, VACUUM_HELP_FILE_NAME); break;
		case vcCheckPurgePressure:	m_BackEnd->DispatchHelp(70, VACUUM_HELP_FILE_NAME); break;
		case vcDisconnectPurgeDevice:	m_BackEnd->DispatchHelp(80, VACUUM_HELP_FILE_NAME); break;
		
		default: break;
	}
}

void CVacuumCalibrationWizard::CleanUp()
{
  m_BackEnd->EnableDisableVacuumErrorHandling(true);
  // Turn Off Waste Pump...
  m_BackEnd->SetActuatorState(ACTUATOR_ID_WASTE_PUMP,false);

  m_BackEnd->SetActuatorState(ACTUATOR_ID_AIR_VALVE, false);

  // Perform Home and wipe only if the Door is Closed...
  if (m_BackEnd->CheckIfDoorIsClosed() == Q_NO_ERROR)
  {
	m_BackEnd->WipeHeads(true);
	YieldWizardThread();
	EnableAxeAndHome(AXIS_T, true, true);
	EnableAxeAndHome(AXIS_Y, true, true);
	EnableAxeAndHome(AXIS_X, true, true);
	EnableAxeAndHome(AXIS_Z, true, true);	
	YieldWizardThread();
  }
  // Unlock the door
  m_BackEnd->EnableDoor(false);
}

void CVacuumCalibrationWizard::UpdateParams()
{
  QString   FileName  = Q2RTApplication->AppFilePath.Value() + LOAD_STRING(IDS_CONFIGS_DIRNAME) + LOAD_STRING(IDS_VACUUM_FILENAME);
  ofstream* INIFile   = NULL;

  // Updating Parameter Manager
  m_ParamMgr->VacuumHighLimit         = m_CurrVacuumValue + VACUUM_LIMITS_THRESHOLD;
  m_ParamMgr->VacuumLowLimit          = m_CurrVacuumValue - VACUUM_LIMITS_THRESHOLD;
  m_ParamMgr->VacuumHighCriticalLimit = m_CurrVacuumValue + VACUUM_CRITICAL_LIMITS_THRESHOLD;
  m_ParamMgr->VacuumLowCriticalLimit  = m_CurrVacuumValue - VACUUM_CRITICAL_LIMITS_THRESHOLD;

  m_ParamMgr->SaveSingleParameter(&m_ParamMgr->VacuumHighLimit);
  m_ParamMgr->SaveSingleParameter(&m_ParamMgr->VacuumLowLimit);
  m_ParamMgr->SaveSingleParameter(&m_ParamMgr->VacuumHighCriticalLimit);
  m_ParamMgr->SaveSingleParameter(&m_ParamMgr->VacuumLowCriticalLimit);

  m_VacuumLineOffset = m_CurrVacuumValue - (m_VacuumLineGain * VACUUM_MEASURED_VALUE);


  // Rewriting INI file....
  INIFile = new ofstream(FileName.c_str());

  if (INIFile != NULL)
  {
    int A2DValue;
    QString res;
    for (int i = A2D_FILE_MIN_VALUE; i < A2D_FILE_MAX_VALUE; i++)
    {
      A2DValue = (m_VacuumLineGain * i) + m_VacuumLineOffset;
      res += QFormatStr("%d=%d\n", A2DValue, i);
    }
    *INIFile << res;
    m_WizardResults = QFormatStr(LOAD_STRING(IDS_VACUUM_PARAMETERS_CALIBRATED), LOAD_STRING(IDS_VACUUM_FILENAME));
    delete INIFile;
  }
  else
    m_WizardResults = QFormatStr(LOAD_STRING(IDS_VACUUM_PARAMETERS_ERROR), LOAD_STRING(IDS_VACUUM_FILENAME));
}

bool CVacuumCalibrationWizard::PurgeCycle(int NumberOfPurgeCycles)
{
  CBEMonitorActivator MonitorActivator; //meaning m_BackEnd->TurnHeadFilling(true);
  for (int i = NumberOfPurgeCycles; i > 0 ; i--)
  {
	HeadsFillingCycle(m_ParamMgr->HeadsHeatingTimeoutSec);

    if (Q2RTWizardSleep(m_ParamMgr->MRW_DelayBetweenPurges) == false)
      return true;

    m_BackEnd->Purge(true);
  }

  if (Q2RTWizardSleep(m_ParamMgr->MotorPostPurgeTimeSec) == false)
     return true;

  m_BackEnd->WipeHeads(false);
  m_BackEnd->WipeHeads(false);
  TQErrCode Err = Q_NO_ERROR;
  TRY_FUNCTION(Err, m_BackEnd->GotoPurgePosition(true, false));
  return true;
}


bool CVacuumCalibrationWizard::IsRestartRequired()
{
  return m_IsRestartRequired;
}

void CVacuumCalibrationWizard::PageEventsHandler(CWizardPage *WizardPage,int Param1,int Param2)
{
  int PageIndex = WizardPage->GetPageNumber();
  switch(PageIndex)
  {
    case vcCompletionPage:
      m_IsRestartRequired = (dynamic_cast<CRadioGroupWizardPage *>(WizardPage)->GetSelectedOption() == RESTART_APPLICATION_NOW);
      break;
  }
}

void CVacuumCalibrationWizard::UserButton1Event(CWizardPage *WizardPage)
{
 CMessageWizardPage *Page = dynamic_cast<CMessageWizardPage *>(WizardPage);
 bool TurnAirValveOn = !(m_BackEnd->GetActuatorOnOff(ACTUATOR_ID_AIR_VALVE));
 m_BackEnd->SetActuatorState(ACTUATOR_ID_AIR_VALVE, TurnAirValveOn);

 if(TurnAirValveOn)
 {
   Page->SubTitle =	CHECK_PURGE_PRESSURE_AIR_VALVE_ON;
 }
 else
 {
   Page->SubTitle =	CHECK_PURGE_PRESSURE_AIR_VALVE_OFF;
 }

 Page->Refresh();
}
