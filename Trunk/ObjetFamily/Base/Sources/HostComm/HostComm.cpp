/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Host-communication                                       *
 * Module Description: This is cross-platform implementation of the *
 *                     host communicaiton thread used in Q1.        *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 10/02/2001                                           *
 * Last upate: 10/07/2002                                           *
 ********************************************************************/

#include <cstring>
#include "HostComm.h"
#include "QTcpIpServer.h"
#include "FrontEnd.h"
#include "Slice.h"
#include "BPECompress.h"
#include "QMonitor.h"
#include "Q2RTApplication.h"
#include "MachineManager.h"
#include "AppLogFile.h"
#include "AppParams.h"
#include "UvLamps.h"
#include "MachineSequencer.h"
#include "QVersionInfo.h"
#include "pkware.h"
#include "Container.h"
#include "Layer.h"
#include "LayerProcessDefs.h"
#include "MotorDefs.h"
#include "ScatterGenerator.h"
#include "Door.h"
#include "ConfigBackupFile.h"
#include "ScatterGenerator.h"
#include "TempAppParams.h" 
#include "QThreadUtils.h"
#include "BMPFile.h"


// If this flag is defined, an extended print-outs are used
#define EXTENDED_HOST_LOG
#define SLICE_HEADER_SIZE 16


#define THE_PRODUCT_SIZE_OF_TRAY_OBJET1000_IN_TRIPLEX_TRAY 4
#define MAX_INCOMING_DATA_SIZE 40000000   //bug 5811
#define MAX_INCOMING_RESINN_DATA_SIZE 1000000//bug 5811
#define MAX_SLICE_MEM_SIZE 15000000   //bug 6171
#define MAX_LEGAL_BYTE 8
#define DM3_NUMBER_OF_ADDITIONAL_MODEL_RESINS 2
#define DM7_NUMBER_OF_ADDITIONAL_MODEL_RESINS 6



const BYTE CURRENT_PRINT       = 0;
const BYTE HIGH_QUALITY        = 1;
const BYTE HIGH_SPEED          = 2;
const BYTE HIGH_QUALITY_3RESIN = 3;
const BYTE HIGH_SPEED_3RESIN   = 4;

const float DEFAULT_LAYER_HEIGHT[NUMBER_OF_QUALITY_MODES] = {30.0f, 16.0f};
const char  CONFIG_BACKUP_FILE_NAME[] = "ConfigBackup\\ConfigBackupForHost.zip";

const int HOST_IP_PORT    = 11;
const unsigned char  SOH  = 2;     // Start of Header for message.
const unsigned char  ACK  = 6;     // Acknowledge.
const unsigned char  NAK  = 21;    // NOT-Acknowledged (error condition).

// Maximum size of transmit buffer for outgoing messages
const int MAX_SEND_BUFFER = 512;
const int HEADER_SIZE     = 7;

// Compression types
const int NO_COMPRESSION     = 0;
const int BPE_COMPRESSION    = 1;
const int PKWARE_COMPRESSION = 2;

const int NO_WEIGHT_SENSORS = 2;

const int EMPTY_SLICE_DEMO_SIZE   = 32;
const int EMPTY_SLICE_DEMO_WIDTH  = 256;
const int EMPTY_SLICE_DEMO_HEIGHT = 1;

const int BITS_PER_PIXEL = 4;

// Class CSlicePreProcessor implementation
// *******************************************************************
// Constructor
CSlicePreProcessor::CSlicePreProcessor(CHostComm *HostComm)
  : CQThread(true,"SlicePreProcessorThread",false)
{
  m_TotalSlicesNum = 0;
  m_isLastSlice = false;
  memset(m_BmpsReceivedFromHostArr, 0, sizeof(m_BmpsReceivedFromHostArr));
  m_NumOfAddModelResins = 0;
  m_HostComm = HostComm;
  Priority = Q_PRIORITY_LOW;

  m_LastSliceAccessMutex = new CQMutex;
  m_PrintEndFlagMutex    = new CQMutex;

  m_PrintEndFlag      = false;
  m_SlicePreviewEvent = NULL;
  m_LastIncomingSlice = NULL;

  // Message queue for the ready slices
  m_SliceMessageQueue = new TSliceMessageQueue(SLICE_QUEUE_SIZE);

  InitPKExtract();

  m_LastPositionY = 0 ;
  /**************************************************************************************************************************/
  /**/ //Attention!!! The following code mout be during run time (because we compile on 32bit and sometimes run on 64 bit)/**/
  /**/ m_Max_Slice_Mem_Size = MAX_SLICE_MEM_SIZE;                                                                         /**/
  /**/if((IsRunningOnWindows64Bit()) && ((Q2RTApplication->GetMachineType()) == (mtObjet1000 )) )                         /**/
  /**/{                                                                                                                   /**/
  /**/	 CQLog::Write(LOG_TAG_GENERAL,"Info: CSlicePreProcessor::Constructor runs on Objet1000 64bit");					  /**/																				          /**/
  /**/   m_Max_Slice_Mem_Size*= THE_PRODUCT_SIZE_OF_TRAY_OBJET1000_IN_TRIPLEX_TRAY;                                       /**/
  /**/}                                                                                                                   /**/
  /**************************************************************************************************************************/
  m_SlicesAllocator.Init((SLICE_QUEUE_SIZE + 2) * NUMBER_OF_CHAMBERS,m_Max_Slice_Mem_Size); //bug 6171
  
}

bool CSlicePreProcessor::IsLastSlice()
{
  return m_isLastSlice;
}

void CSlicePreProcessor::SetTotalSlicesNum(int TotalSlicesNum)
{
  m_TotalSlicesNum = TotalSlicesNum;
}

TSliceMessageQueue *CSlicePreProcessor::GetSliceMessageQueue(void)
{
  return m_SliceMessageQueue;
}

// Set the total number of slices in the current job
int CSlicePreProcessor::GetTotalSlicesNum()
{
  return m_TotalSlicesNum;
}

void CSlicePreProcessor::SetSlicePreviewUpdateEvent(TGenericCallback SlicePreviewEvent,
                                                    TGenericCockie SlicePreviewEventCockie)
{
  m_SlicePreviewEvent       = SlicePreviewEvent;
  m_SlicePreviewEventCockie = SlicePreviewEventCockie;
}

// Destructor
CSlicePreProcessor::~CSlicePreProcessor(void)
{
  Terminate();
  m_SliceMessageQueue->Release();
  WaitFor();

  delete m_SliceMessageQueue;
  delete m_LastSliceAccessMutex;
  delete m_PrintEndFlagMutex;

  DeInitPKExtract();
}

void CSlicePreProcessor::SetBmpsReceivedFromHostArr(int BmpsReceivedFromHost[], short NumOfAddModelResins)
{
   for(int i=0;i<NUMBER_OF_CHAMBERS;i++)
   {
	 m_BmpsReceivedFromHostArr[i] = BmpsReceivedFromHost[i];
   }
   m_NumOfAddModelResins = NumOfAddModelResins;
}

// Unpack a slice
bool CSlicePreProcessor::UnpackSlice(BYTE CompressionType,PBYTE RawSliceData,ULONG RawDataSize,
                                     PBYTE UnpackedSliceData,ULONG MaxUnpackedSize)
{
  // Unpack incoming data according to the compression type
  switch(CompressionType)
  {
    case NO_COMPRESSION:
         // Copy data as is
         memcpy(UnpackedSliceData,RawSliceData,RawDataSize);
         break;

    case BPE_COMPRESSION:
         // Decompress buffer and check if error
         if (BPE_DecompressBuffer(RawSliceData,RawDataSize,UnpackedSliceData,MaxUnpackedSize) == -1)
            return false;
         break;

    case PKWARE_COMPRESSION:
         {
         unsigned DestBufferActualSize;
         int RetCode = PKExtract(RawSliceData,UnpackedSliceData,RawDataSize,MaxUnpackedSize,DestBufferActualSize);

         if(RetCode != 0)
            return false;

         if(DestBufferActualSize != MaxUnpackedSize)
           return false;
         }
         break;

    default:
         return false;
  }

  return true;
}

// Delete all slices currently in the slice queue
void CSlicePreProcessor::FlushSlices(void)
{
  CQLog::Write(LOG_TAG_HOST_COMM,"SlicePreProcessor: Disposing slices");
  m_SliceMessageQueue->UnRelease();
  while(m_SliceMessageQueue->ItemsCount > 0)
  {
    CSlice *TmpPtr;
    if (m_SliceMessageQueue->Receive(TmpPtr) != QLib::wrReleased)
       DisposeSlice(TmpPtr);
    else
       break;
  }
  
  // Free the slices allocated in the pool
  m_SlicesAllocator.ReleaseBlocks();//bug 6171
  //m_SlicesAllocator.DeInit(); //bug 6171 
}

bool CSlicePreProcessor::IsPrintEnd(void)
{
  bool Flag;
  m_PrintEndFlagMutex->WaitFor();
  Flag = m_PrintEndFlag;
  m_PrintEndFlagMutex->Release();
  return Flag;
}

void CSlicePreProcessor::SetPrintEndFlag(bool Flag)
{
  m_PrintEndFlagMutex->WaitFor();
  m_PrintEndFlag = Flag;
  m_PrintEndFlagMutex->Release();
}

void CSlicePreProcessor::ReleaseBuffers(PBYTE TmpBuffers[], int LastIndex)
{
  for(int i = FIRST_CHAMBER_TYPE;i < LastIndex; i++)
      m_SlicesAllocator.ReleaseBlock(TmpBuffers[i]);
}


bool CSlicePreProcessor::AllocateTmpBuffers(PBYTE TmpBuffers[],ULONG SliceUncompressedSize)
{
  bool ret = true;
  for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
  {
     // Prepare buffers
     TmpBuffers[i] = (PBYTE)m_SlicesAllocator.GetBlock(SliceUncompressedSize,true);
     if(TmpBuffers[i] == NULL)
     {
        CQLog::Write(LOG_TAG_HOST_COMM,"SlicePreProcessor: cannot find free block (%d)",i);
        FrontEndInterface->ErrorMessage("Memory allocation failed.\nClose and restart the printer application.");
        ReleaseBuffers(TmpBuffers, i);
        ret = false;
        break;
     }
  }
  return ret;
}//AllocateTmpBuffers

 /*******************************************************************************************************************/
