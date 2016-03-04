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

enum TThermOpMode
{
	omLow, // For pipes washing - first phase
	omHigh // For block washing - second phase
};

class CShutdownWizard : public CQ2RTAutoWizard
{
private:

	ResinFlowUtils::PumpControllers m_PumpControllers;	
	int  m_WizardModeOption; // selected wizard mode (Short or Long)	

    // Start the wizard session event
    void StartEvent();

    // End the wizard session event
    void EndEvent();

    // Cancel the wizard session event
    void CancelEvent(CWizardPage *WizardPages);
    void CloseEvent();

    void PromoteCurrentTank();
    TTankIndex ModelCurrentTank();
    TTankIndex SupportCurrentTank();
    bool ValidateModelCurrentTank();
    bool ValidateSupportCurrentTank();
    bool ValidateCurrentTank();
    bool ValidateNextLoopCurrentTank();

    bool IsCurrentTankReset();
    void ResetCurrentTank();

    void ShortWizardInProgressPageEnter(CWizardPage *WizardPage);
    void InsertCleanserPageEnter(CWizardPage *WizardPage);
    bool LongWizardPrimingProgressPageEnter(CWizardPage *WizardPage);
    bool LongWizardInProgressPageEnter(CWizardPage *WizardPage);
    void BlockDrainingPageEnter(CWizardPage *WizardPage);
    void WizardCompletedPageEnter(CWizardPage *WizardPage);
    void ReplaceCartridgePageEnter(CWizardPage *WizardPage);
    bool NoticePagePageEnter(CWizardPage *WizardPage);
    void RemoveCleanserPageEnter(CWizardPage *WizardPage);

    void PageEnter(CWizardPage *Page);
	void PageLeave(CWizardPage *Page, TWizardPageLeaveReason LeaveReason);
	void HelpEvent(CWizardPage *Page);

    void ReplaceCartridgePageLeave(CWizardPage *WizardPage);
    void LongWizardInProgressPageLeave(CWizardPage *WizardPage);
	void RemoveCartridgesPageLeave(CWizardPage *WizardPage);
    void SelectWizardModePageLeave(CWizardPage *WizardPage);
    bool InsertCleanserPageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason);

	void WashingCycle();
	void SetMaterialsToCleanser();
	void SetOperationMode(TThermOpMode om);
	bool AreRelevantHeadsFilled(int PumpID);
	void HeadsHeatingCycle();
	void PipesClearingCycle(CWizardPage *Page, unsigned int PipesClearingTime);
	void BlockWashingCycles(CWizardPage *Page);
	void setMachineCleaningStartPage();
	void CleanUp();
	void DisableServiceMaterials();
	void createPumpControllers(TTankIndexVector& RequiredTanks, ResinFlowUtils::PumpingMode mode);
	void createPumpControllers(ResinFlowUtils::TPumpIndicesVector& RequiredPumps, ResinFlowUtils::PumpingMode mode);
    void suspendAllPumpControllers();
	void deletePumpControllers();
	void controlAllPumps();
	
	void AddModelTanks();
	void PerformWashingCycle(TTankIndexVector &Tanks);
	// ----------------------------------------------------------------------------
	// Washing cycle parameters in case of cartridge replacement
	// ----------------------------------------------------------------------------


	TTankIndex	     m_TankToReplace;
	TTankIndexVector m_RequiredTanks;
	

	// ----------------------------------------------------------------------------
	// Block washing  parameters in case of cartridge replacement
	// ----------------------------------------------------------------------------

	int  m_CurrentBlockWashingCycle;
	bool m_IsWashingBlock; // indicates that the wizard is in block washing phase

    int m_CurrentTank;

public:

	CShutdownWizard();
	~CShutdownWizard();
    void GotoNextPage(bool DoGotoNextPage);

};

#endif



