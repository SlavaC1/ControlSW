//#include "FEResources.h"
#include "GlobalDefs.h"
#include "Q2RTApplication.h"
#include "FrontEnd.h"
#include "WizardImages.h"
#include "ResinFlowUtils.h"

#if (0 != USE_FACTORY_SHUTDOWN_WIZARD)
#include "FactoryShutdownWizard.h"


#define PAGE_TITLE_NOTE     "Note:"
#define PAGE_TITLE_CHECKING "Checking..."


CFactoryShutdownWizard::CFactoryShutdownWizard(void)
: CQ2RTAutoWizard(IDS_FSW_WIZARD, true, SHD_PROGRESS_PICTURE_ID),
  m_PumpControllers(this), m_isFinishedOk(false)
{
    // gather data about the system
    for (int i = 0; i < MAX_AXIS; ++i)
    {
        m_AllowAxes[i] = ((AXIS_T == i) || (Q2RTApplication->GetMachineType() != (mtObjet1000)));
    }

    for (int i=FIRST_TANK_TYPE; i < TOTAL_NUMBER_OF_CONTAINERS; ++i)
    {
        m_AllTanks.push_back((TTankIndex)i);
        QString * DescPtr = &(SegmentDescription[GetTankAdjacentSegment((TTankIndex)i)]);
        if (0 == DescPtr->size())
        {
            continue;
        }
        if ((*DescPtr)[DescPtr->size() - 1] == 'L')
        {
            m_TanksAtLeft.push_back((TTankIndex)i);
        }
        else
        {
            m_TanksAtRight.push_back((TTankIndex)i);
        }
    }
/*---------------------------------------------------------------------*/

    CCheckBoxWizardPage         * pCheckPage;
    CSuspensionPointsStatusPage * pStatPage;
    TPagePointers                 pagesVector(fswPageCount, 0);

    pagesVector[fswWelcome]                         = CreateMesageWizardPage(GetTitle(), LOAD_STRING(IDS_FSW_WELCOME_SUBTITLE), IN_PROCESS_IMAGE_ID);

    pagesVector[fswStartCheck]      = pCheckPage    = new CCheckBoxWizardPage(LOAD_STRING(IDS_SYSTEM_CHECKLIST), PREPARATIONS_IMAGE_ID, wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow | wpHelpNotVisible);
    pCheckPage->SubTitle = LOAD_STRING(IDS_SYSTEM_CHECKLIST_CONFIRM);
    pCheckPage->Strings.Add("The build tray is empty");
    pCheckPage->Strings.Add("The cover is closed");

    pagesVector[fswProtectMove]                     = new CCloseDoorPage(this,PREPARATIONS_IMAGE_ID, wpPreviousDisabled | wpHelpNotVisible);

    pagesVector[fswMoveToPurge]     = pStatPage     = new CSuspensionPointsStatusPage("", IN_PROCESS_IMAGE_ID, wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible);
    pStatPage->PointsStatusMessage = LOAD_STRING(IDS_FSW_MOVING);//"Moving Axes";

    pagesVector[fswSetupHW]         = pCheckPage    = new CCheckBoxWizardPage(LOAD_STRING(IDS_SYSTEM_CHECKLIST), PREPARATIONS_IMAGE_ID, wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow | wpHelpNotVisible);
    pCheckPage->SubTitle = LOAD_STRING(IDS_FSW_WELCOME_SUBTITLE);
    pCheckPage->Strings.Add("Place the waste bucket on the build tray");
    pCheckPage->Strings.Add("Disconnect the material tubes from the block");
    pCheckPage->Strings.Add("Connect the tubes to the external manifold");
    pCheckPage->Strings.Add("Insert 16 cartridges with cleanser fluid");
    pCheckPage->Strings.Add("Close the printer cover");

    pagesVector[fswCartridgeIssue]                  = CreateMesageWizardPage(PAGE_TITLE_NOTE, LOAD_STRING(IDS_FSW_INSUFFICIENT_CLEANSER), SHD_INFORMATION_PICTURE_ID);

    pagesVector[fswProtectPurge]                    = new CCloseDoorPage(this,PREPARATIONS_IMAGE_ID, wpPreviousDisabled | wpHelpNotVisible);

    pagesVector[fswPurgeTubes]                      = new CElapsingTimeWizardPage(LOAD_STRING(IDS_FSW_PURGE_TUBES), IN_PROCESS_IMAGE_ID, wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible);

    pagesVector[fswRestoreTubes]    = pCheckPage    = new CCheckBoxWizardPage(LOAD_STRING(IDS_SYSTEM_CHECKLIST), PREPARATIONS_IMAGE_ID, wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow | wpHelpNotVisible);
    pCheckPage->SubTitle = LOAD_STRING(IDS_SYSTEM_CHECKLIST_CONFIRM);
    pCheckPage->Strings.Add("Disconnect the external manifold");
    pCheckPage->Strings.Add("Connect all material tubes to the block");
    pCheckPage->Strings.Add("Remove the waste bucket from the build tray");
    pCheckPage->Strings.Add("Close the printer cover");

    pagesVector[fswProtectCycles]                   = new CCloseDoorPage(this, PREPARATIONS_IMAGE_ID, wpPreviousDisabled | wpHelpNotVisible);

    pagesVector[fswMoveToCycles]    = pStatPage     = new CSuspensionPointsStatusPage("", IN_PROCESS_IMAGE_ID, wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible);
    pStatPage->PointsStatusMessage = LOAD_STRING(IDS_FSW_MOVING);// "Moving Axes";

    pagesVector[fswVacuumProblem]                   = CreateMesageWizardPage(PAGE_TITLE_NOTE, LOAD_STRING(IDS_FSW_VACUUM_PROBLEM), SHD_INFORMATION_PICTURE_ID);

    pagesVector[fswPurgeBlock]                      = new CElapsingTimeWizardPage(LOAD_STRING(IDS_FSW_PURGE_BLOCK), IN_PROCESS_IMAGE_ID, wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible);

    pagesVector[fswRemoveCartriges] = pCheckPage    = new CCheckBoxWizardPage(LOAD_STRING(IDS_SYSTEM_CHECKLIST), PREPARATIONS_IMAGE_ID, wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow | wpHelpNotVisible);
    pCheckPage->SubTitle = LOAD_STRING(IDS_SYSTEM_CHECKLIST_CONFIRM);
    pCheckPage->Strings.Add("Remove the 16 cartridges with cleanser fluid");

    pagesVector[fswCartrigesRemain]                 = CreateMesageWizardPage(PAGE_TITLE_NOTE, LOAD_STRING(IDS_FSW_CARTRIDGES_REMAIN), IN_PROCESS_IMAGE_ID);

    pagesVector[fswPurgeAndWipe]                    = new CElapsingTimeWizardPage(LOAD_STRING(IDS_FSW_PURGE_BLOCK), IN_PROCESS_IMAGE_ID, wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible);

    pagesVector[fswDone]                            = CreateMesageWizardPage(GetTitle(), WIZARD_COMPLETED_STR, IN_PROCESS_IMAGE_ID);

    for(TPagePointersIterator p = pagesVector.begin(); p != pagesVector.end(); p++)
    {
        AddPage(*p);
    }
}


