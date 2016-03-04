
#pragma hdrstop
#include "Chamber.h"
#pragma package(smart_init)

#include "QTimer.h"
#include "AppLogFile.h"

int GetDrainPumpCounterID(TTankIndex Tank);

/*******************************************************************************
class CChamber implementation
*******************************************************************************/
CChamber::CChamber(void) :  m_ActiveTank(NO_TANK), m_ActiveDrainTank(NO_TANK)
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
    return (GetTotalWeight() > m_ParamsMgr->WeightLevelLimitArray[Chamber]);
}//IsLiquidWeightOk

int CChamber::GetTotalWeight()
{
    int       TankWeight;
    int       TotalWeight = 0;

    // Iterating through all the tanks - trying to find a suitable tank to switch to
    for(TTankToPtrMapIter Iter = m_Tanks.begin(); Iter != m_Tanks.end(); ++Iter)
    {
        if(Iter == NULL)
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

        if (Tank!=NULL && Tank->m_IsDrainNeeded)
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

TTankIndex CChamber::GetTankToActivate(int HysteresisConstInit)
{
    TTankIndex         ret             = NO_TANK;
    int                MinTankWeight   = INT_MAX;
    int                TankWeight      = 0;
    int                HysteresisConst;
    TTankToPtrMapIter  Iter;    

    // If this method is called before Tanks array is built (may happen during startup), return 'NO_TANK'.
    if (m_Tanks.empty())
        return NO_TANK;

	// Iterating through all the tanks - trying to find a suitable tank to switch to
	for(Iter = m_Tanks.begin(); Iter != m_Tanks.end(); ++Iter)
    {
		CQSingleContainer *Tank;
		
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

        // if there isn't enough material in the tank - continue
        if (TankWeight <= m_ParamsMgr->WeightLevelLimitArray[m_Type] + HysteresisConst)
			continue;

        // If tank consumption reached 0
		if(0 == Tank->GetCurrentWeightInTag())
			continue;	

        if (TankWeight < MinTankWeight)
        {
            // The tank is Enabled, inserted and has enough resin - return it.
            ret         = Iter->first;
            MinTankWeight = TankWeight;
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
            if(Iter!=NULL)
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
            if( Iter!=NULL)
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
