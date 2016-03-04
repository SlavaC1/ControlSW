/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT.                                                   *
 * Module: test pattern sequencer.                                  *
 * Module Description: Implementation of the specific Q2RT          *
 *                     test pattern                                 *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger.                                         *
 * Start date: 2/june/2002                                          *
 ********************************************************************/
 
#include "MachineSequencer.h"
#include "Q2RTErrors.h"
#include "GlobalDefs.h"
#include "HeadHeaters.h"
#include "UVLamps.h"
#include "TrayHeater.h"
#include "Power.h"
#include "Door.h"
#include "HeadFilling.h"
#include "Motor.h"
#include "LayerProcess.h"
#include "Layer.h"
#include "TestPatternLayer.h"
#include "configuration.h"
#include "Q2RTApplication.h"
#include "PrintControl.h"
#include "FIFOPci.h"
#include "FrontEnd.h"
#include "AppLogFile.h"
#include "Actuator.h"
#include "HeadStatus.h"
#include "Tester.h"
#include "MotorMcb.h"
#include "FEResources.h"
#include "QScopeRestorePoint.h"

const int DWORDS_IN_ALL_HEADS = 24;


void CMachineSequencer::StopSequence(TMachineState State)
{
   //this procedure is to be used only to stop from Machine Manager

   switch(State)
      {
      case msTestPattern:
         m_TestPatternStopped = true;
         m_HeadHeaters->Cancel();
         m_HeadFilling->Cancel();
         m_UVLamps->Cancel();
         return;

      case msFireAll:
         m_FireAllStopped = true;
         m_HeadHeaters->Cancel();
         m_HeadFilling->Cancel();
         return;

      case msPurge:
         m_PurgeStopped = true;
         m_HeadHeaters->Cancel();
         m_HeadFilling->Cancel();
         return;

      default:break;
      }
}

//Encapsulate test pattern to verify the status when returning from test pattern
TQErrCode CMachineSequencer::TestPattern(bool AskToPrintInCurrentPosition, bool MoveAxisZInit)
{
  TQErrCode Err;
  try
  {
    CParamScopeRestorePointArray<int,TOTAL_NUMBER_OF_HEATERS> sc(m_ParamsMgr->HeatersTemperatureArray);
    CScopeRestorePoint<TMachineState> scKeepMachineState(m_CurrentMachineState);

    m_CurrentMachineState = msTestPattern;
    // Before starting the PT - rise the temperature in TestPatternTempDelta
    for (int i = FIRST_SUPPORT_HEAD; i < LAST_SUPPORT_HEAD; i++)
      m_ParamsMgr->HeatersTemperatureArray[i] -= m_ParamsMgr->TestPatternSupportTempDelta;
    for (int i = FIRST_MODEL_HEAD; i < LAST_MODEL_HEAD; i++)
      m_ParamsMgr->HeatersTemperatureArray[i] -= m_ParamsMgr->TestPatternModelTempDelta;

    Err = TestPatternSequence(AskToPrintInCurrentPosition, MoveAxisZInit);
  }
  catch (EQException& err)
  {
    throw EMachineSequencer(err.GetErrorMsg(),err.GetErrorCode());
  }
  return Err;
}


