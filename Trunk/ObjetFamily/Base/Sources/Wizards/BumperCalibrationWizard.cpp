/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Bumper Calibration.                                      *
 * Module Description: Bumper calibration wizard.                   *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Nir Sade.                                                *
 * Start date: 18/12/2003                                           *
 * Last upate: 18/12/2003                                           *
 ********************************************************************/

#include "BumperCalibrationWizard.h"
#include "BackEndInterface.h"
#include "MotorDefs.h"
#include "AppParams.h"
#include "AppLogFile.h"
#include "Configuration.h"
#include "QThreadUtils.h"
#include "Q2RTApplication.h"
#include "WizardImages.h"
#include "QScopeRestorePoint.h"

const int TRAY_OFFSET_MICRONS   = 120;

const int MIN_SENSITIVITY_VALUE = 0;
const int MAX_SENSITIVITY_VALUE = 4095;
const int SENSITIVITY_STEPS = 50;
const int SENSITIVITY_FINE_STEPS = 10;
const int DELTA_FROM_SENSITIVITY_THRESHOLD = 450;
                                                           
const int Z_POSITION_STEPS             = 10;
const int Z_AXIS_SAFETY_MARGIN         = 100; // steps
const int Z_BACKLASH_MOVEMENT          = 5000; // steps

const QString ROLLER_CUBE_FILE_NAME    = "CalibrationFiles\\Roller\\Roller Cube.bmp";
const int ROLLER_CUBE_SLICE_NUM        = 50;
const int ROLLER_CUBE_PRINT_MODE       = 0;

const int LEFT_X_LOOP_POS = 5; // mm
const int RIGHT_X_LOOP_POS = 250; // mm

const int ROLLER_CUBE_WITDH               = 6500;
const int ROLLER_CUBE_BUMPER_NUM_OF_FIRES = 3250;
const int BUMPER_IMPACT_MSG_DELAY_TIME    = 500; //ms



enum {bcWelcomePage,
	  bcIsTrayInserted,
      bcCheckTrayClean,
      bcCloseDoor,
      bcThresholdCalibration,
      bcPrintCube,
      bcCalibrateSensitivity,
      bcRemoveCube,
      bcWizardCompleted,
      
      bcPagesCount // this must element must be the last in this enum

};

CBumperCalibrationWizard::CBumperCalibrationWizard(void) : CQ2RTAutoWizard(IDS_BC_WIZARD,true,IN_PROCESS_IMAGE_ID)
{
   m_SensitivityThreshold = 0;
   m_RequiredTrayOffset = 0;
   m_CurrTrayOffset = 0;
   m_BumperSensitivity = 0 ;
   m_PrevBumperStopAfterImpact = 0;
   m_PrevBumperBypassParamValue = 0;
   m_MaterialPermissiveCount = 0;
  TPagePointers pagesVector(bcPagesCount, 0);
  {
	CMessageWizardPage *pPage = new CMessageWizardPage(GetTitle(),IN_PROCESS_IMAGE_ID, wpHelpNotVisible);
	pPage->SubTitle = LOAD_STRING(IDS_BC_WIZARD_WELCOME);
	pagesVector[bcWelcomePage] = pPage;
  }
  {
	CInsertTrayPage *pPage = new CInsertTrayPage(this, PREPARATIONS_IMAGE_ID, wpPreviousDisabled | wpHelpNotVisible);
  	pagesVector[bcIsTrayInserted] = pPage;
  }
  {
    CVerifyCleanTrayPage *pPage = new CVerifyCleanTrayPage(this,PREPARATIONS_IMAGE_ID, wpPreviousDisabled);
  	pagesVector[bcCheckTrayClean] = pPage;
  }
  {
	CCloseDoorPage *pPage = new CCloseDoorPage(this,PREPARATIONS_IMAGE_ID, wpPreviousDisabled);
  	pagesVector[bcCloseDoor] = pPage;
  }
  {
	CSuspensionPointsStatusPage *pPage = new CSuspensionPointsStatusPage("",BUMPER_SENSOR_IMAGE_ID,wpNextDisabled | wpPreviousDisabled);
    pPage->PointsStatusMessage = LOAD_STRING(IDS_SENSITIVITY_THRESHOLD);
  	pagesVector[bcThresholdCalibration] = pPage;
  }
  {
  CElapsingTimeWizardPage *pPage = new CElapsingTimeWizardPage("Printing Model",IN_PROCESS_IMAGE_ID,wpNextDisabled | wpPreviousDisabled);
  	pagesVector[bcPrintCube] = pPage;
  }
  {
	CSuspensionPointsStatusPage *pPage = new CSuspensionPointsStatusPage(GetTitle(),BUMPER_SENSOR_IMAGE_ID,wpNextDisabled | wpPreviousDisabled | wpCancelDisabled);
    pPage->PointsStatusMessage = "Bumper sensitivity calibration";
  	pagesVector[bcCalibrateSensitivity] = pPage;
  }
  {
  	CCheckBoxWizardPage *pPage = new COneCheckBoxWizardPage(this, "Confirm before continuing", "Bumper-Sensor calibration cube is removed" ,PREPARATIONS_IMAGE_ID,wpPreviousDisabled | wpHelpNotVisible);
  	pagesVector[bcRemoveCube] = pPage;
  }
  
  {
  	CMessageWizardPage *pPage = new CMessageWizardPage(WIZARD_COMPLETED_STR,SUCCESSFULLY_COMPLETED_IMAGE_ID, wpPreviousDisabled | wpHelpNotVisible | wpDonePage);
  	pagesVector[bcWizardCompleted] = pPage;
  }
  for(TPagePointersIterator p = pagesVector.begin(); p != pagesVector.end(); p++)
   	AddPage(*p);

};

