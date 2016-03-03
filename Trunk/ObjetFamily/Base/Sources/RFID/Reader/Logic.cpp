//Tag.cpp
//

//#include "stdafx.h"
#include "Logic.h"
#include <iostream>



/////////////////////////////////////////////////////////////////
// Function name...: CLogic()
// Description.....: Constructor
// Parameters......: none
// Return..........: none
/////////////////////////////////////////////////////////////////
CLogic::CLogic()
{
	//m_pReadBytes = 0;
        m_iTagsCount = 0 ;
        m_iCurrentError= 0 ;
	m_pHitag = 0;
	bRdLockMap = NULL;
	bWtLockMap = NULL;
	pWtData = NULL;
	m_pReadBytes = NULL;
        for(int i = 0; i< MAX_TAGS; ++i)
        {
            m_uiTag[i] = 0 ;
        }
}

/////////////////////////////////////////////////////////////////
// Function name...: ~CLogic()
// Description.....: Destructor
// Parameters......: none
// Return..........: none
/////////////////////////////////////////////////////////////////
CLogic::~CLogic()
{
	//ResetReadData();
	if ( m_pHitag )
		delete m_pHitag;
	if(bRdLockMap) 
	  delete[] bRdLockMap;
	if(bWtLockMap) 
	  delete[] bWtLockMap;
	if(pWtData) 
	  delete[] pWtData;
	if(m_pReadBytes) 
	  delete[] m_pReadBytes;
}

/////////////////////////////////////////////////////////////////
// Function name...: Connect
// Description.....: Connects to a reader using a serial connection
// Parameters......: int iComPort, int iBaudRate, int iParity, int iDataBits, int iStopBit
// Return..........: 1 - connect, -1 - unable to connect (serial problem), -2 - unable to connect (reader problem), 0 - unspecified problem
/////////////////////////////////////////////////////////////////
int CLogic::Connect(int iComPort/*, int iBaudRate, int iParity, int iDataBits, int iStopBit*/)
{
	int iRet;
	char pComPort[6];

    try
	{
	  m_pHitag = new CHitag();
	  bRdLockMap = new bool [ MAX_LOCKED_BLOCKS ];
	  bWtLockMap = new bool [ MAX_LOCKED_BLOCKS ];
	  pWtData = new BYTE[10*PAGE_LEN_BYTES];
	  m_pReadBytes = new BYTE[10];
	}
	catch(std::bad_alloc& exc)
	{
	  if(m_pHitag) delete m_pHitag;
	  if (bRdLockMap) delete[] bRdLockMap;
	  if (bWtLockMap) delete[] bWtLockMap;
	  if (pWtData) delete[] pWtData;
	  if (m_pReadBytes) delete[] m_pReadBytes;
	  return -1;
	}


	pComPort[0] = 'C';
	pComPort[1] = 'O';
	pComPort[2] = 'M';
	if ( iComPort >= 10 )
	{
		pComPort[3] = (int)iComPort/10 + '0';
		pComPort[4] = (int)iComPort%10 + '0';
		pComPort[5] = '\0';
	}
	else
	{
		pComPort[3] = (int)iComPort%10 + '0';
		pComPort[4] = '\0';
	}

	iRet = (int)m_pHitag->SP_Init( pComPort );

	return iRet;

}

/////////////////////////////////////////////////////////////////
// Function name...: Disconnect
// Description.....: Disconnects from a reader
// Parameters......: none
// Return..........: 1 - disconnect, 0 - else
/////////////////////////////////////////////////////////////////
int CLogic::Disconnect()
{

	return m_pHitag->SP_Exit();

}

/////////////////////////////////////////////////////////////////
// Function name...: SetChannel
// Description.....: sets active channel
// Parameters......: int v_iChannel
// Return..........: true - success, false - else
/////////////////////////////////////////////////////////////////
int CLogic::SetChannel(int v_iChannel)
{
	BYTE bChannel;
	switch (v_iChannel)
	{
	case 1:
		bChannel = 0x00;
		break;
	case 2:
		bChannel = 0x04;
		break;
	case 3:
		bChannel = 0x08;
		break;
	case 4:
		bChannel = 0x0C;
		break;

	}

	return m_pHitag->RWD_SetOutput(bChannel);

	
}

