//Tag.h
//


#ifndef __CTAG
#define __CTAG



#include "Logic.h"

////////////////////////////////////////////////////////////////
class CTag
{
public:
	CTag();
	~CTag();
	int GetCurrentError();//Returns current error number
	int SetUID(unsigned __int64	v_uiUID);
	unsigned __int64 GetUID();
	BYTE* ReadTag(bool v_bReadBlockData, int* v_iDataSize, int v_iBeginByte, int v_iReadLength);//bool v_fReadBlockData, int* v_iDataSize
	int WriteTag(bool v_bWriteLockedData, int* v_iDataSize, BYTE* v_bData, int v_iBeginByte, int v_iWriteLength);//Writes data to tag
	int SetLogicPtr(CLogic* v_pLogic);//sets value to m_pLogic
	int Settings(int v_iLockDataSize, BYTE bPaddingByte = 0x00);//Sets blocked data size (in bytes) into the tags currently in the reader

	BYTE GetPaddingByte();//returns the padding byte of tag
	int GetLockedDataSize();//Gets locked data size (in bytes)
	int GetTagInfo();//Gets information from the tag
	int GetMemorySize();//returns the number of memory bytes of tag
protected:
    int SetInitParam();// initialization of parameters


	unsigned __int64	m_uiUID;
	int					m_iLockedDataSizeReserve;//number of bytes reserved for locking
	BYTE				m_bPadByte;
	CLogic*				m_pLogic;
};


#endif