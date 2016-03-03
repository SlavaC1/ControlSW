//---------------------------------------------------------------------------

#ifndef PFFSliceQueueH
#define PFFSliceQueueH
//---------------------------------------------------------------------------
#include <stdio.h>
//#include <unistd.h>
#include <sys/stat.h>
#include "QMessageQueue.h"
#include "QThread.h"
#include "GlobalDefs.h"
#include "Slice.h"
#include "BMPFile.h"
#include "PrintJob.h"
#include "PFFPrintJob.h"
#include "AppLogFile.h"
#include "Q2RTApplication.h"
#include "BlocksAllocator.h"
#include "FrontEnd.h"
#include "AppParams.h"
#include "PFFPrintJob.h"

//---------------------------------------------------------------------------
//class CSlice;

const int PFF_SLICE_QUEUE_SIZE = 3;
const int PFF_BMP_CREATION_QUEUE_SIZE = (PFF_SLICE_QUEUE_SIZE + 2);

typedef CQMessageQueue<CSlice *> TSliceMessageQueue;




class CPFFSliceQueueThread : public CQThread
{
private:

	char m_Path[256];
	int m_CurrentSliceNumber;
	int m_NumberOFSlices;
	bool m_SliceMissingMessageShowed;

	// Message queue for the ready slices
    // Note: This queue object is created by this (CSlicePreProcessor) class.
	TSliceMessageQueue *m_SliceMessageQueue;
	// Memory manager for slices
	CBlocksAllocator m_SlicesAllocator;

	TBMPFileName m_BmpFileNames[TFileNamesArraySIZE];

	bool GetNextFilesNamesAndValidate( TFileNamesArray BmpFileNames );

	PBYTE LoadImage(const QString ImageFileName, int& SliceWidth, int& SliceHeight, unsigned& BufferSize);
	void  CreateSliceFromFiles(const TFileNamesArray BmpFileNames, CSlice &Slice);

	void FlushSlices(void);

	void Execute();
	
public:
	CPFFSliceQueueThread();
	~CPFFSliceQueueThread();
	void Init(QString Path);
	void Cancel();
	TSliceMessageQueue* GetSliceMessageQueue(void);
	bool  ValidatePFFJobPramas();
	void DisposeSlice(CSlice *Slice);
	void GetPrintingMode(int PFFPrintingMode, int &QualityMode, int &OperationMode);
};
//---------------------------------------------------------------------------
#endif
