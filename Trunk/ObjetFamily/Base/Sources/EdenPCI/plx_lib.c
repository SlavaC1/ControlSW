
/************************************************************************
*  File: plx_lib.c
*
*  Library for accessing PLX devices.
*  The code accesses hardware using WinDriver's WDC library.
*
*  Copyright (c) 2003 - 2005 Jungo Ltd.  http://www.jungo.com
*************************************************************************/
#if defined (__KERNEL__)
    #include "kpstdlib.h"
#else
    #include <stdio.h>
    #include <stdarg.h>
#endif
#include "plx_lib.h"
#include "bits.h"
#include "pci_regs.h"
#include "status_strings.h"
/*************************************************************
  Internal definitions
 *************************************************************/
extern CHAR gsPLX_LastErr[256];

/* WinDriver license registration string */
/* TODO -> DONE: When using a registered WinDriver version, replace the license string
         below with your specific WinDriver license registration string */
#define PLX_DEFAULT_LICENSE_STRING "6f1ead6f3ac0d3014711beb6faaf673cc61f41d5db963c.WD1040_64_NL_Objet"

/* VPD EEPROM delay */
#define EEPROM_VPD_Delay()     WDC_Sleep(20000, WDC_SLEEP_BUSY)

/* Run-time registers EEPROM delay */
#define EEPROM_RT_Delay()      WDC_Sleep(500, WDC_SLEEP_BUSY)

/* Soft board reset delay */
#define PLX_SOFT_RESET_DELAY() WDC_Sleep(5000000, WDC_SLEEP_NON_BUSY)

/*************************************************************
  Global variables definitions
 *************************************************************/

/*************************************************************
  Static functions prototypes and inline implementation
 *************************************************************/
#if !defined (__KERNEL__)
static BOOL  IsDeviceValid(const PWDC_DEVICE pDev);
static DWORD DeviceInit   (PWDC_DEVICE pDev, BOOL fIsMaster, BOOL fIsUseInt);

static DWORD IntEnable   (PWDC_DEVICE hDev,
                          PLX_INT_HANDLER funcDiagIntHandler);
static DWORD IntDisable   (WDC_DEVICE_HANDLE hDev);

static void DLLCALLCONV PLX_IntHandler   (PVOID pData);
static void PLX_EventHandler(WD_EVENT *pEvent, PVOID pData);
#endif

static DWORD LocalAddrSetMode(WDC_DEVICE_HANDLE hDev, PLX_ADDR addrSpace,
    DWORD dwLocalAddr);

static DWORD EEPROM_VPD_EnableAccess         (WDC_DEVICE_HANDLE hDev, UINT32 *pu32DataOld);
static DWORD EEPROM_VPD_RestoreAccess        (WDC_DEVICE_HANDLE hDev, UINT32 u32Data);
static DWORD EEPROM_VPD_RemoveWriteProtection(WDC_DEVICE_HANDLE hDev, WORD   wAddr,
                                              PBYTE pbDataOld);
static DWORD EEPROM_VPD_RestoreWriteProtection(WDC_DEVICE_HANDLE Dev, WORD wAddr);
static void EEPROM_RT_ChipSelect        (WDC_DEVICE_HANDLE hDev, BOOL fSelect);
static void EEPROM_RT_ReadBit           (WDC_DEVICE_HANDLE hDev, BOOL *pBit);
static void EEPROM_RT_WriteBit          (WDC_DEVICE_HANDLE hDev, BOOL bit);
static void EEPROM_RT_WriteEnableDisable(WDC_DEVICE_HANDLE hDev, BOOL fEnable);
static inline BOOL IsValidDevice(PWDC_DEVICE pDev, const CHAR *sFunc)
{
    if (!pDev || !(PPLX_DEV_CTX)(pDev->pCtx))
    {
        snprintf(gsPLX_LastErr, sizeof(gsPLX_LastErr) - 1, "%s: NULL device %s\n",
            sFunc, !pDev ? "handle" : "context");
        ErrLog(gsPLX_LastErr);
        return FALSE;
    }

    return TRUE;
}

BOOL PLX_IsMaster(WDC_DEVICE_HANDLE hDev)
{
    PPLX_DEV_CTX pDevCtx = (PPLX_DEV_CTX)WDC_GetDevContext(hDev);
    return pDevCtx->fIsMaster;
}//PLX_IsMaster

BOOL PLX_IsUseInt(WDC_DEVICE_HANDLE hDev)
{
    PPLX_DEV_CTX pDevCtx = (PPLX_DEV_CTX)WDC_GetDevContext(hDev);
    return pDevCtx->fIsUseInt;
}//PLX_IsUseInt

#if !defined(__KERNEL__)
/* -----------------------------------------------
    Device open/close
   ----------------------------------------------- */
DWORD PLX_DeviceOpenBySlot(WDC_DEVICE_HANDLE* pDeviceHandle,
                           const WD_PCI_SLOT* pSlot,
                           BOOL               fIsMaster,
                           BOOL               fIsUseInt)
{
#if defined(USE_KERNEL_PLUGIN)
    KP_EDEN_VERSION       kpVer;
#endif //USE_KERNEL_PLUGIN

    DWORD                dwStatus   = 0;
    WD_PCI_CARD_INFO     deviceInfo;
    PPLX_DEV_CTX         pDevCtx    = NULL;
	WDC_DEVICE_HANDLE    hDev       = NULL;
	PWDC_DEVICE 		 pDev;
	PLX_DEV_ADDR_DESC    devAddrDesc;
	
    /* Retrieve the device's resources information */
    BZERO(deviceInfo);
    deviceInfo.pciSlot = *pSlot;
    dwStatus = WDC_PciGetDeviceInfo(&deviceInfo);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("PLX_DeviceOpenBySlot: Failed retrieving the device's resources information.\n"
            "Error 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
        return dwStatus;
    }

    /* NOTE: You can modify the device's resources information here, if
       necessary (mainly the deviceInfo.Card.Items array or the items number -
       deviceInfo.Card.dwItems) in order to register only some of the resources
       or register only a portion of a specific address space, for example. */
////////////////////////////////////////////////////////////////////////////////
    if (!fIsUseInt)                                                           //
    {                                                                         //
        DWORD i;                                                              //
        // Remove interrupt item if not needed                                //
        for (i=0; i<deviceInfo.Card.dwItems; i++)                             //
        {                                                                     //
            WD_ITEMS *pItem = &deviceInfo.Card.Item[i];                       //
            if (pItem->item==ITEM_INTERRUPT)                                  //
                pItem->item = ITEM_NONE;                                      //
        }                                                                     //
    }                                                                         //
    else                                                                      //
    {                                                                         //
        DWORD i;                                                              //
        // make interrupt resource sharable                                   //
        for (i=0; i<deviceInfo.Card.dwItems; i++)                             //
        {                                                                     //
            WD_ITEMS *pItem = &deviceInfo.Card.Item[i];                       //
            if (pItem->item==ITEM_INTERRUPT)                                  //
                pItem->fNotSharable = FALSE;                                  //
        }                                                                     //
    }                                                                         //
////////////////////////////////////////////////////////////////////////////////

    /* Allocate memory for the PLX device context */
    pDevCtx = (PPLX_DEV_CTX)malloc(sizeof (PLX_DEV_CTX));
    if (!pDevCtx)
    {
        ErrLog("PLX_DeviceOpenBySlot: Failed allocating memory for PLX device context\n");
        return WD_INSUFFICIENT_RESOURCES;
    }

    BZERO(*pDevCtx);
	/* Open a WDC device handle */

	dwStatus = WDC_PciDeviceOpen(&hDev, &deviceInfo, pDevCtx, NULL, NULL, NULL);

    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed opening a WDC device handle. Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));
        goto Error;
    }

	pDev = hDev;

	devAddrDesc.dwNumAddrSpaces = pDev->dwNumAddrSpaces;
	devAddrDesc.pAddrDesc       = pDev->pAddrDesc;

	/* Open a handle to a Kernel PlugIn driver */
    WDC_KernelPlugInOpen(hDev, KP_PLX_DRIVER_NAME, &devAddrDesc);

    /* Validate device information */
    if (!IsDeviceValid(hDev))
    {
        dwStatus = WD_INVALID_PARAMETER;
        goto Error;
    }

    /* Initialize device context and validate device information */
    dwStatus = DeviceInit(hDev, fIsMaster, fIsUseInt);
    if (dwStatus)
        goto Error;

