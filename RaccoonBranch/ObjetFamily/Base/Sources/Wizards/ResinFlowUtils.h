//---------------------------------------------------------------------------

#ifndef ResinFlowUtilsH
#define ResinFlowUtilsH

#include "Q2RTWizard.h"
#include "BackEndInterface.h"
#include "QTypes.h"
#include "QOSWrapper.h"
#include "ProgressReporting.h"
#include <map>
#include <limits>

#include "QTimer.h" // TODO 4 -oArcady.Volman@objet.com -ccompilation : only for QGetTicks. Remove

namespace ResinFlowUtils
{

typedef enum
{
	PipesPriming, // first washing cycle
	PipesWashing, // second washing cycle
	PipesEmptying,
	BlockFilling,
	FillingCycles
} PumpingMode;

typedef enum
{
	vaTankWeight		= 1 << 0
	,vaTankEnabled		= 1 << 1
	,vaTankResinType	= 1 << 2
	,vaTankValid	    = 1 << 3
} VerificationAttributes;

class PumpControllerBase; // forward declaration.

typedef std::vector<PumpControllerBase *> TPumpControllerPointerVector;

typedef std::vector<TPumpIndex> TPumpIndicesVector;

class EResinFlowException : public EQException
{
public:
	TTankIndex _tankIndex;
    EResinFlowException(TTankIndex i);
};

class ETankInvalid : public EResinFlowException
{    
public:
	ETankInvalid(TTankIndex i);
};

class ETankOutOfResin : public EResinFlowException
{
public:
    ETankOutOfResin(TTankIndex i);
};

class ETankDisabled : public EResinFlowException
{
public:
    ETankDisabled(TTankIndex i);
};

class ETankBadResinType : public EResinFlowException
{
public:
    ETankBadResinType(TTankIndex i); 
};

class Interfacer
{
protected:
	CQ2RTAutoWizard		*_pWizard;
	CBackEndInterface	*_pBackEnd;
	CAppParams			*_pParamMgr;

	Interfacer(CQ2RTAutoWizard *pW = 0);
};

class AirValve : public Interfacer
{
	enum TMode {TryToDrainAllChambers, UntilTimeIsUp};
	
	void holdOpen(	TMode mode,
					TQMilliseconds timeout,
					ProgressReporting::CProgressListenerInterface *pPL = 0); 
public:
	
	AirValve::AirValve(CQ2RTAutoWizard *pWizard = 0);
	
	void holdOpen(TQMilliseconds timeout, ProgressReporting::CProgressListenerInterface *pPL = 0);
	void holdOpenUntilNoFullChambers(TQMilliseconds timeout, ProgressReporting::CProgressListenerInterface *pPL = 0);
	void close();
	void open();
	
	~AirValve();

};

class PumpControllers : public Interfacer
{
	TPumpIndicesVector				_requiredPumpIndicesVector;
	TPumpControllerPointerVector 	_pumpControllerPointersVector;
	TQMilliseconds 					_requiredNetPumpingMilliseconds;
    bool 							_isCurrentPumpingCycleDone;
    bool 							_isControllerOperationDone;

public:	
	PumpControllers(CQ2RTAutoWizard *pWizard);
	~PumpControllers();  

  void deletePumpControllers();
  void suspendAllPumpControllers();
  void controlAllPumps();
  void restartAllControllers();
  void clearCurrentPumpingCycleDoneFlag();
  bool isCurrentPumpingCycleDone();
  void clearControllerOperationDoneFlag();
  bool isControllerOperationDone();
  
  bool isPumpsVectorEmpty();
  void getPumpsVector(TPumpIndicesVector& pumpsVector);

  void AddSpecializedPumpController(
                               PumpingMode				pumpingMode,
                               TPumpIndex				pumpIndex,
                               TQMilliseconds			requiredNetPumpingTime,
                               TQMilliseconds			chamberWaitingDurationLimit, // (timeout)
                               TQMilliseconds			dutyCycleOnMilliseconds		= numeric_limits<TQMilliseconds>::max(),
                               TQMilliseconds			dutyCycleOffMilliseconds	= 0,
							   VerificationAttributes 	verificationAttributes		= static_cast<VerificationAttributes>(vaTankValid | vaTankWeight | vaTankEnabled),
							   QString 					materialName				= QString(""),
							   int 						requiredNumOfFillingCycles  = 0);					   							  
};

class PumpControllerBase : public Interfacer
{
	friend bool lessPumped(const PumpControllerBase *p1, const PumpControllerBase *p2);
public:
	enum Phase
	{
		Suspended,
		WaitingForChamberToBeNotFull,
	    PumpingDutyCycleOnTime,
	    PumpingDutyCycleOffTime,
	    Done
	};
	
	#define CASE_ID_RETURN_STR(id) case id: return #id;
	QString PhaseToDescription(Phase p)
	{
		switch(p)
		{
			CASE_ID_RETURN_STR(Suspended)
			CASE_ID_RETURN_STR(WaitingForChamberToBeNotFull)
			CASE_ID_RETURN_STR(PumpingDutyCycleOnTime)
			CASE_ID_RETURN_STR(PumpingDutyCycleOffTime)
			CASE_ID_RETURN_STR(Done)
		default:
			throw EQException("Invalid phase");
		}
	}
	
protected:

