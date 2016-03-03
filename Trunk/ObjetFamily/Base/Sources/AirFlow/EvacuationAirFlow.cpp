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
#include "EvacuationAirFlow.h"
#include "BackEndInterface.h"
#include "QMonitor.h"
#include "QUtils.h"
#include "QThreadUtils.h"
#include "OCBCommDefs.h"
#include "Q2RTErrors.h"
#include "GlobalDefs.h"
#include "QApplication.h"
#include "Q2RTApplication.h"
#include "AppParams.h"
#include "FrontEnd.h"
#include "AppLogFile.h"
#include "MachineManager.h"
#include "MachineSequencer.h"
#include "Actuator.h"

// Functions prototipes for test.
// ------------------------------

// Class CEvacAirFlow Base implementation
// -----------------------------------
// Static members
CLinearInterpolator<RET_TYPE_SPEED> CATSpeed::m_A2DToSpeed;
CLinearInterpolator<int> CATSpeed::m_SpeedToA2D;

void CATSpeed::LoadSpeedTable(const QString& FileName)
{
   m_A2DToSpeed.LoadFromFile(FileName);
   m_SpeedToA2D.LoadInverseFromFile(FileName);
}

int CATSpeed::ConvertSpeedToA2D(RET_TYPE_SPEED value) {
   return (m_SpeedToA2D.Interpolate(value));
}

RET_TYPE_SPEED CATSpeed::ConvertA2DToSpeed(int value){
   return (m_A2DToSpeed.Interpolate(value));
}


// Constructor - create the thread in suspend mode
// ------------------------------------------------------
CEvacAirFlowBase::CEvacAirFlowBase(const QString& Name) : CQComponent(Name),
  m_EvacuationError(false),
  m_AirFlowStatus(AIR_FLOW_OK), m_AirFlowGlidingStatus(AIR_FLOW_OK), m_AirFlowPrevGlidingStatus(AIR_FLOW_OK) //,m_BoundaryCheckSuspended(true)
{
   // Initialize linear interpolation
   m_ParamsMgr  = CAppParams::Instance();
   try
   {
	  if (m_ParamsMgr->EvacuationIndicatorEnabled)
		CATSpeed::LoadSpeedTable(Q2RTApplication->AppFilePath.Value() +
						  LOAD_STRING(IDS_CONFIGS_DIRNAME) + "AirFlow.txt");

   } // Catch file loading related errors
   catch (EQStringList& Err)
   {
	  QMonitor.WarningMessage(Err.GetErrorMsg());
   }

   INIT_METHOD(CEvacAirFlowBase,ConvertA2DToSpeed);
   INIT_METHOD(CEvacAirFlowBase,ConvertSpeedToA2D);
   INIT_METHOD(CEvacAirFlowBase,GetCurrentEvacAirFlowSpeed);
   INIT_METHOD(CEvacAirFlowBase,GetCurrentEvacAirFlowA2D);
   INIT_METHOD(CEvacAirFlowBase,GetCurrentEvacAirFlowStatus);
   INIT_METHOD(CEvacAirFlowBase,GetGlidingEvacAirFlowStatus);

   INIT_METHOD(CEvacAirFlowBase,SetupAirFlowParams);
   INIT_METHOD(CEvacAirFlowBase,GetEvacuationSensorStatusResponse);
   INIT_METHOD(CEvacAirFlowBase,ComputeGlidingFlowStatusRecentSamples);   

   m_OCBClient  = COCBProtocolClient::Instance();
   m_BackEnd    = CBackEndInterface::Instance();

   //Instance to error handler
   m_ErrorHandlerClient = CErrorHandler::Instance();

   m_OCBCurrentAirFlow.AssignFromA2D(255);

   m_HistoryWnd = new CEvacHistoryStatuses(m_ParamsMgr->EvacAirFlowSensitivity.Value());
}
   
// D'tor
CEvacAirFlowBase::~CEvacAirFlowBase() 
{
   Q_SAFE_DELETE(m_HistoryWnd);
}

//  =====================================
//	======= CEvacHistoryStatuses ========

//Init'ing history with HISTORY_MOVING_WND_SIZE times Ok values.
//Counters (m_CounterOk etc.) are set accordingly.
CEvacHistoryStatuses::CEvacHistoryStatuses(unsigned int HistorySize) : CFixedSizeQueue(HistorySize, AIR_FLOW_DONTCARE),
   m_CounterOk(0), m_CounterWarning(0), m_CounterCritical(0), m_CounterEmptySpaces(HistorySize),
   m_LastComputedStatus(AIR_FLOW_OK)
{}

CEvacHistoryStatuses::~CEvacHistoryStatuses() {}