// Start the wizard session event
void CBumperCalibrationWizard::StartEvent()
{
  m_CurrTrayOffset     = 0;
  m_RequiredTrayOffset = (float)TRAY_OFFSET_MICRONS;

  // Save the previous value of 'BumperStopAfterImpact' and 'BumperBypass' parameters
  m_PrevBumperStopAfterImpact       = m_ParamMgr->BumperStopAfterImpact;
  m_ParamMgr->BumperStopAfterImpact = false;
  m_PrevBumperBypassParamValue      = m_ParamMgr->BumperBypass;
  m_ParamMgr->BumperBypass          = false;
  m_BumperSensitivity               = m_ParamMgr->BumperSensivity;
  }

void CBumperCalibrationWizard::CleanUp()
{
  m_ParamMgr->BumperStopAfterImpact = m_PrevBumperStopAfterImpact;
  m_ParamMgr->BumperBypass = m_PrevBumperBypassParamValue;
  // Unlock the Door
  m_BackEnd->LockDoor(false);
}

// End the wizard session event
void CBumperCalibrationWizard::EndEvent()
{
  m_ParamMgr->BumperSensivity = m_BumperSensitivity;
  m_ParamMgr->SaveSingleParameter(&m_ParamMgr->BumperSensivity);
  m_BackEnd->GoToMotorHomePosition(AXIS_X,true);
  m_BackEnd->GoToMotorHomePosition(AXIS_Y,true);
  m_BackEnd->MoveMotorToAbsolutePosition(AXIS_Z, m_ParamMgr->Z_StartPrintPosition, true, muMM);
  CleanUp();
}

// Cancel the wizard session event
void CBumperCalibrationWizard::CancelEvent(CWizardPage *WizardPage)
{
  CleanUp();
  m_BackEnd->EnableDisableBumper(false);
  m_BackEnd->EnableDisableBumperPeg(true);
  m_BackEnd->TurnRoller(false);
}

void CBumperCalibrationWizard::HelpEvent(CWizardPage *WizardPage)
{
	Application->HelpFile = DefaultHelpFileName().c_str();
	switch(WizardPage->GetPageNumber())
	{
		case bcWelcomePage: Application->HelpContext(10); break;
		case bcPrintCube: Application->HelpContext(20); break;
		case bcWizardCompleted: Application->HelpContext(30); break;
        default: break;
	}
}