/////////////////////////////////////////////////////////////////
// Function name...: GetSWVersion
// Description.....: gets software version from reader
// Parameters......: none
// Return..........: 1 - disconnect, 0 - else
/////////////////////////////////////////////////////////////////
bool CLogic::GetSWVersion()
{
	bool bRet = true;
	int i; 
	BYTE * pVersion;
	BYTE pExpVersion[6] = SW_VERSION;
	BYTE bResult;
	pVersion = new BYTE[256];

	bResult = m_pHitag->RWD_GetVersion(pVersion);

	
	if ((BYTE)ESERIELL == bResult)
	{
		delete [] pVersion;
		return false;
	}
	for (i=0;i<1;i++)
	{
		if (pVersion[i]!=pExpVersion[i])
		{
			bRet = false;
			break;
		}

	}
	delete [] pVersion;
	return bRet;
}

/////////////////////////////////////////////////////////////////
// Function name...: DetectTags
// Description.....: gets tags from reader
// Parameters......: none
// Return..........: 1 - disconnect, 0 - else
/////////////////////////////////////////////////////////////////
int CLogic::DetectTags()
{
	DWORD iSerialNumber = 0;
	byte isMore = 0;
	m_iTagsCount = 0;

	byte result = m_pHitag->HTS_GetSnrReset( (byte)SAFE_FIRST_TAG , & iSerialNumber , & isMore );

	
	if( 0 == result || iSerialNumber != 0 )
	{
		m_uiTag[m_iTagsCount++]= iSerialNumber;
		/*
		while( (0 == (result = HTS_GetSnrReset( (byte)NEXT_TAG , & iSerialNumber , & isMore ))) && iSerialNumber != 0 )
		{
			m_uiTag[m_iTagsCount++] = iSerialNumber;
		}
		*/
	}
	return m_iTagsCount;
}

/////////////////////////////////////////////////////////////////
// Function name...: ReadTag
// Description.....: Reads from tag
// Parameters......: DWORD v_dwTagID, BYTE* v_bResult, bool v_fReadBlockData, int* v_iDataSize, int v_iBeginByte, int v_iReadLength
// Return..........: BYTE*
/////////////////////////////////////////////////////////////////
BYTE* CLogic::ReadTag(DWORD v_dwTagID, BYTE* v_bResult, bool v_fReadBlockData, int* v_iDataSize, int v_iBeginByte, int v_iReadLength, bool v_iIsWrite)
{
	int i, iByteCount = 0;
	DWORD dwResponse;
	BYTE pPageData[ PAGE_LEN_BYTES ];
	int iFirstPage, iLastPage;


	*v_iDataSize = 0;

	if ( v_iIsWrite == 1 )
		*v_bResult = m_pHitag->HTS_SelectSnrReset((BYTE)0x21, v_dwTagID, &dwResponse);
	else
		*v_bResult = m_pHitag->HTS_SelectSnrReset((BYTE)0x81, v_dwTagID, &dwResponse);
	if ( *v_bResult != ERROR_SUCCESS)
	{
		m_iCurrentError = ERROR_TAG_SELECT;
		return NULL;
	}

	//reads configuration of the tag
	//bLockMap = new bool [ MAX_LOCKED_BLOCKS ];
	
	/*bool bLockBit = */ ReadConfig(bRdLockMap);
	/*
	//if data on the tag is not locked and need to read only locked data
	if ( !bLockBit && v_fReadBlockData )
	{
		delete [] bLockMap;
		m_pReadBytes = 0;
		m_iCurrentError = LOCK_FAIL;
		return m_pReadBytes;
	}
	*/
	iFirstPage = iLastPage = 0;
	GetPagesToRead(&iFirstPage, &iLastPage, v_iBeginByte, v_iReadLength);
	//ResetReadData();
	//m_pReadBytes = new BYTE [ v_iReadLength ];

	for (i = iFirstPage; i <= iLastPage ; i++)
	{


		if ((*v_bResult = m_pHitag->HT1_ReadPage( 0x00, i, pPageData )) == ERROR_SUCCESS)
		{
			for ( int j = 0; j < PAGE_LEN_BYTES; j++)
			{
				if ( i != iFirstPage && i != iLastPage)
				{
					if ( ReadAllowed(v_fReadBlockData, /*bLockBit,*/ bRdLockMap, i) && iByteCount < v_iReadLength )
						m_pReadBytes[ iByteCount++ ] = pPageData[ j ];
				}
				else if ( i == iFirstPage )
				{	
					if ( j >= v_iBeginByte % PAGE_LEN_BYTES )
					{
						if ( ReadAllowed(v_fReadBlockData, /*bLockBit,*/ bRdLockMap, i)  && iByteCount < v_iReadLength )
							m_pReadBytes[ iByteCount++ ] = pPageData[ j ];
					}
				}
				else
				{
					if ( j <= (v_iBeginByte + v_iReadLength - 1)% PAGE_LEN_BYTES )
					{
						if ( ReadAllowed(v_fReadBlockData, /*bLockBit,*/ bRdLockMap, i) && iByteCount < v_iReadLength )
							m_pReadBytes[ iByteCount++ ] = pPageData[ j ];
					}
				}

			}
		}
	}
	*v_iDataSize = iByteCount;
	//delete [] bLockMap;
	m_iCurrentError = *v_bResult;
	return m_pReadBytes;
	

}