int CEvacHistoryStatuses::UpdateHistory(int Status)
{
	 int poppedStatus = Insert(Status); //update the cyclic array
	 
	 if (poppedStatus != Status) {
		//There's an actual change in the counters.
		//Note: There's no worry we would de/increase counter out-of-bounds,
		//      because m_HistoryWnd is a cyclic array of a fixed size.
		//		i.e. Values will get saturated automatically as long as they're init'd correctly.
		if (AIR_FLOW_LOW_CRITICAL == poppedStatus)
			--m_CounterCritical;
		else if (AIR_FLOW_LOW_CAUTION == poppedStatus)
			--m_CounterWarning;
		else if (AIR_FLOW_OK == poppedStatus)
			--m_CounterOk;
		else if	(0 < m_CounterEmptySpaces) //m_CounterEmptySpaces isn't initialized
			--m_CounterEmptySpaces;        //with 0, so it needs to be saturated.

		if (AIR_FLOW_LOW_CRITICAL == Status)
			++m_CounterCritical;
		else if (AIR_FLOW_LOW_CAUTION == Status)
			++m_CounterWarning;
		else if (AIR_FLOW_OK == Status)
			++m_CounterOk;
		else  //DONT CARE (i.e. clear a spot in the queue)
			++m_CounterEmptySpaces;
	 }
	 
	 int glidingStatus = GetHistoryStatus();

	/* QMonitor.Printf( */
	CQLog::Write(LOG_TAG_AMBIENT,
		QFormatStr("Evac. stats : Ok(%d) Warn(%d) Critical(%d) - Final(%s)",
		m_CounterOk, m_CounterWarning, m_CounterCritical,
		(AIR_FLOW_OK == glidingStatus) ? "OK" : (
			(AIR_FLOW_LOW_CAUTION == glidingStatus) ? "WARN" : "ERR")).c_str()
	);

   m_MutexWndStatus.WaitFor();
	  m_LastComputedStatus = glidingStatus;
   m_MutexWndStatus.Release();
   return glidingStatus;
}

int CEvacHistoryStatuses::GetHistoryLastStatus()
{
   int Status;
   m_MutexWndStatus.WaitFor();
	  Status = m_LastComputedStatus;
   m_MutexWndStatus.Release();
   return Status;
}

int CEvacHistoryStatuses::GetHistoryStatus(unsigned int NumOfSamples)
{
   int calcStatus;
   //If the presence of a level within the window is at least 50%, then a counter has an absolute majority.
   //Also, upon examining the number of warnings in the history window,
   //AIR_FLOW_LOW_CRITICAL level is considered AT LEAST like the AIR_FLOW_LOW_CAUTION level
   if (0 == NumOfSamples)
   {
		if (m_CounterCritical > m_CounterOk + m_CounterWarning) //same as m_CounterCritical > GetSize()-m_CounterCritical
			calcStatus = AIR_FLOW_LOW_CRITICAL;
		else if (m_CounterWarning + m_CounterCritical > m_CounterOk) //same as m_CounterOk < GetSize()-m_CounterOk
			calcStatus = AIR_FLOW_LOW_CAUTION;
		else
			calcStatus = AIR_FLOW_OK;
   }
   else
   {   //Calculate the overall status according to a partial history
	   if (NumOfSamples > (unsigned)GetSize()) NumOfSamples = GetSize();
	   int cloneCounterCritical = 0, cloneCounterOk = 0, cloneCounterWarning = 0;
	   int tmpIdx, datum;
	   m_MutexArrGuard.WaitFor();
		   for (unsigned int i = 0; i < NumOfSamples; ++i) {
				tmpIdx = (m_Head -1 -i);
				if (tmpIdx<0) tmpIdx += 100;
				datum = m_Data[tmpIdx];
				switch (datum)
				{
					case AIR_FLOW_LOW_CRITICAL: ++cloneCounterCritical; break;
					case AIR_FLOW_LOW_CAUTION: 	++cloneCounterWarning; 	break;
					case AIR_FLOW_OK: 			++cloneCounterOk; 		break;
                }
		   }
	   m_MutexArrGuard.Release();

	   if (cloneCounterCritical > cloneCounterOk + cloneCounterWarning)
			calcStatus = AIR_FLOW_LOW_CRITICAL;
	   else if (cloneCounterWarning + cloneCounterCritical > cloneCounterOk)
			calcStatus = AIR_FLOW_LOW_CAUTION;
	   else
			calcStatus = AIR_FLOW_OK;
   }

   return calcStatus;
}

//	======= CEvacHistoryStatuses (END) ==	
//  =====================================	