// Print a test pattern single layer
TQErrCode CMachineSequencer::TestPatternSequence(bool AskToPrintInCurrentPosition, bool MoveAxisZInit)
{
   TQErrCode Err;
   long Location;

   m_TestPatternStopped=false;

   CQLog::Write(LOG_TAG_GENERAL,"Performing Pattern test");
   FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Start Pattern Test");

   // wait until the user inserts the tray before continuing
   m_TrayHeater->WaitUntilTrayInserted();

   if(m_TestPatternStopped)
     return Q2RT_STOP_DURING_TESTPATTERN;

   //Turns the interlock on
   if((Err=m_Door->Enable())!= Q_NO_ERROR)
      return Err;


   bool MoveAxisZ = MoveAxisZInit;

//#if defined EDEN_500 || defined EDEN_350 || defined EDEN_350_V || defined CONNEX_500 || defined CONNEX_350 || defined OBJET_350 || defined OBJET_500 || defined OBJET_1000
  if(m_ParamsMgr->OfficeProfessionalType==PROFESSIONAL_MACHINE) //runtime objet
  {
    if(!m_ParamsMgr->AutoPrintCurrentZLocation && AskToPrintInCurrentPosition)
        MoveAxisZ = !QMonitor.AskYesNo("Is the build tray clear?\n(If you click 'NO', the tray will be lowered.)");
  }
  else //office machine
  {
//#else // small chasis machines - TP has to be printed on the tray, so we have to prepare Z axis
    MoveAxisZ = true;
  }
//#endif

   //Initiate motor parameters
   if ((Err=m_Motors->InitMotorParameters()) != Q_NO_ERROR)
      return Err;

   if(m_TestPatternStopped)
     return Q2RT_STOP_DURING_TESTPATTERN;

// Heat heads: Turn heads heating on
   if((Err = m_HeadHeaters->SetDefaultHeateresTemperature()) != Q_NO_ERROR)
   {
      QMonitor.WarningMessage("Set heater temperature error");
      return Err;
   }

   if((Err = m_HeadHeaters->SetDefaultPrintingHeadsVoltages()) != Q_NO_ERROR)
   {
      QMonitor.WarningMessage("Set default heads voltages error");
      return Err;
   }

   if ((Err = m_HeadHeaters->SetDefaultOnOff(HEAD_HEATER_ON)) != Q_NO_ERROR)
   {
      QMonitor.WarningMessage("Set heater on error");
      return Err;
   }

   if((Err = m_Vacuum->SetDefaultVacuumParams()) != Q_NO_ERROR)
   {
      QMonitor.WarningMessage("Set default vacuum error");
      return Err;
   }

   FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Warming heads");

   if(m_TestPatternStopped)
     return Q2RT_STOP_DURING_TESTPATTERN;

   // Wait for heads heating
   if((Err = m_HeadHeaters->WaitForHeadsTemperatureOK(false)) != Q_NO_ERROR)
   {
      if(Err == Q2RT_HEAD_HEATER_STOPPED_OR_CANCELED)
      {
        CQLog::Write(LOG_TAG_PRINT,"Test Pattern stopped");
        return Q_NO_ERROR;
      }

      return Err;
   }

  FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Filling heads");
   //Turn Heads liquid monitoring ON
  if((Err = m_HeadFilling->SetDefaultParms()) != Q_NO_ERROR)
     return Err;

  if(m_TestPatternStopped)
     return Q2RT_STOP_DURING_TESTPATTERN;

  CMonitorActivator MonitorActivator(m_HeadFilling); //meaning HeadFillingOnOff(true);

   if(m_TestPatternStopped)
     return Q2RT_STOP_DURING_TESTPATTERN;

  if((Err = m_HeadFilling->WaitForFilledHeadContainer()) != Q_NO_ERROR)
     return Err;

  FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Setting Dirt pump");

  //set Support/Model valve parameters
  //May be one actuator should be turned here
  //Turn pump gifa ON
  if ((Err = m_Actuator->SetDirtPumpOnOff(true) ) != Q_NO_ERROR)
     return Err;

   if(m_TestPatternStopped)
     return Q2RT_STOP_DURING_TESTPATTERN;

   // Turn UV lamps ON if required
   if(m_ParamsMgr->TestPatternUvLampON)
      if((Err = m_UVLamps->Enable()    ) != Q_NO_ERROR)
         return Err;

   FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Setting Motor Parameters");

   // Monitor On for Material fillings - OCB
   if ((Err = m_Motors->SetMotorEnableDisable(AXIS_ALL,MOTOR_ENABLE)) != Q_NO_ERROR)
      return Err;

   FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Home Axes");

   if(m_TestPatternStopped)
     return Q2RT_STOP_DURING_TESTPATTERN;

   // Home for X,Y,T
   if ((Err = m_Motors->GoWaitHomeSequence(AXIS_T)) != Q_NO_ERROR)
      return Err;

   if ((Err = m_Motors->GoWaitHomeSequence(AXIS_Y)) != Q_NO_ERROR)
      return Err;

   if(m_TestPatternStopped)
     return Q2RT_STOP_DURING_TESTPATTERN;

   if ((Err = m_Motors->GoWaitHomeSequence(AXIS_X)) != Q_NO_ERROR)
      return Err;

   if(m_TestPatternStopped)
     return Q2RT_STOP_DURING_TESTPATTERN;

   FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Moving Axis");
   if(MoveAxisZ)
   {
      if ((Err = m_Motors->GoWaitHomeSequence(AXIS_Z)) != Q_NO_ERROR)
         return Err;

      if(m_TestPatternStopped)
        return Q2RT_STOP_DURING_TESTPATTERN;
      if(m_ParamsMgr->OfficeProfessionalType==PROFESSIONAL_MACHINE) //runtime objet
      {
//#if defined EDEN_500 || defined EDEN_350 || defined EDEN_350_V || defined CONNEX_500 || defined CONNEX_350 || defined OBJET_350 || defined OBJET_500 || defined OBJET_1000
 	  Location = m_ParamsMgr->MaxPositionStep[AXIS_Z];
      }
      else //office machine
      {
//#else
 	  Location = CONFIG_ConvertMMToStep(AXIS_Z,m_ParamsMgr->TestPatternZPosition);
//#endif
      }
      if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Z,Location,60)) != Q_NO_ERROR)
         return Err;
   }

   if(m_TestPatternStopped)
     return Q2RT_STOP_DURING_TESTPATTERN;

