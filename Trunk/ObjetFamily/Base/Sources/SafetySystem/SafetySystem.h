/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Safety System                                            *
 * Module Description: This class implement services related to the *
 *                     Safety System                                *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Guy Ezra                                                 *
 * Start date: 2/11/2012                                            *
 ********************************************************************/


#ifndef _SAFETY_SYSTEM_H_
#define _SAFETY_SYSTEM_H_

#include "OCBProtocolClient.h"
#include "ErrorHandler.h"




enum {UN_PRESSED  = 0,
	  PRESSED = 1};

enum{  SERVICE_KEY_WAS_NOT_INSERTED = 0,
	SERVICE_KEY_INSERTED = 1
};


// Exception class for all the QLib RTTI elements
class ESafetySystem : public EQException {
  public:
	ESafetySystem(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};



class CSafetySystemBase : public CQComponent
{

	 protected:
		//Represent status array
		BYTE m_EmergencyStopButtonsStatus[MAX_NUM_OF_EMERGENCY_STOP_BUTTONS];
		BYTE m_ServiceKeyStatus;  // represent the service key status
		bool m_FlagGetStatusUnderUse;

		COCBProtocolClient *m_OCBClient;
		CErrorHandler *m_ErrorHandlerClient;
		 // Protection mutex for database
		CQMutex m_MutexDataBase;
		CQMutex m_Maintenance;

		bool m_bNeedToUpdate;
		bool GetUpdateState();
	public:

		CSafetySystemBase(const QString& Name);
		virtual ~CSafetySystemBase();

		TQErrCode GetStatus(void);//EM send to OCB- Get Safety System Status

		// Get OCB repond
		static void GetStatusAckResponse(int TransactionId,PVOID Data,	unsigned DataLength,TGenericCockie Cockie);
		//Update Ocb Status
	    void UpdateSafetySystemStatus(BYTE emergencyStopRear, BYTE emergencyStopFront, BYTE serviceKey);

		DEFINE_V_METHOD_1(CSafetySystemBase,bool,IsEmegencyStopButtonUnpressed,BYTE);
		//bool IsEmegencyStopButtonUnpressed (BYTE buttonIndex);
		bool IsServiceKeyInserted();
		//Set window visible/invisible
		void UpdatePopUpMessage();

};

class CSafetySystemDummy : public CSafetySystemBase
{
public:

	CSafetySystemDummy(const QString& Name);
	~CSafetySystemDummy();
	TQErrCode GetStatus(void);
	bool IsAllEmegencyStopButtonsUnpressed (void);
	bool IsEmegencyStopButtonUnpressed (BYTE buttonIndex);
};




#endif