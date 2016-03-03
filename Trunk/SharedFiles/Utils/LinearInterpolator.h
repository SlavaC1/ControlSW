/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Shared utils.                                           *
 * Module: Linear interpolation.                                    *
 * Module Description: Linear interpolation class.                  *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 29/08/2001                                           *
 * Last upate: 12/03/2003                                           *
 ********************************************************************/

#ifndef _LINEAR_INTERPOLATOR_H_
#define _LINEAR_INTERPOLATOR_H_

#include <stdio.h>
#include <vector>
#include <algorithm>
#include "QStringList.h"


/* Implementation notes
   --------------------
   This interpolator implement the simple and fast method of uniform spacing in the X axis.
   If all the points for interpolation has are in fixed distance along the X axis a simple scale
   factor can be used to lookup in the X and Y tables.
*/


// Exception class for the CLinearInterpolator class
class ELinearInterpolator : public EQException {
  public:
    ELinearInterpolator(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};


// Comparison object for sorting the table according to X values
template<class R>
struct XCompare {
  bool operator()(std::pair<R,R> a,std::pair<R,R> b) {
    return (a.first < b.first);
  }
};

// Simple linerar interpolator
template <class T>
class CLinearInterpolator {
  private:
    // Interpolator table type
    typedef std::vector<std::pair<T,T> > TInterpolatorTable;

    // Interpolation tables
    TInterpolatorTable m_Table;

    // Minimum and maximum values in the X table
    T m_MinXValue,m_MaxXValue;

    // Minimum and maximum values in the Y table
    T m_MinYValue,m_MaxYValue;

  public:
    // Constructor
    CLinearInterpolator(void) {
      m_MinXValue = m_MaxXValue = 0;
      m_MinYValue = m_MaxYValue = 0;
      Clear();
    }

    // Add entry to the table of interpolator points
    void AddEntry(T X,T Y);

    // Load the tables from text file
    void LoadFromFile(const QString& FileName);

    // Save the table to text file
    void SaveToFile(const QString& FileName);

    // Load the tables from text file
    void LoadInverseFromFile(const QString& FileName);

    // Clear all entries
    void Clear(void) {
      m_Table.clear();
    }

    // Changle the curve offset (retain gain)
    void Offset(T X);

    // Interpolate a single value
    T Interpolate(T X);

    // Interpolate a single value without any limits
    T InterpolateWithoutLimits(T X);

    // Interpolate a single value in the inverse direction
    T InverseInterpolate(T Y);

    // Sort the interpolation table according to the X value
    void Sort(void) {
      std::sort(m_Table.begin(),m_Table.end(),XCompare<T>());
    }

