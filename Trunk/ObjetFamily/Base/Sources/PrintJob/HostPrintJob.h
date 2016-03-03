/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: File print job.                                          *
 * Module Description: Implement CPrintJob for a host link.         *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 29/08/2001                                           *
 * Last upate: 02/06/2003                                           *
 ********************************************************************/

#ifndef _HOST_PRINT_JOB_H_
#define _HOST_PRINT_JOB_H_

#include "PrintJob.h"
#include "QMessageQueue.h"


class CSlice;
class CHostComm;
typedef CQMessageQueue<CSlice *> TSliceMessageQueue;


class CHostPrintJob : public CPrintJob {
  private:
    TSliceMessageQueue* m_SliceMessageQueue;
    bool                m_Continued;
    CHostComm*          m_HostCommPtr;

  public:
    // Constructor
	CHostPrintJob();
    CHostPrintJob(CHostComm *HostCommPtr,int TotalNumberOfSlices,float TotalHeightInMM,
                  int QualityMode,int OperationMode,int YResolutionFactor,bool Continued,QString JobName, ULONG JobId);

    // Destructor
    ~CHostPrintJob(void);
	
	void Init(CHostComm *HostCommPtr,int TotalNumberOfSlices,float TotalHeightInMM,
                  int QualityMode,int OperationMode,int YResolutionFactor,bool Continued,QString JobName, ULONG JobId);
    
    // Bring a single slice (override)
    // If returned with false, no more slices are available
    CSlice *BringSlice(void);

    // Return true if the job is a continue (recovered) job (override)
    bool IsContinue(void) {
      return m_Continued;
    }

    // Notify that we are done with a specific slice
    void DoneWithSlice(CSlice *Slice);

};

#endif

