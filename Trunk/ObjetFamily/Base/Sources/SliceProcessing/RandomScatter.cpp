/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Scatter handling object.                                 *
 * Module Description:                                              *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Rachel / Ran                                             *
 * Start date: 08/01/2002                                           *
 * Last upate: 26/10/2003                                           *
 ********************************************************************/

#include <cmath>
#include "RandomScatter.h"
#include "AppParams.h"
#include "LayerProcessDefs.h"
#include "AppLogFile.h"
#include "MotorDefs.h"
#include "configuration.h"
#include "QMonitor.h"
#include "Q2RTApplication.h"

// Folowing g_Nozzles... are declared in LayerProcess.cpp
// Number of nozzles in all printing heads
extern int g_NozzlesInAllHeads; // 384; 196;
// The distance between two sequential nozzles in pixels.
extern int g_NozzlesGapInPixels;
#define SCATTER_NOZZLE_FILE (Q2RTApplication->AppFilePath.Value()+"\\"+ "Scatter_Nozzle.txt")

extern "C" __declspec(dllimport) unsigned __stdcall CalcNumOfPasses(int SliceHeight,int SliceSizeType,int SliceYOffset,int s_HeadPrintWidth,int ScatterPlusEvenOdd,int CoveredHeight);
extern "C" __declspec(dllimport) unsigned __stdcall CalcYScatterOffset(int SliceHeight,int SliceSizeType,int SliceYOffset,int s_HeadPrintWidth, int g_NozzlesGapInPixels,
																	   int NoOfPasses,int ScatterPlusEvenOdd, int YInterlaceNoOfPixels, int YSecondaryInterlaceNoOfPixels);

// Constructor
CRandomScatter::CRandomScatter( ) : CScatterGenerator()
{

   memset( m_ScatterTable2, 0, sizeof(m_ScatterTable2));
   memset( m_ScatterTable1, 0, sizeof(m_ScatterTable1));
   memset( m_SortedScatterTable, 0, sizeof( m_SortedScatterTable));
   memset( &m_FirstSliceSizeType, 0, sizeof(TSliceSizeType));
   m_CurrentScatterIndex = 0;
   m_PreviousFromTable2 = false;
   m_YHChanged = false;
   m_FirstSliceTrayYOffset = 0;
   m_PreviousH = 0;
   m_PreviousY = 0;
  m_ScatterTableRefreshCounter = 0;
  m_FirstSlice = true;
  m_uniqueScatterValuesNum = 0;
  m_dontShuffle = false;
}

// Destructor
CRandomScatter::~CRandomScatter(void)
{
}

