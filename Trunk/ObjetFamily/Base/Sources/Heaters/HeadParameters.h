
#ifndef _HEAD_PARAMETERS_H_
#define _HEAD_PARAMETERS_H_

// Align all structures to byte boundry
#ifdef OS_WINDOWS

// The following pragma is for BCB only
#ifndef __BCPLUSPLUS__
#error Q2RT: This module contain pragma statements for Borland C++ Builder
#endif

#pragma pack(push,1)

#endif

#ifdef OS_VXWORKS
#define STRUCT_ATTRIBUTE __attribute__ ((packed))
#elif defined(OS_WINDOWS)
#define STRUCT_ATTRIBUTE
#endif

#define SERIAL_NUMBER_SIZE 9
typedef char T_SerialNumber[SERIAL_NUMBER_SIZE];

struct THeadParameters
{
  unsigned short A2DValueFor60C;
  unsigned short A2DValueFor80C;
  float Model10KHzLineGain;
  float Model10KHzLineOffset;
  float Model20KHzLineGain;
  float Model20KHzLineOffset;
  float Support10KHzLineGain;
  float Support10KHzLineOffset;
  float Support20KHzLineGain;
  float Support20KHzLineOffset;
//  float ResolutionFactor;
  float XOffset;
  unsigned long ProductionDate;
  T_SerialNumber SerialNumber;
  unsigned short CheckSum;
  unsigned char Validity;

  bool operator == (struct THeadParameters TmpHeadParameters) const
  {
    if (A2DValueFor60C != TmpHeadParameters.A2DValueFor60C)
      return false;

    if (A2DValueFor80C != TmpHeadParameters.A2DValueFor80C)
	  return false;

    if (Model10KHzLineGain != TmpHeadParameters.Model10KHzLineGain)
      return false;

    if (Model10KHzLineOffset != TmpHeadParameters.Model10KHzLineOffset)
      return false;

    if (Model20KHzLineGain != TmpHeadParameters.Model20KHzLineGain)
      return false;

    if (Model20KHzLineOffset != TmpHeadParameters.Model20KHzLineOffset)
      return false;

    if (Support10KHzLineGain != TmpHeadParameters.Support10KHzLineGain)
      return false;

    if (Support10KHzLineOffset != TmpHeadParameters.Support10KHzLineOffset)
      return false;

    if (Support20KHzLineGain != TmpHeadParameters.Support20KHzLineGain)
      return false;

    if (Support20KHzLineOffset != TmpHeadParameters.Support20KHzLineOffset)
      return false;

    if (XOffset != TmpHeadParameters.XOffset)
      return false;

    if (ProductionDate != TmpHeadParameters.ProductionDate)
      return false;

    for (int i = 0; i < 9; i++)
    {
      if (SerialNumber[i] != TmpHeadParameters.SerialNumber[i])
        return false;
    }

    return true;
  }
}STRUCT_ATTRIBUTE;


// Restore previous align settings
#ifdef OS_WINDOWS
#pragma pack(pop)
#endif


#endif
