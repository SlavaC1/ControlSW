#ifndef _EDEN_PCI_INTR_H_
#define _EDEN_PCI_INTR_H_

/* INCLUDE FILES
   ----------------*/
#include "EdenPci.h"
#include "status_strings.h"

const int FF_READ_SERIAL = 2;
const int FF_READ_HIGH_SPEED = 3;
const int FF_READ_MANUAL = 0;

typedef enum
{
LOCAL_MASTER,
LOCAL_FIFO,
LOCAL_UART_A,
LOCAL_UART_B,
LOCAL_UART_C,
LOCAL_UART_D,
LOCAL_ALL       //Available only for HWPCI_INTR_Disable
}INTR_TYPE;

typedef enum { INITIAL_WS, UART_WS, FIFO_WS } WS_TYPE;

DWORD   EdenPCISystem_Init(void);
void    EdenPCISystem_RESET (void);
bool    EdenPCISystem_ResetBufferAddress(void);
void    EdenPCISystem_INTREnable(INTR_TYPE IntType);
void    EdenPCISystem_INTRDisable(INTR_TYPE IntType);
bool    EdenPCISystem_ReadINTR(INTR_TYPE IntType); // INTR  is enabled or disabled
bool    EdenPCISystem_ReadINTRStatus(INTR_TYPE IntType);  // INTR exist & Enabled
void    EdenPCISystem_WriteWS(WS_TYPE WsType);
void    EdenPCI_FIFOSetReadMode(int ReadMode);
void    EdenPCIEnableINT(void);
DWORD   EdenPCISystemTest_Init(void);
QString EdenPCISystem_GetKernelPluginVersion();
#endif
