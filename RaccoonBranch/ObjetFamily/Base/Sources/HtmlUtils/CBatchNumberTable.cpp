//---------------------------------------------------------------------------


#pragma hdrstop

#include "CBatchNumberTable.h"
#include "GlobalDefs.h"
#include "QLogFile.h"
#include "AppLogFile.h"
#include "Q2RTApplication.h"
//#include "CHTML_BatchNoReport.h"

CBatchNoTable* CBatchNoTable::m_this = NULL;

const char BATCH_NUMBER_TABLE_FILE[] = "Configs\\Material batch number.htm";

CTankBatchNoTable::CTankBatchNoTable():m_nextBatchNoIndex(0) {}

void CTankBatchNoTable::SetBatchNo(char* batchNumber)
{
  if(batchNumber==NULL)
	return;
  if(m_nextBatchNoIndex==0)
	batchNoArr[m_nextBatchNoIndex++] = batchNumber;
  else //if same batch no. was already added, skip it
  {
	if(strcmp(batchNoArr[m_nextBatchNoIndex-1].c_str(),batchNumber)!=0)
	{
	  batchNoArr[m_nextBatchNoIndex++] = batchNumber;
	}
  }
}

QString CTankBatchNoTable::GetLastBatchNo()
{
  QString tmp = "";
  if(m_nextBatchNoIndex==0)
    return tmp;
  return batchNoArr[m_nextBatchNoIndex-1];
}

QString CTankBatchNoTable::GetResinType()
  {
	QString tmp = "";
	if(m_nextBatchNoIndex==0)
      return tmp;
	return m_resinType;
  }

QString CTankBatchNoTable::GetBatchNo(int index)
{
  QString tmp = "";
  if(index>=0 && index<m_nextBatchNoIndex)
    return batchNoArr[index];
  else
    return tmp;
}

void CTankBatchNoTable::PrintBatchNoTable()
{
  if(m_nextBatchNoIndex==0)
    return;
  CQLog::Write(LOG_TAG_OBJET_MACHINE,"Objet: Resin type: %s", m_resinType.c_str());
  for(int i=0;i<m_nextBatchNoIndex;i++)
    CQLog::Write(LOG_TAG_OBJET_MACHINE,"Objet: BatchNumber %d: %s", i,batchNoArr[i].c_str());

}
//constructor
CBatchNoTable::CBatchNoTable():m_isPrinting(false)
{
  QString FileName = Q2RTApplication->AppFilePath.Value() + BATCH_NUMBER_TABLE_FILE;
  m_BatchNoReport = new CHTML_BatchNoReport(FileName);
}

void CBatchNoTable::Init(bool isPrinting, QString printStart, QString trayID, QString printingMode)
{
  m_isPrinting = isPrinting;
  m_printStart = printStart;
  m_trayID = trayID;
  m_printingMode = printingMode;
}

CBatchNoTable* CBatchNoTable::GetInstance()
{
	if(m_this == NULL)
		m_this = new CBatchNoTable();
	return m_this;
}

void CBatchNoTable::DeInit()
{
	Q_SAFE_DELETE(m_this);
}

CBatchNoTable::~CBatchNoTable()
{
  Q_SAFE_DELETE(m_BatchNoReport);
}

void CBatchNoTable::ClearBatchNoTable()
{
	for (int i=0; i < TOTAL_NUMBER_OF_CONTAINERS; i++)
	{
	  m_batchNoArr[i].ClearBatchNoTable();
	}
}

void CBatchNoTable::PrintTanksBatchNoTable()
{
  m_BatchNoReport->CreateBatchNoReport();
}

void CBatchNoTable::ShowMaterialBatchNoReport()
{
  m_BatchNoReport->OpenReportFile();
}

void CBatchNoTable::SetPrintEnd(QString printEnd)
{
  m_printEnd = printEnd;
  m_isPrinting = false;
  //write the batch no. table to "Material batch number.htm"
  PrintTanksBatchNoTable();
}

void CBatchNoTable::AddTankBatchNumber(int tankIndex,char * batchNo, QString resinType)
{
  if(m_isPrinting)
  {
      m_batchNoArr[tankIndex].SetResinType(resinType);
      m_batchNoArr[tankIndex].SetBatchNo(batchNo);
	  CQLog::Write(LOG_TAG_OBJET_MACHINE,"Objet: Batch number:%s Resin type: %s, Tank index: %d", batchNo, resinType.c_str(),tankIndex);
   }
}


//---------------------------------------------------------------------------

#pragma package(smart_init)
