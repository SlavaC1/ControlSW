#pragma hdrstop
#include "ContainerBase.h"
#pragma package(smart_init)

#include "ModesManager.h"
#include "Q2RTApplication.h"
#include "MachineSequencer.h"
#include "HeadFilling.h"


/*******************************************************************************
  Class CContainerBase implementation
*******************************************************************************/
// Constructor - create the thread in suspend mode
// ------------------------------------------------------
CContainerBase::CContainerBase(const QString& Name) : CQComponent(Name)
{
	m_Waiting = false;

	INIT_METHOD(CContainerBase,IsActiveLiquidTankInserted);
	INIT_METHOD(CContainerBase,IsActiveLiquidTankEnabled);
	INIT_METHOD(CContainerBase,SetAllLiquidTanksEnabled);
	INIT_METHOD(CContainerBase,GetIfAllLiquidTankAreInserted);
	INIT_METHOD(CContainerBase,GetIfAllLiquidTankAreEnabled);
	INIT_METHOD(CContainerBase,GetLiquidTankInsertedStatus);
	INIT_METHOD(CContainerBase,GetLiquidTankInsertedStatusFiltered);
	INIT_METHOD(CContainerBase,IsWeightOk);
	INIT_METHOD(CContainerBase,GetWasteWeightStatusDuringPrinting);
	INIT_METHOD(CContainerBase,GetWasteWeightStatusBeforePrinting);

	INIT_METHOD(CContainerBase,SetContainerGain);
	INIT_METHOD(CContainerBase,SetContainerOffset);

	INIT_METHOD(CContainerBase,SetActiveContainersBeforePrinting);
	INIT_METHOD(CContainerBase,DrainIfNeeded);

	INIT_METHOD(CContainerBase,GetLiquidTanksWeight);

	INIT_METHOD(CContainerBase,GetWeight);
	INIT_METHOD(CContainerBase,GetRemainingWasteWeight);
	INIT_METHOD(CContainerBase,GetTankWeightInA2D);
	INIT_METHOD(CContainerBase,GetTankWeightInGram);

	INIT_METHOD(CContainerBase,GetActiveTankNum);
	INIT_METHOD(CContainerBase,GetModelResinType);
	INIT_METHOD(CContainerBase,GetSupportResinType);

	INIT_METHOD(CContainerBase,GetTotalWeight);
	INIT_METHOD(CContainerBase,RemountSingleTankStatus);
	INIT_METHOD(CContainerBase,WaitForIdentificationCompletion);

	m_OCBClient = COCBProtocolClient::Instance();
	m_ParamsMgr = CAppParams::Instance();

	//Instance to error handler
	m_ErrorHandlerClient = CErrorHandler::Instance();

	CModesManager::Instance()->EnumerateMaterialModes();

	for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
	{
		m_ChambersArray[i].SetType((TChamberIndex)i);
		m_ChambersArray[i].SetActiveTank(static_cast<TTankIndex>(m_ParamsMgr->ActiveTanks[i].Value()));
	}
	for(int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; i++)
	{
		int InplaceSensorTag = GetInplaceTagByContainer(static_cast<TTankIndex>(i));

		m_TanksArray[i] = new CQSingleContainer(m_ParamsMgr->CartridgeFullWeight,
		                                        m_ParamsMgr->CartridgeEmptyWeight,
		                                        (TTankIndex)i,
		                                        InplaceSensorTag);
		m_TanksArray[i]->SetTankGain(m_ParamsMgr->WeightSensorGainArray[i]);
		m_TanksArray[i]->SetTankOffset(m_ParamsMgr->WeightSensorOffsetArray[i]);
		m_TanksArray[i]->SetMaterialType(m_ParamsMgr->TypesArrayPerPipe[i]);
		m_TanksArray[i]->SetTankMaterialType(m_ParamsMgr->TypesArrayPerTank[i]);
		m_TanksArray[i]->m_IsDrainNeeded = true;
		m_TanksArray[i]->m_IsDrainActive = false;
		m_TanksArray[i]->SetContainerPtr(this);
	}

// todo -oShahar.Behagen@objet.com -cNone: add this name to actual c'tor) m_TanksArray[TYPE_TANK_WASTE] = new CQSingleContainer( QReplaceSubStr(TankToStr(TYPE_TANK_WASTE), " ", "")
	m_TanksArray[TYPE_TANK_WASTE] = new CQSingleContainer(0   // was: m_ParamsMgr->WasteFullContainerWeight. todo: remove it completely.
	        ,m_ParamsMgr->WasteCartridgeEmptyWeight
	        ,TYPE_TANK_WASTE
	        ,-1);   // a fake sensor tag


	m_TanksArray[TYPE_TANK_WASTE]->SetTankEnabled(true);
	m_TanksArray[TYPE_TANK_WASTE]->SetTankGain(m_ParamsMgr->WeightSensorGainArray[TYPE_TANK_WASTE]);
	m_TanksArray[TYPE_TANK_WASTE]->SetTankOffset(m_ParamsMgr->WeightSensorOffsetArray[TYPE_TANK_WASTE]);
	m_WasteIsFull = false;
	m_ChambersArray[TYPE_CHAMBER_WASTE].SetType(TYPE_CHAMBER_WASTE);
	m_ChambersArray[TYPE_CHAMBER_WASTE].AddTank(m_TanksArray[TYPE_TANK_WASTE], TYPE_TANK_WASTE);
	m_ChambersArray[TYPE_CHAMBER_WASTE].SetActiveTank(TYPE_TANK_WASTE);

//OBJET_MACHINE
	m_batchNoTable = CBatchNoTable::GetInstance();
}

