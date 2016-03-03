/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: ContainerTag                                             *
 * Module Description: Tag Memory accessor for Resin Containers     *
 *                                                                  *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Shahar Behagen                                           *
 * Start date: 14/09/2006                                           *
 ********************************************************************/

#include "ContainerTag.h"
#include "rfid.h"
#include "AppLogFile.h"
#include "ReaderDfn.h"


//////////////////////////////////////////////////////////////////////
// CContainerTag()
// Description: check if a specific parameter is
//              available in current tag version.
//
CContainerTag::CContainerTag(int rdr_num, int channel_num)
:
  m_rdr_num(rdr_num),
  m_channel_num(channel_num),
  m_tag_id(0) // reset Tag ID (will be given when Identification is stable for this container)
{
  memset (m_param_valid, 0, TAG_PARAM__PARAM_COUNT);
  memset ((void*)&m_tag_struct, 0, sizeof(TAG_STRUCT));
}
//---------------------------------------------------------------------------

TQErrCode CContainerTag::TagReadAll(TAG_STRUCT& tag_struct)
{
  TQErrCode res = Q_NO_ERROR;

  // Get the Unlocked data:
  if ((res = TagReadSingleParam(TAG_PARAM__UNLOCKED_DATA, &tag_struct.unlockedData)) != Q_NO_ERROR)
    return res;

  // Get the Locked data:
  if ((res = TagReadSingleParam(TAG_PARAM__LOCKED_DATA, &tag_struct.lockedData)) != Q_NO_ERROR)
    return res;

  // Get the Mfg data:
  if ((res = TagReadSingleParam(TAG_PARAM__MFG_DATA, &tag_struct.mfgData)) != Q_NO_ERROR)
    return res;

  return res;
}

TQErrCode CContainerTag::TagReadAllRaw(TAG_STRUCT& tag_struct)
{
  TQErrCode res = Q_NO_ERROR;

  if ((res = TagReadSingleParam(TAG_PARAM__ALL_DATA, &tag_struct)) != Q_NO_ERROR)
    return res;

  return res;
}

TQErrCode CContainerTag::TagReadStructID(TAG_TYPE__STRUCT_ID& structId)
{
  return TagReadSingleParam(TAG_PARAM__STRUCT_ID, &structId);
}
//---------------------------------------------------------------------------

TQErrCode CContainerTag::TagWriteStructID(TAG_TYPE__STRUCT_ID& structId)
{
  return TagWriteSingleParam(TAG_PARAM__STRUCT_ID, &structId);
}
//---------------------------------------------------------------------------

TQErrCode CContainerTag::TagReadBatchNo(TAG_TYPE__BATCH_NO batchNo)
{
  return TagReadSingleParam(TAG_PARAM__BATCH_NUM, batchNo);
}
//---------------------------------------------------------------------------

TQErrCode CContainerTag::TagWriteBatchNo(TAG_TYPE__BATCH_NO batchNo)
{
  return TagWriteSingleParam(TAG_PARAM__BATCH_NUM, batchNo);
}
//---------------------------------------------------------------------------

TQErrCode CContainerTag::TagReadMfgDateTime(TAG_TYPE__MFG_DATE_TIME& mfg_date_time)
{
  return TagReadSingleParam(TAG_PARAM__MFG_DATE_TIME, &mfg_date_time);
}
//---------------------------------------------------------------------------

TQErrCode CContainerTag::TagWriteMfgDateTime(TAG_TYPE__MFG_DATE_TIME& mfg_date_time)
{
  return TagWriteSingleParam(TAG_PARAM__MFG_DATE_TIME, &mfg_date_time);
}
//---------------------------------------------------------------------------

TQErrCode CContainerTag::TagReadWeight(TAG_TYPE__CURRENT_WEIGHT& weight)
{
  return TagReadSingleParam(TAG_PARAM__CURRENT_WEIGHT, &weight);
}
//---------------------------------------------------------------------------

TQErrCode CContainerTag::TagWriteWeight(TAG_TYPE__CURRENT_WEIGHT& weight)
{
  return TagWriteSingleParam(TAG_PARAM__CURRENT_WEIGHT, &weight);
}
//---------------------------------------------------------------------------

