/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: RFID                                                     *
 * Module Description: This class implement services related to the *
 *                     RFID                                         *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Shahar Behagen                                           *
 * Start date: 20/09/2005                                           *
 ********************************************************************/

#include "QMonitor.h"
#include "QUtils.h"
#include "QThreadUtils.h"
#include "Q2RTErrors.h"
#include "QApplication.h"
#include "Q2RTApplication.h"
#include "AppLogFile.h"
#include "rfid.h"
#include "Reader/ReaderDfn.h"

#undef EXTENDED_RF_LOG

// Connection parameters
const int COMPortBaud =       9600;
const int COMPortParity =     0;
const int COMPortDataBits =   8;
const int COMPortStopBits =   1;
//OBJET_MACHINE adjustment
const int WAIT_BETWEEN_RF_TAG_CHECKS = 50;

const int RF_TRANSACTION_TIMEOUT = 10000;
const int TIMEOUT_FOR_READER_POWERUP = 10000;
const int DELAY_BETWEEN_CHANNELS = 20;//5; // 1 was actually enough.


CRFReaderChannel::CRFReaderChannel(int readerNum, int channelNum)
 : m_RDRnum(readerNum), m_ChannelNum(channelNum)

{
  m_RFIDInstance = NULL;
  //m_TagsID       = NULL;
  m_LastTagsNum  = 0;
  m_checkTagsCounter = 0;
  m_comErrorCounter = 0;
  m_removeTagCounter = 0;

  memset(m_LastTagIDs, 0, sizeof(m_LastTagIDs));

  m_RFTransactionQueue = new TRFTransactionQueue(1, "", false);

  DisableSampling();
}
//--------------------------------------------------------------------------------------------

void CRFReaderChannel::ExecuteChannel()
{
  RFQueueEntry Transaction;
  QLib::TQWaitResult WaitResult;

  memset (&Transaction, 0, sizeof(RFQueueEntry));

  if((WaitResult = m_RFTransactionQueue->Receive(Transaction, WAIT_BETWEEN_RF_TAG_CHECKS)) == QLib::wrReleased)
  {
	// todo -oShahar.Behagen@objet.com -cNone: handle error condition here?
	CQLog::Write(LOG_TAG_RFID,"ExecuteChannel() received released rdr:%d chnl:%d", m_RDRnum,m_ChannelNum);
    return;
  }

  // if no one raised the event. (timeout reached)  
  if(WaitResult != QLib::wrSignaled)
  {
    m_MutexChannelSampling.WaitFor();

	if (IsSampled() && m_checkTagsCounter==0)
	{
	  // This will dispatch the ADD / REMOVE Tag notifications.
	  CheckTagsAndNotify();
	}
	m_checkTagsCounter = (m_checkTagsCounter+1)%10;
	m_MutexChannelSampling.Release();
  } else
  {
    // Following Sleep() was introduced after a new PC HW caused errors when reading from two subsequent channels.
    QSleep(DELAY_BETWEEN_CHANNELS);
    DoTransaction(Transaction);
  }
}
//--------------------------------------------------------------------------------------------

void CRFReaderChannel::EnableSampling()
{
  m_MutexChannelSampling.WaitFor();

  m_EnableSampling = true;

  m_MutexChannelSampling.Release();
}
//--------------------------------------------------------------------------------------------

void CRFReaderChannel::DisableSampling()
{
  m_MutexChannelSampling.WaitFor();

  m_EnableSampling = false;

  m_MutexChannelSampling.Release();
}
//--------------------------------------------------------------------------------------------

bool CRFReaderChannel::IsSampled()
{
  bool retVal;

  m_MutexChannelSampling.WaitFor();

  retVal = m_EnableSampling;

  m_MutexChannelSampling.Release();

  return retVal;
}
//--------------------------------------------------------------------------------------------

void CRFReaderChannel::SetActive()
{
  m_theReader->SetChannel(m_ChannelNum);

  // todo -oShahar.Behagen@objet.com -cNone:  set m_ActiveChannelNum only if theres no error.
  m_ActiveChannelNum = m_ChannelNum;
}
//--------------------------------------------------------------------------------------------

