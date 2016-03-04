/*=======================================================================

                            MODULE DECLARATION
                            ------------------


    FILENAME : EdenPCISys.C

    PURPOSE :

    This package handles the PCI System support.
    DATE CREATED : 20 NOV 2001

    Written by: Rachel Ezrachi

==============================================================================*/

/*COMPILER DEFINITIONS
   -------------------- */
#ifndef  _EDEN_PCI_SYS_C__
#define  _EDEN_PCI_SYS_C__

#include "QTypes.h"
#include <stdio.h>
#include "EdenPCISys.h"
#include "EdenPci.h"
#include "FIFOPCI.h"
#include "QMonitor.h"
#include "FIFOPci_defs.h"
#include "AppLogFile.h"

static void EdenPCI_ISRFunction(void) throw();

// ==========================================================================
DWORD EdenPCISystem_Init()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEdenPCIError	err      = EDEN_PCI_NO_ERR;
	DWORD           dwStatus = WD_STATUS_SUCCESS;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	dwStatus = EdenPCI_Init();

	if(dwStatus != WD_STATUS_SUCCESS)
		return dwStatus;

	//Initialize WS
	EdenPCISystem_WriteWS(INITIAL_WS);

	// Initialize Intr Control/status to enable (0x68)
	EdenPCI_EnableInterrupt(EdenPCI_ISRFunction);

	//Initialize Control Enable Register
	dwStatus = EdenPCI_WriteDWORD(AD_PCI_BAR2, PCI_WRITE_ENABLE_CTR_REG, 0x0F);
	
	if (WD_STATUS_SUCCESS != dwStatus)	
		CQLog::Write(LOG_TAG_PRINT,QFormatStr("dwStatus: %s", Stat2Str(dwStatus)));

	// Initialize Write Controll
	EdenPCI_FIFOSetReadMode(FF_READ_HIGH_SPEED);

	// Initialize the master interrupt to Enable
	EdenPCISystem_INTREnable(LOCAL_MASTER);

	return dwStatus;
}

// ==========================================================================
void  EdenPCISystem_RESET ()
{
  // Initialize Bar 0 addr 0
  //Initialize WS

  EdenPCISystem_WriteWS(INITIAL_WS);

  // Initialize All intr to disable (0x70)

  // Initialize Intr Control/status to enable (0x68)

}
//===========================================================================

DWORD EdenPCISystemTest_Init()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEdenPCIError	err = EDEN_PCI_NO_ERR;
	DWORD           dwStatus = WD_STATUS_SUCCESS;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	dwStatus = EdenPCI_Init();

	if(dwStatus != WD_STATUS_SUCCESS)
		return dwStatus;

	//Initialize WS
	EdenPCISystem_WriteWS(INITIAL_WS);

	//Initialize Control Enable Register
	dwStatus = EdenPCI_WriteDWORD(AD_PCI_BAR2, PCI_WRITE_ENABLE_CTR_REG, 0x0F);
	
	if (WD_STATUS_SUCCESS != dwStatus)	
		CQLog::Write(LOG_TAG_PRINT,QFormatStr("dwStatus: %s", Stat2Str(dwStatus)));

	// Initialize Write Controll
	EdenPCI_FIFOSetReadMode(FF_READ_HIGH_SPEED);

	return dwStatus;
}
//===========================================================================
void EdenPCIEnableINT()
{
  // Initialize Intr Control/status to enable (0x68)
  EdenPCI_EnableInterrupt(EdenPCI_ISRFunction);
}
// ==========================================================================
void EdenPCI_FIFOSetReadMode(int ReadMode)
{
	DWORD dwStatus = WD_STATUS_SUCCESS;
	
	dwStatus = EdenPCI_WriteDWORD(AD_PCI_BAR2, PCI_WRITE_CTR_REG, ReadMode);
	
	if (WD_STATUS_SUCCESS != dwStatus)	
		CQLog::Write(LOG_TAG_PRINT,QFormatStr("dwStatus: %s", Stat2Str(dwStatus)));
}
// ==========================================================================
bool EdenPCISystem_ResetBufferAddress(void)
{
   return (EdenPCI_ResetBufferAddress() == WD_STATUS_SUCCESS);
}

void EdenPCISystem_INTREnable(INTR_TYPE intType)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DWORD	val = EdenPCI_ReadDWORD(AD_PCI_BAR2, PCI_LCL_INT_CTR_REG);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch(intType)
	{
	case LOCAL_MASTER:	val |= BIT7; break;
	case LOCAL_FIFO:	val |= BIT4; break;
	case LOCAL_UART_A:	val |= BIT0; break;
	case LOCAL_UART_B:	val |= BIT1; break;
	case LOCAL_UART_C:	val |= BIT2; break;
	case LOCAL_UART_D:	val |= BIT3; break;
	default:			return;
	}

	DWORD dwStatus = WD_STATUS_SUCCESS;
	dwStatus = EdenPCI_WriteDWORD(AD_PCI_BAR2, PCI_LCL_INT_CTR_REG, val);
	
	if (WD_STATUS_SUCCESS != dwStatus)	
		CQLog::Write(LOG_TAG_PRINT,QFormatStr("dwStatus: %s", Stat2Str(dwStatus)));
		
}	/* EdenPCISystem_INTREnable */
// ==========================================================================
void EdenPCISystem_INTRDisable(INTR_TYPE intType)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DWORD	val = EdenPCI_ReadDWORD(AD_PCI_BAR2, PCI_LCL_INT_CTR_REG);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch(intType)
	{
	case LOCAL_ALL:		val = 0x0; break;
	case LOCAL_MASTER:	val &= ~BIT7; break;
	case LOCAL_FIFO:	val &= ~BIT4; break;
	case LOCAL_UART_A:	val &= ~BIT0; break;
	case LOCAL_UART_B:	val &= ~BIT1; break;
	case LOCAL_UART_C:	val &= ~BIT2; break;
	case LOCAL_UART_D:	val &= ~BIT3; break;
	default:			return;
	}

	DWORD dwStatus = WD_STATUS_SUCCESS;
	dwStatus = EdenPCI_WriteDWORD(AD_PCI_BAR2, PCI_LCL_INT_CTR_REG, val);
	
	if (WD_STATUS_SUCCESS != dwStatus)	
		CQLog::Write(LOG_TAG_PRINT,QFormatStr("dwStatus: %s", Stat2Str(dwStatus)));
		
}	/* EdenPCISystem_INTRDisable */

