/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Speed class                                              *
 * Module Description: This class implement services related to the *
 *                     speed conversion from M/Sec to A/D.          *
 *                                                                  *
 *                                                                  *
 * Compilation: Standard  C++.                                      *
 *                                                                  *
 * Author: Elad Atar                                                *
 * Start date: 06/11/2012                                           *
 *                                                                  *
 * See examples of use in module EvacuationAirFlow.h                *
 ********************************************************************/

#ifndef _SPEED_H_  //code guard
#define _SPEED_H_

#include <assert.h>
#include "QTypes.h"
#include "LinearInterpolator.h"

#define RET_TYPE_SPEED float
//TODO (Elad): Unite the different classes by creating a shared base class, thus discarding code duplication

//Group Speed
// each group will perform it own group LoadSpeedTable.
class  CSpeed {
protected:
  int m_A2DValue;
  
public:
  // Constructor
  CSpeed()
  {
	 m_A2DValue = 0;
  }

  // Destructor
  ~CSpeed() {}

  //Copy constructor
  CSpeed(const CSpeed & T2)
  {
	 m_A2DValue=T2.m_A2DValue;
  }

  virtual int ConvertSpeedToA2D(RET_TYPE_SPEED value)=0;
  virtual RET_TYPE_SPEED ConvertA2DToSpeed(int value)=0;
  
  int operator + (const CSpeed& T2) const  { return (m_A2DValue + T2.m_A2DValue);}
  int operator - (const CSpeed& T2) const  { return (m_A2DValue - T2.m_A2DValue);}

  bool operator == (const CSpeed& T2) const {
    return (m_A2DValue == T2.m_A2DValue);
  }

  bool operator != (const CSpeed& T2) const{
    return (m_A2DValue != T2.m_A2DValue);
  }

  bool operator > (const CSpeed& T2) const {
	return (m_A2DValue > T2.m_A2DValue);
  }

  bool operator < (const CSpeed& T2) const {
	 return (m_A2DValue < T2.m_A2DValue);
  }
  
  bool operator > (const int& T2) const {
    return (m_A2DValue > T2);
  }

  bool operator < (const int& T2) const {
     return (m_A2DValue < T2);
  }

  CSpeed& operator = (const CSpeed& T2){
       // operator equals, frees existing memory
       // then copies m_A2DValue (Speed).
       if (this == &T2)
           return *this;
       m_A2DValue=T2.m_A2DValue;
       return *this;
       }

  void AssignFromA2D(int Value) {m_A2DValue = Value;}

  // This function looks in table the A/D value for each Speed Speed and
  // kept the value in A/D.
  void AssignFromSpeed(RET_TYPE_SPEED Value) {
     m_A2DValue = ConvertSpeedToA2D(Value);
	 }

  RET_TYPE_SPEED SpeedValue() {
    return ConvertA2DToSpeed(m_A2DValue);
  }

  int A2DValue() {
    return m_A2DValue;
  }
};


//Group Speed
// each group will perform it own group LoadSpeedTable.
// This class is used when X-Y are proportional
class  CProportionalSpeed {
protected:
  int m_A2DValue;
  
public:
  // Constructor
  CProportionalSpeed() {
     m_A2DValue = 0;
     }

  // Destructor
  ~CProportionalSpeed() {}

  //Copy constructor
  CProportionalSpeed(const CProportionalSpeed & T2)
  {
     m_A2DValue=T2.m_A2DValue;
  }

  //virtual void LoadSpeedTable(const QString& FileName);
  virtual int ConvertSpeedToA2D(RET_TYPE_SPEED value)=0;
  virtual RET_TYPE_SPEED ConvertA2DToSpeed(int value)=0;
  
  int operator + (const CProportionalSpeed& T2)  { return (m_A2DValue + T2.m_A2DValue);}
  int operator - (const CProportionalSpeed& T2)  { return (m_A2DValue - T2.m_A2DValue);}

  bool operator == (const CProportionalSpeed& T2) const{
    return (m_A2DValue == T2.m_A2DValue);
  }

  bool operator != (const CProportionalSpeed& T2) const{
    return (m_A2DValue != T2.m_A2DValue);
  }