TQErrCode CContainerTag::TagReadInitialWeight(TAG_TYPE__INITIAL_WEIGHT& weight)
{
  return TagReadSingleParam(TAG_PARAM__INITIAL_WEIGHT, &weight);
}
//---------------------------------------------------------------------------

TQErrCode CContainerTag::TagWriteInitialWeight(TAG_TYPE__INITIAL_WEIGHT& weight)
{
  return TagWriteSingleParam(TAG_PARAM__INITIAL_WEIGHT, &weight);
}
//---------------------------------------------------------------------------

TQErrCode CContainerTag::TagReadResinType(TAG_TYPE__RESIN_TYPE& resin_type)
{
  return TagReadSingleParam(TAG_PARAM__RESIN_TYPE, &resin_type);
}
//---------------------------------------------------------------------------

TQErrCode CContainerTag::TagWriteResinType(TAG_TYPE__RESIN_TYPE& resin_type)
{
  return TagWriteSingleParam(TAG_PARAM__RESIN_TYPE, &resin_type);
}
//---------------------------------------------------------------------------

TQErrCode CContainerTag::TagReadPumpingTime(TAG_TYPE__CURRENT_PUMPING_TIME& pumping_time)
{
  return TagReadSingleParam(TAG_PARAM__CURRENT_PUMPING_TIME, &pumping_time);
}
//---------------------------------------------------------------------------

TQErrCode CContainerTag::TagWritePumpingTime(TAG_TYPE__CURRENT_PUMPING_TIME& pumping_time)
{
  return TagWriteSingleParam(TAG_PARAM__CURRENT_PUMPING_TIME, &pumping_time);
}
//---------------------------------------------------------------------------

TQErrCode CContainerTag::TagReadExpirationDate(TAG_TYPE__EXPIRATION_DATE& expiration_date)
{
  return TagReadSingleParam(TAG_PARAM__EXPIRATION_DATE, &expiration_date);
}
//---------------------------------------------------------------------------

TQErrCode CContainerTag::TagWriteExpirationDate(TAG_TYPE__EXPIRATION_DATE& expiration_date)
{
  return TagWriteSingleParam(TAG_PARAM__EXPIRATION_DATE, &expiration_date);
}
//---------------------------------------------------------------------------

//OBJET_MACHINE feature
TQErrCode CContainerTag::TagReadErrorCode(TAG_TYPE__ERROR_CODE& error_code)
{
  return TagReadSingleParam(TAG_PARAM__ERROR_CODE, &error_code);
}
//---------------------------------------------------------------------------

TQErrCode CContainerTag::TagWriteErrorCode(TAG_TYPE__ERROR_CODE& error_code)
{
  return TagWriteSingleParam(TAG_PARAM__ERROR_CODE, &error_code);
}

//OBJET_MACHINE feature
TQErrCode CContainerTag::TagReadLastConsumedWeight(TAG_TYPE__LAST_CONSUMED_WEIGHT& lastConsumedWeight)
{
  return TagReadSingleParam(TAG_PARAM__LAST_CONSUMED_WEIGHT, &lastConsumedWeight);
}
//---------------------------------------------------------------------------

TQErrCode CContainerTag::TagWriteLastConsumedWeight(TAG_TYPE__LAST_CONSUMED_WEIGHT& lastConsumedWeight)
{
  return TagWriteSingleParam(TAG_PARAM__LAST_CONSUMED_WEIGHT, &lastConsumedWeight);
}
//---------------------------------------------------------------------------

//OBJET_MACHINE feature
TQErrCode CContainerTag::TagReadFirstUsageTime(TAG_TYPE__FIRST_USAGE_TIME& first_usage_time)
{
  return TagReadSingleParam(TAG_PARAM__1ST_USAGE_TIME_INSERTION, &first_usage_time);
}
//---------------------------------------------------------------------------

