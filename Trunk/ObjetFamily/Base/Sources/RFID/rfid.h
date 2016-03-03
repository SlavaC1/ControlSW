/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: RFID                                                     *
 * Module Description: This class implements RFID services          *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Shahar Behagen                                           *
 * Start date: 20/09/2005                                           *
 ********************************************************************/
#ifndef _RFID_H_
#define _RFID_H_

#include "QTimer.h"
#include "QMessageQueue.h"
#include "Errorhandler.h"
#include "Q2RTErrors.h"
#include "GlobalDefs.h"
#include "AppParams.h"
#include "QMonitor.h"
#include "ReaderDsp.h"
#include "Tag.h"

#define MAX_TAGS 10
const int RDR_QUEUE_SEND_TIMEOUT = 500;// /*Seconds*/ * 1000;

const int RDR_CHECK_TAG_ERROR = -16;
const int RDR_COM_ERROR = -17;
const int NUM_OF_RETRIES = 50;
typedef enum {
   TAGS_LAST_CHANGE_REMOVE,
   TAGS_LAST_CHANGE_ADD
} TTagsLastChange;

typedef enum {
   READ_LOCKED,
   READ_UNLOCKED,
   WRITE_LOCKED,
   WRITE_UNLOCKED
} TRFTransactionType;

typedef struct  {
    unsigned int        TransactionID; // ?? needed ?
    int                 RDRnum;
    int                 ChannelNum;
    unsigned __int64    TagID;
    TRFTransactionType  TransactionType;
    unsigned int        BaseAddress;
    int                 *DataLength; // in/out
    BYTE                **Data; // for in/out data
    int                 *Result;
    CQEvent             *TransactionEnd; // sync the end of the transtaction.
} RFTransaction;

typedef unsigned __int64* TRawTagList;
typedef int* TIntptr;

// Forward declarations:
class CRFIDBase;
class CRFReader;

// Type for a user receive callback
typedef void (*TRFTagCallback)(TGenericCockie Cockie, CRFReader* theReader, int rdr_num, int channel_num, TTagsLastChange TagsCondition, unsigned __int64 tag_num);

typedef struct  {
    RFTransaction   transaction;
} RFQueueEntry;

// Exception class for all the QLib RTTI elements
class ERFID : public EQException {
  public:
    ERFID(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};
//---------------------------------------------------------------------------------------


class CRFReaderTask : public CQThread
{

public:

  CRFReaderTask(int readerNum);

  // Used by Reader class, to set this Task's associated reader.
  void SetReader(CRFReader* rdr);

  virtual ~CRFReaderTask();

private:

  // Thread function (override)
  void Execute(void);

  // The reader associated with this CRFReaderTask instance.
  int m_RDRnum;

  // If m_ForceReadEvent is set, the next operation with RF reader will be executed immediatly
  CQEvent m_ForceReadEvent;

  CQMutex m_Guard; // Lock access to tag ID's arrays during Tags update.
  CRFReader* m_theReader;
};
//---------------------------------------------------------------------------------------


/*
CRFReaderChannel class is responsible for keeping track of Tags repository - per channel, and
invoking ADD_TAG and REMOVE_TAG events when needed.
*/
class CRFReaderChannel : public CQComponent
{

public:

  // C'tor
  CRFReaderChannel(int readerNum, int channelNum);

  // Calls API SetChannel()
  void SetActive();

  // EnableSampling() causes this channel to be iteratively sampled
  void EnableSampling();

  // DisableSampling() causes this channel to stop from being iteratively sampled
  void DisableSampling();

  // returned whether Samplingis Enabled/Disabled
  bool IsSampled();

  // Sends a transaction (Read/Write) if pending, and sampling Tags and notifies changes (Add/Remove) - if not.
  void ExecuteChannel();

  // Forces an immediate Tags sampling (Generates Add/Remove notifications if applicable.)
  void ForceCheckTags();

  // GetLastChangeTagID: Get the ID of the current Tag repository.
  void GetLastTagsIDList(TRawTagList tagsList, int* num_of_tags);

  // GetLastChangeTagID: Get the IDs of the recently ADDED Tag(s):
  void GetLastAddedTagsIDList(TRawTagList tagsList, int* num_of_tags);

  // GetLastChangeTagID: Get the IDs of the recently REMOVED Tag(s):
  void GetLastRemovedTagsIDList(TRawTagList tagsList, int* num_of_tags);

  // RemoveTagFromList will cause an additional 'ADD' notification for the 'removed' Tag.
  bool RemoveTagFromList(unsigned __int64 tagID);

  // RemoveAllTagFromList Will cause 'ADD' notification for all currently identified Tags.
  bool RemoveAllTagFromList();

  // (Blocking.) Register a read / write transaction to this channel, and return a result.
  QLib::TQWaitResult AddTransaction(RFTransaction* Transaction); // RFPriority priority = NORMAL); // ?? todo ?