// Compute new scatter table
void CRandomScatter::ScatterCompute(int MaxHeight,int SliceYOffset,bool ResumedJob)
{
  TSliceSizeType SliceSizeType;

  m_PreviousH = MaxHeight;  // MaxHeight is the slice height in pure pixels.
  m_PreviousY = SliceYOffset; // SliceYOffset is the offset of the BMP in pure pixels from (0,0).

  CAppParams *ParamsMgr = CAppParams::Instance();

  int ScatterFactor;


  // Decrease Delta due to interlace:
  // Accordint to Eq. 1.9

  // convert to nozzles:

  // Delta: "spare" y height left from bitmap height modulo head height, this is the largest scatter we may use
  // without causing an additional pass.
  int Delta = GetDefaultScatter(MaxHeight);

  // Get min/max scatter values from parameter manager.
  int ScatterFactorMin = ParamsMgr->ScatterFactorMin;
  int ScatterFactorMax = ParamsMgr->ScatterFactorMax;

   // if the bitmap itself allows a larger scatter then requested in scatterMin, use it instead of scatterMin.
  if(Delta >= ScatterFactorMin)
    ScatterFactor = Delta;
  else
  {
    double DeltaPower = pow(Delta,0.1);
    double ScatterFactorMinPower = pow(ScatterFactorMin,0.1);

    ScatterFactor = QSimpleRound((ScatterFactorMin * DeltaPower + g_NozzlesInAllHeads *
                                (ScatterFactorMinPower - DeltaPower)) / ScatterFactorMinPower);
  }

  int LowestScatter,HighestScatter;

  // Clip to scatter factor max
  ScatterFactor = min(ScatterFactor,ScatterFactorMax);

  if(m_FirstSlice)
  {
    // Get LowestScatter and HighestScatter in terms of nozzles:
    m_FirstSliceSizeType = SliceSizeType = CalcTrayOffset(MaxHeight,ResumedJob,LowestScatter,HighestScatter);
    m_FirstSliceTrayYOffset = CScatterGenerator::GetTrayYOffset();
    m_FirstSlice = false;
  } else
      // "CalcTrayOffsetNext" is dependent on the first slice size type
      SliceSizeType = CalcTrayOffsetNext(MaxHeight,SliceYOffset,LowestScatter,HighestScatter);

  // ??? add a check that scattermin/max is never > head_width, and if so alert to log file.
  if (LowestScatter > g_NozzlesInAllHeads || HighestScatter > g_NozzlesInAllHeads)
  {
   CQLog::Write(LOG_TAG_PROCESS,"Error in scatter max or min computation: (LowestScatter=%d, HighestScatter=%d) continuing with default values.",
               LowestScatter,HighestScatter);
   LowestScatter = 0;
   HighestScatter = g_NozzlesInAllHeads;
  }

  // Convert the scatter limitations from nozzles to Y DPI related pixels
/* ??? delete this
  LowestScatter = ConvertNozzleToYDPI(LowestScatter);
  HighestScatter = ConvertNozzleToYDPI(HighestScatter);
*/

  int ScatterFactorInPx = ScatterFactor * g_NozzlesGapInPixels;

  // Clip the scatter factor according to the tray limitation
    // in case of a medium and large models, ignore ScatterFactor optimization, and use the maximal
    // scatter span possible (dictated by tray limits only)
  if (SliceSizeType == sstMedium || SliceSizeType == sstLargeOnRelativeSmallTray)
    ScatterFactorInPx = min (ScatterFactorInPx, HighestScatter * g_NozzlesGapInPixels);
  else if (SliceSizeType == sstLarge)
    ScatterFactorInPx = HighestScatter * g_NozzlesGapInPixels;


  int ScatterCounter = LowestScatter * g_NozzlesGapInPixels;

  // Init table 1 (even or odd)
  int ScatterResolution = 1;
  if (ParamsMgr->TwoNozzle_ScatterResolution == true)
    ScatterResolution = 2;

  // Check and inform on possible mismatch in parameters:
  if (ScatterCounter > ScatterFactorInPx)
  {
	CQLog::Write(LOG_TAG_PROCESS,"Error: A Possible scattering problem was detected. Y - MaxPositionStep,TrayStartPositionY,YStepsPerPixel,StepsPerMM should be calibrated properly.");
  }
  
  m_uniqueScatterValuesNum = 0;
 //  nozzle scatter  from file
  QString   FileName  = SCATTER_NOZZLE_FILE;
  FILE      *f;
  DWORD     read=0;
  int       locInTable=0;
  int       lowLimit = LowestScatter * g_NozzlesGapInPixels ; 
  unsigned  FileSize = 0;
  

  //----------------------------
  for(int i = 0; i < SCATTER_TABLE_SIZE; i++)
  {
    m_ScatterTable1[i] = ScatterCounter;

    if((ScatterCounter += (g_NozzlesGapInPixels*ScatterResolution)) >= ScatterFactorInPx)
    {
      ScatterCounter = LowestScatter * g_NozzlesGapInPixels;

      if (!m_uniqueScatterValuesNum)
        m_uniqueScatterValuesNum = i + 1;
    }
  }

  KeepSortedScatterTable();

  // Random shuffle of both tables (m_dontShuffle is used for testing)
  if (m_dontShuffle)
	m_dontShuffle = false;
  else
	ShuffleTable(m_ScatterTable1,SCATTER_TABLE_SIZE);


   //check from Alex L file
  if( ParamsMgr->ReadFromFileNozzleScattering )
  {
	bool noSuitableValuesInFile = false;
	f = fopen(FileName.c_str(),"r");
	//write to the log:
	CQLog::Write(LOG_TAG_PROCESS,"Nozzle_Scatter File: Min value %d; Max value %d; Scatter table size %d.",
	lowLimit,ScatterFactorInPx,SCATTER_TABLE_SIZE);
	locInTable = 0 ;
	//run till the table is full from files values (or no suitable values at all in the file)
	while ( (locInTable < SCATTER_TABLE_SIZE)  && (noSuitableValuesInFile == false) )
	{
	    m_ScatterTable1[locInTable] =  lowLimit - 1 ; //Init before reading from file
		fscanf (f,"%d",&(m_ScatterTable1[locInTable]));
		//value is in the correct limits
		if (m_ScatterTable1[locInTable] > lowLimit && m_ScatterTable1[locInTable] < ScatterFactorInPx )
		{
			//write to the log:
			CQLog::Write(LOG_TAG_PROCESS,"Nozzle_Scatter File:inserted value %d	in place %d .",
			   m_ScatterTable1[locInTable],locInTable);
			locInTable++ ; //table location
		}
		read++ ; //from file read
		//check here if still has values in file if not continue from the start
		if ( feof(f)  ) //finished reading file
		{
			read=0;

			//no values are suitable in the file
			if (locInTable == 0 )
			{
				fclose(f);
				noSuitableValuesInFile = true;
				QMonitor.ErrorMessage("Nozzle_Scatter File: \
				\nValues in the Nozzle_Scatter.txt file are wrong .\
				\nPlease Update file values:\
				");
				m_ScatterTable1[0] = ScatterCounter;//returned first value( the only one that is run over before for check)

			}
			//re-start reading the file (read from the start)
			else if (locInTable < SCATTER_TABLE_SIZE )
			{
				fseek(f, 0, SEEK_SET); //start file from the begining.
				//f = fopen(SCATTER_NOZZLE_FILE,"r");
			}
		}
	}
    fclose(f);
  }

  m_CurrentScatterIndex = 0;
  m_ScatterTableRefreshCounter = 0;
  m_YHChanged = false;
  m_PreviousFromTable2 = true;

  CQLog::Write(LOG_TAG_PROCESS,"Calculating random scatter table (Size type=%d, min=%d, max=%d)",
               (int)SliceSizeType,LowestScatter,(ScatterFactorInPx / g_NozzlesGapInPixels) - 1);
}

