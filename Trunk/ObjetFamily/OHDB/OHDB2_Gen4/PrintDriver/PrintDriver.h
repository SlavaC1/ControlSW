/*===========================================================================
 *   FILENAME       : Print Driver {PrintDriver.h}  
 *   PURPOSE        : Print driver header file  
 *   DATE CREATED   : 10/Jan/2002
 *   PROGRAMMER     : Nir Saadon 
 *===========================================================================*/

#ifndef _PRINT_DRIVER_H_
#define _PRINT_DRIVER_H_

#include "Define.h"
#include "HeadData.h"

// FPGA registers

#define FPGA_REG_START_PEG			   0x01
#define FPGA_REG_END_PEG  			   0x02
#define FPGA_REG_ACTUAL_POSITION       0x01
#define FPGA_REG_NUMBER_OF_FIRES       0x40
#define FPGA_REG_GO 				   0x03
#define FPGA_REG_DIAG				   0x04
#define FPGA_REG_CONFIG                0x05

#define FPGA_REG_BUMPER_START_PEG      0x0C
#define FPGA_REG_BUMPER_END_PEG		   0x0B
#define FPGA_REG_BUMPER_IMPACT 		   0x30
#define FPGA_REG_BUMPER_PEG_ENABLE     0x76

#define FPGA_REG_WR_HEAD_ON            0x31
#define FPGA_REG_SIM_FIRE_FREQ  	   0x35							
#define FPGA_REG_RESET_ALL			   0x10
#define FPGA_REG_RESET_STATE_MACHINE   0x11
#define FPGA_REG_SPARE_COMM_DIRECTION  0x70

#define FPGA_REG_DATA_MULTIPLIER       0x20
#define FPGA_REG_RESOLUTION_DIVIDER    0x21

// Registers used for fire all sequence
#define FPGA_REG_FIRES_ON_H  0x37
#define FPGA_REG_FIRES_ON_L  0x38
#define FPGA_REG_FIRES_ON_DUTY_CYCLE_H 0x39
#define FPGA_REG_FIRES_ON_DUTY_CYCLE_L 0x3A
#define FPGA_REG_FIRES_NUM_OF_CYCLES   0x3B
#define FPGA_REG_MAIN_FIFO_RESET 0x12


// Pulse registers
// ------------------------------------------------

#define FPGA_REG_DATA_SOURCE     0x2B // FF_WR_SEL
#define FPGA_REG_PULSE_OP_MODE   0x1D // WR_PLSR_MODE: 0 - single, 1 - double

// Fall time for first pulse
#define FPGA_REG_PULSE1_FT_HEAD1 0xB0
#define FPGA_REG_PULSE1_FT_HEAD2 0xB1
#define FPGA_REG_PULSE1_FT_HEAD3 0xB2
#define FPGA_REG_PULSE1_FT_HEAD4 0xB3
#define FPGA_REG_PULSE1_FT_HEAD5 0xB4
#define FPGA_REG_PULSE1_FT_HEAD6 0xB5
#define FPGA_REG_PULSE1_FT_HEAD7 0xB6
#define FPGA_REG_PULSE1_FT_HEAD8 0xB7

// Pulse width for second pulse
#define FPGA_REG_PULSE2_WIDTH_HEAD1 0xB8
#define FPGA_REG_PULSE2_WIDTH_HEAD2 0xB9
#define FPGA_REG_PULSE2_WIDTH_HEAD3 0xBA
#define FPGA_REG_PULSE2_WIDTH_HEAD4 0xBB
#define FPGA_REG_PULSE2_WIDTH_HEAD5 0xBC
#define FPGA_REG_PULSE2_WIDTH_HEAD6 0xBD
#define FPGA_REG_PULSE2_WIDTH_HEAD7 0xBE
#define FPGA_REG_PULSE2_WIDTH_HEAD8 0xBF

// Dwell time for first pulse
#define FPGA_REG_PULSE1_DWELL_HEAD1 0xC0
#define FPGA_REG_PULSE1_DWELL_HEAD2 0xC1
#define FPGA_REG_PULSE1_DWELL_HEAD3 0xC2
#define FPGA_REG_PULSE1_DWELL_HEAD4 0xC3
#define FPGA_REG_PULSE1_DWELL_HEAD5 0xC4
#define FPGA_REG_PULSE1_DWELL_HEAD6 0xC5
#define FPGA_REG_PULSE1_DWELL_HEAD7 0xC6
#define FPGA_REG_PULSE1_DWELL_HEAD8 0xC7

