#include "EdenPCI.h"

// If this macro is defined, verify the PLX handle on every read and write
#define VERIFY_HANDLE


const DWORD EDEN_PCI_VENDOR_ID = 0x11ae;
const DWORD EDEN_PCI_DEVICE_ID = 0x464e;

// EDEN_250: Although 250 has a small tray, we define a larger buffer, since data is created for 8 heads,
//           masking the 4 non-used. And number of X-movements per pass is twice then in 8-head machines.
//           But, in the print sequencers, we send data twice during a single pass.
#ifdef OBJET_1000
 const unsigned SIZE_OF_NON_PAGED_BUFFER = 5.5 * 1024 * 1024*2; //11 MBytes (*2 is for the XL)
#elif defined EDEN_260 || defined EDEN_250
const unsigned SIZE_OF_NON_PAGED_BUFFER = 3 * 1024 * 1024; //3 MBytes
#elif defined EDEN_500 || defined EDEN_350 || defined EDEN_350_V || defined EDEN_260_V || defined CONNEX_MACHINE || defined OBJET_MACHINE
const unsigned SIZE_OF_NON_PAGED_BUFFER = 5.5 * 1024 * 1024; //5.5 MBytes
#elif defined TESTER
const unsigned SIZE_OF_NON_PAGED_BUFFER = 0.5 * 1024 * 1024; 
#endif
// Handle type for the Eden PCI driver
typedef PWDC_DEVICE		TEdenPCIHandle;


// Handle to the open card
static TEdenPCIHandle EdenPCIHandle = 0;

static TEdenPCIFuncPtr gFuncPtr = NULL;

PVOID  pUserAddr;
KPTR   pKernelAddr;
HANDLE phBuf = 0;


DWORD EdenPCI_Init(void)
{
	DWORD dwOptions;
	DWORD dwStatus = WD_STATUS_SUCCESS;
	/*~~~~~~~~~~~~~*/
	WD_PCI_SLOT slot;
	/*~~~~~~~~~~~~~*/
    
#ifdef OS_VXWORKS
	drvrInit();
#endif
	dwStatus = PLX_LibInit();
	if(dwStatus != WD_STATUS_SUCCESS)
		return dwStatus;

	dwStatus = PLX_DeviceFind(EDEN_PCI_VENDOR_ID, EDEN_PCI_DEVICE_ID, &slot);
	if(dwStatus != WD_STATUS_SUCCESS)
		return dwStatus;
        #pragma warn -8075 // Disable warning Number 8075 "Suspicious pointer conversion"
	dwStatus = PLX_DeviceOpenBySlot(&EdenPCIHandle, &slot, IS_MASTER, IS_USE_INT);
        #pragma warn .8075 // Enable warning Number 8075 "Suspicious pointer conversion"
	if(dwStatus != WD_STATUS_SUCCESS)
		return dwStatus;

	/*
	 * Initialize the controller ;
	 * LAS0RR: 0x00 BIT4-31: Specifies which PCI address to use for decoding PCI
	 * access to local bus space 0
	 */
	PLX_WriteReg32(EdenPCIHandle, PLX_M_LAS0RR, 0xFF000000);

	/*
	 * LBRD0: 0x18 11 1:0 RW Memory space 0 Local Bus Width. 00-8 bits;
	 * 01- 16 bits;
	 * 10 or 11 - 32 bits 1000 5:2 RW Memory space 0 Internal Wait States (data to
	 * data: 0-15 wait states) 0 6 RW Memory space 0 Ready Input Enable. 1=Enable 0 7
	 * RW Memory space 0 BTERM# Enable. 1=Enable;
	 * if set to 0 bursts four Lword maximum at a time 1 8 RW Memory space 0 Prefetch
	 * Disable. 0=Enable;
	 * if disable disconnects after each memory read 0 9 RW Expansion ROM Space
	 * Prefetch Disable. 0=Enable;
	 * if disable disconnects after each memory read 0 10 RW Read Prefetch Count
	 * Enable 0 14:11 RW Prefetch counter 0 15 R- Reserved 11 17:16 RW Expansion ROM
	 * Space Local Bus Width. 00-8 bits;
	 * 01- 16 bits;
	 * 10 or 11 - 32 bits 21:18 RW Expansion ROM Space Internal Wait States (data to
	 * data: 0-15 wait states) 22 RW Expansion ROM Space Ready Input Enable. 1=Enable
	 * 23 RW Expansion ROM Space BTERM# Enable. 1=Enable;
	 * if set to 0 bursts four Lword maximum at a time 24 RW Memory space 0 Burst
	 * Enable. 1=Enable 25 R- Extra Long Load from Serial EEPROM. 1=Load 26 RW
	 * Expansion ROM Space Burst Enable. 1=Enable 27 RW Direct Slave PCI Write Mode.
	 * 0=disconnect when Direct Slave Write FIFO is full 0100 32:28 RW PCI Target
	 * Retry Delay Clock.(Multiply by 8)
	 */
	PLX_WriteReg32(EdenPCIHandle, PLX_M_LBRD0, 0x40030123);

	/*
	 * LAS0BA: 0x04 - Remap value must be multiple of Range(not Range register) 0 RW
	 * Space 0 Enable. 1=Enable 1 R- Reserved 3:2 RW if Local Space is mapped to
	 * memory space, bits are not used. If mapped into IO space, bits is included with
	 * bits [31:4] for remapping 31:4 RW Remap PCI Adddress to Local Address Space 0
	 * into a Local Address Space.
	 */
	PLX_WriteReg32(EdenPCIHandle, PLX_M_LAS0BA, 0x15000001);

#if defined(USE_KERNEL_PLUGIN)
	/*
	 * Allocate non paged memory buffer to be used both in the user mode and in the
	 * kernel mode
	 */

    dwOptions = DMA_ALLOW_64BIT_ADDRESS;
	dwStatus = WDC_SharedBufferAlloc(&pUserAddr, &pKernelAddr, SIZE_OF_NON_PAGED_BUFFER, dwOptions, &phBuf);
	if(dwStatus != WD_STATUS_SUCCESS)
		return dwStatus;

	NonPagedBuffer = pUserAddr;

	// Pass the physical address of the buffer to the kernel
	EdenPCI_ResetBufferAddress();
#endif /* USE_KERNEL_PLUGIN */
	return dwStatus;
}	/* EdenPCI_Init */

