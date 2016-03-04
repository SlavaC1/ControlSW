/*===========================================================================
 *   FILENAME       : EXTMem  {EXTMem.C}  
 *   PURPOSE        : External memory interface module  
 *   DATE CREATED   : 05/11/2005
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/
#ifdef OCB_SIMULATOR
#include "..\EXTMemSim\EXTMem.h"
#include "c8051f120.h"
#include "..\TimerSim\TimerDrv.h"
#define UNDESIGNATED_ADDRESS              0x0151 // This address is overlooked by the Lotus FW.
#else
#include "..\EXTMem\EXTMem.h"
#include "..\SysDef\c8051f120.h"
#include "..\HWAccess\HWAccess.h"
#include "..\Timer\TimerDrv.h"
#endif



#include "..\Utils\ByteOrder.h"

#define EXT_MEM_INIT_TIMEOUT 200
#define TEST_BYTE_VALUE      85  //0x55

BYTE xdata ExtMem_BaseAddress _at_ 0x2000;

TIMER_struct xdata AckTimeout;

// I2C initializaion
BOOL EXTMemInit()
{
  SFRPAGE = EMI0_PAGE; // Switch to EXTERNAL MEMORY INTERFACE page (for EMI0CF)

 // Init the external memory:
 // Bits1-0: EALE1-0: ALE Pulse-Width Select Bits = 00  (1 sysclk cycle)
 // Bits3-2: EMD1-0: EMIF Operating Mode Select.  = 01: 
 //          Split Mode without Bank Select: Accesses below the 8k boundary are directed on-chip.
 //          Accesses above the 8k boundary are directed off-chip. 8-bit off-chip MOVX operations use
 //          the current contents of the Address High port latches to resolve upper address byte. Note
 //          that in order to access off-chip space, EMI0CN must be set to a page that is not contained in
 //          the on-chip address space.
 // Bit4: EMD2: EMIF Multiplex Mode Select. = 0 
 //       0: EMIF operates in multiplexed address/data mode.
 // Bit5: PRTSEL: EMIF Port Select. = 0
 //       0: EMIF active on P0-P3.

//  EMI0CF |= 0x04;
  EMI0CF |= 0x07;


  SFRPAGE = CONFIG_PAGE; // Switch to Config page (for P0MDOUT..P3MDOUT, XBR0)

//  Set external memory control ports to push/pull mode
  P0MDOUT |= 0xE5;
  P1MDOUT |= 0x04; // Board specific - dual port handshake (Request Bit)
  P1MDIN |= 0x08;  // Board specific - dual port handshake (Ack Bit) Set as Digital Input
  P2MDOUT |= 0xFF;
  P3MDOUT |= 0x00; // 0x00 is for OpenDrain , 0xFF is for Push Pull.


//    Bit1: EMIFLE: External Memory Interface Low-Port Enable Bit.
//    0: P0.7, P0.6, and P0.5 functions are determined by the Crossbar or the Port latches.
//    1: If EMI0CF.4 = 0 (External Memory Interface is in Multiplexed mode)
//    P0.7 (/WR), P0.6 (/RD), and P0.5 (ALE) are skipped by the Crossbar and their
//         output states are determined by the Port latches and the External Memory Interface.
//    1: If EMI0CF.4 = 1 (External Memory Interface is in Non-multiplexed mode)
//    P0.7 (/WR) and P0.6 (/RD) are skipped by the Crossbar and their output states are
//         determined by the Port latches and the External Memory Interface.

// Crossbar 2 settings: bit-7: WEAKPUD: Weak Pull-up disable bit
//                      0: Weak pull ups globally enabled
//                      1: Weak pull ups globally disabled

    XBR2 |= 0x02; 
//    XBR2 |= 0x82; 

  return TRUE;
}

// Test the ExtMem modul to be sure it is functional before continuing to run.
#ifndef OCB_SIMULATOR
BOOL EXTMemWaitForReady()
{
  BYTE TestByte = 0x00;
  int i=0;

  // Wait until ReadWriteRequest is functional:
  for (;;)
  {
    i++;

    if (EXTMem_ReadWriteRequest())
      break;

    if (i > 10000)
      return FALSE;
  }

  // Now try to write and read from Memory: (MemTestTimeout timer is still counting...)
  i = 0;
  for (;;)
  {
    i++;

    if (i > 10000)
	  return FALSE;

	TestByte = 0x00;

    EXTMem_Write((WORD)UNDESIGNATED_ADDRESS, TEST_BYTE_VALUE);
    EXTMem_Read ((WORD)UNDESIGNATED_ADDRESS, &TestByte);

    if (TestByte != TEST_BYTE_VALUE)
      continue;

    EXTMem_Write((WORD)UNDESIGNATED_ADDRESS, 0x00);
    EXTMem_Read ((WORD)UNDESIGNATED_ADDRESS, &TestByte);

    if (TestByte == 0x00)
      break;
  }

  return TRUE;
}
#endif
/*
When runnning under Simulation, there's no need to actually synchronize Ext Memory Read/Writes.
And because this code is executed *a lot*, it is better to completely remove it to get better performance.

AGSI DLL Read/Writes are anyway synchronized with the code:
1. Multi-byte AGSI writes are atomic. (via AGSI special API)
2. Multi-byte AGSI reads are acheived by a callback invoked as a result of a change to the *last* written byte in the sequence. 
   - So it is effectively atomic as well.
*/
BOOL EXTMem_ReadWriteRequest(void)
{
#if defined SIMULATOR  || defined OCB_SIMULATOR
  return TRUE;
#else
	
  TimerSetTimeout(&AckTimeout,10);
  
  EXT_MEM_REQ = SET;

  while(!TimerHasTimeoutExpired(&AckTimeout))
    if (EXT_MEM_ACK == SET)
      return TRUE;

  EXT_MEM_REQ = RESET;

  return FALSE;
#endif
}