void CRFReaderChannel::CheckTagsAndNotify()
{
  VerifyChannel();
  m_NumberOfTags = -1;
  //static int Counter = 0;

  // Get Tag's IDs of Tag found in Reader proximity:
  try
  {
    // Must first make this channel active:
    SetActive();

	// Check Tags:
	m_theReader->CheckTags(&m_NumberOfTags);

    // check for error that should stop RFID execution:
    if (m_NumberOfTags < 0)
    {
	  throw ERFID("CheckTags() error. NumOfTags: " + QIntToStr(m_NumberOfTags),RDR_CHECK_TAG_ERROR);
    }

    // Note:
    //   We don't check against: (m_NumberOfTags == 0) since we will still need to execute this function -
	//   in case there is a TAG REMOVE situation.
	m_theReader->GetTags(m_TagsID);
	m_comErrorCounter = 0;
  }
  catch(ERFID& err)
  {
	 CQLog::Write(LOG_TAG_RFID,"RF error:%s",err.GetErrorMsg().c_str());
	 CQLog::Write(LOG_TAG_RFID,"Error accessing RDR dll, trying GetTags()");
	 m_comErrorCounter++;
	 if(err.GetErrorCode()!=RDR_COM_ERROR || m_comErrorCounter<NUM_OF_RETRIES)
	   return;
	 else
       m_removeTagCounter = 21;
  }
  catch(...)
  {
	CQLog::Write(LOG_TAG_RFID,"Error accessing RDR dll, trying GetTags()");
    return;
  }

  // check for errors:
  if (!m_TagsID)
    return;

  m_Guard.WaitFor();

  AnalyzeAddedAndRemoved();

  // Dispatch the ADD / REMOVE Tag notifications:
  // ADD Notifications
  for (int i=0; i < m_LastAddedTagNum; i++)
	NotifyTagCondition(TAGS_LAST_CHANGE_ADD, m_LastAddedTagIDs[i]);
	  // REMOVE Notifications
  if(m_removeTagCounter>20)
  {
	m_removeTagCounter = 0;
		for (int i=0; i < m_LastRemovedTagNum; i++)
			NotifyTagCondition(TAGS_LAST_CHANGE_REMOVE, m_LastRemovedTagIDs[i]);
  }
  else
	m_removeTagCounter++;
			
  // Reset stuff before next iteration:
  m_LastTagsNum = m_NumberOfTags;

  // Set the Tags ID's of the current read:
  memset(m_LastTagIDs, 0, sizeof(m_LastTagIDs));
  memset(m_CurrentTagIDs, 0, sizeof(m_CurrentTagIDs));

  // Update m_LastTagIDs[] with current values for the next iteration...
  try
  {
    for (int i=0; i < m_NumberOfTags; i++)
	  m_LastTagIDs[i] = m_TagsID[i]->GetUID();

#ifdef EXTENDED_RF_LOG
    CRFIDBase* RFIDInstance = CRFID::Instance();
    CQLog::Write(LOG_TAG_RFID,"Updating: Cnnl=%d  m_NumberOfTags=%d  m_LastTagIDs=[%s, %s, %s]"
                             ,m_ChannelNum
                             ,m_NumberOfTags
                             ,RFIDInstance->GetTagIDAsString(m_LastTagIDs[0]).c_str()
                             ,RFIDInstance->GetTagIDAsString(m_LastTagIDs[1]).c_str()
                             ,RFIDInstance->GetTagIDAsString(m_LastTagIDs[2]).c_str());
#endif                             
  }
  catch(ERFID& err)
  {
	 CQLog::Write(LOG_TAG_RFID,"RF error:%s",err.GetErrorMsg().c_str());
	 CQLog::Write(LOG_TAG_RFID,"Error accessing RDR dll, trying GetUID()");
  }
  catch(...)
  {
    CQLog::Write(LOG_TAG_RFID,"Error accessing RDR dll, trying GetUID()");
  }

  // When there are zero tags in the sampled channel, m_TagsID is a valid pointer, but *m_TagsID is NULL, and Release() shouldn't be called on it.

  m_Guard.Release();
}
//--------------------------------------------------------------------------------------------

void CRFReaderChannel::AnalyzeAddedAndRemoved()
{
  // copy currently read Tag ID's into an array:
  try
  {
    for (int i = 0; i < m_NumberOfTags; i++)
    {
	   m_CurrentTagIDs[i] = m_TagsID[i]->GetUID();
    }
  }
  catch(ERFID& err)
  {
	 CQLog::Write(LOG_TAG_RFID,"RF error:%s",err.GetErrorMsg().c_str());
	 CQLog::Write(LOG_TAG_RFID,"Error accessing RDR dll, trying GetUID()");
  }
  catch(...)
  {
    CQLog::Write(LOG_TAG_RFID,"Error accessing RDR dll, trying GetUID()");
  }

  ///////////////////////////////
  // Mark changes from last read:
  ///////////////////////////////

  // Iterate all tags currently found by the reader:
  for (int i = 0; i < m_NumberOfTags; i++)
  {

    // Iterate all known tags from the last read, and see when a match can be found for the current tag.
    for (int j = 0; j < m_LastTagsNum; j++)
    {

      if (m_CurrentTagIDs[i] == m_LastTagIDs[j])
      {
        // 'reset' the common Tag:
        m_CurrentTagIDs[i] = m_LastTagIDs[j] = 0;
        break;
      }
    }
  } // for (int i = 0; i < m_NumberOfTags; i++)

  // Now we have the ADDED Tag ID in m_CurrentTagIDs
  // and the REMOVED Tags ID in m_LastTagIDs

  memset(m_LastAddedTagIDs, 0, sizeof(m_LastAddedTagIDs));
  memset(m_LastRemovedTagIDs, 0, sizeof(m_LastRemovedTagIDs));

  // We still need to remove the '0's from the m_CurrentTagIDs and m_LastTagIDs lists:
  int j = 0;
  for (int i=0; i < m_NumberOfTags; i++)
  {
    if (m_CurrentTagIDs[i] != 0)
    {
       m_LastAddedTagIDs[j] = m_CurrentTagIDs[i];
       j++;
    }
  }
  m_LastAddedTagNum = j;

  j = 0;
  for (int i=0; i < m_LastTagsNum; i++)
  {
    if (m_LastTagIDs[i] != 0)
    {
       m_LastRemovedTagIDs[j] = m_LastTagIDs[i];
       j++;
    }
  }
  m_LastRemovedTagNum = j;
}
//--------------------------------------------------------------------------------------------