CWizardPage * CFactoryShutdownWizard::CreateMesageWizardPage(const QString& TitleStr, const QString& SubTitleStr, int PicId)
{
    CMessageWizardPage * pPage = new CMessageWizardPage(TitleStr, PicId, wpPreviousDisabled | wpHelpNotVisible);
    if (NULL != pPage)
    {
        pPage->SubTitle = SubTitleStr;
    }
    return pPage;
}


// upon start wizard session
void CFactoryShutdownWizard::StartEvent()
{
    m_isFinishedOk = false;

    // Stopping material mismatch messages
    for (int i = LAST_TANK_TYPE-1; i >= FIRST_TANK_TYPE; --i)
    {
        FrontEndInterface->HideTankIDNoticeDlg(i);
        m_BackEnd->SkipMachineResinTypeChecking(static_cast<TTankIndex>(i), true);
    }

    // Override leftover settings from previous actions
    m_BackEnd->GotoDefaultMode();

    // Ignore vacuum errors
    m_BackEnd->EnableDisableVacuumErrorHandling(false);

    // prepare block
    m_BackEnd->ActivateWaste(true);
    m_BackEnd->TurnHeadFilling(false);
    StartAsyncHeatingCycle(TOTAL_NUMBER_OF_HEATERS, true);
}


// upon end wizard session
void CFactoryShutdownWizard::EndEvent()
{
    CleanUp();
}