BOOL EXTMem_Read(WORD addOffset, BYTE* dataItem)
{
  if (EXTMem_ReadWriteRequest())
  {
    EXTMem_ReadNoLock(addOffset, dataItem);
	EXT_MEM_REQ = RESET;
	return TRUE;
  }

  return FALSE;
}

BOOL EXTMem_Write(WORD addOffset, BYTE dataItem)
{

  if (EXTMem_ReadWriteRequest())
  {
    EXTMem_WriteNoLock(addOffset, dataItem);
	EXT_MEM_REQ = RESET;
	return TRUE;
  }

  return FALSE;
}

// Assumes that memory is already locked (by a higher function)
BOOL EXTMem_ReadNoLock(WORD addOffset, BYTE* dataItem)
{
  *dataItem = *(&ExtMem_BaseAddress + addOffset);

  return TRUE;
}

// Assumes that memory is already locked (by a higher function)
BOOL EXTMem_WriteNoLock(WORD addOffset, BYTE dataItem)
{
  BYTE retries = 0;
  BYTE verifyByte = 0;
  // Todo: the verification and retries mechanism comes since there is a problem with the Write in Elec.
  //       once the problem is solved, remove the write verification and retries.

  for (retries = 0; retries < 1; retries++)
  {
    *(&ExtMem_BaseAddress + addOffset) = dataItem;

	// Read the value to verify if Write was successful:
	EXTMem_ReadNoLock(addOffset, &verifyByte);

	if (verifyByte == dataItem)
	  break;
  }

  if (verifyByte != dataItem)
    return FALSE;

  return TRUE;
}
BOOL EXTMem_ReadRange(WORD addOffset, BYTE Size, BYTE* dataItem)
{
  BYTE i;

  if (EXTMem_ReadWriteRequest())
  {
    for (i = 0; i < Size; i++)
      EXTMem_ReadNoLock(addOffset + i, &dataItem[i]);

    EXT_MEM_REQ = RESET;

    return TRUE;
  }

  return FALSE;
}

#ifndef OCB_SIMULATOR
BOOL EXTMem_WriteRange(WORD addOffset, BYTE Size, BYTE* dataItem)
{
  BYTE i;
 
  if (EXTMem_ReadWriteRequest())
  {
    for (i = 0; i < Size; i++)
	  EXTMem_WriteNoLock((addOffset + i), dataItem[i]);

    EXT_MEM_REQ = RESET;

    return TRUE;
  }

  return FALSE;
}
#endif

