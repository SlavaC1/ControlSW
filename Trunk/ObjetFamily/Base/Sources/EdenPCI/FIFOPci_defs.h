/***********************
FIFOPci_defs.h
***********************/
#ifndef __FIFO_PCI_DEFS_H_
#define __FIFO_PCI_DEFS_H_
#ifdef __cplusplus
extern "C"
{
#endif

const int	PCI_INTCTRL                     = 0x68;         /* Global interrupt control */
const int	PCI_LCL_INT_CTR_REG             = 0x70;         /* Enable interrupt control - Read/Write */
const int	PCI_LCL_INT_STS_REG             = 0x80;         /* Interrupt status - read only */
const int	PCI_LCL_INT_STS_CTR_REG         = 0x84;         /* CTR & STS - read only */
const int	PCI_WRITE_CTR_REG               = 0x310;
const int	PCI_WRITE_ENABLE_CTR_REG        = 0x30c;        /* Write control enable */

const int	FIFO_MASTER_RESET_REG           = 0x134;
const int	FIFO_PARTIAL_RESET_REG          = 0x130;
const int	FIFO_ST_RST_REG                 = 0x20;;
const int	FIFO_RST_COMM_REG               = 0x24;
const int	FIFO_GO_REG                     = 0x1C;
const int	FIFO_STS_REG                    = 0x120;
const int	FIFO_WRITE_REG                  = 0x100;
const int	FIFO_READ_TEMP_REG              = 0x104;
const int	FIFO_READ_D7D0_TEMP_REG         = 0x110;
const int	FIFO_READ_D15D8_TEMP_REG        = 0x114;
const int	FIFO_READ_D23D16_TEMP_REG       = 0x118;
const int	FIFO_READ_D31D24_TEMP_REG       = 0x11C;

#define FIFO_EMPTY	    0x1C		/* 0 DWORD */
#define FIFO_1_TO_AE	    0x1D		/* 1 to PAE */
#define FIFO_AE_TO_HF	    0x1F		/* AE+1 to 4096 */
#define FIFO_HF_TO_AF	    0x1B		/* 4097 to (8192-(PAF+1)) */
#define FIFO_AF_TO_FULL	    0x13		/* (8192 - PAF) to 8191 */
#define FIFO_FULL	    0x3			/* 8192 */
#define FIFO_CAPACITY_ERROR 0xFF		/* error */

#define MAX_FIFO_AVAIL	    ((32 * 1024) - 4100)	/* interrupt is programmed to 4096 DWORD */
#define MAX_FIFO_SIZE	    (32 * 1024)		/* 8KDwords start from zero */

extern BOOL             gbFIFOStartFlag;

#if !defined(__KERNEL__)

extern int              gUnderrunErrorCounter;
extern int              gEmulationMode;
extern unsigned long*   BufferForIntrPtr;     /* PTR to the Layer buffer */
extern void*            BufferLayerRcvPtr;
extern long             BufferSizeForIntr;    /* in DWORDS */
extern int              gFifoIsNotEmptyErrorCounter;
extern int              bFIFOUnderrunError;
extern int              SIZE_OF_NON_PAGED_BUFFER;

#else
extern BYTE             InterruptMessage;
extern long             BufferLength;
extern DWORD*           FIFOBuffer;
WD_DEBUG_ADD	        Debug;
WD_SLEEP 	        Sleep_struc;

#endif //__KERNEL__

#ifdef __cplusplus
}
#endif

#endif //__FIFO_PCI_DEFS_H_
