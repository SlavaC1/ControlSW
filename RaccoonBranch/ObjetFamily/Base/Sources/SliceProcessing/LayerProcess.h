/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Layers processing thread (slice to layers process)       *
 * Module Description:                                              *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Rachel / Ran                                             *
 * Start date: 08/01/2002                                           *
 * Last upate: 19/09/2002                                           *
 ********************************************************************/

#ifndef _LAYER_PROCESS_H_
#define _LAYER_PROCESS_H_

#include "QThread.h"
#include "QMessageQueue.h"
#include "LayerProcessDefs.h"
#include "BlocksAllocator.h"

/* E500
HEAD_0  M0_
HEAD_1  M1_
HEAD_2  M2_
HEAD_3  M3_
HEAD_4  S0_
HEAD_5  S1_
HEAD_6  S2_
HEAD_7  S3_
*/

// Max number of elements for the layers queue
const int MAX_LAYERS_QUEUE_SIZE = 3; //bug 5885

// Exception class for the CLayerProcess class
class ELayerProcess : public EQException {
  public:
    ELayerProcess(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

typedef enum {glrOk,glrQueueReleased,glrTimeout,glrUnknownError} TGetLayerResult;

// TEmulatedModelType is used in ScatterTester only, which means it is meant for... Testing. 
typedef enum {emsPyrmaid,emsRearPrism,emsFrontPrism} TEmulatedModelStyle;

// Class forwards
class CPrintJob;
class CLayer;
class CSlice;
class CScatterGenerator;
class CAppParams;
class CScatterTester;
class CRandomScatter;
class CMachineSequencer;

class CLayerProcess : public CQThread {
  private:
    // Machine manager commands queue
    typedef enum {cmdStart,cmdStop,cmdDummy} TLayersProcessCommands;

    // Machine manager commands queue
    typedef CQMessageQueue<TLayersProcessCommands> TCommandsQueue;

    // Type for a layers queue
    typedef CQMessageQueue<CLayer *> TLayersQueue;

    // Queue object pointers
    TCommandsQueue *m_CommandsQueue;
    TLayersQueue *m_LayersQueue;

    CLayer * m_CurrentLayer[MAX_LAYERS_QUEUE_SIZE*2]; //bug 5885
    int m_layerIndex; //bug 5885

    // Pointer to the current print job
    CPrintJob *m_CurrentPrintJob;

    // Pointer to the parameters
    CAppParams *m_ParamsMgr;

    // Current print related configuration parameters (updated on Reset() )
    TPrintConfigParams m_PrintConfigParams;

    int m_XCloneCounter;

    // Current processed layer number (total)
    int m_TotalLayersCounter;

    // First slice offset related to tray start position
    int m_XFirstSliceOffset,m_YFirstSliceOffset;

    float m_SliceHeightZReminder;

    // True if currently paused
    bool m_Paused;

    // Pointer to the current scattering object
    CScatterGenerator *m_CurrentScatterGenerator;

    THeadMapEntry m_HeadMapArr_1200DPI[HEADS_NUM][NOZZLES_PER_HEAD];

    TPrintScanDirection m_CurrentPrintScanDirection;

    // Memory manager for layers
    CBlocksAllocator m_LayersAllocators;

    // m_LastFireData holds a 8head X 96Nozzles of data. This is used
    // by the head simulator.
    PBYTE m_LastFireData;

    CQEvent m_EventSimulationStartLayerProcessing;
    CQEvent m_EventSimulationStartSimulator;

     // Calculate the number of fires required for a given slice
    unsigned CalcFiresNum(CSlice *Slice);
    unsigned CalculateLayerSize(unsigned FiresNum, unsigned PassesNum);

    void AllocateLayersMemory(CSlice *FirstSlice);
    void AllocateLayersMemory(); //bug 5885

    void FlushLayers(void);

    // Compute the parameters that the DPC-PC needs before
    void PrepareOHDBParams(CSlice *Slice,int StartCounter1200DPI_X,TDPCPCLayerParams* DCPPCLayerParams);

    // Calculate slice start position on the tray (PEG)
    void CalculateLayerStartXY(CSlice *Slice,int YScatterOffset,int& X,int& Y);

    // Prepare the configuration-related parameters block for printing
    void PrepareConfigParams(void);

    // Create a scattering object
    void CreateScatterObject(void);

    void PrepareForProcess(void);

    // Thread execute function (override)
    void Execute(void);

    // Help function for calculating shift factor
    // Note: In order to save time, we use bit shift logic for calculating ratios.
    //       The calculations is as follows:
    //       (HeadXRes * 4) / Factor -> (HeadXRes << 2) >> Factor
    int CalcResolutionBitShiftFactor(int SliceResolution);

    PBYTE GenerateOnePassData(int CurrentY,int NoOfFires,CSlice *Slice,PBYTE LayerBuffer,bool Direction);

    // Layer filling routines
    void FillLayerBidirection(CSlice *Slice,int NoOfPasses,int StartY,int NoOfFires,PBYTE LayerBuffer);
    void FillLayerBidirection150(CSlice *Slice,int NoOfPasses,int StartY,int NoOfFires,PBYTE LayerBuffer);
    void FillLayerBidirection300(CSlice *Slice,int NoOfPasses,int StartY,int NoOfFires,PBYTE LayerBuffer);
    void FillLayerBidirection600(CSlice *Slice,int NoOfPasses,int StartY,int NoOfFires,PBYTE LayerBuffer);

    void FillLayerBidirection300_8Heads(CSlice *Slice,int NoOfPasses,int StartY,int NoOfFires,PBYTE LayerBuffer);
    void FillLayerBidirection300_4Heads(CSlice *Slice,int NoOfPasses,int StartY,int NoOfFires,PBYTE LayerBuffer);
    void FillLayerBidirection600_8Heads(CSlice *Slice,int NoOfPasses,int StartY,int NoOfFires,PBYTE LayerBuffer);

    // Calculate the number of layers to generate from a given slice
    int GetCurrentSliceLayersNum(float CurrentSliceHeightZ);

    // Process a single slice
    void ProcessSlice(CSlice *Slice, bool IsContinue);

    // Mask the unused 4 heads (4 heads printing in an 8 heads system)
    void MaskHeadData(int HeadIndex, int TotalNumberOfFires,PBYTE LayerBuffer);
    void Mask4Heads  (int TotalNumberOfFires,PBYTE LayerBuffer);

    // luda Callback for Expected error - for turning off everything on print exception
    static void ThreadExceptionHandlerCallback(CQThread *ThreadPtr,EQException Exception, TGenericCockie Cockie );
    // luda Callback unexpected - for turning off everything on print exception
    //ThreadUnExpectedException
    static void ThreadUnExpectedExceptionHandlerCallback(CQThread *ThreadPtr, TGenericCockie Cockie );

    CScatterTester* m_ScatterTester;

    friend class CScatterTester;

  public:
    // Constructor
    CLayerProcess(void);

    // Destructor
    ~CLayerProcess(void);

    // Reset internal state
    void Reset(void);

    // Set a new working job (and start processing)
    void SetNewPrintJob(CPrintJob *NewJob);

    // Stop processing
    void StopProcessing(void);

    // Pause processing
    void PauseProcessing(void);

    // Resume processing after pause
    void ResumeProcessing(void);

    // Get next (processed) layer
    CLayer *GetLayer(TGetLayerResult& GetLayerResult);

    // Release the specified layer
    void DoneWithLayer(CLayer *Layer);

    void DisplayStatistics(void);

    CQEvent* GetEventSimulationStartLayerProcessing();
    CQEvent* GetEventSimulationStartSimulator();

};

class CScatterTester : public CQComponent {

private:

  int  m_ScatterSize;
  long m_StartYPosition;
  long m_YWorstMotorLocation;
  long m_MinMotorLocation;

  TEmulatedModelStyle m_EmulatedModelStyle;
  unsigned int m_EndHeight;
  unsigned int m_StartHeight;

  CLayerProcess* m_LayerProcess;
  CMachineSequencer *m_MachineSequencer;
  CRandomScatter* m_Scatter;
  CSlice* m_Slice;
  CLayer *m_Layer;

  bool GetNextSliceRect(unsigned int &height, unsigned int &SliceOffset);

public:

  CScatterTester(CLayerProcess* LayerProcObj, const QString& Name,bool RosterRegistration)
   : m_LayerProcess(LayerProcObj), CQComponent(Name,RosterRegistration)
   {
    INIT_METHOD(CScatterTester,DoCompleteScatterTest);
   }

  ~CScatterTester(){};
  TQErrCode PrepareForTest(); // todo -oNobody -cNone: Scatter Type (random... )
  TQErrCode SetEmulatedModel(TEmulatedModelStyle ModelStyle, unsigned int EndHeight, unsigned int StartHight);
  TQErrCode TestScatterOnEmulatedModel();
  TQErrCode CleanUp();

  DEFINE_METHOD(CScatterTester,TQErrCode,DoCompleteScatterTest);
};

#endif

