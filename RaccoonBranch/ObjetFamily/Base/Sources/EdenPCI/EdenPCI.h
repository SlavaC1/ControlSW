#ifndef _EDEN_PCI_H_
#define _EDEN_PCI_H_
#include "plx_lib.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* Possible EDEN PCI error codes */
typedef enum	TEdenPCIError
{
	EDEN_PCI_NO_ERR			      	=  0,
	EDEN_PCI_NO_CARD_ERR			= -1,
	EDEN_PCI_OPEN_ERR		      	= -2,
	EDEN_PCI_PLX_INIT_ERROR			= -3,
	EDEN_PCI_PLX_DEINIT_ERROR		= -4,
	EDEN_PCI_MEMORY_ALLOCATION_ERROR        = -5,
	EDEN_PCI_DMA_LOCK_ERROR			= -6
} TEdenPCIError;

typedef void (*TEdenPCIFuncPtr)(void);

DWORD EdenPCI_Init(void);
DWORD EdenPCI_DeInit(void);
DWORD EdenPCI_WriteDWORD(int BarNum,unsigned long Addr,unsigned long Data);
unsigned long EdenPCI_ReadDWORD(int BarNum,unsigned long Addr);

void EdenPCI_WriteEEPROM_DWORD(unsigned long Addr,unsigned long Data);
unsigned long EdenPCI_ReadEEPROM_DWORD(unsigned long Addr);


void EdenPCI_EnableInterrupt(TEdenPCIFuncPtr FuncPtr);
void EdenPCI_DisableInterrupt(void);

DWORD                   EdenPCI_ResetBufferAddress(void);
#if defined(USE_KERNEL_PLUGIN)
DWORD			EdenPCI_InitParamsForISR(long BufferLength);
BYTE			*EdenPCI_GetNonPagedBuffer();
BYTE			EdenPCI_GetInterruptMessage();
KP_EDEN_VERSION EdenPCI_GetKernelPluginVersion();
#endif /* USE_KERNEL_PLUGIN */
#ifdef __cplusplus
}
#endif
#endif