//void CEvacAirFlowBase::SetBoundaryCheckSuspended(bool Suspended)
//{
//	CQMutexHolder MutexHolder(&m_BoundaryCheckSuspendMutex);
//	m_BoundaryCheckSuspended = Suspended;
//}
//
//bool CEvacAirFlowBase::IsBoundaryCheckSuspended()
//{
//	CQMutexHolder MutexHolder(&m_BoundaryCheckSuspendMutex);
//	return m_BoundaryCheckSuspended;
//}

//-----------------------------------------------------------------------
// Constructor - AirFlow
// ------------------------------------------------------
CEvacAirFlow::CEvacAirFlow(const QString& Name):CEvacAirFlowBase(Name),
	m_FlagGetOCBAirFlowStatusComUnderUse(false)
{
   INIT_METHOD(CEvacAirFlow,Test);
}

// D'tor
CEvacAirFlow::~CEvacAirFlow() {}

// Dummy c'tor
CEvacAirFlowDummy::CEvacAirFlowDummy(const QString& Name):CEvacAirFlowBase(Name) {}

// D'tor
CEvacAirFlowDummy::~CEvacAirFlowDummy() {}

//--------------------------------------------------------------

//---------------------------------------------
int CEvacAirFlowBase::ComputeGlidingFlowStatusRecentSamples(unsigned int NumOfSamples)
{
	//Test a special status here - "Hardware is disconnected".
	//This verbose mode is relevant for a single sample only.
	if (1 == NumOfSamples) {
		if (GetCurrentEvacAirFlowA2D() <= m_ParamsMgr->EvacAirFlowHwFaultLimit.Value())
			return AIR_FLOW_DISCONNECTED;
	}
	
	return m_HistoryWnd->GetHistoryStatus(NumOfSamples);
}

int CEvacAirFlowBase::GetLowLimitSpeed()
{
   int Speed;

   m_MutexDataBase.WaitFor();
      Speed = m_LowLimit.SpeedValue();
   m_MutexDataBase.Release();

   return Speed;
}
int CEvacAirFlowBase::GetLowLimitA2D()
{
   int Speed;

   m_MutexDataBase.WaitFor();
	  Speed = m_LowLimit.A2DValue();
   m_MutexDataBase.Release();

   return Speed;
}
void CEvacAirFlowBase::SetLowLimitA2D(int Speed)
{
   m_MutexDataBase.WaitFor();
	  m_LowLimit.AssignFromA2D(Speed);
   m_MutexDataBase.Release();
}

int CEvacAirFlowBase::GetCriticalLowLimitSpeed()
{
   int Speed;

   m_MutexDataBase.WaitFor();
	  Speed = m_CriticalLowLimit.SpeedValue();
   m_MutexDataBase.Release();

   return Speed;
}
int CEvacAirFlowBase::GetCriticalLowLimitA2D()
{
   int Speed;

   m_MutexDataBase.WaitFor();
	  Speed = m_CriticalLowLimit.A2DValue();
   m_MutexDataBase.Release();

   return Speed;
}
void CEvacAirFlowBase::SetCriticalLowLimitA2D(int Speed)
{
   m_MutexDataBase.WaitFor();
	  m_CriticalLowLimit.AssignFromA2D(Speed);
   m_MutexDataBase.Release();
}

RET_TYPE_SPEED CEvacAirFlowBase::ConvertA2DToSpeed(int value)
{
   return m_OCBCurrentAirFlow.ConvertA2DToSpeed(value);
}

int CEvacAirFlowBase::ConvertSpeedToA2D(RET_TYPE_SPEED value)
{
   return m_OCBCurrentAirFlow.ConvertSpeedToA2D(value);
}

int CEvacAirFlowBase::GetCurrentEvacAirFlowA2D()
{
   int airFlowA2DSpeed;

   m_MutexDataBase.WaitFor();
	  airFlowA2DSpeed = m_OCBCurrentAirFlow.A2DValue();
   m_MutexDataBase.Release();

   return (airFlowA2DSpeed);
}

RET_TYPE_SPEED CEvacAirFlowBase::GetCurrentEvacAirFlowSpeed()
{
   RET_TYPE_SPEED EvacAirFlow;

   m_MutexDataBase.WaitFor();
	  EvacAirFlow = m_OCBCurrentAirFlow.SpeedValue();
   m_MutexDataBase.Release();

   return (EvacAirFlow);
}

int CEvacAirFlowBase::GetCurrentEvacAirFlowStatus()
{
   int airFlowStatus;

   m_MutexDataBase.WaitFor();
	  airFlowStatus = m_AirFlowStatus;
   m_MutexDataBase.Release();

   return airFlowStatus;
}

