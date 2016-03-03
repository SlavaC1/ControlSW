//---------------------------------------------------------------------------

#ifndef MSVCR120RH
#define MSVCR120RH
//---------------------------------------------------------------------------

typedef enum {Small_sst=0,Medium_sst,Large_sst,LargeOnRelativeSmallTray_sst} SliceSizeType;

extern "C" __declspec(dllexport) unsigned __stdcall CalcNumOfFires(
	int SliceWidth, float SliceRes, int LastHeadOffset_1200DPI, int SliceResX, int InitialEmptyFires);
	
extern "C" __declspec(dllexport) int __stdcall CalcEndOfPlot(
	int StartOfPlot, int SliceWidth, float SliceRes, int LastHeadOffset_1200DPI, int AdvanceFire, int InitialEmptyFires); 	
	
extern "C" __declspec(dllexport) int __stdcall CalcNumOfPasses(
	int SliceHeight,int SliceSizeType,int SliceYOffset,int s_HeadPrintWidth,int ScatterPlusEvenOdd,int CoveredHeight);
	
extern "C" __declspec(dllexport) int __stdcall CalcYScatterOffset(
	int SliceHeight,int SliceSizeType,int SliceYOffset,int s_HeadPrintWidth, int g_NozzlesGapInPixels,
	int NoOfPasses,int ScatterPlusEvenOdd, int YInterlaceNoOfPixels, int YSecondaryInterlaceNoOfPixels); 


#endif