// upon wizard session cancelation
void CFactoryShutdownWizard::CancelEvent(CWizardPage *WizardPage)
{
    WizardPage=WizardPage;
    m_BackEnd->SetActuatorState(ACTUATOR_ID_AIR_VALVE, ACTUATOR_OFF); //this is a necessary ugly patch till all wizards event will be synchronized
    m_PumpControllers.deletePumpControllers();
    m_BackEnd->TurnHeadHeaters(false); // todo 1 -cReview -oArcady.Volman@objet.com: ask why
    CleanUp();
}


void CFactoryShutdownWizard::CleanUp()
{
    WriteToLogFile(LOG_TAG_GENERAL,"FactoryShutdownWizard: CleanUp");
    SetOperationModeHigh(false);
    m_BackEnd->UpdateChambers(false);
    m_PumpControllers.deletePumpControllers();

    // Resetting the modes
    m_BackEnd->GotoDefaultMode();
    m_BackEnd->EnableStandByMode();

    // Enable vacuum errors
    m_BackEnd->ActivateWaste(false);
    if (Q2RTApplication->GetMachineType() != (mtObjet1000))
    {
        m_BackEnd->BEI_WaitUntilTrayInserted();
    }
    m_BackEnd->EnableMotor(false, AXIS_ALL);
    m_BackEnd->EnableDisableVacuumErrorHandling(true);
    m_BackEnd->LockDoor(false);
}


// upon closing the wizard session
void CFactoryShutdownWizard::CloseEvent()
{
    DisableServiceMaterials();
    if (m_isFinishedOk)
    {
        m_BackEnd->TurnPower(false); // remark 2 lines for debugging
        Q2RTApplication->TerminateAndShutdown();
    }
}


