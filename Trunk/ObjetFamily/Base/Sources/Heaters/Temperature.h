/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Temperature class                                        *
 * Module Description: This class implement services related to the *
 *                     temperature conversion from Celcius to A/D.  *
 *                                                                  *
 *                                                                  *
 * Compilation: Standard  C++.                                      *
 *                                                                  *
 * Author: Gedalia Trejger                                          *
 * Start date: 10/01/2001                                           *
 *                                                                  *
 * See examples of use in module TrayHeater.h                       *
 ********************************************************************/

#ifndef _TEMPERATURE_H_
#define _TEMPERATURE_H_

#include <assert.h>
#include "QTypes.h"
#include "LinearInterpolator.h"


//Group temperature
// each group will perform it own group LoadTemperatureTable.
class  CTemperature {
protected:
  int m_A2DValue;
  
public:
  // Constructor
  CTemperature(void) {
     m_A2DValue = 0;
     }

  // Destructor
  ~CTemperature(void) {}

  //Copy constructor
  CTemperature(const CTemperature & T2)
     {
     m_A2DValue=T2.m_A2DValue;
     }

  //virtual void LoadTemperatureTable(const QString& FileName);
  virtual int ConvertCelciusToA2D(int value)=0;
  virtual int ConvertA2DToCelcius(int value)=0;
  
  int operator + (const CTemperature& T2) const  { return (m_A2DValue + T2.m_A2DValue);}
  int operator - (const CTemperature& T2) const  { return (m_A2DValue - T2.m_A2DValue);}

  bool operator == (const CTemperature& T2) const{
    return (m_A2DValue == T2.m_A2DValue);
  }

  bool operator != (const CTemperature& T2) const {
    return (m_A2DValue != T2.m_A2DValue);
  }

  bool operator > (const CTemperature& T2)const {
	return (m_A2DValue > T2.m_A2DValue);
  }

  bool operator < (const CTemperature& T2)const {
	 return (m_A2DValue < T2.m_A2DValue);
  }

  bool operator > (const int& T2)const {
    return (m_A2DValue > T2);
  }

  bool operator < (const int& T2) const{
     return (m_A2DValue < T2);
  }

  CTemperature& operator = (const CTemperature& T2){
       // operator equals, frees existing memory
       // then copies m_A2DValue (temperature).
       if (this == &T2)
           return *this;
       m_A2DValue=T2.m_A2DValue;
       return *this;
       }

  void AssignFromA2D(int Value) {m_A2DValue = Value;}

  // This function looks in table the A/D value for each celcius temperature and
  // kept the value in A/D.
  void AssignFromCelcius(int Value) {
     m_A2DValue = ConvertCelciusToA2D(Value);
     }

  int CelciusValue(void) {
    return ConvertA2DToCelcius(m_A2DValue);
  }

  int A2DValue(void) {
    return m_A2DValue;
  }
};


//Group temperature
// each group will perform it own group LoadTemperatureTable.
// This class is used when X-Y are proportional
class  CProportionalTemperature {
protected:
  int m_A2DValue;
  
public:
  // Constructor
  CProportionalTemperature(void) {
     m_A2DValue = 0;
     }

  // Destructor
  ~CProportionalTemperature(void) {}

  //Copy constructor
  CProportionalTemperature(const CProportionalTemperature & T2)
     {
     m_A2DValue=T2.m_A2DValue;
     }

  //virtual void LoadTemperatureTable(const QString& FileName);
  virtual int ConvertCelciusToA2D(int value)=0;
  virtual int ConvertA2DToCelcius(int value)=0;
  
  int operator + (const CProportionalTemperature& T2)  { return (m_A2DValue + T2.m_A2DValue);}
  int operator - (const CProportionalTemperature& T2)  { return (m_A2DValue - T2.m_A2DValue);}

  bool operator == (const CProportionalTemperature& T2) const {
    return (m_A2DValue == T2.m_A2DValue);
  }

  bool operator != (const CProportionalTemperature& T2) const {
    return (m_A2DValue != T2.m_A2DValue);
  }