#pragma warn -8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
#ifdef OBJET_MACHINE_KESHET
bool CSlicePreProcessor::UnpackNonEmptySlice(TSlicePackageType* SlicePackage,
                                             PBYTE TmpBuffers[],
                                             ULONG SliceUncompressedSize)
{
    bool ret = true;

    //In every iteration (on i, in byte jumps), we can check two 4 bits (=1 byte) at once in bmpPtr and extract from it 2 bits
    //for our 1-bit bitmap (each chamber receives 1-bit bitmap)
    for (unsigned int k=0, j=8 ,l=0; l < SlicePackage->model->len;)
    {
        if (j==0) //means we finished placing 8 bits in bmp[k]
        {
            j=8;
            k++; //next 8 bits in bmp[k]
        }

        //index1/index2 - Check if the 4 left/right most bits match a material and put it in the relevant place (according to j) in bmp[k]
		BYTE index1 = (SlicePackage->model->bmp[l])&0x0F;
        BYTE index2 = ((SlicePackage->model->bmp[l])&0xF0)>>4;
        if (index1 > MAX_LEGAL_BYTE)
        {
            CQLog::Write(LOG_TAG_HOST_COMM,"Error: Objet Studio sent illegal input 0x%02X in the first 4 bits of byte %d slice %d, the value needs to be between 0 to %d", SlicePackage->model->bmp[l], l, SlicePackage->header.sliceNumber, MAX_LEGAL_BYTE);
            FrontEndInterface->NotificationMessage(QFormatStr("Cannot unpack slice %d (%d:0x%02X).\nClose and restart the printer application.", SlicePackage->header.sliceNumber, l, SlicePackage->model->bmp[l]));
			index1 = (index1)%8;
        }

        if (index2 > MAX_LEGAL_BYTE)
        {
            CQLog::Write(LOG_TAG_HOST_COMM,"Error: Objet Studio sent illegal input 0x%02X in the second 4 bits of byte %d slice %d, the value needs to be between 0 to %d", SlicePackage->model->bmp[l], l, SlicePackage->header.sliceNumber, MAX_LEGAL_BYTE);
            FrontEndInterface->NotificationMessage(QFormatStr("Cannot unpack slice %d (%d:0x%02X).\nClose and restart the printer application.", SlicePackage->header.sliceNumber, l, SlicePackage->model->bmp[l]));
			index2 = (index2)%8;
		}
		if (index2 != 0)
        {
            TmpBuffers[index2-1][k] = (0x01<<(j-1))|TmpBuffers[index2-1][k];
		}
        if (index1 != 0)
        {
			TmpBuffers[index1-1][k] = (0x01<<(j-2))|TmpBuffers[index1-1][k];
        }

        j -= 2;
        l++;
    }
    return ret;
}
/*******************************************************************************************************************/
#pragma warn .8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
#else
bool CSlicePreProcessor::UnpackNonEmptySlice(TSlicePackageType* SlicePackage,
                                             PBYTE TmpBuffers[],
                                             ULONG SliceUncompressedSize)
{
        bool ret = true;
		TBmp* BMPArray[NUMBER_OF_CHAMBERS];

		BMPArray[TYPE_CHAMBER_MODEL1]  = SlicePackage->model;

		unsigned Ptr = (unsigned)(SlicePackage->model->bmp + SlicePackage->model->len);
		BMPArray[TYPE_CHAMBER_SUPPORT] = reinterpret_cast<TBmp *>(Ptr);
		Ptr = (unsigned)( ((TBmp*)Ptr)->bmp + ((TBmp*)Ptr)->len );
		short* pNumberOfAdditionalModelResins   = reinterpret_cast<short *>(Ptr);
		Ptr += sizeof(short);
		short* pNumberOfAdditionalSupportResins = reinterpret_cast<short *>(Ptr);
		Ptr += sizeof(short);
        //check slice message validity
		if(m_NumOfAddModelResins!=*pNumberOfAdditionalModelResins)
		{
		   CQLog::Write(LOG_TAG_HOST_COMM, "Difference in NumberOfAdditionalResins! NoOfBmp=%d, StartMsg=%d, SliceMsg.=%d",SlicePackage->header.NofBitmaps,
										   m_NumOfAddModelResins, *pNumberOfAdditionalModelResins);
		   CBackEndInterface* BackEnd = CBackEndInterface::Instance();
		   if(BackEnd!=NULL)
		   	FrontEndInterface->ErrorMessage("Difference in NumberOfAdditionalResins!\nClose and restart the printer application.");
		   return false;
		}

		if (SlicePackage->header.NofBitmaps != *pNumberOfAdditionalModelResins   +
											   *pNumberOfAdditionalSupportResins +
											   MINIMUM_REQUIRED_RESIN_NUM )
		{
		   CQLog::Write(LOG_TAG_HOST_COMM, "Invalid no. of additional resins. NoOfBmp=%d, Model=%d, Supp.=%d",SlicePackage->header.NofBitmaps,
										   *pNumberOfAdditionalModelResins,*pNumberOfAdditionalSupportResins);
		   FrontEndInterface->ErrorMessage("Invalid number of additional model resins.\nClose and restart the printer application.");
		   return false;
		}

		CQLog::Write(LOG_TAG_OBJET_MACHINE, "NumOfBmps: %d, NumOfAddModelResins: %d",SlicePackage->header.NofBitmaps, *pNumberOfAdditionalModelResins);

		if(*pNumberOfAdditionalModelResins>0 || *pNumberOfAdditionalSupportResins>0)
		{
		  for(int k = FIRST_SUPPORT_CHAMBER_INDEX+1;k<LAST_SUPPORT_CHAMBER_INDEX;k++)
		  {
			if(m_BmpsReceivedFromHostArr[k]!=-1)
		    {
			  BMPArray[k] = reinterpret_cast<TBmp *> (Ptr);
			  Ptr = (unsigned)( ((TBmp*)Ptr)->bmp + ((TBmp*)Ptr)->len );
			}
			/*else
			{
			  memset(TmpBuffers[k],0,BMPArray[TYPE_CHAMBER_MODEL1]->len);
			  memcpy(BMPArray[k]->bmp, TmpBuffers[k],SlicePackage->model->len);
			  TmpBuffers[k] = SlicePackage->model->bmp;
			  memset(TmpBuffers[k],0,SlicePackage->model->len);
			  BMPArray[k]->rect = SlicePackage->model->rect;
			  BMPArray[k]->len = SlicePackage->model->len;
			}*/
		  }

		  for(int k = FIRST_MODEL_CHAMBER_INDEX+1;k<LAST_MODEL_CHAMBER_INDEX;k++)
		  {
			if(m_BmpsReceivedFromHostArr[k]!=-1)
		    {
			  BMPArray[k] = reinterpret_cast<TBmp *> (Ptr);
			  Ptr = (unsigned)(((TBmp*)Ptr)->bmp + ((TBmp*)Ptr)->len );
		    }
		  }
        }


		/*if(SlicePackage->header.sliceNumber>50)
		{
		  unsigned int k=0;
		  for(k=0; k<BMPArray[TYPE_CHAMBER_MODEL1]->len;k++)
		  {
		   if((BMPArray[TYPE_CHAMBER_MODEL1]->bmp[k])&0x1)
			 k=BMPArray[TYPE_CHAMBER_MODEL1]->len+2;
		  }
		  if(k==BMPArray[TYPE_CHAMBER_MODEL1]->len)
			CQLog::Write(LOG_TAG_HOST_COMM, "Itamar: MODEL_1 bmp is empty SliceNum:%d",SlicePackage->header.sliceNumber);

		  for(k=0; k<BMPArray[TYPE_CHAMBER_MODEL2]->len;k++)
		  {
		   if((BMPArray[TYPE_CHAMBER_MODEL2]->bmp[k])&0x1)
			 k=BMPArray[TYPE_CHAMBER_MODEL2]->len+2;
		  }
		  if(k==BMPArray[TYPE_CHAMBER_MODEL2]->len)
			CQLog::Write(LOG_TAG_HOST_COMM, "Itamar: MODEL_2 bmp is empty SliceNum:%d",SlicePackage->header.sliceNumber);

		  for(k=0; k<BMPArray[TYPE_CHAMBER_MODEL3]->len;k++)
		  {
		   if((BMPArray[TYPE_CHAMBER_MODEL3]->bmp[k])&0x1)
			 k=BMPArray[TYPE_CHAMBER_MODEL3]->len+2;
		  }
		  if(k==BMPArray[TYPE_CHAMBER_MODEL3]->len)
		    CQLog::Write(LOG_TAG_HOST_COMM, "Itamar: MODEL_3 bmp is empty SliceNum:%d",SlicePackage->header.sliceNumber);
		}*/
		//test only, check if there is a bug in Studio
		/*for(unsigned int k=0; k<SlicePackage->model->len;k++)
		{
		  //check if there is an illegal bmp
		  if((BMPArray[TYPE_CHAMBER_MODEL2]->bmp[k])&(BMPArray[TYPE_CHAMBER_MODEL3]->bmp[k])&0x1)
		  {
			if(k%2)//arbitrary
				BMPArray[TYPE_CHAMBER_MODEL2]->bmp[k]=0;
			else
				BMPArray[TYPE_CHAMBER_MODEL3]->bmp[k]=0;
			//CQLog::Write(LOG_TAG_HOST_COMM, "M2-M3 Itamar: SliceNum:%d, k:%d",SlicePackage->header.sliceNumber,k);
		  }
		  else if((BMPArray[TYPE_CHAMBER_MODEL1]->bmp[k])&(BMPArray[TYPE_CHAMBER_MODEL3]->bmp[k])&0x1)
		  {
			if(k%2)//arbitrary
				BMPArray[TYPE_CHAMBER_MODEL1]->bmp[k]=0;
			else
				BMPArray[TYPE_CHAMBER_MODEL3]->bmp[k]=0;
			//CQLog::Write(LOG_TAG_HOST_COMM, "M1-M3 Itamar: SliceNum:%d, k:%d",SlicePackage->header.sliceNumber,k);
		  }
		  else if((BMPArray[TYPE_CHAMBER_SUPPORT]->bmp[k])&(BMPArray[TYPE_CHAMBER_MODEL3]->bmp[k])&0x1)
		  {
				BMPArray[TYPE_CHAMBER_MODEL3]->bmp[k]=0;
			//CQLog::Write(LOG_TAG_HOST_COMM, "Support-M3 Itamar: SliceNum:%d, k:%d",SlicePackage->header.sliceNumber,k);
		  }
		  if((BMPArray[TYPE_CHAMBER_MODEL1]->bmp[k])&(BMPArray[TYPE_CHAMBER_MODEL2]->bmp[k])&0x1 ||
			  (BMPArray[TYPE_CHAMBER_MODEL2]->bmp[k])&(BMPArray[TYPE_CHAMBER_MODEL3]->bmp[k])&0x1 ||
			 (BMPArray[TYPE_CHAMBER_MODEL1]->bmp[k])&(BMPArray[TYPE_CHAMBER_SUPPORT]->bmp[k])&0x1 ||
			  (BMPArray[TYPE_CHAMBER_MODEL2]->bmp[k])&(BMPArray[TYPE_CHAMBER_SUPPORT]->bmp[k])&0x1 ||
			  (BMPArray[TYPE_CHAMBER_MODEL3]->bmp[k])&(BMPArray[TYPE_CHAMBER_SUPPORT]->bmp[k])&0x1 ||
			  (BMPArray[TYPE_CHAMBER_MODEL1]->bmp[k])&(BMPArray[TYPE_CHAMBER_MODEL3]->bmp[k])&0x1)
			  CQLog::Write(LOG_TAG_HOST_COMM, "Itamar: SliceNum:%d, k:%d",SlicePackage->header.sliceNumber,k);
		}*/
		//end of test
   #ifdef OBJET_1000
		for(int i = FIRST_CHAMBER_TYPE; i < SlicePackage->header.NofBitmaps; i++)
   #else
   		for(int i = FIRST_CHAMBER_TYPE; i < NUMBER_OF_CHAMBERS; i++)
   #endif
		{
		   if(m_BmpsReceivedFromHostArr[i]==-1)  //need to crate an empty bmp
		   {
			   memset(TmpBuffers[i],0,BMPArray[TYPE_CHAMBER_MODEL1]->len);
			   if(!UnpackSlice(SlicePackage->header.compressType, TmpBuffers[i], BMPArray[TYPE_CHAMBER_MODEL1]->len,
						   TmpBuffers[i],SliceUncompressedSize))
				{
				 ret = false;
				 CQLog::Write(LOG_TAG_HOST_COMM,"Cannot unpack slice");
				 FrontEndInterface->ErrorMessage("Cannot unpack slice.\nClose and restart the printer application.");
				 ReleaseBuffers(TmpBuffers, i);
				 break;
				}
           }
           // Unpack the image
		   else
		   {
			if(!UnpackSlice(SlicePackage->header.compressType, BMPArray[i]->bmp, BMPArray[i]->len,
						   TmpBuffers[i],SliceUncompressedSize))
			{
			  ret = false;
			  CQLog::Write(LOG_TAG_HOST_COMM,"Cannot unpack slice");
			  FrontEndInterface->ErrorMessage("Cannot unpack slice.\nClose and restart the printer application.");
			  ReleaseBuffers(TmpBuffers, i);
			  break;
			}
		   }
		}

        return ret;
}
#endif

bool CSlicePreProcessor::UnpackEmptySlice(TSlicePackageType* SlicePackage, 
                                            PBYTE TmpBuffers[])
{
        CQLog::Write(LOG_TAG_HOST_COMM,"Empty slice detected");
        // Prepare dummy support and model buffers
        for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
        {
           TmpBuffers[i] = (PBYTE)m_SlicesAllocator.GetBlock(EMPTY_SLICE_DEMO_SIZE);
           memset(TmpBuffers[i],0,EMPTY_SLICE_DEMO_SIZE);
        }
        SlicePackage->model->rect.width  = EMPTY_SLICE_DEMO_WIDTH;
        SlicePackage->model->rect.height = EMPTY_SLICE_DEMO_HEIGHT;
        SlicePackage->model->rect.y =  m_LastPositionY;  //2305;
        return true;
}