// pages with special treatment
void CFactoryShutdownWizard::PageEnter(CWizardPage *WizardPage)
{
    TQErrCode         Err       = Q_NO_ERROR;
    TWizardPagesIndex PageIndex = (TWizardPagesIndex)WizardPage->GetPageNumber();

    try
    {
        switch(PageIndex)
        {
        case fswMoveToPurge:
        {
            EnableAllAxesAndHome(NULL/*all*/, true, true);
            TRY_FUNCTION(Err, m_BackEnd->MoveMotorToAbsolutePosition(AXIS_Z, m_ParamMgr->FSW_ZPosForFixturePurge, BLOCKING, muSteps));
            TRY_FUNCTION(Err, m_BackEnd->GotoPurgePositionEX(true, m_AllowAxes));
            /* move Z to lowest ~95,000 but use parameter */
            m_BackEnd->ActivateWaste(true);
            m_BackEnd->TurnHeadHeaters(true);
            HeadsHeatingCycle();

            ResinFlowUtils::AirValve av(this); // possible solution for no-pumping bug
            av.holdOpen(1000 * m_ParamMgr->FSW_BlockWashDrainingTime);

            m_BackEnd->LockDoor(false); //Release interlock
            GotoNextPage();
        }
        break;

        case fswCartridgeIssue:
        {
            WizardPage->Title = PAGE_TITLE_CHECKING;
            WizardPage->Refresh();
            YieldAndSleepWizardThread();

            int     i;
            QString CleanserStr(LOAD_STRING(IDS_CLEANSER_STR));

            for (i = 0; i < TOTAL_NUMBER_OF_CONTAINERS; ++i)
            {
                if ((string::npos == m_BackEnd->GetTankMaterialType((TTankIndex)i).find(CleanserStr, 0))       ||
                    (m_BackEnd->GetTankWeightInGram(i) < m_ParamMgr->FSW_MinCleanserWeight)                    ||
                    m_BackEnd->IsResinExpired((TTankIndex)i) || m_BackEnd->IsMaxPumpTimeExceeded((TTankIndex)i)  )
                {
                    break;
                }
            }
            if (i < TOTAL_NUMBER_OF_CONTAINERS)  /* something's missing */
            {
                WizardPage->Title = PAGE_TITLE_NOTE;
                WizardPage->Refresh();
                SetNextPage(PageIndex);
                break;
            }
            // if all went well...
            GotoNextPage();
        }
        break;

        case fswPurgeTubes:
            // Heating in 'High Quality' Mode
            m_BackEnd->GotoDefaultMode();
            m_BackEnd->EnterMode(PRINT_MODE[HIGH_TEMPATURE_INDEX][FIRST_OPERATION_MODE],
                         MACHINE_QUALITY_MODES_DIR(HIGH_TEMPATURE_INDEX,FIRST_OPERATION_MODE));
            HeadsHeatingCycle();
            if (IsCancelled())
                throw CQ2RTAutoWizardCancelledException();
            m_CancelReason.clear();
            // washing
            SetOperationModeHigh(false);
            SetMaterialsToCleanser();
            m_BackEnd->UpdateChambers();
            m_PumpControllers.deletePumpControllers();
            createPumpControllers(m_TanksAtLeft, ResinFlowUtils::PipesPriming);
            WashingCycle("rinsing");
            if (IsCancelled())
                throw CQ2RTAutoWizardCancelledException();
            m_PumpControllers.deletePumpControllers();
            if (m_CancelReason != "")
                throw CQ2RTAutoWizardException(m_CancelReason);
            createPumpControllers(m_TanksAtRight, ResinFlowUtils::PipesWashing);
            WashingCycle("rinsing");
            if (IsCancelled())
                throw CQ2RTAutoWizardCancelledException();
            m_PumpControllers.deletePumpControllers();
            if (m_CancelReason != "")
                throw CQ2RTAutoWizardException(m_CancelReason);
            UpdatePipingFullWashingCyclesState();
            GotoNextPage();
        break;

        case fswMoveToCycles:
            EnableAllAxesAndHome(m_AllowAxes, true, true);
            TRY_FUNCTION(Err, m_BackEnd->GotoPurgePositionEX(true, m_AllowAxes));
            GotoNextPage();
        break;

        case fswVacuumProblem:
        {
            WizardPage->Title = PAGE_TITLE_CHECKING;
            WizardPage->Refresh();
            YieldAndSleepWizardThread();

            int pressure = m_BackEnd->GetAverageVacuumValue_A2D();

            if ((pressure > m_ParamMgr->VacuumHighLimit) ||
                (pressure < m_ParamMgr->VacuumLowLimit)     )
            {
                WizardPage->Title = PAGE_TITLE_NOTE;
                WizardPage->Refresh();
                SetNextPage(PageIndex); /* bring us back here */
                break;
            }

            GotoNextPage();//GotoPage(fswPurgeBlock);
        }
        break;

        case fswPurgeBlock:
            HeadsHeatingCycle();
            if (IsCancelled())
                throw CQ2RTAutoWizardCancelledException();
            BlockWashingCycles(WizardPage);
            GotoNextPage();
        break;

        case fswCartrigesRemain:
        {
            WizardPage->Title = PAGE_TITLE_CHECKING;
            WizardPage->Refresh();
            YieldAndSleepWizardThread();

            int     i;
            for (i = 0; i < TOTAL_NUMBER_OF_CONTAINERS; ++i)
            {
                if (0 != m_BackEnd->GetTankMaterialType((TTankIndex)i).compare(""))
                {
                    break;
                }
            }
            if (i < TOTAL_NUMBER_OF_CONTAINERS)  /* something's missing */
            {
                WizardPage->Title = PAGE_TITLE_NOTE;
                WizardPage->Refresh();
                SetNextPage(PageIndex); /* bring us back here */
                break;
            }
            // if all went well...
            GotoNextPage();
        }
        break;

        case fswPurgeAndWipe:
            // Making sure that the block is hot.
            HeadsHeatingCycle();
            if (IsCancelled())
                throw CQ2RTAutoWizardCancelledException();
            m_CancelReason = "";

            m_PumpControllers.deletePumpControllers();
            createPumpControllers(m_AllTanks, ResinFlowUtils::PipesEmptying);
            WashingCycle("draining"); // translate exception message to relevant user readable message
            if (IsCancelled())
                throw CQ2RTAutoWizardCancelledException();
            // After successfull WashingCycle - delete WashingCycle PumpControllers
            m_PumpControllers.deletePumpControllers();
            if (m_CancelReason != "")
                throw CQ2RTAutoWizardException(m_CancelReason);

//            HeadsDrainingCycle(WizardPage, m_ParamMgr->SHD_BlockDrainingTimeInSec, false, true, (Q2RTApplication->GetMachineType()) != (mtObjet1000));
//            if (IsCancelled())
//                throw CQ2RTAutoWizardCancelledException();

            m_BackEnd->WipeHeads(false);
            m_BackEnd->WipeHeads(false);
            GotoPage(fswDone);
        break;

        case fswDone:
            m_isFinishedOk = true;
        break;
        }
    }
    catch(ETimeout &e)
    {
        m_PumpControllers.suspendAllPumpControllers();
        throw EQException(QFormatStr(LOAD_STRING(IDS_TIMEOUT_REASON), e.GetErrorMsg().c_str()));
    }
    catch(ResinFlowUtils::EResinFlowException& ResinFlowE)
    {
        CElapsingTimeWizardPage* pPage = dynamic_cast<CElapsingTimeWizardPage*>(GetCurrentPage());
        pPage->Pause();
        m_PumpControllers.suspendAllPumpControllers();
//        m_TankToReplace = ResinFlowE._tankIndex;
//        GotoPage(shdReplaceCartridge);
        return;
    }
    catch(...)
    {
        m_PumpControllers.suspendAllPumpControllers();
        throw;
    }
} //PageEnter


