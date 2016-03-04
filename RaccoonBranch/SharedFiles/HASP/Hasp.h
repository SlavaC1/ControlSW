/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT.                                                   *
 * Module: HASP Interface                                           *
 * Module Description: Aladdin HASP application protection.         *
 *                                                                  *
 * Compilation: Standard C++ , BCB                                  *
 *                                                                  *
 * Author: Slava Chuhovich                                          *
 * Start date: 22/02/2010                                           *
 * Last upate:                                                      *
 ********************************************************************/

#ifndef HaspH
#define HaspH
//---------------------------------------------------------------------------
#include "hasp_api_cpp_.h"
#include "HaspDefs.h"
#include "QThread.h"
#include "QMutex.h"
#include "QUtils.h"
#include "QLogFile.h"
#include "QTypes.h"

class EHaspInterfaceException : public EQException 
{
	public:
		EHaspInterfaceException(const QString& ErrMsg = "", const TQErrCode ErrCode = 0);		
};

typedef void (*THandleHaspStatusCallback)();

class CHaspInterfaceWrapper;

class CHaspInterface : public CQThread
{
	friend CHaspInterfaceWrapper;
	
	private:        
		// Constructor		
		CHaspInterface(ChaspFeature HaspFeature, TLogFileTag HaspLogTag, THandleHaspStatusCallback HandleHaspStatusCallback = NULL);		      
		
		Chasp *m_Hasp;
		int    m_HaspFeatureNum;
		
		CQMutex m_MutexHaspConnectionStatus;
		CQMutex m_MutexCheckPlugValidity;
		
		THaspConnectionStatus m_HaspStatus;
		THaspConnectionStatus m_HaspPrevStatus;		
		
		THaspPlugInfo m_PlugInfo;
		TLogFileTag   m_HaspLogTag;
		int           m_DaysToExpiration;
		
		THandleHaspStatusCallback m_HandleHaspStatusCallback;		

        THaspDate ConvertStringToDate(AnsiString HaspTime);
		THaspDate GetCurrentDate();
		void      SetDaysToExpiration();
		void      SetPlugInfo();
		void      WriteToLog(QString string, bool WriteToEncrypted = false);
		void      CheckPlugValidity(bool CheckDirectly = false);
		void      LogIn(bool CheckDirectly = false);
		void      LogOut();
		void 	  SetConnectionStatus(THaspConnectionStatus status);
		
		THaspConnectionStatus GetSessionStatus();

		bool m_WriteToLog;		

	public:
		
		virtual ~CHaspInterface();		
		
		// Thread function (override)
		virtual void Execute();
		virtual void CheckPlugValidityDirectly();
		virtual int  GetDaysToExpiration();
		virtual THaspConnectionStatus GetConnectionStatus();
		virtual THaspPlugInfo GetPlugInfo();
		virtual QString GetAPIVersion();
		virtual int CalculateTimeToExpiration();

		void SetWriteToLog(bool WriteToLog);

	protected:
	
		CHaspInterface();
};

class CHaspInterfaceDummy : public CHaspInterface
{
	friend CHaspInterfaceWrapper;
	
	private:
		
		CHaspInterfaceDummy(){};
		
	public:		
		
		virtual ~CHaspInterfaceDummy(){};
		void Execute(){};			
		void CheckPlugValidityDirectly(){};		
		int  GetDaysToExpiration();
		THaspConnectionStatus GetConnectionStatus();
		THaspPlugInfo GetPlugInfo();
		QString GetAPIVersion();
		int CalculateTimeToExpiration();
};

class CHaspInterfaceWrapper
{
	private:
		
		CHaspInterfaceWrapper(ChaspFeature HaspFeature, TLogFileTag HaspLogTag, THandleHaspStatusCallback HandleHaspStatusCallback = NULL);
		CHaspInterfaceWrapper();
		
		static CHaspInterfaceWrapper *m_SingletonInstance;
		CHaspInterface *m_HaspInterfaceInstance;		
		
	public:
		
		~CHaspInterfaceWrapper();
		static CHaspInterface *Instance();
		
		static void Init(ChaspFeature HaspFeature, TLogFileTag HaspLogTag, THandleHaspStatusCallback HandleHaspStatusCallback = NULL);
		static void Init();
		static void DeInit();
};

#endif