  // Used by Reader class, to set this channel's associated reader.
  void SetReader(CRFReader* rdr);

  void ReleaseQueue();

  // D'tor
  virtual ~CRFReaderChannel();

private:

  // Compars the current Tag invetory and the last, and lists which tags were added and which removed.
  void AnalyzeAddedAndRemoved();

  // Verify that this channel was "activated" (called before running commands through it)
  void VerifyChannel();

  // Tags condition notification functions:
  void NotifyTagCondition(TTagsLastChange change, unsigned __int64 tag_id);

  // Checks for Tags Added/Removed and notify by callback.
  void CheckTagsAndNotify();

  // Executes a (Blocking) Read Write transaction.
  void DoTransaction(RFQueueEntry &Transaction);

  // Safely Sets the Transaction Wait event
  void SetTransactionEvent(RFQueueEntry &Transaction);

  // Safely deletes the Transaction Wait event
  void DeleteTransactionEvent(RFQueueEntry &Transaction);

  // Returns transaction's parameters as string. (used by error messaging)
  QString GetTransactionAsString(RFTransaction* trans);
  
  // The reader associated with this CRFReaderTask instance.
  int m_RDRnum;

  // Channel number of this channel.
  int m_ChannelNum;

  // If m_ForceReadEvent is set, the next operation with RF reader will be executed immediatly
  CQEvent m_ForceReadEvent;

  CQEvent m_pqEvent;
  RFQueueEntry queueEntry;

  // Transactions queue
  typedef CQMessageQueue<RFQueueEntry> TRFTransactionQueue;
  TRFTransactionQueue* m_RFTransactionQueue;

  CQMutex     m_Guard; // Lock access to tag ID's arrays during Tags update.
  CQMutex     m_TransactionMutex; // Lock access to transaction's wait event.

  CTag*    m_TagsID[4];
//  TCOMIRFTag   m_TagsID;
  CRFIDBase*  m_RFIDInstance;
  CRFReader*  m_theReader;

  CQMutex m_MutexChannelSampling;
  bool m_EnableSampling;    // is this channel being iteratively sampled
  int  m_LastAddedTagNum;   // number of items in: m_LastAddedTagIDs[]
  int  m_LastRemovedTagNum; // number of items in: m_LastRemovedTagIDs[]
  int  m_LastTagsNum;       // Last read Number of tags
  int  m_NumberOfTags;      // Current read Number of tags

  unsigned __int64 m_CurrentTagIDs[MAX_TAGS];
  unsigned __int64 m_LastTagIDs[MAX_TAGS];
  unsigned __int64 m_LastAddedTagIDs[MAX_TAGS];
  unsigned __int64 m_LastRemovedTagIDs[MAX_TAGS];
  TTagsLastChange  m_TagsLastChange;

  static int m_ActiveChannelNum;

  unsigned int m_checkTagsCounter;
  unsigned int m_comErrorCounter; //indicates if Com is disconnected
  unsigned int m_removeTagCounter;

};
//---------------------------------------------------------------------------------------
int CRFReaderChannel::m_ActiveChannelNum = 0;



class CRFReader : public CQComponent
{

public:

  CRFReader(const QString& Name, int ReaderNum, int NumOfChannels);  
  virtual ~CRFReader();

  void EnableSampling(int channelNum);

  void DisableSampling(int channelNum);

  void DisableAllChannels();  

  bool IsSampled(int channelNum);

  QLib::TQWaitResult ChannelAddTransaction(RFTransaction* Transaction); // RFPriority priority = NORMAL); // ?? todo ?

  CRFReaderChannel* GetChannel(int ChannelNum);

  // This version of GetTags dosn't check the reader.
  void GetTags(int channelNum, TRawTagList tagsList, int* num_of_tags);

  void CheckTags(int *iTagsCount);

  void RemoveTagFromList(int channelNum, unsigned __int64 tagID);

  void RemoveAllTagsFromChannel(int channelNum);

  void GetCurrentError(HRESULT *err);

  void LogCurrentError(HRESULT err);

  // Resume Reader's Task.
  void Resume();

  void Terminate();

  int  Disconnect();  // close port but retain parameters

  void Connect();

  bool WaitForReaderPowerUp();

 /* int GetReaderNum() { return m_readerNum; } //objet test */

protected:

  CAppParams    *m_ParamsMgr;

private:

  void VerifyChannel(int ChannelNum);

  // Verifies reader's serial port connection:
  bool VerifyConnection();

  // Get tag from COM interface. (the private ver. does't require to set channel)
  void GetTags(CTag** TagsID);

  void SetChannel(int channel_num);

  // todo -oShahar.Behagen@objet.com -cNone: see if friend is realy needed. currently used only in CRFReaderTask::Execute(void)
  friend class CRFReaderTask;
  friend class CRFReaderChannel;

  CReaderDsp*      theReader;
  CRFReaderTask*     m_ReaderTask;
  CRFReaderChannel** m_Channels;