//void CFactoryShutdownWizard::PageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason)
//{
//    LeaveReason=LeaveReason;
//    TWizardPagesIndex PageIndex = (TWizardPagesIndex)WizardPage->GetPageNumber();
///*    if (LeaveReason == lrCanceled)
//    {
//        switch(PageIndex)
//        {
//        case ShutdownWizard::shdInsertCleanser:
//            InsertCleanserPageLeave(WizardPage, LeaveReason);
//            break;
//        }
//    }
//
//    if (LeaveReason != lrGoNext)
//        return;*/
//
//    switch(PageIndex)
//    {
//    case fswCartridgeIssue: // "looped" pages
//    case fswVacuumProblem:
//    case fswCartrigesRemain:
//        SetNextPage(PageIndex);
//    //break;
//    }
//} // PageLeave


void CFactoryShutdownWizard::SetOperationModeHigh(bool isHigh)
{
    TThermistorsOperationMode Thermistors = LOW_THERMISTORS_MODE;
    TTankOperationMode        Tanks
#ifdef OBJET_MACHINE_KESHET
                                          = S_M1_M7_ACTIVE_TANKS_MODE; //OBJET_MACHINE_KESHET
#else
                                          = S_M1_M2_M3_ACTIVE_TANKS_MODE;
#endif

    if (isHigh)
    {
        Thermistors = GetThermOpModeForHQHS();
#ifdef OBJET_MACHINE_KESHET
        Tanks       = S_M1_M3_M5_ACTIVE_TANKS_MODE;//itamar, check!!!!!
#else
        Tanks       = S_M1_ACTIVE_TANKS_MODE;
#endif
    }

    m_ParamMgr->ThermistorsOperationMode = Thermistors;
    m_ParamMgr->SaveSingleParameter(&m_ParamMgr->ThermistorsOperationMode);

    m_ParamMgr->TanksOperationMode = Tanks;
    m_ParamMgr->SaveSingleParameter(&m_ParamMgr->TanksOperationMode);

    m_ParamMgr->PrintingOperationMode = DIGITAL_MATERIAL_OPERATION_MODE;
    m_ParamMgr->PipesOperationMode    = DIGITAL_MATERIAL_OPERATION_MODE;
    m_ParamMgr->SaveSingleParameter(&m_ParamMgr->PrintingOperationMode);
    m_ParamMgr->SaveSingleParameter(&m_ParamMgr->PipesOperationMode);

    m_BackEnd->SetDefaultParamsToOCB();
}