#if defined (USE_KERNEL_PLUGIN)
    dwStatus = PLX_CallKerPlug(hDev, KP_EDEN_MSG_VERSION, &kpVer);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed sending a 'Get Version' message (0x%x) to the Kernel PlugIn driver (%s).\n"
            "Error 0x%lx - %s\n",
			KP_EDEN_MSG_VERSION, KP_PLX_DRIVER_NAME, dwStatus, Stat2Str(dwStatus));
		goto Error;
    }
    else
    {
        printf("Using %s Kernel-Plugin driver version %ld.%02ld - %s\n",
            KP_PLX_DRIVER_NAME, kpVer.dwVer/100, kpVer.dwVer%100, kpVer.cVer);
    }

    dwStatus = PLX_CallKerPlug(hDev, KP_EDEN_MSG_WDC_HANDLE, hDev);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed sending a 'Handle Version' message (0x%x) to the Kernel PlugIn driver (%s).\n"
            "Error 0x%lx - %s\n",
			KP_EDEN_MSG_WDC_HANDLE, KP_PLX_DRIVER_NAME, dwStatus, Stat2Str(dwStatus));
		goto Error;	
    }

#endif

	*pDeviceHandle = hDev;

    /* Return handle to the new device */
    TraceLog("PLX_DeviceOpenBySlot: Opened a PLX device (handle 0x%p)\n", hDev);
    return dwStatus;

Error:
    if (hDev)
        PLX_DeviceClose(hDev);
    else
        free(pDevCtx);

    return dwStatus;
}//PLX_DeviceOpenBySlot


DWORD PLX_DeviceOpen(WDC_DEVICE_HANDLE* pDeviceHandle,
                     DWORD              dwVendorID,
                     DWORD              dwDeviceID,
                     DWORD              nCardNum,
                     BOOL               fIsMaster,
                     BOOL               fIsUseInt)
{
    DWORD dwStatus;
    WDC_PCI_SCAN_RESULT scanResult;
    WD_PCI_SLOT *slot;
    
    BZERO(scanResult);
    dwStatus = WDC_PciScanDevices(dwVendorID, dwDeviceID, &scanResult);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("PLX_DeviceOpen: Failed scanning the PCI bus.\n"
            "Error: 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
        return dwStatus;
    }

    if (!scanResult.dwNumDevices || nCardNum >= scanResult.dwNumDevices)
    {
        ErrLog("PLX_DeviceOpen: No matching PLX device was found for search criteria "
            "(Vendor ID 0x%lX, Device ID 0x%lX, Card Nmuber %ld)\n",
            dwVendorID, dwDeviceID, nCardNum);
        return WD_DEVICE_NOT_FOUND;
    }

    slot = &scanResult.deviceSlot[nCardNum];

    return PLX_DeviceOpenBySlot(pDeviceHandle, slot, fIsMaster, fIsUseInt);
}//PLX_DeviceOpen

DWORD PLX_DeviceClose(WDC_DEVICE_HANDLE hDev)
{
	DWORD dwStatus   = WD_STATUS_SUCCESS;
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
    PPLX_DEV_CTX pDevCtx;
    
    TraceLog("PLX_DeviceClose entered. Device handle: 0x%p\n", hDev);

    if (!hDev)
    {
        ErrLog("PLX_DeviceClose: Error - NULL device handle\n");
        return dwStatus;
    }

    pDevCtx = (PPLX_DEV_CTX)WDC_GetDevContext(pDev);
    
    /* Disable interrupts */
    if (WDC_IntIsEnabled(hDev))
    {
        dwStatus = PLX_IntDisable(hDev);
		if (WD_STATUS_SUCCESS != dwStatus)
        {
            ErrLog("Failed disabling interrupts. Error 0x%lx - %s\n",
				dwStatus, Stat2Str(dwStatus));
			return dwStatus;	
        }
    }
    /*
    fixme
        if (hPlx->kerPlug.hKernelPlugIn)
        WD_KernelPlugInClose(hPlx->hWD, &hPlx->kerPlug);

    */
    /* Close the device */
    dwStatus = WDC_PciDeviceClose(hDev);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed closing a WDC device handle (0x%p). Error 0x%lx - %s\n",
			hDev, dwStatus, Stat2Str(dwStatus));
		return dwStatus;	
    }

    /* Free PLX device context memory */
    if (pDevCtx)
		free(pDevCtx);
	return dwStatus;	
}//PLX_DeviceClose


/* Interrupts */
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
DWORD PLX_IntEnable(WDC_DEVICE_HANDLE hDev,
                    PLX_INT_HANDLER   funcDiagIntHandler,
                    PVOID             pData)
{
    DWORD       dwStatus;
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;

    TraceLog("PLX_IntEnable entered. Device handle: 0x%p\n", hDev);

    if (!IsValidDevice(pDev, "PLX_IntEnable"))
        return WD_INVALID_PARAMETER;

    if (!PLX_IsUseInt(hDev))
        return WD_WINDRIVER_STATUS_ERROR;

    /* Check if interrupts are already enabled */
    if (WDC_IntIsEnabled(hDev))
    {
        ErrLog("Interrupts are already enabled ...\n");
        return WD_OPERATION_ALREADY_DONE;
    }

    dwStatus = IntEnable(pDev, funcDiagIntHandler);

    if (WD_STATUS_SUCCESS == dwStatus)
        TraceLog("PLX_IntEnable: Interrupts enabled\n");

    return dwStatus;
}//PLX_IntEnable
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"

DWORD PLX_IntDisable(WDC_DEVICE_HANDLE hDev)
{
    DWORD        dwStatus;
    PWDC_DEVICE  pDev = (PWDC_DEVICE)hDev;

#if !defined(USE_KERNEL_PLUGIN)
    PPLX_DEV_CTX pDevCtx;
#endif    

    TraceLog("PLX_IntDisable entered. Device handle: 0x%p\n", hDev);

    if (!IsValidDevice(pDev, "PLX_IntDisable"))
        return WD_INVALID_PARAMETER;

#if !defined(USE_KERNEL_PLUGIN)
    pDevCtx = (PPLX_DEV_CTX)WDC_GetDevContext(hDev);
#endif
 
    if (!WDC_IntIsEnabled(hDev))
    {
        ErrLog("Interrupts are already disabled ...\n");
        return WD_OPERATION_ALREADY_DONE;
    }

    /* Physically disable the interrupts on the board */
    dwStatus = IntDisable(hDev);

    if (WD_STATUS_SUCCESS == dwStatus)
        TraceLog("PLX_IntEnable: Physically disabled the interrupts on the board\n");

    /* Disable the interrupts */
    dwStatus = WDC_IntDisable(hDev);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed disabling interrupts. Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));
    }

