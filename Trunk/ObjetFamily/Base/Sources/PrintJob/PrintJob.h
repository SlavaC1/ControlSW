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

#ifndef _PRINT_JOB_H_
#define _PRINT_JOB_H_

#include "QObject.h"
#include "QException.h"


const int LAST_SLICE_MARKER = 1;

class CSlice;


// Exception class for the print job classes
class EPrintJob : public EQException {
  public:
    EPrintJob(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

// Print job base class (abstract)
class CPrintJob : public CQObject
{
private:
    // Print job parameters
    int     m_TotalNumberOfSlices;
    float   m_TotalHeightInMM;
    int     m_QualityMode;
    int     m_OperationMode;
    int     m_YResolutionFactor;
    bool    m_PausedByHost;
    QString m_JobName;
    ULONG   m_JobId;

protected:	
    bool    m_IsInitialized;

public:
	CPrintJob();
    // Constructor
    CPrintJob(int TotalNumberOfSlices,
              float TotalHeightInMM,
              int QualityMode,
              int OperationMode,
              int YResolutionFactor,
              QString JobName,
              ULONG JobId);
    // Destructor
    ~CPrintJob(void);

    // Bring a single slice
    // If returned with false, no more slices are available
    virtual CSlice *BringSlice(void) = 0;

    // Notify that we are done with a specific slice
    virtual void DoneWithSlice(CSlice *Slice) = 0;

    // Return true if the job is a continue (recovered) job
    virtual bool IsContinue(void) = 0;

    QString GetJobName(void);
    ULONG GetJobId(void);

    //Return true if it was stopped by embedded.
    bool IsPrintPausedByHost (void);
    void UpdatePrintPausedByHost (bool);

    int GetTotalNumberOfSlices(void);
    float GetTotalHeightInMM(void);

    int GetQualityMode(void);
    int GetOperationMode(void);
    int GetYResolutionFactor(void);
	
	void Init(int TotalNumberOfSlices,
              float TotalHeightInMM,
              int QualityMode,
              int OperationMode,
              int YResolutionFactor,
              QString JobName,
              ULONG JobId);
};

#endif

