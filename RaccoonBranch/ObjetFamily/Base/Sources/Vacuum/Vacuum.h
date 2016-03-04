/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Vacuum class                                             *
 * Module Description: This class implement services related to the *
 *                  vacuum conversion from cmH20(Pressure) to A/D.  *
 *                  via linear interpolation                        *
 *                                                                  *
 * Compilation: Standard  C++.                                      *
 *                                                                  *
 * Author: Gedalia Trejger                                          *
 * Start date: 20/12/2002                                           *
 *                                                                  *
 ********************************************************************/

#ifndef _VACUUM_SENSOR_H_
#define _VACUUM_SENSOR_H_

#include <assert.h>
#include "QTypes.h"
#include "LinearInterpolator.h"
#include "QParameter.h"

//Group temperature
// each group will perform it own group LoadTemperatureTable.

class  CVacuum
{
private:
  int m_A2DValue;

protected:
   static CLinearInterpolator <float> m_A2DToPressure;
   static CLinearInterpolator <float> m_PressureToA2D;

public:
  // Constructor
  CVacuum(void)
  {
     m_A2DValue = 0;
  }

  // Destructor
  ~CVacuum(void) {}

  //Copy constructor
  CVacuum(const CVacuum & V2)
  {
     m_A2DValue=V2.m_A2DValue;
  }

  virtual float ConvertVacuumA2DToPressure(int)=0;
  virtual int ConvertVaccumPressureToA2D(float)=0;

  int operator + (const CVacuum& V2) {return (m_A2DValue + V2.m_A2DValue);}
  int operator - (const CVacuum& V2) {return (m_A2DValue - V2.m_A2DValue);}

  bool operator == (const CVacuum& V2)
  {
     return (m_A2DValue == V2.m_A2DValue);
  }

  operator int & ()
  {
     return m_A2DValue;
  }

  bool operator != (const CVacuum& V2)
  {
     return (m_A2DValue != V2.m_A2DValue);
  }

  bool operator > (const CVacuum& V2)
  {
     return (m_A2DValue > V2.m_A2DValue);
  }

  bool operator < (const CVacuum& V2)
  {
     return (m_A2DValue < V2.m_A2DValue);
  }
  
  bool operator > (const int& v2)
  {
    return (m_A2DValue > v2);
  }

  bool operator < (const int& v2)
  {
     return (m_A2DValue < v2);
  }

  CVacuum& operator = (const CVacuum& V2)
  {
     // operator equals, frees existing memory
     // then copies m_Value (vacuum).
     if (this == &V2)
         return *this;
     m_A2DValue=V2.m_A2DValue;
     return *this;
  }

  void AssignFromA2D(int Value)
  {
     m_A2DValue = Value;
  }

  void AssignFromCmH2O(float Value)
  {
     m_A2DValue = ConvertVaccumPressureToA2D(Value);
  }

  int A2DValue(void)
  {
    return m_A2DValue;
  }

  float CmH2OValue(void)
  {
    return ConvertVacuumA2DToPressure(m_A2DValue);
  }
    
};

#endif




