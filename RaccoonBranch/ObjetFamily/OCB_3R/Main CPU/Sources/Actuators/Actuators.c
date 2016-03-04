/*===========================================================================
 *   FILENAME       : Actuators {Actuators.c}  
 *   PURPOSE        : Interface to the digital outputs 
 *   DATE CREATED   : 14/06/2004
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/

#include "Actuators.h"
#include "UVLamps.h"
#include "Spi_A2D.h"
#ifdef OCB_SIMULATOR
#include "c8051F120.h"
#include "..\ExtMemAddSim\ExtMemAddSim.h"
#include "..\EXTMemSim\EXTMem.h"
#else
#include "c8051F020.h"
#endif
#include "I2C_DRV.h" 
#include "string.h"
#include "MiniScheduler.h"


// Constants
#define NUM_OF_ACTUATOR_PORTS 6
enum {PORT_4, PORT_5, PORT_6, PORT_7, PORT_I2C_1, PORT_I2C_2};


// Local variables
// The current state of each actuator
BYTE xdata ActuatorsState[NUM_OF_ACTUATOR_PORTS];

#ifdef DEBUG
// The state of the actuators that were on by the 'Timed Set On Off' command
BYTE xdata TimedActuatorsState[NUM_OF_ACTUATOR_PORTS];
#endif

BYTE ActuatorsGetPortNumber(BYTE ActutorNum);

// Local routines
//WORD xdata tempValue; //for testing
 
WORD xdata RollerSuctionPumpOnValue;	   
WORD xdata RollerSuctionPumpOffValue; 
WORD xdata SuctionValveOnTime;
WORD xdata SuctionValveOffTime; 	  
TTaskHandle xdata SuctionSystemTaskHandle;	

void ActuatorsInit()
{
	//tempValue = 0; //for testing
  // Configure ports 4 - 7 to push-pull
#ifndef OCB_SIMULATOR
   	P74OUT = 0xFF;
#endif

  ActuatorsSetAllOff();

  // Enable all the ports (write "0" to the port enable)
  ActuatorsSetOnOff(PORT_ENABLE_ACTUATOR_ID, FALSE);

  SuctionSystemTaskHandle = SchedulerInstallTask(SuctionSystemTaskFunc);
}

void ActuatorsSetOnOff(BYTE ActuatorNum, BOOL OnOff)
{
  BYTE PortNum = ActuatorsGetPortNumber(ActuatorNum);
  BYTE PortNumMask = 1 << (ActuatorNum % 8);

  // ********************************************
  // SAFETY -- prevent the ignition of UV lamps
  // from actuators in case of UV safety trigger
  if ( ActuatorNum == RIGHT_LAMP_ACTUATOR_ID || ActuatorNum == LEFT_LAMP_ACTUATOR_ID ) // if referring to one of the UV lamps
  {
  	if( OnOff ) // if the request is to turn ON the actuator (which means to turn ON one of the lamps)
	{
	  if( GetUVSafetyActivated() ) // if the UV safety trigger is set
      {
	    // If the UV safety trigger is set, the following command will cause the 
		// UV state machine to send an immobility error notification to the EM,
		// (and of course also ignore the turning-on request)
        UVLampsSetOnOff(TRUE);
	    return;
      }
	}
  }
  // ********************************************

  if (PortNum > NUM_OF_ACTUATOR_PORTS - 1)
    return;

  if (OnOff)
    ActuatorsState[PortNum] |= PortNumMask;
  else
    ActuatorsState[PortNum] &= ~PortNumMask;

  switch(PortNum)
  {
    case PORT_4:
	  
	  #ifdef OCB_SIMULATOR	//luda
	  EXTMem_Write(P4_ADD, ActuatorsState[PortNum]);
	  #else
	  P4 = ActuatorsState[PortNum]; 
	  #endif
	  // under SIM, it should be: ExtMemWrite(P4_ADD, ActuatorsState[PortNum]);
      break;

    case PORT_5:
	  
	  #ifdef OCB_SIMULATOR	//luda
	  EXTMem_Write(P5_ADD, ActuatorsState[PortNum]);
	  #else
	  P5 = ActuatorsState[PortNum];
	  #endif
      break;

    case PORT_6: 
	  
	  #ifdef OCB_SIMULATOR	//luda
	  EXTMem_Write(P6_ADD, ActuatorsState[PortNum]);
	  #else
	  P6 = ActuatorsState[PortNum];
	  #endif
      break;

    case PORT_7:
      
	  #ifdef OCB_SIMULATOR	//luda
	  EXTMem_Write(P7_ADD, ActuatorsState[PortNum]);
	  #else
	  P7 = ActuatorsState[PortNum];
	  #endif
      break;

	case PORT_I2C_1:
	 I2CWriteByte(FIRST_CHIP_SELECT,ActuatorsState[PortNum]);
	  break;

	case PORT_I2C_2:
	  I2CWriteByte(SECOND_CHIP_SELECT,ActuatorsState[PortNum]);
	  break;

  }
 
}
#ifdef DEBUG
void ActuatorsTimedSetOnOff(BYTE ActuatorNum, BOOL OnOff, DWORD Time)
{
  BYTE PortNum = ActuatorsGetPortNumber(ActuatorNum);
  BYTE PortNumMask = 1 << (ActuatorNum % 8);

  if (PortNum > NUM_OF_ACTUATOR_PORTS - 1)
    return;

  if (OnOff)
    TimedActuatorsState[PortNum] |= PortNumMask;
  else
    TimedActuatorsState[PortNum] &= ~PortNumMask;

  ActuatorsSetOnOff(ActuatorNum, OnOff);

}
#endif
BOOL ActuatorsGetState(BYTE ActuatorNum)
{
  BYTE PortNumMask = 1 << (ActuatorNum % 8);
  if (ActuatorsState[ActuatorsGetPortNumber(ActuatorNum)] & PortNumMask)
    return TRUE;

  return FALSE;
}

void ActuatorsSetAllOff()
{
  BYTE idata i;

  #ifdef OCB_SIMULATOR	//luda
  EXTMem_Write(P4_ADD, 0);
  EXTMem_Write(P5_ADD, 0);
  EXTMem_Write(P6_ADD, 0);
  EXTMem_Write(P7_ADD, 0);

  #else
  P4 = 0;
  P5 = 0;
  P6 = 0;
  P7 = 0;

  #endif

  for ( i = 0; i < NUM_OF_ACTUATOR_PORTS; i++)
    ActuatorsState[i] = 0;
}

BYTE ActuatorsGetPortNumber(BYTE ActutorNum)
{
  return ActutorNum / 8;
}

void ActuatorsGetAllActuatorsState(BYTE *State)
{
  // The ActuatorsState always holds the latest actuators *command* - which is also what this function returns.
  // But note that the Cordillia physical actuators may have not yet been updated according to the new command.
  // This may happen if a call to ActuatorsSetOnOff() was issued just before this function was called.
  // If this is the case it is ascertained that the Cordillia physical actuators are going to be updated shortly according to the new command.
  // So it should be considered correct to return ActuatorsState.
  memcpy(State, ActuatorsState, sizeof(ActuatorsState)); 
}

//RSS, itamar
void InitiateSuctionSystemActivation(WORD OnTime, WORD OffTime, BYTE OnOff)
{	
	if(!OnOff) //Stop system activation
	{
		ActuatorsSetOnOff(ROLLER_SUCTION_PUMP_ACTUATOR_ID, OnOff); //pump
		ActuatorsSetOnOff(ROLLER_SUCTION_VALVE_ACTUATOR_ID, OnOff); //valve
		SchedulerSuspendTask(SuctionSystemTaskHandle);
	}
	else //start system activation
	{
		SuctionValveOnTime = OnTime;
		SuctionValveOffTime = OffTime;
		ActuatorsSetOnOff(ROLLER_SUCTION_PUMP_ACTUATOR_ID, OnOff);
		SchedulerResumeTask(SuctionSystemTaskHandle,OnOff);
	}	
}

void ReadRollerSuctionPumpValue(WORD * ReadingValue)
{
	*ReadingValue = SpiA2D_GetReading(ANALOG_IN_9);
	//*ReadingValue = tempValue++;  //for testing
} 

void GetRollerSuctionPumpReadings(WORD * OnReadingValue, WORD * OffReadingValue)
{
	*OnReadingValue = RollerSuctionPumpOnValue;
	*OffReadingValue = RollerSuctionPumpOffValue;
}

void SuctionSystemTaskFunc(BYTE Arg)
{
	if(Arg)
	{
		ReadRollerSuctionPumpValue(&RollerSuctionPumpOnValue);
		ActuatorsSetOnOff(ROLLER_SUCTION_VALVE_ACTUATOR_ID, Arg);
		SchedulerLeaveTask(!Arg); 
   		SchedulerTaskSleep(-1, SuctionValveOnTime);
	}
	else
	{
		ReadRollerSuctionPumpValue(&RollerSuctionPumpOffValue);
		ActuatorsSetOnOff(ROLLER_SUCTION_VALVE_ACTUATOR_ID, Arg);
		SchedulerLeaveTask(!Arg); 
   		SchedulerTaskSleep(-1, SuctionValveOffTime);
	}
}


