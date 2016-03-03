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

#ifndef _CONTAINERTAG_H_
#define _CONTAINERTAG_H_

// todo -oShahar.Behagen@objet.com -cNone: find more suitable header to include.
#include "QMessageQueue.h"

// The following pragma is for BCB only
#ifndef __BCPLUSPLUS__
#error Q2RT: This module contain pragma statements for Borland C++ Builder
#endif

// set allignment for this struct to 1bit
#pragma pack(push,1)

typedef int TAG_PARAM_ID;

const int RESERVED_SIZE     = 0x20;
const int BATCH_NO_SIZE     = 0x0A;
const int MACHINE_NAME_SIZE = 0x08;

enum {
  CARTRIDGE_OK_TAG_ERROR,     //no error
  CARTRIDGE_REFILL_TAG_ERROR, //cartridge weight is greater than expected (according to rf tag)
  NO_RESIN_CONSUMPTION_TAG_ERROR, //no resin consumption during printing
  NUM_OF_TAG_ERROR_CODES
};

enum {
	TAG_VER_NO_1 = 1, //Eden/Connex' tag
	TAG_VER_NO_2  //Objet' tag (with refill protection)
};

// Mfg Data
typedef WORD    TAG_TYPE__STRUCT_ID;
typedef WORD    TAG_TYPE__RESIN_TYPE;
typedef DWORD   TAG_TYPE__MFG_DATE_TIME;
typedef char*   TAG_TYPE__BATCH_NO;
typedef WORD    TAG_TYPE__INITIAL_WEIGHT; // grams
typedef DWORD   TAG_TYPE__EXPIRATION_DATE;

// Locked Data
typedef INT    TAG_TYPE__LAST_CONSUMED_WEIGHT;
typedef DWORD  TAG_TYPE__FIRST_USAGE_TIME;
typedef INT    TAG_TYPE__CURRENT_WEIGHT; // in grams
typedef DWORD  TAG_TYPE__CURRENT_PUMPING_TIME;
typedef INT    TAG_TYPE__ERROR_CODE; //OBJET_MACHINE feature


// Unlocked
typedef DWORD  TAG_TYPE__INSERTION_TIME;
typedef DWORD  TAG_TYPE__PRINT_TIME;
typedef char*  TAG_TYPE__MACHINE_NAME;
typedef DWORD  TAG_TYPE__MACHINE_ID;

// Factory Settings are always locked
typedef struct ReservedArea {
char ReservedBytes[RESERVED_SIZE]; // todo -oShahar.Behagen@objet.com -cNone: change to 32
} TAG_RESERVED_STRUCT;

// Factory Settings are always locked
typedef struct MfgData {
	TAG_TYPE__STRUCT_ID         TagStructID; // Tag revision number.
    TAG_TYPE__RESIN_TYPE        ResinType;
    TAG_TYPE__MFG_DATE_TIME     MfgDateTime;
    char                        BatchNo[BATCH_NO_SIZE];
	TAG_TYPE__INITIAL_WEIGHT    InitialWeight;
    TAG_TYPE__EXPIRATION_DATE   ExpirationDate;
} TAG_MFG_DATA_STRUCT;

// Local settings:
typedef struct LockedData {         // potentially locked
	TAG_TYPE__CURRENT_PUMPING_TIME      CurrentPumpingTime;
	TAG_TYPE__CURRENT_WEIGHT            CurrentWeight;
	TAG_TYPE__ERROR_CODE                ErrorCode;
	TAG_TYPE__LAST_CONSUMED_WEIGHT      LastConsumedWeight;
	TAG_TYPE__FIRST_USAGE_TIME          FirstUsageTimePrint;
} TAG_LOCKED_DATA_STRUCT;

typedef struct UnlockedData {
    // InsertionTime: First unlocked data item: used to obtain the address of the unlocked data.
	TAG_TYPE__INSERTION_TIME    InsertionTime;    // Tank insertion time
    TAG_TYPE__PRINT_TIME        PrintTime;        // Print start time (using this tank)
    char                        MachineName[MACHINE_NAME_SIZE];    //
    TAG_TYPE__MACHINE_ID        MachineID;        // Current machine (computer) that reads this tag.
} TAG_UNLOCKED_DATA_STRUCT;

#define TAG_SIZE 256
#define DATA_GAP_SIZE (TAG_SIZE - sizeof(TAG_RESERVED_STRUCT)       \
                                - sizeof(TAG_TYPE__STRUCT_ID)       \
                                - sizeof(TAG_MFG_DATA_STRUCT)       \
                                - sizeof(TAG_LOCKED_DATA_STRUCT)    \
                                - sizeof(TAG_UNLOCKED_DATA_STRUCT))

typedef struct Tag {
    // Reserved data. Used internally by the Reader.
    TAG_RESERVED_STRUCT reservedArea;

    // Mfg Data
    TAG_MFG_DATA_STRUCT mfgData;

    // Locked Data
    TAG_LOCKED_DATA_STRUCT lockedData;

    // Create a Gap between the Locked and the Unlocked data regions.
    char dataGap[DATA_GAP_SIZE];

    // unlocked data section
    TAG_UNLOCKED_DATA_STRUCT unlockedData;

} TAG_STRUCT;

enum {
 TAG_PARAM__STRUCT_ID = 0,
 TAG_PARAM__RESIN_TYPE,
 TAG_PARAM__MFG_DATE_TIME,
 TAG_PARAM__BATCH_NUM,
 TAG_PARAM__INITIAL_WEIGHT,
 TAG_PARAM__1ST_USAGE_TIME_INSERTION,
 TAG_PARAM__EXPIRATION_DATE,