void CContainerBase::Init()
{}  // Don't need the UpdateTanksStatus() here, it is updated through RFID.

// Destructor
// -------------------------------------------------------
CContainerBase::~CContainerBase(void)
{
	for(int i = 0; i < TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE; i++)
	{
		if(m_TanksArray[i])
			delete m_TanksArray[i];
	}
}

bool CContainerBase::DrainIfNeeded(bool PrintToLog)
{
	// Go over all the tanks and check whether  drain should be activated
	TTankIndex DrainPumps[NUMBER_OF_CHAMBERS];
	for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
	{
		m_ChambersArray[i].SelectTankForDraining(PrintToLog, IsActiveLiquidTankInserted((int)TYPE_CHAMBER_WASTE), m_WasteIsFull);

		DrainPumps[i] = m_ChambersArray[i].GetActiveDrainTank();
	}

	SetDrainTanks(DrainPumps, PrintToLog);

	return true;
}//DrainIfNeeded

bool CContainerBase::IsTankInAndEnabled(TTankIndex Tank)
{
	if(!VALIDATE_TANK_INCLUDING_WASTE(Tank))
		throw EContainer("'IsTankInAndEnabled' Error: invalid parameter");

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
	if(!GetWasteWeightStatusDuringPrinting())
		return Q2RT_CONTAINERS_WASTE_FULL;

	for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
	{
		if(!m_ChambersArray[i].IsLiquidWeightOk())
			return (IS_SUPPORT_CHAMBER(i) ? Q2RT_CONTAINERS_SUPPORT_EMPTY : Q2RT_CONTAINERS_MODEL_EMPTY);
	}
	return Q_NO_ERROR;
}//AreAllLiquidsWeightOk

bool CContainerBase::IsRelevantTankInserted(TTankIndex Tank)
{
	return AreRelevantTanksInserted(TankToStaticChamber(Tank), Tank);
}//IsRelevantTankInserted

bool CContainerBase::AreRelevantTanksInserted(TChamberIndex Chamber, TTankIndex Tank)
{
	//objet_machine test
	if(FindWindow(0, QFormatStr("Tank%dOut.txt - Notepad", (int)Tank).c_str()))
		return false;
	if(Tank != ACTIVE_TANK && !VALIDATE_CHAMBER_INCLUDING_WASTE(Chamber))
		throw EContainer("'AreRelevantTanksInserted' Error: invalid parameter");
	return m_ChambersArray[Chamber].AreRelevantTanksInserted(Tank);
}//AreRelevantTanksInserted

