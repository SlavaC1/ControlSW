#ifndef _EDEN_LIB_H_
#define _EDEN_LIB_H_

/************************************************************************
*  File: eden_lib.h
*
*  Library for accessing EDEN devices using a Kernel PlugIn driver.
*  The code accesses hardware using WinDriver's WDC library.
*  Code was generated by DriverWizard v10.40.
*
*  Jungo Confidential. Copyright (c) 2011 Jungo Ltd.  http://www.jungo.com
*************************************************************************/

#include "wdc_lib.h"
#include "wdc_defs.h"
#include "../../../samples/shared/pci_regs.h"
#include "../../../samples/shared/bits.h"



#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************
  General definitions
 *************************************************************/
/* Kernel PlugIn driver name (should be no more than 8 characters) */
#define KP_EDEN_DRIVER_NAME "KP_EDENP"
#define KP_EDEN_DRIVER_VERSION 2

/* Kernel PlugIn messages - used in WDC_CallKerPlug() calls (user mode) / KP_EDEN_Call() (kernel mode) */
enum 
{
    KP_EDEN_MSG_VERSION = 0,
	KP_EDEN_MSG_WDC_HANDLE,
	KP_EDEN_MSG_BUFFER_ADDR,			/* Send the address of the non paged buffer */
	KP_EDEN_MSG_BUFFER_LENGTH,
	KP_EDEN_MSG_GET_INTERRUPT_MSG,
    KP_EDEN_MSG_NUM
};

/* Kernel PlugIn messages status */
enum {
    KP_EDEN_STATUS_OK = 0x1,
    KP_EDEN_STATUS_MSG_NO_IMPL = 0x1000,
};

/* Default vendor and device IDs */
#define EDEN_DEFAULT_VENDOR_ID 0x11AE /* Vendor ID */
#define EDEN_DEFAULT_DEVICE_ID 0x464E /* Device ID */


/* Kernel PlugIn version information struct */
typedef struct {
    DWORD dwVer;
    CHAR cVer[100];
} KP_EDEN_VERSION;

/* Device address description struct */
typedef struct {
    DWORD dwNumAddrSpaces;    /* Total number of device address spaces */
    WDC_ADDR_DESC *pAddrDesc; /* Array of device address spaces information */
	long		    bufferLength;
	KPTR            pNonPagedBuffer;
	DWORD*          pBuffer;
	BOOL            bFIFOStartFlag;
	BYTE            intMsg;
} EDEN_DEV_ADDR_DESC;

/* Address space information struct */
#define MAX_TYPE 8
typedef struct {
    DWORD dwAddrSpace;
    CHAR sType[MAX_TYPE];
    CHAR sName[MAX_NAME];
    CHAR sDesc[MAX_DESC];
} EDEN_ADDR_SPACE_INFO;

/* Interrupt result information struct */
typedef struct
{
    DWORD dwCounter; /* Number of interrupts received */
    DWORD dwLost;    /* Number of interrupts not yet handled */
    WD_INTERRUPT_WAIT_RESULT waitResult; /* See WD_INTERRUPT_WAIT_RESULT values in windrvr.h */
    DWORD dwEnabledIntType; /* Interrupt type that was actually enabled
                               (MSI/MSI-X/Level Sensitive/Edge-Triggered) */
    DWORD dwLastMessage; /* Message data of the last received MSI/MSI-X
                        (Windows Vista/7); N/A to line-based interrupts) */
} EDEN_INT_RESULT;
/* TODO: You can add fields to EDEN_INT_RESULT to store any additional
         information that you wish to pass to your diagnostics interrupt
         handler routine (DiagIntHandler() in eden_diag.c) */

/* EDEN diagnostics interrupt handler function type */
typedef void (*EDEN_INT_HANDLER)(WDC_DEVICE_HANDLE hDev,
    EDEN_INT_RESULT *pIntResult);

/* EDEN diagnostics plug-and-play and power management events handler function type */
typedef void (*EDEN_EVENT_HANDLER)(WDC_DEVICE_HANDLE hDev,
    DWORD dwAction);

/*************************************************************
  Function prototypes
 *************************************************************/
DWORD EDEN_LibInit(void);
DWORD EDEN_LibUninit(void);

#if !defined(__KERNEL__)
WDC_DEVICE_HANDLE EDEN_DeviceOpen(const WD_PCI_CARD_INFO *pDeviceInfo);
BOOL EDEN_DeviceClose(WDC_DEVICE_HANDLE hDev);

static BOOL IsItemExists(PWDC_DEVICE pDev, ITEM_TYPE item);
DWORD EDEN_IntEnable(WDC_DEVICE_HANDLE hDev, EDEN_INT_HANDLER funcIntHandler);
DWORD EDEN_IntDisable(WDC_DEVICE_HANDLE hDev);
BOOL EDEN_IntIsEnabled(WDC_DEVICE_HANDLE hDev);

DWORD EDEN_EventRegister(WDC_DEVICE_HANDLE hDev, EDEN_EVENT_HANDLER funcEventHandler);
DWORD EDEN_EventUnregister(WDC_DEVICE_HANDLE hDev);
BOOL EDEN_EventIsRegistered(WDC_DEVICE_HANDLE hDev);
#endif

DWORD EDEN_GetNumAddrSpaces(WDC_DEVICE_HANDLE hDev);
BOOL EDEN_GetAddrSpaceInfo(WDC_DEVICE_HANDLE hDev, EDEN_ADDR_SPACE_INFO *pAddrSpaceInfo);


const char *EDEN_GetLastErr(void);

#ifdef __cplusplus
}
#endif

#endif
