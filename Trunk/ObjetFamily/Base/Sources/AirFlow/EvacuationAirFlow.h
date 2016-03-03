/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Evaluation Indicator                                    *
 * Module: Evaluation Sensor ON 8051 h/w                            *
 * Module Description: This class implement services related to the *
 *                     Evacuation (airflow) sensor.                 *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Elad Atar                                                *
 * Start date: 06/11/2012                                           *
 ********************************************************************/
 
#ifndef _AirFlow_H_
#define _AirFlow_H_

#include "ErrorHandler.h"
#include "LinearInterpolator.h"
#include "FixedSizeQueue.h"
#include "OCBCommDefs.h"
#include "OCBProtocolClient.h"
#include "OCBCommDefs.h"
#include "OCBProtocolClient.h"
#include "Q2RTErrors.h"
#include "QMessageQueue.h"
#include "QTimer.h"
#include "Speed.h"

//The constants here are also used to determine colors. Divided by <0,0,>0
enum {
	AIR_FLOW_DISCONNECTED = -2,
	AIR_FLOW_LOW_CRITICAL = -1,
	AIR_FLOW_LOW_CAUTION,
	AIR_FLOW_OK,
	AIR_FLOW_DONTCARE
};

class CBackEndInterface;

// Exception class for all the QLib RTTI elements
class EAirFlow : public EQException {
  public:
    EAirFlow (const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

class CATSpeed : public CProportionalSpeed {
protected:
   //The following two lines should be declared in the inherit CSpeed.
   static CLinearInterpolator<RET_TYPE_SPEED> m_A2DToSpeed;
   static CLinearInterpolator<int> m_SpeedToA2D;
public:
   static void LoadSpeedTable(const QString& FileName);
   int ConvertSpeedToA2D(RET_TYPE_SPEED value);
   RET_TYPE_SPEED ConvertA2DToSpeed(int value);
};

class CEvacHistoryStatuses : protected CFixedSizeQueue {
  private:
    unsigned int m_CounterOk, m_CounterWarning, m_CounterCritical, m_CounterEmptySpaces;
	int m_LastComputedStatus;
	
  protected:
    CQMutex m_MutexWndStatus;
	
  public:
	//Inserts a new data to the moving window and returns 
	//the gliding status considering the entire window (same as GetHistoryStatus)
	int UpdateHistory(int Status);
	//This is the last computed "gliding" status
	int GetHistoryLastStatus();
	//Computes the status based on NumOfSamples (0 = all history size)
	int GetHistoryStatus(unsigned int NumOfSamples=0);
	
	CEvacHistoryStatuses(unsigned int HistorySize);
	~CEvacHistoryStatuses();
};

class CEvacAirFlowBase : public CQComponent {
  protected:

   CATSpeed m_OCBCurrentAirFlow;
   CATSpeed m_LowLimit;
   CATSpeed m_CriticalLowLimit;

   //m_AirFlowStatus is the status of the last sensor reading
   int m_AirFlowStatus;
   //m_AirFlowPrevGlidingStatus helps in keeping track if the error level changes to the worse
   int m_AirFlowGlidingStatus, m_AirFlowPrevGlidingStatus;
   
   COCBProtocolClient *m_OCBClient;
   CEvacHistoryStatuses* m_HistoryWnd;

   // Protection mutex for database
   CQMutex m_MutexDataBase;

   //CQMutex m_BoundaryCheckSuspendMutex;
   //bool    m_BoundaryCheckSuspended;

   CErrorHandler*     m_ErrorHandlerClient;
   CAppParams*        m_ParamsMgr;
   CBackEndInterface* m_BackEnd;

   void UpdateOCBCurrentSpeedA2D(int);
   void SetCriticalLowLimitA2D	(int);
   void SetLowLimitA2D			(int);

   //makes sure that we issue an error whenever the air flow isn't OK anymore
   bool m_EvacuationError;

  private:
	void UpdateGlidingWindowCounters(int newStatus);

  public:
   // Constructor
    CEvacAirFlowBase(const QString& Name);

    // Destructor. 
	// We don't add the Virtual modifier, because the mem (de)alloc (e.g. CEvacHistoryStatuses)
	// is done by this base class, and not by the polymorphism dervied classes.
	~CEvacAirFlowBase();

	int GetLowLimitA2D();
	int GetLowLimitSpeed();
	int GetCriticalLowLimitA2D();
	int GetCriticalLowLimitSpeed();

	void ResetErrorState();  //TODO (Elad): refactor - delete it ?
	//void SetBoundaryCheckSuspended(bool Suspended);
	//virtual bool IsBoundaryCheckSuspended();

    // Converting units procedures
	DEFINE_METHOD_1(CEvacAirFlowBase,RET_TYPE_SPEED,ConvertA2DToSpeed,int);
    DEFINE_METHOD_1(CEvacAirFlowBase,int,ConvertSpeedToA2D,RET_TYPE_SPEED);
	
	//This procedure gets the value of Current AirFlow value in A/D units
	DEFINE_METHOD(CEvacAirFlowBase,int,GetCurrentEvacAirFlowA2D);
	//This procedure gets the value of Current airflow's speed [m/sec]
	DEFINE_METHOD(CEvacAirFlowBase,RET_TYPE_SPEED,GetCurrentEvacAirFlowSpeed);
	//This procedure gets the value of airflow status based only upon last received datum
	DEFINE_METHOD(CEvacAirFlowBase,int,GetCurrentEvacAirFlowStatus);
	//This procedure computes the gliding value of the airflow status over time (history-based)
	DEFINE_METHOD(CEvacAirFlowBase,int,GetGlidingEvacAirFlowStatus);
	//This procedure is used for testing / BIT
	DEFINE_METHOD_1(CEvacAirFlowBase,int,ComputeGlidingFlowStatusRecentSamples,unsigned int);

	//This procedure checks if the speed is within range
	void UpdateCurrentEvacAirFlowStatus();
	
	DEFINE_METHOD(CEvacAirFlowBase,TQErrCode,SetupAirFlowParams);
	// Command to get Evacuation AirFlow sensor's status
	DEFINE_V_METHOD(CEvacAirFlowBase,TQErrCode,GetEvacuationSensorStatusResponse)=0;

	//Test procedure
	DEFINE_METHOD(CEvacAirFlowBase,TQErrCode,Test);
};


class CEvacAirFlow : public CEvacAirFlowBase {

 private:
   //Flag to detect re-execution of a command
   bool m_FlagGetOCBAirFlowStatusComUnderUse;
        
   //GetAirFlowStatus ack response
   static void GetOCBAirFlowStatusAckResponse(int TransactionId,PVOID Data,
								 unsigned DataLength,TGenericCockie Cockie);

public:
	// Constructor
	CEvacAirFlow(const QString& Name);

	// Destructor
	~CEvacAirFlow();

	// Command to get AirFlow tempearture sensor status
	TQErrCode GetEvacuationSensorStatusResponse();

};


class CEvacAirFlowDummy : public CEvacAirFlowBase {
public:
   // Constructor
   CEvacAirFlowDummy(const QString& Name);

   // Destructor
   ~CEvacAirFlowDummy();
   
   // Command to get AirFlow tempearture sensor status
   TQErrCode GetEvacuationSensorStatusResponse();

   //bool IsBoundaryCheckSuspended();
};

#endif

