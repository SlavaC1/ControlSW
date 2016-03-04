/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Layers processing thread (slice to layers process)       *
 * Module Description:                                              *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg                                                *
 * Start date: 08/01/2002                                           *
 * Last upate: 11/08/2003                                           *
 ********************************************************************/

#include <math.h>
#include "PrintJob.h"
#include "Layer.h"
#include "AppParams.h"
#include "ScatterGenerator.h"
#include "SequentialScatter.h"
#include "RandomScatter.h"
#include "QMonitor.h"
#include "QTimer.h"
#include "LayerProcess.h"
#include "Slice.h"
#include "QThreadUtils.h"
#include "AppLogFile.h"
#include "Q2RTApplication.h"
#include "MachineManager.h"
#include "Configuration.h"
#include "ExcelSequenceAnalyzer.h"
#include "MachineSequencer.h"
#include "HeadsMapper.h"
#include "ModesManager.h"
#include "ModesDefs.h"
#include <algorithm>
#include "BackEndInterface.h"
#include "HeadFilling.h"
#include "HeadHeaters.h"

using namespace Raccoon;

extern const int mapYOffset[];
#define HEADNUM_LP_TO_GENERAL(_headNum_) (TOTAL_NUMBER_OF_HEADS-_headNum_-1)

#if defined EDEN_500 || defined EDEN_350 || defined EDEN_350_V || defined EDEN_260_V || defined CONNEX_MACHINE || defined OBJET_MACHINE
// Default heads offsets in 1200 DPI
static const float DefaultHeadMapArray[8] = { 0 * 1200 / 25.4f,
                                              10 * 1200 / 25.4f,
                                              20 * 1200 / 25.4f,
                                              30 * 1200 / 25.4f,
                                              40 * 1200 / 25.4f,
                                              50 * 1200 / 25.4f,
                                              60 * 1200 / 25.4f,
                                              70 * 1200 / 25.4f
                                            };

#endif
#ifdef EDEN_250
static const float DefaultHeadMapArray[8] = { 0 * 1200 / 25.4f,
                                              10 * 1200 / 25.4f,
                                              20 * 1200 / 25.4f,
                                              30 * 1200 / 25.4f,
                                              40 * 1200 / 25.4f,
                                              50 * 1200 / 25.4f,
                                              70 * 1200 / 25.4f, // not 60, since there is an extra gap here.
                                              80 * 1200 / 25.4f
                                            };
#endif

#ifndef OBJET_1000
#define MAX_LAYER_MEM_SIZE 80000000   //itamar 5811
#else
#define MAX_LAYER_MEM_SIZE 80000000*4  //itamar 5811
#endif
#ifdef _DEBUG
#define MAX_LAYER_MEM_SIZE 80000000
#endif

// Constructor
CLayerProcess::CLayerProcess(void)
    : CQThread(true, "LayerProcess")
{
    Priority = Q_PRIORITY_LOW;

    // Create a message queue for layers process commands (only one command can be at the queue at a time)
    m_CommandsQueue = new TCommandsQueue(1, "", false);

    // Layers output queue
    m_LayersQueue = new TLayersQueue(MAX_LAYERS_QUEUE_SIZE, "LayersQueue", true);

    m_CurrentScatterGenerator = NULL;
    m_ParamsMgr = CAppParams::Instance();

    if(m_ParamsMgr->YScanDirection == ALTERNATE_PRINT_SCAN)
        m_CurrentPrintScanDirection = FORWARD_PRINT_SCAN;
    else
        m_CurrentPrintScanDirection = m_ParamsMgr->YScanDirection;

    // used by head simulator
    m_LastFireData = NULL;

    Reset();
#ifdef _DEBUG
    m_ScatterTester = new CScatterTester(this, "ScatterTester", true);
#else
    m_ScatterTester = NULL;
#endif
    //ThreadUnExpectedException
    //callback for turning off everything on print exception
    SetExceptionCallbacks(ThreadExceptionHandlerCallback, reinterpret_cast<TGenericCockie>(this), ThreadUnExpectedExceptionHandlerCallback, reinterpret_cast<TGenericCockie>(this));

    //bug 5885
    for(int i = 0; i < MAX_LAYERS_QUEUE_SIZE * 2; i++)
        m_CurrentLayer[i] = new CLayer;
    m_layerIndex = 0;
    AllocateLayersMemory();
}

// Destructor
CLayerProcess::~CLayerProcess(void)
{
    StopProcessing();

    Terminate();

    m_CommandsQueue->Release();

    WaitFor();

    // Delete all remaining layers
    FlushLayers();

    //bug 5885
    for(int i = 0; i < MAX_LAYERS_QUEUE_SIZE * 2; i++)
    {
        if(m_CurrentLayer[i])
        {
            Q_SAFE_DELETE(m_CurrentLayer[i]);
        }
    }

    Q_SAFE_DELETE(m_CommandsQueue);
    Q_SAFE_DELETE(m_LayersQueue);

    if(m_CurrentScatterGenerator != 0)
        Q_SAFE_DELETE(m_CurrentScatterGenerator);

    Q_SAFE_DELETE(m_ScatterTester);

    CExcelSequenceAnalyzerBase::DeInit();
}

// Reset internal state
void CLayerProcess::Reset(void)
{
    // Initialize internal variables
    m_CurrentPrintJob      = NULL;
    m_XCloneCounter        = 0;
    m_TotalLayersCounter   = 0;
    m_SliceHeightZReminder = 0.0f;
    m_Paused               = false;
}

// Thread execute function (override)
void CLayerProcess::Execute(void)
{
    TLayersProcessCommands Cmd;
  try {
    do
    {
        // Get the new command
        if(m_CommandsQueue->Receive(Cmd) == QLib::wrReleased)
            return;

        switch(Cmd)
        {
            case cmdStart:
                // If we are not continuing after pause mode, do pre-process preperation
                if(!m_Paused)
                {
                    // Delete all layers currently in the queue
                    FlushLayers();
                    CQLog::Write(LOG_TAG_PROCESS, "Slice processing started.");
                    PrepareForProcess();
                }
                else
                    CQLog::Write(LOG_TAG_PROCESS, "Slice processing continued after pause.");

                // Continue until new command is received
                while(m_CommandsQueue->ItemsCount == 0)
                {
                    try
                    {
                        CSlice *TmpSlice;
                        if(m_CurrentPrintJob == NULL)
                        {
                            CQLog::Write(LOG_TAG_PROCESS, "LayerProcess: 'm_CurrentPrintJob' is a NULL pointer");
                            throw ELayerProcess("LayerProcess: Access violation");
                        }

                        CQLog::Write(LOG_TAG_PROCESS, "LayerProcess: request next slice");

                        TmpSlice = m_CurrentPrintJob->BringSlice();


                        if((TmpSlice != NULL) && (TmpSlice != (CSlice *)LAST_SLICE_MARKER))
                        {
                            // Process new slice
                            ProcessSlice(TmpSlice, m_CurrentPrintJob->IsContinue());
                            m_CurrentPrintJob->DoneWithSlice(TmpSlice);
                        }
                        else
                        {
                            CQLog::Write(LOG_TAG_PROCESS, "Job processing completed.");

                            if(TmpSlice == (CSlice *)LAST_SLICE_MARKER)
                                // Add an end-print marker to the queue
                                m_LayersQueue->Send((CLayer *)LAST_SLICE_MARKER);
                            else
                            {
                                // NULL is used to signal stop processing
                                CQLog::Write(LOG_TAG_PROCESS, "Releasing layer queue");
                                m_LayersQueue->Release();
                            }
                            break;
                        }
                    }
                    catch(EQException& E)
                    {
                        // Print to screen the error
                        QMonitor.ErrorMessage(E.GetErrorMsg());
                        // Stop printing
                        Q2RTApplication->GetMachineManager()->StopPrint(E.GetErrorCode());
                        if(m_LayersQueue->ItemsCount == 0)
                            m_LayersQueue->Release();
                        break;
                    }
                }
                break;

            case cmdStop:
                CQLog::Write(LOG_TAG_PROCESS, "Slice processing stopped.");
                //fix bug 5847
                //if(m_CurrentPrintJob)
                //{
                //delete m_CurrentPrintJob;
                //m_CurrentPrintJob = NULL;
                //}
                break;

            case cmdDummy:
                // Do nothing
                break;
        }

    }
    while(!Terminated);
  }catch(...) {
      CQLog::Write(LOG_TAG_GENERAL, "CLayerProcess::Execute - unexpected error");
      if (!Terminated)
        throw EQException("CLayerProcess::Execute - unexpected error");
    }
}

