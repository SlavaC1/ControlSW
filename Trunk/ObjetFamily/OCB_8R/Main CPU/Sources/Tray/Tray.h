/*===========================================================================
 *   FILENAME       : Tray {Tray.h}  
 *   PURPOSE        : 
 *   DATE CREATED   : 09/09/2004
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/


#ifndef _TRAY_H_
#define _TRAY_H_

#include "Define.h"

void TrayInit(void);
BOOL IsTrayInserted();

// Initiated by Messages from embedded.
void TraySetTemperaturMonitoringOnOff(BOOL OnOff);
void TraySetParameters(WORD Temperature, BYTE ActiveMargin, WORD Timeout, WORD OverShoot);
void TrayGetStatus(	WORD *CurrentTrayTemp, WORD *SetTrayTemp, BYTE *ActiveMargine);

void TrayGetCurrentTemperature(void);

BOOL TrayNotNeedHeating(void);
BOOL TrayNeedHeating(void);
BOOL TrayIsCurrentTemperatureInActiveRange(void);
BOOL TrayIsHotterThenActiveRange(void);
BOOL TrayIsColderThenActiveRange(void);

BOOL ThermistorOutOfRange(WORD temp);
BOOL ThermistorShorted (WORD temp);
BOOL ThermistorOpened (WORD temp);

void TrayHeatingTask(BYTE Arg);

#endif

