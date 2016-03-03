
/********************************************************************
 *                  Objet Geometries LTD.                           *
 *                  ---------------------                           *
 * Project: Q2RT                                                    *
 * Module: EOL control                                              *
 * Module Description: This class implement services related to the *
 *                EOL                                               *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger                                          *
 * Start date: 16/12/2001                                           *
 ********************************************************************/

#include "Container.h"
#include "QMonitor.h"
#include "QUtils.h"
#include "QThreadUtils.h"
#include "OCBCommDefs.h"
#include "Q2RTErrors.h"
#include "QApplication.h"
#include "Q2RTApplication.h"
#include "MachineSequencer.h"
#include "AppLogFile.h"
#include "BackEndInterface.h"
#include "FrontEnd.h"
#include "FrontEndControlIDs.h"
#include "MaintenanceCounters.h"
#include "HeadFilling.h"
#include "rfid.h"
#include "ModesManager.h"
#include "ContainerTag.h"
#include "MaintenanceCountersDefs.h"
#include "ModesDefs.h"
#include "AppParams.h"
#include "FrontEndParams.h" 
#include "SignalTower.h"

#if 0
char * debugContainerStateStr[] =
{
/* BEGIN_NO_TANK              */                                           "BEGIN_NO_TANK",
/* RF_WAITING                 */                                           "RF_WAITING",
/* TANK_ABORT_PENDING         */                                           "TANK_ABORT_PENDING",
/* MS_NOISY_NO_TAG            */                                           "MS_NOISY_NO_TAG",
/* IDENTIFIED_STABLE          */                                           "IDENTIFIED_STABLE",
/* NO_TAG_STABLE              */                                           "NO_TAG_STABLE",
/* TAG_REMOVED_PENDING = LAST_STATE_OF_TANK_IDENTIFICATION_STATEMACHINE */ "TAG_REMOVED_PENDING (last ID state)",
/* DATA_READ_NOT_ACTIVE       */                                           "DATA_READ_NOT_ACTIVE",
/* CHECK_RESIN_TYPE           */                                           "CHECK_RESIN_TYPE",
/* CHECK_PUMP_TIME            */                                           "CHECK_PUMP_TIME",
/* CHECK_EXPIRATION_DATE      */                                           "CHECK_EXPIRATION_DATE",
/* CHECK_TAG_ERRORS           */                                           "CHECK_TAG_ERRORS",
/* READ_BATCH_NUMBER          */                                           "READ_BATCH_NUMBER",
/* CHECK_WEIGHT_STABILIZATION */                                           "CHECK_WEIGHT_STABILIZATION",
/* CHECK_CONTAINER_REFILL     */                                           "CHECK_CONTAINER_REFILL",
/* DISABLED_TANK              */                                           "DISABLED_TANK",
/* ENABLED_TANK               */                                           "ENABLED_TANK",
/* LAST_STATE_OF_TANK_DATA_READ_STATEMACHINE = IDLE*/                      "IDLE"
};
#endif

int GetDrainPumpCounterID(TTankIndex Tank);
const int WASTE_IS_EMPTY                         = -250;

const int WEIGHT_ERROR_WARNNING_DELTA            = 100; //gr
const int WASTE_WEIGHT_ERROR_WARNNING_DELTA      = 500; //gr
const int WEIGHT_QUANTIZATION_UNITS              = 20;
const int ACTIVE_PUMPS_PERIODIC_CHECK_ITERATIONS = 10;


//OBJET_MACHINE
const int STATE_MACHINE_TIMER_MSG_INTERVAL       = 200; // ms
//skip timer msg sending if m_SMContainerIDEventQueue is more than half full
const int SM_EVENT_QUEUE_MAX_MESSAGES            = 60;

const int TIMEOUT_FOR_CONTAINERS_REMOVAL         = 3000;
const int QUEUE_SEND_TIMEOUT                     = 50;// /*Seconds*/ * 1000;
const int MIN_TIME_FOR_TAG_PUMP_UPDATE           = 10; // seconds
//const int PUMP_SECONDS_PER_GRAM                  = 5;
const int WAIT_BEFORE_GET_EXPIRATIONDATE         = 5;
const int WAIT_BEFORE_GET_PUMPINGTIME            = 7;

//OBJET_MACHINE //refill req.
const int MINIMAL_MATERIAL_CONSUMPTION           = 100; //amount of material consumption indicating a valid operation, in gr.
const unsigned int WAIT_FOR_STABLE_WEIGHT                 = 9000; //in ms
const unsigned int WAIT_FOR_STABLE_WEIGHT_OBJET1000       = 13000;  // in ms
const int ALLOW_WEIGHT_TO_STABLIZE               = 6000; //in ms 
#ifndef OBJET_1000
const int WEAK_STRONG_PUMP_RATIO		 = 10; //the ratio between weakest pump and strongest pump per sec
const int MAX_FLUCTUATION_WEIGHT                 = 20; //for stable weight (grams)
#else
const int WEAK_STRONG_PUMP_RATIO       		 = 5*4; //the ratio between weakest pump and strongest pump per sec
const int MAX_FLUCTUATION_WEIGHT                 = 80; //for stable weight (grams)
#endif
const int MAX_ITERATIONS_FOR_STABLE_WEIGHT       = 3;
const int MAX_DEVIATION_WEIGHT                   = 520; //between different load cells (grams)
const int NUM_OF_WEIGHT_SAMPLES 	      	 = 60;
const int WEIGHT_READINGS_SAMPLES_PERIOD 	 = 10000; //in ms
const int NUM_OF_CONSUMPTION_READING_RETRIES     = 10;
const int MAX_INITIAL_WEIGHT		      	 = (1024*64)-1;
const int INITIAL_GAP				 = -10000;



// Associating Container microswitches and RF antenna-channel.
const int SUPPOR_CONTAINER1_RF_CHANNEL = RFRDR_CHANNEL1;
const int SUPPOR_CONTAINER2_RF_CHANNEL = RFRDR_CHANNEL2;
const int MODEL_CONTAINER1_RF_CHANNEL  = RFRDR_CHANNEL3;
const int MODEL_CONTAINER2_RF_CHANNEL  = RFRDR_CHANNEL4;
const int MODEL_CONTAINER3_RF_CHANNEL  = RFRDR_CHANNEL1;
const int MODEL_CONTAINER4_RF_CHANNEL  = RFRDR_CHANNEL2;

const int MODEL_CONTAINER5_RF_CHANNEL  = RFRDR_CHANNEL3;
const int MODEL_CONTAINER6_RF_CHANNEL  = RFRDR_CHANNEL4;

#ifdef OBJET_MACHINE_KESHET
const int MODEL_CONTAINER7_RF_CHANNEL  = RFRDR_CHANNEL1;
const int MODEL_CONTAINER8_RF_CHANNEL  = RFRDR_CHANNEL2;
const int MODEL_CONTAINER9_RF_CHANNEL  = RFRDR_CHANNEL3;
const int MODEL_CONTAINER10_RF_CHANNEL  = RFRDR_CHANNEL4;
const int MODEL_CONTAINER11_RF_CHANNEL  = RFRDR_CHANNEL1;
const int MODEL_CONTAINER12_RF_CHANNEL  = RFRDR_CHANNEL2;
const int MODEL_CONTAINER13_RF_CHANNEL  = RFRDR_CHANNEL3;
const int MODEL_CONTAINER14_RF_CHANNEL  = RFRDR_CHANNEL4;
#endif

extern const int PumpsCounterID[NUMBER_OF_PUMPS];

/*******************************************************************************
   Class CQSingleContainer implementation
*******************************************************************************/

// Constructor
CQSingleContainer::CQSingleContainer(int FullContainerWeight, int EmptyContainerWeight, TTankIndex tank_index, int rdr_num, int channel_num)
	: CQComponent ("CQSingleContainer" + QIntToStr(tank_index))
{
  for ( int i = 0; i < BATCH_NO_SIZE ; ++i)
	{
        m_batchNo[i] = '\0';
    }
    m_IsInserted = false;
    m_IsMicroswitchInserted = false;
    m_NewInsertion = false;;
    m_stable_flag = false;
    m_CurrentWeightInA2D = 0;
    m_CurrentWeightInGram =0;
    m_PrevWeightInGram = 0;
    m_currReadTagWeight = 0;
    m_Gain = 0 ;
	m_Offset = 0;
    m_NeedToShowWeight = false;
    m_Enabled = false;
    m_TagID = 0;
    m_StateTimer = 0;
    m_PreviousStateMachineIteration = 0;
    m_ContainerPtr = NULL;
    m_KeepCurrentPumpTime = 0;
    m_TagCurrentWeightInA2D = 0;
    m_RemountInProgress = false;
    m_IsDrainNeeded = false;
    m_IsDrainActive = false;
    m_DrainTankStartTime = 0 ;
	
	memset(&m_TankIDState, 0, sizeof (TankIdentificationStates));
    memset(&m_LastTankIDState, 0, sizeof (TankIdentificationStates));
	memset(&m_State, 0, sizeof (TankIdentificationStates));
    memset(&m_LastState, 0, sizeof (TankIdentificationStates));
	m_EmptyContainerWeight    = EmptyContainerWeight;
	m_TankIndex               = tank_index;
	m_ParamsMgr               = CAppParams::Instance();
	m_NewInsertion            = true;
	m_DialogAcked             = false;
	m_ForceUsingTank          = false;
	m_RunRRWizard             = false;
	m_ResinExpired            = false;
	m_MaxPumpTimeExceeded     = false;
	m_SkipMachineResinTypeChecking = false;
	m_AllowServiceMaterials   = false;
	m_ContainerPumpTime       = 0;
	m_CurrentPumpTime         = 0;
	m_ContainerExpirationDate = 0;
	m_IdentificationTime      = 0;
	m_ContainerMaxPumpTime    = 0;
	m_RdrNum                  = rdr_num;
	m_ChannelNum              = channel_num;
	m_OnEnterBeginNoTankCallback = NULL;
	m_OnEnterBeginNoTankCockie   = NULL;
	m_OnLeaveCheckResinTypeCallback = NULL;
	m_OnLeaveCheckResinTypeCockie   = NULL;
	m_IgnoreTankInsertionAndRemovalSignals = false;

	// Create the Tag accessor object:
	m_TagAccess = new CContainerTag(rdr_num, channel_num);

	// m_FEEnabledControlID = FE_MODEL_CONTAINER_1_ENABLED_STATUS;
	// todo -oNobody -cNone:
	m_FEEnabledControlID = FE_TANK_ENABLING_STATUS_BASE + tank_index;

	m_FEExistenceControlID = FE_TANK_EXISTENCE_STATUS_BASE + tank_index;

	m_PumpCounterIDList.clear();

	for(int i = FIRST_PUMP; i < NUMBER_OF_PUMPS; i++)
		if(IsPumpRelatedToTank(static_cast<TPumpIndex>(i), tank_index))
			m_PumpCounterIDList.push_back(PumpsCounterID[i]);

	m_LastInsertionTime    = QGetTicks();
	//OBJET_MACHINE
	m_nofWeightStabilizationIterations = 1;
	m_lastConsumedWeight = 0;
	m_actualAmountOfPumpsActivation = 0;
	m_amountOfPumpsActivationForNextCheck = (WEAK_STRONG_PUMP_RATIO * MINIMAL_MATERIAL_CONSUMPTION * 1000 * 2) / (3 * m_ParamsMgr->PumpsTimeDutyOn);
	m_minimalNumberOfPumpsActivation = (1000 * m_ParamsMgr->FillingTimeout) / (m_ParamsMgr->PumpsTimeDutyOn + m_ParamsMgr->PumpsTimeDutyOff);
	m_weightGap = INITIAL_GAP;
	m_containerSMTimer = 0;
	m_previousTime = 0;
    m_consumptionReadingCounter = 0;
	m_firstEntrance = true;
	m_WriteErrorTagCntr = 0;
	m_needBatchNoUpdate = false;
	m_weightReadingSamples = new CMovingAverage<int>(NUM_OF_WEIGHT_SAMPLES);
	m_addRemoveRFIDCounter = new CAddRemoveRFIDCounter();
	m_minWeight = 0;
	m_maxWeight = 0;
        if(Q2RTApplication->GetMachineType() == (mtObjet1000 ))
  	{
		  m_wait_for_stable_weight = WAIT_FOR_STABLE_WEIGHT_OBJET1000;
        }
  	else
  	{
		  m_wait_for_stable_weight = WAIT_FOR_STABLE_WEIGHT;
  	}
}

// Destructor
CQSingleContainer::~CQSingleContainer()
{
	Q_SAFE_DELETE(m_TagAccess);
	Q_SAFE_DELETE(m_addRemoveRFIDCounter);
	Q_SAFE_DELETE(m_weightReadingSamples);
}


QString CQSingleContainer::GetMaterialType()
{
	QString retVal;

	m_MutexPipeMaterialName.WaitFor();
	retVal = m_PipeMaterial;
	m_MutexPipeMaterialName.Release();

	return retVal;
}

bool CQSingleContainer::NeedBatchNumberUpdate()
{
	bool retVal;

	m_MutexBatchNoUpdate.WaitFor();
	retVal = m_needBatchNoUpdate;
	m_MutexBatchNoUpdate.Release();

	return retVal;
}

QString CQSingleContainer::GetTankMaterialType()
{
	QString retVal;

	m_MutexTankMaterialName.WaitFor();
	retVal = m_TankMaterial;
	m_MutexTankMaterialName.Release();

	return retVal;
}
QString CQSingleContainer::GetResinTypeByIndex(int resin_index)
{
	QString ret;
	if (IS_MODEL_TANK(m_TankIndex))
		if (m_ContainerPtr->GetModelResinType(resin_index) == "Unsupported")
			ret = m_ContainerPtr->GetServiceModelResinType(resin_index);
		else
			ret = m_ContainerPtr->GetModelResinType(resin_index);
	else if (m_ContainerPtr->GetSupportResinType(resin_index) == "Unsupported")
		ret = m_ContainerPtr->GetServiceSupportResinType(resin_index);
	else
		ret = m_ContainerPtr->GetSupportResinType(resin_index);
	return ret;
}

unsigned long CQSingleContainer::GetExpirationDate()
{
	return m_ContainerExpirationDate;
}

void CQSingleContainer::SendNotification(TTagIdentifyNotificationMessage& Message)
{
	m_ContainerPtr->m_NotificationDistributeMutexGuard.WaitFor();

	try
	{
		TankIDentificationSM(Message);
	}
	catch(...)
	{
		throw EContainer("SingleContainer: TankIDentificationSM() failed.");
	}

	m_ContainerPtr->m_NotificationDistributeMutexGuard.Release();
}

void CQSingleContainer::InitStateMachine(void)
{
	// check if current SingleContainer had an RF Tag during last program run:
	if (!m_ContainerPtr)
		throw EContainer("m_ContainerPtr not initialized");

	CQLog::Write(LOG_TAG_RFID, "Initializing Container Identification StateMachine for: " \
				 "TankID: %d", m_TankIndex);

	m_TagID = 0;
	m_State = BEGIN_NO_TANK;
	m_TankIDState = DATA_READ_NOT_ACTIVE;

	//  m_LastState given state other then BEGIN_NO_TANK to allow OnEnter to work.
	m_LastState = TANK_ABORT_PENDING;

	HandleStateEnter();

	m_state_text = "";
	m_StateTimer = 0;
	m_PreviousStateMachineIteration = QGetTicks();
}

TQErrCode CQSingleContainer::TankIDentificationSM(TTagIdentifyNotificationMessage& Message)
{
	m_ContainerPtr->m_StateMachineMutexGuard.WaitFor();

	TQErrCode StateMachineErr = 0;

	TTagIdentificationEventType Input = Message.EventType;

	// Calc time from last function State Machine iteration:
	unsigned long CurrentTime = QGetTicks();
	m_StateTimer += (QGetTicks() - m_PreviousStateMachineIteration);
	m_PreviousStateMachineIteration = CurrentTime;

	// "Translate" generic RF ADD/REMOVE messages:
	if ((m_TagID != 0) && (m_State != BEGIN_NO_TANK))
	{
		if ((Input == RF_REMOVE) && (Message.data_2 == m_TagID))
			Input = MY_RF_REMOVE;
		else if
		((Input == RF_ADD)    && (Message.data_2 == m_TagID))
			Input = MY_RF_ADD;

		if ((Input == RF_REMOVE) && (Message.data_2 != m_TagID))
			Input = OTHER_RF_REMOVE;

		// OTHER_RF_ADD is used in case two Tags are introduces to a single channel. (this disqualifies the Container)
		else if
		((Input == RF_ADD)    && (Message.data_2 != m_TagID))
			Input = OTHER_RF_ADD;
	}

	m_LastState = m_State;
	if(Input == MY_RF_REMOVE)
		m_addRemoveRFIDCounter->IncCounter();

/*    CQLog::Write(LOG_TAG_GENERAL, "^^^^^^^^^^^^^ TankIDentificationSM|%d| in %s |%08d|", m_TankIndex, debugContainerStateStr[m_State], QGetTicks());*/
	switch (m_State)
	{

		case BEGIN_NO_TANK:
			m_state_text  = "No Tank";

			// If we work without RFID at all: go directly to NO_TAG_STABLE state.
			if (Input == MY_MS_IN)
			{
				//if(!m_ParamsMgr->USING_RFID)
#ifdef DONT_USING_RFID
				m_State = NO_TAG_STABLE;
#else
				//else
				{
					m_State      = RF_WAITING;
					m_StateTimer = 0;
				}
#endif
			}
			break;

		case RF_WAITING:

			m_state_text = "RF_WAITING";

			if (Input == MY_MS_OUT)
				m_State = BEGIN_NO_TANK;
			else if (Input == RF_ADD)
			{
				m_StateTimer = 0;
				m_TagID      = Message.data_2; // place the Tag ID.
				m_State      = IDENTIFIED_STABLE;

				// We put Tank Read state initialization here, (And not in OnEnter)
				// since we don't want it ALWAYS when we go to IDENTIFIED_STABLE.
				m_TankIDState = CHECK_RESIN_TYPE;

				// ... And To allow OnEnterState work properly, give it some other value:
				m_LastTankIDState = DISABLED_TANK;
			}
			else if (m_StateTimer >= RF_WAITING_TIMEOUT)
			{
				m_State = BEGIN_NO_TANK;
			}
			break;

		case NO_TAG_STABLE:

			if (Input == MY_MS_OUT)
			{
				m_StateTimer = 0;
				m_State      = MS_NOISY_NO_TAG;
			}
			break;

		case IDENTIFIED_STABLE:
		{
			ContainerEnablingSM();

			if (Input == MY_MS_OUT)
			{
				m_StateTimer = 0;
				m_State      = TANK_ABORT_PENDING;
			}
			else if (Input == MY_RF_REMOVE)
			{
				m_StateTimer = 0;
				m_State      = TAG_REMOVED_PENDING;
			}
			break;
		}

		case TANK_ABORT_PENDING:

			if (Input == MY_MS_IN)
			{
				m_StateTimer = 0;
				m_State = IDENTIFIED_STABLE;
			}
			else if (m_StateTimer >= TANK_ABORT_PENDING_TIMEOUT)
			{
				m_TagID = 0;
				m_TagAccess->InvalidateTagData();
				m_TagAccess->SetTagID(0);

				m_State = BEGIN_NO_TANK;
			}
			break;

		case TAG_REMOVED_PENDING:
			if (Input == MY_RF_ADD)
			{
				m_StateTimer = 0;
				m_State = IDENTIFIED_STABLE;
			}
			else if (Input == MY_MS_OUT)
			{
				m_State = BEGIN_NO_TANK;
			}
			else if (m_StateTimer >= TAG_REMOVED_PENDING_TIMEOUT)
			{
				CQLog::Write(LOG_TAG_RFID, "RF Tag Error: RF Tag reading was lost. TankID: %d", m_TankIndex);

				m_TagID = 0;
				m_TagAccess->InvalidateTagData();
				m_TagAccess->SetTagID(0);

				m_State = BEGIN_NO_TANK;
			}
			break;

		case MS_NOISY_NO_TAG:

			if (Input == MY_MS_IN)
				m_State = NO_TAG_STABLE;
			else if (m_StateTimer >= MS_NOISY_NO_TAG_TIMEOUT)
				m_State = BEGIN_NO_TANK;
			break;

		default:

			break;
	}

	///////////////////////////////////////////
	// Handling States "On-Leave" and "On-Enter"
	///////////////////////////////////////////
	HandleStateLeave();
	HandleStateEnter();
/*            CQLog::Write(LOG_TAG_GENERAL, "^^^^^^^^^^^^^ TankIDentificationSM|%d| out %s |%08d|", m_TankIndex, debugContainerStateStr[m_State], QGetTicks());*/

	WriteDebugMessages();
	WriteLogData(Message);

	m_ContainerPtr->m_StateMachineMutexGuard.Release();

	return StateMachineErr;
}

void CQSingleContainer::HandleStateLeave()
{
// todo -oShahar.Behagen@objet.com -cNone:  add if check and Handle leave RF_WAITING state
	LeaveState(RF_WAITING);
}

