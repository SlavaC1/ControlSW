 /********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Single slice information and handling.                   *
 * Module Description: Encapsulate all the information required for *
 *                     a single slice.                              *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 29/08/2001                                           *
 * Last upate: 14/03/2002                                           *
 ********************************************************************/

#ifndef _SLICE_H_
#define _SLICE_H_

#include "QObject.h"
#include "GlobalDefs.h"


// Single slice
class CSlice : public CQObject {
  private:
    // Pointer to the actuall slice memory
    PBYTE* m_Buffers;

    // Slice properties
    int   m_SliceNumber;
    int   m_Width,m_Height;
    float m_SliceHeightZ;
    int   m_XOffset,m_YOffset;
    int   m_Resolution;

  public:
    // Constructor (ownership transfer for the slice buffer)
    CSlice(PBYTE* Buffers       = 0,
           int    SliceNumber   = 0,
           int    Width         = 0,
           int    Height        = 0,
           float  SliceHeightZ  = 0,
           int    XOffset       = 0,
           int    YOffset       = 0,
           int    Resolution    = 0);

    // Destructor
    ~CSlice(void);

    // Set the slice properties
    void Init(PBYTE* Buffers,
              int    SliceNumber,
              int    Width,
              int    Height,
              float  SliceHeightZ,
              int    XOffset,
              int    YOffset,
              int    Resolution);

    void DeInit(void);

    // Set new slice number
    void SetSliceNumber(int SliceNumber)
    {
      m_SliceNumber = SliceNumber;
    }

    // Getters for the slice properties 

    inline PBYTE GetBuffer(TChamberIndex Chamber)
    {
      return m_Buffers[Chamber];
    }

    inline int GetWidth(void)
    {
      return m_Width;
    }

    inline int GetHeight(void)
    {
      return m_Height;
    }

    inline float GetSliceHeightZ(void)
    {
      return m_SliceHeightZ;
    }

    inline int GetXOffset(void)
    {
      return m_XOffset;
    }

    inline int GetYOffset(void)
    {
      return m_YOffset;
    }

    inline int GetResolution(void)
    {
      return m_Resolution;
    }

    inline int GetSliceNumber(void)
    {
      return m_SliceNumber;
    }
};

#endif

