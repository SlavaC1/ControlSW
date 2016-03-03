/*********************************************************************
*                               Stratasys LTD.                       *
*                        -----------------------------               *
* Project:  Q2RT                                                     *
* Module:   Machine shutdown.                                        *
* Module Description:   Machine shutdown wizard for the special case *
*                       of production                                *
*                                                                    *
* Compilation:  Standard C++.                                        *
*                                                                    *
* Start date: 2/10/2014                                              *
*                                                                    *
**********************************************************************/


#ifndef _FACTORY_SHUTDOWN_WIZARD_H_
#define _FACTORY_SHUTDOWN_WIZARD_H_

#include "Q2RTWizard.h"
#include "WizardPages.h"
#include "BackEndInterface.h"
#include "QMutex.h"
#include "ProgressReporting.h"

#define USE_FACTORY_SHUTDOWN_WIZARD 0

#if (0 != USE_FACTORY_SHUTDOWN_WIZARD)

class CFactoryShutdownWizard : public CQ2RTAutoWizard
{
private:

    typedef enum wizard_pages_index_
    {
        fswWelcome,
        fswStartCheck,
        fswProtectMove,
        fswMoveToPurge,
        fswSetupHW,
        fswCartridgeIssue,
        fswProtectPurge,
        fswPurgeTubes,
        fswRestoreTubes,
        fswProtectCycles,
        fswMoveToCycles,
        fswVacuumProblem,
        fswPurgeBlock,
        fswRemoveCartriges,
        fswCartrigesRemain,
        fswPurgeAndWipe,
        fswDone,
/*--------- add only above this line ---------*/
        fswPageCount
    } TWizardPagesIndex;

    // Start the wizard session event
    void StartEvent();

    // End the wizard session event
    void EndEvent();

    // Cancel the wizard session event
    void CancelEvent(CWizardPage *WizardPages);
    void CloseEvent();

    void PageEnter(CWizardPage *Page);
    void PageLeave(CWizardPage *Page, TWizardPageLeaveReason LeaveReason) {}
    void HelpEvent(CWizardPage *Page) {}

    // common finishing activities
    void CleanUp();

    // utilities
    CWizardPage * CreateMesageWizardPage(const QString& TitleStr, const QString& SubTitleStr, int PicId);
    void          SetOperationModeHigh(bool isHigh);
    void          SetMaterialsToCleanser();
    void          WashingCycle(const char * purposeStr);
    void          BlockWashingCycles(CWizardPage *WizardPage);
    void          createPumpControllers(TTankIndexVector& RequiredTanks, ResinFlowUtils::PumpingMode mode);
    void          UpdatePipingFullWashingCyclesState();
    void          DisableServiceMaterials();

    ResinFlowUtils::PumpControllers m_PumpControllers;
    T_AxesTable                     m_AllowAxes;
    TTankIndexVector                m_TanksAtLeft;
    TTankIndexVector                m_TanksAtRight;
    TTankIndexVector                m_AllTanks;
    bool                            m_isFinishedOk;

public:

    CFactoryShutdownWizard();
    virtual ~CFactoryShutdownWizard() {}
};

#endif /* USE_FACTORY_SHUTDOWN_WIZARD */

#endif /*_FACTORY_SHUTDOWN_WIZARD_H_*/