#if !defined(USE_KERNEL_PLUGIN)
    /* Free the memory allocated for the interrupt transfer commands */
    if (pDevCtx->pIntTransCmds)
    {
        free(pDevCtx->pIntTransCmds);
        pDevCtx->pIntTransCmds = NULL;
    }
#endif //USE_KERNEL_PLUGIN

    return dwStatus;
}//PLX_IntDisable


BOOL PLX_IntIsEnabled(WDC_DEVICE_HANDLE hDev)
{
    if (!IsValidDevice((PWDC_DEVICE)hDev, "PLX_IntIsEnabled"))
        return FALSE;

    return WDC_IntIsEnabled(hDev);
}//PLX_IntIsEnabled

/* Plug-and-play and power management events */
static void PLX_EventHandler(WD_EVENT *pEvent, PVOID pData)
{
    PWDC_DEVICE pDev = (PWDC_DEVICE)pData;
    PPLX_DEV_CTX pDevCtx = (PPLX_DEV_CTX)WDC_GetDevContext(pDev);

    TraceLog("PLX_EventHandler entered, pData 0x%p, dwAction 0x%lx\n",
        pData, pEvent->dwAction);
    
    /* Execute the diagnostics application's event handler function */
    pDevCtx->funcDiagEventHandler(pDev, pEvent->dwAction);
}//PLX_EventHandler

DWORD PLX_EventRegister(WDC_DEVICE_HANDLE hDev, PLX_EVENT_HANDLER funcEventHandler)
{
    DWORD dwStatus;
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
    PPLX_DEV_CTX pDevCtx;
    DWORD dwActions = WD_ACTIONS_ALL;
    /* TODO: Modify the above to set up the plug-and-play/power management
             events for which you wish to receive notifications.
             dwActions can be set to any combination of the WD_EVENT_ACTION
             flags defined in windrvr.h */

    TraceLog("PLX_EventRegister entered. Device handle: 0x%p\n", hDev);
    
    if (!IsValidDevice(pDev, "PLX_EventRegister"))
        return WD_INVALID_PARAMETER;

    pDevCtx = (PPLX_DEV_CTX)WDC_GetDevContext(hDev);

    /* Check if event is already registered */
    if (WDC_EventIsRegistered(hDev))
    {
        ErrLog("Events are already registered ...\n");
        return WD_OPERATION_ALREADY_DONE;
    }

    /* Store the diag event handler routine to be executed from PLX_EventHandler() upon an event */
    pDevCtx->funcDiagEventHandler = funcEventHandler;

    /* Register event */
#if defined(USE_KERNEL_PLUGIN)
    dwStatus = WDC_EventRegister(hDev, dwActions, PLX_EventHandler, hDev, TRUE);
#else
    dwStatus = WDC_EventRegister(hDev, dwActions, PLX_EventHandler, hDev, FALSE);
#endif //USE_KERNEL_PLUGIN

    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed to register events. Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));
        return dwStatus;
    }

    TraceLog("Events registered\n");

    return WD_STATUS_SUCCESS;
}//PLX_EventRegister

DWORD PLX_EventUnregister(WDC_DEVICE_HANDLE hDev)
{
    DWORD dwStatus;
    
    TraceLog("PLX_EventUnregister entered. Device handle: 0x%p\n", hDev);
    
    if (!IsValidDevice((PWDC_DEVICE)hDev, "PLX_EventUnregister"))
        return WD_INVALID_PARAMETER;

    if (!WDC_EventIsRegistered(hDev))
    {
        ErrLog("Cannot unregister events - no events currently registered ...\n");
        return WD_OPERATION_ALREADY_DONE;
    }

    dwStatus = WDC_EventUnregister(hDev);
    
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed to unregister events. Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));
    }

    return dwStatus;
}//PLX_EventUnregister

BOOL PLX_EventIsRegistered(WDC_DEVICE_HANDLE hDev)
{
    if (!IsValidDevice((PWDC_DEVICE)hDev, "PLX_EventIsRegistered"))
        return FALSE;

    return WDC_EventIsRegistered(hDev);
}//PLX_EventIsRegistered

#endif
/* Address spaces information */
DWORD PLX_GetNumAddrSpaces(WDC_DEVICE_HANDLE hDev)
{
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
    
    if (!IsValidDevice(pDev, "PLX_GetNumAddrSpaces"))
        return 0;

    return pDev->dwNumAddrSpaces;
}//PLX_GetNumAddrSpaces

BOOL PLX_GetAddrSpaceInfo(WDC_DEVICE_HANDLE hDev, PLX_ADDR_SPACE_INFO* pAddrSpaceInfo)
{
    PWDC_DEVICE    pDev       = (PWDC_DEVICE)hDev;
    WDC_ADDR_DESC* pAddrDesc;
    DWORD          dwAddrSpace, dwMaxAddrSpace;
    BOOL           fIsMemory;
    
    if (!IsValidDevice(pDev, "PLX_GetAddrSpaceInfo"))
        return FALSE;

#if defined(DEBUG)
    if (!pAddrSpaceInfo)
    {
        ErrLog("PLX_GetAddrSpaceInfo: Error - NULL address space information pointer\n");
        return FALSE;
    }
#endif

    dwAddrSpace    = pAddrSpaceInfo->dwAddrSpace;
    dwMaxAddrSpace = pDev->dwNumAddrSpaces - 1;

    if (dwAddrSpace > dwMaxAddrSpace)
    {
        ErrLog("PLX_GetAddrSpaceInfo: Error - Address space %ld is out of range (0 - %ld)\n",
                dwAddrSpace, dwMaxAddrSpace);
        return FALSE;
    }

    pAddrDesc = &pDev->pAddrDesc[dwAddrSpace];

    fIsMemory = WDC_ADDR_IS_MEM(pAddrDesc);
    
    snprintf(pAddrSpaceInfo->sName, MAX_NAME - 1, "BAR %ld", dwAddrSpace);
    snprintf(pAddrSpaceInfo->sType, MAX_TYPE - 1, fIsMemory ? "Memory" : "I/O");
        
    if (WDC_AddrSpaceIsActive(pDev, dwAddrSpace))
    {
        WD_ITEMS *pItem = &pDev->cardReg.Card.Item[pAddrDesc->dwItemIndex];
        DWORD dwAddr = fIsMemory ? pItem->I.Mem.dwPhysicalAddr : (DWORD)pItem->I.IO.dwAddr;
        
        snprintf(pAddrSpaceInfo->sDesc, MAX_DESC - 1, "0x%0*lX - 0x%0*lX (%ld bytes)",
            (int)WDC_SIZE_32 * 2, dwAddr,
            (int)WDC_SIZE_32 * 2, dwAddr + pAddrDesc->dwBytes - 1,
            pAddrDesc->dwBytes);
    }
    else
        snprintf(pAddrSpaceInfo->sDesc, MAX_DESC - 1, "Inactive address space");

    /* TODO: You can modify the code above to set a different address space name/description */

    return TRUE;
}//PLX_GetAddrSpaceInfo

