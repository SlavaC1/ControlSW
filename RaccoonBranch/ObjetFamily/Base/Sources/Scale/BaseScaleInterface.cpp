#include "BaseScaleInterface.h"
/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/
	CBaseScales::CBaseScales(int ComNum) 
	{
		  m_ComNum = ComNum;
		  if( m_ComNum>0)
		 	 Init(m_ComNum);
	}
	
	// Destructor
	CBaseScales::~CBaseScales(void)
	{
        DeInit();
	}

	// Implementation functions
	// ------------------------
	// init the communication with the scales
	void CBaseScales::Init(int ComNum)
	{
		m_ComPort.Init(ComNum);
	  	m_ComPort.SetReadTimeout(300);
	}
/*-----------------------------------------------------------------------------*/
	void CBaseScales::DeInit()
	{
		m_ComPort.DeInit();
	}
/*-----------------------------------------------------------------------------*/
	void CBaseScales::Write(char* Cmd,int size)
	{
	   //	m_ComPort.Flush();
		try
		{

		if (m_ComPort.Write(Cmd, size) != (unsigned)size)
		  throw EScales("Communication error");
		}
	 catch (...)
		{
		QMonitor.ErrorMessage("Communication error with the scales");
		}
	}
/*-----------------------------------------------------------------------------*/
	int CBaseScales::Read (char* msg, int size)
	{
	//	m_ComPort.Flush();
	   int numOfBytes =  m_ComPort.Read(msg,size);
	   return numOfBytes;

	}
/*-----------------------------------------------------------------------------*/