void CContainerBase::UpdateTanksStatus(TTankIndex Tank)
{
	// Set Pipe material according to the updated Parameter:
	// Notice that the TypesArrayPerTank parameter (below) is changed via Material Mode activation, and not "programatically".
	m_TanksArray[Tank]->SetMaterialType(m_ParamsMgr->TypesArrayPerPipe[Tank]);

	if(CHECK_EMULATION(m_ParamsMgr->OCB_Emulation) && (m_TanksArray[Tank]->GetMaterialType() == "UNDEFINED"))   // really ugly patch just to allow QAing the silent MRW.
	{
		m_TanksArray[Tank]->SetTankEnabled(false);
		FrontEndInterface->UpdateStatus(FE_TANK_DISABLING_HINT_BASE +Tank, QString("Material Mismatch"), true);
		FrontEndInterface->UpdateStatus(FE_TANK_ENABLING_STATUS_BASE+Tank, FE_TANK_DISABLED_STATUS, true);
	}              

	RemountSingleTankStatus(Tank);

	// Saving parameters in the parameter manager
	m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->TypesArrayPerTank);
}//UpdateTanksStatus

bool CContainerBase::IsChambersTank(TChamberIndex Chamber, TTankIndex Tank)
{
	if(!VALIDATE_CHAMBER_INCLUDING_WASTE(Chamber) ||
	        !VALIDATE_TANK_INCLUDING_WASTE(Tank))
		throw EContainer("'IsChambersTank' Error: invalid parameter");
	return m_ChambersArray[Chamber].IsChambersTank(Tank);
}//IsChambersTank

// In case there is 2nd tank and it's ready (not empty and inserted) set it as active tank.
TQErrCode CContainerBase::SwitchToNewTank(TChamberIndex Chamber)
{
	if(!VALIDATE_CHAMBER_INCLUDING_WASTE(Chamber))
		throw EContainer("'SwitchToNewTank' Error: invalid parameter");
	//decide which tank should be the active tank (decided upon its weight)
	TTankIndex Tank = m_ChambersArray[Chamber].GetTankToActivate();
	if(NO_TANK == Tank)
		return Q2RT_NO_AVAILABLE_LIQUID_TANK;

	TTankIndexArray ActiveTanks;
	for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
		ActiveTanks[i] = m_ChambersArray[i].GetActiveTank();
	ActiveTanks[Chamber] = Tank;

// todo -oShahar.Behagen@objet.com -cNone: see if this SwitchToNewTank() function could be dumped completly. if not it should look at IsTankEnabled()
	SetActiveTank(ActiveTanks);

	return Q_NO_ERROR;
}//SwitchToNewTank

TTankIndex CContainerBase::GetActiveTankNum(int /* TChamberIndex */ Chamber)
{
	if(!VALIDATE_CHAMBER_INCLUDING_WASTE(Chamber))
		return NO_TANK;

	return m_ChambersArray[Chamber].GetActiveTank();
}//GetActiveTank

void CContainerBase::SetActiveDrainTank(TChamberIndex Chamber, TTankIndex DrainTank)
{
	if(!VALIDATE_CHAMBER(Chamber))
		throw EContainer("'SetActiveDrainTank' Error: invalid parameter");
	return m_ChambersArray[Chamber].SetActiveDrainTank(DrainTank);
}//SetActiveDrainTank

TTankIndex CContainerBase::GetActiveDrainTank(TChamberIndex Chamber)
{
	if(!VALIDATE_CHAMBER(Chamber))
		throw EContainer("'GetActiveDrainTank' Error: invalid parameter");
	return m_ChambersArray[Chamber].GetActiveDrainTank();
}//GetActiveDrainTank

