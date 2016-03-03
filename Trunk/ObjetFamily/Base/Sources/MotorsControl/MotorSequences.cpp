/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Motor class                                              *
 * Module Description: This class implement services related to the *
 *                     Motors.                                      *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger                                          *
 * Start date: 22/August/2001                                       *
 * Last upate: 7/May/02                                             *
 ********************************************************************/

#include "Motor.h"
#include "QMonitor.h"
#include "QUtils.h"
#include "QThreadUtils.h"
#include "AppParams.h"
#include "CONFIGURATION.H"
#include "Q2RTErrors.h"
#include "GlobalDefs.h"
#include "QApplication.h"
#include "Q2RTApplication.h"
#include "time.h"
#include "AppLogFile.h"
#include "FrontEnd.h"
#include "Actuator.h"
#include "MotorMcb.h"
#include "TrayHeater.h"
#include "MachineSequencer.h"


const int MCB_KILL_TIME_Sec = 30;


const int WAIT_AND_SEND_COMMAND_AGAIN_SEC = 20; //In seconds


//Commands mini-sequencer
//---------------------------------------
//---------------------------------------
TQErrCode CMotorsBase::GoWaitToYRelativePositionWithBacklashSequence(long Relative)
{
    return GoWaitToYPositionWithBacklashSequence(GetAxisLocation(AXIS_Y)+Relative);
}

//Command to Go To Position . this command verify for Y axis if backlash is needed.
// --------------------------------------------------------------------------------
TQErrCode CMotorsBase::GoWaitToYPositionWithBacklashSequence(long Destination)
{
   TQErrCode Err;
   long BacklashPosition;

   if(GetAbsoluteBacklashLocation(Destination,BacklashPosition))
      if((Err = GoWaitAbsolutePositionSequence(AXIS_Y,BacklashPosition)) != Q_NO_ERROR)
         return Err;

   return GoWaitAbsolutePositionSequence(AXIS_Y,Destination);
}

//Command Go To Position that moving if backlash is needed
// --------------------------------------------------------------------------------
TQErrCode CMotorsBase::PerformBacklashIfNeeded(long Destination)
{
   TQErrCode Err;
   long BacklashPosition;

   if(GetAbsoluteBacklashLocation(Destination,BacklashPosition))
      if((Err = GoWaitAbsolutePositionSequence(AXIS_Y,BacklashPosition)) != Q_NO_ERROR)
         return Err;

   return Q_NO_ERROR;
}

//Get the location for the backlash if return false there is no need to do backlash
// --------------------------------------------------------------------------------
bool CMotorsBase::GetAbsoluteBacklashLocation(long Destination,long & BackLashPosition)
{

   long Current = GetAxisLocation(AXIS_Y);

   long Backlash=m_YBacklashMoveInSteps;
   if(!Backlash)
      return false;

   if(Destination == Current) // no move is needed
      return false;

   if(m_ParamsMgr->OfficeProfessionalType==PROFESSIONAL_MACHINE) //runtime objet
   {
//OBJET_MACHINE config  
	 if((Destination - Current) > Backlash)
		//there is no need to perform backlash
		return false;
    }
    else //office machine
    {
        // actually what's relevant is that we have an encoder. Currently, E500 doesn't have it, while 250 and 260 have it.
//OBJET_260
        if ( labs(Destination - Current) > labs(Backlash) )
          //there is no need to perform backlash
		  return false;
    }

   //Backlash is needed:  go to the backlash position
   if(Destination > Current)
      {
      //perform the backlash movent by current position
      BackLashPosition = CheckAxisPositionValue(AXIS_Y,Current-Backlash) ?
                       (Current-Backlash) : m_Axis[AXIS_Y]->GetMinPosition();
      return true;
      }
   //else
   // Current > Destination
   //perform the bachlash movent by Destination position
   BackLashPosition = CheckAxisPositionValue(AXIS_Y,Destination-Backlash) ?
                       (Destination-Backlash) : m_Axis[AXIS_Y]->GetMinPosition();
   return true;
}


//Command to Go To Position this command add the value received to
// the current position and call to GoToAbsolutePositionSequence
// with this new value
// ----------------------------------------------------------
TQErrCode CMotorsBase::GoToRelativePositionSequence(TMotorAxis Axis, long Destination)
{
   return(GoToAbsolutePositionSequence(Axis,GetAxisLocation(Axis)+Destination));
}

TQErrCode CMotorsBase::GoToAbsolutePositionSequence(TMotorAxis Axis,TMotorPosition Pos,TMotorUnits Units)
{
  long PosInSteps = CONFIG_ConvertUnitsToStep(Axis,Pos,Units);
  return GoToAbsolutePositionSequence(Axis,PosInSteps);
}

TQErrCode CMotorsBase::GoToAbsolutePositionSequence(TMotorAxis Axis,long Destination)
{
   TQErrCode Err;
   int j=0;

   //We want to be sure that we arrived to the destination position
   Err = GoToAbsolutePosition(Axis,Destination,muSteps);

   switch(Err)
      {
      case Q2RT_MCB_ACK_NOT_RELEVANT_COMMAND_DURING_GO_TO:
         FrontEndInterface->NotificationMessage("MCB error:Command not relevant during GoTo");

         //In this error wait and try again to perform the motor move
         while (Err==Q2RT_MCB_ACK_NOT_RELEVANT_COMMAND_DURING_GO_TO && j++<5)
            {
            //bool MotorInMovement = GetAxisIfMotorIsMoving(Axis);
            QSleepSec(WAIT_AND_SEND_COMMAND_AGAIN_SEC);
            CQLog::Write(LOG_TAG_MOTORS,"Motor %s is marked moving wait and send again to destination(%d)",
                                PrintAxis(Axis).c_str(),
                               (int)Destination);

            if(Destination != GetAxisLocation(Axis))
               Err = GoToAbsolutePosition(Axis,Destination,muSteps);
            else
               Err=Q_NO_ERROR;
            }
            break;

      case ACK_MOTOR_IS_IN_HW_LIMIT:
      case ACK_MOTOR_OVERCURRENT_STATE:
      case ACK_VOLTAGE_DROP:
        if(Axis==AXIS_Z)
           break;

        //Home and send again.
        if ((Err=GoWaitHome(Axis)) != Q_NO_ERROR)
           return Err;
        Err = GoToAbsolutePosition(Axis,Destination,muSteps);
        break;

      case ACK_SUM_ERROR:
        //Send command again
        Err = GoToAbsolutePosition(Axis,Destination,muSteps);
        break;

      case ACK_STATUS_SUCCESS: // = Q_NO_ERROR
      default:
         break;
      }

   return Err;
}


