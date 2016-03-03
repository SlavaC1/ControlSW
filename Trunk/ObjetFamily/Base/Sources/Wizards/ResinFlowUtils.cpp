//---------------------------------------------------------------------------


#pragma hdrstop

#ifndef _DEBUG
#define NDEBUG
#endif
#include <assert.h>

#include "ResinFlowUtils.h"
#include "QOSWrapper.h"
#include "QTimer.h"
#include "Q2RTErrors.h"
#include <iterator>

ResinFlowUtils::EResinFlowException::EResinFlowException(TTankIndex i) : _tankIndex(i)
{}

ResinFlowUtils::ETankInvalid::ETankInvalid(TTankIndex i) : EResinFlowException(i)
{}

ResinFlowUtils::ETankOutOfResin::ETankOutOfResin(TTankIndex i) : EResinFlowException(i)
{}

ResinFlowUtils::ETankDisabled::ETankDisabled(TTankIndex i) : EResinFlowException(i)
{}

ResinFlowUtils::ETankBadResinType::ETankBadResinType(TTankIndex i) : EResinFlowException(i)
{}

ResinFlowUtils::Interfacer::Interfacer(CQ2RTAutoWizard *pW) :
	_pBackEnd(CBackEndInterface::Instance()),
	_pParamMgr(CAppParams::Instance()),
	_pWizard(pW)
{
	if (!_pBackEnd)  
		throw EQException("CBackEndInterface::Instance() returned NULL in ResinFlowUtils::AirValve::AirValve(CAutoWizard *pW)");
	if (!_pParamMgr)
		throw EQException("CAppParams::Instance() returned NULL in ResinFlowUtils::AirValve::AirValve(CAutoWizard *pW)");
}



ResinFlowUtils::AirValve::AirValve(CQ2RTAutoWizard *pW) : ResinFlowUtils::Interfacer(pW)
{}

void ResinFlowUtils::AirValve::holdOpen(TQMilliseconds timeout, ProgressReporting::CProgressListenerInterface *pPL)
{
	holdOpen(UntilTimeIsUp, timeout, pPL);
}

void ResinFlowUtils::AirValve::holdOpenUntilNoFullChambers(TQMilliseconds timeout, ProgressReporting::CProgressListenerInterface *pPL)
{
	holdOpen(TryToDrainAllChambers, timeout, pPL);
}

void ResinFlowUtils::AirValve::holdOpen(TMode mode,
										TQMilliseconds timeout,
										ProgressReporting::CProgressListenerInterface *pPL)
{
	const TQMilliseconds StartTime 	= QTicksToMs(QGetTicks());
	const TQMilliseconds EndTime     = StartTime + timeout;
	TQMilliseconds CurrentTime = QTicksToMs(QGetTicks());	
	bool AreThereFullChambers = true;

	open();
	while(!_pWizard->IsCancelled() && (CurrentTime < EndTime)) // todo 3 -oArcady.Volman@objet.com -cSafety: QGetTicksSince
	{
		if (ResinFlowUtils::AirValve::TryToDrainAllChambers == mode)
		{
			AreThereFullChambers = false;

			if (CHECK_EMULATION(_pParamMgr->OCB_Emulation))
				break;

			for(int i = 0; i < NUM_OF_CHAMBERS_THERMISTORS; ++i)
			   AreThereFullChambers = AreThereFullChambers || _pBackEnd->AreResinHeadsFilled(i);

			if (!AreThereFullChambers)
		    	break;
		}

		CurrentTime = QTicksToMs(QGetTicks());
			
		if (pPL)
        {
        	assert(timeout);
        	pPL->reportProgressPercentage((CurrentTime - StartTime) * 100 / timeout);
        }

		_pWizard->YieldAndSleepWizardThread();
	}

	close();
	if (AreThereFullChambers && ResinFlowUtils::AirValve::TryToDrainAllChambers == mode)
		throw ETimeout("draining the heads block using air valve");
		
}

ResinFlowUtils::AirValve::~AirValve()
{
	try
	{
		close();
	}
	catch(...)
	{
		// todo -oArcady.Volman@objet.com -cNone: catch exceptions and report to error handler or do other error reporting, but make sure the handling itself does not throw
	}

}