// Thread execute function (override)
void CSlicePreProcessor::ProcessSlicePackage(TSlicePackageType *SlicePackage)
{
  CAppParams* ParamMgr = CAppParams::Instance();
#ifdef HOST_TEST
	//generate bitmap
	int Height=0, Width=0;
	//read bitmap
	const char IMAGE_FILE_NAME[]   = "Configs\\HostBitmap.bmp";
	CBMPFile BMPFile(IMAGE_FILE_NAME);

	// Aloocate slice buffer
	Width = BMPFile.GetWidth();//BMPFile.GetLineSizeInBytes()*8;
	Height = BMPFile.GetHeight();
	// Read file content
	BMPFile.ReadImage(SlicePackage->model->bmp,0);
	// Close file
	BMPFile.Close();

    //data
	SlicePackage->model->rect.width = Width;
	SlicePackage->model->rect.height = Height;
	SlicePackage->model->len = ((Width * Height)/8) * BITS_PER_PIXEL;//bitmap (4BPP) size in bytes (<= /8)
	SlicePackage->header.NofBitmaps = 8;
	static int sliceNumber = 0;
	SlicePackage->header.sliceNumber = sliceNumber;
	sliceNumber++;
	SlicePackage->header.bitmapRes = 600;
	SlicePackage->header.sliceHeight = 0.03f;
	SlicePackage->header.compressType = NO_COMPRESSION;
	SlicePackage->model[0].rect.x = 0;
	SlicePackage->model[0].rect.y = 0;
#endif

    CQLog::Write(LOG_TAG_HOST_COMM,"New slice (N=%d, X=%d, Y=%d, W=%d, H=%d, Res=%d)",SlicePackage->header.sliceNumber,
                 SlicePackage->model[0].rect.x,SlicePackage->model[0].rect.y,
                 SlicePackage->model[0].rect.width,SlicePackage->model[0].rect.height,
				 ParamMgr->SliceResolution_InXAxis.Value());

     //updated the last Y position (for handlig empty slices without returning to 0 for Y)
    if (SlicePackage->model[0].rect.y != 0 )    //only for "good positions"
    {
    	m_LastPositionY =  SlicePackage->model[0].rect.y ;
    }

    // Check if the slice contain exactly correct number of images
    if(SlicePackage->header.NofBitmaps > NUMBER_OF_CHAMBERS)
    {
      QString ErrMessage = "Invalid number of bitmaps in slice. Must be in ["+
                            QIntToStr(MINIMUM_REQUIRED_RESIN_NUM) +"," +
                            QIntToStr(NUMBER_OF_CHAMBERS) + "] range";
      CQLog::Write(LOG_TAG_HOST_COMM, ErrMessage);
      FrontEndInterface->ErrorMessage(ErrMessage);
      return;
    }
    //Find slice uncompressed size
    ULONG SliceUncompressedSize = (SlicePackage->model->rect.width * SlicePackage->model->rect.height)  / 8;

    PBYTE TmpBuffers[NUMBER_OF_CHAMBERS];
    memset(TmpBuffers, 0, sizeof(PBYTE)*NUMBER_OF_CHAMBERS);
    try
	{
    if(SliceUncompressedSize != 0L)
    {
        if (!AllocateTmpBuffers(TmpBuffers, SliceUncompressedSize))
        {
		   CQLog::Write(LOG_TAG_HOST_COMM, "SlicePreProcessor: Failed to allocate buffers"); //5993
           return;
        }
        CQLog::Write(LOG_TAG_HOST_COMM, "SlicePreProcessor: Unpacking Slices"); //5993
        if(!UnpackNonEmptySlice(SlicePackage, TmpBuffers, SliceUncompressedSize))
        {
           CQLog::Write(LOG_TAG_HOST_COMM, "SlicePreProcessor: Failed to unpack slices"); //5993
           return;
        }
    }
    // If it is an empty slice, create a dummy small, empty slice
    else
        UnpackEmptySlice(SlicePackage, TmpBuffers);



      CQLog::Write(LOG_TAG_HOST_COMM, "SlicePreProcessor: Creating new slice"); //5993
	  // Create a slice object
	  CSlice* Slice = new CSlice(TmpBuffers,SlicePackage->header.sliceNumber,SlicePackage->model->rect.width,
							   SlicePackage->model->rect.height,SlicePackage->header.sliceHeight,SlicePackage->model->rect.x,
							   SlicePackage->model->rect.y,ParamMgr->SliceResolution_InXAxis);

      CQLog::Write(LOG_TAG_HOST_COMM, "SlicePreProcessor: New slice created");
    // Remember last incoming slice (use for slice preview)
	  int LastSliceNum = 0;
	  if(Slice!=NULL)
	  {
    m_LastSliceAccessMutex->WaitFor();
    m_LastIncomingSlice = Slice;
	  	LastSliceNum = Slice->GetSliceNumber();
    m_LastSliceAccessMutex->Release();
	  }
	  else
	  	CQLog::Write(LOG_TAG_HOST_COMM, "SlicePreProcessor: Slice creation failed");

    // Update the slice preview in the UI
    if(m_SlicePreviewEvent != NULL)
      (*m_SlicePreviewEvent)(m_SlicePreviewEventCockie);

    CQLog::Write(LOG_TAG_HOST_COMM,"Adding new slice to slices queue...");

    if(m_SliceMessageQueue->Send(Slice) == QLib::wrReleased)
    {
      DisposeSlice(Slice);

      CQLog::Write(LOG_TAG_HOST_COMM,"Slice queue has been released");

      // If the thread is terminating, leave thread loop
      if(Terminated)
        return;
    }
    else
    {
        if(!IsPrintEnd())
        {
          // If the print has not ended, request next one
          if(LastSliceNum < m_TotalSlicesNum - 1)
          {
            if (!m_HostComm->IsPaused())
              m_HostComm->RequestSlice();
            else
              CQLog::Write(LOG_TAG_HOST_COMM,"IsPaused = true");
          }
          else
            if(LastSliceNum == m_TotalSlicesNum - 1)
            {              
              m_HostComm->TriggerHostCommTimeoutTimer(ParamMgr->HostEndModelTimeoutInSec);

#ifdef EXTENDED_HOST_LOG
              CQLog::Write(LOG_TAG_HOST_COMM,"Waiting for end-of-model...");
#endif
            }
         }
         else
         {
             CQLog::Write(LOG_TAG_HOST_COMM,"PreProcessor: print has ended");
         }
		}
	}
	catch (std::bad_alloc& ba)
	{
	  CQLog::Write(LOG_TAG_HOST_COMM,"CSlice - Memory allocation failed: %s",ba.what());
	}
	catch (...)
    {
	   CQLog::Write(LOG_TAG_HOST_COMM,"SlicePreProcessor: Error in ProcessSlicePackage: %d",SlicePackage->header.NofBitmaps);
    }

}

void CSlicePreProcessor::Execute(void)
{
  BYTE*       tmp;

  try {
  do
  {
    if(m_HostComm->m_SliceDataMessageQueue->Receive(tmp) == QLib::wrReleased)
      break;

    // If we got NULL, we should send an end-print marker to the slice queue and continue waiting
    if(tmp == NULL)
    {
	  if(m_SliceMessageQueue->Send((CSlice *)LAST_SLICE_MARKER) == QLib::wrReleased)
        if(Terminated) // If the thread is terminating, leave thread loop
          break;
      continue;
    }
    ProcessSlicePackage(reinterpret_cast<TSlicePackageType *>(tmp));
    //Q_SAFE_DELETE_ARRAY(tmp);  //bug 5811
  } while(!Terminated);
  } catch(...) {
        CQLog::Write(LOG_TAG_GENERAL, "CSlicePreProcessor::Execute - unexpected error");
        if (!Terminated)
          throw EQException("CSlicePreProcessor::Execute - unexpected error");
  }

  // Flush all remaining slices in the slice queue
  FlushSlices();
}

// Dump a slice
void CSlicePreProcessor::DisposeSlice(CSlice *Slice)
{
  m_LastSliceAccessMutex->WaitFor();

  if(Slice == m_LastIncomingSlice)
    m_LastIncomingSlice = NULL;

  m_LastSliceAccessMutex->Release();
  for(int i = FIRST_CHAMBER_TYPE;i < LAST_CHAMBER_TYPE; i++)
        m_SlicesAllocator .ReleaseBlock(Slice->GetBuffer((TChamberIndex)i));

  delete Slice;
}
void CSlicePreProcessor::ResetLastPositionY(void)
{
     m_LastPositionY=0;
}
CSlice *CSlicePreProcessor::GetLastSliceAndLock(void)
{
  m_LastSliceAccessMutex->WaitFor();
  return m_LastIncomingSlice;
}

void CSlicePreProcessor::ReleaseLastSliceLock(void)
{
  m_LastSliceAccessMutex->Release();
}

// *******************************************************************
// Class CHostComm implementation
// *******************************************************************

CHostComm::CHostComm() : CQThread(true,"HostCommThread")
{
  //m_IncomingMsgData        = NULL; //bug 6171
  m_Comm                   = new CQTcpIpServer(HOST_IP_PORT);
  m_ConfigBackup           = new CConfigBackup;
  m_MessageIsBeingReceived = false;  
  m_SliceDataMessageQueue  = new TSliceDataMessageQueue(SLICE_QUEUE_SIZE);
  m_SlicePreProcessor      = new CSlicePreProcessor(this);
  m_CurrJobId              = -1;
  m_MachineManager         = Q2RTApplication->GetMachineManager();
  m_AllocatedLength        = 0;
  m_Data                   = NULL;
  m_ModelIsBeingReceived   = false;
  m_IsMute                 = false;

  Priority = Q_PRIORITY_LOW; 

  TInitStatus_E       StatusInit         = INIT_FAIL_E; //init
  m_ParamsMngr = new CTempAppParams("Host", StatusInit, true); //bug 5719
  m_IncomingMsgDataIndex = 0;// bug 6171

  InitFlexibleParams();
  /**************************************************************************************************************************/
  /**/ //Attention!!!! The following code must be during run time (because we compile on 32bit and sometimes runs on 64 bit)/**/
  /**/ m_Max_Incoming_Data_Size = MAX_INCOMING_DATA_SIZE;                                                                  /**/
  /**/ m_Max_Incoming_Resinn_Data_Size = MAX_INCOMING_RESINN_DATA_SIZE;                                                    /**/																 /**/
  /**/if((IsRunningOnWindows64Bit()) && ((Q2RTApplication->GetMachineType()) == (mtObjet1000 )) )                          /**/
  /**/{                                                                                                                    /**/
  /**/	 CQLog::Write(LOG_TAG_GENERAL,"Info: CHostComm::Constructor runs on Objet1000 64bit");					 		   /**/																													   /**/
  /**/   m_Max_Incoming_Data_Size*= THE_PRODUCT_SIZE_OF_TRAY_OBJET1000_IN_TRIPLEX_TRAY;                                    /**/
  /**/   m_Max_Incoming_Resinn_Data_Size*= THE_PRODUCT_SIZE_OF_TRAY_OBJET1000_IN_TRIPLEX_TRAY;                             /**/
  /**/}                                                                                                                    /**/
  /***************************************************************************************************************************/

  //bug 5811
  try
  {
     for(int i=0;i<SLICE_QUEUE_SIZE;i++)
		m_IncomingMsgData[i] = new unsigned char[m_Max_Incoming_Data_Size];
  }
  catch (std::bad_alloc& ba)
  {
     for(int i=0;i<SLICE_QUEUE_SIZE;i++)
        m_IncomingMsgData[i] = NULL;
     CQLog::Write(LOG_TAG_HOST_COMM,"m_IncomingMsgData - Memory allocation failed: %s",ba.what());
  }
  try
  {
     m_IncomingMsgResinData = new unsigned char [m_Max_Incoming_Resinn_Data_Size];
  }
  catch (std::bad_alloc& ba)
  {
     m_IncomingMsgResinData = NULL;
     CQLog::Write(LOG_TAG_HOST_COMM,"m_IncomingMsgResinData - Memory allocation failed: %s",ba.what());
  }
  
  m_HostPrintJob = new CHostPrintJob();
  
  #define DEFINE_COMMAND(Name, Cmd) HostOpcode2Str[Cmd]= #Name;
  #include "HostCommands.def"
  #undef DEFINE_COMMAND
  
}

// Destructor
 CHostComm::~CHostComm(void)
{
  Terminate();

  m_Comm->Disconnect();

  m_SliceDataMessageQueue->Release();
  delete m_SlicePreProcessor;

  WaitFor();

  delete m_Comm;
  delete m_ConfigBackup;

  Q_SAFE_DELETE(m_ParamsMngr);

  delete m_SliceDataMessageQueue;

  //bug 6171
  for(int i=0;i<SLICE_QUEUE_SIZE;i++)
  {
        if(m_IncomingMsgData[i])
        {
			Q_SAFE_DELETE_ARRAY(m_IncomingMsgData[i]);
        }
  }

  Q_SAFE_DELETE_ARRAY(m_IncomingMsgResinData); 

  if (m_Data)
     free(m_Data);
	 
  Q_SAFE_DELETE(m_HostPrintJob);
}

//  Thread execute method (override).
//  ---------------------------------
void CHostComm::Execute(void)
{
  TCommErrorCodes CommErr;
#ifdef HOST_TEST
	m_IncomingMsgCommand = CMD_NEW_MODEL;
	ProcessCommand();
	m_IncomingMsgCommand = CMD_SLICE;
	ProcessCommand();
	m_IncomingMsgCommand = CMD_SLICE;
	ProcessCommand();
	m_IncomingMsgCommand = CMD_SLICE;
	ProcessCommand();
#endif

  while(!Terminated)
  {
    try
    {
      if(!m_Comm->IsConnected())
      {
		CQLog::Write(LOG_TAG_HOST_COMM,"Waiting for host connect...");

        if(!m_Comm->Connect())
		{
          if(Terminated)
            return;

          CQLog::Write(LOG_TAG_HOST_COMM,"TCP/IP connect error");
          continue;
        }

        if(!Terminated)
          CQLog::Write(LOG_TAG_HOST_COMM,"Host connection established");
      }

      if(Terminated)
        break;

      CommErr = Decode();

      if(CommErr != ceNoError)
        if(CommErr != ceCommError)
          SendNak();

    }
    catch(EQTcpIpServer& E) //bug 6029
    {
	    CQLog::Write(LOG_TAG_HOST_COMM,"Closing connection ret: %d",E.GetErrorCode());
        QLib::TQWaitResult Result;                
        // Releasing the mutex if needed.
        Result = m_MessageReceiveMutex.WaitFor(0);
		if(Result != QLib::wrSignaled)
          CQLog::Write(LOG_TAG_HOST_COMM,"'m_MessageReceiveMutex' wasn't released! Releasing it...");
        m_MessageReceiveMutex.Release();
    }
    catch(EQException& E)
    {
		QLib::TQWaitResult Result;
        CQLog::Write(LOG_TAG_HOST_COMM,"TCP/IP error - %s",E.GetErrorMsg().c_str());
        // Releasing the mutex if needed.
        Result = m_MessageReceiveMutex.WaitFor(0);
        if(Result != QLib::wrSignaled)
          CQLog::Write(LOG_TAG_HOST_COMM,"'m_MessageReceiveMutex' wasn't released! Releasing it...");
        m_MessageReceiveMutex.Release();
    }
    catch(...)
    {
        CQLog::Write(LOG_TAG_HOST_COMM,"Unexpected error during CHostComm::Execute()");
        if (m_MessageIsBeingReceived)
        {
            m_MessageReceiveMutex.Release();
        }
        if (!Terminated)
          throw EQException("CHostComm::Execute - unexpected error");
    }
  }
}

// Send an Ack reply
// Note: This function is not protected by the mutex and should be used only in the context of
//       already protectedblock.
void CHostComm::SendAck(void)
{
  BYTE AckBuffer = ACK;
  CQMutexHolder MutexHolder(&m_HostCommandsMutex);
  m_Comm->Write(&AckBuffer,1);
#ifdef EXTENDED_HOST_LOG
  CQLog::Write(LOG_TAG_HOST_COMM,"Sent ACK to host");
#endif
  m_MessageReceiveMutex.Release();
}

//  Send an Nak reply
void CHostComm::SendNak(void)
{
  BYTE NakBuffer = NAK;
  CQMutexHolder MutexHolder(&m_HostCommandsMutex);
  m_Comm->Write(&NakBuffer,1);
#ifdef EXTENDED_HOST_LOG
  CQLog::Write(LOG_TAG_HOST_COMM,"Sent NAK to host");
#endif
 m_MessageReceiveMutex.Release();
}