// Set a new working job
void CLayerProcess::SetNewPrintJob(CPrintJob *NewJob)
{
    if(m_CurrentPrintJob)
    {
        StopProcessing();

        // Use the dummy command in order to block until the layer process thread
        // is ready for new command.
        m_CommandsQueue->Send(cmdDummy);
        m_CommandsQueue->Send(cmdDummy);
    }

    // Reset to initial setup
    Reset();

    // Assign new print job
    m_CurrentPrintJob = NewJob;

    // start new process
    m_CommandsQueue->Send(cmdStart);
}

// Stop processing
void CLayerProcess::StopProcessing(void)
{
    m_CommandsQueue->Send(cmdStop);
    m_LayersQueue->Release();
}

// Pause processing
void CLayerProcess::PauseProcessing(void)
{
    CLayer* TmpPtr = NULL;
    m_Paused = true;
    m_LayersQueue->SendUrgentAndBreakInto(NULL, TmpPtr);
    if(TmpPtr != NULL)
        DoneWithLayer(TmpPtr);

}

// Resume processing after pause
void CLayerProcess::ResumeProcessing(void)
{
}

// Get next (processed) layer
CLayer *CLayerProcess::GetLayer(TGetLayerResult& GetLayerResult)
{
    CLayer *TmpPtr = NULL;

    switch(m_LayersQueue->Receive(TmpPtr, QSecondsToTicks(m_ParamsMgr->HostSliceTimeoutInSec)))
    {
        case QLib::wrReleased:
            GetLayerResult = glrQueueReleased;
            TmpPtr = NULL;
            break;

        case QLib::wrTimeout:
            GetLayerResult = glrTimeout;
            CQLog::Write(LOG_TAG_PROCESS, "glrTimeout");
            TmpPtr = NULL;
            break;

        case QLib::wrSignaled:
            GetLayerResult = glrOk;
            break;

        default:
            CQLog::Write(LOG_TAG_PROCESS, "glrUnknownError");
            GetLayerResult = glrUnknownError;
    }

    return TmpPtr;
}

// Calculate the number of fires required for a given slice
unsigned CLayerProcess::CalcFiresNum(CSlice *Slice)
{
	int Width = Slice->GetWidth();
	float Res = Slice->GetResolution();
	int SliceResX = m_PrintConfigParams.PrintRes_X;
	//ITAMAR RACCOON
	unsigned FiresNum = CRaccoon::Instance()->R_CNOF(
		Width, Res, m_PrintConfigParams.LastHeadOffset_1200DPI, SliceResX,0);

  
  return FiresNum;
}

// Allocate memory for layers according to the first slice
unsigned CLayerProcess::CalculateLayerSize(unsigned FiresNum, unsigned PassesNum)
{

    // In 600 DPI in Y mode we need twice the memory (like in 4 heads mode)
    int HeadsMultiplier = ((m_PrintConfigParams.PrintRes_Y == 600) ? 2 : (HEADS_NUM / m_PrintConfigParams.HeadsNum));
    unsigned LayerMemSize = FiresNum * PassesNum * 2 * HEADS_FILL_SIZE_BYTE * HeadsMultiplier;
    return LayerMemSize;
}//CalculateLayerSize


void CLayerProcess::AllocateLayersMemory(CSlice *FirstSlice)
{
    // Find the amount of memory required for each layer
    unsigned HeadWidth = HEAD_PRINT_WIDTH_300_DPI * (m_PrintConfigParams.PrintRes_Y / 300);
    unsigned LayerMemSize = CalculateLayerSize(CalcFiresNum(FirstSlice),
                            std::min(m_CurrentScatterGenerator->GetMaxPassesNum(), (int)(FirstSlice->GetHeight() / HeadWidth + 3)));

    // Pre-allocate blocks
    m_LayersAllocators.Init(MAX_LAYERS_QUEUE_SIZE + 3, LayerMemSize);
}//AllocateLayersMemory

void CLayerProcess::AllocateLayersMemory()
{
    // Pre-allocate blocks
    if(CHECK_EMULATION(m_ParamsMgr->OCB_Emulation))  // for emultion ,till we are working not on 64 bit computer.
        m_LayersAllocators.Init(MAX_LAYERS_QUEUE_SIZE * 2, 80000000);
    else
        m_LayersAllocators.Init(MAX_LAYERS_QUEUE_SIZE * 2, MAX_LAYER_MEM_SIZE);
}//AllocateLayersMemory

void CLayerProcess::FillLayerBidirection300(CSlice *Slice, int NoOfPasses, int StartY, int NoOfFires, PBYTE LayerBuffer)
{
    switch(m_PrintConfigParams.HeadsNum)
    {
        case 8:
            FillLayerBidirection300_8Heads(Slice, NoOfPasses, StartY, NoOfFires, LayerBuffer);
            break;

        case 4:
            FillLayerBidirection300_4Heads(Slice, NoOfPasses, StartY, NoOfFires, LayerBuffer);
            //in objet DM use 6 heads for model and not 4
#if defined CONNEX_MACHINE/* || defined OBJET_MACHINE*/ //relevant for connex mode
            //DM: using all model chambers
            //(Connex: 4 model heads + 2 support heads) or (Objet: 6 model heads + 2 support heads)
            if(GetActiveChambersNum() == MM_RESIN_NUM)
            {
                MaskHeadData(HEAD_6, NoOfPasses * NoOfFires * 4, LayerBuffer);
                MaskHeadData(HEAD_7, NoOfPasses * NoOfFires * 4, LayerBuffer);
            }
            else
#endif
#if defined EDEN_500 || defined EDEN_350 || defined EDEN_350_V || defined EDEN_260_V || defined CONNEX_MACHINE/* || defined OBJET_MACHINE*/
                // Masking is done for Even or Odd heads in 500. this is a bad habbit for 250, since
                // it will destroy half of the data. Since we don't physicaly have these 4 heads
                // in 250, we may skip this function altogether.
                //DM: not using all model chambers
                //(Connex: 2 model heads + 2 support heads) or (Objet: 2/4 model heads + 2 support heads)
                Mask4Heads(NoOfPasses * NoOfFires * 4, LayerBuffer);
#endif
            break;

        default:
            throw ELayerProcess("Invalid number of print heads");
    }
}//FillLayerBidirection300

void CLayerProcess::FillLayerBidirection600(CSlice *Slice, int NoOfPasses, int StartY, int NoOfFires, PBYTE LayerBuffer)
{
    switch(m_PrintConfigParams.HeadsNum)
    {
        case 8:
            FillLayerBidirection600_8Heads(Slice, NoOfPasses, StartY, NoOfFires, LayerBuffer);
            break;
        case 4:
            //FillLayerBidirection600_4Heads(Slice,NoOfPasses,YScatterOffset,CurrentDPCPCParams->NoOfFires,LayerBufferTmpPtr);
            throw ELayerProcess("Printing in 600 DPI with 4 Heads is not supported.");
        default:
            throw ELayerProcess("Invalid number of print heads");
    }
}//FillLayerBidirection600

