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
 * Last upate: 10/01/2002                                           *
 ********************************************************************/

#include <cmath>
#include "ScatterGenerator.h"
#include "LayerProcessDefs.h"
#include "AppParams.h"
#include "AppLogFile.h"
#include "MotorDefs.h"
#include "Configuration.h"

const float NOZZLES_PER_INCH_PER_HEAD = 37.5; // ??? why not = (96 / (65.2541 / 25.4));
// Folowing g_Nozzles... are declared in LayerProcess.cpp
// Number of nozzles in all printing heads
int g_NozzlesInAllHeads  = 0; // 384; 196;
// Number of nozzles in all printing heads
int g_NozzlesGapInPixels = 0;

int  CScatterGenerator::s_TrayRearOffset     = 0;
int  CScatterGenerator::s_TrayFrontOffset    = 0;
int  CScatterGenerator::s_HeadPrintWidth     = 0;
long CScatterGenerator::s_TrayStartYPosition = 0;
long CScatterGenerator::s_YMaxPosition       = 0;
long CScatterGenerator::s_TotalTrayHeight    = 0;
int  CScatterGenerator::s_NozzlesSeqOffset   = 0;
int  CScatterGenerator::s_TrayYStartOffset   = 0;
int  CScatterGenerator::s_InterlaceFrontDelta= 0;
int  CScatterGenerator::s_InterlaceRearDelta = 0;
int  CScatterGenerator::s_EvenOddDelta       = 0;
int  CScatterGenerator::s_SmallSliceMaxSize  = 0;
int  CScatterGenerator::s_MediumSliceMaxSize = 0;
int  CScatterGenerator::s_LargeSliceMaxSize  = 0;


// Constructor
CScatterGenerator::CScatterGenerator(void)
{
  Init();
  m_YHPrevious = 0;
  m_DeltaH                  = 0;
  m_TrayYOffset             = 0;
  m_LowestScatter           = 0;

  // Calculate the maximum number of passes
  // The height to be covered by the head passes without the pass covered by the scatter
  int TmpH = s_TotalTrayHeight - s_TrayFrontOffset;

  // Check if there is a need for passes more than the scatter pass
  if(TmpH > 0)
  {
    if((TmpH % s_HeadPrintWidth) != 0)
    {
      // The reamining height (not including the height covered by the scatter) can not be covered by an exact number of
      // head multiples (one for the scatter pass and one for the remaining pass)
      m_MaxPassesNum = TmpH / s_HeadPrintWidth + 2;
    }
    else
    {
      // The reamining height (not including the height covered by the scatter) can be covered by an exact number of
      // head multiples
      m_MaxPassesNum =  TmpH / s_HeadPrintWidth + 1;
    }
  }
  else
  {
    // The scatter pass covers the slice
    m_MaxPassesNum = 1;
  }
}