int CEvacAirFlowBase::GetGlidingEvacAirFlowStatus()
{
   int airFlowStatus;

   m_MutexDataBase.WaitFor();
		airFlowStatus = m_AirFlowGlidingStatus;
   m_MutexDataBase.Release();
   
   return airFlowStatus;
}

//Re-enables any error handling even if the m_EvacuationError flag is on
void CEvacAirFlowBase::ResetErrorState()
{
	m_AirFlowPrevGlidingStatus = AIR_FLOW_OK;
}

//Check if Speed is within limits and update the Status member accordingly
void CEvacAirFlowBase::UpdateCurrentEvacAirFlowStatus()
{
   m_MutexDataBase.WaitFor();

   m_AirFlowStatus = AIR_FLOW_OK;
   m_AirFlowGlidingStatus = AIR_FLOW_OK; //If the bypass is active, fool external
										 //objects to think that the sensor is OK

	if (! m_ParamsMgr->EvacAirFlowErrorBypass)
    {
		 int currentSpeed = GetCurrentEvacAirFlowA2D();

		 //Update the latest single-reading class member (A2D-typed value)
		 if (currentSpeed < m_ParamsMgr->EvacAirFlowLowCriticalLimit)
			 m_AirFlowStatus = AIR_FLOW_LOW_CRITICAL;
		 else if (currentSpeed < m_ParamsMgr->EvacAirFlowLowLimit)
			 m_AirFlowStatus = AIR_FLOW_LOW_CAUTION;
		 
		 //Update the gliding window
		 m_AirFlowGlidingStatus = m_HistoryWnd->UpdateHistory(m_AirFlowStatus);

		 /* Note: The machine's status is checked here at the sensor's sampling rate.
		  * It is wiser to nest this if-clause INSIDE the other if's, because
		  * it's a much heavier operation than a comparison of immidiate in-memory variables.
		  * This GetCurrentState condition is checked HERE for the sake of the
		  * Logging hereafter. */
		 CMachineManager* ManagerPtr = Q2RTApplication->GetMachineManager();
		 if ((ManagerPtr->GetCurrentState() == msPrinting) ||
			 (ManagerPtr->GetCurrentState() == msPrePrint))
		 {
			 CQLog::Write(LOG_TAG_AMBIENT,
				"Evac. sample : %d (%d)", currentSpeed, m_AirFlowStatus);
			 
			 //Inspect the gliding status and determine if an error is in order
			 if (AIR_FLOW_LOW_CRITICAL == m_AirFlowGlidingStatus)
			 {
			   //check if this is the first time we encounter a problem, or if it got worse
			   if ((!m_EvacuationError) || (AIR_FLOW_LOW_CRITICAL < m_AirFlowPrevGlidingStatus))
			   {
					m_EvacuationError = true;
					m_ErrorHandlerClient->ReportError("Evacuation air flow speed is under the critical limit (too slow).\nPrinting will stop.",
						Q2RT_EVAC_AIRFLOW_SPEED_CRITICAL,GetCurrentEvacAirFlowA2D());
			   }
			 }
			 else
			 {
			   if (AIR_FLOW_LOW_CAUTION == m_AirFlowGlidingStatus)
			   {
				 if ((!m_EvacuationError) || (AIR_FLOW_LOW_CAUTION < m_AirFlowPrevGlidingStatus))
				 {
					   m_EvacuationError = true;
					   m_ErrorHandlerClient->ReportError("Evacuation air flow speed is becoming too slow.\nPlease check site conditions and damper.",
							Q2RT_EVAC_AIRFLOW_SPEED_WARNING,GetCurrentEvacAirFlowA2D());
				 }
			   }
			 }
		 }

	}

	if (AIR_FLOW_OK == m_AirFlowGlidingStatus)
		m_EvacuationError = false;

   m_AirFlowPrevGlidingStatus = m_AirFlowGlidingStatus;

   m_MutexDataBase.Release();
}
void CEvacAirFlowBase::UpdateOCBCurrentSpeedA2D(int AirFlow)
{
   m_MutexDataBase.WaitFor();

   m_OCBCurrentAirFlow.AssignFromA2D(AirFlow);

   m_MutexDataBase.Release();
}

TQErrCode CEvacAirFlowBase::SetupAirFlowParams()
{
	SetLowLimitA2D(m_ParamsMgr->EvacAirFlowLowLimit.Value());
	SetCriticalLowLimitA2D (m_ParamsMgr->EvacAirFlowLowCriticalLimit.Value());

    return Q_NO_ERROR;
}