/////////////////////////////////////////////////////////////////
// Function name...: WriteTag
// Description.....: Writes data to tag
// Parameters......: bool v_fWriteLockedData, int v_iReadLength, BYTE* v_bData, int v_iBeginByte, int* v_iDataSize
// Return..........: none
/////////////////////////////////////////////////////////////////
int CLogic::WriteTag(DWORD v_dwTagID, BYTE* v_bResult, bool v_fWriteLockedData, BYTE* v_bData, int v_iBeginByte, int v_iWriteLength, int* v_iDataSize)
{
	int i;
	DWORD dwResponse;
	BYTE *pPageData;
	//BYTE *pData;
	int iFirstPage, iLastPage;
	//bool *bLockMap = 0, bLockBit;
//	bool bLockBit;
	//CString strLog;
	//strLog = "before HTS_SelectSnrReset";
	//AddMessageToLog(strLog);

	if ( (*v_bResult = m_pHitag->HTS_SelectSnrReset((BYTE)0x41, v_dwTagID, &dwResponse) ) != ERROR_SUCCESS)
	{
		m_iCurrentError = ERROR_TAG_SELECT;
		return ERROR_TAG_SELECT;
	}
	//reads configuration of the tag
	//bLockMap = new bool [ MAX_LOCKED_BLOCKS ];
	/*bool bLockBit =*/ ReadConfig(bWtLockMap);
	/*
	//if tag is already locked and v_fWriteLockedData is true - do nothing
	if ( bLockBit && v_fWriteLockedData )
	{
		*v_iDataSize = 0;
		delete [] bLockMap;
		m_iCurrentError = LOCK_FAIL;
		return LOCK_FAIL;
	}
	*/
	iFirstPage = iLastPage = 0;
	GetPagesToRead(&iFirstPage, &iLastPage, v_iBeginByte, v_iWriteLength);
	//pPageData = 0;
	//pData = 0;
	/////////////pPageData = new BYTE[ PAGE_LEN_BYTES ];

	//pData = new BYTE[ (iLastPage - iFirstPage + 1) * PAGE_LEN_BYTES ];
	//read first page and fill array with its content
	pPageData = ReadTag(v_dwTagID, v_bResult, false, v_iDataSize, iFirstPage * PAGE_LEN_BYTES, PAGE_LEN_BYTES, 1);
	if ( *v_bResult == ( ESERIELL + 256 ) )
	{
		*v_iDataSize = 0;
		m_iCurrentError = ESERIELL;
		return ESERIELL;
	}
	for ( i = 0; i < *v_iDataSize; i++)
		pWtData[ i ] = pPageData[ i ];
	//read last page and fill array with its content
	pPageData = ReadTag(v_dwTagID, v_bResult, false, v_iDataSize, iLastPage * PAGE_LEN_BYTES, PAGE_LEN_BYTES, 1);
	if ( *v_bResult == ( ESERIELL + 256 ))
	{
		*v_iDataSize = 0;		
		m_iCurrentError = ESERIELL;
		return ESERIELL;
	}
	for ( i = 0; i < *v_iDataSize; i++)
		pWtData[ (iLastPage - iFirstPage + 1) * PAGE_LEN_BYTES - PAGE_LEN_BYTES + i ] = pPageData[ i ];
	//fill array with content to be written
	int iBegin = v_iBeginByte - iFirstPage * PAGE_LEN_BYTES;
	for ( i = iBegin; i < iBegin + v_iWriteLength; i++)
		pWtData[ i ] = v_bData[i - iBegin];
	*v_iDataSize = 0;
	for (i = iFirstPage; i <= iLastPage; i++)
	{
		if ( pPageData == NULL )
		{
			*v_iDataSize = 0;		
			m_iCurrentError = ESERIELL;
			return ESERIELL;
		}
		memcpy( pPageData, &pWtData[ (i - iFirstPage)*PAGE_LEN_BYTES ], PAGE_LEN_BYTES );

		if ( m_pHitag->HT1_WritePage(0x00, i, pPageData) == ( ESERIELL + 256 ) )

		
		{
			*v_iDataSize = 0;		
			m_iCurrentError = ESERIELL;
			return ESERIELL;
		}
		*v_iDataSize += PAGE_LEN_BYTES;
	}
	if (v_fWriteLockedData)
		Lock( iFirstPage, iLastPage );
	/////////if (pPageData)
	///////	delete [] pPageData;
	m_iCurrentError = ERROR_SUCCESS;
	return ERROR_SUCCESS;
}