void CScatterGenerator::Init()
{
  CAppParams* ParamsMgr     = CAppParams::Instance();
  // Figure out the head print width according to the Y resolution
  //int ModelHeadsNum         = GetHeadsNum() - NUMBER_OF_SUPPORT_HEADS; // OBJET_MACHINE
  int ModelHeadsNum         = GetHeadsNum() / 2;
  int PrintRes_Y            = ParamsMgr->DPI_InYAxis;
  g_NozzlesInAllHeads       = NOZZLES_PER_HEAD * ModelHeadsNum;
  g_NozzlesGapInPixels      = PrintRes_Y / (NOZZLES_PER_INCH_PER_HEAD * ModelHeadsNum);

  s_HeadPrintWidth          = HEAD_PRINT_WIDTH_300_DPI;
  if(PrintRes_Y == 600)
    s_HeadPrintWidth *= 2;
  s_NozzlesSeqOffset        = g_NozzlesGapInPixels;  //Why Do I need this - s_NozzlesSeqOffset?
  s_TrayYStartOffset        = CONFIG_ConvertYmmTo300DPIAdjustedPixels(TRAY_Y_START_OFFSET_MM);
  s_TotalTrayHeight         = CONFIG_ConvertYmmTo300DPIAdjustedPixels(ParamsMgr->TrayYSize);
  s_YMaxPosition            = ParamsMgr->MaxPositionStep[AXIS_Y] / ParamsMgr->YStepsPerPixel;
  s_TrayStartYPosition      = CONFIG_ConvertYmmTo300DPIAdjustedPixels(ParamsMgr->TrayStartPositionY);
  s_EvenOddDelta            = g_NozzlesGapInPixels / 2;

  long YMinPosition         = ParamsMgr->MinPositionStep[AXIS_Y] / ParamsMgr->YStepsPerPixel;
  long Backlash             = CONFIG_ConvertYmmTo300DPIAdjustedPixels(ParamsMgr->Y_BacklashMove_mm);

  s_InterlaceRearDelta      = abs((float)ParamsMgr->YInterlaceNoOfPixels) + abs((float)ParamsMgr->YSecondaryInterlaceNoOfPixels + (g_NozzlesGapInPixels - 1)); // (|N1| + |N2| + (g_NozzlesGapInPixels - 1))
  if (ParamsMgr->YInterlaceNoOfPixels > 0)
  {
    s_InterlaceFrontDelta = -(g_NozzlesGapInPixels - 1 - ParamsMgr->YSecondaryInterlaceNoOfPixels); // -(3 - N2)
    s_InterlaceFrontDelta += ParamsMgr->YInterlaceNoOfPixels;
    if (s_InterlaceFrontDelta < 0)
       s_InterlaceFrontDelta = 0;
  }
  else // (ParamsMgr->YInterlaceNoOfPixels < 0). == 0 is not allowed!
  {
    s_InterlaceFrontDelta = -(g_NozzlesGapInPixels - 1 - ParamsMgr->YSecondaryInterlaceNoOfPixels + ParamsMgr->YInterlaceNoOfPixels); // -(3 - N2 + N1) always >= 0
  }

  // antiBacklash is added to s_TrayRearOffset (and not to s_TrayFrontOffset) since it is performed in
  // moving up direction only.
  int antiBacklash = abs((float)ParamsMgr->Y_DirectionSwitchCorrection);

  s_TrayRearOffset = (YMinPosition - (s_TrayStartYPosition  - HEAD_PRINT_WIDTH_300_DPI - s_NozzlesSeqOffset)
                      + s_InterlaceRearDelta + Backlash + antiBacklash - s_TrayYStartOffset + s_EvenOddDelta);

  s_TrayFrontOffset = HEAD_PRINT_WIDTH_300_DPI - (s_YMaxPosition - (s_TrayStartYPosition - HEAD_PRINT_WIDTH_300_DPI + s_TotalTrayHeight))
                      + s_InterlaceFrontDelta + Backlash + antiBacklash + s_EvenOddDelta;

  // In case there is no front offset problem
  if (s_TrayFrontOffset < 0)
    s_TrayFrontOffset = 0;

  if (s_TrayRearOffset < 0)
    s_TrayRearOffset = 0;

  // Calculate the slice Max slice size, for each slice type. (Small, Medium, Large)
  s_SmallSliceMaxSize  = (s_TotalTrayHeight - s_TrayRearOffset - s_TrayFrontOffset - s_TrayYStartOffset);
  s_MediumSliceMaxSize = (s_TotalTrayHeight - s_TrayRearOffset - s_TrayYStartOffset);
  s_LargeSliceMaxSize  =  s_TotalTrayHeight; // todo -oNobody -cNone: . is this the maximum ???
}

// Destructor
CScatterGenerator::~CScatterGenerator(void)
{
}