TQErrCode CMotorsBase::CheckForEndOfMovement(TMotorAxis Axis,int ExtraTimeWaitingSec)
{
   TQErrCode Err;
   long Destination = GetAxisSolicitedLocation(Axis);

   if ((Err =  WaitForEndOfMovement(Axis,ExtraTimeWaitingSec)) != Q_NO_ERROR)
      {
      if (Axis==AXIS_Z)
         return Err;

      if(m_MovementErrorCounter[Axis]>5)
         return Err;

      switch (Err)
         {
         case Q2RT_MCB_EVENT_ERROR_Y_ENCODER_DETECT_ERROR:
            CQLog::Write(LOG_TAG_MOTORS,"MCB:%s : Y encoder error",PrintAxis(Axis).c_str());
            FrontEndInterface->NotificationMessage("Y ENCODER ERROR");
            if ((Err=GoWaitHome(Axis)) != Q_NO_ERROR)
               return Err;
            if((Err=GoWaitToAbsolutePosition(Axis,Destination,muSteps, false))!=Q_NO_ERROR)
               return Err;
            break;

         case Q2RT_MCB_EVENT_ERROR_OVER_CURRENT :
            CQLog::Write(LOG_TAG_MOTORS,"MCB:%s : MCB Error over current",PrintAxis(Axis).c_str());
            FrontEndInterface->NotificationMessage("MCB Error over current");
            if ((Err=GoWaitHome(Axis)) != Q_NO_ERROR)
               return Err;
            if((Err=GoWaitToAbsolutePosition(Axis,Destination,muSteps, false))!=Q_NO_ERROR)
               return Err;
            break;

         case Q2RT_MCB_EVENT_ERROR_HIGH_TEMPERATURE:
            CQLog::Write(LOG_TAG_MOTORS,"MCB:%s : MCB Error high temperature",PrintAxis(Axis).c_str());
            FrontEndInterface->NotificationMessage("MCB Error high temperature");
            if ((Err=GoWaitHome(Axis)) != Q_NO_ERROR)
               return Err;
            if((Err=GoWaitToAbsolutePosition(Axis,Destination,muSteps, false))!=Q_NO_ERROR)
               return Err;
            break;

         case Q2RT_MCB_EVENT_ERROR_LOW_TEMPERATURE:
            CQLog::Write(LOG_TAG_MOTORS,"MCB:%s : MCB Error low temperature",PrintAxis(Axis).c_str());
            FrontEndInterface->NotificationMessage("MCB Error low temperature");
            if ((Err=GoWaitHome(Axis)) != Q_NO_ERROR)
               return Err;
            if((Err=GoWaitToAbsolutePosition(Axis,Destination,muSteps, false))!=Q_NO_ERROR)
               return Err;
            break;

         case Q2RT_MCB_EVENT_ERROR_VOLTAGE_DROP:
            CQLog::Write(LOG_TAG_MOTORS,"MCB:%s : MCB Error voltage drop",PrintAxis(Axis).c_str());
            FrontEndInterface->NotificationMessage("MCB Error voltage drop");
            if ((Err=GoWaitHome(Axis)) != Q_NO_ERROR)
               return Err;
            if((Err=GoWaitToAbsolutePosition(Axis,Destination,muSteps, false))!=Q_NO_ERROR)
               return Err;
            break;

         case Q2RT_MCB_EVENT_ERROR_RIGHT_LIMIT:
            CQLog::Write(LOG_TAG_MOTORS,"MCB:%s : MCB Error right limit",PrintAxis(Axis).c_str());
            FrontEndInterface->NotificationMessage("MCB Error right limit");
            if ((Err=GoWaitHome(Axis)) != Q_NO_ERROR)
               return Err;
            if((Err=GoWaitToAbsolutePosition(Axis,Destination,muSteps, false))!=Q_NO_ERROR)
               return Err;
            break;

         case Q2RT_MCB_EVENT_ERROR_LEFT_LIMIT:
            CQLog::Write(LOG_TAG_MOTORS,"MCB:%s : MCB Error left limit",PrintAxis(Axis).c_str());
            FrontEndInterface->NotificationMessage("MCB Error left limit");
            if ((Err=GoWaitHome(Axis)) != Q_NO_ERROR)
               return Err;
            if((Err=GoWaitToAbsolutePosition(Axis,Destination,muSteps, false))!=Q_NO_ERROR)
               return Err;
            break;

         case Q2RT_MCB_EVENT_ERROR_TIME_OUT:
            CQLog::Write(LOG_TAG_MOTORS,"MCB:%s : MCB Error time out",PrintAxis(Axis).c_str());
            FrontEndInterface->NotificationMessage("MCB Error time out");
            if ((Err=GoWaitHome(Axis)) != Q_NO_ERROR)
               return Err;
            if((Err=GoWaitToAbsolutePosition(Axis,Destination,muSteps, false))!=Q_NO_ERROR)
               return Err;
            break;

         case Q2RT_MCB_EVENT_ERROR_CANNOT_ESCAPE_FROM_HOME_POSITION:
            CQLog::Write(LOG_TAG_MOTORS,"MCB:%s : Error can not escape from home position",PrintAxis(Axis).c_str());
            FrontEndInterface->NotificationMessage("Error can not escape from home position");
            if ((Err=GoWaitHome(Axis)) != Q_NO_ERROR)
               return Err;
            if((Err=GoWaitToAbsolutePosition(Axis,Destination,muSteps, false))!=Q_NO_ERROR)
               return Err;
            break;

         case Q2RT_MCB_ACK_TIMEOUT:
            CQLog::Write(LOG_TAG_MOTORS,"MCB:%s:Timeout while wait for end of movement",PrintAxis(Axis).c_str());
            FrontEndInterface->NotificationMessage("Timeout while wait for end of movement");
            if((Err=GoWaitToAbsolutePosition(Axis,Destination,muSteps, false))!=Q_NO_ERROR)
               return Err;
            break;

         case Q2RT_MCB_WRONG_POSITION_RX_FROM_MCB:
            CQLog::Write(LOG_TAG_MOTORS,"MCB:%s:wrong position RX from MCB-%d",
                         PrintAxis(Axis).c_str(),GetAxisLocation(Axis));
            FrontEndInterface->NotificationMessage("MCB:%s:wrong position RX from MCB");
            if((Err=GoWaitToAbsolutePosition(Axis,Destination,muSteps, false))!=Q_NO_ERROR)
               return Err;
            break;

         case Q2RT_SEQUENCE_CANCELED:
         case Q2RT_FATAL_ERROR:
         case Q2RT_MODEL_ON_TRAY_SENSOR_1_DETECTED:
         case Q2RT_MODEL_ON_TRAY_SENSOR_2_DETECTED:
         case Q2RT_MODEL_ON_TRAY_BOTH_SENSOR_DETECTED:
         case Q2RT_MODEL_ON_TRAY_RESPONSE_MSG_FAIL:
         case Q2RT_BUMPER_IMPACT: 
            return Err;

          default:
             FrontEndInterface->NotificationMessage("MCB error event");
             CQLog::Write(LOG_TAG_MOTORS,"MCB:%s : MCB Error current = %d",
                              PrintAxis(Axis).c_str(),Err);
             return Err;
        }
      }

   return Err;
}