//  Communication protocol Message decoder
TCommErrorCodes CHostComm::Decode(void)
{
  enum Decode_Steps {COMM_IDLE, MSG_OPCODE, MSG_LENGTH, MSG_DATA, MSG_CHECKSUM, MSG_COMMAND, MSG_RESIN_DATA};

  static unsigned int iCntr;                 // Byte counter for Length and Data fields.
  unsigned char       RcvByte    = 0;
  TCommErrorCodes     Decode_Err = ceNoError;
  static Decode_Steps Step       = COMM_IDLE;// Current step in Decode of message.

  if ((COMM_IDLE == Step) && (m_IsMute))
  {  // can mute in between commands
      return ceCommError;
  }

  if(m_Comm->Read(&RcvByte,1) == tsOK)
  {
    switch (Step)
    {
      case COMM_IDLE:
           //Ignore all input until a SOH character is received.
           //---------------------------------------------------
           if (RcvByte == SOH)    // Start of Header
           {
               Step = MSG_OPCODE;
               // Wait for the mutex in order to prevent collision
               m_MessageReceiveMutex.WaitFor();
               // Mark start of message handling block
               m_MessageIsBeingReceived = true;
           }
           break;

      case MSG_OPCODE:
           //Check validity of the opcode
           if((RcvByte >= FIRST_CMD) && (RcvByte <= LAST_CMD))
           {                           //Reset message structure.
              m_IncomingMsgCommand = RcvByte;
              m_IncomingMsgLength  = 0;
              iCntr                = 0;
              Step                 = MSG_LENGTH;
           }
           else
           {
              //Illegal command was received !!
              Decode_Err = ceIllegalCommand;
              //Reset the state machine
              //-----------------------
              Step = COMM_IDLE;
           }
           break;

      case MSG_LENGTH:
           //Length of message DATA. The message Length is composed of 4 bytes,
           //which must be 'glued' Together to get the actual value.
           //Using a long value so that the data does NOT get shifted out of the byte.
           //-------------------------------------------------------------------------
           m_IncomingMsgLength += ((static_cast <unsigned long> (RcvByte)) << (8 * iCntr));
           iCntr++;
           if(iCntr >= sizeof(unsigned long))  // Entire field has been read.
           {
              if(m_IncomingMsgLength>0)      // Message has data.
              {
                 //Reset data index.
                 //-----------------
                 iCntr = 0;
				 if (m_IncomingMsgLength<m_Max_Incoming_Data_Size)
		 {
                        if(m_IncomingMsgCommand==CMD_RESIN_DATA)
                                Step = MSG_RESIN_DATA;
                        else
                                Step = MSG_DATA; //Data Buffer was allocated.
		 }
                 else
                 {
                    CQLog::Write(LOG_TAG_HOST_COMM,"Memory allocation failed: msg size %d",m_IncomingMsgLength);
                    //Allocation failed.
                    Decode_Err = cePayloadAlloc;
                    //Reset the state machine
                    //-----------------------
                    Step = COMM_IDLE;
                 }
              }
              else
              {
                 //No data. So skip data stage.
                 Step = MSG_CHECKSUM;
              }
           }
           break;

      case MSG_DATA:
//      Read data into buffer, until end of buffer.
//      --------------------------------------------
        m_IncomingMsgData[m_IncomingMsgDataIndex][iCntr++] = RcvByte;

        if(m_Comm->Read(m_IncomingMsgData[m_IncomingMsgDataIndex] + iCntr,m_IncomingMsgLength - iCntr) != tsOK)
        {
          CQLog::Write(LOG_TAG_HOST_COMM,"Reading host Error: msg length: %d",m_IncomingMsgLength);
          Step = COMM_IDLE;
          return ceCommError;
        }

//      if (CMD_SLICE == m_IncomingMsgCommand)
//      { /*******************  iddan.kalo debug ******************/
//           TSlicePackageType * pDummySlice = (TSlicePackageType *)(m_IncomingMsgData[m_IncomingMsgDataIndex]);
//           CQLog::Write(LOG_TAG_GENERAL, "------DEBUG------ slice %d", pDummySlice->header.sliceNumber);
//           for (UINT i=0; i < pDummySlice->model[0].len; ++i)
//           {
//               if (((pDummySlice->model[0].bmp[i] & 0xf0) > 0x80) ||
//                   ((pDummySlice->model[0].bmp[i] & 0x0f) > 0x08)    )
//               {
//                   CQLog::Write(LOG_TAG_GENERAL, "------DEBUG: bad byte %d:0x%02x", i, pDummySlice->model[0].bmp[i]);
//               }
//           }
//      } /******************** iddan.kalo debug ******************/
        iCntr += m_IncomingMsgLength - iCntr;

        if(iCntr >= m_IncomingMsgLength)
        {
//        All of data has been received.
//        ------------------------------
          Step = MSG_CHECKSUM;
        }
        break;
      case MSG_RESIN_DATA:
        //      Read data into buffer, until end of buffer.
//      --------------------------------------------
        m_IncomingMsgResinData[iCntr++] = RcvByte;

        if(m_Comm->Read(m_IncomingMsgResinData + iCntr,m_IncomingMsgLength - iCntr) != tsOK)
        {
          CQLog::Write(LOG_TAG_HOST_COMM,"Reading host Error: msg size %d",m_IncomingMsgLength);
          Step = COMM_IDLE;
          return ceCommError;
        }

        iCntr += m_IncomingMsgLength - iCntr;

        if(iCntr >= m_IncomingMsgLength)
        {
//        All of data has been received.
//        ------------------------------
          Step = MSG_CHECKSUM;
        }
        break;

      case MSG_CHECKSUM:
          Step = MSG_COMMAND;

      case MSG_COMMAND:
//      We got the message Ok
//      ---------------------
        m_MessageIsBeingReceived = false;

//      Restart decoding state-machine.
//      -------------------------------
        Step = COMM_IDLE;

//      Process Message.
//      ----------------
        Decode_Err = ProcessCommand();

        if(Decode_Err == ceNoError)
        {
           SendAck();
           SendSavedCmds();
        }

        break;
    }
  }
  else
  {
      if (!Terminated)
      {
          m_MachineManager->StopPrint(Q2RT_HOST_TCP_ERROR); //bug 5811
      }

      Decode_Err = ceCommError;

      // If there were an error and we are in the middle of a message receive, release the mutex
      if(m_MessageIsBeingReceived)
      {
        m_MessageIsBeingReceived = false;
        m_MessageReceiveMutex.Release();
      }
  }

  return Decode_Err;
}
TCommErrorCodes CHostComm::CheckObjetStudioParams(const BYTE& byPrintMode, const short& sNumberOfAdditionalModelResins)
{
   if(sNumberOfAdditionalModelResins==DM7_NUMBER_OF_ADDITIONAL_MODEL_RESINS)
   {
        if(byPrintMode==HIGH_SPEED)
        {
             return ceNoError;
        }
        else
        {
             CQLog::Write(LOG_TAG_HOST_COMM,"wrong value: %d from Objet studio. DM7 supports only High speed mode",byPrintMode);
             return ceIllegalCommandField;
        }

   }
   else
   {
        if(sNumberOfAdditionalModelResins==DM3_NUMBER_OF_ADDITIONAL_MODEL_RESINS)
        {
              if((byPrintMode==HIGH_SPEED)||(byPrintMode==HIGH_QUALITY))
              {
                return ceNoError;
              }
              else
              {
                 CQLog::Write(LOG_TAG_HOST_COMM,"Print mode wrong value: %d from Objet studio. DM3 supports only High speed mode or High Quality mode",byPrintMode);
             	return ceIllegalCommandField;
              }
        }
        else
        {
              CQLog::Write(LOG_TAG_HOST_COMM,"wrong value: %d for Number Of Additional Model Resins, expected value is 2 or 6",sNumberOfAdditionalModelResins);
              return ceIllegalCommandField;
        }
    }
}
TCommErrorCodes CHostComm::ValidateStartModel(void)
{
//OBJET_MACHINE config
#ifndef OBJET_MACHINE_KESHET
   StartModelType *StartModel = reinterpret_cast<StartModelType *>(m_IncomingMsgData[m_IncomingMsgDataIndex]);
   if((StartModel->NumberOfAdditionalModelResins   >  MAX_NUMBER_OF_MODEL_CHAMBERS  -1) ||
	  (StartModel->NumberOfAdditionalSupportResins != MAX_NUMBER_OF_SUPPORT_CHAMBERS-1))
   {
	  CQLog::Write(LOG_TAG_HOST_COMM,"Received CMD_NEW_MODEL or CMD_CONTINUE_MODEL with illegal fields from host");
	  return ceIllegalCommandField;
   }
  
   return  CheckObjetStudioParams(StartModel->PrintMode, StartModel->NumberOfAdditionalModelResins);
#endif   
   return ceNoError;
}//ValidateStartModel