// Deinitialize the card
DWORD EdenPCI_DeInit(void)
{
	DWORD dwStatus = WD_STATUS_SUCCESS;
#if defined(USE_KERNEL_PLUGIN)
	/* Free the non paged buffer */
	dwStatus = PLX_IntDisable(EdenPCIHandle);
	if(dwStatus != WD_STATUS_SUCCESS)
		return dwStatus;

	dwStatus = WDC_SharedBufferFree(phBuf);
	if(dwStatus != WD_STATUS_SUCCESS)
		return dwStatus;
#endif /* USE_KERNEL_PLUGIN */

	if(EdenPCIHandle != 0)
	{
		dwStatus = PLX_DeviceClose(EdenPCIHandle);
		if(dwStatus != WD_STATUS_SUCCESS)
			return dwStatus;
	}

	return dwStatus;
}	/* EdenPCI_DeInit */

DWORD EdenPCI_WriteDWORD(int barNum, DWORD dwAddr, DWORD u32Data)
{
	DWORD dwStatus = WD_STATUS_SUCCESS;
#ifdef VERIFY_HANDLE
	if(EdenPCIHandle == 0) 
		return dwStatus;
#endif
	dwStatus = WDC_WriteAddr32(EdenPCIHandle, barNum, dwAddr, u32Data);
	
	if(dwStatus != WD_STATUS_SUCCESS)	
		ErrLog("EdenPCI_WriteDWORD: Error - failed to write 0x%lX\n", dwAddr);
	
	return dwStatus;	
}	/* EdenPCI_WriteDWORD */

DWORD EdenPCI_ReadDWORD(int barNum, DWORD dwAddr)
{
	/*~~~~~~~~~~~~*/
	UINT32	u32data;
	/*~~~~~~~~~~~~*/

#ifdef VERIFY_HANDLE
	if(EdenPCIHandle == 0) return 0;
#endif
	if(WD_STATUS_SUCCESS != WDC_ReadAddr32(EdenPCIHandle, barNum, dwAddr, &u32data))
	{
		ErrLog("EdenPCI_ReadDWORD: Error - failed to read 0x%lX\n", dwAddr);
		return 0;
	}

	return u32data;
}	/* EdenPCI_ReadDWORD */

#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
static void BasicInterruptHandler(WDC_DEVICE_HANDLE hPlx, PLX_INT_RESULT *intResult)
{
	(*gFuncPtr) ();
}	/* BasicInterruptHandler */
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"