void CRFReaderChannel::DoTransaction(RFQueueEntry &Transaction)
{
  RFTransaction    * trans           = NULL;
  HRESULT            err             = ERROR_NONE;
  unsigned __int64   TagID           = 0;
  unsigned __int64   tagsList[MAX_TAGS];
  int                numoftags       = 0;
  int                tagnum          = 0;
  bool               isCriticalError = false;

  memset(tagsList, 0, sizeof(tagsList));
  trans      = &Transaction.transaction;

  // Keep Transaction data for error reporting (report may be done when transaction already deleted)
  QString TransactionDataReport = GetTransactionAsString(trans);

  // Safely get the recent Tags list:
  GetLastTagsIDList(tagsList, &numoftags);

  for (tagnum=0; tagnum < numoftags; tagnum++)
    if (tagsList[tagnum] == trans->TagID)
      break;

  if (tagsList[tagnum] != trans->TagID)
  {
    SetTransactionEvent(Transaction);
	CQLog::Write(LOG_TAG_RFID,("Error. Could not find Tag match. Rdr=%d Chnl=%d"+TransactionDataReport).c_str(),m_RDRnum,m_ChannelNum);
#ifdef EXTENDED_RF_LOG
    CRFIDBase* RFIDInstance = CRFID::Instance();
    CQLog::Write(LOG_TAG_RFID,"(cont.)Error data: numoftags=%d trans->TagID=%s tagsList=[%s, %s, %s]"
                             ,numoftags
                             ,RFIDInstance->GetTagIDAsString(trans->TagID).c_str()
                             ,RFIDInstance->GetTagIDAsString(tagsList[0]).c_str()
                             ,RFIDInstance->GetTagIDAsString(tagsList[1]).c_str()
                             ,RFIDInstance->GetTagIDAsString(tagsList[2]).c_str());
#endif
    return; // could not find a match to the requested Tag.
  }

  // Must first make this channel active:
  SetActive();

  // This operation should be done to refresh m_TagsID.
  m_theReader->GetTags(m_TagsID);
  TagID = m_TagsID[tagnum]->GetUID();

  if (TagID != trans->TagID)
  {
    SetTransactionEvent(Transaction);
	CQLog::Write(LOG_TAG_RFID,("Error. Could not find Tag match (with DLL). Rdr=%d Chnl=%d"+ TransactionDataReport).c_str(),m_RDRnum,m_ChannelNum);
  }

  CQLog::Write(LOG_TAG_RFID,("Accessing RDR DLL. Rdr=%d Chnl=%d"+ TransactionDataReport).c_str(),m_RDRnum,m_ChannelNum);

  try
  {

    switch (trans->TransactionType)
    {

      case READ_LOCKED:
		*(trans->Data) = m_TagsID[tagnum]->ReadTag(false,  // todo -oShahar.Behagen@objet.com -cNone: change to 'true'
								  trans->DataLength,    // output
                                  trans->BaseAddress,   // begin address
								  *(trans->DataLength) // required length
								  );
        break;

      case READ_UNLOCKED:
		*(trans->Data) = m_TagsID[tagnum]->ReadTag(false,  // false for UNLOCKED data
                                  trans->DataLength,    // output
								  trans->BaseAddress,   // begin address
								  *(trans->DataLength) // required length
								  );
        break;

      case WRITE_LOCKED:
		*(trans->Result) = m_TagsID[tagnum]->WriteTag(false,  // todo -oShahar.Behagen@objet.com -cNone: change to 'true'
                                   trans->DataLength,    // output
                                   (unsigned char*)*trans->Data,
                                   trans->BaseAddress,   // begin address
								   *(trans->DataLength) // required length
								   );
        break;

      case WRITE_UNLOCKED:
		*(trans->Result) = m_TagsID[tagnum]->WriteTag(false,  // false for UNLOCKED data
                                   trans->DataLength,    // output
                                   (unsigned char*)*trans->Data,
                                   trans->BaseAddress,   // begin address
								   *(trans->DataLength) // required length
							 	   );
        break;
    }

	HRESULT           retVal   = 0;
	CQLog::Write(LOG_TAG_RFID,("RDR DLL Return Code=%d Rdr=%d Chnl=%d"+ TransactionDataReport).c_str(),retVal,m_RDRnum,m_ChannelNum);
  }
  catch(ERFID& err)
  {
	 CQLog::Write(LOG_TAG_RFID,"RF error:%s",err.GetErrorMsg().c_str());
	 CQLog::Write(LOG_TAG_RFID,("Exception while accessing RDR DLL. Rdr=%d Chnl=%d"+ TransactionDataReport).c_str(),m_RDRnum,m_ChannelNum);
  }
  catch(...)
  {
    isCriticalError = true;
    CQLog::Write(LOG_TAG_RFID,("Exception while accessing RDR DLL. Rdr=%d Chnl=%d"+ TransactionDataReport).c_str(),m_RDRnum,m_ChannelNum);
  }

  m_theReader->GetCurrentError(&err);

  if (err != ERROR_NONE)
    m_theReader->LogCurrentError(err);

  if (isCriticalError)
  {
    CQLog::Write(LOG_TAG_RFID, "* com port reset *%d", m_RDRnum);
    try {
      if (ERROR_SUCCESS == m_theReader->Disconnect())
      {
        m_theReader->Connect();
      }
    } catch (...) {
      CQLog::Write(LOG_TAG_RFID, "Failed to recover from RDR DLL Exception. Rdr=%d Chnl=%d", m_RDRnum,m_ChannelNum);
    }

  }
  // Check for Nullness: if transaction timed-out, the event is now already deleted by the caller thread.
  SetTransactionEvent(Transaction);
}
//--------------------------------------------------------------------------------------------