/////////////////////////////////////////////////////////////////
// Function name...: Lock
// Description.....: locked blocks on the tag
// Parameters......: none
// Return..........:
/////////////////////////////////////////////////////////////////
bool CLogic::Lock(int v_iFirstPage, int v_iLastPage)
{
	BYTE pPageData[ PAGE_LEN_BYTES ];


	if ( m_pHitag->HT1_ReadPage( 0x00, 1, pPageData ) == ERROR_SUCCESS)

	
	{	
		for (int i = v_iFirstPage; i <= v_iLastPage; i++)
		{
			if ( i <= 5 && i >= 4 )
			{
				pPageData[2] |= 0x80; 
			}
			else if ( i <= 7 && i >= 6 )
			{
				pPageData[2] |= 0x40;
			}
			else if ( i <= 11 && i >= 8 )
			{
				pPageData[2] |= 0x20;
			}
			else if ( i <= 15 && i >= 12 )
			{
				pPageData[2] |= 0x10;
			}
			else if ( i <= 23 && i >= 16 )
			{
				pPageData[2] |= 0x08;
			}
			else if ( i <= 31 && i >= 24 )
			{
				pPageData[2] |= 0x04;
			}
			else if ( i <= 47 && i >= 32 )
			{
				pPageData[2] |= 0x02; 
			}
			else if ( i <= 63 && i >= 48 )
			{
				pPageData[2] |= 0x01;
			}
		}
		//sets lock bit - remark during test period
		//pPageData[1] |= 0x02;
		//write modified page

		if ( (BYTE)ESERIELL == m_pHitag->HT1_WritePage(0x00, 1, pPageData) )

		
		{
			m_iCurrentError = LOCK_FAIL;
			return false;
		}
		m_iCurrentError = ERROR_SUCCESS;
		return true;
	}
	m_iCurrentError = LOCK_FAIL;
	return false;
	/*
	BYTE pPageData[ PAGE_LEN_BYTES ];
	int i, iFirstBlock, iLastBlock;
	if (HT1_ReadPage( 0x00, 1, pPageData ) == ERROR_SUCCESS)
	{	
		iFirstBlock = (int)(v_iFirstPage / BLOCK_LEN_PAGES);
		iLastBlock = (int)(v_iLastPage / BLOCK_LEN_PAGES);
		//reset RO bits
		pPageData[0] |= 0x3F;// = 00111111 - all bit are set to 1 (r/w)
		for (i = iFirstBlock; i <= iLastBlock; i++)
		{
			if (i >= 2 && i <= 7)
			{
				switch (i)
				{
				case 2:
					pPageData[0] ^= 0x20;
					break;
				case 3:
					pPageData[0] ^= 0x10;
					break;
				case 4:
					pPageData[0] ^= 0x08;
					break;
				case 5:
					pPageData[0] ^= 0x04;
					break;
				case 6:
					pPageData[0] ^= 0x02;
					break;
				case 7:
					pPageData[0] ^= 0x01;
					break;
				}

			}
		}
		//sets lock bit
		pPageData[1] |= 0x10;
		//write modified page
		if ( HT1_WritePage(0x00, 1, pPageData) == ESERIELL )
		{
			m_iCurrentError = LOCK_FAIL;
			return false;
		}
		m_iCurrentError = ERROR_SUCCESS;
		return true;
	}
	m_iCurrentError = LOCK_FAIL;
	return false;
	*/
}

