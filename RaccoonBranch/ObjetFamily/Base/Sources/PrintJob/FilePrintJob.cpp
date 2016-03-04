/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: File print job.                                          *
 * Module Description: Implement CPrintJob for a windows BMP file.  *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 29/08/2001                                           *
 * Last upate: 02/06/2003                                           *
 ********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include "QUtils.h"
#include "FilePrintJob.h"
#include "Slice.h"
#include "BMPFile.h"


// Constructor
CFilePrintJobBase::CFilePrintJobBase(QString         JobName,
                                     TFileNamesArray FilesToUse,
                                     int                   SlicesNum,
                                     float                 SliceHeightZ,
                                     int                   BitmapResolution,
                                     int                   QualityMode,
                                     int                   OperationMode,
                                     int                   PrintYResolution) :
// Set base class variables
CPrintJob(SlicesNum,SliceHeightZ * SlicesNum,QualityMode,OperationMode,
// PrintYResolutionFactor always assumes 300 DPI printing for file print jobs.
1,
JobName,
// Make up a JobId to pass to CPrintJob's c'tor:
9999)
{
  m_CurrentSlice             = 0;
  m_SliceHeightZ             = SliceHeightZ;
  m_BitmapResolution         = BitmapResolution;
  m_PrintYResolution         = PrintYResolution;
}

CFilePrintJobBase::CFilePrintJobBase()
{
}

void CFilePrintJobBase::Init(QString         JobName,
                                     TFileNamesArray FilesToUse,
                                     int                   SlicesNum,
                                     float                 SliceHeightZ,
                                     int                   BitmapResolution,
                                     int                   QualityMode,
                                     int                   OperationMode,
                                     int                   PrintYResolution)
{
	CPrintJob::Init(SlicesNum,SliceHeightZ * SlicesNum,QualityMode,OperationMode,1,JobName,9999);
	m_CurrentSlice             = 0;
	m_SliceHeightZ             = SliceHeightZ;
	m_BitmapResolution         = BitmapResolution;
	m_PrintYResolution         = PrintYResolution;

}

PBYTE CFilePrintJobBase::LoadAndScaleImage(const QString ImageFileName,int& SliceWidth,int& SliceHeight,unsigned& BufferSize)
{
  CBMPFile BMPFile(ImageFileName);

  int LinesToSkip;

  // Calculate the number of lines to skip, use the assumption that the bitmap resolution
  // equals to the X print resolution.
  switch(m_BitmapResolution)
  {
    case 600:  LinesToSkip = 1; break;
    case 1200: LinesToSkip = 3; break;
    default:   LinesToSkip = 0;
  }

  if(m_PrintYResolution == 600)
    LinesToSkip = std::max(0,LinesToSkip - 1);

  BufferSize = BMPFile.GetDataSizeInBytes() / (LinesToSkip + 1);

  // The slice width is adjusted to 32 bits 
  SliceWidth = BMPFile.GetLineSizeInBytes() * 8;
  SliceHeight = BMPFile.GetHeight() / (LinesToSkip + 1);

  // Aloocate slice buffer
  PBYTE Buffer = (PBYTE)malloc(BufferSize);

  try
  {
    // Read file content
    BMPFile.ReadImage(Buffer,LinesToSkip);

    // Close file
    BMPFile.Close();

  } catch(...)
    {
      // Free allocated memory in a case of exception
      free(Buffer);
      Buffer = NULL;

      // Re-throw exception
      throw; 
    }

  return Buffer;
}

