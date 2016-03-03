/*=======================================================================

                            MODULE DECLARATION
                            ------------------


    FILENAME : FIFOPCI.C

    PURPOSE :

    This package handles the FIFO support. The package provides the following
    services:
               
      - FIFO initialization
      - FIFO interrupt service routine
      - Write to FIFO
      - Read From FIFO

      - Enable the FIFO interrupt
      - Disable the FIFO interrupt
      - Read the interrupt status
      - FIFO Partial RESET
      - FIFO Master RESET

       - A Short test to Write/Read and compare.

    DATE CREATED : 10 NOV 2001

    PROGRAMMER: Rachel Ezrachi

==============================================================================*/

/* INCLUDE FILES
   ----------------*/
#include "QTypes.h"
#include "EdenPci.h"
#include "EdenPCISys.h"
#include "FIFOPci.h"
#include "QEvent.h"
#include "QThread.h"
#include "Q2RTErrors.h"
#include "AppLogFile.h"
#include "status_strings.h"

const int FIFO_FULL_EVENT_TIMEOUT = 5000;

static CQEvent gFIFOFullEvent;
static TGenericCallback gFIFOUnderrunErrorCallback;
static TGenericCockie gFIFOUnderrunErrorCockie;


void FIFOPCI_Init(bool EmulationMode)
{
  gEmulationMode = (EmulationMode) ? 1 : 0;

  // Master RESET
  FIFOPCI_MasterReset();
  EdenPCISystem_INTRDisable(LOCAL_FIFO);
}

void FIFOPCI_DeInit()
{}

void  FIFOPCI_MasterReset ()
{
  // Master RESET
  EdenPCI_WriteDWORD(AD_PCI_BAR2, FIFO_MASTER_RESET_REG, 0xFFFFFFFF);
  EdenPCI_WriteDWORD(AD_PCI_BAR2, FIFO_ST_RST_REG,       0x1);
  EdenPCI_WriteDWORD(AD_PCI_BAR2, FIFO_RST_COMM_REG,     0x1);
  /* write GO OFF */
  EdenPCI_WriteDWORD(AD_PCI_BAR2, FIFO_GO_REG,           0x0);
}

void  FIFOPCI_PartialReset ()
{
  // Partial RESET
  EdenPCI_WriteDWORD(AD_PCI_BAR2, FIFO_PARTIAL_RESET_REG, 0xFFFFFFFF);
}

void FIFOPCI_InitParamsForISR(unsigned long *BufferPtr, long BufferSize)
{
  BufferForIntrPtr = BufferPtr;
  BufferLayerRcvPtr = (void *) BufferPtr; /* Save the initial value for free */
  BufferSizeForIntr = BufferSize;
}

#if defined(USE_KERNEL_PLUGIN) /*{*/
void FIFOPCI_ISR_UKP(void)
{
  BYTE Message = EdenPCI_GetInterruptMessage();
  switch(Message)
  {
    case EDEN_PCI_FIFO_UNDERRUN_ERROR:
         // Call the error callback
         if (gFIFOUnderrunErrorCallback)
            (*gFIFOUnderrunErrorCallback)(gFIFOUnderrunErrorCockie);
         gUnderrunErrorCounter++;
         break;

    case EDEN_PCI_FIFO_START_MESSAGE:
         gFIFOFullEvent.SetEvent();
         break;

    default:
         break;
  }
}
/*}*/
#else /*{USE_KERNEL_PLUGIN*/
void FIFOPCI_ISR(void)
{
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  int NoOfWrDwords, MaxWriteData;
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /* Indicate entering the FIFO ISR ;
   * EdenPCI_WriteDWORD(2,0x32C,1); */
  /* If we already started */
  if (FALSE == gbFIFOStartFlag)
  {	/* The FIFO should not be empty in this point */
     if ((EdenPCI_ReadDWORD(AD_PCI_BAR2, FIFO_STS_REG) & 0x1F) == FIFO_EMPTY)
     {
	EdenPCISystem_INTRDisable(LOCAL_FIFO);
	/* Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetPrintControlInstance()->Stop();
	 * CQLog::Write(LOG_TAG_PRINT,"FIFO underrun error"); */
	gUnderrunErrorCounter++;
	bFIFOUnderrunError = 1;
	return;
     }
  }
  if (BufferSizeForIntr <= 0)
  {
     EdenPCISystem_INTRDisable(LOCAL_FIFO);
     return;
  }
  MaxWriteData = ((FALSE == gbFIFOStartFlag) ? MAX_FIFO_AVAIL : MAX_FIFO_SIZE - 10);
  NoOfWrDwords = FIFOPCI_WriteDWORD(BufferForIntrPtr, MIN(MaxWriteData, BufferSizeForIntr));
  /* When we are first full since last write request , trigger the full event */
  if (TRUE == gbFIFOStartFlag)
  {
     gbFIFOStartFlag = FALSE;
     gFIFOFullEvent.SetEvent();
  }

  BufferForIntrPtr += NoOfWrDwords;
  BufferSizeForIntr -= NoOfWrDwords;
  if(BufferSizeForIntr <= 0) EdenPCISystem_INTRDisable(LOCAL_FIFO);
  /* Indicate exit from the FIFO ISR ;
   * EdenPCI_WriteDWORD(2,0x32C,0);*/
}/* FIFOPCI_ISR */
#endif /* } USE_KERNEL_PLUGIN */

