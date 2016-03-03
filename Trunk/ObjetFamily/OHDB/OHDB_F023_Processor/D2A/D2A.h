/*===========================================================================
 *   FILENAME       : D2A {D2A.h}  
 *   PURPOSE        : D/A driver (header file) 
 *   DATE CREATED   : 28/7/2002
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/
#ifndef _D2A_H_
#define _D2A_H_

#include "Define.h"

// Initialization of the D/A (0) driver
void D2A0_Init();

// Set the output of the D2A (0)
void D2A0_Write(WORD NewValue);

// Initialization of the D/A (1) driver
void D2A1_Init();

// Set the output of the D2A (1)
void D2A1_Write(WORD NewValue);

#endif