BOOL EXTMem_ReadAnalog12(WORD addOffset, WORD* dataItem)
{
    if (EXTMem_ReadRange(addOffset,2 ,(PBYTE)dataItem) == FALSE)
      return FALSE;

    // Since we are reading a 12bit value, we reset the 4 high bits:
	((PBYTE)(dataItem))[1] &= 0x0F;

    // Convert Little/Big endian:
    SwapUnsignedShort(dataItem);

    return TRUE;
}
BOOL EXTMem_WriteAnalog12(WORD addOffset, WORD dataItem)
{
  if (EXTMem_ReadWriteRequest())
  {    
    // Convert Little/Big endian:
    SwapUnsignedShort(&dataItem);

    // Write the first byte:
    EXTMem_WriteNoLock(addOffset, ((PBYTE)(&dataItem))[0]);

    // Write the second byte:
    EXTMem_WriteBitsNoLock((addOffset + 1), ((PBYTE)(&dataItem))[1], 0x0F);

   	EXT_MEM_REQ = RESET;

    return TRUE;   
  }

  return FALSE;
}
/*  The Original EXTMem_WriteAnalog12() (above) was changed to workaround HW Ext Mem problems:
BOOL EXTMem_WriteAnalog12(WORD addOffset, WORD dataItem)
{
  BYTE TargetByte = 0;
  BYTE DataItemHi = 0;

  if (EXTMem_ReadWriteRequest())
  {    
    // Convert Little/Big endian:
    SwapUnsignedShort(&dataItem);

    DataItemHi = ((PBYTE)(&dataItem))[1];

    EXTMem_ReadNoLock((addOffset + 1), &TargetByte);

    TargetByte &= 0xF0; // reset bits that we will update in the target byte.  
    DataItemHi &= 0x0F; // reset bits that we dont want to update in source byte
    TargetByte |=  DataItemHi; 

    // Write the first byte:
    EXTMem_WriteNoLock(addOffset, ((PBYTE)(&dataItem))[0]);

    // Write the second byte:
    EXTMem_WriteNoLock((addOffset + 1), TargetByte);

   	EXT_MEM_REQ = RESET;

    return TRUE;   
  }

  return FALSE;
}
*/

BOOL EXTMem_ReadWord(WORD addOffset, WORD* dataItem)
{
    if (EXTMem_ReadRange(addOffset,2 ,(PBYTE)dataItem) == FALSE)
      return FALSE;

    // Convert Little/Big endian:
    SwapUnsignedShort(dataItem);

    return TRUE;
}
#ifndef OCB_SIMULATOR
BOOL EXTMem_WriteBits(WORD addOffset, BYTE dataItem, BYTE bitMask)
{
  BYTE TargetByte;

  if (EXTMem_ReadWriteRequest())
  {    
    EXTMem_ReadNoLock(addOffset, &TargetByte);

    TargetByte &= ~bitMask;
    dataItem   &=  bitMask;
    TargetByte |=  dataItem;

    EXTMem_WriteNoLock(addOffset, TargetByte);

	EXT_MEM_REQ = RESET;

	return TRUE;
  }

  return FALSE;
}
#endif
BOOL EXTMem_WriteBitsNoLock(WORD addOffset, BYTE dataItem, BYTE bitMask)
{
  BYTE TargetByte;

  if (EXTMem_ReadNoLock(addOffset, &TargetByte) == FALSE)
    return FALSE;

  TargetByte &= ~bitMask;
  dataItem   &=  bitMask;
  TargetByte |=  dataItem;

  if (EXTMem_WriteNoLock(addOffset, TargetByte) == FALSE)
    return FALSE;

  return TRUE;
}
#ifndef OCB_SIMULATOR
BOOL EXTMem_ReadBits(WORD addOffset, BYTE* dataItem, BYTE bitMask)
{
  if (EXTMem_ReadWriteRequest())
  {    
    EXTMem_ReadNoLock(addOffset, dataItem);

    (*dataItem) &= bitMask;

	EXT_MEM_REQ = RESET;

	return TRUE;
  }

  return FALSE;
}
#endif