void CFilePrintJobBase::CreateSliceFromFiles(const TFileNamesArray FilesToUse, CSlice &Slice)
{
  int      SliceWidth        = 0;
  int      SliceHeight       = 0;

  PBYTE    Buffers[NUMBER_OF_CHAMBERS] = {NULL};
  unsigned Sizes  [NUMBER_OF_CHAMBERS] = {0};
  unsigned LastOkIndex                 = 0;

  for(unsigned i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
  {
      TBMPFileName FileName = (TBMPFileName)FilesToUse + i*MAX_FILE_PATH;
      // Check if a file is assigned for usage
      if(0 != strcmp(FileName, ""))
      {
         if((Buffers[i] = LoadAndScaleImage(FileName,SliceWidth,SliceHeight,Sizes[i])) != NULL)
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
         Buffers[i] = (PBYTE)malloc(Sizes[LastOkIndex]);
         memset(Buffers[i],0,Sizes[LastOkIndex]);
      }
  }

  for(int i = FIRST_CHAMBER_TYPE+1; i < LAST_CHAMBER_TYPE; i++)
  {
     if(Sizes[i-1] != Sizes[i])
       throw EFilePrintJob("Files differ in size");
  }

  // Initialize the slice
  Slice.Init(Buffers,m_CurrentSlice,SliceWidth,SliceHeight,m_SliceHeightZ,0,0,m_BitmapResolution);
}
// Notify that we are done with a specific slice
void CFilePrintJobBase::DoneWithSlice(CSlice *Slice)
{}

bool CFilePrintJobBase::IsContinue(void)
{
  // A file print job is never recovered...
  return false;
}

CFilePrintJob::CFilePrintJob(QString         JobName,
                             TFileNamesArray FilesToUse,
                             int                   SlicesNum,
                             float                 SliceHeightZ,
                             int                   BitmapResolution,
                             int                   QualityMode,
                             int                   OperationMode,
                             int                   PrintYResolution) :
CFilePrintJobBase(JobName,FilesToUse,SlicesNum,SliceHeightZ,BitmapResolution,QualityMode,OperationMode,PrintYResolution)
{
  CreateSliceFromFiles(FilesToUse, m_Slice);
}

void CFilePrintJob::Init(    QString         JobName,
                             TFileNamesArray FilesToUse,
                             int                   SlicesNum,
                             float                 SliceHeightZ,
                             int                   BitmapResolution,
                             int                   QualityMode,
                             int                   OperationMode,
                             int                   PrintYResolution)
{
	CFilePrintJobBase::Init(JobName,FilesToUse,SlicesNum,SliceHeightZ,BitmapResolution,QualityMode,OperationMode,PrintYResolution);
	CreateSliceFromFiles(FilesToUse, m_Slice);
}

CFilePrintJob::CFilePrintJob()
{
}

void CFilePrintJob::DeInit()
{
	if(! m_IsInitialized)
		return;

	for(int i = FIRST_CHAMBER_TYPE+1; i < LAST_CHAMBER_TYPE; i++)
	{
		if(m_Slice.GetBuffer((TChamberIndex)i) != NULL)
			free(m_Slice.GetBuffer((TChamberIndex)i));
	}
}

// Destructor
CFilePrintJob::~CFilePrintJob(void)
{
  if(! m_IsInitialized)
    return;

  for(int i = FIRST_CHAMBER_TYPE+1; i < LAST_CHAMBER_TYPE; i++)
  {
     if(m_Slice.GetBuffer((TChamberIndex)i) != NULL)
        free(m_Slice.GetBuffer((TChamberIndex)i));
  }
}

// Bring a single slice (override)
// If returned with NULL, no more slices are available
CSlice *CFilePrintJob::BringSlice(void)
{
  // Increment only the slice number
  m_Slice.SetSliceNumber(m_CurrentSlice);

  if(m_CurrentSlice++ == GetTotalNumberOfSlices())
    return (CSlice *)LAST_SLICE_MARKER;

  return &m_Slice;
}

CDoubleFilePrintJob::CDoubleFilePrintJob(QString         JobName,
                                         TFileNamesArray FilesToUse1,
                                         TFileNamesArray FilesToUse2,
                                         int                   SlicesNum1,
                                         int                   SlicesNum2,
                                         float                 SliceHeightZ,
                                         int                   BitmapResolution,
                                         int                   QualityMode,
                                         int                   OperationMode,
                                         int                   PrintYResolution) :
CFilePrintJobBase(JobName,FilesToUse1,SlicesNum1+SlicesNum2,SliceHeightZ,BitmapResolution,QualityMode,OperationMode,PrintYResolution)
{
  m_FirstBMPSliceNumber = SlicesNum1;
  CreateSliceFromFiles(FilesToUse1, m_Slice1);
  CreateSliceFromFiles(FilesToUse2, m_Slice2);
}

void CDoubleFilePrintJob::Init(QString   JobName,
							 TFileNamesArray FilesToUse1,
							 TFileNamesArray FilesToUse2,
							 int                   SlicesNum1,
							 int                   SlicesNum2,
							 float                 SliceHeightZ,
							 int                   BitmapResolution,
							 int                   QualityMode,
							 int                   OperationMode,
							 int                   PrintYResolution)
{
	CFilePrintJobBase::Init(JobName,FilesToUse1,SlicesNum1+SlicesNum2,SliceHeightZ,BitmapResolution,QualityMode,OperationMode,PrintYResolution);
	m_FirstBMPSliceNumber = SlicesNum1;
	CreateSliceFromFiles(FilesToUse1, m_Slice1);
	CreateSliceFromFiles(FilesToUse2, m_Slice2);
}

void CDoubleFilePrintJob::DeInit()
{
	if(! m_IsInitialized)
		return;
    
	for(int i = FIRST_CHAMBER_TYPE+1; i < LAST_CHAMBER_TYPE; i++)
	{
		if(m_Slice1.GetBuffer((TChamberIndex)i) != NULL)
			free(m_Slice1.GetBuffer((TChamberIndex)i));
		if(m_Slice2.GetBuffer((TChamberIndex)i) != NULL)
			free(m_Slice2.GetBuffer((TChamberIndex)i));
	}
}

CDoubleFilePrintJob::CDoubleFilePrintJob()
{
}

// Destructor
CDoubleFilePrintJob::~CDoubleFilePrintJob(void)
{
  if(! m_IsInitialized)
    return;
    
  for(int i = FIRST_CHAMBER_TYPE+1; i < LAST_CHAMBER_TYPE; i++)
  {
     if(m_Slice1.GetBuffer((TChamberIndex)i) != NULL)
        free(m_Slice1.GetBuffer((TChamberIndex)i));
     if(m_Slice2.GetBuffer((TChamberIndex)i) != NULL)
        free(m_Slice2.GetBuffer((TChamberIndex)i));
  }
}

// Bring a single slice (override)
// If returned with NULL, no more slices are available
CSlice* CDoubleFilePrintJob::BringSlice(void)
{
  // Increment only the slice number
  m_Slice1.SetSliceNumber(m_CurrentSlice);
  m_Slice2.SetSliceNumber(m_CurrentSlice);

  if(m_CurrentSlice++ == GetTotalNumberOfSlices())
    return (CSlice *)LAST_SLICE_MARKER;

  if(m_CurrentSlice < m_FirstBMPSliceNumber)
    return &m_Slice1;
  else
    return &m_Slice2;
}