    // Reverse the table (Y -> X)
    void Reverse(CLinearInterpolator& ReverseInterpolator);
    CLinearInterpolator<T>& operator=(const CLinearInterpolator<T>& T2);



};

// Add entry to the table of interpolator points
template <class T>
void CLinearInterpolator<T>::AddEntry(T X,T Y)
{
  if(X < m_MinXValue)
    {
    m_MinXValue = X;
    m_MinYValue = Y;
    }
  else
    if(X > m_MaxXValue)
      {
      m_MaxXValue = X;
      m_MaxYValue = Y;
      }

  m_Table.push_back(std::pair<T,T>(X,Y));
}

// Load the tables from text file
template <class T>
void CLinearInterpolator<T>::LoadFromFile(const QString& FileName)
{
  CQStringList StrList;

  // Load the file into the string list
  StrList.LoadFromFile(FileName);

  Clear();

  for(unsigned i = 0; i < StrList.Count(); i++)
  {
    // Ignore empty lines
    if(StrList[i] == "")
      continue;

    double X,Y;

    // Extract X and Y values ("2" is the number of fields that need to be scanned)
    if(sscanf(StrList[i].c_str(),"%lf=%lf",&X,&Y) != 2)
      throw ELinearInterpolator("Bad interpolation table file format");

    // Add to the points list
    AddEntry((T)X,(T)Y);
  }

  Sort();
}

// Save the tables to text file
template <class T>
void CLinearInterpolator<T>::SaveToFile(const QString& FileName)
{
  CQStringList StrList;
  Sort();

  unsigned XIndex;

  for(XIndex = 0; XIndex < m_Table.size(); XIndex++)
    StrList.Add(QFloatToStr(m_Table[XIndex].first) + "=" + QFloatToStr(m_Table[XIndex].second));

  // Save the string list into the file
  StrList.SaveToFile(FileName);
}


// Load the tables from text file
template <class T>
void CLinearInterpolator<T>::LoadInverseFromFile(const QString& FileName)
{
  CQStringList StrList;

  // Load the file into the string list
  StrList.LoadFromFile(FileName);

  Clear();

  for(unsigned i = 0; i < StrList.Count(); i++)
  {
    // Ignore empty lines
    if(StrList[i] == "")
      continue;

    double X,Y;

    // Extract X and Y values ("2" is the number of fields that need to be scanned)
    //But becase this is the inverse lad case invert X,Y values.
    if(sscanf(StrList[i].c_str(),"%lf=%lf",&X,&Y) != 2)
      throw ELinearInterpolator("Bad interpolation table file format");

    // Add to the points list
    AddEntry((T)Y,(T)X);
  }

  Sort();
}

// Changle the curve offset (retain gain)
template <class T>
void CLinearInterpolator<T>::Offset(T X)
{
  // If no table is loaded:
  if(m_Table.empty())
    return;

  unsigned XIndex;

  // Search for the X table index
  for(XIndex = 0; XIndex < m_Table.size(); XIndex++)
    m_Table[XIndex].first += X;
}

// Interpolate a value
template <class T>
T CLinearInterpolator<T>::Interpolate(T X)
{
  // If no table is loaded, output = input
  if(m_Table.empty())
    return X;

  if(X <= m_MinXValue)
    return m_Table[0].second;

  if(X >= m_MaxXValue)
    return m_Table[m_Table.size() - 1].second;

  unsigned XIndex;

  // Search for the X table index
  for(XIndex = 0; XIndex < m_Table.size(); XIndex++)
    if(X < m_Table[XIndex].first)
      break;

  if(XIndex > 0)
    XIndex--;

  // The line slope is (y2-y1)/(x2-x1)
  T YDelta = m_Table[XIndex + 1].second - m_Table[XIndex].second;
  T XDelta = m_Table[XIndex + 1].first - m_Table[XIndex].first;

  if(XDelta == 0)
    return m_Table[XIndex].second;

  T b = m_Table[XIndex].second - (YDelta * m_Table[XIndex].first) / XDelta;

  return ((YDelta * X) / XDelta + b);
}

// Interpolate a value
template <class T>
T CLinearInterpolator<T>::InterpolateWithoutLimits(T X)
{
  unsigned XIndex;

  // If no table is loaded, output = input
  if(m_Table.empty() || m_Table.size() < 2)
    return X;

  if(X >= m_MaxXValue)
  {
    XIndex = m_Table.size() - 1;
  } else
  {
    // Search for the X table index
    for(XIndex = 0; XIndex < m_Table.size(); XIndex++)
      if(X < m_Table[XIndex].first)
        break;
  }

  if(XIndex > 0)
    XIndex--;

  // The line slope is (y2-y1)/(x2-x1)
  T YDelta = m_Table[XIndex + 1].second - m_Table[XIndex].second;
  T XDelta = m_Table[XIndex + 1].first - m_Table[XIndex].first;

  if(XDelta == 0)
    return m_Table[XIndex].second;

  T b = m_Table[XIndex].second - (YDelta * m_Table[XIndex].first) / XDelta;

  return ((YDelta * X) / XDelta + b);
}

/* Simple version of the interpolation code
  -----------------------------------------
  The order of the multiplication and divisions has been change in the real
  code in order to preserve precision with integer arithematics.

  double Factor = (m_YTable.size() - 1) / (m_MaxXValue - m_MinXValue);

  // Calculate X index position in the Y table
  int XIndex = (int)(Factor * (X - m_MinXValue));

  // Interpolate between the index point and the index + 1 point
  double a = (m_YTable[XIndex + 1] - m_YTable[XIndex]) / (m_XTable[XIndex + 1] - m_XTable[XIndex]);
  double b = m_YTable[XIndex] - a * m_XTable[XIndex];

  return (a * X + b);
*/

// Interpolate a value
template <class T>
T CLinearInterpolator<T>::InverseInterpolate(T Y)
{
  // If no table is loaded, output = input
  if(m_Table.empty())
    return Y;

  if(Y <= m_MinYValue)
    return m_Table[0].first;

  if(Y >= m_MaxYValue)
    return m_Table[m_XTable.size() - 1].first;

  unsigned YIndex;

  // Search for the Y table index
  for(YIndex = 0; YIndex < m_Table.size(); YIndex++)
    if(Y < m_Table[YIndex].second)
      break;

  if(YIndex > 0)
    YIndex--;

  // The line slope is (x2-x1)/(y2-y1)
  T XDelta = m_Table[YIndex + 1].first - m_Table[YIndex].first;
  T YDelta = m_Table[YIndex + 1].second - m_Table[YIndex].second;

  if(YDelta == 0)
    return m_Table[YIndex].first;

  T b = m_Table[YIndex].first - (XDelta * m_Table[YIndex].second) / YDelta;

  return ((XDelta * Y) / YDelta + b);
}


// Reverse the table (Y -> X)
template <class T>
void CLinearInterpolator<T>::Reverse(CLinearInterpolator& ReverseInterpolator)
{
  ReverseInterpolator.Clear();

  for(unsigned i = 0; i < m_Table.size(); i++)
    ReverseInterpolator.AddEntry(m_Table[i].second,m_Table[i].first);
}

template <class T>
CLinearInterpolator<T>& CLinearInterpolator<T>::operator=(const CLinearInterpolator<T>& T2)
{
   if (this == &T2)
   {  return *this;}
   
   m_Table = T2.m_Table;
   m_MinXValue = T2.m_MinXValue;
   m_MaxXValue = T2.m_MaxXValue;
   m_MinYValue = T2.m_MinYValue;
   m_MaxYValue = T2.m_MaxYValue;
   return *this;  
}


#endif
