/*===========================================================================
 *   FILENAME       : Byte Order {ByteOrder.h}  
 *   PURPOSE        : Byte order manipulation routines header file  
 *   DATE CREATED   : 06/Dec/2001
 *   PROGRAMMER     : Nir Saadon 
 *===========================================================================*/

#ifndef _BYTE_ORDER_H_
#define _BYTE_ORDER_H_

// Function Prototype 
// ====================

// Swap a short integer
// --------------------
void SwapUnsignedShort(PUSHORT S);

// Another version of SwapShort with a signed short as a parameter
// ---------------------------------------------------------------
void SwapShort(PSHORT S);

// Swap a long integer
// -------------------
void SwapUnsignedLong(PULONG L);

// Another version of SwapLong with a signed long as a parameter
// -------------------------------------------------------------
void SwapLong(PLONG L);

// Swap an array of short numbers
// ------------------------------
void SwapUnsignedShortArray(PUSHORT Src,PUSHORT Dst,int ElementsNum);

// Another version of SwapShort with a signed short array as a parameter
// ---------------------------------------------------------------------
void SwapShortArray(PSHORT Src,PSHORT Dst,int ElementsNum);

// Swap an array of long numbers
// -----------------------------
void SwapUnsignedLongArray(PULONG Src,PULONG Dst,int ElementsNum);

// Another version of SwapLongArray with a signed long array as a parameter
// ------------------------------------------------------------------------
void SwapLongArray(PLONG Src,PLONG Dst,int ElementsNum);



#endif	