// Move heads to test pattern position
   long YPosInSteps = CONFIG_ConvertYmmToStep(m_ParamsMgr->TestPatternYPosition);

   if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Y,YPosInSteps)) != Q_NO_ERROR)
      return Err;

// Get test pattern layer from Layer Generator
   // Create the TestPattern class.
   CQLog::Write(LOG_TAG_GENERAL,"Create CTestPatternLayer"); //objet test
   CTestPatternLayer *TestPattern = new CTestPatternLayer();

   TDPCPCLayerParams *LayerParms = TestPattern->GetDPCPCParams();

   CheckForFIFOEmpty();

// Stop fire mechanism in OHDB
   m_PrintControl->Stop();

// Stop fire mechanism in PCI
   FIFOPCI_NoGo();

// Initialize OHDB for passing printing
   m_PrintControl->ResetDriverCircuit();

   m_PrintControl->ApplyDefaultPrintParams();

   // Set parameters for job configuration (should be called once per job)
   m_PrintControl->SetDefaultConfigParams ();

   //Send layer to PCI card with all relevant initializations
   long StartPEG_step;
   int StartPEG,EndPEG;

   long XHeadSizeStep = CONFIG_ConvertXmmToStep(INT_FROM_RESOURCE(IDN_SPACE_FROM_H0_TO_H7));

   long PrintXStartPosition_step = CONFIG_ConvertXmmToStep(m_ParamsMgr->TestPatternXPosition.Value());

   StartPEG_step = (PrintXStartPosition_step - m_ParamsMgr->XEncoderHomeInStep +
                    XHeadSizeStep);
   StartPEG = static_cast<int>(static_cast<float>(StartPEG_step)/m_ParamsMgr->XStepsPerPixel) +
              LayerParms->StartOfPlot;

   EndPEG = StartPEG + LayerParms->EndOfPlot - LayerParms->StartOfPlot ;
   m_PrintControl->SetLayerParams(StartPEG,EndPEG,LayerParms->NoOfFires);

   // Reset state machine
   m_PrintControl->ResetDriverStateMachine();

   if (Q_NO_ERROR != (Err=FIFOPCI_WriteAsync(TestPattern->GetBuffer(),(m_ParamsMgr->TestPatternPrintBiderection ? 2 : 1) * DWORDS_IN_ALL_HEADS * LayerParms->NoOfFires)))
        return Err;
        
   //GO
   m_PrintControl->Go();

   // Motors moving
   long XStopLocationStep = StartPEG_step +
                            m_ParamsMgr->XEncoderHomeInStep +
                            LayerParms->NoOfFires + 2000;

   if(XStopLocationStep > m_ParamsMgr->MaxPositionStep[AXIS_X])
       XStopLocationStep = m_ParamsMgr->MaxPositionStep[AXIS_X]-50;

   if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_X,XStopLocationStep)) != Q_NO_ERROR)
      return Err;

   if(m_TestPatternStopped)
     return Q2RT_STOP_DURING_TESTPATTERN;

   if(!m_ParamsMgr->TestPatternPrintBiderection)
      {
      //Stop
      m_PrintControl->Stop();
      }

   long XStartLocationStep = (m_ParamsMgr->MinPositionStep[AXIS_X]+50);
   if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_X,XStartLocationStep)) != Q_NO_ERROR)
      return Err;

   CheckForFIFOEmpty();

   delete TestPattern;

   //Stop OHDB
   m_PrintControl->Stop();

   // Stop fire mechanism in PCI
   FIFOPCI_NoGo();

   if(m_TestPatternStopped)
     return Q2RT_STOP_DURING_TESTPATTERN;

   //If UV lamps is ON perform scanning and turn off
   if (m_ParamsMgr->TestPatternUvLampON && m_ParamsMgr->UVLampsEnabled)
      {
      FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Scanning");

      if ((Err = m_Motors->JogToAbsolutePositionSequence (AXIS_X,
                       XStopLocationStep,
                       m_ParamsMgr->NumberOfScanningPasses,muSteps)) != Q_NO_ERROR)
          return Err;

      if((Err = m_UVLamps->TurnOnOff(false)) != Q_NO_ERROR)
         return Err;
      }

   FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Home axis");

   if(m_TestPatternStopped)
     return Q2RT_STOP_DURING_TESTPATTERN;

   //Move to purge position
   if ((Err = m_Motors->GoWaitHomeSequence(AXIS_X)) != Q_NO_ERROR)
       {
       QMonitor.WarningMessage("Go to home X after test pattern - error ");
       return Err;
       }

   if ((Err = m_Motors->GoWaitHomeSequence(AXIS_Y)) != Q_NO_ERROR)
       {
       QMonitor.WarningMessage("Go to home Y after test pattern - error ");
       return Err;
       }

   if(m_TestPatternStopped)
     return Q2RT_STOP_DURING_TESTPATTERN;

   if(m_ParamsMgr->OfficeProfessionalType==OFFICE_MACHINE) //runtime objet
   {
//#if defined EDEN_260 || defined EDEN_250 || defined EDEN_260_V || defined CONNEX_260 || defined OBJET_260
        // Move Z back up to tray out position
        if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Z, m_ParamsMgr->ZAxisEndPrintPosition,
                                                       MORE_TIME_TO_FINISH_MOVEMENT))!= Q_NO_ERROR)
                return Err;
