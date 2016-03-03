/*===========================================================================
 *   FILENAME       : Average.c
 *   PURPOSE        : Average
 *   DATE CREATED   : 25/Jul/2006
 *   PROGRAMMER     : Shahar Behagen
 *===========================================================================*/
#include "Average.h"
#include "Define.h"


BOOL AverageInitialize(AVERAGE_struct *AverageStruct, WORD WindowSize)
{
  AverageStruct->Average = 0;
  AverageStruct->CurrentReadingIndex = 0;
  AverageStruct->WindowSize = WindowSize;
  AverageStruct->Sum = 0;

  return TRUE;
}

BOOL AverageAddReading(AVERAGE_struct *AverageStruct, long Reading)
{
    if (AverageStruct->CurrentReadingIndex < AverageStruct->WindowSize)
	{
		AverageStruct->CurrentReadingIndex++;
		AverageStruct->Sum += Reading;
		AverageStruct->Average = (AverageStruct->Sum) / (AverageStruct->CurrentReadingIndex);
	} else 
	{
		AverageStruct->Sum -= AverageStruct->Average;
		AverageStruct->Sum += Reading;
		AverageStruct->Average = AverageStruct->Sum / AverageStruct->CurrentReadingIndex;
	}	

	return TRUE;
}
#ifdef DEBUG
BOOL AverageGetAverage(AVERAGE_struct *AverageStruct, long *Average)
{
  *Average = AverageStruct->Average;

  return TRUE;
}
#endif
BOOL GlidingAverageInitialize(GLIDING_AVERAGE_struct *GlidingAverageStruct, WORD WindowSize)
{
  unsigned char idata i;

  for(i = 0; i < GLIDING_AVG_ARR_SIZE; i++) {
  	GlidingAverageStruct->Arr[i] = 0;
  }

  GlidingAverageStruct->Average = 0;
  GlidingAverageStruct->CurrentReadingIndex = 0;
  GlidingAverageStruct->Sum = 0;
  GlidingAverageStruct->NumOfElements = 0;

  if( WindowSize > 0 && WindowSize <= GLIDING_AVG_ARR_SIZE ) {
  	GlidingAverageStruct->WindowSize = WindowSize;
  }
  else {
    GlidingAverageStruct->WindowSize = GLIDING_AVG_ARR_SIZE;
  }

  return TRUE;
}

BOOL GlidingAverageAddReading(GLIDING_AVERAGE_struct *AverageStruct, long Reading)
{
    if (AverageStruct->NumOfElements < AverageStruct->WindowSize)
	{
		AverageStruct->Sum += Reading;

		AverageStruct->Arr[ AverageStruct->CurrentReadingIndex ] = Reading;

		AverageStruct->NumOfElements++;
		AverageStruct->CurrentReadingIndex++;
		if( AverageStruct->CurrentReadingIndex == AverageStruct->WindowSize ) {
		  AverageStruct->CurrentReadingIndex = 0;
		}

		AverageStruct->Average = (AverageStruct->Sum) / (AverageStruct->NumOfElements);
	} 
	else 
	{
	    AverageStruct->Sum -= AverageStruct->Arr[ AverageStruct->CurrentReadingIndex ];
		AverageStruct->Sum += Reading;

	    AverageStruct->Arr[ AverageStruct->CurrentReadingIndex ] = Reading;

		AverageStruct->CurrentReadingIndex++;
		if( AverageStruct->CurrentReadingIndex == AverageStruct->WindowSize ) {
		  AverageStruct->CurrentReadingIndex = 0;
		}

		AverageStruct->Average = (AverageStruct->Sum) / (AverageStruct->NumOfElements);
	}	

	return TRUE;
}
#ifdef DEBUG
BOOL GlidingAverageGetAverage(GLIDING_AVERAGE_struct *AverageStruct, long *Average)
{
  *Average = AverageStruct->Average;

  return TRUE;
}
#endif
