/*===========================================================================
 *   FILENAME       : Print Driver {PrintDriver.h}  
 *   PURPOSE        : Print driver header file  
 *   DATE CREATED   : 10/Jan/2002
 *   PROGRAMMER     : Nir Saadon 
 *===========================================================================*/

#ifndef _PRINT_DRIVER_H_
#define _PRINT_DRIVER_H_

#include "Define.h"


typedef enum {
	PRINT_DRV_NO_ERROR = 0,
	PRINT_DRV_OP_NOT_DONE,
	PRINT_DRV_OP_FAIL
	}PRINT_DRV_STATUS;


// Function Prototype 
// ====================

// Init the xilinx registers and state machine
// -------------------------------------------							
void PrintDrv_Init();

// Reset the print driver 
// ----------------------
PRINT_DRV_STATUS PrintDrv_ResetDriver();

// Reset the print driver state machine  
// ------------------------------------
PRINT_DRV_STATUS PrintDrv_ResetStateMachine();

// Set the 'start peg' register value
// ----------------------------------
PRINT_DRV_STATUS PrintDrv_SetStartPeg(WORD StartPeg);

// Set the 'end peg' register value
// --------------------------------
PRINT_DRV_STATUS PrintDrv_SetEndPeg(WORD EndPeg);

// Set the 'number of fires' register value
// ----------------------------------------
PRINT_DRV_STATUS PrintDrv_SetNumOfFires(WORD NumOfFires);

// Get the printing head actual position
// -------------------------------------
PRINT_DRV_STATUS PrintDrv_GetHeadPosition();

// Enable the printing driver (Go command)
// ---------------------------------------
PRINT_DRV_STATUS PrintDrv_EnablePrintingCircuitry();

// Disble the printing driver 
// --------------------------
PRINT_DRV_STATUS PrintDrv_DisblePrintingCircuitry();

// Set the printing resulotion
// ---------------------------
PRINT_DRV_STATUS PrintDrv_SetResulotion(BYTE Resulotion);

// Set the polarity
// ----------------
PRINT_DRV_STATUS PrintDrv_SetPolarity();

// Set the pulse width and the pulse delay
// ---------------------------------------
PRINT_DRV_STATUS PrintDrv_SetPulseWidthAndDelay(BYTE Width, BYTE Delay);
// Set the pulse width and the pulse delay for new heads
// -------------------------------------------------------
PRINT_DRV_STATUS PrintDrv_SetPulseWidthAndDelay2(BYTE Width, BYTE Delay);

// Set the print direction
// -----------------------
PRINT_DRV_STATUS PrintDrv_SetPrintDirection(BYTE Direction);

// Set a head delay
// ----------------
PRINT_DRV_STATUS PrintDrv_SetHeadDelay(BYTE HeadNumOption/*new head or old head*/, BYTE Delay);

// Set the bumper end peg
// ----------------------
PRINT_DRV_STATUS PrintDrv_SetBumperEndPeg(WORD EndPeg);

// Set the bumper start peg
// ----------------------
PRINT_DRV_STATUS PrintDrv_SetBumperStartPeg(WORD StartPeg);

// Get the bumper impact status
// ----------------------------
PRINT_DRV_STATUS PrintDrv_GetBumperImpact();


// Set the pre pulser delay register                                         
// ---------------------------------
PRINT_DRV_STATUS PrintDrv_SetPrePulserDelay(WORD Delay);

// Set the post pulser delay register                                         
// ---------------------------------
PRINT_DRV_STATUS PrintDrv_SetPostPulserDelay(WORD Delay);

// Set the post pulser delay register for new head                                        
// --------------------------------------------------
PRINT_DRV_STATUS PrintDrv_SetPostPulserDelay2(WORD Delay);

//  Set the head table: '0' represent old head, '1' represent new head                                      
// --------------------------------------------------
PRINT_DRV_STATUS PrintDrv_HeadDriveSelectTable(WORD HeadTable);

// Set the diagnostics register
// ----------------------------
PRINT_DRV_STATUS PrintDrv_SetDiagnostics(WORD RegValue);

// Set the communication direction register
// ----------------------------------------
PRINT_DRV_STATUS PrintDrv_SetCommDirection(BYTE Direction);

// Set the control register
// ----------------------------
PRINT_DRV_STATUS PrintDrv_SetControl(BYTE RegValue);

// Is the 'Set' operation done (for all the 'set' operations)?
// ---------------------------------------------------
PRINT_DRV_STATUS PrintDrv_IsSetOperationDone();

// Is the 'Get'operation done?
// ---------------------------
PRINT_DRV_STATUS PrintDrv_IsGetOperationDone(WORD *Data);

// Set the parameters for the fire all task                                         
// ----------------------------------------
void PrintDrv_SetFireAllParams(WORD Frequency, WORD Time, WORD Head, WORD NumOfFires,BYTE DestId, BYTE TransactionId);

// This task performs the fire all operation. first it sends ACK
// to EDEN, then it set all the XILINIX registers required for the 
// fire all operation, then it waits for the fire all time, sets
// the xilinx reghisters for terminating the fire all and send 
// notification to EDEN
// --------------------
void PrintDrv_FireAllTask(BYTE Arg);

// Returns the current status of the fire all task
// -----------------------------------------------
BYTE PrintDrv_GetFireAllStatus();

// Returns the current counter of the fire all task
// ------------------------------------------------
BYTE PrintDrv_GetFireAllCounter();

// Set the voltage of a printing head by setting its potentiometer value
// ---------------------------------------------------------------------
PRINT_DRV_STATUS PrintDrv_SetHeadVoltage(BYTE HeadNum, BYTE PotentiometerValue);

// Is the 'Set head voltage' action done?                                          
// --------------------------------------
PRINT_DRV_STATUS PrintDrv_IsSetVoltageDone(BYTE HeadNum);

// Get the current printing heads voltages
// ---------------------------------------
void PrintDrv_GetHeadsVoltages(WORD* Voltages);

// Enable\disable the bumper PEG mechanism
PRINT_DRV_STATUS PrintDrv_EnableDisableBumperPeg(BOOL Enable);



#endif	