void CQSingleContainer::HandleStateEnter()
{
	// Handle enter RF_WAITING state:
	if (EnterState(RF_WAITING))
	{
		CRFIDBase* RFIDInstance = CRFID::Instance();
		RFIDInstance->EnableSampling(m_RdrNum, m_ChannelNum);
	}

	// Handle enter IDENTIFIED_STABLE state:
	if (EnterState(IDENTIFIED_STABLE))
	{
		SetTankInserted(true);
		FrontEndInterface->UpdateStatus(m_FEExistenceControlID,  static_cast<int>(true));

		// the  m_StateTimer is common to the main Statemachine and to the Tag Read state machine.
		m_StateTimer  = 0;

		// To cause Tank status update after resin replacement in the NON-replaced Tank, we call:
		// m_ContainerPtr->TanksStatusLogic();

		if ((m_state_text = GetTankMaterialType()) == "")
		{
			CMaintenanceCounters *MaintCounters = CMaintenanceCounters::GetInstance();

			// Initialize the pump time:
			m_LastPumpTimeVector.clear();

			for(TPumpCounterIDs::iterator i = m_PumpCounterIDList.begin(); i != m_PumpCounterIDList.end(); ++i)
				m_LastPumpTimeVector.push_back(MaintCounters->GetElapsedSeconds(*i));

			m_IdentificationTime = QGetTicks();
			m_TagAccess->SetTagID(m_TagID);

			ContainerEnablingSM();
		}
	}
	else

		// Handle enter BEGIN_NO_TANK state:
		if (EnterState(BEGIN_NO_TANK))
		{
			SetTankInserted(false);
			FrontEndInterface->UpdateStatus(m_FEExistenceControlID,  static_cast<int>(false));
			//if(m_ParamsMgr->USING_RFID)
#ifndef DONT_USING_RFID
			{
				CRFIDBase* RFIDInstance = CRFID::Instance();
				RFIDInstance->DisableSampling(m_RdrNum, m_ChannelNum);
				RFIDInstance->RemoveAllTagsFromChannel(m_RdrNum, m_ChannelNum);
			}
#endif

			// Clear Tag ID and Resolved Array when entering this state:
			m_TagID                   = 0;
			m_ContainerPumpTime       = 0;
			m_CurrentPumpTime         = 0;
			m_ContainerExpirationDate = 0;
			m_IdentificationTime      = 0;
			m_DialogAcked         = false;
			m_ForceUsingTank      = false;
			m_RunRRWizard         = false;
			m_ResinExpired        = false;
			m_MaxPumpTimeExceeded = false;
			m_TankIDState         = DATA_READ_NOT_ACTIVE;

			//OBJET_MACHINE
						m_weightGap = INITIAL_GAP;
			m_containerSMTimer = 0;
			m_firstEntrance = true;
			m_actualAmountOfPumpsActivation = 0;


			// ! Do not reset m_SkipMachineResinTypeChecking. It is set through mainUI when running RRWizard.
			// m_SkipMachineResinTypeChecking = false;

			m_TagAccess->InvalidateTagData();
			SetTankMaterialType("");
			SetMaterialType(m_ParamsMgr->TypesArrayPerPipe[m_TankIndex]);

			m_ContainerPtr->TanksStatusLogic(m_TankIndex);

			// Invoking the callback if available
			if (m_OnEnterBeginNoTankCallback != NULL && m_OnEnterBeginNoTankCockie != NULL)
				(*m_OnEnterBeginNoTankCallback)(m_TankIndex, m_OnEnterBeginNoTankCockie);
		}

	//////////////////
	// Handle NO RFID
	//////////////////

	// No RF at all. (cases:)
	// 1. If Tank with no Tag is inserted,
	// 2. Entire RFID capability is disabled
	// 3. RF signal disappeared, *after* being Identified.
		else if (EnterState(NO_TAG_STABLE))
		{
#ifdef DONT_USING_RFID
			//if(!m_ParamsMgr->USING_RFID)
			//{
			SetTankMaterialType(GetMaterialType());
			m_state_text = GetMaterialType();
			//}
			//else
			//{
#else
			CRFIDBase* RFIDInstance = CRFID::Instance();
			RFIDInstance->DisableSampling(m_RdrNum, m_ChannelNum);
			RFIDInstance->RemoveAllTagsFromChannel(m_RdrNum, m_ChannelNum);

			SetTankMaterialType("Unknown");
			m_state_text = "Unknown";
#endif
			//}
			m_ContainerPtr->TanksStatusLogic(m_TankIndex);
		}
}
// Handle Tank Data Read State Machine On Enter
///////////////////////////////////////////////
void CQSingleContainer::HandleStateEnterContainerEnablingSM()
{
	if (EnterState(CHECK_RESIN_TYPE))
	{
		SetDialogAcked(false);
		m_StateTimer = 0;
	}
	else if (EnterState(CHECK_PUMP_TIME))
	{
		SetDialogAcked(false);
		m_StateTimer = 0;
	}
	else if (EnterState(CHECK_EXPIRATION_DATE))
	{
		SetDialogAcked(false);
		m_StateTimer = 0;
	}
//OBJET_MACHINE
	else if (EnterState(CHECK_TAG_ERRORS))
	{
		SetDialogAcked(false);
		m_StateTimer = 0;
	}
	else if (EnterState(READ_BATCH_NUMBER))
	{
		SetDialogAcked(false);
		m_StateTimer = 0;
	}
	else if (EnterState(CHECK_CONTAINER_REFILL))
	{
		SetDialogAcked(false);
		m_StateTimer = 0;
	}
	else if (EnterState(CHECK_WEIGHT_STABILIZATION))
	{
		SetDialogAcked(false);
		m_StateTimer = 0;
	}
	else if (EnterState(DISABLED_TANK))
	{
		QString TankNotice = FrontEndInterface->GetTankIDNoticeText(m_TankIndex);
		FrontEndInterface->UpdateStatus(FE_TANK_DISABLING_HINT_BASE + m_TankIndex, TankNotice, false);
		SetDialogAcked(false);
		m_StateTimer = 0;
	}
	else if (EnterState(ENABLED_TANK))
	{
		QString TankNotice = "";
		FrontEndInterface->UpdateStatus(FE_TANK_DISABLING_HINT_BASE + m_TankIndex, TankNotice, false);
	}
}
void CQSingleContainer::HandleStateLeaveContainerEnablingSM()
{
	if (LeaveState(CHECK_RESIN_TYPE))
	{
		// Invoking the callback - if available
		if (m_OnLeaveCheckResinTypeCallback != NULL && m_OnLeaveCheckResinTypeCockie != NULL)
			(*m_OnLeaveCheckResinTypeCallback)(m_TankIndex, m_OnLeaveCheckResinTypeCockie);

		SetRemountInProgress(false);
	}
}
void CQSingleContainer::WriteLogData(TTagIdentifyNotificationMessage& Message)
{
	// generate the message string:
	TTagIdentificationEventType Input = Message.EventType;

	QString stateMachineInputStr = "NoMessage"; // for debug

	switch(Input)
	{
		case TIMER:
			stateMachineInputStr = "TIMER          ";
			break;
		case MY_MS_IN:
			stateMachineInputStr = "MY_MS_IN       ";
			break;
		case MY_MS_OUT:
			stateMachineInputStr = "MY_MS_OUT      ";
			break;
		case RF_ADD:
			stateMachineInputStr = "RF_ADD         ";
			break;
		case RF_REMOVE:
			stateMachineInputStr = "RF_REMOVE      ";
			break;
		case MY_RF_ADD:
			stateMachineInputStr = "MY_RF_ADD      ";
			break;
		case OTHER_RF_ADD:
			stateMachineInputStr = "OTHER_RF_ADD   ";
			break;
		case MY_RF_REMOVE:
			stateMachineInputStr = "MY_RF_REMOVE   ";
			break;
		case OTHER_RF_REMOVE:
			stateMachineInputStr = "OTHER_RF_REMOVE";
			break;
		default:
			stateMachineInputStr = "ERR_MESSAGE    ";
			break;
	}

	QString WhichContainer = TankToStr(m_TankIndex);

	// Write to Log only when a change occures in the state machine.
	if (m_LastState != m_State)
		CQLog::Write(LOG_TAG_RFID, "Q: "      + WhichContainer
					 + " Msg: "    + stateMachineInputStr
					 + " Time: "   + QIntToStr(Message.TimeStamp)
					 + " Result: " + m_state_text);
}
void CQSingleContainer::WriteDebugMessages()
{
	QString ui_text = "";

	switch (m_State)
	{
		case BEGIN_NO_TANK:
			ui_text = "";
			m_state_text = "No Tank";
			break;
		case RF_WAITING:
			ui_text = "";
			m_state_text = "RF_WAITING";
			break;
		case NO_TAG_STABLE:
			ui_text = m_state_text; // == Resin Name
			break;
		case IDENTIFIED_STABLE:
			ui_text = m_state_text; // == Resin Name
			break;
		case TANK_ABORT_PENDING:
			m_state_text = "TANK_ABORT_PENDING";
			break;
		case TAG_REMOVED_PENDING:
			ui_text = m_state_text; // == Resin Name
			break;
		case MS_NOISY_NO_TAG:
			m_state_text = "MS_NOISY_NO_TAG";
			break;
		default:
			ui_text = m_state_text = "RF_ERROR";
			break;
	}

	// Update the state text in front end:
	FrontEndInterface->UpdateStatus(FE_UPDATE_RESIN_TYPE_BASE + m_TankIndex, ui_text);
}

bool CQSingleContainer::LeaveState(TankIdentificationStates leaveState)
{
	// Determine to which State Machine this check refers:
	if ((m_State == IDENTIFIED_STABLE) && (leaveState > LAST_STATE_OF_TANK_IDENTIFICATION_STATEMACHINE))
	{
		if ((m_LastTankIDState != m_TankIDState) && (m_LastTankIDState == leaveState))
			return true;
	}
	else
	{
		if ((m_LastState != m_State) && (m_LastState == leaveState))
			return true;
	}
	return false;
}
bool CQSingleContainer::EnterState(TankIdentificationStates enteredState)
{
	// Determine to which State Machine this check refers:
	if (enteredState > LAST_STATE_OF_TANK_IDENTIFICATION_STATEMACHINE)
	{
		if ((m_LastTankIDState != m_TankIDState) && (m_TankIDState == enteredState))
			return true;
	}
	else
	{
		if ((m_LastState != m_State) && (m_State == enteredState))
			return true;
	}
	return false;
}

// This function is called while statemachine is in IDENTIFIED_STABLE state.
void CQSingleContainer::ContainerEnablingSM()
{
	TPumpTimeVector       currentPumpTimeVector;

	TAG_TYPE__RESIN_TYPE  resinType                 = 0;
	int                   FEMessageEnabledDisabled  = FE_TANK_DISABLED_STATUS;
	int                   TankIDNoticeType          = FE_TANK_ID_NOTICE_DLG_PIPE_NO_MATCH;

	QString               MachineMaterialType       = "";
	CMachineManager       *Mgr                      = Q2RTApplication->GetMachineManager();
	TMachineState         MachineState              = Mgr->GetCurrentState();
	bool                  IsWizardRunning           = Mgr->IsWizardRunning();

	TAG_TYPE__ERROR_CODE tagErrorCode = 0;

	m_LastTankIDState = m_TankIDState;

	// Calc time from last function State Machine iteration:
	unsigned long CurrentTime = QGetTicks();
	m_containerSMTimer += (QGetTicks() - m_previousTime);
	m_previousTime = CurrentTime;

	TQErrCode Err = Q_NO_ERROR;

//OBJET_MACHINE //refill req.
	m_currReadTagWeight = 0;
	//static int ContainerRefillCounter = 0;
/*            CQLog::Write(LOG_TAG_GENERAL, "^^^^^^^^^^^^^ ContainerEnablingSM|%d| in %s |%08d|", m_TankIndex, debugContainerStateStr[m_TankIDState], QGetTicks());*/

	switch (m_TankIDState)
	{
		case CHECK_RESIN_TYPE:
		{
			bool                  ResinTypeOK               = false;
			//////////////////////
			TChamberIndex ti = TankToChamber(m_TankIndex);
			if (NUMBER_OF_CHAMBERS == ti)
				MachineMaterialType = "";
			else
				MachineMaterialType = m_ParamsMgr->TypesArrayPerChamber[ti];

			// Read Resin Type from Tag
			if ((m_TagAccess->TagReadResinType(resinType)) == Q_NO_ERROR)
			{
				SetTankMaterialType(m_state_text = GetResinTypeByIndex(resinType));
			}
			else
			{
				// stays in CHECK_RESIN_TYPE and re-tries to read Tag.
				m_TankIDState = CHECK_RESIN_TYPE;
				break;
			}
				// Allow all model material ,Keshet request "M.Cleanser"
			if (IS_MODEL_TANK(m_TankIndex))
				SetAllowServiceMaterials(true);

			// Handle No Tank: (denoted by an empty string for the Tank Material name.)
			if (GetTankMaterialType() == "")
			{
				// Close any dialog related to this tank if it was open:
				TankIDNoticeType         = FE_TANK_ID_NOTICE_DLG_HIDE;

				// Although tanks are Disabled if not inserted, we don't need the "Disabled" icon.
				// (so send an Enabled notification)
				FEMessageEnabledDisabled = FE_TANK_ENABLED_STATUS;
			}
			else
				// Check for a "Support instead of Model" (manufacturing mistake)
				if ((IS_MODEL_TANK(m_TankIndex)) && (m_ContainerPtr->GetSupportResinType(resinType) != "Unsupported"))
				{
					FEMessageEnabledDisabled = FE_TANK_DISABLED_STATUS;
					TankIDNoticeType         = FE_TANK_ID_NOTICE_DLG_SUPPORT_IN_MODEL_MISPLACE;
				}
				else
					// Check for a "Model instead of Support" (manufacturing mistake)
					if ((IS_SUPPORT_TANK(m_TankIndex)) && (m_ContainerPtr->GetModelResinType(resinType) != "Unsupported"))
					{
						FEMessageEnabledDisabled = FE_TANK_DISABLED_STATUS;
						TankIDNoticeType         = FE_TANK_ID_NOTICE_DLG_MODEL_IN_SUPPORT_MISPLACE;
					}
					else
						// Model Service material is inserted
						if (IS_MODEL_TANK(m_TankIndex) && (m_ContainerPtr->GetServiceModelResinType(resinType) != "Unsupported") && !AllowServiceMaterials())
						{
							if ((MachineState == msPrinting) || (MachineState == msPrePrint) || (true == IsWizardRunning))
							{
								FEMessageEnabledDisabled = FE_TANK_DISABLED_STATUS;
								TankIDNoticeType         = FE_TANK_ID_NOTICE_DLG_MACHINE_NO_MATCH_MSPRINTING;
							}
							else
							{
								FEMessageEnabledDisabled = FE_TANK_DISABLED_STATUS;
								TankIDNoticeType         = FE_TANK_ID_NOTICE_DLG_SERVICE_IN_REGULAR;
							}
						}
						else
							// Support Service material is inserted
							if (IS_SUPPORT_TANK(m_TankIndex) && (m_ContainerPtr->GetServiceSupportResinType(resinType) != "Unsupported") && !AllowServiceMaterials())
							{
								if ((MachineState == msPrinting) || (MachineState == msPrePrint) || (true == IsWizardRunning))
								{
									FEMessageEnabledDisabled = FE_TANK_DISABLED_STATUS;
									TankIDNoticeType         = FE_TANK_ID_NOTICE_DLG_MACHINE_NO_MATCH_MSPRINTING;
								}
								else
								{
									FEMessageEnabledDisabled = FE_TANK_DISABLED_STATUS;

									TankIDNoticeType         = FE_TANK_ID_NOTICE_DLG_SERVICE_IN_REGULAR;
								}
							}
							else if (NUMBER_OF_CHAMBERS == ti)
							{
								FEMessageEnabledDisabled = FE_TANK_DISABLED_STATUS;
								TankIDNoticeType         = FE_TANK_NOT_ASSOCIATED_WITH_CHAMBER;
							}
							else
								// Insert Model material if Model Service material is in the machine
								if ((IS_MODEL_TANK(m_TankIndex)) &&
										(CBackEndInterface::Instance()->IsServiceModelMaterial(MachineMaterialType)) &&
										("Unsupported" != m_ContainerPtr->GetModelResinType(resinType)))
								{
									FEMessageEnabledDisabled = FE_TANK_DISABLED_STATUS;
									TankIDNoticeType         = FE_TANK_ID_NOTICE_DLG_REGULAR_IN_SERVICE;
								}
								else
									// Insert Support material if Support Service material is in the machine
									if ((IS_SUPPORT_TANK(m_TankIndex)) &&
											(CBackEndInterface::Instance()->IsServiceSupportMaterial(MachineMaterialType)) &&
											("Unsupported" != m_ContainerPtr->GetSupportResinType(resinType)))
									{
										FEMessageEnabledDisabled = FE_TANK_DISABLED_STATUS;
										TankIDNoticeType         = FE_TANK_ID_NOTICE_DLG_REGULAR_IN_SERVICE;
									}
									else
										// Handle Unsupported resins. (new Objet resins that are not recognized by an older SW version.)
										if (GetTankMaterialType() == "Unsupported")
										{
											FEMessageEnabledDisabled = FE_TANK_DISABLED_STATUS;
											TankIDNoticeType         = FE_TANK_ID_NOTICE_DLG_UNSUPPORTED_RESIN;
										}
										else
											// Check if material is licensed in License Manager
											if(! Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetLicenseManager()->IsMaterialLicensed(GetTankMaterialType()))
											{
												FEMessageEnabledDisabled = FE_TANK_DISABLED_STATUS;
												//TankStatus               = false; //*
												TankIDNoticeType         = FE_TANK_ID_NOTICE_DLG_NOT_LICENSED_CARTRIDGE;
											}
											else
												// Check if Tank resin matches Machine Resin:
												if (GetTankMaterialType() != MachineMaterialType)
												{
													if ((MachineState == msPrinting) || (MachineState == msPrePrint) || (true == IsWizardRunning))
													{
														FEMessageEnabledDisabled = FE_TANK_DISABLED_STATUS;
														TankIDNoticeType         = FE_TANK_ID_NOTICE_DLG_MACHINE_NO_MATCH_MSPRINTING;
													}
													else
													{
														FEMessageEnabledDisabled = FE_TANK_DISABLED_STATUS;
														if( IS_MODEL_TANK(m_TankIndex) )
															TankIDNoticeType         = FE_TANK_ID_NOTICE_DLG_MACHINE_NO_MATCH;
														else
															TankIDNoticeType 		 = FE_TANK_ID_NOTICE_DLG_MACHINE_NO_MATCH_SUPPORT;
													}
												}
												else
													// Check if current tank resin (as obtained from container's Tag) also matches the short-pipe's resin:
													if (GetTankMaterialType() != GetMaterialType())
													{
														if ((MachineState == msPrinting) || (MachineState == msPrePrint) || (true == IsWizardRunning))
														{
															FEMessageEnabledDisabled = FE_TANK_DISABLED_STATUS;
															TankIDNoticeType         = FE_TANK_ID_NOTICE_DLG_PIPE_NO_MATCH_MSPRINTING;
														}
														else
														{
															if( IS_MODEL_TANK(m_TankIndex) )
																TankIDNoticeType         = FE_TANK_ID_NOTICE_DLG_PIPE_NO_MATCH;
															else
																TankIDNoticeType = FE_TANK_ID_NOTICE_DLG_PIPE_NO_MATCH_SUPPORT;
														}

													 }
													else
													{
														TankIDNoticeType         = FE_TANK_ID_NOTICE_DLG_HIDE;
														FEMessageEnabledDisabled = FE_TANK_ENABLED_STATUS;
														ResinTypeOK              = true;
													}

			if (TankIDNoticeType == FE_TANK_ID_NOTICE_DLG_HIDE)
				FrontEndInterface->HideTankIDNoticeDlg(m_TankIndex);

			if (SkipMachineResinTypeChecking() &&
					((TankIDNoticeType == FE_TANK_ID_NOTICE_DLG_MACHINE_NO_MATCH)
					 ||  (TankIDNoticeType == FE_TANK_ID_NOTICE_DLG_PIPE_NO_MATCH)
					 ||  (TankIDNoticeType == FE_TANK_ID_NOTICE_DLG_MACHINE_NO_MATCH_MSPRINTING)
					 ||  (TankIDNoticeType == FE_TANK_ID_NOTICE_DLG_PIPE_NO_MATCH_MSPRINTING)
					 ||	(TankIDNoticeType == FE_TANK_ID_NOTICE_DLG_SERVICE_IN_REGULAR)
					 ||  (TankIDNoticeType == FE_TANK_ID_NOTICE_DLG_REGULAR_IN_SERVICE)
					 ||  (TankIDNoticeType == FE_TANK_NOT_ASSOCIATED_WITH_CHAMBER)))
			{
				// Don't show the dialog.
			}
			else if ((GetDialogAcked() == false) && (TankIDNoticeType != FE_TANK_ID_NOTICE_DLG_HIDE))
			{
				FrontEndInterface->ShowTankIDNotice(TankIDNoticeType, m_TankIndex, false);
			}

			FrontEndInterface->UpdateStatus(m_FEEnabledControlID, FEMessageEnabledDisabled, false);
			// Check to which state we go:
			if (m_StateTimer > WAIT_BEFORE_GET_EXPIRATIONDATE)
			{
				if (ResinTypeOK == true)
				{
					m_TankIDState = CHECK_EXPIRATION_DATE;
				}
				// If During RR Wizard: the Dialog was not shown at all, don't wait for an Acknowledgement from it:
				/*else if(SkipMachineResinTypeChecking())
				{
					m_TankIDState = CHECK_EXPIRATION_DATE;
				}
				else
				{
					m_TankIDState = DISABLED_TANK;
				} */
				else
				{
					m_TankIDState = DISABLED_TANK;
				}
			}
			else
			{
				m_TankIDState = CHECK_RESIN_TYPE;
			}

			break;
		}
		case CHECK_EXPIRATION_DATE:
			//////////////////////////

			if (m_TagAccess->TagIsParamValid(TAG_PARAM__EXPIRATION_DATE) == TAG_PARAM_INVALID)
			{
				if ((m_TagAccess->TagReadExpirationDate(m_ContainerExpirationDate)) != Q_NO_ERROR)
				{
					// stays in CHECK_EXPIRATION_DATE and re-tries to read Tag.
					m_TankIDState = CHECK_EXPIRATION_DATE;
					break;
				}//  throw ERFID("Cannot access tag");
			}

			// Check if Resin has expired, show dialog and raise a flag:
			if (m_TagAccess->TagIsParamValid(TAG_PARAM__EXPIRATION_DATE))
			{
				if (static_cast<unsigned>(QGetCurrentTime()) > m_ContainerExpirationDate)
				{
					if (m_ResinExpired == false)
					{
						m_ResinExpired = true;
						if (SkipMachineResinTypeChecking())
							// Don't show the dialog.
						{}
						else
						{
							FrontEndInterface->ShowTankIDNotice(FE_TANK_ID_NOTICE_DLG_RESIN_EXPIRED, m_TankIndex, false);
						}
						FrontEndInterface->UpdateStatus(m_FEEnabledControlID, FE_TANK_DISABLED_STATUS, false);
						SetTankEnabled(false);
					}
				}
			}

			// Check to which state we go:
			if (m_ResinExpired == true)
			{
				m_TankIDState = DISABLED_TANK;
			}
			else //OBJET_MACHINE
			{
				if (m_StateTimer > WAIT_BEFORE_GET_PUMPINGTIME)
				{
					m_TankIDState = CHECK_TAG_ERRORS; //go to next state
				}
				else
				{
					m_TankIDState = CHECK_EXPIRATION_DATE;
				}
			}
			break;
		case CHECK_TAG_ERRORS:
			/*			if(m_ParamsMgr->RefillProtectionBypass)
				{
				  m_TankIDState = CHECK_PUMP_TIME; //go to next state
				  break;
				}
			 */
			if (m_TagAccess->TagIsParamValid(TAG_PARAM__ERROR_CODE) == TAG_PARAM_INVALID)
			{
				if((m_TagAccess->TagReadErrorCode(tagErrorCode)) != Q_NO_ERROR)
				{
					// stays in CHECK_TAG_ERRORS and re-tries to read Tag.
					m_TankIDState = CHECK_TAG_ERRORS;
					break;
				}
			}
			if(m_TagAccess->TagIsParamValid(TAG_PARAM__ERROR_CODE))
			{
				if(tagErrorCode == NO_RESIN_CONSUMPTION_TAG_ERROR)
				{
					CQLog::Write(LOG_TAG_OBJET_MACHINE, "Error R_P 0: TankIndex:%d TgErrCode:%d", m_TankIndex, tagErrorCode);
					FrontEndInterface->ShowTankIDNotice(FE_TANK_ID_MATERIAL_CONSUMPTION_PROBLEM, m_TankIndex, false);
					FrontEndInterface->UpdateStatus(m_FEEnabledControlID, FE_TANK_DISABLED_STATUS, false);
					SetTankEnabled(false);
					m_TankIDState = DISABLED_TANK;
					break;
				}
			}
			if (m_StateTimer > WAIT_BEFORE_GET_PUMPINGTIME)
			{
				m_TankIDState = READ_BATCH_NUMBER; //go to next state
			}
			else
			{
				m_TankIDState = CHECK_TAG_ERRORS;
			}

			break;
		case READ_BATCH_NUMBER:
			// Read batch number from Tag
			if(m_TagAccess->TagReadBatchNo(m_batchNo) != Q_NO_ERROR)
			{
				// stays in READ_BATCH_NUMBER and re-tries to read Tag.
				CQLog::Write(LOG_TAG_GENERAL, "Error reading batch number, TankIndex: %d", m_TankIndex);
				m_TankIDState = READ_BATCH_NUMBER;
				break;
			}
			if(m_batchNo != '\0')
			{
				CQLog::Write(LOG_TAG_RFID, "TankIndex:%d BatchNo:%s", m_TankIndex, m_batchNo);
				SetNeedBatchNumberUpdate(true);
			}

			m_TankIDState = CHECK_WEIGHT_STABILIZATION; //go to next state
			m_containerSMTimer = 0;
			m_minWeight = MAX_INITIAL_WEIGHT;
			m_maxWeight = 0;
			/*if (m_StateTimer > WAIT_BEFORE_GET_PUMPINGTIME)
			{
				m_TankIDState = CHECK_WEIGHT_STABILIZATION; //go to next state
				m_containerSMTimer = 0;
			}
			else
			{
				m_TankIDState = READ_BATCH_NUMBER;
			}*/

			break;
//OBJET_MACHINE //refill req.
		case CHECK_WEIGHT_STABILIZATION:
            if (m_containerSMTimer<(m_wait_for_stable_weight-ALLOW_WEIGHT_TO_STABLIZE)) //check weight stabilization
			{                    /* early enabling */
				if(m_minWeight > m_CurrentWeightInGram)
				  m_minWeight = m_CurrentWeightInGram;
				if(m_maxWeight < m_CurrentWeightInGram)
				  m_maxWeight = m_CurrentWeightInGram;
				if(abs(m_maxWeight - m_minWeight) <= MAX_FLUCTUATION_WEIGHT)
				{
					m_TankIDState = CHECK_WEIGHT_STABILIZATION; //continue to check stabilization
				}
				else //not stable
				{
					if(m_nofWeightStabilizationIterations == MAX_ITERATIONS_FOR_STABLE_WEIGHT) //weight unstable
					{
						CQLog::Write(LOG_TAG_OBJET_MACHINE, "Error WeightStabilizationCheck: TankIndex:%d currWeight:%d prevWeight:%d", m_TankIndex, m_CurrentWeightInGram, m_PrevWeightInGram);
						FrontEndInterface->ShowTankIDNotice(FE_TANK_ID_NOTICE_DLG_CALIBRATION_PROBLEM, m_TankIndex, false);
						FrontEndInterface->UpdateStatus(m_FEEnabledControlID, FE_TANK_DISABLED_STATUS, false);
						SetTankEnabled(false);
						m_nofWeightStabilizationIterations = 1; //reset counter
						m_TankIDState = DISABLED_TANK;
					}
					else
					{
                        			m_containerSMTimer = 0;
						m_nofWeightStabilizationIterations++;
						m_TankIDState = CHECK_WEIGHT_STABILIZATION; //do another iteration
					}
				}
			}
			else //>9 sec
			{
				m_TankIDState = CHECK_CONTAINER_REFILL; //go to next step
				m_nofWeightStabilizationIterations = 1; //reset counter
				m_containerSMTimer = 0; //reset timer
				m_minWeight = MAX_INITIAL_WEIGHT;
				m_maxWeight = 0;
                SetTankEnabled(true);       /* early enabling */
			}
			break;
		case CHECK_CONTAINER_REFILL:
			CQLog::Write(LOG_TAG_OBJET_MACHINE, "R_P 1: TankIndex:%d", m_TankIndex);
			m_MutexUpdateTagWeight.WaitFor();
			// Read Resin Type from Tag
			if((m_TagAccess->TagReadWeight(m_currReadTagWeight)) != Q_NO_ERROR)
			{
				//read failed, stay in state and try to read again
				m_TankIDState = CHECK_CONTAINER_REFILL;
				m_containerSMTimer = 0;
				//break;
			}
			else
			{
				int currWeight = GetTankWeightInGram();
				m_weightGap = 0;
				CQLog::Write(LOG_TAG_OBJET_MACHINE, "R_P 1.1: TankIndex:%d CurrWt:%d TagWt:%d", m_TankIndex, currWeight, m_currReadTagWeight);
				//check that read is valid
				if(m_currReadTagWeight > -MAX_DEVIATION_WEIGHT  && m_currReadTagWeight <= m_ParamsMgr->CartridgeInitialWeight + MAX_DEVIATION_WEIGHT)
				{
					if(m_currReadTagWeight == currWeight) //weight ok, no need to write to tag, go to next state
					{
						m_TankIDState = CHECK_PUMP_TIME;
						m_containerSMTimer = 0;
					}
					else
					{
						if(m_currReadTagWeight == 0) //first insertion of the cartridge to Objet
						{
							m_TankIDState = CHECK_PUMP_TIME; //go to next state
							m_containerSMTimer = 0;
							m_currReadTagWeight = m_ParamsMgr->CartridgeInitialWeight;
							if((m_TagAccess->TagWriteWeight(m_currReadTagWeight)) != Q_NO_ERROR)
							{
								CQLog::Write(LOG_TAG_OBJET_MACHINE, "Error.Couldn't write CurrWt to Tag: %d", m_currReadTagWeight);
								m_TankIDState = CHECK_CONTAINER_REFILL;
								m_currReadTagWeight = 0;
								m_containerSMTimer = 0;
							}
						}
						else if((currWeight - m_currReadTagWeight) < MAX_DEVIATION_WEIGHT) //less material than written on Tag is ok
						{
							m_weightGap = currWeight - m_currReadTagWeight;
							m_TankIDState = CHECK_PUMP_TIME; //go to next state
							m_containerSMTimer = 0;
						}
						else if((currWeight - m_currReadTagWeight) >= MAX_DEVIATION_WEIGHT) //there was a refill
						{
							FrontEndInterface->ShowTankIDNotice(FE_TANK_ID_NOTICE_DLG_CHECK_CONTAINER_REFILL, m_TankIndex, false);
							FrontEndInterface->UpdateStatus(m_FEEnabledControlID, FE_TANK_DISABLED_STATUS, false);
							SetTankEnabled(false);
							//write error code to tag
							int errCode = CARTRIDGE_REFILL_TAG_ERROR;
							m_TagAccess->TagWriteErrorCode(errCode);
							LogUserTampering(REFILL_PROTECTION_EVENT);
							CQLog::Write(LOG_TAG_OBJET_MACHINE, "Error R_P 1.2: TankIndex:%d errCode:%d ", m_TankIndex, errCode);
							m_TankIDState = DISABLED_TANK;
						}
					}
					m_containerSMTimer = 0;
				}
				else  //value is not valid, stay in state and try to read again
				{
					m_TankIDState = CHECK_CONTAINER_REFILL;
					if(m_containerSMTimer > (m_wait_for_stable_weight / 9)) //objet_test
					{
						CQLog::Write(LOG_TAG_OBJET_MACHINE, "R_P 1.3: Weight out of range TankIndex:%d TagWt:%d", m_TankIndex, m_currReadTagWeight);
						FrontEndInterface->ShowTankIDNotice(FE_TANK_ID_NOTICE_DLG_CHECK_CONTAINER_REFILL, m_TankIndex, false);
						FrontEndInterface->UpdateStatus(m_FEEnabledControlID, FE_TANK_DISABLED_STATUS, false);
						SetTankEnabled(false);
						m_TankIDState = DISABLED_TANK;
					}
				}
			}
			m_MutexUpdateTagWeight.Release();
			break;
		case CHECK_PUMP_TIME:
		{
			/*	if(m_ParamsMgr->RefillProtectionBypass)
				{
				   m_TankIDState = ENABLED_TANK; //go to next state
				   SetTankEnabled(true);
				   FrontEndInterface->UpdateStatus(m_FEEnabledControlID, FE_TANK_ENABLED_STATUS, false);
				   break;
				}
			 */
			if(MachineState == msPrinting)
			{
				//in print
				//if the tank is not active, no need to check during print
				TChamberIndex ti = TankToStaticChamber(m_TankIndex);
				if (m_ParamsMgr->ActiveTanks[ti] != m_TankIndex)
				{
					m_TankIDState = ENABLED_TANK; //go to next state
					SetTankEnabled(true);
					FrontEndInterface->UpdateStatus(m_FEEnabledControlID, FE_TANK_ENABLED_STATUS, false);
					break;
				}
				if(m_firstEntrance)
				{
					m_amountOfPumpsActivationForNextCheck = (WEAK_STRONG_PUMP_RATIO * MINIMAL_MATERIAL_CONSUMPTION * 1000 * 2) / (3 * m_ParamsMgr->PumpsTimeDutyOn);
					m_minimalNumberOfPumpsActivation = (1000 * m_ParamsMgr->FillingTimeout) / (m_ParamsMgr->PumpsTimeDutyOn + m_ParamsMgr->PumpsTimeDutyOff);
					m_actualAmountOfPumpsActivation = 0;
					m_CurrentPumpTime = 0;
					m_consumptionReadingCounter = 0;
					//take the max value between m_amountOfPumpsActivationForNextCheck and m_minimalNumberOfPumpsActivation
					if(m_amountOfPumpsActivationForNextCheck < m_minimalNumberOfPumpsActivation)
						m_amountOfPumpsActivationForNextCheck = m_minimalNumberOfPumpsActivation;

					m_weightReadingSamples->Clear();

					CQLog::Write(LOG_TAG_OBJET_MACHINE, "PumpTime 2: TankIndex:%d PumpsActivationForNextCheck:%d minPumpsActivation:%d", m_TankIndex, m_amountOfPumpsActivationForNextCheck, m_minimalNumberOfPumpsActivation);

					Err = m_TagAccess->TagReadWeight(m_currReadTagWeight);
					if(Err!=Q_NO_ERROR || m_currReadTagWeight<-MAX_DEVIATION_WEIGHT || m_currReadTagWeight>m_ParamsMgr->CartridgeInitialWeight+MAX_DEVIATION_WEIGHT)
					{
						CQLog::Write(LOG_TAG_GENERAL, "Failed reading m_currReadTagWeight TankIndex:%d Value:%d", m_TankIndex, m_currReadTagWeight);
						// stays in CHECK_PUMP_TIME and re-tries to read Tag.
						m_TankIDState = CHECK_PUMP_TIME;
						break;
					}
					m_lastConsumedWeight = m_currReadTagWeight;
					Err = m_TagAccess->TagReadPumpingTime(m_ContainerPumpTime);
					if(Err != Q_NO_ERROR)
					{
						CQLog::Write(LOG_TAG_GENERAL, "Failed reading m_ContainerPumpTime TankIndex:%d Value:%d", m_TankIndex, m_ContainerPumpTime);
						// stays in CHECK_PUMP_TIME and re-tries to read Tag.
						m_TankIDState = CHECK_PUMP_TIME;
						break;
					}

					int currWeight = GetTankWeightInGram();
					CQLog::Write(LOG_TAG_OBJET_MACHINE, "PumpTime 2.1: TankIndex:%d CurrWt:%d TagWt:%d", m_TankIndex, currWeight, m_currReadTagWeight);
					if((currWeight - m_currReadTagWeight) < MAX_DEVIATION_WEIGHT) //less material than written on Tag is allowed
					{
						m_weightGap = currWeight - m_currReadTagWeight;
						m_TankIDState = CHECK_PUMP_TIME; //go to next state
						FrontEndInterface->UpdateStatus(m_FEEnabledControlID, FE_TANK_ENABLED_STATUS, false);
						SetTankEnabled(true);
					}
					else if((currWeight - m_currReadTagWeight) >= MAX_DEVIATION_WEIGHT) //there was a refill
					{
						FrontEndInterface->ShowTankIDNotice(FE_TANK_ID_NOTICE_DLG_CHECK_CONTAINER_REFILL, m_TankIndex, false);
						FrontEndInterface->UpdateStatus(m_FEEnabledControlID, FE_TANK_DISABLED_STATUS, false);
						SetTankEnabled(false);
						//write error code to tag
						int errCode = CARTRIDGE_REFILL_TAG_ERROR;
						m_TagAccess->TagWriteErrorCode(errCode);
						LogUserTampering(REFILL_PROTECTION_EVENT);
						CQLog::Write(LOG_TAG_OBJET_MACHINE, "Error PumpTime 2.2: TankIndex:%d errCode:%d", m_TankIndex, errCode);
						m_TankIDState = DISABLED_TANK;
					}
					m_firstEntrance = false;
					break;
				}
				else //not first entrance
				{
					if(m_containerSMTimer >= WEIGHT_READINGS_SAMPLES_PERIOD)
					{
						m_containerSMTimer = 0;
						int currWeight = GetTankWeightInGram();
						int prevAvg = m_weightReadingSamples->GetAverage();
						m_weightReadingSamples->AddSample(currWeight);
						//if(currWeight!=prevAvg)
						if(currWeight != m_PrevWeightInGram) //update tag only when curr weight changes
						{
							if(m_weightReadingSamples->IsQueueFull())
							{
								int currentDeviation = currWeight - m_weightReadingSamples->GetAverage();
								if(currentDeviation < MAX_DEVIATION_WEIGHT)
								{
									int currTagWeight = currWeight - m_weightGap - (m_weightReadingSamples->GetAverage() - prevAvg);
									CQLog::Write(LOG_TAG_OBJET_MACHINE, "PumpTime 3: TankIndex:%d CurrWt:%d WtGap:%d PrevAvgWt:%d CurrTagWt:%d", m_TankIndex, currWeight, m_weightGap, prevAvg, currTagWeight);
									if(currTagWeight==0)
				  						currTagWeight = 1; //to avoid confusion with new cartridge
									if((m_TagAccess->TagWriteWeight(currTagWeight)) != Q_NO_ERROR)
									{
										m_WriteErrorTagCntr++;
										if(m_WriteErrorTagCntr > 10)
										{
											FrontEndInterface->ShowTankIDNotice(FE_TANK_ID_NOTICE_DLG_CHECK_CONTAINER_REFILL, m_TankIndex, false);
											FrontEndInterface->UpdateStatus(m_FEEnabledControlID, FE_TANK_DISABLED_STATUS, false);
											SetTankEnabled(false);
											m_TankIDState = DISABLED_TANK;
											break;
										}
									}
									else
										m_WriteErrorTagCntr = 0;
								}
								else if(currentDeviation >= MAX_DEVIATION_WEIGHT) //there was a refill
								{
									FrontEndInterface->ShowTankIDNotice(FE_TANK_ID_NOTICE_DLG_CHECK_CONTAINER_REFILL, m_TankIndex, false);
									FrontEndInterface->UpdateStatus(m_FEEnabledControlID, FE_TANK_DISABLED_STATUS, false);
									SetTankEnabled(false);
									//write error code to tag
									int errCode = CARTRIDGE_REFILL_TAG_ERROR;
									m_TagAccess->TagWriteErrorCode(errCode);
									LogUserTampering(REFILL_PROTECTION_EVENT);
									CQLog::Write(LOG_TAG_OBJET_MACHINE, "Error PumpTime 3.1: TankIndex:%d CurrWt:%d Avg:%d PrevAvg:%d errCode:%d", m_TankIndex, currWeight, m_weightReadingSamples->GetAverage(), prevAvg, errCode);
									m_TankIDState = DISABLED_TANK;
									break;
								}
							}
						}
					}
					// Accumulate Pump-On Time:
					CMaintenanceCounters *MaintCounters = CMaintenanceCounters::GetInstance();

					// Check pumps time:
					currentPumpTimeVector.clear();
					for(TPumpCounterIDs::iterator i = m_PumpCounterIDList.begin(); i != m_PumpCounterIDList.end(); ++i)
						currentPumpTimeVector.push_back(MaintCounters->GetElapsedSeconds(*i));

					// If the Pump's maintenance counter was reset by user (should happen rarely)
					// then we lose the pump time from last Tag update until counter reset.
					for(unsigned i = 0 ; i < currentPumpTimeVector.size(); i++)
						if (currentPumpTimeVector[i] < m_LastPumpTimeVector[i])
							m_LastPumpTimeVector[i] = currentPumpTimeVector[i];
                    unsigned long         diftime                   = 0;
					for(unsigned i = 0 ; i < currentPumpTimeVector.size(); i++)
						diftime += currentPumpTimeVector[i] - m_LastPumpTimeVector[i];
					if (diftime > MIN_TIME_FOR_TAG_PUMP_UPDATE)
					{
						// Accumulate all pumps' time that worked with *this* tank, and set m_LastPumpTime for next iteration:
						m_ContainerPumpTime += diftime;
						m_CurrentPumpTime   += diftime;

						for(unsigned i = 0 ; i < currentPumpTimeVector.size(); i++)
							m_LastPumpTimeVector[i] = currentPumpTimeVector[i];

						m_amountOfPumpsActivationForNextCheck = (WEAK_STRONG_PUMP_RATIO * MINIMAL_MATERIAL_CONSUMPTION * 1000 * 2) / (3 * m_ParamsMgr->PumpsTimeDutyOn);
						m_actualAmountOfPumpsActivation = (m_CurrentPumpTime * 1000) / m_ParamsMgr->PumpsTimeDutyOn;
						CQLog::Write(LOG_TAG_OBJET_MACHINE, "PumpTime 3.2: TankIndex:%d CurrPT:%d ActualPumpsActivation:%d", m_TankIndex, m_CurrentPumpTime, m_actualAmountOfPumpsActivation);

						if(m_actualAmountOfPumpsActivation > m_amountOfPumpsActivationForNextCheck)
						{
							int currWeight = GetTankWeightInGram();
							m_TagAccess->InvalidateTagSingleParam(TAG_PARAM__CURRENT_WEIGHT);
							Err = m_TagAccess->TagReadWeight(m_currReadTagWeight);
							if(Err!=Q_NO_ERROR || m_currReadTagWeight<-MAX_DEVIATION_WEIGHT || m_currReadTagWeight>m_ParamsMgr->CartridgeInitialWeight+MAX_DEVIATION_WEIGHT)
							{
			  					CQLog::Write(LOG_TAG_GENERAL,"Failed reading TagWeight TankIndex:%d Value:%d", m_TankIndex, m_currReadTagWeight);
								// stays in CHECK_PUMP_TIME and re-tries to read Tag.
								m_TankIDState = CHECK_PUMP_TIME;
								break;
							}
							else
							{
								CQLog::Write(LOG_TAG_OBJET_MACHINE, "PumpTime 3.3: TankIndex:%d CurrWt:%d CurrTagWt:%d lastConsumedWt:%d", m_TankIndex, currWeight, m_currReadTagWeight, m_lastConsumedWeight);
								// Write Pumping time to Tag:
								if ((m_TagAccess->TagWritePumpingTime(m_ContainerPumpTime)) != Q_NO_ERROR)
								{
									CQLog::Write(LOG_TAG_OBJET_MACHINE, "Error. Could not write Pump Time to Tag: %d", m_ContainerPumpTime);
								}
								//if(m_currReadTagWeight-currWeight>MINIMAL_MATERIAL_CONSUMPTION)
								if(m_lastConsumedWeight - m_currReadTagWeight >= MINIMAL_MATERIAL_CONSUMPTION)
								{

									m_lastConsumedWeight = m_currReadTagWeight;
									m_CurrentPumpTime = 0;
				   					m_consumptionReadingCounter = 0;
									m_TankIDState = CHECK_PUMP_TIME;
								}
								else
								{
								   if(m_consumptionReadingCounter<NUM_OF_CONSUMPTION_READING_RETRIES)
								   {
									 m_consumptionReadingCounter++;
									 CQLog::Write(LOG_TAG_GENERAL,"TankIndex:%d Retry reading tag weight", m_TankIndex, m_currReadTagWeight);
									 //stays in CHECK_PUMP_TIME and re-tries to read Tag.
									 m_TankIDState = CHECK_PUMP_TIME;
									 break;
								   }
								   else
								   {
									 FrontEndInterface->ShowTankIDNotice(FE_TANK_ID_MATERIAL_CONSUMPTION_PROBLEM, m_TankIndex, false);
									 FrontEndInterface->UpdateStatus(m_FEEnabledControlID, FE_TANK_DISABLED_STATUS, false);
									 SetTankEnabled(false);
									 //write error code to tag
									 int errCode = NO_RESIN_CONSUMPTION_TAG_ERROR;
									 m_TagAccess->TagWriteErrorCode(errCode);
									 LogUserTampering(REFILL_PROTECTION_EVENT);
									 CQLog::Write(LOG_TAG_OBJET_MACHINE,"Error PumpTime 3.4: TankIndex:%d errCode:%d ",m_TankIndex,errCode);
									 m_TankIDState = DISABLED_TANK;
								   }
								}
							}
						}
					}
				}// not first entrance
			}//in print
			else
				m_firstEntrance = true;
			if(m_TankIDState != DISABLED_TANK)
			{
				SetTankEnabled(true);
				FrontEndInterface->UpdateStatus(m_FEEnabledControlID, FE_TANK_ENABLED_STATUS, false);
			}
			else
				CQLog::Write(LOG_TAG_OBJET_MACHINE, "PumpTime 3.5: TankIndex:%d", m_TankIndex);
			break;
		}
		case DISABLED_TANK:
			///////////////////

			SetTankEnabled(false);
			FrontEndInterface->UpdateStatus(m_FEEnabledControlID, FE_TANK_DISABLED_STATUS, false);
			m_TankIDState = IDLE;
			break;

		case ENABLED_TANK:
			//////////////////

			SetTankEnabled(true);
			FrontEndInterface->UpdateStatus(m_FEEnabledControlID, FE_TANK_ENABLED_STATUS, false);
			m_TankIDState = IDLE;
			break;

		case IDLE:
			//////////////////

			m_TankIDState = IDLE;
			break;

	} // switch (state)

	////////////////////////////////////////////
	// Handling States "On-Leave" and "On-Enter"
	////////////////////////////////////////////
	HandleStateLeaveContainerEnablingSM();
	HandleStateEnterContainerEnablingSM();
/*            CQLog::Write(LOG_TAG_GENERAL, "^^^^^^^^^^^^^ ContainerEnablingSM|%d| out %s |%08d|", m_TankIndex, debugContainerStateStr[m_TankIDState], QGetTicks());*/
}

