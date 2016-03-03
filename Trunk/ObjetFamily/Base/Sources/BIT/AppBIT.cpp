/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT.                                                   *
 * Module: BIT                                                      *
 * Module Description: Application specific BIT tests.              *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg                                                *
 * Start date: 28/07/2003                                           *
 * Last upate: 21/09/2003                                           *
 ********************************************************************/

#include "AppBit.h"
#include "TestGroup.h"
#include "BITManager.h"

#include "EdenPCI.h"
#include "EdenPCISys.h"
#include "FIFOPCI.h"
#include "Q2RTApplication.h"
#include "Tester.h"
#include "MotorDefs.h"
#include "AppLogFile.h"	

namespace AppBIT
{


// Proxy object for test functions
class CBITFunctions : public CQComponent {
  public:
    DEFINE_METHOD(CBITFunctions,int,ReadEncoderPos);

    // Constructor
    CBITFunctions(void) : CQComponent("BITFunctions") {
      INIT_METHOD(CBITFunctions,ReadEncoderPos);
    }

} *gBITFunctionsProxy = NULL;

// Read data encoder position from OHDB
int CBITFunctions::ReadEncoderPos(void)
{
  return Q2RTApplication->GetTester()->ReadDataFromOHDBXilinx(ENCODER_ADDRESS);
}

// Initialize
void Init(void)
{
  // Initilize global proxy object
  if(gBITFunctionsProxy == NULL)
    gBITFunctionsProxy = new CBITFunctions;
}

// De-Initialize
void DeInit(void)
{
  if(gBITFunctionsProxy != NULL)
    delete gBITFunctionsProxy;
}

// Test objects
// ******************************************************************
#include "FIFOPci_defs.h"

const int FIFO_INTR_TEST_BLOCK_SIZE = 1024;
const int FIFO_INTR_TEST_BLOCKS_NUM = 16;
const int FIFO_GO_REG    = 0x1C;

const FIFO_INTR        = 0 ; 
const FIFO_INTR_FF     = 1 ; 
const FIFO_INTR_00     = 2 ; 
const FIFO_INTR_55     = 3 ; 
const FIFO_INTR_AA     = 4 ; 

// PCI FIFO interrupt test
class CFIFOInterruptTest : public CSingleTest
{
 
  private:
    DWORD *m_WriteBuffer;
    DWORD *m_ReadBuffer;

	void clearBuffers(){
		for(int i = 0; i < FIFO_INTR_TEST_BLOCK_SIZE * FIFO_INTR_TEST_BLOCKS_NUM; i++)
				m_WriteBuffer[i] = 6 ;
				
		for(int i = 0; i < FIFO_INTR_TEST_BLOCK_SIZE ; i++)
				m_ReadBuffer[i] =  6 ;		
	}
	 /* translate from hex to Binary  */
	QString FormatNumToBinary(DWORD n)
    {

		QString out = "";
		for(int i = 0; i < 32; i++) {
			out += (n & 0x80000000) ? "1" : "0";
			n <<= 1;

		}
		return out;

	}
	

				
    TTestResult Start(void)
    {
      
      EdenPCI_FIFOSetReadMode(FF_READ_MANUAL);

      // Allocate buffer for the test
      m_WriteBuffer   = new DWORD[FIFO_INTR_TEST_BLOCK_SIZE * FIFO_INTR_TEST_BLOCKS_NUM];
    	
      m_ReadBuffer  = new DWORD[FIFO_INTR_TEST_BLOCK_SIZE];
	  
      return trGo;
    }

    TTestResult End(void)
    {
      delete []m_WriteBuffer;
      delete []m_ReadBuffer;
      EdenPCISystem_INTRDisable(LOCAL_FIFO);
      EdenPCI_WriteDWORD(AD_PCI_BAR2,FIFO_GO_REG,0);
      EdenPCI_FIFOSetReadMode(FF_READ_HIGH_SPEED);
      return trGo;
    }

