#include <stdio.h>
#include <assert.h>
#include "MaintenanceCounters.h"
#include "MaintenanceCountersFile.h"
#include "MaintenanceCountersFileDefs.h"
#include "QMonitor.h"
#include "QTimer.h"
#include "FrontEnd.h"
#include "QErrors.h"
#include "AppParams.h"
#include "AppLogFile.h"


///////////////////////////////////////////////////////////////////////////////
#define DEFINE_MAINTENANCE_COUNTER(_id_,_str_,_groupStr_,_warningTime_,_needToInformUser_,_isAdvanced_) _needToInformUser_,
#define DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(_id_,_str_,_groupStr_,_warningTime_,_needToInformUser_,_isAdvanced_,_displayName_) _needToInformUser_,
const bool ServiceNeedToInformLookup[]=
{
  #include "MaintenanceCountersDefsList.h"
  false
};
#undef DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME
#undef DEFINE_MAINTENANCE_COUNTER

#define DEFINE_MAINTENANCE_COUNTER(_id_,_str_,_groupStr_,_warningTime_,_needToInformUser_,_isAdvanced_) _isAdvanced_,
#define DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(_id_,_str_,_groupStr_,_warningTime_,_needToInformUser_,_isAdvanced_,_displayName_) _isAdvanced_,
const bool ServiceIsAdvancedLookup[]=
{
  #include "MaintenanceCountersDefsList.h"
  false
};
#undef DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME
#undef DEFINE_MAINTENANCE_COUNTER

#define DEFINE_MAINTENANCE_COUNTER(_id_,_str_,_groupStr_,_warningTime_,_needToInformUser,_isAdvanced) _str_,
#define DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(_id_,_str_,_groupStr_,_warningTime_,_needToInformUser_,_isAdvanced_,_displayName_) _displayName_,
const char *ServiceWarningLookup[]=
{
  #include "MaintenanceCountersDefsList.h"
  "Waste cartridge is full"           // WASTE_CARTRIDGE_ALERT_ID
};
#undef DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME
#undef DEFINE_MAINTENANCE_COUNTER

#define DEFINE_MAINTENANCE_COUNTER(_id_,_str_,_groupStr_,_warningTime_,_needToInformUser,_isAdvanced) _str_,
#define DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(_id_,_str_,_groupStr_,_warningTime_,_needToInformUser_,_isAdvanced_,_displayName_) _str_,
const char *CounterNameLookup[]=
{
  #include "MaintenanceCountersDefsList.h"
  "Waste cartridge is full"           // WASTE_CARTRIDGE_ALERT_ID
};
#undef DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME
#undef DEFINE_MAINTENANCE_COUNTER

#define DEFINE_MAINTENANCE_COUNTER(_id_,_str_,_groupStr_,_warningTime_,_needToInformUser,_isAdvanced) _groupStr_,
#define DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(_id_,_str_,_groupStr_,_warningTime_,_needToInformUser_,_isAdvanced_,_displayName_) _groupStr_,
const char *ServiceGroupStrLookup[]=
{
  #include "MaintenanceCountersDefsList.h"
  "General"
};
#undef DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME
#undef DEFINE_MAINTENANCE_COUNTER

#define DEFINE_MAINTENANCE_COUNTER(_id_,_str_,_groupStr_,_warningTime_,_needToInformUser,_isAdvanced) _id_##_COUNTER_ID,
#define DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(_id_,_str_,_groupStr_,_warningTime_,_needToInformUser_,_isAdvanced_,_displayName_) _id_##_COUNTER_ID,
const int ServiceCounterIDLookup[]=
{
  #include "MaintenanceCountersDefsList.h"
  NUM_OF_MAINTENANCE_COUNTERS,
};
#undef DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME
#undef DEFINE_MAINTENANCE_COUNTER

#define DEFINE_MAINTENANCE_COUNTER(_id_,_str_,_groupStr_,_warningTime_,_needToInformUser,_isAdvanced) _warningTime_,
#define DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(_id_,_str_,_groupStr_,_warningTime_,_needToInformUser_,_isAdvanced_,_displayName_) _warningTime_,

const long ServiceWarningTimeLookup[]=
{
  #include "MaintenanceCountersDefsList.h"
  -1
};
#undef DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME
#undef DEFINE_MAINTENANCE_COUNTER
///////////////////////////////////////////////////////////////////////////////

