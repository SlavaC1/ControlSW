/*===========================================================================
 *   FILENAME       : ExtMemAddSim {ExtMemAddSim.h}  
 *   PURPOSE        : This file contains the external address in simulation mode only
 *   DATE CREATED   : 29/01/2012
 *   PROGRAMMER     : Luda Margolis
 *   Description    : ExtMemAddSim H File 
 *   Include        : 
 *===========================================================================*/
#ifndef _EXT_MEM_ADD_SIM_H_
#define _EXT_MEM_ADD_SIM_H_

/*Next available address: 0x116*/

//sensors OCB
#define  P0_ADD	 							0x1A
#define  P1_ADD	  							0x1B
#define  P2_ADD	  							0x1C
#define  P3_ADD	  							0x1D

//actuators OCB
#define  P4_ADD	  							0x2A
#define  P5_ADD	  							0x2B
#define  P6_ADD	  							0x2C
#define  P7_ADD	  							0x2D

//actuators OHDB
#define  P1_ADD_OHDB	  					0x3A

//sensors OHDB
//#define  P1_ADD_OHDB_SENS	  				0x1B
#define  P2_ADD_OHDB/*_SENS*/	  				0x4A
#define  P3_ADD_OHDB/*_SENS*/	  				0x4B


//IIC - actuators
#define  FIRSTMSCCARD_ADD					0x5A
#define  SECONDMSCCARD_ADD		    		0x5B

//IIC - sensors
#define  FIRSTMSCCARD_SENS_ADD				0x6A
#define  SECONDMSCCARD_SENS_ADD		    	0x6B
#define  THIRDMSCCARD_SENS_ADD              0x112
#define  FOURTHMSCCARD_SENS_ADD			    0x113

//IIC - sensors	Load cells -
#define  FIRSTMSCCARD_LOAD_CELL1_ADD		0x83	 //in triplex: MODEL_1_WEIGHT  and 0x84
#define  FIRSTMSCCARD_LOAD_CELL2_ADD		0x85	 //in triplex: MODEL_2_WEIGHT  and 0x86
#define  FIRSTMSCCARD_LOAD_CELL3_ADD		0x87	//in triplex: SUPPORT_1_WEIGHT and 0x88
#define  FIRSTMSCCARD_LOAD_CELL4_ADD		0x81	//in triplex: SUPPORT_2_WEIGHT and 0x82

#define  SECONDMSCCARD_LOAD_CELL2_ADD	    0x75 	 //in triplex: MODEL_4_WEIGHT  and 0x76
#define  SECONDMSCCARD_LOAD_CELL1_ADD	    0x73     //in triplex: MODEL_3_WEIGHT  and 0x74
#define  SECONDMSCCARD_LOAD_CELL3_ADD	    0x77    //in triplex: MODEL_5_WEIGHT   and 0x78
#define  SECONDMSCCARD_LOAD_CELL4_ADD	    0x71 	//in triplex: MODEL_6_WEIGHT   and 0x72

#ifdef 	SIM_GEN4

#define  THIRDMSCCARD_LOAD_CELL1_ADD		0xFA	 //
#define  THIRDMSCCARD_LOAD_CELL2_ADD		0xFC	 //
#define  THIRDMSCCARD_LOAD_CELL3_ADD		0xFE	 //
#define  THIRDMSCCARD_LOAD_CELL4_ADD		0x100	 //

#define  FOURTHMSCCARD_LOAD_CELL1_ADD		0x102	 //
#define  FOURTHMSCCARD_LOAD_CELL2_ADD		0x104	 //
#define  FOURTHMSCCARD_LOAD_CELL3_ADD		0x106	 //
#define  FOURTHMSCCARD_LOAD_CELL4_ADD		0x108	 //
#endif

//SPI	-OCB
#define  ANALOG_IN_8_ADD 					0x61       //WASTE_WEIGHT	and 
#define  ANALOG_IN_20_ADD					0x63
#define  ANALOG_IN_9_ADD 					0x65       //tray heater
#define  ANALOG_IN_21_ADD					0x67	   //Evacuation (AirFlow) Indicator

//SPI OHDB
#define  ANALOG_IN_1_ADD_OHDB					0x91 //heaters
#define  ANALOG_IN_2_ADD_OHDB					0x93
#define  ANALOG_IN_3_ADD_OHDB					0x95
#define  ANALOG_IN_4_ADD_OHDB					0x97
#define  ANALOG_IN_5_ADD_OHDB					0x99
#define  ANALOG_IN_6_ADD_OHDB					0x9B
#define  ANALOG_IN_7_ADD_OHDB					0x9D
#define  ANALOG_IN_8_ADD_OHDB					0x9F  
#define  ANALOG_IN_22_ADD_OHDB				0xB3 //for pre heater - EXTERNAL_LIQUID_TEMP

