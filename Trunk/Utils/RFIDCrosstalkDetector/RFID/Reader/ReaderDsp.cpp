//ReaderDsp.cpp
//

#include "ReaderDsp.h"


/////////////////////////////////////////////////////////////////
// Function name...: CReaderDsp()
// Description.....: Constructor
// Parameters......: none
// Return..........: none
/////////////////////////////////////////////////////////////////
CReaderDsp::CReaderDsp()
{
	SetInitParam();
}

/////////////////////////////////////////////////////////////////
// Function name...: ~CReaderDsp()
// Description.....: Destructor
// Parameters......: none
// Return..........: none
/////////////////////////////////////////////////////////////////
CReaderDsp::~CReaderDsp()
{
	if (m_pLogic)
	{
		delete m_pLogic;    
		m_pLogic = 0;
	}
}

/////////////////////////////////////////////////////////////////
// Function name...: SetInitParam()
// Description.....: Intializes class parameters
// Parameters......: none
// Return..........: none
/////////////////////////////////////////////////////////////////
int CReaderDsp::SetInitParam()
{
	m_pLogic = 0;
	m_pLogic = new CLogic;
	m_iChannel = 0;
	return 1;
}

/////////////////////////////////////////////////////////////////
// Function name...: Connect
// Description.....: Connects to a reader using a serial connection
// Parameters......: int iComPort, int iBaudRate, int iParity, int iDataBits, int iStopBit
// Return..........: 1 - connect, -1 - unable to connect (serial problem), -2 - unable to connect (reader problem), 0 - unspecified problem
/////////////////////////////////////////////////////////////////
int CReaderDsp::Connect(int iComPort, int iBaudRate, int iParity, int iDataBits, int iStopBit)
{
	if (!m_pLogic)
	{
		m_pLogic = new CLogic;
	}
	return m_pLogic->Connect(iComPort, iBaudRate, iParity, iDataBits, iStopBit);
}

/////////////////////////////////////////////////////////////////
// Function name...: Disconnect
// Description.....: Disconnects from a reader
// Parameters......: none
// Return..........: 1 - disconnect, 0 - else
/////////////////////////////////////////////////////////////////
int CReaderDsp::Disconnect()
{
	return m_pLogic->Disconnect();
}

/////////////////////////////////////////////////////////////////
// Function name...: IsConnect
// Description.....: Checks if reader is connected or not
// Parameters......: none
// Return..........: true - connected, false - else
/////////////////////////////////////////////////////////////////
bool CReaderDsp::IsConnect()
{
	return m_pLogic->GetSWVersion();
}

/////////////////////////////////////////////////////////////////
// Function name...: SetChannel
// Description.....: sets active channel
// Parameters......: int v_iChannel
// Return..........: true - success, false - else
/////////////////////////////////////////////////////////////////
int CReaderDsp::SetChannel(int v_iChannel)
{
	m_iChannel = v_iChannel - 1;
	return m_pLogic->SetChannel(v_iChannel);
}


/////////////////////////////////////////////////////////////////
// Function name...: CheckTags
// Description.....: Checks number of tags currently in the reader
// Parameters......: none
// Return..........: number of tags
/////////////////////////////////////////////////////////////////
int CReaderDsp::CheckTags()
{
	if (!m_pLogic)
		return 0;

	m_pLogic->DetectTags();
	CreateTags();

	return m_pLogic->GetTagsCountValue();
}

/////////////////////////////////////////////////////////////////
// Function name...: CreateTags
// Description.....: Creates an array with all availabe tags
// Parameters......: none
// Return..........: none
/////////////////////////////////////////////////////////////////
int CReaderDsp::CreateTags()
{
	int i;
	if (m_pLogic->GetTagsCountValue()==0)
		return 0;
	//m_pTags[m_iChannel] = new CTag[m_pLogic->GetTagsCountValue()];
	for (i=0;i<m_pLogic->GetTagsCountValue();i++)
	{
		m_pTags[m_iChannel][i].SetUID(m_pLogic->GetUID(i));
		m_pTags[m_iChannel][i].SetLogicPtr(m_pLogic);
	}
	return 1;
}

/////////////////////////////////////////////////////////////////
// Function name...: GetTags
// Description.....: Gets an array of Tags - currently available tags
// Parameters......: none
// Return..........: CTag*
/////////////////////////////////////////////////////////////////
CTag* CReaderDsp::GetTags()
{
	return m_pTags[m_iChannel];
}

/////////////////////////////////////////////////////////////////
// Function name...: GetLastError
// Description.....: Returns current error number
// Parameters......: none
// Return..........: none
/////////////////////////////////////////////////////////////////
int CReaderDsp::GetCurrentError()
{
	return m_pLogic->GetCurrentError();
}

/////////////////////////////////////////////////////////////////
// Function name...: GetDLLVersion
// Description.....: Returns DLL_SW_VERSION
// Parameters......: none
// Return..........: none
/////////////////////////////////////////////////////////////////
/*int CReaderDsp::GetDLLVersion()
{
	//char* bpVersion = new char[10];
	string strVersion;
	CDLLVersion version((LPSTR)"Reader.dll");
	strVersion = version.GetFullVersion();
	int iVer = 0;
	int iPos1 = 0, iPos2 = strVersion.find(".", iPos1);
	int iSegment = atoi(strVersion.substr(iPos1, iPos2).c_str());
	iSegment *= 1000000;
	iVer += iSegment;
	iPos1 = iPos2+1;

	iPos2 = strVersion.find(".", iPos1);
	iSegment = atoi(strVersion.substr(iPos1, iPos2).c_str());
	iSegment *= 10000;
	iVer += iSegment;
	iPos1 = iPos2+1;

	iPos2 = strVersion.find(".", iPos1);
	iSegment = atoi(strVersion.substr(iPos1, iPos2).c_str());
	iSegment *= 100;
	iVer += iSegment;
	iPos1 = iPos2+1;

	iPos2 = strVersion.find(".", iPos1);
	iSegment = atoi(strVersion.substr(iPos1, iPos2).c_str());
	iVer += iSegment;
	iPos1 = iPos2+1;

	//strcpy( (char*)v_pVersion , strVersion.c_str() );
	return iVer;
}*/