const int COUNTER_UPDATE_DELAY = 20000;
const int SECONDS_IN_ONE_HOUR = 3600;
const double MS_IN_ONE_SEC = 1000.0;
const int FILE_UPDATE_INTERVAL = 5 * 60 * 1000;

const int CURRENT_FILE_FORMAT_VERSION = 1;

// The pointer to the singleton instance
CMaintenanceCounters* CMaintenanceCounters::m_Instance = NULL;


// Private constructor - implemented as a singleton
CMaintenanceCounters::CMaintenanceCounters(int NumOfCounters, QString FileName) : CQThread(true,"MaintenanceCounters")
{
  INIT_METHOD(CMaintenanceCounters,EnableCounter);
  INIT_METHOD(CMaintenanceCounters,DisableCounter);
  INIT_METHOD(CMaintenanceCounters,GetElapsedSeconds);
  INIT_METHOD(CMaintenanceCounters,ResetCounter);

  m_NumOfCounters = NumOfCounters;
  m_Counters.resize(NumOfCounters);
  m_DefaultWarnings.resize(NumOfCounters);
  m_WarningsUpdates.resize(NumOfCounters);

  m_CountersFileName = FileName;

  m_BackupCountersFileName = FileName.replace(FileName.find_last_of(".")+1,3,"bak");
  m_SaveToFileTime = QGetTicks() + FILE_UPDATE_INTERVAL;
}

// Private destructor - implemented as a singleton
CMaintenanceCounters::~CMaintenanceCounters()
{
  SaveToFile();
  m_TerminateEvent.SetEvent();
  WaitFor();
}


// Create the singleton object
void CMaintenanceCounters::Init(int NumOfCounters, QString FileName)
{
  if (m_Instance == NULL)
    m_Instance = new CMaintenanceCounters(NumOfCounters,FileName);
}

// Destroy the singleton object
void CMaintenanceCounters::DeInit()
{
  if (m_Instance != NULL)
    delete m_Instance;
}

// Get the pointer to the singleton object
CMaintenanceCounters* CMaintenanceCounters::GetInstance()
{
  return m_Instance;
}

// Enable a counter
TQErrCode CMaintenanceCounters::EnableCounter(int CounterNumber)
{
  m_CountersMutex.WaitFor();

  // If already enabled
  if (m_Counters[CounterNumber].Enabled)
  {
    m_CountersMutex.Release();
    return Q_NO_ERROR;
  }

  m_Counters[CounterNumber].Enabled = true;
  m_Counters[CounterNumber].LastMeasuredTime = QGetTicks();

  m_CountersMutex.Release();
  return Q_NO_ERROR;
}

// Enable a range of counters
void CMaintenanceCounters::EnableCounters(int FirstCounterNumber, int LastCounterNumber)
{
  m_CountersMutex.WaitFor();

  for (int i = FirstCounterNumber; i <= LastCounterNumber; i++)
  {
    // If already enabled
    if (m_Counters[i].Enabled)
      continue;

    m_Counters[i].Enabled = true;
    m_Counters[i].LastMeasuredTime = QGetTicks();
  }

  m_CountersMutex.Release();
}


// Disable a counter
TQErrCode CMaintenanceCounters::DisableCounter(int CounterNumber)
{
  m_CountersMutex.WaitFor();

  // Ignore if the counter is not enabled
  if (!m_Counters[CounterNumber].Enabled)
  {
    m_CountersMutex.Release();
    return Q_NO_ERROR;
  }

  m_Counters[CounterNumber].Enabled = false;
  m_Counters[CounterNumber].ElapsedSeconds += ((double)(QGetTicks() - m_Counters[CounterNumber].LastMeasuredTime) / MS_IN_ONE_SEC);

  m_CountersMutex.Release();
  return Q_NO_ERROR;
}

// Disable a range of counters
void CMaintenanceCounters::DisableCounters(int FirstCounterNumber, int LastCounterNumber)
{
  m_CountersMutex.WaitFor();

  for (int i = FirstCounterNumber; i <= LastCounterNumber; i++)
  {
    // Ignore if the counter is not enabled
    if (!m_Counters[i].Enabled)
      continue;

    m_Counters[i].Enabled = false;
    m_Counters[i].ElapsedSeconds += ((double)(QGetTicks() - m_Counters[i].LastMeasuredTime) / MS_IN_ONE_SEC);
  }

  m_CountersMutex.Release();
}

