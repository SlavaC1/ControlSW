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
#include "QUtils.h"
#include "OCBCommDefs.h"
#include "MachineSequencer.h"
#include "QApplication.h"
#include "HeadFilling.h"
#include "Q2RTApplication.h"
#include "BackEndInterface.h"
#include "Raccoon.h"

using namespace Raccoon;

const int WASTE_IS_EMPTY                         = -250;
const int WEIGHT_ERROR_WARNNING_DELTA            = 100; //gr
const int WASTE_WEIGHT_ERROR_WARNNING_DELTA      = 500; //gr
const int ACTIVE_PUMPS_PERIODIC_CHECK_ITERATIONS = 10;

const int DISPATCHER_QUEUE_SEND_TIMEOUT = 500;// /*Seconds*/ * 1000;
const int SM_EVENT_QUEUE_MAX_MESSAGES   = 60;

const int CONSUMPTION_UPDATE_INTERVAL   = 100;

extern const int PumpsCounterID[NUMBER_OF_PUMPS];

/*******************************************************************************
// Class CContainer implementation
*******************************************************************************/
CContainer::CContainer(const QString& Name) : CContainerBase(Name)
{
    LiquidTank_AckOk        = false; //Flag ack ok
    m_FlagGetStatusUnderUse = false;
    m_WasteAlert            = false;
    m_AllLiquidTankEnabled  = false;

    m_TanksInPlacePrevStatus = 0;

    m_ContainerDispatcher = new CContainersDispatcher;
	m_ContainerDispatcher->SetContainerPtr(this);

	m_ConsumptionUpdater = new CConsumptionUpdater;
	m_ConsumptionUpdater->SetContainerPtr(this);

    CRaccoon::Init();
	CRaccoon::Instance()->AddInplaceChangeObserver(this);

	m_ContainerDispatcher->Resume();
	m_ConsumptionUpdater->Resume();

    Init();

}//Constructor

// Destructor
// -------------------------------------------------------
CContainer::~CContainer(void)
{
    try
    {
        CRaccoon::DeInit();
    }
    catch(EQException& Exception)
    {
        QMonitor.ErrorMessage(Exception.GetErrorMsg());
        CQLog::Write(LOG_TAG_RACCOON, Exception.GetErrorMsg() + " Unable to de-initialize Raccoon.");
	}

	Q_SAFE_DELETE(m_ContainerDispatcher);
	Q_SAFE_DELETE(m_ConsumptionUpdater);
}//destructor


