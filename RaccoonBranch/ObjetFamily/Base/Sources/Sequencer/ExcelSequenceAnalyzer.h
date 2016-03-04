/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT.                                                   *
 * Module: Machine sequencer.                                       *
 * Module Description: Manage a CSV file                            *
 *                                                                  *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Shahar Behagen                                           *
 * Start date: 07/06/2001                                           *
 * Last upate: --/--/----                                           *
 ********************************************************************/

#include "QFileWithCheckSum.h"
#include "QMutex.h"
#include "AppParams.h"
#include <vector>
#include <string>

#ifndef _EXCELSEQUENCEANALYZER_H_
#define _EXCELSEQUENCEANALYZER_H_

using namespace std;

typedef enum
            {
             SourceLayerProcess = 0,
             SourceSequencer =    1,
             DATA_SOURCES_SIZE =  2
            }  DATA_SOURCES;

template <class Object>
class CSimpleMatrix
{
public:
    CSimpleMatrix( int rows, int cols )
    : array( rows ),
      LAYER_PROCESS_COL(0),
      SEQUENCER_COL(1)
    {
      SourceCols[SourceLayerProcess] = LAYER_PROCESS_COL;
      SourceCols[SourceSequencer] = SEQUENCER_COL;

      for(int i=0; i < DATA_SOURCES_SIZE; i++)
       SourceRows[i] = 1;

      for(int i = 0; i < rows; i++ )
       array[ i ].resize( cols );
    }

    CSimpleMatrix( const CSimpleMatrix & rhs ) : array( rhs.array ) { }
    virtual vector<Object> & operator[]( int row )
      {
       m_Mutex.WaitFor();
       return array[ row ];
       m_Mutex.Release();
      }
    virtual int numrows( ) const
      { return array.size( ); }
    virtual int numcols( ) const
      { return numrows( ) ? array[ 0 ].size( ) : 0; }
    void SetIndex (DATA_SOURCES datasource, int index)
      {SourceRows[datasource] = index;}

    virtual int GetIndex (DATA_SOURCES datasource)
      {return SourceRows[datasource];}

    virtual void IncIndex (DATA_SOURCES datasource)
      {SourceRows[datasource]++;}
    virtual void SetValue(DATA_SOURCES datasource, string val)
    {
      int row = SourceRows[datasource];
      int col = SourceCols[datasource];

      AutoResize(row, col);
      array[row][col] = val;
    }
    virtual void SetValueNext(DATA_SOURCES datasource, string val)
    {
      SetValue(datasource, val);
      IncIndex(datasource);
    }
    virtual void SetValueNext(DATA_SOURCES datasource, long val)
    {
      char buff[256] = "";
      sprintf(buff, "%d", val);
      SetValue(datasource, buff);
      IncIndex(datasource);
    }
    virtual void SetCell(unsigned int row, unsigned int col, long val)
    {
      if (row >= numrows() || col >= numcols())
       return;

      char buff[256] = "";
      sprintf(buff, "%d", val);

      array[row][col] = buff;
    }
    virtual void SetCell(int row, int col, string val)
    {
      if (row >= numrows() || col >= numcols())
       return;

      array[row][col] = val;
    }

    virtual const vector< vector<Object> > GetRawMatrix() {return array;}
    virtual void Clear()
    {
      for(int i = 0; i < numrows(); i++ )
        array[ i ].clear();
    }

private:
    vector< vector<Object> > array;
    int SourceRows[DATA_SOURCES_SIZE];
    int SourceCols[DATA_SOURCES_SIZE];
    const int LAYER_PROCESS_COL;
    const int SEQUENCER_COL;
    CQMutex m_Mutex;
    
    virtual void AutoResize(int new_rows, int new_cols)
    {
      int curr_rows = array.size();
      int curr_cols = array[0].size();

      if (new_cols >= curr_cols)
      {
       for(int i = 0; i < curr_rows; i++ )
        array[ i ].resize(new_cols*2);

       curr_cols = new_cols*2;
      }

      if (new_rows >= curr_rows)
      {
        array.resize(new_rows*2);
        for (int i = curr_rows; i < new_rows*2; i++ )
         array[i].resize(curr_cols);
      }
    }
};

typedef CSimpleMatrix<string> TStringMatrix;
// forward declarations:
//class CExcelSequenceAnalyzer;
//class CExcelSequenceAnalyzerDummy;


class CExcelSequenceAnalyzerBase : public CQFileWithCheckSum, public TStringMatrix
{
  public:

  // Pointer to the singleton object
  static CExcelSequenceAnalyzerBase *m_InstancePtr;

  /////////////// Interface funcs. ////////////////
  virtual void Clear() = 0;
  virtual void GenerateCsv() = 0;
  virtual void WriteParameters () = 0;
  virtual void WriteDiffCol() = 0;
  static void DeInit()
  {
	if(m_InstancePtr)
		delete m_InstancePtr;
  }
  protected:

 CExcelSequenceAnalyzerBase (string fname, int rows, int cols)
  : CQFileWithCheckSum(fname) ,TStringMatrix(rows, cols)
  {
    AssignFile(fname);  
  }

  CExcelSequenceAnalyzerBase () : CQFileWithCheckSum("dummy"), TStringMatrix (1,1) {}
};
CExcelSequenceAnalyzerBase *CExcelSequenceAnalyzerBase::m_InstancePtr = 0;