    // Test implementation function (override)
    TTestResult Execute(int StepNumber)
    {	
		DWORD CompareValue = 0;
		int t = FIFO_INTR_TEST_BLOCK_SIZE * FIFO_INTR_TEST_BLOCKS_NUM;
		
		if(!Q2RTApplication->IsPciCardInitialized())
		{
			SetResultDescription("PCI card not found");
			return trNoGo;
		}
		
		switch(StepNumber)
		{	
		//step #1			
		case FIFO_INTR:
		    CQLog::Write(LOG_TAG_GENERAL,"step #1");
		    for(int i = 0; i < FIFO_INTR_TEST_BLOCK_SIZE * FIFO_INTR_TEST_BLOCKS_NUM; i++)
				m_WriteBuffer[i] = i;
			// Write to PCI	
		    FIFOPCI_WriteAsync((PBYTE)m_WriteBuffer,FIFO_INTR_TEST_BLOCK_SIZE * FIFO_INTR_TEST_BLOCKS_NUM);
			
			for(int i = 0; i < FIFO_INTR_TEST_BLOCKS_NUM; i++)
			{
				// Give the interrupt some time for writing the data
				//Sleep(250);
				Sleep(100);
				// Disabel interrupts and clear go flag for read back operation
				EdenPCISystem_INTRDisable(LOCAL_FIFO);
				EdenPCI_WriteDWORD(AD_PCI_BAR2,FIFO_GO_REG,0);

				// Read back from FIFO
				if(FIFOPCI_ReadDWORD(m_ReadBuffer,FIFO_INTR_TEST_BLOCK_SIZE) == -1)
				{
					SetResultDescription("FIFO read failed");
					//CQLog::Write(LOG_TAG_GENERAL,QFormatStr("----------ReadDWORD failed: %d",FIFO_INTR_TEST_BLOCK_SIZE) );
					return trNoGo;
				}

				EdenPCISystem_INTREnable(LOCAL_FIFO);
				EdenPCI_WriteDWORD(AD_PCI_BAR2,FIFO_GO_REG,1);

				// Check read buffer content
				for(int j = 0; j < FIFO_INTR_TEST_BLOCK_SIZE; j++,CompareValue++)
				if(m_ReadBuffer[j] != CompareValue)
				{
					SetActualVsRequested(QFormatStr("%X",CompareValue),QFormatStr("%X",m_ReadBuffer[j]),"Failed after reading " + QIntToStr(j) + " DWORDs." );
					SetResultDescription("FIFO read-back error");
					CQLog::Write(LOG_TAG_GENERAL,QFormatStr("ERROR: Written: %s ",FormatNumToBinary(CompareValue)) +QFormatStr("Read: %s ",FormatNumToBinary(m_ReadBuffer[j])));
					for (int k = i+1  ; k < FIFO_INTR_TEST_BLOCKS_NUM  ; k++){
							// Give the interrupt some time for writing the data
							Sleep(100);

							// Disabel interrupts and clear go flag for read back operation
							EdenPCISystem_INTRDisable(LOCAL_FIFO);
							EdenPCI_WriteDWORD(AD_PCI_BAR2,FIFO_GO_REG,0);

							// Read back from FIFO
							if(FIFOPCI_ReadDWORD(m_ReadBuffer,FIFO_INTR_TEST_BLOCK_SIZE) == -1)
							{
								SetResultDescription("FIFO_FF read failed");
								return trNoGo;
							}
								
							EdenPCISystem_INTREnable(LOCAL_FIFO);
							EdenPCI_WriteDWORD(AD_PCI_BAR2,FIFO_GO_REG,1);
						}	
					return trNoGo;
				}
			}

			
			SetActualVsRequested(QFormatStr("%X DWORDS",t),QFormatStr("%X DWORDS",t));
			break;
		//step #2	
		case FIFO_INTR_FF:	
		    CQLog::Write(LOG_TAG_GENERAL,"step #2 ");
		    /* CLEAR ALL BUFFERS */
			clearBuffers() ; 
			
		    for(int i = 0; i < FIFO_INTR_TEST_BLOCK_SIZE * FIFO_INTR_TEST_BLOCKS_NUM; i++)
				m_WriteBuffer[i] = 0xFFFFFFFF;
				
		    FIFOPCI_WriteAsync((PBYTE)m_WriteBuffer,FIFO_INTR_TEST_BLOCK_SIZE * FIFO_INTR_TEST_BLOCKS_NUM);
			
			for(int i = 0; i < FIFO_INTR_TEST_BLOCKS_NUM; i++)
			{
				// Give the interrupt some time for writing the data
				Sleep(100);

				// Disabel interrupts and clear go flag for read back operation
				EdenPCISystem_INTRDisable(LOCAL_FIFO);
				EdenPCI_WriteDWORD(AD_PCI_BAR2,FIFO_GO_REG,0);

				// Read back from FIFO
				if(FIFOPCI_ReadDWORD(m_ReadBuffer,FIFO_INTR_TEST_BLOCK_SIZE) == -1)
				{
					SetResultDescription("FIFO_FF read failed");
					return trNoGo;
				}

				EdenPCISystem_INTREnable(LOCAL_FIFO);
				EdenPCI_WriteDWORD(AD_PCI_BAR2,FIFO_GO_REG,1);
									
				// Check read buffer content
				for(int j = 0; j < FIFO_INTR_TEST_BLOCK_SIZE ; j++)
					if(m_ReadBuffer[j] != 0xFFFFFFFF)
					{
						SetActualVsRequested("0xFFFFFFFF",QFormatStr("%X ",m_ReadBuffer[j]),"Failed after reading " + QIntToStr(j) + " DWORDs.");
						SetResultDescription("FIFO_FF read-back error");
						CQLog::Write(LOG_TAG_GENERAL,QFormatStr("ERROR: Written: %s ",FormatNumToBinary(0xFFFFFFFF)) +QFormatStr("Read: %s ",FormatNumToBinary(m_ReadBuffer[j])));
						
						/**/
						
						for (int k = i+1  ; k < FIFO_INTR_TEST_BLOCKS_NUM  ; k++){
							// Give the interrupt some time for writing the data
							Sleep(100);

							// Disabel interrupts and clear go flag for read back operation
							EdenPCISystem_INTRDisable(LOCAL_FIFO);
							EdenPCI_WriteDWORD(AD_PCI_BAR2,FIFO_GO_REG,0);

							// Read back from FIFO
							if(FIFOPCI_ReadDWORD(m_ReadBuffer,FIFO_INTR_TEST_BLOCK_SIZE) == -1)
							{
								SetResultDescription("FIFO_FF read failed");
								return trNoGo;
							}
								
							EdenPCISystem_INTREnable(LOCAL_FIFO);
							EdenPCI_WriteDWORD(AD_PCI_BAR2,FIFO_GO_REG,1);
						}		
						/**/
						return trNoGo;
					}
			}

			SetActualVsRequested(QFormatStr("0xFFFFFFFF %X times",t),QFormatStr("0xFFFFFFFF %X times",t));
			break;
		//step #3	
		case FIFO_INTR_00:
			CQLog::Write(LOG_TAG_GENERAL,"step #3 ");
		    /* CLEAR ALL BUFFERS */
			clearBuffers() ; 
			
		    for(int i = 0; i < FIFO_INTR_TEST_BLOCK_SIZE * FIFO_INTR_TEST_BLOCKS_NUM; i++)
				m_WriteBuffer[i] = 0x00000000;
				
		    FIFOPCI_WriteAsync((PBYTE)m_WriteBuffer,FIFO_INTR_TEST_BLOCK_SIZE * FIFO_INTR_TEST_BLOCKS_NUM);
			
			for(int i = 0; i < FIFO_INTR_TEST_BLOCKS_NUM; i++)
			{
				// Give the interrupt some time for writing the data
				Sleep(100);

				// Disabel interrupts and clear go flag for read back operation
				EdenPCISystem_INTRDisable(LOCAL_FIFO);
				EdenPCI_WriteDWORD(AD_PCI_BAR2,FIFO_GO_REG,0);

				// Read back from FIFO
				if(FIFOPCI_ReadDWORD(m_ReadBuffer,FIFO_INTR_TEST_BLOCK_SIZE) == -1)
				{
					SetResultDescription("FIFO_00 read failed");
					return trNoGo;
				}

				EdenPCISystem_INTREnable(LOCAL_FIFO);
				EdenPCI_WriteDWORD(AD_PCI_BAR2,FIFO_GO_REG,1);

				// Check read buffer content
				for(int j = 0; j < FIFO_INTR_TEST_BLOCK_SIZE; j++)
					if(m_ReadBuffer[j] != 0x00000000)
					{
						SetActualVsRequested(QFormatStr("0x00000000"),QFormatStr("%X",m_ReadBuffer[j]),"Failed after reading " + QIntToStr(j) + " DWORDs.");
						SetResultDescription("FIFO_00 read-back error");
						CQLog::Write(LOG_TAG_GENERAL,QFormatStr("ERROR: Written: %s ",FormatNumToBinary(0x00000000)) +QFormatStr("Read: %s ",FormatNumToBinary(m_ReadBuffer[j])));
						for (int k = i+1  ; k < FIFO_INTR_TEST_BLOCKS_NUM  ; k++){
							// Give the interrupt some time for writing the data
							Sleep(100);

							// Disabel interrupts and clear go flag for read back operation
							EdenPCISystem_INTRDisable(LOCAL_FIFO);
							EdenPCI_WriteDWORD(AD_PCI_BAR2,FIFO_GO_REG,0);

							// Read back from FIFO
							if(FIFOPCI_ReadDWORD(m_ReadBuffer,FIFO_INTR_TEST_BLOCK_SIZE) == -1)
							{
								SetResultDescription("FIFO_FF read failed");
								return trNoGo;
							}
								
							EdenPCISystem_INTREnable(LOCAL_FIFO);
							EdenPCI_WriteDWORD(AD_PCI_BAR2,FIFO_GO_REG,1);
						}	
						return trNoGo;
					}
			}

			SetActualVsRequested(QFormatStr("0x00000000 %X times",t),QFormatStr("0x00000000 %X times",t));
			break;	
			
		//step #4	
		case FIFO_INTR_55:	
		    //CQLog::Write(LOG_TAG_GENERAL,"step #4 ");
		    /* CLEAR ALL BUFFERS */
			clearBuffers() ; 
			
		    for(int i = 0; i < FIFO_INTR_TEST_BLOCK_SIZE * FIFO_INTR_TEST_BLOCKS_NUM; i++)
				m_WriteBuffer[i] = 0x55555555;
				
		    FIFOPCI_WriteAsync((PBYTE)m_WriteBuffer,FIFO_INTR_TEST_BLOCK_SIZE * FIFO_INTR_TEST_BLOCKS_NUM);
			
			for(int i = 0; i < FIFO_INTR_TEST_BLOCKS_NUM; i++)
			{
				// Give the interrupt some time for writing the data
				Sleep(100);

				// Disabel interrupts and clear go flag for read back operation
				EdenPCISystem_INTRDisable(LOCAL_FIFO);
				EdenPCI_WriteDWORD(AD_PCI_BAR2,FIFO_GO_REG,0);

				// Read back from FIFO
				if(FIFOPCI_ReadDWORD(m_ReadBuffer,FIFO_INTR_TEST_BLOCK_SIZE) == -1)
				{
					SetResultDescription("FIFO_55 read failed");
					return trNoGo;
				}

				EdenPCISystem_INTREnable(LOCAL_FIFO);
				EdenPCI_WriteDWORD(AD_PCI_BAR2,FIFO_GO_REG,1);

				// Check read buffer content
				for(int j = 0; j < FIFO_INTR_TEST_BLOCK_SIZE; j++)
					if(m_ReadBuffer[j] != 0x55555555)
					{
						SetActualVsRequested(QFormatStr("0x55555555"),QFormatStr("%X",m_ReadBuffer[j]),"Failed after reading " + QIntToStr(j) + " DWORDs.");
						SetResultDescription("FIFO_55 read-back error");
						CQLog::Write(LOG_TAG_GENERAL,QFormatStr("ERROR: Written: %s ",FormatNumToBinary(0x55555555)) +QFormatStr("Read: %s ",FormatNumToBinary(m_ReadBuffer[j])));
						for (int k = i+1  ; k < FIFO_INTR_TEST_BLOCKS_NUM  ; k++){
							// Give the interrupt some time for writing the data
							Sleep(100);

							// Disabel interrupts and clear go flag for read back operation
							EdenPCISystem_INTRDisable(LOCAL_FIFO);
							EdenPCI_WriteDWORD(AD_PCI_BAR2,FIFO_GO_REG,0);

							// Read back from FIFO
							if(FIFOPCI_ReadDWORD(m_ReadBuffer,FIFO_INTR_TEST_BLOCK_SIZE) == -1)
							{
								SetResultDescription("FIFO_FF read failed");
								return trNoGo;
							}
								
							EdenPCISystem_INTREnable(LOCAL_FIFO);
							EdenPCI_WriteDWORD(AD_PCI_BAR2,FIFO_GO_REG,1);
						}	
						return trNoGo;
					}
			}

			SetActualVsRequested(QFormatStr("0x55555555 %X times",t),QFormatStr("0x55555555 %X times",t));
			break;
			
		//step #5	
		case FIFO_INTR_AA:	
		   // CQLog::Write(LOG_TAG_GENERAL,"step #5 ");
		    /* CLEAR ALL BUFFERS */
			clearBuffers() ; 
			
		    for(int i = 0; i < FIFO_INTR_TEST_BLOCK_SIZE * FIFO_INTR_TEST_BLOCKS_NUM; i++)
				m_WriteBuffer[i] = 0xAAAAAAAA;
				
		    FIFOPCI_WriteAsync((PBYTE)m_WriteBuffer,FIFO_INTR_TEST_BLOCK_SIZE * FIFO_INTR_TEST_BLOCKS_NUM);
			
			for(int i = 0; i < FIFO_INTR_TEST_BLOCKS_NUM; i++)
			{
				// Give the interrupt some time for writing the data
				Sleep(100);

				// Disabel interrupts and clear go flag for read back operation
				EdenPCISystem_INTRDisable(LOCAL_FIFO);
				EdenPCI_WriteDWORD(AD_PCI_BAR2,FIFO_GO_REG,0);

				// Read back from FIFO
				if(FIFOPCI_ReadDWORD(m_ReadBuffer,FIFO_INTR_TEST_BLOCK_SIZE) == -1)
				{
					SetResultDescription("FIFO_AA read failed");
					return trNoGo;
				}

				EdenPCISystem_INTREnable(LOCAL_FIFO);
				EdenPCI_WriteDWORD(AD_PCI_BAR2,FIFO_GO_REG,1);

				// Check read buffer content
				for(int j = 0; j < FIFO_INTR_TEST_BLOCK_SIZE; j++)
					if(m_ReadBuffer[j] != 0xAAAAAAAA)
					{
						SetActualVsRequested(QFormatStr("0xAAAAAAAA"),QFormatStr("%X",m_ReadBuffer[j]),"Failed after reading " + QIntToStr(j) + " DWORDs.");
						SetResultDescription("FIFO_AA read-back error");
						CQLog::Write(LOG_TAG_GENERAL,QFormatStr("ERROR: Written: %s ",FormatNumToBinary(0xAAAAAAAA)) +QFormatStr("Read: %s ",FormatNumToBinary(m_ReadBuffer[j])));
						for (int k = i+1  ; k < FIFO_INTR_TEST_BLOCKS_NUM  ; k++){
							// Give the interrupt some time for writing the data
							Sleep(100);

							// Disabel interrupts and clear go flag for read back operation
							EdenPCISystem_INTRDisable(LOCAL_FIFO);
							EdenPCI_WriteDWORD(AD_PCI_BAR2,FIFO_GO_REG,0);

							// Read back from FIFO
							if(FIFOPCI_ReadDWORD(m_ReadBuffer,FIFO_INTR_TEST_BLOCK_SIZE) == -1)
							{
								SetResultDescription("FIFO_FF read failed");
								return trNoGo;
							}
								
							EdenPCISystem_INTREnable(LOCAL_FIFO);
							EdenPCI_WriteDWORD(AD_PCI_BAR2,FIFO_GO_REG,1);
						}	
						return trNoGo;
					}
			}

			SetActualVsRequested(QFormatStr("0xAAAAAAAA %X times",t),QFormatStr("0xAAAAAAAA %X times",t));
			return trGo;
			
			
		default:
            SetResultDescription("Invalid step number");
            return trNoGo;			

			
	  }
	  
	  //return trGo;
	  return trUnknown;
    }