/////////////////////////////////////////////////////////////////
// Function name...: GetPagesToRead
// Description.....: gets the first and the last pages to read from the tag
// Parameters......: int* iFirstPage, int* iLastPage, int v_iBeginByte, int v_iLength
// Return..........: 
/////////////////////////////////////////////////////////////////
void CLogic::GetPagesToRead(int* v_iFirstPage, int* v_iLastPage, int v_iBeginByte, int v_iLength)
{
	*v_iFirstPage = (int)(v_iBeginByte/PAGE_LEN_BYTES);
	*v_iLastPage = (int)((v_iBeginByte + v_iLength - 1) / PAGE_LEN_BYTES);
}

/////////////////////////////////////////////////////////////////
// Function name...: ReadConfig
// Description.....: reads configuration data of the tag
// Parameters......: bool *v_pLockMap
// Return..........: 
/////////////////////////////////////////////////////////////////
bool CLogic::ReadConfig(bool *v_pLockMap)
{
	BYTE pPageData[ PAGE_LEN_BYTES ];

	if (m_pHitag->HT1_ReadPage( 0x00, 1, pPageData ) == ERROR_SUCCESS)

	
	{
		BYTE pTemp;
		for ( int i = 0; i < MAX_LOCKED_BLOCKS; i++ )
		{
			pTemp = pPageData[2];
			pPageData[2] >>= 1;
			pPageData[2] <<= 1;
			v_pLockMap[i] = (bool)(pTemp - pPageData[2]);
			pPageData[2] >>= 1;
		}
	}
	return GetLockBitStatus(pPageData[1]);


}

/////////////////////////////////////////////////////////////////
// Function name...: GetLockBitStatus
// Description.....: gets the status of the lock bit (bit #4, 5th LSB)
// Parameters......: BYTE v_cConfigByte
// Return..........: 
/////////////////////////////////////////////////////////////////
bool CLogic::GetLockBitStatus(BYTE v_cConfigByte)
{
	BYTE pTemp;
	//shift 1 bit right
	v_cConfigByte >>= 1;
	pTemp = v_cConfigByte;
	v_cConfigByte >>= 1;
	v_cConfigByte <<= 1;
	bool bLockBit = (bool)(pTemp - v_cConfigByte);
	return bLockBit;
}

