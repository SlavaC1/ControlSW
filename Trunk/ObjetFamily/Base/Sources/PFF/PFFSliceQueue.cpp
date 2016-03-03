//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "PFFSliceQueue.h"
#include "QMonitor.h"
#pragma package(smart_init)


#define THE_PRODUCT_SIZE_OF_TRAY_OBJET1000_IN_TRIPLEX_TRAY 4
#define MAX_SLICE_MEM_SIZE 15000000   //bug 6171

//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(&UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall CPFFSliceQueueThread::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

CPFFSliceQueueThread::CPFFSliceQueueThread()
	: CQThread(true,"PFFSliceQueueThread",false)
{
	// Message queue for the ready slices
	m_SliceMessageQueue = new TSliceMessageQueue(PFF_SLICE_QUEUE_SIZE);

	int MaxSliceMemSize = MAX_SLICE_MEM_SIZE;
	if((IsRunningOnWindows64Bit()) && ((Q2RTApplication->GetMachineType()) == (mtObjet1000 )) )
	{
		CQLog::Write(LOG_TAG_GENERAL,"Info: CSlicePreProcessor::Constructor runs on Objet1000 64bit");
		MaxSliceMemSize*= THE_PRODUCT_SIZE_OF_TRAY_OBJET1000_IN_TRIPLEX_TRAY;
	}
	m_SlicesAllocator.Init( PFF_BMP_CREATION_QUEUE_SIZE * NUMBER_OF_CHAMBERS, MaxSliceMemSize); //bug 6171
}

CPFFSliceQueueThread::~CPFFSliceQueueThread()
{
	FlushSlices();
	
	Terminate();
	m_SliceMessageQueue->Release();
	WaitFor();

	Q_SAFE_DELETE(m_SliceMessageQueue);
 }

 void CPFFSliceQueueThread::Init(QString Path)
 {
 	m_NumberOFSlices = 0;
	m_CurrentSliceNumber = 0;
	m_SliceMissingMessageShowed = false;
	strcpy(m_Path, Path.c_str());
	FlushSlices();
 }

//---------------------------------------------------------------------------
void CPFFSliceQueueThread::Execute()
{
	while ( !Terminated )
	{
		if (m_CurrentSliceNumber < m_NumberOFSlices)
		{

			try
			{
				if (!GetNextFilesNamesAndValidate(m_BmpFileNames))
				{
					// Do nothing and wait for the BMP's to be found again :)
				}
				else
				{
					CSlice* Slice = new CSlice();

					CreateSliceFromFiles( m_BmpFileNames, (*Slice) );

					if(m_SliceMessageQueue->Send(Slice) == QLib::wrReleased)
					{
						DisposeSlice(Slice);
						CQLog::Write(LOG_TAG_PFF,"Slice queue has been released");
					}
					else
					{
						  m_CurrentSliceNumber++;
					}
				}
			}
			catch(...)
			{
				  FlushSlices();

				  CQLog::Write(LOG_TAG_HOST_COMM,"Unexpected error during CPFFSliceQueueThread::Execute()");
				  if (!Terminated)
					throw EQException("CPFFSliceQueueThread::Execute - unexpected error");
			}
		}

		PausePoint(0);
	}

	FlushSlices();
}
//---------------------------------------------------------------------------
void CPFFSliceQueueThread::Cancel()
{
	FlushSlices();
	Suspend();
}
//---------------------------------------------------------------------------
TSliceMessageQueue *CPFFSliceQueueThread::GetSliceMessageQueue(void)
{
  return m_SliceMessageQueue;
}