 /// more here
 TAG_PARAM__CURRENT_WEIGHT,
 TAG_PARAM__CURRENT_PUMPING_TIME,
 TAG_PARAM__INSERTION_TIME,
 TAG_PARAM__MACHINE_NAME,
 TAG_PARAM__MACHINE_ID,
 //OBJET_MACHINE feature
 TAG_PARAM__ERROR_CODE,
 TAG_PARAM__LAST_CONSUMED_WEIGHT,

 // "Higher level" structs:
 TAG_PARAM__MFG_DATA,
 TAG_PARAM__LOCKED_DATA,
 TAG_PARAM__UNLOCKED_DATA,
 TAG_PARAM__ALL_DATA,
 TAG_PARAM__PARAM_COUNT = TAG_PARAM__ALL_DATA + 1
};

// Restore previous align settings
#pragma pack(pop)

class CContainerTag
{

public:

  CContainerTag(int rdr_num, int channel_num); // c'tor

  void SetTagID(unsigned __int64 tag_id) {m_tag_id = tag_id;}

  TQErrCode TagReadAll(TAG_STRUCT& tag_struct);
  TQErrCode TagReadAllRaw(TAG_STRUCT& tag_struct);

  TQErrCode TagReadStructID(TAG_TYPE__STRUCT_ID& structId);
  TQErrCode TagWriteStructID(TAG_TYPE__STRUCT_ID& structId);

  TQErrCode TagReadBatchNo(TAG_TYPE__BATCH_NO batchNo);
  TQErrCode TagWriteBatchNo(TAG_TYPE__BATCH_NO batchNo);

  TQErrCode TagReadMfgDateTime(TAG_TYPE__MFG_DATE_TIME& mfg_date_time);
  TQErrCode TagWriteMfgDateTime(TAG_TYPE__MFG_DATE_TIME& mfg_date_time);

  TQErrCode TagReadWeight(TAG_TYPE__CURRENT_WEIGHT& weight);
  TQErrCode TagWriteWeight(TAG_TYPE__CURRENT_WEIGHT& weight);

  TQErrCode TagReadInitialWeight(TAG_TYPE__INITIAL_WEIGHT& weight);
  TQErrCode TagWriteInitialWeight(TAG_TYPE__INITIAL_WEIGHT& weight);
  
  TQErrCode TagReadResinType(TAG_TYPE__RESIN_TYPE& resin_type);
  TQErrCode TagWriteResinType(TAG_TYPE__RESIN_TYPE& resin_type);

  TQErrCode TagSetPumpingTime();
  TQErrCode TagSetWeight();

  TQErrCode TagReadPumpingTime(TAG_TYPE__CURRENT_PUMPING_TIME& pumping_time);
  TQErrCode TagWritePumpingTime(TAG_TYPE__CURRENT_PUMPING_TIME& pumping_time);

  TQErrCode TagReadExpirationDate(TAG_TYPE__EXPIRATION_DATE& expiration_date);
  TQErrCode TagWriteExpirationDate(TAG_TYPE__EXPIRATION_DATE& expiration_date);
  //OBJET_MACHINE feature
  TQErrCode TagReadErrorCode(TAG_TYPE__ERROR_CODE& error_code);
  TQErrCode TagWriteErrorCode(TAG_TYPE__ERROR_CODE& error_code);

  TQErrCode TagReadFirstUsageTime(TAG_TYPE__FIRST_USAGE_TIME& first_usage_time);
  TQErrCode TagWriteFirstUsageTime(TAG_TYPE__FIRST_USAGE_TIME& first_usage_time);
  //OBJET_MACHINE feature
  TQErrCode TagReadLastConsumedWeight(TAG_TYPE__LAST_CONSUMED_WEIGHT& lastConsumedWeight);
  TQErrCode TagWriteLastConsumedWeight(TAG_TYPE__LAST_CONSUMED_WEIGHT& lastConsumedWeight);

  TQErrCode InvalidateTagData();
  bool      TagIsParamValid(TAG_PARAM_ID tag_param_id);
  void InvalidateTagSingleParam(TAG_PARAM_ID tag_param_id);


private:

  TQErrCode TagWriteSingleParam(TAG_PARAM_ID tag_param_id, PVOID param_value);
  TQErrCode TagReadSingleParam(TAG_PARAM_ID tag_param_id, PVOID param_value);

  bool TagIsParamAvailable();

  TQErrCode TagIsParamLocked(TAG_PARAM_ID tag_param_id, bool& paramLocked);

  TQErrCode TagDataRead(bool locked,
                        PVOID struct_member_add,
                        int  data_length);

  TQErrCode TagDataWrite(bool locked,
                         PVOID struct_member_add,
                         int  data_length);

  TQErrCode TagGetParamMemoryAllocation(TAG_PARAM_ID tag_param_id, PVOID* struct_member_add, int& struct_member_sizeof);

  CQMutex m_database_guard;
  int m_rdr_num; // the RF reader that this containerTag is linked to.
  int m_channel_num;  
  unsigned __int64 m_tag_id; // the RF tag that containerTag is linked to.

  // The struct that holds all parameters data:
  TAG_STRUCT m_tag_struct;

  // m_param_retrieved: '0' marks that a param was not yet retrieved from TAG.
  char m_param_valid[TAG_PARAM__PARAM_COUNT];
};

#endif

