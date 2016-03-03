//---------------------------------------------------------------------------

#ifndef FansH
#define FansH
//---------------------------------------------------------------------------

#include <assert.h>
#include "OHDBProtocolClient.h"
#include "OHDBCommDefs.h"
#include "QMessageQueue.h"
#include "ErrorHandler.h"
#include "FrontEnd.h"
#include "AppLogFile.h"



class CFansBase : public CQComponent
{
	public:
		CFansBase(const QString& Name);
		~CFansBase(){};

		virtual void ResetMiddleFanWarning() = 0;

	protected:
		int  m_MainFanCriticalSpeedRetriesCounter;
		int  m_MainFanWarningSpeedRetriesCounter;
		int  m_FansData[NUM_OF_FANS];
		COHDBProtocolClient *m_OHDBClient;
		CErrorHandler 		*m_ErrorHandlerClient;
		CAppParams    		*m_ParamsMgr;
};


class CFans : public CFansBase
{
	public:
		CFans(const QString& Name);
		~CFans(){};
		TQErrCode IsError();

		void ResetMiddleFanWarning();

	private:
		void DataToRpm(TFansDataMessage *Msg);
		bool IsFansOn(WORD mask, WORD FansState);
		bool IsMaterialFansDutyCycle100(WORD DutyCycle);
		void UpdateRightFans(TFansDataMessage *Msg);
		void UpdateLeftFans(TFansDataMessage *Msg);
		void UpdateMiddleFan(TFansDataMessage *Msg);
		static void FansReceiveDataMsgHandler(int TransactionId, PVOID Data, unsigned DataLength,TGenericCockie Cockie);
};


class CFansDummy : public CFansBase
{
	public:
		CFansDummy();
		~CFansDummy(){};
};

#endif
