/*===========================================================================
 *   FILENAME       : Utils {Utils.c}  
 *   PURPOSE        : Some useful utilities 
 *   DATE CREATED   : 21/Aug/2002
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/

#include "Utils.h"



/****************************************************************************
 *
 *  NAME        : Utils_CalculateCheckSum 
 *
 *  DESCRIPTION : Calculates a word check sum of a data buffer                                           
 *
 ****************************************************************************/
WORD Utils_CalculateCheckSum(BYTE *Data, BYTE Length)
{
	BYTE xdata i;
	WORD xdata CheckSum = 0;

	for (i = 0; i < Length; i++)
		CheckSum += Data[i];
 
	return CheckSum;
}