/* Read/write local addresses */
#define MASK_LOCAL(hDev, addrSpace) (((PWDC_DEVICE)hDev)->pAddrDesc[addrSpace].dwBytes - 1)
static DWORD LocalAddrSetMode(WDC_DEVICE_HANDLE hDev,
                              PLX_ADDR          addrSpace,
                              DWORD             dwLocalAddr)
{
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
    DWORD dwLocalBase = (dwLocalAddr & ~MASK_LOCAL(pDev, addrSpace)) | BIT0;
    DWORD dwOffset = ((PPLX_DEV_CTX)(pDev->pCtx))->dwLAS0BA +
        4 * (addrSpace - PLX_ADDR_SPACE0);

    return WDC_WriteAddr32(pDev, PLX_ADDR_REG, dwOffset, (UINT32)dwLocalBase);
}//LocalAddrSetMode

DWORD PLX_ReadAddrLocalBlock(WDC_DEVICE_HANDLE   hDev,
                             PLX_ADDR            addrSpace,
                             DWORD               dwLocalAddr,
                             DWORD               dwBytes,
                             PVOID               pData,
                             WDC_ADDR_MODE       mode,
                             WDC_ADDR_RW_OPTIONS options)
{
    DWORD dwOffset = MASK_LOCAL(hDev, addrSpace) & dwLocalAddr;

    LocalAddrSetMode(hDev, addrSpace, dwLocalAddr);
    return WDC_ReadAddrBlock(hDev, addrSpace, dwOffset, dwBytes, pData,
        mode, options);
}//PLX_ReadAddrLocalBlock

DWORD PLX_WriteAddrLocalBlock(WDC_DEVICE_HANDLE   hDev,
                              PLX_ADDR            addrSpace,
                              DWORD               dwLocalAddr,
                              DWORD               dwBytes,
                              PVOID               pData,
                              WDC_ADDR_MODE       mode,
                              WDC_ADDR_RW_OPTIONS options)
{
    DWORD dwOffset = MASK_LOCAL(hDev, addrSpace) & dwLocalAddr;

    LocalAddrSetMode(hDev, addrSpace, dwLocalAddr);
    return WDC_WriteAddrBlock(hDev, addrSpace, dwOffset, dwBytes, pData,
        mode, options);
}//PLX_WriteAddrLocalBlock

DWORD PLX_ReadAddrLocal32(WDC_DEVICE_HANDLE hDev, PLX_ADDR addrSpace,
    DWORD dwLocalAddr, UINT32 *pu32Data)
{
    DWORD dwOffset = MASK_LOCAL(hDev, addrSpace) & dwLocalAddr;
    LocalAddrSetMode(hDev, addrSpace, dwLocalAddr);
    return WDC_ReadAddr32(hDev, addrSpace, dwOffset, pu32Data);
}//PLX_ReadAddrLocal32

DWORD PLX_WriteAddrLocal32(WDC_DEVICE_HANDLE hDev, PLX_ADDR addrSpace,
    DWORD dwLocalAddr, UINT32 u32Data)
{
    DWORD dwOffset = MASK_LOCAL(hDev, addrSpace) & dwLocalAddr;
    LocalAddrSetMode(hDev, addrSpace, dwLocalAddr);
    return WDC_WriteAddr32(hDev, addrSpace, dwOffset, u32Data);
}//PLX_WriteAddrLocal32

/*
 * Access the serial EEPROM ;
 * Verify that a blank or programmed serial EEPROM is present
 */
BOOL PLX_EEPROMIsPresent(WDC_DEVICE_HANDLE hDev)
{
    PPLX_DEV_CTX pDevCtx = (PPLX_DEV_CTX)WDC_GetDevContext(hDev);
    UINT32 u32CNTRL = 0;
    
    PLX_ReadReg32(hDev, pDevCtx->dwCNTRL, &u32CNTRL);
    return (u32CNTRL & BIT28) == BIT28;
}//PLX_EEPROMIsPresent

/* Access EEPROM via VPD (supported by PLX 9030, 9054, 9056, 9656) */
BOOL PLX_EEPROM_VPD_Validate(WDC_DEVICE_HANDLE hDev)
{
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
    BYTE bData = 0;

    TraceLog("PLX_EEPROM_VPD_Validate entered. Device handle 0x%p\n", hDev);
    
    if (!IsValidDevice(pDev, "PLX_EEPROM_VPD_Validate"))
        return FALSE;
    
    /* Verify that a blank or programmed serial EEPROM is present */
    if (!PLX_EEPROMIsPresent(hDev))
    {
        ErrLog("PLX_EEPROM_VPD_Validate: Error - serial EEPROM was not found on board "
            "(handle 0x%p)\n", hDev);
        return FALSE;
    }
        
    /* Check the next capability pointers */
    WDC_PciReadCfg8(hDev, PCI_CAP, &bData);
    if (bData != (BYTE)PLX_PMCAPID)
    {
        ErrLog("PLX_EEPROM_VPD_Validate: NEW_CAP register validation failed\n");
        return FALSE;
    }

    WDC_PciReadCfg8(hDev, PLX_PMNEXT, &bData);
    if (bData != (BYTE)PLX_HS_CAPID)
    {
        ErrLog("PLX_EEPROM_VPD_Validate: PMNEXT register validation failed\n");
        return FALSE;
    }

    WDC_PciReadCfg8(hDev, PLX_HS_NEXT, &bData);
    if (bData != (BYTE)PLX_VPD_CAPID)
    {
        ErrLog("PLX_EEPROM_VPD_Validate: HS_NEXT register validation failed\n");
        return FALSE;
    }

    WDC_PciReadCfg8(hDev, PLX_VPD_NEXT, &bData);
    if (bData != 0)
    {
        ErrLog("PLX_EEPROM_VPD_Validate: VPD_NEXT register validation failed\n");
        return FALSE;
    }

    return TRUE;
}//PLX_EEPROM_VPD_Validate

DWORD PLX_EEPROM_VPD_Read32(WDC_DEVICE_HANDLE hDev, DWORD dwOffset,
    UINT32 *pu32Data)
{
    DWORD i;
    UINT32 u32EnableAccess;
    WORD wAddr, wData;

    if (!IsValidDevice((PWDC_DEVICE)hDev, "PLX_EEPROM_VPD_Read32"))
        return WD_INVALID_PARAMETER;

    if (dwOffset % 4)
    {
        ErrLog("PLX_EEPROM_VPD_Read32: Error - offset (0x%lX) is not a multiple of 4 "
            "(device handle: 0x%p)\n", dwOffset, hDev);
        return WD_INVALID_PARAMETER;
    }

    /* Clear EEDO Input Enable */
    EEPROM_VPD_EnableAccess(hDev, &u32EnableAccess);

    /* Write a destination serial EEPROM address and flag of operation, value of 0 */
    wAddr = (WORD)(dwOffset & ~BIT15);
    WDC_PciWriteCfg16(hDev, PLX_VPD_ADDR, wAddr);

    /* Probe a flag of operation until it changes to a 1 to ensure the Read data is available */
    for (i = 0; i < 10; i++)
    {
        EEPROM_VPD_Delay();
        WDC_PciReadCfg16(hDev, PLX_VPD_ADDR, &wData);
        
        if (wData & BIT15)
            break;
    }
    
    if (i == 10)
    {
        ErrLog("PLX_EEPROM_VPD_Read32: Error - Acknowledge to EEPROM read was not received "
            "(device handle 0x%p)\n", hDev);
        return WD_OPERATION_FAILED;
    }

    /* Read back the requested data from PVPDATA register */
    WDC_PciReadCfg32(hDev, PLX_VPD_DATA, pu32Data);

    /* Restore EEDO Input Enable */
    EEPROM_VPD_RestoreAccess(hDev, u32EnableAccess);
    
    return WD_STATUS_SUCCESS;
}//PLX_EEPROM_VPD_Read32