void ResinFlowUtils::AirValve::close()
{
	if (_pBackEnd->GetActuatorOnOff(ACTUATOR_ID_AIR_VALVE))
		_pBackEnd->SetActuatorState(ACTUATOR_ID_AIR_VALVE, ACTUATOR_OFF);

}

void ResinFlowUtils::AirValve::open()
{
	if (!_pBackEnd->GetActuatorOnOff(ACTUATOR_ID_AIR_VALVE))
		_pBackEnd->SetActuatorState(ACTUATOR_ID_AIR_VALVE, ACTUATOR_ON);
}


//--------------------------------------------------------------------------- ResinFlowUtils::PumpControllerBase --------------------------------------------------------------------------
void ResinFlowUtils::PumpControllers::clearCurrentPumpingCycleDoneFlag()
{
   _isCurrentPumpingCycleDone = false;
}

bool ResinFlowUtils::PumpControllers::isCurrentPumpingCycleDone()
{
   return  _isCurrentPumpingCycleDone;
}

void ResinFlowUtils::PumpControllers::clearControllerOperationDoneFlag()
{
   _isControllerOperationDone = false;
}

bool ResinFlowUtils::PumpControllers::isControllerOperationDone()
{
   return  _isControllerOperationDone;
}

bool ResinFlowUtils::PumpControllers::isPumpsVectorEmpty()
{
   return _pumpControllerPointersVector.empty();
}

//Constructor
ResinFlowUtils::PumpControllerBase::PumpControllerBase
	(
		CQ2RTAutoWizard *pWizard,
		TPumpIndex pumpIndex, /* todo -oArcady.Volman@objet.com -cNone: verify it's a pump*/
		TQMilliseconds	dutyCycleOnMilliseconds,
		TQMilliseconds	dutyCycleOffMilliseconds,
		VerificationAttributes 	verificationAttributes,
		QString 				materialName		
	) :
	ResinFlowUtils::Interfacer(pWizard),
	// initializers
	_pumpIndex(pumpIndex),
	_pumpDescription(PumpIndexToDescription(pumpIndex)),
	_phase(Suspended),
	_phaseStartTimeInTicks(QGetTicks()),
	_totalActualPumpingMilliseconds(0),
	_dutyCycleOnMilliseconds(dutyCycleOnMilliseconds), // todo 4 -oArcady.Volman@objet.com -crefactoring: should be settable to save code in shd
	_dutyCycleOffMilliseconds(dutyCycleOffMilliseconds),
	_verificationAttributes(verificationAttributes),
	_materialName(materialName)
	
{
	
}

ResinFlowUtils::PumpControllerBase::~PumpControllerBase()
{
//	if (_pBackEnd->GetActuatorOnOff(PumpsActuatorID[_pumpIndex]))
	    _pBackEnd->SetActuatorState(PumpsActuatorID[_pumpIndex], ACTUATOR_OFF); // TODO 3 -oArcady.Volman@objet.com -cExceptionSafety : Exception safty
}