TQErrCode CMotorsBase::CheckForEndOfMovementSequence(TMotorAxis Axis,int ExtraTimeWaitingSec)
{
   TQErrCode Err;
   long Destination = GetAxisSolicitedLocation(Axis);

   if ((Err = CheckForEndOfMovement(Axis,ExtraTimeWaitingSec)) != Q_NO_ERROR)
      return Err;
   if(Destination != GetAxisLocation(Axis) && m_LastSolicitedPositions[Axis] == GetAxisLocation(Axis))
   {
     // If we received more than one 'End of movement' messages for the same position (could occur due to a retry)
     // wait for the 'End of movement' message again
     if ((Err = CheckForEndOfMovement(Axis,ExtraTimeWaitingSec)) != Q_NO_ERROR)
        return Err;
   }
   if (Axis == AXIS_Y)
   {
	  CQLog::Write(LOG_TAG_MOTORS,"Motor %s : position is (%d)",
								  PrintAxis(Axis).c_str(),(int)GetAxisLocation(Axis));
   }
   //We want to be sure that we arrived to the destination position
   if(Destination != GetAxisLocation(Axis))
      {
      //check with MCB what is its current position May be an error occured in EndOfMov
      if ((Err=GetAxisStatus(Axis)) != Q_NO_ERROR)
         return Err;

      if (Destination == GetAxisLocation(Axis))
         {
         FrontEndInterface->NotificationMessage("There was an mismatch in position - continue printing");
		 CQLog::Write(LOG_TAG_MOTORS,"Motor %s : destination position is ok now (%d)",
                                  PrintAxis(Axis).c_str(),(int)Destination);
         }
      else
         {
         int MoveError = abs(static_cast<int>(Destination - GetAxisLocation(Axis)));

         if (Axis == AXIS_Y)
            {
            if(abs(MoveError) > 12)
               {
               CQLog::Write(LOG_TAG_MOTORS,"Motor %s : Solicited position(%d) is different then arrived (%d)",
                                  PrintAxis(Axis).c_str(),
								  (int)Destination,(int)GetAxisLocation(Axis));
               FrontEndInterface->NotificationMessage("Motor - Axis Y - did not arrived to solicited position - continue printing ");

               if((Err=GoToAbsolutePositionSequence(Axis,Destination))!=Q_NO_ERROR)
                  return Err;
               if((Err=CheckForEndOfMovement(Axis,ExtraTimeWaitingSec)) != Q_NO_ERROR)
                  return Err;

               int MoveError = abs(static_cast<int>(Destination - GetAxisLocation(Axis)));

               if(abs(MoveError) > 12)
                  {
                  CQLog::Write(LOG_TAG_MOTORS,"Motor %s : Solicited position(%d) is different then arrived AGAIN (%d)",
                                  PrintAxis(Axis).c_str(),
                                  (int)Destination,(int)GetAxisLocation(Axis));
                  FrontEndInterface->NotificationMessage("Motor did not arrived to solicited position AGAIN - stop printing ");
                  return (Q2RT_MCB_NOT_ARRIVED_TO_SOLICITED_POSITION_X-Axis);
                  }
               }
            else
               {
               CQLog::Write(LOG_TAG_MOTORS,"Motor %s : Solicited position(%d) is different then arrived (%d) - continue printing",
                                  PrintAxis(Axis).c_str(),
                                  (int)Destination,(int)GetAxisLocation(Axis));
               FrontEndInterface->NotificationMessage("Motor (Y) did not arrived to solicited position - continue printing ");
               }
            }
         else
            //Axis is AXIS_X or AXIS_T and AXIS Z
            {
            if(abs(MoveError) > 1)
               {
               CQLog::Write(LOG_TAG_MOTORS,"Motor %s : Solicited position(%d) is different then arrived (%d) - Try again",
                                  PrintAxis(Axis).c_str(),
                                  (int)Destination,(int)GetAxisLocation(Axis));
               FrontEndInterface->NotificationMessage("Motor did not arrived to solicited position");

               if((Err=GoToAbsolutePositionSequence(Axis,Destination))!=Q_NO_ERROR)
                  return Err;
               if((Err=CheckForEndOfMovement(Axis,ExtraTimeWaitingSec)) != Q_NO_ERROR)
                  return Err;
               int MoveError = abs(static_cast<int>(Destination - GetAxisLocation(Axis)));

               if(abs(MoveError) > 1)
                  {
                  CQLog::Write(LOG_TAG_MOTORS,"Motor %s : Solicited position(%d) is different then arrived AGAIN (%d)",
                                  PrintAxis(Axis).c_str(),
                                  (int)Destination,(int)GetAxisLocation(Axis));
                  FrontEndInterface->NotificationMessage("Motor did not arrived to solicited position AGAIN - stop printing ");
                  return (Q2RT_MCB_NOT_ARRIVED_TO_SOLICITED_POSITION_X-Axis);
                  }
               }
            else
               {
               CQLog::Write(LOG_TAG_MOTORS,"Motor %s : Solicited position(%d) is different then arrived (%d) - 1 step error - continue printing",
                                  PrintAxis(Axis).c_str(),
                                  (int)Destination,(int)GetAxisLocation(Axis));
               FrontEndInterface->NotificationMessage("Motor - did not arrived to solicited position - continue printing ");
               }
            }
         }
      }

   return Err;
}

TQErrCode CMotorsBase::GoWaitRelativePositionSequence(TMotorAxis Axis,
                                                      long Destination,
                                                      int ExtraTimeWaitingSec)
{
   return(GoWaitAbsolutePositionSequence(Axis,
                                         GetAxisLocation(Axis)+Destination,
                                         ExtraTimeWaitingSec));
}

TQErrCode CMotorsBase::GoWaitAbsolutePositionSequence (TMotorAxis Axis,
                                                       TMotorPosition Pos,
                                                       int ExtraTimeWaitingSec,
                                                       TMotorUnits Units)
{
  long PosInSteps = CONFIG_ConvertUnitsToStep(Axis,Pos,Units);
  return GoWaitAbsolutePositionSequence(Axis,PosInSteps,ExtraTimeWaitingSec);
}

TQErrCode CMotorsBase::GoWaitAbsolutePositionSequence (TMotorAxis Axis,
                                                       long Destination,
                                                       int ExtraTimeWaitingSec)
{
   TQErrCode Err;
   long Current = GetAxisLocation(Axis);
   
   if(Destination == Current) // no move is needed
         return Q_NO_ERROR;

   if ((Err = GoToAbsolutePositionSequence(Axis,Destination)) != Q_NO_ERROR)
      return Err;
   if ((Err = CheckForEndOfMovementSequence(Axis,ExtraTimeWaitingSec)) != Q_NO_ERROR)
         return Err;

   return Q_NO_ERROR;
}

//Command to Go To Position this command add the value received to
// the current position and call to GoWaitToAbsolutePosition
// with this new value
// ----------------------------------------------------------
TQErrCode CMotorsBase::GoWaitToRelativePosition(TMotorAxis Axis,TMotorPosition Destination,TMotorUnits Units, bool IgnoreBumperImpact)
{
   TMotorPosition DestPos;
   long CurrPosSteps;

   CurrPosSteps = GetAxisLocation(Axis);
   DestPos = CONFIG_ConvertStepToUnits(Axis, CurrPosSteps, Units) + Destination;

   return GoWaitToAbsolutePosition(Axis, DestPos, Units, IgnoreBumperImpact);
}

TQErrCode CMotorsBase::GoWaitToAbsolutePosition (TMotorAxis Axis,TMotorPosition Destination,TMotorUnits Units, bool IgnoreBumperImpact)
{
   TQErrCode Err;
   long Current = GetAxisLocation(Axis);

   long DestinationInSteps = CONFIG_ConvertUnitsToStep(Axis,Destination,Units);

   if(DestinationInSteps == Current) // no move is needed
      return Q_NO_ERROR;

   if ((Err = GoToAbsolutePosition(Axis,DestinationInSteps,muSteps, IgnoreBumperImpact)) != Q_NO_ERROR)
      return Err;
   if ((Err = WaitForEndOfMovement(Axis,Axis == AXIS_Z ? MOTOR_FINISH_MOVEMENT_TIMEOUT_IN_SEC :
                                                         MORE_TIME_TO_FINISH_MOVEMENT)) != Q_NO_ERROR)
      return Err;
      
   if(GetAxisSolicitedLocation(Axis) != GetAxisLocation(Axis))
      return (Q2RT_MCB_NOT_ARRIVED_TO_SOLICITED_POSITION_X-Axis);

   return Q_NO_ERROR;
}
                       
//---------------------------------------------------------------------------------

TQErrCode CMotorsBase::JogToRelativePositionSequence(TMotorAxis Axis,TMotorPosition Destination,
                                                     int NumberOfJogs,TMotorUnits Units)
{
   return JogToAbsolutePositionSequence(Axis,
                                        CONFIG_ConvertUnitsToStep(Axis,GetAxisLocation(Axis),Units)+Destination,
                                        NumberOfJogs,Units);
}