  public:
    // Constructor
    CFIFOInterruptTest(void) : CSingleTest("FIFO Interrupt Test") {
      AddStep("Testing FIFO");
	  AddStep("Testing FIFO 0xFF");
      AddStep("Testing FIFO 0x00"); 
	  AddStep("Testing FIFO 0x55");
	  AddStep("Testing FIFO 0xAA");
      m_WriteBuffer = NULL;
      m_ReadBuffer = NULL;
    }

};

const FIFO_WRITE_STEP     = 0 ; 
const FIFO_READ_STEP      = 1 ;
const FIFO_WRITE_FF_STEP  = 2 ;
const FIFO_READ_FF_STEP   = 3 ;
const FIFO_WRITE_00_STEP  = 4 ;
const FIFO_READ_00_STEP   = 5 ;
const FIFO_WRITE_55_STEP  = 6 ;
const FIFO_READ_55_STEP   = 7 ;
const FIFO_WRITE_AA_STEP  = 8 ;
const FIFO_READ_AA_STEP   = 9 ;

int StatusChangesCount;

// PCI FIFO interrupt test
class CFIFONonInterruptTest : public CSingleTest
{
  private:
    TTestResult Start(void)
    {
	  StatusChangesCount = 0 ;	
	  //CQLog::Write(LOG_TAG_GENERAL,"-------------I'm in FIFO_READ_STEP---------------------binary check:  %X", 0xFFFFFFFF);
      return trGo;
    }