	TPumpIndex	_pumpIndex;
	QString		_pumpDescription;
	Phase		_phase;
private:

	TQTicks 				_phaseStartTimeInTicks;
	TQMilliseconds 			_totalActualPumpingMilliseconds; //Actual pumping time not including current phase
	TQMilliseconds 			_dutyCycleOnMilliseconds;
	TQMilliseconds 			_dutyCycleOffMilliseconds;
	VerificationAttributes 	_verificationAttributes;
	QString 				_materialName;

	bool isPhaseOver() const;


	
protected:

	void startPhase(Phase ph); // todo -oArcady.Volman@objet.com -cNone: RENAME
	
	TQTicks currentPhaseElapsedTicks() const;
	void verifyTankValid(); // throws ETankInvalid
	void verifyTankWeight(); // throws ETankOutOfResin
	void verifyTankEnabled(); // throws ETankDisabled
	void verifyTankResinType(); // throws ETankBadResinType

	PumpControllerBase(
					CQ2RTAutoWizard			*pWizard,
					TPumpIndex				pumpIndex,
					TQMilliseconds			dutyCycleOnMilliseconds 	= numeric_limits<TQMilliseconds>::max(),
					TQMilliseconds			dutyCycleOffMilliseconds 	= 0,
					VerificationAttributes 	verificationAttributes 		= static_cast<VerificationAttributes>(vaTankValid | vaTankWeight | vaTankEnabled),
					QString 				materialName 				= QString(""));

	virtual void preControlProcedure()
	{
    if (vaTankValid &    _verificationAttributes)
      verifyTankValid(); // throws ETankInvalid

    if (vaTankWeight &    _verificationAttributes)
      verifyTankWeight(); // throws ETankOutOfResin

    if (vaTankEnabled &   _verificationAttributes)
      verifyTankEnabled(); // throws ETankDisabled

    if (vaTankResinType & _verificationAttributes)
      verifyTankResinType(); // throws ETankBadResinType
	}

	bool isChamberFull() const;
	CHAMBERS_THERMISTORS_EN ThermistorIndexByPumpIndex(TPumpIndex pump) const;

public:


    virtual ResinFlowUtils::PumpControllerBase::~PumpControllerBase();


	virtual void performControlProcedure();

	TQMilliseconds totalActualPumpingMilliseconds() const; // TODO 3 -oArcady.Volman@objet.com -cSafety : should be private in order not to encourage the user skipping performeControlProcedure. for progress reporting a pumps manager class should be used.
	void resetTotalActualPumpingTime();

    void suspend();
	

	bool isDone();
	bool isCurrentCycleDone();
	
	void restartController();
	TPumpIndex getPumpIndex() {return _pumpIndex;}
	
}; // end of PumpControllerBase

///////////////////////////////////   Specific Pump Controllers    ///////////////////////////////////////

class PumpControllerForOneFilling : public PumpControllerBase
{
	TQMilliseconds _fillingDurationLimit;
protected:
	virtual void preControlProcedure();
public:
	// constructor
	PumpControllerForOneFilling(	
			CQ2RTAutoWizard	*pWizard,
			TPumpIndex				pumpIndex,
			TQMilliseconds			fillingDurationLimit, // (timeout)
			TQMilliseconds			dutyCycleOnMilliseconds 	= numeric_limits<TQMilliseconds>::max(),
			TQMilliseconds			dutyCycleOffMilliseconds 	= 0,
			VerificationAttributes 	verificationAttributes 		= static_cast<VerificationAttributes>(vaTankValid | vaTankWeight | vaTankEnabled),
			QString 				materialName 				= QString(""))
								
	 :
				PumpControllerBase(	pWizard, 
									pumpIndex, 
									dutyCycleOnMilliseconds, 
									dutyCycleOffMilliseconds, 
									verificationAttributes, 
									materialName),
		_fillingDurationLimit(fillingDurationLimit)
	{
	}
};

class PumpControllerForFlushing : public PumpControllerBase
{
	TQMilliseconds _requiredNetPumpingMilliseconds;
	TQMilliseconds _chamberTimeoutInMilliseconds;  // (WaitingForChamberToBeNotFull  timeout)

protected:
	virtual void preControlProcedure();
public:
	// constructor
	PumpControllerForFlushing(	
			CQ2RTAutoWizard			*pWizard,
			TPumpIndex				pumpIndex,
			TQMilliseconds			requiredNetPumpingTime,
			TQMilliseconds			chamberWaitingDurationLimit, // (timeout)
			TQMilliseconds			dutyCycleOnMilliseconds 	= numeric_limits<TQMilliseconds>::max(),
			TQMilliseconds			dutyCycleOffMilliseconds 	= 0,
			VerificationAttributes 	verificationAttributes 		= static_cast<VerificationAttributes>(vaTankValid | vaTankWeight | vaTankEnabled),
			QString 				materialName 				= QString(""))
								