void CBumperCalibrationWizard::PageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason)
{
  if (LeaveReason != lrGoNext)
     return; 

  switch(WizardPage->GetPageNumber())
  {
	case bcWelcomePage:
	{
		 m_MaterialPermissiveCount = 0;
		 m_BackEnd->GotoDefaultMode();
		 int qm = ROLLER_CUBE_PRINT_MODE; //=0, HS
		 int om = m_ParamMgr->PrintingOperationMode;
		 m_BackEnd->EnterMode(PRINT_MODE[qm][om], MACHINE_QUALITY_MODES_DIR(qm,om));

         for(int i = FIRST_MODEL_CHAMBER_INDEX; i < LAST_MODEL_CHAMBER_INDEX; ++i)
			 if (m_ParamMgr->BumperCalibrationPermissiveArray[i])
			 {
				 if(IsChamberRelatedToTank((TChamberIndex)i,(TTankIndex)(2*i)) || IsChamberRelatedToTank((TChamberIndex)i,(TTankIndex) (2*i+1)))
                       			m_MaterialPermissiveCount++;
			 }
		 m_BackEnd->GotoDefaultMode();	 
		 if (0 == m_MaterialPermissiveCount)
			 throw EQException(QFormatStr(LOAD_STRING(IDS_BC_ILLEGAL_MATERIALS), LOAD_STRING(IDS_RR_WIZARD)));
	}
		 break;


    case bcCheckTrayClean:
         EnableDisableNext(false);
         //Do not call EnableAllAxesAndHome here, because it'll try to close the door.
         //The next page is THERE to check if the door is closed.
         //If the door is open, the user will see a a 'please close the door' dialog
         //together with the wizard page. 
         break;
  }
}


void CBumperCalibrationWizard::ThresholdCalibrationPageEnter(CWizardPage *WizardPage)
{
  EnableDisableNext(false);
  m_SensitivityThreshold = FindThresholdValue(MIN_SENSITIVITY_VALUE, MAX_SENSITIVITY_VALUE);
  m_BackEnd->EnableDisableBumperPeg(true);
	WriteToLogFile(LOG_TAG_GENERAL,"sensitivity threshold = %d", m_SensitivityThreshold);
}