/////////////////////////////////////////////////////////////////
// Function name...: ReadAllowed
// Description.....: checks if cuurent page can be read - if it's locked and need to read locked data or if it's unlocked and need to read unlocked data
// Parameters......: bool v_fReadBlockData, bool* v_bLockMap, int v_iPageNum
// Return..........: 
/////////////////////////////////////////////////////////////////
bool CLogic::ReadAllowed(bool v_fReadBlockData/*, bool v_bLockBit*/, bool* v_bLockMap, int v_iPageNum)
{
	bool bLock = false, bRet = false;

	if ( v_iPageNum <= 5 && v_iPageNum >= 4 )
	{
		if ( v_bLockMap[ 7 ] )
			bLock = true; 
	}
	else if ( v_iPageNum <= 7 && v_iPageNum >= 6 )
	{
		if ( v_bLockMap[ 6 ] )
			bLock = true; 
	}
	else if ( v_iPageNum <= 11 && v_iPageNum >= 8 )
	{
		if ( v_bLockMap[ 5 ] )
			bLock = true; 
	}
	else if ( v_iPageNum <= 15 && v_iPageNum >= 12 )
	{
		if ( v_bLockMap[ 4 ] )
			bLock = true; 
	}
	else if ( v_iPageNum <= 23 && v_iPageNum >= 16 )
	{
		if ( v_bLockMap[ 3 ] )
			bLock = true; 
	}
	else if ( v_iPageNum <= 31 && v_iPageNum >= 24 )
	{
		if ( v_bLockMap[ 2 ] )
			bLock = true; 
	}
	else if ( v_iPageNum <= 47 && v_iPageNum >= 32 )
	{
		if ( v_bLockMap[ 1 ] )
			bLock = true; 
	}
	else if ( v_iPageNum <= 63 && v_iPageNum >= 48 )
	{
		if ( v_bLockMap[ 0 ] )
			bLock = true; 
	}

	if ( v_fReadBlockData )
	{
		if ( bLock )
			bRet = true;
		else
			bRet = false;
	}
	else
	{
		if ( bLock )
			bRet = false;
		else
			bRet = true;		
	}
	return bRet;
	/*
	bool bLock = false, bRet = false;
	int iBlockNum;
	iBlockNum = (int)(v_iPageNum / BLOCK_LEN_PAGES);

	if ( v_bLockBit )
	{
		if ( iBlockNum <= 7 && iBlockNum >= 2)
		{
			if ( v_bLockMap[ iBlockNum - 2 ] )
				bLock = true; 
		}
	}
	if ( v_fReadBlockData )
	{
		if ( bLock )
			bRet = true;
		else
			bRet = false;
	}
	else
	{
		if ( bLock )
			bRet = false;
		else
			bRet = true;		
	}
	return bRet;
	*/
}

/////////////////////////////////////////////////////////////////
// Function name...: GetUID
// Description.....: returns the UID of a tag in a certain position
// Parameters......: int v_iPosition
// Return..........: unsigned __int64 m_uiUID
/////////////////////////////////////////////////////////////////
unsigned __int64 CLogic::GetUID(int v_iPosition)  const
{
	return m_uiTag[v_iPosition];
}

/////////////////////////////////////////////////////////////////
// Function name...: GetTagsCountValue
// Description.....: returns value of m_iTagsCount: number of read tags
// Parameters......: none
// Return..........: int m_iTagsCount
/////////////////////////////////////////////////////////////////
int CLogic::GetTagsCountValue()   const
{
	return m_iTagsCount;
}

/////////////////////////////////////////////////////////////////
// Function name...: GetLastError
// Description.....: Returns current error number
// Parameters......: none
// Return..........: none
/////////////////////////////////////////////////////////////////
int CLogic::GetCurrentError()  const
{
	return m_iCurrentError;
}