int* CRandomScatter::GetSortedScatterTable(unsigned int& TableSize)
{
  TableSize = m_uniqueScatterValuesNum;

  // If table was not yet initialized return NULL
  if (!m_uniqueScatterValuesNum)
    return NULL;

  return m_SortedScatterTable;
}

void CRandomScatter::KeepSortedScatterTable()
{
  memset (m_SortedScatterTable, 0, sizeof(m_SortedScatterTable));
  memcpy (m_SortedScatterTable, m_ScatterTable1, m_uniqueScatterValuesNum * sizeof(int));
}

// Shuffle the scatter table in random
void CRandomScatter::ShuffleTable(int *Table,int TableSize)
{


  for(int i = 0, Tmp; i < TableSize; i++)
  {
    int RandomIndex = rand() % TableSize;

    // Swap between current element and random element
    Tmp = Table[i];
    Table[i] = Table[RandomIndex];
    Table[RandomIndex] = Tmp;
  }
}

// Get next scatter value (YOffset is from the first slice (max height))
void CRandomScatter::ScatterGetNext(int SliceHeight,int SliceYOffset,int& NoOfPasses,
                                    int& YScatterOffset,int& ScatterOnly, bool EvenOdd)
{
  CAppParams *ParamsMgr = CAppParams::Instance();


  if((m_PreviousH != SliceHeight) || (m_PreviousY != SliceYOffset))
  {
    m_PreviousH = SliceHeight;
    m_PreviousY = SliceYOffset;
    m_YHChanged = true;
  }

  // Check if it is time to refresh scatter table
  if(++m_ScatterTableRefreshCounter == ParamsMgr->ScatterTableRefreshCycles)
  {
    if(m_YHChanged)
      ScatterCompute(SliceHeight,SliceYOffset,false);

    m_ScatterTableRefreshCounter = 0;
  }

  int LowestScatter,HighestScatter;
  int SliceSizeType = CalcTrayOffsetNext(SliceHeight,SliceYOffset,LowestScatter,HighestScatter);

  // add Ystart half Nozzle Space in odd layers, to balance X forward and backwards weights.
  int EvenOddAdjustment = EvenOdd * (g_NozzlesGapInPixels / 2);

  if (ParamsMgr->HalfNozzleDirectionBalancing == false)
    EvenOddAdjustment = 0;

  //int NozzleGapMinusOne = g_NozzlesGapInPixels - 1;

  ScatterOnly = GetNextScatterValue();

  //int TrimmedInterlace = ParamsMgr->YInterlaceNoOfPixels;
  //if (TrimmedInterlace > 0)
  //    TrimmedInterlace = 0;

  int ScatterPlusEvenOdd = ScatterOnly + EvenOddAdjustment;

  //int ScatterPlusInterlace = ScatterPlusEvenOdd;
  //ScatterPlusInterlace -= (NozzleGapMinusOne - ParamsMgr->YSecondaryInterlaceNoOfPixels + TrimmedInterlace);

  // CoveredHeight is the number of *consequent* pixels that are covered by a single pass, (which may
  // be contained of two or four movements). Since there may be areas in the edges of the pass that are not
  // fully covered, CoveredHeight is <= the 'pass head movement area'
  // Accordint to Eq. 1.9
  int CoveredHeight = s_HeadPrintWidth + (g_NozzlesGapInPixels - 1) - ParamsMgr->YSecondaryInterlaceNoOfPixels
                      - abs((int)ParamsMgr->YInterlaceNoOfPixels);

  // Calculate the y offset and no. of passes
  NoOfPasses = CalcNumOfPasses(SliceHeight,SliceSizeType,SliceYOffset,s_HeadPrintWidth,ScatterPlusEvenOdd,CoveredHeight);
  YScatterOffset = CalcYScatterOffset(SliceHeight,SliceSizeType,SliceYOffset,s_HeadPrintWidth,g_NozzlesGapInPixels,NoOfPasses,ScatterPlusEvenOdd,ParamsMgr->YInterlaceNoOfPixels,ParamsMgr->YSecondaryInterlaceNoOfPixels);
}