void EdenPCI_EnableInterrupt(TEdenPCIFuncPtr FuncPtr)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	UINT32			regValue;
	PPLX_DEV_CTX	pDevCtx = WDC_GetDevContext(EdenPCIHandle);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(EdenPCIHandle == 0) return;

	gFuncPtr = FuncPtr;
	if(WD_STATUS_SUCCESS != PLX_IntEnable(EdenPCIHandle, BasicInterruptHandler, NULL))
	{
		ErrLog("EdenPCI_EnableInterrupt: Error - failed to enable interrupts\n");
		return;
	}

	if(WD_STATUS_SUCCESS != WDC_ReadAddr32(EdenPCIHandle, (PLX_ADDR) 0, PLX_M_INTCSR, &regValue))
	{
		ErrLog("EdenPCI_EnableInterrupt: Error - failed to read 0x%lX(PLX_M_INTCSR)\n", pDevCtx->dwINTCSR);
		return;
	}

	if(WD_STATUS_SUCCESS != WDC_WriteAddr32(EdenPCIHandle, (PLX_ADDR) 0, PLX_M_INTCSR, regValue | BIT11))
	{
		ErrLog("EdenPCI_EnableInterrupt: Error - failed to write 0x%lX(PLX_M_INTCSR)\n", pDevCtx->dwINTCSR);
		return;
	}
}	/* EdenPCI_EnableInterrupt */

void EdenPCI_DisableInterrupt(void)
{
	if(EdenPCIHandle == 0) return;

	PLX_IntDisable(EdenPCIHandle);
}

void EdenPCI_WriteEEPROM_DWORD(unsigned long dwAddr, unsigned long Data)
{
	if(WD_STATUS_SUCCESS != PLX_EEPROM_RT_Write32(EdenPCIHandle, dwAddr, Data, CS46EEPROM))
	{
		ErrLog("EdenPCI_WriteEEPROM_DWORD: Error - failed to write to EEPROM 0x%lX\n", dwAddr);
	}
}	/* EdenPCI_WriteEEPROM_DWORD */

unsigned long EdenPCI_ReadEEPROM_DWORD(unsigned long dwAddr)
{
	/*~~~~~~~~~~~~~*/
	UINT32	Data = 0;
	/*~~~~~~~~~~~~~*/

	if(WD_STATUS_SUCCESS != PLX_EEPROM_RT_Read32(EdenPCIHandle, dwAddr, &Data, CS46EEPROM))
	{
		ErrLog("EdenPCI_ReadEEPROM_DWORD: Error - failed to read to EEPROM 0x%lX\n", dwAddr);
	}

	return Data;
}	/* EdenPCI_ReadEEPROM_DWORD */

DWORD EdenPCI_ResetBufferAddress(void)
{
  DWORD ret = KP_STATUS_OK;
#if defined(USE_KERNEL_PLUGIN)
//  DWORD dwKPResult = 0;
  ret = PLX_CallKerPlug(EdenPCIHandle, KP_EDEN_MSG_BUFFER_ADDR,(PVOID)&pKernelAddr);
#endif
  return ((KP_STATUS_OK == ret) ? WD_STATUS_SUCCESS : WD_KERPLUG_FAILURE);
}

#if defined(USE_KERNEL_PLUGIN)
DWORD EdenPCI_InitParamsForISR(long BufferLength)
{
    return PLX_CallKerPlug(EdenPCIHandle, KP_EDEN_MSG_BUFFER_LENGTH, &BufferLength);
}	/* EdenPCI_InitParamsForISR */

BYTE* EdenPCI_GetNonPagedBuffer()
{
  return NonPagedBuffer;
}	/* EdenPCI_GetNonPagedBuffer */

BYTE EdenPCI_GetInterruptMessage()
{
  BYTE Message;
	PLX_CallKerPlug(EdenPCIHandle, KP_EDEN_MSG_GET_INTERRUPT_MSG, &Message);
  return Message;
}	/* EdenPCI_GetInterruptMessage */

KP_EDEN_VERSION EdenPCI_GetKernelPluginVersion()
{
	DWORD dwStatus;
	KP_EDEN_VERSION kpVer;
	dwStatus = PLX_CallKerPlug(EdenPCIHandle, KP_EDEN_MSG_VERSION, &kpVer);
	if (WD_STATUS_SUCCESS != dwStatus)
	{
		kpVer.dwVer = 0;
                #pragma warn -8065 // Disable warning Number 8065 "Call to functoin 'sprintf' with no prototype"
                #pragma warn -8045 // Disable warning Number 8045 "CNo declaration for function 'sprintf'"
		sprintf(kpVer.cVer,"Failed to read kernel plugin version");
                #pragma warn .8065 // Enable warning Number 8065 "Call to functoin 'sprintf' with no prototype"
                #pragma warn .8045 // Enable warning Number 8045 "CNo declaration for function 'sprintf'"
	}
	return kpVer;
}
#endif /* USE_KERNEL_PLUGIN */