DWORD PLX_EEPROM_VPD_Write32(WDC_DEVICE_HANDLE hDev, DWORD dwOffset,
    UINT32 u32Data)
{
    DWORD i;
    UINT32 u32ReadBack, u32EnableAccess;
    WORD wAddr, wData;
    BYTE bEnableOffset;

    if (!IsValidDevice((PWDC_DEVICE)hDev, "PLX_EEPROM_VPD_Write32"))
        return WD_INVALID_PARAMETER;

    if (dwOffset % 4)
    {
        ErrLog("PLX_EEPROM_VPD_Write32: Error - offset (0x%lX) is not a multiple of 4 "
            "(device handle: 0x%p)\n", dwOffset, hDev);
        return WD_INVALID_PARAMETER;
    }
    
    /* Clear EEDO Input Enable */
    EEPROM_VPD_EnableAccess(hDev, &u32EnableAccess);

    wAddr = (WORD)dwOffset;
    EEPROM_VPD_RemoveWriteProtection(hDev, wAddr, &bEnableOffset);

    EEPROM_VPD_Delay();

    /* Write desired data to PVPDATA register */
    WDC_PciWriteCfg32(hDev, PLX_VPD_DATA, u32Data);

    /* Write a destination serial EEPROM address and flag of operation, value of 1 */
    wAddr = (WORD)(wAddr | BIT15);
    WDC_PciWriteCfg16(hDev, PLX_VPD_ADDR, wAddr);

    /* Probe a flag of operation until it changes to a 0 to ensure the write completes */
    for (i = 0; i < 10; i++)
    {
        EEPROM_VPD_Delay();
        WDC_PciReadCfg16(hDev, PLX_VPD_ADDR, &wData);
        if (wData & BIT15)
            break;
    }

    EEPROM_VPD_RestoreWriteProtection((PWDC_DEVICE)hDev, bEnableOffset);

    /* Restore EEDO Input Enable */
    EEPROM_VPD_RestoreAccess(hDev, u32EnableAccess);

    PLX_EEPROM_VPD_Read32(hDev, dwOffset, &u32ReadBack);
  
    if (u32ReadBack != u32Data)
    {
        ErrLog("PLX_EEPROM_VPD_Write32: Error - Wrote 0x%08X, read back 0x%08X "
            "(device handle 0x%p)\n", u32Data, u32ReadBack);
        return WD_OPERATION_FAILED;
    }

    return WD_STATUS_SUCCESS;
}//PLX_EEPROM_VPD_Write32

/* Access EEPROM via run-time registers Supported by all PLX cards */
static DWORD EEPROM_VPD_EnableAccess(WDC_DEVICE_HANDLE hDev, UINT32 *pu32DataOld)
{
    DWORD dwCNTRL = ((PPLX_DEV_CTX)(((PWDC_DEVICE)hDev)->pCtx))->dwCNTRL;
    
    PLX_ReadReg32(hDev, dwCNTRL, pu32DataOld);
    return PLX_WriteReg32(hDev, dwCNTRL, *pu32DataOld & ~BIT31);
}//EEPROM_VPD_EnableAccess

/* Restore EEDO Input Enable */
static DWORD EEPROM_VPD_RestoreAccess(WDC_DEVICE_HANDLE hDev, UINT32 u32Data)
{
    return PLX_WriteReg32(hDev, ((PPLX_DEV_CTX)((((PWDC_DEVICE)hDev)->pCtx)))->dwCNTRL,
        u32Data);
}//EEPROM_VPD_RestoreAccess

static DWORD EEPROM_VPD_RemoveWriteProtection(WDC_DEVICE_HANDLE hDev, WORD wAddr,
    PBYTE pbDataOld)
{
    PPLX_DEV_CTX pDevCtx = (PPLX_DEV_CTX)WDC_GetDevContext(hDev);

    PLX_ReadReg8(hDev, pDevCtx->dwPROT_AREA, pbDataOld);
    
    wAddr /= 4;
    wAddr &= 0x7F;

    PLX_WriteReg8(hDev, pDevCtx->dwPROT_AREA, (BYTE)wAddr);

    *pbDataOld *= 4; /* Expand from DWORD to BYTE count */

    return WD_STATUS_SUCCESS;
}//EEPROM_VPD_RemoveWriteProtection

static DWORD EEPROM_VPD_RestoreWriteProtection(WDC_DEVICE_HANDLE hDev, WORD wAddr)
{    
    PPLX_DEV_CTX pDevCtx = (PPLX_DEV_CTX)WDC_GetDevContext(hDev);
    return PLX_WriteReg8(hDev, pDevCtx->dwPROT_AREA, (BYTE)wAddr);
}//EEPROM_VPD_RestoreWriteProtection

DWORD PLX_EEPROM_RT_Read16(WDC_DEVICE_HANDLE hDev,
                           DWORD             dwOffset,
                           PWORD             pwData,
                           DWORD             EEPROMmsb)
{
    WORD i;
    DWORD dwAddr = dwOffset >> 1;
    BOOL bit = 0;
    
    if (!IsValidDevice((PWDC_DEVICE)hDev, "PLX_EEPROM_RT_Read16"))
        return WD_INVALID_PARAMETER;

    *pwData = 0;

    EEPROM_RT_ChipSelect(hDev, TRUE);
    EEPROM_RT_WriteBit(hDev, 1);
    EEPROM_RT_WriteBit(hDev, 1);
    EEPROM_RT_WriteBit(hDev, 0);

    /* CS06, CS46 EEPROM - send 6bit address
     * CS56, CS66 EEPROM - send 8bit address */
    #pragma warn -8012 // Diasable warning Number 8012 "Comparing signed to unsigned values"
    for (i = (WORD)EEPROMmsb; i; i >>= 1)
        EEPROM_RT_WriteBit(hDev, (dwAddr & i) == i);
    #pragma warn .8012 // Enable warning Number 8012 "Comparing signed to unsigned values"
    for (i = BIT15; i; i >>= 1)
    {
        EEPROM_RT_ReadBit(hDev, &bit);
        *pwData |= (bit ? i : 0);
    }

    EEPROM_RT_ChipSelect(hDev, FALSE);

    return WD_STATUS_SUCCESS;
}//PLX_EEPROM_RT_Read16

