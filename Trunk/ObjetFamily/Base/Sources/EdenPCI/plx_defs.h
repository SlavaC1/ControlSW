/*$T Quadra2/Q2RT/Sources/EdenPCI/plx_defs.h GC 1.139 03/14/06 09:49:32 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef _PLX_DEFS_H_
#define _PLX_DEFS_H_

/* File: plx_defs.h */
#ifdef __cplusplus
extern "C"
{
#endif
#include "wdc_defs.h"
#include "pci_regs.h"

/*
 * General definitions ;
 * PLX configuration registers
 */
enum PLX_CFG_REGS
{
	PLX_PMCAPID	= 0x40,		/* Power Management Capability ID */
	PLX_PMNEXT	= 0x41,		/* Power Management Next Capability Pointer */
	PLX_PMCAP	= 0x42,		/* Power Management Capabilities Register */
	PLX_PMCSR	= 0x44,		/* Power Management Control/Status Register */
	PLX_HS_CAPID	= 0x48,		/* Hot Swap Capability ID */
	PLX_HS_NEXT	= 0x49,		/* Hot Swap Next Capability Pointer */
	PLX_HS_CSR	= 0x4A,		/* Hot Swap Control/Status Register */
	PLX_VPD_CAPID	= 0x4C,		/* PCI Vital Product Data Capability ID */
	PLX_VPD_NEXT	= 0x4D,		/* PCI Vital Product Data Next Capability Pointer */
	PLX_VPD_ADDR	= 0x4E,		/* PCI Vital Product Data Address */
	PLX_VPD_DATA	= 0x50		/* PCI VPD Data */
};

/* Run-time registers of PLX master devices (9054, 9056, 9080, 9656) */
enum PLX_M_REGS
{
	/* Local configuration registers */
	PLX_M_LAS0RR	= 0x00,		/* Local Addr Space 0 Range for PCI-to-Local Bus */
	PLX_M_LAS0BA	= 0x04,		/* Local BAR (Remap) for PCI-to-Local Addr Space 0 */
	PLX_M_MARBR    	= 0x08,		/* Mode/DMA Arbitration */
	PLX_M_BIGEND	= 0x0C,		/* Big/Little Endian Descriptor */
	PLX_M_LMISC    	= 0x0D,		/* Local Miscellananeous Control */
	PLX_M_PROT_AREA = 0x0E,		/* Serial EEPROM Write-Protected Addr Boundary */
	PLX_M_EROMRR	= 0x10,		/* Expansion ROM Range */
	PLX_M_EROMBA	= 0x14,		/* EROM Local BAR (Remap) & BREQ0 Control */
	PLX_M_LBRD0    	= 0x18,		/* Local Addr Space 0 Bus Region Descriptor */
	PLX_M_DMRR     	= 0x1C,		/* Local Range for PCI initiatior-to-PCI */
	PLX_M_DMLBAM	= 0x20,		/* Local Bus Addr for PCI Initiatior-to-PCI Mem */
	PLX_M_DMLBAI	= 0x24,		/* Local BAR for PCI Initiatior-to-PCI I/O */
	PLX_M_DMPBAM	= 0x28,		/* PCI BAR (Remap) for Initiatior-to-PCI Mem */
	PLX_M_DMCFGA	= 0x2C,		/* PCI Config Addr for PCI Initiatior-to-PCI I/O */

	PLX_M_LAS1RR	= 0xF0,		/* Local Addr Space 1 Range for PCI-to-Local Bus */
	PLX_M_LAS1BA	= 0xF4,		/* Local Addr Space 1 Local BAR (Remap) */
	PLX_M_LBRD1    	= 0xF8,		/* Local Addr Space 1 Bus Region Descriptor */
	PLX_M_DMDAC    	= 0xFC,		/* PCI Initiatior PCI Dual Address Cycle */
	PLX_M_PCIARB	= 0x100,	/* PCI Arbiter Control */
	PLX_M_PABTADR	= 0x104,	/* PCI Abort Address */

	/* mailbox, doorbell, interrupt status, control, id registers */
	PLX_M_MBOX0    	= 0x40,		/* Mailbox 0 */
	PLX_M_MBOX1    	= 0x44,		/* Mailbox 1 */
	PLX_M_MBOX2    	= 0x48,		/* Mailbox 2 */
	PLX_M_MBOX3    	= 0x4C,		/* Mailbox 3 */
	PLX_M_MBOX4    	= 0x50,		/* Mailbox 4 */
	PLX_M_MBOX5    	= 0x54,		/* Mailbox 5 */
	PLX_M_MBOX6    	= 0x58,		/* Mailbox 6 */
	PLX_M_MBOX7    	= 0x5C,		/* Mailbox 7 */
	PLX_M_P2LDBELL	= 0x60,		/* PCI-to-Local Doorbell */
	PLX_M_L2PDBELL	= 0x64,		/* Local-to-PCI Doorbell */
	PLX_M_INTCSR	= 0x68,		/* INTCSR - Interrupt Control/Status */
	PLX_M_CNTRL    	= 0x6C,		/* Serial EEPROM/User I/O/Init Ctr & PCI Cmd Codes */
	PLX_M_PCIHIDR	= 0x70,		/* PCI Hardcoded Configuration ID */
	PLX_M_PCIHREV	= 0x74,		/* PCI Hardcoded Revision ID */
	PLX_M_MBOX0_I2O = 0x78,		/* Mailbox 0 - I2O enabled */
	PLX_M_MBOX1_I2O = 0x7C,		/* Mailbox 1 - I2O enabled */

	/* DMA registers */
	PLX_M_DMAMODE0	= 0x80,		/* DMA Channel 0 Mode */
	PLX_M_DMAPADR0	= 0x84,		/* DMA Channel 0 PCI Address */
	PLX_M_DMALADR0	= 0x88,		/* DMA Channel 0 Local Address */
	PLX_M_DMASIZ0	= 0x8C,		/* DMA Channel 0 Transfer Size (bytes) */
	PLX_M_DMADPR0	= 0x90,		/* DMA Channel 0 Descriptor Pointer */
	PLX_M_DMAMODE1	= 0x94,		/* DMA Channel 1 Mode */
	PLX_M_DMAPADR1	= 0x98,		/* DMA Channel 1 PCI Address */
	PLX_M_DMALADR1	= 0x9C,		/* DMA Channel 1 Local Address */
	PLX_M_DMASIZ1	= 0xA0,		/* DMA Channel 1 Transfer Size (bytes) */
	PLX_M_DMADPR1	= 0xA4,		/* DMA Channel 1 Descriptor Pointer */
	PLX_M_DMACSR0	= 0xA8,		/* DMA Channel 0 Command/Status */
	PLX_M_DMACSR1	= 0xA9,		/* DMA Channel 1 Command/Status */
	PLX_M_DMAARB	= 0xAC,		/* DMA Arbitration */
	PLX_M_DMATHR	= 0xB0,		/* DMA Threshold (Channel 0 only) */
	PLX_M_DMADAC0	= 0xB4,		/* DMA 0 PCI Dual Address Cycle Address */
	PLX_M_DMADAC1	= 0xB8,		/* DMA 1 PCI Dual Address Cycle Address */

	/* Messaging queue (I20) registers */
	PLX_M_OPQIS    	= 0x30,		/* Outbound Post Queue Interrupt Status */
	PLX_M_OPQIM    	= 0x34,		/* Outbound Post Queue Interrupt Mask */
	PLX_M_IQP      	= 0x40,		/* Inbound Queue Post */
	PLX_M_OQP      	= 0x44,		/* Outbound Queue Post */
	PLX_M_MQCR     	= 0xC0,		/* Messaging Queue Configuration */
	PLX_M_QBAR     	= 0xC4,		/* Queue Base Address */
	PLX_M_IFHPR    	= 0xC8,		/* Inbound Free Head Pointer */
	PLX_M_IFTPR    	= 0xCC,		/* Inbound Free Tail Pointer */
	PLX_M_IPHPR    	= 0xD0,		/* Inbound Post Head Pointer */
	PLX_M_IPTPR    	= 0xD4,		/* Inbound Post Tail Pointer */
	PLX_M_OFHPR    	= 0xD8,		/* Outbound Free Head Pointer */
	PLX_M_OFTPR    	= 0xDC,		/* Outbound Free Tail Pointer */
	PLX_M_OPHPR    	= 0xE0,		/* Outbound Post Head Pointer */
	PLX_M_OPTPR    	= 0xE4,		/* Outbound Post Tail Pointer */
	PLX_M_QSR      	= 0xE8		/* Queue Status/Control */
};

/* Run-time registers of PLX target devices (9030, 9050, 9052) */
enum PLX_T_REGS
{
	/* Local configuration registers */
	PLX_T_LAS0RR   	= 0x00, /* Local Addr Space 0 Range */
	PLX_T_LAS1RR   	= 0x04, /* Local Addr Space 1 Range */
	PLX_T_LAS2RR   	= 0x08, /* Local Addr Space 2 Range */
	PLX_T_LAS3RR   	= 0x0C, /* Local Addr Space 3 Range */
	PLX_T_EROMRR   	= 0x10, /* Expansion ROM Range */
	PLX_T_LAS0BA   	= 0x14, /* Local Addr Space 0 Local BAR (Remap) */
	PLX_T_LAS1BA   	= 0x18, /* Local Addr Space 1 Local BAR (Remap) */
	PLX_T_LAS2BA   	= 0x1C, /* Local Addr Space 2 Local BAR (Remap) */
	PLX_T_LAS3BA   	= 0x20, /* Local Addr Space 3 Local BAR (Remap) */
	PLX_T_EROMBA   	= 0x24, /* Expansion ROM Local BAR (Remap) */
	PLX_T_LAS0BRD  	= 0x28, /* Local Addr Space 0 Bus Region Descriptors */
	PLX_T_LAS1BRD  	= 0x2C, /* Local Addr Space 1 Bus Region Descriptors */
	PLX_T_LAS2BRD  	= 0x30, /* Local Addr Space 2 Bus Region Descriptors */
	PLX_T_LAS3BRD  	= 0x34, /* Local Addr Space 3 Bus Region Descriptors */
	PLX_T_EROMBRD  	= 0x38, /* Expansion ROM Bus Region Descriptors */

	/* Chip select registers */
	PLX_T_CS0BASE  	= 0x3C, /* Chip Select 0 Base Address */
	PLX_T_CS1BASE  	= 0x40, /* Chip Select 1 Base Address */
	PLX_T_CS2BASE  	= 0x44, /* Chip Select 2 Base Address */
	PLX_T_CS3BASE  	= 0x48, /* Chip Select 3 Base Address */

	/* Control registers */
	PLX_T_INTCSR   	= 0x4C, /* Interrupt Control/Status (16 bit) */
	PLX_T_PROT_AREA	= 0x4E, /* Serial EEPROM Write-Protected Addr Boundary (16 bit) */
	PLX_T_CNTRL    	= 0x50, /* PCI Target
		       				 * Response;
								 * Serial
								 * EEPROM;
								 * Init Ctr */
	PLX_T_GPIOC    	= 0x54, /* General Purpose I/O Control */
	PLX_T_PMDATASEL	= 0x70, /* Hidden 1 Power Management Data Select */
	PLX_T_PMDATASCALE = 0x74	/* Hidden 2 Power Management Data Scale */
};


/* PLX 9080 is a master device */
#define IS_MASTER	TRUE
#define IS_USE_INT	TRUE
#define CS46EEPROM	BIT5

/* Kernel PlugIn driver name (should be no more than 8 characters) */
#define KP_PLX_DRIVER_NAME	"KP_EDENP"

/*
 * Internal definitions ;
 * WinDriver license registration string ;
 * TODO -> DONE: When using a registered WinDriver version, replace the license
 * string below with your specific WinDriver license registration string
 */
#define PLX_DEFAULT_LICENSE_STRING	"6f1ead6f3ac0d3014711beb6faaf673cc61f41d5db963c.WD1040_64_NL_Objet"

#if defined(USE_KERNEL_PLUGIN)
/*
 * Kernel PlugIn messages - used in WDC_CallKerPlug() calls (user mode)
 * KP_PCI_Call() (kernel mode)
 */
enum
{
	KP_EDEN_MSG_VERSION             = 0,
	KP_EDEN_MSG_WDC_HANDLE,			/* Send the address of the non paged buffer */
	KP_EDEN_MSG_BUFFER_ADDR,			/* Send the address of the non paged buffer */
	KP_EDEN_MSG_BUFFER_LENGTH,
	KP_EDEN_MSG_GET_INTERRUPT_MSG,
    KP_EDEN_MSG_NUM
};

#ifdef _DEBUG
extern char msgStr[KP_EDEN_MSG_NUM][sizeof("KP_EDEN_MSG_GET_INTERRUPT_MSG")];
#endif
/* Kernel Plugin messages status */
enum { KP_STATUS_OK          = 0x0001,
       KP_STATUS_NOK         = 0x0010,
       KP_STATUS_MSG_NO_IMPL = 0x1000, };

/* Kernel PlugIn version information struct */
typedef struct
{
    DWORD dwVer;
    CHAR  cVer[100];
} KP_EDEN_VERSION;

#endif

/*
 -----------------------------------------------------------------------------------------------------------------------
    Interrupt result information struct
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct
{
	DWORD						dwCounter;	/* Number of interrupts received */
	DWORD						dwLost;		/* Number of interrupts not yet handled */
	WD_INTERRUPT_WAIT_RESULT	waitResult; /* See WD_INTERRUPT_WAIT_RESULT values in windrvr.h */
	UINT32						u32INTCSR;	/* value of interrupt control/status register */
	BYTE						bDMACSR;	/* value of DMA channel control/status register (relevant only for master
											 * device) */

	/*
	 * TODO: You can add fields to PLX_INT_RESULT to store any additional information
	 * that you wish to pass to your diagnostics interrupt handler routine
	 * (DiagIntHandler() in xxx_diag.c)
	 */
} PLX_INT_RESULT;

