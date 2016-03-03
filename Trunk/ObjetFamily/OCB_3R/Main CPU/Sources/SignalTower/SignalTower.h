#ifndef _SIGNAL_TOWER_H_
#define _SIGNAL_TOWER_H_

#include "Define.h"

typedef enum
{
	ST_LIGHT_ON    = 1 << 0,
	ST_LIGHT_OFF   = 1 << 1,
	ST_LIGHT_BLINK = 1 << 2,
	ST_LIGHT_NC	   = 1 << 3
}TSignalTowerLightState;

void SignalTowerBlinkingTask(BYTE Arg);
void SetSignalTowerLights(BYTE RedLight, BYTE GreenLight, BYTE YellowLight, WORD DutyOnTime, WORD DutyOffTime);



#endif