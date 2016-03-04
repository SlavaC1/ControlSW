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
* Updated:    29/04/2012		                                 *
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
#include "QScopeRestorePoint.h"

namespace ShutdownWizard
{

	typedef enum Pages
	{
		// IMPORTANT NOTES:
		// 	- The order of the elements in this enum determines the order of the pages (see CShutdownWizard::CShutdownWizard).
		// 	- The values of the elements of this enum must not be manully alterd (otherwise u'll break shdPagesCount)

		shdWelcomePage,
		shdSelectWizardMode,
		shdCheckTray,
		shdCheckTrayInserted,
		shdCloseDoor,
		shdHoming,
		// beginning of Short Shutdown wizard
		shdShortWizardInProgress,
		// beginning of Long Shutdown wizard
		shdRemoveResinCartridges,
		// these conditional definitions determine the pages orders in the wizard in various machines (notice the pages order assignment in CShutdownWizard::CShutdownWizard)
		shdInsertCleanser,
		shdNoticePage,
		shdLongWizardInProgress,
		shdRemoveCleanser,
		shdBlockDraining,
		// service pages
		shdReplaceCartridge,
		// ending for both modes
		shdWizardCompleted,

		shdPagesCount // this must element must be the last in this enum
	} TWizardPagesIndex;

	const int SHORT_MODE_OPTION    	= 0;
	const int LONG_MODE_OPTION   	= 1;

