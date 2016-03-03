/* ========================================================================
   FILENAME : FIFOPCI.H

   DESCRIPTION :
      This file contains all constants, enumerations types,
        structurs, data types and functions
      that are exported by the FIFO package.
   ======================================================================== */

#ifndef __FIFOPCI_H__
#define __FIFOPCI_H__

#include "QTypes.h"
#include "FIFOPci_defs.h"

void      FIFOPCI_Init             (bool Emulation = false);
void      FIFOPCI_DeInit           (void);
void      FIFOPCI_MasterReset      (void);
void      FIFOPCI_PartialReset     (void);
void      FIFOPCI_InitParamsForISR (unsigned long *BufferPtr, long BufferSize);
#if defined(USE_KERNEL_PLUGIN)
void	  FIFOPCI_ISR_UKP          (void);
#else
void      FIFOPCI_ISR              (void);
#endif /* USE_KERNEL_PLUGIN */
unsigned  FIFOPCI_ReadStatus       (void);
int       FIFOPCI_ReadDWORD        (unsigned long *bufferPtr,int  NumberOfDwords);
int       FIFOPCI_WriteDWORD       (unsigned long *bufferPtr,int  NumberOfDwords);

int       FIFOPCI_ReadLeftDWORDS   (unsigned long *bufferPtr,int BufferSize);

// Interface functions to the FIFO
TQErrCode FIFOPCI_WriteAsync (PBYTE Buffer,unsigned BufferLength);
void      FIFOPCI_CancelWriteAsync(void);
void      FIFOPCI_NoGo(void);
void      FIFOPCI_ResetBufferAddress(void);
bool      FIFOPCI_IsEmpty(void);

//Pci statistics
void GetPCIStatistics(int& UnderrunErrorCounter,int& FifoIsNotEmptyErrorCounter);
void ResetPCIStatistics(void);

void SetFIFOUnderrunErrorCallback(TGenericCallback FIFOUnderrunErrorCallback,TGenericCockie FIFOUnderrunErrorCockie);


#endif   /* __FIFO_H__ */



