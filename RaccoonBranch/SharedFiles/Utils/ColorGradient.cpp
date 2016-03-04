/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Shared utils.                                           *
 * Module: Color gradient generator.                                *
 * Module Description: Calculate a color value according to a       *
 *                     relative scale.                              *
 *                                                                  *
 * Compilation: C++ Builder.                                        *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 09/06/2001                                           *
 * Last upate: 23/06/2001                                           *
 ********************************************************************/

#include <vcl.h>
#pragma hdrstop

#include <math.h>
#include "ColorGradient.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)


// An ugly macro - do not use except from the simplest case
#define SQR(x) ((x) * (x))


// Default constructor
CColorGradient::CColorGradient(void)
{
  // Set default values
  SetRange3(0,clBlack,50,clBlack,100,clBlack);
}

// Constructor 1: two colors gradient
CColorGradient::CColorGradient(int Value1,TColor Color1,int Value2,TColor Color2)
{
  SetRange2(Value1,Color1,Value2,Color2);
}

// Constructor 2: three colors gradient
CColorGradient::CColorGradient(int Value1,TColor Color1,int Value2,TColor Color2,int Value3,TColor Color3)
{
  SetRange3(Value1,Color1,Value2,Color2,Value3,Color3);
}

// Get a color from value
TColor CColorGradient::GetColor(int Value)
{
  ULONG RetColor;

  if(SelectRange(Value,m_Value2))
  {
    float ScaledValue = (float)(Value - m_Value1) * m_Scale1;

    float r = (ScaledValue * m_RFactor1) + GetRValue(m_Color1);
    float g = (ScaledValue * m_GFactor1) + GetGValue(m_Color1);
    float b = (ScaledValue * m_BFactor1) + GetBValue(m_Color1);

    RetColor = RGB(ClipColorChanel(r),ClipColorChanel(g),ClipColorChanel(b));
  } else
    {
      float ScaledValue = (float)(Value - m_Value2) * m_Scale2;

      float r = (ScaledValue * m_RFactor2) + GetRValue(m_Color2);
      float g = (ScaledValue * m_GFactor2) + GetGValue(m_Color2);
      float b = (ScaledValue * m_BFactor2) + GetBValue(m_Color2);

      RetColor = RGB(ClipColorChanel(r),ClipColorChanel(g),ClipColorChanel(b));
    }

  return TColor(RetColor);
}

// Utility function for clipping color chanel values
BYTE CColorGradient::ClipColorChanel(float c)
{
  if(c < 0.0f)
    c = 0.0f;
  else
    if(c > 255.0f)
      c = 255.0f;
    else
      c += 0.5f;

  return BYTE(c);
}

void CColorGradient::SetRange2(int Value1,TColor Color1,int Value2,TColor Color2)
{
  BYTE c1r = GetRValue(Color1);
  BYTE c1g = GetGValue(Color1);
  BYTE c1b = GetBValue(Color1);

  BYTE c2r = GetRValue(Color2);
  BYTE c2g = GetGValue(Color2);
  BYTE c2b = GetBValue(Color2);

  // Calculate the distance in RGB space between the background and foreground heaters color
  float RGBDist = sqrt(SQR(c2r - c1r) + SQR(c2g - c1g) + SQR(c2b - c1b));

  // Calculate the normalize color vector
  m_RFactor1 = (c2r - c1r) / RGBDist;
  m_GFactor1 = (c2g - c1g) / RGBDist;
  m_BFactor1 = (c2b - c1b) / RGBDist;

  m_Color1 = Color1;

  if(Value2 - Value1 != 0)
    m_Scale1 = RGBDist / (Value2 - Value1);
  else
    m_Scale1 = 0.0;

  m_Value1 = Value1;
  m_Value2 = Value2;
}

void CColorGradient::SetRange3(int Value1,TColor Color1,int Value2,TColor Color2,int Value3,TColor Color3)
{
  BYTE c1r = GetRValue(Color1);
  BYTE c1g = GetGValue(Color1);
  BYTE c1b = GetBValue(Color1);

  BYTE c2r = GetRValue(Color2);
  BYTE c2g = GetGValue(Color2);
  BYTE c2b = GetBValue(Color2);

  // Calculate the distance in RGB space between the background and foreground heaters color
  float RGBDist = sqrt(SQR(c2r - c1r) + SQR(c2g - c1g) + SQR(c2b - c1b));

  if(Value2 - Value1 != 0)
    m_Scale1 = RGBDist / (Value2 - Value1);
  else
    m_Scale1 = 0.0;

  // Calculate the normalize color vector
  m_RFactor1 = (c2r - c1r) / RGBDist;
  m_GFactor1 = (c2g - c1g) / RGBDist;
  m_BFactor1 = (c2b - c1b) / RGBDist;

  c1r = GetRValue(Color2);
  c1g = GetGValue(Color2);
  c1b = GetBValue(Color2);

  c2r = GetRValue(Color3);
  c2g = GetGValue(Color3);
  c2b = GetBValue(Color3);

  // Calculate the distance in RGB space between the background and foreground heaters color
  RGBDist = sqrt(SQR(c2r - c1r) + SQR(c2g - c1g) + SQR(c2b - c1b));

  if(Value3 - Value2 != 0)
    m_Scale2 = RGBDist / (Value3 - Value2);
  else
    m_Scale2 = 0.0;
    
  // Calculate the normalize color vector
  m_RFactor2 = (c2r - c1r) / RGBDist;
  m_GFactor2 = (c2g - c1g) / RGBDist;
  m_BFactor2 = (c2b - c1b) / RGBDist;

  m_Color1 = Color1;
  m_Color2 = Color2;
  m_Value1 = Value1;
  m_Value2 = Value2;
}

bool CColorGradient::SelectRange(int Value1,int Value2)
{
  if(m_Value1 < m_Value2)
    return (Value1 <= Value2);

  return (Value1 >= Value2);
}
