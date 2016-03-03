/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Test pattern Printing                                    *
 * Module Description:                                              *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Rachel / Ran                                             *
 * Start date: 08/01/2002                                           *
 * Last upate: 14/02/2002                                           *
 ********************************************************************/

#include <stdlib.h>
#include "AppParams.h"
#include "TestPatternLayer.h"
#include "AppLogFile.h"
#include "Q2RTApplication.h"


// Constructor
CTestPatternLayer::CTestPatternLayer (void) : CLayer()
{
    // Initialize a pattern test layer
	PrepareTestPattern();
}

CTestPatternLayer::CTestPatternLayer (bool bNeedToPrintInTwoItereation, bool bPrintTheEvenHeads) : CLayer()
{
    // Initialize a pattern test layer
	PrepareTestPattern(bNeedToPrintInTwoItereation, bPrintTheEvenHeads);
}
// Destructor
CTestPatternLayer::~CTestPatternLayer(void)
{
   PBYTE buf = GetBuffer();
   Q_SAFE_DELETE(buf);
}  

//check if we need to have "Not" bitwise operation
bool CTestPatternLayer::IsNOTBitwiseNeeded(const int& index, const bool& bIsFirstIteration)
{
    return (((index / NUM_OF_BYTES_SINGLE_HEAD) % 2 == 0) != (bIsFirstIteration));
}

void CTestPatternLayer::PrepareTestPattern(bool bNeedToPrintInTwoItereation, bool bIsPrintingEvenHeads)
{
    CAppParams *ParamsMgr = CAppParams::Instance();

    // Compute the memory size needed for the layer
    int TestPatternSize = ParamsMgr->TestPatternNoOfFires * NUM_OF_BYTES_SINGLE_FIRE;

    // Allocate memmory for the layer
	PBYTE TestPatternBufferTmpPtr = new BYTE[TestPatternSize];
	
    memset(TestPatternBufferTmpPtr, 0, TestPatternSize);

    // Check allocation
    if(TestPatternBufferTmpPtr == 0)
        throw ETestPattern("No memory is available for test pattern buffer");

    PBYTE TmpPtr = TestPatternBufferTmpPtr;

    for (int j = 0; j < ParamsMgr->TestPatternNoOfFires; j++)
    {
		for (int i = 0; i < NOZZLES_PER_HEAD; i++, TmpPtr++)
        {
            if(bNeedToPrintInTwoItereation)
            {
                if(IsNOTBitwiseNeeded(i, bIsPrintingEvenHeads))
                {
					*TmpPtr = BYTE(0);
				}
				else
				{
					*TmpPtr = BYTE(QHexToInt(ParamsMgr->TestPatternData[i])) & 0xff;
                }
            }
            else
            {
                *TmpPtr = BYTE(QHexToInt(ParamsMgr->TestPatternData[i])) & 0xff;
            }
        }
    }

    Init(TestPatternBufferTmpPtr,TestPatternSize,0,0,0,1,0,FORWARD_PRINT_SCAN);

    m_DCPPCLayerParams.StartOfPlot = 0;

    m_DCPPCLayerParams.EndOfPlot = m_DCPPCLayerParams.StartOfPlot + ParamsMgr->TestPatternNoOfFires * 1200 / ParamsMgr->SliceResolution_InXAxis + ParamsMgr->AdvanceFire_1200DPI;
    m_DCPPCLayerParams.NoOfFires = ParamsMgr->TestPatternNoOfFires;

    if(ParamsMgr->DumpPassToFile || FindWindow(0, "DumpPassToFile.txt - Notepad"))
	{
        QString FileName;
        FILE *out = NULL;
		static int BufferNumber = 0;
		QString Path = ParamsMgr->DumpPassToFilePath;
        ForceDirectories(Path.c_str());
        FileName = Path+ "\\" + "PatternTest_" + QIntToStr(BufferNumber);
        out = fopen(FileName.c_str(), "wb");

        fwrite(TestPatternBufferTmpPtr, sizeof(BYTE), TestPatternSize, out);

        BufferNumber++;

        if (out)
            fclose(out);
    }           
}