	const int PRE_HEATER_TEMP_A2D = 2000; // Pre-Heater remains cold

}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
CShutdownWizard::CShutdownWizard(void) :
CQ2RTAutoWizard(IDS_SHD_WIZARD,  true, IN_PROCESS_IMAGE_ID),
m_PumpControllers(this)
{
	using namespace ShutdownWizard;
	TPagePointers pagesVector(shdPagesCount, 0);

	{
		CMessageWizardPage *pPage = new CMessageWizardPage(GetTitle(), IN_PROCESS_IMAGE_ID, wpHelpNotVisible);
		pPage->SubTitle = LOAD_STRING(IDS_SHD_WELCOME_SUBTITLE);
		pagesVector[shdWelcomePage] = pPage;
	}
	{
		CRadioGroupWizardPage *pPage = new CRadioGroupWizardPage(LOAD_STRING(IDS_SYSTEM_ACTION_SELECT),QUESTION_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled);
		pPage->SubTitle = LOAD_STRING(IDS_SHD_MODE_SELECT_SUBTITLE);
		pPage->Strings.Add(LOAD_STRING(IDS_SHD_MODE_SELECT_STR1));
		pPage->Strings.Add(LOAD_STRING(IDS_SHD_MODE_SELECT_STR2));
		pPage->DefaultOption = ShutdownWizard::SHORT_MODE_OPTION;
		pagesVector[shdSelectWizardMode] = pPage;
	}
	{
		CVerifyCleanTrayPage *pPage = new CVerifyCleanTrayPage(this, PREPARATIONS_IMAGE_ID, wpHelpNotVisible);
		pagesVector[shdCheckTray] = pPage;
	}
	{
		CInsertTrayPage *pPage = new CInsertTrayPage(this, PREPARATIONS_IMAGE_ID, wpHelpNotVisible);
		pagesVector[shdCheckTrayInserted] = pPage;
    }
	{
		CCloseDoorPage *pPage = new CCloseDoorPage(this,PREPARATIONS_IMAGE_ID, wpPreviousDisabled | wpHelpNotVisible);
		pagesVector[shdCloseDoor] = pPage;
	}
	{
		CHomingAxisPage *pPage = new CHomingAxisPage(this);
		pagesVector[shdHoming] = pPage;
	}
	{
		CElapsingTimeWizardPage *pPage = new CElapsingTimeWizardPage("Shutdown in Progress",IN_PROCESS_IMAGE_ID,wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible);
		pagesVector[shdShortWizardInProgress] = pPage;
	}
	{
		CCheckBoxWizardPage *pPage = new CCheckBoxWizardPage(LOAD_STRING(IDS_SYSTEM_CHECKLIST),PREPARATIONS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow | wpHelpNotVisible);
		pPage->SubTitle = LOAD_STRING(IDS_SYSTEM_CHECKLIST_CONFIRM);
		pPage->Strings.Add("All cartridges have been removed.");
		pagesVector[shdRemoveResinCartridges] = pPage;
	}
	{
		CCheckBoxWizardPage *pPage = new CCheckBoxWizardPage(LOAD_STRING(IDS_SYSTEM_CHECKLIST),MATERIALS_CABINET_PAGE_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow | wpHelpNotVisible );
		pPage->SubTitle = LOAD_STRING(IDS_SYSTEM_CHECKLIST_CONFIRM);
		pagesVector[shdInsertCleanser] = pPage;
	}
	{
		CMessageWizardPage *pPage = new CMessageWizardPage("Note:",SHD_INFORMATION_PICTURE_ID, wpPreviousDisabled | wpHelpNotVisible);
		pagesVector[shdNoticePage] = pPage;
	}		
	{
		CElapsingTimeWizardPage *pPage = new CElapsingTimeWizardPage("Rinsing System",IN_PROCESS_IMAGE_ID,wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible);
		pagesVector[shdLongWizardInProgress] = pPage;
	}
	{
		CCheckBoxWizardPage *pPage = new CCheckBoxWizardPage("Remove cleaning cartridges",SHD_CLEANING_FLUID_PICTURE_ID,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow | wpHelpNotVisible);
		pagesVector[shdRemoveCleanser] = pPage;
	}
	{
		CElapsingTimeWizardPage *pPage = new CElapsingTimeWizardPage("Draining System (Step 3 of 3)",IN_PROCESS_IMAGE_ID,wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible);
		pPage->SubTitle = STEP_DURATION(LOAD_STRING_ARRAY(3-1, IDN_SHD_RINSING_DURATION));
		pagesVector[shdBlockDraining] = pPage;
	}	
	{
		CMessageWizardPage *pPage = new CMessageWizardPage(WIZARD_COMPLETED_STR,SUCCESSFULLY_COMPLETED_IMAGE_ID,wpDonePage | wpPreviousDisabled | wpCancelDisabled | wpHelpNotVisible);
		pagesVector[shdWizardCompleted] = pPage;
	}
	{
		CCheckBoxWizardPage *pPage = new CCheckBoxWizardPage(LOAD_STRING(IDS_SYSTEM_CHECKLIST),MATERIALS_CABINET_PAGE_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow | wpHelpNotVisible);
		pPage->SubTitle = LOAD_STRING(IDS_SYSTEM_CHECKLIST_CONFIRM);
		pagesVector[shdReplaceCartridge] = pPage;
	}
	for(TPagePointersIterator p = pagesVector.begin(); p != pagesVector.end(); p++)
		AddPage(*p);
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Start the wizard session event
void CShutdownWizard::StartEvent()
{
	// Closing any open RF dialog
	for(int i = LAST_TANK_TYPE-1; i >= FIRST_TANK_TYPE; i--)
	{
		FrontEndInterface->HideTankIDNoticeDlg(i);
		//Disable RF messages for all tanks
		m_BackEnd->SkipMachineResinTypeChecking(static_cast<TTankIndex>(i), true);
	}
	m_WizardModeOption = ShutdownWizard::SHORT_MODE_OPTION;	
	ResetCurrentTank();
	// Initilize members involved in cartridge replacement during Washing Cycle
	m_TankToReplace             = NO_TANK;
	m_IsWashingBlock 			= false;
	m_CurrentBlockWashingCycle  = 0;

	// Resetting the modes
	m_BackEnd->GotoDefaultMode();

	// Disable vacuum errors
	m_BackEnd->EnableDisableVacuumErrorHandling(false);
	m_BackEnd->SetActuatorState(ACTUATOR_ID_WASTE_PUMP, true); // todo 1 -oArcady.Volman@objet.com -cEnd User Benifit: make sure the pump is on only when required

	m_BackEnd->TurnHeadFilling(false);
	if (m_ParamMgr->SHD_RunUnattended)
		GetPageById(ShutdownWizard::shdWizardCompleted)->AddAttribute(wpSelfClosingPage);
	else
		GetPageById(ShutdownWizard::shdWizardCompleted)->RemoveAttribute(wpSelfClosingPage);

	StartAsyncHeatingCycle(TOTAL_NUMBER_OF_HEATERS, true);	
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// End the wizard session event
void CShutdownWizard::EndEvent()
{
	CleanUp();
	m_EnterStandBy = !m_ParamMgr->SHD_RunUnattended;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CShutdownWizard::HelpEvent(CWizardPage *WizardPage)
{
	Application->HelpFile = DefaultHelpFileName().c_str();

	switch(WizardPage->GetPageNumber())
	{
	case ShutdownWizard::shdWelcomePage:
		Application->HelpContext(10);break;
	case ShutdownWizard::shdSelectWizardMode:
		Application->HelpContext(20);break;
	default:
		break;
	}
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Cancel the wizard session event
void CShutdownWizard::CancelEvent(CWizardPage *WizardPage)
{
	m_BackEnd->SetActuatorState(ACTUATOR_ID_AIR_VALVE, ACTUATOR_OFF); //this is a necessary ugly patch till all wizards event will be synchronized
	switch (WizardPage->GetPageNumber())
	{
	case ShutdownWizard::shdShortWizardInProgress:
	case ShutdownWizard::shdInsertCleanser:
	case ShutdownWizard::shdLongWizardInProgress:
	case ShutdownWizard::shdRemoveCleanser:
	case ShutdownWizard::shdBlockDraining:
	case ShutdownWizard::shdNoticePage:
	case ShutdownWizard::shdReplaceCartridge:
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
	{
		SetOperationMode(omLow);
		m_BackEnd->UpdateChambers(false);
	}
	
	m_PumpControllers.deletePumpControllers();
	// Resetting the modes
	m_BackEnd->GotoDefaultMode();
	m_BackEnd->EnableStandByMode();
	// Enable vacuum errors
	m_BackEnd->SetActuatorState(ACTUATOR_ID_WASTE_PUMP,false);
	EnableAllAxesAndHome(NULL, true, true);
	TRY_FUNCTION_IGNORE_CANCELLED(Err, m_BackEnd->GotoPurgePosition(true, false,true));	// 3rd 'true' arg will send T axis to a lower point which the basket (of the new Purge unit) does not touch the heads);
	m_BackEnd->EnableMotor(false, AXIS_ALL);
	m_BackEnd->EnableDisableVacuumErrorHandling(true);
	m_BackEnd->LockDoor(false);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CShutdownWizard::ShortWizardInProgressPageEnter(CWizardPage *WizardPage)
{
	WriteToLogFile(LOG_TAG_GENERAL,"Starting short shutdown process");
	// When unattended, there's no user to tell us whether or not the tray is clean. So we move Z Down - to be on the "safe side".
	if (m_ParamMgr->SHD_RunUnattended)
	{
		EnableAxeAndHome(AXIS_Z);
		m_BackEnd->MoveZAxisDown();
	}
	YieldAndSleepWizardThread();
	// Making sure that the block is hot.
	HeadsHeatingCycle();
	if (m_ParamMgr->SHD_RunUnattended)
		m_BackEnd->WaitForEndOfMovement(AXIS_Z, Z_DOWN_WAIT_TIME);
	HeadsDrainingCycle(WizardPage, m_ParamMgr->SHD_BlockDrainingTimeInSec);
	if (IsCancelled())
		throw CQ2RTAutoWizardCancelledException();
	GotoPage(ShutdownWizard::shdWizardCompleted);
}

void CShutdownWizard::AddModelTanks()
{
    if(m_CurrentTank > m_ParamMgr->CartridgeRedundancy)
		return;

	CScopeRestorePoint<int> sr(m_CurrentTank);
	int CartridgeRedundancy = m_ParamMgr->CartridgeRedundancy;	
	for(int i = ModelCurrentTank(); i < TOTAL_NUMBER_OF_CONTAINERS; i += CartridgeRedundancy + 1)
    {
		if(ValidateModelCurrentTank())
			m_RequiredTanks.push_back(ModelCurrentTank());
		for(int j = 0; j < CartridgeRedundancy + 1; j++)
			PromoteCurrentTank();
	}		
}

void CShutdownWizard::InsertCleanserPageEnter(CWizardPage *WizardPage)
{
	m_RequiredTanks.clear();
	if(ValidateSupportCurrentTank())
		m_RequiredTanks.push_back(SupportCurrentTank());
	AddModelTanks();
	if(m_RequiredTanks.size() == 0)
	{
		GotoPage(ShutdownWizard::shdBlockDraining);
		ResetCurrentTank();
		return;
	}

	CCheckBoxWizardPage *Page = dynamic_cast<CCheckBoxWizardPage*>(WizardPage);
	if (!Page)
		throw "Unexpected";
	Page->Strings.Clear();
	// Allow Service materials
	for (TTankIndexVectorIter i = m_RequiredTanks.begin(); i != m_RequiredTanks.end(); i++)
	{
		m_BackEnd->AllowServiceMaterials(*i, true);
		Page->Strings.Add(QFormatStr("Cleaning cartridge loaded in '%s' compartment.", TankToStr(*i).c_str()));
	}
	Page->Refresh();

	// Refresh the front-end display
	FrontEndInterface->UpdateStatus(FE_REFRESH_MODES_DISPLAY,0,true);
}

bool CShutdownWizard::LongWizardInProgressPageEnter(CWizardPage *WizardPage)
{
    // If the process was interrupted due to empty / missing cartridge during BlockWashingCycles phase
	if(m_IsWashingBlock)
		return true;

	CElapsingTimeWizardPage *Page = dynamic_cast<CElapsingTimeWizardPage*>(WizardPage);
	if (Page->IsPaused)
	{
		Page->Resume();
		WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Resuming long shutdown process - cycle %d", (int)m_CurrentTank));
	}
	else
	{
		Page->Reset();
		WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Starting long shutdown process - cycle %d", (int)m_CurrentTank));
		Page->Title = QFormatStr("Rinsing System (Step %d of %d)", 1 + m_CurrentTank, m_ParamMgr->CartridgeRedundancy + 2);
		Page->SubTitle = STEP_DURATION(QIntToStr(m_ParamMgr->SHD_StepDurationMin[m_CurrentTank]).c_str());
		Page->Refresh();
	}
	
	// Making sure that the block is hot.
	HeadsHeatingCycle();
	if (IsCancelled()) 
		throw CQ2RTAutoWizardCancelledException();
	WriteToLogFile(LOG_TAG_GENERAL,"Starting pipes washing cycle.");

	m_CancelReason = "";

	SetOperationMode(omLow); // Set to low thermistors
	SetMaterialsToCleanser();     
	m_BackEnd->UpdateChambers(false);

	// Splitting the washing process in order to refrain from purge tank flooding
	TTankIndexVector TanksFirstHalf;
	TTankIndexVector TanksSecondHalf;
	for (unsigned i = 0; i < m_RequiredTanks.size(); i++)
	{
		if(i < m_RequiredTanks.size() / 2)
			TanksFirstHalf.push_back(m_RequiredTanks.at(i));
		else
			TanksSecondHalf.push_back(m_RequiredTanks.at(i));
	}
	PerformWashingCycle(TanksFirstHalf);
	PerformWashingCycle(TanksSecondHalf);

	WriteToLogFile(LOG_TAG_GENERAL,"Pipes washing cycle completed.");
	return true;
}

void CShutdownWizard::PerformWashingCycle(TTankIndexVector &Tanks)
{
	if (m_PumpControllers.isPumpsVectorEmpty())
		createPumpControllers(Tanks, (IsCurrentTankReset() ? ResinFlowUtils::PipesPriming : ResinFlowUtils::PipesWashing));

	WashingCycle();
	if (IsCancelled())
		throw CQ2RTAutoWizardCancelledException();

	// After successfull WashingCycle - delete WashingCycle PumpControllers
	m_PumpControllers.deletePumpControllers();
	if (m_CancelReason != "")
		throw CQ2RTAutoWizardException(m_CancelReason);
}

void CShutdownWizard::BlockDrainingPageEnter(CWizardPage *WizardPage)
{
    CElapsingTimeWizardPage *Page = dynamic_cast<CElapsingTimeWizardPage*>(WizardPage);
	Page->Title = QFormatStr("Draining System (Step %d of %d)", m_ParamMgr->CartridgeRedundancy + 2, m_ParamMgr->CartridgeRedundancy + 2);
	Page->SubTitle = STEP_DURATION(QIntToStr(m_ParamMgr->SHD_StepDurationMin[m_ParamMgr->CartridgeRedundancy + 1]).c_str());
	Page->Refresh();

	// Making sure that the block is hot.
	HeadsHeatingCycle();
	if (IsCancelled()) 
		throw CQ2RTAutoWizardCancelledException();
	m_CancelReason = "";

	ResinFlowUtils::TPumpIndicesVector v;
	for(int i = FIRST_PUMP; i < NUMBER_OF_PUMPS; i++)
		v.push_back(static_cast<TPumpIndex>(i));
	createPumpControllers(v, ResinFlowUtils::PipesEmptying);

	try
	{
		WashingCycle(); // translate exception message to relevant user readable message
	}
	catch(const ETimeout &e) // override WashingCycle timeout description
	{
		throw ETimeout("waiting for the block to become not full during pipes draining");
	}
	if (IsCancelled()) 
		throw CQ2RTAutoWizardCancelledException();
	// After successfull WashingCycle - delete WashingCycle PumpControllers
	m_PumpControllers.deletePumpControllers();
	if (m_CancelReason != "")
		throw CQ2RTAutoWizardException(m_CancelReason);
	HeadsDrainingCycle(WizardPage, m_ParamMgr->SHD_BlockDrainingTimeInSec);
	if (IsCancelled()) 
		throw CQ2RTAutoWizardCancelledException();
	GotoPage(ShutdownWizard::shdWizardCompleted);
}

void CShutdownWizard::WizardCompletedPageEnter(CWizardPage *WizardPage)
{
	CMessageWizardPage *Page = dynamic_cast<CMessageWizardPage *>(WizardPage);
	QString msg = "You may now shut down the printer computer and turn off power to the printer.\n\n";
	if (m_WizardModeOption == ShutdownWizard::LONG_MODE_OPTION)
	msg += "Remember that there is cleaning fluid in the printer. You must run the Material Replacement wizard before next printing.";
	Page->SubTitle = msg;
	Page->Refresh();
}

void CShutdownWizard::ReplaceCartridgePageEnter(CWizardPage *WizardPage)
{
	CCheckBoxWizardPage *Page = dynamic_cast<CCheckBoxWizardPage *>(WizardPage);
	SetNextPage(WizardPage);

	Page->Strings.Clear();
	QString str = "Empty cartridge replaced in '" + TankToStr(m_TankToReplace) + "' compartment.";
	WriteToLogFile(LOG_TAG_GENERAL,str);
	Page->Strings.Add(str);
	Page->Refresh();

	// Turn heads heaters OFF after PausedTime_min time (2 hr. by default)
//	if (Q2RTWizardSleep(m_ParamMgr->PausedTime_min * 60)) // This method deals with seconds
//		m_BackEnd->TurnHeadHeaters(false);

	// This timeout is not bug prone. The wizard (and heating) will shutdown after 2 Hr either way
}

bool CShutdownWizard::NoticePagePageEnter(CWizardPage *WizardPage)
{
	if (!IsCurrentTankReset()) //We want tot display the notice only once : on the first enter to this page
	{
		GotoNextPage(false);
		return false;
	}

	CMessageWizardPage *Page = dynamic_cast<CMessageWizardPage *>(WizardPage);
	// Check if the liquid level in tanks is enough to complete the wizard without replacing the tanks
	Page->SubTitle = "";
	TQStringVector tokens;
	for (TTankIndexVectorIter i = m_RequiredTanks.begin(); i != m_RequiredTanks.end(); i++)
		if (m_BackEnd->GetTankWeightInGram(*i) < m_ParamMgr->SHD_NeededCartridgeWeight) // todo -oArcady.Volman@objet.com -cNone: MAKE SURE THIS DOES NOT INITIATE COMMUNICATION.
			tokens.push_back(ChamberToStr(TankToStaticChamber(*i)));
	JoinTokens(Page->SubTitle, tokens, APP_PARAMS_DELIMETER);
	if (Page->SubTitle == "") // if there's nothing to inform the usr, skip this page
	{
		GotoNextPage(false);
		return false;
	}

	Page->SubTitle = QFormatStr(LOAD_STRING(IDS_INSUFFICIENT_AMOUNT), Page->SubTitle.c_str());
	Page->Refresh();
	return true;
}

void CShutdownWizard::PageEnter(CWizardPage *WizardPage)
{
	TQErrCode Err = Q_NO_ERROR;
	ShutdownWizard::TWizardPagesIndex PageIndex = (ShutdownWizard::TWizardPagesIndex)WizardPage->GetPageNumber();
	try
	{
		switch(PageIndex)
		{
			// todo -oArcady.Volman@objet.com -cNone: checkif all exceptions that should be caught are caught in the entire wizard
			case ShutdownWizard::shdWelcomePage:
				if (false == m_ParamMgr->SHD_RunUnattended)
					break;
				GotoNextPageAsync();
				return;

			case ShutdownWizard::shdHoming:
				setMachineCleaningStartPage();
				break;

			case ShutdownWizard::shdShortWizardInProgress:
				ShortWizardInProgressPageEnter(WizardPage);
				break;

			case ShutdownWizard::shdInsertCleanser:  // After priming or a machine with one model and one support
				InsertCleanserPageEnter(WizardPage);
				break;

			case ShutdownWizard::shdNoticePage:
				if (!NoticePagePageEnter(WizardPage))
					return;
				break;

			case ShutdownWizard::shdLongWizardInProgress:
				if (!LongWizardInProgressPageEnter(WizardPage))
					return;
				//performe block washing only after all cartridges are done
				if ( !ValidateNextLoopCurrentTank() )
				{
					TRY_FUNCTION(Err, m_BackEnd->GotoPurgePosition(true, false));
					HeadsDrainingCycle(WizardPage, m_ParamMgr->SHD_BlockDrainingTimeInSec); // Drain the block after pipes washing before starting block washing - to make sure the block is fully filled SHD_BlockWashingCycles times
					SetOperationMode(omHigh); // Wash the block with high thermistors
					m_BackEnd->UpdateChambers(false);
					BlockWashingCycles(WizardPage); // number of cycles is according to SHD_BlockWashingCycles parameter
				}
				GotoNextPage(false);
				break;

			case ShutdownWizard::shdRemoveCleanser:
				RemoveCleanserPageEnter(WizardPage);
				break;

			case ShutdownWizard::shdBlockDraining: //also drains pipes
				BlockDrainingPageEnter(WizardPage);
				break;

			case ShutdownWizard::shdWizardCompleted:
				WizardCompletedPageEnter(WizardPage);
				break;

			case ShutdownWizard::shdReplaceCartridge:
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
		GotoPage(ShutdownWizard::shdReplaceCartridge);
		return;
	}
	catch(...)
	{
		m_PumpControllers.suspendAllPumpControllers();
		throw;
	}

	if (true == m_ParamMgr->SHD_RunUnattended)
	{
		YieldAndSleepWizardThread(2 * 1000);
		if(PageIndex != ShutdownWizard::shdWizardCompleted &&  
				PageIndex != ShutdownWizard::shdShortWizardInProgress &&
				PageIndex != ShutdownWizard::shdHoming /*&&
		/*PageIndex != ShutdownWizard::shdPagesCount */  )
		GotoNextPage(true );

	}

} //PageEnter

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CShutdownWizard::setMachineCleaningStartPage()
{
	if (ShutdownWizard::LONG_MODE_OPTION == m_WizardModeOption)
		SetNextPage(ShutdownWizard::shdRemoveResinCartridges);
	else if (ShutdownWizard::SHORT_MODE_OPTION == m_WizardModeOption)
		SetNextPage(ShutdownWizard::shdShortWizardInProgress);
	else
	{
		assert(false);
		throw EQException("Unexpected wizard option");
	}
}

void CShutdownWizard::SelectWizardModePageLeave(CWizardPage *WizardPage)
{
	m_WizardModeOption = dynamic_cast<CRadioGroupWizardPage *>(WizardPage)->GetSelectedOption();
	if (m_WizardModeOption == ShutdownWizard::LONG_MODE_OPTION)
		WriteToLogFile(LOG_TAG_GENERAL,"Long wizard mode selected.");
	else if (m_WizardModeOption == ShutdownWizard::SHORT_MODE_OPTION)
		WriteToLogFile(LOG_TAG_GENERAL,"Short wizard mode selected.");
}		

bool CShutdownWizard::InsertCleanserPageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason)
{
#ifdef _DEBUG
		if(FindWindow(0, "AllTanksOK.txt - Notepad"))
			return true;
#endif
		
	CCheckBoxWizardPage *Page = dynamic_cast<CCheckBoxWizardPage *>(WizardPage);

	if (LeaveReason == lrCanceled)
	{
		Page->SubTitle = "";
		return false;
	}
	
	for (TTankIndexVectorIter i = m_RequiredTanks.begin(); i != m_RequiredTanks.end(); i++)
	{
		QString requiredMaterial = (IS_MODEL_TANK(*i) ? MODEL_CLEANSER : SUPPORT_CLEANSER);
		// Check if resin types matches the requested.
		if (CHECK_NOT_EMULATION(m_ParamMgr->OCB_Emulation) && m_BackEnd->GetTankMaterialType(*i) != requiredMaterial)
		{
			QString str = QFormatStr(LOAD_STRING(IDS_RESIN_TYPE_SHOULD_BE_DIFFERENT),TankToStr(*i).c_str(),
									 m_BackEnd->GetTankMaterialType(*i).c_str(),requiredMaterial.c_str());
            WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("%s", str.c_str()));
			Page->SubTitle = str;
			SetNextPage(GetCurrentPage());
			return false;
		}
	}
	// Check if tanks are not empty
	for (TTankIndexVectorIter i = m_RequiredTanks.begin(); i != m_RequiredTanks.end(); i++)
	{
		if (CHECK_NOT_EMULATION(m_ParamMgr->OCB_Emulation) && (m_BackEnd->GetTankWeightInGram(*i) < m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(*i)]))
		{
			QString str = QFormatStr("The current %s cleanser level is less than required by the wizard.",
									 ChamberToStr(TankToStaticChamber(*i), true).c_str());
            WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("%s", str.c_str()));
			Page->SubTitle = str;
			SetNextPage(GetCurrentPage());
			return false;
		}
	}

	// reset subtitles for next wizard run
	if (GetNextPage() != GetCurrentPage())
		Page->SubTitle = "";

	return true;
}

void CShutdownWizard::ReplaceCartridgePageLeave(CWizardPage *WizardPage)
{
	CCheckBoxWizardPage *Page = dynamic_cast<CCheckBoxWizardPage *>(WizardPage);
	if (CHECK_NOT_EMULATION(m_ParamMgr->OCB_Emulation) && ! m_BackEnd->IsRelevantTankInserted(m_TankToReplace))
	{
		Page->SubTitle = "Load " + TankToStr(m_TankToReplace) + " cleanser cartridge.";
		SetNextPage(GetCurrentPage());
		return;
	}

	if (CHECK_NOT_EMULATION(m_ParamMgr->OCB_Emulation) && (m_BackEnd->GetTankWeightInGram(m_TankToReplace) <
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
	ShutdownWizard::TWizardPagesIndex PageIndex = (ShutdownWizard::TWizardPagesIndex)WizardPage->GetPageNumber();
	if (LeaveReason == lrCanceled)
	{
		switch(PageIndex)
		{
			case ShutdownWizard::shdInsertCleanser:
				InsertCleanserPageLeave(WizardPage, LeaveReason);
				break;			
		}
	}

	if (LeaveReason != lrGoNext)
		return;

	switch(PageIndex)
	{
		case ShutdownWizard::shdSelectWizardMode:
			SelectWizardModePageLeave(WizardPage);
			break;

		case ShutdownWizard::shdRemoveResinCartridges:
			//Avi T. request - remove check - RemoveCartridgesPageLeave(WizardPage);
			break;

		case ShutdownWizard::shdInsertCleanser:
			if (InsertCleanserPageLeave(WizardPage, LeaveReason) == false)
				return;
			break;

		case ShutdownWizard::shdRemoveCleanser:
			RemoveCartridgesPageLeave(WizardPage);
			break;	

		case ShutdownWizard::shdReplaceCartridge:
			ReplaceCartridgePageLeave(WizardPage);
			break;
	}
} // PageLeave


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CShutdownWizard::HeadsHeatingCycle()
{
	// Resetting the modes
	m_BackEnd->GotoDefaultMode();
	// Entering 'High Quality' Mode
	m_BackEnd->EnterMode(PRINT_MODE[HIGH_TEMPATURE_INDEX][FIRST_OPERATION_MODE],
	MACHINE_QUALITY_MODES_DIR(HIGH_TEMPATURE_INDEX,FIRST_OPERATION_MODE));
	/* FIXME REMOVE
	CScopeRestorePoint<int> sc(m_ParamMgr->HeatersTemperatureArray[PRE_HEATER]);
	m_ParamMgr->HeatersTemperatureArray[PRE_HEATER] = ShutdownWizard::PRE_HEATER_TEMP_A2D;
	*/
	CQ2RTAutoWizard::HeadsHeatingCycle();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CShutdownWizard::WashingCycle()
{
	TQErrCode Err = Q_NO_ERROR;
	m_PumpControllers.clearControllerOperationDoneFlag();
	TRY_FUNCTION(Err, m_BackEnd->GotoPurgePosition(true, false));
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
			throw ETimeout("waiting for the block to become not full during pipes rinsing");
		}


		YieldAndSleepWizardThread();
	}
	
	av.close();
	if (m_PumpControllers.isControllerOperationDone())
	{
		// _requiredNetPumpingMilliseconds = 0;  // todo: shahar: check is needed.
	}
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CShutdownWizard::BlockWashingCycles(CWizardPage *WizardPage)
{
	m_IsWashingBlock = true;
	WriteToLogFile(LOG_TAG_GENERAL,"Starting block washing cycles.");

	TQErrCode Err = Q_NO_ERROR;
	TRY_FUNCTION(Err, m_BackEnd->GotoPurgePosition(true, false));

	for (int i = m_CurrentBlockWashingCycle; i < m_ParamMgr->SHD_BlockWashingCycles; i++)
	{
		if (IsCancelled()) 
			throw CQ2RTAutoWizardCancelledException();
		m_CurrentBlockWashingCycle = i;
		
		WriteToLogFile(LOG_TAG_GENERAL, QFormatStr("Block washing cycle #%d.", i+1));
		
		if (m_PumpControllers.isPumpsVectorEmpty())
			createPumpControllers(m_RequiredTanks, ResinFlowUtils::BlockFilling); // todo -oArcady.Volman@objet.com -cNone: consider to add the substring time to the time related function names
		// use pump controllers to fill block
		m_PumpControllers.clearControllerOperationDoneFlag();
		while (!m_PumpControllers.isControllerOperationDone() && !IsCancelled())
		{
			try
			{
				m_PumpControllers.controlAllPumps();
			}
			catch(const ETimeout &e) // translate exception message to user readable message
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
		av.holdOpen(1000 * m_ParamMgr->SHD_BlockDrainingTimeInSec);		
	}
	m_IsWashingBlock = false;

	WriteToLogFile(LOG_TAG_GENERAL,"Block washing cycles completed.");

	m_BackEnd->WipeHeads(false);
	m_BackEnd->WipeHeads(false);
}


void CShutdownWizard::RemoveCleanserPageEnter(CWizardPage *WizardPage)
{
	CCheckBoxWizardPage *Page = dynamic_cast<CCheckBoxWizardPage *>(WizardPage);
	Page->Strings.Clear();
	for (TTankIndexVectorIter i = m_RequiredTanks.begin(); i != m_RequiredTanks.end(); i++)
		Page->Strings.Add(QFormatStr("Cartridge removed from '%s' compartment.", TankToStr(*i).c_str()));
	Page->Refresh();
	// Initilize members involved in cartridge replacement during Washing Cycle for the next cycle
	m_TankToReplace = NO_TANK;

	PromoteCurrentTank();
	SetNextPage(ShutdownWizard::shdInsertCleanser);
}

void CShutdownWizard::RemoveCartridgesPageLeave(CWizardPage *WizardPage)
{
	if ( ValidateCurrentTank() ) //current tank is already incremented
		return;

	// In case of OCB Emulation - continue
	// Verify that all cartridges were removed
	for (int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
	{
		if (CHECK_EMULATION(m_ParamMgr->OCB_Emulation))
			break;
		if (m_BackEnd->IsRelevantTankInserted(i))
		{
			SetNextPage(GetCurrentPage());
			return;
		}
	}
}

CShutdownWizard::~CShutdownWizard()
{}

void CShutdownWizard::GotoNextPage(bool CheckDoGotoNextPage)
{
	bool DoGotoNextPage = true;
	if (true == m_ParamMgr->SHD_RunUnattended)
		DoGotoNextPage = CheckDoGotoNextPage;
	if (DoGotoNextPage)
		CQ2RTAutoWizard::GotoNextPage();
}

// Close the wizard session event
void CShutdownWizard::CloseEvent()
{
	DisableServiceMaterials();
	if (m_ParamMgr->SHD_RunUnattended )
	{
		m_ParamMgr->SHD_RunUnattended = false;
		m_ParamMgr->SaveSingleParameter(&m_ParamMgr->SHD_RunUnattended);
		m_BackEnd->TurnPower(false);
		Q2RTApplication->TerminateAndShutdown();

	}
}

bool CShutdownWizard::IsCurrentTankReset()
{
	return (m_CurrentTank == 0);
}

void CShutdownWizard::ResetCurrentTank()
{
	m_CurrentTank = 0;
}

void CShutdownWizard::PromoteCurrentTank()
{
	m_CurrentTank++;
}

TTankIndex CShutdownWizard::ModelCurrentTank()
{
	return (TTankIndex)(m_CurrentTank + FIRST_MODEL_TANK_TYPE);
}

TTankIndex CShutdownWizard::SupportCurrentTank()
{
	return (TTankIndex)(m_CurrentTank + FIRST_SUPPORT_TANK_TYPE);
}

bool CShutdownWizard::ValidateModelCurrentTank()
{
	TTankIndex ModelIndex   = ModelCurrentTank();
	return ( VALIDATE_TANK(ModelIndex) && IS_MODEL_TANK(ModelIndex) );
}

bool CShutdownWizard::ValidateSupportCurrentTank()
{
	TTankIndex SupportIndex = SupportCurrentTank();
	return ( VALIDATE_TANK(SupportIndex) && IS_SUPPORT_TANK(SupportIndex));
}

bool CShutdownWizard::ValidateCurrentTank()
{
	if (!ValidateModelCurrentTank())
		return false; 
	return ValidateSupportCurrentTank();
}

bool CShutdownWizard::ValidateNextLoopCurrentTank()
{
	m_CurrentTank++;
	bool ret = ValidateCurrentTank();
	m_CurrentTank--;
	return ret;
}

void CShutdownWizard::SetOperationMode(TThermOpMode om)
{
	TThermistorsOperationMode Thermistors;
	TTankOperationMode        Tanks;

	switch(om)
	{
		case omLow:
			Thermistors = LOW_THERMISTORS_MODE;
			Tanks       = S_M1_M2_M3_ACTIVE_TANKS_MODE;
			break;
		case omHigh:
			Thermistors = HIGH_THERMISTORS_MODE;
			Tanks       = S_M1_ACTIVE_TANKS_MODE;
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

	for(int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; i++)
	{
		if(IS_MODEL_TANK(i))
		{
			m_ParamMgr->TypesArrayPerTank[i] = MODEL_CLEANSER;
			m_ParamMgr->TypesArrayPerPipe[i] = MODEL_CLEANSER;
			for(int Segment = NUMBER_OF_SEGMENTS-1; Segment >= FIRST_SEGMENT; Segment--)
				if (IsSegmentRelatedToTank(static_cast<TSegmentIndex>(Segment), static_cast<TTankIndex>(i)))
				   m_ParamMgr->MRW_TypesArrayPerSegment[Segment] = MODEL_CLEANSER;
		}
		else
		{
			m_ParamMgr->TypesArrayPerTank[i] = SUPPORT_CLEANSER;
			m_ParamMgr->TypesArrayPerPipe[i] = SUPPORT_CLEANSER;
			for(int Segment = NUMBER_OF_SEGMENTS-1; Segment >= FIRST_SEGMENT; Segment--)
				if (IsSegmentRelatedToTank(static_cast<TSegmentIndex>(Segment), static_cast<TTankIndex>(i)))
				   m_ParamMgr->MRW_TypesArrayPerSegment[Segment] = SUPPORT_CLEANSER;
		}

		m_BackEnd->UpdateTanks(static_cast<TTankIndex>(i));	
	}

	for(int i = FIRST_MODEL_CHAMBER_INDEX; i < LAST_MODEL_CHAMBER_INDEX; i++)
		m_ParamMgr->TypesArrayPerChamber[i] = MODEL_CLEANSER;
	for(int i = FIRST_SUPPORT_CHAMBER_INDEX; i < LAST_SUPPORT_CHAMBER_INDEX; i++)
		m_ParamMgr->TypesArrayPerChamber[i] = SUPPORT_CLEANSER;

	m_ParamMgr->SaveSingleParameter(&m_ParamMgr->TypesArrayPerTank);
	m_ParamMgr->SaveSingleParameter(&m_ParamMgr->TypesArrayPerPipe);
	m_ParamMgr->SaveSingleParameter(&m_ParamMgr->TypesArrayPerChamber);
	m_ParamMgr->SaveSingleParameter(&m_ParamMgr->MRW_TypesArrayPerSegment);
	m_BackEnd->GotoDefaultMode();

	for(int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; i++)
	{
		while (false == m_BackEnd->WaitForIdentificationCompletion(static_cast<TTankIndex>(i),true, /*true = DontWaitIfTankIsOut*/ 100   /*time to wait*/))
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
	FrontEndInterface->UpdateStatus(FE_REFRESH_MODES_DISPLAY,0,true);
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
	for (TPumpIndicesVector::iterator itrPumpIndx =	v.begin(); itrPumpIndx != v.end(); itrPumpIndx++)
	{// todo -oArcady.Volman@objet.com -cNone: make sure to reset the controllers when required
		switch (mode)
		{
			case PipesPriming:            
				WriteToLogFile(LOG_TAG_GENERAL,"PipesPriming");
				requiredNetPumpingMilliseconds =
				static_cast<TQMilliseconds>( m_ParamMgr->SHD_CleanserPrimingTimeInSec[PumpIndexByResinTypeIndex(*itrPumpIndx)] * 1000);
				break;

			case PipesWashing:
				WriteToLogFile(LOG_TAG_GENERAL,"PipesWashing");
				requiredNetPumpingMilliseconds =
				static_cast<TQMilliseconds>(m_ParamMgr->SHD_CleanserWashingTimeInSec[PumpIndexByResinTypeIndex(*itrPumpIndx)] * 1000);
				break;

			case BlockFilling:
				WriteToLogFile(LOG_TAG_GENERAL,"BlockFilling");
				requiredNetPumpingMilliseconds = 0;  // Ignored for BlockFillingPumpController.
				timeout = m_ParamMgr->SHD_FillingTimeoutInSec;
				break;

			case PipesEmptying: // todo -oArcady.Volman@objet.com -cNone: rename to pipes draining?
				WriteToLogFile(LOG_TAG_GENERAL,"PipesEmptying");
				requiredNetPumpingMilliseconds =
				static_cast<TQMilliseconds>(m_ParamMgr->SHD_PipesClearingTimeInSec * 1000);
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
		m_ParamMgr->SHD_PumpsTimeDutyOn_ms,
		m_ParamMgr->SHD_PumpsTimeDutyOff_ms,
		Attributes,
		( (IS_MODEL_TANK(tankIndexByPumpIndex(*itrPumpIndx))) ? MODEL_CLEANSER : SUPPORT_CLEANSER));

	}
	// =======================================================
}
