

#ifndef _VERSION_H_
#define _VERSION_H_


#define EXTERNAL_SOFTWARE_VERSION			12
#define INTERNAL_SOFTWARE_VERSION			01

/*

Version 12.01
--------------
1) Crossing between 2 high block thermistor readings


Version 12.00
--------------
1) Version for actual OHDB2 card


Version 11.00
--------------
1) Adding support for C8051F123 controller. Start of OHDB2 (1.5) project - OHDB1 card with C8051F123 controller.


Version 10.41
--------------
1) Added a queue for READ_FROM_XILINX_MSG which receives msgs while handling the current one.    


Version 10.4
--------------
1) Added support for simulation mode


Version 10.3
--------------
1) Removed PreHeater from messages
2) Instead of 3 arrays (HeadsTemp,PerimetersTemp,ExternalLiquidTemp) defined 1 array - HeatersTemp[NUM_OF_HEATERS]  


Version 10.2
--------------
1) FireAll with mask for each head 


Version 10.1
--------------
1) Returned Sensors_GetPowerSuppliesVoltages function for use with Objet adjustments 


Version 10.0 
--------------
1) OBJET version
2) TMaterialLevelSensorsStatusMsg was changed (2 thermistors were added).
3) Sensors_GetMaterialLevelSensors() was updated.


Version 1.15
--------------
1) Changed the COMMUNICATION_LOSS_TIMEOUT to 1 minute (from 5 secs). This will enable updating field 
   installations that run older versions of Embedded, before the STL fix, that hungs the application.


Version 1.14
--------------
1) Awakened the (sleeping) communication watchdog...  (See Bugzilla bug# 1537)

Version 1.13
--------------
1) Corrected problem of Head Temperature error notifications that occured when starting OHDB.
   See bugzilla item: 67

Version 1.12
--------------
1) Added Heaters load balancing. (Delay heads heating) 


*/


#endif
