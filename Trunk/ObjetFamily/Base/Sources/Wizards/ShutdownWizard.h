/*********************************************************************
*                        Objet Geometries LTD.                       *
*                        -----------------------------               *
* Project: 	Q2RT                                                     *
* Module: 	Machine shutdown.                                        *
* Module Description: 	Machine shutdown wizard					     *
*                                                                  	 *
* Compilation:	Standard C++.                                        *
*                                                                  	 *
* Start date: 16/11/2008											 *
*													                 *
**********************************************************************/


#ifndef _SHUTDOWN_WIZARD_H_
#define _SHUTDOWN_WIZARD_H_

#include "Q2RTWizard.h"
#include "ResinFlowUtils.h"
#include "WizardPages.h"
#include "BackEndInterface.h"
#include "QMutex.h"
#include "ProgressReporting.h"

namespace ShutdownWizard
{

const int SHORT_MODE_OPTION    	= 0;
const int LONG_MODE_OPTION   	= 1;

const int PRE_HEATER_TEMP_A2D = 2000; // Pre-Heater remains cold

const int CLEANING_STEPS = 4;

enum TThermOpMode
{
    omLow, // For pipes washing - first phase
    omHigh // For block washing - second phase
};

typedef std::vector<QString> TCabinetNames;

class CShutdownWizard : public CQ2RTAutoWizard
{
private:

	ResinFlowUtils::PumpControllers m_PumpControllers;
	int  m_WizardModeOption; // selected wizard mode (Short or Long)
	T_AxesTable m_AxesTable1;
    ///////////////////////////////
    TTimer * m_pNextPageTimer;

	// Start the wizard session event
	void StartEvent();

	// End the wizard session event
	void EndEvent();

	// Cancel the wizard session event
	void CancelEvent(CWizardPage *WizardPages);
	void CloseEvent();

	void ShortWizardInProgressPageEnter(CWizardPage *WizardPage);
	void InsertCleanserPageEnter(CWizardPage *WizardPage);
	void LongWizardInProgressPageEnter(CWizardPage *WizardPage);
	void BlockDrainingPageEnter(CWizardPage *WizardPage);
	void WizardCompletedPageEnter(CWizardPage *WizardPage);
	void ReplaceCartridgePageEnter(CWizardPage *WizardPage);
	bool NoticePagePageEnter(CWizardPage *WizardPage);
	void CheckWastePageEnter(CWizardPage *WizardPage);

	void PageEnter(CWizardPage *Page);
	void PageLeave(CWizardPage *Page, TWizardPageLeaveReason LeaveReason);
	void HelpEvent(CWizardPage *Page);

	void ReplaceCartridgePageLeave(CWizardPage *WizardPage);
	void LongWizardInProgressPageLeave(CWizardPage *WizardPage);
	void SelectWizardModePageLeave(CWizardPage *WizardPage);
	bool InsertCleanserPageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason);
	void RemoveAllCartridgesPageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason);
	void CheckWastePageLeave(CWizardPage *WizardPage);

	void WashingCycle();
	void SetMaterialsToCleanser();
	void SetOperationMode(TThermOpMode om);
	bool AreRelevantHeadsFilled(int PumpID);
	void HeadsHeatingCycle();
	void PipesClearingCycle(CWizardPage *Page, unsigned int PipesClearingTime);
	void SetMachineCleaningStartPage();
	void CleanUp();
	void DisableServiceMaterials();
	void createPumpControllers(TTankIndexVector& RequiredTanks, ResinFlowUtils::PumpingMode mode);
	void createPumpControllers(ResinFlowUtils::TPumpIndicesVector& RequiredPumps, ResinFlowUtils::PumpingMode mode);
	void suspendAllPumpControllers();
	void deletePumpControllers();
	void controlAllPumps();

	void WaitForDelayBetweenStepsToFinish(unsigned Duration);

	void AddTanks();
	void PerformWashingCycle(TTankIndexVector &Tanks);
	void UpdatePipingFullWashingCyclesState();

	TTankIndex	     m_TankToReplace;
	TTankIndexVector m_RequiredTanks;
	TCabinetNames    m_CabinetNames;

	int m_CurrentTank;
	int m_CurrentCleaningStep;
	unsigned m_DelayBetweenStepsStart;

	bool m_FactoryModeEnabled;  // Unuttended mode, if all tanks are inserted at first checkbox page
	bool m_SetOriginalSubtitle; // For shdInsertCleanser page

    /////////////////////////////
    void __fastcall NextPageTimerExec(TObject * Sender);
public:

	CShutdownWizard();
	~CShutdownWizard();
	void GotoNextPage(bool DoGotoNextPage);

};

};

#endif


