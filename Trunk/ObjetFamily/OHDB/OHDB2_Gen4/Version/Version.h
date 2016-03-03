

#ifndef _VERSION_H_
#define _VERSION_H_


#define EXTERNAL_SOFTWARE_VERSION 12
#define INTERNAL_SOFTWARE_VERSION 52

/*
Version 12.52
--------------
1) Omitting counter for counting the critical rate errors and then sending it to the embedded (so there won't be unnecessary tasks and messages in the OHDB).

Version 12.51
--------------
1) Adding counter for counting the critical rate errors and then sending it to the embedded.
2) Increasing number of tasks in OHDB.

Version 12.50
--------------
1) Adding counting mechanism for critical rate error (due to unwanted WD during cleaning head swizard when cleaning the heads with Ethanol) - BUG 3877.

Version 12.49
--------------
1) Maximum temperatures (minimum A2D) for the various heaters - BUG 4110.

Version 12.48
--------------
1) New DM6 mode 

Version 12.47
--------------
1) Bug 3988 - Changed the HeatersActivation task of HeatersControl from self-suspending and resumed by others, to always-running.

Version 12.46
--------------
SW Reset in case of stuck scheduler
PLEASE NOTE: remark the lines containing "WDTCN" if you are debugging the OHDB, otherwise it will reset itself

Version 12.45
--------------
SW Reset in case of communication loss

Version 12.44
--------------
1) Bug 3316 - separate critical rates for heating and cooling in the heater watchdog

Version 12.43
--------------
1) Temporary revert  "Fixing bug at block heaters calculation average"

Version 12.42
--------------
1) DM6 temporary (?) changes

Version 12.41
--------------
1) Fixing bug at block heaters calculation average

Version 12.40 (Shahar)
--------------
1) Added a delay at boot before launching HeaterWatchdogTask to allow temperature to stabilize before starting to sample.

Version 12.39
--------------
1) Added driver FIFO reset message (0x92)

Version 12.38
--------------
1) Changing heater algorithm: "fixating" the time interval between two steps, instead of the a2d diff.

Version 12.37
--------------
1) Bug fixing

Version 12.36
--------------
1) Adding the heaters watchdog.

Version 12.35
--------------
1) Added Tacho Fans feature.

Version 12.34
--------------
1) FireAll with Head mask.
Adding 0x39,0x3A,0x3B registers to FireAll Seq.

Version 12.33
--------------
1) Adding interface between EM and FPGA register 0x50, for thermistor group (de)activation according to operation mode

Version 12.32
--------------
1) Removing 2 sequential activations prevention from HeaterControlSetOnOff, which allows us to reset the heating sequence 
2) THeadsStandbyTemperatureStatusMsg was set with incorrect message ID

Version 12.31
--------------
1) Separate threshold for heads and block temperature

Version 12.30
--------------
1) Fixing heating to lower temperature

Version 12.29
--------------
1) Adding FPGA reset at startup

Version 12.28
--------------
1) Head heaters mask was added to support head optimization wizard.

Version 12.27
--------------
1) Implementing support for OHDB2 Rev C and Head Drive Rev B. Using OHDB2_REV_C define, which must be removed when OHDB2 Rev B is phased out.
2) Switching channels for upper filling thermistors.
3) Crossing AD heaters readings between head halves.

Version 12.26
--------------
1) Implementing heating control mechanism differently. Setting "next set value" independently from current measured value.

Version 12.25
--------------
1) Fixing ports selection for AD2

Version 12.24
--------------
1) Fixing AD reading after power off

Version 12.23
--------------
1) Getting the software version of external AD CPU differently

Version 12.22
--------------
1) Correcting conversion formula from head ADU to block ADU

Version 12.21
--------------
1) Switch sensors M5-6<->M1-2

Version 12.20
--------------
1) Implementing heating rate control in addition to existing mechanism

Version 12.19
--------------
1) Activating the upper thermistors A2D functionality

Version 12.18
--------------
1) Configuring for OHDB2 Rev B

Version 12.17
--------------
1) Selecting RS232 as data path in FireAll sequence
2) Resetting FPGA main FIFO at ResetDriverCircuit sequence

Version 12.16
--------------
1) Adding DataMultiplier and ResolutionDivider to print logic

Version 12.15
--------------
1) Implementing PrintDriver logic and FPGA registers loading for Gen4 heads
2) Moving the reading of head voltages to HeadVoltageReader module


Version 12.14
--------------
1) Implementing higher block thermistors A2D readings with additional, external AD chip Linear LTC1863 


Version 12.13
--------------
1) Crossing the temperature AD reading of two halves of the same head, due to a hardware limitation


Version 12.12
--------------
1) Redesigning heating control mechanism. Head heaters temperatures are following perimeter heaters values until set point is reached


Version 12.11
--------------
1) Adding Pre-Heater to heaters control


Version 12.10
--------------
1) This version is for OHDB2 card with Gen4 heads
2) Support for 12 block filling thermistors
3) Implementing HeadsVoltageReader mechanism, to switch between AD readings using FPGA mux
4) Adding to TPowerSuppliesVoltagesMsg message 3.3V and 1.2V power supply readings
5) Selecting different potentiometer address value for each half of Gen4 head
6) Reading voltages only from even heads for Gen4 head drives. Switching is done with mux. 

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