TQErrCode CQSingleContainer::UpdateWeightAfterWizard()
{
    TQErrCode err ;
    m_MutexUpdateTagWeight.WaitFor();
    err = m_TagAccess->TagReadWeight(m_currReadTagWeight);
    //it is better to not update the weight due to erroneous read than to block the cartridge 
    if(err!=Q_NO_ERROR || m_currReadTagWeight<-MAX_DEVIATION_WEIGHT || m_currReadTagWeight>m_ParamsMgr->CartridgeInitialWeight+MAX_DEVIATION_WEIGHT)
    {
	  m_MutexUpdateTagWeight.Release();
  	  CQLog::Write(LOG_TAG_OBJET_MACHINE,"Failed reading m_currReadTagWeight TankIndex:%d Value:%d", m_TankIndex, m_currReadTagWeight);
	  return -1;
    }
	else
	{
		int currWeight = GetTankWeightInGram();
		if(m_currReadTagWeight == 0) //first insertion of the cartridge
		{
			m_currReadTagWeight = m_ParamsMgr->CartridgeInitialWeight;
			m_TagAccess->TagWriteWeight(m_currReadTagWeight);
		     // CQLog::Write(LOG_TAG_OBJET_MACHINE, "R_P Wizard: TankIndex:%d currWeight:%d TagWeight:%d", m_TankIndex, currWeight, m_currReadTagWeight);
                       CQLog::Write(LOG_TAG_OBJET_MACHINE,"R_P Wizard: TankIndex:%d currWeight:%d", m_TankIndex,currWeight);
		}
		else if((currWeight - m_currReadTagWeight) < MAX_DEVIATION_WEIGHT)
		{
			//m_weightGap = currWeight-m_currReadTagWeight;
			int weightTag = currWeight - m_weightGap;
						if(weightTag<m_currReadTagWeight && m_weightGap!=INITIAL_GAP) //write to tag only in material reduction
				m_TagAccess->TagWriteWeight(weightTag);
			//CQLog::Write(LOG_TAG_OBJET_MACHINE, "R_P Wizard: TankIndex:%d currWeight:%d TagWeight:%d", m_TankIndex, currWeight, m_currReadTagWeight);
                        CQLog::Write(LOG_TAG_OBJET_MACHINE,"R_P Wizard: TankIndex:%d currWeight:%d TagWeight:%d gap:%d", m_TankIndex,currWeight,m_currReadTagWeight,m_weightGap);
		}
		else if((currWeight - m_currReadTagWeight) >= MAX_DEVIATION_WEIGHT) //there was a refill
		{
			FrontEndInterface->ShowTankIDNotice(FE_TANK_ID_NOTICE_DLG_CHECK_CONTAINER_REFILL, m_TankIndex, false);
			FrontEndInterface->UpdateStatus(m_FEEnabledControlID, FE_TANK_DISABLED_STATUS, false);
			SetTankEnabled(false);
			//write error code to tag
			int errCode = CARTRIDGE_REFILL_TAG_ERROR;
			m_TagAccess->TagWriteErrorCode(errCode);
			LogUserTampering(REFILL_PROTECTION_EVENT);
			CQLog::Write(LOG_TAG_OBJET_MACHINE, "Error R_P Wizard: TankIndex:%d currWeight:%d TagWeight:%d errCode:%d", m_TankIndex, currWeight, m_currReadTagWeight, errCode);
			m_TankIDState = DISABLED_TANK;
		}
	}
	CMaintenanceCounters *MaintCounters = CMaintenanceCounters::GetInstance();

	// Initialize the pump time:
	m_LastPumpTimeVector.clear();

	for(TPumpCounterIDs::iterator i = m_PumpCounterIDList.begin(); i != m_PumpCounterIDList.end(); ++i)
		m_LastPumpTimeVector.push_back(MaintCounters->GetElapsedSeconds(*i));


	m_MutexUpdateTagWeight.Release();
	return err;
}

void CQSingleContainer::LogUserTampering(TTamperEventType event_type)
{
	// Get RFID Instance:
	CRFIDBase* RFIDInstance = CRFID::Instance();
 

	int currentMachineState = (int)(Q2RTApplication->GetMachineManager()->GetCurrentState());

	QString MaterialType = TankToStr(m_TankIndex);

	switch (event_type)
	{
		case NO_RF_TAG:
			///////////////
			CQEncryptedLog::Write(LOG_TAG_GENERAL
								  , "UnTagged Cartridge Was Inserted. (%s) StateId: %d"
								  , MaterialType.c_str()
								  , currentMachineState);
			break;

		case NO_RF_TAG_FORCED_PRINT:
			////////////////////////////
			CQEncryptedLog::Write(LOG_TAG_GENERAL
								  , "User Continued with UnTagged Cartridge. (%s) StateId: %d"
								  , MaterialType.c_str()
								  , currentMachineState);
			break;
		case PUMP_TIME_EXCEED:
			//////////////////////
			// if RFID instance was never created (for example, rdr dll was not registered) then we assume working
			// without RFID. In this case, return "".
			if (!RFIDInstance)
				return;

			CQEncryptedLog::Write(LOG_TAG_GENERAL
								  , "Pump Time Exceeded. TagID: %s (%s) StateId: %d"
								  , RFIDInstance->GetTagIDAsString(m_TagID).c_str()
								  , MaterialType.c_str()
								  , currentMachineState);
			CQLog::Write(LOG_TAG_GENERAL, "Container: Pump time exceeded cartrige inserted. (%s)", MaterialType.c_str());
			break;

		case PUMP_TIME_EXCEED_FORCED_PRINT:
			///////////////////////////////////
			CQEncryptedLog::Write(LOG_TAG_GENERAL
								  , "User Continued with Pump Time Exceeded Cartridge. TagID: %s (%s) StateId: %d"
								  , RFIDInstance->GetTagIDAsString(m_TagID).c_str()
								  , MaterialType.c_str()
								  , currentMachineState);
			break;
			//OBJET_MACHINE
		case REFILL_PROTECTION_EVENT:
			// if RFID instance was never created (for example, rdr dll was not registered) then we assume working
			// without RFID. In this case, return "".
			if (!RFIDInstance)
				return;
			//read mfg date
			unsigned long mfgDate;
			m_TagAccess->TagReadMfgDateTime(mfgDate);

			CQLog::Write(LOG_TAG_OBJET_MACHINE, "Resin type: %s, Previous weight: %d, Curr weight: %d,Batch no: %s, Mfg date: %s"
						 , m_TankMaterial.c_str(), m_currReadTagWeight, GetTankWeightInGram(), m_batchNo, QDateToStr(mfgDate).c_str());

			CQEncryptedLog::Write(LOG_TAG_OBJET_MACHINE, "Resin type: %s, Previous weight: %d, Curr weight: %d,Batch no: %s, Mfg date: %s"
								  , m_TankMaterial.c_str(), m_currReadTagWeight, GetTankWeightInGram(), m_batchNo, QDateToStr(mfgDate).c_str());
			break;
	}
}
void CQSingleContainer::SetMaterialType(QString MaterialType)
{
	m_MutexPipeMaterialName.WaitFor();

	m_PipeMaterial = MaterialType;

	m_MutexPipeMaterialName.Release();
}

void CQSingleContainer::SetNeedBatchNumberUpdate(bool needUpdate)
{
	m_MutexBatchNoUpdate.WaitFor();

	m_needBatchNoUpdate = needUpdate;

	m_MutexBatchNoUpdate.Release();
}

