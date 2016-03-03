

#ifndef _VERSION_H_
#define _VERSION_H_

#define EXTERNAL_SOFTWARE_VERSION			31
#define INTERNAL_SOFTWARE_VERSION			11
/*
ver 31.11
---------------------	
1. Added NumOfSaturatedReadings	- readings above pre-defined threshold (UV_SAMPLES_SATURATED_THRESHOLD)
2. Added NumOfActualReadings - readings	in a pre-defined range
3. Added SaturationFlag - True if (NumOfSaturatedReadings > UV_NUM_OF_SATURATED_SAMPLES_THRESHOLD)
4. Removed Internal UV mechanism - not used in proffesional							  
5. Added the ability to change A2D Task Delay at runtime using SET_UV_SAMPLING_PARAMS_EX

ver 31.10
---------------------								  
1. MSC2

ver 31.09
---------------------								  
1. changing GPIO 1.7 init from output to input for Keshet

ver 31.08
---------------------								  
1. Fixing second delay during Power OFF

/*ver 31.07
---------------------								  
1. Adding 1 second delay between turning ON main 24V PS and heaters 24V to prevent heaters latchup

/*ver 31.06
---------------------								  
1. Removing power up delays

/*ver 31.05
---------------------								  
1. Fixing MSC voltages readings
2. Removing duplicate task installation

/*ver 31.04
---------------------								  
1. Changing Liquid Tanks sample rate to 230 for Objet500.For Objet1000, the same old code.

/*ver 31.03
---------------------								  
1. Changing Liquid Tanks sample rate to 230. Same way it was in Triplex OCB. Previous sample rate caused tank weight to be updated too slowly.

/*ver 31.02
---------------------								  
1. Fixing the garbage value for pumps5-6 - made by Iddan

/*ver 31.01
---------------------								  
1. Adding 3 seconds delay between power supplies activations

/*ver 31.00
---------------------								  
1. Separating MSC activation from I2C driver into two modules: I2C_DRV and MSCInterface
2. Rafactoring MSC interface, making it more modular, allowing further expantion
3. Starting to implement 16 pumps activation with 4 MSC cards for Gen4 printing block
4. Removing drain pumps logic
5. Fixing compiler warnings
*/

/*ver 30.32
---------------------								  
1. Outgoing messages to EDEN_ID are discarded if communication is lost for 
   RESPONSIVENESS_COMMUNICATION_LOSS_TIMEOUT (10 seconds) or more.
   The timer is checked as part of the OCB's watchdog task and is separate 
   from the COMMUNICATION_LOSS_TIMEOUT definition.
*/

/*ver 30.31
---------------------								  
1. Added handling of jitter movement when recognizing immobility
*/

/*ver 30.30
---------------------								  
1. Added normally open interlock feedback for service key.
*/