TQErrCode CContainerTag::TagWriteFirstUsageTime(TAG_TYPE__FIRST_USAGE_TIME& first_usage_time)
{
  return TagWriteSingleParam(TAG_PARAM__1ST_USAGE_TIME_INSERTION, &first_usage_time);
}
//---------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// TagIsParamAvailable()
// Description: check if a specific parameter is
//              available in current tag version.
//
bool CContainerTag::TagIsParamAvailable()
{
  return true;
}
//---------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Remarks: In order to check whether param is Locked or not,
//          see if it's address is in the Unlocked range,
//          which starts with: TAG_TYPE__INSERTION_TIME
TQErrCode CContainerTag::TagIsParamLocked(TAG_PARAM_ID tag_param_id, bool& paramLocked)
{
  paramLocked = false;
  if (tag_param_id < TAG_PARAM__INSERTION_TIME)
    paramLocked = true;

  return Q_NO_ERROR;
}
//---------------------------------------------------------------------------

TQErrCode CContainerTag::TagReadSingleParam(TAG_PARAM_ID tag_param_id, PVOID param_value)
{

  PVOID  struct_member_add = NULL;
  int    struct_member_sizeof = 0;
  TQErrCode res = Q_NO_ERROR;

  m_database_guard.WaitFor();

  if (!TagIsParamAvailable())
    return Q_NO_ERROR;

  // TagGetParamBlockRange(tag_param_id, from_block, count_blocks);
  TagGetParamMemoryAllocation(tag_param_id, &struct_member_add, struct_member_sizeof);

  if (!m_param_valid[tag_param_id])
  {
     bool   paramLocked = false;
    // see if this is a Locked parameter (write once)
    TagIsParamLocked(tag_param_id, paramLocked);

    // Set param to invalid:
    // m_param_valid[tag_param_id] = false;

    res = TagDataRead(paramLocked,    // Read/Write (TRFTransactionType)
                      struct_member_add,
                      struct_member_sizeof);

    // Now requested parameter should be updated in the TAG_STRUCT struct..
  }

  // copy the result to the output param and set valid:

  if (res == Q_NO_ERROR)
  {
    memcpy (param_value, struct_member_add, struct_member_sizeof);
    m_param_valid[tag_param_id] = true;
  }
  else
  {
    m_param_valid[tag_param_id] = false;
  }

  m_database_guard.Release();

  return res;
}
//---------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// TagWriteSingleParam(TAG_PARAM_ID tag_param_id, PVOID value)
// Remarks:
//   We cannot update the struct without immediatly writing the data to Tag, since
//   an updated param may be spread in two blocks, shared with other params.
TQErrCode CContainerTag::TagWriteSingleParam(TAG_PARAM_ID tag_param_id, PVOID param_value)
{
  bool   paramLocked = false;
  PVOID  struct_member_add = NULL;
  int    struct_member_sizeof = 0;

  m_database_guard.WaitFor();

  if (!TagIsParamAvailable())
    return Q_NO_ERROR;

  // see if this is a Locked parameter (write once)
  TagIsParamLocked(tag_param_id, paramLocked);

  // TagGetParamBlockRange(tag_param_id, from_block, count_blocks);
  TagGetParamMemoryAllocation(tag_param_id, &struct_member_add, struct_member_sizeof);

  // copy the requested value into the local struct and mark param as invalid:
  memcpy (struct_member_add, param_value, struct_member_sizeof);

  TQErrCode res = TagDataWrite(paramLocked,    // Read/Write (TRFTransactionType)
                               struct_member_add,
                               struct_member_sizeof);

  // copy the result to the output param and set valid:
  if (res == Q_NO_ERROR)
  {
    m_param_valid[tag_param_id] = true;
  }
  else
  {
// todo -oShahar.Behagen@objet.com -cNone: Following line causes trying to read again after an errnous writing occurs, which may result in an erronous read value.
//          to solve, may be we should use two arrays:  m_param_read_valid and m_param_write_valid
//    m_param_valid[tag_param_id] = false;
  }

//  TagUpdateParam(tag_param_id, param_value);
//  TagGetParamBlockRange(tag_param_id, from_block, count_blocks);

  m_database_guard.Release();

  return res;
} // TagWriteSingleParam(TAG_PARAM_ID tag_param_id, PVOID value)
//---------------------------------------------------------------------------


