/*****************************************************************
*                        Objet Geometries LTD.        	         *
*                        -----------------------------           *
* Project: 	Q2RT                                                 *
* Module: 	Machine shutdown.                                    *
* Module Description: 	Machine shutdown wizard  		         *
*                                                                *
* Compilation:	Standard C++.                                    *
*                                                                *
* Start date: 16/11/2008					                     *
* Updated:    11/2014		                                 *
*****************************************************************/

#ifndef _DEBUG
#define NDEBUG
#endif
#include <assert.h>

#include "ResinFlowUtils.h"

#include "ShutdownWizard.h"
#include "AppParams.h"
#include "QTimer.h"
#include "ModesDefs.h"
#include "GlobalDefs.h"
#include "WizardImages.h"
#include "QMonitor.h"
#include "AppLogFile.h"
#include "Q2RTApplication.h"
#include <forms.hpp>		// Requiered for Application instance
#include "FrontEnd.h"
#include <algorithm>
#include "FEResources.h"

using namespace ShutdownWizard;

const int InsertTanksImageID[] =
{
	SHD_INSERT_TANK_STEP_1_PICTURE_ID,
	SHD_INSERT_TANK_STEP_2_PICTURE_ID,
	SHD_INSERT_TANK_STEP_3_PICTURE_ID,
	SHD_INSERT_TANK_STEP_4_PICTURE_ID
};