    TTestResult End(void)
    {
      EdenPCISystem_WriteWS(INITIAL_WS);    
      EdenPCI_FIFOSetReadMode(FF_READ_HIGH_SPEED);
      return trGo;
    }

    // Check a FIFO flag according to a step number
    bool CheckFIFOExpectedStatus(int StepNum,unsigned Status)
    {

      if(Status == FIFO_CAPACITY_ERROR)
      {
        SetActualVsRequested("N/A","N/A","FIFO capacity error");
        return false;
      }

      switch(StepNum)
      {
        case 0:
          if(Status != FIFO_EMPTY)
          {
            SetActualVsRequested("FIFO_EMPTY",QFormatStr("Status: %x",Status));
            return false;
          }
          break;

        case 1:
          if(Status != FIFO_1_TO_AE)
          {
            SetActualVsRequested("FIFO_1_TO_AE",QFormatStr("Status: %x",Status));
            return false;
          }
          break;

        case 2:
          if(Status != FIFO_AE_TO_HF)
          {
            SetActualVsRequested("FIFO_AE_TO_HF",QFormatStr("Status: %x",Status));
            return false;
          }
          break;

        case 3:
          if(Status != FIFO_HF_TO_AF)
          {
            SetActualVsRequested("FIFO_HF_TO_AF",QFormatStr("Status: %x",Status));
            return false;
          }
          break;

        case 4:
          if(Status != FIFO_AF_TO_FULL)
          {
            SetActualVsRequested("FIFO_AF_TO_FULL",QFormatStr("Status: %x",Status));
            return false;
          }
          break;

        case 5:
          if(Status != FIFO_FULL)
          {
            SetActualVsRequested("FIFO_FULL",QFormatStr("Status: %x",Status));
            return false;
          }
          break;
      }

      return true;
    }
	
	/* translate from hex to Binary */ 
    QString FormatNumToBinary(BYTE n)
    {

		QString out = "";
		for(int i = 0; i < 8; i++) {
			out += (n & 0x80) ? "1" : "0";
			n <<= 1;

		}
		return out;

	}