void ResinFlowUtils::PumpControllerBase::performControlProcedure() // TODO 4 -oArcady.Volman@objet.com -cSafety : comment on the fact that if this method throws, other pump controllers might stay on. Or better write a PumpControllers manager
{
	using namespace ResinFlowUtils;
	//CQLog::Write(LOG_TAG_GENERAL,"MRW: Entering PerformControlProcedure");
	
	preControlProcedure();

	
	switch(_phase)
	{
		case Done:
			return;
		case Suspended:
			startPhase(WaitingForChamberToBeNotFull);
		case WaitingForChamberToBeNotFull:
			if (!isChamberFull() || CHECK_EMULATION(_pParamMgr->OCB_Emulation)) // pumping required
				startPhase(PumpingDutyCycleOnTime);

			break;
		case PumpingDutyCycleOnTime:

			if (CHECK_NOT_EMULATION(_pParamMgr->OCB_Emulation) && isChamberFull())
			{
				// Suspend pumping until the Chamber becomes   not full  or until timeout expires
				startPhase(WaitingForChamberToBeNotFull);
				_pBackEnd->PrintHeatersTemperaturesToLog(); // it happened several times that the thermistor that is located beneath the air valve stayed cool (1180 A2D) after more than 5 minutes. we could not recreate that, but since we suspect that heads/pre-heaters were too cold for the thermistor to get hot, I have added this print so that we would know if that happen. Arcady, 29/01/08.

				// todo -oArcady.Volman@objet.com -cNone: log with wizard info
                /*
				_pWizard->WriteToLogFile(LOG_TAG_GENERAL,
							QFormatStr("Pump %s total filling time: %dms"
										_pumpDescription, _totalActualPumpingMilliseconds));
                                        */
				return;
			}
			if (isPhaseOver())
			{
				startPhase(PumpingDutyCycleOffTime);

// todo -oArcady.Volman@objet.com -cNone: log with wizard info
/*
				_pWizard->WriteToLogFile(LOG_TAG_GENERAL,
							QFormatStr("Pump %s total filling time: %dms"
										_pumpDescription, _totalActualPumpingMilliseconds));
*/

				return;
			}
			break; // end of PumpingDutyCycleOnTime case
		case PumpingDutyCycleOffTime:
			if (isPhaseOver())
      {
        if (isChamberFull())
				  startPhase(WaitingForChamberToBeNotFull);
        else
				  startPhase(PumpingDutyCycleOnTime);
      }
		break;
	}
	
	//CQLog::Write(LOG_TAG_GENERAL,"MRW: Exiting PerformControlProcedure");	
}

void ResinFlowUtils::PumpControllerBase::startPhase(Phase newPhase)
{
	if (PumpingDutyCycleOnTime == newPhase)
	{
    	// todo 1 -oArcady.Volman@objet.com -cSafety: what if it is already on ?
		_pBackEnd->SetActuatorState(PumpsActuatorID[_pumpIndex], ACTUATOR_ON);
	}
	else
	{
		if (PumpingDutyCycleOnTime == _phase)
			_totalActualPumpingMilliseconds += QTicksToMs(currentPhaseElapsedTicks());

//    	if (_pBackEnd->GetActuatorOnOff(PumpsActuatorID[_pumpIndex]))
			_pBackEnd->SetActuatorState(PumpsActuatorID[_pumpIndex], ACTUATOR_OFF);
	}
		
	_phase					= newPhase;
	_phaseStartTimeInTicks = QGetTicks();
}

void ResinFlowUtils::PumpControllerBase::suspend()
{
	startPhase(Suspended);
}

bool ResinFlowUtils::PumpControllerBase::isChamberFull() const
{
  return _pBackEnd->AreResinHeadsFilled(ThermistorIndexByPumpIndex(_pumpIndex));
}

CHAMBERS_THERMISTORS_EN ResinFlowUtils::PumpControllerBase::ThermistorIndexByPumpIndex(TPumpIndex pump) const
{
	CHAMBERS_THERMISTORS_EN LowTherm  = GetTankRelatedLowThermistor(PumpToTank(pump));
	CHAMBERS_THERMISTORS_EN HighTherm = GetTankRelatedHighThermistor(PumpToTank(pump));	

	if (true == _pParamMgr->ActiveThermistors[LowTherm])
		return LowTherm;
	else
		return HighTherm;
}

bool ResinFlowUtils::PumpControllerBase::isPhaseOver() const
{
	using namespace ResinFlowUtils;
	assert(_dutyCycleOnMilliseconds >= 0);
	assert(_dutyCycleOffMilliseconds >= 0);
    switch(_phase)
    {
        case PumpingDutyCycleOnTime:
            return QTicksToMs(currentPhaseElapsedTicks()) >= _dutyCycleOnMilliseconds;
        case PumpingDutyCycleOffTime:
            return QTicksToMs(currentPhaseElapsedTicks()) >= _dutyCycleOffMilliseconds;
        case WaitingForChamberToBeNotFull:
        case Done:
            return false;
        default:
            assert(false);
            return false;

    }
}


TQTicks ResinFlowUtils::PumpControllerBase::currentPhaseElapsedTicks() const
{
    return QGetTicks() - _phaseStartTimeInTicks; // todo 3 -oArcady.Volman@objet.com -cSafety: QGetTicksSince
}