TQErrCode CMotorsBase::JogToAbsolutePositionSequence (TMotorAxis Axis,TMotorPosition Destination,
                                                      int NumberOfJogs,TMotorUnits Units)
{
   TQErrCode Err;
   long StartPosition = GetAxisLocation(Axis);

   long DestinationInSteps = CONFIG_ConvertUnitsToStep(Axis,Destination,Units);

   if(DestinationInSteps == StartPosition) // no move is needed
         return Q_NO_ERROR;

   if (Axis == AXIS_Z)
      {
      Err = JogToAbsolutePosition(Axis,DestinationInSteps,NumberOfJogs,muSteps);

      if(Err == Q_NO_ERROR)
         Err =  WaitForEndOfMovement(Axis,
                                     (Axis == AXIS_Z ? 60 : 1)*NumberOfJogs);

      if(Err == Q_NO_ERROR)
         if(StartPosition != GetAxisLocation(Axis))
            {
            CQLog::Write(LOG_TAG_MOTORS,"Motor %s : end position(%d) is different then start position(%d)",
                                  PrintAxis(Axis).c_str(),
                                  (int)DestinationInSteps,(int)GetAxisLocation(Axis));
            return (Q2RT_MCB_NOT_ARRIVED_TO_SOLICITED_POSITION_X-Axis);
            }

      return Err;
      }

   Err = JogToAbsolutePosition(Axis,DestinationInSteps,NumberOfJogs,muSteps);

   if(Err == Q2RT_MCB_ACK_NOT_RELEVANT_COMMAND_DURING_JOG_TO)
      {
      FrontEndInterface->NotificationMessage("MCB error:Command not relevant during JogTo");
      //if this error is received I think it is performing the jog discard the error
      Err = Q_NO_ERROR;
      }

   if(Err == Q_NO_ERROR)
      {
      if ((Err =  WaitForEndOfMovement(Axis,((Axis == AXIS_Z ? 90 : 10)*NumberOfJogs))) != Q_NO_ERROR)
         {
         if(m_MovementErrorCounter[Axis]>5)
            return Err;

         switch (Err)
            {
            case Q2RT_MCB_EVENT_ERROR_Y_ENCODER_DETECT_ERROR:
               CQLog::Write(LOG_TAG_MOTORS,"MCB:%s : Y encoder error",PrintAxis(Axis).c_str());
               FrontEndInterface->NotificationMessage("Y ENCODER ERROR");
               if ((Err=GoWaitHome(Axis)) != Q_NO_ERROR)
                  return Err;
               if((Err=GoWaitToAbsolutePosition(Axis,StartPosition,muSteps, false))!=Q_NO_ERROR)
                  return Err;
               break;


            case Q2RT_MCB_EVENT_ERROR_OVER_CURRENT :
               CQLog::Write(LOG_TAG_MOTORS,"MCB:%s : Error over current",PrintAxis(Axis).c_str());
               FrontEndInterface->NotificationMessage("Error over current");
               if ((Err=GoWaitHome(Axis)) != Q_NO_ERROR)
                  return Err;
               if((Err=GoWaitToAbsolutePosition(Axis,StartPosition,muSteps, false))!=Q_NO_ERROR)
                  return Err;
               break;

            case Q2RT_MCB_EVENT_ERROR_HIGH_TEMPERATURE:
               CQLog::Write(LOG_TAG_MOTORS,"MCB:%s : Error high temperature",PrintAxis(Axis).c_str());
               FrontEndInterface->NotificationMessage("Error high temperature");
               if ((Err=GoWaitHome(Axis)) != Q_NO_ERROR)
                  return Err;
               if((Err=GoWaitToAbsolutePosition(Axis,StartPosition,muSteps, false))!=Q_NO_ERROR)
                  return Err;
               break;

            case Q2RT_MCB_EVENT_ERROR_LOW_TEMPERATURE:
               CQLog::Write(LOG_TAG_MOTORS,"MCB:%s : Error low temperature",PrintAxis(Axis).c_str());
               FrontEndInterface->NotificationMessage("Error low temperature");
               if ((Err=GoWaitHome(Axis)) != Q_NO_ERROR)
                  return Err;
               if((Err=GoWaitToAbsolutePosition(Axis,StartPosition,muSteps, false))!=Q_NO_ERROR)
                  return Err;
               break;

            case Q2RT_MCB_EVENT_ERROR_VOLTAGE_DROP:
               CQLog::Write(LOG_TAG_MOTORS,"MCB:%s : Error voltage drop",PrintAxis(Axis).c_str());
               FrontEndInterface->NotificationMessage("Error voltage drop");
               if ((Err=GoWaitHome(Axis)) != Q_NO_ERROR)
                  return Err;
               if((Err=GoWaitToAbsolutePosition(Axis,StartPosition,muSteps, false))!=Q_NO_ERROR)
                  return Err;
               break;

            case Q2RT_MCB_EVENT_ERROR_RIGHT_LIMIT:
               CQLog::Write(LOG_TAG_MOTORS,"MCB:%s : Error right limit",PrintAxis(Axis).c_str());
               FrontEndInterface->NotificationMessage("Error right limit");
               if ((Err=GoWaitHome(Axis)) != Q_NO_ERROR)
                  return Err;
               if((Err=GoWaitToAbsolutePosition(Axis,StartPosition,muSteps, false))!=Q_NO_ERROR)
                  return Err;
               break;

            case Q2RT_MCB_EVENT_ERROR_LEFT_LIMIT:
               CQLog::Write(LOG_TAG_MOTORS,"MCB:%s : Error left limit",PrintAxis(Axis).c_str());
               FrontEndInterface->NotificationMessage("Error left limit");
               if ((Err=GoWaitHome(Axis)) != Q_NO_ERROR)
                  return Err;
               if((Err=GoWaitToAbsolutePosition(Axis,StartPosition,muSteps, false))!=Q_NO_ERROR)
                  return Err;
               break;

            case Q2RT_MCB_EVENT_ERROR_TIME_OUT:
               CQLog::Write(LOG_TAG_MOTORS,"MCB:%s : Error time out",PrintAxis(Axis).c_str());
               FrontEndInterface->NotificationMessage("Error time out");
               if ((Err=GoWaitHome(Axis)) != Q_NO_ERROR)
                  return Err;
               if((Err=GoWaitToAbsolutePosition(Axis,StartPosition,muSteps, false))!=Q_NO_ERROR)
                  return Err;
               break;

            case Q2RT_MCB_EVENT_ERROR_CANNOT_ESCAPE_FROM_HOME_POSITION:
               CQLog::Write(LOG_TAG_MOTORS,"MCB:%s : Error can not escape from home position",PrintAxis(Axis).c_str());
               FrontEndInterface->NotificationMessage("Error can not escape from home position");
               if ((Err=GoWaitHome(Axis)) != Q_NO_ERROR)
                 return Err;
               if((Err=GoWaitToAbsolutePosition(Axis,StartPosition,muSteps, false))!=Q_NO_ERROR)
                  return Err;
               break;

            case Q2RT_MCB_ACK_TIMEOUT:
               CQLog::Write(LOG_TAG_MOTORS,"MCB:%s : Timeout while wait for Jog end of movement",PrintAxis(Axis).c_str());
               FrontEndInterface->NotificationMessage("Timeout while wait for Jog end of movement");
               if((Err=GoWaitToAbsolutePosition(Axis,StartPosition,muSteps, false))!=Q_NO_ERROR)
                  return Err;
               break;

            case Q2RT_MCB_WRONG_POSITION_RX_FROM_MCB:
               CQLog::Write(LOG_TAG_MOTORS,"MCB:%s:wrong position RX from MCB-%d",
                                  PrintAxis(Axis).c_str(),GetAxisLocation(Axis));
               FrontEndInterface->NotificationMessage("MCB:%s:wrong position RX from MCB");
               if((Err=GoWaitToAbsolutePosition(Axis,StartPosition,muSteps, false))!=Q_NO_ERROR)
                 return Err;
               break;

            case Q2RT_SEQUENCE_CANCELED:
            case Q2RT_FATAL_ERROR:
            case Q2RT_MODEL_ON_TRAY_SENSOR_1_DETECTED:
            case Q2RT_MODEL_ON_TRAY_SENSOR_2_DETECTED:
            case Q2RT_MODEL_ON_TRAY_BOTH_SENSOR_DETECTED:
            case Q2RT_MODEL_ON_TRAY_RESPONSE_MSG_FAIL:
               return Err;

            default:
               return Err;
            }
         }
      }
   else
      return Err;

   if(StartPosition != GetAxisLocation(Axis))
      {
      CQLog::Write(LOG_TAG_MOTORS,"Motor %s : Solicited position (%d) is different then arrived (%d)",
                               PrintAxis(Axis).c_str(),
                               (int)StartPosition,(int)GetAxisLocation(Axis));

      FrontEndInterface->NotificationMessage("Motor did not arrived to requested position");

      //check with MCB what is its current position May be an error occured in EndOfMov
      if ((Err=GetAxisStatus(Axis)) != Q_NO_ERROR)
         return Err;

      if (StartPosition == GetAxisLocation(Axis))
         {
         CQLog::Write(LOG_TAG_MOTORS,"Motor %s : destination position is ok now (%d)",
                               PrintAxis(Axis).c_str(),(int)DestinationInSteps);
         }

      Err = (Q2RT_MCB_NOT_ARRIVED_TO_SOLICITED_POSITION_X-Axis);
      }  
   else
      {
      return Q_NO_ERROR;
      }

   return Err;
}

