/*===========================================================================
 *   FILENAME       : Actuators {Actuators.c}  
 *   PURPOSE        : Interface to some general actuators 
 *   DATE CREATED   : 21/Nov/2002
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/

#include "ActuatorsOHDB.h"
#ifdef OCB_SIMULATOR
#include "c8051F120.h"
#include "..\ExtMemAddSim\ExtMemAddSim.h"
#include "..\EXTMemSim\EXTMem.h"
#else
#include "c8051F020.h"
#endif
#include "TimerDrv.h"

#define MATERIAL_COOLING_FAN_PORT_BIT   0x10
#define PRE_HEATER_PORT_BIT             0x20
#define COUNT_1_MS                      0x399    


WORD xdata OnPeriodCount;
WORD xdata OffPeriodCount;
BOOL xdata FansStatus;


/****************************************************************************
 *
 *  NAME        : InitMaterialCoolingFans 
 *
 *  DESCRIPTION :                                            
 *
 ****************************************************************************/
void InitMaterialCoolingFans()
{
#ifdef OCB_SIMULATOR
 BYTE xdata Item;
#endif
  // Turn off the fans 
  P1 = (P1 & 0xEF);
  #ifdef OCB_SIMULATOR
  EXTMem_Read(P1_ADD_OHDB,&Item );
  //dataItemRes = dataItemRes & 0xEF ;
  EXTMem_Write(P1_ADD_OHDB,  Item & 0xEF);
  #endif

  Timer_4_Init();

  FansStatus = FALSE;
	
}

/****************************************************************************
 *
 *  NAME        : TurnMaterialCoolingFansOnOff 
 *
 *  DESCRIPTION : Set on/off the material cooling fans control                                           
 *
 ****************************************************************************/
void SetMaterialCoolingFansOnOff(BOOL OnOff, BYTE OnPeriod)
{
#ifdef OCB_SIMULATOR
 BYTE xdata Item;
#endif

  if (OnPeriod == 0)
    OnPeriod = 1;

  if (OnOff)
	{
	  OnPeriodCount = 0xFFFF - ((unsigned long)COUNT_1_MS * OnPeriod  / 100);
    OffPeriodCount = 0xFFFF - ((unsigned long)COUNT_1_MS * (100 - OnPeriod) / 100);

#ifndef OCB_SIMULATOR
		// Load the timer for the off period 
		TL4 = OffPeriodCount & LSB_MASK;
    TH4 = (OffPeriodCount & MSB_MASK) >> 8;
#endif
		FansStatus = FALSE;

 #ifndef OCB_SIMULATOR
    // Enable timer 4 interrupt
	  EIE2 |= 0x04;
 #endif
	}
	else
	{
  #ifndef OCB_SIMULATOR
    // Disable timer 4 interrupt
	  EIE2 &= 0xFB;
 
	  // Turn off the fans 
    P1 = (P1 & 0xEF);
	#else
  	EXTMem_Read(P1_ADD_OHDB,&Item );
  	EXTMem_Write(P1_ADD_OHDB, Item & 0xEF );
  	#endif
	}
}

/****************************************************************************
 *
 *  NAME        : MaterialCoolingFansControl 
 *
 *  DESCRIPTION : The control for the material cooling fans                                           
 *
 ****************************************************************************/
void MaterialCoolingFansControl() using 1
{
#ifdef OCB_SIMULATOR
 BYTE xdata Item;
#endif
	
  if (FansStatus)
	{
	  FansStatus = FALSE;

		// Turn off the fans (modify only bit 4 of port 1) 
    P1 = (P1 & 0xEF);
	#ifdef OCB_SIMULATOR
  	EXTMem_Read(P1_ADD_OHDB,&Item );
  	EXTMem_Write(P1_ADD_OHDB, Item & 0xEF );
  	#endif

#ifndef OCB_SIMULATOR
		// Load the timer for the off period
		TL4 = OffPeriodCount & LSB_MASK;
    TH4 = (OffPeriodCount & MSB_MASK) >> 8;
#endif
	}
	else
	{
	  FansStatus = TRUE;

		// Turn on the fans (modify only bit 4 of port 1) 
    P1 = P1 | MATERIAL_COOLING_FAN_PORT_BIT;
	#ifdef OCB_SIMULATOR
  	EXTMem_Read(P1_ADD_OHDB,&Item );
  	EXTMem_Write(P1_ADD_OHDB, Item | MATERIAL_COOLING_FAN_PORT_BIT);
  	#endif
 #ifndef OCB_SIMULATOR
		// Load the timer for the on period
		TL4 = OnPeriodCount & LSB_MASK;
    TH4 = (OnPeriodCount & MSB_MASK) >> 8;
 #endif
	}

}