// Get next scatter value (YOffset is from the first slice (max height))
void CScatterGenerator::ScatterGetNext(int SliceHeight,int SliceYOffset,int& NoOfPasses,
                                       int& YScatterOffset,int& ScatterOnly, bool EvenOdd)
{
  CAppParams *ParamsMgr = CAppParams::Instance();
  if(m_YHPrevious != SliceHeight + SliceYOffset)
  {
    m_DeltaH = (m_YHPrevious - (SliceHeight + SliceYOffset)) % s_HeadPrintWidth;
    m_YHPrevious = SliceHeight + SliceYOffset;
  }

  int NozzleGapMinusOne = g_NozzlesGapInPixels - 1;
  ScatterOnly = m_LowestScatter;
  int Scatter = ScatterOnly * s_NozzlesSeqOffset; // needed since not taken from scatter tables

  int TrimmedInterlace = ParamsMgr->YInterlaceNoOfPixels;
  if (TrimmedInterlace > 0)
      TrimmedInterlace = 0;

  Scatter -= (NozzleGapMinusOne - ParamsMgr->YSecondaryInterlaceNoOfPixels + TrimmedInterlace);

  // CoveredHeight is the number of *consequent* pixels that are covered by a single pass, (which may
  // be contained of two or four movements). Since there may be areas in the edges of the pass that are not
  // fully covered, CoveredHeight is <= the 'pass head movement area'
  // Accordint to Eq. 1.9
  int CoveredHeight = s_HeadPrintWidth + NozzleGapMinusOne - ParamsMgr->YSecondaryInterlaceNoOfPixels
                      - abs((int)ParamsMgr->YInterlaceNoOfPixels);

  // According to Eq. 2.1
  NoOfPasses = (int)floor((float)(SliceHeight + ScatterOnly - 1 - CoveredHeight) / (float)s_HeadPrintWidth) + 2;

  int NewHeight = SliceHeight + m_DeltaH;

  if(sstLarge == m_FirstSliceSizeType)
  {
      // The scatter offset is relative to the tray start
      // Note: There is an implicit assumption that the slice Y offset is zero (in a case of
      //       a large tray and no-scatter).
      NoOfPasses = (int)floor((float)(SliceHeight + SliceYOffset - ScatterOnly) / (float)s_HeadPrintWidth) + 2;

      YScatterOffset = Scatter; // - SliceYOffset  - For future test, overcome the problem mentioned in the above note.
  }
  else
  {
      // According to Eq. 2.1
      NoOfPasses = (int)floor((float)(SliceHeight + ScatterOnly - 1 - CoveredHeight) / (float)s_HeadPrintWidth) + 2;

      YScatterOffset = NewHeight + Scatter - (NoOfPasses - 1) * s_HeadPrintWidth;
  }

  YScatterOffset--; // since bmp Y data starts at zero (not one)
}

// Compute new scatter table
// MaxHeight is actualy the slice's height, but because it is calculated for the first slice only,
// and the first slice is agreed to be the larget slice in any model, this is calles MaxHeight.
void CScatterGenerator::ScatterCompute(int MaxHeight,int SliceYOffset,bool ResumedJob)
{
  m_YHPrevious = MaxHeight + SliceYOffset;

  int HighestScatter;
  CalcTrayOffset(MaxHeight,ResumedJob,m_LowestScatter,HighestScatter);
}

// Utility function for converting a Y value from nozzles to DPI related scatter value
int CScatterGenerator::ConvertNozzleToYDPI(int Y)
{
  return (Y * g_NozzlesGapInPixels);
}