// Process commands received from Host
TCommErrorCodes CHostComm::ProcessCommand(void)
{
  TCommErrorCodes Command_Err = ceNoError;
  try
  {
  CQLog::Write(LOG_TAG_HOST_COMM,"Received " + ((HostOpcode2Str[m_IncomingMsgCommand] == "") ? QFormatStr("Unknown Command[0x%d]", m_IncomingMsgCommand) :  HostOpcode2Str[m_IncomingMsgCommand]) + " from host");
  switch(m_IncomingMsgCommand)
  {
///////////////////////////
     case CMD_BUILD_STATUS:
          ReplyBuildStatus();
          break;

///////////////////////////
    case CMD_STATUS:
         if (!(m_IsMute))
         {
             ReplyStatus();
         }
         break;
///////////////////////////

    case CMD_GET_VERSIONS:
         ReplyVersions();
         break;

///////////////////////////
    case CMD_RESIN_DATA:
         {
            ResinPrinterDataType ResinRequestedData;
            ClearResinRequestedData(ResinRequestedData);

             // Updating the Requested Data
		 if(m_IncomingMsgLength<m_Max_Incoming_Data_Size)
	     {
                  memcpy(&ResinRequestedData,m_IncomingMsgResinData,m_IncomingMsgLength);
	     }
	     else
	       CQLog::Write(LOG_TAG_HOST_COMM,"Resin data Msg legth error: %d",m_IncomingMsgLength);

             Command_Err = ReplyResinPrinterData(ResinRequestedData);
        }
        break;

///////////////////////////
    case CMD_PRINTER_DATA:
         {
            BYTE RequestedData = CURRENT_PRINT;
			if(m_IncomingMsgLength<m_Max_Incoming_Data_Size)
            {
            		// Updating the Requested Data
            		RequestedData   = *m_IncomingMsgData[m_IncomingMsgDataIndex];
                        m_IncomingMsgDataIndex = (m_IncomingMsgDataIndex+1)%SLICE_QUEUE_SIZE;//bug 6171
            }
            else
                     CQLog::Write(LOG_TAG_HOST_COMM,"Printer data Msg legth error: %d",m_IncomingMsgLength);

             ReplyPrinterData(RequestedData);
       }
       break;
	case CMD_FLEXIBLE_PARAMETERS:
	   {
		  FlexibleParametersType *FlexParamsMsg = reinterpret_cast<FlexibleParametersType *>(m_IncomingMsgData[m_IncomingMsgDataIndex]);
		  m_IncomingMsgDataIndex = (m_IncomingMsgDataIndex+1)%SLICE_QUEUE_SIZE;//bug 6171
		  ReplyFlexibleParameters(FlexParamsMsg);
	   }
	   break;
///////////////////////////

//  Incoming Slice from Host.
//  --------------------------------------------
    case CMD_SLICE:
         // Cancel the timeout timer for host coomunication
         CancelHostCommTimeoutTimer();
         //BYTE *TempPtr = m_IncomingMsgData;
         //m_SliceDataMessageQueue->Send(TempPtr);
         m_SliceDataMessageQueue->Send(m_IncomingMsgData[m_IncomingMsgDataIndex]);
         m_IncomingMsgDataIndex = (m_IncomingMsgDataIndex+1)%SLICE_QUEUE_SIZE;//bug 6171
         break;

//  Control operations from Host.
//  --------------------------------------------
    case CMD_STOP:
         CancelHostCommTimeoutTimer();
         m_MachineManager->StopPrint(Q2RT_HOST_RESPONSE);
         break;

    case CMD_PAUSE:
         CancelHostCommTimeoutTimer();
         //Call to pause print and notify that it was paused by Job Manager
         m_MachineManager->PausePrint(PAUSED_BY_HOST);
         m_Paused = true;
         break;

    case CMD_RESUME:
         m_MachineManager->StartPrint();  //ResumePrint
         m_Paused = false;
         if (m_SliceDataMessageQueue->ItemsCount == 0)
            RequestSlice();
         break;

    case CMD_CLEAR:
         break;

	case CMD_NEW_MODEL:
	case CMD_CONTINUE_MODEL:
         {
         m_SlicePreProcessor-> ResetLastPositionY();     //refresh on the Last Position of Y.
         m_SlicePreProcessor->FlushSlices();
         if ( (Command_Err = ValidateStartModel()) != ceNoError )
         {
            if ( Command_Err == ceOnTimeError )
                 Command_Err = ceNoError;
            break;
         }
		 m_OnlineMessagePending = true;
		 StartModelType *StartModel = reinterpret_cast<StartModelType *>(m_IncomingMsgData[m_IncomingMsgDataIndex]);
#ifdef HOST_TEST
		 StartModel->TotalSlices = 1;
		 StartModel->TotalHeight = 0.030f;
		 StartModel->PrintMode = HIGH_SPEED;
		 StartModel->NumberOfAdditionalModelResins = 2;
		 StartModel->YMul = 1;
		 //StartModel->ModelName = "lines";
		 StartModel->JobId = 1234;
#endif


		 // Make sure that there are no left-overs in the slice queue
		 // Allocate a new printing job
		 int QualityMode = (StartModel->PrintMode-1)&0x01;
		 int PrintMode = (StartModel->PrintMode-1)&0x02;
		 m_HostPrintJob->Init(this,
								 StartModel->TotalSlices,
								 StartModel->TotalHeight,
								 ((QualityMode)? HS_INDEX : HQ_INDEX),
								  ((PrintMode)? DIGITAL_MATERIAL_OPERATION_MODE : SINGLE_MATERIAL_OPERATION_MODE),
								 StartModel->YMul,
								 (m_IncomingMsgCommand == CMD_CONTINUE_MODEL),
								 StartModel->ModelName,
								 StartModel->JobId);
//OBJET_MACHINE config
		 CQLog::Write(LOG_TAG_OBJET_MACHINE,"Modes: m_HostPrintJob->Init: PrintMode%d(1=HQ,2=HS) NumOfAddResins:%d(0=SM, >0=DM)",StartModel->PrintMode,StartModel->NumberOfAdditionalModelResins);
		 CQLog::Write(LOG_TAG_OBJET_MACHINE,"StartModel->TotalSlices = %d",StartModel->TotalSlices);
#ifndef OBJET_MACHINE_KESHET
		 int BmpsReceivedFromHostArr[NUMBER_OF_CHAMBERS];// = { -1 };
		 int k=0, i=0;
		 for(; i<NUMBER_OF_CHAMBERS; i++)
				 BmpsReceivedFromHostArr[i] = -1;
		 BmpsReceivedFromHostArr[FIRST_SUPPORT_CHAMBER_INDEX] = FIRST_SUPPORT_CHAMBER_INDEX;
		 BmpsReceivedFromHostArr[FIRST_MODEL_CHAMBER_INDEX] = FIRST_MODEL_CHAMBER_INDEX;


		 if (NUMBER_OF_CHAMBERS == StartModel->NumberOfAdditionalModelResins   +
									StartModel->NumberOfAdditionalSupportResins + MINIMUM_REQUIRED_RESIN_NUM)
		 {
			   for(i=0; i<NUMBER_OF_CHAMBERS; i++)
				 BmpsReceivedFromHostArr[i] = i;
		 }
		 else if(StartModel->NumberOfAdditionalModelResins>0 || StartModel->NumberOfAdditionalSupportResins>0)
		 {
		   CAppParams* gParamsMngr = CAppParams::Instance();

		   k = FIRST_SUPPORT_CHAMBER_INDEX+1;
		   i = 0;
		   //Additional support
		   for(; i<StartModel->NumberOfAdditionalSupportResins; i++)
		   {
			 for(; k<LAST_SUPPORT_CHAMBER_INDEX; k++)
			 {
			   if(strcmp(StartModel->AdditionalMaterials[i],gParamsMngr->TypesArrayPerChamber[k].c_str())==0)
			   {
				 CQLog::Write(LOG_TAG_OBJET_MACHINE, "Additional support resins %d: %s (%s)", i,StartModel->AdditionalMaterials[i],ChamberToStr((TChamberIndex)k));
				 BmpsReceivedFromHostArr[k] = k;
				 break;
			   }
			   else
				 BmpsReceivedFromHostArr[k] = -1;
			 }
		   }
           //Additional models
		   k = FIRST_MODEL_CHAMBER_INDEX+1;
		   for(; i<StartModel->NumberOfAdditionalModelResins; i++)
		   {
		     for(; k<LAST_MODEL_CHAMBER_INDEX; k++)
		     {
			   if(strcmp(StartModel->AdditionalMaterials[i],gParamsMngr->TypesArrayPerChamber[k].c_str())==0)
			   {
		   		 CQLog::Write(LOG_TAG_OBJET_MACHINE, "Additional model resins %d: %s", i,StartModel->AdditionalMaterials[i]);
				 BmpsReceivedFromHostArr[k] = k;
		   	   	 break;
			   }
			   else
				 BmpsReceivedFromHostArr[k] = -1;
			 }
		   }
		 }
		 else //Single material, set -1 in additional resins
		 {
		   for(k=FIRST_SUPPORT_CHAMBER_INDEX+1; k<LAST_SUPPORT_CHAMBER_INDEX; k++)
		   {
			 BmpsReceivedFromHostArr[k] = -1;
		   }
		   for(k=FIRST_MODEL_CHAMBER_INDEX+1; k<LAST_MODEL_CHAMBER_INDEX; k++)
		   {
			 BmpsReceivedFromHostArr[k] = -1;
		   }
		 }
		 m_SlicePreProcessor->SetBmpsReceivedFromHostArr(BmpsReceivedFromHostArr, StartModel->NumberOfAdditionalModelResins);
#endif
         m_CurrJobId = StartModel->JobId;
		 m_SlicePreProcessor->SetTotalSlicesNum(StartModel->TotalSlices);
         m_SlicePreProcessor->SetPrintEndFlag(false);
         // Mark that we got a new print job
		 m_Paused = false;
		 // Add the new print job to the machine manager (marking this is not a Local Job)
         try
         {
			CQLog::Write(LOG_TAG_HOST_COMM,"NumOfUVOn:%d Delay:%d",m_flexibleParameters.m_numOfUVOn,m_flexibleParameters.m_delayBetweenLayers);
			m_MachineManager->SetFlexibleParams(m_flexibleParameters);
		//	m_MachineManager->SetHeadsAmbientTempAccordingToFlexibleParams(m_flexibleParameters);
			m_MachineManager->AddJob(m_HostPrintJob, JobFromHost);
            m_ModelIsBeingReceived = true;            
         }
         catch(EMachineManager& E)
         {
           CQLog::Write(LOG_TAG_HOST_COMM,E.GetErrorMsg());
           StopPrint(E.GetErrorCode(),0);
         }
         catch(EQException& E)
         {
		   QMonitor.ErrorMessage(E.GetErrorMsg());
           StopPrint(E.GetErrorCode(),0);
         }
         break;
         }
         
    case CMD_END_MODEL:
         CancelHostCommTimeoutTimer();
         m_SlicePreProcessor->SetPrintEndFlag(true);
         // Send NULL to the slice data queue as a marker for print-end
         m_SliceDataMessageQueue->Send(NULL);
         break;

    case CMD_CONFIG_FILE:
         SendConfigFile();
         break;

    case CMD_RR_REQUEST:
         Command_Err = AcceptRRRequest();
         m_IncomingMsgDataIndex = (m_IncomingMsgDataIndex+1)%SLICE_QUEUE_SIZE;//bug 6171
		 break;

	case CMD_LICENSED_INFO:
	{
		CQLog::Write(LOG_TAG_HOST_COMM,"Received CMD_LICENSED_INFO from host");
		ReplyLicensedInfo();
		break;
    }	 

    default:
        Command_Err = ceIllegalCommand;  // Unknown command.
  }
  }
  catch(EQException& err)
  {
     CQLog::Write(LOG_TAG_HOST_COMM,"Error while processing command: " + err.GetErrorMsg());
     Command_Err = ceIllegalCommandField;
  }
  catch (...)
  {
     CQLog::Write(LOG_TAG_HOST_COMM,"Error while processing command");
     Command_Err = ceIllegalCommandField;
  }
  return Command_Err;
}

//  Create and Send meesage to Host (without or with data).
//  -------------------------------------------------------
void CHostComm::SaveCmd(HostCommands Cmd,unsigned Length,void *Data)
{
   CQMutexHolder MutexHolder(&m_HostCommandsMutex); //bug 5811
   
   TTag2HostMessage HostMessage;
   HostMessage.Cmd             = Cmd;
   HostMessage.Length          = Length;

   if (! HostMessageQueue.empty())
      HostMessage.Data = malloc(Length+1);
   else
   {
      if (Length > m_AllocatedLength)
      {
         void *new_data = realloc(m_Data, Length+1);
         if (new_data)
         {
            m_Data = new_data;
            m_AllocatedLength = Length;
         }
         else
			throw EQException ("CHostComm::SaveCmd : Can not allocate memory");
      }
      HostMessage.Data     = m_Data;
   }
   memcpy(HostMessage.Data,Data,Length);
   HostMessageQueue.push_back(HostMessage);
   CQLog::Write(LOG_TAG_HOST_COMM,"Saved cmd " + HostOpcode2Str[Cmd] + " to host");
}

void CHostComm::SendSavedCmds()
{
   bool Waiting = false;
   // If a message is currently being received, block until end of message
   if(m_MessageIsBeingReceived)
   {
     m_MessageReceiveMutex.WaitFor();
     Waiting = true;
   }
   CQMutexHolder MutexHolder(&m_HostCommandsMutex);
   TTag2HostMessageQueueIterator Iter = HostMessageQueue.begin();
   for(; Iter != HostMessageQueue.end(); ++Iter)
     SendCmd(Iter->Cmd, Iter->Length, Iter->Data);

   if (HostMessageQueue.size() > 1)
   {
      Iter = HostMessageQueue.begin();
      ++Iter;
      for(; Iter != HostMessageQueue.end(); ++Iter)
        free(Iter->Data);
   }
   
   HostMessageQueue.clear();
   if(Waiting)
     m_MessageReceiveMutex.Release();
}

void CHostComm::SendCmd(HostCommands Cmd,unsigned Length,void *Data)
{
  if ((!VALID_HOST_COMMAND(Cmd)) || m_IsMute)
  // If a message is currently being received, block until end of message
     return;

  
  unsigned char* sbuf;

  // In case of large Data, allocate dynamic memory
  sbuf = new char [(Length + HEADER_SIZE > MAX_SEND_BUFFER) ? Length + HEADER_SIZE : MAX_SEND_BUFFER];
  sbuf[0] = SOH;

//Message command.
//----------------
  sbuf[1] = static_cast<unsigned char>(Cmd);
  unsigned char cksm   = static_cast<unsigned char>(SOH + Cmd);
  unsigned int  OutInd = 2;

//4 bytes for message length:
//---------------------------
  long ln = Length;
  unsigned char *pb = reinterpret_cast<unsigned char *>(&ln);

  for(unsigned int i = 0; i < sizeof(unsigned int); i++)
  {
    sbuf[OutInd++] = *pb;
    cksm += *pb++;
  }

  // Reserve a place for the checksum
  OutInd++;

//"length" bytes for message data:
//--------------------------------
  pb = static_cast <unsigned char *> (Data);

  for(unsigned int i = 0; i < Length; i++)
  {
    sbuf[OutInd++] = *pb;
	cksm += *pb++;
  }

  sbuf[6] = static_cast<unsigned char>(256 - cksm);

  m_Comm->Write(sbuf, OutInd);

  delete []sbuf;


  CQLog::Write(LOG_TAG_HOST_COMM,"Sent " + HostOpcode2Str[Cmd] + " to host");
  
}

void CHostComm::CloseConnection(void)
{
   CancelHostCommTimeoutTimer();
   if(m_Comm->IsConnected())
   {
       m_Comm->Disconnect();
       m_IncomingMsgLength = 0;
   }
}

void CHostComm::RequestSlice(void)
{
  if (!m_ModelIsBeingReceived)
  {
     CQLog::Write(LOG_TAG_HOST_COMM,"RequestSlice(): m_ModelIsBeingReceived=false");
     return;
  }
  // Wind the host reply timeout timer
  TriggerHostCommTimeoutTimer(CAppParams::Instance()->HostSliceTimeoutInSec);
  SaveCmd(CMD_SLICE);
  SendSavedCmds(); //commenting this line caused data shift
}

void CHostComm::StopPrint(TQErrCode ErrCode, int aLastPrintedSlice)
{
  TPrintStopReason Reason = ErrCodeToHostError(ErrCode);
  m_ModelIsBeingReceived = false;
  CancelHostCommTimeoutTimer();
  m_SlicePreProcessor->GetSliceMessageQueue()->Release();
  if(m_Comm->IsConnected())
  {
    TPrinterStoppedMsg StopMsg;
    // Get the time stamp
    time(&StopMsg.TimeStamp);
    StopMsg.StopReason = Reason;

    int LastPrintedSlice = ((aLastPrintedSlice == -1) ? m_MachineManager->GetMachineSequencer()->GetLastPrintedSliceNum() : aLastPrintedSlice);

    CQLog::Write(LOG_TAG_PRINT,"Printing Stopped. Last Slice (Host Comm) - %d", LastPrintedSlice);
    CQLog::Write(LOG_TAG_PRINT,"Printing Stopped. StopReason (Host Comm) - %d", Reason);

    StopMsg.LastPrintedSlice = (LastPrintedSlice > 0) ? LastPrintedSlice : 0;
    StopMsg.TotalNumberOfPasses = CLayer::GetTotalPassesCounter();
    StopMsg.ActualNumberOfPasses = CLayer::GetActualPassesCounter();

    SaveCmd(CMD_STOP,sizeof(TPrinterStoppedMsg),(void *)&StopMsg);

    //if (ErrCode != Q2RT_HOST_RESPONSE) //itamar 5644
    //if (ErrCode != Q2RT_HOST_TCP_ERROR) //itamar 5644
    SendSavedCmds();
    //CloseConnection(); //bug 6029
  }
}

void CHostComm::ReportEndOfModelPrint(void)
{
  if(m_Comm->IsConnected())
  {
    TPrinterEndOfModelMsg EndOfModelMsg;

    // Get the time stamp
    time(&EndOfModelMsg.TimeStamp);
    EndOfModelMsg.LastPrintedSlice     = m_MachineManager->GetMachineSequencer()->GetLastPrintedSliceNum();
    EndOfModelMsg.TotalNumberOfPasses  = CLayer::GetTotalPassesCounter();
    EndOfModelMsg.ActualNumberOfPasses = CLayer::GetActualPassesCounter();
    SaveCmd(CMD_END_MODEL,sizeof(TPrinterEndOfModelMsg),(void *)&EndOfModelMsg);
    SendSavedCmds();
  }
}

void CHostComm::TriggerHostCommTimeoutTimer(int TimeInSec)
{
  m_TimeoutTimer.TriggerTimer(TimeInSec * 1000,TimeoutCallback,
                              reinterpret_cast<TGenericCockie>(this));
}

void CHostComm::CancelHostCommTimeoutTimer(void)
{
  m_TimeoutTimer.Cancel();
}