// Enable all counters
void CMaintenanceCounters::EnableAllCounters()
{
  m_CountersMutex.WaitFor();

  for (int i = 0; i < m_NumOfCounters; i++)
  {
    // If the ccounter is already enabled
    if (m_Counters[i].Enabled)
      continue;

    m_Counters[i].Enabled = true;
    m_Counters[i].LastMeasuredTime = QGetTicks();
  }

  m_CountersMutex.Release();
}

// Disable all counters
void CMaintenanceCounters::DisableAllCounters()
{
  m_CountersMutex.WaitFor();

  unsigned long CurrTime = QGetTicks();
  for (int i = 0; i < m_NumOfCounters; i++)
  {
    // Ignore if the counter is not enabled
    if (!m_Counters[i].Enabled)
      continue;

    m_Counters[i].Enabled = false;
    m_Counters[i].ElapsedSeconds += ((double)(CurrTime - m_Counters[i].LastMeasuredTime) / MS_IN_ONE_SEC);
  }

  m_CountersMutex.Release();
}

// Get the elapsed seconds since the reset of a counter
unsigned long CMaintenanceCounters::GetElapsedSeconds(int CounterNumber)
{
  return m_Counters[CounterNumber].ElapsedSeconds;
}

long CMaintenanceCounters::GetWarningTimeInSeconds(int CounterNumber)
{
  return m_Counters[CounterNumber].WarningTime;
}


bool CMaintenanceCounters::GetUserWarning(int CounterNumber)
{
  return m_Counters[CounterNumber].NeedsUserUpdate;
}


// Get the reset date of a counter
time_t CMaintenanceCounters::GetResetDate(int CounterNumber)
{
  return m_Counters[CounterNumber].ResetDate;
}

// Get the reset date of a counter as a string
QString CMaintenanceCounters::GetResetDateAsString(int CounterNumber)
{
  return QDateToStr(m_Counters[CounterNumber].ResetDate);
}

// Advance a counter by X seconds
void CMaintenanceCounters::AdvanceCounterBySeconds(int CounterNumber, unsigned Seconds)
{
  m_CountersMutex.WaitFor();

  m_Counters[CounterNumber].ElapsedSeconds += Seconds;

  m_CountersMutex.Release();
}

// Advance a counter by X hours
void CMaintenanceCounters::AdvanceCounterByHours(int CounterNumber, unsigned Hours)
{
  m_CountersMutex.WaitFor();

  m_Counters[CounterNumber].ElapsedSeconds += Hours * SECONDS_IN_ONE_HOUR;

  m_CountersMutex.Release();
}

// Reset  a counter
TQErrCode CMaintenanceCounters::ResetCounter(int CounterNumber)
{
  m_CountersMutex.WaitFor();

  m_Counters[CounterNumber].ResetDate = QGetCurrentTime();
  m_Counters[CounterNumber].LastMeasuredTime = QGetTicks();
  m_Counters[CounterNumber].ElapsedSeconds = 0;

  if(m_Counters[CounterNumber].WarningState)
  {
	m_Counters[CounterNumber].WarningState = false;
	if(!IsUserWarningActive())
		FrontEndInterface->UpdateStatus(FE_CLEAR_SERVICE_ALERT,CounterNumber,true);
  }

  m_CountersMutex.Release();

  return Q_NO_ERROR;
}