  int m_readerCom; // Reader's serial port number
  int m_readerNum; // to access P.M
  int m_numOfChannels;
  bool m_isConnected;

  // Lock ChannelAddTransaction() to make it thread safe.
  CQMutex m_AddTransactionGuard;
};
//---------------------------------------------------------------------------------------

class CRFIDBase : public CQComponent
{

public:

  CRFIDBase(const QString& Name);
  virtual ~CRFIDBase();
  DEFINE_V_METHOD  (CRFIDBase,int,LaunchReaderTasks)=0;
  DEFINE_V_METHOD  (CRFIDBase,int,Start)=0;
  DEFINE_V_METHOD_3(CRFIDBase,int,RemoveTagFromList,int /* readernum */, int /*channelNum*/, unsigned __int64 /*TagID*/ )=0;
  DEFINE_V_METHOD_2(CRFIDBase,int,RemoveAllTagsFromChannel,int /* readernum */, int /*channelNum*/)=0;
  DEFINE_V_METHOD_2(CRFIDBase,int,EnableSampling,int /* readernum */, int /*channelNum*/ )=0;
  DEFINE_V_METHOD_2(CRFIDBase,int,DisableSampling,int /* readernum */, int /*channelNum*/ )=0;
  DEFINE_V_METHOD_1(CRFIDBase,int,ReaderStopAndClear,int /* readernum */)=0;  
  DEFINE_V_METHOD_2(CRFIDBase,bool,IsSampled,int /* readernum */, int /*channelNum*/ )=0;
  DEFINE_V_METHOD_1(CRFIDBase,bool,WaitForReaderPowerUp,int /* readernum */)=0;

  int  GetTags(int rdrnum, int channelNum, TRawTagList tagsList, int* num_of_tags);

  // Cannot register following methods since passing pointers. (currently not allowed by object roster)
  // ReaderAddTransaction gets the Reader num in the RFTransaction struct.
  QLib::TQWaitResult ReaderAddTransaction(RFTransaction* Transaction); // RFPriority priority = NORMAL); // ?? todo ?

  int  TerminateReaderTasks(void);

  int  SetRFTagsChangeCallBack(TRFTagCallback RFTagsChangeCallback, TGenericCockie RFTagsChangeCockie);

  int  InvokeCallBack(CRFReader* the_reader, int rdr_num, int channel_num, TTagsLastChange tags_condition, unsigned __int64 tag_num);

  QString GetTagIDAsString(unsigned __int64 tagID);

  // Pointer to the singleton object
  static CRFIDBase *m_InstancePtr;
  
protected:

  int m_numOfReaders;
  CErrorHandler *m_ErrorHandlerClient;
  CAppParams    *m_ParamsMgr;
  CRFReader    **theReaders;
  static CQMutex m_InitGuard;

  // CallBack data:
  TRFTagCallback   m_RFTagsChangeCallback;
  TGenericCockie   m_RFTagsChangeCockie;
};

CRFIDBase *CRFIDBase::m_InstancePtr = 0;
CQMutex    CRFIDBase::m_InitGuard;
//---------------------------------------------------------------------------------------

class CRFID : public CRFIDBase
{

public:

  // registered methids:
  int LaunchReaderTasks(void);

  int Start(void);

  // un-registered methids:
  int  SetRFTagsChangeCallBack(TRFTagCallback RFTagsChangeCallback, TGenericCockie RFTagsChangeCockie);

  int  RemoveTagFromList(int rdrnum, int channelNum, unsigned __int64 tagID);
  int  RemoveAllTagsFromChannel(int rdrnum, int channelNum);
  int  EnableSampling(int rdrnum, int channelNum);
  int  DisableSampling(int rdrnum, int channelNum);
  int  ReaderStopAndClear(int rdrnum);
  bool IsSampled(int rdrnum, int channelNum);
  bool WaitForReaderPowerUp(int rdrnum);

  // Init/DeInit the singleton object
  static void Init(void)
  {
    m_InitGuard.WaitFor();
    if(m_InstancePtr == 0)
    {
      m_InstancePtr = new CRFID("rfid");
    }
    m_InitGuard.Release();
  }
  static void DeInit(void)
  {
    m_InitGuard.WaitFor();
    if(m_InstancePtr != 0)
    {
      delete m_InstancePtr;
      m_InstancePtr = 0;
    }
    m_InitGuard.Release();
  }

  static CRFIDBase* Instance(void)
  {
    CRFIDBase* retVal;
    m_InitGuard.WaitFor();
    retVal = m_InstancePtr;
    m_InitGuard.Release();
    return retVal;
  }

  virtual ~CRFID();

protected:

  int InvokeCallBack(CRFReader* reader, TTagsLastChange tags_change, int count);

private:

  CRFID(const QString& Name) : CRFIDBase(Name)
  {}
};
//---------------------------------------------------------------------------------------
#endif
