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
 * Last upate: 03/09/2001                                           *
 ********************************************************************/

#include "Slice.h"


CSlice::CSlice(PBYTE* Buffers,int SliceNumber,int Width,int Height,
               float SliceHeightZ,int XOffset,int YOffset,int Resolution) :
m_Buffers(NULL)
{
  Init(Buffers, SliceNumber,Width,Height,SliceHeightZ,XOffset,YOffset,Resolution);
}// Constructor


CSlice::~CSlice(void)
{
  DeInit();
}// Destructor

// Set the slice properties
void CSlice::Init(PBYTE* Buffers, int SliceNumber,int Width,int Height,
                  float SliceHeightZ,int XOffset,int YOffset,int Resolution)
{
  if(!Buffers)
    return;

  if(m_Buffers)
    free(m_Buffers);
     
  size_t size = sizeof(PBYTE)* NUMBER_OF_CHAMBERS;
  m_Buffers   = (PBYTE*)malloc(size);
  memcpy(m_Buffers, Buffers, size);

  m_SliceNumber   = SliceNumber;
  m_Width         = Width;
  m_Height        = Height;
  m_SliceHeightZ  = SliceHeightZ;
  m_XOffset       = XOffset;
  m_YOffset       = YOffset;
  m_Resolution    = Resolution;
}//Init

void CSlice::DeInit()
{
  if(m_Buffers)
    free(m_Buffers);
}