DWORD PLX_EEPROM_RT_Write16(WDC_DEVICE_HANDLE hDev,
                            DWORD             dwOffset,
                            WORD              wData,
                            DWORD             EEPROMmsb)
{
    WORD i;
    DWORD dwAddr = dwOffset >> 1;
    WORD wReadBack;
    
    if (!IsValidDevice((PWDC_DEVICE)hDev, "PLX_EEPROM_RT_Write16"))
        return WD_INVALID_PARAMETER;

    EEPROM_RT_WriteEnableDisable(hDev, TRUE);

    EEPROM_RT_ChipSelect(hDev, TRUE);

    /* Send a PRWRITE instruction */
    EEPROM_RT_WriteBit(hDev, 1);
    EEPROM_RT_WriteBit(hDev, 0);
    EEPROM_RT_WriteBit(hDev, 1);
    
    /* CS06, CS46 EEPROM - send 6bit address
     * CS56, CS66 EEPROM - send 8bit address */
     #pragma warn -8012 // Diasable warning Number 8012 "Comparing signed to unsigned values"
    for (i = (WORD)EEPROMmsb; i; i >>= 1)
        EEPROM_RT_WriteBit(hDev, (dwAddr & i) == i);
    #pragma warn .8012 // Enable warning Number 8012 "Comparing signed to unsigned values"
    for (i = BIT15; i; i >>= 1)
        EEPROM_RT_WriteBit(hDev, (wData & i) == i);

    EEPROM_RT_ChipSelect(hDev, FALSE);
    
    EEPROM_RT_WriteEnableDisable(hDev, TRUE);

    PLX_EEPROM_RT_Read16(hDev, dwOffset, &wReadBack, EEPROMmsb);
    
    if (wData != wReadBack)
    {
        ErrLog("PLX_EEPROM_RT_Write16: Error - Wrote 0x%04X, read back 0x%04X "
            "(device handle 0x%p)\n", wData, wReadBack);
        return WD_OPERATION_FAILED;
    }
    
    return WD_STATUS_SUCCESS;
}//PLX_EEPROM_RT_Write16

DWORD PLX_EEPROM_RT_Read32(WDC_DEVICE_HANDLE hDev,
                           DWORD             dwOffset,
                           UINT32*           pu32Data,
                           DWORD             EEPROMmsb)
{
    WORD wData1, wData2;
    
    if (!IsValidDevice((PWDC_DEVICE)hDev, "PLX_EEPROM_RT_Read32"))
        return WD_INVALID_PARAMETER;

    if (dwOffset % 4)
    {
        ErrLog("PLX_EEPROM_RT_Read32: Error - offset (0x%lX) is not a multiple of 4 "
            "(device handle: 0x%p)\n", dwOffset, hDev);
        return WD_INVALID_PARAMETER;
    }
    
    PLX_EEPROM_RT_Read16(hDev, dwOffset, &wData1, EEPROMmsb);

    PLX_EEPROM_RT_Read16(hDev, dwOffset + 2, &wData2, EEPROMmsb);

    *pu32Data = (UINT32)((wData1 << 16) + wData2);

    TraceLog("PLX_EEPROM_RT_Read32: success, offset (0x%lX) , value (0x%lX)"
            "(device handle: 0x%p)\n", dwOffset, *pu32Data, hDev);

    return WD_STATUS_SUCCESS;
}//PLX_EEPROM_RT_Read32

DWORD PLX_EEPROM_RT_Write32(WDC_DEVICE_HANDLE hDev,
                            DWORD             dwOffset,
                            UINT32            u32Data,
                            DWORD             EEPROMmsb)
{
    WORD wData1, wData2;

    if (!IsValidDevice((PWDC_DEVICE)hDev, "PLX_EEPROM_RT_Write32"))
        return WD_INVALID_PARAMETER;

    if (dwOffset % 4)
    {
        ErrLog("PLX_EEPROM_RT_Write32: Error - offset (0x%lX) is not a multiple of 4 "
            "(device handle: 0x%p)\n", dwOffset, hDev);
        return WD_INVALID_PARAMETER;
    }

    wData1 = (WORD)(u32Data >> 16);
    wData2 = (WORD)(u32Data & 0xFFFF);

    PLX_EEPROM_RT_Write16(hDev, dwOffset, wData1, EEPROMmsb);

    PLX_EEPROM_RT_Write16(hDev, dwOffset + 2, wData2, EEPROMmsb);

    TraceLog("PLX_EEPROM_RT_Write32: success, offset (0x%lX) , value (0x%lX)"
            "(device handle: 0x%p)\n", dwOffset, u32Data,hDev);
    return WD_STATUS_SUCCESS;
}//PLX_EEPROM_RT_Write32

static void EEPROM_RT_ChipSelect(WDC_DEVICE_HANDLE hDev, BOOL fSelect)
{
    DWORD dwCNTRL = ((PPLX_DEV_CTX)(((PWDC_DEVICE)hDev)->pCtx))->dwCNTRL;
    UINT32 u32CNTRL;

    PLX_ReadReg32(hDev, dwCNTRL, &u32CNTRL);
    
    PLX_WriteReg32(hDev, dwCNTRL,
        fSelect ? (u32CNTRL | BIT25) : (u32CNTRL & ~BIT25) );
    
    EEPROM_RT_Delay();
}//EEPROM_RT_ChipSelect

static void EEPROM_RT_ReadBit(WDC_DEVICE_HANDLE hDev, BOOL *pBit)
{
    DWORD dwCNTRL = ((PPLX_DEV_CTX)(((PWDC_DEVICE)hDev)->pCtx))->dwCNTRL;
    UINT32 u32CNTRL;

    PLX_ReadReg32(hDev, dwCNTRL, &u32CNTRL);

    /* clock */
    PLX_WriteReg32(hDev, dwCNTRL, u32CNTRL & ~BIT24);

    EEPROM_RT_Delay();

    PLX_WriteReg32(hDev, dwCNTRL, u32CNTRL | BIT24);

    EEPROM_RT_Delay();

    PLX_WriteReg32(hDev, dwCNTRL, u32CNTRL & ~BIT24);

    EEPROM_RT_Delay();

    /* data */
    PLX_ReadReg32(hDev, dwCNTRL, &u32CNTRL);
    *pBit = (u32CNTRL & BIT27) == BIT27;
}//EEPROM_RT_ReadBit

static void EEPROM_RT_WriteBit(WDC_DEVICE_HANDLE hDev, BOOL bit)
{
    DWORD dwCNTRL = ((PPLX_DEV_CTX)(((PWDC_DEVICE)hDev)->pCtx))->dwCNTRL;
    UINT32 u32CNTRL;
    
    PLX_ReadReg32(hDev, dwCNTRL, &u32CNTRL);
    
    if (bit) /* data */
        u32CNTRL |= BIT26;
    else
        u32CNTRL &= ~BIT26;

    /* clock */
    PLX_WriteReg32(hDev, dwCNTRL, u32CNTRL & ~BIT24);

    EEPROM_RT_Delay();

    PLX_WriteReg32(hDev, dwCNTRL, u32CNTRL | BIT24);

    EEPROM_RT_Delay();

    PLX_WriteReg32(hDev, dwCNTRL, u32CNTRL & ~BIT24);

    EEPROM_RT_Delay();
}//EEPROM_RT_WriteBit

static void EEPROM_RT_WriteEnableDisable(WDC_DEVICE_HANDLE hDev, BOOL fEnable)
{
    EEPROM_RT_ChipSelect(hDev, TRUE);

    /* Send a WEN instruction */
    EEPROM_RT_WriteBit(hDev, 1);
    EEPROM_RT_WriteBit(hDev, 0);
    EEPROM_RT_WriteBit(hDev, 0);
    EEPROM_RT_WriteBit(hDev, fEnable ? 1 : 0);
    EEPROM_RT_WriteBit(hDev, fEnable ? 1 : 0);

    EEPROM_RT_WriteBit(hDev, 0);
    EEPROM_RT_WriteBit(hDev, 0);
    EEPROM_RT_WriteBit(hDev, 0);
    EEPROM_RT_WriteBit(hDev, 0);

    EEPROM_RT_ChipSelect(hDev, FALSE);
}//EEPROM_RT_WriteEnableDisable