/*int CRandomScatter::CalcNumOfPasses(int SliceHeight,int SliceSizeType,int SliceYOffset,int s_HeadPrintWidth,int ScatterPlusEvenOdd,int CoveredHeight)
{
  if(SliceSizeType == sstLarge)
  {
    // in case we are printing large model, take rearoffset into consideration:
    if (SliceHeight + SliceYOffset - ScatterPlusEvenOdd > 0)
    {
      NoOfPasses = (int)floor((float)(SliceHeight + SliceYOffset - ScatterPlusEvenOdd) / (float)s_HeadPrintWidth);

      if ((SliceHeight + SliceYOffset - ScatterPlusEvenOdd) % s_HeadPrintWidth != 0)
        NoOfPasses += 2;
      else
        NoOfPasses += 1;
    }
    else
      NoOfPasses = 1;
  }
  else
  {
    // According to Eq. 2.1
    NoOfPasses = (int)floor((float)(SliceHeight + ScatterPlusEvenOdd - 1 - CoveredHeight) / (float)s_HeadPrintWidth);

    if ((SliceHeight + ScatterPlusEvenOdd - 1 - CoveredHeight) % s_HeadPrintWidth != 0)
      NoOfPasses += 2;
    else
      NoOfPasses += 1;
  }
}

int CRandomScatter::CalcYScatterOffset(int SliceHeight,int SliceSizeType,int SliceYOffset,int s_HeadPrintWidth, int ScatterPlusInterlace,int NoOfPasses)
{
  if(SliceSizeType == sstLarge)
  {
	//&YScatterOffset
    YScatterOffset = ScatterPlusInterlace - SliceYOffset;
  }
  else
  {
    YScatterOffset = SliceHeight + ScatterPlusInterlace - (NoOfPasses - 1) * s_HeadPrintWidth;
  }

  YScatterOffset--; // since bmp Y data starts at zero (not one)
}*/