void CLayerProcess::FillLayerBidirection(CSlice *Slice, int NoOfPasses, int StartY, int NoOfFires, PBYTE LayerBuffer)
{
    switch(m_PrintConfigParams.PrintRes_Y)
    {
        case 150:
            FillLayerBidirection150(Slice, NoOfPasses, StartY, NoOfFires, LayerBuffer);
            break;
        case 300:
            FillLayerBidirection300(Slice, NoOfPasses, StartY, NoOfFires, LayerBuffer);
            break;
        case 600:
            FillLayerBidirection600(Slice, NoOfPasses, StartY, NoOfFires, LayerBuffer);
            break;
        default:
            throw ELayerProcess("LayerProcess:FillLayerBidirection Y configuration not supported");
    }
    //CQLog::Write(LOG_TAG_OBJET_MACHINE,"Modes: Y Res:%d, HeadsNum:%d",m_PrintConfigParams.PrintRes_Y,m_PrintConfigParams.HeadsNum);

}//FillLayerBidirection

// Process a single slice
void CLayerProcess::ProcessSlice(CSlice *Slice, bool IsContinue)
{
    CQLog::Write(LOG_TAG_PROCESS, "Starting slice processing (N=%d)", Slice->GetSliceNumber());
    int SliceHeight = Slice->GetHeight();
    if(SliceHeight == 0 || Slice->GetWidth() == 0)  //slice is empty
    {
        CQLog::Write(LOG_TAG_PROCESS, "slice is empty", Slice->GetSliceNumber()); //itamar memory leak
        if(m_LayersQueue->Send(new CEmptyLayer(Slice->GetSliceNumber())) == QLib::wrReleased)
            m_TotalLayersCounter++;
        return;
    }

    // Check if its the first layer of the carpet or the first layer of the model
    if(m_TotalLayersCounter == 0)
    {
        m_CurrentScatterGenerator->ScatterCompute(
            SliceHeight , // Slice's Y dimension
            Slice->GetYOffset(), // value as received from Host (or file).
            IsContinue);
        m_LayersAllocators.ReleaseBlocks(); //bug 5885
    }

    // Get the number of layers consisting this slice:
    int LayersNum = GetCurrentSliceLayersNum(Slice->GetSliceHeightZ());

    // For Each Layer of the current slice, Perform: Prepare the layer information & FIFO Data
    for(int SliceLayerIndex = 0; SliceLayerIndex < LayersNum; SliceLayerIndex++)
    {
        // Get layer process start time
        int t1 = QGetTicks();

        // Get scatter and number of passes
        int NoOfPasses, YScatterOffset, ScatterOnly;

        // The out param: YScatterOffset is the *final* Y position in the BMP in which we place our "virtual" head.
        // (i.e: the Y position from which we sample the data)
        // YScatterOffset does not consider Slice->GetYOffset, which is relevant only for the *printing* location.
        // Slice->GetYOffset is the difference (offset) between the FIRST slice's top position, and the CURRENT slice's top position.
        //   (received from Host)
        m_CurrentScatterGenerator->ScatterGetNext(
            Slice->GetHeight() , // Slice's Y dimension
            Slice->GetYOffset(),
            NoOfPasses,            // output param.
            YScatterOffset,        // output param.
            ScatterOnly,           // output param. Scatter raw value as obtained from the scatter tables. only for Logging.
            m_TotalLayersCounter % 2);  // evenodd layer

        int XLayerStart1200DPI, YLayerStart;

        // Calculate start position for the current slice on the tray
        CalculateLayerStartXY(Slice, YScatterOffset, XLayerStart1200DPI, YLayerStart);

        // Get computation results for OHDB: Start Of Plot, End Of Plot & No. Of Fires
        PrepareOHDBParams(Slice, XLayerStart1200DPI, m_CurrentLayer[m_layerIndex]->GetDPCPCParams()); //bug 5885

        // Utility pointer to the current OHDB params block
        // TDPCPCLayerParams holds: start of plot, end of plot, and num of fires.
        // since num of fires is (currently) common to the *entire* layer, we compute it only once.
        TDPCPCLayerParams *CurrentDPCPCParams = m_CurrentLayer[m_layerIndex]->GetDPCPCParams(); //bug 5885

        if(m_PrintConfigParams.PrintDirection != BIDIRECTION_PRINT_MODE)
            throw ELayerProcess("LayerProcess:ProcessSlice UNIDIRECTIONAL currently not supported");

        unsigned LayerMemSize = CalculateLayerSize(CurrentDPCPCParams->NoOfFires, NoOfPasses);

        // Allocate memmory for the layer
        PBYTE LayerBufferTmpPtr = (PBYTE)m_LayersAllocators.GetBlock(LayerMemSize, true);

        // Check allocation
        if(LayerBufferTmpPtr == NULL)
        {
            TQErrCode ErrCode = Q2RT_MEMORY_ALLOCATION_ERROR;
            QMonitor.ErrorMessage(PrintErrorMessage(ErrCode));
            CQLog::Write(LOG_TAG_PROCESS, PrintErrorMessage(ErrCode));

            // Stop printing
            Q2RTApplication->GetMachineManager()->StopPrint(ErrCode);
            return;
        }
        FillLayerBidirection(Slice, NoOfPasses, YScatterOffset, CurrentDPCPCParams->NoOfFires, LayerBufferTmpPtr);
        //bug 5885
        m_CurrentLayer[m_layerIndex]->Init(LayerBufferTmpPtr, LayerMemSize, Slice->GetSliceNumber(), m_TotalLayersCounter,
                                           SliceLayerIndex, NoOfPasses, YLayerStart, m_CurrentPrintScanDirection);

        // Prepare the pass mask under the context of the process thread
        m_CurrentLayer[m_layerIndex]->CalculatePassMask(); //bug 5885

        // If we are in Y alternate mode we need to switch the scan direction for next time
        if(m_ParamsMgr->YScanDirection == ALTERNATE_PRINT_SCAN)
        {
            m_CurrentPrintScanDirection = ((m_CurrentPrintScanDirection == FORWARD_PRINT_SCAN)
                                           ? BACKWARD_PRINT_SCAN : FORWARD_PRINT_SCAN);
        }

        int t2 = QGetTicks() - t1;
        //bug 5885
        CQLog::Write(LOG_TAG_PROCESS, "Layer process: Slice = %d , time = %d ms , Scatter = %d , YOffset = %d , N. of passes = %d/%d",
                     Slice->GetSliceNumber(), QTicksToMs(t2), ScatterOnly, YScatterOffset, m_CurrentLayer[m_layerIndex]->GetPassCounter(), NoOfPasses);

        // Add the layer to the output layers queue
        if(m_LayersQueue->Send(m_CurrentLayer[m_layerIndex]) == QLib::wrReleased) //bug 5885
        {
            // If we got release, get rid from the new layer and return
            DoneWithLayer(m_CurrentLayer[m_layerIndex]);//bug 5885
            break;
        }
        m_layerIndex = (m_layerIndex + 1) % (MAX_LAYERS_QUEUE_SIZE * 2);
        m_TotalLayersCounter++;
    }
}

