/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: File print job.                                          *
 * Module Description: Implement CPrintJob for a disk file.         *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 29/08/2001                                           *
 * Last upate: 02/06/2003                                           *
 ********************************************************************/

#ifndef _FILE_PRINT_JOB_H_
#define _FILE_PRINT_JOB_H_

#include "PrintJob.h"
#include "Slice.h"



// Exception class for the file print job
class EFilePrintJob : public EPrintJob {
  public:
    EFilePrintJob(const QString& ErrMsg,const TQErrCode ErrCode=0) : EPrintJob(ErrMsg,ErrCode) {}
};


class CFilePrintJobBase : public CPrintJob
{

protected:
    // Current slice index
    int   m_CurrentSlice;
    int   m_BitmapResolution;
    int   m_PrintYResolution;
    int   m_LayersNum;
    float m_SliceHeightZ;

protected:


    void CreateSliceFromFiles(const TFileNamesArray FilesToUse, CSlice &Slice);
    PBYTE LoadAndScaleImage(const QString ImageFileName,int& SliceWidth,int& SliceHeight,unsigned& BufferSize);

  public:
	CFilePrintJobBase();
    // Constructor	
    CFilePrintJobBase(QString  JobName,TFileNamesArray FilesToUse,int SlicesNum,
                      float SliceHeightZ,int BitmapResolution,int QualityMode,int OperationMode,int PrintYResolution);

    // Destructor
    virtual ~CFilePrintJobBase(void){};
    // Bring a single slice (override)
    // If returned with false, no more slices are available
    virtual CSlice *BringSlice(void) = 0;
    // Return true if the job is a continue (recovered) job (override)
    bool IsContinue(void);
    // Notify that we are done with a specific slice
    void DoneWithSlice(CSlice *Slice);
	void Init(QString  JobName,TFileNamesArray FilesToUse,int SlicesNum,
                      float SliceHeightZ,int BitmapResolution,int QualityMode,int OperationMode,int PrintYResolution);

};

class CFilePrintJob : public CFilePrintJobBase
{
private:
    // A single slice object is used as the actual placeholder of slice data
    CSlice m_Slice;

public:
	CFilePrintJob();
    CFilePrintJob(QString  JobName, TFileNamesArray FilesToUse,int SlicesNum,
                  float SliceHeightZ,int BitmapResolution,int QualityMode,int OperationMode,int PrintYResolution);
    virtual ~CFilePrintJob(void);
    virtual CSlice *BringSlice(void);
	void Init(QString  JobName, TFileNamesArray FilesToUse,int SlicesNum,
                  float SliceHeightZ,int BitmapResolution,int QualityMode,int OperationMode,int PrintYResolution);
	void DeInit();
};

class CDoubleFilePrintJob : public CFilePrintJobBase
{
private:
    int m_FirstBMPSliceNumber;

    // A single slice object is used as the actual placeholder of slice data
    CSlice m_Slice1;
    CSlice m_Slice2;

public:
	CDoubleFilePrintJob();
    CDoubleFilePrintJob(QString  JobName,TFileNamesArray FilesToUse1,TFileNamesArray FilesToUse2,int SlicesNum1,int SlicesNum2,
                  float SliceHeightZ,int BitmapResolution,int QualityMode,int OperationMode,int PrintYResolution);

    virtual ~CDoubleFilePrintJob(void);
	
	void Init(QString  JobName,TFileNamesArray FilesToUse1,TFileNamesArray FilesToUse2,int SlicesNum1,int SlicesNum2,
                  float SliceHeightZ,int BitmapResolution,int QualityMode,int OperationMode,int PrintYResolution);

	void DeInit();
    // Bring a single slice (override)
    // If returned with false, no more slices are available
    virtual CSlice *BringSlice(void);
};

#endif