QString CRFReaderChannel::GetTransactionAsString(RFTransaction* trans)
{
  QString str = "TransType = ";

  switch (trans->TransactionType)
  {
    case READ_LOCKED:
      str += "READ_LOCKED";
      break;
    case READ_UNLOCKED:
      str += "READ_UNLOCKED";
      break;
    case WRITE_LOCKED:
      str += "WRITE_LOCKED";
      break;
    case WRITE_UNLOCKED:
      str += "WRITE_UNLOCKED";
      break;
  }

  str += " BaseAdd = "   + QIntToStr(trans->BaseAddress)
       + " DataLengh = " + QIntToStr(*trans->DataLength);

  return str;
}
//--------------------------------------------------------------------------------------------

// NotifyTagErrorCondition
void CRFReaderChannel::NotifyTagCondition(TTagsLastChange tags_condition, unsigned __int64 tag_id)
{
  // todo -oShahar.Behagen@objet.com -cNone:  m_RFIDInstance should not be in this class. The callbacks should be registered here instead.
  if (!m_RFIDInstance)
    if ((m_RFIDInstance = CRFID::Instance()) == NULL)
      throw ERFID("NotifyTagCondition() error.");

  m_RFIDInstance->InvokeCallBack(m_theReader, m_RDRnum, m_ChannelNum, tags_condition, tag_id);
}
//--------------------------------------------------------------------------------------------

void CRFReaderChannel::ForceCheckTags()
{
  m_ForceReadEvent.SetEvent();
}
//--------------------------------------------------------------------------------------------

void CRFReaderChannel::GetLastTagsIDList(TRawTagList tagsList, int* num_of_tags)
{
  m_Guard.WaitFor();
  memcpy(tagsList, m_LastTagIDs, sizeof(m_LastTagIDs));
  *num_of_tags = m_LastTagsNum;
  m_Guard.Release();
}
//--------------------------------------------------------------------------------------------

// GetLastChangeTagID: Get the ID of the recently ADDED / REMOVED Tag:
void CRFReaderChannel::GetLastAddedTagsIDList(TRawTagList tagsList, int* num_of_tags)
{
  m_Guard.WaitFor();
  memcpy(tagsList, m_LastAddedTagIDs, sizeof(m_LastAddedTagIDs));
  *num_of_tags = m_LastAddedTagNum;
  m_Guard.Release();
}
//--------------------------------------------------------------------------------------------

// GetLastChangeType: Was it ADD or REMOVE
void CRFReaderChannel::GetLastRemovedTagsIDList(TRawTagList tagsList, int* num_of_tags)
{
  m_Guard.WaitFor();
  memcpy(tagsList, m_LastRemovedTagIDs, sizeof(m_LastRemovedTagIDs));
  *num_of_tags = m_LastRemovedTagNum;
  m_Guard.Release();
}
//--------------------------------------------------------------------------------------------

// RemoveTagFromList will generate an additional 'ADD' notification for the 'removed' Tag.
bool CRFReaderChannel::RemoveTagFromList(unsigned __int64 tagID)
{
  bool retVal = false;

  m_Guard.WaitFor();

  for (int i=0,j=0; i < m_LastTagsNum; i++,j++)
  {
    if (m_LastTagIDs[i] == tagID)
    {
      retVal = true;
      j++;
    }

    m_LastTagIDs[i] = m_LastTagIDs[j];
  }

  if (retVal == true)
    m_LastTagsNum--;

  m_Guard.Release();

  return retVal;
}
//--------------------------------------------------------------------------------------------

// RemoveAllTagFromList Will generate 'ADD' notification for all currently identified Tags.
bool CRFReaderChannel::RemoveAllTagFromList()
{
  m_Guard.WaitFor();

  // Set the Tags ID's of the current read:
  memset(m_LastTagIDs, 0, sizeof(m_LastTagIDs));
  memset(m_CurrentTagIDs, 0, sizeof(m_CurrentTagIDs));
  m_LastTagsNum = 0;

  m_Guard.Release();  

  return true;
}
//--------------------------------------------------------------------------------------------

QLib::TQWaitResult CRFReaderChannel::AddTransaction(RFTransaction* Transaction) // RFPriority priority = NORMAL); // ?? todo ?
{
  //RFQueueEntry queueEntry;

  queueEntry.transaction = *Transaction; // copy the transaction data.
 
  queueEntry.transaction.TransactionEnd = &m_pqEvent;//new CQEvent;
 
  queueEntry.transaction.TransactionEnd->ResetEvent();

  if(m_RFTransactionQueue->Send(queueEntry, QMsToTicks(RDR_QUEUE_SEND_TIMEOUT)) == QLib::wrTimeout)
    QMonitor.Print("queue timeout");

  QLib::TQWaitResult WaitResult = queueEntry.transaction.TransactionEnd->WaitFor(RF_TRANSACTION_TIMEOUT);

  if(WaitResult != QLib::wrSignaled)
  {
	CQLog::Write(LOG_TAG_RFID,("Timeout Transaction: " + GetTransactionAsString(Transaction)).c_str());
    // throw ERFID("Timeout while waiting for RF transaction to complete.");
  }

  return WaitResult;
}
//--------------------------------------------------------------------------------------------

void CRFReaderChannel::DeleteTransactionEvent(RFQueueEntry &Transaction)
{
  m_TransactionMutex.WaitFor();
  if (Transaction.transaction.TransactionEnd)
  {
    delete Transaction.transaction.TransactionEnd;
    Transaction.transaction.TransactionEnd = NULL;
  }
  m_TransactionMutex.Release();
}
//--------------------------------------------------------------------------------------------

void CRFReaderChannel::SetTransactionEvent(RFQueueEntry &Transaction)
{
  m_TransactionMutex.WaitFor();

  if (Transaction.transaction.TransactionEnd)
    Transaction.transaction.TransactionEnd->SetEvent();

  m_TransactionMutex.Release();
}
//--------------------------------------------------------------------------------------------