TQErrCode CContainerBase::SetActiveTank(TTankIndexArray& TankArray, bool PrintToLog)
{
	QString Output = "'SetActiveTank' ";
	for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
	{
		if(m_ChambersArray[i].GetActiveTank() != TankArray[i])
		{
			m_ChambersArray[i].SetActiveTank(TankArray[i]);
			FrontEndInterface->UpdateStatus(FE_SET_ACTIVE_TANK_BASE+TankArray[i], i, true);
			//Remove the previous active tank from GUI DataBase
			FrontEndInterface->RemoveStatus(FE_SET_ACTIVE_TANK_BASE+GetSiblingTank(TankArray[i]));
			Output = Output + ChamberToStr((TChamberIndex)i) + " Active Tank = " + TankToStr(TankArray[i]) + "; ";
			// Updating the Parameter Manager
			m_ParamsMgr->ActiveTanks[i] = TankArray[i];
		}
	}
	m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->ActiveTanks);
	// CQLog::Write(LOG_TAG_HEAD_FILLING,"SetActiveTank,Active Tanks: %d %d %d %d",m_ParamsMgr->ActiveTanks[0].Value(),m_ParamsMgr->ActiveTanks[1].Value(),m_ParamsMgr->ActiveTanks[2].Value(),m_ParamsMgr->ActiveTanks[3].Value());

	if(PrintToLog)
		CQLog::Write(LOG_TAG_EOL, Output);

	return Q_NO_ERROR;
}

//Set all active tanks batch numbers when printing starts
void CContainerBase::SetActiveTanksBatchNo()
{
	TTankIndex activeTank;
	for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
	{
		activeTank = GetActiveTankNum((TChamberIndex)i);
		m_batchNoTable->SetTankResinType(activeTank, GetTankMaterialType(activeTank));
		m_batchNoTable->SetTankBatchNo(activeTank, m_TanksArray[activeTank]->GetBatchNo());
	}
}

bool CContainerBase::GetIfAllLiquidTankAreEnabled(void)
{
	TQErrCode Err = Q2RT_LIQUID_TANKS_DISABLED_ERROR;
	bool allEnabled = true;

	for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
	{
		if(! m_ChambersArray[i].IsActiveLiquidTankEnabled())
		{
			allEnabled = false;
			if(m_AllLiquidTankEnabled)    // If there was a change in the Enabled status, and it became 'false' - report as an Error.
			{
				CQLog::Write(LOG_TAG_GENERAL, QFormatStr("%s: %s.", ChamberToStr((TChamberIndex)i).c_str(), PrintErrorMessage(Err).c_str()));
				m_ErrorHandlerClient->ReportError(PrintErrorMessage(Err), Err, static_cast<int>(i));
			}
			break;
		}
	}

	// Update the Machine Sequencer with current status if 'true'. ('false' is handled by the m_ErrorHandlerClient)
	if(true == (m_AllLiquidTankEnabled = allEnabled))
		Q2RTApplication->GetMachineManager()->GetMachineSequencer()->SetLiquidTanksEnabledStatus(m_AllLiquidTankEnabled);

	return m_AllLiquidTankEnabled;
}//GetIfAllLiquidTankAreEnabled

// Get liquid tanks weight
TQErrCode CContainerBase::GetLiquidTanksWeight(void)
{
	//Updating the Active Pumps
	SetActiveContainersBeforePrinting(false);
	//Updating the Drain Pumps
	if(!m_ParamsMgr->DrainBypass)
		DrainIfNeeded(false);
	GetIfAllLiquidTankAreEnabled(); // issues an error in case not all Tanks are enabled, to prevent other modules from using it. (e.g: headFilling)
	return Q_NO_ERROR;
}

CQMutex* CContainerBase::GetStateMachineMutex()
{
	return &m_StateMachineMutexGuard;
}

CQMutex* CContainerBase::GetNotificationDistributeMutex()
{
	return &m_NotificationDistributeMutexGuard;
}

//This procedure get if the Model and Support Tank is inserted.
bool CContainerBase::IsActiveLiquidTankInserted(int Chamber)
{
	if(!VALIDATE_CHAMBER_INCLUDING_WASTE(Chamber))
		throw EContainer("'IsActiveLiquidTankInserted' Error: invalid MaterialType");

	return AreRelevantTanksInserted(static_cast<TChamberIndex>(Chamber), ACTIVE_TANK);
}//IsActiveLiquidTankInserted

