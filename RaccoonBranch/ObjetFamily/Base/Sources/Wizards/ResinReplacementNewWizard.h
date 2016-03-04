
#ifndef _RESIN_REPLACEMENT_NEW_WIZARD_H_
#define _RESIN_REPLACEMENT_NEW_WIZARD_H_

#include "WizardPages.h"
#include "BackEndInterface.h"
#include "Q2RTWizard.h"

#include "CustomResinSelectSummaryPage.h"
#include "ResinReplacementPipeline.h"
#include "CustomResinSelectSummaryFrame.h"
#include "ResinFlowUtils.h"

typedef std::vector <TTankIndex> TTanksIndicesVector;

enum TMaterialParametersUpdateMode {MR_Undefine, MR_Update};

class CResinReplacementNewWizard : public CQ2RTAutoWizard
{
private:
    struct TSection
	{
		int Cycles;
		int NumPumps;
		int SectionTimeSec;
		int SingleCycleTimeSec;
	};

	struct TProgress
	{
		TProgress() : CurrentProgress(0),
					  SinglePurgeCyclePercent(0),
					  CombinedCycles(0),
					  PerformedSections(0),
					  OverallTimeEstimation(0),
					  OverallRemainingTime(0),
					  DrainingPercent(0),
					  PurgesTime(0),
					  DrainingTime(0){}

		std::vector<TSection> ETAEachSection;
		float OverallTimeEstimation;
		float OverallRemainingTime;
		int CurrentProgress;
		int SinglePurgeCyclePercent;
		int DrainingPercent;
		int CombinedCycles;
		int PerformedSections;
		float PurgesTime;
		int DrainingTime;
	};

	QString              	   m_UndefinedStr;
	QString					   m_SupportCleanserName;

	CResinReplacementPipeline *m_Pipeline;
	bool					   m_CalculatedEconomySegments[NUMBER_OF_SEGMENTS];
	bool                       m_UndefinedSegments[NUMBER_OF_SEGMENTS];
	TTanksVector 			   m_TanksForReplacement;

	TThermistorsOperationMode  m_CurrentThermistorsOpMode;
	TTankOperationMode		   m_CurrentTanksOpMode;
	TThermistorsOperationMode  m_NewThermistorsOpMode;
	TTankOperationMode		   m_NewTanksOpMode;
	bool					   m_IsWithinCurrSessionCriticalSection; //True, if any parameter has been modified
	bool					   m_IsHswRecovery;

	bool         			   m_LeavingSummaryPage;
	bool         			   m_PrevHeatersMaskTable[TOTAL_NUMBER_OF_HEATERS];
	bool         			   m_GoToAdvanced;
	TProgress                  m_Progress;
	int						   m_OptimizedEmptyBlockTime;

	ResinFlowUtils::PumpControllers    m_PumpControllers;
	ResinFlowUtils::TPumpIndicesVector m_CurrentPumps;
	bool m_bNeedRestartAfterParamsChanged;  

private:

	QString GetSupportCleanserMaterial() const;
	bool IsSystemAfterShutdown() const;

	//Prepares a list of all the available resins to choose from
	void SetResinsChoices				(CWizardPage* WizardPage);
	void SetResinsAdvancedChoices		(CWizardPage* WizardPage);
	//Backs up the relevant settings from the Advanced Settings page to the Main Settings Page
	void ReflectAdvancedSettings		(CWizardPage* WizardPage, CWizardPage* WizardDestPage);
	//Back up the preferred resin name for single mode in PM
	void SavePreferredResinName			();
	void LoadPreferredResinName			();
	//Sets up the main settings page to show the real state of the machine
	void FetchMachineCurrentState		(CWizardPage* WizardPage);
	bool IsChamberNotFloodedByNeighbour	(TChamberIndex ChamberID) const;
	TSegmentIndex ChamberToLongPipe		(TChamberIndex ChamberID);

	bool PlaceAndCalculateWastePageLeave(CWizardPage* WizardPage);
	void SummaryPageEnter				(CWizardPage* WizardPage);
	bool ApplyPipelineResult			(COptimalTanks *result);

	void BackupAllOpModes				();
	void RestoreAllOpModes				();
	void ApplyThermistorsOpMode			(TThermistorsOperationMode val);
	int  GetOptimizedDrainingTime		(TThermistorsOperationMode val);
	int  GetOptimizedDrainingTime		(TThermistorsOperationMode val, TTanksIndicesVector& activeTanks);
	void SetOptimizedDrainingTime		(TThermistorsOperationMode val, TTanksIndicesVector& activeTanks);
	void ApplyTanksOpMode				(TTankOperationMode val);
	void ApplyPrintingAndPipesOpModes	(TThermistorsOperationMode val);
	void ReInitPages					(); //Init. wizpages' data members