	 :
				PumpControllerBase(	pWizard, 
									pumpIndex, 
									dutyCycleOnMilliseconds, 
									dutyCycleOffMilliseconds, 
									verificationAttributes, 
									materialName),
		_requiredNetPumpingMilliseconds	(requiredNetPumpingTime),
		_chamberTimeoutInMilliseconds	(chamberWaitingDurationLimit)
	{
	}
};

//The difference between this class and PumpControllerForFlushing is that this one makes sure that there is actual material in hte tank we are pumping from ( the tank is tnserted and there is enough resin in it) 
class PumpControllerForContinuousFilling : public PumpControllerForFlushing
{
protected:
	virtual void preControlProcedure();
public:
	// constructor
	PumpControllerForContinuousFilling(	
			CQ2RTAutoWizard			*pWizard,
			TPumpIndex				pumpIndex,
			TQMilliseconds			requiredNetPumpingTime,
			TQMilliseconds			chamberWaitingDurationLimit, // (timeout)
			TQMilliseconds			dutyCycleOnMilliseconds 	= numeric_limits<TQMilliseconds>::max(),
			TQMilliseconds			dutyCycleOffMilliseconds 	= 0,
			VerificationAttributes 	verificationAttributes 		= static_cast<VerificationAttributes>(vaTankValid | vaTankWeight | vaTankEnabled),
			QString 				materialName 				= QString(""))
								
	 :
		PumpControllerForFlushing(	pWizard,
									pumpIndex,
									requiredNetPumpingTime,
									chamberWaitingDurationLimit,
									dutyCycleOnMilliseconds,
									dutyCycleOffMilliseconds,
									verificationAttributes,
									materialName)
	{
	}
};

// Following controller will perform a predefined number of block filling-cycles. It supports a synchronization method.
class PumpControllerForFillingCycles : public PumpControllerForOneFilling
{
	int 			_numOfFillingCycles;
	bool			_enteringWaitingForChamberToBeNotFull;
	TQMilliseconds 	_chamberTimeoutInMilliseconds;  // (WaitingForChamberToBeNotFull  timeout)

protected:
	virtual void preControlProcedure();
public:
	// constructor
	PumpControllerForFillingCycles(	
			CQ2RTAutoWizard			*pWizard,
			TPumpIndex				pumpIndex,
			int						requiredNumOfFillingCycles,
			TQMilliseconds			chamberWaitingDurationLimit, // (timeout)
			TQMilliseconds			dutyCycleOnMilliseconds 	= numeric_limits<TQMilliseconds>::max(),
			TQMilliseconds			dutyCycleOffMilliseconds 	= 0,
			VerificationAttributes 	verificationAttributes 		= static_cast<VerificationAttributes>(vaTankValid | vaTankWeight | vaTankEnabled),
			QString 				materialName 				= QString(""))
								
	 :
				PumpControllerForOneFilling(	
									pWizard, 
									pumpIndex,
									chamberWaitingDurationLimit,
									dutyCycleOnMilliseconds, 
									dutyCycleOffMilliseconds, 
									verificationAttributes, 
									materialName),
		_numOfFillingCycles						(requiredNumOfFillingCycles),
		_enteringWaitingForChamberToBeNotFull 	(false),
		_chamberTimeoutInMilliseconds			(chamberWaitingDurationLimit)
	{
	}
};

//************************************** Utility Types/Functions **************************************
//for progress bar needs; we need to update progress with the pumping time of the pump with the shortest actual pumping time
bool lessPumped(const PumpControllerBase *p1, const PumpControllerBase *p2)
{
	return p1->totalActualPumpingMilliseconds() < p2->totalActualPumpingMilliseconds();
}

// todo -oBojena.Gleizer@objet.com -cNone:currently this function disregards operation mode!!! 
TPumpIndicesVector pumpIndicesByTankIndex(TTankIndex tank);// todo -oArcady.Volman@objet.com -cNone: maybe rename to x_To_y ?

TResinTypeIndex PumpIndexByResinTypeIndex(TPumpIndex pump);
TResinTypeIndex TankIndexByResinTypeIndex(TTankIndex tank);

TPumpIndicesVector pumpIndicesVectorByTankIndicesVector(TTankIndexVector& v); // todo -oArcady.Volman@objet.com -cNone: maybe rename to x_To_y ?

// Deprecated. Allows the old ResinReplacementWizard to compile.
TChamberIndex chamberIndexByPumpIndex(TPumpIndex pump);
TTankIndex tankIndexByPumpIndex(TPumpIndex pump);

}

template<typename T> T QOverflowSafeDiff(T high, T low)
{
	if (high >= low)
	    return high - low;
	else
		return numeric_limits<T>::max() - high + low;
}

TQTicks QGetTicksSince(TQTicks startTime)
{
	return QOverflowSafeDiff<TQTicks>(QGetTicks(), startTime);
}
//---------------------------------------------------------------------------
#endif
