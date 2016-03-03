//---------------------------------------------------------------------------

#pragma hdrstop
#pragma argsused
#pragma package(smart_init)

#include "msvcr120r.h"
#include <math.h>

unsigned __stdcall CalcNumOfFires(
	int SliceWidth, float SliceRes, int LastHeadOffset_1200DPI, int SliceResX, int InitialEmptyFires)
{
	unsigned FiresNum = ((((SliceWidth) * 600) / (SliceRes)) + (LastHeadOffset_1200DPI) / 2); // To normalize to 600 DPI data

	if((((LastHeadOffset_1200DPI) * (SliceResX)) % 1200) != 0)
		FiresNum++;

	// Add the initial empty fires (x2 is because we have initial empty fires at the begining and end of each travel)
	FiresNum += (2 * (InitialEmptyFires));
	return FiresNum;
}

int __stdcall CalcEndOfPlot(
	int StartOfPlot, int SliceWidth, float SliceRes, int LastHeadOffset_1200DPI, int AdvanceFire, int InitialEmptyFires)
{
int EndOfPlot   = StartOfPlot + ((SliceWidth * 1200) / SliceRes) + LastHeadOffset_1200DPI + AdvanceFire;

	EndOfPlot  += InitialEmptyFires
					* 2  /*adapt 600-DPI fires-data to 1200-DPI PEG*/
					* 2; /*prefix + postfix*/

	return EndOfPlot;
} 

int __stdcall CalcNumOfPasses(
	int SliceHeight,int SliceSizeType,int SliceYOffset,int s_HeadPrintWidth,int ScatterPlusEvenOdd,int CoveredHeight)
{
	int NoOfPasses = 0;
	if(SliceSizeType == Large_sst)
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
	return NoOfPasses;
}

int __stdcall CalcYScatterOffset(
	int SliceHeight,int SliceSizeType,int SliceYOffset,int s_HeadPrintWidth, int g_NozzlesGapInPixels,
	int NoOfPasses,int ScatterPlusEvenOdd, int YInterlaceNoOfPixels, int YSecondaryInterlaceNoOfPixels)
{
	int YScatterOffset = 0;

	int NozzleGapMinusOne = g_NozzlesGapInPixels - 1;

	NozzleGapMinusOne = ((NozzleGapMinusOne*2)/2);

	int TrimmedInterlace = YInterlaceNoOfPixels;
    if (TrimmedInterlace > 0)
	  TrimmedInterlace = 0;

	int ScatterPlusInterlace = ScatterPlusEvenOdd;
	ScatterPlusInterlace -= (NozzleGapMinusOne - YSecondaryInterlaceNoOfPixels + TrimmedInterlace);

	if(SliceSizeType == Large_sst)
    {
		//&YScatterOffset
		YScatterOffset = ScatterPlusInterlace - SliceYOffset;
    }
    else
    {
		YScatterOffset = SliceHeight + ScatterPlusInterlace - (NoOfPasses - 1) * s_HeadPrintWidth;
	}

	YScatterOffset--; // since bmp Y data starts at zero (not one)
    return YScatterOffset;
} 
//---------------------------------------------------------------------------