void CRFReaderChannel::VerifyChannel()
{
 // todo -oShahar.Behagen@objet.com -cNone:
}
//--------------------------------------------------------------------------------------------

void CRFReaderChannel::SetReader(CRFReader* rdr)
{
  m_theReader = rdr;
}
//--------------------------------------------------------------------------------------------

void CRFReaderChannel::ReleaseQueue()
{
  m_RFTransactionQueue->Release();
}
//--------------------------------------------------------------------------------------------

CRFReaderChannel::~CRFReaderChannel()
{
  if (m_RFTransactionQueue)
    delete m_RFTransactionQueue;
}
//--------------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

// CRFReaderTask Constructor:
CRFReaderTask::CRFReaderTask(int readerNum)
  : CQThread(true, "ReaderTask" + QIntToStr(readerNum), true) , m_RDRnum(readerNum)
{
	m_theReader = NULL;
}
//--------------------------------------------------------------------------------------------

void CRFReaderTask::Execute(void)
{
  do
  {
    for (int  channel = 0; channel < m_theReader->m_numOfChannels; channel++)
    {
      try
      {
        m_theReader->m_Channels[channel]->ExecuteChannel();
      }
	  catch(ERFID& err)
	  {
		CQLog::Write(LOG_TAG_RFID,"RF error:%s",err.GetErrorMsg().c_str());
		CQLog::Write(LOG_TAG_RFID,"Error executing RFReaderTask.Rdr:%d Chnl:%d",m_theReader->m_readerNum,channel);
	 }
      catch(...)
      {
		CQLog::Write(LOG_TAG_RFID,"Error executing RFReaderTask.Rdr:%d Chnl:%d",m_theReader->m_readerNum,channel);
      }
    }
  } while(!Terminated);
/*#endif*/  
}
//--------------------------------------------------------------------------------------------

void CRFReaderTask::SetReader(CRFReader* rdr)
{
  m_theReader = rdr;
}
//--------------------------------------------------------------------------------------------

CRFReaderTask::~CRFReaderTask()
{
  // Note: should be already terminated by the Reader d'tor. 
  Terminate();

  // Wait for thread termination.
  WaitFor();
}
//--------------------------------------------------------------------------------------------


////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

CRFIDBase::CRFIDBase(const QString& Name) : CQComponent(Name)
{
  bool              NoSerialPortDefined = true;
  m_ErrorHandlerClient = NULL;
  INIT_METHOD(CRFIDBase,LaunchReaderTasks);
  INIT_METHOD(CRFIDBase,Start);
  INIT_METHOD(CRFIDBase,RemoveTagFromList);
  INIT_METHOD(CRFIDBase,RemoveAllTagsFromChannel);
  INIT_METHOD(CRFIDBase,WaitForReaderPowerUp);


//  INIT_METHOD(CRFIDBase,SetRFTagsChangeCallBack);
//  INIT_METHOD(CRFIDBase,InvokeCallBack);

  // SetRFTagsChangeCallBack(TRFTagCallback RFTagsChangeCallback, TGenericCockie RFTagsChangeCockie);
  m_RFTagsChangeCallback = NULL;

  m_ParamsMgr = CAppParams::Instance();
  m_numOfReaders = m_ParamsMgr->RFReadersConnection.Size();
  //m_numOfReaders = 1; //objet test
  // Initializes the Component Object Model(COM) library:
  HRESULT COMRetVal = ::CoInitialize (NULL);
  if (COMRetVal != S_OK)
    CQLog::Write(LOG_TAG_RFID,"Cannot ::CoInitialize() COM Library. Return code: " + QIntToStr(COMRetVal)); 

  // Create the readers ptr array:
  theReaders = new CRFReader*[m_numOfReaders];
  
  // For each reader: CreateInstance and connect:
  for (int i=0; i < m_numOfReaders; i++)
  {
    int readerCom = m_ParamsMgr->RFReadersConnection[i];

    if (readerCom != 0)
    {
      NoSerialPortDefined = false;
    }

    // Create the Reader instance:
    theReaders[i] = new CRFReader(Name, i, m_ParamsMgr->RFReadersChannels[i]);
  }

  if (NoSerialPortDefined)
  {
    CQLog::Write(LOG_TAG_RFID,"Warning: All RFID Readers were defined with serial port = 0.");
    QMonitor.ErrorMessage("All RFID Readers were defined with serial port = 0. \n\
All Resins will be identified as: 'UNKNOWN'.");
  }
}
//--------------------------------------------------------------------------------------------

CRFIDBase::~CRFIDBase()
{
  for (int i=0; i < m_numOfReaders; i++)
    if (theReaders[i] != NULL)
      delete theReaders[i];

  delete []theReaders;
}
//--------------------------------------------------------------------------------------------

int CRFIDBase::GetTags(int rdrNum, int channelNum, TRawTagList tagsList, int* num_of_tags)
{
  // channelnum is '0' based.
  theReaders[rdrNum]->GetTags((channelNum - 1), tagsList, num_of_tags);

  return true;
// todo -oShahar.Behagen@objet.com -cNone:
}
//--------------------------------------------------------------------------------------------

int CRFIDBase::SetRFTagsChangeCallBack(TRFTagCallback RFTagsChangeCallback, TGenericCockie RFTagsChangeCockie)
{
  m_RFTagsChangeCallback = RFTagsChangeCallback;
  m_RFTagsChangeCockie   = RFTagsChangeCockie;

  return true;
}
//--------------------------------------------------------------------------------------------

