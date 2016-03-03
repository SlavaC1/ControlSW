

#ifndef _VERSION_H_
#define _VERSION_H_


#define EXTERNAL_SOFTWARE_VERSION			10
#define INTERNAL_SOFTWARE_VERSION			41


// Version 10.41
/*
1. Added a queue for READ_FROM_XILINX_MSG which receives msgs while handling the current one.    
*/

// Version 10.4
//added support for simulation mode

// Version 10.3
/*
1. Removed PreHeater from messages
2. Instead of 3 arrays (HeadsTemp,PerimetersTemp,ExternalLiquidTemp) defined 1 array - HeatersTemp[NUM_OF_HEATERS]  
*/
// Version 10.2
/* FireAll with mask for each head 
*/
// Version 10.1
/*
1. Returned Sensors_GetPowerSuppliesVoltages function for use with Objet adjustments 
*/  

// Version 10.0 

/*
1. OBJET version
2. TMaterialLevelSensorsStatusMsg was changed (2 thermistors were added).
3. Sensors_GetMaterialLevelSensors() was updated.
*/

// Version 1.15 changeLog
/*
1. Changed the COMMUNICATION_LOSS_TIMEOUT to 1 minute (from 5 secs). This will enable updating field 
   installations that run older versions of Embedded, before the STL fix, that hungs the application.
*/

// Version 1.14 changeLog
/*
1. Awakened the (sleeping) communication watchdog...  (See Bugzilla bug# 1537)
*/

// Version 1.13 changeLog
/* Corrected problem of Head Temperature error notifications that occured when starting OHDB.
See bugzilla item: 67
*/


// Version 1.12 changeLog
/* Added Heaters load balancing. (Delay heads heating) 
*/


#endif
