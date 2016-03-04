/*===========================================================================
 *   FILENAME       : Potentiometer Interface  {Potentiometer.H}  
 *   PURPOSE        : Potentiometer interface header file
 *   DATE CREATED   : 17/Oct/2001
 *   PROGRAMMER     : Nir Saadon 
 *   Include        :    
 *===========================================================================*/

#ifndef _POTENTIOMETER_H_
#define _POTENTIOMETER_H_

#include "Define.h"


// Potentiometer Errors
// --------------------
typedef enum {
	POTENMTR_NO_ERROR = 0,
	POTENMTR_SEND_SUCCESS,
	POTENMTR_SEND_FAILED,
	POTENMTR_BUSY
	}POTENMTR_STATUS;


// Function Prototype 
// ====================

// write a new value to the poteniometer device
// --------------------------------------------
POTENMTR_STATUS PotenmtrWriteValue(BYTE Value);

// initialization of the poteniometer interface
// --------------------------------------------
void PotenmtrInit();

// callback function for the SPI ISR
// ---------------------------------
void PotenmtrCallBack(BYTE Status);

// Returns the status of the last write operation
// ----------------------------------------------
POTENMTR_STATUS PotenmtrGetWriteStatus() ;


#endif	