void CFactoryShutdownWizard::SetMaterialsToCleanser()
{
    m_BackEnd->GotoDefaultMode();

    char * pMaterialType;
    for(int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; ++i)
    {
        pMaterialType = GetTankCleanserLabel((TTankIndex)i);

        m_ParamMgr->TypesArrayPerTank[i] = pMaterialType;
        m_ParamMgr->TypesArrayPerPipe[i] = pMaterialType;

        for(int Segment = FIRST_SEGMENT; Segment < NUMBER_OF_SEGMENTS; Segment++)
            if(IsSegmentRelatedToTank(static_cast<TSegmentIndex>(Segment), static_cast<TTankIndex>(i)))
                m_ParamMgr->MRW_TypesArrayPerSegment[Segment] = pMaterialType;

        m_BackEnd->UpdateTanks(static_cast<TTankIndex>(i));
    }

    for (int i = FIRST_MODEL_CHAMBER_INDEX; i < LAST_MODEL_CHAMBER_INDEX; ++i)
        m_ParamMgr->TypesArrayPerChamber[i] = MODEL_CLEANSER;

    for (int i = FIRST_SUPPORT_CHAMBER_INDEX; i < LAST_SUPPORT_CHAMBER_INDEX; ++i)
        m_ParamMgr->TypesArrayPerChamber[i] = SUPPORT_CLEANSER;

    m_ParamMgr->SaveSingleParameter(&m_ParamMgr->TypesArrayPerTank);
    m_ParamMgr->SaveSingleParameter(&m_ParamMgr->TypesArrayPerPipe);
    m_ParamMgr->SaveSingleParameter(&m_ParamMgr->TypesArrayPerChamber);
    m_ParamMgr->SaveSingleParameter(&m_ParamMgr->MRW_TypesArrayPerSegment);

    m_BackEnd->GotoDefaultMode();

    for(int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; ++i)
    {
        while (false == m_BackEnd->WaitForIdentificationCompletion(static_cast<TTankIndex>(i),true/* = DontWaitIfTankIsOut*/, 100 /*time to wait*/))
        {
            YieldWizardThread();
            if(IsCancelled())
                throw CQ2RTAutoWizardCancelledException();
        }
    }
}


void CFactoryShutdownWizard::WashingCycle(const char * purposeStr)
{
    TQErrCode Err = Q_NO_ERROR;
    m_PumpControllers.clearControllerOperationDoneFlag();
    TRY_FUNCTION(Err, m_BackEnd->GotoPurgePositionEX(true, m_AllowAxes));
    ResinFlowUtils::AirValve av(this);
    av.open();

    m_PumpControllers.clearControllerOperationDoneFlag();
    while (!m_PumpControllers.isControllerOperationDone() && !IsCancelled())
    {
        try
        {
            m_PumpControllers.controlAllPumps();
        }
        catch(const ETimeout &e) // translate exception message to user readable message
        {
            throw ETimeout(QFormatStr("waiting for the block to become not full during pipes %s", purposeStr));
        }

        YieldAndSleepWizardThread();
    }

    av.close();
//    if (m_PumpControllers.isControllerOperationDone())
//    {
//        // _requiredNetPumpingMilliseconds = 0;  // todo: shahar: check is needed.
//    }
}


void CFactoryShutdownWizard::BlockWashingCycles(CWizardPage *WizardPage)
{
    WizardPage=WizardPage;
    WriteToLogFile(LOG_TAG_GENERAL,"Starting block washing cycles.");

    TQErrCode Err = Q_NO_ERROR;
    TRY_FUNCTION(Err, m_BackEnd->GotoPurgePositionEX(true, m_AllowAxes));

    for (int i = 0; i < m_ParamMgr->FSW_BlockWashCycleCount; ++i)
    {
        if (IsCancelled())
            throw CQ2RTAutoWizardCancelledException();

        WriteToLogFile(LOG_TAG_GENERAL, QFormatStr("Block washing cycle #%d.", i+1));

        if (m_PumpControllers.isPumpsVectorEmpty())
            createPumpControllers(m_AllTanks, ResinFlowUtils::BlockFilling); // todo -oArcady.Volman@objet.com -cNone: consider to add the substring time to the time related function names
        // use pump controllers to fill block
        m_PumpControllers.clearControllerOperationDoneFlag();
        while (!m_PumpControllers.isControllerOperationDone() && !IsCancelled())
        {
            try
            {
                m_PumpControllers.controlAllPumps();
            }
            catch (const ETimeout &e) // translate exception message to user readable message
            {
                throw ETimeout("filling the block during block washing cycle");
            }

            YieldAndSleepWizardThread();
        }
        if (IsCancelled())
            throw CQ2RTAutoWizardCancelledException();
        m_PumpControllers.deletePumpControllers();

        // drainblock
        ResinFlowUtils::AirValve av(this);
        av.holdOpen(1000 * m_ParamMgr->FSW_BlockWashDrainingTime);
    }

    WriteToLogFile(LOG_TAG_GENERAL,"Block washing cycles completed.");
}