int CRFIDBase::InvokeCallBack(CRFReader* the_reader, int rdr_num, int channel_num, TTagsLastChange tags_condition, unsigned __int64 tag_num)
{
  if(m_RFTagsChangeCallback)
  {
    (*m_RFTagsChangeCallback)(m_RFTagsChangeCockie, the_reader, rdr_num, channel_num, tags_condition, tag_num);
    return true;
  }

  return false;
}
//--------------------------------------------------------------------------------------------

QLib::TQWaitResult CRFIDBase::ReaderAddTransaction(RFTransaction* Transaction) // RFPriority priority = NORMAL); // ?? todo ?
{
  int rdrNum = Transaction->RDRnum;

  // Channelnum is '0'-based here.
  Transaction->ChannelNum--;

  if (rdrNum > m_numOfReaders)
    throw ERFID("Reader num out of limit.");

  return theReaders[rdrNum]->ChannelAddTransaction(Transaction);
}
//--------------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

CRFID::~CRFID() {}
//--------------------------------------------------------------------------------------------
int CRFID::LaunchReaderTasks(void)
{
  for (int i=0; i < m_numOfReaders; i++)
  {
    if (theReaders[i] == NULL)
    {
      continue;
    } else
    {
      theReaders[i]->Resume();
    }
  }

  return 0;
}


//--------------------------------------------------------------------------------------------
int CRFIDBase::TerminateReaderTasks(void)
{
  for (int i=0; i < m_numOfReaders; i++)
  {
    if (theReaders[i] != NULL)
    {
      theReaders[i]->Terminate();
    }
  }

  return 0;
}


//--------------------------------------------------------------------------------------------
int CRFID::Start(void)
{
  CRFIDBase::m_InitGuard.WaitFor();
  if (NULL != Instance())
  {
    for (int i=0; i < m_numOfReaders; i++)
    {
      if (theReaders[i] == NULL)
      {
        continue;
		} else
      {
        try
		{
		  theReaders[i]->Connect();
		  theReaders[i]->Resume();
        }
        catch(...)
        {
          CRFIDBase::m_InitGuard.Release();
            throw;
        }
      }
    }
  }

  CRFIDBase::m_InitGuard.Release();
  return 0;
}
//--------------------------------------------------------------------------------------------

int CRFID::RemoveTagFromList(int rdrNum, int channelNum, unsigned __int64 tagID)
{
  // channelnum is '0' based.
  theReaders[rdrNum]->RemoveTagFromList((channelNum - 1), tagID);

  return 0;
}
//--------------------------------------------------------------------------------------------

int CRFID::RemoveAllTagsFromChannel(int rdrNum, int channelNum)
{
  // channelnum is '0' based.
  theReaders[rdrNum]->RemoveAllTagsFromChannel(channelNum - 1);

  return 0;
}
//--------------------------------------------------------------------------------------------

int CRFID::EnableSampling(int rdrNum, int channelNum)
{
  theReaders[rdrNum]->EnableSampling(channelNum - 1);

  return 0;
}
//--------------------------------------------------------------------------------------------

int CRFID::DisableSampling(int rdrNum, int channelNum)
{
  theReaders[rdrNum]->DisableSampling(channelNum - 1);

  return 0;
}
//--------------------------------------------------------------------------------------------

int  CRFID::ReaderStopAndClear(int rdrNum)
{
  theReaders[rdrNum]->DisableAllChannels();

  return 0;
}
//--------------------------------------------------------------------------------------------

bool CRFID::IsSampled(int rdrNum, int channelNum)
{
  return theReaders[rdrNum]->IsSampled(channelNum - 1);
}
//--------------------------------------------------------------------------------------------

bool CRFID::WaitForReaderPowerUp(int rdrNum)
{
  return theReaders[rdrNum]->WaitForReaderPowerUp();
}
//--------------------------------------------------------------------------------------------

QString CRFIDBase::GetTagIDAsString(unsigned __int64 tagID)
{
   unsigned __int64 uiTempUI;
   QString          retStr = "";

   while(tagID>0)
   {
     if (tagID>99)
     	uiTempUI = tagID/10;
     else
     	uiTempUI = 10;

     retStr = QIntToStr(tagID%uiTempUI) + retStr;
     tagID /= 10;
   }

   return retStr;
}
//--------------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

CRFReader::CRFReader(const QString& Name, int ReaderNum, int NumOfChannels)
  : CQComponent(Name), m_readerNum(ReaderNum), m_numOfChannels(NumOfChannels)
{
  m_ParamsMgr   = CAppParams::Instance();
  m_isConnected = false;

  // Get reader's com port, if equals to '0', dont create a Reader object:
  m_readerCom = m_ParamsMgr->RFReadersConnection[m_readerNum];

      // Create the reader:
  theReader = new CReaderDsp();

  // Create the channel container applicable for this reader:
  m_Channels = new CRFReaderChannel*[m_numOfChannels];

  for (int channelNum = 0; channelNum < m_numOfChannels; channelNum++)
  {
    // ChannelNum is '0' based here and '1'-based outside.
    m_Channels[channelNum] = new CRFReaderChannel(m_readerNum, channelNum + 1);
    m_Channels[channelNum]->SetReader(this);
  }
  // Create the task associated with this reader:
  m_ReaderTask = new CRFReaderTask(m_readerNum);
  m_ReaderTask->SetReader(this);
}