    // Test implementation function (override)
    TTestResult Execute(int StepNumber)
    {
      int i,j;
      DWORD ReadValue,WriteValue = 0;
      unsigned PrevStatus;
      

  	  if(!Q2RTApplication->IsPciCardInitialized())
      {
         SetResultDescription("PCI card not found");
         return trNoGo;
      }

      switch(StepNumber)
      {
        case FIFO_WRITE_STEP:
          FIFOPCI_MasterReset();
          EdenPCI_FIFOSetReadMode(FF_READ_MANUAL);

          PrevStatus = FIFOPCI_ReadStatus();

          // Check if empty
          if(PrevStatus != FIFO_EMPTY)
          { 
            SetActualVsRequested("FIFO_EMPTY (0)",QFormatStr("Status: %d",PrevStatus));
            SetResultDescription("FIFO should be empty after reset");
            return trNoGo;
          }

          // WS 0  -
          EdenPCISystem_WriteWS(FIFO_WS);

          // Fill the fifo with data, and check for status flag changes
          for(i = 0; i < MAX_FIFO_SIZE; i++)
          {
            // Write one dword
            EdenPCI_WriteDWORD(AD_PCI_BAR2,FIFO_WRITE_REG,WriteValue);
            WriteValue++;

            unsigned Status = FIFOPCI_ReadStatus();
			
            //CQLog::Write(LOG_TAG_GENERAL,"Status: "+ QIntToStr(Status));
            
			if(Status != PrevStatus)
            {
              StatusChangesCount++;

              if(!CheckFIFOExpectedStatus(StatusChangesCount,Status))
              {
                SetResultDescription("FIFO status change error");
                EdenPCISystem_WriteWS(INITIAL_WS);
                return trNoGo;
              }

              PrevStatus = Status;
            }
          }

          Sleep(1);

          // Return the WS back
          EdenPCISystem_WriteWS(INITIAL_WS);

          if(StatusChangesCount != 5)
          {
            SetActualVsRequested("5",QIntToStr(StatusChangesCount),"Not all status bits changed during write");
            SetResultDescription("FIFO status change error");
            return trNoGo;
          }
		  
		  SetActualVsRequested(QFormatStr("%X  DWORDS",MAX_FIFO_SIZE),QFormatStr("%X  DWORDS",MAX_FIFO_SIZE));
          break;

        // Step #2
        case FIFO_READ_STEP:
		  //CQLog::Write(LOG_TAG_GENERAL,"-------------I'm in FIFO_READ_STEP---------------------binary check:  %b", 15);
		  //CQLog::Write(LOG_TAG_GENERAL,"-------------I'm in FIFO_READ_STEP---------------------binary check:" );

          EdenPCI_FIFOSetReadMode(FF_READ_MANUAL);
          WriteValue = 0;
          
          // Read from the fifo and check for status flag changes
          for(i = 0; i < MAX_FIFO_SIZE; i++)
          {
            // Read one dword
            if(FIFOPCI_ReadDWORD(&ReadValue,1) == -1)
            {
              SetResultDescription("FIFO read failed");
              return trNoGo;
            }
			//CQLog::Write(LOG_TAG_GENERAL,QFormatStr("EXAMPLE: Written: %s ",FormatNumToBinary(WriteValue)) +QFormatStr("Read: %s ",FormatNumToBinary(ReadValue)));
            if(ReadValue != WriteValue)
            {
              SetActualVsRequested(QFormatStr("%X",WriteValue),QFormatStr("%X",ReadValue),
                                   "Failed after reading " + QIntToStr(WriteValue) + " DWORDs.");
              SetResultDescription("FIFO read-back error");
			  //CQLog::Write(LOG_TAG_GENERAL,QFormatStr("ERROR: Written: %X ",WriteValue) +QFormatStr("Read: %X ",ReadValue) );
			  /* WRITE INTO LOG IN BINARY */
			  CQLog::Write(LOG_TAG_GENERAL,QFormatStr("ERROR: Written: %s ",FormatNumToBinary(WriteValue)) +QFormatStr("Read: %s ",FormatNumToBinary(ReadValue)));
             /*read till the end for the fifo to be empty */
			   for(j= i +1 ; j< MAX_FIFO_SIZE; j++){
					// Read one dword
					if(FIFOPCI_ReadDWORD(&ReadValue,1) == -1)
					{
						SetResultDescription("FIFO_FF read failed");
						return trNoGo;
					}
			   }
			 return trNoGo;
            }

            WriteValue++;

            unsigned Status = FIFOPCI_ReadStatus();
            if(Status != PrevStatus)
            {
              StatusChangesCount--;

              if(!CheckFIFOExpectedStatus(StatusChangesCount,Status))
              {
                SetResultDescription("FIFO status change error");
                return trNoGo;
              }

              PrevStatus = Status;
            }
          }

          if(StatusChangesCount != 0)
          {
            SetActualVsRequested("0",QIntToStr(StatusChangesCount),"Not all status bits changed during read");
            SetResultDescription("FIFO status change error");
            return trNoGo;
          }
		  
		  SetActualVsRequested(QFormatStr("%X DWORDS",MAX_FIFO_SIZE),QFormatStr("%X DWORDS",MAX_FIFO_SIZE));
		  break; 
		  
		 //step #3
        case FIFO_WRITE_FF_STEP:
		  StatusChangesCount =0 ;
		  //CQLog::Write(LOG_TAG_GENERAL,"------------------I'm in FIFO_WRITE_FF_STEP---------------");
		  WriteValue = 0;
		  FIFOPCI_MasterReset();
          EdenPCI_FIFOSetReadMode(FF_READ_MANUAL);

          PrevStatus = FIFOPCI_ReadStatus();

          // Check if empty
          if(PrevStatus != FIFO_EMPTY)
          {
            SetActualVsRequested("FIFO_EMPTY (0)",QFormatStr("Status: %d",PrevStatus));
            SetResultDescription("FIFO should be empty after reset");
            return trNoGo;
          }

          // WS 0  -
          EdenPCISystem_WriteWS(FIFO_WS);

          // Fill the fifo with data, and check for status flag changes
          for(i = 0; i < MAX_FIFO_SIZE; i++)
          {
            // Write one dword
            EdenPCI_WriteDWORD(AD_PCI_BAR2,FIFO_WRITE_REG,0xFFFFFFFF);
            WriteValue++;

            unsigned Status = FIFOPCI_ReadStatus();
			
            //CQLog::Write(LOG_TAG_GENERAL,"Status: "+ QIntToStr(Status));
            
			if(Status != PrevStatus)
            {
              StatusChangesCount++;

              if(!CheckFIFOExpectedStatus(StatusChangesCount,Status))
              {
                SetResultDescription("FIFO_FF status change error");
                EdenPCISystem_WriteWS(INITIAL_WS);
                return trNoGo;
              }

              PrevStatus = Status;
            }
          }

          Sleep(1);

          // Return the WS back
          EdenPCISystem_WriteWS(INITIAL_WS);

          if(StatusChangesCount != 5)
          {
            SetActualVsRequested("5",QIntToStr(StatusChangesCount),"Not all status bits changed during write");
            SetResultDescription("FIFO_FF status change error");
            return trNoGo;
          } 
		  SetActualVsRequested(QFormatStr("0xFFFFFFFF %X times",MAX_FIFO_SIZE),QFormatStr("0xFFFFFFFF %X times",MAX_FIFO_SIZE));
		  break;
		  //step #4
        case FIFO_READ_FF_STEP:
		
		  //CQLog::Write(LOG_TAG_GENERAL,"------------------I'm in FIFO_READ_FF_STEP---------------");
		  EdenPCI_FIFOSetReadMode(FF_READ_MANUAL);
          WriteValue = 0;

          // Read from the fifo and check for status flag changes
          for(i = 0; i < MAX_FIFO_SIZE; i++)
          {
            // Read one dword
            if(FIFOPCI_ReadDWORD(&ReadValue,1) == -1)
            {
              SetResultDescription("FIFO_FF read failed");
              return trNoGo;
            }

            if(ReadValue != 0xFFFFFFFF)
            {
              SetActualVsRequested(QFormatStr("%X",0xFFFFFFFF),QFormatStr("%X",ReadValue),
                                   "Failed after reading " + QIntToStr(WriteValue) + " DWORDs.");
              SetResultDescription("FIFO_FF read-back error");
			  //CQLog::Write(LOG_TAG_GENERAL,QFormatStr("ERROR: Written: %X ",WriteValue) +QFormatStr("Read: %X ",ReadValue) );
			  /* WRITE INTO LOG IN BINARY */
			  CQLog::Write(LOG_TAG_GENERAL,QFormatStr("ERROR: Written: %s ",FormatNumToBinary(WriteValue)) +QFormatStr("Read: %s ",FormatNumToBinary(ReadValue)));
              //FIFOPCI_MasterReset() ; 
			  /*read till the end for the fifo to be empty */
			   for(j= i +1 ; j< MAX_FIFO_SIZE; j++){
					// Read one dword
					if(FIFOPCI_ReadDWORD(&ReadValue,1) == -1)
					{
						SetResultDescription("FIFO_FF read failed");
						return trNoGo;
					}
			   }
			  /**/
			  return trNoGo;
            }

            WriteValue++;

            unsigned Status = FIFOPCI_ReadStatus();
            if(Status != PrevStatus)
            {
              StatusChangesCount--;

              if(!CheckFIFOExpectedStatus(StatusChangesCount,Status))
              {
                SetResultDescription("FIFO_FF status change error");
                return trNoGo;
              }

              PrevStatus = Status;
            }
          }

          if(StatusChangesCount != 0)
          {
            SetActualVsRequested("0",QIntToStr(StatusChangesCount),"Not all status bits changed during read of 0xFFFFFFFF");
            SetResultDescription("FIFO_FF status change error");
            return trNoGo;
          }
		  
		  SetActualVsRequested(QFormatStr("0xFFFFFFFF %X times",MAX_FIFO_SIZE),QFormatStr("0xFFFFFFFF %X times",MAX_FIFO_SIZE));
		  
		  break;
		  
		  //step #5
        case FIFO_WRITE_00_STEP:
		  StatusChangesCount =0 ;
		  //CQLog::Write(LOG_TAG_GENERAL,"------------------I'm in FIFO_WRITE_FF_STEP---------------");
		  WriteValue = 0;
		  FIFOPCI_MasterReset();
          EdenPCI_FIFOSetReadMode(FF_READ_MANUAL);

          PrevStatus = FIFOPCI_ReadStatus();

          // Check if empty
          if(PrevStatus != FIFO_EMPTY)
          {
            SetActualVsRequested("FIFO_EMPTY (0)",QFormatStr("Status: %d",PrevStatus));
            SetResultDescription("FIFO should be empty after reset");
            return trNoGo;
          }

          // WS 0  -
          EdenPCISystem_WriteWS(FIFO_WS);

          // Fill the fifo with data, and check for status flag changes
          for(i = 0; i < MAX_FIFO_SIZE; i++)
          {
            // Write one dword
            EdenPCI_WriteDWORD(AD_PCI_BAR2,FIFO_WRITE_REG,0x00000000);
            WriteValue++;

            unsigned Status = FIFOPCI_ReadStatus();
			
            //CQLog::Write(LOG_TAG_GENERAL,"Status: "+ QIntToStr(Status));
            
			if(Status != PrevStatus)
            {
              StatusChangesCount++;

              if(!CheckFIFOExpectedStatus(StatusChangesCount,Status))
              {
                SetResultDescription("FIFO_00 status change error");
                EdenPCISystem_WriteWS(INITIAL_WS);
                return trNoGo;
              }

              PrevStatus = Status;
            }
          }

          Sleep(1);

          // Return the WS back
          EdenPCISystem_WriteWS(INITIAL_WS);

          if(StatusChangesCount != 5)
          {
            SetActualVsRequested("5",QIntToStr(StatusChangesCount),"Not all status bits changed during write 0x00000000");
            SetResultDescription("FIFO_00 status change error");
            return trNoGo;
          } 
		  SetActualVsRequested(QFormatStr("0x00000000 %X times",MAX_FIFO_SIZE),QFormatStr("0x00000000 %X times",MAX_FIFO_SIZE));
		  break;
		  
		 //step #6  
		case FIFO_READ_00_STEP:
		
		  //CQLog::Write(LOG_TAG_GENERAL,"------------------I'm in FIFO_READ_FF_STEP---------------");
		  EdenPCI_FIFOSetReadMode(FF_READ_MANUAL);
          WriteValue = 0;

          // Read from the fifo and check for status flag changes
          for(i = 0; i < MAX_FIFO_SIZE; i++)
          {
            // Read one dword
            if(FIFOPCI_ReadDWORD(&ReadValue,1) == -1)
            {
              SetResultDescription("FIFO_00 read failed");
              return trNoGo;
            }

            if(ReadValue != 0x00000000)
            {
              SetActualVsRequested(QFormatStr("%X",0x00000000),QFormatStr("%X",ReadValue),
                                   "Failed after reading " + QIntToStr(WriteValue) + " DWORDs.");
              SetResultDescription("FIFO_00 read-back error");
			  //CQLog::Write(LOG_TAG_GENERAL,QFormatStr("ERROR: Written: %X ",WriteValue) +QFormatStr("Read: %X ",ReadValue) );
			  /* WRITE INTO LOG IN BINARY */
			  CQLog::Write(LOG_TAG_GENERAL,QFormatStr("ERROR: Written: %s ",FormatNumToBinary(WriteValue)) +QFormatStr("Read: %s ",FormatNumToBinary(ReadValue)));
              /*read till the end for the fifo to be empty */
			   for(j= i +1 ; j< MAX_FIFO_SIZE; j++){
					// Read one dword
					if(FIFOPCI_ReadDWORD(&ReadValue,1) == -1)
					{
						SetResultDescription("FIFO_FF read failed");
						return trNoGo;
					}
			   }
			  return trNoGo;
            }

            WriteValue++;

            unsigned Status = FIFOPCI_ReadStatus();
            if(Status != PrevStatus)
            {
              StatusChangesCount--;

              if(!CheckFIFOExpectedStatus(StatusChangesCount,Status))
              {
                SetResultDescription("FIFO_00 status change error");
                return trNoGo;
              }

              PrevStatus = Status;
            }
          }

          if(StatusChangesCount != 0)
          {
            SetActualVsRequested("0",QIntToStr(StatusChangesCount),"Not all status bits changed during read of 0x00000000");
            SetResultDescription("FIFO_00 status change error");
            return trNoGo;
          }
		  
		  SetActualVsRequested(QFormatStr("0x00000000 %X times",MAX_FIFO_SIZE),QFormatStr("0x00000000 %X times",MAX_FIFO_SIZE));
		  
		  break;
		//step #7
        case FIFO_WRITE_55_STEP:
		  StatusChangesCount =0 ;
		  //CQLog::Write(LOG_TAG_GENERAL,"------------------I'm in FIFO_WRITE_FF_STEP---------------");
		  WriteValue = 0;
		  FIFOPCI_MasterReset();
          EdenPCI_FIFOSetReadMode(FF_READ_MANUAL);

          PrevStatus = FIFOPCI_ReadStatus();

          // Check if empty
          if(PrevStatus != FIFO_EMPTY)
          {
            SetActualVsRequested("FIFO_EMPTY (0)",QFormatStr("Status: %d",PrevStatus));
            SetResultDescription("FIFO should be empty after reset");
            return trNoGo;
          }

          // WS 0  -
          EdenPCISystem_WriteWS(FIFO_WS);

          // Fill the fifo with data, and check for status flag changes
          for(i = 0; i < MAX_FIFO_SIZE; i++)
          {
            // Write one dword
            EdenPCI_WriteDWORD(AD_PCI_BAR2,FIFO_WRITE_REG,0x55555555);
            WriteValue++;

            unsigned Status = FIFOPCI_ReadStatus();
			
            //CQLog::Write(LOG_TAG_GENERAL,"Status: "+ QIntToStr(Status));
            
			if(Status != PrevStatus)
            {
              StatusChangesCount++;

              if(!CheckFIFOExpectedStatus(StatusChangesCount,Status))
              {
                SetResultDescription("FIFO_55 status change error");
                EdenPCISystem_WriteWS(INITIAL_WS);
                return trNoGo;
              }

              PrevStatus = Status;
            }
          }

          Sleep(1);

          // Return the WS back
          EdenPCISystem_WriteWS(INITIAL_WS);

          if(StatusChangesCount != 5)
          {
            SetActualVsRequested("5",QIntToStr(StatusChangesCount),"Not all status bits changed during write");
            SetResultDescription("FIFO_55 status change error");
            return trNoGo;
          } 
		  SetActualVsRequested(QFormatStr("0x55555555 %X times",MAX_FIFO_SIZE),QFormatStr("0x55555555 %X times",MAX_FIFO_SIZE));
		  break;
		  
		  //step #8
        case FIFO_READ_55_STEP:
		
		  //CQLog::Write(LOG_TAG_GENERAL,"------------------I'm in FIFO_READ_FF_STEP---------------");
		  EdenPCI_FIFOSetReadMode(FF_READ_MANUAL);
          WriteValue = 0;

          // Read from the fifo and check for status flag changes
          for(i = 0; i < MAX_FIFO_SIZE; i++)
          {
            // Read one dword
            if(FIFOPCI_ReadDWORD(&ReadValue,1) == -1)
            {
              SetResultDescription("FIFO_55 read failed");
              return trNoGo;
            }

            if(ReadValue != 0x55555555)
            {
              SetActualVsRequested(QFormatStr("%X",0x55555555),QFormatStr("%X",ReadValue),
                                   "Failed after reading " + QIntToStr(WriteValue) + " DWORDs.");
              SetResultDescription("FIFO_55 read-back error");
			  //CQLog::Write(LOG_TAG_GENERAL,QFormatStr("ERROR: Written: %X ",WriteValue) +QFormatStr("Read: %X ",ReadValue) );
			  /* WRITE INTO LOG IN BINARY */
			  CQLog::Write(LOG_TAG_GENERAL,QFormatStr("ERROR: Written: %s ",FormatNumToBinary(WriteValue)) +QFormatStr("Read: %s ",FormatNumToBinary(ReadValue)));
              /*read till the end for the fifo to be empty */
			   for(j= i +1 ; j< MAX_FIFO_SIZE; j++){
					// Read one dword
					if(FIFOPCI_ReadDWORD(&ReadValue,1) == -1)
					{
						SetResultDescription("FIFO_FF read failed");
						return trNoGo;
					}
			   }
			  return trNoGo;
            }

            WriteValue++;

            unsigned Status = FIFOPCI_ReadStatus();
            if(Status != PrevStatus)
            {
              StatusChangesCount--;

              if(!CheckFIFOExpectedStatus(StatusChangesCount,Status))
              {
                SetResultDescription("FIFO_55 status change error");
                return trNoGo;
              }

              PrevStatus = Status;
            }
          }

          if(StatusChangesCount != 0)
          {
            SetActualVsRequested("0",QIntToStr(StatusChangesCount),"Not all status bits changed during read of 0x55555555");
            SetResultDescription("FIFO_55 status change error");
            return trNoGo;
          }
		  
		  SetActualVsRequested(QFormatStr("0x55555555 %X times",MAX_FIFO_SIZE),QFormatStr("0x55555555 %X times",MAX_FIFO_SIZE));
		  
		  //for last test - it passed all till now retrun test passed
		  //return trGo;
		  
		  break;
		 //step #9
        case FIFO_WRITE_AA_STEP:
		  StatusChangesCount =0 ;
		  //CQLog::Write(LOG_TAG_GENERAL,"------------------I'm in FIFO_WRITE_FF_STEP---------------");
		  WriteValue = 0;
		  FIFOPCI_MasterReset();
          EdenPCI_FIFOSetReadMode(FF_READ_MANUAL);

          PrevStatus = FIFOPCI_ReadStatus();

          // Check if empty
          if(PrevStatus != FIFO_EMPTY)
          {
            SetActualVsRequested("FIFO_EMPTY (0)",QFormatStr("Status: %d",PrevStatus));
            SetResultDescription("FIFO should be empty after reset");
            return trNoGo;
          }

          // WS 0  -
          EdenPCISystem_WriteWS(FIFO_WS);

          // Fill the fifo with data, and check for status flag changes
          for(i = 0; i < MAX_FIFO_SIZE; i++)
          {
            // Write one dword
            EdenPCI_WriteDWORD(AD_PCI_BAR2,FIFO_WRITE_REG,0xAAAAAAAA);
            WriteValue++;

            unsigned Status = FIFOPCI_ReadStatus();
			
            //CQLog::Write(LOG_TAG_GENERAL,"Status: "+ QIntToStr(Status));
            
			if(Status != PrevStatus)
            {
              StatusChangesCount++;

              if(!CheckFIFOExpectedStatus(StatusChangesCount,Status))
              {
                SetResultDescription("FIFO_AA status change error");
                EdenPCISystem_WriteWS(INITIAL_WS);
                return trNoGo;
              }

              PrevStatus = Status;
            }
          }

          Sleep(1);

          // Return the WS back
          EdenPCISystem_WriteWS(INITIAL_WS);

          if(StatusChangesCount != 5)
          {
            SetActualVsRequested("5",QIntToStr(StatusChangesCount),"Not all status bits changed during write");
            SetResultDescription("FIFO_AA status change error");
            return trNoGo;
          } 
		  SetActualVsRequested(QFormatStr("0xAAAAAAAA %X times",MAX_FIFO_SIZE),QFormatStr("0xAAAAAAAA %X times",MAX_FIFO_SIZE));
		  break;
		  //step #10
        case FIFO_READ_AA_STEP:
		
		  //CQLog::Write(LOG_TAG_GENERAL,"------------------I'm in FIFO_READ_FF_STEP---------------");
		  EdenPCI_FIFOSetReadMode(FF_READ_MANUAL);
          WriteValue = 0;

          // Read from the fifo and check for status flag changes
          for(i = 0; i < MAX_FIFO_SIZE; i++)
          {
            // Read one dword
            if(FIFOPCI_ReadDWORD(&ReadValue,1) == -1)
            {
              SetResultDescription("FIFO_AA read failed");
              return trNoGo;
            }

            if(ReadValue != 0xAAAAAAAA)
            {
              SetActualVsRequested(QFormatStr("%X",0xAAAAAAAA),QFormatStr("%X",ReadValue),
                                   "Failed after reading " + QIntToStr(WriteValue) + " DWORDs.");
              SetResultDescription("FIFO_FF read-back error");
			  //CQLog::Write(LOG_TAG_GENERAL,QFormatStr("ERROR: Written: %X ",WriteValue) +QFormatStr("Read: %X ",ReadValue) );
			  /* WRITE INTO LOG IN BINARY */
			  CQLog::Write(LOG_TAG_GENERAL,QFormatStr("ERROR: Written: %s ",FormatNumToBinary(WriteValue)) +QFormatStr("Read: %s ",FormatNumToBinary(ReadValue)));
              /*read till the end for the fifo to be empty */
			   for(j= i +1 ; j< MAX_FIFO_SIZE; j++){
					// Read one dword
					if(FIFOPCI_ReadDWORD(&ReadValue,1) == -1)
					{
						SetResultDescription("FIFO_FF read failed");
						return trNoGo;
					}
			   }
			  return trNoGo;
            }

            WriteValue++;

            unsigned Status = FIFOPCI_ReadStatus();
            if(Status != PrevStatus)
            {
              StatusChangesCount--;

              if(!CheckFIFOExpectedStatus(StatusChangesCount,Status))
              {
                SetResultDescription("FIFO_AA status change error");
                return trNoGo;
              }

              PrevStatus = Status;
            }
          }

          if(StatusChangesCount != 0)
          {
            SetActualVsRequested("0",QIntToStr(StatusChangesCount),"Not all status bits changed during read of 0xAAAAAAAA");
            SetResultDescription("FIFO_AA status change error");
            return trNoGo;
          }
		  
		  SetActualVsRequested(QFormatStr("0xAAAAAAAA %X times",MAX_FIFO_SIZE),QFormatStr("0xAAAAAAAA %X times",MAX_FIFO_SIZE));
		  
		  //for last test - it passed all till now retrun test passed
		  return trGo;

		  
		default:
            SetResultDescription("Invalid step number");
            return trNoGo;
      }

      
      //return trGo;
	  return trUnknown;
    }