// Calculate the best matching Y position on the tray according to the model height and start position.
// Also, calculate the minimum and maximum possible scatter values (in heads nozzles units).
TSliceSizeType CRandomScatter::CalcTrayOffsetNext(int ModelHeight,int SliceYOffset,int& LowestScatter,
                                                  int& HighestScatter)
{
  TSliceSizeType SliceSizeType;

  CAppParams *ParamsMgr = CAppParams::Instance();

  int ModelHeight300DPI = ModelHeight * 300 / ParamsMgr->DPI_InYAxis;
  int SliceYOffset300DPI = SliceYOffset * 300 / ParamsMgr->DPI_InYAxis;
  int InterlaceRearExtend = 0;
  int InterlaceFrontExtend = 0;

  // Calculating how much head will extend (rear and front) from pass due to interlace movement.
  if ((int)ParamsMgr->YInterlaceNoOfPixels > 0)
  {
    InterlaceRearExtend  = (int)ParamsMgr->YInterlaceNoOfPixels;
    InterlaceFrontExtend = (int)ParamsMgr->YSecondaryInterlaceNoOfPixels;
  }
  else
  {
    InterlaceRearExtend  = 0;
    InterlaceFrontExtend = (int)ParamsMgr->YSecondaryInterlaceNoOfPixels - /*N1 is Negative*/ (int)ParamsMgr->YInterlaceNoOfPixels;
  }
  int antiBacklash = abs((float)ParamsMgr->Y_DirectionSwitchCorrection);
  long Backlash = CONFIG_ConvertYmmTo300DPIAdjustedPixels(ParamsMgr->Y_BacklashMove_mm);
  
  // Different behavior according to the original size
  switch(m_FirstSliceSizeType)
  {
    case sstLarge:
      if(SliceYOffset300DPI < s_TrayRearOffset)
      {
        SliceSizeType = sstLarge;
        LowestScatter = QSimpleRound(((float)s_TrayRearOffset + InterlaceRearExtend) / g_NozzlesGapInPixels);
        if((SliceYOffset300DPI + ModelHeight300DPI) >= (s_TotalTrayHeight - s_TrayFrontOffset - s_TrayYStartOffset))

        //   following mathod isn't good since it assumes TrayFrontOffset to be accurate:
        //   HighestScatter = (HEAD_PRINT_WIDTH_300_DPI - s_TrayFrontOffset) / g_NozzlesGapInPixels;

          HighestScatter = QSimpleRound((float)(s_YMaxPosition - s_TrayStartYPosition + HEAD_PRINT_WIDTH_300_DPI - s_TrayYStartOffset -
                           (m_MaxPassesNum - 1) * HEAD_PRINT_WIDTH_300_DPI - s_InterlaceFrontDelta - s_EvenOddDelta - Backlash - antiBacklash)
                           / g_NozzlesGapInPixels - 0.5f /*to round to the lower nozzle num*/);

        else
          HighestScatter = g_NozzlesInAllHeads;
      } else
        {
          LowestScatter = 0;

          if((SliceYOffset300DPI + ModelHeight300DPI) > (s_TotalTrayHeight - s_TrayFrontOffset - s_TrayYStartOffset))
          {
            SliceSizeType = sstMedium;
            HighestScatter = g_NozzlesInAllHeads - QSimpleRound(((float)s_TrayFrontOffset+InterlaceFrontExtend) / g_NozzlesGapInPixels);
          }
          else
          {
            SliceSizeType = sstSmall;
            HighestScatter = g_NozzlesInAllHeads;
          }
        }
      break;
case sstLargeOnRelativeSmallTray:
	{	
		bool treatas_sstLarge = false;
		SliceSizeType = sstLargeOnRelativeSmallTray;	  
		LowestScatter = 0;
				
		// This is the same HighestScatter calculation as in sstMedium. Should be trimmed according to sstLargeOnRelativeSmallTray
		int HighestScatterMedium = g_NozzlesInAllHeads - QSimpleRound(((float)s_TrayFrontOffset+InterlaceFrontExtend) / g_NozzlesGapInPixels);

		
		if(SliceYOffset300DPI < s_TrayRearOffset)
		{
			// This happens if slices becomes shorter only from their front side.
			// In such a case, if we had treated this slice as a normal sstLargeOnRelativeSmallTray, we would get into trouble with scatter == 0 at some (short enough) slice hight.
			// An sstLarge slice knows how to overcome this shortcoming, so we fall-through to the sstLarge: case.
			if((SliceYOffset300DPI + ModelHeight300DPI) <= (s_TotalTrayHeight - s_TrayFrontOffset - s_TrayYStartOffset))
			{
				treatas_sstLarge = true; // fall-through to "case sstLarge:"
			}
			else
			{
				int DefaultScatter = GetDefaultScatter(ModelHeight);
				HighestScatter = min(HighestScatterMedium, DefaultScatter);
			}	
		}			
		else  if((SliceYOffset300DPI + ModelHeight300DPI) > (s_TotalTrayHeight - s_TrayFrontOffset - s_TrayYStartOffset))
		{
			SliceSizeType  = sstMedium;
	        HighestScatter = HighestScatterMedium;
		} 
        else
        {
        	SliceSizeType  = sstSmall;
        	HighestScatter = g_NozzlesInAllHeads;
        }

		if (!treatas_sstLarge)
			break;
	}
    case sstMedium:
      LowestScatter = 0;
      if((SliceYOffset300DPI + ModelHeight300DPI) > (s_TotalTrayHeight - s_TrayRearOffset - s_TrayFrontOffset - s_TrayYStartOffset))
      {
        SliceSizeType = sstMedium;
        HighestScatter = g_NozzlesInAllHeads - QSimpleRound(((float)s_TrayFrontOffset+InterlaceFrontExtend) / g_NozzlesGapInPixels);
      }
      else
      {
        SliceSizeType = sstSmall;
        HighestScatter = g_NozzlesInAllHeads;
      }
      break;

    //case sstSmall:
    default:
      SliceSizeType = sstSmall;
      LowestScatter  = 0;
      HighestScatter = g_NozzlesInAllHeads;
  }

  return SliceSizeType;
}