// Slice receving timeout handling
void CHostComm::TimeoutCallback(TGenericCockie /*Cockie*/)
{
  TQErrCode ErrCode = Q2RT_TIMEOUT_WAITING_END_OF_MODEL;
  CQLog::Write(LOG_TAG_HOST_COMM,PrintErrorMessage(ErrCode));
  // Stop print
  Q2RTApplication->GetMachineManager()->StopPrint(ErrCode);
  FrontEndInterface->ErrorMessage(PrintErrorMessage(ErrCode));
}

// Start execution (resume threads)
void CHostComm::Start(void)
{
  m_SlicePreProcessor->Resume();
  Resume();
  m_MachineManager = Q2RTApplication->GetMachineManager();
}

void CHostComm::ReplyBuildStatus(void)
{
  TPrinterBuildStatusMsg  StatusMsg;
  memset(&StatusMsg,0,sizeof(StatusMsg));
  time(&StatusMsg.TimeStamp);
  StatusMsg.TotalNumberOfPasses  = CLayer::GetTotalPassesCounter();
  StatusMsg.ActualNumberOfPasses = CLayer::GetActualPassesCounter();
  SaveCmd(CMD_BUILD_STATUS,sizeof(TPrinterBuildStatusMsg),(void *)&StatusMsg);
}

void CHostComm::ReplyStatus(void)
{
  CMachineSequencer * MachineSequencer  = m_MachineManager->GetMachineSequencer();
  if (!MachineSequencer)
  {
      CQLog::Write(LOG_TAG_PRINT,"CHostComm::ReplyStatus - error: MachineSequencer not initlaized");
      return;
  }
  CBackEndInterface * BackEnd           = CBackEndInterface::Instance();
//  CAppParams*        ParamsMgr         = CAppParams::Instance();
  CContainerBase    * ContainerInstance = MachineSequencer->GetContainerInstance();
  CAppParams        * gParamsMgr        = CAppParams::Instance();

  TPrinterStatusMsg  StatusMsg;
  size_t             Size              = sizeof(TPrinterStatusMsg);

  BYTE ModelContainerExists;
  BYTE SupportContainerExists;
  BYTE WasteContainerExists;

  // Containers Weight
  int ModelTotalWeight   = 0;
  int SupportTotalWeight = 0;
  int WasteRemainWeight  = 0;

  bool WeightSensorExist = true;
  //bool WeightSensorExist = !(ParamsMgr->WeightSensorsBypass);

  // Containers Existence
  if (WeightSensorExist)
  {
    ModelContainerExists   = ContainerInstance->IsActiveLiquidTankEnabled(TYPE_CHAMBER_MODEL1);
    ModelTotalWeight       = ModelContainerExists   * max(ContainerInstance->GetTotalWeight((int)TYPE_CHAMBER_MODEL1),0);
    SupportContainerExists = ContainerInstance->IsActiveLiquidTankEnabled(TYPE_CHAMBER_SUPPORT);
    SupportTotalWeight     = SupportContainerExists * max(ContainerInstance->GetTotalWeight((int)TYPE_CHAMBER_SUPPORT),0);
    WasteContainerExists   = ContainerInstance->IsActiveLiquidTankEnabled(TYPE_CHAMBER_WASTE);
    WasteRemainWeight      = WasteContainerExists   * max(ContainerInstance->GetRemainingWasteWeight(),0);
  }
  else
  {
    ModelContainerExists   = NO_WEIGHT_SENSORS;
    SupportContainerExists = NO_WEIGHT_SENSORS;
    WasteContainerExists   = NO_WEIGHT_SENSORS;
  }

  // Get the time stamp
  time(&StatusMsg.TimeStamp);

  // Prepare status message
  StatusMsg.PrinterMode            = GetPrinterMode();
  StatusMsg.PrinterSubMode         = PRINTER_SUB_MODE_NORMAL;
  StatusMsg.TrayExistence          = BackEnd->IsTrayInserted();
  StatusMsg.ModelOnTray            = 0;
  CDoorBase * pDoor = MachineSequencer->GetDoorInstance();
  if (pDoor)
      StatusMsg.DoorStatus         = pDoor->GetDoorStatus();
  CUvLamps * pUV = MachineSequencer->GetUVLampsInstance();
  if (pUV)
      StatusMsg.UVLampsStatus      = pUV->GetUVLampsStatus();
  StatusMsg.CurrentJobID           = m_CurrJobId;
  StatusMsg.NumberOfJobSlices      = m_SlicePreProcessor->GetTotalSlicesNum();
  StatusMsg.CurrentModelHeight     = MachineSequencer->GetCurrentModelHeight();
  StatusMsg.CurrentPrintedSlice    = MachineSequencer->GetLastSliceNum();
  StatusMsg.CurrentLayer           = 0;

  StatusMsg.ModelContainer.ContainerExists   = ModelContainerExists;
  StatusMsg.ModelContainer.ContainerWeight   = ModelTotalWeight;
  StatusMsg.SupportContainer.ContainerExists = SupportContainerExists;
  StatusMsg.SupportContainer.ContainerWeight = SupportTotalWeight;
  StatusMsg.WasteContainer.ContainerExists   = WasteContainerExists;
  StatusMsg.WasteContainer.ContainerWeight   = WasteRemainWeight;

  StatusMsg.NumberOfAdditionalModelResins   = 0;//ModelActiveContainersNum()   - MINIMUM_REQUIRED_MODEL_NUM;
  StatusMsg.NumberOfAdditionalSupportResins = 0;//SupportActiveContainersNum() - MINIMUM_REQUIRED_SUPPORT_NUM;

  CQLog::Write(LOG_TAG_PRINT, "Weight: Model1: %d Support: %d Waste: %d",
               StatusMsg.ModelContainer.ContainerWeight, StatusMsg.SupportContainer.ContainerWeight,
               StatusMsg.WasteContainer.ContainerWeight);

  int j = 0; // current additional container index
  for (int i = FIRST_MODEL_CHAMBER_INDEX+1; i < LAST_MODEL_CHAMBER_INDEX; ++i)
  {
    if (BackEnd->IsSupportMaterial(gParamsMgr->MRW_HostTypesPerChamber[i])) //in case of SM, ignore support material
      continue;

    if (strcmp(gParamsMgr->MRW_HostTypesPerChamber[i-1].c_str(), gParamsMgr->MRW_HostTypesPerChamber[i].c_str())!=0) // non-flooded chamber
    {
      bool ContainerExist;

      ++StatusMsg.NumberOfAdditionalModelResins;
      StatusMsg.AdditionalContainersInfo[j].ContainerExists = ContainerExist
               = (WeightSensorExist ? ContainerInstance->IsActiveLiquidTankEnabled(i) : NO_WEIGHT_SENSORS);
      StatusMsg.AdditionalContainersInfo[j].ContainerWeight = ContainerExist * max(ContainerInstance->GetTotalWeight(i),0);

      CQLog::Write(LOG_TAG_PRINT, "Weight: Model%d: %d", i, StatusMsg.AdditionalContainersInfo[j].ContainerWeight);
      ++j;
    }
  }

  for (int i = FIRST_SUPPORT_CHAMBER_INDEX+1; i < LAST_MODEL_CHAMBER_INDEX; ++i)
  {
    if (IS_SUPPORT_CHAMBER(i))
    {
      bool ContainerExist;

      ++StatusMsg.NumberOfAdditionalSupportResins;
      StatusMsg.AdditionalContainersInfo[j].ContainerExists = ContainerExist
               = (WeightSensorExist ? ContainerInstance->IsActiveLiquidTankEnabled(i) : NO_WEIGHT_SENSORS);
      StatusMsg.AdditionalContainersInfo[j].ContainerWeight = ContainerExist * max(ContainerInstance->GetTotalWeight(i),0);

      CQLog::Write(LOG_TAG_PRINT, "Weight: Support%d: %d", i, StatusMsg.AdditionalContainersInfo[j].ContainerWeight);
      ++j;
    }
  }

/*
    for(int i=0; i<StatusMsg.NumberOfAdditionalModelResins;i++)
    {
     bool ContainerExist = StatusMsg.AdditionalContainersInfo[i].ContainerExists = (WeightSensorExist) ?
          ContainerInstance->IsActiveLiquidTankEnabled(FIRST_MODEL_CHAMBER_INDEX+i+1) : NO_WEIGHT_SENSORS;

	 StatusMsg.AdditionalContainersInfo[i].ContainerWeight = ContainerExist * max(ContainerInstance->GetTotalWeight((int)FIRST_MODEL_CHAMBER_INDEX+i+1),0);

	 CQLog::Write(LOG_TAG_PRINT,"Weight: Model%d: %d",i+MINIMUM_REQUIRED_MODEL_NUM,StatusMsg.AdditionalContainersInfo[i].ContainerWeight);
  }
  //else
  //	 Size -= sizeof(TContainerInfo); //OBJET_MACHINE check shouldn't it be Size -= sizeof(TContainerInfo)*NUMBER_OF_ADDITIONAL_RESINS ?
  int totalNumOfAdditionalContainers = StatusMsg.NumberOfAdditionalModelResins + StatusMsg.NumberOfAdditionalSupportResins;
*/
  Size -= sizeof(TContainerInfo)*((MM_RESIN_NUM-MINIMUM_REQUIRED_RESIN_NUM) - j);

  SaveCmd(CMD_STATUS,Size,(void *)&StatusMsg);
}


void CHostComm::ReplyVersions(void)
{
  TPrinterVersionsMsg PrinterVersionMsg;

  // Get the time stamp
  time(&PrinterVersionMsg.TimeStamp);

  // Prepare printer versions reply
  QString s = QGetAppVersionStr();
  strcpy(PrinterVersionMsg.RPJetSoftware,s.c_str());
  PrinterVersionMsg.PrinterType = (BYTE)(INT_FROM_RESOURCE(IDN_PRINTER_TYPE));

  // Current host protocol version
  strcpy(PrinterVersionMsg.HostPrinterProtocol,"8");

  PrinterVersionMsg.RPJetHardware[0] = NULL;
  PrinterVersionMsg.OCBSoftware[0]   = NULL;
  PrinterVersionMsg.OCBHardware[0]   = NULL;
  PrinterVersionMsg.OHDBSoftware[0]  = NULL;
  PrinterVersionMsg.OHDBHardware[0]  = NULL;
  PrinterVersionMsg.MCBSoftware[0]   = NULL;
  PrinterVersionMsg.MCBHardware[0]   = NULL;

  if ((PrinterVersionMsg.HaspExpirationTimeStamp = CBackEndInterface::Instance()->CalculateHaspTimeToExpiration()) != -1)
     PrinterVersionMsg.HaspExpirationTimeStamp += PrinterVersionMsg.TimeStamp;
  SaveCmd(CMD_GET_VERSIONS,sizeof(TPrinterVersionsMsg),(void *)&PrinterVersionMsg);
}

void CHostComm::GetMode(BYTE RequestedData, int &qm, int &om)
{
  switch (RequestedData)
  {
//OBJET_MACHINE config check!!!
    case HIGH_QUALITY_3RESIN:
         om = DIGITAL_MATERIAL_OPERATION_MODE;
         qm = HQ_INDEX;
         break;

    case HIGH_SPEED_3RESIN:
         om = DIGITAL_MATERIAL_OPERATION_MODE;
         qm = HS_INDEX;
         break;

    case HIGH_QUALITY:
         om = SINGLE_MATERIAL_OPERATION_MODE;
         qm = HQ_INDEX;
         break;

    case HIGH_SPEED:
         om = SINGLE_MATERIAL_OPERATION_MODE;
         qm = HS_INDEX;
         break;

    case CURRENT_PRINT:
         om = CAppParams::Instance()->PipesOperationMode;
         qm = HS_INDEX;
         break;

    default:
         om = CAppParams::Instance()->PipesOperationMode;
         qm = HS_INDEX;
         CQLog::Write(LOG_TAG_HOST_COMM,"GetMode Error: Nonexisting mode requested by host");
         break;
  }
}

