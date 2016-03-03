/*===========================================================================
 *   FILENAME       : LoopBackTest  {LoopBackTest.H}  
 *   PURPOSE        : LoopBackTest header file
 *   DATE CREATED   : 13/Jan/2002
 *   PROGRAMMER     : Nir Saadon 
 *   Include        :    
 *===========================================================================*/

#ifndef _LOOPBACK_TEST_H_
#define _LOOPBACK_TEST_H_

#include "Define.h"




// Function Prototype 
// ====================
BOOL LoopBackTestInit();

BOOL IsLoopBackTestInitDone();

void LoopBackTestTask(BYTE Arg);


#endif	