TQErrCode CContainerTag::TagDataRead(bool locked,
                                     PVOID struct_member_add,
                                     int data_length)
{
  RFTransaction trans;
  char* strPtr = NULL;
  int actual_data_length = 0;
  QLib::TQWaitResult WaitRes;

  if (m_tag_id == 0)
    return Q2RT_RFRDR_READ_ERROR;

  // Get an instance to the reader, and submit the transaction:
  CRFIDBase* RFIDInstance = CRFID::Instance();

  // if RFID instance was never created (for example, rdr dll was not registered) then we assume working
  // without RFID. In this case, return true, and do not submit the transaction.
  if (!RFIDInstance)
    return Q_NO_ERROR; // todo -oShahar.Behagen@objet.com -cNone: return Q_NO_RFID

  switch (locked)
  {
    case true:
      trans.TransactionType = READ_LOCKED;
      break;
    case false:
      trans.TransactionType = READ_UNLOCKED;
      break;
  }

  // Define the desired transaction fields:
  trans.RDRnum      = m_rdr_num;
  trans.ChannelNum  = m_channel_num;
  trans.TagID       = m_tag_id;
  trans.BaseAddress = (char*)struct_member_add - (char*)&m_tag_struct; // the (char*) casting is a must for pointer arithmetics...
  trans.DataLength  = new int; *(trans.DataLength) = data_length;
  trans.Data        = (unsigned char**)&strPtr;

  // ReaderAddTransaction() is a blocking call.
  WaitRes = RFIDInstance->ReaderAddTransaction(&trans);

  // Get the actual number of bytes read
  actual_data_length = *(trans.DataLength);

  if (trans.DataLength)
    delete trans.DataLength;

  // First, check for timeouts:
  if (WaitRes != QLib::wrSignaled)
  {
    CQLog::Write(LOG_TAG_RFID,"Error. TagDataRead() Timedout. Reader: %d, Channel Num: %d", m_rdr_num, m_channel_num);
    return Q2RT_RFRDR_READ_ERROR;
  }

  // Validate that the message length is as required:
  if (actual_data_length != data_length)
  {
    CQLog::Write(LOG_TAG_RFID,"Error. TagDataRead() Read data length mismatch (expected %d, got %d). Reader: %d, Channel Num: %d", data_length, actual_data_length, m_rdr_num, m_channel_num);
    return Q2RT_RFRDR_READ_ERROR;
  }

  // Validate that data was received without errors: 
  if (strPtr != NULL)
  {
    // copy the non-zero terminated data into a zero-terminated newly created string memory:
    char* theStr = new char[data_length + 1];
    memcpy((void*)theStr, (void*)strPtr, actual_data_length);
    theStr[data_length] = '\0';

    // Copy the requested parameter should be updated in the TAG_STRUCT struct.
    memcpy ((void*)struct_member_add, strPtr, data_length);

    // delete the stuff:
    delete []theStr;

    return Q_NO_ERROR;
  } else
  {
    CQLog::Write(LOG_TAG_RFID,"Error. TagDataRead() returned empty string. Reader: %d, Channel Num: %d", m_rdr_num, m_channel_num);
    return Q2RT_RFRDR_READ_ERROR;
  }
}
//---------------------------------------------------------------------------

