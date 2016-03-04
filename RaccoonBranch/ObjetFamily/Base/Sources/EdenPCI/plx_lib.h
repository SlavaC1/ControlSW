/*$T Quadra2/Q2RT/Sources/EdenPCI/plx_lib.h GC 1.139 03/14/06 09:49:32 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef _PLX_LIB_H_
#define _PLX_LIB_H_



/*
 * File: plx_lib.h Library for accessing PLX devices. The code accesses hardware
 * using WinDriver's WDC library. Copyright (c) 2003 - 2005 Jungo Ltd.
 * http://www.jungo.com
 */
#define EXTERN extern
#include "plx_defs.h"
#undef EXTERN

typedef WORD* PWORD;

#ifdef __cplusplus
extern "C"
{
#endif

/* PLX address spaces */
#if defined(SOLARIS) && defined(x86)

/*
 * On some Solaris x86 platforms, when the size of a physical memory range is not
 * aligned to system page boundary, the mapping of the physical memory to virtual
 * user-mode space might be inconsistent. PLX 9030, 9050, 9052, 9056, 9080 and
 * 9656 boards map the run-time registers both to memory region BAR0 and to IO
 * region BAR1. Since the size of BAR0 is smaller than a system page, this BAR
 * might be mapped inconsistently. Using BAR1 to access the run-time registers
 * will avoid the possible problem described above. The following macro
 * definition, combined with the macro definition of PLX_ADDR_REG below, ensures
 * that run-time registers will be accessed via IO mapping (BAR1).
 */
#define REG_IO_ACCESS
#endif
#if defined(REG_IO_ACCESS)
#define PLX_ADDR_REG	AD_PCI_BAR1
#else
#define PLX_ADDR_REG	AD_PCI_BAR0
#endif
#define PLX_ADDR_REG_IO AD_PCI_BAR1
#define PLX_ADDR_SPACE0 AD_PCI_BAR2
#define PLX_ADDR_SPACE1 AD_PCI_BAR3
#define PLX_ADDR_SPACE2 AD_PCI_BAR4
#define PLX_ADDR_SPACE3 AD_PCI_BAR5
typedef DWORD	PLX_ADDR;

/* Address space information struct */
#define MAX_TYPE	8
typedef struct
{
	DWORD	dwAddrSpace;
	CHAR	sType[MAX_TYPE];
	CHAR	sName[MAX_NAME];
	CHAR	sDesc[MAX_DESC];
} PLX_ADDR_SPACE_INFO;

/* Device address description struct */
typedef struct {
    DWORD dwNumAddrSpaces;    /* Total number of device address spaces */
	WDC_ADDR_DESC *pAddrDesc; /* Array of device address spaces information */
	long   bufferLength;
	BYTE*  pNonPagedBuffer;
	DWORD* pBuffer;
	BOOL   pFIFODtartFlag;
	BYTE   intMsg;
} PLX_DEV_ADDR_DESC;

/*
 * Function prototypes ;
 */


BOOL	PLX_IsMaster(WDC_DEVICE_HANDLE hDev);
BOOL	PLX_IsUseInt(WDC_DEVICE_HANDLE hDev);

#if !defined(__KERNEL__)
/* Device open/close */
DWORD	PLX_DeviceOpenBySlot
		(
			WDC_DEVICE_HANDLE	*pDeviceHandle,
			const WD_PCI_SLOT	*pSlot,
			BOOL				fIsMaster,
			BOOL				fIsUseInt
		);
DWORD	PLX_DeviceOpen
		(
			WDC_DEVICE_HANDLE	*pDeviceHandle,
			DWORD				dwVendorID,
			DWORD				dwDeviceID,
			DWORD				nCardNum,
			BOOL				fIsMaster,
			BOOL				fIsUseInt
		);

DWORD	PLX_DeviceClose(WDC_DEVICE_HANDLE hDev);
                
/* Interrupts */
DWORD	PLX_IntEnable(WDC_DEVICE_HANDLE hDev, PLX_INT_HANDLER funcIntHandler, PVOID pData);
DWORD	PLX_IntDisable(WDC_DEVICE_HANDLE hDev);
BOOL	PLX_IntIsEnabled(WDC_DEVICE_HANDLE hDev);

/* Plug-and-play and power management events */
DWORD	PLX_EventRegister(WDC_DEVICE_HANDLE hDev, PLX_EVENT_HANDLER funcEventHandler);
DWORD	PLX_EventUnregister(WDC_DEVICE_HANDLE hDev);
BOOL	PLX_EventIsRegistered(WDC_DEVICE_HANDLE hDev);

#endif

/* Address spaces information */
DWORD	PLX_GetNumAddrSpaces(WDC_DEVICE_HANDLE hDev);
BOOL	PLX_GetAddrSpaceInfo(WDC_DEVICE_HANDLE hDev, PLX_ADDR_SPACE_INFO *pAddrSpaceInfo);

/* Read/write local addresses */
DWORD	PLX_ReadAddrLocalBlock
		(
			WDC_DEVICE_HANDLE	hDev,
			PLX_ADDR			addrSpace,
			DWORD				dwLocalAddr,
			DWORD				dwBytes,
			PVOID				pData,
			WDC_ADDR_MODE		mode,
			WDC_ADDR_RW_OPTIONS options
		);
DWORD	PLX_WriteAddrLocalBlock
		(
			WDC_DEVICE_HANDLE	hDev,
			PLX_ADDR			addrSpace,
			DWORD				dwLocalAddr,
			DWORD				dwBytes,
			PVOID				pData,
			WDC_ADDR_MODE		mode,
			WDC_ADDR_RW_OPTIONS options
		);
/*fixme - remove block?
DWORD	PLX_ReadAddrLocal8(WDC_DEVICE_HANDLE hDev, PLX_ADDR addrSpace, DWORD dwLocalAddr, BYTE *pbData);
DWORD	PLX_WriteAddrLocal8(WDC_DEVICE_HANDLE hDev, PLX_ADDR addrSpace, DWORD dwLocalAddr, BYTE bData);
DWORD	PLX_ReadAddrLocal16(WDC_DEVICE_HANDLE hDev, PLX_ADDR addrSpace, DWORD dwLocalAddr, WORD *pwData);
DWORD	PLX_WriteAddrLocal16(WDC_DEVICE_HANDLE hDev, PLX_ADDR addrSpace, DWORD dwLocalAddr, WORD wData);
DWORD	PLX_ReadAddrLocal32(WDC_DEVICE_HANDLE hDev, PLX_ADDR addrSpace, DWORD dwLocalAddr, UINT32 *pu32Data);
DWORD	PLX_WriteAddrLocal32(WDC_DEVICE_HANDLE hDev, PLX_ADDR addrSpace, DWORD dwLocalAddr, UINT32 u32Data);
DWORD	PLX_ReadAddrLocal64(WDC_DEVICE_HANDLE hDev, PLX_ADDR addrSpace, DWORD dwLocalAddr, UINT64 *pu64Data);
DWORD	PLX_WriteAddrLocal64(WDC_DEVICE_HANDLE hDev, PLX_ADDR addrSpace, DWORD dwLocalAddr, UINT64 u64Data);
fixme*/
/*
 * Access the serial EEPROM ;
 * Verify that a blank or programmed serial EEPROM is present
 */
BOOL	PLX_EEPROMIsPresent(WDC_DEVICE_HANDLE hDev);

/* Access EEPROM via VPD (supported by PLX 9030, 9054, 9056, 9656) */
BOOL	PLX_EEPROM_VPD_Validate(WDC_DEVICE_HANDLE hDev);
DWORD	PLX_EEPROM_VPD_Read32(WDC_DEVICE_HANDLE hDev, DWORD dwOffset, UINT32 *pu32Data);
DWORD	PLX_EEPROM_VPD_Write32(WDC_DEVICE_HANDLE hDev, DWORD dwOffset, UINT32 u32Data);

/* Access EEPROM via run-time registers Supported by all PLX cards */
DWORD	PLX_EEPROM_RT_Read16(WDC_DEVICE_HANDLE hDev, DWORD dwOffset, PWORD pwData, DWORD EEPROMmsb);
DWORD	PLX_EEPROM_RT_Write16(WDC_DEVICE_HANDLE hDev, DWORD dwOffset, WORD wData, DWORD EEPROMmsb);
DWORD	PLX_EEPROM_RT_Read32(WDC_DEVICE_HANDLE hDev, DWORD dwOffset, UINT32 *pu32Data, DWORD EEPROMmsb);
DWORD	PLX_EEPROM_RT_Write32(WDC_DEVICE_HANDLE hDev, DWORD dwOffset, UINT32 u32Data, DWORD EEPROMmsb);

#define PLX_ReadReg8(hDev, dwOffset, pData)		WDC_ReadAddr8(hDev, PLX_ADDR_REG, dwOffset, pData)
#define PLX_ReadReg16(hDev, dwOffset, pData)	WDC_ReadAddr16(hDev, PLX_ADDR_REG, dwOffset, pData)
#define PLX_ReadReg32(hDev, dwOffset, pData)	WDC_ReadAddr32(hDev, PLX_ADDR_REG, dwOffset, pData)
#define PLX_ReadReg64(hDev, dwOffset, pData)	WDC_ReadAddr64(hDev, PLX_ADDR_REG, dwOffset, pData)
#define PLX_WriteReg8(hDev, dwOffset, pData)	WDC_WriteAddr8(hDev, PLX_ADDR_REG, dwOffset, pData)
#define PLX_WriteReg16(hDev, dwOffset, pData)	WDC_WriteAddr16(hDev, PLX_ADDR_REG, dwOffset, pData)
#define PLX_WriteReg32(hDev, dwOffset, pData)	WDC_WriteAddr32(hDev, PLX_ADDR_REG, dwOffset, pData)
#define PLX_WriteReg64(hDev, dwOffset, pData)	WDC_WriteAddr64(hDev, PLX_ADDR_REG, dwOffset, pData)

/*
 * Reset of PLX board ;
 * Software board reset for master devices (9054, 9056, 9080, 9656)
 */
void		PLX_SoftResetMaster(WDC_DEVICE_HANDLE hDev);

/* Get last PLX library error string */
DWORD		PLX_DeviceFind(DWORD dwVendorId, DWORD dwDeviceId, WD_PCI_SLOT *pSlot);

#if defined(USE_KERNEL_PLUGIN)
DWORD		PLX_CallKerPlug(WDC_DEVICE_HANDLE hplx, DWORD dwMessage, PVOID pData);
#endif

#ifdef __cplusplus
}
#endif
#endif
