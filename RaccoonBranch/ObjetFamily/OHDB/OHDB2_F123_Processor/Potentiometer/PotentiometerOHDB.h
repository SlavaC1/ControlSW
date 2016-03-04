/*===========================================================================
 *   FILENAME       : Potentiometer Interface  {Potentiometer.H}  
 *   PURPOSE        : Potentiometer interface header file
 *   DATE CREATED   : 17/Oct/2001
 *   PROGRAMMER     : Nir Saadon 
 *   Include        :    
 *===========================================================================*/

#ifndef _POTENTIOMETER_OHDB_H_
#define _POTENTIOMETER_OHDB_H_

#include "Define.h"


// Potentiometer Errors
// --------------------
typedef enum {
	OHDB_POTENMTR_NO_ERROR = 0,
	OHDB_POTENMTR_SEND_SUCCESS,
	OHDB_POTENMTR_SEND_FAILED,
	OHDB_POTENMTR_BUSY
	}OHDB_POTENMTR_STATUS;


// Function Prototype 
// ====================

// write a new value to the poteniometer device
// --------------------------------------------
OHDB_POTENMTR_STATUS OHDBPotenmtrWriteValue(BYTE DeviceNum, BYTE Value);

// initialization of the poteniometer interface
// --------------------------------------------
void OHDBPotenmtrInit();

// callback function for the SPI ISR
// ---------------------------------
void OHDBPotenmtrCallBack(BYTE Status);

// Returns the status of the last write operation
// ----------------------------------------------
OHDB_POTENMTR_STATUS OHDBPotenmtrGetWriteStatus(BYTE DeviceNum) ;


#endif	