bool CPFFSliceQueueThread::GetNextFilesNamesAndValidate( TFileNamesArray BmpFileNames )
{
	QString FileName;
	bool ret_value = false;
	CAppParams *ParamMgr = CAppParams::Instance();
	
	memset(m_BmpFileNames, 0, TFileNamesArraySIZE);

	for(unsigned i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
	{
		FileName = QFormatStr("%sSlice_%04d_%d.bmp", m_Path, m_CurrentSliceNumber, ParamMgr->PFFMaterialBMPNumberMapping[i].Value());
		if (FileExists(FileName.c_str()))
		{
			ret_value |= true;
			strcpy((TBMPFileName)BmpFileNames + i*MAX_FILE_PATH, FileName.c_str());
		}
	}

	if (!ret_value)
	{
		if (!m_SliceMissingMessageShowed)
		{
			QString str;
			m_SliceMissingMessageShowed = true;
        	CQLog::Write(LOG_TAG_PFF,"BMP files for slice %d not found.",m_CurrentSliceNumber);
			str = QFormatStr("BMP files for slice %d not found.",m_CurrentSliceNumber);
			QMonitor.ErrorMessageWaitOk(str);
		}
	}
	else
	{
		m_SliceMissingMessageShowed = false;
    }

	return ret_value;
}

PBYTE CPFFSliceQueueThread::LoadImage(const QString ImageFileName, int& SliceWidth, int& SliceHeight, unsigned& BufferSize)
{
	CBMPFile BMPFile(ImageFileName);

	int LinesToSkip = 0; // No need to skip lines. Resolution in Y will not be normalized to X and will be print as intended in BMP

    BufferSize = BMPFile.GetDataSizeInBytes() / (LinesToSkip + 1);

    // The slice width is adjusted to 32 bits
    SliceWidth  = BMPFile.GetLineSizeInBytes() * 8;
	SliceHeight = BMPFile.GetHeight() / (LinesToSkip + 1);

    // Allocate slice buffer
	PBYTE Buffer = (PBYTE)m_SlicesAllocator.GetBlock(BufferSize, true);	//(PBYTE)malloc(BufferSize);

	try
	{
		BMPFile.ReadImage(Buffer,LinesToSkip); // Read file content
        BMPFile.Close();                       // Close file
    }
    catch(...)
    {
        // Free allocated memory in a case of exception
        m_SlicesAllocator.ReleaseBlock(Buffer);
        Buffer = NULL;

        // Re-throw exception
        throw;
    }

    return Buffer;
}

void CPFFSliceQueueThread::CreateSliceFromFiles(const TFileNamesArray BmpFileNames, CSlice &Slice)
{
    int      SliceWidth        = 0;
	int      SliceHeight       = 0;

	PBYTE    Buffers[NUMBER_OF_CHAMBERS] = {NULL};
	unsigned Sizes  [NUMBER_OF_CHAMBERS] = {0};
	unsigned LastOkIndex                 = 0;
	CAppParams *ParamMgr = CAppParams::Instance();

	for(unsigned i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
	{
		TBMPFileName FileName = (TBMPFileName)BmpFileNames + i*MAX_FILE_PATH;
		// Check if a file is assigned for usage
		if(0 != strcmp(FileName, ""))
		{
			if((Buffers[i] = LoadImage(FileName,SliceWidth,SliceHeight,Sizes[i])) != NULL)
				LastOkIndex = i;
		}
	}

	// Check if all files have been read
	for(unsigned i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
	{
		if(i == LastOkIndex) continue;
		if(Buffers[i] == NULL)
		{
			Sizes[i]   = Sizes[LastOkIndex];
			Buffers[i] = (PBYTE)m_SlicesAllocator.GetBlock(Sizes[LastOkIndex], true); //(PBYTE)malloc(Sizes[LastOkIndex]);
			memset(Buffers[i],0,Sizes[LastOkIndex]);
        }
	}

	for(int i = FIRST_CHAMBER_TYPE+1; i < LAST_CHAMBER_TYPE; i++)
	{
		if(Sizes[i-1] != Sizes[i])
        {
			for(unsigned j = FIRST_CHAMBER_TYPE; j < LAST_CHAMBER_TYPE; ++j)
				m_SlicesAllocator.ReleaseBlock(Buffers[i]);	//Q_SAFE_DELETE(Buffers[j]); //memory cleanup

			throw EPFFPrintJob("Files differ in size");
		}
	}

	int BitmapResolution = 600; // Bitmap resolution is always 600.
	// Initialize the slice
	Slice.Init(Buffers,m_CurrentSliceNumber,SliceWidth,SliceHeight,ParamMgr->LayerHeightDPI_um/1000.0f,0,0,BitmapResolution);
}

bool  CPFFSliceQueueThread::ValidatePFFJobPramas()
{
	int res;
	QString path;
	int QualityMode;
	int OperationMode;
	CAppParams *ParamMgr = CAppParams::Instance();
	CBackEndInterface *BackEnd = CBackEndInterface::Instance();

	path = QFormatStr("%s%s",m_Path,"pff_params.cfg");

	if (!FileExists(path.c_str()))
	{
		QString str;
		CQLog::Write(LOG_TAG_PFF,"PFF parameters file not found (%s)",path);
		str = QFormatStr("PFF parameters file not found (%s)",path);
		FrontEndInterface->ErrorMessage(str);
		return false;
	}

	FOR_ALL_QUALITY_MODES(qm)
      FOR_ALL_OPERATION_MODES(om)
		  if (GetModeAccessibility(qm, om) == true)
		  {
			BackEnd->EnableConfirmationBypass(PRINT_MODE[qm][om], MACHINE_QUALITY_MODES_DIR(qm,om));
			BackEnd->EnableConfirmationBypass(PER_MACHINE_MODE[qm][om], MACHINE_QUALITY_MODES_DIR(qm,om));
		  }

	BackEnd->ImportConfiguration(path);
	GetPrintingMode(ParamMgr->PFFPrintingMode, QualityMode, OperationMode);
	BackEnd->GotoDefaultMode();
	BackEnd->EnterMode(PRINT_MODE[QualityMode][OperationMode], MACHINE_QUALITY_MODES_DIR(QualityMode, OperationMode));
	BackEnd->ImportConfiguration(path);

	m_NumberOFSlices = ParamMgr->PFFNumberOfSlices;

	for(int i = 0; i < m_NumberOFSlices; i++)
	{
		if (!GetNextFilesNamesAndValidate(m_BmpFileNames))
		{
			return false;
		}

		m_CurrentSliceNumber++;
    }

	m_CurrentSliceNumber = 0;
	m_SliceMissingMessageShowed = false;
	
	return true;
}

void CPFFSliceQueueThread::GetPrintingMode(int PFFPrintingMode, int &QualityMode, int &OperationMode)
{
	switch (PFFPrintingMode)
	{
		case 0: // High Speed
		{
			QualityMode   = 0;
			OperationMode = 0;
			break;
		}
		case 1: // High Qality
		{
			QualityMode   = 1;
			OperationMode = 1;
			break;
		}
		case 2: // Mix mode
		{
			QualityMode   = 0;
			OperationMode = 1;
			break;
		}
		default: // Mix mode
		{
			QualityMode   = 0;
			OperationMode = 1;
			break;
        }
	}
}

// Dump a slice
void CPFFSliceQueueThread::DisposeSlice(CSlice *Slice)
{
  for(int i = FIRST_CHAMBER_TYPE;i < LAST_CHAMBER_TYPE; i++)
  {
		m_SlicesAllocator.ReleaseBlock(Slice->GetBuffer((TChamberIndex)i));	//free(Slice->GetBuffer((TChamberIndex)i));
  }

  Q_SAFE_DELETE(Slice);
}


// Delete all slices currently in the slice queue
void CPFFSliceQueueThread::FlushSlices(void)
{
  CQLog::Write(LOG_TAG_PFF,"SlicePreProcessor: Disposing slices");
  m_SliceMessageQueue->UnRelease();
  while(m_SliceMessageQueue->ItemsCount > 0)
  {
    CSlice *TmpPtr;
    if (m_SliceMessageQueue->Receive(TmpPtr) != QLib::wrReleased)
	   DisposeSlice(TmpPtr);
    else
	   break;
  }

  m_SlicesAllocator.ReleaseBlocks();
}


