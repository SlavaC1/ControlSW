#include "plx_defs.h"
#include "FIFOPci_defs.h"
#include "status_strings.h"

#if defined (__KERNEL__)
    #include "kpstdlib.h"
#else
    #include <stdio.h>
    #include <stdarg.h>
#endif

#if defined(_DEBUG) && defined(USE_KERNEL_PLUGIN)
char msgStr[KP_EDEN_MSG_NUM][sizeof("KP_EDEN_MSG_GET_INTERRUPT_MSG")] =
{
	"KP_EDEN_MSG_VERSION",
	"KP_EDEN_MSG_WDC_HANDLE",
	"KP_EDEN_MSG_BUFFER_ADDR",
	"KP_EDEN_MSG_BUFFER_LENGTH",
	"KP_EDEN_MSG_GET_INTERRUPT_MSG"
};
#endif
#if defined(USE_KERNEL_PLUGIN) && !defined(__KERNEL__)
WD_DMA gDma;
BYTE*  NonPagedBuffer = NULL;
#endif /* USE_KERNEL_PLUGIN */


#if !defined(__KERNEL__)

BOOL	       gbFIFOStartFlag                = TRUE;
unsigned long* BufferForIntrPtr                = NULL; /* PTR to the Layer buffer */
void*          BufferLayerRcvPtr               = NULL;
long	       BufferSizeForIntr               = 0;    /* in DWORDS */
int            gEmulationMode;
int            gUnderrunErrorCounter            = 0;
int            gFifoIsNotEmptyErrorCounter      = 0;
int	       bFIFOUnderrunError              = 0;

#endif

/* -----------------------------------------------
    PLX and WDC library initialize/uninit
   ----------------------------------------------- */
/* Last PLX library error string */
CHAR gsPLX_LastErr[256];

static DWORD LibInit_count = 0;

const char *PLX_GetLastErr(void)
{
    return gsPLX_LastErr;
}//PLX_GetLastErr

/* -----------------------------------------------
    PLX and WDC library initialize/uninit
   ----------------------------------------------- */
DWORD PLX_LibInit(void)
{
    DWORD dwStatus;
	
	/* init only once */
    if (++LibInit_count > 1)
        return WD_STATUS_SUCCESS;
 
    /* Set WDC library's debug options (default: level TRACE, output to Debug Monitor) */
    dwStatus = WDC_SetDebugOptions(WDC_DBG_DEFAULT, NULL);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed to initialize debug options for WDC library.\n"
            "Error 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
        
        return dwStatus;
    }

    /* Open a handle to the driver and initialize the WDC library */
    dwStatus = WDC_DriverOpen(WDC_DRV_OPEN_DEFAULT, PLX_DEFAULT_LICENSE_STRING);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed to initialize the WDC library. Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));
        
        return dwStatus;
    }

    return WD_STATUS_SUCCESS;
}//PLX_LibInit

DWORD PLX_LibUninit(void)
{
    DWORD dwStatus;
	
	if (--LibInit_count > 0)
        return WD_STATUS_SUCCESS;

    /* Uninit the WDC library and close the handle to WinDriver */
    dwStatus = WDC_DriverClose();
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed to uninit the WDC library. Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));
    }

    return dwStatus;
}//PLX_LibUninit

/* -----------------------------------------------
    Debugging and error handling
   ----------------------------------------------- */
void ErrLog(const CHAR *sFormat, ...)
{
    va_list argp;
    va_start(argp, sFormat);
    vsnprintf(gsPLX_LastErr, sizeof(gsPLX_LastErr) - 1, sFormat, argp);
#if defined(_DEBUG)
#if defined (__KERNEL__)
    WDC_Err("%s: %s", KP_PLX_DRIVER_NAME, PLX_GetLastErr());
#else
{
    WD_DEBUG_ADD add;
    BZERO(add);
    add.dwLevel   = D_ERROR;
    add.dwSection = S_MISC;
    sprintf(add.pcBuffer, "%s: %s", "USER MODE", PLX_GetLastErr());
    WD_DebugAdd(WDC_GetWDHandle(), &add);
}
#endif
#endif
    va_end(argp);
}//ErrLog

void TraceLog(const CHAR *sFormat, ...)
{
#if defined(_DEBUG)
    CHAR sMsg[256];
    va_list argp;
    va_start(argp, sFormat);
    vsnprintf(sMsg, sizeof(sMsg) - 1, sFormat, argp);
#if defined (__KERNEL__)
    WDC_Trace("%s: %s", KP_PLX_DRIVER_NAME, sMsg);
#else
{
    WD_DEBUG_ADD add;
    BZERO(add);
    add.dwLevel   = D_TRACE;
    add.dwSection = S_MISC;
    sprintf(add.pcBuffer, "%s: %s", "USER MODE", sMsg);
    WD_DebugAdd(WDC_GetWDHandle(), &add);
}
#endif
    va_end(argp);
#endif
}//TraceLog