TQErrCode CMotorsBase::JogWaitToPosition(TMotorAxis Axis,TMotorPosition Destination,int NumberOfJogs,TMotorUnits Units)
{
   long StartPosition = GetAxisLocation(Axis);
   TQErrCode Err;

   long DestinationInSteps = CONFIG_ConvertUnitsToStep(Axis,Destination,Units);

   if(DestinationInSteps == StartPosition) // no move is needed
      return Q_NO_ERROR;

   if((Err = JogToAbsolutePosition(Axis,DestinationInSteps,NumberOfJogs,muSteps)) != Q_NO_ERROR)
      return Err;

   if((Err = WaitForEndOfMovement(Axis,(Axis == AXIS_Z ? 90 : 10)*NumberOfJogs)) != Q_NO_ERROR)
      return Err;

   if(GetAxisSolicitedLocation(Axis) != GetAxisLocation(Axis))
      return (Q2RT_MCB_NOT_ARRIVED_TO_SOLICITED_POSITION_X-Axis);

   return Err;
}

TQErrCode CMotorsBase::GoHomeSequence(TMotorAxis Axis, bool IgnoreBumperImpact)
{
   TQErrCode Err;

   long Destination = GetAxisSolicitedLocation(Axis);

   // Axis Y only: see if a backlash movement is needed due to a too-small movement (and perform it)
   if (Axis == AXIS_Y)
     if ((Err = PerformBacklashIfNeeded(GetAxisCurrentHomePosition(AXIS_Y))) != Q_NO_ERROR)
       return Err;

   Err = GoHome(Axis, IgnoreBumperImpact);

   if(Err == Q2RT_MCB_ACK_ILLEGAL_COMMAND)
      {
      FrontEndInterface->NotificationMessage("MCB error:illegal command during Go Home");
	   int j = 0;
      //In this error wait and try again the motor movement
      while (Err==Q2RT_MCB_ACK_ILLEGAL_COMMAND && j++<5)
         {
         QSleepSec(WAIT_AND_SEND_COMMAND_AGAIN_SEC);
         CQLog::Write(LOG_TAG_MOTORS,"Motor %s is marked moving wait and send again to destination(%d)",
                                PrintAxis(Axis).c_str(),
                               (int)Destination);

         Err = GoHome(Axis);
         }
      }

   return Err;
}


TQErrCode CMotorsBase::GoWaitHomeSequence(TMotorAxis Axis)
{
   TQErrCode Err;

   if((Err = GoHomeSequence(Axis)) != Q_NO_ERROR)
      return Err;

   Err = CheckForEndOfMovementSequence(Axis,Axis == AXIS_Z ? 60 : 10);
   QSleep(m_WAIT_BETWEEN_HOME_AND_GOTO_MOVEMENT_MS);
   return Err;
}


TQErrCode CMotorsBase::GoWaitHome(TMotorAxis Axis)
{
   TQErrCode Err = GoHome(Axis);
   if(Err == Q_NO_ERROR)
      Err = WaitForEndOfMovement(Axis,Axis == AXIS_Z ? MOTOR_FINISH_MOVEMENT_TIMEOUT_IN_SEC :
                                                       MORE_TIME_TO_FINISH_MOVEMENT);
   QSleep(m_WAIT_BETWEEN_HOME_AND_GOTO_MOVEMENT_MS);
   return Err;
}

//------------------------------------------------------------------------------------
TQErrCode CMotorsBase::MoveABRelPositionSequencer(TMotorAxis Axis_A,TMotorAxis Axis_B,long Destination)
{
   return MoveABAbsPositionSequencer(Axis_A,Axis_B,Destination+GetAxisLocation(Axis_A));
}

TQErrCode CMotorsBase::MoveABAbsPositionSequencer (TMotorAxis Axis_A, TMotorAxis Axis_B, long Destination)
{
   TQErrCode Err;


   if (m_ParamsMgr->MCB_InhibitOverlappedDecelerationMoves)
   {
    // instead of the usual behavior of this function, wait until B axis finishes its movement.
    if ((Err = CheckForEndOfMovementSequence(Axis_B)) != Q_NO_ERROR)
      return Err;
    if ((Err = GoToAbsolutePositionSequence(Axis_A, Destination)) != Q_NO_ERROR)
      return Err;
    return Q_NO_ERROR;
   }

   //We want to be sure that we arrived to the destination position
   Err = MoveAWhenBStartDeceler(Axis_A,Axis_B,Destination,muSteps);

   if((Err == Q2RT_MCB_ACK_NOT_RELEVANT_COMMAND_DURING_GO_TO) ||
      (Err == Q2RT_MCB_ACK_NOT_RELEVANT_COMMAND_DURING_STOP))
      //TBD it is a BYPASS the Motor answer  Q2RT_MCB_ACK_NOT_RELEVANT_COMMAND_DURING_STOP
      // instead of Q2RT_MCB_ACK_NOT_RELEVANT_COMMAND_DURING_MOVE_AB
      {
      FrontEndInterface->NotificationMessage("MCB error:Command not relevant during GoTo");
	  int j = 0;
      //In this error wait and try again to perform the movement
      while (Err==Q2RT_MCB_ACK_NOT_RELEVANT_COMMAND_DURING_GO_TO && j++<5)
         {
         //bool MotorInMovement = GetAxisIfMotorIsMoving(Axis);
         QSleepSec(WAIT_AND_SEND_COMMAND_AGAIN_SEC);
         CQLog::Write(LOG_TAG_MOTORS,"Motor %s is marked moving wait and send again to destination(%d)",
                                PrintAxis(Axis_A).c_str(),
                               (int)Destination);

         if(Destination != GetAxisLocation(Axis_A))
            Err = MoveAWhenBStartDeceler(Axis_A,Axis_B,Destination,muSteps);
         else
            Err=Q_NO_ERROR;
         }
      }

   return Err;
}

