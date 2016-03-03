//Tag.cpp
//

//#include "stdafx.h"
#include "Tag.h"

/////////////////////////////////////////////////////////////////
// Function name...: CTag()
// Description.....: Constructor
// Parameters......: none
// Return..........: none
/////////////////////////////////////////////////////////////////
CTag::CTag()
{
	SetInitParam();
}

/////////////////////////////////////////////////////////////////
// Function name...: ~CTag()
// Description.....: Destructor
// Parameters......: none
// Return..........: none
/////////////////////////////////////////////////////////////////
CTag::~CTag()
{

}

/////////////////////////////////////////////////////////////////
// Function name...: SetInitParam()
// Description.....: Intializes class parameters
// Parameters......: none
// Return..........: none
/////////////////////////////////////////////////////////////////
int CTag::SetInitParam()
{
	m_uiUID = 0;

	return 1;
}

/////////////////////////////////////////////////////////////////
// Function name...: SetUID
// Description.....: Sets value to m_uiUID
// Parameters......: unsigned __int64	v_uiUID
// Return..........: none
/////////////////////////////////////////////////////////////////
int CTag::SetUID(unsigned __int64 v_uiUID)
{
	m_uiUID = v_uiUID;
	return 1;
}

/////////////////////////////////////////////////////////////////
// Function name...: GetUID
// Description.....: Gets value of m_uiUID
// Parameters......: none
// Return..........: unsigned __int64 m_uiUID
/////////////////////////////////////////////////////////////////
unsigned __int64 CTag::GetUID()
{
	return m_uiUID;
}

/////////////////////////////////////////////////////////////////
// Function name...: SetLogicPtr
// Description.....: sets value to m_pLogic
// Parameters......: CLogic* v_pLogic
// Return..........: none
/////////////////////////////////////////////////////////////////
int CTag::SetLogicPtr(CLogic* v_pLogic)
{
	m_pLogic = v_pLogic;
	return 1;
}

/////////////////////////////////////////////////////////////////
// Function name...: Settings
// Description.....: Sets blocked data size (in bytes) into the tags currently in the reader
// Parameters......: int v_iLockDataSize, BYTE bPaddingByte
// Return..........: none
/////////////////////////////////////////////////////////////////
int CTag::Settings(int v_iLockDataSize, BYTE v_bPaddingByte)
{
	m_iLockedDataSizeReserve = v_iLockDataSize;
	m_bPadByte = v_bPaddingByte;
	return 1;
}

/////////////////////////////////////////////////////////////////
// Function name...: GetLockedDataSize
// Description.....: Gets locked data size (in bytes)
// Parameters......: none
// Return..........: int m_iLockedDataSizeReserve
/////////////////////////////////////////////////////////////////
int CTag::GetLockedDataSize()
{
	return m_iLockedDataSizeReserve;
}

/////////////////////////////////////////////////////////////////
// Function name...: GetPaddingByte
// Description.....: returns the padding byte of tag
// Parameters......: none
// Return..........: BYTE m_bPadByte
/////////////////////////////////////////////////////////////////
BYTE CTag::GetPaddingByte()
{
	return m_bPadByte;
}

/////////////////////////////////////////////////////////////////
// Function name...: GetTagInfo
// Description.....: Gets information from the tag
// Parameters......: none
// Return..........: none
/////////////////////////////////////////////////////////////////
int CTag::GetTagInfo()
{

	return 0;
}

/////////////////////////////////////////////////////////////////
// Function name...: GetMemorySize
// Description.....: returns the number of memory bytes of tag
// Parameters......: CLogic* v_pLogic
// Return..........: none
/////////////////////////////////////////////////////////////////
int CTag::GetMemorySize()
{
	return 0;
}

/////////////////////////////////////////////////////////////////
// Function name...: ReadTag
// Description.....: Reads from tag
// Parameters......: bool v_fReadBlockData, int* v_iDataSize, int v_iBeginByte, int v_iReadLength
// Return..........: BYTE*
/////////////////////////////////////////////////////////////////
BYTE* CTag::ReadTag(bool v_bReadBlockData, int* v_iDataSize, int v_iBeginByte, int v_iReadLength)
{
	BYTE bResult;
	return m_pLogic->ReadTag((DWORD)m_uiUID, &bResult, v_bReadBlockData, v_iDataSize, v_iBeginByte, v_iReadLength);
}

/////////////////////////////////////////////////////////////////
// Function name...: WriteTag
// Description.....: Writes data to tag
// Parameters......: bool v_fWriteLockedData, int v_iReadLength, BYTE* v_bData, int v_iBeginByte, int* v_iDataSize
// Return..........: none
/////////////////////////////////////////////////////////////////
int CTag::WriteTag(bool v_bWriteLockedData, int* v_iDataSize, BYTE* v_bData, int v_iBeginByte, int v_iWriteLength )
{
	BYTE bResult;
	return m_pLogic->WriteTag((DWORD)m_uiUID, &bResult, v_bWriteLockedData, v_bData, v_iBeginByte, v_iWriteLength, v_iDataSize);
}

/////////////////////////////////////////////////////////////////
// Function name...: GetLastError
// Description.....: Returns current error number
// Parameters......: none
// Return..........: none
/////////////////////////////////////////////////////////////////
int CTag::GetCurrentError()
{
	return m_pLogic->GetCurrentError();
}

