//---------------------------------------------------------------------------


#pragma hdrstop

#include "AddRemoveRFIDCounter.h"
#include "QLogFile.h"
#include "AppLogFile.h"

CAddRemoveRFIDCounter::CAddRemoveRFIDCounter()
{
	m_NumOfReconnections = 0;
}
CAddRemoveRFIDCounter::~CAddRemoveRFIDCounter()
{

}
//---------------------------------------------------------------------------
int CAddRemoveRFIDCounter::GetNumOfReconnections()
{
  CQLog::Write(LOG_TAG_OBJET_MACHINE,"Objet: RFID GetNumberOfReconnections %d",m_NumOfReconnections );
  return m_NumOfReconnections;
}
//---------------------------------------------------------------------------
void CAddRemoveRFIDCounter::ResetCounter()
{
  m_NumOfReconnections = 0;
}
//---------------------------------------------------------------------------
void CAddRemoveRFIDCounter::IncCounter()
{
  m_NumOfReconnections++;
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