// Get the next "raw" scatter value from the odd or even table
int CRandomScatter::GetNextScatterValue(void)
{
  int r;
  r = m_ScatterTable1[m_CurrentScatterIndex];

  // Prepare the next scatter entry
  if(++m_CurrentScatterIndex == SCATTER_TABLE_SIZE)
    m_CurrentScatterIndex = 0;

  return r;
}

int CRandomScatter::GetDefaultScatter(int MaxHeight)
{
  int Delta = 0;
  CAppParams *ParamsMgr = CAppParams::Instance();
  int MaxHeightMinusOne = MaxHeight;
  if (MaxHeightMinusOne > 0)
    MaxHeightMinusOne--;
  int MaxHeightModHeadHeight = (MaxHeightMinusOne / s_NozzlesSeqOffset) % g_NozzlesInAllHeads;
  int CoveredHeight = s_HeadPrintWidth + (g_NozzlesGapInPixels - 1) - ParamsMgr->YSecondaryInterlaceNoOfPixels
                      - abs((int)ParamsMgr->YInterlaceNoOfPixels);
  CoveredHeight /= s_NozzlesSeqOffset;
  Delta = CoveredHeight - MaxHeightModHeadHeight;
  Delta--; // Keneged Ayn-Hara !
  if (Delta < 0)
    Delta = 0; // to avoid: "pow() domain error"  
  return Delta;
}
long CRandomScatter::GetFileSize(FILE *Stream)
{
   long Length;

   fseek(Stream, 0L, SEEK_END);
   Length = ftell(Stream);
   fseek(Stream, 0, SEEK_SET);
   return Length;
}
