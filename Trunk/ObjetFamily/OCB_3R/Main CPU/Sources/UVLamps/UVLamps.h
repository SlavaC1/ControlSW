/*===========================================================================
 *   FILENAME       : UVLamps {UVLamps.h}  
 *   PURPOSE        : UV lamps control and monitor (header file)
 *   DATE CREATED   : 28/06/2004
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/


#ifndef _UV_LAMPS_H_
#define _UV_LAMPS_H_

#include "Define.h"


BOOL IsLeftUVLampEnabled();

BOOL IsRightUVLampEnabled();
#ifdef DEBUG
BOOL IsBothUVLampEnabled();
#endif

// Initialization of the UV lamps interface
void UVLampsInit();

// Set the parameters for the UV lamps ignition
void UVLampsSetParameters(WORD IgnitionTimeout, WORD PostIgnitionTimeout, BOOL SensorBypass, BYTE ActiveLampsMask);

// Set a request to turn on/off the UV lamps
void UVLampsSetOnOff(BOOL OnOff);

// Get the UV lamps status
BOOL UVLampsGetStatus();

// Returns the current status of the left UV lamp
BOOL UVLampsGetLeftLampStatus();

// Returns the current status of the right UV lamp
BOOL UVLampsGetRightLampStatus();

BOOL UVLampsGetCurrentRequest();

// Getters for the A2D value read by the UV sensor.
BOOL CheckUVThreshold(WORD input /*, WORD Avg */ );
WORD UVLampsGetValueINT();
WORD UVLampsGetValueEXT();
WORD UVLampsGetValueINT_Max();
WORD UVLampsGetValueEXT_Max();
WORD UVLampsGetUVReadingMaxDeltaINT();
WORD UVLampsGetUVReadingMaxDeltaEXT();
DWORD UVLampsGetUVReadingSumINT();
DWORD UVLampsGetUVReadingSumEXT();
WORD UVLampsGetUVNumOfReadings();

BOOL UVLampsSetSamplingParams(BOOL RestartSampling, WORD WindowSize);
BOOL UVLampsSetSamplingParamsEx(BOOL a_RestartSampling, WORD a_WindowSizeForAvg, WORD a_WindowSizeForMax, WORD a_UVSamplesThreshold);

// UV safety mechanism (Axis X immobility recognition)
void SetUVSafetyActivated(BOOL IsActivated);
BOOL GetUVSafetyActivated();

#endif