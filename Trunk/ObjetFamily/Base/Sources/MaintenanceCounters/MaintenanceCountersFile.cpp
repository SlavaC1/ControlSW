/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Eden                                                    *
 * Module: Maintenance counters file format read/write object       *
 *                                                                  *
 * Compilation: Standard C/C++.                                     *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 08/10/2003                                           *
 * Last upate: 08/10/2003                                           *
 ********************************************************************/

#include "MaintenanceCountersFile.h"
#include "MaintenanceCountersFileDefs.h"
#include "QErrors.h"


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