// This procedure set the Active Pumps
//-------------------------------------------
TQErrCode CContainer::SetActiveTank(TTankIndexArray& TankArray, bool PrintToLog)
{
    SetActiveTankMutex.WaitFor();

    // Build the Head Fillings set Active Pumps Message
    TOCBSetChamberTankMsg SendMsg;
    memset(&SendMsg, 0, sizeof(TOCBSetChamberTankMsg));

    SendMsg.MessageID   = OCB_SET_CHAMBERS_TANK;
    for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
        SendMsg.TankID[i] = TankArray[i];

    // Send a Turn ON request
    if(m_OCBClient->SendInstallWaitReply(&SendMsg, sizeof(TOCBSetChamberTankMsg),
                                         SetActiveTankAckResponse,
                                         reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
    {
        CQLog::Write(LOG_TAG_HEAD_FILLING, "OCB didn't get ack for \"SetActiveTank\" message");
        throw EContainer("OCB didn't get ack for \"SetActiveTank\" message");
    }
    CContainerBase::SetActiveTank(TankArray, PrintToLog);
    SetActiveTankMutex.Release();

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
    if(static_cast<int>(ResponseMsg->MessageID) != OCB_ACK)
    {
        FrontEndInterface->NotificationMessage("Containers \"SetActiveTankAckResponse\" message id error");
        CQLog::Write(LOG_TAG_HEAD_FILLING, "Containers \"SetActiveTankAckResponse\" message id error (0x%X)",
                     (int)ResponseMsg->MessageID);
        return;
    }

    if(static_cast<int>(ResponseMsg->RespondedMessageID) != OCB_SET_CHAMBERS_TANK)
    {
        FrontEndInterface->NotificationMessage("Containers \"SetActiveTankAckResponse\" responded message id error");
        CQLog::Write(LOG_TAG_HEAD_FILLING, "Containers \"SetActiveTankAckResponse\" responded message id error (0x%X)",
                     (int)ResponseMsg->RespondedMessageID);
        return;
    }

    if(ResponseMsg->AckStatus)
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
    if(m_OCBClient->SendInstallWaitReply(&SendMsg, sizeof(TOCBSetDrainPumpsMsg),
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
    if(static_cast<int>(ResponseMsg->MessageID) != OCB_ACK)
    {
        FrontEndInterface->NotificationMessage("Containers \"SetDrainTankAckResponse\" message id error");
        CQLog::Write(LOG_TAG_HEAD_FILLING, "Containers \"SetDrainTankAckResponse\" message id error (0x%X)",
                     (int)ResponseMsg->MessageID);
        return;
    }

    if(static_cast<int>(ResponseMsg->RespondedMessageID) != OCB_SET_HEAD_FILLING_DRAIN_PUMPS)
    {
        FrontEndInterface->NotificationMessage("Containers \"SetDrainTankAckResponse\" responded message id error");
        CQLog::Write(LOG_TAG_HEAD_FILLING, "Containers \"SetDrainTankAckResponse\" responded message id error (0x%X)",
                     (int)ResponseMsg->RespondedMessageID);
        return;
    }

    if(ResponseMsg->AckStatus)
    {
        FrontEndInterface->NotificationMessage("Containers \"SetDrainTankAckResponse\" ack status error");
        CQLog::Write(LOG_TAG_HEAD_FILLING, "Containers \"SetDrainTankAckResponse\" ack status error (%d)",
                     (int)ResponseMsg->AckStatus);
        return;
    }
}//

void CContainer::UpdateLiquidTankInserted(TTankIndex Tank, bool Inserted)
{
    if(true == m_TanksArray[Tank]->GetIgnoreTankInsertionAndRemovalSignalsState())
        return;

    if(!VALIDATE_TANK_INCLUDING_WASTE(Tank))
        throw EContainer("'UpdateLiquidTankInserted' Error: invalid param");

    if(TYPE_TANK_WASTE != Tank)
    {
        m_TanksArray[Tank]->SetTankInserted(Inserted);
        FrontEndInterface->UpdateStatus(m_TanksArray[Tank]->GetFEExistenceControlID(), static_cast<int>(Inserted));
    }
    else // For the Waste tank
    {
        m_TanksArray[Tank]->SetTankInserted(Inserted);
    }

    return;
}


//--------------------------------------------------------------------------------------------
bool CContainer::SendNotification(TContainerIdentifyNotificationMessage &Message)
{
    m_TanksArray[Message.TankIndex]->SendNotification(Message.MessageBody);
    return true;
}

QString CContainer::GetTagIDAsString(TTankIndex TankIndex)
{
    return QFormatStr("%lld", m_TanksArray[TankIndex]->GetTagID());
}

QString CContainer::GetPipeMaterialType(TTankIndex TankIndex)
{
    return m_TanksArray[TankIndex]->GetMaterialType();
}

unsigned long CContainer::GetContainerExpirationDate(TTankIndex TankIndex)
{
    return m_TanksArray[TankIndex]->GetExpirationDate();
}

bool CContainer::IsContainerInserted(TTankIndex TankIndex)
{
    return m_TanksArray[TankIndex]->IsTankInserted();
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
    if(Result == FE_TANK_ID_NOTICE_DLG_RESULT_OK)
    {
        return;
    }
    else

        // reset result flags:
        m_TanksArray[TankIndex]->SetForceUsingTank(false);
    m_TanksArray[TankIndex]->SetRunRRWizard(false);

    switch(Result)
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

//---- Liquid Models -----------------------------------
//Model on sensor procedures
TQErrCode CContainer::GetLiquidTankInsertedStatus()
{
    return GetLiquidTankInsertedStatusFiltered(ALL_TANKS);
}

TQErrCode CContainer::GetLiquidTankInsertedStatusFiltered(int /* TTankIndex */ TanksFilter)
{
    TTankIndex index = static_cast<TTankIndex>(TanksFilter);

    for(int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; i++)
        if((index == ALL_TANKS) || (index == i))
            UpdateLiquidTankInserted(static_cast<TTankIndex>(i), static_cast<bool>((m_TanksInPlacePrevStatus >> i) & 1));

    return Q_NO_ERROR;
}

TQErrCode CContainer::IgnoreTankInsertionAndRemovalSignals()
{
    for(int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; i++)
        m_TanksArray[i]->IgnoreTankInsertionAndRemovalSignals();
    return Q_NO_ERROR;
}

TQErrCode CContainer::AllowTankInsertionAndRemovalSignals()
{
    for(int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; i++)
        m_TanksArray[i]->AllowTankInsertionAndRemovalSignals();
    return Q_NO_ERROR;
}

// Called to simulate Tanks Removal.
// Used to silence Raccoon sampling before turning FCB power Off (e.g. during SHR Wizard) - to avoid exceptions.
TQErrCode CContainer::SendTanksRemovalSignals()
{
    CQLog::Write(LOG_TAG_GENERAL, "Removing all tanks");

    for(int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; i++)
        UpdateTankStatus(i, TANK_REMOVED);

    return Q_NO_ERROR;
}

TQErrCode CContainer::GetIfAllLiquidTankAreInserted(void)
{
    TQErrCode Err;
    if((Err = GetLiquidTankInsertedStatus()) != Q_NO_ERROR)
        return Err;

    for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
    {
        if(m_ChambersArray[i].IsActiveLiquidTankInserted() == false)
        {
            CQLog::Write(LOG_TAG_GENERAL, QFormatStr("Liquid tank: %s is not inserted", ChamberToStr((TChamberIndex)i).c_str()));
            return ((IS_SUPPORT_CHAMBER(i)) ? Q2RT_LIQUID_SUPPORT_TANK_ARE_NOT_INSERTED : Q2RT_LIQUID_MODEL_TANK_ARE_NOT_INSERTED);
        }
    }
    return Q_NO_ERROR;
}//GetIfAllLiquidTankAreInserted

TQErrCode CContainer::GetLiquidTanksWeight(void)
{
    CContainerBase::GetLiquidTanksWeight();
    if(m_FlagGetStatusUnderUse)
    {
        CQLog::Write(LOG_TAG_EOL, "Container \"GetLiquidTanksWeight\" reentry problem");
        return Q_NO_ERROR;
    }
    TOCBGetLiquidTanksWeightMessage GetMsg;
    m_FlagGetStatusUnderUse = true;
    GetMsg.MessageID        = static_cast<BYTE>(OCB_GET_LIQUID_TANK_WEIGHT_STATUS);

    // Send a Turn ON request
	if(m_OCBClient->SendInstallWaitReply(&GetMsg,  sizeof(TOCBGetLiquidTanksWeightMessage), GetLiquidTanksWeightAckResponse, reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
		CQLog::Write(LOG_TAG_EOL, "OCB didn't get ack for Container \"GetLiquidTanksWeight\" message");                                                                                  

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

    if(static_cast<int>(StatusMsg->MessageID) != OCB_LIQUID_TANK_WEIGHT)
    {
        FrontEndInterface->NotificationMessage("Container \"GetLiquidTanksWeightAckResponse\" message id error");
        CQLog::Write(LOG_TAG_EOL, "Container \"GetLiquidTanksWeightAckResponse\" message id error (0x%X)", (int)StatusMsg->MessageID);
        return;
    }
    float Weights[TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE] =
        INIT_MSG_VALUES_W_ARRAY(StatusMsg, Weight);

    // Updating the Weights of the liquid tanks
    for(int i = FIRST_TANK_INCLUDING_WASTE_TYPE; i < LAST_TANK_INCLUDING_WASTE_TYPE; i++)
        InstancePtr->m_TanksArray[i]->SetTankWeight(Weights[i]);

    // Write the new weights the Log only if one of the 'Printed' weights is different atleast in 40 gr
    bool NeedToWriteWeightsToLog = false;

    for(int i = FIRST_TANK_INCLUDING_WASTE_TYPE; i < TOTAL_NUMBER_OF_CONTAINERS; i++)
    {
        if(InstancePtr->m_TanksArray[i]->IsPrintWeightNecessary())
            NeedToWriteWeightsToLog = true;
    }

    int WeightArray[TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE];
    QString tmp = "Liquids weight: ";
    for(int i = FIRST_TANK_INCLUDING_WASTE_TYPE; i < LAST_TANK_INCLUDING_WASTE_TYPE; i++)
    {
        WeightArray[i] = InstancePtr->GetTankWeightInGram((TTankIndex)i);
        tmp += TankToStr((TTankIndex)i) + " = " + QFloatToStr(WeightArray[i]) + "; ";
    }
    InstancePtr->UpdateLiquidTankInserted(TYPE_TANK_WASTE, (InstancePtr->m_TanksArray[TYPE_TANK_WASTE]->GetTankWeightInGram(true) > WASTE_IS_EMPTY));
    if(NeedToWriteWeightsToLog)
        CQLog::Write(LOG_TAG_EOL, tmp);

    for(int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; i++)
    {
        // Update the front end with the status of the containers
        FrontEndInterface->UpdateStatus(FE_CURRENT_TANK_STATUS_BASE + i,
                                        (int)(WeightArray[i] > (InstancePtr->m_ParamsMgr->WeightLevelLimitArray[TankToStaticChamber((TTankIndex)i)]
                                                + WEIGHT_ERROR_WARNNING_DELTA)));

        WeightArray[i] -= InstancePtr->m_ParamsMgr->WeightLevelLimitArray[TankToStaticChamber((TTankIndex)i)];
        // Update the front end with the current weight
        FrontEndInterface->UpdateStatus(FE_CURRENT_TANK_WEIGHT_BASE + i,
                                        ((WeightArray[i] > 0) && (! InstancePtr->IsTankInOverconsumption((TTankIndex)i))) ? WeightArray[i] : 0);

        // Update the front end with the relative weight
        FrontEndInterface->UpdateStatus(FE_CURRENT_TANK_RELATIVE_WEIGHT_BASE + i,
                                        WeightArray[i] * 100 /
                                        InstancePtr->m_ParamsMgr->CartridgeFullWeight);
    }
    // Update the front end with the status of the containers
    FrontEndInterface->UpdateStatus(FE_CURRENT_TANK_STATUS_WASTE_TANK,
                                    (int)(WeightArray[TYPE_TANK_WASTE] < (InstancePtr->m_ParamsMgr->WeightLevelLimitArray[TYPE_CHAMBER_WASTE]
                                            - WASTE_WEIGHT_ERROR_WARNNING_DELTA)));

    FrontEndInterface->UpdateStatus(FE_TANK_EXISTENCE_STATUS_WASTE_TANK, (int)(InstancePtr->IsActiveLiquidTankInserted((int)TYPE_CHAMBER_WASTE)));

    // Update the front end with the current weight
    FrontEndInterface->UpdateStatus(FE_CURRENT_TANK_WEIGHT_WASTE_TANK, WeightArray[TYPE_TANK_WASTE] > 0 ? WeightArray[TYPE_TANK_WASTE] : 0);

    // Display a service alert in case the waste cartride is full
    if(WeightArray[TYPE_TANK_WASTE] > (InstancePtr->m_ParamsMgr->WeightLevelLimitArray[TYPE_CHAMBER_WASTE] - WASTE_WEIGHT_ERROR_WARNNING_DELTA))
    {
        if(!InstancePtr->m_WasteAlert)
        {
            FrontEndInterface->UpdateStatus(FE_SERVICE_ALERT, WASTE_CARTRIDGE_ALERT_ID, true);
            InstancePtr->m_WasteAlert  = true;
            InstancePtr->m_WasteIsFull = true;
        }
    }
    else
    {
        if(InstancePtr->m_WasteAlert)
        {
            FrontEndInterface->UpdateStatus(FE_CLEAR_SERVICE_ALERT, 0, true);
            InstancePtr->m_WasteAlert  = false;
            InstancePtr->m_WasteIsFull = false;
        }
    }
}//GetLiquidTanksWeightAckResponse

int CContainer::GetTotalWeight(int Chamber)
{
    int Weight = 0;
    if(VALIDATE_CHAMBER(Chamber))
        Weight = m_ChambersArray[Chamber].GetTotalWeight();
    return Weight;
}//GetTotalWeight

unsigned int CContainer::GetLastInsertionTimeInTicks(int /* TTankIndex */ Tank)
{
    if(!VALIDATE_TANK(Tank))
        throw EContainer("'GetLastInsertionTimeInTicks' Error: invalid parameter");

    return m_TanksArray[Tank]->GetLastInsertionTimeInTicks();
}

bool CContainer::GetWasteWeightStatusDuringPrinting()
{
    int CurrWeight = GetTankWeightInGram(TYPE_TANK_WASTE);
    return (CurrWeight < m_ParamsMgr->WeightLevelLimitArray[TYPE_CHAMBER_WASTE]);
}//GetWasteWeightStatusDuringPrinting

bool CContainer::GetWasteWeightStatusBeforePrinting()
{
    int CurrWeight = GetTankWeightInGram(TYPE_TANK_WASTE);
    return (CurrWeight < (m_ParamsMgr->WeightLevelLimitArray[TYPE_CHAMBER_WASTE] - WASTE_WEIGHT_ERROR_WARNNING_DELTA));
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
    for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
    {
        //decide which tank should be the active tank (decided upon its weight)
        TmpTank        = m_ChambersArray[i].GetTankToActivate();
        ActiveTanks[i] = m_ChambersArray[i].GetActiveTank();
        if((TmpTank != NO_TANK) && (TmpTank != ActiveTanks[i]))
        {
            TankSwapNeeded = true;
            ActiveTanks[i] = TmpTank;
        }
        //keep history of all batch numbers (of the printing tanks)
        if(TankSwapNeeded)
            m_batchNoTable->AddTankBatchNumber(ActiveTanks[i], m_TanksArray[ActiveTanks[i]]->GetBatchNo(), GetTankMaterialType(ActiveTanks[i]));
    }
    if(TankSwapNeeded)
    {
        SetActiveTank(ActiveTanks, PrintToLog); // Note: SetActiveTank() sends a message to OCB to swap pumps.
    }
    // Each CheckActiveTanksPeriod iterations we check if OCB Active pumps matches PC's, and if not,
    // we perform a SetActiveTank().
    else if(CheckActiveTanksPeriod <= 0)
    {

        CHeadFillingBase* HeadFillingInstance = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadFillingInstance();

        // If Heads Filling not yet created:
        if(!HeadFillingInstance)
        {
            return true;
        }
        // Following command gets the Active Pumps from OCB:
        HeadFillingInstance->GetHeadsFillingsActiveTanks(); // This is the OCBStatusSender exception causing call !!!
        for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
        {
            TmpTank      = (TTankIndex)HeadFillingInstance->GetActiveTank((TChamberIndex)i);
            ActiveTanks[i] = m_ChambersArray[i].GetActiveTank();
            if(TmpTank != ActiveTanks[i])
            {
                TankSwapNeeded = true;
                ActiveTanks[i] = TmpTank;
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

//----------------------------------------------------------------
void CContainer::AckToOcbNotification(int MessageID, int TransactionID, int AckStatus, TGenericCockie Cockie)
{
	// Build the EOL turn on message
	TOCBAck AckMsg;

	// Get a pointer to the instance
	CContainer *InstancePtr = reinterpret_cast<CContainer *>(Cockie);

	AckMsg.MessageID          = OCB_EDEN_ACK;
	AckMsg.RespondedMessageID = MessageID;
	AckMsg.AckStatus          = AckStatus;

	// Send a Turn ON request
	InstancePtr->m_OCBClient->SendNotificationAck(TransactionID, &AckMsg, sizeof(TOCBAck));
}//

//----------------------------------------------------------------
void CContainer::Cancel(void)
{}//Cancel


bool CContainer::RemountSingleTankStatus(int /* TTankIndex */ Tank)
{
    // Nothing to remount if there's no Tank. (remount means "re-insert")
    if(false == m_TanksArray[Tank]->IsTankInserted())
		return true;

	CQLog::Write(LOG_TAG_RACCOON, "Remount single tank. TankIndex: %d", Tank);

    UpdateTankStatus(Tank, TANK_REMOVED);
    UpdateTankStatus(Tank, TANK_INSERTED);

    return true;
}

bool CContainer::WaitForIdentificationCompletion(int /* TTankIndex */ Tank, bool DontWaitIfTankIsOut, unsigned int timetowait)
{
    /*
    // Reason for using m_RemountInProgress: without it, we might actually be during a remount, but before GetTankIDState() has changed to DATA_READ_NOT_ACTIVE.
    //   - Another issue it solves is: When GetState() == BEGIN_NO_TANK during Remount. In that case, if we have DontWaitIfTankIsOut=true, we would *still* want to wait.
    if(!m_TanksArray[Tank]->IsRemountInProgress())
    {
    	if(DontWaitIfTankIsOut)
    		// if(m_TanksArray[Tank]->GetState() == BEGIN_NO_TANK)  // Todo: Shahar: Raccoon
    			return true;

    	// In case we're already in (or after) CHECK_PUMP_TIME state, return 'true' without actually waiting.
    	if(m_TanksArray[Tank]->GetTankIDState() != CHECK_PUMP_TIME)
    		return true;
    }
                       */

    // Set RemountSingleTankCallback() to be the callback function.
    SetIdentificationCompletedCallback(static_cast<TTankIndex>(Tank), CContainer::IdentificationCompletedCallback, reinterpret_cast<TGenericCockie>(this));

    if(QLib::wrSignaled == m_TanksArray[Tank]->WaitEventIdentificationCompletion(timetowait))
        return true;
    else
        return false;
}

void CContainer::IdentificationCompletedCallback(TTankIndex Tank, TGenericCockie Cockie)
{
    CContainer *InstancePtr = reinterpret_cast<CContainer *>(Cockie);

    // Erase the callback pointers after callback was done.
    InstancePtr->SetIdentificationCompletedCallback(Tank, NULL, NULL);

    InstancePtr->m_TanksArray[Tank]->SetEventIdentificationCompletion();
}

void CContainer::SetIdentificationCompletedCallback(TTankIndex Tank, TStateEnterCallback CallbackFunction, TGenericCockie Cockie)
{
    m_TanksArray[Tank]->SetIdentificationCompletedCallback(CallbackFunction, Cockie);
}

void CContainer::RemountSingleTankCallback(TTankIndex Tank, TGenericCockie Cockie)
{
}

void CContainer::TanksStatusLogic(TTankIndex TankIndex)
{
    CQSingleContainer* ContainerPtr  = NULL;
    QString MachineMaterialType      = "";
    bool    DialogAcked              = false;
    bool    ForceUsingTank           = false;
    int     FEMessageID              = 0;
    int     FEEnabledControlID       = 0;
    ContainerPtr                     = m_TanksArray[TankIndex];

    // Get the current resin type in the pipe system and Tanks:
    QString PipeMaterialType   = ContainerPtr->GetMaterialType();
    QString TankMaterialType   = ContainerPtr->GetTankMaterialType();
    DialogAcked                = ContainerPtr->GetDialogAcked();
    ForceUsingTank             = ContainerPtr->GetForceUsingTank();
    FEMessageID                = ContainerPtr->GetFEMessageID();
    FEEnabledControlID         = ContainerPtr->GetFEEnabledControlID();

    // Handle No Tank:
    if(TankMaterialType == "")
    {
        // Closes any dialog related to this tank (if it was open):
        FrontEndInterface->HideTankIDNoticeDlg(FEMessageID);

        // Although tanks are Disabled, we don't need the "Disabled" icon since tanks are out.
        // (so we send an Enabled notification)
        FrontEndInterface->UpdateStatus(FEEnabledControlID, FE_TANK_ENABLED_STATUS, false);

        ContainerPtr->SetTankEnabled(false);
    }
    // Handle Non Objet resin:
    else if(TankMaterialType == "Unknown")
    {
        if(DialogAcked == false)
        {
            // Issue the nasty message:
            FrontEndInterface->ShowTankIDNotice(FE_TANK_ID_NOTICE_DLG_NO_TAG, FEMessageID, false);

            // Disable tank:
            ContainerPtr->SetTankEnabled(false);

            FrontEndInterface->UpdateStatus(FEEnabledControlID, FE_TANK_DISABLED_STATUS, false);
        }
        // Check if user has agreed to work without warranty:
        else if(ForceUsingTank == true)
        {
            ContainerPtr->SetTankEnabled(true);
            FrontEndInterface->UpdateStatus(FEEnabledControlID, FE_TANK_ENABLED_STATUS, false);
        }
    }
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// Constructor
CContainersDispatcher::CContainersDispatcher() : CQThread(true, "CContainersDispatcherTask", true)
{
    m_SMContainerIDEventQueue = new TContainerIDEventQueue(SM_EVENT_QUEUE_MAX_MESSAGES, "", false);
    m_SMContainerIDEventQueue->Name() = "EventQueueForContainers";
}

// Destructor
CContainersDispatcher::~CContainersDispatcher()
{
    // Mark terminate and release the queue
    Terminate();

    m_SMContainerIDEventQueue->Release();

    // Wait for thread termination. We need the YieldUIThread() since this D'tor is called from the context of the GUI thread. And since the CContainersDispatcher thread (which we here wait for to finish) waits at a certain point for SendMessage() - application may become deadlocked.
    while(WaitFor(100) != QLib::wrSignaled)
        FrontEndInterface->YieldUIThread();

    // Free the message queue
    delete m_SMContainerIDEventQueue;
}


bool CContainersDispatcher::SendNotification(TContainerIdentifyNotificationMessage &Message)
{
	int count = 0;

#ifdef _DEBUG	
	QString event;
	if(Message.MessageBody.EventType == TANK_INSERTED)
		event = "Inserted";
	else if(Message.MessageBody.EventType == TANK_REMOVED)
		event = "Removed";
	else if(Message.MessageBody.EventType == TANK_UPDATE_CONSUMPTION)
		event = "Consumption updated";
	else
		event = "Incorrect event type";	

	count = m_SMContainerIDEventQueue->ItemsCount;
	QString str = QFormatStr("ContainersDispatcher::SendNotification. m_SMContainerIDEventQueue->ItemsCount: %d, Tank: %s (%d), Event: %s",
												count,
												TankToStr(Message.TankIndex).c_str(),
												Message.TankIndex, event.c_str());
	CQLog::Write(LOG_TAG_RACCOON, str.c_str());
#endif	                                   

	if(m_SMContainerIDEventQueue->Send(Message, QMsToTicks(DISPATCHER_QUEUE_SEND_TIMEOUT)) == QLib::wrTimeout)
	{
		count = m_SMContainerIDEventQueue->ItemsCount;
		CQLog::Write(LOG_TAG_RACCOON, QFormatStr("Container Tag ID queue timeout. m_SMContainerIDEventQueue->ItemsCount: %d", count));
		throw EContainer("Container Tag ID queue timeout");
    }

    return true;
}

void CContainersDispatcher::Execute(void)
{
    TContainerIdentifyNotificationMessage Message;

    do
    {
        memset(&Message, 0, sizeof(TContainerIdentifyNotificationMessage));

        if(m_SMContainerIDEventQueue->Receive(Message) == QLib::wrReleased)
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
    while(! Terminated);
}

void CContainersDispatcher::SetContainerPtr(CContainer* cont_ptr)
{
    m_ContainerPtr = cont_ptr;
}

void CContainer::NotifyInplaceChange(int AllTanksStatus)
{
    for(int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; i++)
    {
        bool NewStatus  = (AllTanksStatus           >> i) & 1;
        bool PrevStatus = (m_TanksInPlacePrevStatus >> i) & 1;

        if(NewStatus == PrevStatus)
            continue;

        UpdateTankStatus(i, (NewStatus ? TANK_INSERTED : TANK_REMOVED));
    }

    m_TanksInPlacePrevStatus = AllTanksStatus;
}

void CContainer::NotifyConsumptionReachedZero(int TankIndex)
{
    m_TanksArray[TankIndex]->DisableTankAfterOverconsumption();
}

// Updates material consumption for one cartridge
void CContainer::UpdateConsumption(TTankIndex TankIndex, const unsigned int Weight, const unsigned int SliceNum)
{
    UpdateTankStatus(TankIndex, TANK_UPDATE_CONSUMPTION, Weight, SliceNum);
}

// Sends message notifying cartridge insertion or removal
void CContainer::UpdateTankStatus(int TankIndex, TTagIdentificationEventType Status, const unsigned int Weight, const unsigned int SliceNum)
{
    TContainerIdentifyNotificationMessage Msg;
    memset(&Msg, 0, sizeof(TContainerIdentifyNotificationMessage));

    Msg.TankIndex             = static_cast<TTankIndex>(TankIndex);
    Msg.MessageBody.EventType = Status;
    Msg.MessageBody.Weight    = Weight;
    Msg.MessageBody.SliceNum  = SliceNum;
    Msg.MessageBody.TimeStamp = QGetTicks();

    m_ContainerDispatcher->SendNotification(Msg);

    QString StatusStr = "";
    switch(Status)
    {
        case TANK_INSERTED:
            StatusStr = "inserted";
            break;

        case TANK_REMOVED:
            StatusStr = "removed";
            break;

        case TANK_UPDATE_CONSUMPTION:
            StatusStr = QFormatStr("accumulating weight %d mGr for consumption update", Weight);
            break;
    }

    CQLog::Write(LOG_TAG_RACCOON, QFormatStr("Cartridge %s (%d) %s", TankToStr(static_cast<TTankIndex>(TankIndex)).c_str(), TankIndex, StatusStr.c_str()));
}

unsigned int CContainer::GetTankCurrentWeightInTag(TTankIndex TankIndex)
{
    return m_TanksArray[TankIndex]->GetCurrentWeightInTag();
}

bool CContainer::IsTankInOverconsumption(TTankIndex TankIndex)
{
    return (GetTankCurrentWeightInTag(TankIndex) == 0);
}

bool CContainer::IsConsumptionUpdateNeeded(TTankIndex TankIndex)
{
	return m_TanksArray[TankIndex]->IsConsumptionUpdateNeeded();
}

Raccoon::TTagInfo* CContainer::GetTagInfo(TTankIndex TankIndex)
{
	return m_TanksArray[TankIndex]->GetTagInfo();
}

void CContainer::GetConsumptionData(TTankIndex TankIndex, int &Weight, int &Slice)
{
	m_TanksArray[TankIndex]->GetConsumptionData(Weight, Slice);
}

void CContainer::ResetConsumedData(TTankIndex TankIndex, int CurrentSlice)
{
	m_TanksArray[TankIndex]->ResetConsumedData(CurrentSlice);
}

void CContainer::InitRaccoon()
{
    CRaccoon::Instance()->InitHardwareAndStart();
}

//---------------------------------------------------------------------------------

CConsumptionUpdater::CConsumptionUpdater(): CQThread(true, "CConsumptionUpdater", false)
{
	m_CurrentTank = 0;
}

CConsumptionUpdater::~CConsumptionUpdater()
{
	Terminate();
}

void CConsumptionUpdater::Execute(void)
{
	while(! Terminated)
	{
		TTankIndex CurrentTank = static_cast<TTankIndex>(m_CurrentTank);

		if(m_ContainerPtr->IsConsumptionUpdateNeeded(CurrentTank))
		{
			Raccoon::TTagInfo *TagInfo = m_ContainerPtr->GetTagInfo(CurrentTank);

			int ConsumedWeight, CurrentSlice;
			m_ContainerPtr->GetConsumptionData(CurrentTank, ConsumedWeight, CurrentSlice);

			CQLog::Write(LOG_TAG_RACCOON, QFormatStr("Consumption updater: Update needed for cartridge: %s (%d), weight: %.2f Gr", TankToStr(static_cast<TTankIndex>(CurrentTank)).c_str(),
																																   CurrentTank,
																															       MilliGramsToGrams(ConsumedWeight)));

            // Perform only if no other Raccoon transaction is in process
			if(! CRaccoon::Instance()->IsBusy())
			{
				CQLog::Write(LOG_TAG_RACCOON, QFormatStr("Consumption updater: Performing consumption update for tank: %s (%d)", TankToStr(static_cast<TTankIndex>(CurrentTank)).c_str(), CurrentTank));
				CRaccoon::Instance()->UpdateConsumption(TagInfo, ConsumedWeight);
				m_ContainerPtr->ResetConsumedData(CurrentTank, CurrentSlice);
			}
			else
			{
				CQLog::Write(LOG_TAG_RACCOON, QFormatStr("Consumption updater: Raccoon driver is busy. Delaying consumption update for tank: %s (%d)",
																TankToStr(static_cast<TTankIndex>(CurrentTank)).c_str(), CurrentTank));
            }
		}

		m_CurrentTank = ++m_CurrentTank % TOTAL_NUMBER_OF_CONTAINERS;
		
    	PausePoint(CONSUMPTION_UPDATE_INTERVAL);
	}
}

void CConsumptionUpdater::SetContainerPtr(CContainer *cont_ptr)
{
	m_ContainerPtr = cont_ptr;
}
