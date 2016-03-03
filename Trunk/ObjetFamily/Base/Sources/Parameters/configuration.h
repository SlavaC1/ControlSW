/*+**********************************************************************
 * File name    :  CONFIGURATION.H
 * Title        :  Configuration package
 * Project      :
 * Subsystem    :
 * Date created :
 * Author       :  Gedalia Trejger
 * History      :
 *
 * Description  :   This package includes the global configuration of
                    Q2 RT environment.

Resources Used:
---------------

* Limitation:

* Procedures:

/************************INCLUDE FILES**********************************/

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include "QTypes.h"
#include "LayerProcessDefs.h"
#include "MotorDefs.h"

void CONFIG_Init(void);

long CONFIG_GetPrintXStartPosition_step (void);
long CONFIG_GetPrintTrayXSize_step (void);
long CONFIG_GetPrintXEndPosition_step (void);

long CONFIG_GetPrintYStartPosition_step (/*float Space*/);
long CONFIG_GetPrintYEndPosition_step (void);

long CONFIG_ConvertXmmToStep(float Size);
long CONFIG_ConvertYmmToStep(float Size);
long CONFIG_ConvertZmmToStep(float Size);
long CONFIG_ConvertZumToStep(float Size);
long CONFIG_ConvertMMToStep(TMotorAxis Axis,float Pos);

long CONFIG_ConvertYmmTo300DPIAdjustedPixels(float Size);

float CONFIG_ConvertYmmToFloat300DPIAdjustedPixels(float Size);

TMotorPosition CONFIG_ConvertStepToUnits(TMotorAxis Axis,long Pos,TMotorUnits Units);
long CONFIG_ConvertUnitsToStep(TMotorAxis Axis,TMotorPosition Pos,TMotorUnits Units);

long CONFIG_GetXAccelerationSpace (void);
long CONFIG_GetXDecelerationSpace (void);

long CONFIG_GetHeadsPrintWidht (void);

#endif