/*ver 30.29
---------------------
1. ISRKeepAliveCheck changed to 300 cycles. 
2. I2C_DRV - Commands to I2C (i.e. turn on/off pumps) were discarded when receiving a nack (SMB_MRADDNACK).
   SMBUS_ISR(), in the fail handling  WriteMSC1Pending was set to false causing a command to be discarded.
3. Erased comments and unused variables.
4. In SMBUS_ISR() - in SMB_MRADDNACK - we can't be sure that the voltage dropped,
   by resetting all the voltages, we give a false report to EM. 
   Now resetting only the 24V (to still indicate a problem to EM).  

ver 30.28
-----------------------
LIQUID_TANKS_TASK_DELAY_TIME       2000

ver 30.27
---------------------								  
1. Added normally open interlock feedback for doors & emergency stop buttons.

ver 30.26
---------------------
1. Added support for Evacuation (AirFlow) indicator

ver 30.25
---------------------
1. remove "SafetySystem.h", remove "SafetySystem.c"

ver 30.24
---------------------
1. Adding support for safety system - two emergency stop buttons 

=======
ver 30.23
---------------------
1. for simulator use, in order to maintain same Minischeduler for both OCB and OHDB added function in OCB
SchedulerGetTaskArg


ver 30.22
---------------------
1. Adding support for safety system - four doors

ver 30.21
---------------------
1. Adding second Ambient Heater actuator and splitting the activation

ver 30.20
---------------------
1. Adding Ambient Heater support.

ver 30.11
---------------------
1. Merge with releases, version 20.96.

ver 30.10
---------------------
1. Adding Signal Tower lights actuators, tasks and blinking logic

ver 30.0
---------------------
1. Changes to support new OCB2 card with Silabs C8051F12x controller 

ver 20.95
---------------------
1. Added variable ISRKeepAliveCheck in I2C module, that checks if SMBUS_ISR is running, if not, send an error message to EM

ver 20.94
---------------------
1. Fixing the "pump stays on during air-valve" bug. (Change in Actuator.c   ActuatorsGetAllActuatorsState())

ver 20.93
---------------------
1. Switch between M1 and S connectors (Pumps, MicroSwitches and LoadCells)

ver 20.92
---------------------
1. Reactivated the immobility detection mechanism ( a.k.a UV watchdog / UV Safety )

ver 20.91
---------------------
1. Added more delay to prevent false error messages at start up - this solves bug: http://bugzilla/bugs/show_bug.cgi?id=8200
2. Cosmetics changes. 
3. Sending Card Number = 0/1 on error messages instead of 0xA4/0xAA

ver 20.9
---------------------
1. Added 'LIMIT_TO_DM2' to project defines. this differentiates between Objet500 and first phase
 of Objet1000. this define will be eliminated from code once we add a message that sets this in runtime 

ver 20.81
---------------------
1. Added handling of jitter movement when recognizing immobility (in UV safety mechanism)
2. in addition - *DISABLED* the sending of EncData MSG to the OHDB + *DISABLED* the UV safety trigger (the EncData msgs trigger a bug in the OHDB)

ver 20.8
---------------------
1. Fixing memory problems in I2C.
2. Memory size reduction. 
3. Removed unnecessary fields in Voltages message.

ver 20.7
---------------------
1. Added support for simulation

ver 20.6
---------------------
1. Added 'Immobility detection mechanism' ( also known as 'UV safety mechanism' or 'UV Watchdog')

ver 20.5
---------------------
1. This version number is obsolete (temporary version for MSC integration that was never used due to stabilization issues).

ver 20.4
---------------------
1. Remove I2C debug Message

ver 20.3
---------------------
1.Fixing the Flooding Bug and changing the state machine of the I2C  Upper level(I2CReadWriteTask()) \\Elad.H  

ver 20.2
--------------------- 
1. Added I2C driver to the OCB project in order to communicate with the MSC Cards 
   regarding Objet & Excel Machines. (Added by Elad)

ver 20.1
---------------------
1. Changed TSetHeadFillingParamsMsg - PrintingOperationMode field to ActiveThermistors[]
2. Legal options for thermistors activation: 
	a. 2 high (S + M) 
	b. 4 low (S + 3 Model)
	c. High and 2 low (S + M1 + M2) or (S + M3 + M1/M2)
3. Removed all warnings (to reduce memory allocation) - uncalled functions
				    
ver 20.0
---------------------
1. Support additional thermistors (HeadsFilling.c)
2. Used arrays in messages which are related to HeadsFilling module.

ver 8.111
---------------------
1. OCB for Objet

ver 8.0
---------------------
1. Added Messages&Logic for Roller Suction System (0x98,0x99,0xF5) (itamar)

ver 7.9
---------------------
1. Fixing an issue of synchronizing the door Notification and the door GetStatus.

ver 7.8
---------------------
absolutely tha same as 0.7, except the version number


ver 0.7 (change this number to 7.8 when passed sanity tests)
------------------------------------------------------------
1. fix the odour fan
2. adsd new UV calibration capabilities

ver 7.0
---------------------
1. This is the initial version of the OCB_3R project. It is a branch from OCB2 ver 1.45. 

ver 7.1
1. 4 model pumps (instead of 2 model pumps and 3 valves) support

ver 7.2
1. Added communication loss watchdog.

ver 7.3
1. Changed the COMMUNICATION_LOSS_TIMEOUT to 1 minute (from 5 secs). This will enable updating field 
   installations that run older versions of Embedded, before the STL fix, that hungs the application.

ver 7.4
1. Added initialization to HeadFilling 'relatedChamber'
2. Added HeadsFillingManagerTask() to allow correct PumpOn PumpOff notification - when switching pumps.

ver 7.5
1. Fixed a bug that caused UV lamps not to turn off in case of Comm Loss watchdog activation.

ver 7.6
1. Changed the Drain Pumps OnOff message according to the standard format.
2. Workaround a HW problem of Hood Fan velocity write-errors (to potentiometer) - by continous writings.

ver 7.7
1. Undo fix of Ver. 7.6: "Workaround a HW problem of Hood Fan velocity write-errors (to potentiometer) - by continous writings."

*/


#endif
