//Logic.h
//
#ifndef __CLOGIC
#define __CLOGIC

#pragma once

//#include "afxwin.h"
#include "ReaderDfn.h"


#include "..\FELibOO\SerialPort.h" 
#include "..\FELibOO\Hitag.h"



////////////////////////////////////////////////////////////////
class CLogic
{
public:
	CLogic();
	~CLogic();

	int Disconnect();//Disconnects from a reader
	int Connect(int iComPort, int iBaudRate, int iParity, int iDataBits, int iStopBit);//Connects to a reader using a serial connection
	int SetChannel(int v_iChannel);//sets active channel
	bool GetSWVersion();//gets software version from reader
	int DetectTags();//gets tags from reader

	BYTE* ReadTag(DWORD v_dwTagID, BYTE* v_bResult, bool v_fReadBlockData, int* v_iDataSize, int v_iBeginByte, int v_iReadLength, bool v_iIsWrite = 0);//Reads from tag
	int WriteTag(DWORD v_dwTagID, BYTE* v_bResult, bool v_fWriteLockedData, BYTE* v_bData, int v_iBeginByte, int v_iWriteLength, int* v_iDataSize);//Writes data to tag

	int GetTagsCountValue();//returns value of m_iTagsCount: number of read tags
	unsigned __int64 GetUID(int v_iPosition);//returns the UID of a tag in a certain position


	int GetCurrentError();//Returns current error number


private:
	const static int MAX_TAGS			=	20;

	const static BYTE SAFE_FIRST_TAG	=	0x01;
	const static BYTE FIRST_TAG			=	0x11;
	const static BYTE NEXT_TAG			=	0x10;

	const static int PAGE_LEN_BYTES		=	4;
	const static int BLOCK_LEN_PAGES	=	4;
	const static int MAX_BYTES			=	256;
	const static int MAX_LOCKED_BLOCKS	=	8;


	CHitag* m_pHitag;
	bool *bWtLockMap;
	bool *bRdLockMap;
	BYTE* pWtData;

	int m_iTagsCount;
	int m_iCurrentError;

	unsigned __int64 m_uiTag[MAX_TAGS];
	BYTE*			m_pReadBytes;
	
	void GetPagesToRead(int* v_iFirstPage, int* v_iLastPage, int v_iBeginByte, int v_iLength);//gets the first and the last pages to read from the tag
	bool ReadConfig(bool *v_pLockMap);//reads configuration data of the tag
	bool GetLockBitStatus(BYTE v_cConfigByte);//gets the status of the lock bit
	bool ReadAllowed(bool v_fReadBlockData, bool v_bLockBit, bool* v_bLockMap, int v_iPageNum);//checks if current page can be read - if it's locked and need to read locked data or if it's unlocked and need to read unlocked data
	bool Lock(int v_iFirstPage, int v_iLastPage);//locked blocks on the tag
	//int ResetReadData();//resets memory of read data array
};


#endif