////////////////////////////////////////////////////////////////
//  class CExcelSequenceAnalyzer
////////////////////////////////////////////////////////////////

class CExcelSequenceAnalyzer : public CExcelSequenceAnalyzerBase
{
public:
  // Init/DeInit the singleton object
  static void Init(string fname, int rows, int cols)
  {
    if(m_InstancePtr == 0)
    {
      m_InstancePtr = new CExcelSequenceAnalyzer(fname, rows, cols);
    }
  }

  static CExcelSequenceAnalyzerBase* Instance(void)
  {
    return m_InstancePtr;
  }

  virtual void Clear()
  {
   TStringMatrix::Clear();
  }
  virtual void GenerateCsv()
  {
   string document = "";
   const vector< vector<string> > array = GetRawMatrix();

   int rows = array.size();
   int cols = 0;

   for(int i = 0; i < rows; i++)
   {
     cols = array[i].size();
     for(int j = 0; j < cols; j++)
     {
       document += array[i][j] + ",";
     }

	 document.erase(document.end() - 1, document.end()); //erases the last ","
     document += "\r\n";  // and enter a CRLF instead.
   }

   // Write the file:
   BeginWrite();
   Write (document.c_str(), document.size());
   EndWrite();
  } //  GenerateCsv(string& document)

  virtual void CExcelSequenceAnalyzer::WriteParameters()
  {
    CAppParams *m_ParamsMgr = CAppParams::Instance();

    int i=0;
    SetCell(0,0,"BMP (px)");
    SetCell(0,1,"Motor (steps)");
    SetCell(i++, 9, "YStepsPerPixel");
    SetCell(i++, 9, m_ParamsMgr->YStepsPerPixel);
    SetCell(i++, 9, "ScatterEnabled");
    SetCell(i++, 9, m_ParamsMgr->ScatterEnabled);
    SetCell(i++, 9, "ScatterFactorMin");
    SetCell(i++, 9, m_ParamsMgr->ScatterFactorMin);
    SetCell(i++, 9, "ScatterFactorMax");
    SetCell(i++, 9, m_ParamsMgr->ScatterFactorMax);
    SetCell(i++, 9, "DPI_InYAxis");
    SetCell(i++, 9, m_ParamsMgr->DPI_InYAxis);
    SetCell(i++, 9, "Y_BacklashMove_mm");
    SetCell(i++, 9, m_ParamsMgr->Y_BacklashMove_mm);
    SetCell(i++, 9, "YInterlaceNoOfPixels");
    SetCell(i++, 9, m_ParamsMgr->YInterlaceNoOfPixels);
    SetCell(i++, 9, "YSecondaryInterlaceNoOfPixels");
    SetCell(i++, 9, m_ParamsMgr->YSecondaryInterlaceNoOfPixels);

    if (m_ParamsMgr->NozzleTest)
    {
     SetCell(i++, 9, "NozzlesTestYOffset");
     SetCell(i++, 9, m_ParamsMgr->NozzleTest);
     SetCell(i++, 9, "NozzlesTestYOffset");
     SetCell(i++, 9, m_ParamsMgr->NozzlesTestYOffset);
     SetCell(i++, 9, "NozzlesTestYCloneDistance_pixel");
     SetCell(i++, 9, m_ParamsMgr->NozzlesTestYCloneDistance_pixel);
    }
  }

  virtual void CExcelSequenceAnalyzer::WriteDiffCol()
  {
    char col = 'B';
    char buff[16] = "";
    for (int i=1; i < numrows(); i++)
    {
     sprintf(buff, "=%c%d-%c%d", col, i+1, col, i);
     SetCell(i, 3, buff);
     strcpy (buff,"");

     sprintf(buff, "=D%d/J2", i+1);
     SetCell(i, 5, buff);
     strcpy (buff,"");
    }
  }

private:
  CExcelSequenceAnalyzer (string fname, int rows, int cols)
    : CExcelSequenceAnalyzerBase(fname, rows, cols)  { }

};



class CExcelSequenceAnalyzerDummy : public CExcelSequenceAnalyzerBase
{
  public:
  // Init/DeInit the singleton object
  static void Init()
  {
    if(m_InstancePtr == 0)
    {
      m_InstancePtr = new CExcelSequenceAnalyzerDummy();
    }
  }

  static CExcelSequenceAnalyzerBase* Instance(void)
  {
    return m_InstancePtr;
  }

  virtual void Clear()
  {
   int j=0;
  }
  virtual void GenerateCsv()
  {
   int j=0;
  }
  virtual void WriteParameters()
  {
   int j=0;
  }
  virtual void WriteDiffCol()
  {
   int j=0;
  }

  // overrides of SimpleMatrix
  virtual void SetValue(DATA_SOURCES datasource, string val)
  {
   int j=0;
  }
  virtual void SetValueNext(DATA_SOURCES datasource, string val)
  {
   int j=0;
  }
  virtual void SetValueNext(DATA_SOURCES datasource, long val)
  {
   int j=0;
  }
  virtual void SetCell(unsigned int row, unsigned int col, long val)
  {
   int j=0;
  }
  virtual void SetCell(int row, int col, string val)
  {
   int j=0;
  }




  private:
  CExcelSequenceAnalyzerDummy () : CExcelSequenceAnalyzerBase () {}
};

#endif



