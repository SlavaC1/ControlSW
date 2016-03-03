/*===========================================================================
 *   FILENAME       : AVERAGE H FILE {AVERAGE.H}  
 *   PURPOSE        : Add generic value Avaraging capability.
 *   DATE CREATED   : 25/Jul/2006
 *   PROGRAMMER     : Shahar Behagen
 *   Description    : Average .h File
 *===========================================================================*/
#ifndef _AVERAGE_H_
#define _AVERAGE_H_

#include "Define.h"


// Structure definitions
// =====================
typedef struct
{
 long	Average;
 long	Sum;
 WORD   WindowSize; 
 WORD   CurrentReadingIndex;
}AVERAGE_struct;

#define GLIDING_AVG_ARR_SIZE  9

typedef struct
{
 long   Arr[GLIDING_AVG_ARR_SIZE];
 long	Average;
 long	Sum;
 WORD   WindowSize; 
 WORD   CurrentReadingIndex;
 short  NumOfElements;
}GLIDING_AVERAGE_struct;

BOOL AverageInitialize(AVERAGE_struct *AverageStruct, WORD WindowSize);
BOOL AverageAddReading(AVERAGE_struct *AverageStruct, long Reading);
#ifdef DEBUG
BOOL AverageGetAverage(AVERAGE_struct *AverageStruct, long *Average);
#endif
BOOL GlidingAverageInitialize(GLIDING_AVERAGE_struct *GlidingAverageStruct, WORD WindowSize);
BOOL GlidingAverageAddReading(GLIDING_AVERAGE_struct *GlidingAverageStruct, long Reading);
#ifdef DEBUG
BOOL GlidingAverageGetAverage(GLIDING_AVERAGE_struct *GlidingAverageStruct, long *Average);
#endif

#endif