// Prepare the configuration-related parameters block for printing
void CLayerProcess::PrepareConfigParams(void)
{
    THeadsMapArray mapper;

#ifndef EDEN_250
    int map[] = {0, 1, 2, 3, 4, 5, 6, 7};
#else
    int map[] = {3, 3, 3, 3, 4, 5, 0, 1};
#endif

    mapper.AssignMapper(map, 8);

    for(int i = 0; i < TOTAL_NUMBER_OF_HEATERS; i++)
        mapper[i] = i;

    m_PrintConfigParams.PrintRes_X                    = m_ParamsMgr->DPI_InXAxis;
    m_PrintConfigParams.PrintRes_Y                    = m_ParamsMgr->DPI_InYAxis;
    m_PrintConfigParams.PrintDirection                = m_ParamsMgr->PrintDirection;
    m_PrintConfigParams.XCloneDistance                = m_ParamsMgr->CloneGap1200DPI;
    m_PrintConfigParams.CloneStateEnabled             = m_ParamsMgr->CloneStateEnabled;
    m_PrintConfigParams.YInterlaceNoOfPixels          = m_ParamsMgr->YInterlaceNoOfPixels;
    m_PrintConfigParams.YSecondaryInterlaceNoOfPixels = m_ParamsMgr->YSecondaryInterlaceNoOfPixels;

    m_PrintConfigParams.X_OffsetMax_1200DPI    = m_ParamsMgr->XRightLimitInSteps * m_ParamsMgr->XStepsPerPixel -
            CONFIG_GetPrintXStartPosition_step() * m_ParamsMgr->XStepsPerPixel;

    m_PrintConfigParams.AdvanceFire_1200DPI    = m_ParamsMgr->AdvanceFire_1200DPI;
    int index                                  = mapper.Lookup(7);
    /*#ifdef OBJET_MACHINE
      m_PrintConfigParams.LastHeadOffset_1200DPI = QSimpleRound(MICRONS_TO_PIXELS_1200DPI*m_ParamsMgr->HeadMapArray[index] + DefaultHeadMapArray[7]);
    #else */
    m_PrintConfigParams.LastHeadOffset_1200DPI = QSimpleRound(m_ParamsMgr->HeadMapArray[index] + DefaultHeadMapArray[7]);
//#endif
    m_PrintConfigParams.LayerHeight_mm         = m_ParamsMgr->GetLayerHeight_um(m_ParamsMgr->DPI_InXAxis) / 1000.0f;
    m_PrintConfigParams.HeadsNum               = GetHeadsNum();

    // currently not using m_PrintConfigParams.SliceHeightInterlaceModifier
    if(m_ParamsMgr->YInterlaceNoOfPixels > 0)
    {
        m_PrintConfigParams.SliceHeightInterlaceModifier =
            m_PrintConfigParams.YSecondaryInterlaceNoOfPixels + m_PrintConfigParams.YInterlaceNoOfPixels;
    }
    else // if (ParamsMgr->YInterlaceNoOfPixels < 0)
    {
        m_PrintConfigParams.SliceHeightInterlaceModifier =
            m_PrintConfigParams.YSecondaryInterlaceNoOfPixels - m_PrintConfigParams.YInterlaceNoOfPixels;
    }

    if(m_ParamsMgr->YScanDirection == ALTERNATE_PRINT_SCAN)
        m_CurrentPrintScanDirection = FORWARD_PRINT_SCAN;
    else
        m_CurrentPrintScanDirection = m_ParamsMgr->YScanDirection;

    // Initialize the Head Map
    int YResMul  = (m_PrintConfigParams.PrintRes_Y == 600) ? 4 : 2;
    int NzlNum   = 0;
    int byte_num = 0;
    int bit_num  = 0;
    //int NumOfActiveModels = ModelActiveContainersNum()*2;

    for(int i = 0; i < HEADS_NUM; i++)
    {
        for(int j = 0; j < NOZZLES_PER_HEAD; j++)
        {
#ifdef EDEN_250
            index = mapper.Lookup(i);
#else
            index = i;
#endif

//#ifdef OBJET_MACHINE
//      NzlNum   = mapYOffset[i] + j * NumOfActiveModels;
//#else
            NzlNum   = mapYOffset[i] + j * 4; //4 is the number of model heads
//#endif
            byte_num = (i * HEAD_SIZE_BYTE) + (j / 8);
            bit_num  = (j % 8);
            /*if nozzle active*/
            bool nozzle_active = true;

            if(m_ParamsMgr->NozzleShutter)
                nozzle_active = (QHexToInt(m_ParamsMgr->TestPatternData[byte_num])) & (0x01 << bit_num);

            if(nozzle_active)
            {
                /*#ifdef OBJET_MACHINE
                		m_HeadMapArr_1200DPI[i][j].Offset_X_1200DPI = -QSimpleRound(MICRONS_TO_PIXELS_1200DPI*m_ParamsMgr->HeadMapArray[index] + DefaultHeadMapArray[i]);
                #else */
                m_HeadMapArr_1200DPI[i][j].Offset_X_1200DPI = -QSimpleRound(m_ParamsMgr->HeadMapArray[index] + DefaultHeadMapArray[i]);
//#endif
                m_HeadMapArr_1200DPI[i][j].Offset_Y         = -(NzlNum * YResMul);
            }
            else // nozzle should be shut:
            {
                m_HeadMapArr_1200DPI[i][j].Offset_X_1200DPI = MaxInt;
                m_HeadMapArr_1200DPI[i][j].Offset_Y         = MaxInt;
            }
        }
    }
}

// Calculate slice start position on the tray (PEG)
void CLayerProcess::CalculateLayerStartXY(CSlice *Slice, int YScatterOffset, int& X, int& Y)
{
    // Compute the first print point for X
    X = (((Slice->GetXOffset()) * 1200) / Slice->GetResolution());

    // Compute the first print point for Y
    Y = Slice->GetYOffset() + YScatterOffset + m_CurrentScatterGenerator->GetTrayYOffset();

    /*
     if(m_CurrentPrintScanDirection == BACKWARD_PRINT_SCAN)
       Y -= m_PrintConfigParams.YInterlaceNoOfPixels;
     */

    // Add X clone offset
    if(m_PrintConfigParams.CloneStateEnabled)
    {
        int TmpXStart = X + m_XCloneCounter * m_PrintConfigParams.XCloneDistance;

        // Check if we got the right boundry of the tray
        if(TmpXStart + (Slice->GetWidth() * 1200) / Slice->GetResolution() >= m_PrintConfigParams.X_OffsetMax_1200DPI)
            m_XCloneCounter = 0;
        else
            X = TmpXStart;

        m_XCloneCounter++;
    }
}

// Compute the parameters that the DPC-PC needs
void CLayerProcess::PrepareOHDBParams(CSlice *Slice, int StartCounter1200DPI_X, TDPCPCLayerParams* DCPPCLayerParams)
{
	DCPPCLayerParams->StartOfPlot = StartCounter1200DPI_X;
	//ITAMAR RACCOON
	DCPPCLayerParams->EndOfPlot   =  CRaccoon::Instance()->R_CEOP(DCPPCLayerParams->StartOfPlot,
													Slice->GetWidth(), Slice->GetResolution(),
													m_PrintConfigParams.LastHeadOffset_1200DPI,
													m_PrintConfigParams.AdvanceFire_1200DPI,0);

    if((Slice->GetWidth() * 1200) / Slice->GetResolution() >
            CONFIG_GetPrintTrayXSize_step() / m_ParamsMgr->XStepsPerPixel)
    {
        CQLog::Write(LOG_TAG_PRINT, "Slice X-size out of range.");
        throw ELayerProcess("Slice X-size out of range - probably trying to print a bitmap that is too large.");
    }

    DCPPCLayerParams->NoOfFires   = CalcFiresNum(Slice);
}