// Load the counters data from the counters file
void CMaintenanceCounters::LoadFromFile()
{
  CAppParams *ParamsMgr = CAppParams::Instance();

  m_CountersMutex.WaitFor();

  try
  {
    CMaintenanceCountersFile File(m_CountersFileName);

    File.BeginRead();

    TMaintenanceCountersFileRecordList m_FileData(File.GetRecordsNum());

    TMaintenanceCountersFileRecordList::iterator i;
    for(i = m_FileData.begin(); i != m_FileData.end(); ++i)
			File.ReadRecord(&(*i));

    File.EndRead();

    for(int j = 0; j < m_NumOfCounters; j++)
    {
      // Try to find the current counter in the data read from the file
      for(i = m_FileData.begin(); i != m_FileData.end(); ++i)
        if((int)(*i).CounterID == j)
        {
          m_Counters[j].LastMeasuredTime = 0;
          m_Counters[j].ElapsedSeconds   = (*i).ElapsedSeconds;
          m_Counters[j].WarningTime      = (ParamsMgr->UpdateMaintenanceCounters) ? m_DefaultWarnings[j] : (*i).WarningTime;
          m_Counters[j].ResetDate        = (*i).ResetDate;
          m_Counters[j].Enabled          = false;
          m_Counters[j].WarningState     = false;
          m_Counters[j].NeedsUserUpdate  = (ParamsMgr->UpdateMaintenanceCounters) ? m_WarningsUpdates[j] : (*i).NeedsUserUpdate;
          break;
        }

      // If not found
      if(i == m_FileData.end())
      {
        m_Counters[j].LastMeasuredTime = 0;
        m_Counters[j].ElapsedSeconds   = 0;
        m_Counters[j].WarningTime      = m_DefaultWarnings[j];
        m_Counters[j].ResetDate        = QGetCurrentTime();
        m_Counters[j].Enabled          = false;
        m_Counters[j].WarningState     = false;
        m_Counters[j].NeedsUserUpdate  = m_WarningsUpdates[j];
      }
    }

  } catch(EQException& Err)
    {
      // In a case of an error reset all maintenance counters to defualts
      RestoreDefaults();
      QMonitor.ErrorMessage(Err.GetErrorMsg());
    }

    if (ParamsMgr->UpdateMaintenanceCounters)
	{
		SaveToFile();
		
		ParamsMgr->UpdateMaintenanceCounters = false; // Need to update counters only once
		ParamsMgr->SaveSingleParameter(&ParamsMgr->UpdateMaintenanceCounters);		
	}

  m_CountersMutex.Release();
}

// Save the counters data to the counters file
void CMaintenanceCounters::SaveToFile()
{
  m_CountersMutex.WaitFor();

  try
  {
    CMaintenanceCountersFile File(m_CountersFileName);

    File.SetVersionNumber(CURRENT_FILE_FORMAT_VERSION);
    File.SetRecordsNum(m_NumOfCounters);
    File.SetRecordSize(sizeof(TMaintenanceCountersFileRecord));
    File.BeginWrite();

    // Write counters records
    for(int i = 0; i < m_NumOfCounters; i++)
    {
      TMaintenanceCountersFileRecord Record;

      // Initialize file record
      Record.CounterID       = i;
      Record.ElapsedSeconds  = m_Counters[i].ElapsedSeconds;
      Record.WarningTime     = m_Counters[i].WarningTime;
      Record.ResetDate       = m_Counters[i].ResetDate;
      Record.NeedsUserUpdate = m_Counters[i].NeedsUserUpdate;

      File.WriteRecord(&Record);
    }

    File.EndWrite();

  } catch(EQException& Err)
    {
      QMonitor.ErrorMessage(Err.GetErrorMsg());
    }

  m_CountersMutex.Release();
}

// Thread execute function (override) 
void CMaintenanceCounters::Execute()
{
  try {
  while (!Terminated)
  {
    m_CountersMutex.WaitFor();

    unsigned long CurrTime = QGetTicks();
    for (int i = 0; i < m_NumOfCounters; i++)
    {
      if (m_Counters[i].Enabled)
      {
        m_Counters[i].ElapsedSeconds += (double)(CurrTime - m_Counters[i].LastMeasuredTime) / MS_IN_ONE_SEC;
        m_Counters[i].LastMeasuredTime = CurrTime;
      }
    }

    m_CountersMutex.Release();

    if (CurrTime >= m_SaveToFileTime)
    {
      SaveToFile();
      m_SaveToFileTime = CurrTime + FILE_UPDATE_INTERVAL;
    }

    CheckForServiceWarning();

	if(m_TerminateEvent.WaitFor(COUNTER_UPDATE_DELAY) == QLib::wrSignaled)
      break;
  }
  } catch(...) {
      CQLog::Write(LOG_TAG_GENERAL, "CMaintenanceCounters::Execute - unexpected error");
      if (!Terminated)
        throw EQException("CMaintenanceCounters::Execute - unexpected error");
  }
}

USHORT CMaintenanceCounters::CalculateCheckSum(BYTE *Buffer, int Size)
{
  USHORT CheckSum = 0;

  for (int i = 0; i < Size; i++)
    CheckSum += Buffer[i];

  return CheckSum;
}

// Set a warning time in hours for a specific counter (-1 = don't give warning)
void CMaintenanceCounters::SetDefaultWarningTimeInHours(int CounterID,int WarningTime, bool DemandsUserWarning /* = false by default*/)
{
  assert((unsigned)CounterID < m_Counters.size());

  // The counter maintain time in seconds
  if(WarningTime != -1)
    m_DefaultWarnings[CounterID] = WarningTime * 60 * 60;
  else
    m_DefaultWarnings[CounterID] = -1;

  m_WarningsUpdates[CounterID] = DemandsUserWarning;

}