/* Definition of abort bits in commad (CMD) configuration register */
#define PLX_M_CR_ABORT_BITS (\
    BIT24   /* Detected Master Data Parity Error */ \
    | BIT27 /* Signaled Target Abort */ \
    | BIT28 /* Received Target Abort */ \
    | BIT29 /* Received Master Abort */ \
    | BIT30 /* Signaled System Error */ \
    | BIT31 /* Detected Parity Error on PCI bus */ \
    )
 /*
 * Reset of PLX board ;
 * Software board reset for master devices (9054, 9056, 9080, 9656)
 */
void PLX_SoftResetMaster(WDC_DEVICE_HANDLE hDev)
{
    BOOL   fMUEnabled, fEEPROMPresent;
    WORD   wCMD       = 0;
    UINT32 u32QSR     = 0;
    UINT32 u32CNTRL   = 0;
    UINT32 u32INTLN   = 0;
    UINT32 u32MBOX0   = 0, u32MBOX1 = 0;
    UINT32 u32HS_CNTL = 0;
    UINT32 u32PMCSR   = 0;
    
    /* Clear any PCI errors */
    WDC_PciReadCfg16(hDev, PCI_CR, &wCMD);
    if (wCMD & PLX_M_CR_ABORT_BITS)
    {
        /* Write value back to clear aborts */
        WDC_PciWriteCfg16(hDev, PCI_CR, wCMD);
    }

    /* Save state of I2O Decode Enable */
    PLX_ReadReg32(hDev, PLX_M_QSR, &u32QSR);
    fMUEnabled = u32QSR & BIT0 ? TRUE : FALSE;

    /* Make sure S/W Reset & EEPROM reload bits are clear */
    PLX_ReadReg32(hDev, PLX_M_CNTRL, &u32CNTRL);
    u32CNTRL &= ~(BIT30 | BIT29);

    /* Determine if an EEPROM is present */
    fEEPROMPresent = PLX_EEPROMIsPresent(hDev);

    /* Save some registers if EEPROM present */
    if (fEEPROMPresent)
    {
        /* Run-time registers */
        PLX_ReadReg32(hDev, PLX_M_MBOX0, &u32MBOX0);
        PLX_ReadReg32(hDev, PLX_M_MBOX1, &u32MBOX1);

        /* PCI configuration registers */       
        WDC_PciReadCfg32(hDev, PCI_ILR,      &u32INTLN);
        WDC_PciReadCfg32(hDev, PLX_HS_CAPID, &u32HS_CNTL);
        WDC_PciReadCfg32(hDev, PLX_PMCSR,    &u32PMCSR);
    }

    /* Issue Software Reset to hold PLX chip in reset */
    PLX_WriteReg32(hDev, PLX_M_CNTRL, u32CNTRL | BIT30);

    /* Bring chip out of reset */
    PLX_SOFT_RESET_DELAY();
    PLX_WriteReg32(hDev, PLX_M_CNTRL, u32CNTRL);

    /* Issue EEPROM reload in case now programmed */
    PLX_WriteReg32(hDev, PLX_M_CNTRL, u32CNTRL | BIT29);

    /* Clear EEPROM reload */
    PLX_SOFT_RESET_DELAY();
    PLX_WriteReg32(hDev, PLX_M_CNTRL, u32CNTRL);

    /* Restore I2O Decode Enable state */
    if (fMUEnabled)
    {
        PLX_ReadReg32(hDev, PLX_M_QSR, &u32QSR);
        PLX_WriteReg32(hDev, PLX_M_QSR, u32QSR | BIT0);
    }

    /* If EEPROM was present, restore registers */
    if (fEEPROMPresent)
    {
        /* Run-time registers */
        PLX_WriteReg32(hDev, PLX_M_MBOX0, u32MBOX0);
        PLX_WriteReg32(hDev, PLX_M_MBOX1, u32MBOX1);

        /* PCI configuration registers */
        WDC_PciReadCfg32(hDev, PCI_ILR, &u32INTLN);

        /* Mask out HS bits that can be cleared */
        u32HS_CNTL &= ~(BIT23 | BIT22 | BIT17);
        WDC_PciWriteCfg32(hDev, PLX_HS_CAPID, u32HS_CNTL);

        /* Mask out PM bits that can be cleared */
        u32PMCSR &= ~BIT15;
        WDC_PciWriteCfg32(hDev, PLX_PMCSR, u32PMCSR);
    }
}//PLX_SoftResetMaster

/* Get last PLX library error string */
DWORD PLX_DeviceFind(DWORD dwVendorId, DWORD dwDeviceId, WD_PCI_SLOT *pSlot)
{
	DWORD               dwStatus;
    WDC_PCI_SCAN_RESULT scanResult;

    BZERO(scanResult);
    dwStatus = WDC_PciScanDevices(dwVendorId, dwDeviceId, &scanResult);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("PLX_DIAG_DeviceFind: Failed scanning the PCI bus.\n"
            "Error: 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
		return dwStatus;
    }

    if (!scanResult.dwNumDevices)
    {
        printf("No matching PLX device was found for search criteria "
            "(Vendor ID 0x%lX, Device ID 0x%lX)\n",
            dwVendorId, dwDeviceId);

		return dwStatus;
    }

    *pSlot = scanResult.deviceSlot[0];

    return dwStatus;
}//PLX_DeviceFind

#if defined(USE_KERNEL_PLUGIN)
/* -------------------------------------------------------------------------- */
// Function: PLX_CallKerPlug()
//   Pass data to/from the Kerenl-Plugin
// Parameters:
//   hEDENPCI [in] handle to the PCI card as received from EDENPCI_Open().
//   dwMessage [in] the message number to be handled by the Kernel-Plugin.
//   pData [in/out] the buffer of the data to pass to/from the Kernel-Plugin.
// Return Value:
//   The result of the processing of the message, as filled by the Kernel-Plugin.
/* -------------------------------------------------------------------------- */
DWORD PLX_CallKerPlug(WDC_DEVICE_HANDLE hplx, DWORD dwMessage, PVOID pData)
{
	DWORD dwStatus, dwKPResult = 0;
	
	if(! hplx)
		return WD_INVALID_HANDLE;

    dwStatus = WDC_CallKerPlug (hplx, dwMessage, pData, &dwKPResult) ;
    if (KP_STATUS_OK != dwKPResult)
    {
		 dwStatus = WD_KERPLUG_FAILURE;
    }
    return dwStatus;
}//PLX_CallKerPlug
#endif


static BOOL IsDeviceValid(const PWDC_DEVICE pDev)
{
    DWORD i, dwNumAddrSpaces = pDev->dwNumAddrSpaces;

    /* TODO: Modify the implementation of this function in order to verify
             that your device has all expected resources */

    /* Verify that the device has at least one active address space */
    for (i = 0; i < dwNumAddrSpaces; i++)
    {
        if (WDC_AddrSpaceIsActive(pDev, i))
            return TRUE;
    }
    
    ErrLog("Device does not have any active memory or I/O address spaces\n");
    return FALSE;
}//IsDeviceValid