void CLayerProcess::FillLayerBidirection150(CSlice *Slice, int NoOfPasses, int StartY, int NoOfFires, PBYTE LayerBuffer)
{
    int CurrentY = StartY;

    if(m_TotalLayersCounter % 2)
        // start print the even layers with offset of NozzleOffest / 2
        CurrentY += 1;

    for(int i = 0; i < NoOfPasses; i++)
    {
        // Perform FW filling
        LayerBuffer = GenerateOnePassData(CurrentY, NoOfFires, Slice, LayerBuffer, true);

        // Perform BW filling
        LayerBuffer = GenerateOnePassData(CurrentY, NoOfFires, Slice, LayerBuffer, false);

        // Compute Y Counter
        CurrentY += HEAD_PRINT_WIDTH_300_DPI;
    }
}

void CLayerProcess::FillLayerBidirection300_8Heads(CSlice *Slice, int NoOfPasses, int StartY,
        int NoOfFires, PBYTE LayerBuffer)
{
    int CurrentY;
    int InterlaceSign;

    if(m_CurrentPrintScanDirection == BACKWARD_PRINT_SCAN)
    {
        InterlaceSign = 1;
        CurrentY = StartY;
    }
    else
    {
        InterlaceSign = -1;
        CurrentY = StartY + m_PrintConfigParams.YInterlaceNoOfPixels;
    }

    for(int i = 0; i < NoOfPasses; i++)
    {
        // Perform FW filling
        LayerBuffer = GenerateOnePassData(CurrentY, NoOfFires, Slice, LayerBuffer, true);

        // Compute Y Counter
        CurrentY += m_PrintConfigParams.YInterlaceNoOfPixels * InterlaceSign;

        // Perform BW filling
        LayerBuffer = GenerateOnePassData(CurrentY, NoOfFires, Slice, LayerBuffer, false);

        // Compute Y Counter
        CurrentY += HEAD_PRINT_WIDTH_300_DPI - m_PrintConfigParams.YInterlaceNoOfPixels * InterlaceSign;
    }
}

void CLayerProcess::FillLayerBidirection300_4Heads(CSlice *Slice, int NoOfPasses, int StartY,
        int NoOfFires, PBYTE LayerBuffer)
{
    int CurrentY;
    int InterlaceSign;
    int Offset1, Offset2;

    CurrentY = StartY;
    /*  ?? should be like BW ?
    if(m_CurrentPrintScanDirection != BACKWARD_PRINT_SCAN)
        CurrentY += m_PrintConfigParams.YInterlaceNoOfPixels;
    */

    // since each 4th layer is being printed with Y shift of -0.5px by the sequencer,
    // we sample here (i.e: move the bmp) one pixel higher, and get the desired shift.
    if(m_ParamsMgr->Shift_600DPI)
        if(m_ParamsMgr->Shift_600DPI_Cycle_4)
            if(Slice->GetSliceNumber() % 4 == 3) // on each 4th layer
                //if (m_TotalLayersCounter % 4 == 3) // on each 4th layer
                CurrentY -= 1;

    InterlaceSign = 1;
    Offset1 = 0;
    Offset2 = (-1) * m_ParamsMgr->YSecondaryInterlaceNoOfPixels;

    for(int i = 0; i < NoOfPasses; i++)
    {
        // Perform FW filling
        LayerBuffer = GenerateOnePassData(CurrentY + Offset1, NoOfFires, Slice, LayerBuffer, true);
        LayerBuffer = GenerateOnePassData(CurrentY + Offset2, NoOfFires, Slice, LayerBuffer, false);

        // Compute Y Counter
        CurrentY += m_PrintConfigParams.YInterlaceNoOfPixels * InterlaceSign;

        LayerBuffer = GenerateOnePassData(CurrentY + Offset1, NoOfFires, Slice, LayerBuffer, true);
        LayerBuffer = GenerateOnePassData(CurrentY + Offset2, NoOfFires, Slice, LayerBuffer, false);

        // Compute Y Counter
        CurrentY += HEAD_PRINT_WIDTH_300_DPI - m_PrintConfigParams.YInterlaceNoOfPixels * InterlaceSign;
    }
}

// todo -oNobody -cNone: ?
// Added for Eden 250, but should work for 500 as well.
/*
void CLayerProcess::FillLayerBidirection600_4Heads(CSlice *Slice,int NoOfPasses,int StartY,
                                            int NoOfFires,PBYTE LayerBuffer)
{

}
*/


// Prepare 600 DPI layer
void CLayerProcess::FillLayerBidirection600_8Heads(CSlice *Slice, int NoOfPasses, int StartY,
        int NoOfFires, PBYTE LayerBuffer)
{
    int CurrentY;
    int InterlaceSign;

    if(m_CurrentPrintScanDirection == BACKWARD_PRINT_SCAN)
    {
        InterlaceSign = 1;
        CurrentY = StartY;
    }
    else
    {
        InterlaceSign = -1;
        CurrentY = StartY + m_PrintConfigParams.YInterlaceNoOfPixels;
    }

    int Offset1, Offset2;

    // Heads group 1...
    if(m_ParamsMgr->FourHeadsGroup == 1)
    {
        Offset1 = 0;
        Offset2 = -2;
    }
    else
    {
        Offset1 = 2;
        Offset2 = 0;
    }

    for(int i = 0; i < NoOfPasses; i++)
    {
        // Perform FW filling
        LayerBuffer = GenerateOnePassData(CurrentY + Offset1, NoOfFires, Slice, LayerBuffer, true);
        LayerBuffer = GenerateOnePassData(CurrentY + Offset2, NoOfFires, Slice, LayerBuffer, false);

        // Compute Y Counter
        CurrentY += m_PrintConfigParams.YInterlaceNoOfPixels * InterlaceSign;

        LayerBuffer = GenerateOnePassData(CurrentY + Offset1, NoOfFires, Slice, LayerBuffer, true);
        LayerBuffer = GenerateOnePassData(CurrentY + Offset2, NoOfFires, Slice, LayerBuffer, false);

        // Compute Y Counter
        CurrentY += HEAD_PRINT_WIDTH_300_DPI * 2 - m_PrintConfigParams.YInterlaceNoOfPixels * InterlaceSign;
    }
}

// Help function for calculating shift factor
// Note: In order to save time, we use bit shift logic for calculating ratios.
//       The calculations is as follows:
//       (HeadXRes * 4) / Factor -> (HeadXRes << 2) >> Factor
int CLayerProcess::CalcResolutionBitShiftFactor(int SliceResolution)
{
    // Prepare bit shift factor for resolution conversion
    int ResShiftFactor = 2;

    if(SliceResolution != 1200)
    {
        int r = 1200 / SliceResolution;

        switch(r)
        {
            case 2:
                ResShiftFactor = 3;
                break;
            case 4:
                ResShiftFactor = 4;
                break;
        }
    }

    return ResShiftFactor;
}