//--------------------------------------------------------------------------------------------
int CRFReader::Disconnect()
{
/*  if(m_readerCom==4) //objet test
    return;*/
  int  rc = theReader->Disconnect();

  // Check that COM port was closed:
  if (rc != ERROR_SUCCESS)
  {
      CQLog::Write(LOG_TAG_RFID, "Cannot Disconnect() from RFID Reader. RDR num:"
                                 + QIntToStr(m_readerNum) + " COM Port:"+ QIntToStr(m_readerCom));
  }

    return rc;
}


void CRFReader::Connect()
{
  int  ComOK     = 0; // for return value from Connect()
  long IsConnect = 0; // for return value from IsConnect()

  if (m_readerCom == 0)
  {
    CQLog::Write(LOG_TAG_RFID,"Cannot Connect() to a reader initialized with serial port = 0");
    return;
  }
  /*if(m_readerCom==4) //objet test
    return;*/
  // for unknown reason, i cannot use the 5-param connect() version. (so using 6-param):
  ComOK = theReader->Connect(m_readerCom/*,
                     COMPortBaud,
                     COMPortParity,
                     COMPortDataBits,
                     COMPortStopBits*/);

  // Check that COM port was openned:
  if (ComOK != S_OK)
    throw ERFID("Cannot Connect() RFID Reader. Err code:" + QIntToStr(ComOK) +
               " RDR num:" + QIntToStr(m_readerNum) +
			   " COM Port:"+ QIntToStr(m_readerCom)+"\n\nPlease check"+
			   " MSC"+QIntToStr(m_readerCom-2)+" connections"); //m_readerCom range is 3-4
															   //and MSC range currently is 1-2
															  //so the correlation between both is:(m_readerCom-2)
  // Check that a Reader is attached:
  for (int i = 0; i < 15; ++i) {
	  IsConnect = theReader->IsConnect();
	  if (true == (m_isConnected = IsConnect)) break;
	  //else - retry
  }
  if (true != m_isConnected)
  {
	  throw ERFID("Error. No RF-Reader is connected to specified COM port: " + QIntToStr(m_readerCom));
  }
}

// Warning: This API is not threadsafe.
//          Call it only when theReader COM object is not occupied with other calls.
bool CRFReader::WaitForReaderPowerUp()
{

  unsigned long StartTime = QGetTicks();
  HRESULT RDRLastError = ERROR_NONE;

  for (bool IsConnect = false;;)
  {
	IsConnect = theReader->IsConnect();

    // Check if any error occured and report to Log:
    GetCurrentError(&RDRLastError);
	if (RDRLastError != ERROR_NONE)
      LogCurrentError(RDRLastError);

	if (IsConnect == true)
     return true;

    if ((QGetTicks() - StartTime) > TIMEOUT_FOR_READER_POWERUP)
      return false;

    QSleep(50);
  }
}

QLib::TQWaitResult CRFReader::ChannelAddTransaction(RFTransaction* Transaction) // RFPriority priority = NORMAL); // ?? todo ?
{
  m_AddTransactionGuard.WaitFor();

  QLib::TQWaitResult WaitResult;

  int channelNum = Transaction->ChannelNum;

  if (channelNum > m_numOfChannels)
  {
    m_AddTransactionGuard.Release();
    throw ERFID("VerifyChannel() Channel num out of limit.");
  }

  WaitResult = m_Channels[channelNum]->AddTransaction(Transaction);

  m_AddTransactionGuard.Release();

  return WaitResult;
}
//--------------------------------------------------------------------------------------------

void CRFReader::Resume()
{
  if (m_readerCom == 0)
  {
    CQLog::Write(LOG_TAG_RFID,"Cannot Resume() a reader's Task initialized with serial port = 0");
    return;
  }
  if (m_ReaderTask)
    m_ReaderTask->Resume();
}


//--------------------------------------------------------------------------------------------
void CRFReader::Terminate()
{
  if (m_ReaderTask)
    m_ReaderTask->Terminate();
}


//--------------------------------------------------------------------------------------------
CRFReader::~CRFReader()
{
  // Mark Task as terminated:
  Terminate();

//  m_ReaderTask->WaitFor(60000);

  // Release the Queues:
  for (int channelNum = 0; channelNum < m_numOfChannels; channelNum++)
    if (m_Channels[channelNum] != NULL)
      m_Channels[channelNum]->ReleaseQueue();

  // Delete the task object:
  if (m_ReaderTask)
    delete m_ReaderTask;

  // Delete the Channels
  for (int channelNum = 0; channelNum < m_numOfChannels; channelNum++)
    if (m_Channels[channelNum] != NULL)
      delete m_Channels[channelNum];

  delete []m_Channels;

  // Delete the COM object:
  if (theReader)
  {
    delete theReader;
    theReader = NULL;
  }
}
//--------------------------------------------------------------------------------------------

CRFReaderChannel* CRFReader::GetChannel(int ChannelNum)
{
  VerifyChannel(ChannelNum);

  return m_Channels[ChannelNum];
}
//--------------------------------------------------------------------------------------------

void CRFReader::VerifyChannel(int ChannelNum)
{
  if ((ChannelNum >= m_numOfChannels) || (ChannelNum < 0))
  {
    CQLog::Write(LOG_TAG_RFID,"Error: Channel number out of bounds. Make sure parameter 'RFReadersChannels' is set to: '4,0'");
    QMonitor.ErrorMessageWaitOk("Error: Channel number out of bounds. Make sure parameter 'RFReadersChannels' is set to: '4,0'");
    throw ERFID("VerifyChannel() Channel num out of limit.");
  }
}
//--------------------------------------------------------------------------------------------