  bool operator > (const CProportionalTemperature& T2) const {
    return (m_A2DValue > T2.m_A2DValue);
  }

  bool operator < (const CProportionalTemperature& T2) const{
     return (m_A2DValue < T2.m_A2DValue);
  }
  
  bool operator > (const int& T2) const{
    return (m_A2DValue > T2);
  }

  bool operator < (const int& T2) const{
     return (m_A2DValue < T2);
  }

  CProportionalTemperature& operator = (const CProportionalTemperature& T2){
       // operator equals, frees existing memory
       // then copies m_A2DValue (temperature).
       if (this == &T2)
           return *this;
       m_A2DValue=T2.m_A2DValue;
       return *this;
       }

  void AssignFromA2D(int Value) {m_A2DValue = Value;}

  // This function looks in table the A/D value for each celcius temperature and
  // kept the value in A/D.
  void AssignFromCelcius(int Value) {
     m_A2DValue = ConvertCelciusToA2D(Value);
     }

  int CelciusValue(void) {
    return ConvertA2DToCelcius(m_A2DValue);
  }

  int A2DValue(void) {
    return m_A2DValue;
  }
};

// ----------------------------------------------------------------
//-----------------------------------------------------------------


//Single/Own temperature
// each client most perform it own LoadTemperatureTable.
class  CSelfTemperature {
protected:
  //The following two lines should be declared in the inherit CTemperature.
  CLinearInterpolator<int> m_A2DToCelcius;
  CLinearInterpolator<int> m_CelciusToA2D;

  int m_A2DValue;

public:
  // Constructor
  CSelfTemperature(void) {
     m_A2DValue = 0;
     }

  // Destructor
  ~CSelfTemperature(void) {}

  //Copy constructor
  CSelfTemperature(const CSelfTemperature & T2)
     {
     m_A2DValue=T2.m_A2DValue;
	 m_A2DToCelcius=T2.m_A2DToCelcius;
	 m_CelciusToA2D=T2.m_CelciusToA2D;
	 
	 
     }

  void LoadTemperatureTable(const QString& FileName){
     m_A2DToCelcius.LoadFromFile(FileName);
     m_CelciusToA2D.LoadInverseFromFile(FileName);
     }


  // Static function
  int ConvertCelciusToA2D(int value) {
     return (m_CelciusToA2D.Interpolate(value));
     }

  int ConvertA2DToCelcius(int value){
     return (m_A2DToCelcius.Interpolate(value));
     }
  
  int operator + (const CSelfTemperature& T2)  { return (m_A2DValue + T2.m_A2DValue);}
  int operator - (const CSelfTemperature& T2)  { return (m_A2DValue - T2.m_A2DValue);}

  bool operator == (const CSelfTemperature& T2) const {
    return (m_A2DValue == T2.m_A2DValue);
  }

  bool operator != (const CSelfTemperature& T2) const{
    return (m_A2DValue != T2.m_A2DValue);
  }

  bool operator > (const CSelfTemperature& T2) const{
	return (m_A2DValue > T2.m_A2DValue);
  }

  bool operator < (const CSelfTemperature& T2) const{
     return (m_A2DValue < T2.m_A2DValue);
  }

  CSelfTemperature& operator = (const CSelfTemperature& T2){
       // operator equals, frees existing memory
       // then copies m_A2DValue (temperature).
       if (this == &T2)
        {   return *this;}
       m_A2DValue=T2.m_A2DValue;
	   m_A2DToCelcius=T2.m_A2DToCelcius;
	   m_CelciusToA2D=T2.m_CelciusToA2D;
       return *this;
       }

  void AssignFromA2D(int Value) {m_A2DValue = Value;}

  // This function looks in table the A/D value for each celcius temperature and
  // kept the value in A/D.
  void AssignFromCelcius(int Value) {
     m_A2DValue = ConvertCelciusToA2D(Value);
     }

  int CelciusValue(void) {
    return ConvertA2DToCelcius(m_A2DValue);
  }

  int A2DValue(void) {
    return m_A2DValue;
  }
};


#endif