// Delay between first and second pulses
#define FPGA_REG_DELAY_HEAD1 0xD0
#define FPGA_REG_DELAY_HEAD2 0xD1
#define FPGA_REG_DELAY_HEAD3 0xD2
#define FPGA_REG_DELAY_HEAD4 0xD3
#define FPGA_REG_DELAY_HEAD5 0xD4
#define FPGA_REG_DELAY_HEAD6 0xD5
#define FPGA_REG_DELAY_HEAD7 0xD6
#define FPGA_REG_DELAY_HEAD8 0xD7

// ------------------------------------------------ 


typedef enum 
{
	PRINT_DRV_NO_ERROR = 0,
	PRINT_DRV_OP_NOT_DONE,
	PRINT_DRV_OP_FAIL
}PRINT_DRV_STATUS;


// Function Prototype 
// ====================

// Init the xilinx registers and state machine
// -------------------------------------------							
void PrintDrv_Init();

// Write data to FPGA register
PRINT_DRV_STATUS PrintDrv_WriteToRegister(BYTE Address, WORD Data);

// Reset the print driver 
PRINT_DRV_STATUS PrintDrv_ResetDriver();

// Reset the print driver state machine  
PRINT_DRV_STATUS PrintDrv_ResetStateMachine();

// Set the 'start peg' register value
PRINT_DRV_STATUS PrintDrv_SetStartPeg(WORD StartPeg);

// Set the 'end peg' register value
PRINT_DRV_STATUS PrintDrv_SetEndPeg(WORD EndPeg);

// Set the 'number of fires' register value
PRINT_DRV_STATUS PrintDrv_SetNumOfFires(DWORD NumOfFires);

// Get the printing head actual position
PRINT_DRV_STATUS PrintDrv_GetHeadPosition();

// Enable the printing driver (Go command)
PRINT_DRV_STATUS PrintDrv_EnablePrintingCircuitry();

// Disble the printing driver 
PRINT_DRV_STATUS PrintDrv_DisblePrintingCircuitry();

// Set the printing resulotion
PRINT_DRV_STATUS PrintDrv_SetResulotion(BYTE Resulotion);

// Set the bumper end peg
PRINT_DRV_STATUS PrintDrv_SetBumperEndPeg(WORD EndPeg);

// Set the bumper start peg
PRINT_DRV_STATUS PrintDrv_SetBumperStartPeg(WORD StartPeg);

// Get the bumper impact status
PRINT_DRV_STATUS PrintDrv_GetBumperImpact();

// Set the diagnostics register
PRINT_DRV_STATUS PrintDrv_SetDiagnostics(WORD RegValue);

// Set the communication direction register
PRINT_DRV_STATUS PrintDrv_SetCommDirection(BYTE Direction);

// Is the 'Set' operation done (for all the 'set' operations)?
PRINT_DRV_STATUS PrintDrv_IsSetOperationDone();

// Is the 'Get'operation done?
PRINT_DRV_STATUS PrintDrv_IsGetOperationDone(WORD *Data);

// Set the parameters for the fire all task                                         
void PrintDrv_SetFireAllParams(WORD Frequency, WORD Time, WORD Head, DWORD NumOfFires,BYTE DestId, BYTE TransactionId);

// This task performs the fire all operation. first it sends ACK
// to EDEN, then it set all the XILINIX registers required for the 
// fire all operation, then it waits for the fire all time, sets
// the xilinx reghisters for terminating the fire all and send 
// notification to EDEN
// --------------------
void PrintDrv_FireAllTask(BYTE Arg);

// Returns the current status of the fire all task
BYTE PrintDrv_GetFireAllStatus();

// Returns the current counter of the fire all task
BYTE PrintDrv_GetFireAllCounter();

// Set the voltage of a printing head by setting its potentiometer value
PRINT_DRV_STATUS PrintDrv_SetHeadVoltage(BYTE HeadNum, BYTE PotentiometerValue);

// Is the 'Set head voltage' action done?                                          
PRINT_DRV_STATUS PrintDrv_IsSetVoltageDone(BYTE HeadNum);

// Enable\disable the bumper PEG mechanism
PRINT_DRV_STATUS PrintDrv_EnableDisableBumperPeg(BOOL Enable);



#endif	

