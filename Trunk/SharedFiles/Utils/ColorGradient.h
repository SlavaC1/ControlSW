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

#ifndef _COLOR_GRADIENT_H_
#define _COLOR_GRADIENT_H_

class CColorGradient {
  private:
    float m_Scale1;
    float m_Scale2;    
    float m_RFactor1;
    float m_GFactor1;
    float m_BFactor1;

    float m_RGBDist2;
    float m_RFactor2;
    float m_GFactor2;
    float m_BFactor2;

    TColor m_Color1;
    TColor m_Color2;

    int m_Value1;
    int m_Value2;

    // Utility function for clipping color chanel values
    BYTE ClipColorChanel(float c);

    bool SelectRange(int Value1,int Value2);

  public:
    // Default constructor
    CColorGradient(void);

    // Constructor 1: two colors gradient
    CColorGradient(int Value1,TColor Color1,int Value2,TColor Color2);

    // Constructor 2: three colors gradient
    CColorGradient(int Value1,TColor Color1,int Value2,TColor Color2,int Value3,TColor Color3);

    void SetRange2(int Value1,TColor Color1,int Value2,TColor Color2);
    void SetRange3(int Value1,TColor Color1,int Value2,TColor Color2,int Value3,TColor Color3);

    // Get a color from value
    TColor GetColor(int Value);
};

#endif