	// Set the MRW_WithinWizard according to given state
	void MarkOnOffCriticalSection		(bool isUndefinedState);
	// Check whether the current session of the wizard is in the pumping/purging phase.
	// It is not the WithinWizard param, but rather initiated with each wizard re-entry  
	bool IsWithinCurrSessionCriticalSection();

	// Start the wizard session event
	void StartEvent();
	// End the wizard session event	- cleaning up all resources / final parameter updates
	void EndEvent();
	// Called after the wizard session ending event - final updates to frontend
	void CloseEvent();
	// Cancel the wizard session event
	void CancelEvent(CWizardPage *WizardPages);
	void HelpEvent(CWizardPage *WizardPage);
	void SetHowRequiredAfterMRW();
	void CleanUp();
	bool CheckTanksValidity(CWizardPage* WizardPage);
	void PerformReplacementCycles(CWizardPage* WizardPage);
	// Prepare for PerformReplacementCycles()
	void BeforePerformReplacementCycles();
	// Teardown after: PerformReplacementCycles()	
	void AfterPerformReplacementCycles(CWizardPage* WizardPage);
	// Updating the Tanks availablity status
	void UpdateSingleTank(TTankIndex Tank,bool isBlocking=false);
	void UpdateAllTanks(bool isBlocking=NO_BLOCKING); //BLOCKING
	// Copy relevant Segments into TypesArrayPerPipe parameter to make it visible outside the MRW scope.
	void UpdateTypesArrayPerPipe();
	// Prepare the parameter(s) marking economy-resins-filled segments
	void CalculateEconomySegments(COptimalTanks* optimized);
	// Update whether or not the segments are being replaced in Economy mode
	void UpdateEconomySegments(bool safeChangesOnly = false);
	// Async wait for Tanks RFID identification process to complete.
	void WaitForIdentificationCompletion(TTankIndex Tank);
	void HeadsHeatingCycle(void);
	bool PurgeCycle(int NumberOfPurgeCycles, CWizardPage* WizardPage);
	// Manipulations with parameter TypesArrayPerSegment
	void MarkTypesArrayPerSegmentUndefined();
	void UpdateTypesArrayPerSegment();
	void TypesArrayPerSegmentSetter(TMaterialParametersUpdateMode mode);
	// Manipulations with parameter TypesArrayPerChamber
	void MarkTypesArrayPerChamberUndefined();
	void UpdateTypesArrayPerChamber();
	void TypesArrayPerChamberSetter(TMaterialParametersUpdateMode mode);
	void UpdateHostTypesArrayPerChamber();
	void UpdateTypesArrayPerTank();

	void LogSegments();
	void LogBlockStatus();
	const QString GetSegmentDescription(TSegmentIndex idx);
	void CalculateEstimatedTime();
	void PopulatePumpControllers();
	void CreatePumpController(ResinFlowUtils::PumpControllers& Controller,
							   ResinFlowUtils::TPumpIndicesVector& Pumps,
							   int Cycles);
	void BlockWashingCycles(CWizardPage* WizardPage);
	TTankIndex GetChamberActiveTankID(TChamberIndex Chamber);
	bool IsTankMarkedForReplacement(TTankIndex TankIndex);
	bool IsWashingComplete(TTanksVector &Tanks);
	void DecrementDoneCycles(int Cycles);
	bool ValidateUserSelections(CWizardPage* WizardPage);
	void LogProgressEstimations();
	void LogRequiredWeights();
	void LogUserSelections(CWizardPage* WizardPage);

public:
	CResinReplacementNewWizard();
	~CResinReplacementNewWizard();

	void PageEventsHandler(CWizardPage *WizardPage,int Param1,int Param2){};
	int  GetContinuePageNumber();
	void Update();

	void UserButton2Event(CWizardPage *WizardPage);
	void PageLeave(CWizardPage *Page, TWizardPageLeaveReason LeaveReason);
	void PageEnter(CWizardPage *Page);

	bool IsRestartRequired()
	{
	  return m_bNeedRestartAfterParamsChanged;
	}
};

#endif //_RESIN_REPLACEMENT_NEW_WIZARD_H_