bool CRFReader::VerifyConnection()
{
  return m_isConnected;
}
//--------------------------------------------------------------------------------------------

void CRFReader::GetTags(int channelNum, TRawTagList tagsList, int* num_of_tags)
{
  GetChannel(channelNum)->GetLastTagsIDList(tagsList, num_of_tags);
}
//--------------------------------------------------------------------------------------------

void CRFReader::GetTags(CTag** TagsID)
{
  HRESULT RDRLastError = ERROR_NONE;

  if (VerifyConnection() == false)
    return;
  
  *(TagsID) = theReader->GetTags();

  // Check if any error occured and report to Log:
  GetCurrentError(&RDRLastError);
  if (RDRLastError != ERROR_NONE)
    LogCurrentError(RDRLastError);
}
//--------------------------------------------------------------------------------------------

void CRFReader::CheckTags(int *iTagsCount)
{
  HRESULT RDRLastError = ERROR_NONE;

  if (VerifyConnection() == false)
    return;
  
  *(iTagsCount) = theReader->CheckTags();

  // Check if any error occured and report to Log:
  GetCurrentError(&RDRLastError);
  if (RDRLastError != ERROR_NONE)
    LogCurrentError(RDRLastError);
}
//--------------------------------------------------------------------------------------------

void CRFReader::RemoveTagFromList(int channelNum, unsigned __int64 tagID)
{
  GetChannel(channelNum)->RemoveTagFromList(tagID);
}
//--------------------------------------------------------------------------------------------

void CRFReader::RemoveAllTagsFromChannel(int channelNum)
{
  GetChannel(channelNum)->RemoveAllTagFromList();
}
//--------------------------------------------------------------------------------------------

void CRFReader::EnableSampling(int channelNum)
{
  GetChannel(channelNum)->EnableSampling();
}
//--------------------------------------------------------------------------------------------

void CRFReader::DisableSampling(int channelNum)
{
  GetChannel(channelNum)->DisableSampling();
}
//--------------------------------------------------------------------------------------------

void CRFReader::DisableAllChannels()
{
  for (int channelNum = 0; channelNum < m_numOfChannels; channelNum++)
  {
    GetChannel(channelNum)->DisableSampling();
    GetChannel(channelNum)->RemoveAllTagFromList();
  }
}
//--------------------------------------------------------------------------------------------

bool CRFReader::IsSampled(int channelNum)
{
  return GetChannel(channelNum)->IsSampled();
}
//--------------------------------------------------------------------------------------------

void CRFReader::SetChannel(int channel_num)
{
  int err = -1;
  HRESULT RDRLastError = ERROR_NONE;

  if (VerifyConnection() == false)
    return;
  
  err = theReader->SetChannel(channel_num);

  // Check if any error occured and report to Log:
  GetCurrentError(&RDRLastError);
  if (RDRLastError != ERROR_NONE)
    LogCurrentError(RDRLastError);

  if (err != 0)
  {
	  if(!theReader->IsConnect())
	  {
		err = RDR_COM_ERROR;
		CQLog::Write(LOG_TAG_RFID,"SetChannel Err:%d Com:%d", err,m_readerCom);
		throw ERFID("Com port is disconnected. Com: " + QIntToStr(m_readerCom),err);
      } 
	  CQLog::Write(LOG_TAG_RFID,"SetChannel err:%d chnl:%d Com:%d", err,channel_num,m_readerCom);
	  throw ERFID("SetChannel() error. Unable to select channel. Channel num,err,com: " + QIntToStr(channel_num) + "," + QIntToStr(err) + "," + QIntToStr(m_readerCom));
  }
}
//--------------------------------------------------------------------------------------------

void CRFReader::GetCurrentError(HRESULT *err)
{
  *err = theReader->GetCurrentError();

  // todo -oShahar.Behagen@objet.com -cNone: see what error codes we need to check, and where to put the write to Log.
//  if ((*err == ERROR_NO_RESPONSE_FROM_READER))
//    CQLog::Write(LOG_TAG_RFID,"RFID GetCurrentError() returned error: %d", *err);

/*
  if (err == ERROR_NO_RESPONSE_FROM_READER)
   return err;

  if (err == ERROR_TOKEN_NOT_PRESENT)
   return err;
*/

}
//--------------------------------------------------------------------------------------------

void CRFReader::LogCurrentError(HRESULT err)
{
 QString strStr = "";

  switch (err)
  {
/*
// todo -oShahar.Behagen@objet.com -cNone: get .h from Alon
  case ERROR_NONE:
  	strStr += "No Error";
  	break;
  case SERIAL_PORT_ERROR:
  	strStr += "Serial Port Error";
  	break;
  case SERIAL_PORT_OCCUPIED:
        strStr += "Serial Port Occupied";
  	break;
  case WRITE_LOCK_FAILED:
  	strStr += "Write Locked Failed";
  	break;
  case READ_OR_WRITE_UNABLE_TO_SELECT_TAG:
  	strStr += "Unable to Select Tag during Tag Read or Write";
  	break;
  case UNABLE_TO_READ_OR_WRITE:
  	strStr += "Unable to Read or Write Tag";
  	break;
  case NOT_ENOUGH_ENERGY_IN_TAG_TO_READ_OR_WRITE:
  	strStr += "Not enough energy in Tag to Read or Write";
  	break;
*/
  default:
   return;
//        CQLog::Write(LOG_TAG_RFID,"RFID error occured. err code: %0x02X", err);
//        return;
  }

//  CQLog::Write(LOG_TAG_RFID, strStr);
}