unsigned int FIFOPCI_ReadStatus()
{
  if (gEmulationMode)
     return FIFO_EMPTY;
  unsigned long	Value  = EdenPCI_ReadDWORD(AD_PCI_BAR2, FIFO_STS_REG);
  unsigned long Value1 = Value & 0x1F;
  return Value1;
}

int FIFOPCI_ReadDWORD(DWORD *bufferPtr,int  NumberOfDwords)
{
  int i;
  for (i = 0; i < NumberOfDwords; i++)
  {
      if (unsigned int FifoStatus = FIFOPCI_ReadStatus () == (unsigned int) FIFO_EMPTY)
          break;
      EdenPCISystem_WriteWS(FIFO_WS);
      // read FIFO so data is written to temp register
      EdenPCI_ReadDWORD(AD_PCI_BAR2, FIFO_READ_TEMP_REG);
      // Return the WS back
      EdenPCISystem_WriteWS(INITIAL_WS);
/*Reg32=*/
      *bufferPtr = ((EdenPCI_ReadDWORD(AD_PCI_BAR2, FIFO_READ_D7D0_TEMP_REG)          & 0x000000FF) |
                   ((EdenPCI_ReadDWORD(AD_PCI_BAR2, FIFO_READ_D15D8_TEMP_REG)  <<  8) & 0x0000FF00) |
  	           ((EdenPCI_ReadDWORD(AD_PCI_BAR2, FIFO_READ_D23D16_TEMP_REG) << 16) & 0x00FF0000) |
	           ((EdenPCI_ReadDWORD(AD_PCI_BAR2, FIFO_READ_D31D24_TEMP_REG) << 24) & 0xFF000000));
       bufferPtr++;
  }
  return ((i > 0) ? i : -1);
}

//-----------------------------------------------------------------
int FIFOPCI_WriteDWORD(unsigned long *bufferPtr,int  NumberOfDwords)
{
  int i;
  // WS 0  -
  EdenPCISystem_WriteWS(FIFO_WS);
  for (i = 0; i < NumberOfDwords ; i++)
  {
    EdenPCI_WriteDWORD(AD_PCI_BAR2, FIFO_WRITE_REG, *bufferPtr);
    bufferPtr++;
  }
  Sleep(1);
  // Return the WS back
  EdenPCISystem_WriteWS(INITIAL_WS);
  return i;
}