void CQSingleContainer::SetTankMaterialType(QString MaterialType)
{
	m_MutexTankMaterialName.WaitFor();

	m_TankMaterial = MaterialType;

	m_MutexTankMaterialName.Release();
}
bool CQSingleContainer::IsTankInserted()
{
	bool Inserted;

	m_MutexTankInserted.WaitFor();
	Inserted = m_IsInserted;
	m_MutexTankInserted.Release();

	return Inserted;
}

unsigned int CQSingleContainer::GetLastInsertionTimeInTicks()
{
	return m_LastInsertionTime;
}

void CQSingleContainer::SetTankInserted(bool Inserted)
{
	if (Inserted && ! m_IsInserted)
		m_LastInsertionTime = QGetTicks();

	// Setting the 'Inserted' flag
	m_MutexTankInserted.WaitFor();
	m_IsInserted = Inserted;
	m_MutexTankInserted.Release();

	// In case of container removal detection - set the drain flag to true.....
	if (Inserted == false)
	{
		m_IsDrainNeeded = true;
		m_NewInsertion  = true;
	}
}

bool CQSingleContainer::IsMicroswitchInserted()
{
	bool ret;
	m_MutexDatabase.WaitFor();
	ret = m_IsMicroswitchInserted;
	m_MutexDatabase.Release();
	return ret;
}

// This method checks expiration directly bypassing the SM
bool CQSingleContainer::IsResinExpiredDirect()
{
	unsigned long ExpDate = 0;
	if ((m_TagAccess->TagReadExpirationDate(ExpDate)) == Q_NO_ERROR)
		if (static_cast<unsigned>(QGetCurrentTime()) > ExpDate)
			return true;
	return false;
}

void CQSingleContainer::SetMicroswitchInserted(bool Inserted)
{
	m_MutexDatabase.WaitFor();
	m_IsMicroswitchInserted = Inserted;
	m_MutexDatabase.Release();
}
bool CQSingleContainer::ResetCounter()
{
	m_MutexDatabase.WaitFor();
	m_addRemoveRFIDCounter->ResetCounter();
	m_MutexDatabase.Release();

	return true; // To make compiler happy
}

bool CQSingleContainer::IncCounter()
{
	m_MutexDatabase.WaitFor();
	m_addRemoveRFIDCounter->IncCounter();
	m_MutexDatabase.Release();

	return true; // To make compiler happy
}

int CQSingleContainer::GetNumOfReconnections()
{
	int numOfReconnections = 0;
	m_MutexDatabase.WaitFor();
	numOfReconnections = m_addRemoveRFIDCounter->GetNumOfReconnections();
	m_MutexDatabase.Release();
	return  numOfReconnections;
}
TQErrCode CQSingleContainer::TagReadFirstUsageTime(TAG_TYPE__FIRST_USAGE_TIME& first_usage_time)
{
	TQErrCode error ;
	m_MutexDatabase.WaitFor();
	error = m_TagAccess->TagReadFirstUsageTime(first_usage_time);
	m_MutexDatabase.Release();
	return error;
}
TQErrCode CQSingleContainer::TagWriteFirstUsageTime(TAG_TYPE__FIRST_USAGE_TIME& first_usage_time)
{
	TQErrCode error ;
	m_MutexDatabase.WaitFor();
	error = m_TagAccess->TagWriteFirstUsageTime(first_usage_time);
	m_MutexDatabase.Release();
	return error;
}
void CQSingleContainer::SetTankWeight(float CurrentWeightA2D)
{
	m_MutexWeights.WaitFor();

	// Updating the A2D Value
	m_CurrentWeightInA2D = CurrentWeightA2D;

	// Calculating the weight according to the A2D Value, the gain & the offset
	int CurrWeight = CurrentWeightA2D * m_Gain + m_Offset - m_EmptyContainerWeight;
	if (CurrWeight < m_CurrentWeightInGram)
	{
		m_CurrentWeightInGram = CurrWeight + WEIGHT_QUANTIZATION_UNITS;
		m_CurrentWeightInGram -= m_CurrentWeightInGram % WEIGHT_QUANTIZATION_UNITS;
	}
	else
		m_CurrentWeightInGram = CurrWeight - CurrWeight % WEIGHT_QUANTIZATION_UNITS;

	m_MutexWeights.Release();

	// If there is more than 40 gr difference than last 'Monitor' update
	m_NeedToShowWeight = (abs(m_PrevWeightInGram - m_CurrentWeightInGram) > WEIGHT_QUANTIZATION_UNITS);
	if (m_NeedToShowWeight)
		m_PrevWeightInGram = m_CurrentWeightInGram;

}

float CQSingleContainer::GetTankWeightInA2D()
{
	float WeightInA2D;

	m_MutexWeights.WaitFor();
	WeightInA2D = m_CurrentWeightInA2D;
	m_MutexWeights.Release();

	return WeightInA2D;
}

int CQSingleContainer::GetTankWeightInGram(bool ForceRealValue/* = false by default */)
{
	int WeightInGr;
	//objet_machine test
	if (FindWindow(0, QFormatStr( "Tank%dEmpty.txt - Notepad", (int)m_TankIndex ).c_str() ) )
		return 0;

    m_MutexWeights.WaitFor();
	WeightInGr = m_CurrentWeightInGram;
	m_MutexWeights.Release();

	if (ForceRealValue == false)
	{
		// In the case Tank is out, or we have "Disabled Tanks" (eg: E250, E350, E260) return 'zero'.
		if ((IsTankInserted() == false) || (GetMaterialType() == "Disabled-Tank") || (WeightInGr < 0))
			return 0;
		//  CQLog::Write(LOG_TAG_OBJET_MACHINE,"GetTankWeightInGram() IsTankInserted: %d, GetMaterialType: %s",IsTankInserted(),GetMaterialType());
	}
	
	return WeightInGr;
}



void CQSingleContainer::SetTankGain(float Gain)
{
	m_MutexGainOffset.WaitFor();
	m_Gain = Gain;
	m_MutexGainOffset.Release();
}


void CQSingleContainer::SetTankOffset(float Offset)
{
	m_MutexGainOffset.WaitFor();
	m_Offset = Offset;
	m_MutexGainOffset.Release();
}


bool CQSingleContainer::IsTankEnabled()
{
	bool Enabled;

	m_MutexTankEnabled.WaitFor();
	Enabled = m_Enabled;
	m_MutexTankEnabled.Release();

	return Enabled;
}

void CQSingleContainer::SetTankEnabled(bool Enabled)
{
	m_MutexTankEnabled.WaitFor();
	m_Enabled = Enabled;
	m_MutexTankEnabled.Release();
}

bool CQSingleContainer::IsPrintWeightNecessary()
{
	return m_NeedToShowWeight;
}

unsigned __int64 CQSingleContainer::GetTagID()
{
	return m_TagID;
}

void CQSingleContainer::SetTagID(unsigned __int64 tagID)
{
	m_TagID = tagID;
}

bool CQSingleContainer::IsTagParamValid(TAG_PARAM_ID tag_param_id)
{
	bool ret = false;
	if (m_TagAccess)
		ret = m_TagAccess->TagIsParamValid(tag_param_id);
	return ret;
}
/*******************************************************************************
  Class CContainerBase implementation
*******************************************************************************/
// Constructor - create the thread in suspend mode
// ------------------------------------------------------
CContainerBase::CContainerBase(const QString& Name) : CQThread(true, Name)
{
	m_Waiting = false;

	INIT_METHOD(CContainerBase, IsActiveLiquidTankInserted);
	INIT_METHOD(CContainerBase, IsActiveLiquidTankEnabled);
	INIT_METHOD(CContainerBase, SetAllLiquidTanksEnabled);
	INIT_METHOD(CContainerBase, GetIfAllLiquidTankAreInserted);
	INIT_METHOD(CContainerBase, GetIfAllLiquidTankAreEnabled);
	INIT_METHOD(CContainerBase, GetLiquidTankInsertedStatus);
	INIT_METHOD(CContainerBase, GetLiquidTankInsertedStatusFiltered);
	INIT_METHOD(CContainerBase, IsWeightOk);
	INIT_METHOD(CContainerBase, GetWasteWeightStatusDuringPrinting);
	INIT_METHOD(CContainerBase, GetWasteWeightStatusBeforePrinting);

	INIT_METHOD(CContainerBase, SetContainerGain);
	INIT_METHOD(CContainerBase, SetContainerOffset);

	INIT_METHOD(CContainerBase, SetActiveContainersBeforePrinting);

	INIT_METHOD(CContainerBase, GetLiquidTanksWeight);

	INIT_METHOD(CContainerBase, GetWeight);
	INIT_METHOD(CContainerBase, GetRemainingWasteWeight);
	INIT_METHOD(CContainerBase, GetTankWeightInA2D);
	INIT_METHOD(CContainerBase, GetTankWeightInGram);

	INIT_METHOD(CContainerBase, GetActiveTankNum);
	INIT_METHOD(CContainerBase, GetModelResinType);
	INIT_METHOD(CContainerBase, GetSupportResinType);

	INIT_METHOD(CContainerBase, GetTotalWeight);
	INIT_METHOD(CContainerBase, RemountSingleTankStatus);
	INIT_METHOD(CContainerBase, WaitForIdentificationCompletion);

	INIT_METHOD(CContainerBase, ResetCounter);
	INIT_METHOD(CContainerBase, IncCounter);
	INIT_METHOD(CContainerBase, GetNumOfReconnections);
	INIT_METHOD(CContainerBase, IsMicroSwitchInserted);
	INIT_METHOD(CContainerBase, GetSiblingContainer);
	INIT_METHOD(CContainerBase, IsTankInAndEnabled);
	INIT_METHOD(CContainerBase, UpdateLiquidTankInserted);
	INIT_METHOD(CContainerBase, TagReadWriteFirstUsageTime);

	INIT_METHOD(CContainerBase, ActivateWaste);
	INIT_METHOD(CContainerBase, ActivateRollerAndPurgeWaste);
	INIT_METHOD(CContainerBase, IsWasteActive);

	m_OCBClient = COCBProtocolClient::Instance();
	m_ParamsMgr = CAppParams::Instance();

	int NumOfWasteTanks = (m_ParamsMgr->DualWasteEnabled)?2:1;
	m_lastWasteTank = TOTAL_NUMBER_OF_CONTAINERS+NumOfWasteTanks;

	if(!m_ParamsMgr->DualWasteEnabled) //change!!!! the parameter should indicate if waste is treated as tank or not
	{
		m_numOfTanks = TOTAL_NUMBER_OF_CONTAINERS;
		m_numOfChambers = NUMBER_OF_CHAMBERS;
	}
	else
	{
		m_numOfTanks = TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE;
		m_numOfChambers = NUMBER_OF_CHAMBERS_INCLUDING_WASTE;
	}

	// The correct and final description of the cells will be set here, because DualWasteEnabled is
	// checked inside TankToStr, but CAppParams is not available yet
	for (int i = 0; i < m_ParamsMgr->WeightSensorGainArray.Size(); i++)
		m_ParamsMgr->WeightSensorGainArray.SetCellDescription(i, TankToStr((TTankIndex)i));
	for (int i = 0; i < m_ParamsMgr->WeightSensorOffsetArray.Size(); i++)
		m_ParamsMgr->WeightSensorOffsetArray.SetCellDescription(i, TankToStr((TTankIndex)i));

	//Instance to error handler
	m_ErrorHandlerClient = CErrorHandler::Instance();

	CModesManager::Instance()->EnumerateMaterialModes();

	for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE_INCLUDING_WASTE; i++)
	{
		m_ChambersArray[i].SetType(static_cast<TChamberIndex>(i));
		m_ChambersArray[i].SetActiveTank(static_cast<TTankIndex>(m_ParamsMgr->ActiveTanks[i].Value()));
	}

	for (int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; i++)
	{
		int rdr_num     = 0;
		int channel_num = 0;

// todo -oShahar.Behagen@objet.com -cNone:  add this name to actual c'tor)     m_TanksArray[i] = new CQSingleContainer(QReplaceSubStr(QReplaceSubStr(TankToStr((TTankIndex)i), " ", ""),"-",""), m_ParamsMgr->CartridgeEmptyWeight);
//               add:  const QString& Name,
		GetReaderNumByContainer((TTankIndex)i, rdr_num, channel_num);
		m_TanksArray[i] = new CQSingleContainer(m_ParamsMgr->CartridgeFullWeight,
												m_ParamsMgr->CartridgeEmptyWeight,
												static_cast<TTankIndex>(i),
												rdr_num,
												channel_num);
		m_TanksArray[i]->SetTankGain   (m_ParamsMgr->WeightSensorGainArray[i]);
		m_TanksArray[i]->SetTankOffset  (m_ParamsMgr->WeightSensorOffsetArray[i]);
		m_TanksArray[i]->SetMaterialType(m_ParamsMgr->TypesArrayPerPipe[i]);
		m_TanksArray[i]->SetTankMaterialType(m_ParamsMgr->TypesArrayPerTank[i]);
		m_TanksArray[i]->m_IsDrainNeeded = true;
		m_TanksArray[i]->m_IsDrainActive = false;
		m_TanksArray[i]->SetContainerPtr(this);
	}

	for(int i = FIRST_WASTE_TANK_TYPE; i < m_lastWasteTank; i++)
	{
		// todo -oShahar.Behagen@objet.com -cNone: add this name to actual c'tor) m_TanksArray[TYPE_TANK_WASTE] = new CQSingleContainer( QReplaceSubStr(TankToStr(TYPE_TANK_WASTE), " ", "")
		m_TanksArray[i] = new CQSingleContainer(0 // was: m_ParamsMgr->WasteFullContainerWeight. todo: remove it completely.
												, m_ParamsMgr->WasteCartridgeEmptyWeight
												, static_cast<TTankIndex>(i)
												, -1  // a fake reader num.
												, -1); // a fake channel num.

		m_TanksArray[i]->SetTankEnabled(true);
		m_TanksArray[i]->SetTankGain(m_ParamsMgr->WeightSensorGainArray[i]);
		m_TanksArray[i]->SetTankOffset(m_ParamsMgr->WeightSensorOffsetArray[i]);

		if(!m_ParamsMgr->DualWasteEnabled) //change!!!! the parameter should indicate if waste is treated as tank or not
		{
			m_ChambersArray[TYPE_CHAMBER_WASTE].AddTank(m_TanksArray[i], static_cast<TTankIndex>(i));
		}
	}


	m_WasteIsFull    = false;
	m_WasteWasActive = false;
	m_batchNoTable = CBatchNoTable::GetInstance();
}

void CContainerBase::Init()
{}  // Don't need the UpdateTanksStatus() here, it is updated through RFID.

// Destructor
// -------------------------------------------------------
CContainerBase::~CContainerBase(void)
{
	for (int i = 0; i < m_lastWasteTank; i++)
	{
		if (m_TanksArray[i])
			delete m_TanksArray[i];
	}
}


// Thread execute function (override)
void CContainer::Execute(void)
{
	TContainerIdentifyNotificationMessage Message;
	memset (&Message, 0, sizeof(TContainerIdentifyNotificationMessage));

	Message.MessageBody.EventType = TIMER;
    try {
	do
	{
        m_ForceStateMachineStep.WaitFor(STATE_MACHINE_TIMER_MSG_INTERVAL);

        Message.MessageBody.TimeStamp = QGetTicks();

        for(int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; i++)
        {
            m_ForceStateMachineStep.WaitFor(STATE_MACHINE_TIMER_MSG_INTERVAL);
            Message.TankIndex = (TTankIndex)i;
            m_ContainerDispatcher->SendNotification(Message);

            if (Terminated)
            {
                break;
            }
        }
	}
	while(!Terminated);
    } catch (...)
    {
        CQLog::Write(LOG_TAG_HOST_COMM,"Unexpected error during CHostComm::Execute()");
        if (!Terminated) throw EQException("CHostComm::Execute - unexpected error");
    }

}

bool CContainerBase::IsWasteTank(TTankIndex Tank)
{
	if(! VALIDATE_TANK_INCLUDING_WASTE(Tank))
		throw EContainer("'IsWasteTank' Error: invalid parameter");
	if(Tank>=FIRST_WASTE_TANK_TYPE && Tank<LAST_TANK_INCLUDING_WASTE_TYPE) //OBJET_MACHINE_KESHET, verify this line
		return true;
	
	return false;
}

bool CContainerBase::IsWasteActive()
{
	CActuatorBase *Actuators = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetActuatorInstance();
	if(m_ParamsMgr->DualWasteEnabled)
		return (Actuators->GetActuatorOnOff(ACTUATOR_ID_WASTE_PUMP_LEFT) || Actuators->GetActuatorOnOff(ACTUATOR_ID_WASTE_PUMP_RIGHT));
	else
		return Actuators->GetActuatorOnOff(ACTUATOR_ID_WASTE_PUMP);
}

TQErrCode CContainerBase::ActivateRollerAndPurgeWaste(bool Activate)
{
	CActuatorBase     *Actuators = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetActuatorInstance();
	CBackEndInterface *BackEnd   = CBackEndInterface::Instance();
	Actuators->SetOnOff(ACTUATOR_ID_ROLLER_PUMP, Activate);
	BackEnd->EnableDisableMaintenanceCounter(ROLLER_DIRT_PUMP_COUNTER_ID, Activate);
	BackEnd->EnableDisableMaintenanceCounter(ROLLER_PUMP_TUBES_COUNTER_ID, Activate);
	return ActivateWaste(Activate);
}

TQErrCode CContainerBase::ActivateWaste(bool Activate)
{
	CBackEndInterface *BackEnd = CBackEndInterface::Instance();

	// Throw exceptions only when trying to turn the waste ON
	if(Activate)
	{
		if(IsWasteFull())
		{
			if(m_ParamsMgr->DualWasteEnabled == TRUE)
				CQLog::Write(LOG_TAG_GENERAL, "Both waste cartridges are full. Empty before continuing.");
 //				throw EContainer("Both waste cartridges are full. Empty before continuing.");

			else
			//	throw EContainer("Waste cartridge is full. Empty before continuing.");
			CQLog::Write(LOG_TAG_GENERAL,"Waste cartridge is full. Empty before continuing.");
		}
		if(! IsActiveLiquidTankInserted(TYPE_CHAMBER_WASTE))
			{
			if(m_ParamsMgr->DualWasteEnabled == TRUE)
	 //			throw EContainer("Both waste cartridges are removed. Insert before continuing.");
			 CQLog::Write(LOG_TAG_GENERAL,"Both waste cartridges are removed. Insert before continuing.");
			 else
	  //          throw EContainer("Waste cartridge is removed. Insert before continuing.");
				  CQLog::Write(LOG_TAG_GENERAL,"Waste cartridge is removed. Insert before continuing.");
			}
	}

	BackEnd->EnableDisableMaintenanceCounter(DIRT_PUMPS_TUBES_COUNTER_ID, Activate);

	//OBJET_MACHINE_KESHET, verify this change
   /*	TTankIndex tankIndex = m_ChambersArray[TYPE_CHAMBER_WASTE].GetActiveTank();
	BackEnd->SetActuatorState(PumpsActuatorID[tankIndex], Activate);
	BackEnd->EnableDisableMaintenanceCounter(PumpsCounterID[tankIndex], Activate);
	*/
	BackEnd->SetActuatorState(ACTUATOR_ID_WASTE_PUMP, Activate);
	BackEnd->EnableDisableMaintenanceCounter(ACTUATOR_ID_WASTE_PUMP, Activate);
	return Q_NO_ERROR;
}

bool CContainerBase::IsChambersTank(TChamberIndex Chamber, TTankIndex Tank)
{
	if (!VALIDATE_CHAMBER_INCLUDING_WASTE(Chamber) ||
			!VALIDATE_TANK_INCLUDING_WASTE(Tank))
		throw EContainer("'IsChambersTank' Error: invalid parameter");
	return m_ChambersArray[Chamber].IsChambersTank(Tank);
}//IsChambersTank

// In case there is 2nd tank and it's ready (not empty and inserted) set it as active tank.
TQErrCode CContainerBase::SwitchToNewTank(TChamberIndex Chamber)
{
	if (!VALIDATE_CHAMBER_INCLUDING_WASTE(Chamber))
		throw EContainer("'SwitchToNewTank' Error: invalid parameter");
	//decide which tank should be the active tank (decided upon its weight)
	TTankIndex Tank = m_ChambersArray[Chamber].GetTankToActivate();
	if (NO_TANK == Tank)
		return Q2RT_NO_AVAILABLE_LIQUID_TANK;

	TTankIndexArray ActiveTanks;
	for (int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
		ActiveTanks[i] = m_ChambersArray[i].GetActiveTank();
	ActiveTanks[Chamber] = Tank;

// todo -oShahar.Behagen@objet.com -cNone: see if this SwitchToNewTank() function could be dumped completly. if not it should look at IsTankEnabled()
	SetActiveTank(ActiveTanks);

	return Q_NO_ERROR;
}//SwitchToNewTank

TTankIndex CContainerBase::GetActiveTankNum(int /* TChamberIndex */ Chamber)
{
	if (!VALIDATE_CHAMBER_INCLUDING_WASTE(Chamber))
		return NO_TANK;

	return m_ChambersArray[Chamber].GetActiveTank();
}//GetActiveTank

void CContainerBase::SetActiveDrainTank(TChamberIndex Chamber, TTankIndex DrainTank)
{
	if (!VALIDATE_CHAMBER(Chamber))
		throw EContainer("'SetActiveDrainTank' Error: invalid parameter");
	return m_ChambersArray[Chamber].SetActiveDrainTank(DrainTank);
}//SetActiveDrainTank

TTankIndex CContainerBase::GetActiveDrainTank(TChamberIndex Chamber)
{
	if (!VALIDATE_CHAMBER(Chamber))
		throw EContainer("'GetActiveDrainTank' Error: invalid parameter");
	return m_ChambersArray[Chamber].GetActiveDrainTank();
}//GetActiveDrainTank

TQErrCode CContainerBase::SetActiveTank(TTankIndexArray& TankArray, bool PrintToLog)
{
	QString Output = "'SetActiveTank' ";

	for(int i = FIRST_CHAMBER_TYPE; i < m_numOfChambers; ++i)
	{
			if (m_ChambersArray[i].GetActiveTank() != TankArray[i])
			{
				m_ChambersArray[i].SetActiveTank(TankArray[i]);

				FrontEndInterface->UpdateStatus(FE_SET_ACTIVE_TANK_BASE + TankArray[i], i, true);
				//Remove the previous active tank from GUI DataBase
				FrontEndInterface->RemoveStatus(FE_SET_ACTIVE_TANK_BASE + GetSiblingTank(TankArray[i]));
				Output = Output + ChamberToStr(static_cast<TChamberIndex>(i)) + " Active Tank = " + TankToStr(TankArray[i]) + "; ";
				// Updating the Parameter Manager
				m_ParamsMgr->ActiveTanks[i] = TankArray[i];
			}
	}
	m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->ActiveTanks);
	// CQLog::Write(LOG_TAG_HEAD_FILLING,"SetActiveTank,Active Tanks: %d %d %d %d",m_ParamsMgr->ActiveTanks[0].Value(),m_ParamsMgr->ActiveTanks[1].Value(),m_ParamsMgr->ActiveTanks[2].Value(),m_ParamsMgr->ActiveTanks[3].Value());

	if (PrintToLog)
		CQLog::Write(LOG_TAG_EOL, Output);

	return Q_NO_ERROR;
}
//Set all active tanks batch numbers when printing starts
void CContainerBase::SetActiveTanksBatchNo()
{
	TTankIndex activeTank;
	for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
	{
		{
			activeTank = GetActiveTankNum((TChamberIndex)i);
			m_batchNoTable->SetTankResinType(activeTank, GetTankMaterialType(activeTank));
			m_batchNoTable->SetTankBatchNo(activeTank, m_TanksArray[activeTank]->GetBatchNo());
		}
	}
}
/*******************************************************************************
// Class CContainer implementation
*******************************************************************************/
CContainer::CContainer(const QString& Name) : CContainerBase(Name)
{
	m_FlagLiquidTank      = false;

	//Flag ack ok
	LiquidTank_AckOk      = false;
	m_FlagGetStatusUnderUse = false;

	m_WasteAlert         = false;
	m_AllLiquidTankEnabled = false;

	m_OCBClient->InstallMessageHandler(OCB_LIQUID_TANK_STATUS_CHANGED,
									   &NotificationLiquidTankStatusChanged,
									   reinterpret_cast<TGenericCockie>(this));


	/////////////////////////////////
	// Create the SingleContainers //
	/////////////////////////////////
	for (int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; i++)
		m_TanksArray[i]->InitStateMachine();


	////////////////////////////////
	// Create the list of materials
	////////////////////////////////

	m_ContainerDispatcher = new CContainersDispatcher;
	m_ContainerDispatcher->SetContainerPtr(this);
	// Init the RFID.
#ifdef DONT_USING_RFID
	//if(!m_ParamsMgr->USING_RFID)
	//{
	CQLog::Write(LOG_TAG_RFID, "Working in non-RFID mode.");
	// Set a small value to "RF WAITs" relevant timeouts of state machine:
	RF_WAITING_TIMEOUT = 0;
	//}
	//else
	//{
#else
	CRFID::Init();
	CRFIDBase* RFIDInstance = CRFID::Instance();
	RFIDInstance->SetRFTagsChangeCallBack(CContainer::RFTagsChangeCallBack, reinterpret_cast<TGenericCockie>(this));
#endif
	//}


	// Note: We still want the state machine to function, with Timer and Micro-switch events only.
	m_ContainerDispatcher->Resume();
	this->Resume();

	Init();

}//Constructor

