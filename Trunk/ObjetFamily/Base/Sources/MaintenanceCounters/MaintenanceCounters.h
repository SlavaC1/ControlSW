
#ifndef _MAINTENANCE_COUNTERS_H_
#define _MAINTENANCE_COUNTERS_H_

#include <vector>
#include "QThread.h"
#include "QMutex.h"
#include "QEvent.h"
#include "MaintenanceCountersDefs.h"

struct TCounter
{
  ULONG LastMeasuredTime;
  double ElapsedSeconds;
  long WarningTime;
  time_t ResetDate;
  bool Enabled;
  bool WarningState;
  bool NeedsUserUpdate;
};

typedef std::vector<TCounter> TCounters;

class CMaintenanceCounters: public CQThread
{
private:

  // The singleton instance
  static CMaintenanceCounters* m_Instance;

  bool IsUserWarningActive();

  int m_NumOfCounters;

  // The counters
  TCounters m_Counters;

  // Default warning times
  std::vector<long> m_DefaultWarnings;

  // Default Warning Updates
  std::vector<long> m_WarningsUpdates;

  // A mutex for protecting the access for the counters
  CQMutex m_CountersMutex;

  // an event for terminatation of the thread
  CQEvent m_TerminateEvent;

  // The name of the file for storing the counters data
  QString m_CountersFileName;

  // The name of the backup file for storing the counters data
  QString m_BackupCountersFileName;

  // Time to save the updates to the file
  unsigned long m_SaveToFileTime;

  // Private constructor - implemented as a singleton
  CMaintenanceCounters(int NumOfCounters, QString FileName);

  // Private destructor - implemented as a singleton
  ~CMaintenanceCounters();

  void Execute();

  USHORT CalculateCheckSum(BYTE *Buffer, int Size);

  void CheckForServiceWarning(void);

  // Restore all counters to their default states
  void RestoreDefaults(void);

public:

  // Create the singleton object
  static void Init(int NumOfCounters, QString FileName);

  // Destroy the singleton object
  static void DeInit();

  // Get the pointer to the singleton object
  static CMaintenanceCounters* GetInstance();

  // Enable a counter
  DEFINE_METHOD_1(CMaintenanceCounters,TQErrCode,EnableCounter,int /*CounterNumber*/);

  // Disable a counter
  DEFINE_METHOD_1(CMaintenanceCounters,TQErrCode,DisableCounter,int /*CounterNumber*/);

  // Enable a range of counters
  void EnableCounters(int FirstCounterNumber, int LastCounterNumber);

  // Disable a range of counters
  void DisableCounters(int FirstCounterNumber, int LastCounterNumber);

  // Enable all counters
  void EnableAllCounters();

  // Disable all counters
  void DisableAllCounters();

  // Get the elapsed seconds since the reset of a counter
  DEFINE_METHOD_1(CMaintenanceCounters,ULONG,GetElapsedSeconds,int /*CounterNumber*/);

  // Get the reset date of a counter
  time_t GetResetDate(int CounterNumber);

  long GetWarningTimeInSeconds(int CounterNumber);

  bool GetUserWarning(int CounterNumber);

  // Get the reset date of a counter as a string
  QString GetResetDateAsString(int CounterNumber);

  // Advance a counter by X seconds
  void AdvanceCounterBySeconds(int CounterNumber, unsigned Seconds);

  // Advance a counter by X hours
  void AdvanceCounterByHours(int CounterNumber, unsigned Hours);

  // Reset  a counter
  DEFINE_METHOD_1(CMaintenanceCounters,TQErrCode,ResetCounter,int /*CounterNumber*/);

  // Load the counters data from the counters file
  void LoadFromFile();

  // Save the counters data to the counters file
  void SaveToFile();

  // Set the default warning time in hours for a specific counter (-1 = don't give warning)
  void SetDefaultWarningTimeInHours(int CounterID,int WarningTime, bool DemandsUserWarning = false);

  // Set the warning time in hours for a specific counter (-1 = don't give warning)
  void SetWarningTimeInSecs(int CounterID,long WarningTime);

  // Set the 'User Warning' flag for a specific counter
  void SetUserWarning(int CounterID,bool WarnUser);

  void SetCounterTime(int CounterID,long Secs);
  void SetResetDateAsString(int CounterID,const QString DateStr);

  // Return true if a specific counter is in warning state
  bool IsWarningState(int CounterID);

  // Reset the warning state
  void ResetWarningState(int CounterID = ALL_COUNTERS);

  void RestoreDefaultWarningTime(int CounterID);

  void WriteCountersToCSLog();
};

#endif