void CHostComm::ReplyPrinterData(BYTE RequestedData)
{
  CAppParams*         gParamsMgr         = CAppParams::Instance();
  QString             ConfigFilePath     = gParamsMgr->GetConfigPath() ;
  CBackEndInterface* BackEnd           = CBackEndInterface::Instance();
  int                 om,qm;

  CQLog::Write(LOG_TAG_TRAY_HEATERS, "ThermistorOpMode debugging: init temp params");
  m_ParamsMngr->Init();

  CQLog::Write(LOG_TAG_TRAY_HEATERS, "ThermistorOpMode debugging: find mode");
  GetMode(RequestedData, qm, om);

  CQLog::Write(LOG_TAG_TRAY_HEATERS, "ThermistorOpMode debugging: Enter temp mode. Temp manager = 0x%x; Main manager = 0x%x", m_ParamsMngr, CAppParams::Instance());
  if (GetModeAccessibility(qm, om) == true)
     m_ParamsMngr->EnterMode(PRINT_MODE[qm][om], MACHINE_QUALITY_MODES_DIR(qm,om));

  TPrinterDataMsg PrinterDataMsg;
  size_t          Size           = sizeof(TPrinterDataMsg);

  // Get the time stamp
  time(&PrinterDataMsg.TimeStamp);
  PrinterDataMsg.ScatterFactor       = m_ParamsMngr->ScatterEnabled ? m_ParamsMngr->ScatterFactorMin : 0;
  PrinterDataMsg.LongPurgePeriod     = m_ParamsMngr->TimeBetweenPurgesInSec;
  PrinterDataMsg.ShortPurgePeriod    = 0;
  PrinterDataMsg.ExtraPurgePeriod    = 0;
  PrinterDataMsg.WipePeriod          = 0;
  PrinterDataMsg.FireAllPeriod       = m_ParamsMngr->TimeBetweenFireAllInSec;
  PrinterDataMsg.AverageLayer600DPI  = m_ParamsMngr->SmartRollerEffectiveLayerThickness;// <-- Smart Roller <-- LayerHeightDPI_um;
  if(Q2RTApplication->GetMachineType() == (mtObjet1000 ))
	PrinterDataMsg.ModelXResolution    = 300;   // Fixate to 300 DPI for a test required by Udi.
  else //requested by O.S. for Keshet. Means that the EM expects to receive from the Host bmps with this resolution
	PrinterDataMsg.ModelXResolution    = m_ParamsMngr->SliceResolution_InXAxis;

  PrinterDataMsg.ModelYResolution    = 300;
  PrinterDataMsg.PassMaskUsed        = m_ParamsMngr->PassMask;
  PrinterDataMsg.YScanMode           = m_ParamsMngr->YScanDirection;
  PrinterDataMsg.YDeltaReturn        = m_ParamsMngr->YInterlaceNoOfPixels;
  PrinterDataMsg.AverageLayer1200DPI = m_ParamsMngr->SmartRollerEffectiveLayerThickness;// <-- Smart Roller <-- LayerHeightDPI_um;
  PrinterDataMsg.MaxScatter          = m_ParamsMngr->ScatterEnabled ? m_ParamsMngr->ScatterFactorMax : 0;

  CScatterGenerator::Init();
  PrinterDataMsg.TrayRearOffset  = CScatterGenerator::s_TrayRearOffset  * MM_PER_INCH / m_ParamsMngr->DPI_InYAxis;
  PrinterDataMsg.TrayFrontOffset = CScatterGenerator::s_TrayFrontOffset * MM_PER_INCH / m_ParamsMngr->DPI_InYAxis;

  PrinterDataMsg.TrayYSize = m_ParamsMngr->TrayYSize - TRAY_Y_START_OFFSET_MM;
  strcpy (PrinterDataMsg.ModelMaterial,   gParamsMgr->MRW_HostTypesPerChamber[FIRST_MODEL_CHAMBER_INDEX].c_str());
  strcpy (PrinterDataMsg.SupportMaterial, gParamsMgr->MRW_HostTypesPerChamber[FIRST_SUPPORT_CHAMBER_INDEX].c_str());
  PrinterDataMsg.XVelocity      = m_ParamsMngr->MotorsVelocity[AXIS_X];
  // ONLY FOR TEST , YOAV
  int tmpYStepPixel = 0;
  tmpYStepPixel = m_ParamsMngr->YStepsPerPixel;
  PrinterDataMsg.YStepsPerPixel = (float)tmpYStepPixel;
 // PrinterDataMsg.YStepsPerPixel = m_ParamsMngr->YStepsPerPixel;

  //Information about additional resins is always from application Parameter Manager
  PrinterDataMsg.NumberOfAdditionalModelResins   = 0;
  PrinterDataMsg.NumberOfAdditionalSupportResins = 0;
  Size -= NUMBER_OF_ADDITIONAL_RESINS * sizeof(TResinName);


//OBJET_MACHINE config
  int j = 0;
  for (int i = FIRST_MODEL_CHAMBER_INDEX+1; i < LAST_MODEL_CHAMBER_INDEX; ++i)
  {
    if (BackEnd->IsSupportMaterial(gParamsMgr->MRW_HostTypesPerChamber[i])) //in case of SM, ignore support material
      continue;
    if (strcmp(gParamsMgr->MRW_HostTypesPerChamber[i-1].c_str(),gParamsMgr->MRW_HostTypesPerChamber[i].c_str())!=0)
    {
      PrinterDataMsg.NumberOfAdditionalModelResins++;
      strcpy (PrinterDataMsg.AdditionalMaterials[j++], gParamsMgr->MRW_HostTypesPerChamber[i].c_str());
      Size += sizeof(TResinName);
    }
  }

  for (int i = FIRST_SUPPORT_CHAMBER_INDEX+1; i < LAST_MODEL_CHAMBER_INDEX; ++i)
  {
    if (IS_SUPPORT_CHAMBER(i))
    {
        ++PrinterDataMsg.NumberOfAdditionalSupportResins;
        strcpy(PrinterDataMsg.AdditionalMaterials[j++], gParamsMgr->MRW_HostTypesPerChamber[i].c_str());
        Size += sizeof(TResinName);
    }
  }

  SaveCmd(CMD_PRINTER_DATA,Size,(void *)&PrinterDataMsg);
}

int CHostComm::ClearResinRequestedData(ResinPrinterDataType& ResinRequestedData)
{
  memset(&ResinRequestedData, 0, sizeof(ResinRequestedData));
  ResinRequestedData.PrintMode = CURRENT_PRINT;
  strcpy (ResinRequestedData.ModelMaterial,   "");
  strcpy (ResinRequestedData.SupportMaterial, "");
  ResinRequestedData.NumberOfAdditionalModelResins = 0;
  ResinRequestedData.NumberOfAdditionalSupportResins = 0;
  return 0;
}

TCommErrorCodes CHostComm::ReplyResinPrinterData(ResinPrinterDataType& ResinRequestedData)
{
  if (ResinRequestedData.NumberOfAdditionalModelResins >= MAX_NUMBER_OF_MODEL_CHAMBERS ||
      ResinRequestedData.NumberOfAdditionalSupportResins >= MAX_NUMBER_OF_SUPPORT_CHAMBERS )
  {
     CQLog::Write(LOG_TAG_HOST_COMM,"Received CMD_RESIN_DATA with illegal fields from host");
     return ceIllegalCommandField;
  }

  QString             ConfigFilePath     = CAppParams::Instance()->GetConfigPath() ;
  int                 om,qm;
//  TInitStatus_E       StatusInit         = INIT_FAIL_E; //init
  
  m_ParamsMngr->Init();

  if (QString(ResinRequestedData.ModelMaterial) != "")
	 m_ParamsMngr->TypesArrayPerChamber[FIRST_MODEL_CHAMBER_INDEX] = ResinRequestedData.ModelMaterial;
  if (QString(ResinRequestedData.SupportMaterial) != "")
	 m_ParamsMngr->TypesArrayPerChamber[FIRST_SUPPORT_CHAMBER_INDEX] = ResinRequestedData.SupportMaterial;
  for (int i = FIRST_MODEL_CHAMBER_INDEX; i < FIRST_MODEL_CHAMBER_INDEX + ResinRequestedData.NumberOfAdditionalModelResins; i++)
	 m_ParamsMngr->TypesArrayPerChamber[i] = ResinRequestedData.AdditionalMaterials[i- FIRST_MODEL_CHAMBER_INDEX + ResinRequestedData.NumberOfAdditionalSupportResins];
  for (int i = FIRST_SUPPORT_CHAMBER_INDEX; i < FIRST_SUPPORT_CHAMBER_INDEX + ResinRequestedData.NumberOfAdditionalSupportResins; i++)
	 m_ParamsMngr->TypesArrayPerChamber[i] = ResinRequestedData.AdditionalMaterials[i- FIRST_SUPPORT_CHAMBER_INDEX];
//OBJET_MACHINE config
  if ((ResinRequestedData.NumberOfAdditionalSupportResins + ResinRequestedData.NumberOfAdditionalModelResins) > 0)
     m_ParamsMngr->PipesOperationMode = DIGITAL_MATERIAL_OPERATION_MODE;
  m_ParamsMngr->EnterMode(DEFAULT_MATERIALS_MODE, DEFAULT_MODES_DIR);
  m_ParamsMngr->EnterMode(OPERATION_MODE_NAME(m_ParamsMngr->PipesOperationMode), GENERAL_MODES_DIR);
  GetMode(ResinRequestedData.PrintMode, qm, om);
  if (GetModeAccessibility(qm, om) == true)
     m_ParamsMngr->EnterMode(PRINT_MODE[qm][om], MACHINE_QUALITY_MODES_DIR(qm,om));

  TResinPrinterDataMsg PrinterDataMsg;

  // Get the time stamp
  time(&PrinterDataMsg.TimeStamp);
  PrinterDataMsg.ScatterFactor       = m_ParamsMngr->ScatterEnabled ? m_ParamsMngr->ScatterFactorMin : 0;
  PrinterDataMsg.LongPurgePeriod     = m_ParamsMngr->TimeBetweenPurgesInSec;
  PrinterDataMsg.ShortPurgePeriod    = 0;
  PrinterDataMsg.ExtraPurgePeriod    = 0;
  PrinterDataMsg.WipePeriod          = 0;
  PrinterDataMsg.FireAllPeriod       = m_ParamsMngr->TimeBetweenFireAllInSec;
  PrinterDataMsg.AverageLayer600DPI  = m_ParamsMngr->SmartRollerEffectiveLayerThickness;// <-- Smart Roller <-- LayerHeightDPI_um;
  //requested by O.S. for Keshet. Means that the EM expects to receive from the Host bmps with this resolution 	
  PrinterDataMsg.ModelXResolution    = m_ParamsMngr->SliceResolution_InXAxis;
  PrinterDataMsg.ModelYResolution    = 300;
  PrinterDataMsg.PassMaskUsed        = m_ParamsMngr->PassMask;
  PrinterDataMsg.YScanMode           = m_ParamsMngr->YScanDirection;
  PrinterDataMsg.YDeltaReturn        = m_ParamsMngr->YInterlaceNoOfPixels;
  PrinterDataMsg.AverageLayer1200DPI = m_ParamsMngr->SmartRollerEffectiveLayerThickness;// <-- Smart Roller <-- LayerHeightDPI_um;
  PrinterDataMsg.MaxScatter          = m_ParamsMngr->ScatterEnabled ? m_ParamsMngr->ScatterFactorMax : 0;

  CScatterGenerator::Init();
  PrinterDataMsg.TrayRearOffset  = CScatterGenerator::s_TrayRearOffset  * MM_PER_INCH / m_ParamsMngr->DPI_InYAxis;
  PrinterDataMsg.TrayFrontOffset = CScatterGenerator::s_TrayFrontOffset * MM_PER_INCH / m_ParamsMngr->DPI_InYAxis;

  PrinterDataMsg.TrayYSize = m_ParamsMngr->TrayYSize - TRAY_Y_START_OFFSET_MM;
  PrinterDataMsg.XVelocity      = m_ParamsMngr->MotorsVelocity[AXIS_X];
 // PrinterDataMsg.YStepsPerPixel = m_ParamsMngr->YStepsPerPixel;
   // ONLY FOR TEST , YOAV
   int tmpYStepPixel = 0;
  tmpYStepPixel = m_ParamsMngr->YStepsPerPixel;
  PrinterDataMsg.YStepsPerPixel = (float)tmpYStepPixel;
  PrinterDataMsg.AtLeastDelayTimeBetweenLayers = m_ParamsMngr->AtLeastDelayTimeBetweenLayers;

//for the new time estimation algorithm
  PrinterDataMsg.YVelocity      = m_ParamsMngr->MotorsVelocity[AXIS_Y];
  PrinterDataMsg.AccelerationX      = m_ParamsMngr->MotorsAcceleration[AXIS_X];
  PrinterDataMsg.AccelerationY      = m_ParamsMngr->MotorsAcceleration[AXIS_Y];
  PrinterDataMsg.YSecondaryInterlaceNoOfPixels      = m_ParamsMngr->YSecondaryInterlaceNoOfPixels;

  CQLog::Write(LOG_TAG_HOST_COMM,"AtLeastDelayTimeBetweenLayers: " + QIntToStr(PrinterDataMsg.AtLeastDelayTimeBetweenLayers));

  SaveCmd(CMD_RESIN_DATA,sizeof(PrinterDataMsg),(void *)&PrinterDataMsg);

  return ceNoError;
}