TQErrCode CMotorsBase::MoveABWaitRelPositionSequencer(TMotorAxis Axis_A, TMotorAxis Axis_B, long Destination)
{
   return MoveABWaitAbsPositionSequencer(Axis_A,Axis_B,Destination+GetAxisLocation(Axis_A));
}

TQErrCode CMotorsBase::MoveABWaitAbsPositionSequencer(TMotorAxis Axis_A, TMotorAxis Axis_B, long Destination)
{
   TQErrCode Err;

   if ((Err=MoveABAbsPositionSequencer(Axis_A,Axis_B,Destination)) != Q_NO_ERROR)
      return Err;

   if ((Err=CheckForEndOfMovementSequence(Axis_A,Axis_A == AXIS_Z ? 60 : 10)) != Q_NO_ERROR)
      return Err;

   return Err;
}

TQErrCode CMotorsBase::MoveYZWhenXAbsPositionSequencer(long YPositionInSteps,long ZPositionInSteps)
{
   CQLog::Write(LOG_TAG_MOTORS,"MoveYZwhenXAbsPositionSequencer: Command not support for Eden260");
   return Q2RT_FATAL_ERROR;
}

//-------------------------------------------------------------------------------

TQErrCode CMotors::StopWaitSequence(TMotorAxis Axis)
{
   CQLog::Write(LOG_TAG_MOTORS,"Stop %s",PrintAxis(Axis).c_str() );
   TQErrCode Err = Stop(Axis);
   if (Err != Q_NO_ERROR)
       {
       CQLog::Write(LOG_TAG_MOTORS,"Error during stop %s",PrintAxis(Axis).c_str() );
       return Err;
       }

   if ((Err=CheckForEndOfMovementSequence(Axis,60)) != Q_NO_ERROR)
      return Err;
   CQLog::Write(LOG_TAG_MOTORS,"Stop finished: %s",PrintAxis(Axis).c_str() );

   return Err;
}

TQErrCode CMotors::KillMotionWaitEndMov(TMotorAxis Axis)
{
   CQLog::Write(LOG_TAG_MOTORS,"Kill %s",PrintAxis(Axis).c_str() );
   TQErrCode ReturnCommand = KillMotion(Axis);
   if (ReturnCommand != Q_NO_ERROR)
       {
       CQLog::Write(LOG_TAG_MOTORS,"Error during kill motion %s",PrintAxis(Axis).c_str() );
       return ReturnCommand;
       }

   m_WaitKillMotionError=true;
   // Wait for reply
   QLib::TQWaitResult WaitResult =
      m_SyncEventWaitToKillError.WaitFor(QSecondsToTicks(MCB_KILL_TIME_Sec));

   m_WaitKillMotionError=false;

   if(WaitResult != QLib::wrSignaled)
      {
      CQLog::Write(LOG_TAG_MOTORS,"Kill Motion but did not receive Stop Error: %s",PrintAxis(Axis).c_str() );
      return Q_NO_ERROR;
      }

   CQLog::Write(LOG_TAG_MOTORS,"Kill motion finished: %s",PrintAxis(Axis).c_str() );

   return ReturnCommand;
}

