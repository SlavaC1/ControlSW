/*===========================================================================
 *   FILENAME       : EXTMem  {EXTMem.H}  
 *   PURPOSE        : External memory interface module (header file) 
 *   DATE CREATED   : 05/11/2005
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/
#ifndef _EXT_MEM_H_
#define _EXT_MEM_H_
#ifdef OCB_SIMULATOR
#include "c8051f120.h"
#else
#include "..\SysDef\c8051f120.h"
#endif
#include "..\SysDef\Define.h"

sbit EXT_MEM_REQ = P1 ^ 2;
sbit EXT_MEM_ACK = P1 ^ 3;

// External memory intrface initializaion
BOOL EXTMemInit();

#ifndef OCB_SIMULATOR
BOOL EXTMemWaitForReady();
BOOL EXTMem_WriteRange(WORD addOffset, BYTE Size, BYTE* dataItem);
BOOL EXTMem_WriteBits(WORD addOffset, BYTE dataItem, BYTE bitMask);
BOOL EXTMem_ReadBits(WORD addOffset, BYTE* dataItem, BYTE bitMask);
#endif

BOOL EXTMem_ReadRange(WORD addOffset, BYTE Size, BYTE* dataItem);
BOOL EXTMem_ReadWriteRequest(void);
BOOL EXTMem_Read(WORD addOffset, BYTE* dataItem);
BOOL EXTMem_ReadAnalog12(WORD addOffset, WORD* dataItem);
BOOL EXTMem_WriteAnalog12(WORD addOffset, WORD dataItem);
BOOL EXTMem_Write(WORD addOffset, BYTE dataItem);
BOOL EXTMem_ReadWord(WORD addOffset, WORD* dataItem);
BOOL EXTMem_ReadNoLock(WORD addOffset, BYTE* dataItem);
BOOL EXTMem_WriteNoLock(WORD addOffset, BYTE dataItem);

BOOL EXTMem_WriteBitsNoLock(WORD addOffset, BYTE dataItem, BYTE bitMask);
#endif													