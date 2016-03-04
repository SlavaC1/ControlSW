/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Host-communication                                       *
 * Module Description: This is cross-platform implementation of the *
 *                     host communicaiton thread used in Q1.        *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 10/02/2001                                           *
 * Last upate: 24/11/2002                                           *
 ********************************************************************/

#ifndef _HOST_COMM_H_
#define _HOST_COMM_H_

#include "QMessageQueue.h"
#include "QThread.h"
#include "QTimer.h"
#include "HostPrintJob.h"
#include "HostMessages.h"
#include "BackEndInterface.h"
#include "BlocksAllocator.h"
#include "QSafeVariable.h"
#include "CFlexibleParameters.h"

class CQTcpIpServer;
class CSlice;
class CHostComm;
class CConfigBackup;
class CMachineManager;
class CTempAppParams;

const int SLICE_QUEUE_SIZE = 3;// bug 6171
typedef CQMessageQueue<BYTE *> TSliceDataMessageQueue;
// Possible error codes for the decode and command functions
typedef enum {ceNoError,ceIllegalCommand,cePayloadAlloc,ceEmptySlice,ceIllegalCommandField,ceCommError,ceOnTimeError} TCommErrorCodes;

class CSlicePreProcessor : public CQThread
{
  private:

    int m_LastPositionY;
    // Message queue for the ready slices
    // Note: This queue object is created by this (CSlicePreProcessor) class.
    TSliceMessageQueue *m_SliceMessageQueue;

    CHostComm *m_HostComm;

    int m_TotalSlicesNum;

    // Slice preview callback event and cockie
    TGenericCallback m_SlicePreviewEvent;
    TGenericCockie m_SlicePreviewEventCockie;

    CQMutex* m_LastSliceAccessMutex;
    CSlice*  m_LastIncomingSlice;
    
    bool m_PrintEndFlag;
    CQMutex *m_PrintEndFlagMutex;

    bool m_isLastSlice;//itamar 5644

    // Memory manager for slices
    CBlocksAllocator m_SlicesAllocator;

#ifdef OBJET_MACHINE
  int m_BmpsReceivedFromHostArr[NUMBER_OF_CHAMBERS];
  short m_NumOfAddModelResins; //check if the Host sent valid NumOfAdditional resins
#endif
    

    void ReleaseBuffers(PBYTE TmpBuffers[], int LastIndex);

    bool AllocateTmpBuffers(PBYTE TmpBuffers[],ULONG SliceUncompressedSize);

    bool UnpackNonEmptySlice(TSlicePackageType* SlicePackage,PBYTE TmpBuffers[],ULONG SliceUncompressedSize);

    bool UnpackEmptySlice(TSlicePackageType* SlicePackage,PBYTE TmpBuffers[]);

    bool UnpackSlice(BYTE CompressionType,PBYTE RawSliceData,ULONG RawDataSize,
                     PBYTE UnpackedSliceData,ULONG MaxUnpackedSize);


    // Thread execute function (override)
    void Execute(void);

    bool IsPrintEnd(void);	

  public:
    // Constructor
    CSlicePreProcessor(CHostComm *HostComm);

    // Destructor
	virtual ~CSlicePreProcessor(void);

    // Set the total number of slices in the current job
    void SetTotalSlicesNum(int TotalSlicesNum);
    // Set the total number of slices in the current job
    int GetTotalSlicesNum();

    void SetPrintEndFlag(bool Flag);
    bool IsLastSlice(); //itamar 5644
    // Return a pointer to the internal slice message queue
    TSliceMessageQueue *GetSliceMessageQueue(void);

    // Delete all slices currently in the slice queue
    void FlushSlices(void);

	void SetSlicePreviewUpdateEvent(TGenericCallback SlicePreviewEvent,TGenericCockie SlicePreviewEventCockie);

	CSlice *GetLastSliceAndLock(void);
	void ReleaseLastSliceLock(void);

	void DisposeSlice(CSlice *Slice);
	void ProcessSlicePackage(TSlicePackageType *SlicePackage);
	void ResetLastPositionY(void);
#ifdef OBJET_MACHINE
	void SetBmpsReceivedFromHostArr(int BmpsReceivedFromHost[], short NumOfAddModelResins);
#endif
};

class CHostComm : private CQThread
{
  friend CSlicePreProcessor;

private:

#define DEFINE_COMMAND(Name, Cmd) Name=Cmd,
  enum HostCommands   // Message Opcodes for the Command field.
{
#include "HostCommands.def"

      CMD_LAST_INDEX,
      FIRST_CMD           = CMD_STATUS,
      LAST_CMD            = CMD_LAST_INDEX-1,
};
#undef DEFINE_COMMAND

  struct TTag2HostMessage
  {
     HostCommands Cmd;
     unsigned     Length;
     void *       Data;
  };

