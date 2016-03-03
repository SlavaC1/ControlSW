//---------------------------------------------------------------------------


#pragma hdrstop

#include "PFFPrintJob.h"
#include "PFFSliceQueue.h"

//---------------------------------------------------------------------------
// Constructor
CPFFPrintJob::CPFFPrintJob()
{
// m_Continued = false;
   m_SliceMessageQueue = NULL;
   m_PFFSliceQueueThreadPtr  = NULL;
   m_CurrentSliceNumber = 0;
}

// Destructor
CPFFPrintJob::~CPFFPrintJob(void)
{
}

void CPFFPrintJob::Init(CPFFSliceQueueThread* PFFSliceQueueThread)
{
	int QualityMode;
	int OperationMode;
	m_CurrentSliceNumber = 0;
	CAppParams *ParamMgr = CAppParams::Instance();
	m_PFFSliceQueueThreadPtr  = PFFSliceQueueThread;
	m_SliceMessageQueue = m_PFFSliceQueueThreadPtr->GetSliceMessageQueue();
	m_PFFSliceQueueThreadPtr->GetPrintingMode(ParamMgr->PFFPrintingMode, QualityMode, OperationMode);

	CPrintJob::Init(	ParamMgr->PFFNumberOfSlices,
						(float)(ParamMgr->PFFNumberOfSlices)*ParamMgr->GetLayerHeight_um()/1000.0f,
						QualityMode,
						OperationMode,
						1,
						"PFFJob",
						666);
}

// Bring a single slice (override)
// If returned with false, no more slices are available
CSlice *CPFFPrintJob::BringSlice(void)
{
  CSlice *Slice;
  CAppParams *ParamMgr = CAppParams::Instance();
  
  if(m_CurrentSliceNumber == ParamMgr->PFFNumberOfSlices)
  {
  		m_CurrentSliceNumber = 0;
		return (CSlice *)LAST_SLICE_MARKER;
  }

  if(m_SliceMessageQueue->Receive(Slice) == QLib::wrReleased)
	Slice = NULL;

  m_CurrentSliceNumber++;

  return Slice;
}

// Notify that we are done with a specific slice
void CPFFPrintJob::DoneWithSlice(CSlice *Slice)
{
  // Invalidate and get rid from the slice
  m_PFFSliceQueueThreadPtr->DisposeSlice(Slice);
}




#pragma package(smart_init)