#ifdef 	SIM_GEN4
#define  ANALOG_IN_9_ADD_OHDB					0x10
#define  ANALOG_IN_10_ADD_OHDB					0x12
#define  ANALOG_IN_11_ADD_OHDB					0x14
#define  ANALOG_IN_12_ADD_OHDB					0x16
#else
#define  ANALOG_IN_9_ADD_OHDB					0xA1
#define  ANALOG_IN_10_ADD_OHDB					0xA3
#define  ANALOG_IN_11_ADD_OHDB					0xA5
#define  ANALOG_IN_12_ADD_OHDB					0xA7
#endif

#define  ANALOG_IN_20_ADD_OHDB					0xB1 //Support thermistors
#define  ANALOG_IN_24_ADD_OHDB					0xB5 //?not in use?
#define  ANALOG_IN_18_ADD_OHDB                  0xB9 // M6 thermistors!! (before vacuum)

#define  ANALOG_IN_26_ADD_OHDB				    0x10A// not in use, in simulator for upper thermistors
#define  ANALOG_IN_27_ADD_OHDB				    0x10C// not in use, in simulator for upper thermistors
#define  ANALOG_IN_28_ADD_OHDB				    0x10E// not in use, in simulator for upper thermistors
#define  ANALOG_IN_29_ADD_OHDB				    0x110// not in use, in simulator for upper thermistors
					
/*heaters requested temp*/

#define  HEATER_1_REQ_DUMMY_ADD_OHDB		  	0xC1
#define  HEATER_2_REQ_DUMMY_ADD_OHDB		  	0xC3
#define  HEATER_3_REQ_DUMMY_ADD_OHDB		  	0xC5
#define  HEATER_4_REQ_DUMMY_ADD_OHDB		  	0xC7
#define  HEATER_5_REQ_DUMMY_ADD_OHDB		  	0xC9
#define  HEATER_6_REQ_DUMMY_ADD_OHDB		  	0xCB
#define  HEATER_7_REQ_DUMMY_ADD_OHDB		  	0xCD
#define  HEATER_8_REQ_DUMMY_ADD_OHDB		  	0xCF
#define  HEATER_9_REQ_DUMMY_ADD_OHDB		  	0xD1
#define  HEATER_10_REQ_DUMMY_ADD_OHDB		  	0xD3
#define  HEATER_11_REQ_DUMMY_ADD_OHDB		  	0xD5
#define  HEATER_12_REQ_DUMMY_ADD_OHDB		  	0xD7
#define  HEATER_13_REQ_DUMMY_ADD_OHDB		  	0xD9

#ifdef SIM_GEN4
#define  ANALOG_IN_13_ADD_OHDB					0x18 //M1 thermistors
#define  ANALOG_IN_14_ADD_OHDB					0x20 //M2 thermistors
#define  ANALOG_IN_15_ADD_OHDB					0x22 //M3 thermistors
#define  ANALOG_IN_16_ADD_OHDB					0x24 //M4 thermistors
#else
#define  ANALOG_IN_13_ADD_OHDB					0xA9 //thermistors
#define  ANALOG_IN_14_ADD_OHDB					0xAB //thermistors
#define  ANALOG_IN_15_ADD_OHDB					0xAD //thermistors
#define  ANALOG_IN_16_ADD_OHDB					0xAF //thermistors
#endif



#define  ANALOG_IN_17_ADD_OHDB					0x32 //M5 thermistors
#define  ANALOG_IN_19_ADD_OHDB					0x30 //M7 thermistors

#define  ANALOG_IN_21_ADD_OHDB					0xF4 //Vacuum
#define  ANALOG_IN_23_ADD_OHDB					0xF6 //Voltages - OHDB VDD

#define  AMBIENT_TEMP_ADD_OHDB					0xF8 //Ambient Temp

//SPI Drv
#define  XILINX_ADD_OHDB						0xB7 //Xilinx read data
#define  XILINX_PRINTING_ADD_OHDB				0xBB //Xilinx read Data, Printing bit

//A2D 
#define  VS_SENSOR_ADD					0xBC //voltages	
#define  VCC_SENSOR_ADD					0xBE //voltages
#define  V_12_SENSOR_ADD				0xE0 //voltages	
#define  V_24_SENSOR_ADD				0xE2 //voltages	

//MSC voltages
#define  FIRSTMSCCARD_24V_STATUS_ADD	0xE4
#define  FIRSTMSCCARD_7V_STATUS_ADD	   	0xE6
#define  FIRSTMSCCARD_5V_STATUS_ADD		0xE8
#define  FIRSTMSCCARD_3_3V_STATUS_ADD   0xEA

#define  SECONDMSCCARD_24V_STATUS_ADD	0xEC
#define  SECONDMSCCARD_7V_STATUS_ADD   	0xEE
#define  SECONDMSCCARD_5V_STATUS_ADD	0xF0
#define  SECONDMSCCARD_3_3V_STATUS_ADD	0xF2

#define  CHIP_SELECT_MSC_1_ADD  		0xA4 // First MSC card address  
#define  CHIP_SELECT_MSC_2_ADD  		0xAA // Second MSC card address
#define  CHIP_SELECT_MSC_3_ADD  		0xAC // Third MSC card address  
#define  CHIP_SELECT_MSC_4_ADD  		0xA6 // Fourth MSC card address 

#define  FORCE_SLEEP_ADD                0x114
		
#endif