TQErrCode CMotorsBase::CheckForTStateEndOfMovementSequence(void)
{
   TQErrCode Err;
   int Destination = static_cast<int>(GetAxisSolicitedLocation(AXIS_T));

   if ((Err =  WaitForEndOfMovement(AXIS_T,5)) != Q_NO_ERROR)
      {
      switch (Err)
         {
         case Q2RT_MCB_EVENT_ERROR_OVER_CURRENT :
            CQLog::Write(LOG_TAG_MOTORS,"MCB:Axis T : Error Over current");
            FrontEndInterface->NotificationMessage("Error over current");
            if ((Err=GoWaitHome(AXIS_T)) != Q_NO_ERROR)
               return Err;
            if((Err=GoWaitToTAxisStateSequencer(Destination))!=Q_NO_ERROR)
               return Err;
            break;

         case Q2RT_MCB_EVENT_ERROR_HIGH_TEMPERATURE:
            CQLog::Write(LOG_TAG_MOTORS,"MCB:Axis T : Error high temperature");
            FrontEndInterface->NotificationMessage("Error high temperature");
            if ((Err=GoWaitHome(AXIS_T)) != Q_NO_ERROR)
               return Err;
            if((Err=GoWaitToTAxisStateSequencer(Destination))!=Q_NO_ERROR)
               return Err;
            break;

         case Q2RT_MCB_EVENT_ERROR_LOW_TEMPERATURE:
            CQLog::Write(LOG_TAG_MOTORS,"MCB:Axis T : Error low temperature");
            FrontEndInterface->NotificationMessage("Error low temperature");
            if ((Err=GoWaitHome(AXIS_T)) != Q_NO_ERROR)
               return Err;
            if((Err=GoWaitToTAxisStateSequencer(Destination))!=Q_NO_ERROR)
               return Err;
            break;

         case Q2RT_MCB_EVENT_ERROR_VOLTAGE_DROP:
            CQLog::Write(LOG_TAG_MOTORS,"MCB:Axis T : Error voltage drop");
            FrontEndInterface->NotificationMessage("Error voltage drop");
            if ((Err=GoWaitHome(AXIS_T)) != Q_NO_ERROR)
               return Err;
             if((Err=GoWaitToTAxisStateSequencer(Destination))!=Q_NO_ERROR)
               return Err;
            break;

         case Q2RT_MCB_EVENT_ERROR_RIGHT_LIMIT:
            CQLog::Write(LOG_TAG_MOTORS,"MCB:Axis T : Error right limit");
            FrontEndInterface->NotificationMessage("Error right limit");
            if ((Err=GoWaitHome(AXIS_T)) != Q_NO_ERROR)
               return Err;
            if((Err=GoWaitToTAxisStateSequencer(Destination))!=Q_NO_ERROR)
               return Err;
            break;

         case Q2RT_MCB_EVENT_ERROR_LEFT_LIMIT:
            CQLog::Write(LOG_TAG_MOTORS,"MCB:Axis T : Error left limit");
            FrontEndInterface->NotificationMessage("Error left limit");
            if ((Err=GoWaitHome(AXIS_T)) != Q_NO_ERROR)
               return Err;
            if((Err=GoWaitToTAxisStateSequencer(Destination))!=Q_NO_ERROR)
               return Err;
            break;

         case Q2RT_MCB_EVENT_ERROR_TIME_OUT:
            CQLog::Write(LOG_TAG_MOTORS,"MCB:Axis T : Error time out");
            FrontEndInterface->NotificationMessage("Error time out");
            if ((Err=GoWaitHome(AXIS_T)) != Q_NO_ERROR)
               return Err;
            if((Err=GoWaitToTAxisStateSequencer(Destination))!=Q_NO_ERROR)
               return Err;
            break;

         case Q2RT_MCB_EVENT_ERROR_CANNOT_ESCAPE_FROM_HOME_POSITION:
            CQLog::Write(LOG_TAG_MOTORS,"MCB:Axis T : Error can not escape from home position");
            FrontEndInterface->NotificationMessage("Error can not escape from home position");
            if ((Err=GoWaitHome(AXIS_T)) != Q_NO_ERROR)
               return Err;
            if((Err=GoWaitToTAxisStateSequencer(Destination))!=Q_NO_ERROR)
               return Err;
            break;

         case Q2RT_MCB_ACK_TIMEOUT:
            CQLog::Write(LOG_TAG_MOTORS,"MCB:Axis T : Timeout while wait for end of movement");
            FrontEndInterface->NotificationMessage("Axis T: Timeout while wait for end of movement");
            if ((Err=GoWaitHome(AXIS_T)) != Q_NO_ERROR)
               return Err;
            if((Err=GoWaitToTAxisStateSequencer(Destination))!=Q_NO_ERROR)
               return Err;
            break;


         case Q2RT_MCB_WRONG_POSITION_RX_FROM_MCB:
            CQLog::Write(LOG_TAG_MOTORS,"MCB:AXIS T : wrong position RX from MCB-%d",
                                        GetAxisLocation(AXIS_T));
            FrontEndInterface->NotificationMessage("MCB:Axis T:wrong position RX from MCB");
            if((Err=GoWaitToTAxisStateSequencer(Destination))!=Q_NO_ERROR)
              return Err;
            break;

         case Q2RT_SEQUENCE_CANCELED:
         case Q2RT_FATAL_ERROR:
         case Q2RT_MODEL_ON_TRAY_SENSOR_1_DETECTED:
         case Q2RT_MODEL_ON_TRAY_SENSOR_2_DETECTED:
         case Q2RT_MODEL_ON_TRAY_BOTH_SENSOR_DETECTED:
         case Q2RT_MODEL_ON_TRAY_RESPONSE_MSG_FAIL:
            return Err;

          default:
             FrontEndInterface->NotificationMessage("MCB Axis T error event");
             CQLog::Write(LOG_TAG_MOTORS,"MCB Axis T error event = %d)",Err);
             return Err;
        }
      }

   if((Err = GetTAxisState()) != Q_NO_ERROR)
      return Err;

   int State = GetAxisState(AXIS_T);
   //We want to be sure that we arrived to the destination position
   if(Destination != State)
      {
      FrontEndInterface->NotificationMessage("T Axis did not arrive to asked state position");
      CQLog::Write(LOG_TAG_MOTORS,
      "Motor T : did not arrive to solicited state (%d), arrived(%d)",
                                   Destination,State);
      return Q2RT_MCB_NOT_ARRIVED_TO_SOLICITED_POSITION_T;
      }

   //Verify if T arrive to an acceptable position or not, this check
   //prevent from error in T "microswitch" state.
   long CurrentPosition = GetAxisLocation(AXIS_T);
   long Tolerance = m_ParamsMgr->MotorPurgeTTolerance;
   //long AllowedPosition;
   switch (State)
      {
      case PURGE_TANK_CLOSED:
         if(CurrentPosition >= (m_ParamsMgr->MinPositionStep[AXIS_T] + Tolerance))
            {
            CQLog::Write(LOG_TAG_MOTORS,"Motor T:Closed position mismatch (%d),Limits(%d,%d)",
                         CurrentPosition,m_ParamsMgr->MinPositionStep[AXIS_T],
                         m_ParamsMgr->MotorPurgeTTolerance);
            return Q2RT_MCB_NOT_ARRIVED_TO_SOLICITED_POSITION_T;
            }
         break;

      case PURGE_TANK_IN_PURGE_POSITION:
         {
         long AllowedPosition = m_ParamsMgr->MotorPurgeTOpenPosition;

         if((CurrentPosition < (AllowedPosition - Tolerance) ) ||
            (CurrentPosition > (AllowedPosition + Tolerance) ) )
            {
            CQLog::Write(LOG_TAG_MOTORS,"Motor T:Purge position mismatch (%d),Limits(%d,%d)",
                   CurrentPosition,(AllowedPosition - Tolerance),(AllowedPosition + Tolerance));
            return Q2RT_MCB_NOT_ARRIVED_TO_SOLICITED_POSITION_T;
            }
         }
         break;

      case PURGE_TANK_IN_WIPE_POSITIOND:
         {
         long AllowedPosition = m_ParamsMgr->MotorPurgeTActPosition;

         if((CurrentPosition < (AllowedPosition - Tolerance) ) ||
            (CurrentPosition > (AllowedPosition + Tolerance) ) )
            {
            CQLog::Write(LOG_TAG_MOTORS,"Motor T:Wipe position mismatch (%d),Limits(%d,%d)",
                   CurrentPosition,(AllowedPosition - Tolerance),(AllowedPosition + Tolerance));
            return Q2RT_MCB_NOT_ARRIVED_TO_SOLICITED_POSITION_T;
            }
         break;
         }

      default:
         break;
      }

   return Err;
}

TQErrCode CMotorsBase::GoWaitToTAxisStateSequencer(int DestinationState)
{
   TQErrCode Err;

   //we want to be sure what is the current state
   //if we moved manually before the state can be uncorrect
   if((Err = GetTAxisState()) != Q_NO_ERROR)
      return Err;
   int CurrentState = GetAxisState(AXIS_T);
   
   if(DestinationState == CurrentState) // no move is needed
      return Q_NO_ERROR;

   if ((Err = GoToTAxisState(DestinationState)) != Q_NO_ERROR)
      return Err;
   if ((Err = CheckForTStateEndOfMovementSequence()) != Q_NO_ERROR)
         return Err;
      
   return Q_NO_ERROR;
}

//Set Zero Position
//----------------------------------------------------------------------
TQErrCode CMotorsBase::SetZPosition(long & Position)
{
   //Check if home was performed if yes take the current place
   if(GetAxisIfHomeWasEverPerformed(AXIS_Z))
      {
      Position = GetAxisLocation(AXIS_Z);
      return Q_NO_ERROR;
      }

   long PosInSteps = CONFIG_ConvertMMToStep(AXIS_Z,m_ParamsMgr->Z_StartPrintPosition);
   UpdateAxisLocation(AXIS_Z,PosInSteps);

   Position = GetAxisLocation(AXIS_Z);

   return Q_NO_ERROR;
}

//-------------------------------------------------------------
// The following procedure initial all the Motor Parameters
//---------------------------------------------------------
TQErrCode CMotorsBase::InitMotorAxisParameters(TMotorAxis Axis)
{
   TQErrCode Err;

   if(Axis>AXIS_T)
      {
      FrontEndInterface->ErrorMessage(QFormatStr("Wrong Axis Initialized %d",Axis));
      return Q_NO_ERROR;
      }

   if(!MustInitializeParams[Axis])
      return Q_NO_ERROR;

   CQLog::Write(LOG_TAG_MOTORS,"Motors:Initiate %s",PrintAxis(Axis).c_str());
   if((Err=SetVelocity(Axis,m_ParamsMgr->MotorsVelocity[Axis],muMM)) != Q_NO_ERROR)
      return  Err;

   if(Axis!=AXIS_T)
      {
      if((Err=SetAcceleration(Axis,m_ParamsMgr->MotorsAcceleration[Axis],muMM))!= Q_NO_ERROR)
         return  Err;
      if((Err=SetDeceleration(Axis,m_ParamsMgr->MotorsDeceleration[Axis],muMM))!= Q_NO_ERROR)
         return  Err;
      if((Err=SetKillDeceleration(Axis,m_ParamsMgr->MotorsKillDeceleration[Axis],muMM))!= Q_NO_ERROR)
         return  Err;
      if((Err=SetSmoothFactor(Axis,m_ParamsMgr->MotorsSmoothFactor[Axis]))!= Q_NO_ERROR)
         return  Err;
      }
   MustInitializeParams[Axis]=false;
   ResetBumperImpact();

   return Q_NO_ERROR;
}