//#endif
   }

   //Turn heating off
  if(!m_ParamsMgr->KeepHeadsHeated.Value())
     if ((Err = m_HeadHeaters->SetDefaultOnOff(HEAD_HEATER_OFF)) != Q_NO_ERROR)
        {
        QMonitor.WarningMessage("Set heater off error");
        return Err;
        }

   //Turn Filling monitoring OFF  - OCB
   if ((Err = m_HeadFilling->HeadFillingOnOff(false)) != Q_NO_ERROR)
      return Err;

   //Open Door - OCB
   if((Err=m_Door->Disable())!= Q_NO_ERROR)
      return Err;

   // Update front-end
   FrontEndInterface->EnableDisableControl(FE_STOP,false);

   if ((Err = m_Motors->SetMotorEnableDisable(AXIS_T,MOTOR_DISABLE)) != Q_NO_ERROR)
      return Err;

   CQLog::Write(LOG_TAG_GENERAL,"Test Pattern Done OK");
   return Q_NO_ERROR;
}

/*
TQErrCode CMachineSequencer::TestUVThermPaper(void)
{
   TQErrCode Err;
   m_TestOk=true;
   long XPosition,YPosition,ZPosition;
   int Position;
   bool Continue=true;
   int XTime;

   //Turns the interlock on
   if((Err=m_Door->Enable())!= Q_NO_ERROR)
      return Err;

   if ((Err=m_Motors->InitMotorParameters()) != Q_NO_ERROR)
      throw EMachineSequencer("Error during test Motor Init parameters");

   if ((Err = m_Motors->SetMotorEnableDisable(AXIS_ALL,MOTOR_ENABLE)) != Q_NO_ERROR)
      return Err;
   if ((Err = m_Motors->GoWaitHomeSequence(AXIS_X)) != Q_NO_ERROR)
      return Err;
   if (QMonitor.AskYesNo("Do you want to Move Z Axis ?"))
      {
      QMonitor.GetNumber("Enter Z Position ?",Position);
      ZPosition = (long)Position;
      if ((Err = m_Motors->GoWaitHomeSequence(AXIS_Z)) != Q_NO_ERROR)
         return Err;
      if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Z,ZPosition)) != Q_NO_ERROR)
         return Err;
      }

   while(Continue && m_TestOk)
      {
      if (QMonitor.AskYesNo("Do you want to Move Y Axis ?"))
         {
         QMonitor.GetNumber("Enter Y Position ?",Position);
         YPosition = (long)Position;
         if ((Err = m_Motors->GoWaitHomeSequence(AXIS_Y)) != Q_NO_ERROR)
            return Err;
         if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Y,YPosition)) != Q_NO_ERROR)
            return Err;
         }

      QMonitor.GetNumber("Enter Time to wait after X Stop Moving in miliseconds ?",XTime);
      QMonitor.GetNumber("Enter X Position ?",Position);
      XPosition = (long)Position;

      if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_X,XPosition)) != Q_NO_ERROR)
         return Err;
      QSleep(XTime);
      if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_X,50)) != Q_NO_ERROR)
         return Err;

      if (!QMonitor.AskYesNo("Do you want to Continue?"))
         Continue = false;
      }

   if ((Err = m_Motors->SetMotorEnableDisable(AXIS_ALL,MOTOR_DISABLE)) != Q_NO_ERROR)
      return Err;

   if (m_TestOk)
      QMonitor.Print("Test Finish !");
   else
      QMonitor.Printf("Test stopped !");

   return Q_NO_ERROR;
}
*/