TQErrCode CContainerBase::UpdateWeightAfterWizard(TTankIndex tankIndex)
{
	TQErrCode err = Q_NO_ERROR;
	if(tankIndex == ALL_TANKS)
	{
		for (int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; i++)
		{
			if(m_TanksArray[i]->IsTankInserted())
			{
				err += m_TanksArray[i]->UpdateWeightAfterWizard();
				QSleep(10);
			}
		}
	}
	else
		err = m_TanksArray[tankIndex]->UpdateWeightAfterWizard();
	return err;
}
/*
TQErrCode CContainerBase::UpdateActiveThermistors()
{
	CHeadFillingBase* HeadFillingInstance = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadFillingInstance();
	if(!HeadFillingInstance)
	{
		CQLog::Write(LOG_TAG_OBJET_MACHINE, "HeadFillingInstance Memory allocation failed");
		return Q_FAIL_ACTION; //allocation failed
	}

	return HeadFillingInstance->UpdateActiveThermistors();
}*/
TQErrCode CContainerBase::SetActiveThermistorsAccordingToParamMngr()
{
	CHeadFillingBase* HeadFillingInstance = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadFillingInstance();
	if(!HeadFillingInstance)
	{
		CQLog::Write(LOG_TAG_OBJET_MACHINE, "HeadFillingInstance Memory allocation failed");
		return Q_FAIL_ACTION; //allocation failed
	}

	return HeadFillingInstance->SetActiveThermistorsAccordingToParamMngr();
}

/* IMPORTANT: MRW / HSW / SDW must call this method if the PM's parameters are changed!
	See more details in header. */
void CContainerBase::UpdateChambers(bool UpdateActiveTherms)
{
	TTankIndexArray ActiveTanks;

// if(UpdateActiveTherms)
//	  UpdateActiveThermistors();
//  else
	SetActiveThermistorsAccordingToParamMngr();

	for (int c = FIRST_CHAMBER_TYPE; c < m_numOfChambers; c++)
	{
		m_ChambersArray[c].ClearTanks();
		// Create a list of Tanks related to each Chamber.
		for (int t = FIRST_TANK_TYPE; t < m_lastWasteTank; t++)
		{
			if (m_ParamsMgr->ChamberTankRelation[c + t * NUMBER_OF_CHAMBERS_INCLUDING_WASTE] == true)
			{
				m_ChambersArray[c].AddTank(m_TanksArray[t], (TTankIndex)t);
				ActiveTanks[c] = (TTankIndex)t; // if only one tank is associated with a certain chamber, then we're in DM mode, and we will always want this tank "active". (Because in DM both Tank should show active)
			}
		}

		//[@Assumption] Chambers with a single tank (e.g. Waste) cannot be flooded (i.e. become inactive)
		if (m_ChambersArray[c].GetTanksCount() > 1)
		{
			ActiveTanks[c] = m_ChambersArray[c].GetTankToActivate();
			if (ActiveTanks[c] == NO_TANK)
			{
				/* If no tank is available then either the operation-mode has changed
				and the flooding chamber's tanks aren't ready, or the current chamber's tanks aren't.
				  If the chamber is flooded, select the tank of the sibling (flooding) chamber.
				  Else - reassign an arbitrary tank that is statically related to the chamber
				*/
				if (! IsChamberFlooded(static_cast<TChamberIndex>(c)))
					ActiveTanks[c] = GetSpecificTank(static_cast<TChamberIndex>(c), 1);
			}
		}
	}

	//Second part of fix: If a chamber is flooded, select the flooding chamber's active tank
	for (int c = FIRST_CHAMBER_TYPE; c < m_numOfChambers; c++)
		if (NO_TANK == ActiveTanks[c])
		{
			ActiveTanks[c] = static_cast<TTankIndex>(ActiveTanks[GetSiblingChamber(static_cast<TChamberIndex>(c))]);
			/* do NOT use m_ParamsMgr->ActiveTanks here, so that a write-write race won't
			   occur and 2 different tanks would be active for the same chamber.
			   In the above mentioned way, ActiveTanks might need to be updated,
			   yet it is consistent by itself */
		}

	SetActiveTank(ActiveTanks);

// CQLog::Write(LOG_TAG_HEAD_FILLING,"UpdateChambers after updatestatus,Active Thermistors: S:%d M1:%d M2:%d M3:%d S_M3:%d M1_M2:%d",
//  m_ParamsMgr->ActiveThermistors[0].Value(),m_ParamsMgr->ActiveThermistors[1].Value(),m_ParamsMgr->ActiveThermistors[2].Value(),m_ParamsMgr->ActiveThermistors[3].Value(),m_ParamsMgr->ActiveThermistors[4].Value(),m_ParamsMgr->ActiveThermistors[5].Value());
}

// Destructor
// -------------------------------------------------------
CContainer::~CContainer(void)
{
	try
	{
		CRFID::DeInit();
	}
	catch(EQException& Exception)
	{
		QMonitor.ErrorMessage(Exception.GetErrorMsg());
		CQLog::Write(LOG_TAG_RFID, Exception.GetErrorMsg() + " Unable to de-initialize RFID.");
	}

	// Mark terminate
	Terminate();

	// Wait for thread termination
	WaitFor();

	if (m_ContainerDispatcher)
		delete m_ContainerDispatcher;

	CRFID::DeInit();
}//destructor