bool CBumperCalibrationWizard::PrintCubePageEnter(CWizardPage *WizardPage)
{
   WriteToLogFile(LOG_TAG_GENERAL,"CBumperCalibrationWizard::PrintCubePageEnter:FirstEnter:m_ParamMgr->TrayStartPositionX = %d m_ParamMgr->TrayStartPositionY = %d ",m_ParamMgr->TrayStartPositionX.Value(), m_ParamMgr->TrayStartPositionY.Value());

   TFileNamesArray FileNames = (TFileNamesArray)malloc(TFileNamesArraySIZE);
   for (int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
       strcpy((TBMPFileName)FileNames + i*MAX_FILE_PATH, "");

   m_BackEnd->GotoDefaultMode();
   CScopeRestorePoint<bool> scKeepDoorLocked(m_ParamMgr->KeepDoorLocked);
   m_ParamMgr->KeepDoorLocked = true;
   int qm = ROLLER_CUBE_PRINT_MODE; //=0, HS   
   int om = m_ParamMgr->PrintingOperationMode;
   m_BackEnd->EnterMode(PRINT_MODE[qm][om], MACHINE_QUALITY_MODES_DIR(qm,om));
   m_BackEnd->EnterMode("~Roller Cube", GENERAL_MODES_DIR);
   m_BackEnd->EnterMode("~RollerCube_" + GetModeShortStr(qm, om), MACHINE_QUALITY_MODES_DIR(qm, om));
   	   
   for (int i = FIRST_MODEL_CHAMBER_INDEX; i < LAST_MODEL_CHAMBER_INDEX; ++i)
   {
	   if (m_ParamMgr->BumperCalibrationPermissiveArray[i])
	   {
           //check that related tanks are not disabled
		   //[@Assumption] A chamber has a maximum of 2 possible tanks attached. If otherwise - revise the if-condition
		   if(IsChamberRelatedToTank(static_cast<TChamberIndex>(i), static_cast<TTankIndex>(2*i)) || IsChamberRelatedToTank(static_cast<TChamberIndex>(i), static_cast<TTankIndex>(2*i+1)))
		   {
			 if(om==SINGLE_MATERIAL_OPERATION_MODE)
			   i = i/2+i%2;

			 strcpy((TBMPFileName)FileNames + i*MAX_FILE_PATH, (Q2RTApplication->AppFilePath.Value() + ROLLER_CUBE_FILE_NAME).c_str());
			 break;
		   }
       }
   }
   
   if( (Q2RTApplication->GetMachineType()) == (mtObjet1000 ))
   {

	   WriteToLogFile(LOG_TAG_GENERAL,"CBumperCalibrationWizard::PrintCubePageEnter objet1000");
	   WriteToLogFile(LOG_TAG_GENERAL,"OBJET1000 = Before changing:m_ParamMgr->TrayStartPositionX = %d ", (m_ParamMgr->TrayStartPositionX.Value()));
		WriteToLogFile(LOG_TAG_GENERAL,"OBJET1000 = m_ParamMgr->TrayStartPositionY = %d ",  (m_ParamMgr->TrayStartPositionY.Value()));
		WriteToLogFile(LOG_TAG_GENERAL,"OBJET1000 = m_ParamMgr->TrayHighestPoint[0] = %d" , (m_ParamMgr->TrayHighestPoint[0].Value()));
		WriteToLogFile(LOG_TAG_GENERAL,"OBJET1000 = m_ParamMgr->TrayHighestPoint[1]= %d " , (m_ParamMgr->TrayHighestPoint[1].Value()));

	   CScopeRestorePoint<float> scKeepXStartPosition(m_ParamMgr->TrayStartPositionX);
       CScopeRestorePoint<float> scKeepYStartPosition(m_ParamMgr->TrayStartPositionY);
	   
		m_ParamMgr->TrayStartPositionX = m_ParamMgr->TrayHighestPoint[0];
		m_ParamMgr->TrayStartPositionY = m_ParamMgr->TrayHighestPoint[1];
		

		WriteToLogFile(LOG_TAG_GENERAL,"OBJET1000 = After changing:  m_ParamMgr->TrayStartPositionX = %d ", (m_ParamMgr->TrayStartPositionX.Value()));
		WriteToLogFile(LOG_TAG_GENERAL,"OBJET1000 = m_ParamMgr->TrayStartPositionY = %d ",  (m_ParamMgr->TrayStartPositionY).Value());

		m_BackEnd->LoadLocalPrintJob( FileNames,DPI[qm], qm, om, ROLLER_CUBE_SLICE_NUM);
		m_BackEnd->SetOnlineOffline(true);
		WaitForEndOfPrint();
   }
   else
   {
   
	   WriteToLogFile(LOG_TAG_GENERAL,"CBumperCalibrationWizard::PrintCubePageEnter is not objet1000");
	   WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Resolution is %d",m_ParamMgr->PrintResolution_InXAxis.Value()));
	   m_BackEnd->LoadLocalPrintJob( FileNames, 600, qm, om, ROLLER_CUBE_SLICE_NUM);
	   m_BackEnd->SetOnlineOffline(true);
	   WaitForEndOfPrint();


   }

  

   WriteToLogFile(LOG_TAG_GENERAL,"OBJET1000 = End Scop:  m_ParamMgr->TrayStartPositionX = %d ",(m_ParamMgr->TrayStartPositionX.Value()));
   WriteToLogFile(LOG_TAG_GENERAL,"OBJET1000 = m_ParamMgr->TrayStartPositionY = %d ", ( m_ParamMgr->TrayStartPositionY.Value()));


   if(FileNames)
      free(FileNames);
   EnableAllAxesAndHome();
   // Lower Z axis down to the height of the roller cube
   m_BackEnd->MoveMotorToAbsolutePosition(AXIS_Z, m_ParamMgr->Z_StartPrintPosition, true, muMM);
   m_BackEnd->MoveMotorRelative(AXIS_Z, Z_BACKLASH_MOVEMENT, true);
   long Offset = CONFIG_ConvertZumToStep(ROLLER_CUBE_SLICE_NUM * m_ParamMgr->LayerHeightDPI_um) - Z_BACKLASH_MOVEMENT;
   m_BackEnd->MoveMotorRelative(AXIS_Z, Offset, true);
   m_BackEnd->GotoDefaultMode();
   return true;
}

bool CBumperCalibrationWizard::CalibrateSensitivityPageEnter(CWizardPage *WizardPage)
{
 try
 {
   WriteToLogFile(LOG_TAG_GENERAL,"CBumperCalibrationWizard::CalibrateSensitivityPageEnter()");
   YieldWizardThread();
   if( (Q2RTApplication->GetMachineType()) == (mtObjet1000 ))
   {
   	   WriteToLogFile(LOG_TAG_GENERAL,"OBJET1000 = m_ParamMgr->TrayHighestPoint[1] = %d", QIntToStr(m_ParamMgr->TrayHighestPoint[1].Value()));
       m_BackEnd->MoveMotorToAbsolutePosition(AXIS_Y, m_ParamMgr->TrayHighestPoint[1], true, muMM);
   }
   else
   {
		WriteToLogFile(LOG_TAG_GENERAL,"  m_ParamMgr->BumperCalibrationYPosition = %d",  QIntToStr(m_ParamMgr->BumperCalibrationYPosition.Value()));
		m_BackEnd->MoveMotorToAbsolutePosition(AXIS_Y, m_ParamMgr->BumperCalibrationYPosition, true, muMM);
   }
   // Move Z axis up according to the value eneterd by the user
   long DeltaOffset = m_RequiredTrayOffset - m_CurrTrayOffset;
   m_BackEnd->MoveMotorRelative(AXIS_Z, CONFIG_ConvertZumToStep(-DeltaOffset), true);
   m_CurrTrayOffset = m_RequiredTrayOffset;
   // Turn roller on
   m_BackEnd->TurnRoller(true, m_ParamMgr->RollerVelocity);
   CScopeRestorePoint<float> scKeepXStartPosition(m_ParamMgr->TrayStartPositionX);
   CScopeRestorePoint<float> scKeepYStartPosition(m_ParamMgr->TrayStartPositionY);
   m_ParamMgr->TrayStartPositionX = m_ParamMgr->TrayHighestPoint[0];
   m_ParamMgr->TrayStartPositionY = m_ParamMgr->TrayHighestPoint[1];
   // Set the bumper PEG values
   long StartPEG_step = (CONFIG_GetPrintXStartPosition_step() - m_ParamMgr->XEncoderHomeInStep);
  
   int  StartPEG      = (float)StartPEG_step / m_ParamMgr->XStepsPerPixel;
   m_BackEnd->SetLayerPrintParameters(StartPEG, StartPEG + ROLLER_CUBE_WITDH, ROLLER_CUBE_BUMPER_NUM_OF_FIRES);
   m_BackEnd->EnableDisableBumperPeg(true);
   m_BackEnd->EnableDisableBumper(true);
   m_BumperSensitivity = FindSensitivityValue(m_SensitivityThreshold, MAX_SENSITIVITY_VALUE);
   m_BackEnd->TurnRoller(false); // Turn roller off
   if(IsCancelled())  throw CQ2RTAutoWizardCancelledException();
   WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Z = %d, sensitivity = %d",m_BackEnd->GetMotorsPosition(AXIS_Z), m_BumperSensitivity));
   EnableAllAxesAndHome();
   EnableAxeAndHome(AXIS_Y, true, true);
   EnableAxeAndHome(AXIS_X, true, true);
   // Go to Z start position
   m_BackEnd->MoveMotorToAbsolutePosition(AXIS_Z, m_ParamMgr->Z_StartPrintPosition, true, muMM);
   m_BackEnd->LockDoor(false); // Unlock the Door
 }
 catch( CQ2RTAutoWizardCancelledException& Exception )
 {
     m_BackEnd->EnableDisableBumper(false);
     m_BackEnd->ResetBumperImpactStatus();
     return false;
 }
 return true;
}

void CBumperCalibrationWizard::PageEnter(CWizardPage *WizardPage)
{

  switch(WizardPage->GetPageNumber())
  {


    case bcThresholdCalibration:
         EnableAllAxesAndHome();
         ThresholdCalibrationPageEnter(WizardPage);
         GotoNextPage();
         break;

    case bcPrintCube:
         if (PrintCubePageEnter(WizardPage) == false)
            return;
         GotoNextPage();
         break;

    case bcCalibrateSensitivity:
         if (CalibrateSensitivityPageEnter(WizardPage) == false)
            return;
         GotoNextPage();
         break;

	case bcWizardCompleted:
    {
         CMessageWizardPage *Page = dynamic_cast<CMessageWizardPage *>(WizardPage);
         Page->SubTitle = "New Bumper sensitivity value = " + QIntToStr(m_BumperSensitivity);
         Page->Refresh();
         break;
    }
  }
}

int CBumperCalibrationWizard::FindThresholdValue(int LowValue, int HighValue)
{
  if (IsCancelled())
    return 0;

  if (LowValue > HighValue)
    return 0;

  if (HighValue - LowValue <= SENSITIVITY_FINE_STEPS)
    return LowValue;

  int MidValue = (HighValue + LowValue) / 2;

  m_BackEnd->EnableDisableBumper(false);
  m_BackEnd->EnableDisableBumperPeg(true);
  m_BackEnd->ResetBumperImpactStatus();
  m_BackEnd->SetBumperParams(MidValue, m_ParamMgr->BumperResetTime , m_ParamMgr->BumperImpactCounter);
  m_BackEnd->EnableDisableBumper(true);
  m_BackEnd->EnableDisableBumperPeg(false);

  if (m_BackEnd->GetBumperImpactStatus())
    return FindThresholdValue(MidValue, HighValue);

  return FindThresholdValue(LowValue, MidValue-1);
}


int CBumperCalibrationWizard::FindSensitivityValue(int LowValue, int HighValue)
{
  if (IsCancelled())
    return 0;

  if (LowValue > HighValue)
    return 0;

  if (HighValue - LowValue <= SENSITIVITY_FINE_STEPS)
    return LowValue;

  int MidValue = (HighValue + LowValue) / 2;

  m_BackEnd->ResetBumperImpactStatus();
  m_BackEnd->SetBumperParams(MidValue, m_ParamMgr->BumperResetTime , m_ParamMgr->BumperImpactCounter);

   if( (Q2RTApplication->GetMachineType()) == (mtObjet1000 ))
   {
	   WriteToLogFile(LOG_TAG_GENERAL,"FindSensitivityValue(): MoveMotorToAbsolutePosition to TrayHighestPoint[0]");
	   WriteToLogFile(LOG_TAG_GENERAL,"TrayHighestPoint[0] = %d", QIntToStr((m_ParamMgr->TrayHighestPoint[0].Value())));
	   m_BackEnd->MoveMotorToAbsolutePosition(AXIS_X, CONFIG_ConvertXmmToStep(m_ParamMgr->TrayHighestPoint[0]-120), true);
	   WriteToLogFile(LOG_TAG_GENERAL,"FindSensitivityValue(): MoveMotorToAbsolutePosition to TrayHighestPoint[0]+BumperCalibrationDelta");
	   WriteToLogFile(LOG_TAG_GENERAL,"BumperCalibrationDelta.Value() = %d", QIntToStr(m_ParamMgr->BumperCalibrationDelta.Value()));
	   m_BackEnd->MoveMotorToAbsolutePosition(AXIS_X, CONFIG_ConvertXmmToStep(m_ParamMgr->TrayHighestPoint[0]+m_ParamMgr->BumperCalibrationDelta.Value()), true);
   }
   else
   {
	   WriteToLogFile(LOG_TAG_GENERAL,"FindSensitivityValue(): MoveMotorToAbsolutePosition to RIGHT_X_LOOP_POS+BumperCalibrationDelta= %d", QIntToStr((RIGHT_X_LOOP_POS+m_ParamMgr->BumperCalibrationDelta.Value())));
	   m_BackEnd->MoveMotorToAbsolutePosition(AXIS_X, CONFIG_ConvertXmmToStep(RIGHT_X_LOOP_POS+m_ParamMgr->BumperCalibrationDelta.Value()), true);
	   WriteToLogFile(LOG_TAG_GENERAL,"FindSensitivityValue(): MoveMotorToAbsolutePosition to LEFT_X_LOOP_POS+BumperCalibrationDelta=%d", QIntToStr((LEFT_X_LOOP_POS+m_ParamMgr->BumperCalibrationDelta.Value())) );
	   m_BackEnd->MoveMotorToAbsolutePosition(AXIS_X, CONFIG_ConvertXmmToStep(LEFT_X_LOOP_POS+m_ParamMgr->BumperCalibrationDelta.Value()), true);
   }



  if (m_BackEnd->GetBumperImpactStatus())
    return FindSensitivityValue(MidValue, HighValue);

  return FindSensitivityValue(LowValue, MidValue-1);
}