TQErrCode CContainerTag::TagDataWrite(bool locked,
                                      PVOID struct_member_add,
                                      int data_length)
{
  RFTransaction trans;
  int Result = 0;
  int actual_data_length = 0;
  QLib::TQWaitResult WaitRes;

  if (m_tag_id == 0)
    return Q2RT_RFRDR_READ_ERROR;

  // Get an instance to the reader, and submit the transaction:
  CRFIDBase* RFIDInstance = CRFID::Instance();

  // if RFID instance was never created (for example, rdr dll was not registered) then we assume working
  // without RFID. In this case, return true, and do not submit the transaction.
  if (!RFIDInstance)
    return Q_NO_ERROR; // todo -oShahar.Behagen@objet.com -cNone: return Q_NO_RFID

  switch (locked)
  {
    case true:
      trans.TransactionType = WRITE_LOCKED;
      break;
    case false:
      trans.TransactionType = WRITE_UNLOCKED;
      break;
  }

  // Define the desired transaction fields:
  trans.RDRnum      = m_rdr_num;
  trans.ChannelNum  = m_channel_num;
  trans.TagID       = m_tag_id;
  trans.BaseAddress = (char*)struct_member_add - (char*)&m_tag_struct; // the (char*) casting is a must for pointer arithmetics...
  trans.DataLength  = new int; *(trans.DataLength) = data_length;
  trans.Data        = (unsigned char**)&struct_member_add;
  trans.Result      = &Result;

  // ReaderAddTransaction() is a blocking call.
  WaitRes = RFIDInstance->ReaderAddTransaction(&trans);

  // Get the actual number of bytes read
  actual_data_length = *(trans.DataLength);

  if (trans.DataLength)
    delete trans.DataLength;

  // First, check for timeouts:
  if (WaitRes != QLib::wrSignaled)
  {
    CQLog::Write(LOG_TAG_RFID,"Error. TagDataWrite() Timedout. Reader: %d, Channel Num: %d", m_rdr_num, m_channel_num);
    return Q2RT_RFRDR_WRITE_ERROR;
  }

  // Validate that the message length is as required:
  if (actual_data_length != data_length)
  {
    CQLog::Write(LOG_TAG_RFID,"Error. TagDataWrite() Write data length mismatch. Reader: %d, Channel Num: %d", m_rdr_num, m_channel_num);
    return Q2RT_RFRDR_WRITE_ERROR;
  }

  // check if data was written without errors:
  if (Result == ERROR_NONE)
  {
    return Q_NO_ERROR;
  } else
  {
    CQLog::Write(LOG_TAG_RFID,"Error. TagDataWrite() 'Result' Error. Reader: %d, Channel Num: %d", m_rdr_num, m_channel_num);
    return Q2RT_RFRDR_WRITE_ERROR;
  }
}
//---------------------------------------------------------------------------


TQErrCode CContainerTag::TagGetParamMemoryAllocation(TAG_PARAM_ID tag_param_id, PVOID* struct_member_add, int& struct_member_sizeof)
{
  *struct_member_add = NULL;
  struct_member_sizeof = 0;

  switch (tag_param_id)
  {
    ///////////////
    // MGF Data
    ///////////////
        
    case TAG_PARAM__STRUCT_ID:
      *struct_member_add = &m_tag_struct.mfgData.TagStructID;
      struct_member_sizeof = sizeof(TAG_TYPE__STRUCT_ID);
      break;

    case TAG_PARAM__BATCH_NUM:
      *struct_member_add = m_tag_struct.mfgData.BatchNo;
      struct_member_sizeof = sizeof(m_tag_struct.mfgData.BatchNo);
      break;

    case TAG_PARAM__RESIN_TYPE:
      *struct_member_add = &m_tag_struct.mfgData.ResinType;
      struct_member_sizeof = sizeof(TAG_TYPE__RESIN_TYPE);
      break;

    case TAG_PARAM__INITIAL_WEIGHT:
      *struct_member_add = &m_tag_struct.mfgData.InitialWeight;
      struct_member_sizeof = sizeof(TAG_TYPE__INITIAL_WEIGHT);
      break;

    case TAG_PARAM__MFG_DATE_TIME:
      *struct_member_add = &m_tag_struct.mfgData.MfgDateTime;
      struct_member_sizeof = sizeof(m_tag_struct.mfgData.MfgDateTime);
      break;

    case TAG_PARAM__EXPIRATION_DATE:
      *struct_member_add = &m_tag_struct.mfgData.ExpirationDate;
      struct_member_sizeof = sizeof(m_tag_struct.mfgData.ExpirationDate);
      break;

    ///////////////
    // Locked Data
    ///////////////

    case TAG_PARAM__INSERTION_TIME:
      *struct_member_add = &m_tag_struct.unlockedData.InsertionTime;
      struct_member_sizeof = sizeof(TAG_TYPE__INSERTION_TIME);
      break;

    case TAG_PARAM__MACHINE_NAME:
      *struct_member_add = m_tag_struct.unlockedData.MachineName;
      struct_member_sizeof = sizeof(m_tag_struct.unlockedData.MachineName);
      break;

    case TAG_PARAM__MACHINE_ID:
      *struct_member_add = &m_tag_struct.unlockedData.MachineID;
      struct_member_sizeof = sizeof(m_tag_struct.unlockedData.MachineID);
      break;

    case TAG_PARAM__CURRENT_WEIGHT:
      *struct_member_add = &m_tag_struct.lockedData.CurrentWeight;
      struct_member_sizeof = sizeof(m_tag_struct.lockedData.CurrentWeight);
      break;

    case TAG_PARAM__CURRENT_PUMPING_TIME:
	  *struct_member_add = &m_tag_struct.lockedData.CurrentPumpingTime;
	  struct_member_sizeof = sizeof(m_tag_struct.lockedData.CurrentPumpingTime);
	  break;
	//OBJET_MACHINE feature 
	case TAG_PARAM__ERROR_CODE:
	  *struct_member_add = &m_tag_struct.lockedData.ErrorCode;
	  struct_member_sizeof = sizeof(m_tag_struct.lockedData.ErrorCode);
	  break;
	case TAG_PARAM__1ST_USAGE_TIME_INSERTION:
	  *struct_member_add = &m_tag_struct.lockedData.FirstUsageTimePrint;
	  struct_member_sizeof = sizeof(m_tag_struct.lockedData.FirstUsageTimePrint);
	  break;
	case TAG_PARAM__LAST_CONSUMED_WEIGHT:
	  *struct_member_add = &m_tag_struct.lockedData.LastConsumedWeight;
	  struct_member_sizeof = sizeof(m_tag_struct.lockedData.LastConsumedWeight);
	  break;
    ////////////////
    // UnLocked Data (todo)
    ////////////////


    ////////////////    ////////////////
    // "Higher Level" struct addresses:
    ////////////////    ////////////////

    case  TAG_PARAM__MFG_DATA:
      *struct_member_add = &m_tag_struct.mfgData;
      struct_member_sizeof = sizeof(m_tag_struct.mfgData);
      break;

    case  TAG_PARAM__LOCKED_DATA:
      *struct_member_add = &m_tag_struct.lockedData;
      struct_member_sizeof = sizeof(m_tag_struct.lockedData);
      break;

    case  TAG_PARAM__UNLOCKED_DATA:
      *struct_member_add = &m_tag_struct.unlockedData;
      struct_member_sizeof = sizeof(m_tag_struct.unlockedData);
	  break;
	  
	case  TAG_PARAM__ALL_DATA:
	  *struct_member_add = &m_tag_struct;
	  struct_member_sizeof = sizeof(m_tag_struct);
	  break;

	default:
      break;
  }

  return Q_NO_ERROR;
}