void CHostComm::ReplyLicensedInfo(void)
{
  TPackagesList             PackagesList             = CBackEndInterface::Instance()->GetLicensedPackagesList();
  TLicensedMaterialsList    MaterialsList            = CBackEndInterface::Instance()->GetLicensedMaterialsList();
  TLicensedMaterialsList    DigitalMaterialsList     = CBackEndInterface::Instance()->GetLicensedDigitalMaterialsList();
  TAdditionalHostParamsList AdditionalHostParamsList = CBackEndInterface::Instance()->GetLicensedAdditionalHostParamsList();
  TTrayPropertiesList       TrayPropertiesList       = CBackEndInterface::Instance()->GetLicensedTrayPropertiesList();
  TLicensedModesList        ModesList                = CBackEndInterface::Instance()->GetLicensedModesList();

  TLicensedInfoMsg LicensedInfoMsg;
  memset(&LicensedInfoMsg, 0, sizeof(LicensedInfoMsg));

  size_t Size = sizeof(TLicensedInfoMsg);
  Size -= NUM_OF_POSSIBLE_MATERIALS * sizeof(TResinName);
  Size -= NUM_OF_POSSIBLE_MATERIALS * sizeof(TResinName); // For DM materials
  Size -= NUM_OF_POSSIBLE_PACKAGES  * sizeof(TPackageName);
  Size -= NUM_OF_POSSIBLE_PACKAGES  * sizeof(int);        // Packages HASP features (machine IDs)
  Size -= MAX_STRING_SIZE * sizeof(char);
																  
  // Get the time stamp
  time(&LicensedInfoMsg.TimeStamp);

  // Get licensed modes list
#ifdef EMULATION_ONLY
  LicensedInfoMsg.OperationModes = 0xff;
#else
  LicensedInfoMsg.OperationModes = 0;
#endif
  for (TLicensedModesList::iterator it = ModesList.begin() ; it < ModesList.end() ; ++it)
  {
	  // Converting from Operation Modes indexing to modes indexing used by host:
	  // -------------- HostMode = 2 ^ (OperationMode - 1) ------------------------
	  // HQ = 2^(1-1) = 1, HS = 2^(2-1) = 2, DM = 2^(4-1) = 8, Draft = 2^(5-1) = 16
	  // --------------------------------------------------------------------------
	  switch(it->ModeTag)
	  {
		  case mmHighQualityMode:
			LicensedInfoMsg.OperationModes |= static_cast<BYTE>(pow((double)2, HIGH_QUALITY_3RESIN - 1));
			break;
		  case mmHighSpeedMode:
			LicensedInfoMsg.OperationModes |= static_cast<BYTE>(pow((double)2, HIGH_SPEED - 1));
			break;
		  case mmDigitalMaterialMode:
			LicensedInfoMsg.OperationModes |= static_cast<BYTE>(pow((double)2, HIGH_SPEED_3RESIN - 1));
			break;
      }
  }

  // Get the licensed packages list
  int NumOfPacks = 0;
  for (TPackagesList::iterator it = PackagesList.begin(); it != PackagesList.end() ; ++it)
  {
	  strcpy(LicensedInfoMsg.Packages[NumOfPacks], it->PackageName.c_str());
	  LicensedInfoMsg.PackageIDs[NumOfPacks] = it->HaspFeature;
	  Size += sizeof(TPackageName);
	  Size += sizeof(int);
	  NumOfPacks++;
  }
  LicensedInfoMsg.NumOfPackages = NumOfPacks;

  // Moving PackageIDs to a correct place after Packages list
  size_t EmptyPacksSize = (NUM_OF_POSSIBLE_PACKAGES * sizeof(TPackageName)) - (NumOfPacks * sizeof(TPackageName));
  memmove((char *)LicensedInfoMsg.PackageIDs - EmptyPacksSize, LicensedInfoMsg.PackageIDs, sizeof(int) * NumOfPacks);

  // Get the licensed materials list
  int NumOfMaterials = 0;
  TLicensedMaterialsList::iterator it = MaterialsList.begin();
  for (; it < MaterialsList.end() ; ++it)
  {
	  strcpy(LicensedInfoMsg.Materials[NumOfMaterials++], it->MaterialName.c_str());
	  Size += sizeof(TResinName);
  }
  LicensedInfoMsg.NumOfMaterials = NumOfMaterials;

  // Moving NumOfMaterials and Materials to a correct place inside the struct after the Packages and HASP features dynamic lists
  size_t EmptyPacksIDsSize = (NUM_OF_POSSIBLE_PACKAGES * sizeof(int)) - (NumOfPacks * sizeof(int));
  size_t MoveMaterials     = EmptyPacksIDsSize + EmptyPacksSize;
  memmove((char *)(&(LicensedInfoMsg.NumOfMaterials)) - MoveMaterials, &(LicensedInfoMsg.NumOfMaterials), sizeof(int));
  memmove((char *)LicensedInfoMsg.Materials - MoveMaterials, LicensedInfoMsg.Materials, sizeof(TResinName) * NumOfMaterials);

  // Get the licensed digital materials list
  int NumOfDMMaterials = 0;
  for (TLicensedMaterialsList::iterator it = DigitalMaterialsList.begin(); it != DigitalMaterialsList.end() ; ++it)
  {
	  strcpy(LicensedInfoMsg.DigitalMaterials[NumOfDMMaterials++], it->MaterialName.c_str());
	  Size += sizeof(TResinName);
  }
  LicensedInfoMsg.NumOfDigitalMaterials = NumOfDMMaterials;

  // Moving NumOfDigitalMaterials and DigitalMaterials to a correct place inside the struct after the Materials dynamic list
  size_t EmptyMaterialsSize = (NUM_OF_POSSIBLE_MATERIALS * sizeof(TResinName)) - (NumOfMaterials * sizeof(TResinName));
  size_t MoveDMs            = EmptyMaterialsSize + MoveMaterials;
  memmove((char *)(&(LicensedInfoMsg.NumOfDigitalMaterials)) - MoveDMs, &(LicensedInfoMsg.NumOfDigitalMaterials), sizeof(int));
  memmove((char *)LicensedInfoMsg.DigitalMaterials - MoveDMs, LicensedInfoMsg.DigitalMaterials, sizeof(TResinName) * NumOfDMMaterials);

  // Get the licensed tray properties
  LicensedInfoMsg.TrayProperties = 0;
  for(TTrayPropertiesList::iterator it = TrayPropertiesList.begin(); it != TrayPropertiesList.end() ; ++it)
	  LicensedInfoMsg.TrayProperties |= it->BitMask;

  // Moving TrayProperties to a correct place inside the struct after the DigitalMaterials dynamic list
  size_t EmptyDMsSize  = (NUM_OF_POSSIBLE_MATERIALS * sizeof(TResinName)) - (LicensedInfoMsg.NumOfDigitalMaterials * sizeof(TResinName));
  size_t MoveTrayProps = MoveDMs + EmptyDMsSize;
  memmove((char *)(&(LicensedInfoMsg.TrayProperties)) - MoveTrayProps, &(LicensedInfoMsg.TrayProperties), sizeof(BYTE));

  // Get Additional Host Parameters as combined XML string for all packages
  QString AdditionalHostParamsXML = "";
  for (TAdditionalHostParamsList::iterator it = AdditionalHostParamsList.begin(); it != AdditionalHostParamsList.end() ; ++it)
	  AdditionalHostParamsXML += it->AdditionalHostParamsXML;                   
  strcpy(LicensedInfoMsg.AdditionalHostParams, AdditionalHostParamsXML.c_str());
  int XMLStringSize = strlen(LicensedInfoMsg.AdditionalHostParams) * sizeof(char);
  Size += XMLStringSize;
  LicensedInfoMsg.AdditionalHostParamsSize = XMLStringSize;

  // Moving AdditionalHostParamsSize and AdditionalHostParams to a correct place inside the struct after the TrayProperties
  memmove((char *)(&(LicensedInfoMsg.AdditionalHostParamsSize)) - MoveTrayProps, &(LicensedInfoMsg.AdditionalHostParamsSize), sizeof(int));
  memmove((char *)LicensedInfoMsg.AdditionalHostParams - MoveTrayProps, LicensedInfoMsg.AdditionalHostParams, sizeof(char) * LicensedInfoMsg.AdditionalHostParamsSize);

  SaveCmd(CMD_LICENSED_INFO,Size,(void *)&LicensedInfoMsg);
  
  CQLog::Write(LOG_TAG_HOST_COMM,"Sent CMD_LICENSED_INFO to host");
}

void CHostComm::InitFlexibleParams()
{
   m_flexibleParameters.m_numOfUVOn = -1;
   m_flexibleParameters.m_delayBetweenLayers = -1;
   m_flexibleParameters.m_rollerSpeed = "RGD";
}

void CHostComm::ReplyFlexibleParameters(FlexibleParametersType* FlexParamsMsg)
{

  if(FlexParamsMsg->NumofFlexibleParams<=NUM_OF_FLEXIBLE_PARAMETERS)
  {
	for (int i=0; i < FlexParamsMsg->NumofFlexibleParams; i++)
	{
	  switch(FlexParamsMsg->FlexibleParamsArr[i].ParamID)
	  {
		case NUM_OF_UV_ON:
		  m_flexibleParameters.m_numOfUVOn = (int)(FlexParamsMsg->FlexibleParamsArr[i].val);
		  break;

		case DELAY_BETWEEN_LAYERS:
		{
          //the casting is done because Studio is sending float instead of int
		  float tmp = *(reinterpret_cast<float*>(&(FlexParamsMsg->FlexibleParamsArr[i].val)));
		  m_flexibleParameters.m_delayBetweenLayers = static_cast<int>(tmp);
		  break;
		}
		case ROLLER_SPEED:
		{
		   string tmp(reinterpret_cast<char*>(&(FlexParamsMsg->FlexibleParamsArr[i].val)));
			m_flexibleParameters.m_rollerSpeed = /* static_cast<string>*/(tmp);
			break;

        }
		default:
		  CQLog::Write(LOG_TAG_HOST_COMM,"Received wrong paramID:%d ",FlexParamsMsg->FlexibleParamsArr[i].ParamID);
	  }
	}
  }
  else
	CQLog::Write(LOG_TAG_OBJET_MACHINE, "Invalid Num of flex params size:%d", FlexParamsMsg->NumofFlexibleParams);
}

// Help function to decide what is the current printer mode
int CHostComm::GetPrinterMode(void)
{
  int Mode;

  switch(m_MachineManager->GetCurrentState())
  {
    case msPrePrint: Mode = PRINTER_MODE_PRE_PRINT; break;
    case msPrinting: Mode = PRINTER_MODE_PRINTING;  break;
    case msStopping: Mode = PRINTER_MODE_STOPPING;  break;
    case msPausing:  Mode = PRINTER_MODE_PAUSING;   break;
    case msPaused:   Mode = PRINTER_MODE_PAUSED;    break;
    case msStopped:  Mode = PRINTER_MODE_STOPPED;   break;

    default: Mode = PRINTER_MODE_IDLE;
  }

  return Mode;
}

void CHostComm::SetSlicePreviewUpdateEvent(TGenericCallback SlicePreviewEvent,TGenericCockie SlicePreviewEventCockie)
{
  m_SlicePreProcessor->SetSlicePreviewUpdateEvent(SlicePreviewEvent,SlicePreviewEventCockie);
}

PBYTE CHostComm::RequestBMPAndLock(int Chamber, int& Width,int& Height,int& SliceNumber)
{
  CSlice *TmpSlicePtr = m_SlicePreProcessor->GetLastSliceAndLock();

  if(TmpSlicePtr != NULL)
  {
    Width       = TmpSlicePtr->GetWidth();
    Height      = TmpSlicePtr->GetHeight();
    SliceNumber = TmpSlicePtr->GetSliceNumber();
    if(VALIDATE_CHAMBER(Chamber))
       return TmpSlicePtr->GetBuffer((TChamberIndex)Chamber);
  }

  return NULL;
}

void CHostComm::ReleaseBMPBuffersLock(void)
{
  m_SlicePreProcessor->ReleaseLastSliceLock();
}

// Did the host send a 'pause' command
bool CHostComm::IsPaused()
{
  return m_Paused.Value();
}

//OBJET_MACHINE config
TCommErrorCodes CHostComm::AcceptRRRequest(void)
{
  ResinReplacementRequestType* Msg = reinterpret_cast<ResinReplacementRequestType *>(m_IncomingMsgData[m_IncomingMsgDataIndex]);

  switch(Msg->MaintenanceMode)
  {
    case HIGH_QUALITY_3RESIN:
    case HIGH_SPEED_3RESIN:
         Msg->MaintenanceMode = DIGITAL_MATERIAL_OPERATION_MODE;
         break;

    case HIGH_QUALITY:
    case HIGH_SPEED:
         Msg->MaintenanceMode = SINGLE_MATERIAL_OPERATION_MODE;
         break;

    default:
         CQLog::Write(LOG_TAG_HOST_COMM,"AcceptPendingResinReplacementRequest Error: Nonexisting mode requested by host");
         return ceIllegalCommandField;
  
  }
  m_MachineManager->AcceptPendingResinReplacementRequest(Msg);
  return ceNoError;
}


// Send the config file to the host.
void CHostComm::SendConfigFile(void)
{
  char *Buffer      = NULL;
  unsigned  FileSize    = 0;
  QString FileName = Q2RTApplication->AppFilePath.Value() + CONFIG_BACKUP_FILE_NAME;
  m_ConfigBackup->CreateConfigBackupFile();

  try
  {
  try
  {
	unsigned  ByteCounter = 0;
	FILE *Stream;
	Stream = fopen(FileName.c_str(), "rb");
	FileSize = GetFileSize(Stream);

	Buffer = new BYTE[FileSize + 1];
	if (Buffer==NULL)
	  throw EQException ("CHostComm:Failed to allocate buff for read from the config file");

	ByteCounter = fread (Buffer, sizeof(BYTE), FileSize, Stream);

	if (ByteCounter != FileSize)
	  throw EQException ("CHostComm:Wrong Number of bytes read from the config file");

	//Sending the file
	SaveCmd(CMD_CONFIG_FILE , ByteCounter, (void *)Buffer);

	fclose(Stream);
	if(Buffer)
	{
		delete [] Buffer;
		Buffer = NULL;
	}
  }
  catch(EQException& E)
  {
	QMonitor.ErrorMessage(E.GetErrorMsg());
  }
  }
  __finally
  {
	 if(Buffer)
		delete [] Buffer;
  }
}


long CHostComm::GetFileSize(FILE *Stream)
{
   long Length;

   fseek(Stream, 0L, SEEK_END);
   Length = ftell(Stream);
   fseek(Stream, 0, SEEK_SET);
   return Length;
}

void CHostComm::SendEOLMessage()
{
  SaveCmd(CMD_EOL);
}

void CHostComm::SetOnlineOffline(bool Online)
{
  m_OnlineOffline = Online;
  if (m_OnlineMessagePending && m_OnlineOffline)
  {
     SaveCmd(CMD_ONLINE);
     m_OnlineMessagePending = false;
  }
}

TSliceMessageQueue* CHostComm:: GetSliceMessageQueue(void)
{
  return m_SlicePreProcessor->GetSliceMessageQueue();
}

void CHostComm::DisposeSlice(CSlice *Slice)
{
  m_SlicePreProcessor->DisposeSlice(Slice);
}

TPrintStopReason CHostComm::ErrCodeToHostError(TQErrCode ErrCode)
{
   int ErrIndex = TOGGLE_Q2RT_ERROR_ID(ErrCode);
   TPrintStopReason ReturnValue = (VALIDATE_Q2RT_ERROR(ErrCode) ? ERR_ENUM_TO_HOST_ERROR(ErrIndex) : STOP_UNKNOWN_HOST_ERROR);

   switch(ErrCode)
   {

	  case Q2RT_STOP_DURING_PRE_PRINT:
	  case Q2RT_SEQUENCE_CANCELED:
		   ReturnValue = STOP_EMBEDDED_RESPONSE;
	       break;
   }

   return  ReturnValue;
}