typedef enum Pages
{
    // IMPORTANT NOTES:
    // 	- The order of the elements in this enum determines the order of the pages (see CShutdownWizard::CShutdownWizard).
    // 	- The values of the elements of this enum must not be manully alterd (otherwise u'll break shdPagesCount)

    shdWelcomePage,
    shdSelectWizardMode,
    shdIsTrayInserted,
    shdCheckTrayClean,
    shdCheckWaste,
    shdCloseDoor,
    shdHoming,
    // beginning of Short Shutdown wizard
    shdShortWizardInProgress,
    // beginning of Long Shutdown wizard
    shdInsertCleanser,
    shdMaterialNoticePage,
    shdFactoryModeNoticePage,
    shdLongWizardInProgress,
    shdRemoveAllCartridges,
    shdBlockDraining,
    // service pages
    shdReplaceCartridge,
    // ending for both modes
    shdWizardCompleted,

    shdPagesCount // this must element must be the last in this enum
} TWizardPagesIndex;

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
CShutdownWizard::CShutdownWizard(void) : CQ2RTAutoWizard(IDS_SHD_WIZARD,  true, SHD_PROGRESS_PICTURE_ID), m_PumpControllers(this), m_pNextPageTimer(NULL)
{
	m_WizardModeOption       = SHORT_MODE_OPTION;
	m_TankToReplace          = NO_TANK;
	m_CurrentTank            = 0;
	m_CurrentCleaningStep    = 0;
	m_DelayBetweenStepsStart = 0;
	m_FactoryModeEnabled     = false;
	m_SetOriginalSubtitle    = true;

	using namespace ShutdownWizard;

    m_pNextPageTimer = new TTimer(NULL);
    m_pNextPageTimer->Enabled = false;
    m_pNextPageTimer->Interval = 3000;
    m_pNextPageTimer->OnTimer = NextPageTimerExec;

	TPagePointers pagesVector(shdPagesCount, 0);

	{
		CMessageWizardPage *pPage = new CMessageWizardPage(GetTitle(), IN_PROCESS_IMAGE_ID, wpHelpNotVisible);
		pPage->SubTitle = LOAD_STRING(IDS_SHD_WELCOME_SUBTITLE);
		pagesVector[shdWelcomePage] = pPage;
	}
	{
		CRadioGroupWizardPage *pPage = new CRadioGroupWizardPage(LOAD_STRING(IDS_SYSTEM_ACTION_SELECT), QUESTION_IMAGE_ID, wpNextWhenSelected | wpPreviousDisabled);
		pPage->SubTitle = LOAD_STRING(IDS_SHD_MODE_SELECT_SUBTITLE);
		pPage->Strings.Add(LOAD_STRING(IDS_SHD_MODE_SELECT_STR1));
		pPage->Strings.Add(LOAD_STRING(IDS_SHD_MODE_SELECT_STR2));
		pPage->DefaultOption = ShutdownWizard::SHORT_MODE_OPTION;
		pagesVector[shdSelectWizardMode] = pPage;
	}
	{
		CInsertTrayPage *pPage = new CInsertTrayPage(this, PREPARATIONS_IMAGE_ID, wpPreviousDisabled | wpHelpNotVisible);
		pagesVector[shdIsTrayInserted] = pPage;
	}
	{
		CVerifyCleanTrayPage *pPage = new CVerifyCleanTrayPage(this, PREPARATIONS_IMAGE_ID, wpHelpNotVisible);
		pagesVector[shdCheckTrayClean] = pPage;
	}
	{
		CCheckBoxWizardPage *pPage = new CCheckBoxWizardPage(LOAD_STRING(IDS_SYSTEM_CHECKLIST), PREPARATIONS_IMAGE_ID, wpNextWhenSelected | wpClearOnShow | wpHelpNotVisible);
		pPage->SubTitle = LOAD_STRING(IDS_SYSTEM_CHECKLIST_CONFIRM);
		pagesVector[shdCheckWaste] = pPage;
	}
	{
		CCloseDoorPage *pPage = new CCloseDoorPage(this, PREPARATIONS_IMAGE_ID, wpPreviousDisabled | wpHelpNotVisible);
		pagesVector[shdCloseDoor] = pPage;
	}
	{
		CHomingAxisPage *pPage = new CHomingAxisPage(this);
		pagesVector[shdHoming] = pPage;
	}
	{
		CElapsingTimeWizardPage *pPage = new CElapsingTimeWizardPage("Shutdown in Progress", IN_PROCESS_IMAGE_ID, wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible);
		pagesVector[shdShortWizardInProgress] = pPage;
	}
	{
		CCheckBoxWizardPage *pPage = new CCheckBoxWizardPage(LOAD_STRING(IDS_SYSTEM_CHECKLIST), MATERIALS_CABINET_PAGE_IMAGE_ID, wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow | wpHelpNotVisible);
		pPage->SubTitle = LOAD_STRING(IDS_SYSTEM_CHECKLIST_CONFIRM);
		pagesVector[shdInsertCleanser] = pPage;
	}
	{
		CMessageWizardPage *pPage = new CMessageWizardPage("Note:", SHD_INFORMATION_PICTURE_ID, wpPreviousDisabled | wpHelpNotVisible);
		pagesVector[shdMaterialNoticePage] = pPage;
	}
	{
		CMessageWizardPage *pPage = new CMessageWizardPage("Advanced Mode", SHD_INFORMATION_PICTURE_ID, wpPreviousDisabled | wpHelpNotVisible);
		pPage->SubTitle = LOAD_STRING(IDS_FACTORY_MODE_NOTICE);
		pagesVector[shdFactoryModeNoticePage] = pPage;
	}
	{
		CElapsingTimeWizardPage *pPage = new CElapsingTimeWizardPage("Washing Pipes", IN_PROCESS_IMAGE_ID, wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible);
		pagesVector[shdLongWizardInProgress] = pPage;
	}
	{
		CCheckBoxWizardPage *pPage = new CCheckBoxWizardPage("Remove All Cartridges", MATERIALS_CABINET_PAGE_IMAGE_ID, wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow | wpHelpNotVisible);
		pPage->SubTitle = LOAD_STRING(IDS_SYSTEM_CHECKLIST_CONFIRM);
		pPage->Strings.Add("All cartridges have been removed.");
		pagesVector[shdRemoveAllCartridges] = pPage;
	}
	{
		CElapsingTimeWizardPage *pPage = new CElapsingTimeWizardPage("Draining System", IN_PROCESS_IMAGE_ID, wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible);
		pPage->SubTitle = STEP_DURATION(LOAD_STRING_ARRAY(3 - 1, IDN_SHD_RINSING_DURATION));
		pagesVector[shdBlockDraining] = pPage;
	}
	{
		CMessageWizardPage *pPage = new CMessageWizardPage(WIZARD_COMPLETED_STR, SUCCESSFULLY_COMPLETED_IMAGE_ID, wpDonePage | wpPreviousDisabled | wpCancelDisabled | wpHelpNotVisible);
		pagesVector[shdWizardCompleted] = pPage;
	}
	{
		CCheckBoxWizardPage *pPage = new CCheckBoxWizardPage(LOAD_STRING(IDS_SYSTEM_CHECKLIST), MATERIALS_CABINET_PAGE_IMAGE_ID, wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow | wpHelpNotVisible | wpIgnoreOnPrevious);
		pPage->SubTitle = LOAD_STRING(IDS_SYSTEM_CHECKLIST_CONFIRM);
		pagesVector[shdReplaceCartridge] = pPage;
	}
	for(TPagePointersIterator p = pagesVector.begin(); p != pagesVector.end(); p++)
	{
		AddPage(*p);
	}

	for(int i = 0; i < AXIS_T; ++i)
	{
		m_AxesTable1[i] = false ;
	}
	if((Q2RTApplication->GetMachineType()) == (mtObjet1000))
	{
		m_AxesTable1[AXIS_T] = false;   //avoid home t during purge (The requirment was to fix it only on objet1000)
	}
	else
	{
		m_AxesTable1[AXIS_T] = true;
	}
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Start the wizard session event
void CShutdownWizard::StartEvent()
{
	// Closing any open RF dialog
	for(int i = LAST_TANK_TYPE - 1; i >= FIRST_TANK_TYPE; i--)
	{
		FrontEndInterface->HideTankIDNoticeDlg(i);
		//Disable RF messages for all tanks
		m_BackEnd->SkipMachineResinTypeChecking(static_cast<TTankIndex>(i), true);
	}

	SetDefaultTimeout(m_ParamMgr->SHD_PagesInactivityTimeout_Min.Value() * 60 * 1000);

	m_WizardModeOption       = SHORT_MODE_OPTION;
	m_TankToReplace          = NO_TANK;
	m_CurrentCleaningStep    = 0;
	m_DelayBetweenStepsStart = 0;
	m_FactoryModeEnabled     = false;
	m_CurrentTank            = 0;
	m_SetOriginalSubtitle    = true;

	// Resetting the modes
	m_BackEnd->GotoDefaultMode();

	// Disable vacuum errors
	m_BackEnd->EnableDisableVacuumErrorHandling(false);
	m_BackEnd->ActivateWaste(true);

	m_BackEnd->TurnHeadFilling(false);
	if(m_ParamMgr->SHD_RunUnattended)
		GetPageById(shdWizardCompleted)->AddAttribute(wpSelfClosingPage);
	else
		GetPageById(shdWizardCompleted)->RemoveAttribute(wpSelfClosingPage);

	// Cabinet A, Cabinet B, Cabinet C, ..
	m_CabinetNames.clear();
	for(int i = 0; i < m_ParamMgr->NumOfMaterialCabinets; i++)
		m_CabinetNames.push_back(QString("Cabinet ").append(1, 'A' + i));

	StartAsyncHeatingCycle(TOTAL_NUMBER_OF_HEATERS, true);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// End the wizard session event
void CShutdownWizard::EndEvent()
{
	CleanUp();
	m_EnterStandBy = ! m_ParamMgr->SHD_RunUnattended;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CShutdownWizard::HelpEvent(CWizardPage *WizardPage)
{
	Application->HelpFile = DefaultHelpFileName().c_str();

	switch(WizardPage->GetPageNumber())
	{
		case shdWelcomePage:
			Application->HelpContext(10);
			break;
		case shdSelectWizardMode:
			Application->HelpContext(20);
			break;
		default:
			break;
	}
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Cancel the wizard session event
void CShutdownWizard::CancelEvent(CWizardPage *WizardPage)
{
	m_BackEnd->SetActuatorState(ACTUATOR_ID_AIR_VALVE, ACTUATOR_OFF); //this is a necessary ugly patch till all wizards event will be synchronized
	switch(WizardPage->GetPageNumber())
	{
		case shdShortWizardInProgress:
		case shdInsertCleanser:
		case shdLongWizardInProgress:
		case shdBlockDraining:
		case shdMaterialNoticePage:
		case shdReplaceCartridge:
		{
			m_PumpControllers.deletePumpControllers();
			m_BackEnd->TurnHeadHeaters(false); // todo 1 -cReview -oArcady.Volman@objet.com: ask why
			m_BackEnd->WipeHeads(true);
			m_BackEnd->WipeHeads(false);
			break;
		}
		default:
			break;
	}
	CleanUp();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CShutdownWizard::CleanUp()
{
	TQErrCode Err = Q_NO_ERROR;

	// Finish the wizard in DM3 mode, only if performing Long wizard with Cleanser
	if(m_WizardModeOption == ShutdownWizard::LONG_MODE_OPTION)
		m_BackEnd->UpdateChambers(false);

	m_PumpControllers.deletePumpControllers();
	// Resetting the modes
	m_BackEnd->GotoDefaultMode();
	m_BackEnd->EnableStandByMode();
	// Enable vacuum errors
	m_BackEnd->ActivateWaste(false);
	m_BackEnd->BEI_WaitUntilTrayInserted();
	WriteToLogFile(LOG_TAG_GENERAL, "ShutdownWizard. CleanUp runs GoHome for Axis t, x,y");
	/*
	z_axis doesn't perform home movment, because we need to make sure the model won't collide with the Heads - bug 374
	*/
	T_AxesTable AxesTable = {true,   /* X Axis*/
	                         true,   /* Y Axis*/
	                         false,  /* Z Axis*/
	                         true
	                        };  /* T Axis*/
	EnableAllAxesAndHome(AxesTable, true, true);
	//3rd 'true' arg will send T axis to a lower point which the basket (of the new Purge unit) does not touch the heads
	TRY_FUNCTION_IGNORE_CANCELLED(Err, m_BackEnd->GotoPurgePosition(true, false, true));	// 3rd 'true' arg will send T axis to a lower point which the basket (of the new Purge unit) does not touch the heads);
	m_BackEnd->EnableMotor(false, AXIS_ALL);
	m_BackEnd->LockDoor(false);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CShutdownWizard::ShortWizardInProgressPageEnter(CWizardPage *WizardPage)
{
	WriteToLogFile(LOG_TAG_GENERAL, "Starting short shutdown process");
	// When unattended, there's no user to tell us whether or not the tray is clean. So we move Z Down - to be on the "safe side".
	if(m_ParamMgr->SHD_RunUnattended)
	{
		WriteToLogFile(LOG_TAG_GENERAL, "ShutdownWizard. ShortWizardInProgressPageEnter( runs Enable AXIS_Z  and move Z down");
		EnableAxeAndHome(AXIS_Z);
		m_BackEnd->MoveZAxisDownToLowestPosition();  //On the wheels
	}

	YieldAndSleepWizardThread();

	// Making sure that the block is hot.
	HeadsHeatingCycle();

	if(m_ParamMgr->SHD_RunUnattended)
		m_BackEnd->WaitForEndOfMovement(AXIS_Z, Z_DOWN_WAIT_TIME);

	HeadsDrainingCycle(WizardPage, m_ParamMgr->SHD_BlockDrainingTimeInSec_ShortMode);

	if(IsCancelled())
		throw CQ2RTAutoWizardCancelledException();

	GotoPage(shdWizardCompleted);
}

void CShutdownWizard::AddTanks()
{
	// All tanks are washed
	if(m_CurrentCleaningStep >= CLEANING_STEPS)
		return;

	for(int i = m_CurrentTank; i < TOTAL_NUMBER_OF_CONTAINERS; i += 4)
		m_RequiredTanks.push_back(static_cast<TTankIndex>(i));
}

void CShutdownWizard::InsertCleanserPageEnter(CWizardPage *WizardPage)
{
	m_RequiredTanks.clear();

	AddTanks(); // Fills m_RequiredTanks vector

	if(m_RequiredTanks.empty())
	{
		GotoPage(shdBlockDraining);
		m_CurrentTank = 0;
		return;
	}

	if(m_FactoryModeEnabled)
	{
		GotoPage(shdLongWizardInProgress);
		return;
	}

	CCheckBoxWizardPage *Page = dynamic_cast<CCheckBoxWizardPage*>(WizardPage);
	if(! Page)
		throw "Unexpected";

	if(m_SetOriginalSubtitle)
		Page->SubTitle = LOAD_QSTRING(IDS_SYSTEM_CHECKLIST_CONFIRM);
		
	Page->SetImageID(InsertTanksImageID[m_CurrentCleaningStep]);

	Page->Strings.Clear();

	for(TTankIndexVector::const_iterator i = m_RequiredTanks.begin(); i != m_RequiredTanks.end(); ++i)
	{
		m_BackEnd->AllowServiceMaterials(*i, true); // Allow Service materials
		Page->Strings.Add(QFormatStr("Cleaning cartridge loaded in '%s' compartment.", TankToStr(*i).c_str()));
	}
	Page->Refresh();

	// Refresh the front-end display
	FrontEndInterface->UpdateStatus(FE_REFRESH_MODES_DISPLAY, 0, true);
}

void CShutdownWizard::LongWizardInProgressPageEnter(CWizardPage *WizardPage)
{
	// Making sure that the block is hot.
	HeadsHeatingCycle();
	if(IsCancelled())
		throw CQ2RTAutoWizardCancelledException();
	WriteToLogFile(LOG_TAG_GENERAL, "Starting pipes washing cycle.");

	m_CancelReason = "";

	SetOperationMode(omHigh); // Set to high thermistors
	SetMaterialsToCleanser();
	m_BackEnd->UpdateChambers(false);

	PerformWashingCycle(m_RequiredTanks);

	//Notify the machine that its piping has been flushed thoroughly (without reduced number of washing cycles)
	UpdatePipingFullWashingCyclesState();

	m_CurrentCleaningStep++;

	WriteToLogFile(LOG_TAG_GENERAL, "Pipes washing cycle completed.");
}

void CShutdownWizard::UpdatePipingFullWashingCyclesState()
{
	//mark all the segments as being flushed in 'Full' mode
	//(in Material-Replacement-Wizard's terms)
	for(int seg = NUMBER_OF_SEGMENTS - 1; seg >= FIRST_SEGMENT; --seg)
		m_ParamMgr->MRW_IsSegmentEconomy[seg] = 0;

	m_ParamMgr->SaveSingleParameter(&m_ParamMgr->MRW_IsSegmentEconomy);
}

void CShutdownWizard::PerformWashingCycle(TTankIndexVector &Tanks)
{
	if(m_PumpControllers.isPumpsVectorEmpty())
		createPumpControllers(Tanks, ((m_CurrentCleaningStep % 2 == 0) ? ResinFlowUtils::PipesPriming : ResinFlowUtils::PipesWashing));

	WashingCycle();
	if(IsCancelled())
		throw CQ2RTAutoWizardCancelledException();

	// After successfull WashingCycle - delete WashingCycle PumpControllers
	m_PumpControllers.deletePumpControllers();
	if(m_CancelReason != "")
		throw CQ2RTAutoWizardException(m_CancelReason);
}

void CShutdownWizard::BlockDrainingPageEnter(CWizardPage *WizardPage)
{
	CElapsingTimeWizardPage *Page = dynamic_cast<CElapsingTimeWizardPage*>(WizardPage);
	Page->Title = QFormatStr("Draining System Step %d/%d", m_CurrentCleaningStep + 1, m_CurrentCleaningStep + 1).c_str();
	Page->SubTitle = STEP_DURATION(QIntToStr(m_ParamMgr->SHD_StepDurationMin[m_CurrentCleaningStep]).c_str());
	Page->Refresh();

	TQErrCode Err = Q_NO_ERROR;
	TRY_FUNCTION(Err, m_BackEnd->GotoPurgePosition(true, true));

	// Making sure that the block is hot.
	HeadsHeatingCycle();
	if(IsCancelled())
		throw CQ2RTAutoWizardCancelledException();
	m_CancelReason = "";

	ResinFlowUtils::TPumpIndicesVector v;
	for(int i = FIRST_PUMP; i < NUMBER_OF_PUMPS; i++)
		v.push_back(static_cast<TPumpIndex>(i));
	createPumpControllers(v, ResinFlowUtils::PipesEmptying);

	WaitForDelayBetweenStepsToFinish(QSecondsToTicks(m_ParamMgr->SHD_DelayTimeBetweenStepsSec_LongPipe));

	try
	{
		WashingCycle(); // translate exception message to relevant user readable message
	}
	catch(const ETimeout &e) // override WashingCycle timeout description
	{
		throw ETimeout("Waiting for the block to become not full during pipes draining");
	}

	if(IsCancelled())
		throw CQ2RTAutoWizardCancelledException();

	m_PumpControllers.deletePumpControllers(); // After successfull WashingCycle - delete WashingCycle PumpControllers

	if(m_CancelReason != "")
		throw CQ2RTAutoWizardException(m_CancelReason);

	HeadsDrainingCycle(WizardPage, m_ParamMgr->SHD_BlockDrainingTimeInSec_LongMode, false, false, false);

	if(IsCancelled())
		throw CQ2RTAutoWizardCancelledException();

	GotoPage(shdWizardCompleted);
}

void CShutdownWizard::WizardCompletedPageEnter(CWizardPage *WizardPage)
{
	CMessageWizardPage *Page = dynamic_cast<CMessageWizardPage *>(WizardPage);
	QString msg = "You may now shut down the printer computer and turn off power to the printer.\n\n";
	if(m_WizardModeOption == ShutdownWizard::LONG_MODE_OPTION)
		msg += "Remember that there is cleaning fluid in the printer. You must run the Material Replacement wizard before next printing.";
	Page->SubTitle = msg;
	Page->Refresh();
}

void CShutdownWizard::ReplaceCartridgePageEnter(CWizardPage *WizardPage)
{
	CCheckBoxWizardPage *Page = dynamic_cast<CCheckBoxWizardPage *>(WizardPage);
	SetNextPage(WizardPage);

	Page->Strings.Clear();
	QString str = "Empty cleaning cartridge replaced in '" + TankToStr(m_TankToReplace) + "' compartment.";
	WriteToLogFile(LOG_TAG_GENERAL, str);
	Page->Strings.Add(str);
	Page->Refresh();
}

bool CShutdownWizard::NoticePagePageEnter(CWizardPage *WizardPage)
{
	if(m_CurrentCleaningStep != 0)  //We want to display the notice only once : on the first enter to this page
	{
		GotoNextPage(false);
		return false;
	}

	CMessageWizardPage *Page = dynamic_cast<CMessageWizardPage *>(WizardPage);
	// Check if the liquid level in tanks is enough to complete the wizard without replacing the tanks
	Page->SubTitle = "";
	TQStringVector tokens;
	for(TTankIndexVectorIter i = m_RequiredTanks.begin(); i != m_RequiredTanks.end(); i++)
		if(m_BackEnd->GetTankWeightInGram(*i) < m_ParamMgr->SHD_NeededCartridgeWeight)  // todo -oArcady.Volman@objet.com -cNone: MAKE SURE THIS DOES NOT INITIATE COMMUNICATION.
			tokens.push_back(ChamberToStr(TankToStaticChamber(*i)));
	JoinTokens(Page->SubTitle, tokens, APP_PARAMS_DELIMETER);
	if(Page->SubTitle == "")  // if there's nothing to inform the usr, skip this page
	{
		GotoNextPage(false);
		return false;
	}

	Page->SubTitle = QFormatStr(LOAD_STRING(IDS_INSUFFICIENT_AMOUNT), Page->SubTitle.c_str());
	Page->Refresh();
	return true;
}

void CShutdownWizard::RemoveAllCartridgesPageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason)
{
	CCheckBoxWizardPage *Page = dynamic_cast<CCheckBoxWizardPage*>(WizardPage);

    for (int retries = 0; retries < 5; ++retries)
    {
        YieldAndSleepWizardThread(3 * 1000);
        for (int i = FIRST_TANK_TYPE; i < TOTAL_NUMBER_OF_CONTAINERS; ++i)
        {
            if (m_BackEnd->IsMicroSwitchInserted(i)                                                     ||
                (CHECK_NOT_EMULATION(m_ParamMgr->OCB_Emulation) && m_BackEnd->IsRelevantTankInserted(i))  )
            {
                Page->SubTitle = QFormatStr("Remove cartridge %s", TankToStr(static_cast<TTankIndex>(i)).c_str());
                WriteToLogFile(LOG_TAG_GENERAL, Page->SubTitle.c_str());
                SetNextPage(GetCurrentPage());
                return;
            }
        }
    }
}

void CShutdownWizard::PageEnter(CWizardPage *WizardPage)
{
	TQErrCode Err = Q_NO_ERROR;
	TWizardPagesIndex PageIndex = (TWizardPagesIndex)WizardPage->GetPageNumber();
	try
	{
		switch(PageIndex)
		{
				// todo -oArcady.Volman@objet.com -cNone: checkif all exceptions that should be caught are caught in the entire wizard
			case shdWelcomePage:
				if(false == m_ParamMgr->SHD_RunUnattended)
					break;
                m_pNextPageTimer->Enabled = true;
				return;

            case shdSelectWizardMode:
                m_pNextPageTimer->Enabled = false;
                break;

			case shdHoming:
				SetMachineCleaningStartPage();
				break;

			case shdCheckWaste:
				CheckWastePageEnter(WizardPage);
				break;

			case shdShortWizardInProgress:
				ShortWizardInProgressPageEnter(WizardPage);
				break;

			case shdInsertCleanser:
				InsertCleanserPageEnter(WizardPage);
				break;

			case shdMaterialNoticePage:
				if(! NoticePagePageEnter(WizardPage))
					return;
				break;

			case shdFactoryModeNoticePage:
				if(! m_FactoryModeEnabled)
					GotoNextPage(false);
				break;

			case shdLongWizardInProgress:
			{
				CElapsingTimeWizardPage *Page = dynamic_cast<CElapsingTimeWizardPage*>(WizardPage);
				Page->Title = QFormatStr("Washing Pipes Step %d/%d", m_CurrentCleaningStep + 1, CLEANING_STEPS + 1).c_str();
				Page->SubTitle = STEP_DURATION(QIntToStr(m_ParamMgr->SHD_StepDurationMin[m_CurrentCleaningStep]).c_str());
				if(Page->IsPaused)
				{
					Page->Resume();
					WriteToLogFile(LOG_TAG_GENERAL, QFormatStr("Resuming long shutdown process - step: %d", m_CurrentCleaningStep + 1));
				}
				else
				{
					Page->Reset();
					WriteToLogFile(LOG_TAG_GENERAL, QFormatStr("Starting long shutdown process - step: %d", m_CurrentCleaningStep + 1));
				}
				Page->Refresh();

				// Perform only once at the beginning
				if(m_CurrentCleaningStep == 0)
					TRY_FUNCTION(Err, m_BackEnd->GotoPurgePosition(true, true));

				// Complete the waste draining delay between the steps
				if(m_CurrentCleaningStep != 0)
				{
					if(m_CurrentCleaningStep % 2 == 0)
						WaitForDelayBetweenStepsToFinish(QSecondsToTicks(m_ParamMgr->SHD_DelayTimeBetweenStepsSec_LongPipe)); // between 1 to 2
					else
						WaitForDelayBetweenStepsToFinish(QSecondsToTicks(m_ParamMgr->SHD_DelayTimeBetweenStepsSec_ShortPipe)); // between 0 to 1 & 2 to 3
				}

				LongWizardInProgressPageEnter(WizardPage);

				// Initilize members involved in cartridge replacement during Washing Cycle for the next cycle
				m_TankToReplace = NO_TANK;

				m_CurrentTank++;

				m_DelayBetweenStepsStart = QGetTicks();

				if(m_CurrentCleaningStep < CLEANING_STEPS)
					GotoPage(shdInsertCleanser);
				else
					GotoPage(shdRemoveAllCartridges);
			}
			break;

			case shdBlockDraining: //also drains pipes
				BlockDrainingPageEnter(WizardPage);
				break;

			case shdWizardCompleted:
				WizardCompletedPageEnter(WizardPage);
				break;

			case shdReplaceCartridge:
				ReplaceCartridgePageEnter(WizardPage);
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
		m_TankToReplace = ResinFlowE._tankIndex;
		GotoPage(shdReplaceCartridge);
		return;
	}
	catch(...)
	{
		m_PumpControllers.suspendAllPumpControllers();
		throw;
	}

	if(true == m_ParamMgr->SHD_RunUnattended)
	{
		YieldAndSleepWizardThread(2 * 1000);
		if(PageIndex != shdWizardCompleted &&
		        PageIndex != shdShortWizardInProgress &&
		        PageIndex != shdHoming /*&&
		/*PageIndex != ShutdownWizard::shdPagesCount */)
			GotoNextPage(true);

	}

} //PageEnter

void CShutdownWizard::WaitForDelayBetweenStepsToFinish(unsigned Duration)
{
	while(! IsCancelled())
	{
		if(QGetTicks() > (m_DelayBetweenStepsStart + Duration))
		{
			WriteToLogFile(LOG_TAG_GENERAL, QFormatStr("Finished delay between step %d and %d", m_CurrentCleaningStep, m_CurrentCleaningStep + 1));
			return;
		}

		YieldAndSleepWizardThread();
	}
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CShutdownWizard::SetMachineCleaningStartPage()
{
	if(LONG_MODE_OPTION == m_WizardModeOption)
		SetNextPage(shdInsertCleanser);
	else if(SHORT_MODE_OPTION == m_WizardModeOption)
		SetNextPage(shdShortWizardInProgress);
	else
	{
		assert(false);
		throw EQException("Unexpected wizard option");
	}
}

void CShutdownWizard::SelectWizardModePageLeave(CWizardPage *WizardPage)
{
	m_WizardModeOption = dynamic_cast<CRadioGroupWizardPage *>(WizardPage)->GetSelectedOption();
	if(m_WizardModeOption == LONG_MODE_OPTION)
		WriteToLogFile(LOG_TAG_GENERAL, "Long wizard mode selected.");
	else if(m_WizardModeOption == SHORT_MODE_OPTION)
		WriteToLogFile(LOG_TAG_GENERAL, "Short wizard mode selected.");
}

bool CShutdownWizard::InsertCleanserPageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason)
{
	CCheckBoxWizardPage *Page = dynamic_cast<CCheckBoxWizardPage *>(WizardPage);

	if(LeaveReason == lrCanceled)
	{
		Page->SubTitle = "";
		return false;
	}

	if(m_FactoryModeEnabled && (m_CurrentCleaningStep != 0))
		return true;

	// Check for "hidden" factory mode
	// -----------------------------------------------------------------------------
	int i = FIRST_TANK_TYPE;
	for(; i < TOTAL_NUMBER_OF_CONTAINERS; i++)
	{
		if(CHECK_NOT_EMULATION(m_ParamMgr->OCB_Emulation))
		{  
			if(m_BackEnd->GetTankMaterialType(static_cast<TTankIndex>(i)).compare((IS_MODEL_TANK(i) ? MODEL_CLEANSER : SUPPORT_CLEANSER)) == 0)
			{
				if(m_BackEnd->GetTankWeightInGram(i) < m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(static_cast<TTankIndex>(i))])
				{  
					break;
				}
			}
			else
			{
                break;
            }
		}
	}

	if(i == TOTAL_NUMBER_OF_CONTAINERS)
		m_FactoryModeEnabled = true;

	if(m_FactoryModeEnabled)
	{
		SetNextPage(shdFactoryModeNoticePage);
		return m_FactoryModeEnabled;
	}
	// -----------------------------------------------------------------------------

	for(TTankIndexVectorIter i = m_RequiredTanks.begin(); i != m_RequiredTanks.end(); i++)
	{
		QString requiredMaterial = (IS_MODEL_TANK(*i) ? MODEL_CLEANSER : SUPPORT_CLEANSER);
		// Check if resin types matches the requested.
		if(CHECK_NOT_EMULATION(m_ParamMgr->OCB_Emulation) && m_BackEnd->GetTankMaterialType(*i) != requiredMaterial)
		{
			QString str = QFormatStr(LOAD_STRING(IDS_RESIN_TYPE_SHOULD_BE_DIFFERENT), TankToStr(*i).c_str(), m_BackEnd->GetTankMaterialType(*i).c_str(), requiredMaterial.c_str());
			WriteToLogFile(LOG_TAG_GENERAL, QFormatStr("%s", str.c_str()));
			Page->SubTitle        = str;
			m_SetOriginalSubtitle = false;
			SetNextPage(GetCurrentPage());
			return false;
		}
	}
	// Check if tanks are not empty
	for(TTankIndexVectorIter i = m_RequiredTanks.begin(); i != m_RequiredTanks.end(); i++)
	{
		if(CHECK_NOT_EMULATION(m_ParamMgr->OCB_Emulation) && (m_BackEnd->GetTankWeightInGram(*i) < m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(*i)]))
		{
			QString str = QFormatStr("The current %s cleanser level is less than required by the wizard.", ChamberToStr(TankToStaticChamber(*i), true).c_str());
			WriteToLogFile(LOG_TAG_GENERAL, QFormatStr("%s", str.c_str()));
			Page->SubTitle        = str;
			m_SetOriginalSubtitle = false;
			SetNextPage(GetCurrentPage());
			return false;
		}
	}

	// reset subtitles for next wizard run
	if(GetNextPage() != GetCurrentPage())
	{
		Page->SubTitle        = "";
		m_SetOriginalSubtitle = true;
	}

	return true;
}

void CShutdownWizard::ReplaceCartridgePageLeave(CWizardPage *WizardPage)
{
	CCheckBoxWizardPage *Page = dynamic_cast<CCheckBoxWizardPage *>(WizardPage);
	QString requiredMaterial = (IS_MODEL_TANK(m_TankToReplace) ? MODEL_CLEANSER : SUPPORT_CLEANSER);
	if(CHECK_NOT_EMULATION(m_ParamMgr->OCB_Emulation) && m_BackEnd->GetTankMaterialType(m_TankToReplace) != requiredMaterial)
	{
		Page->SubTitle = "Load " + TankToStr(m_TankToReplace) + " cleanser cartridge.";
		SetNextPage(GetCurrentPage());
		return;
	}

	if(CHECK_NOT_EMULATION(m_ParamMgr->OCB_Emulation) && (m_BackEnd->GetTankWeightInGram(m_TankToReplace) <
	        m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(m_TankToReplace)]))
	{
		Page->SubTitle = "The current " + TankToStr(m_TankToReplace) + " cleanser level is less than required by the wizard.";
		SetNextPage(GetCurrentPage());
		return;
	}

	Page->SubTitle = ""; // in case the correct tank with good weight was inserted - resetting for the next replacement
	//The tank is OK now, we can go back and continue the process interrupted
	SetNextPage(GetPreviousPage());
}

void CShutdownWizard::PageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason)
{
	TWizardPagesIndex PageIndex = (TWizardPagesIndex)WizardPage->GetPageNumber();
	if(LeaveReason == lrCanceled)
	{
		switch(PageIndex)
		{
			case shdInsertCleanser:
				InsertCleanserPageLeave(WizardPage, LeaveReason);
				break;
		}
	}

	if(LeaveReason != lrGoNext)
		return;

	switch(PageIndex)
	{
		case shdSelectWizardMode:
			SelectWizardModePageLeave(WizardPage);
			break;

		case shdCheckWaste:
			CheckWastePageLeave(WizardPage);
			break;

		case shdInsertCleanser:
			if(InsertCleanserPageLeave(WizardPage, LeaveReason) == false)
				return;
			break;

		case shdRemoveAllCartridges:
			RemoveAllCartridgesPageLeave(WizardPage, LeaveReason);
			break;

		case shdReplaceCartridge:
			ReplaceCartridgePageLeave(WizardPage);
			break;
	}
} // PageLeave

void CShutdownWizard::CheckWastePageLeave(CWizardPage *WizardPage)
{
	if(CHECK_NOT_EMULATION(m_ParamMgr->OCB_Emulation) && (m_BackEnd->GetRemainingWasteWeight() < m_ParamMgr->SHD_WasteFreeSpace_Gr))
		SetNextPage(WizardPage);
}

void CShutdownWizard::CheckWastePageEnter(CWizardPage *WizardPage)
{
	CCheckBoxWizardPage *Page = dynamic_cast<CCheckBoxWizardPage*>(WizardPage);

	Page->Strings.Clear();
	if(CHECK_NOT_EMULATION(m_ParamMgr->OCB_Emulation) && (m_BackEnd->GetRemainingWasteWeight() < m_ParamMgr->SHD_WasteFreeSpace_Gr))
	{
		Page->Strings.Add("Waste cartridge replaced");
		Page->Refresh();
	}
	else
	{
		GotoNextPage(false);
		return;
	}
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CShutdownWizard::HeadsHeatingCycle()
{
	// Resetting the modes
	m_BackEnd->GotoDefaultMode();
	// Entering 'High Quality' Mode
	m_BackEnd->EnterMode(PRINT_MODE[HIGH_TEMPATURE_INDEX][DIGITAL_MATERIAL_OPERATION_MODE],
	                     MACHINE_QUALITY_MODES_DIR(HIGH_TEMPATURE_INDEX, DIGITAL_MATERIAL_OPERATION_MODE));
	/* FIXME REMOVE
	CScopeRestorePoint<int> sc(m_ParamMgr->HeatersTemperatureArray[PRE_HEATER]);
	m_ParamMgr->HeatersTemperatureArray[PRE_HEATER] = ShutdownWizard::PRE_HEATER_TEMP_A2D;
	*/
	CQ2RTAutoWizard::HeadsHeatingCycle();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CShutdownWizard::WashingCycle()
{
	m_PumpControllers.clearControllerOperationDoneFlag();
	ResinFlowUtils::AirValve av(this);
	av.open();

	m_PumpControllers.clearControllerOperationDoneFlag();
	while(!m_PumpControllers.isControllerOperationDone() && !IsCancelled())
	{
		try
		{
			m_PumpControllers.controlAllPumps();
		}
		catch(const ETimeout &e) // translate exception message to user readable message
		{
			throw ETimeout("waiting for the block to become not full during pipes rinsing");
		}

		YieldAndSleepWizardThread();
	}

	av.close();
	if(m_PumpControllers.isControllerOperationDone())
	{
		// _requiredNetPumpingMilliseconds = 0;  // todo: shahar: check is needed.
	}
}

CShutdownWizard::~CShutdownWizard()
{ Q_SAFE_DELETE(m_pNextPageTimer); }

void CShutdownWizard::GotoNextPage(bool CheckDoGotoNextPage)
{
	bool DoGotoNextPage = true;
	if(true == m_ParamMgr->SHD_RunUnattended)
		DoGotoNextPage = CheckDoGotoNextPage;
	if(DoGotoNextPage)
		CQ2RTAutoWizard::GotoNextPage();
}

// Close the wizard session event
void CShutdownWizard::CloseEvent()
{
	DisableServiceMaterials();
	m_BackEnd->EnableDisableVacuumErrorHandling(true);
	if(m_ParamMgr->SHD_RunUnattended)
	{
		m_ParamMgr->SHD_RunUnattended = false;
		m_ParamMgr->SaveSingleParameter(&m_ParamMgr->SHD_RunUnattended);
		m_BackEnd->TurnPower(false);
		Q2RTApplication->TerminateAndShutdown();
	}
}

void CShutdownWizard::SetOperationMode(TThermOpMode om)
{
	TThermistorsOperationMode Thermistors;
	TTankOperationMode        Tanks;

	switch(om)
	{
		case omLow:
			Thermistors = LOW_THERMISTORS_MODE;
#ifdef OBJET_MACHINE_KESHET
			Tanks       = S_M1_M7_ACTIVE_TANKS_MODE; //OBJET_MACHINE_KESHET
#else
			Tanks       = S_M1_M2_M3_ACTIVE_TANKS_MODE;
#endif
			break;
		case omHigh:
			Thermistors = GetThermOpModeForHQHS();
#ifdef OBJET_MACHINE_KESHET
			Tanks       = S_M1_M3_M5_ACTIVE_TANKS_MODE;//itamar, check!!!!!
#else
			Tanks       = S_M1_ACTIVE_TANKS_MODE;
#endif
			break;
		default:
			throw EQException("Incorrect TThermOpMode");
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

void CShutdownWizard::SetMaterialsToCleanser()
{
	m_BackEnd->GotoDefaultMode();

	for(int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; ++i)
	{
		QString MaterialType = IS_MODEL_TANK(i) ? MODEL_CLEANSER : SUPPORT_CLEANSER;

		m_ParamMgr->TypesArrayPerTank[i] = MaterialType;
		m_ParamMgr->TypesArrayPerPipe[i] = MaterialType;

		for(int Segment = FIRST_SEGMENT; Segment < NUMBER_OF_SEGMENTS; Segment++)
			if(IsSegmentRelatedToTank(static_cast<TSegmentIndex>(Segment), static_cast<TTankIndex>(i)))
				m_ParamMgr->MRW_TypesArrayPerSegment[Segment] = MaterialType;

		m_BackEnd->UpdateTanks(static_cast<TTankIndex>(i));
	}

    for (int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; ++i)  // DM6
    {
        m_ParamMgr->TypesArrayPerChamber[i] = (IS_MODEL_CHAMBER(i) ? MODEL_CLEANSER : SUPPORT_CLEANSER);
    }
//	for(int i = FIRST_MODEL_CHAMBER_INDEX; i < LAST_MODEL_CHAMBER_INDEX; i++)   // DM6
//		m_ParamMgr->TypesArrayPerChamber[i] = MODEL_CLEANSER;
//
//	for(int i = FIRST_SUPPORT_CHAMBER_INDEX; i < LAST_SUPPORT_CHAMBER_INDEX; i++)   // DM6
//		m_ParamMgr->TypesArrayPerChamber[i] = SUPPORT_CLEANSER;

	m_ParamMgr->SaveSingleParameter(&m_ParamMgr->TypesArrayPerTank);
	m_ParamMgr->SaveSingleParameter(&m_ParamMgr->TypesArrayPerPipe);
	m_ParamMgr->SaveSingleParameter(&m_ParamMgr->TypesArrayPerChamber);
	m_ParamMgr->SaveSingleParameter(&m_ParamMgr->MRW_TypesArrayPerSegment);

	m_BackEnd->GotoDefaultMode();

	for(int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; i++)
	{
		while(false == m_BackEnd->WaitForIdentificationCompletion(static_cast<TTankIndex>(i), true, /*true = DontWaitIfTankIsOut*/ 100   /*time to wait*/))
		{
			YieldWizardThread();
			if(IsCancelled())
				throw CQ2RTAutoWizardCancelledException();
		}
	}
}

void CShutdownWizard::DisableServiceMaterials()
{
	// Refresh the front-end display
	FrontEndInterface->UpdateStatus(FE_REFRESH_MODES_DISPLAY, 0, true);
	for(int /* TTankIndex */ i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; i++)
	{
		m_BackEnd->SkipMachineResinTypeChecking(i, false);
		m_BackEnd->AllowServiceMaterials(i, false);
		m_BackEnd->UpdateTanks(i);
	}
}
void CShutdownWizard::createPumpControllers(TTankIndexVector& RequiredTanks, ResinFlowUtils::PumpingMode mode)
{
	ResinFlowUtils::TPumpIndicesVector v;
	v = ResinFlowUtils::pumpIndicesVectorByTankIndicesVector(RequiredTanks);

	createPumpControllers(v, mode);
}

// todo -oArcady.Volman@objet.com -cNone: limit blockcycles & other paramas
void CShutdownWizard::createPumpControllers(ResinFlowUtils::TPumpIndicesVector& v, ResinFlowUtils::PumpingMode mode)
{
	using namespace ResinFlowUtils;

	TQMilliseconds requiredNetPumpingMilliseconds = 0;
	TQMilliseconds timeout = m_ParamMgr->SHD_BlockDrainTimeoutInSec;
	VerificationAttributes Attributes = static_cast<VerificationAttributes>(vaTankValid | vaTankWeight | vaTankResinType);
	for(TPumpIndicesVector::iterator itrPumpIndx =	v.begin(); itrPumpIndx != v.end(); ++itrPumpIndx)
	{
		// todo -oArcady.Volman@objet.com -cNone: make sure to reset the controllers when required
		switch(mode)
		{
			case PipesPriming:
				WriteToLogFile(LOG_TAG_GENERAL, QFormatStr("PipesPriming - %s", TankToStr(PumpToTank(*itrPumpIndx)).c_str()));
				requiredNetPumpingMilliseconds = static_cast<TQMilliseconds>(m_ParamMgr->SHD_CleanserPrimingTimeInSec * 1000);
				break;

			case PipesWashing:
				WriteToLogFile(LOG_TAG_GENERAL, QFormatStr("PipesWashing - %s", TankToStr(PumpToTank(*itrPumpIndx)).c_str()));
				requiredNetPumpingMilliseconds = static_cast<TQMilliseconds>(m_ParamMgr->SHD_CleanserWashingTimeInSec * 1000);
				break;

			case BlockFilling:
				WriteToLogFile(LOG_TAG_GENERAL, QFormatStr("BlockFilling - %s", TankToStr(PumpToTank(*itrPumpIndx)).c_str()));
				requiredNetPumpingMilliseconds = 0;  // Ignored for BlockFillingPumpController.
				timeout = m_ParamMgr->SHD_FillingTimeoutInSec;
				break;

			case PipesEmptying:
				WriteToLogFile(LOG_TAG_GENERAL, QFormatStr("PipesEmptying - %s", TankToStr(PumpToTank(*itrPumpIndx)).c_str()));
				requiredNetPumpingMilliseconds = static_cast<TQMilliseconds>(m_ParamMgr->SHD_PipesClearingTimeInSec * 1000);
				Attributes = static_cast<VerificationAttributes>(0);
				break;

			default:
				assert(false);
				throw EQException("bad PumpingMode");
		}

		m_PumpControllers.AddSpecializedPumpController(mode,
		        *itrPumpIndx,
		        requiredNetPumpingMilliseconds,
		        timeout * 1000,
		        m_ParamMgr->SHD_PumpsTimeDutyOn_ms,
		        m_ParamMgr->SHD_PumpsTimeDutyOff_ms,
		        Attributes,
		        ((IS_MODEL_TANK(tankIndexByPumpIndex(*itrPumpIndx))) ? MODEL_CLEANSER : SUPPORT_CLEANSER));

	}
	// =======================================================
}


#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
void __fastcall CShutdownWizard::NextPageTimerExec(TObject * Sender)
{
    GotoNextPageAsync();
}
#pragma warn .8057 // Disable warning Number 8057 "Never use parameter"

