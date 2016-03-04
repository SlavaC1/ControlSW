/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: File print job.                                          *
 * Module Description: Implement print job for host link.           *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 29/08/2001                                           *
 * Last upate: 02/06/2003                                           *
 ********************************************************************/

#include "HostPrintJob.h"
#include "Slice.h"
#include "HostComm.h"


// Constructor
CHostPrintJob::CHostPrintJob(CHostComm*        HostCommPtr,
                             int               TotalNumberOfSlices,
                             float             TotalHeightInMM,
                             int               QualityMode,
                             int               OperationMode,
                             int               YResolutionFactor,
                             bool              Continued,
                             QString           JobName,
                             ULONG             JobId)
  : CPrintJob(TotalNumberOfSlices,TotalHeightInMM,QualityMode,OperationMode,YResolutionFactor,JobName,JobId)

{
  m_Continued = Continued;
  m_HostCommPtr = HostCommPtr;
  m_SliceMessageQueue = m_HostCommPtr->GetSliceMessageQueue();
}

CHostPrintJob::CHostPrintJob()
{
}

// Destructor
CHostPrintJob::~CHostPrintJob(void)
{
}

// Bring a single slice (override)
// If returned with false, no more slices are available
CSlice *CHostPrintJob::BringSlice(void)
{
  CSlice *TmpSlice;

  if(m_SliceMessageQueue->Receive(TmpSlice) == QLib::wrReleased)
    TmpSlice = NULL;

  return TmpSlice;
}

// Notify that we are done with a specific slice
void CHostPrintJob::DoneWithSlice(CSlice *Slice)
{
  // Invalidate and get rid from the slice
  m_HostCommPtr->DisposeSlice(Slice);
}

void CHostPrintJob::Init(CHostComm *HostCommPtr,int TotalNumberOfSlices,float TotalHeightInMM,
                  int QualityMode,int OperationMode,int YResolutionFactor,bool Continued,QString JobName, ULONG JobId)
{
	CPrintJob::Init(TotalNumberOfSlices,TotalHeightInMM,QualityMode,OperationMode,YResolutionFactor,JobName,JobId);
	m_Continued = Continued;
	m_HostCommPtr = HostCommPtr;
	m_SliceMessageQueue = m_HostCommPtr->GetSliceMessageQueue();	
}