TQMilliseconds ResinFlowUtils::PumpControllerBase::totalActualPumpingMilliseconds() const
{
	TQMilliseconds ms = _totalActualPumpingMilliseconds;////Actual pumping time not including current phase
	if (PumpingDutyCycleOnTime == _phase)
		ms += QMsToTicks(currentPhaseElapsedTicks());
	return ms;
}

void ResinFlowUtils::PumpControllerBase::resetTotalActualPumpingTime()
{
	_totalActualPumpingMilliseconds = 0;
}


bool ResinFlowUtils::PumpControllerBase::isDone()
{
    return Done == _phase;
}

bool ResinFlowUtils::PumpControllerBase::isCurrentCycleDone()
{
    return ((WaitingForChamberToBeNotFull == _phase) || isDone());
}

// PumpControllerBase::verifyTankValid() checks the essentials like expiration date.
void ResinFlowUtils::PumpControllerBase::verifyTankValid()
{
	TTankIndex ti = tankIndexByPumpIndex(_pumpIndex);
	if (_pBackEnd->IsResinExpired(ti) || _pBackEnd->IsMaxPumpTimeExceeded(ti))
	{
		startPhase(Suspended);
    	CQLog::Write(LOG_TAG_GENERAL,QFormatStr("ResinFlowUtils:verifyTankValid Tank %s Tank Expired", TankToStr(ti).c_str()).c_str());
		throw ResinFlowUtils::ETankInvalid(ti);
	}
}

void ResinFlowUtils::PumpControllerBase::verifyTankWeight()
{
	TTankIndex ti = tankIndexByPumpIndex(_pumpIndex);
	if (_pBackEnd->GetTankWeightInGram(ti) < _pParamMgr->WeightLevelLimitArray[TankToStaticChamber(ti)])
	{
		startPhase(Suspended);
    	CQLog::Write(LOG_TAG_GENERAL,QFormatStr("ResinFlowUtils:verifyTankWeight Tank %s TankWeightInGram %f gr WeightLimit %d", TankToStr(ti).c_str(), _pBackEnd->GetTankWeightInGram(ti), _pParamMgr->WeightLevelLimitArray[TankToStaticChamber(ti)]).c_str());
		throw ResinFlowUtils::ETankOutOfResin(ti);
	}
}

void ResinFlowUtils::PumpControllerBase::verifyTankEnabled()
{
	TTankIndex ti = tankIndexByPumpIndex(_pumpIndex);
	if (!_pBackEnd->IsTankInAndEnabled((int)ti))
	{
		startPhase(Suspended);
    	CQLog::Write(LOG_TAG_GENERAL,QFormatStr("ResinFlowUtils:verifyTankEnabled Tank %s TankDisabled", TankToStr(ti).c_str()).c_str());
		throw ResinFlowUtils::ETankDisabled(ti);
	}
}

void ResinFlowUtils::PumpControllerBase::verifyTankResinType()
{
	TTankIndex ti = tankIndexByPumpIndex(_pumpIndex);

#ifdef _DEBUG
    if(FindWindow(0, "TankBadResinType.txt - Notepad"))
	  throw ResinFlowUtils::ETankBadResinType(ti);
#endif

  if (CHECK_EMULATION(_pParamMgr->OCB_Emulation))
    return;

	if (_materialName != _pBackEnd->GetTankMaterialType(ti))
	{
		startPhase(Suspended);
    	CQLog::Write(LOG_TAG_GENERAL,QFormatStr("ResinFlowUtils:verifyTankResinType Tank %s _materialName %s TankMaterialType %s", TankToStr(ti).c_str() ,_materialName.c_str(), _pBackEnd->GetTankMaterialType(ti).c_str()).c_str());
		throw ResinFlowUtils::ETankBadResinType(ti);
	}
}

void ResinFlowUtils::PumpControllerBase::restartController()
{
	_phase = Suspended;
	_phaseStartTimeInTicks = QGetTicks();
	_totalActualPumpingMilliseconds = 0;
}
	
////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////   Specific Pump Controllers    ///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////