/* PLX diagnostics interrupt handler function type */
typedef void (*PLX_INT_HANDLER) (WDC_DEVICE_HANDLE hDev, PLX_INT_RESULT * pIntResult);

/* PLX diagnostics plug-and-play and power management events handler function type */
typedef void (*PLX_EVENT_HANDLER) (WDC_DEVICE_HANDLE hDev, DWORD dwAction);

/* DMA channels */
typedef enum { PLX_DMA_CHANNEL_0 = 0, PLX_DMA_CHANNEL_1 = 1 } PLX_DMA_CHANNEL;

typedef struct
{
	WD_DMA			*pDma;
	WD_DMA			*pDmaList;
	PLX_DMA_CHANNEL dmaChannel;

	/* offsets of DMA registers */
	DWORD			dwDMACSR;
	DWORD			dwDMAMODE;
	DWORD			dwDMAPADR;
	DWORD			dwDMALADR;
	DWORD			dwDMADPR;
	DWORD			dwDMASIZ;

	/* physical address of the first descriptor in chain (used in chain mode) */
	UINT32			u32StartOfChain;
} PLX_DMA_STRUCT;

/* PLX DMA handle */
typedef PLX_DMA_STRUCT	*PLX_DMA_HANDLE;

/*
 -----------------------------------------------------------------------------------------------------------------------
    PLX device information struct
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct
{
	WD_PCI_ID			id;
	BOOL				fIsMaster;

#if !defined(USE_KERNEL_PLUGIN)
	WD_TRANSFER			*pIntTransCmds;
#endif /* USE_KERNEL_PLUGIN */

	/* indices of registers inside the transfer commands */
	BYTE				bIntCsrIndex;
	BYTE				bDmaCsrIndex;

	PLX_INT_HANDLER		        funcDiagIntHandler;
	PLX_EVENT_HANDLER	        funcDiagEventHandler;

	PLX_DMA_STRUCT*                 pPLXDma;  /* relevant only for master devices */

	/* offsets of some useful registers */
	DWORD				dwINTCSR;
	DWORD				dwCNTRL;
	DWORD				dwPROT_AREA;
	DWORD				dwLAS0BA;

	/* TODO -> DONE: You can add fields to store additional device-specific information */
	BOOL				fIsUseInt;
}PLX_DEV_CTX, *PPLX_DEV_CTX;

enum { EDEN_PCI_FIFO_UNDERRUN_ERROR = 100, EDEN_PCI_FIFO_START_MESSAGE };


#if defined(USE_KERNEL_PLUGIN) || defined(__KERNEL__) 
extern BYTE*  NonPagedBuffer;
extern WD_DMA gDma;
#endif /* USE_KERNEL_PLUGIN */

#undef EXTERN

const char* PLX_GetLastErr(void);
void TraceLog(const CHAR *sFormat, ...);
void ErrLog(const CHAR *sFormat, ...);
DWORD PLX_LibInit(void);
DWORD PLX_LibUninit(void);

#ifdef __cplusplus
}
#endif


#endif /* _PLX_DEFS_H_ */