//This procedure get if the Model and Support Tank is inserted.
bool CContainerBase::IsActiveLiquidTankEnabled(int /* TChamberIndex */ Chamber)
{
	if(!VALIDATE_CHAMBER_INCLUDING_WASTE(Chamber))
		throw EContainer("'IsActiveLiquidTankEnabled' Error: invalid MaterialType");

	return m_ChambersArray[Chamber].IsActiveLiquidTankEnabled();
}//IsActiveLiquidTankEnabled

bool CContainerBase::SetAllLiquidTanksEnabled(bool Enabled)
{
	for(int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; i++)
	{
		m_TanksArray[i]->SetTankEnabled(Enabled);
	}

	return true;
}

int CContainerBase::GetInplaceTagByContainer(TTankIndex TankIndex)
{
	switch(TankIndex)
	{
			case TYPE_TANK_SUPPORT1: return SENSOR_ID_16;
			case TYPE_TANK_SUPPORT2: return SENSOR_ID_17;
#ifdef LIMIT_TO_DM2
			case TYPE_TANK_MODEL1:   return SENSOR_ID_20;
			case TYPE_TANK_MODEL2:   return SENSOR_ID_21;
			case TYPE_TANK_MODEL3:   return SENSOR_ID_18;
			case TYPE_TANK_MODEL4:   return SENSOR_ID_19;
#else
			case TYPE_TANK_MODEL1:   return SENSOR_ID_18;
			case TYPE_TANK_MODEL2:   return SENSOR_ID_19;
			case TYPE_TANK_MODEL3:   return SENSOR_ID_20;
			case TYPE_TANK_MODEL4:   return SENSOR_ID_21;
#endif 
			case TYPE_TANK_MODEL5:   return SENSOR_ID_22;
			case TYPE_TANK_MODEL6:   return SENSOR_ID_23;
	}
}

int CContainerBase::GetRemainingWasteWeight()
{
	int CurrWeight      = GetTankWeightInGram(TYPE_TANK_WASTE);
	int RemainingWeight = m_ParamsMgr->WeightLevelLimitArray[TYPE_CHAMBER_WASTE] - CurrWeight;
	return RemainingWeight;
}//GetRemainingWasteWeight

float CContainerBase::GetTankWeightInA2D(int /* TTankIndex */ Tank)
{
	if(!VALIDATE_TANK_INCLUDING_WASTE(Tank))
		throw EContainer("'GetTankWeightInA2D' Error: invalid parameter");
	return m_TanksArray[Tank]->GetTankWeightInA2D();
}//GetTankWeightInA2D

QString CContainerBase::GetTankMaterialType(TTankIndex TankIndex)
{
	return m_TanksArray[TankIndex]->GetTankMaterialType();
}

float CContainerBase::GetTankWeightInGram(int /* TTankIndex */ Tank)
{
	if(!VALIDATE_TANK_INCLUDING_WASTE(Tank))
		throw EContainer("'GetTankWeightInGram' Error: invalid parameter");
	return m_TanksArray[Tank]->GetTankWeightInGram(/*TYPE_TANK_WASTE == Tank*/);
}//GetTankWeightInGram

int CContainerBase::GetWeight(int /* TChamberIndex */ Chamber, int /* TTankIndex */ Tank)
{
	if(!VALIDATE_CHAMBER_INCLUDING_WASTE(Chamber))
		throw EContainer("'GetWeight' Error: invalid parameter");
	return m_ChambersArray[Chamber].GetWeightInGram(static_cast<TTankIndex>(Tank));

}

bool CContainerBase::IsWeightOk(int /* TChamberIndex */ Chamber)
{
	if(TYPE_CHAMBER_WASTE == Chamber)
		return (GetWeight(Chamber, ACTIVE_TANK) < m_ParamsMgr->WeightLevelLimitArray[Chamber] &&
		        IsTankInAndEnabled(TYPE_TANK_WASTE));

	if(!VALIDATE_CHAMBER(Chamber))
		throw EContainer("'GetWeight' Error: invalid parameter");
	return IsTankWeightOk(m_ChambersArray[Chamber].GetActiveTank());
}//IsWeightOk

