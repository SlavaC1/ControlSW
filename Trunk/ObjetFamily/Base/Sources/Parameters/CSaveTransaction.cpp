//---------------------------------------------------------------------------


#pragma hdrstop

#include "CSaveTransaction.h"
#include "FEResources.h"
#include "AppLogFile.h"
//#include "GlobalDefs.h"

CSaveTransaction::CSaveTransaction():m_commit(false)
{
    m_swFile  = NULL;
	m_dataFile = NULL;
	m_backupFile = NULL;
	m_state =  stDataInA;
	m_stSwFilePath = Q2RTApplication->AppFilePath.Value() + LOAD_STRING(IDS_CONFIGS_DIRNAME) + "DataSwitch.txt";
	m_stAFilePath = Q2RTApplication->AppFilePath.Value() + LOAD_STRING(IDS_CONFIGS_DIRNAME) + "MainData.txt";
	m_stBFilePath = Q2RTApplication->AppFilePath.Value() + LOAD_STRING(IDS_CONFIGS_DIRNAME) + "BackupData.txt";
}

void CSaveTransaction::LoadFiles()
{
	errno = 0;

	m_swFile = fopen(m_stSwFilePath.c_str(),"r+");
	if(m_swFile!=NULL)
	  fscanf(m_swFile,"%x",&m_state);

	if (m_state != stDataInA && m_state != stDataInB)
		CQLog::Write(LOG_TAG_GENERAL,"Switch file corrupted");
	if(m_state == stDataInA)
	{
		m_dataFile = fopen(m_stAFilePath.c_str(),"r+");
		m_backupFile = fopen(m_stBFilePath.c_str(),"r+");
	}
	else
	{
	  m_dataFile = fopen(m_stBFilePath.c_str(),"r+");
	  m_backupFile = fopen(m_stAFilePath.c_str(),"r+");
	}
}

void CSaveTransaction::CreateFiles()
{
	if (!FileExists(m_stSwFilePath.c_str()))
	{
	  errno = 0;


	  m_swFile = fopen(m_stSwFilePath.c_str(),"w+");
	  m_dataFile = fopen(m_stAFilePath.c_str(),"w+");
	  m_backupFile = fopen(m_stBFilePath.c_str(),"w+");

	  if(m_swFile!=NULL && m_dataFile!=NULL && m_backupFile!=NULL)
	  {
	    int i = 0;
		fprintf(m_swFile, "%x",stDataInA);//m_swFile.WriteByte (otherState);
		fprintf(m_dataFile,"%d %d %d %d",i,i,i,i);
	  }
	  else
      {
		CQLog::Write(LOG_TAG_GENERAL,"Failed opening file: %s", strerror(errno));
		throw EQException(QFormatStr("Failed opening file: %s", strerror(errno)));
	  }

	  fclose(m_dataFile);
	  fclose(m_backupFile);
	  fclose(m_swFile);
	}
}

CSaveTransaction::~CSaveTransaction()
{
	if (m_commit)
	{
	  if(m_state == stDataInA)
		m_dataFile = freopen(m_stAFilePath.c_str(),"w+",m_dataFile); //m_dataFile.Empty ();
	  else
		m_dataFile = freopen(m_stBFilePath.c_str(),"w+",m_dataFile); //m_dataFile.Empty ();
	}
	else
	{
	  if(m_state == stDataInA)
		m_backupFile = freopen(m_stBFilePath.c_str(),"w+",m_backupFile); //m_backupFile.Empty ();
	  else
		m_backupFile = freopen(m_stAFilePath.c_str(),"w+",m_backupFile); //m_backupFile.Empty ();
	}

	if (m_dataFile) 
		fclose(m_dataFile);
	if (m_backupFile)
		fclose(m_backupFile);
	if (m_swFile)
		fclose(m_swFile);
}
void CSaveTransaction::Commit()
{
	State otherState;
	errno = 0;

	if (m_state == stDataInA)
		otherState = stDataInB;
	else
		otherState = stDataInA;

	fflush(m_backupFile);

	m_swFile = freopen(m_stSwFilePath.c_str(),"w+",m_swFile); //m_backupFile.Empty ();
	if(m_swFile!=NULL)
	{
        fprintf(m_swFile, "%x",otherState);
		fflush(m_swFile);
		m_commit = true;
    }
	else
		CQLog::Write(LOG_TAG_GENERAL,"Failed reopening file: %s", strerror(errno));
}

//---------------------------------------------------------------------------

#pragma package(smart_init)