PBYTE CLayerProcess::GenerateOnePassData(int CurrentY, int NoOfFires, CSlice *Slice, PBYTE LayerBuffer, bool Direction)
{
    unsigned ByteIndex;
    int      CurrentXPosition;
    unsigned HeadNum;
    unsigned NozzleNum;
    int      X_NozzlePosition;
    int      Y_NozzlePosition;
	BYTE*    CurrentBmpPTR;
	unsigned MaterialWeightPerTank[TOTAL_NUMBER_OF_CONTAINERS] = {0};
	unsigned *MaterialWeightPerTankPtr = NULL;
	TChamberIndex ChamberIndex;
	TTankIndex    ActiveTank;

	CContainerBase *Container = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();

    static int BufferNumber = 0;
    static unsigned int preemptCount = 0;

    PBYTE    DestPtr = LayerBuffer;

    // ?? write the Y loation to log file, to compare with Axis Y movement from Sequencer.
    CQLog::Write(LOG_TAG_PROCESS, "(Axis Y) Data Y in location= %d", CurrentY);

    if(m_ParamsMgr->GenerateExcelAnalyzer)
        CExcelSequenceAnalyzer::Init(Q2RTApplication->AppFilePath.Value() + string("SequencerAnalyzer.csv"), 10, 10);
    else
        CExcelSequenceAnalyzerDummy::Init();

    CExcelSequenceAnalyzerBase *csv = CExcelSequenceAnalyzer::Instance();

    csv->SetValueNext(SourceLayerProcess, CurrentY);

    int SliceWidth  = Slice->GetWidth();
    int SliceHeight = Slice->GetHeight();

    int ResShiftFactor = CalcResolutionBitShiftFactor(Slice->GetResolution());

    // Forward
    if(Direction)
        // Initialize the X_counter
        CurrentXPosition = 0;
    else
        // Backward
        CurrentXPosition = ((NoOfFires - 1) * Slice->GetResolution()) / m_PrintConfigParams.PrintRes_X;

    int DirectionSign = Direction ? 1 : -1;

    // Perform for each fire
    for(int FireIndex = 0; FireIndex < NoOfFires; FireIndex++)
    {
#ifdef HEAD_SIMULATION
        m_EventSimulationStartLayerProcessing.WaitFor();

        // for use by the head simulator:
        m_LastFireData = DestPtr;
#endif
        if(preemptCount == 1000)
        {
            QSleep(0);
            preemptCount = 0;
        }
        else preemptCount++;
        if(FireIndex != 0)
        {
            if(Slice->GetResolution() >= m_PrintConfigParams.PrintRes_X)
                CurrentXPosition += DirectionSign * (Slice->GetResolution() / m_PrintConfigParams.PrintRes_X);
            else if(((FireIndex * Slice->GetResolution()) % m_PrintConfigParams.PrintRes_X) == 0)
                CurrentXPosition += DirectionSign;
        }

        // Start from head 0
        HeadNum   = 0;
        NozzleNum = 0;

        // CurrentHeadsMapEntry is a matrix of X and Y nozzle offsets for ALL nozzles: [HEADS_NUM] X [NOZZLES_PER_HEAD]
        THeadMapEntry* CurrentHeadsMapEntry = m_HeadMapArr_1200DPI[0];

		// Each Head contains 96 nozzles - fill one fire
        for(ByteIndex = 0; ByteIndex < HEADS_FILL_SIZE_BYTE; ByteIndex++)
		{
			// Check if it is the next head
            if((ByteIndex % HEAD_SIZE_BYTE) == 0)
			{
				// Don't increment the HeadNum for the first time
                if(NozzleNum != 0)
                {
                    HeadNum++;
                    NozzleNum = 0;
				}
				
				ChamberIndex             = GetHeadsChamber(HEADNUM_LP_TO_GENERAL(HeadNum), m_ParamsMgr->PrintingOperationMode, true);
				CurrentBmpPTR            = Slice->GetBuffer(ChamberIndex);
				ActiveTank               = Container->GetActiveTankNum(ChamberIndex);
				MaterialWeightPerTankPtr = &MaterialWeightPerTank[ActiveTank];
			}

            // Compute for each BYTE - each bit indicate one nozzle
            for(int BitIndex = 7; BitIndex >= 0; BitIndex--)
            {
                // Compute the position of the nozzle on the BMP in X and Y axes
                X_NozzlePosition = (int)(CurrentXPosition + ((CurrentHeadsMapEntry->Offset_X_1200DPI << 2) >> ResShiftFactor));
                Y_NozzlePosition = CurrentY +  CurrentHeadsMapEntry->Offset_Y;

                CurrentHeadsMapEntry++;
                /*
                        // Debugging only !!! remove !!!
                        if (X_NozzlePosition == 70 && Y_NozzlePosition >= 0 && Y_NozzlePosition <= 10)
                           X_NozzlePosition = X_NozzlePosition;   // break here.
                */
                // Check if the nozzle is not out of BMP
                if(((X_NozzlePosition >= 0) && (X_NozzlePosition < SliceWidth)) && ((Y_NozzlePosition >= 0) && (Y_NozzlePosition < SliceHeight)))
                {
                    // Check if bit in source data is set
					if(*(CurrentBmpPTR + ((Y_NozzlePosition * SliceWidth + X_NozzlePosition) >> 3)) & (0x80 >> (X_NozzlePosition & 7)))
					{
                        // Set bit in destination
						*DestPtr |= (BYTE)(1 << BitIndex);

						// Collect the consumed material weight for corresponding active tank of each bitmap.
						// Counting the pixels here for efficiency and multiplying later by weight
						(*MaterialWeightPerTankPtr)++;
                    }
				}
			}

            NozzleNum += 8;
			DestPtr++;
        }
#ifdef HEAD_SIMULATION
        m_EventSimulationStartLayerProcessing.Reset();
        m_EventSimulationStartSimulator.Set();
#endif
    }
    if(m_ParamsMgr->DumpPassToFile ||  FindWindow(0, "DumpPassToFile.txt - Notepad"))
    {
        QString FileName;
        FILE *out = NULL;

        FileName = Q2RTApplication->AppFilePath.Value() + "Pass_" + QIntToStr(BufferNumber);
        out = fopen(FileName.c_str(), "wb");

        fwrite(LayerBuffer, sizeof(BYTE), ((char*)DestPtr - (char*)LayerBuffer), out);

        BufferNumber++;

        if(out)
            fclose(out);
	}

	// Notify each tank of consumed weight
	for(int i = FIRST_TANK_TYPE; i < TOTAL_NUMBER_OF_CONTAINERS; i++)
	{		
		if(MaterialWeightPerTank[i] > 0)
		{
			MaterialWeightPerTank[i] *= m_ParamsMgr->RaccoonSingleDropWeight_nGr;
			Container->UpdateConsumption(static_cast<TTankIndex>(i), NanoGramsToMilliGrams(MaterialWeightPerTank[i]), Slice->GetSliceNumber());
		}
	}

    return DestPtr;
}

// Create a scattering object
void CLayerProcess::CreateScatterObject(void)
{
    // If a scatter object has already  been allocated, get rid of it
    if(m_CurrentScatterGenerator != 0)
        delete m_CurrentScatterGenerator;

    if(m_ParamsMgr->ScatterEnabled)
        switch(m_ParamsMgr->ScatterAlgorithm)
        {
            case SEQUENTIAL_SCATTER:
                m_CurrentScatterGenerator = new CSequentialScatter(0, 0);
                return;

            case RANDOM_SCATTER:
                m_CurrentScatterGenerator = new CRandomScatter(0, 0);
                return;
        }

    // No scattering (default scatter object)
    m_CurrentScatterGenerator = new CScatterGenerator();
}

void CLayerProcess::PrepareForProcess(void)
{
    PrepareConfigParams();
    CreateScatterObject();

    // Cleat the total passes statistics variable
    CLayer::ClearTotalPassesCounter();
}

// Release the specified layer
void CLayerProcess::DoneWithLayer(CLayer *Layer)
{
    m_LayersAllocators.ReleaseBlock(Layer->GetBuffer());
    CQLog::Write(LOG_TAG_PROCESS, "Done with layer");
    //delete Layer; //bug 5885
}

// Calculate the number of layers to generate from a given slice
int CLayerProcess::GetCurrentSliceLayersNum(float CurrentSliceHeightZ)
{
    int LayersNum;

    // Compute the number of layers of from slice
    if(m_PrintConfigParams.LayerHeight_mm != 0)
    {
        LayersNum = (int)((CurrentSliceHeightZ +  m_SliceHeightZReminder) / m_PrintConfigParams.LayerHeight_mm);
        m_SliceHeightZReminder  = (float) fmod((double)(CurrentSliceHeightZ +  m_SliceHeightZReminder),
                                               (double)m_PrintConfigParams.LayerHeight_mm);
    }
    else
        LayersNum = 1;

    return LayersNum;
}