  public:
    // Constructor
    CFIFONonInterruptTest(void) : CSingleTest("FIFO Non-Interrupt Test") {
      AddStep("FIFO Write");
      AddStep("FIFO Read");
      
	  AddStep("FIFO 0xFF Write");
      AddStep("FIFO 0xFF Read"); 
      
	  AddStep("FIFO 0x00 Write");
      AddStep("FIFO 0x00 Read");
	  
	  AddStep("FIFO 0x55 Write");
      AddStep("FIFO 0x55 Read");
      
      AddStep("FIFO 0xAA Write");
      AddStep("FIFO 0xAA Read");  	  
       	  
    }
};

// PCI test group
class CPCITestGroup : public CTestGroup {
  public:
    CPCITestGroup(void) : CTestGroup("Data Card") {
      // add tests
      Add(new CFIFONonInterruptTest);
      Add(new CFIFOInterruptTest);
    }
};

// System environment
// ******************************************************************

#ifdef OS_WINDOWS
// Different test steps
const int TOTAL_PHYSICAL_MEMORY_TEST = 0;
//const int AVAIL_PHYSICAL_MEMORY_TEST = 1; bug 6182
const int DISK_FREE_TEST             = 1; //bug 6182
const int SCREEN_RESOLUTION_TEST     = 2; //bug 6182

// Minimum memory/storage requirements in mb
const int MIN_TOTAL_PHYSICAL_MEM = 7600;
const int MIN_AVAIL_PHYSICAL_MEM = 500;
#if defined(OBJET_1000)
//const int MIN_TOTAL_PHYSICAL_MEM = 7600;
//const int MIN_AVAIL_PHYSICAL_MEM = 500;
const int MIN_AVAIL_DISK_SPACE   = 200000;
#else
//const int MIN_TOTAL_PHYSICAL_MEM = 950;
//const int MIN_AVAIL_PHYSICAL_MEM = 500;
const int MIN_AVAIL_DISK_SPACE   = 1000;
#endif

// Minimum screen resolution in pixels
const int MIN_SCREEN_WIDTH  = 1024;
const int MIN_SCREEN_HEIGHT = 768;

// Computer environment test
class CSystemInfoTest : public CSingleTest {
  private:
    bool m_NoGoFlag;
    bool m_WarnFlag;