bool CContainerBase::IsTankWeightOk(TTankIndex Tank)
{
	for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
		if(m_ChambersArray[i].IsChambersTank(Tank))
			return (GetTankWeightInGram(Tank) > m_ParamsMgr->WeightLevelLimitArray[i]);
	return false;
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
#ifdef OBJET_MACHINE //itamar objet check!!!
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
#else
			case TYPE_TANK_MODEL1:
				SetDrainPumpsMsg->ModelTank2PumpID   = 1;
				break;
			case TYPE_TANK_MODEL2:
				SetDrainPumpsMsg->ModelTank1PumpID   = 1;
				break;
#endif
		}
		FrontEndInterface->UpdateStatus(FE_SET_DRAIN_PUMPS_BASE+i, (int)DrainPumps[i]); // todo -oNobody -cNone: Don't update the status before you sure the operation succeeded
	}
	if(PrintToLog)
		CQLog::Write(LOG_TAG_EOL,tmp);

	return (Q_NO_ERROR);
}

TQErrCode CContainerBase::UpdateWeightAfterWizard(TTankIndex tankIndex)
{
	TQErrCode err = Q_NO_ERROR;
	if(tankIndex==ALL_TANKS)
	{
		for(int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; i++)
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

TQErrCode CContainerBase::UpdateActiveThermistors()
{
	CHeadFillingBase* HeadFillingInstance = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadFillingInstance();
	if(!HeadFillingInstance)
	{
		CQLog::Write(LOG_TAG_OBJET_MACHINE,"HeadFillingInstance Memory allocation failed");
		return Q_FAIL_ACTION; //allocation failed
	}

	return HeadFillingInstance->UpdateActiveThermistors();
}

TQErrCode CContainerBase::SetActiveThermistorsAccordingToParamMngr()
{
	CHeadFillingBase* HeadFillingInstance = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadFillingInstance();
	if(!HeadFillingInstance)
	{
		CQLog::Write(LOG_TAG_OBJET_MACHINE,"HeadFillingInstance Memory allocation failed");
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

	for(int c = FIRST_CHAMBER_TYPE; c < LAST_CHAMBER_TYPE; c++)
	{
		m_ChambersArray[c].ClearTanks();
		// Create a list of Tanks related to each Chamber.
		for(int t = FIRST_TANK_TYPE; t < LAST_TANK_TYPE; t++)
		{
			if(m_ParamsMgr->ChamberTankRelation[c+t*NUMBER_OF_CHAMBERS_INCLUDING_WASTE] == true)
			{
				m_ChambersArray[c].AddTank(m_TanksArray[t], (TTankIndex)t);
				ActiveTanks[c] = (TTankIndex)t; // if only one tank is associated with a certain chamber, then we're in DM mode, and we will always want this tank "active". (Because in DM both Tank should show active)
			}
		}

		//[@Assumption] Chambers with a single tank (e.g. Waste) cannot be flooded (i.e. become inactive)
		if(m_ChambersArray[c].GetTanksCount() > 1)
		{
			ActiveTanks[c] = m_ChambersArray[c].GetTankToActivate();
			if(ActiveTanks[c] == NO_TANK)
			{
				/* If no tank is available then either the operation-mode has changed
				and the flooding chamber's tanks aren't ready, or the current chamber's tanks aren't.
				  If the chamber is flooded, select the tank of the sibling (flooding) chamber.
				  Else - reassign an arbitrary tank that is statically related to the chamber
				*/
				if(! IsChamberFlooded(static_cast<TChamberIndex>(c)))
					ActiveTanks[c] = GetSpecificTank(static_cast<TChamberIndex>(c), 1);
			}
		}
	}

	//Second part of fix: If a chamber is flooded, select the flooding chamber's active tank
	for(int c = FIRST_CHAMBER_TYPE; c < LAST_CHAMBER_TYPE; c++)
		if(NO_TANK == ActiveTanks[c])
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

void CContainerBase::UpdateTanks(TTankIndex Tank)
{
	if(!VALIDATE_TANK(Tank))
		throw EContainer("Containers: UpdateTanks - Invalid param");
	UpdateTanksStatus(Tank);
}//UpdateTanks

bool CContainerBase::IsTankInOverconsumption(TTankIndex TankIndex)
{
	return false;
}

void CContainerBase::InitRaccoon()
{

}
