/*===========================================================================
 *   FILENAME       : Sensors {Sensors.c}  
 *   PURPOSE        : Interface to the digital inputs 
 *   DATE CREATED   : 21/06/2004
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/

#include "Sensors.h"
#ifdef OCB_SIMULATOR
#include "c8051F120.h"
#include "..\ExtMemAddSim\ExtMemAddSim.h"
#include "..\EXTMemSim\EXTMem.h"
#elif defined OCB2
#include "c8051F120.h"
#else
#include "c8051F020.h"
#endif
#include "I2C_DRV.h" 

// Constants
#ifdef OBJET_MACHINE
#define NUM_OF_SENSOR_PORTS   6
enum {PORT_0, PORT_1, PORT_2, PORT_3, PORT_I2C_1, PORT_I2C_2};
#else
#define NUM_OF_SENSOR_PORTS   3
enum {PORT_1, PORT_2, PORT_3};
#endif
#define SENSOR_1_MASK     0x80 //0
#define SENSOR_2_MASK     0x08
#define SENSOR_3_MASK     0x10
#define SENSOR_4_MASK     0x20
#define SENSOR_5_MASK     0x40
#define SENSOR_6_MASK     0x80
#define SENSOR_7_MASK     0x01 //
#define SENSOR_8_MASK     0x02 //7
#define SENSOR_9_MASK     0x04
#define SENSOR_10_MASK    0x08 
#define SENSOR_11_MASK    0x10 //10
#define SENSOR_12_MASK    0x20 
#define SENSOR_13_MASK    0x40
#define SENSOR_14_MASK    0x80 //13
#ifdef OBJET_MACHINE
#define SENSOR_15_MASK    0x40
#define SENSOR_16_MASK    0x80
#define SENSOR_17_MASK    0x01
#define SENSOR_18_MASK    0x02
#define SENSOR_19_MASK    0x04
#define SENSOR_20_MASK    0x08
#define SENSOR_21_MASK    0x01
#define SENSOR_22_MASK    0x02
#define SENSOR_23_MASK    0x04
#define SENSOR_24_MASK    0x08
#endif

#define INVERT_PORT_3_MASK  0xEF

#ifdef OBJET_MACHINE
BYTE xdata bIsNormallyOpen = FALSE;
// Local variables
// The current state of each sensor
const BYTE code SensorPortsLookup[] = {PORT_1,PORT_2,PORT_2,PORT_2,PORT_2,PORT_2,
                                  PORT_3,PORT_3,PORT_3,PORT_3,PORT_3,PORT_3,
                                  PORT_3,PORT_3,PORT_0,PORT_0,PORT_I2C_1,
								  PORT_I2C_1,PORT_I2C_1,PORT_I2C_1,PORT_I2C_2,
								  PORT_I2C_2,PORT_I2C_2,PORT_I2C_2};

const BYTE code SensorsMaskLookup[] = {SENSOR_1_MASK, SENSOR_2_MASK, SENSOR_3_MASK,
                                  SENSOR_4_MASK, SENSOR_5_MASK, SENSOR_6_MASK,
                                  SENSOR_7_MASK, SENSOR_8_MASK, SENSOR_9_MASK,
                                  SENSOR_10_MASK, SENSOR_11_MASK, SENSOR_12_MASK,
                                  SENSOR_13_MASK, SENSOR_14_MASK,SENSOR_15_MASK,
								  SENSOR_16_MASK, SENSOR_17_MASK,SENSOR_18_MASK,
								  SENSOR_19_MASK,SENSOR_20_MASK, SENSOR_21_MASK,
								  SENSOR_22_MASK, SENSOR_23_MASK,SENSOR_24_MASK
								  };
#else
// Local variables
// The current state of each sensor
const BYTE code SensorPortsLookup[] = {PORT_1,PORT_2,PORT_2,PORT_2,PORT_2,PORT_2,
                                  PORT_3,PORT_3,PORT_3,PORT_3,PORT_3,PORT_3,
                                  PORT_3,PORT_3};
const BYTE code SensorsMaskLookup[] = {SENSOR_1_MASK, SENSOR_2_MASK, SENSOR_3_MASK,
                                  SENSOR_4_MASK, SENSOR_5_MASK, SENSOR_6_MASK,
                                  SENSOR_7_MASK, SENSOR_8_MASK, SENSOR_9_MASK,
                                  SENSOR_10_MASK, SENSOR_11_MASK, SENSOR_12_MASK,
                                  SENSOR_13_MASK, SENSOR_14_MASK};
#endif


void SensorsInit()
{
#ifdef OBJET_MACHINE
#ifdef OCB2
	SFRPAGE = CONFIG_PAGE;
	// Note: Following SFRs behave the SAME in F120 as in F020:
	//		 P1MDIN, P0MDOUT, P1MDOUT, P2MDOUT, P3MDOUT.
#endif
  // Configure P1.7 as analog input
  P1MDIN &= 0x7F;
  P1MDOUT &= 0x7F;
  P1 |= 0x80;
  // Configure P0.6 and P0.7 as digital inputs
  P0MDOUT &= 0x3F;  
  // Disable the I2C port pin
  //XBR0 &= 0xFE;
  XBR2 &= 0x7F;
  P0 |= 0xC0; 
#else
  // Configure P1.7 as digital input
  P1MDIN |= 0x80;
  P1MDOUT &= 0x7F;
#endif
  // Configure P2.3 - P2.7 as digital inputs
  P2MDOUT &= 0x07;

  // Configure P3.0 - P3.7 as digital inputs
  P3MDOUT = 0;
}

BOOL SensorsGetState(BYTE SensorNum)
{
  BYTE xdata State, PortStatus;
  BYTE xdata SensorPortNum = SensorPortsLookup[SensorNum];

  switch(SensorPortNum)
  {
    case PORT_0:  
	  #ifdef OCB_SIMULATOR
	  EXTMem_Read(P0_ADD,&PortStatus );
	  #else
	  PortStatus = P0;
	  #endif
      
      // The sensors logic is inverted logic
      PortStatus = ~PortStatus;
      State  = (PortStatus & SensorsMaskLookup[SensorNum]? TRUE : FALSE);
      break;
    case PORT_1:	  
	  #ifdef OCB_SIMULATOR
	  EXTMem_Read(P1_ADD,&PortStatus );
	  #else
	  PortStatus = P1;
	  #endif  

      // The sensors logic is inverted logic
      PortStatus = ~PortStatus;
      State  = (PortStatus & SensorsMaskLookup[SensorNum]? TRUE : FALSE);
      break;

    case PORT_2:
	  #ifdef OCB_SIMULATOR
	  EXTMem_Read(P2_ADD,&PortStatus );
	  #else
	  PortStatus = P2;
	  #endif      

      // The sensors logic is inverted logic
      PortStatus = ~PortStatus;
      State  = (PortStatus & SensorsMaskLookup[SensorNum]? TRUE : FALSE);
      break;

    case PORT_3:
	  #ifdef OCB_SIMULATOR
	  EXTMem_Read(P3_ADD,&PortStatus );
	  #else
	  PortStatus = P3;
	  #endif
      
      // The sensors logic is inverted logic (except the interlock sensor)
      PortStatus = PortStatus ^ INVERT_PORT_3_MASK;
      State  = (PortStatus & SensorsMaskLookup[SensorNum]? TRUE : FALSE);
      break;

	 case PORT_I2C_1:
      PortStatus = I2CA2D_GetReading(SWITCHES, FIRSTMSCCARD);
	  
      State  = (PortStatus  & SensorsMaskLookup[SensorNum] ? TRUE : FALSE);
	  
      break;

	  case PORT_I2C_2:
      PortStatus = I2CA2D_GetReading(SWITCHES, SECONDMSCCARD);
	  
      State  = (PortStatus  & SensorsMaskLookup[SensorNum] ? TRUE : FALSE);
	  
      break;

  }
    if(
       (SensorGetNormallyOpenStatus())&& // If it is normally open inter lock feed back
       (
	      (IS_FB_SENSOR_ID  == SensorNum) || // Service door
	      (IS_RB_SENSOR_ID  == SensorNum) || // Front right door	 
	      (IL_FB_SENSOR_ID  == SensorNum) || // Front left door
		  (SERVICE_KEY_SENSOR_ID == SensorNum) || // service key
	      (SSR_FB_SENSOR_ID == SensorNum) || // Emergency stop rear
	      (SSF_FB_SENSOR_ID == SensorNum)	 // Emergency stop rear	   
		)   		 
	  )	
  {
    State = State^1; 		 //inverse the bit because it's normally open inter lock 
  }
  return State;
}

void SensorsGetAllSensorsState(BYTE *SensorsState)
{
  BYTE idata i;
  WORD Mask = 1;
  SensorsState[0] = 0;
  SensorsState[1] = 0;
  SensorsState[2] = 0;

  for (i = 0; i < NUM_OF_SENSORS; i++)
  {
  	if (SensorsGetState(i))
	  	SensorsState[i / 8] |= (Mask << (i % 8));
	  else
		  SensorsState[i / 8] &= ~(Mask << (i % 8));

  }  
}

void SensorSetNormallyOpen(BYTE status)
{
	 bIsNormallyOpen = 	status;
}
BYTE SensorGetNormallyOpenStatus()
{
	return 	 bIsNormallyOpen;
}