  bool operator > (const CProportionalSpeed& T2) const{
    return (m_A2DValue > T2.m_A2DValue);
  }

  bool operator < (const CProportionalSpeed& T2) const{
     return (m_A2DValue < T2.m_A2DValue);
  }
  
  bool operator > (const int& T2) const{
    return (m_A2DValue > T2);
  }

  bool operator < (const int& T2) const {
     return (m_A2DValue < T2);
  }

  CProportionalSpeed& operator = (const CProportionalSpeed& T2)
  {
       // operator equals, frees existing memory
       // then copies m_A2DValue (Speed).
       if (this == &T2)
           return *this;
       m_A2DValue=T2.m_A2DValue;
       return *this;
  }

  void AssignFromA2D(int value) {m_A2DValue = value;}

  // This function looks in table the A/D value for each Speed Speed and
  // kept the value in A/D.
  void AssignFromSpeed(RET_TYPE_SPEED value) { m_A2DValue = ConvertSpeedToA2D(value); }

  RET_TYPE_SPEED SpeedValue() { return ConvertA2DToSpeed(m_A2DValue); }

  int A2DValue() { return m_A2DValue; }
};

// ----------------------------------------------------------------
//-----------------------------------------------------------------


//Single/Own Speed
// each client must perform its own LoadSpeedTable.
class  CSelfSpeed {
protected:
  //The following two lines should be declared in the inherit CSpeed.
  CLinearInterpolator<RET_TYPE_SPEED> m_A2DToSpeed;
  CLinearInterpolator<int> m_SpeedToA2D;

  int m_A2DValue;

public:
  // Constructor
  CSelfSpeed()
  {
	m_A2DValue = 0;
  }

  // Destructor
  ~CSelfSpeed() {}

  //Copy constructor
  CSelfSpeed(const CSelfSpeed & T2)
  {
	 m_A2DValue   = T2.m_A2DValue;
	 m_A2DToSpeed = T2.m_A2DToSpeed;
	 m_SpeedToA2D = T2.m_SpeedToA2D;
  }

  void LoadSpeedTable(const QString& FileName)
  {
	 m_A2DToSpeed.LoadFromFile(FileName);
	 m_SpeedToA2D.LoadInverseFromFile(FileName);
  }


  // Static function
  int ConvertSpeedToA2D(RET_TYPE_SPEED value) { return (m_SpeedToA2D.Interpolate(value)); }
  RET_TYPE_SPEED ConvertA2DToSpeed(int value) { return (m_A2DToSpeed.Interpolate(value)); }
  
  int operator + (const CSelfSpeed& T2)  { return (m_A2DValue + T2.m_A2DValue);}
  int operator - (const CSelfSpeed& T2)  { return (m_A2DValue - T2.m_A2DValue);}

  bool operator == (const CSelfSpeed& T2) const { return (m_A2DValue == T2.m_A2DValue); }
  bool operator != (const CSelfSpeed& T2) const { return (m_A2DValue != T2.m_A2DValue); }

  bool operator > (const CSelfSpeed& T2) const { return (m_A2DValue > T2.m_A2DValue); }
  bool operator < (const CSelfSpeed& T2) const { return (m_A2DValue < T2.m_A2DValue); }

  CSelfSpeed& operator = (const CSelfSpeed& T2)
  {
	   // operator equals, frees existing memory
	   // then copies m_A2DValue (Speed).
	   if (this == &T2)
	   {	   return *this;}
	   m_A2DValue = T2.m_A2DValue;
	   m_A2DToSpeed = T2.m_A2DToSpeed;
       m_SpeedToA2D = T2.m_SpeedToA2D;
	   
	   return *this;
  }

  void AssignFromA2D(int Value) {m_A2DValue = Value;}

  // This function looks in table the A/D value for each Speed and keeps the value as A/D.
  void AssignFromSpeed(RET_TYPE_SPEED Value) { m_A2DValue = ConvertSpeedToA2D(Value); }

  RET_TYPE_SPEED SpeedValue() { 	return ConvertA2DToSpeed(m_A2DValue); }

  int A2DValue() { return m_A2DValue; }
};


#endif