// Help function for calculating the tray offset and lowest/highest available scatter
TSliceSizeType CScatterGenerator::CalcTrayOffset(int ModelMaxHeight,bool ResumedJob,
                                                 int& LowestScatter,int& HighestScatter)
{
  m_FirstSliceSizeType = sstLarge;

  CAppParams *ParamsMgr = CAppParams::Instance();

  int DPIFactor = ParamsMgr->DPI_InYAxis / 300;

  int ModelMaxHeight300DPI = ModelMaxHeight / DPIFactor;

  // Restore previous job slice size type (-1 = unknown)
  int SliceSizeType = (TSliceSizeType)ParamsMgr->LastSliceSizeType.Value();

  if(ResumedJob)
    CQLog::Write(LOG_TAG_PRINT,"Resumed job - Size ID (%d)",SliceSizeType);

  if(!ResumedJob || (SliceSizeType == -1))
  {
    // Find the slice type
    if(ModelMaxHeight300DPI <= s_SmallSliceMaxSize)
    {
      m_FirstSliceSizeType = sstSmall;
    } else
        if(ModelMaxHeight300DPI <= s_MediumSliceMaxSize)
        {
          m_FirstSliceSizeType = sstMedium;
        } else
		{
		  m_FirstSliceSizeType = sstLarge;
		}
    ParamsMgr->LastSliceSizeType = m_FirstSliceSizeType;
	ParamsMgr->SaveSingleParameter(&ParamsMgr->LastSliceSizeType);
  } else
    {
      // Restore previous job slice size type
      m_FirstSliceSizeType = (TSliceSizeType)SliceSizeType;
    }

  // Set the scatter properties according to the slice type
  switch(m_FirstSliceSizeType)
  {
    case sstSmall:
      m_TrayYOffset = s_TrayRearOffset * DPIFactor;
      LowestScatter  = 0;
      HighestScatter = g_NozzlesInAllHeads;
      break;

    case sstMedium:
	  m_TrayYOffset = s_TrayRearOffset * DPIFactor;        
      LowestScatter  = 0;
      HighestScatter = g_NozzlesInAllHeads - QSimpleRound((float)s_TrayFrontOffset / g_NozzlesGapInPixels);
      break;

    case sstLarge:
      m_TrayYOffset = 0;

      int InterlaceRearExtend = 0;

      // Calculating how much head will extend (rear and front) from pass due to interlace movement.
      if ((int)ParamsMgr->YInterlaceNoOfPixels > 0)
      {
        InterlaceRearExtend  = (int)ParamsMgr->YInterlaceNoOfPixels;
      }
      else
      {
        InterlaceRearExtend  = 0;
      }
      // backlash and antibacklash sizes are needed for Highest/Lowest scatters.
      int antiBacklash = abs((float)ParamsMgr->Y_DirectionSwitchCorrection);
      long Backlash = CONFIG_ConvertYmmTo300DPIAdjustedPixels(ParamsMgr->Y_BacklashMove_mm);

      LowestScatter = QSimpleRound(((float)s_TrayRearOffset /*<-already accounts for backlashes*/ + InterlaceRearExtend) / g_NozzlesGapInPixels);

      HighestScatter = QSimpleRound((float)(s_YMaxPosition - s_TrayStartYPosition + HEAD_PRINT_WIDTH_300_DPI - s_TrayYStartOffset -
                       (m_MaxPassesNum - 1) * HEAD_PRINT_WIDTH_300_DPI - s_InterlaceFrontDelta - s_EvenOddDelta - Backlash - antiBacklash)
                        / g_NozzlesGapInPixels - 0.5f /*to round to the lower nozzle num*/);

      if (HighestScatter > ParamsMgr->ScatterFactorMax)
        HighestScatter = ParamsMgr->ScatterFactorMax;

      // For explanation of following code please see: (VSS) $/Quadra2/Documents/General/Knowledge transfer program - details.doc
      // The Visio diagram that handles: Scatter Span calculations - Large Models
      //                                 Special Case: Lowest Scatter > Highest Scatter
      if (LowestScatter > HighestScatter)
        HighestScatter = ParamsMgr->ScatterFactorMax;
      break;
  }

  return m_FirstSliceSizeType;
}

int CScatterGenerator::GetTrayYOffset(void)
{
  return m_TrayYOffset;
}

TSliceSizeType CScatterGenerator::GetFirstSliceSizeType(void)
{
  return m_FirstSliceSizeType;
}

int CScatterGenerator::GetMaxPassesNum()
{
  return m_MaxPassesNum;
}

int CScatterGenerator::GetTrayRearOffset()
{
  return s_TrayRearOffset;
}

int CScatterGenerator::GetTrayFrontOffset()
{
  return s_TrayFrontOffset;
}

int CScatterGenerator::GetSmallSliceMaxSize()
{
   return  s_SmallSliceMaxSize;
}

int CScatterGenerator::GetMediumSliceMaxSize()
{
   return s_MediumSliceMaxSize;
}

int CScatterGenerator::GetLargeSliceMaxSize()
{
   return  s_LargeSliceMaxSize;
}
