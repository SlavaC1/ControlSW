/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Motors related general definitions.                      *
 * Module Description: Motors related general definitions.          *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 03/03/2003                                           *
 * Last update: 03/03/2003                                          *
 ********************************************************************/

#ifndef _MOTOR_DEFS_H_
#define _MOTOR_DEFS_H_

// Number of different Axis
const int MAX_AXIS = 4;
const int AXIS_X = 0;
const int AXIS_Y = 1;
const int AXIS_Z = 2;
const int AXIS_T = 3;
const int AXIS_GENERAL = 4;
const int AXIS_XYZ = 5;
const int AXIS_ALL = 6;

typedef bool T_AxesTable[MAX_AXIS];

const int ENCODER_ADDRESS = 0x80;

typedef int TMotorAxis;
typedef int TMotorUnits;
const int muSteps = 0;
const int muMM    = 1;
const int muInch  = 2;

typedef float TMotorPosition;
typedef float TMotorVelocity;
typedef float TMotorAcceleration;

const unsigned int MOTOR_FINISH_MOVEMENT_TIMEOUT_IN_SEC = 120;

const int NOT_IN_A_PRE_DEFINED_STATE=0;
const int PURGE_TANK_CLOSED=1;
const int PURGE_TANK_IN_PURGE_POSITION=2;
const int PURGE_TANK_IN_WIPE_POSITIOND=3;

const int WAIT_AND_CHECK_AGAIN_IF_MOTOR_IS_MOVING_MS = 500; //In miliseconds

const int EXTRA_WAIT_TIME_FOR_END_OF_MOVE = 5;

#ifdef MOTOR_MCB
const int MCB_SW_TABLE_FirstElement = 1;
#endif

#endif

