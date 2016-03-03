/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT.                                                   *
 * Module: Configuration                                            *
 * Module Description: Configuration related services.              *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger.                                         *
 * Start date: 22/05/2001                                           *
 * Last upate: 02/05/2002                                           *
 ********************************************************************/

#include "Configuration.h"
#include "AppParams.h"


//************************CONSTANTS**********************************
static CAppParams *gParamsMgr;

// Init
void CONFIG_Init (void)
{
  gParamsMgr = CAppParams::Instance();
}


/***********************************************************************
* Function Name:  CONFIG_GetPrintXStartPosition_step
*
* Description:    Get the X start position. i.e. the start of the Tray.
*
* Input:          None
*
* Output:         X start position   
*
* Called By:
***/
long CONFIG_GetPrintXStartPosition_step (void)
{
   return CONFIG_ConvertXmmToStep(gParamsMgr->TrayStartPositionX);
}

long CONFIG_GetPrintTrayXSize_step (void)
{
   return CONFIG_ConvertXmmToStep(gParamsMgr->TrayXSize);
}


/***********************************************************************
* Function Name:  CONFIG_GetPrintYStartPosition_step
*
* Description:    Get the Y start position.
*
* Input:          Space : The Y distance between the both Y move of the same pass
*
* Output:         Y start position
*
* Called By:      
*
*/
long CONFIG_GetPrintYStartPosition_step (/*float Space*/)
{
   long YStartPosition = ((gParamsMgr->TrayStartPositionY * gParamsMgr->StepsPerMM[AXIS_Y]) -
                          QSimpleRound(gParamsMgr->YStepsPerPixel * (float)(HEAD_PRINT_WIDTH_300_DPI - CONFIG_ConvertYmmTo300DPIAdjustedPixels(TRAY_Y_START_OFFSET_MM))));
   //executing this line causes bug 1590 - Y calculation error on large model
   //YStartPosition += QSimpleRound(gParamsMgr->YStepsPerPixel * Space);
   return YStartPosition;
}

// Converison functions between mm to steps
long CONFIG_ConvertXmmToStep (float Pos)
{
  return QSimpleRound(Pos * gParamsMgr->StepsPerMM[AXIS_X]);
}

long CONFIG_ConvertYmmToStep (float Pos)
{
  return QSimpleRound(Pos * gParamsMgr->StepsPerMM[AXIS_Y]);
}

long CONFIG_ConvertZumToStep (float Pos)
{
  return QSimpleRound((Pos * gParamsMgr->StepsPerMM[AXIS_Z]) / 1000.0f);
}

long CONFIG_ConvertZmmToStep (float Pos)
{
  return QSimpleRound((Pos * gParamsMgr->StepsPerMM[AXIS_Z]));
}

// Generic version - mm
long CONFIG_ConvertMMToStep(TMotorAxis Axis,float Pos)
{
  return QSimpleRound(Pos * gParamsMgr->StepsPerMM[Axis]);
}

// Generic version - any units
long CONFIG_ConvertUnitsToStep(TMotorAxis Axis,TMotorPosition Pos,TMotorUnits Units)
{
  long Result;

  // Convert to the required units
  switch(Units)
  {
    case muMM:
      Result = QSimpleRound(Pos * gParamsMgr->StepsPerMM[Axis]);
      break;

    case muInch:
      Result = QSimpleRound(Pos * gParamsMgr->StepsPerMM[Axis] * MM_PER_INCH);
      break;

    default:
      Result = Pos;
      break;
  }

  return Result;
}

TMotorPosition CONFIG_ConvertStepToUnits(TMotorAxis Axis,long Pos,TMotorUnits Units)
{
  TMotorPosition Result;

  // Convert to the required units
  switch(Units)
  {
    case muMM:
      Result = Pos / gParamsMgr->StepsPerMM[Axis];
      break;

    case muInch:
      Result = Pos / (gParamsMgr->StepsPerMM[Axis] * MM_PER_INCH);
      break;

    default:
      Result = Pos;
      break;
  }

  return Result;
}


/* ==========================================================================
   PROCEDURE NAME : GetXAccelerationSpace 
   
   DESCRIPTION :  Torricelli Formula:  V-> Velocity
                                       A-> Acceleration
                                       S-> Space
                                       
                  V**2 = Vo**2 +2A(S-So)  --->  Vo= 0, So= 0
                  V**2 = 2AS    ---------------
                                | S = V**2/2A |
                                ---------------
   INPUT
   OUTPUT

   DATE CREATED:  13 Feb 2001

   CHANGED BY :                                    CHANGE DATE :
   CHANGE DESCRIPTION :
=============================================================================*/
long CONFIG_GetXAccelerationSpace (void)
{
   TMotorAcceleration Space = gParamsMgr->MotorsVelocity [AXIS_X] * gParamsMgr->MotorsVelocity[AXIS_X] /
                              (gParamsMgr->MotorsAcceleration[AXIS_X] * 2);

   return CONFIG_ConvertXmmToStep(Space) + 1;
}

/* ==========================================================================
   PROCEDURE NAME : GetXDecelerationSpace

   DESCRIPTION :  Torricelli Formula:  V-> Velocity
                                       A-> Acceleration
                                       S-> Space

                  V**2 = Vo**2 +2A(S-So)  --->  Vo= 0, So= 0
                  V**2 = 2AS    ---------------
                                | S = V**2/2A |
                                ---------------
   INPUT
   OUTPUT

   DATE CREATED:  13 Feb 2001

   CHANGED BY :                                    CHANGE DATE :
   CHANGE DESCRIPTION :
=============================================================================*/
long CONFIG_GetXDecelerationSpace (void)
{
  TMotorAcceleration Space =  gParamsMgr->MotorsVelocity[AXIS_X]*gParamsMgr->MotorsVelocity[AXIS_X]/
                              (gParamsMgr->MotorsDeceleration[AXIS_X]*2);

  return CONFIG_ConvertXmmToStep(Space) + 1;
}


long CONFIG_GetHeadsPrintWidht (void)
{
  return ((HEAD_PRINT_WIDTH_300_DPI * gParamsMgr->DPI_InYAxis) / 300);
}


// Convert Y mm to the adjusted heads DPI (since the heads are not exactly 300 DPI)
long CONFIG_ConvertYmmTo300DPIAdjustedPixels(float Size)
{
  return QSimpleRound(Size * gParamsMgr->StepsPerMM[AXIS_Y] / gParamsMgr->YStepsPerPixel);
}

// Convert Y mm to the adjusted heads DPI - without any round
float CONFIG_ConvertYmmToFloat300DPIAdjustedPixels(float Size)
{
  return (Size * gParamsMgr->StepsPerMM[AXIS_Y] / gParamsMgr->YStepsPerPixel);
}