void CFactoryShutdownWizard::createPumpControllers(TTankIndexVector& RequiredTanks, ResinFlowUtils::PumpingMode mode)
{
    using namespace ResinFlowUtils;

    TPumpIndicesVector v = pumpIndicesVectorByTankIndicesVector(RequiredTanks);

    TQMilliseconds         dutyOn_ms                      = m_ParamMgr->SHD_PumpsTimeDutyOn_ms;
    TQMilliseconds         dutyOff_ms                     = m_ParamMgr->SHD_PumpsTimeDutyOff_ms;
    TQMilliseconds         requiredNetPumpingMilliseconds = 0;
    TQMilliseconds         timeout                        = m_ParamMgr->SHD_BlockDrainTimeoutInSec;
    VerificationAttributes Attributes                     = static_cast<VerificationAttributes>(vaTankValid /*| vaTankWeight*/ | vaTankResinType);
    for (TPumpIndicesVector::iterator itrPumpIndx = v.begin(); itrPumpIndx != v.end(); ++itrPumpIndx)
    {
        // todo -oArcady.Volman@objet.com -cNone: make sure to reset the controllers when required
        switch (mode)
        {
        case PipesPriming:
            WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("PipesPriming - %s", TankToStr(PumpToTank(*itrPumpIndx)).c_str()));
            requiredNetPumpingMilliseconds = static_cast<TQMilliseconds>(m_ParamMgr->FSW_LeftPumpsPurgeDuration * 1000);
            dutyOn_ms  = m_ParamMgr->SHD_PumpsTimeDutyOn_ms + m_ParamMgr->SHD_PumpsTimeDutyOff_ms;
            dutyOff_ms = 0;
        break;

        case PipesWashing:
            WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("PipesWashing - %s", TankToStr(PumpToTank(*itrPumpIndx)).c_str()));
            requiredNetPumpingMilliseconds = static_cast<TQMilliseconds>(m_ParamMgr->FSW_RightPumpsPurgeDuration * 1000);
            dutyOn_ms  = m_ParamMgr->SHD_PumpsTimeDutyOn_ms + m_ParamMgr->SHD_PumpsTimeDutyOff_ms;
            dutyOff_ms = 0;
        break;

        case BlockFilling:
            WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("BlockFilling - %s", TankToStr(PumpToTank(*itrPumpIndx)).c_str()));
            requiredNetPumpingMilliseconds = 0;  // Ignored for BlockFillingPumpController.
            timeout = m_ParamMgr->SHD_FillingTimeoutInSec;
        break;

        case PipesEmptying: // todo -oArcady.Volman@objet.com -cNone: rename to pipes draining?
            WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("PipesEmptying - %s", TankToStr(PumpToTank(*itrPumpIndx)).c_str()));
            requiredNetPumpingMilliseconds = static_cast<TQMilliseconds>(m_ParamMgr->FSW_AirValveTimeForTubeDrain * 1000);
            Attributes = static_cast<VerificationAttributes>(0);
        break;

        default:
            assert(false);
            throw EQException("bad PumpingMode");
        }

        m_PumpControllers.AddSpecializedPumpController( mode,
                *itrPumpIndx,
                requiredNetPumpingMilliseconds,
                timeout * 1000,
                dutyOn_ms,
                dutyOff_ms,
                Attributes,
                ( (IS_MODEL_TANK(tankIndexByPumpIndex(*itrPumpIndx))) ? MODEL_CLEANSER : SUPPORT_CLEANSER));
    }
}


void CFactoryShutdownWizard::UpdatePipingFullWashingCyclesState()
{
    //mark all the segments as being flushed in 'Full' mode
    //(in Material-Replacement-Wizard's terms)
    for (int seg = NUMBER_OF_SEGMENTS-1; seg >= FIRST_SEGMENT; --seg)
        m_ParamMgr->MRW_IsSegmentEconomy[seg] = 0;

    m_ParamMgr->SaveSingleParameter(&m_ParamMgr->MRW_IsSegmentEconomy);
}


void CFactoryShutdownWizard::DisableServiceMaterials()
{
    // Refresh the front-end display
    FrontEndInterface->UpdateStatus(FE_REFRESH_MODES_DISPLAY, 0, true);
    for(int /* TTankIndex */ i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; ++i)
    {
        m_BackEnd->SkipMachineResinTypeChecking(i, false);
        m_BackEnd->AllowServiceMaterials(i, false);
        m_BackEnd->UpdateTanks(i);
    }
}
#endif /* USE_FACTORY_SHUTDOWN_WIZARD */