// Delete all layers currently in the layers queue
void CLayerProcess::FlushLayers(void)
{
    m_LayersQueue->UnRelease();

    while(m_LayersQueue->ItemsCount > 0)
    {
        CLayer *TmpPtr;

        if(m_LayersQueue->Receive(TmpPtr) != QLib::wrReleased)
        {
            if((TmpPtr != NULL) && (TmpPtr != (CLayer *)LAST_SLICE_MARKER))
                DoneWithLayer(TmpPtr);
            //  delete TmpPtr;
        }
        else
            break;
    }//bug 5885
}

void CLayerProcess::MaskHeadData(int HeadIndex, int TotalNumberOfFires, PBYTE LayerBuffer)
{
    LayerBuffer += HEAD_SIZE_BYTE * HeadIndex;
    for(int i = 0; i < TotalNumberOfFires; i++)
    {
        memset(LayerBuffer, 0, HEAD_SIZE_BYTE);
        LayerBuffer += HEADS_FILL_SIZE_BYTE;
    }
}

// Mask the unused 4 heads (4 heads printing in an 8 heads system)
void CLayerProcess::Mask4Heads(int TotalNumberOfFires, PBYTE LayerBuffer)
{
    for(int i = 0; i < TotalNumberOfFires; i++)
    {
        for(int j = 0; j < 4; j++)
            MaskHeadData(mapYOffset[j * 2 + m_ParamsMgr->FourHeadsGroup], TotalNumberOfFires, LayerBuffer);
    }
}

void CLayerProcess::DisplayStatistics(void)
{
    CQLog::Write(LOG_TAG_PROCESS, "Total number of passes: %d/%d", CLayer::GetActualPassesCounter(), CLayer::GetTotalPassesCounter());
}

CQEvent* CLayerProcess::GetEventSimulationStartLayerProcessing()
{
    return &m_EventSimulationStartLayerProcessing;
}
CQEvent* CLayerProcess::GetEventSimulationStartSimulator()
{
    return &m_EventSimulationStartSimulator;
}

////////////////////////
// ScatterTetser code //
////////////////////////

TQErrCode CScatterTester::DoCompleteScatterTest()
{
    int MaxSmall, MaxMedium, MaxLargeOnSmallTray, MaxLarge;

    PrepareForTest();
    CScatterGenerator* Scatter = dynamic_cast<CScatterGenerator*>(m_LayerProcess->m_CurrentScatterGenerator);

    MaxSmall =  Scatter->GetSmallSliceMaxSize();
    MaxMedium = Scatter->GetMediumSliceMaxSize();
    MaxLarge =  Scatter->GetLargeSliceMaxSize();

    CQLog::Write(LOG_TAG_PROCESS, "Slice's max size per type: Small: %d, Medium: %d, LargeOnSmallTray: %d, Large: %d", MaxSmall, MaxMedium, MaxLargeOnSmallTray, MaxLarge);

    /*
            We want to cover the cases listed in the table below.

            When FirstSlice is:    Other Slice types (in Same Model) that should be tested are:
            ==================================================================================
            Large                  Medium, Small
            Medium                 Small
            Small                  N/A
    */

    SetEmulatedModel(emsRearPrism, 1, MaxLarge);
    TestScatterOnEmulatedModel();

    SetEmulatedModel(emsPyrmaid, 1, MaxLarge);
    TestScatterOnEmulatedModel();

    SetEmulatedModel(emsPyrmaid, 1, MaxMedium);
    TestScatterOnEmulatedModel();

    SetEmulatedModel(emsPyrmaid, 1, MaxSmall);
    TestScatterOnEmulatedModel();

    CleanUp();
    QMonitor.NotificationMessage("The Scatter-Tester has successfully finished.");

    return Q_NO_ERROR;
}

TQErrCode CScatterTester::CleanUp()
{
    CModesManager::Instance()->GotoDefaultMode();

    if(m_Layer)
        delete m_Layer;

    if(m_Slice)
        delete m_Slice;

    return Q_NO_ERROR;
}

TQErrCode CScatterTester::SetEmulatedModel(TEmulatedModelStyle ModelStyle, unsigned int EndHeight, unsigned int StartHight)
{
    m_EmulatedModelStyle = ModelStyle;
    m_EndHeight = EndHeight;
    m_StartHeight = StartHight;

    return Q_NO_ERROR;
}

bool CScatterTester::GetNextSliceRect(unsigned int &height, unsigned int &SliceOffset)
{
    // Increment Heights and Offsets according to requested Model Style
    switch(m_EmulatedModelStyle)
    {
        case emsRearPrism:
            if((height > m_EndHeight) && (height >= 2))
            {
                height -= 1;
                SliceOffset = 0;
            }
            else
                return false;
            break;

        case emsFrontPrism: // todo -oNobody -cNone: ?
            break;

        case emsPyrmaid:
            if((height > m_EndHeight) && (height >= 2))
            {
                height -= 2;
                SliceOffset++;
            }
            else
                return false;
            break;

    }

    return true;
}

TQErrCode CScatterTester::PrepareForTest()
{
    m_StartYPosition;
    m_YWorstMotorLocation = 0;
    m_MinMotorLocation = 10000;
    m_EmulatedModelStyle = emsPyrmaid;
    m_EndHeight = 1;
    m_StartHeight = 2;

    // Entering mode according to the print resolution
    CModesManager::Instance()->GotoDefaultMode();
    CModesManager::Instance()->EnterMode(PRINT_MODE[0][1], MACHINE_QUALITY_MODES_DIR(0, 1)); // Entering DM Mode. (If needed for a certain test, it might me (hard-coded) changed to 'Single mode' (0,0))

    m_MachineSequencer = Q2RTApplication->GetMachineManager()->GetMachineSequencer();
    m_Scatter = NULL;
    m_Slice = new CSlice(0, 0, 0, 0, 0, 0, 0, 300);
    m_Layer = new CLayer;

    // PrepareForProcess() must be called to initialize some needed values. (incl. the scatter object)
    m_LayerProcess->PrepareForProcess();
    m_Scatter = dynamic_cast<CRandomScatter*>(m_LayerProcess->m_CurrentScatterGenerator);

    // Stop all writing to Log and Monitor. (otherwise it would take too long to run)
    // todo -oNobody -cNone: ?
    return Q_NO_ERROR;
}


