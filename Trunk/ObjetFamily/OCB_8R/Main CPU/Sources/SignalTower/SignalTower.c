#include "SignalTower.h"
#include "Actuators.h"
#include "MiniScheduler.h"
							
#define NUM_OF_LIGHTS 3

BYTE xdata LightsState[NUM_OF_LIGHTS];

const BYTE code LightsActutatorId[NUM_OF_LIGHTS] = 
{
	SIGNAL_TOWER_RED_ACTUATOR_ID,			
	SIGNAL_TOWER_GREEN_ACTUATOR_ID,
	SIGNAL_TOWER_YELLOW_ACTUATOR_ID			
};

WORD xdata DutyOnTime_ms  = 1000;
WORD xdata DutyOffTime_ms = 1000;

void SignalTowerBlinkingTask(BYTE Arg)
{
	int i;

	enum 
	{
    	BLINK_ON,
    	BLINK_OFF
    };

	switch(Arg)
	{
		case BLINK_ON:
		{
			for(i = 0; i < NUM_OF_LIGHTS; i++)
			{
				if(LightsState[i] == (BYTE)ST_LIGHT_BLINK)
					ActuatorsSetOnOff(LightsActutatorId[i], TRUE);
			}							
			
			SchedulerLeaveTask(BLINK_OFF);
			SchedulerTaskSleep(-1, DutyOnTime_ms);
			break;
		}
		
		case BLINK_OFF:
		{
			for(i = 0; i < NUM_OF_LIGHTS; i++)
			{
				if(LightsState[i] == (BYTE)ST_LIGHT_BLINK)
					ActuatorsSetOnOff(LightsActutatorId[i], FALSE);
			}

			SchedulerLeaveTask(BLINK_ON);
			SchedulerTaskSleep(-1, DutyOffTime_ms);
			break;
		}
		
		default:
			break; 
	}	
}

void SetSignalTowerLights(BYTE RedLight, BYTE GreenLight, BYTE YellowLight, WORD DutyOnTime, WORD DutyOffTime)
{
	int i;

	LightsState[0] = RedLight;
	LightsState[1] = GreenLight;
	LightsState[2] = YellowLight;

	DutyOnTime_ms  = DutyOnTime;
	DutyOffTime_ms = DutyOffTime;	

	for(i = 0; i < NUM_OF_LIGHTS; i++)
	{
		if(LightsState[i] == (BYTE)ST_LIGHT_ON)
			ActuatorsSetOnOff(LightsActutatorId[i], TRUE);
		else if(LightsState[i] == (BYTE)ST_LIGHT_OFF)
			ActuatorsSetOnOff(LightsActutatorId[i], FALSE);	
	}	
}