// This procedure set the Active Pumps
//-------------------------------------------
TQErrCode CContainer::SetActiveTank(TTankIndexArray& TankArray, bool PrintToLog)
{
	//If an exception arises from within this sensitive critical section,
	//the mutex won't be released unless it's on stack	
	CQMutexHolder MutexHolder(&SetActiveTankMutex);

	// Build the Head Fillings set Active Pumps Message
	TOCBSetChamberTankMsg SendMsg;
	memset(&SendMsg, 0, sizeof(TOCBSetChamberTankMsg));

	SendMsg.MessageID   = OCB_SET_CHAMBERS_TANK;
	for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
		SendMsg.TankID[i] = TankArray[i];

	// Send a Turn ON request
	if (m_OCBClient->SendInstallWaitReply(&SendMsg, sizeof(TOCBSetChamberTankMsg),
										  SetActiveTankAckResponse,
										  reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
	{
		CQLog::Write(LOG_TAG_HEAD_FILLING, "OCB didn't get ack for \"SetActiveTank\" message");
		throw EContainer("OCB didn't get ack for \"SetActiveTank\" message");
	}
	CContainerBase::SetActiveTank(TankArray, PrintToLog);

	return (Q_NO_ERROR);
}//SetActiveTank

// Acknowledges for Command Set Parms
// ---------------------------------
void CContainer::SetActiveTankAckResponse(int TransactionId, PVOID Data,
		unsigned DataLength, TGenericCockie Cockie)
{
	// Build the Head Fillings set params message
	TOCBAck* ResponseMsg = static_cast<TOCBAck*>(Data);

	// Verify size of message
	if(DataLength != sizeof(TOCBAck))
	{
		FrontEndInterface->NotificationMessage("Containers \"SetActiveTankAckResponse\" length error");
		CQLog::Write(LOG_TAG_HEAD_FILLING, "Containers \"SetActiveTankAckResponse\" length error");
		return;
	}

	// Update DataBase before Open the Semaphore/Set event.
	if (static_cast<int>(ResponseMsg->MessageID) != OCB_ACK)
	{
		FrontEndInterface->NotificationMessage("Containers \"SetActiveTankAckResponse\" message id error");
		CQLog::Write(LOG_TAG_HEAD_FILLING, "Containers \"SetActiveTankAckResponse\" message id error (0x%X)",
					 (int)ResponseMsg->MessageID);
		return;
	}

	if (static_cast<int>(ResponseMsg->RespondedMessageID) != OCB_SET_CHAMBERS_TANK)
	{
		FrontEndInterface->NotificationMessage("Containers \"SetActiveTankAckResponse\" responded message id error");
		CQLog::Write(LOG_TAG_HEAD_FILLING, "Containers \"SetActiveTankAckResponse\" responded message id error (0x%X)",
					 (int)ResponseMsg->RespondedMessageID);
		return;
	}

	if (ResponseMsg->AckStatus)
	{
		FrontEndInterface->NotificationMessage("Containers \"SetActiveTankAckResponse\" ack status error");
		CQLog::Write(LOG_TAG_HEAD_FILLING, "Containers \"SetActiveTankAckResponse\" ack status error (%d)",
					 (int)ResponseMsg->AckStatus);
		return;
	}
}//SetActiveTankAckResponse

TQErrCode CContainer::SetDrainTanks(TTankIndex* DrainPumps, bool PrintToLog)
{
	// Build the Head Fillings set Drain Pumps Message
	TOCBSetDrainPumpsMsg SendMsg;
	memset(&SendMsg, 0, sizeof(TOCBSetDrainPumpsMsg));

	SetDrainTankMutex.WaitFor();
	SendMsg.MessageID   = OCB_SET_HEAD_FILLING_DRAIN_PUMPS;
	PrepareSetDrainPumpsMessage(&SendMsg, DrainPumps, PrintToLog);

	// Send a Turn ON request
	if (m_OCBClient->SendInstallWaitReply(&SendMsg, sizeof(TOCBSetDrainPumpsMsg),
										  SetDrainTankAckResponse,
										  reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
	{
		CQLog::Write(LOG_TAG_HEAD_FILLING, "OCB didn't get ack for \"TOCBSetDrainPumpsMsg\" message");
		throw EContainer("OCB didn't get ack for \"TOCBSetDrainPumpsMsg\" message");
	}
	SetDrainTankMutex.Release();

	return Q_NO_ERROR;
}//SetDrainTanks

void CContainer::SetDrainTankAckResponse(int TransactionId, PVOID Data,
		unsigned DataLength, TGenericCockie Cockie)
{
	// Build the Head Fillings set params message
	TOCBAck *ResponseMsg = static_cast<TOCBAck *>(Data);

	//Verify size of message
	if(DataLength != sizeof(TOCBAck))
	{
		FrontEndInterface->NotificationMessage("Containers \"SetDrainTankAckResponse\" length error");
		CQLog::Write(LOG_TAG_HEAD_FILLING, "Containers \"SetDrainTankAckResponse\" length error");
		return;
	}

	// Update DataBase before Open the Semaphore/Set event.
	if (static_cast<int>(ResponseMsg->MessageID) != OCB_ACK)
	{
		FrontEndInterface->NotificationMessage("Containers \"SetDrainTankAckResponse\" message id error");
		CQLog::Write(LOG_TAG_HEAD_FILLING, "Containers \"SetDrainTankAckResponse\" message id error (0x%X)",
					 (int)ResponseMsg->MessageID);
		return;
	}

	if (static_cast<int>(ResponseMsg->RespondedMessageID) != OCB_SET_HEAD_FILLING_DRAIN_PUMPS)
	{
		FrontEndInterface->NotificationMessage("Containers \"SetDrainTankAckResponse\" responded message id error");
		CQLog::Write(LOG_TAG_HEAD_FILLING, "Containers \"SetDrainTankAckResponse\" responded message id error (0x%X)",
					 (int)ResponseMsg->RespondedMessageID);
		return;
	}

	if (ResponseMsg->AckStatus)
	{
		FrontEndInterface->NotificationMessage("Containers \"SetDrainTankAckResponse\" ack status error");
		CQLog::Write(LOG_TAG_HEAD_FILLING, "Containers \"SetDrainTankAckResponse\" ack status error (%d)",
					 (int)ResponseMsg->AckStatus);
		return;
	}
}//

//This procedure get if the Model and Support Tank is inserted.
bool CContainerBase::IsActiveLiquidTankInserted (int Chamber)
{
	if (!VALIDATE_CHAMBER_INCLUDING_WASTE(Chamber))
		throw EContainer("'IsActiveLiquidTankInserted' Error: invalid MaterialType");

	return AreRelevantTanksInserted(static_cast<TChamberIndex>(Chamber), ACTIVE_TANK);
}//IsActiveLiquidTankInserted

//This procedure get if the Model and Support Tank is inserted.
bool CContainerBase::IsActiveLiquidTankEnabled (int /* TChamberIndex */ Chamber)
{
	if (!VALIDATE_CHAMBER_INCLUDING_WASTE(Chamber))
		throw EContainer("'IsActiveLiquidTankEnabled' Error: invalid MaterialType");

	return m_ChambersArray[Chamber].IsActiveLiquidTankEnabled();
}//IsActiveLiquidTankEnabled

bool CContainerBase::SetAllLiquidTanksEnabled(bool Enabled)
{
	for (int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; i++)
	{
		m_TanksArray[i]->SetTankEnabled(Enabled);
	}

	return true;
}
bool CContainerBase::ResetCounter(int Tank)
{
	m_TanksArray[Tank]->ResetCounter();
	return true; // To make compiler happy
}

bool CContainerBase::IncCounter(int Tank)
{
	m_TanksArray[Tank]->IncCounter();
	return true; // To make compiler happy
}

int CContainerBase::GetNumOfReconnections(int Tank)
{
	return m_TanksArray[Tank]->GetNumOfReconnections();
}

TQErrCode CContainerBase::TagReadWriteFirstUsageTime(int Tank)
{
	TAG_TYPE__FIRST_USAGE_TIME value = 0;
	TQActionErrCode errorCode = Q_NO_ERROR;
	if(m_TanksArray[Tank]->TagReadFirstUsageTime(value) == Q_NO_ERROR)
	{
		CQLog::Write(LOG_TAG_GENERAL, "Read FirstUsageTime = %d " , (int)value);
		TAG_TYPE__FIRST_USAGE_TIME tmpValue = 0;
		value += 1234;
		if(m_TanksArray[Tank]->TagWriteFirstUsageTime(value) == Q_NO_ERROR)
		{
			if(m_TanksArray[Tank]->TagReadFirstUsageTime(tmpValue) == Q_NO_ERROR)
			{
				if(value != tmpValue)
					errorCode = Q_CRITICAL_FAIL_ACTION;
			}
			else
				errorCode = Q_FAIL_ACTION;
		}
		else
			errorCode = Q_FAIL_ACTION;
	}
	else
		errorCode = Q_FAIL_ACTION;

	return errorCode;
}
bool CContainerDummy::IsActiveLiquidTankEnabled (int /* TChamberIndex */ Chamber)
{
#ifdef _DEBUG
	if (FindWindow(0, QFormatStr( "Chamber%dDisable.txt - Notepad", (int)Chamber ).c_str() ) )
		return false;
#endif
	return CContainerBase::IsActiveLiquidTankEnabled(Chamber);
}

// This method implements "In Place" functionality
TQErrCode CContainerBase::UpdateLiquidTankInserted(/*TTankIndex*/ int  Tank, bool Inserted)
{
	if (true == m_TanksArray[Tank]->GetIgnoreTankInsertionAndRemovalSignalsState())
		return Q_NO_ERROR;

	if (!VALIDATE_TANK_INCLUDING_WASTE(Tank))
		throw EContainer("'UpdateLiquidTankInserted' Error: invalid param");

	// If Tank insertion status has changed, notify. (But not for WASTE, which doesn't participate in the RFID party)
	if (! IsWasteTank(static_cast<TTankIndex>(Tank)))
	{
		if (m_TanksArray[Tank]->IsMicroswitchInserted() != Inserted)
		{
			NotifyMSEvent(static_cast<TTankIndex>(Tank), Inserted);
			m_TanksArray[Tank]->SetMicroswitchInserted(Inserted);
#ifdef DONT_USING_RFID  //ask Shahar
//if(!m_ParamsMgr->USING_RFID)
//{
			m_TanksArray[Tank]->SetTankInserted(Inserted);
			FrontEndInterface->UpdateStatus(m_TanksArray[Tank]->GetFEExistenceControlID(), static_cast<int>(Inserted));
//}
#endif
		}
	}
	else // For the Waste tank
	{
		m_TanksArray[Tank]->SetTankInserted(Inserted);
		if(m_ParamsMgr->DualWasteEnabled)
		{
			m_TanksArray[Tank]->SetMicroswitchInserted(Inserted);
			FrontEndInterface->UpdateStatus(m_TanksArray[Tank]->GetFEExistenceControlID(), static_cast<int>(Inserted));
                        TMachineState machineState = Q2RTApplication->GetMachineManager()->GetCurrentState();
                        if((Inserted)&&
                           (( machineState == msPrePrint)||( machineState == msPrinting)))
                       	{
                           // Bug 1285                                            	
                            SetWasteWasActive(true); 
                        }
		}
	}

	return Q_NO_ERROR;
}

//OBJET_MACHINE rf
void CContainer::RFTagsChangeCallBack(TGenericCockie Cockie, CRFReader* the_reader, int rdr_num, int channel_num, TTagsLastChange tags_condition, unsigned __int64 tag_id)
{
	CContainer *InstancePtr = reinterpret_cast<CContainer *>(Cockie);

	TContainerIdentifyNotificationMessage Message;

	memset (&Message, 0, sizeof(TContainerIdentifyNotificationMessage));

	// Set message params for Message

	// Set the Message.MaterialType field:
	switch (rdr_num)
	{
		case RFRDR1:
			switch (channel_num)
			{
				case MODEL_CONTAINER1_RF_CHANNEL:
					Message.TankIndex = TYPE_TANK_MODEL1;
					break;
				case SUPPOR_CONTAINER1_RF_CHANNEL:
					Message.TankIndex = TYPE_TANK_SUPPORT1;
					break;
				case SUPPOR_CONTAINER2_RF_CHANNEL:
					Message.TankIndex = TYPE_TANK_SUPPORT2;
					break;

				case MODEL_CONTAINER2_RF_CHANNEL:
					Message.TankIndex = TYPE_TANK_MODEL2;
					break;
				default:
					throw EContainer("'RFTagsChangeCallBack' Error: RF Channel number out of range.");
			}
			break;
		case RFRDR2:
			switch (channel_num)
			{
				case MODEL_CONTAINER3_RF_CHANNEL:
					Message.TankIndex = TYPE_TANK_MODEL3;
					break;
				case MODEL_CONTAINER4_RF_CHANNEL:
					Message.TankIndex = TYPE_TANK_MODEL4;
					break;
				case MODEL_CONTAINER5_RF_CHANNEL:
					Message.TankIndex = TYPE_TANK_MODEL5;
					break;
				case MODEL_CONTAINER6_RF_CHANNEL:
					Message.TankIndex = TYPE_TANK_MODEL6;
					break;
				default:
					throw EContainer("'RFTagsChangeCallBack' Error: RF Channel number out of range.");
			}
			break;
#ifdef OBJET_MACHINE_KESHET
		case RFRDR3:
			switch (channel_num)
			{
				case MODEL_CONTAINER7_RF_CHANNEL:
					Message.TankIndex = TYPE_TANK_MODEL7;
					break;
				case MODEL_CONTAINER8_RF_CHANNEL:
					Message.TankIndex = TYPE_TANK_MODEL8;
					break;
				case MODEL_CONTAINER9_RF_CHANNEL:
					Message.TankIndex = TYPE_TANK_MODEL9;
					break;
				case MODEL_CONTAINER10_RF_CHANNEL:
					Message.TankIndex = TYPE_TANK_MODEL10;
					break;
				default:
					throw EContainer("'RFTagsChangeCallBack' Error: RF Channel number out of range.");
			}
			break;
		case RFRDR4:
			switch (channel_num)
			{
				case MODEL_CONTAINER11_RF_CHANNEL:
					Message.TankIndex = TYPE_TANK_MODEL11;
					break;
				case MODEL_CONTAINER12_RF_CHANNEL:
					Message.TankIndex = TYPE_TANK_MODEL12;
					break;
				case MODEL_CONTAINER13_RF_CHANNEL:
					Message.TankIndex = TYPE_TANK_MODEL13;
					break;
				case MODEL_CONTAINER14_RF_CHANNEL:
					Message.TankIndex = TYPE_TANK_MODEL14;
					break;
				default:
					throw EContainer("'RFTagsChangeCallBack' Error: RF Channel number out of range.");
			}
			break;
#endif			
		default:
			throw EContainer("'RFTagsChangeCallBack' Error: RF Reader number out of range.");
	}

	switch (tags_condition)
	{
		case TAGS_LAST_CHANGE_ADD:
			Message.MessageBody.EventType = RF_ADD;
			break;

		case TAGS_LAST_CHANGE_REMOVE:
			Message.MessageBody.EventType = RF_REMOVE;
			break;

		default:
			throw EContainer("'RFTagsChangeCallBack' Error: TagsCondition value out of range.");
	}

	Message.MessageBody.data_2    = tag_id;
	Message.MessageBody.TimeStamp = QGetTicks();

	InstancePtr->m_ContainerDispatcher->SendNotification(Message);
}
//--------------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// CContainer::NotifyMSEvent()
// This function is called upon a Tank-Insersion (microswitch-detected) event.

TQErrCode CContainer::NotifyMSEvent(TTankIndex TankIndex, bool InOut)
{
	TContainerIdentifyNotificationMessage Message;
	memset (&Message, 0, sizeof(TContainerIdentifyNotificationMessage));

	Message.MessageBody.TimeStamp = QGetTicks();

	// Set Message.MessageBody.EventType
	InOut == true ? (Message.MessageBody.EventType = MY_MS_IN) : (Message.MessageBody.EventType = MY_MS_OUT);

	Message.TankIndex = TankIndex;

	m_ContainerDispatcher->SendNotification(Message);

	return Q_NO_ERROR;
}
//--------------------------------------------------------------------------------------------
bool CContainer::SendNotification(TContainerIdentifyNotificationMessage &Message)
{
/* 	Don't process messages (stop comm.), if the thread is terminating.
	Comes to solve access violation during Container's d'tor, where the message queue is destroyed,
	yet Container and/or derived classes reply to a tank identification message. */
	if (Terminated)
		return false;

	m_TanksArray[Message.TankIndex]->SendNotification(Message.MessageBody);
	return true;
}

QString CContainer::GetTagIDAsString(int ContainerNum)
{
	// Get all currently available tags from RFID:
	int RDRnum      = 0;
	int Channelnum  = 0;
	int num_of_tags = 0;
	unsigned __int64 tagsList[MAX_TAGS];

	// Get RFID Instance:
	CRFIDBase* RFIDInstance = CRFID::Instance();

	// if RFID instance was never created (for example, rdr dll was not registered) then we assume working
	// without RFID. In this case, return "".
	if (!RFIDInstance)
		return "RFID DISABLED";

	// Get the relevant reader and channel numbers:
	//GetReaderNumByContainerNum(ContainerNum, RDRnum, Channelnum);
	GetReaderNumByContainer((TTankIndex)ContainerNum, RDRnum, Channelnum);
	// Get the last read tags list:
	RFIDInstance->GetTags( RDRnum
						   , Channelnum
						   , tagsList
						   , &num_of_tags);

	if (num_of_tags == 0)
		return "No Tag";

	return RFIDInstance->GetTagIDAsString(tagsList[0]);
}

QString CContainerBase::GetTankMaterialType(TTankIndex TankIndex)
{
	return m_TanksArray[TankIndex]->GetTankMaterialType();
}

QString CContainer::GetPipeMaterialType(TTankIndex TankIndex)
{
	return m_TanksArray[TankIndex]->GetMaterialType();
}

unsigned long CContainer::GetContainerExpirationDate(TTankIndex TankIndex)
{
	return m_TanksArray[TankIndex]->GetExpirationDate();
}

bool CContainer::IsResinExpired(TTankIndex TankIndex)
{
	return m_TanksArray[TankIndex]->IsResinExpiredDirect();
}

bool CContainer::IsMaxPumpTimeExceeded(TTankIndex TankIndex)
{
	return m_TanksArray[TankIndex]->IsMaxPumpTimeExceeded();
}

void CContainer::SetAllowServiceMaterials(TTankIndex TankIndex, bool val)
{
	return m_TanksArray[TankIndex]->SetAllowServiceMaterials(val);
}

void CContainer::SetSkipMachineResinTypeChecking(TTankIndex TankIndex, bool val)
{
	return m_TanksArray[TankIndex]->SetSkipMachineResinTypeChecking(val);
}

void CContainer::HandleTankIdentificationDlgClose(TTankIndex TankIndex, int Result)
{
	// Mark that the dialog associated with this SingleContainer is closed. (=acked)
	m_TanksArray[TankIndex]->SetDialogAcked(true);

	// If this is a callback from a "DISQUALIFIED type" dialog with only 'OK' button:
	if (Result == FE_TANK_ID_NOTICE_DLG_RESULT_OK)
	{
		return;
	}
	else

		// reset result flags:
		m_TanksArray[TankIndex]->SetForceUsingTank(false);
	m_TanksArray[TankIndex]->SetRunRRWizard(false);

	switch (Result)
	{
		case FE_TANK_ID_NOTICE_DLG_RESULT_DISABLE_TANK:
			// do nothing.
			break;

		case FE_TANK_ID_NOTICE_DLG_RESULT_ENABLE_TANK_SINGLEMODE:
			m_TanksArray[TankIndex]->SetForceUsingTank(true);
			m_TanksArray[TankIndex]->SetForceUsingMode(SINGLE_MATERIAL_OPERATION_MODE);
			break;

		case FE_TANK_ID_NOTICE_DLG_RESULT_ENABLE_TANK_DMMODE:
			m_TanksArray[TankIndex]->SetForceUsingTank(true);
			m_TanksArray[TankIndex]->SetForceUsingMode(DIGITAL_MATERIAL_OPERATION_MODE);
			break;

		case FE_TANK_ID_NOTICE_DLG_RESULT_ENABLE_TANK_NO_TAG:
			m_TanksArray[TankIndex]->SetForceUsingTank(true);
			break;

		case FE_TANK_ID_NOTICE_DLG_RESULT_RUN_RR_WIZARD:
			m_TanksArray[TankIndex]->SetRunRRWizard(true);
			break;
	}

	TanksStatusLogic(TankIndex);
}

//OBJET_MACHINE rf
TQErrCode CContainerBase::GetReaderNumByContainer(TTankIndex TankIndex, int &ReaderNum, int &ChannelNum)
{
	switch (TankIndex)
	{
		case TYPE_TANK_MODEL1:
			ReaderNum  = RFRDR1;
			ChannelNum = MODEL_CONTAINER1_RF_CHANNEL;
			break;
		case TYPE_TANK_MODEL2:
			ReaderNum  = RFRDR1;
			ChannelNum = MODEL_CONTAINER2_RF_CHANNEL;
			break;
		case TYPE_TANK_SUPPORT1:
			ReaderNum  = RFRDR1;
			ChannelNum = SUPPOR_CONTAINER1_RF_CHANNEL;
			break;
		case TYPE_TANK_SUPPORT2:
			ReaderNum  = RFRDR1;
			ChannelNum = SUPPOR_CONTAINER2_RF_CHANNEL;
			break;
		case TYPE_TANK_MODEL3:
			ReaderNum  = RFRDR2;
			ChannelNum = MODEL_CONTAINER3_RF_CHANNEL;
			break;
		case TYPE_TANK_MODEL4:
			ReaderNum  = RFRDR2;
			ChannelNum = MODEL_CONTAINER4_RF_CHANNEL;
			break;
		case TYPE_TANK_MODEL5:
			ReaderNum  = RFRDR2;
			ChannelNum = MODEL_CONTAINER5_RF_CHANNEL;
			break;

		case TYPE_TANK_MODEL6:
			ReaderNum  = RFRDR2;
			ChannelNum = MODEL_CONTAINER6_RF_CHANNEL;
			break;
#ifdef OBJET_MACHINE_KESHET
		case TYPE_TANK_MODEL7:
			ReaderNum  = RFRDR3;
			ChannelNum = MODEL_CONTAINER7_RF_CHANNEL;
			break;
		case TYPE_TANK_MODEL8:
			ReaderNum  = RFRDR3;
			ChannelNum = MODEL_CONTAINER8_RF_CHANNEL;
			break;
		case TYPE_TANK_MODEL9:
			ReaderNum  = RFRDR3;
			ChannelNum = MODEL_CONTAINER9_RF_CHANNEL;
			break;
		case TYPE_TANK_MODEL10:
			ReaderNum  = RFRDR3;
			ChannelNum = MODEL_CONTAINER10_RF_CHANNEL;
			break;

		case TYPE_TANK_MODEL11:
			ReaderNum  = RFRDR4;
			ChannelNum = MODEL_CONTAINER11_RF_CHANNEL;
			break;
		case TYPE_TANK_MODEL12:
			ReaderNum  = RFRDR4;
			ChannelNum = MODEL_CONTAINER12_RF_CHANNEL;
			break;
		case TYPE_TANK_MODEL13:
			ReaderNum  = RFRDR4;
			ChannelNum = MODEL_CONTAINER13_RF_CHANNEL;
			break;
		case TYPE_TANK_MODEL14:
			ReaderNum  = RFRDR4;
			ChannelNum = MODEL_CONTAINER14_RF_CHANNEL;
			break;
#endif
		default:
			return Q_NO_ERROR;
	}

	return Q_NO_ERROR;
}

//---- Liquid Models -----------------------------------
//Model on sensor procedures
TQErrCode CContainer::GetLiquidTankInsertedStatus()
{
	return GetLiquidTankInsertedStatusFiltered(ALL_TANKS);
}

TQErrCode CContainer::GetLiquidTankInsertedStatusFiltered(int /* TTankIndex */ TanksFilter)
{
	// Verify if we are not performing other command
	if (m_FlagLiquidTank)
		throw EContainer("Container:GetLiquidTankInsertedStatus:two message send in the same time");


	m_GetTankInsertedFilter = static_cast<TTankIndex>(TanksFilter);

	// Build the Actuator turn on message
	TOCBIsLiquidTankInsertedMessage LiquidMsg;

	LiquidMsg.MessageID = static_cast<BYTE>(OCB_IS_LIQUID_TANK_INSERTED);
	LiquidTank_AckOk    = false;
	m_FlagLiquidTank    = false;

	// Send a Turn ON request
	if (m_OCBClient->SendInstallWaitReply(&LiquidMsg,
										  sizeof(TOCBIsLiquidTankInsertedMessage),
										  LiquidTankInsertedResponse,
										  reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
	{
		CQLog::Write(LOG_TAG_GENERAL, "OCB didn't get ack for \"GetLiquidTankInsertedStatus\".");
		return Q2RT_LIQUID_MODEL_MSG_FAIL;
	}

	if(!LiquidTank_AckOk)
	{
		CQLog::Write(LOG_TAG_GENERAL, "OCB ack failure \"GetLiquidTankInsertedStatus\".");
		return Q2RT_LIQUID_MODEL_MSG_FAIL;
	}

	return Q_NO_ERROR;
}//GetLiquidTankInsertedStatus


// Acknolodges for Command LiquidTankInsertedResponse
// -------------------------------------------------------
void CContainer::LiquidTankInsertedResponse(int            TransactionId,
		PVOID          Data,
		unsigned       DataLength,
		TGenericCockie Cockie)
{
	// Build the Actuator  turn on message
	TOCBLiquidTankStatusMessage* ResponseMsg = static_cast<TOCBLiquidTankStatusMessage *>(Data);

	// Get a pointer to the instance
	CContainer* InstancePtr = reinterpret_cast<CContainer *>(Cockie);

	//Verify size of message
	if(DataLength != sizeof(TOCBLiquidTankStatusMessage))
	{
		FrontEndInterface->NotificationMessage("Actuator \"LiquidTankInsertedResponse\" length error");
		CQLog::Write(LOG_TAG_GENERAL, "Actuator \"LiquidTankInsertedResponse\" length error");
		return;
	}

	if (static_cast<int>(ResponseMsg->MessageID) != OCB_LIQUID_TANK_INSERTED_STATUS)
	{
		FrontEndInterface->NotificationMessage("Actuator \"LiquidTankInsertedResponse\" message id error");
		CQLog::Write(LOG_TAG_GENERAL, "Actuator \"LiquidTankInsertedResponse\" message id error",
					 (int)ResponseMsg->MessageID);
		return;
	}
#ifdef OBJET_MACHINE_KESHET
	for (int i = FIRST_TANK_TYPE; i < InstancePtr->GetNumOfTanks(); i++)
	{
		if ((InstancePtr->m_GetTankInsertedFilter == ALL_TANKS) || (InstancePtr->m_GetTankInsertedFilter == i))
		{
			InstancePtr->UpdateLiquidTankInserted(static_cast<TTankIndex>(i), (bool)ResponseMsg->TankStatus[i]);
		}
	}
#else
	BYTE Statuses[TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE] = INIT_MSG_VALUES_W_ARRAY(ResponseMsg, Status);
	for (int i = FIRST_TANK_TYPE; i < InstancePtr->GetNumOfTanks(); i++)
	{
		if ((InstancePtr->m_GetTankInsertedFilter == ALL_TANKS) || (InstancePtr->m_GetTankInsertedFilter == i))
		{
			InstancePtr->UpdateLiquidTankInserted(static_cast<TTankIndex>(i), (bool)Statuses[i]);
		}
	}
#endif
	InstancePtr->LiquidTank_AckOk = true;
}//LiquidTankInsertedResponse

TQErrCode CQSingleContainer::IgnoreTankInsertionAndRemovalSignals()
{
	m_IgnoreTankInsertionAndRemovalSignals = true;
	return Q_NO_ERROR;
}

TQErrCode CQSingleContainer::AllowTankInsertionAndRemovalSignals()
{
	m_IgnoreTankInsertionAndRemovalSignals = false;
	return Q_NO_ERROR;
}

TQErrCode CContainer::IgnoreTankInsertionAndRemovalSignals()
{
	for (int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; i++)
		m_TanksArray[i]->IgnoreTankInsertionAndRemovalSignals();
	return Q_NO_ERROR;
}

TQErrCode CContainer::AllowTankInsertionAndRemovalSignals()
{
	for (int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; i++)
		m_TanksArray[i]->AllowTankInsertionAndRemovalSignals();
	return Q_NO_ERROR;
}

// Called to simulate Tanks Removal.
// Used to silence RFID sampling before turning RFID power Off (e.g. during SHR Wizard) - to avoid exceptions.
TQErrCode CContainer::SendTanksRemovalSignals()
{
	for (int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; i++)
		UpdateLiquidTankInserted((TTankIndex)i, false);

	// Busy-wait for the SingleContainer's State Machines to stabilize on BEGIN_NO_TANK state.
	// This guarantees that RFID Modul will not be accessed after this function returns.
	unsigned long StartTime = QGetTicks();

	for(;;)
	{
		bool TanksRemovalCompleted = true;

		for (int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; i++)
			if (m_TanksArray[i]->GetState() != BEGIN_NO_TANK)
			{
				TanksRemovalCompleted = false;
				break;
			}

		if (TanksRemovalCompleted)
			return Q_NO_ERROR;

		if ((QGetTicks() - StartTime) > TIMEOUT_FOR_CONTAINERS_REMOVAL)
			return Q2RT_CONTAINERS_OUT_TIMEOUT;

		QSleep(50);
	}
}

// Called at startup and after statemachine reset to generate MS_In events to Identification Statemachine.
TQErrCode CContainer::InitializeMSEventsNotifications()
{
	for(int i = FIRST_MODEL_TANK_TYPE; i < LAST_MODEL_TANK_TYPE; i++)
		if (m_TanksArray[i]->IsTankInserted())
			NotifyMSEvent((TTankIndex)i, true);

	return Q_NO_ERROR;
}

TQErrCode CContainer::GetIfAllLiquidTankAreInserted(void)
{
	TQErrCode Err;
	if((Err = GetLiquidTankInsertedStatus()) != Q_NO_ERROR)
		return Err;

	for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
	{
		if (m_ChambersArray[i].IsActiveLiquidTankInserted() == false)
		{
			{

				CQLog::Write(LOG_TAG_GENERAL, QFormatStr("Liquid tank: %s is not inserted", ChamberToStr((TChamberIndex)i).c_str()));
				return ((IS_SUPPORT_CHAMBER(i)) ? Q2RT_LIQUID_SUPPORT_TANK_ARE_NOT_INSERTED : Q2RT_LIQUID_MODEL_TANK_ARE_NOT_INSERTED);
			}
		}
	}
	return Q_NO_ERROR;
}//GetIfAllLiquidTankAreInserted


bool CContainerBase::AreAllTanksStable()
{
    CQSingleContainer * currTank;

    for (int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; ++i)
    {
        currTank = m_TanksArray[i];
        if (!(currTank->IsTankInserted()))
        {
            continue; // no tank ==> stable
        }

        if (currTank->IsTankEnabled())
        {
            continue; // this tank is stable
        }

        switch (currTank->GetState())
        {
            case IDENTIFIED_STABLE:
                if ((currTank->GetTankMaterialType() != currTank->GetMaterialType()) &&
                    (currTank->GetMaterialType() != "")                                )
                {
                    continue; // not the correct tank ==> stable
                }
                /* falling through */
            case BEGIN_NO_TANK:
            case RF_WAITING:
                return false; // this tank is not stable
        }
    }

    return true; // if we got to here we didn't find any unstable tanks along the way
}

bool CContainerBase::GetIfAllLiquidTankAreEnabled(void)
{
	TQErrCode Err = Q2RT_LIQUID_TANKS_DISABLED_ERROR;
	bool allEnabled = true;

	for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
	{
		if (! m_ChambersArray[i].IsActiveLiquidTankEnabled())
		{
			{
				allEnabled = false;
				if ( m_AllLiquidTankEnabled ) // If there was a change in the Enabled status, and it became 'false' - report as an Error.
				{
					CQLog::Write(LOG_TAG_GENERAL, QFormatStr("%s: %s.", ChamberToStr((TChamberIndex)i).c_str(), PrintErrorMessage(Err).c_str()));
					m_ErrorHandlerClient->ReportError(PrintErrorMessage(Err), Err, static_cast<int>(i));
				}
				break;
			}
		}
	}

	// Update the Machine Sequencer with current status if 'true'. ('false' is handled by the m_ErrorHandlerClient)
	if (true == (m_AllLiquidTankEnabled = allEnabled))
		Q2RTApplication->GetMachineManager()->GetMachineSequencer()->SetLiquidTanksEnabledStatus(m_AllLiquidTankEnabled);

	return m_AllLiquidTankEnabled;
}//GetIfAllLiquidTankAreEnabled

void CContainer::NotificationLiquidTankStatusChanged(int         TransactionId,
		PVOID        Data,
		unsigned      DataLength,
		TGenericCockie Cockie)
{
// Build the Actuator  turn on message
	TOCBLiquidTanksStatusChangedResponse *NotificationMsg = static_cast<TOCBLiquidTanksStatusChangedResponse *>(Data);

	// Get a pointer to the instance
	CContainer *InstancePtr = reinterpret_cast<CContainer *>(Cockie);

	//Verify size of message
	if(DataLength != sizeof(TOCBLiquidTanksStatusChangedResponse))
	{
		FrontEndInterface->NotificationMessage("Container \"NotificationLiquidTankStatusChanged\" length error");
		CQLog::Write(LOG_TAG_EOL, "Container \"NotificationLiquidTankStatusChanged\" length error");
		return;
	}

	// Update DataBase before Open the Semaphore/Set event.
	if (static_cast<int>(NotificationMsg->MessageID) != OCB_LIQUID_TANK_STATUS_CHANGED)
	{
		FrontEndInterface->NotificationMessage("Container \"NotificationLiquidTankStatusChanged\" message id error");
		CQLog::Write(LOG_TAG_EOL, "Container \"NotificationLiquidTankStatusChanged\" message id error (0x%X)",
					 (int)NotificationMsg->MessageID);
		return;
	}

	InstancePtr->AckToOcbNotification(OCB_LIQUID_TANK_STATUS_CHANGED,
									  TransactionId,
									  OCB_Ack_Success,
									  Cockie);
	QString Output = "Tanks Status Changed:";
#ifdef OBJET_MACHINE_KESHET
	for (int i = FIRST_TANK_TYPE; i < InstancePtr->GetNumOfTanks(); i++)
	{
		InstancePtr->UpdateLiquidTankInserted(static_cast<TTankIndex>(i), (bool)(NotificationMsg->TankStatus[i]));
		Output += TankToStr(static_cast<TTankIndex>(i)) + QString((InstancePtr->m_TanksArray[i]->IsTankInserted() ? " in " : " out "));
	}
#else
	BYTE Statuses[TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE] = INIT_MSG_VALUES_W_ARRAY(NotificationMsg, Status);

	for (int i = FIRST_TANK_TYPE; i < InstancePtr->GetNumOfTanks(); i++)
	{
		InstancePtr->UpdateLiquidTankInserted(static_cast<TTankIndex>(i), (bool)Statuses[i]);
		Output += TankToStr(static_cast<TTankIndex>(i)) + QString((InstancePtr->m_TanksArray[i]->IsTankInserted() ? " in " : " out "));
	}
#endif
	// Writing to the log
	CQLog::Write(LOG_TAG_GENERAL, Output);
}//NotificationLiquidTankStatusChanged

// Get liquid tanks weight
TQErrCode CContainerBase::GetLiquidTanksWeight(void)
{
	//Updating the Active Pumps
	SetActiveContainersBeforePrinting(false);

	// Reactivate waste if it was automatically turned OFF
	if(! IsWasteFull())
	{
		if(IsWasteWasActive())
		{
			SetWasteWasActive(false);
			ActivateWaste(true);
		}
	}

	GetIfAllLiquidTankAreEnabled(); // issues an error in case not all Tanks are enabled, to prevent other modules from using it. (e.g: headFilling)

	return Q_NO_ERROR;
}

TQErrCode CContainer::GetLiquidTanksWeight(void)
{
	CContainerBase::GetLiquidTanksWeight();
	if (m_FlagGetStatusUnderUse)
	{
		CQLog::Write(LOG_TAG_EOL, "Container \"GetLiquidTanksWeight\" reentry problem");
		return Q_NO_ERROR;
	}
	TOCBGetLiquidTanksWeightMessage GetMsg;
	m_FlagGetStatusUnderUse = true;
	GetMsg.MessageID        = static_cast<BYTE>(OCB_GET_LIQUID_TANK_WEIGHT_STATUS);

	// Send a Turn ON request
	if (m_OCBClient->SendInstallWaitReply(&GetMsg,
										  sizeof(TOCBGetLiquidTanksWeightMessage),
										  GetLiquidTanksWeightAckResponse,
										  reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
	{
		CQLog::Write(LOG_TAG_EOL, "OCB didn't get ack for Container \"GetLiquidTanksWeight\" message");
	}
	m_FlagGetStatusUnderUse = false;
	return Q_NO_ERROR;
}//GetLiquidTanksWeight


void CContainer::GetLiquidTanksWeightAckResponse(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
	TOCBLiquidTanksWeightResponse *StatusMsg = static_cast<TOCBLiquidTanksWeightResponse *>(Data);

	// Get a pointer to the instance
	CContainer *InstancePtr = reinterpret_cast<CContainer *>(Cockie);

	//Verify size of message
	if(DataLength != sizeof(TOCBLiquidTanksWeightResponse))
	{
		CQLog::Write(LOG_TAG_EOL, "Container \"GetLiquidTanksWeightAckResponse\" length error");
		return;
	}

	if (static_cast<int>(StatusMsg->MessageID) != OCB_LIQUID_TANK_WEIGHT)
	{
		FrontEndInterface->NotificationMessage("Container \"GetLiquidTanksWeightAckResponse\" message id error");
		CQLog::Write(LOG_TAG_EOL, "Container \"GetLiquidTanksWeightAckResponse\" message id error (0x%X)", (int)StatusMsg->MessageID);
		return;
	}
	int lastWasteTank = InstancePtr->GetLastWasteTank();
#ifdef OBJET_MACHINE_KESHET
	// Updating the Weights of the liquid tanks
	for (int i = FIRST_TANK_INCLUDING_WASTE_TYPE; i < lastWasteTank; i++)
		InstancePtr->m_TanksArray[i]->SetTankWeight(StatusMsg->TankWeight[i]);
#else
	float Weights[TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE] = INIT_MSG_VALUES_W_ARRAY(StatusMsg, Weight);
	// Updating the Weights of the liquid tanks
	for (int i = FIRST_TANK_INCLUDING_WASTE_TYPE; i < lastWasteTank; i++)
		InstancePtr->m_TanksArray[i]->SetTankWeight(Weights[i]);
#endif

	// Write the new weights the Log only if one of the 'Printed' weights is different atleast in 40 gr
	bool NeedToWriteWeightsToLog = false;

	for (int i = FIRST_TANK_INCLUDING_WASTE_TYPE; i < lastWasteTank; i++)
	{
		if (InstancePtr->m_TanksArray[i]->IsPrintWeightNecessary())
			NeedToWriteWeightsToLog = true;
	}

	int WeightArray[TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE];
	QString tmp = "Liquids weight: ";
	QString tmp2 = "Liquids weight: ";
	for(int i = FIRST_TANK_INCLUDING_WASTE_TYPE; i < lastWasteTank; i++)
	{
		WeightArray[i] = InstancePtr->GetTankWeightInGram((TTankIndex)i);
		if( FIRST_WASTE_TANK_TYPE > i )
			tmp += TankToStr(static_cast<TTankIndex>(i)) + " = " + QFloatToStr(WeightArray[i]) + "; ";
		else
			tmp2 +=  TankToStr(static_cast<TTankIndex>(i)) + " = " + QFloatToStr(WeightArray[i]) + "; ";
	}

	// When using Dual waste mechanism on Objet1000 machines, we have "in place" microswitches
	// for each waste tank, so this is not needed
	if(! InstancePtr->m_ParamsMgr->DualWasteEnabled)
	{
		InstancePtr->UpdateLiquidTankInserted(TYPE_TANK_WASTE_LEFT, (InstancePtr->m_TanksArray[TYPE_TANK_WASTE_LEFT]->GetTankWeightInGram(true) > WASTE_IS_EMPTY));
		FrontEndInterface->UpdateStatus(FE_TANK_EXISTENCE_STATUS_WASTE_TANK_LEFT, (int)(InstancePtr->IsActiveLiquidTankInserted((int)TYPE_CHAMBER_WASTE)));
	}

	if (NeedToWriteWeightsToLog)
	{
		CQLog::Write(LOG_TAG_EOL, tmp);
		CQLog::Write(LOG_TAG_EOL, tmp2);
	}

	for(int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; i++)
	{
		// Update the front end with the status of the containers
		FrontEndInterface->UpdateStatus(FE_CURRENT_TANK_STATUS_BASE + i,
										(int)(WeightArray[i] > (InstancePtr->m_ParamsMgr->WeightLevelLimitArray[TankToStaticChamber((TTankIndex)i)]
												+ WEIGHT_ERROR_WARNNING_DELTA)));

		WeightArray[i] -= InstancePtr->m_ParamsMgr->WeightLevelLimitArray[TankToStaticChamber(static_cast<TTankIndex>(i))];
		// Update the front end with the current weight
		FrontEndInterface->UpdateStatus(FE_CURRENT_TANK_WEIGHT_BASE + i,
										WeightArray[i] > 0 ? WeightArray[i] : 0);

		// Update the front end with the relative weight
		FrontEndInterface->UpdateStatus(FE_CURRENT_TANK_RELATIVE_WEIGHT_BASE + i,
										WeightArray[i] * 100 /
										InstancePtr->m_ParamsMgr->CartridgeFullWeight);
	}

	for(int i = FIRST_WASTE_TANK_TYPE; i < lastWasteTank; i++)
	{
		FrontEndInterface->UpdateStatus(FE_CURRENT_TANK_STATUS_BASE + i,
										(int)(WeightArray[i] < (InstancePtr->m_ParamsMgr->WeightLevelLimitArray[TYPE_CHAMBER_WASTE]
												- WASTE_WEIGHT_ERROR_WARNNING_DELTA)));

		FrontEndInterface->UpdateStatus(FE_CURRENT_TANK_WEIGHT_BASE + i, WeightArray[i] > 0 ? WeightArray[i] : 0);

		FrontEndInterface->UpdateStatus(FE_CURRENT_TANK_RELATIVE_WEIGHT_BASE + i,
										WeightArray[i] * 100 /
										InstancePtr->m_ParamsMgr->CartridgeFullWeight);
	}

}//GetLiquidTanksWeightAckResponse

int CContainer::GetTotalWeight(int Chamber)
{
	int Weight = 0;
	if (VALIDATE_CHAMBER(Chamber))
		Weight = m_ChambersArray[Chamber].GetTotalWeight();
	return Weight;
}//GetTotalWeight

int CContainerBase::GetRemainingWasteWeight()
{
	int RemainingWeight = 0;
	for(int i = FIRST_WASTE_TANK_TYPE; i < m_lastWasteTank; i++)
		if(m_TanksArray[i]->IsTankInserted())
		{
			int delta = m_ParamsMgr->WeightLevelLimitArray[TYPE_CHAMBER_WASTE] - m_TanksArray[i]->GetTankWeightInGram();
			if (delta>0) RemainingWeight += delta; //add the weight if it's not overflowed
		}
	return RemainingWeight;
}//GetRemainingWasteWeight

float CContainerBase::GetTankWeightInA2D(int /* TTankIndex */ Tank)
{
	if (!VALIDATE_TANK_INCLUDING_WASTE(Tank))
		throw EContainer("'GetTankWeightInA2D' Error: invalid parameter");
	return m_TanksArray[Tank]->GetTankWeightInA2D();
}//GetTankWeightInA2D

unsigned int CContainer::GetLastInsertionTimeInTicks(int /* TTankIndex */ Tank)
{
	if (!VALIDATE_TANK_INCLUDING_WASTE(Tank))
		throw EContainer("'GetLastInsertionTimeInTicks' Error: invalid parameter");

	return m_TanksArray[Tank]->GetLastInsertionTimeInTicks();
}

float CContainerBase::GetTankWeightInGram(int /* TTankIndex */ Tank)
{
	if (!VALIDATE_TANK_INCLUDING_WASTE(Tank))
		throw EContainer("'GetTankWeightInGram' Error: invalid parameter");
	return m_TanksArray[Tank]->GetTankWeightInGram(/*TYPE_TANK_WASTE == Tank*/);
}//GetTankWeightInGram

int CContainerBase::GetWeight(int /* TChamberIndex */ Chamber, int /* TTankIndex */ Tank)
{
	if (!VALIDATE_CHAMBER_INCLUDING_WASTE(Chamber))
		throw EContainer("'GetWeight' Error: invalid parameter");
	return m_ChambersArray[Chamber].GetWeightInGram(static_cast<TTankIndex>(Tank));
}

bool CContainerBase::IsWeightOk(int /* TChamberIndex */ Chamber)
{
	if (TYPE_CHAMBER_WASTE == Chamber)
		return (! IsWasteFull());

	if (!VALIDATE_CHAMBER(Chamber))
		throw EContainer("'GetWeight' Error: invalid parameter");
	return IsTankWeightOk(m_ChambersArray[Chamber].GetActiveTank());
}//IsWeightOk

bool CContainerBase::IsTankWeightOk(TTankIndex Tank, bool CheckHysteresys )
{
	if(Tank>=FIRST_WASTE_TANK_TYPE && Tank<m_lastWasteTank)
	{
		if (m_ChambersArray[TYPE_CHAMBER_WASTE].IsChambersTank(Tank))
			return (GetTankWeightInGram(Tank) < m_ParamsMgr->WeightLevelLimitArray[TYPE_CHAMBER_WASTE]);
		else
        	return false;
	}

	for(int i = FIRST_CHAMBER_TYPE_INCLUDING_WASTE; i < m_numOfChambers; i++)
	{
		if (m_ChambersArray[i].IsChambersTank(Tank))
		{
			if( CheckHysteresys == true )
				return (GetTankWeightInGram(Tank) > ( m_ParamsMgr->WeightLevelLimitArray[i] + WEIGHT_HYSTERESIS));
			else
				return (GetTankWeightInGram(Tank) > m_ParamsMgr->WeightLevelLimitArray[i]);
        }

	}
	
	return false;
}

bool CContainerBase::IsWasteFull()
{
	bool AnyTankInserted = false;
	for(int i = FIRST_WASTE_TANK_TYPE; i < m_lastWasteTank; i++)
	{
		if(m_TanksArray[i]->IsTankInserted())
		{
			AnyTankInserted = true;
			if(m_TanksArray[i]->GetTankWeightInGram() < (m_ParamsMgr->WeightLevelLimitArray[TYPE_CHAMBER_WASTE] - WASTE_WEIGHT_ERROR_WARNNING_DELTA))
				return false;
		}
	}
	return (AnyTankInserted ? true : false);
}

// Indicates if waste mechanism was active before automaticaly turning OFF
bool CContainerBase::IsWasteWasActive()
{
    return m_WasteWasActive;
}

void CContainerBase::SetWasteFull(bool WasteState)
{
	m_WasteIsFull = WasteState;
}

void CContainerBase::SetWasteWasActive(bool WasteState)
{
	m_WasteWasActive = WasteState;
}

bool CContainer::GetWasteWeightStatusDuringPrinting()
{
	for(int i = FIRST_WASTE_TANK_TYPE; i < m_lastWasteTank; i++)
		if(m_TanksArray[i]->IsTankInserted())
			if(m_TanksArray[i]->GetTankWeightInGram() < m_ParamsMgr->WeightLevelLimitArray[TYPE_CHAMBER_WASTE])
				return true;
	return false;
}//GetWasteWeightStatusDuringPrinting

bool CContainer::GetWasteWeightStatusBeforePrinting()
{
	return (! IsWasteFull());
}//GetWasteWeightStatusBeforePrinting

bool CContainer::SetContainerGain(int /* TTankIndex */ Tank, float Gain)
{
	if(!VALIDATE_TANK_INCLUDING_WASTE(Tank))
		throw EContainer("'SetContainerGain' Error: invalid Tank number");
	m_TanksArray[Tank]->SetTankGain(Gain);
	return true;
}//SetContainerGain

bool CContainer::SetContainerOffset(int /* TTankIndex */ Tank, float Offset)
{
	if(!VALIDATE_TANK_INCLUDING_WASTE(Tank))
		throw EContainer("'SetContainerOffset' Error: invalid Tank number");
	m_TanksArray[Tank]->SetTankOffset(Offset);
	return true;
}//SetContainerOffset

// Set the active containers before printing - print with the 'Low Weight' one
bool CContainer::SetActiveContainersBeforePrinting(bool PrintToLog)
{
	static int      CheckActiveTanksPeriod = ACTIVE_PUMPS_PERIODIC_CHECK_ITERATIONS;
	bool            TankSwapNeeded         = false;
	TTankIndex      TmpTank;
	TTankIndexArray ActiveTanks;
	
	for(int i = FIRST_CHAMBER_TYPE; i < m_numOfChambers; i++)
	{
		{
			// Decide which tank should be the active tank (decided upon its weight)
			TmpTank        = m_ChambersArray[i].GetTankToActivate();
			ActiveTanks[i] = m_ChambersArray[i].GetActiveTank();
			
			if ((TmpTank != NO_TANK) && (TmpTank != ActiveTanks[i]))
			{
				TankSwapNeeded = true;
				ActiveTanks[i] = TmpTank;
			}

			if(i != TYPE_CHAMBER_WASTE)
			{
				TTankIndex currActiveTank = ActiveTanks[i];
				if (NO_TANK != currActiveTank)
				{
					// Keep history of all batch numbers (of the printing tanks)
					if(m_TanksArray[currActiveTank]->NeedBatchNumberUpdate() || TankSwapNeeded)
					{
						m_batchNoTable->AddTankBatchNumber(currActiveTank, m_TanksArray[currActiveTank]->GetBatchNo(), GetTankMaterialType(currActiveTank));
						m_TanksArray[currActiveTank]->SetNeedBatchNumberUpdate(false);
					}
				}
			}
		}
	}
	
	if(TankSwapNeeded)
	{
		MonitorWaste(); // Call this before new active tank is set

		SetActiveTank(ActiveTanks, PrintToLog); // Note: SetActiveTank() sends a message to OCB to swap pumps.

		// Need to split it here because the newly active tank was not set yet
		if(IsWasteWasActive())
		{
            SetWasteWasActive(false);
			ActivateWaste(true);
		}
	}
	// In case that tank swap is not needed but both waste tanks are full, deactivate currently active waste tank
	else if(IsWasteFull())
	{
		MonitorWaste();
	}
	// Each CheckActiveTanksPeriod iterations we check if OCB Active pumps matches PC's, and if not,
	// we perform a SetActiveTank().
	else if (CheckActiveTanksPeriod <= 0)
	{
		CHeadFillingBase* HeadFillingInstance =
			Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadFillingInstance();

		// If Heads Filling not yet created:
		if (! HeadFillingInstance)
		{
			return true;
		}
		// Following command gets the Active Pumps from OCB:
		HeadFillingInstance->GetHeadsFillingsActiveTanks(); // This is the OCBStatusSender exception causing call !!!

		for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
		{
			{
				TmpTank      = (TTankIndex)HeadFillingInstance->GetActiveTank((TChamberIndex)i);
				ActiveTanks[i] = m_ChambersArray[i].GetActiveTank();
				if (TmpTank != ActiveTanks[i])
				{
					TankSwapNeeded = true;
					ActiveTanks[i] = TmpTank;
				}
			}
		}
		if(TankSwapNeeded)
		{
			CQLog::Write(LOG_TAG_HEAD_FILLING, "Mismatch detected between OCB and PC Active Head Filling Pumps.");
			SetActiveTank(ActiveTanks, PrintToLog);
		}
		CheckActiveTanksPeriod = ACTIVE_PUMPS_PERIODIC_CHECK_ITERATIONS;
	}
	CheckActiveTanksPeriod--;

	return true;
}//SetActiveContainersBeforePrinting


// Waste activity monitoring and pump switching
void CContainerBase::MonitorWaste()
{
	if(IsWasteActive())
	{
		CBackEndInterface *BackEnd = CBackEndInterface::Instance();

		BackEnd->EnableDisableMaintenanceCounter(DIRT_PUMPS_TUBES_COUNTER_ID, false);

		//OBJET_MACHINE_KESHET, verify this change
		TTankIndex tankIndex = m_ChambersArray[TYPE_CHAMBER_WASTE].GetActiveTank();
		BackEnd->SetActuatorState(PumpsActuatorID[tankIndex], false);
		BackEnd->EnableDisableMaintenanceCounter(PumpsCounterID[tankIndex], false);

		SetWasteWasActive(true); // indicates that waste was active
	}
}

bool CContainerBase::IsTankInAndEnabled(/*TTankIndex*/int Tank)
{
	if (!VALIDATE_TANK_INCLUDING_WASTE(Tank))
		throw EContainer("'IsTankInAndEnabled' Error: invalid parameter");

    if (NULL == m_TanksArray[Tank])
    {
        return false;
    }

	bool IsEnabled  = m_TanksArray[Tank]->IsTankEnabled();
	bool IsInserted = m_TanksArray[Tank]->IsTankInserted();

	return (IsEnabled && IsInserted);
}//IsTankInAndEnabled


QString CContainerBase::GetModelResinType(int resin_index)
{
	return CModesManager::Instance()->GetModelResinType(resin_index);
}

QString CContainerBase::GetServiceModelResinType(int resin_index)
{
	return CModesManager::Instance()->GetServiceModelResinType(resin_index);
}

QString CContainerBase::GetSupportResinType(int resin_index)
{
	return CModesManager::Instance()->GetSupportResinType(resin_index);
}

QString CContainerBase::GetServiceSupportResinType(int resin_index)
{
	return CModesManager::Instance()->GetServiceSupportResinType(resin_index);
}

TQErrCode CContainerBase::AreAllLiquidsWeightOk()
{
	if (!GetWasteWeightStatusDuringPrinting())
		return Q2RT_CONTAINERS_WASTE_FULL;

	for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
	{
		if (!m_ChambersArray[i].IsLiquidWeightOk())
			return (IS_SUPPORT_CHAMBER(i) ? Q2RT_CONTAINERS_SUPPORT_EMPTY : Q2RT_CONTAINERS_MODEL_EMPTY);
	}
	return Q_NO_ERROR;
}//AreAllLiquidsWeightOk

//----------------------------------------------------------------
void CContainer::AckToOcbNotification(int MessageID,
									  int TransactionID,
									  int AckStatus,
									  TGenericCockie Cockie)
{
	// Build the EOL turn on message
	TOCBAck AckMsg;

	// Get a pointer to the instance
	CContainer *InstancePtr = reinterpret_cast<CContainer *>(Cockie);

	AckMsg.MessageID        = OCB_EDEN_ACK;
	AckMsg.RespondedMessageID = MessageID;
	AckMsg.AckStatus        = AckStatus;

	// Send a Turn ON request
	InstancePtr->m_OCBClient->SendNotificationAck(TransactionID,
			&AckMsg,
			sizeof(TOCBAck));
}//

//----------------------------------------------------------------
void CContainer::Cancel(void)
{}//Cancel

bool CContainerBase::IsRelevantTankInserted(TTankIndex Tank)
{
	return AreRelevantTanksInserted(TankToStaticChamber(Tank), Tank);
}//IsRelevantTankInserted

bool CContainerBase::AreRelevantTanksInserted(TChamberIndex Chamber, TTankIndex Tank)
{
	//objet_machine test
	if (FindWindow(0, QFormatStr( "Tank%dOut.txt - Notepad", (int)Tank ).c_str() ) )
		return false;
	if (Tank != ACTIVE_TANK && !VALIDATE_CHAMBER_INCLUDING_WASTE(Chamber))
		throw EContainer("'AreRelevantTanksInserted' Error: invalid parameter");
	return m_ChambersArray[Chamber].AreRelevantTanksInserted(Tank);
}//AreRelevantTanksInserted

void CContainerBase::UpdateTanksStatus(TTankIndex Tank)
{
	// Set Pipe material according to the updated Parameter:
	// Notice that the TypesArrayPerTank parameter (below) is changed via Material Mode activation, and not "programatically".
	m_TanksArray[Tank]->SetMaterialType(m_ParamsMgr->TypesArrayPerPipe[Tank]);

	if (CHECK_EMULATION(m_ParamsMgr->OCB_Emulation) && (m_TanksArray[Tank]->GetMaterialType() == "UNDEFINED"))  // really ugly patch just to allow QAing the silent MRW.
	{
		m_TanksArray[Tank]->SetTankEnabled(false);
		FrontEndInterface->UpdateStatus(FE_TANK_DISABLING_HINT_BASE + Tank, QString("Material Mismatch"), true);
		FrontEndInterface->UpdateStatus(FE_TANK_ENABLING_STATUS_BASE + Tank, FE_TANK_DISABLED_STATUS, true);
	}


	RemountSingleTankStatus(Tank);

	// Saving parameters in the parameter manager
	m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->TypesArrayPerTank);
}//UpdateTanksStatus

bool CContainer::RemountSingleTankStatus(int /* TTankIndex */ Tank)
{
	// Nothing to remount if there's no Tank. (remount means "re-insert")
	if (false == m_TanksArray[Tank]->IsTankInserted())
		return true;

	m_TanksArray[Tank]->SetRemountInProgress(true);
	m_TanksArray[Tank]->ResetEventIdentificationCompletion();

	UpdateLiquidTankInserted(static_cast<TTankIndex>(Tank), false);

	// Set RemountSingleTankCallback() to be the callback function.
	SetStateEnterCallback(BEGIN_NO_TANK, static_cast<TTankIndex>(Tank), CContainer::RemountSingleTankCallback, reinterpret_cast<TGenericCockie>(this));

	return true;
}

bool CContainer::WaitForIdentificationCompletion(int /* TTankIndex */ Tank, bool DontWaitIfTankIsOut, unsigned int timetowait)
{
// Reason for using m_RemountInProgress: without it, we might actually be during a remount, but before GetTankIDState() has changed to DATA_READ_NOT_ACTIVE.
//   - Another issue it solves is: When GetState() == BEGIN_NO_TANK during Remount. In that case, if we have DontWaitIfTankIsOut=true, we would *still* want to wait.
	if (!m_TanksArray[Tank]->IsRemountInProgress())
	{
		if (DontWaitIfTankIsOut)
			if (m_TanksArray[Tank]->GetState() == BEGIN_NO_TANK)
				return true;

		// In case we're already in (or after) CHECK_PUMP_TIME state, return 'true' without actually waiting.
		if (m_TanksArray[Tank]->GetTankIDState() != DATA_READ_NOT_ACTIVE)
			return true;
	}

	// Set RemountSingleTankCallback() to be the callback function.
	SetStateEnterCallback(CHECK_PUMP_TIME, static_cast<TTankIndex>(Tank), CContainer::IdentificationCompletedCallback, reinterpret_cast<TGenericCockie>(this));

	if (QLib::wrSignaled == m_TanksArray[Tank]->WaitEventIdentificationCompletion(timetowait))
		return true;
	else
		return false;
}
bool CContainerBase::IsMicroSwitchInserted(int Tank)
{
	return m_TanksArray[Tank]->IsMicroswitchInserted();
}
int CContainerBase::GetSiblingContainer(int Tank)
{
	return (int) GetSiblingTank((TTankIndex) Tank);
}

bool CContainerDummy::WaitForIdentificationCompletion(int /* TTankIndex */ Tank, bool DontWaitIfTankIsOut, unsigned int timetowait)
{
	return true;
}

QLib::TQWaitResult CQSingleContainer::WaitEventIdentificationCompletion(unsigned int timetowait)
{
	return m_EventWaitIdentificationCompletion.WaitFor(timetowait);
}

void CQSingleContainer::SetEventIdentificationCompletion()
{
	m_EventWaitIdentificationCompletion.SetEvent();
}

void CQSingleContainer::ResetEventIdentificationCompletion()
{
	m_EventWaitIdentificationCompletion.WaitFor(0);
}

void CContainer::IdentificationCompletedCallback(TTankIndex Tank, TGenericCockie Cockie)
{
	CContainer *InstancePtr = reinterpret_cast<CContainer *>(Cockie);

	// Erase the callback pointers after callback was done.
	InstancePtr->SetStateEnterCallback(CHECK_PUMP_TIME, Tank, NULL, NULL);

	InstancePtr->m_TanksArray[Tank]->SetEventIdentificationCompletion();
}


bool CContainerDummy::RemountSingleTankStatus(int /* TTankIndex */ Tank)
{

	for (int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; i++)
	{
		FrontEndInterface->UpdateStatus(FE_TANK_ENABLING_STATUS_BASE + i, FE_TANK_ENABLED_STATUS, false);
	}

	return CContainerBase::SetAllLiquidTanksEnabled(true);
}

void CContainer::SetStateEnterCallback(TankIdentificationStates state, TTankIndex Tank, TStateEnterCallback CallbackFunction, TGenericCockie Cockie)
{
	// not yet supporting all states.
	if ((BEGIN_NO_TANK != state) && (CHECK_PUMP_TIME != state))
		throw EContainer("Containers: SetStateEnterCallback - unsupported state requested");

	m_TanksArray[Tank]->SetStateEnterCallback(state, CallbackFunction, Cockie);
}

void CQSingleContainer::SetStateEnterCallback(TankIdentificationStates state, TStateEnterCallback CallbackFunction, TGenericCockie Cockie)
{
	switch (state)
	{
		case BEGIN_NO_TANK:
			m_OnEnterBeginNoTankCallback = CallbackFunction;
			m_OnEnterBeginNoTankCockie   = Cockie;
			break;

		case CHECK_PUMP_TIME:
			m_OnLeaveCheckResinTypeCallback  = CallbackFunction;
			m_OnLeaveCheckResinTypeCockie    = Cockie;
			break;

			// not yet supporting callbacks for all states.
		default:
			throw EContainer("Containers: SetStateEnterCallback - unsupported state requested");
//    break;
	}
}

void CContainer::RemountSingleTankCallback(TTankIndex Tank, TGenericCockie Cockie)
{
	CContainer *InstancePtr = reinterpret_cast<CContainer *>(Cockie);

	// Erase the callback pointers after callback was done.
	InstancePtr->SetStateEnterCallback(BEGIN_NO_TANK, Tank, NULL, NULL);

	// Cause sending MS_IN signals for the specific Tank if inserted. Will start re-identification process for the Tank (that was simulated as not-inserted in RemountSingleTankStatus())
	InstancePtr->GetLiquidTankInsertedStatusFiltered(Tank);
}

void CContainerBase::UpdateTanks(TTankIndex Tank)
{
	if (!VALIDATE_TANK(Tank))
		throw EContainer("Containers: UpdateTanks - Invalid param");
	UpdateTanksStatus(Tank);
}//UpdateTanks

/*******************************************************************************
// Class CContainerDummy implementation
*******************************************************************************/
// Dummy Constructor
CContainerDummy::CContainerDummy(const QString& Name): CContainerBase(Name)
{
	for(int i = FIRST_TANK_INCLUDING_WASTE_TYPE; i < m_lastWasteTank; i++)
	{
		m_TanksArray[i]->SetTankInserted(true);
		m_TanksArray[i]->SetTankEnabled(true);
		m_TanksArray[i]->SetTankWeight(m_ParamsMgr->CartridgeEmptyWeight * (i + 2000));
	}
	Init();
	//Loading the materials to ModesManager
  CModesManager::Instance()->EnumerateMaterialModes();
}

void CContainer::TanksStatusLogic(TTankIndex TankIndex)
{
	CQSingleContainer* ContainerPtr  = NULL;
	QString MachineMaterialType      = "";

	int     FEMessageID              = 0;
	int     FEEnabledControlID       = 0;
	ContainerPtr                     = m_TanksArray[TankIndex];

	// Get the current resin type in the pipe system and Tanks:
	QString PipeMaterialType   = ContainerPtr->GetMaterialType();
	QString TankMaterialType   = ContainerPtr->GetTankMaterialType();
	
	
	FEMessageID                = ContainerPtr->GetFEMessageID();
	FEEnabledControlID         = ContainerPtr->GetFEEnabledControlID();

#ifdef DONT_USING_RFID
	//if(!m_ParamsMgr->USING_RFID)
	//{
	FrontEndInterface->HideTankIDNoticeDlg(FEMessageID);
	ContainerPtr->SetTankEnabled(true);
	FrontEndInterface->UpdateStatus(FEEnabledControlID, FE_TANK_ENABLED_STATUS, false);
	//}
	//else
	//{
#else /*{*/
	// Handle No Tank:
	if (TankMaterialType == "")
	{
		// Closes any dialog related to this tank (if it was open):
		FrontEndInterface->HideTankIDNoticeDlg(FEMessageID);

		// Although tanks are Disabled, we don't need the "Disabled" icon since tanks are out.
		// (so we send an Enabled notification)
		FrontEndInterface->UpdateStatus(FEEnabledControlID, FE_TANK_ENABLED_STATUS, false);

		ContainerPtr->SetTankEnabled(false);
	}
	// Handle Non Objet resin:
	else if (TankMaterialType == "Unknown")
	{
		bool DialogAcked = ContainerPtr->GetDialogAcked();
		bool ForceUsingTank = ContainerPtr->GetForceUsingTank();
		if (DialogAcked == false)
		{
			ContainerPtr->LogUserTampering(NO_RF_TAG);

			// Issue the nasty message:
			FrontEndInterface->ShowTankIDNotice(FE_TANK_ID_NOTICE_DLG_NO_TAG, FEMessageID, false);

			// Disable tank:
			ContainerPtr->SetTankEnabled(false);

			FrontEndInterface->UpdateStatus(FEEnabledControlID, FE_TANK_DISABLED_STATUS, false);
		}
		// Check if user has agreed to work without warranty:
		else if (ForceUsingTank == true)
		{
			ContainerPtr->LogUserTampering(NO_RF_TAG_FORCED_PRINT);
			ContainerPtr->SetTankEnabled(true);
			FrontEndInterface->UpdateStatus(FEEnabledControlID, FE_TANK_ENABLED_STATUS, false);
		}
	}
	//}
#endif /*}*/
}

void CContainerDummy::TanksStatusLogic(TTankIndex TankIndex)
{
}

void CContainerDummy::UpdateChambers(bool UpdateActiveTherms)
{
    int WeightOffline;

	for(int i = FIRST_TANK_TYPE; i < m_numOfTanks; i++)
	{
		FrontEndInterface->UpdateStatus(FE_UPDATE_RESIN_TYPE_BASE + i, m_TanksArray[i]->GetMaterialType());
		WeightOffline = m_ParamsMgr->WeightOfflineArray[TankToStaticChamber(static_cast<TTankIndex>(i))];
		FrontEndInterface->UpdateStatus(FE_CURRENT_TANK_WEIGHT_BASE + i, WeightOffline);
		FrontEndInterface->UpdateStatus(FE_CURRENT_TANK_RELATIVE_WEIGHT_BASE + i, WeightOffline / m_ParamsMgr->CartridgeFullWeight);
	}

    WeightOffline = m_ParamsMgr->WeightOfflineArray[TankToStaticChamber(static_cast<TTankIndex>(FIRST_WASTE_TANK_TYPE))];
    FrontEndInterface->UpdateStatus(FE_CURRENT_TANK_WEIGHT_BASE + FIRST_WASTE_TANK_TYPE, WeightOffline);
    FrontEndInterface->UpdateStatus(FE_CURRENT_TANK_RELATIVE_WEIGHT_BASE + FIRST_WASTE_TANK_TYPE, WeightOffline / m_ParamsMgr->CartridgeFullWeight);
    if (m_ParamsMgr->DualWasteEnabled)
    {
        WeightOffline = m_ParamsMgr->WeightOfflineArray[TankToStaticChamber(static_cast<TTankIndex>(TYPE_TANK_WASTE_RIGHT))];
        FrontEndInterface->UpdateStatus(FE_CURRENT_TANK_WEIGHT_BASE + TYPE_TANK_WASTE_RIGHT, WeightOffline);
        FrontEndInterface->UpdateStatus(FE_CURRENT_TANK_RELATIVE_WEIGHT_BASE + TYPE_TANK_WASTE_RIGHT, WeightOffline / m_ParamsMgr->CartridgeFullWeight);
    }
	CContainerBase::UpdateChambers(UpdateActiveTherms);
}

// Destructor
// -------------------------------------------------------
CContainerDummy::~CContainerDummy(void)
{}

// Dummy procedures for CContainerDummy
//Model on sensor procedures

TQErrCode CContainerDummy::GetLiquidTankInsertedStatusFiltered(int /* TTankIndex */ TanksFilter)
{
	return Q_NO_ERROR;
}

TQErrCode CContainerDummy::GetLiquidTankInsertedStatus()
{
	return Q_NO_ERROR;
}

TQErrCode CContainerDummy::GetIfAllLiquidTankAreInserted(void)
{
	return Q_NO_ERROR;
}

int CContainerDummy::GetTotalWeight(int Chamber)
{
	int Weight = 0;
	if (VALIDATE_CHAMBER(Chamber))
		Weight = m_ParamsMgr->WeightOfflineArray[Chamber] +
				 m_ParamsMgr->WeightLevelLimitArray[Chamber];
	return Weight;
}

int CContainerDummy::GetWeight(/* TChamberIndex */int Chamber, /* TTankIndex */int Tank)
{
#ifdef _DEBUG
	if (FindWindow(0, QFormatStr( "Tank%dEmpty.txt - Notepad", (int)Tank ).c_str() ) )
		return 0;
	if (FindWindow(0, "TanksEmpty.txt - Notepad") )
		return 0;
	if (FindWindow(0, QFormatStr( "Chamber%dEmpty.txt - Notepad", (int)TankToStaticChamber(static_cast<TTankIndex>(Tank))).c_str()))
		return 0;
	if (FindWindow(0, "ChambersEmpty.txt - Notepad") )
		return 0;
#endif
	return CContainerBase::GetWeight(Chamber, Tank);
}//GetWeight

float CContainerDummy::GetTankWeightInA2D(int /* TTankIndex */ Tank)
{
#ifdef _DEBUG
	if (FindWindow(0, QFormatStr( "Tank%dEmpty.txt - Notepad", (int)Tank ).c_str() ) )
		return 0;
	if (FindWindow(0, "TanksEmpty.txt - Notepad") )
		return 0;
	if (FindWindow(0, QFormatStr( "Chamber%dEmpty.txt - Notepad", (int)TankToStaticChamber(static_cast<TTankIndex>(Tank))).c_str() ) )
		return 0;
	if (FindWindow(0, "ChambersEmpty.txt - Notepad") )
		return 0;
#endif
	return CContainerBase::GetTankWeightInA2D(Tank);
}

float CContainerDummy::GetTankWeightInGram(int /* TTankIndex */ Tank)
{
#ifdef _DEBUG
	if (FindWindow(0, QFormatStr( "Tank%dEmpty.txt - Notepad", Tank ).c_str() ) )
		return 0;
	if (FindWindow(0, "TanksEmpty.txt - Notepad") )
		return 0;
	if (FindWindow(0, QFormatStr( "Chamber%dEmpty.txt - Notepad", static_cast<int>(TankToStaticChamber(static_cast<TTankIndex>(Tank)))).c_str() ) )
		return 0;
	if (FindWindow(0, "ChambersEmpty.txt - Notepad") )
		return 0;
#endif
	return CContainerBase::GetTankWeightInGram(Tank);
}
bool CContainerDummy::IsWeightOk(/* TChamberIndex */ int Chamber)
{
#ifdef _DEBUG
	if (FindWindow(0, QFormatStr( "Chamber%dEmpty.txt - Notepad", (int)Chamber).c_str()) )
		return false;
	if (FindWindow(0, "ChambersEmpty.txt - Notepad") )
		return false;
#endif
	return CContainerBase::IsWeightOk(Chamber);
}

bool CContainerDummy::GetWasteWeightStatusDuringPrinting()
{
#ifdef _DEBUG
	if (FindWindow(0, "WasteTankFull.txt - Notepad"))
		return false;
#endif
	return true;
}

unsigned int CContainerDummy::GetLastInsertionTimeInTicks(int /* TTankIndex */)
{
	return QGetTicks() - QMinutesToTicks(CAppParams::Instance()->EmptyCartridgeDetectionDelayInSec) - 2000;
}

bool CContainerDummy::GetWasteWeightStatusBeforePrinting()
{
	return true;
}

bool CContainerDummy::SetContainerGain(int /* TTankIndex */ Tank, float Gain)
{
	// This function does nothing in the dummy version
	return true;
}


bool CContainerDummy::SetContainerOffset(int /* TTankIndex */ Tank, float Offset)
{
	// This function does nothing in the dummy version
	return true;
}

TQErrCode CContainerBase::PrepareSetDrainPumpsMessage(TOCBSetDrainPumpsMsg* SetDrainPumpsMsg, TTankIndex* DrainPumps, bool PrintToLog)
{
	memset(SetDrainPumpsMsg, 0, sizeof(TOCBSetDrainPumpsMsg));
	SetDrainPumpsMsg->MessageID = OCB_SET_HEAD_FILLING_DRAIN_PUMPS;
	QString tmp = "Tanks to drain: ";

	for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
	{
		tmp += ChamberToStr((TChamberIndex)i) + " = " + TankToStr(DrainPumps[i]) + "; ";

		switch(DrainPumps[i])
		{
				// TODO 5 -oPublic -cMaintainability : !!! There is an intentional swap between tank1 and tank2 indices here because it is swapped throughout the system. This is how it works today. Should be fixed.
			case TYPE_TANK_SUPPORT1:
				SetDrainPumpsMsg->SupportTank2PumpID = 1;
				break;
			case TYPE_TANK_SUPPORT2:
				SetDrainPumpsMsg->SupportTank1PumpID = 1;
				break;
//OBJET_MACHINE check!!!
			case TYPE_TANK_MODEL1:
			case TYPE_TANK_MODEL2:
				SetDrainPumpsMsg->ModelTank1PumpID   = 1;
				break;
			case TYPE_TANK_MODEL3:
			case TYPE_TANK_MODEL4:
				SetDrainPumpsMsg->ModelTank2PumpID   = 1;
				break;
			case TYPE_TANK_MODEL5:
			case TYPE_TANK_MODEL6:
				SetDrainPumpsMsg->ModelTank3PumpID   = 1;
				break;
		}
		FrontEndInterface->UpdateStatus(FE_SET_DRAIN_PUMPS_BASE + i, (int)DrainPumps[i]); // todo -oNobody -cNone: Don't update the status before you sure the operation succeeded
	}
	if (PrintToLog)
		CQLog::Write(LOG_TAG_EOL, tmp);

	return (Q_NO_ERROR);
}

TQErrCode CContainerDummy::SetDrainTanks(TTankIndex* DrainPumps, bool PrintToLog)
{
	TQErrCode ret;
	SetDrainTankMutex.WaitFor();
	TOCBSetDrainPumpsMsg SetDrainPumpsMsg;
	ret = PrepareSetDrainPumpsMessage(&SetDrainPumpsMsg, DrainPumps, PrintToLog);
	SetDrainTankMutex.Release();
	return ret;
}

bool CContainerDummy::SetActiveContainersBeforePrinting(bool PrintToLog)
{
	return true;
}

// This procedure set the Active Pumps
//-------------------------------------------
bool CContainerDummy::AreRelevantTanksInserted(TChamberIndex Chamber, TTankIndex Tank)
{
#ifdef _DEBUG
	if (Tank == ACTIVE_TANK && FindWindow(0, QFormatStr( "Tank%dOut.txt - Notepad", (int)GetActiveTankNum(Chamber) ).c_str() ) )
		return false;
	if (FindWindow(0, QFormatStr( "Tank%dOut.txt - Notepad", (int)Tank ).c_str() ) )
		return false;
	if (FindWindow(0, "TanksOut.txt - Notepad") )
		return false;
#endif
	return CContainerBase::AreRelevantTanksInserted(Chamber, Tank);
}

bool CContainerDummy::IsTankInAndEnabled(/*TTankIndex*/int Tank)
{
#ifdef _DEBUG
	if (FindWindow(0, QFormatStr( "Tank%dOut.txt - Notepad", (int)Tank ).c_str() ) )
		return false;
	if (FindWindow(0, QFormatStr( "Tank%dDisable.txt - Notepad", (int)Tank ).c_str() ) )
		return false;
#endif
	return CContainerBase::IsTankInAndEnabled(Tank);
}


/*******************************************************************************
class CChamber implementation
*******************************************************************************/
CChamber::CChamber(void) :
	m_ActiveTank(NO_TANK),
	m_ActiveDrainTank(NO_TANK)
{
	m_ParamsMgr = CAppParams::Instance();
}//constructor

CChamber::~CChamber(void)
{}//destructor

void CChamber::AddTank(CQSingleContainer* Container, TTankIndex Tank)
{
	m_Tanks[Tank] = Container;
}//AddTank

int CChamber::GetTanksCount()
{
	return m_Tanks.size();
}

void CChamber::RemoveTank(TTankIndex Tank)
{
	m_Tanks.erase(Tank);
}//RemoveTank

void CChamber::ClearTanks(void)
{
	m_Tanks.clear();
	m_ActiveTank     = NO_TANK;
	m_ActiveDrainTank = NO_TANK;
}//ClearTanks

bool CChamber::IsLiquidWeightOk(void)
{
	TChamberIndex Chamber = GetType();
#ifdef _DEBUG
	if (FindWindow(0, QFormatStr( "Chamber%dEmpty.txt - Notepad", (int)Chamber).c_str()) )
		return Q2RT_CONTAINERS_SUPPORT_EMPTY;
#endif
	return (GetTotalWeight() > m_ParamsMgr->WeightLevelLimitArray[Chamber]);
}//IsLiquidWeightOk

int CChamber::GetTotalWeight()
{
	int       TankWeight;
	int       TotalWeight = 0;

	// Iterating through all the tanks - trying to find a suitable tank to switch to
	for(TTankToPtrMapIter Iter = m_Tanks.begin(); Iter != m_Tanks.end(); ++Iter)
	{
		if((Iter == NULL) || (Iter->second == NULL))
			continue;
		// if this tank isn't Enabled - goto next tank
		if (Iter->second->IsTankEnabled() != true)
			continue;

		// if this tank isn't inserted - goto next tank
		if (Iter->second->IsTankInserted() != true)
			continue;

		TankWeight = GetTankWeightInGram(Iter->second);

		// if there isn't enough material in the tank - continue
		if (TankWeight <= m_ParamsMgr->WeightLevelLimitArray[m_Type])
			continue;

		// The tank is Enabled, inserted and has enough resin
		TotalWeight += TankWeight - m_ParamsMgr->WeightLevelLimitArray[m_Type];
	}

	// Return the total weight of the liquid
	return TotalWeight;
}//GetTotalWeight

void CChamber::SelectTankForDraining(bool PrintToLog, bool IsWasteTankInserted, bool IsWasteTankFull)
{
	TTankIndex            Index;
	CQSingleContainer*    Tank                 = NULL;
	int                   TankWeight           = 0;
	bool                  IsTankInserted       = false;
	QString               DrainOutput;
	bool                  StopDrain            = false;

	for(TTankToPtrMapIter Iter = m_Tanks.begin(); Iter != m_Tanks.end(); ++Iter)
	{
		Tank  = Iter->second;
		Index = Iter->first;

		if (Tank != NULL && Tank->m_IsDrainNeeded)
		{
			TankWeight      = GetTankWeightInGram(Tank);
			IsTankInserted  = Tank->IsTankInserted();

			// If the current drain pump is off....
			if (! Tank->m_IsDrainActive)
			{
				// In case the drain pump was on for some reason (this actually happens sometimes)
				m_ActiveDrainTank     = NO_TANK;

				// Check whether we need to drain from this tank
				if ((TankWeight <= 0)			&&  // Tank weight in drain limits
						(IsTankInserted)			&&  // The tank is inserted
						(Index != m_ActiveTank)		&&  // Never drain liquid from the active tank
						(IsWasteTankInserted)		&&  // Waste tank inserted
						(! IsWasteTankFull))            // Waste tank is not full
				{
					if (QTicksToSeconds(QGetTicks() - Tank->GetLastInsertionTimeInTicks()) <      // Give some time to the user to release the tank and
							CAppParams::Instance()->EmptyCartridgeDetectionDelayInSec + 1) // to the weight sensor to response (+ 1 seconds to give
						continue;                                                                 // chance to other mechanisms (such as cartridge replacement
					// during printing) to react before we start draining).
					// Need to drain from this tank
					Tank->m_IsDrainActive = true; // todo -oNobody -cNone: why is it setting "active == true" before the pump is activated ?
					m_ActiveDrainTank     = Index;

					// When starting a pump, measure time:
					Tank->m_DrainTankStartTime = QGetTicks();

					// Updating Log
					DrainOutput = "Start drain from " + TankToStr(Index);
					CQLog::Write(LOG_TAG_HEAD_FILLING, DrainOutput);
					break; // Current code allows only one tank to be activated for draining for each chamber
				}
			}
			else
			{
				// If the current model drain pump is on....
				// Check whether we should stop pumping....
				if ((TankWeight > WEIGHT_HYSTERESIS) || // Tank weight is above hysteresys
						(! IsTankInserted)               || // Tank is not inserted
						(Index == m_ActiveTank)          || // Never drain from the active tank
						(! IsWasteTankInserted)          || // Waste tank not inserted
						(IsWasteTankFull))                  // Waste tank is full
				{
					StopDrain = true;
				}

				// Drain time has ended... stop draining
				if (QGetTicks() > (Tank->m_DrainTankStartTime + QMinutesToTicks(m_ParamsMgr->DrainTime)))
				{
					Tank->m_IsDrainNeeded = false;
					StopDrain = true;
				}

				if (StopDrain)
				{
					// Stop draining from this tank
					Tank->m_IsDrainActive = false;
					m_ActiveDrainTank     = NO_TANK;

					// Updating Log
					DrainOutput = "Stop drain from " + TankToStr(Index);
					CQLog::Write(LOG_TAG_HEAD_FILLING, DrainOutput);
				}
				else
				{
					// Current code allows only one tank to be activated for draining for each chamber.
					// We must end this loop with m_ActiveDrainTank set to the tank that should be drained, to keep the drain pump active (otherwise, SetDrainTanks will turn the pump off).
					m_ActiveDrainTank = Index;
					// Tank->m_IsDrainActive   = true;
					break;
				}
			}

			CBackEndInterface::Instance()->EnableDisableMaintenanceCounter(GetDrainPumpCounterID(Index), GetActiveDrainTank() == NO_TANK); // todo -oNobody -cNone: should be adapted to the new code of "Draining bugs" branch
		} // end for
	}
}

bool CChamber::IsActiveLiquidTankInserted(void)
{
	return AreRelevantTanksInserted(ACTIVE_TANK);
}//IsActiveLiquidTankInserted

bool CChamber::IsActiveLiquidTankEnabled(void)
{
	if(GetActiveTank() == NO_TANK)
		return false;

	if(NULL == m_Tanks[GetActiveTank()])
		return false;

	return m_Tanks[GetActiveTank()]->IsTankEnabled();
}//IsActiveLiquidTankEnabled

TQErrCode CContainerDummy::NotifyMSEvent(TTankIndex tank, bool InOut)
{
	return Q_NO_ERROR;
}
void CContainerDummy::Execute(void)
{}

void CContainerDummy::HandleTankIdentificationDlgClose(TTankIndex TankIndex, int Result)
{
	if (Result == FE_TANK_ID_NOTICE_DLG_RESULT_DISABLE_TANK)
		return;

	else if ((Result == FE_TANK_ID_NOTICE_DLG_RESULT_ENABLE_TANK_SINGLEMODE) || (Result == FE_TANK_ID_NOTICE_DLG_RESULT_ENABLE_TANK_DMMODE) || (Result == FE_TANK_ID_NOTICE_DLG_RESULT_ENABLE_TANK_NO_TAG))
	{
		FrontEndInterface->UpdateStatus(FE_TANK_ENABLING_STATUS_BASE, FE_TANK_ENABLED_STATUS, false);
	}
}

TQErrCode CContainerDummy::SendTanksRemovalSignals()
{
	return Q_NO_ERROR;
}

TQErrCode CContainerDummy::IgnoreTankInsertionAndRemovalSignals()
{
	return Q_NO_ERROR;
}

TQErrCode CContainerDummy::AllowTankInsertionAndRemovalSignals()
{
	return Q_NO_ERROR;
}

// Called at startup and after statemachine reset to generate MS In events.
TQErrCode CContainerDummy::InitializeMSEventsNotifications()
{
	return Q_NO_ERROR;
}

QString CContainerDummy::GetTagIDAsString(int CartridgeID)
{
	return "00000000";
}

QString CContainerDummy::GetPipeMaterialType(TTankIndex TankIndex)
{
	return "ContainerDummy";
}

unsigned long CContainerDummy::GetContainerExpirationDate(TTankIndex TankIndex)
{
	return 0;
}

bool CContainerDummy::IsResinExpired(TTankIndex TankIndex)
{
	return false;
}

bool CContainerDummy::IsMaxPumpTimeExceeded(TTankIndex TankIndex)
{
	return false;
}

void CContainerDummy::SetSkipMachineResinTypeChecking(TTankIndex TankIndex, bool val)
{

}
void CContainerDummy::SetAllowServiceMaterials(TTankIndex TankIndex, bool val)
{
	return m_TanksArray[TankIndex]->SetAllowServiceMaterials(val);

}

bool CContainerDummy::IsWasteFull()
{
#ifdef _DEBUG
	if (FindWindow(0, "WasteTankFull.txt - Notepad"))
		return true;
#endif
	return false;
}

TTankIndex CChamber::GetTankToActivate(int HysteresisConstInit)
{
	TTankIndex         ret             = NO_TANK;
	int                MinTankWeight   = INT_MAX;
	int                MaxTankWeight   = INT_MIN;
	int                TankWeight      = 0;
	int                HysteresisConst;
	TTankToPtrMapIter  Iter;
	CQSingleContainer* Tank;

	// If this method is called before Tanks array is built (may happen during startup), return 'NO_TANK'.
	if (m_Tanks.empty())
		return NO_TANK;

	for(Iter = m_Tanks.begin(); Iter != m_Tanks.end(); ++Iter)
		// Iterating through all the tanks - trying to find a suitable tank to switch to
	{
		HysteresisConst = HysteresisConstInit;

		if (CHECK_EMULATION(m_ParamsMgr->OCB_Emulation))
			return Iter->first;

		Tank = Iter->second;
		if (!Tank)
			return NO_TANK;

		// if this tank isn't Enabled - goto next tank
		if (true != Tank->IsTankEnabled())
			continue;

		// if this tank isn't inserted - goto next tank
		if (true != Tank->IsTankInserted())
			continue;

		/* Hysteresys... if this is not the active tank and it wasn't used before
		  - use an extra 60 gr*/
		if( Tank->IsItContainerFirstUse() || Iter->first == GetActiveTank() )
			HysteresisConst = 0;

		TankWeight = GetTankWeightInGram(Tank);

		if(m_Type == TYPE_CHAMBER_WASTE)
		{
			// if current waste tank is full - continue
			if (TankWeight >= m_ParamsMgr->WeightLevelLimitArray[m_Type] + HysteresisConst)
				continue;

			if (TankWeight > MaxTankWeight)
			{
				// The tank is Enabled, inserted and has larger weight between the two tanks.
				ret           = Iter->first;
				MaxTankWeight = TankWeight;
			}
		}
		else
		{
			// if there isn't enough material in the tank - continue
			if (TankWeight <= m_ParamsMgr->WeightLevelLimitArray[m_Type] + HysteresisConst)
				continue;

			if (TankWeight < MinTankWeight)
			{
				// The tank is Enabled, inserted and has enough resin - return it.
				ret         = Iter->first;
				MinTankWeight = TankWeight;
			}
		}
	}

	// Mark that this container was already used
	if (NO_TANK != ret)
		m_Tanks[ret]->SetContainerAsUsed();
	else if (HysteresisConst != 0)
		ret = GetTankToActivate(0);
	return ret;
}//GetTankToActivate

int CChamber::GetTankWeightInGram(CQSingleContainer* Tank)
{
	return Tank->GetTankWeightInGram(/*m_Type == TYPE_CHAMBER_WASTE*/);
}

int CChamber::GetWeightInGram(TTankIndex Tank)
{
	TTankToPtrMapIter Iter;
	int               Weight = 0;

	//objet_machine test
	if (FindWindow(0, QFormatStr( "Tank%dEmpty.txt - Notepad", (int)Tank ).c_str() ) )
		return 0;

	switch(Tank)
	{
		case ALL_TANKS:
			Weight = 0;
			for(Iter = m_Tanks.begin(); Iter != m_Tanks.end(); ++Iter)
				if(Iter != NULL)
					Weight += GetTankWeightInGram(Iter->second);
			break;
		case ACTIVE_TANK:
			Tank = m_ActiveTank;
			/*no break command intentionally*/
		default:
			if ((Iter = m_Tanks.find(Tank)) != m_Tanks.end())
				Weight = GetTankWeightInGram(Iter->second);
	}

	return Weight;
}//GetWeightInGram

int CChamber::GetActiveTankWeightInGram(void)
{
	int Weight = 0;
	if(GetActiveTank() != NO_TANK)
		Weight = GetTankWeightInGram(m_Tanks[GetActiveTank()]);
	return Weight;
}//GetActiveTankWeightInGram

bool CChamber::AreRelevantTanksInserted(TTankIndex Tank)
{
	bool           ret = false;
	TTankToPtrMapIter Iter;

	switch(Tank)
	{
		case ACTIVE_TANK:
			if ((Iter = m_Tanks.find(m_ActiveTank)) != m_Tanks.end())
				if( Iter != NULL)
					ret = Iter->second->IsTankInserted();
			break;
		case ALL_TANKS:
			for(Iter = m_Tanks.begin(); Iter != m_Tanks.end(); ++Iter)
				ret &= Iter->second->IsTankInserted();
			break;
		default:
			if ((Iter = m_Tanks.find(Tank)) != m_Tanks.end())
				ret = Iter->second->IsTankInserted();
	}

	return ret;
}//AreRelevantTanksInserted

//Setters/Getters function
TChamberIndex CChamber::GetType(void)
{
	return m_Type;
}

TTankIndex CChamber::GetActiveTank(void)
{
	TTankIndex ActiveTank;

	m_MutexSetActiveTank.WaitFor();
	ActiveTank = m_ActiveTank;
	m_MutexSetActiveTank.Release();

	return ActiveTank;
}

TTankIndex CChamber::GetActiveDrainTank(void)
{
	return m_ActiveDrainTank;
}

//Set methods
void CChamber::SetType(TChamberIndex Type)
{
	m_Type = Type;
}

void CChamber::SetActiveTank(TTankIndex ActiveTank)
{
	m_MutexSetActiveTank.WaitFor();
	m_ActiveTank = ActiveTank;
	m_MutexSetActiveTank.Release();
}

void CChamber::SetActiveDrainTank(TTankIndex ActiveDrainTank)
{
	m_ActiveDrainTank = ActiveDrainTank;
}

bool CChamber::IsChambersTank(TTankIndex Tank)
{
	return (m_Tanks.find(Tank) != m_Tanks.end());
}

// Constructor
CContainersDispatcher::CContainersDispatcher()
	: CQThread(true, "CContainersDispatcherTask", true)
{
	m_ContainerPtr = NULL;
	m_SMContainerIDEventQueue = new TContainerIDEventQueue(SM_EVENT_QUEUE_MAX_MESSAGES, "", false);
	m_SMContainerIDEventQueue->Name() = "EventQueueForContainers";
}

// Destructor
CContainersDispatcher::~CContainersDispatcher()
{
	// Mark terminate and release the queue
	Terminate();

	m_SMContainerIDEventQueue->Release();

	// Wait for thread termination. We need the YieldUIThread() since this D'tor
	// is called from the context of the GUI thread. And since the
	// CContainersDispatcher thread (which we here wait for to finish) waits at
	// a certain point for SendMessage() - application may become deadlocked.
	while (WaitFor(100) != QLib::wrSignaled)
		FrontEndInterface->YieldUIThread();

	// Free the message queue
	delete m_SMContainerIDEventQueue;
}


bool CContainersDispatcher::SendNotification(TContainerIdentifyNotificationMessage& Message)
{
	// if the queue is handled slowly, filling it with Timer Notifications might cause important messages (RF_ADD) to be lost.
	if (Message.MessageBody.EventType == TIMER)
		if ((m_SMContainerIDEventQueue->ItemsCount) > (SM_EVENT_QUEUE_MAX_MESSAGES / 2))
			return true;

	if(m_SMContainerIDEventQueue->Send(Message, QMsToTicks(RDR_QUEUE_SEND_TIMEOUT)) == QLib::wrTimeout)
	{
		QMonitor.Print("Container Tag ID queue timeout");
		return false;
	}

	return true;
}

void CContainersDispatcher::Execute(void)
{

	TContainerIdentifyNotificationMessage Message;

	do
	{
		memset (&Message, 0, sizeof(TContainerIdentifyNotificationMessage));

		if(m_SMContainerIDEventQueue->Receive(Message) == QLib::wrReleased)
		{
			break;
		}

        if (Terminated)
        {
            break;
        }

		try
		{
			m_ContainerPtr->SendNotification(Message);
		}
		catch(...)
		{
			throw EContainer("CContainersDispatcher: Execute() failed.");
		}
	}
	while(!Terminated);
}

void CContainer::CheckWasteAndActivateAlertIfNeeded()
{
	// Display a service alert in case the waste cartride is full and blink yellow signal on signal tower
	if ( IsWasteFull() )
	{
		if ( !m_WasteAlert )
		{
			FrontEndInterface->UpdateStatus(FE_SERVICE_ALERT, WASTE_CARTRIDGE_ALERT_ID, true);
			Q2RTApplication->GetMachineManager()->GetMachineSequencer()->ActivateSignalTower(ST_LIGHT_NC, ST_LIGHT_NC, ST_LIGHT_BLINK);
			m_WasteAlert  = true;
			SetWasteFull(true);
		}

		// Cancel the running wizard if waste is active and full
		if(IsWasteActive())
			FrontEndInterface->CancelWizard("Waste cartridge is full");
	}
	else
	{
		if( m_WasteAlert )
		{
			FrontEndInterface->UpdateStatus(FE_CLEAR_SERVICE_ALERT, 0, true);
			Q2RTApplication->GetMachineManager()->GetMachineSequencer()->ActivateSignalTower(ST_LIGHT_NC, ST_LIGHT_NC, ST_LIGHT_OFF);
			m_WasteAlert  = false;
			SetWasteFull(false);
		}
	}
}