TQErrCode CScatterTester::TestScatterOnEmulatedModel()
{
    unsigned int TableSize, evenodd, height, SliceOffset, LayerMemSize;
    int  YScatterOffset, ScatterOnly, XLayerStart1200DPI, YLayerStart, NoOfPasses;
    BYTE DummyLayerBuffer[0xFF];

    height = m_StartHeight;
    SliceOffset = 0;

    memset(DummyLayerBuffer, 0, sizeof(DummyLayerBuffer));

    do // ran through desired slice heights, and SliceOffsets.
    {
        // Since the TestScatterOnEmulatedModel is probably run through MiniDebugger, main thread is blocked.
        Q2RTApplication->YieldMainThread();

        QMonitor.Print(QString("Begin slice height: ") + QIntToStr(height));
        // Following init needed since SliceOffset is considered later at: CalculateLayerStartXY()
        m_Slice->Init((PBYTE*)DummyLayerBuffer, 0, 0, height, 0, 0, SliceOffset, 300);

        m_Scatter->DontShuffle();

        m_Scatter->ScatterCompute(
            height ,        // Slice's Y dimension
            SliceOffset,    // value as received from Host (or file).
            false);         // IsContinue
        m_LayerProcess->m_LayersAllocators.ReleaseBlocks(); //bug 5885

        int* scatterTable = m_Scatter->GetSortedScatterTable(TableSize);

        if(!TableSize)
            return Q_NO_ERROR;
        if(scatterTable)
            QMonitor.Print(QString("   Scatter min: ") + QIntToStr(scatterTable[0]) + " max: " + QIntToStr(scatterTable[TableSize - 1]) + " unique values: " + QIntToStr(TableSize));

        for(evenodd = 0; evenodd <= 1; evenodd++)
        {
            // m_LayerProcess->AllocateLayersMemory(m_Slice);
            unsigned int maxPass = 0;    // for statistics report.
            unsigned int minPass = 100;  // for statistics report.
            // Test all possible scatter values for this image height.
            for(unsigned int i = 0; i < TableSize; i++)
            {
                m_Scatter->ScatterGetNext(
                    height,         // Slice's Y dimension
                    SliceOffset,    // YOffset
                    NoOfPasses,     // output param. (not used here)
                    YScatterOffset, // output param.
                    ScatterOnly,    // output param. (not used here)
                    evenodd);       // evenodd layer

                if(NoOfPasses > maxPass)
                    maxPass = NoOfPasses;
                if(NoOfPasses < minPass)
                    minPass = NoOfPasses;
                m_LayerProcess->CalculateLayerStartXY(m_Slice, YScatterOffset, XLayerStart1200DPI, YLayerStart);

                m_LayerProcess->PrepareOHDBParams(m_Slice, XLayerStart1200DPI, m_Layer->GetDPCPCParams());
                /*
                        // Following lines instead of calling: PrepareOHDBParams()
                        m_Layer->GetDPCPCParams()->StartOfPlot = 0;
                        m_Layer->GetDPCPCParams()->EndOfPlot   = 1;
                        m_Layer->GetDPCPCParams()->NoOfFires   = 1;
                */
                // The memory allocation
                LayerMemSize = m_LayerProcess->CalculateLayerSize(m_Layer->GetDPCPCParams()->NoOfFires, NoOfPasses);

                // Allocate memory for the layer
                PBYTE LayerBufferTmpPtr = (PBYTE)m_LayerProcess->m_LayersAllocators.GetBlock(LayerMemSize, true);

                // Check allocation
                if(LayerBufferTmpPtr == NULL)
                {
                    TQErrCode ErrCode = Q2RT_MEMORY_ALLOCATION_ERROR;
                    if(!QMonitor.AskYesNo("The Scatter-Tester encountered the following problem: \"" + PrintErrorMessage(ErrCode) + "\"" \
                                          " 'NO' will skip to the next test. 'YES' will continue."))
                    {
                        return Q_NO_ERROR;
                    }
                }

                m_Layer->Init(
                    LayerBufferTmpPtr,         // DummyLayerBuffer,
                    LayerMemSize,              // sizeof(DummyLayerBuffer),
                    evenodd,                   // Slice->GetSliceNumber()
                    9999,                      // m_TotalLayersCounter
                    0,                         // SliceLayerIndex
                    NoOfPasses,
                    YLayerStart,
                    ALTERNATE_PRINT_SCAN);     // m_CurrentPrintScanDirection

                // In case of "Y calculation error", folllowing try{} will generate an exception.
                try
                {
                    m_MachineSequencer->TestPrintSequencer(m_Layer);
                }
                catch(EQException& err)
                {
                    if(!QMonitor.AskYesNo("The Scatter-Tester encountered the following problem: \"" + err.GetErrorMsg() + "\"" \
                                          " 'NO' will skip to the next test. 'YES' will continue."))
                    {
                        return Q_NO_ERROR;
                    }
                }
                m_LayerProcess->DoneWithLayer(m_Layer);
            }
            QMonitor.Print(QString("End slice height: ") + QIntToStr(height) + " Max passes: " + QIntToStr(maxPass));
            // The Clear() forces the new layer, which is smaller then current, to reallocate its memory. This should allow catching allocation related problems.
            // m_LayerProcess->m_LayersAllocators.DeInit(); // not needed anymore once we allocate max layers in ctor.
        }
    }
    while(GetNextSliceRect(height, SliceOffset));

    return Q_NO_ERROR;
}

void CLayerProcess::ThreadExceptionHandlerCallback(CQThread *ThreadPtr, EQException Exception, TGenericCockie Cockie)
{
    CLayerProcess *InstancePtr = reinterpret_cast<CLayerProcess *>(Cockie);

    CMachineManager * MachineManager = Q2RTApplication->GetMachineManager();
    CBackEndInterface* BackEnd = CBackEndInterface::Instance();
    CActuatorBase *Actuators   = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetActuatorInstance();
    CRollerBase *Roller        = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetRollerInstance();
    CHeadFillingBase *HeadFilling = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadFillingInstance();
    CHeadHeatersBase *HeadHeaters = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadHeatersInstance();

    //1. UV Turn off
    BackEnd->TurnUVLamps(false);
    //2. HeadFilling Turn off
    HeadFilling->HeadFillingOnOff(false);

    //3. HeadHeaters Turn off
    HeadHeaters->SetDefaultOnOff(HEAD_HEATER_OFF);

    //4. Roller Turn off
    //Roller->SetRollerOnOff(false);
    BackEnd->TurnRoller(false);

    //5. Actuators Turn off
    //for(int i=0;i<MAX_ACTUATOR_ID;i++)
    //RIGHT_LAMP_ACTUATOR_ID            26
    Actuators->SetOnOff(26, false);
    //LEFT_LAMP_ACTUATOR_ID             25
    Actuators->SetOnOff(25, false);

    //6. Door Un-Lock
    Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetDoorInstance()->Disable();
    CQLog::Write(LOG_TAG_GENERAL, "Un-Expected exception callBack (Layer Process).");
    QMonitor.ErrorMessageWaitOk("An error has occurred.\nClose and restart the printer application.");
}


//ThreadUnExpectedException
void CLayerProcess::ThreadUnExpectedExceptionHandlerCallback(CQThread *ThreadPtr, TGenericCockie Cockie)
{

    CLayerProcess *InstancePtr = reinterpret_cast<CLayerProcess *>(Cockie);

    CMachineManager * MachineManager = Q2RTApplication->GetMachineManager();
    CBackEndInterface* BackEnd = CBackEndInterface::Instance();
    CActuatorBase *Actuators   = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetActuatorInstance();
	CRollerBase *Roller        = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetRollerInstance();
	CHeadFillingBase *HeadFilling = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadFillingInstance();
    CHeadHeatersBase *HeadHeaters = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadHeatersInstance();

    //1. UV Turn off
    BackEnd->TurnUVLamps(false);
    //2. HeadFilling Turn off
    //Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadFillingInstance()->HeadFillingOnOff(false);
    HeadFilling->HeadFillingOnOff(false);

    //3. HeadHeaters Turn off
	//InstancePtr->m_MachineSequencer->CancelHeadHeating();
    //Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadHeatersInstance()->SetDefaultOnOff(HEAD_HEATER_OFF);
    HeadHeaters->SetDefaultOnOff(HEAD_HEATER_OFF);

    //4. Roller Turn off
    //Roller->SetRollerOnOff(false);
    BackEnd->TurnRoller(false);

    //5. Actuators Turn off
    //for(int i=0;i<MAX_ACTUATOR_ID;i++)
    //RIGHT_LAMP_ACTUATOR_ID            26
    Actuators->SetOnOff(26, false);
    //LEFT_LAMP_ACTUATOR_ID             25
    Actuators->SetOnOff(25, false);

    //6. Door Un-Lock
    Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetDoorInstance()->Disable();
    CQLog::Write(LOG_TAG_GENERAL, "Un-Expected exception callBack (Layer Process).");
    QMonitor.ErrorMessageWaitOk("An error has occured.\nClose and restart the printer application.");
}