// ==========================================================================

bool     EdenPCISystem_ReadINTR(INTR_TYPE IntType) // INTR  is enabled or disabled
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	bool			INTRSts;
	unsigned long	Value = EdenPCI_ReadDWORD(AD_PCI_BAR2, PCI_LCL_INT_CTR_REG);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

 switch (IntType)
 {
  case LOCAL_MASTER:
       if (Value & BIT7 )
          INTRSts = true;
       else
          INTRSts = false;
       break;
  case LOCAL_FIFO:
       if (Value & BIT4 )
          INTRSts = true;
       else
          INTRSts = false;
       break;
  case LOCAL_UART_A:
       if (Value & BIT0 )
          INTRSts = true;
       else
          INTRSts = false;
       break;
  case LOCAL_UART_B:
       if (Value & BIT1 )
          INTRSts = true;
       else
          INTRSts = false;
       break;
  case LOCAL_UART_C:
       if (Value & BIT2 )
          INTRSts = true;
       else
          INTRSts = false;
       break;
  case LOCAL_UART_D:
       if (Value & BIT3 )
          INTRSts = true;
       else
          INTRSts = false;
       break;
  default:
       break;
  }
  return (INTRSts);
}
// ==========================================================================
bool       EdenPCISystem_ReadINTRStatus(INTR_TYPE IntType)  // INTR exist & Enabled
{
bool INTRSts;
	unsigned long	Value = EdenPCI_ReadDWORD(AD_PCI_BAR2, PCI_LCL_INT_STS_CTR_REG);
 switch (IntType)
 {
  case LOCAL_FIFO:
       if (Value & BIT4 )
          INTRSts = true;
       else
          INTRSts = false;
       break;
  case LOCAL_UART_A:
       if (Value & BIT0 )
          INTRSts = true;
       else
          INTRSts = false;
       break;
  case LOCAL_UART_B:
       if (Value & BIT1 )
          INTRSts = true;
       else
          INTRSts = false;
       break;
  case LOCAL_UART_C:
       if (Value & BIT2 )
          INTRSts = true;
       else
          INTRSts = false;
       break;
  case LOCAL_UART_D:
       if (Value & BIT3 )
          INTRSts = true;
       else
          INTRSts = false;
       break;
  default:
       break;
  }
  return (INTRSts);
}

void EdenPCISystem_WriteWS(WS_TYPE WsType)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned long	Value = EdenPCI_ReadDWORD(AD_PCI_BAR0, PLX_M_LBRD0);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	DWORD           dwStatus = WD_STATUS_SUCCESS;

	switch(WsType)
	{
		case INITIAL_WS:
			{	
				dwStatus = EdenPCI_WriteDWORD(AD_PCI_BAR0, PLX_M_LBRD0, 0x40030123); 
				
				if (WD_STATUS_SUCCESS != dwStatus)	
				CQLog::Write(LOG_TAG_PRINT,QFormatStr("dwStatus: %s", Stat2Str(dwStatus)));
				break;
			}
			
		case FIFO_WS:		
			{
				dwStatus = EdenPCI_WriteDWORD(AD_PCI_BAR0, PLX_M_LBRD0, Value & 0xFFFFFFC3);
				
				if (WD_STATUS_SUCCESS != dwStatus)	
				CQLog::Write(LOG_TAG_PRINT,QFormatStr("dwStatus: %s", Stat2Str(dwStatus)));
				break;
			}
			
		default:			
			break;
	}
}	/* EdenPCISystem_WriteWS */

static void EdenPCI_ISRFunction(void) throw()
{
	DWORD dwStatus = WD_STATUS_SUCCESS;
	
#if defined(USE_KERNEL_PLUGIN)
	FIFOPCI_ISR_UKP();
#else
	DWORD	ISRregister = (EdenPCI_ReadDWORD(AD_PCI_BAR2, PCI_LCL_INT_STS_CTR_REG) & 0xFF);
	if(ISRregister & BIT4) FIFOPCI_ISR();
#endif /* USE_KERNEL_PLUGIN */

	/* Activate interrupt again */
	DWORD	Value = EdenPCI_ReadDWORD(AD_PCI_BAR0, PLX_M_INTCSR);
	dwStatus = EdenPCI_WriteDWORD(AD_PCI_BAR0, PLX_M_INTCSR, Value | (BIT8 | BIT10 | BIT11));
	
	if (WD_STATUS_SUCCESS != dwStatus)	
		CQLog::Write(LOG_TAG_PRINT,QFormatStr("dwStatus: %s", Stat2Str(dwStatus)));
}

QString EdenPCISystem_GetKernelPluginVersion()
{
	KP_EDEN_VERSION kpVer;
	kpVer = EdenPCI_GetKernelPluginVersion();
	return QIntToStr(kpVer.dwVer) + ";" + kpVer.cVer;
}

#endif   /* _HWPCI_  */