// Set the warning time in hours for a specific counter (-1 = don't give warning)
void CMaintenanceCounters::SetWarningTimeInSecs(int CounterID,long WarningTime)
{
  assert((unsigned)CounterID < m_Counters.size());

  m_CountersMutex.WaitFor();

  if(WarningTime != -1L)
    m_Counters[CounterID].WarningTime = WarningTime;
  else
    m_Counters[CounterID].WarningTime = -1L;

  m_Counters[CounterID].WarningState = false;

  m_CountersMutex.Release();
}

// Set the 'User Warning' flag for a specific counter
void CMaintenanceCounters::SetUserWarning(int CounterID,bool WarnUser)
{
  assert((unsigned)CounterID < m_Counters.size());

  m_CountersMutex.WaitFor();

    m_Counters[CounterID].NeedsUserUpdate = WarnUser;

  m_CountersMutex.Release();
}

void CMaintenanceCounters::CheckForServiceWarning(void)
{
  // Check each counter
  for (int i = 0; i < m_NumOfCounters; i++)
    // If a warning hasent been issued yet
    if(!m_Counters[i].WarningState)
    {
      if((int)m_Counters[i].WarningTime != -1)
      {
        if((int)m_Counters[i].ElapsedSeconds > m_Counters[i].WarningTime)
        {
          m_Counters[i].WarningState = true;

          // Inform the user of the Service Alert only if defined as one...
          if (m_Counters[i].NeedsUserUpdate == true)
            FrontEndInterface->UpdateStatus(FE_SERVICE_ALERT,i,true);
        }
      }
    }
}

// Return true if a specific counter is in warning state
bool CMaintenanceCounters::IsWarningState(int CounterID)
{
  assert((unsigned)CounterID < m_Counters.size());

  return m_Counters[CounterID].WarningState;
}

// Restore all counters to their default states
void CMaintenanceCounters::RestoreDefaults(void)
{
  for(int i = 0; i < m_NumOfCounters; i++)
  {
    m_Counters[i].LastMeasuredTime = 0;
    m_Counters[i].ElapsedSeconds   = 0;
    m_Counters[i].WarningTime      = m_DefaultWarnings[i];
    m_Counters[i].ResetDate        = QGetCurrentTime();
    m_Counters[i].Enabled          = false;
    m_Counters[i].WarningState     = false;
    m_Counters[i].NeedsUserUpdate  = m_WarningsUpdates[i];
  }
}

void CMaintenanceCounters::RestoreDefaultWarningTime(int CounterID)
{
  m_Counters[CounterID].WarningTime = m_DefaultWarnings[CounterID];
}

void CMaintenanceCounters::SetCounterTime(int CounterID,long Secs)
{
  m_CountersMutex.WaitFor();
  m_Counters[CounterID].LastMeasuredTime = QGetTicks();
  m_Counters[CounterID].ElapsedSeconds = Secs;
  m_Counters[CounterID].WarningState = false;
  m_CountersMutex.Release();
}

void CMaintenanceCounters::SetResetDateAsString(int CounterID,const QString DateStr)
{
  m_CountersMutex.WaitFor();
  m_Counters[CounterID].ResetDate = QStrToDate(DateStr);
  m_CountersMutex.Release();
}

// Reset the warning state
void CMaintenanceCounters::ResetWarningState(int CounterID /* Default = ALL_COUNTERS */)
{
  m_CountersMutex.WaitFor();

  if (CounterID == ALL_COUNTERS)
  {
    for (int i = 0; i < m_NumOfCounters; i++)
      m_Counters[i].WarningState = false;
  } else
  {
    m_Counters[CounterID].WarningState = false;
  }

  m_CountersMutex.Release();

}

bool CMaintenanceCounters::IsUserWarningActive()
{
  for (int i = 0; i < m_NumOfCounters; i++)
	if(m_Counters[i].WarningState && m_Counters[i].NeedsUserUpdate)
		return true;

  return false;
}

void CMaintenanceCounters::WriteCountersToCSLog()
{
	int hours;
	QString	counterName, msg;
	for (int i = 0; i < m_NumOfCounters; i++)
	{
		hours = (m_Counters[i].ElapsedSeconds / 3600);
		msg = QFormatStr( "Maintenance counter \"%s\" Value is: %d hours.",CounterNameLookup[i], hours);
		//CQLog::Write(LOG_TAG_SERVICE, msg);
	}
}
