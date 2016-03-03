/*===========================================================================
 *   FILENAME       : Byte Order {ByteOrder.c}  
 *   PURPOSE        : Byte order manipulation routines  
 *   DATE CREATED   : 6/Dec/2001
 *   PROGRAMMER     : Nir Saadon 
 *   PROCEDURES     : 
 *===========================================================================*/



#include "Define.h"
#include "ByteOrder.h"


// Swap a short integer
// --------------------
void SwapUnsignedShort(PUSHORT S)
{
	PBYTE cp = (PBYTE)S;
	BYTE t;

	t = cp[1]; cp[1] = cp[0]; cp[0] = t;
}

// Another version of SwapShort with a signed short as a parameter
// ---------------------------------------------------------------
void SwapShort(PSHORT S)
{
  SwapUnsignedShort((PUSHORT)S);
}

// Swap a long integer
// -------------------
void SwapUnsignedLong(PULONG L)
{
  PBYTE cp = (PBYTE)L;
  BYTE t;

  t = cp[3]; cp[3] = cp[0]; cp[0] = t;
  t = cp[2]; cp[2] = cp[1]; cp[1] = t;
}

// Another version of SwapLong with a signed long as a parameter
// -------------------------------------------------------------
void SwapLong(PLONG L)
{
  SwapUnsignedLong((PULONG)L);
}

// Swap an array of short numbers
// ------------------------------
void SwapUnsignedShortArray(PUSHORT Src,PUSHORT Dst,int ElementsNum)
{
	PBYTE cp;
	BYTE t;

  while(ElementsNum--) {
    Dst[ElementsNum] = Src[ElementsNum];
    cp = (PBYTE)&Dst[ElementsNum];
    t = cp[1]; cp[1] = cp[0]; cp[0] = t;
  }
}

// Another version of SwapShort with a signed short array as a parameter
// ---------------------------------------------------------------------
void SwapShortArray(PSHORT Src,PSHORT Dst,int ElementsNum)
{
  SwapUnsignedShortArray((PUSHORT)Src,(PUSHORT)Dst,ElementsNum);
}

// Swap an array of long numbers
// -----------------------------
void SwapUnsignedLongArray(PULONG Src,PULONG Dst,int ElementsNum)
{
	PBYTE cp;
	BYTE t;

  while(ElementsNum--) {
    Dst[ElementsNum] = Src[ElementsNum];
    cp = (PBYTE)&Dst[ElementsNum];
    t = cp[3]; cp[3] = cp[0]; cp[0] = t;
    t = cp[2]; cp[2] = cp[1]; cp[1] = t;
  }
}

// Another version of SwapLongArray with a signed long array as a parameter
// ------------------------------------------------------------------------
void SwapLongArray(PLONG Src,PLONG Dst,int ElementsNum)
{
  SwapUnsignedLongArray((PULONG)Src,(PULONG)Dst,ElementsNum);
}