void ResinFlowUtils::PumpControllerForOneFilling::preControlProcedure()
{
	if (totalActualPumpingMilliseconds() >= _fillingDurationLimit)
	{
		startPhase(Suspended);
        throw ETimeout("pump " + PumpIndexToDescription(_pumpIndex) + " operation, during head filling.");
	}

	if (isChamberFull())
	{
		// todo -oArcady.Volman@objet.com -cNone: log with wizard info
		/*
		_pWizard->WriteToLogFile(LOG_TAG_GENERAL,
					QFormatStr("Pump %s total filling time: %dms"
								_pumpDescription, _totalActualPumpingMilliseconds));
								*/
		startPhase(Done);
        return;
	}

	ResinFlowUtils::PumpControllerBase::preControlProcedure();
}



void ResinFlowUtils::PumpControllerForFillingCycles::preControlProcedure()
{
	if ((WaitingForChamberToBeNotFull == _phase) && (false == _enteringWaitingForChamberToBeNotFull))
	{
		_enteringWaitingForChamberToBeNotFull = true; // _enteringWaitingForChamberToBeNotFull should be simply set to 'false' once we leave state WaitingForChamberToBeNotFull. (no need for outside intervention) synchronization is done be mutual Airvalve.
		_numOfFillingCycles--;
	}

	if (WaitingForChamberToBeNotFull != _phase)
		_enteringWaitingForChamberToBeNotFull = false;

	ResinFlowUtils::PumpControllerForOneFilling::preControlProcedure();
  if (Done == _phase) // if PumpControllerForOneFilling has set Done phase, override it, since we may have more then one cycle to do here, in PumpControllerForFillingCycles.
  {
    _phase = WaitingForChamberToBeNotFull;
    resetTotalActualPumpingTime(); // reset filling time accumulation. We count from 0 for each new filling cycle.
  }

	if (0 == _numOfFillingCycles)
	{
		startPhase(Done);
		_enteringWaitingForChamberToBeNotFull = false;
        return;
	}
}
	
	
	
void ResinFlowUtils::PumpControllerForFlushing::preControlProcedure()
{

	// is pumping done ?
	if (totalActualPumpingMilliseconds() >= _requiredNetPumpingMilliseconds)
	{
		// todo -oArcady.Volman@objet.com -cNone: log with wizard info
		/*
		_pWizard->WriteToLogFile(LOG_TAG_GENERAL,
					QFormatStr("Pump %s total filling time: %dms"
								_pumpDescription, _totalActualPumpingMilliseconds));
								*/
		startPhase(Done);
        return;
	}

	// if timeout waiting for non-empty chamber
	if (WaitingForChamberToBeNotFull == _phase
		&& currentPhaseElapsedTicks() >= QMsToTicks(_chamberTimeoutInMilliseconds))
	{
		startPhase(Suspended);
        throw ETimeout("pump " + PumpIndexToDescription(_pumpIndex) + " operation, while waiting for the chamber to be not full.");
	}
	ResinFlowUtils::PumpControllerBase::preControlProcedure();

}

