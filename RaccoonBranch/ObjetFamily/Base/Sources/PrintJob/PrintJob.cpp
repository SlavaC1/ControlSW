/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Print job.                                               *
 * Module Description: Encapsulate all the information and actions  *
 *                     required for a single print job.             *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 29/08/2001                                           *
 * Last upate: 02/06/2003                                           *
 ********************************************************************/

#include "PrintJob.h"
#include "GlobalDefs.h"

// Constructor
CPrintJob::CPrintJob(int               TotalNumberOfSlices,
                     float             TotalHeightInMM,
                     int               QualityMode,
                     int               OperationMode,
                     int               YResolutionFactor,
                     QString           JobName,
                     ULONG             JobId)
{
  // Initilaize member parameters
  m_TotalNumberOfSlices = TotalNumberOfSlices;
  m_TotalHeightInMM     = TotalHeightInMM;
  m_PausedByHost        = false;
  m_QualityMode         = QualityMode;
  m_OperationMode       = OperationMode;  
  m_YResolutionFactor   = YResolutionFactor;
  m_JobName             = JobName;
  m_JobId               = JobId;
}

CPrintJob::CPrintJob()
{
    m_IsInitialized = false;
}

void CPrintJob::Init(int               TotalNumberOfSlices,
                     float             TotalHeightInMM,
                     int               QualityMode,
                     int               OperationMode,
                     int               YResolutionFactor,
                     QString           JobName,
                     ULONG             JobId)
{
  // Initilaize member parameters
  m_TotalNumberOfSlices = TotalNumberOfSlices;
  m_TotalHeightInMM     = TotalHeightInMM;
  m_PausedByHost        = false;
  m_QualityMode         = QualityMode;
  m_OperationMode       = OperationMode;  
  m_YResolutionFactor   = YResolutionFactor;
  m_JobName             = JobName;
  m_JobId               = JobId;
  m_IsInitialized       = true;
}

// Destructor
CPrintJob::~CPrintJob(void)
{}

//Return true if it was stopped by embedded.
bool CPrintJob::IsPrintPausedByHost (void)
{
   return m_PausedByHost;
}

void CPrintJob::UpdatePrintPausedByHost (bool PausedByHost)
{
   m_PausedByHost = PausedByHost;
}

QString CPrintJob::GetJobName(void)
{
   return m_JobName;
}

ULONG CPrintJob::GetJobId(void)
{
  return m_JobId;
}

int CPrintJob::GetTotalNumberOfSlices(void)
{
  return m_TotalNumberOfSlices;
}

float CPrintJob::GetTotalHeightInMM(void)
{
  return m_TotalHeightInMM;
}

int CPrintJob::GetQualityMode(void)
{
  return m_QualityMode;
}

int CPrintJob::GetOperationMode(void)
{
  return m_OperationMode;
}

int CPrintJob::GetYResolutionFactor(void)
{
  return m_YResolutionFactor;
}

