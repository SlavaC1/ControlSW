/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: MC_Updater                                              *
 * Module: Maintenance counters file format read/write object       *
 *                                                                  *
 * Compilation: Standard C/C++.                                     *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 08/10/2003                                           *
 * Last upate: 08/10/2003                                           *
 ********************************************************************/
#include <vcl.h>
#include "MaintenanceCountersFile.h"
#include "MaintenanceCountersFileDefs.h"
#include "QErrors.h"

const int CURRENT_FILE_FORMAT_VERSION = 1;
// Constructor
CMaintenanceCountersFile::CMaintenanceCountersFile(const QString FileName)
{
  m_File.AssignFile(FileName);
  m_VersionNum = 0;
  m_RecordsNum = 0;
  m_RecordSize = 0;
}

// Destructor
CMaintenanceCountersFile::~CMaintenanceCountersFile(void)
{
}

// Assign a file to the maintenance counters object
void CMaintenanceCountersFile::AssignFile(const QString FileName)
{
  m_File.AssignFile(FileName);
  m_CountersFileName =  FileName ;
  
}

// Open file for read session
void CMaintenanceCountersFile::BeginRead(void)
{
  // Start reading
  if(m_File.BeginRead() != Q_NO_ERROR)
    throw EMaintenanceCountersFile("Maintenance counters file read error");

  // Read file header
  TMaintenanceCountersFileHeader Header;

  if(!m_File.Read(&Header,sizeof(TMaintenanceCountersFileHeader)))
    throw EMaintenanceCountersFile("Error while reading maintenance counters file header");

  // Verify signature
  if((Header.Signature[0] != 'M') || (Header.Signature[1] != 'C'))
    throw EMaintenanceCountersFile("Maintenance counters file signature error");

  // Initialize internal variables according to the header
  m_VersionNum = Header.VersionNum;
  m_RecordsNum = Header.RecordsNum;
  m_RecordSize = Header.RecordSize;
}

// Read next record from file
void CMaintenanceCountersFile::ReadRecord(void *RecordPtr)
{
  if(!m_File.Read(RecordPtr,m_RecordSize))
    throw EMaintenanceCountersFile("Error while reading maintenance counters file record");
}

// End file read session
void CMaintenanceCountersFile::EndRead(void)
{
  m_File.EndRead();
}

// Begin file write session
void CMaintenanceCountersFile::BeginWrite(void)
{
  m_File.BeginWrite();

  // Write file header
  TMaintenanceCountersFileHeader Header;  
  Header.Signature[0] = 'M';
  Header.Signature[1] = 'C';
  Header.VersionNum   = m_VersionNum;
  Header.RecordsNum   = m_RecordsNum;
  Header.RecordSize   = m_RecordSize;

  m_File.Write(&Header,sizeof(TMaintenanceCountersFileHeader));
}

// Write next file record
void CMaintenanceCountersFile::WriteRecord(void const *Record)
{
  m_File.Write(Record,m_RecordSize);
}

// End file write session
void CMaintenanceCountersFile::EndWrite(void)
{
  m_File.EndWrite();
}
void CMaintenanceCountersFile::LoadFromFile()
{
  try
  {
    m_Counters.resize(m_NumOfCounters);
   
    CMaintenanceCountersFile File(m_CountersFileName);

    File.BeginRead();

    TMaintenanceCountersFileRecordList m_FileData(File.GetRecordsNum());

    TMaintenanceCountersFileRecordList::iterator i;
    for(i = m_FileData.begin(); i != m_FileData.end(); i++)
      File.ReadRecord(i);

    File.EndRead();

    for(int j = 0; j < m_NumOfCounters; j++)
    {
      // Try to find the current counter in the data read from the file
      for(i = m_FileData.begin(); i != m_FileData.end(); i++)
        if((int)(*i).CounterID == j)
        {
          m_Counters[j].LastMeasuredTime = 0;
          m_Counters[j].ElapsedSeconds   = (*i).ElapsedSeconds;
          m_Counters[j].WarningTime      = (*i).WarningTime;
          m_Counters[j].ResetDate        = (*i).ResetDate;
          m_Counters[j].Enabled          = false;
          m_Counters[j].WarningState     = false;
          m_Counters[j].NeedsUserUpdate  = (*i).NeedsUserUpdate;
          break;
        }

      // If not found
      if(i == m_FileData.end())
      {
        m_Counters[j].LastMeasuredTime = 0;
        m_Counters[j].ElapsedSeconds   = 0;
        m_Counters[j].ResetDate        = QGetCurrentTime();

      }
    }

  } catch(EQException& Err)
    {
       	ShowMessage(Err.GetErrorMsg().c_str());
    }

  }

void CMaintenanceCountersFile::SaveToFile()
{

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
      ShowMessage(Err.GetErrorMsg().c_str());
    }

}
void CMaintenanceCountersFile::SetNumOfCounters( int counters)
{
  m_NumOfCounters =   counters ;
}
int CMaintenanceCountersFile::GetNumfCounters()
{
  return  m_NumOfCounters;
}
void CMaintenanceCountersFile::SetCountersFileName(QString fileName )
{
  	m_CountersFileName =  fileName;
}
double CMaintenanceCountersFile::GetElapsedSeconds(int counterParameter)
{
    if (counterParameter< m_NumOfCounters)
    	return  m_Counters[counterParameter].ElapsedSeconds ;
    else
        throw EMaintenanceCountersFile(QFormatStr("Can not find Maintenance Counter with index: %d in file %s", counterParameter,m_CountersFileName));
}
void CMaintenanceCountersFile::SetElapsedSeconds(int counterParameter,double ElapsedSeconds)
{
    if (counterParameter< m_NumOfCounters && ElapsedSeconds >=0 )
    {
    	if (m_Counters[counterParameter].ElapsedSeconds == 0)
    		m_Counters[counterParameter].ElapsedSeconds     = ElapsedSeconds ;
    }
    else
		throw EMaintenanceCountersFile(QFormatStr("Can not find Maintenance Counter with index: %d in file %s", counterParameter,m_CountersFileName));
}
time_t CMaintenanceCountersFile::GetResetDate(int counterParameter)
{
    if (counterParameter< m_NumOfCounters)
    	return  m_Counters[counterParameter].ResetDate ;
    else
        throw EMaintenanceCountersFile(QFormatStr("Can not find Maintenance Counter with index: %d in file %s", counterParameter,m_CountersFileName));
}
void CMaintenanceCountersFile::SetResetDate(int counterParameter,time_t ResetDate )
{
	if (counterParameter< m_NumOfCounters )
        m_Counters[counterParameter].ResetDate          = ResetDate ;
    else
		throw EMaintenanceCountersFile(QFormatStr("Can not find Maintenance Counter with index: %d in file %s", counterParameter,m_CountersFileName));
}