void ResinFlowUtils::PumpControllerForContinuousFilling::preControlProcedure()
{
	PumpControllerForFlushing::preControlProcedure();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////


TChamberIndex ResinFlowUtils::chamberIndexByPumpIndex(TPumpIndex pump)// todo 3 -oArcady.Volman@objet.com -cNone: Add ifdefs for other machines
{
	switch (pump)
	{
//OBJET_MACHINE adjustment
	case MODEL1_PUMP:
	case MODEL2_PUMP:
		return TYPE_CHAMBER_MODEL1;
	case MODEL3_PUMP:
	case MODEL4_PUMP:
		return TYPE_CHAMBER_MODEL2;
	case MODEL5_PUMP:
	case MODEL6_PUMP:
		return TYPE_CHAMBER_MODEL3;
	case SUPPORT_PUMP1:
	case SUPPORT_PUMP2:
		return TYPE_CHAMBER_SUPPORT;
#ifdef OBJET_MACHINE_KESHET
	case MODEL7_PUMP:
	case MODEL8_PUMP:
		return TYPE_CHAMBER_MODEL4;
	case MODEL9_PUMP:
	case MODEL10_PUMP:
		return TYPE_CHAMBER_MODEL5;
	case MODEL11_PUMP:
	case MODEL12_PUMP:
		return TYPE_CHAMBER_MODEL6;
	case MODEL13_PUMP:
	case MODEL14_PUMP:
		return TYPE_CHAMBER_MODEL7;
#endif
	default:
		throw EQException("Invalid pump ID");
        }
}

TResinTypeIndex ResinFlowUtils::PumpIndexByResinTypeIndex(TPumpIndex pump)
{
   return TankIndexByResinTypeIndex(tankIndexByPumpIndex(pump));
}

TResinTypeIndex ResinFlowUtils::TankIndexByResinTypeIndex(TTankIndex tank)
{
  if (IS_MODEL_TANK(tank))
     return TYPE_RESIN_MODEL;                  
  if (IS_SUPPORT_TANK(tank))
     return TYPE_RESIN_SUPPORT;

  throw EQException("Invalid tank ID");
//  return TYPE_RESIN_SUPPORT;
}

TTankIndex ResinFlowUtils::tankIndexByPumpIndex(TPumpIndex pump) // todo -oArcady.Volman@objet.com -cNone: Add ifdefs for other machines
{
	switch (pump)
	{
//OBJET_MACHINE adjustment
		case MODEL1_PUMP:
			return TYPE_TANK_MODEL1;
		case MODEL2_PUMP:
			return TYPE_TANK_MODEL2;
		case MODEL3_PUMP:
			return TYPE_TANK_MODEL3;
		case MODEL4_PUMP:
			return TYPE_TANK_MODEL4;
		case MODEL5_PUMP:
			return TYPE_TANK_MODEL5;
		case MODEL6_PUMP:
			return TYPE_TANK_MODEL6;
		case SUPPORT_PUMP2:
			return TYPE_TANK_SUPPORT2;
		case SUPPORT_PUMP1:
			return TYPE_TANK_SUPPORT1;
#ifdef OBJET_MACHINE_KESHET
		case MODEL7_PUMP:
			return TYPE_TANK_MODEL7;
		case MODEL8_PUMP:
			return TYPE_TANK_MODEL8;
		case MODEL9_PUMP:
			return TYPE_TANK_MODEL9;
		case MODEL10_PUMP:
			return TYPE_TANK_MODEL10;
		case MODEL11_PUMP:
			return TYPE_TANK_MODEL11;
		case MODEL12_PUMP:
			return TYPE_TANK_MODEL12;
		case MODEL13_PUMP:
			return TYPE_TANK_MODEL13;
		case MODEL14_PUMP:
			return TYPE_TANK_MODEL14;
#endif
	}

	return TYPE_TANK_MODEL1; // to silent the compiler warning
}

ResinFlowUtils::TPumpIndicesVector ResinFlowUtils::pumpIndicesByTankIndex(TTankIndex tank)
{
	ResinFlowUtils::TPumpIndicesVector v;

    // TODO 5 -oPublic -cMaintainability : !!! There is an intentional swap between tank1 and tank2 indices here because it is swapped throughout the system. This is how it works today. Should be fixed.
	switch(tank)
	{
    case TYPE_TANK_SUPPORT1:
		v.push_back(SUPPORT_PUMP1);
		break;
//OBJET_MACHINE adjustment
    case TYPE_TANK_SUPPORT2:
		v.push_back(SUPPORT_PUMP2);
		break;
	case TYPE_TANK_MODEL1:
		v.push_back(MODEL1_PUMP);
		break;
	case TYPE_TANK_MODEL2:
		v.push_back(MODEL2_PUMP);
		break;
	case TYPE_TANK_MODEL3:
		v.push_back(MODEL3_PUMP);
				break;
	case TYPE_TANK_MODEL4:
		v.push_back(MODEL4_PUMP);
				break;
	case TYPE_TANK_MODEL5:
		v.push_back(MODEL5_PUMP);
				break;
	case TYPE_TANK_MODEL6:
		v.push_back(MODEL6_PUMP);
				break;
#ifdef OBJET_MACHINE_KESHET
	case TYPE_TANK_MODEL7:
		v.push_back(MODEL7_PUMP);
		break;
	case TYPE_TANK_MODEL8:
		v.push_back(MODEL8_PUMP);
		break;
	case TYPE_TANK_MODEL9:
		v.push_back(MODEL9_PUMP);
				break;
	case TYPE_TANK_MODEL10:
		v.push_back(MODEL10_PUMP);
				break;
	case TYPE_TANK_MODEL11:
		v.push_back(MODEL11_PUMP);
				break;
	case TYPE_TANK_MODEL12:
		v.push_back(MODEL12_PUMP);
				break;
	case TYPE_TANK_MODEL13:
		v.push_back(MODEL13_PUMP);
				break;
	case TYPE_TANK_MODEL14:
		v.push_back(MODEL14_PUMP);
				break;
#endif
	}
	return v;
};

ResinFlowUtils::TPumpIndicesVector ResinFlowUtils::pumpIndicesVectorByTankIndicesVector(TTankIndexVector& v) // todo -oArcady.Volman@objet.com -cNone: maybe rename to x_To_y ? 
{// todo -oArcady.Volman@objet.com -cNone: rename indices... to index... ?
	// for all tanks
	ResinFlowUtils::TPumpIndicesVector res;
	for (TTankIndexVectorIter iterTankIndex = v.begin(); iterTankIndex != v.end(); iterTankIndex++)
	{
		// add all tank's pumps
		ResinFlowUtils::TPumpIndicesVector pumpIndices = ResinFlowUtils::pumpIndicesByTankIndex(*iterTankIndex);
		std::copy(pumpIndices.begin(), pumpIndices.end(), std::back_inserter(res));
	}
	return res;
}

//--------------------------------------------------------------------------- ResinFlowUtils::PumpControllers --------------------------------------------------------------------------
ResinFlowUtils::PumpControllers::PumpControllers(CQ2RTAutoWizard *pWizard)
 : ResinFlowUtils::Interfacer(pWizard)
{
    _isCurrentPumpingCycleDone = false;
    _isControllerOperationDone = false;
    assert(0 == _pumpControllerPointersVector.size());
}

ResinFlowUtils::PumpControllers::~PumpControllers()
{
	deletePumpControllers();
}

// todo -oArcady.Volman@objet.com -cNone: limit blockcycles & other paramas
void ResinFlowUtils::PumpControllers::AddSpecializedPumpController(
							PumpingMode 				mode,
                               TPumpIndex		  		pumpIndex,
                               TQMilliseconds	  		requiredNetPumpingTime,
                               TQMilliseconds	  		chamberWaitingDurationLimit, // (timeout)
                               TQMilliseconds	  		dutyCycleOnMilliseconds,
                               TQMilliseconds	  		dutyCycleOffMilliseconds,
							   VerificationAttributes 	verificationAttributes,
							   QString 					materialName,
							   int 						requiredNumOfFillingCycles)
							   
{

    PumpControllerBase *ppc = 0;

	switch (mode)
	{
	case PipesPriming:
		
		ppc =
			new PumpControllerForContinuousFilling(	_pWizard,
													pumpIndex,
													requiredNetPumpingTime,
													chamberWaitingDurationLimit,
													dutyCycleOnMilliseconds,
													dutyCycleOffMilliseconds,
													verificationAttributes,
													materialName);

		break;
	case PipesWashing:

		ppc =
			new PumpControllerForContinuousFilling(	_pWizard,
													pumpIndex,
													requiredNetPumpingTime,
													chamberWaitingDurationLimit,
													dutyCycleOnMilliseconds,
													dutyCycleOffMilliseconds,
													verificationAttributes,
													materialName);
													
			break;
	case BlockFilling:

		ppc =
			new PumpControllerForOneFilling(		_pWizard,
													pumpIndex,													
													chamberWaitingDurationLimit,
													dutyCycleOnMilliseconds,
													dutyCycleOffMilliseconds,
													verificationAttributes,
													materialName);
													
		break;
	case PipesEmptying: // todo -oArcady.Volman@objet.com -cNone: rename to pipes draining?

		ppc =
			new PumpControllerForFlushing(			_pWizard,
													pumpIndex,
													requiredNetPumpingTime,
													chamberWaitingDurationLimit,
													dutyCycleOnMilliseconds,
													dutyCycleOffMilliseconds,
													verificationAttributes,
													materialName);										

		break;
	case FillingCycles:

		ppc =
			new PumpControllerForFillingCycles(		_pWizard,
													pumpIndex,
													requiredNumOfFillingCycles,
													chamberWaitingDurationLimit,
													dutyCycleOnMilliseconds,
													dutyCycleOffMilliseconds,
													verificationAttributes,
													materialName);										

		break;
				
	default:
		assert(false);
		throw EQException("bad PumpingMode");
	}

	assert(ppc);
    if (!ppc) throw EQException("Could not create pump controller object");

	_pumpControllerPointersVector.push_back(ppc);
	// =======================================================
}

void ResinFlowUtils::PumpControllers::deletePumpControllers()
{
   //	CQLog::Write(LOG_TAG_GENERAL, "deleting all pump controllers");
	for (ResinFlowUtils::TPumpControllerPointerVector::iterator i =
						_pumpControllerPointersVector.begin();
							i != _pumpControllerPointersVector.end(); ++i)
	{
		Q_SAFE_DELETE(*i);
	}
	_pumpControllerPointersVector.clear();
}

void ResinFlowUtils::PumpControllers::suspendAllPumpControllers()
{
	CQLog::Write(LOG_TAG_GENERAL, "suspending all pumps");
	for(ResinFlowUtils::TPumpControllerPointerVector::iterator itrPumpCtrlrPtr =  // todo -oArcady.Volman@objet.com -cNone: MAKE NAMES SHORTER
										_pumpControllerPointersVector.begin();
		itrPumpCtrlrPtr != _pumpControllerPointersVector.end();
	  ++itrPumpCtrlrPtr
	)
	{
		(*itrPumpCtrlrPtr)->suspend();
	}
}


///////////////////////////

void ResinFlowUtils::PumpControllers::controlAllPumps()
{
	_isCurrentPumpingCycleDone = true; // we assume we're done unless, in the for loop we'll see that one of the controllers is not done.
	_isControllerOperationDone = true;
			
	// for each pump controller
	for(ResinFlowUtils::TPumpControllerPointerVector::iterator itrPumpCtrlrPtr = _pumpControllerPointersVector.begin();
		itrPumpCtrlrPtr != _pumpControllerPointersVector.end();
	  ++itrPumpCtrlrPtr
		)
	{
		if (_pWizard->IsCancelled()) // todo -oArcady.Volman@objet.com -cNone: what to do ?!?!
			break;

		(*itrPumpCtrlrPtr)->performControlProcedure(); // pause/resume pumping (according to relevant thermistor), and record pumping time (if pumping in progress)

		_isCurrentPumpingCycleDone = _isCurrentPumpingCycleDone && (*itrPumpCtrlrPtr)->isCurrentCycleDone();
		_isControllerOperationDone = _isControllerOperationDone && (*itrPumpCtrlrPtr)->isDone(); // todo 5 -oArcady.Volman@objet.com -cEmulation : Allow completing the wizard in emulation
	}
}

void ResinFlowUtils::PumpControllers::restartAllControllers()
{
			// for each pump controller
			for(ResinFlowUtils::TPumpControllerPointerVector::iterator itrPumpCtrlrPtr =
												_pumpControllerPointersVector.begin();
				itrPumpCtrlrPtr != _pumpControllerPointersVector.end();
			  ++itrPumpCtrlrPtr
			)
			{
				clearControllerOperationDoneFlag();
                (*itrPumpCtrlrPtr)->restartController(); // pause/resume pumping (according to relevant thermistor), and record pumping time (if pumping in progress)
			}
}

void ResinFlowUtils::PumpControllers::getPumpsVector(TPumpIndicesVector& pumpsVector)
{
	for(ResinFlowUtils::TPumpControllerPointerVector::iterator itrPumpCtrlrPtr =
										_pumpControllerPointersVector.begin();
		itrPumpCtrlrPtr != _pumpControllerPointersVector.end(); ++itrPumpCtrlrPtr)
	{
		// Append Controller's pumps to the vector passed.
		pumpsVector.push_back((*itrPumpCtrlrPtr)->getPumpIndex());
	}
}
#pragma package(smart_init)