TQErrCode CMotorsBase::InitMotorParameters(void)
{
// This procedures performs the set commands need by the Motor in the start
// of the print model
   TQErrCode Err;

   ResetBumperImpact();

   if((Err=GetLastError(AXIS_X)) != Q_NO_ERROR)
      return  Err;
   if((Err=GetLastError(AXIS_Y))!= Q_NO_ERROR)
      return  Err;
   if((Err=GetLastError(AXIS_Z)) != Q_NO_ERROR)
      return  Err;
   if((Err=GetLastError(AXIS_T)) != Q_NO_ERROR)
      return  Err;
           
   //First verify if motor is under moving in this case stop move
   bool IsMoving1,IsMoving2;

   for(int Axis=AXIS_X;Axis<=AXIS_T;Axis++)
      {
      if((Err=GetAxisStatus(static_cast<TMotorAxis>(Axis))) != Q_NO_ERROR)
         return Err;
      IsMoving1 = GetAxisIfMotorIsMoving(static_cast<TMotorAxis>(Axis));
      QSleep(WAIT_AND_CHECK_AGAIN_IF_MOTOR_IS_MOVING_MS);
      if((Err=GetAxisStatus(static_cast<TMotorAxis>(Axis))) != Q_NO_ERROR)
         return Err;
      IsMoving2 = GetAxisIfMotorIsMoving(static_cast<TMotorAxis>(Axis));

      if (IsMoving1 || IsMoving2)
         {
         CQLog::Write(LOG_TAG_MOTORS,"Stop movement in %s Motor - motor is moving when program is entered",
                    PrintAxis(static_cast<TMotorAxis>(Axis)).c_str());
         if((Err=StopWaitSequence(static_cast<TMotorAxis>(Axis)))!= Q_NO_ERROR)
            return Err;
         }
      }

   m_YBacklashMoveInSteps=CONFIG_ConvertYmmToStep(m_ParamsMgr->Y_BacklashMove_mm);

   if((Err=GetGeneralInformation()) != Q_NO_ERROR)
      return  Err;

   if (m_ParamsMgr->MotorsHomeMode[AXIS_X] == HOME_MODE_POSITION_COUNTER_RESET &&
       m_ParamsMgr->MotorsHomeMode[AXIS_Y] == HOME_MODE_POSITION_COUNTER_RESET &&
       m_ParamsMgr->MotorsHomeMode[AXIS_Z] == HOME_MODE_POSITION_COUNTER_RESET &&
       m_ParamsMgr->MotorsHomeMode[AXIS_T] == HOME_MODE_POSITION_COUNTER_RESET)
      {
      if((Err=SetHomeMode(AXIS_ALL,HOME_MODE_POSITION_COUNTER_RESET)) != Q_NO_ERROR)
         return  Err;
      }
   else
      {
      if((Err=SetHomeMode(AXIS_X,m_ParamsMgr->MotorsHomeMode[AXIS_X])) != Q_NO_ERROR)
         return  Err;
      if((Err=SetHomeMode(AXIS_Y,m_ParamsMgr->MotorsHomeMode[AXIS_Y])) != Q_NO_ERROR)
         return  Err;
      if((Err=SetHomeMode(AXIS_Z,m_ParamsMgr->MotorsHomeMode[AXIS_Z])) != Q_NO_ERROR)
         return  Err;
      if((Err=SetHomeMode(AXIS_T,m_ParamsMgr->MotorsHomeMode[AXIS_T])) != Q_NO_ERROR)
         return  Err;
      }

   if((Err=SetHomePosition(AXIS_X,m_ParamsMgr->MotorsHomePosition[AXIS_X],muSteps))!= Q_NO_ERROR)
      return  Err;
   if((Err=SetHomePosition(AXIS_Y,m_ParamsMgr->MotorsHomePosition[AXIS_Y],muSteps))!= Q_NO_ERROR)
      return  Err;
   if((Err=SetHomePosition(AXIS_Z,m_ParamsMgr->MotorsHomePosition[AXIS_Z],muSteps))!= Q_NO_ERROR)
      return  Err;
   if((Err=SetHomePosition(AXIS_T,m_ParamsMgr->MotorsHomePosition[AXIS_T],muSteps))!= Q_NO_ERROR)
      return  Err;

   if((Err=SetVelocity(AXIS_X,m_ParamsMgr->MotorsVelocity[AXIS_X],muMM)) != Q_NO_ERROR)
      return  Err;
   if((Err=SetVelocity(AXIS_Y,m_ParamsMgr->MotorsVelocity[AXIS_Y],muMM))!= Q_NO_ERROR)
      return  Err;
   if((Err=SetVelocity(AXIS_Z,m_ParamsMgr->MotorsVelocity[AXIS_Z],muMM))!= Q_NO_ERROR)
      return  Err;
   if((Err=SetVelocity(AXIS_T,m_ParamsMgr->MotorsVelocity[AXIS_T],muMM))!= Q_NO_ERROR)
      return  Err;
   if((Err=SetAcceleration(AXIS_X,m_ParamsMgr->MotorsAcceleration[AXIS_X],muMM))!= Q_NO_ERROR)
      return  Err;
   if((Err=SetAcceleration(AXIS_Y,m_ParamsMgr->MotorsAcceleration[AXIS_Y],muMM))!= Q_NO_ERROR)
      return  Err;
   if((Err=SetAcceleration(AXIS_Z,m_ParamsMgr->MotorsAcceleration[AXIS_Z],muMM))!= Q_NO_ERROR)
      return  Err;
   if((Err=SetDeceleration(AXIS_X,m_ParamsMgr->MotorsDeceleration[AXIS_X],muMM))!= Q_NO_ERROR)
      return  Err;
   if((Err=SetDeceleration(AXIS_Y,m_ParamsMgr->MotorsDeceleration[AXIS_Y],muMM))!= Q_NO_ERROR)
      return  Err;
   if((Err=SetDeceleration(AXIS_Z,m_ParamsMgr->MotorsDeceleration[AXIS_Z],muMM))!= Q_NO_ERROR)
      return  Err;
   if((Err=SetKillDeceleration(AXIS_X,m_ParamsMgr->MotorsKillDeceleration[AXIS_X],muMM))!= Q_NO_ERROR)
      return  Err;
   if((Err=SetKillDeceleration(AXIS_Y,m_ParamsMgr->MotorsKillDeceleration[AXIS_Y],muMM))!= Q_NO_ERROR)
      return  Err;
   if((Err=SetKillDeceleration(AXIS_Z,m_ParamsMgr->MotorsKillDeceleration[AXIS_Z],muMM))!= Q_NO_ERROR)
      return  Err;
   if((Err=SetSmoothFactor(AXIS_X,m_ParamsMgr->MotorsSmoothFactor[AXIS_X]))!= Q_NO_ERROR)
      return  Err;
   if((Err=SetSmoothFactor(AXIS_Y,m_ParamsMgr->MotorsSmoothFactor[AXIS_Y]))!= Q_NO_ERROR)
      return  Err;
   if((Err=SetSmoothFactor(AXIS_Z,m_ParamsMgr->MotorsSmoothFactor[AXIS_Z]))!= Q_NO_ERROR)
      return  Err;

   for(int Axis=AXIS_X;Axis<MAX_AXIS;Axis++)
      MustInitializeParams[Axis]=false;

   // Start the task that sent ping
   // StartSendingPing();

   return Q_NO_ERROR;
}





