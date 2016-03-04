#pragma hdrstop
#include "ContainerDummy.h"
#pragma package(smart_init)

#include "QTimer.h"

/*******************************************************************************
// Class CContainerDummy implementation
*******************************************************************************/
// Dummy Constructor
CContainerDummy::CContainerDummy(const QString& Name):CContainerBase(Name)
{
    for(int i=FIRST_TANK_INCLUDING_WASTE_TYPE; i<LAST_TANK_INCLUDING_WASTE_TYPE; i++)
    {
        m_TanksArray[i]->SetTankInserted(true);
        m_TanksArray[i]->SetTankEnabled(true);
        m_TanksArray[i]->SetTankWeight(m_ParamsMgr->CartridgeEmptyWeight * (i + 2000));
    }
    Init();
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
    if (FindWindow(0, QFormatStr( "Chamber%dEmpty.txt - Notepad", (int)TankToStaticChamber(static_cast<TTankIndex>(Tank)) ).c_str() ) )
        return 0;
    if (FindWindow(0, "ChambersEmpty.txt - Notepad") )
        return 0;
#endif
    return CContainerBase::GetWeight(Chamber,Tank);
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
    return CContainerBase::AreRelevantTanksInserted(Chamber,Tank);
}

bool CContainerDummy::IsTankInAndEnabled(TTankIndex Tank)
{
#ifdef _DEBUG
    if (FindWindow(0, QFormatStr( "Tank%dOut.txt - Notepad", (int)Tank ).c_str() ) )
        return false;
    if (FindWindow(0, QFormatStr( "Tank%dDisable.txt - Notepad", (int)Tank ).c_str() ) )
        return false;
#endif
	return CContainerBase::IsTankInAndEnabled(Tank);
}

TQErrCode CContainerDummy::NotifyMSEvent(TTankIndex tank, bool InOut)
{
	return Q_NO_ERROR;
}

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

QString CContainerDummy::GetTagIDAsString(TTankIndex CartridgeID)
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

bool CContainerDummy::IsContainerInserted(TTankIndex TankIndex)
{
	return true;
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

bool CContainerDummy::IsActiveLiquidTankEnabled (int /* TChamberIndex */ Chamber)
{
#ifdef _DEBUG
    if (FindWindow(0, QFormatStr( "Chamber%dDisable.txt - Notepad", (int)Chamber ).c_str() ) )
        return false;
#endif
    return CContainerBase::IsActiveLiquidTankEnabled(Chamber);
}

bool CContainerDummy::RemountSingleTankStatus(int /* TTankIndex */ Tank)
{

    for (int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; i++)
    {
        FrontEndInterface->UpdateStatus(FE_TANK_ENABLING_STATUS_BASE + i, FE_TANK_ENABLED_STATUS, false);
    }

    return CContainerBase::SetAllLiquidTanksEnabled(true);
}

bool CContainerDummy::WaitForIdentificationCompletion(int /* TTankIndex */ Tank, bool DontWaitIfTankIsOut, unsigned int timetowait)
{
    return true;
}

void CContainerDummy::TanksStatusLogic(TTankIndex TankIndex)
{
}

void CContainerDummy::UpdateChambers(bool UpdateActiveTherms)
{    
    for(int i=FIRST_TANK_TYPE; i<LAST_TANK_TYPE; i++)
    {
        FrontEndInterface->UpdateStatus(FE_UPDATE_RESIN_TYPE_BASE+i,m_TanksArray[i]->GetMaterialType());
        int WeightOffline = m_ParamsMgr->WeightOfflineArray[TankToStaticChamber((TTankIndex)i)];
        FrontEndInterface->UpdateStatus(FE_CURRENT_TANK_WEIGHT_BASE+i, WeightOffline);
        FrontEndInterface->UpdateStatus(FE_CURRENT_TANK_RELATIVE_WEIGHT_BASE+i,WeightOffline / m_ParamsMgr->CartridgeFullWeight);
    }

    CContainerBase::UpdateChambers(UpdateActiveTherms);
}

void CContainerDummy::UpdateConsumption(TTankIndex TankIndex, const unsigned int Weight, const unsigned int SliceNum)
{

}

unsigned int CContainerDummy::GetTankCurrentWeightInTag(TTankIndex TankIndex)
{
    return 0;
}