// Interface functions to the FIFO
TQErrCode FIFOPCI_WriteAsync(PBYTE Buffer,unsigned BufferLength)
{
   #pragma warn -8012 // Diasable warning Number 8012 "Comparing signed to unsigned values"
  if (BufferLength * sizeof(DWORD) > SIZE_OF_NON_PAGED_BUFFER)
  {
    return Q2RT_FIFOPCI_BUFFER_LENGTH_ERROR;
  }
    #pragma warn .8012 // Enable warning Number 8012 "Comparing signed to unsigned values"

  if(gEmulationMode)
    return Q_NO_ERROR;

  FIFOPCI_MasterReset();
  gbFIFOStartFlag = TRUE;

#if defined(USE_KERNEL_PLUGIN) /*{*/
  DWORD dwStatus = EdenPCI_InitParamsForISR(BufferLength);
  CQLog::Write(LOG_TAG_PRINT,QFormatStr("dwStatus: %s", Stat2Str(dwStatus)));

  if (WD_STATUS_SUCCESS != dwStatus)
     return Q2RT_KERNEL_PLUGIN_FAILURE;

  /* copy the buffer to the non paged buffer */
  memcpy(EdenPCI_GetNonPagedBuffer(), Buffer, BufferLength * sizeof(DWORD));
#else /* } USE_KERNEL_PLUGIN { */

  bFIFOUnderrunError = 0;
  FIFOPCI_InitParamsForISR((PULONG) Buffer, BufferLength);

#endif /* } USE_KERNEL_PLUGIN */
  FIFOPCI_MasterReset();
  EdenPCISystem_INTREnable(LOCAL_FIFO);
  if (gFIFOFullEvent.WaitFor(FIFO_FULL_EVENT_TIMEOUT) == QLib::wrSignaled) // write GO On
     EdenPCI_WriteDWORD(AD_PCI_BAR2, FIFO_GO_REG, 0x01);
  else
     return Q2RT_FIFO_EVENT_TIMEOUT;

  return Q_NO_ERROR;
}/* FIFOPCI_WriteAsync */

//-------------------------------------------------------------------
void FIFOPCI_CancelWriteAsync(void)
{
  if (0 != gEmulationMode)
    return;
  EdenPCISystem_INTRDisable(LOCAL_FIFO);
  // write NOGO
  EdenPCI_WriteDWORD(AD_PCI_BAR2, FIFO_GO_REG, 0);
  BufferForIntrPtr = NULL;
  BufferLayerRcvPtr = NULL;
  BufferSizeForIntr = 0;
}

//--------------------------------------------------------------
void FIFOPCI_ResetBufferAddress(void)
{
  if (0 != gEmulationMode)
    return;
  EdenPCISystem_ResetBufferAddress();
}

void FIFOPCI_NoGo(void)
{
  if (0 != gEmulationMode)
    return;
  EdenPCISystem_INTRDisable(LOCAL_FIFO);
  EdenPCI_WriteDWORD(AD_PCI_BAR2, FIFO_GO_REG, 0);// write NOGO
}

//-------------------------------------------------------------------
bool FIFOPCI_IsEmpty(void)
{
  if (0 != gEmulationMode)
    return false;
 if (FIFOPCI_ReadStatus() == FIFO_EMPTY)
    return true;
  gFifoIsNotEmptyErrorCounter++;
  return false;
}

//-------------------------------------------------------------------
// Remark : don't use this function for the PCI test
int FIFOPCI_ReadLeftDWORDS(unsigned long *bufferPtr, int BufferSize)
{
  unsigned long *LocalBufferPtr = bufferPtr;
  EdenPCISystem_INTRDisable(LOCAL_FIFO);
  EdenPCI_WriteDWORD(AD_PCI_BAR2, FIFO_GO_REG, 0);// write NOGO
  EdenPCI_FIFOSetReadMode(FF_READ_MANUAL);
  int i = 0;
  while (!FIFOPCI_IsEmpty() && (i < BufferSize))
  {
     FIFOPCI_ReadDWORD(LocalBufferPtr, 1);
     i++;
     LocalBufferPtr++;
  }
  EdenPCI_FIFOSetReadMode(FF_READ_HIGH_SPEED);
  return i;
}

//Statistics
void GetPCIStatistics(int& UnderrunErrorCounter,int& FifoIsNotEmptyErrorCounter)
{
  gUnderrunErrorCounter       = UnderrunErrorCounter;
  gFifoIsNotEmptyErrorCounter = FifoIsNotEmptyErrorCounter;
}

void ResetPCIStatistics(void)
{
   gUnderrunErrorCounter       = 0;
   gFifoIsNotEmptyErrorCounter = 0;
}

void SetFIFOUnderrunErrorCallback(TGenericCallback FIFOUnderrunErrorCallback,TGenericCockie FIFOUnderrunErrorCockie)
{
  gFIFOUnderrunErrorCallback = FIFOUnderrunErrorCallback;
  gFIFOUnderrunErrorCockie   = FIFOUnderrunErrorCockie;
}