TQErrCode CContainerTag::InvalidateTagData()
{
  m_database_guard.WaitFor();

  memset (m_param_valid, 0, TAG_PARAM__PARAM_COUNT);

  m_database_guard.Release();

  return Q_NO_ERROR;  
}
//---------------------------------------------------------------------------

void CContainerTag::InvalidateTagSingleParam(TAG_PARAM_ID tag_param_id)
{
  m_database_guard.WaitFor();

  m_param_valid[tag_param_id] = false;

  m_database_guard.Release();
}
//---------------------------------------------------------------------------

bool CContainerTag::TagIsParamValid(TAG_PARAM_ID tag_param_id)
{
  m_database_guard.WaitFor();

  bool ret_val = m_param_valid[tag_param_id];

  m_database_guard.Release();
  return ret_val;
}

/*
TestCContainerTag()
{
   CContainerTag tag;
   TAG_TYPE__MFG_DATE_TIME mfgtime = 0x111111;
   char machineName[7] = "abcdefg";
   DWORD InsertionTime = 9;
   DWORD machineID = 55;

   tag.TagWriteSingleParam(TAG_PARAM__MFG_DATE, static_cast<PVOID>(&mfgtime));
   tag.TagWriteSingleParam(TAG_PARAM__INSERTION_TIME, static_cast<PVOID>(&InsertionTime));
   tag.TagWriteSingleParam(TAG_PARAM__MACHINE_NAME, static_cast<PVOID>(&machineName));
   tag.TagWriteSingleParam(TAG_PARAM__MACHINE_ID, static_cast<PVOID>(&machineID));

   machineID = 0;
   tag.TagReadSingleParam(TAG_PARAM__MACHINE_ID, &machineID);
}
*/
//---------------------------------------------------------------------------

 