  typedef vector<TTag2HostMessage> TTag2HostMessageQueue;
  typedef TTag2HostMessageQueue::iterator TTag2HostMessageQueueIterator;

  TTag2HostMessageQueue HostMessageQueue;
  map<int,QString> HostOpcode2Str;
  #define VALID_HOST_COMMAND(Cmd) (Cmd != CMD_DUMMY)
  
  // Variables used to store message data
  BYTE                    m_IncomingMsgCommand;
  ULONG                   m_IncomingMsgLength;
  PBYTE                   m_IncomingMsgData[SLICE_QUEUE_SIZE];//bug 6171
  int                     m_IncomingMsgDataIndex;//bug 6171
  PBYTE                   m_IncomingMsgResinData; //itamar 5811
  ULONG                   m_CurrJobId;
  unsigned                m_AllocatedLength;
  bool                    m_IsMute;
  void *                  m_Data;

  CMachineManager*        m_MachineManager;
  // TCP/IP server connection object
  CQTcpIpServer*          m_Comm;
  CQMutex                 m_MessageReceiveMutex;  
  CQMutex                 m_HostCommandsMutex;
  bool                    m_MessageIsBeingReceived;  
  CConfigBackup*          m_ConfigBackup;
  // Message queue for transfering the slice data from the communication thread to the pre-process thread
  TSliceDataMessageQueue* m_SliceDataMessageQueue;
  // This flag is used as a Flip-Flop to indicate first slice in a new or recovered print
  CSlicePreProcessor*     m_SlicePreProcessor;
  CQTimer                 m_TimeoutTimer;         
  bool                    m_OnlineOffline;
  bool                    m_OnlineMessagePending;
  bool                    m_ModelIsBeingReceived;

  CFlexibleParameters m_flexibleParameters;
  
  // A safe flag to indicate that the host has send a 'pause' command
  CQSafeVariable<bool>    m_Paused;

  CTempAppParams * m_ParamsMngr; // bug 5719

  static void TimeoutCallback(TGenericCockie Cockie);

  // Send an Ack reply
  void SendAck(void);

  // Send an Nak reply
  void SendNak(void);

  // Thread execute method (override).
  void Execute(void);

  // Communication protocol Message decoder
  TCommErrorCodes Decode(void);

  TCommErrorCodes ValidateStartModel(void);

  //  Process current message.
  TCommErrorCodes ProcessCommand(void);

  //  Create and Send meesage to Host (without or with data).
  void SaveCmd(HostCommands Cmd,unsigned Length=0,void *Data=NULL);
  void SendCmd(HostCommands Cmd,unsigned Length,void *Data);
  void SendSavedCmds(void);

  void HandleNewSliceCommand(void);
  void TriggerHostCommTimeoutTimer(int TimeInSec);
  void CancelHostCommTimeoutTimer();

  // Methods for sending Specific Commands to Host.
  void RequestSlice();
  void ReplyStatus(void);
  void ReplyBuildStatus(void);
  void ReplyVersions(void);
  void ReplyPrinterData(BYTE RequestedData);
  void ReplyLicensedInfo(void);
  void ReplyFlexibleParameters(FlexibleParametersType* FlexParamsMsg);
  TCommErrorCodes ReplyResinPrinterData(ResinPrinterDataType& ResinRequestedData);
  void GetMode(BYTE RequestedData, int &qm, int &om);
  void SendOnline();

  int GetPrinterMode(void);

  void SendConfigFile(void);
  TCommErrorCodes AcceptRRRequest(void);

  long GetFileSize(FILE *Stream);
  int ClearResinRequestedData(ResinPrinterDataType& ResinRequestedData);

  void InitFlexibleParams();
  
  CHostPrintJob *m_HostPrintJob;

public:
    // Constructor
    CHostComm(void);

	// Destructor
	virtual ~CHostComm(void);

	void CloseConnection();
	void SetMute(bool aIsMute) { m_IsMute = aIsMute; }

    void SendEOLMessage();
    void SetOnlineOffline(bool);

    void StopPrint(TQErrCode ErrCode, int aLastPrintedSlice=-1);
    void ReportEndOfModelPrint(void);

    // Start execution (resume threads)
    void Start(void);

    // Return a pointer to the internal slice message queue
    TSliceMessageQueue *GetSliceMessageQueue(void) ;

    void DisposeSlice(CSlice *Slice);

    void SetSlicePreviewUpdateEvent(TGenericCallback SlicePreviewEvent,TGenericCockie SlicePreviewEventCockie);

    PBYTE RequestBMPAndLock(int Chamber, int& Width,int& Height,int& SliceNumber);
    void ReleaseBMPBuffersLock(void);

    // Did the host send a 'pause' command
    bool IsPaused();    
	static TPrintStopReason CHostComm::ErrCodeToHostError(TQErrCode ErrCode);
	TQErrCode Terminate() { return CQThread::Terminate(); }

};

#endif