static DWORD DeviceInit(PWDC_DEVICE pDev, BOOL fIsMaster, BOOL fIsUseInt)
{
    PPLX_DEV_CTX pDevCtx = (PPLX_DEV_CTX)WDC_GetDevContext(pDev);
    
    /* NOTE: You can modify the implementation of this function in order to
             perform any additional device initialization you require */
    
    pDevCtx->fIsUseInt = fIsUseInt;
    
    /* Set device type - master/target */
    pDevCtx->fIsMaster = fIsMaster;

    /* Set offsets for some registers */
    /* Offsets of DMA registers will be set in PLX_DMAOpen() */
    if (fIsMaster)
    {
        pDevCtx->dwINTCSR    = PLX_M_INTCSR;
        pDevCtx->dwCNTRL     = PLX_M_CNTRL;
        pDevCtx->dwPROT_AREA = PLX_M_PROT_AREA;
        pDevCtx->dwLAS0BA    = PLX_M_LAS0BA;
    }
    else
    {
        pDevCtx->dwINTCSR    = PLX_T_INTCSR;
        pDevCtx->dwCNTRL     = PLX_T_CNTRL;
        pDevCtx->dwPROT_AREA = PLX_T_PROT_AREA;
        pDevCtx->dwLAS0BA    = PLX_T_LAS0BA;
    }

    /* Enable target abort for master devices */
    if (fIsMaster)
    {
        UINT32 u32IntStatus = 0;

        PLX_ReadReg32 (pDev, PLX_M_INTCSR, &u32IntStatus);
        PLX_WriteReg32(pDev, PLX_M_INTCSR, u32IntStatus | BIT12);
    }

    return WD_STATUS_SUCCESS;
}//DeviceInit

static DWORD IntEnable(PWDC_DEVICE pDev, PLX_INT_HANDLER funcDiagIntHandler)
{
    DWORD          dwStatus;
    PPLX_DEV_CTX   pDevCtx = (PPLX_DEV_CTX)WDC_GetDevContext(pDev);
    WDC_ADDR_DESC* pAddrDesc;
    DWORD          dwINTCSR = 0;
//    int            i;
#if !defined(USE_KERNEL_PLUGIN)
    WD_TRANSFER*   pTrans;

    /* Define the number of interrupt transfer commands to use */
    const int NUM_TRANS_CMDS_TARGET = 2;

    /* Allocate memory for the interrupt transfer commands */
    pTrans = (WD_TRANSFER*)calloc(NUM_TRANS_CMDS_TARGET, sizeof(WD_TRANSFER));
    if (!pTrans)
    {
        ErrLog("Failed allocating memory for interrupt transfer commands\n");
        return WD_INSUFFICIENT_RESOURCES;
    }

    /* Read the value of the INTCSR register */
    dwStatus = PLX_ReadReg32(pDev, pDevCtx->dwINTCSR, &dwINTCSR);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("IntEnable: Failed reading from the INTCSR register "
            "(vid 0x%lX did 0x%lX, handle 0x%p)\n"
            "Error 0x%lx - %s\n",
            pDevCtx->id.dwVendorId, pDevCtx->id.dwDeviceId, pDev, dwStatus, Stat2Str(dwStatus));
        goto Error;
    }

    /* Prepare the interrupt transfer commands */
    /* The transfer commands will be executed by WinDriver in the kernel
       for each interrupt that is received */

    i = 0;

    /* Read status from the INTCSR register */
    pAddrDesc = &pDev->pAddrDesc[PLX_ADDR_REG];
    pTrans[i].dwPort      = pAddrDesc->kptAddr + pDevCtx->dwINTCSR;
    pTrans[i].cmdTrans    = WDC_ADDR_IS_MEM(pAddrDesc) ? RM_DWORD : RP_DWORD;
    pDevCtx->bIntCsrIndex = i;
    i++;

    /* Write to the INTCSR register to clear the interrupt */
    pTrans[i].dwPort     = pTrans[pDevCtx->bIntCsrIndex].dwPort;
    pTrans[i].cmdTrans   = WDC_ADDR_IS_MEM(pAddrDesc) ? WM_DWORD : WP_DWORD;
    pTrans[i].Data.Dword = dwINTCSR & ~(BIT8 | BIT10 | BIT11); /*fixme*/
    i++;
#endif
    /* Store the diag interrupt handler routine, which will be executed by
       PLX_IntHandler() when an interrupt is received */
    pDevCtx->funcDiagIntHandler = funcDiagIntHandler;

    /* Enable the interrupts */
#if !defined(USE_KERNEL_PLUGIN)
    dwStatus = WDC_IntEnable(pDev, pTrans, i, INTERRUPT_CMD_COPY,
        PLX_IntHandler, (PVOID)pDev, WDC_IS_KP(pDev));
#else
    dwStatus = WDC_IntEnable(pDev, NULL, 0, 0,
        PLX_IntHandler, (PVOID)pDev, WDC_IS_KP(pDev));
#endif        
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("IntEnable: Failed enabling interrupts (vid 0x%lX did 0x%lX, handle 0x%p).\n"
            "Error 0x%lx - %s\n",
            pDevCtx->id.dwVendorId, pDevCtx->id.dwDeviceId, pDev, dwStatus, Stat2Str(dwStatus));

        goto Error;
    }
#if !defined(USE_KERNEL_PLUGIN)
    /* Store the interrupt transfer commands in the device context */
    pDevCtx->pIntTransCmds = pTrans;

#endif

    /* Physically enable the interrupts on the board */
    dwStatus = PLX_WriteReg32(pDev, pDevCtx->dwINTCSR, (DWORD)(dwINTCSR |BIT8|BIT10));
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("IntEnable: Faild writing to the INTCSR register to physically enable "
            "the interrupts on the board (vid 0x%lX did 0x%lX, handle 0x%p)\n"
        "Error 0x%lx - %s\n",
        pDevCtx->id.dwVendorId, pDevCtx->id.dwDeviceId, pDev, dwStatus, Stat2Str(dwStatus));
        goto Error;
    }


    return WD_STATUS_SUCCESS;

Error:
#if !defined(USE_KERNEL_PLUGIN)
    if (pTrans)
    {
        free(pTrans);
   }
#endif
    return dwStatus;
}//IntEnable

static DWORD IntDisable(WDC_DEVICE_HANDLE hDev)
{
    DWORD dwINTCSR = 0;
    #pragma warn -8075 // Disable warning Number 8075 "Suspicious pointer conversion"
    PLX_ReadReg32(hDev,  PLX_M_INTCSR, &dwINTCSR);
    #pragma warn .8075 // Enable warning Number 8075 "Suspicious pointer conversion"
    PLX_WriteReg32(hDev, PLX_M_INTCSR, (DWORD)(dwINTCSR & ~(BIT8 | BIT10)));

    return WD_STATUS_SUCCESS;
}//IntDisable

static void DLLCALLCONV PLX_IntHandler(PVOID pData)
{
    /* TODO: Modify the interrupt handler code to suit your specific needs */
    
    PWDC_DEVICE    pDev    = (PWDC_DEVICE)pData;
    PPLX_DEV_CTX   pDevCtx = (PPLX_DEV_CTX)WDC_GetDevContext(pDev);
    PLX_INT_RESULT intResult;

    BZERO(intResult);
#if !defined(USE_KERNEL_PLUGIN)    
    intResult.u32INTCSR  = pDevCtx->pIntTransCmds[pDevCtx->bIntCsrIndex].Data.Dword;
#endif    
    intResult.dwCounter  = pDev->Int.dwCounter;
    intResult.dwLost     = pDev->Int.dwLost;
    intResult.waitResult = (WD_INTERRUPT_WAIT_RESULT)pDev->Int.fStopped;
    
    /* Execute the diagnostics application's interrupt handler routine */
    pDevCtx->funcDiagIntHandler(pDev, &intResult);
}//PLX_IntHandler