// Command to get AirFlow tempearture sensor status
TQErrCode CEvacAirFlow::GetEvacuationSensorStatusResponse()
{
   //Verify if we are already not performing this command
   if (m_FlagGetOCBAirFlowStatusComUnderUse)
   {
		CQLog::Write(LOG_TAG_GENERAL,"AirFlow \"GetEvacAirFlowStatus\" re-entry problem");
		return Q_NO_ERROR;
   }

   m_FlagGetOCBAirFlowStatusComUnderUse=true;

   TOCBGetAirFlowSensorStatusMessage GetMsg;

   GetMsg.MessageID = static_cast<BYTE>(OCB_GET_EVACUATION_AIRFLOW_STATUS);

   // Send a Turn ON request
   if (m_OCBClient->SendInstallWaitReply(&GetMsg,
							sizeof(TOCBGetAirFlowSensorStatusMessage),
							GetOCBAirFlowStatusAckResponse,
							reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
   {
	  CQLog::Write(LOG_TAG_GENERAL,"OCB didn't get ack for \"GetEvacAirFlowStatus\" message");
   }

   m_FlagGetOCBAirFlowStatusComUnderUse = false;

   UpdateCurrentEvacAirFlowStatus();

   FrontEndInterface->UpdateStatus(FE_CURRENT_EVAC_AIRFLOW_SPEED, GetCurrentEvacAirFlowSpeed());
   FrontEndInterface->UpdateStatus(FE_CURRENT_EVAC_AIRFLOW_IN_A2D, GetCurrentEvacAirFlowA2D());
	//Sends a notification about the status being changed, so that FE will know the value it has is valid/invalid
   FrontEndInterface->UpdateStatus(FE_CURRENT_EVAC_AIRFLOW_STATUS, GetCurrentEvacAirFlowStatus());

   return Q_NO_ERROR;
}

//GetAirFlowStatus ack response
void CEvacAirFlow::GetOCBAirFlowStatusAckResponse(int TransactionId,PVOID Data,
                                 unsigned DataLength,TGenericCockie Cockie)
{
   TOCBEvacAirFlowStatusResponse *StatusMsg =
      static_cast<TOCBEvacAirFlowStatusResponse *>(Data);

   // Get a pointer to the instance
   CEvacAirFlow *InstancePtr = reinterpret_cast<CEvacAirFlow *>(Cockie);

   //Verify the size of message
   if(DataLength != sizeof(TOCBEvacAirFlowStatusResponse))
      {
      FrontEndInterface->NotificationMessage("AirFlow \"GetOCBAirFlowStatusAckResponse\" length error");
      CQLog::Write(LOG_TAG_GENERAL,"AirFlow \"GetOCBAirFlowStatusAckResponse\" length error");
      return;
	  }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(StatusMsg->MessageID) != OCB_EVAC_AIRFLOW_STATUS)
      {
      FrontEndInterface->NotificationMessage("AirFlow \"GetOCBAirFlowStatusAckResponse\" message id error");
      CQLog::Write(LOG_TAG_GENERAL,"AirFlow \"GetOCBAirFlowStatusAckResponse\" message id error (0x%X)",
                                       (int)StatusMsg->MessageID);
      return;
      }

   int AirFlowValueA2D = static_cast<int>(StatusMsg->AirFlowValue);
   InstancePtr->UpdateOCBCurrentSpeedA2D(AirFlowValueA2D);
    //The Airflow's Status member is refreshed during the following request 
}


// Dummy procedures for CEvacAirFlow 
//-----------------------------------
//--------------------------------------------------------------
// Command to get AirFlow tempearture sensor status
TQErrCode CEvacAirFlowDummy::GetEvacuationSensorStatusResponse()
{
   UpdateOCBCurrentSpeedA2D(m_ParamsMgr->EvacAirFlowLowLimit.Value()+15);
   UpdateCurrentEvacAirFlowStatus();

   FrontEndInterface->UpdateStatus(FE_CURRENT_EVAC_AIRFLOW_STATUS,GetCurrentEvacAirFlowStatus());

   FrontEndInterface->UpdateStatus(FE_CURRENT_EVAC_AIRFLOW_IN_A2D, GetCurrentEvacAirFlowA2D());
   FrontEndInterface->UpdateStatus(FE_CURRENT_EVAC_AIRFLOW_SPEED,GetCurrentEvacAirFlowSpeed());

   return Q_NO_ERROR;
}

//bool CEvacAirFlowDummy::IsBoundaryCheckSuspended() { return true; }

//------------------------------
// Test procedure
//------------------------------

TQErrCode CEvacAirFlowBase::Test()
{
  return Q_NO_ERROR;
}