    TTestResult Start(void) {
      m_NoGoFlag = m_WarnFlag = false;
      return trGo;
    }

    // Test implementation function (override)
    TTestResult Execute(int StepNumber) {
      switch(StepNumber)
      {
        case TOTAL_PHYSICAL_MEMORY_TEST:
          {
//            DWORDLONG TotalMemInMB;
            UINT_PTR  TotalMemInMB;
            if(IsRunningOnWindows64Bit())
            {
				MEMORYSTATUSEX statex;   //for 64bit
				statex.dwLength = sizeof (statex);  //must be done on 64bit OS
            	::GlobalMemoryStatusEx (&statex);
            	TotalMemInMB = ((statex.ullTotalPhys)/(1024 * 1024)); //64bit
            }
            else
            {
             // Get memory info structure and check the amount of physical memory
            	MEMORYSTATUS Status;
            	::GlobalMemoryStatus(&Status);
            	TotalMemInMB = (Status.dwTotalPhys) / (1024 * 1024); //32bit
            }

			
            QString StepErrorStr;

            // Check if we have enough physical memory
            if(TotalMemInMB < MIN_TOTAL_PHYSICAL_MEM)
            {
              StepErrorStr = "Not enough memory";
              SetResultDescription("Environment error");
              m_NoGoFlag = true;
            }
			SetActualVsRequested(QFormatStr("%d MB",MIN_TOTAL_PHYSICAL_MEM),QFormatStr("%d MB",TotalMemInMB),StepErrorStr);
		  }
          break;

        case DISK_FREE_TEST:
          {
            // Get amount of free disk space
            long long FreeSpaceAvailable,TotalSpace;

            // Use Win32 API to get free disk space
            if(::GetDiskFreeSpaceEx(Q2RTApplication->AppFilePath.Value().c_str(),(PULARGE_INTEGER)&FreeSpaceAvailable,
                                  (PULARGE_INTEGER)&TotalSpace,NULL))
            {
              double FreeSpace = FreeSpaceAvailable;

              // Convert to MB
              int FreeSpaceInMB = FreeSpace / (1024 * 1024);
              QString StepErrorStr;

              // Check if we have enough physical memory
              if(FreeSpaceInMB < MIN_AVAIL_DISK_SPACE)
              {
                StepErrorStr = "Not enough free disk space";
                SetResultDescription("Environment error");
                m_NoGoFlag = true;
              }

              SetActualVsRequested(QFormatStr("%d MB",MIN_AVAIL_DISK_SPACE),QFormatStr("%d MB",FreeSpaceInMB),StepErrorStr);
            }
          }
          break;

        case SCREEN_RESOLUTION_TEST:
          {
            int ScreenWidth = ::GetSystemMetrics(SM_CXSCREEN);
            int ScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);

            QString StepErrorStr;

            if((ScreenWidth < MIN_SCREEN_WIDTH) || (ScreenHeight < MIN_SCREEN_HEIGHT))
            {
              StepErrorStr = "Low screen resolution";
              SetResultDescription("Environment warning");
              m_WarnFlag = true;
            }

            SetActualVsRequested(QFormatStr("%d,%d",MIN_SCREEN_WIDTH,MIN_SCREEN_HEIGHT),
                                 QFormatStr("%d,%d",ScreenWidth,ScreenHeight),StepErrorStr);

            if(m_NoGoFlag)
              return trNoGo;

            if(m_WarnFlag)
              return trWarning;

            return trGo;
          }
      }

      return trUnknown;
    }

  public:
    // Constructor
    CSystemInfoTest(void) : CSingleTest("System Info",taBreakAll | taVeryHighPriority) {
	  AddStep("Total Physical Memory");
	  //AddStep("Available Physical Memory");//bug6182
      AddStep("Disk Free Space");
      AddStep("Screen Resolution");
      m_NoGoFlag = false;
      m_WarnFlag = false;
    }
};

// Environment test group
class CEnvironmentGroup : public CTestGroup {
  public:
    CEnvironmentGroup(void) : CTestGroup("Environment") {
      // add tests
      Add(new CSystemInfoTest);
    }
};

#endif

//********************************************************************

// Register application specific tests into the given BIT manager
void Register(CBITManager *BITManager)
{
  BITManager->RegisterTestGroup(new CPCITestGroup);
  BITManager->RegisterTestGroup(new CEnvironmentGroup);
}

} // End of namespace "AppBIT"

