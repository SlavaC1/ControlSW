/********************************************************************
 * Module: Resin Replacement                                        *
 * Module Description: Resin replacement wizard.                    *
 *                     A new look and feel, built for ObjetFamily   *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Elad Atar                                                *
 * Module start-date: 13/12/2011                                    *
 ********************************************************************/
#include <assert.h>
#include "ResinFlowUtils.h"
#include "ResinReplacementWizard.h"
#include "GlobalDefs.h"
#include "Q2RTWizard.h"
#include "QErrors.h"
#include <algorithm>  //sorting a vector; min/max in std::list

#include "CustomResinMainSelectWizardFrame.h"
#include "CustomResinSelectAdvancedFrame.h"
#include "MaterialReplacementDonePage.h"
#include "MaterialReplacementDoneFrame.h"

#include "MachineSequencer.h"
#include "QMonitor.h"
#include "WizardImages.h"
#include "TankIdentificationNotice.h"
#include "QThreadUtils.h"
#include "QScopeRestorePoint.h"
#include "MRWDefs.h"
#include "Q2RTApplication.h"
#include "MaintenanceCounters.h"
#include "Q2RTApplication.h"

//extern const int PumpsCounterID[NUMBER_OF_PUMPS];

using namespace MrwPipeline;

#define SECONDS_IN_ONE_HOUR 3600
#define MIN_NUM_RESINS 3 /* floor(#model_chambers / #tank_redundancy_per_chamber) = 7/2 */

/////////// for SetParametersForResinReplacement from machine manager ///
//#include "Q2RTApplication.h"
//#include "MachineManager.h"
/////////////////////////////////////////////////////////////

//TODO (Elad): fix potential minor bug: support resins list contains a blank item, the support resins and the matching cleansers. The condition should be only for the number of the support resins themselves.
#define COND_MULTIPLE_CHOICE_SUPPORT_RESINS (Page->SupportResinsList.size() > 3)
class EMRWLeavingSummaryPageException : public EQException
{
  public:
	EMRWLeavingSummaryPageException() : EQException("",0){}
};


#define GET_DEFAULT_IMAGE -1
#define NON_PAGE_ID -1
#define NO_TANK_INSERTED "N\\A"
#define SEC_TO_MS(seconds) (1000 * (seconds)) //not like QSecondsToTicks()

T_AxesTable AxesTable = {true, true, false, true};

//void ExternalAppExecute(const char *FileName,const char *CmdLineParam);
   enum {
		rrWelcomePage = 0,
		rrCloseDoor,
		rrChooseResinsPage,
		rrSummaryPage,
		rrAdvancedSettings,
		rrWizardCompleted
	  };


CResinReplacementWizard::CResinReplacementWizard() :
  CQ2RTAutoWizard(IDS_RR_WIZARD, true, RESIN_REPLACEMENT_MAIN_IMAGE, false,false, "",IDS_ADVANCED_SETTINGS_BTN),
  m_PumpControllers(this), m_IsWithinCurrSessionCriticalSection(false), m_IsHswRecovery(false)
{
  memset(m_CalculatedEconomySegments, 0, sizeof(m_CalculatedEconomySegments));
  memset(m_UndefinedSegments, 0, sizeof(m_UndefinedSegments));
  //memset(m_PrevHeatersMaskTable, 0, sizeof(m_PrevHeatersMaskTable));
  m_GoToAdvanced = false ;


  //Define the wizpages
  CMessageWizardPage* WelcomePage = new CMessageWizardPage(GetTitle(), GET_DEFAULT_IMAGE, wpHelpNotVisible); //rrWelcomePage
  WelcomePage->SubTitle = WELCOME_MSG.c_str();
  AddPage(WelcomePage);

  CCloseDoorPage *CloseDoorPage = new CCloseDoorPage(this, PREPARATIONS_IMAGE_ID, wpHelpNotVisible); //rrCloseDoor:
  AddPage(CloseDoorPage);

  //C'tor's size argument must match the corresponding viewer's c'tor's size argument; typically: MAX_NUMBER_OF_MODEL_CHAMBERS.
  CResinMainSelectionWizardPage* MainResinConfigPage = new CResinMainSelectionWizardPage("",MAX_NUMBER_OF_MODEL_CHAMBERS, GET_DEFAULT_IMAGE, wpPreviousDisabled | wpFlipButton2Visible);
  AddPage(MainResinConfigPage); //rrChooseResinsPage
  //We don't want to enforce 'wpNoTimeout' on the resin replacement page, to handle unexpected problems.
  //Be advised that MRW_InProgressPageTimeoutSec should be then set accordingly.
  CResinSelectionSummaryWizardPage* SummaryPage = new CResinSelectionSummaryWizardPage("",INSERT_MODEL_CARTRIDGE_IMAGE, wpNextDisabled);
  AddPage(SummaryPage); //rrSummaryPage

  CResinSelectionAdvancedWizardPage* AdvancedSettingsPage = new CResinSelectionAdvancedWizardPage(IDS_ADVANCED_SETTINGS, QUESTION_IMAGE_ID, wpCancelDisabled | wpIgnoreOnPrevious);
  AddPage(AdvancedSettingsPage); //rrAdvancedSettings

   CMaterialReplacementDonePage* DonePage = new CMaterialReplacementDonePage(WIZARD_COMPLETED_STR,MRW_CUSTOM_SUCCESSFULLY_COMPLETED_IMAGE_ID, wpDonePage | wpHelpNotVisible);
   AddPage(DonePage); //rrWizardCompleted

  //Init variables for the entire duration of the wizard
  m_ParamMgr = CAppParams::Instance();
  //the following is redundant if ReInitPages is called
  int extraSupportChamberIndex = MainResinConfigPage->Size()-1; //Originally: ->Size() if no extra support chamber's available
  for (int i = 0; i < extraSupportChamberIndex; ++i)
  {
	  MainResinConfigPage->ActiveChambers[i] =
		IsChamberNotFloodedByNeighbour(
			(TChamberIndex)(FIRST_MODEL_CHAMBER_INDEX + i));
  }
  //Relevant only if above loop doesn't finish at i == Size()-1
  MainResinConfigPage->ActiveChambers[extraSupportChamberIndex] =
	IsValidPipingToChamberResin((TChamberIndex)(FIRST_MODEL_CHAMBER_INDEX + extraSupportChamberIndex));
  MainResinConfigPage->ActiveSupportChamber =
	IsValidPipingToChamberResin(FIRST_SUPPORT_CHAMBER_INDEX);  
//	IsChamberNotFloodedByNeighbour(FIRST_SUPPORT_CHAMBER_INDEX); //Originally: if only 1 support chamber exists

  MainResinConfigPage->DirtyPrintingPeriod = m_ParamMgr->MRW_DirtyPrintingPeriod.Value();

  m_Pipeline = new CResinReplacementPipeline(this);

/*  m_PumpCounterIDList.clear();
  for (int i = FIRST_PUMP; i < NUMBER_OF_PUMPS; ++i)
	m_PumpCounterIDList.push_back(PumpsCounterID[i]);*/

  //not needed here, but sets a default value instead of an unpredicted time
  m_OptimizedEmptyBlockTime = m_ParamMgr->MRW_EmptyBlockTime.Value();

  m_LeavingSummaryPage = false;
  m_UndefinedStr = LOAD_STRING(IDS_MRW_UNDEFINED);
  m_bNeedRestartAfterParamsChanged = false;
  EnableHistoryFile(); // Enable history file creation
  EchoLogToHistory();  // Echo all wizards' log messages to history file

  m_SupportCleanserName = GetSupportCleanserMaterial();
  if (strcmp("" ,m_SupportCleanserName.c_str()) == 0) {
    //A default name. If this appears, we know there's a problem.
    CQLog::Write(LOG_TAG_GENERAL,
      "Error: MRW's Pipline - couldn't get the Support Cleanser's name");
    m_SupportCleanserName = "[ERROR]";
  }
}

CResinReplacementWizard::~CResinReplacementWizard()
{
	Q_SAFE_DELETE(m_Pipeline);
}

QString CResinReplacementWizard::GetSupportCleanserMaterial() const
{
	m_BackEnd->BeginModesEnumeration();
	QString modeName;
	QString result = "";
	while ((modeName = m_BackEnd->GetNextModeName(SUPPORT_SERVICE_MATERIALS_MODES_DIR)) != "")
	{
		// Check if the mode file is not hidden
		if ((false == IS_MODE_HIDDEN(modeName)) && m_BackEnd->IsMaterialLicensed(modeName)
			&& m_BackEnd->IsServiceSupportMaterial(modeName))
		{
		  result = modeName;
		  break;
		}
	}
	return result;
}

 // Start the wizard session event
void CResinReplacementWizard::StartEvent()
{
  GetPageById(rrSummaryPage)->SetTimeout(m_ParamMgr->MRW_InProgressPageTimeoutSec * 1000);

   m_bNeedRestartAfterParamsChanged = false;
  // Disable vacuum errors
  m_BackEnd->EnableDisableVacuumErrorHandling(false);

  // Reseting the modes to make sure that we update heaters mask in default mode
  m_BackEnd->GotoDefaultMode();

  // Ensure that the Air Valve is closed
  ResinFlowUtils::AirValve av(this);
  av.close();

  m_BackEnd->ActivateWaste(true);

  // Turn Off Block Filling.
  m_BackEnd->TurnHeadFilling(false);

  // Start heads heating here to save time. (turned off by Q2RTCleanUp())
  StartAsyncHeatingCycle(TOTAL_NUMBER_OF_HEATERS, true);

  LogBlockStatus();
  LogSegments();

  // Inform the Containers Module to ignore resin-type mismatches upon container insertion events. (that may occur during the Wizard)
  // And hide any open TTankIDNotice dialog - if currently open.
  for (int i = LAST_TANK_TYPE-1; i >= FIRST_TANK_TYPE; --i)
  {
     m_BackEnd->SkipMachineResinTypeChecking(static_cast<TTankIndex>(i), true);
     TTankIDNotice::HideDialog(static_cast<TTankIndex>(i));
  }

  m_GoToAdvanced = false;
  BackupAllOpModes(); //back up PM parameters 

  ReInitPages(); //re-initialize pages' settings
  m_Pipeline->Initialize(); //re-initialize the pipeline; get current pipings
  m_WeightSampling.Initialize(
	(mtObjet1000 == Q2RTApplication->GetMachineType()),
	m_ParamMgr->MRW_WeightStabilizer_SamplingWndSz,
	m_ParamMgr->MRW_WeightStabilizer_StableThreshold,
	m_ParamMgr->MRW_WeightStabilizer_SamplingTimeout
  );
  m_LeavingSummaryPage = false;
  m_IsWithinCurrSessionCriticalSection = false;
  
  m_IsHswRecovery = false;

  m_Progress.CurrentProgress       = 0;
  m_Progress.CombinedCycles        = 0;
  m_Progress.PerformedSections     = 0;
  m_Progress.OverallTimeEstimation = 0;
  m_Progress.OverallRemainingTime  = 0;

  //Save Chamber's status before MRW
  m_ChamberStateBeforeMRW.Clear();
  for(int i = 0; i < NUMBER_OF_CHAMBERS_INCLUDING_WASTE; i++)
	m_ChamberStateBeforeMRW.Add(m_ParamMgr->TypesArrayPerChamber[i].Value());

}

void CResinReplacementWizard::ReInitPages()
{
	//Shut-off the 2nd flushing feature; Clean target materials;
	CResinMainSelectionWizardPage* page1 =
		dynamic_cast<CResinMainSelectionWizardPage *>(Pages[rrChooseResinsPage]);
	if (page1)
	{
		page1->Setup();
		/*  Originally (in case of 1 support chamber only):
		for (int i = 0; i < page1->Size(); ++i)
		{
			  page1->ActiveChambers[i] = IsChamberNotFloodedByNeighbour(
				(TChamberIndex)(FIRST_MODEL_CHAMBER_INDEX + i));
		}
		page1->ActiveSupportChamber =
			IsChamberNotFloodedByNeighbour(FIRST_SUPPORT_CHAMBER_INDEX);
		*/
		int extraSupportChamberIndex = page1->Size()-1;
		for (int i = 0; i < extraSupportChamberIndex; ++i)
		  {
			  page1->ActiveChambers[i] =
				IsChamberNotFloodedByNeighbour(
					(TChamberIndex)(FIRST_MODEL_CHAMBER_INDEX + i));
		  }
		  page1->ActiveChambers[extraSupportChamberIndex] =
			IsValidPipingToChamberResin((TChamberIndex)(FIRST_MODEL_CHAMBER_INDEX + extraSupportChamberIndex));
		  page1->ActiveSupportChamber =
			IsValidPipingToChamberResin(FIRST_SUPPORT_CHAMBER_INDEX);
		//=============

		page1->DirtyPrintingPeriod = m_ParamMgr->MRW_DirtyPrintingPeriod.Value();
	}

	//Init. advanced features' selections
	CResinSelectionAdvancedWizardPage* page2 =
		dynamic_cast<CResinSelectionAdvancedWizardPage *>(Pages[rrAdvancedSettings]);
	if (page2) page2->Setup();

	CResinSelectionSummaryWizardPage *page3 =
		dynamic_cast<CResinSelectionSummaryWizardPage *>(Pages[rrSummaryPage]);
    if (page3) page3->Setup();
}

void CResinReplacementWizard::EndEvent()
{
  CleanUp(); //deleting pumpControllers
}

void CResinReplacementWizard::CancelEvent(CWizardPage *WizardPage)
{
 m_BackEnd->SetActuatorState(ACTUATOR_ID_AIR_VALVE, ACTUATOR_OFF);
  WriteToLogFile(LOG_TAG_GENERAL,"Wiping");
  m_BackEnd->GotoPurgePosition(/* blocking */ true, /* perform home */ true);
  m_BackEnd->WipeHeads(false);
  WriteToLogFile(LOG_TAG_GENERAL,"Finished Wiping");
  //If we have started pumping, do not restore the target operation-mode-related params
  if (!m_IsWithinCurrSessionCriticalSection) {
	RestoreAllOpModes();
  }
  CleanUp(); //deleting pumpControllers
}

void CResinReplacementWizard::CloseEvent()
{

//  m_BackEnd->PauseOCBStatusThread(); //Pauses the OCBStatus thread at an unknown point (unlike PausePoint).
										//Notifications FROM OCB might fail during EdenProtocolClient's operation.

  // Updating Containers Module with the new resins.
  UpdateAllTanks();

  // Fixing inconsistency in parameter manager and Tank <-> Chamber relations, allowing further calls to TankToChamber
  // UpdateChambers() when passed 'false', expects that m_ParamMgr->ThermistorsOperationMode is already updated and correct.
  m_BackEnd->UpdateChambers(false);

//  m_BackEnd->ResumeOCBStatusThread();
    for(int i = LAST_TANK_TYPE-1; i >= FIRST_TANK_TYPE; i--)
  {
	 m_BackEnd->SkipMachineResinTypeChecking(static_cast<TTankIndex>(i), false);
	 TTankIDNotice::HideDialog(static_cast<TTankIndex>(i));
  }
	// fireall
/*	CMachineSequencer *Sequencer = Q2RTApplication->GetMachineManager()->GetMachineSequencer();
	Sequencer->FireAllSequence();
	WriteToLogFile(LOG_TAG_GENERAL,"Finished FireAll");      */
    CheckAndShowHowMessage();
}

void CResinReplacementWizard::CleanUp()
{
  m_BackEnd->TurnHeadFilling(false);

  m_BackEnd->SetActuatorState(ACTUATOR_ID_AIR_VALVE, ACTUATOR_OFF);
  m_BackEnd->ActivateWaste(false);
  m_BackEnd->SetHoodFanIdle();

  m_PumpControllers.deletePumpControllers(); // closes any active pumps - if there were.
  m_CurrentPumps.clear();

  // Close wiper door
  m_BackEnd->GoToMotorHomePosition(AXIS_T,true);
  m_BackEnd->GoToMotorHomePosition(AXIS_X,true);
  m_BackEnd->GoToMotorHomePosition(AXIS_Y,true);

  // Disabling All Axes
  m_BackEnd->EnableMotor(false, AXIS_ALL);

  LogSegments();

  // Enable vacuum errors
  m_BackEnd->EnableDisableVacuumErrorHandling(true);

  // Unlock the Door
  m_BackEnd->LockDoor(false);
  while(m_BackEnd->IsDoorLocked())
  {
	YieldAndSleepWizardThread();
	if (IsCancelled()) return;
  }

  WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Performed %d overall washing cycles during %d sections.",m_Progress.CombinedCycles,
  																									   m_Progress.PerformedSections));
  m_WeightSampling.ClearSamples();																									   
}

void CResinReplacementWizard::HelpEvent(CWizardPage *WizardPage)
{ //TODO (Elad): contact zecharia, check contexts 20,40,60 for material not-related to Keshet.
  //Trunk\ObjetFamily\Objet500\Runtime\Help\Material Replacement Wizard\Material Replacement Wizard.chm
	Application->HelpFile = DefaultHelpFileName().c_str();
	switch(WizardPage->GetPageNumber())
	{
		case rrChooseResinsPage: Application->HelpContext(20); break;
		case rrAdvancedSettings: Application->HelpContext(40); break;
		case rrSummaryPage: 	 Application->HelpContext(60); break;
        default: break;    //no help for other pages
	}
}

/************************  Advanced Settings  *************************/
void CResinReplacementWizard::UserButton2Event(CWizardPage *WizardPage)
/**********************************************************************/
{
	if (WizardPage->GetPageNumber() == rrChooseResinsPage)
	{
        m_GoToAdvanced = true;
		GotoPage(rrAdvancedSettings);
		return;
	}
}


void CResinReplacementWizard::PageEnter(CWizardPage *WizardPage)
{
	int PageIndex = WizardPage->GetPageNumber();

	switch(PageIndex)
	{
		case rrWelcomePage:
		{
			//considering heating up the heaters
			break;
		}

		case rrChooseResinsPage:
		{
			SetResinsChoices(WizardPage);
			FetchMachineCurrentState(WizardPage);
			GetCurrentPage()->Refresh();
			break;
		}

		case rrSummaryPage:
		{
			m_WeightSampling.ClearSamples();
			SummaryPageEnter(WizardPage);
			m_LeavingSummaryPage = false;
			break;
		}

		case rrAdvancedSettings:
		{
            m_GoToAdvanced = false;
			SetResinsAdvancedChoices(WizardPage);
			break;
		}

		case rrWizardCompleted:
		{
			if(m_bNeedRestartAfterParamsChanged)
			  MaterialReplacementDoneFrame->ShowRestartWarningMessage();

			if(Q2RTApplication->GetSolubleSupportAllowed())
			  SetHowRequiredAfterMRW();
			break;
        }
	}
}

void CResinReplacementWizard::PageLeave(CWizardPage* WizardPage, TWizardPageLeaveReason LeaveReason)
{
    if(LeaveReason == lrCanceled)
		return;

	int CurrentPageNumber = WizardPage->GetPageNumber();
		
	if (LeaveReason == lrGoNext)
	{
		switch(CurrentPageNumber)
		{
			case rrChooseResinsPage:
			{
				if(m_GoToAdvanced)
				{
					break;
				}
				else
				{
                    LogUserSelections(WizardPage);
					if (! ValidateUserSelections(WizardPage))
						SetNextPage(rrChooseResinsPage);
					else if(! PlaceAndCalculateWastePageLeave(WizardPage))
						SetNextPage(rrChooseResinsPage);
					else if (! ValidateUserResinSelections(WizardPage))
						SetNextPage(rrChooseResinsPage);
					else
						SetNextPage(rrSummaryPage);
				}
				break;
			}

			case rrSummaryPage:
			{
				m_LeavingSummaryPage = true;
				SetNextPage(rrWizardCompleted);
				break;
			}

			case rrAdvancedSettings:
			{
				//reflect the settings gathered in Advanced mode in the default Settings page
				ReflectAdvancedSettings(WizardPage, Pages[rrChooseResinsPage]);
				SetNextPage(rrChooseResinsPage);
				break;
			}
		}
	}

	if (LeaveReason == lrGoPrevious)
	{
		switch(CurrentPageNumber)
		{
            case rrSummaryPage:
			{
				m_LeavingSummaryPage = true;
				RestoreAllOpModes();

				//if we're in silent mode then economy state was altered automatically to 'Economy'.
				//Hence - restore everything to default.
				RepairUiSelectionsSilentMode(Pages[rrChooseResinsPage]); //ALM defect #2050

				break;
			}
		}
	}
}

void CResinReplacementWizard::RepairUiSelectionsSilentMode(CWizardPage* WizSelectionPage)
{
	CResinMainSelectionWizardPage* destPage = dynamic_cast<CResinMainSelectionWizardPage*>(WizSelectionPage);
	if (destPage->IsSilent)
		destPage->RestoreDefaults = true;
}

void CResinReplacementWizard::LogUserSelections(CWizardPage* WizardPage)
{
	CResinMainSelectionWizardPage *Page = dynamic_cast<CResinMainSelectionWizardPage *>(WizardPage);
	QString str = "";

	str += Page->SupportVirtTargMaterial.SelectedResin + ", ";
	for(int i = Page->Size() -1; i >= 0 ; --i)
		if(Page->VirtTargMaterials[i].SelectedResin.compare("") != 0)
			str += Page->VirtTargMaterials[i].SelectedResin + ", ";

	if(! str.empty())
	{
		DEL_LAST_2_CHARS(str);
		WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("User selected: %s", str.c_str()));
    }

	//Output which chambers are to be 2nd flushed
	for (int i = 0; i < Page->Size(); ++i)
	   if (Page->VirtTargMaterials[i].SecondFlushing)
	   {
		 WriteToLogFile(LOG_TAG_GENERAL, QFormatStr("Chamber %s is set for 2nd-flushing",
		   ChamberToStr(static_cast<TChamberIndex>((int)FIRST_MODEL_CHAMBER_INDEX + i)).c_str()));
	   }
		if (Page->SupportVirtTargMaterial.SecondFlushing)
	{
		 WriteToLogFile(LOG_TAG_GENERAL, QFormatStr("Chamber %s is set for 2nd-flushing",
		   ChamberToStr(TYPE_CHAMBER_SUPPORT).c_str()));
	}
}

bool CResinReplacementWizard::ValidateUserSelections(CWizardPage* WizardPage)
{
	CResinMainSelectionWizardPage *Page = dynamic_cast<CResinMainSelectionWizardPage *>(WizardPage);

	int supIdx = Page->GetIndexExtraSupportChamber(); //Is used to identify Chamber 7 acting as a Support chamber

	bool chkSup1 = Page->SupportVirtTargMaterial.IsValid();
	bool chkSup2 = Page->VirtTargMaterials[supIdx].IsValid();
	//check that at least one Support Resin is chosen (i.e. it is not undefined)
	if (! (chkSup1 && chkSup2) )
	{
		QMonitor.WarningMessage(WARNING_UNDEFINED_SUPPORT.c_str(), ORIGIN_WIZARD_PAGE);
		return false;
	}
	else if ((Page->ForceFlushingEnabled || Page->ManualMode) && chkSup1 && chkSup2 &&
		(Page->VirtTargMaterials[supIdx].SelectedResin.compare(
			Page->SupportVirtTargMaterial.SelectedResin) != 0)) {
			//We've encountered 2 different support resins - do not continue
			QMonitor.WarningMessage(WARNING_MULTIPLE_SUPPORT_RESINS.c_str(), ORIGIN_WIZARD_PAGE);
			return false;
		 }

	
	if (Page->ForceFlushingEnabled)
	{
		//User should not use the 2nd flushing feature prior to completing a M.R. successfully.
		//Otherwise, the pipeline might reveal a bug, deducing the wrong thermistors op-mode
		//(blank user-selections are ignored but can come from either flooding or "undefined" resins)
		if (true == m_ParamMgr->MRW_WithinWizard)
		{
			QMonitor.WarningMessage(WARNING_UNDEFINED_2ND_FLUSHING.c_str(), ORIGIN_WIZARD_PAGE);
			return false;
		}

		int flushingResins = 0;
			//count number of resin that are NOT 2nd-flushed
		for (int i = 0; i < supIdx; ++i)
		{
			if (Page->VirtTargMaterials[i].SecondFlushing)
			{   //Verify that the selected resin is valid (isn't undefined/cleanser)
				if ((Page->VirtTargMaterials[i].SelectedResin.compare(INVALID_RESIN_NAME) == 0) ||
					m_BackEnd->IsServiceModelMaterial(Page->VirtTargMaterials[i].SelectedResin))
						return false;
			}
			else
			{
				++flushingResins;
			}
		}
		//do the same for the extra support chamber (Chamber #7 in DM6 mode)
			if (Page->VirtTargMaterials[supIdx].SecondFlushing)
			{   //Verify that the selected resin is valid (isn't undefined/cleanser)
				if ((Page->VirtTargMaterials[supIdx].SelectedResin.compare(INVALID_RESIN_NAME) == 0) ||
					m_BackEnd->IsServiceSupportMaterial(Page->VirtTargMaterials[supIdx].SelectedResin))
						return false;
			}
			else
				 ++flushingResins;
				 
		//do the same for support(s) chamber(s)
			if (Page->SupportVirtTargMaterial.SecondFlushing)
			{   //Verify that the selected resin is valid (isn't undefined/cleanser)
				if ((Page->SupportVirtTargMaterial.SelectedResin.compare(INVALID_RESIN_NAME) == 0) ||
					m_BackEnd->IsServiceSupportMaterial(Page->SupportVirtTargMaterial.SelectedResin))
					return false;
			}
			else
			{
				++flushingResins;
			}
		//User did not select any material to be flushed again
		if ((int)MAX_NUMBER_OF_SUPPORT_CHAMBERS + Page->Size() == flushingResins)
			return false;
	}
	else
	{
		//Note: The usage of supIdx in the entire block ahead can be replaced with the original Page->Size().

		int Selected = 0;
		
		// Check for 2 materials of the same kind (all resins must be unique except Cleanser)
		for (int i = 0; i < supIdx; ++i)
		{
			if (Page->VirtTargMaterials[i].SelectedResin.compare("") != 0)
			{
				Selected++;
				for (int j = i + 1; j < supIdx; ++j)
				{
					if (Page->VirtTargMaterials[j].SelectedResin.compare("") != 0)
					{
						if (Page->VirtTargMaterials[i].SelectedResin == Page->VirtTargMaterials[j].SelectedResin)
						{
							if(Page->VirtTargMaterials[i].SelectedResin != "M.Cleanser")
							{
							QMonitor.WarningMessage(WARNING_SAME_MATERIAL.c_str(), ORIGIN_WIZARD_PAGE);
							return false;
                            }
						}
					}
				}
			}
		}

		// All model chambers must be in use in Full color mode
		if (Page->PrintingMode == FULL_COLOR_MODE)
		{
			if (Selected != supIdx)
			{
				QMonitor.WarningMessage(
					QFormatStr(WARNING_NUM_MATERIALS.c_str(), supIdx).c_str(),
					ORIGIN_WIZARD_PAGE);
				return false;
			}
		}

		// Check that exactly 3 materials are selected in DM3 mode
		if (Page->PrintingMode == DM3_MODE)
		{
			if (3 != Selected)
			{
				QMonitor.WarningMessage(
					QFormatStr(WARNING_NUM_MATERIALS.c_str(), 3).c_str(),
					ORIGIN_WIZARD_PAGE);
				return false;
			}
		}

		bool wasQuestionAsked = false;
		for(int i = 0; i < supIdx; ++i)
			if(m_BackEnd->IsServiceModelMaterial(Page->VirtTargMaterials[i].SelectedResin))
				if(QMonitor.AskYesNo(WARNING_CLEANSER.c_str()))
				{
					 wasQuestionAsked = true;
					 break;
				}
				else
				{	return false;
				}

		/*
		//the following check isn't needed if we already have to verify that the 2 support resin names are the same (if not empty)
		if((!wasQuestionAsked) && m_BackEnd->IsServiceSupportMaterial(Page->VirtTargMaterials[supIdx].SelectedResin))
		{
			if(QMonitor.AskYesNo(WARNING_CLEANSER.c_str()))
				 wasQuestionAsked = true;
			else
				return false;
		}
		*/

		if((!wasQuestionAsked) && m_BackEnd->IsServiceSupportMaterial(Page->SupportVirtTargMaterial.SelectedResin))
			if(! QMonitor.AskYesNo(WARNING_CLEANSER.c_str()))
				return false;


		//If checked, warn the user about using the silent mode
		if (Page->IsSilent)
			if(! QMonitor.AskYesNo(WARNING_SILENT_MODE.c_str()))
				return false;
	}
	return true;
}

bool CResinReplacementWizard::ValidateUserResinSelections(CWizardPage* WizardPage)
{
	TTankIndex CurrentTank;
	CResinMainSelectionWizardPage *Page = dynamic_cast<CResinMainSelectionWizardPage *>(WizardPage);
	for (TTanksVector::iterator Tank = m_TanksForReplacement.begin(); Tank != m_TanksForReplacement.end(); ++Tank)
	{
		CurrentTank = static_cast<TTankIndex>(Tank->TankIndex);
		if ((CurrentTank > TYPE_TANK_SUPPORT2) && (CurrentTank < SUPPORT_MODEL_TANK_L))
		{
			if ((m_Pipeline->GetNumCycles(m_ParamMgr->TypesArrayPerPipe[CurrentTank],Tank->MaterialName,(TReplacementPurpose)0,(TTubeType)0,0) == ILLEGAL_NUM_OF_CYCLES) ||
				(m_Pipeline->GetNumCycles(m_ParamMgr->MRW_TypesArrayPerSegment[ChamberToLongSegment(TankToStaticChamber(CurrentTank))],Tank->MaterialName,(TReplacementPurpose)0,(TTubeType)0,0) == ILLEGAL_NUM_OF_CYCLES))
			{
				if (Page->ManualMode) //In manual mode the warning notes which resin cannot be replaced with which resin.
				{
					QMonitor.WarningMessage(WARNING_RGD515_FROM + m_ParamMgr->TypesArrayPerPipe[CurrentTank] + WARNING_RGD515_TO + Tank->MaterialName, ORIGIN_WIZARD_PAGE);
				}
				else // Automatic mode: Since the order of resins here might be dynamic unlike a static order in manual mode, the warning message is more generic.
				{
					QMonitor.WarningMessage(WARNING_ALL_AUTO_PERMS_ARE_ILLEGAL, ORIGIN_WIZARD_PAGE);
				}
				return false;
			}
        }
	}
	return true;
}
void CResinReplacementWizard::SetOptimizedDrainingTime(TThermistorsOperationMode val, TTanksIndicesVector& activeTanks)
{
	m_OptimizedEmptyBlockTime = GetOptimizedDrainingTime(val, activeTanks);
}

int CResinReplacementWizard::GetOptimizedDrainingTime(TThermistorsOperationMode val, TTanksIndicesVector& activeTanks)
{
	//The a/v is determined by the non-zero washing-cycles active tanks.
		//Each active tank is then checked for its relevant chamber's high thermistor's state.
		//If at least 1 active tank uses the high thermistor - we return the max. empty block time.

		//[@Assumption] An Observer is set for ThermistorsOperationMode that updates ActiveThermistors param
		CScopeRestorePoint<int> scThermOpMode(m_ParamMgr->ThermistorsOperationMode);
		m_ParamMgr->ThermistorsOperationMode = val;

	for (TTanksIndicesVector::iterator i = activeTanks.begin(); i != activeTanks.end(); ++i)
	{
		////if ((i->MarkedForReplacement) && (i->WashingCycles > 0)) are checked upon inserting a new TTankIndex to activeTanks
		//Look at the relevant high thermistor's state. If it's lit - we're flooding
				if (1 == m_ParamMgr->ActiveThermistors[
			GetTankRelatedHighThermistor(*i)])
					return m_ParamMgr->MRW_EmptyBlockTime.Value();
			}

		return m_ParamMgr->MRW_EmptyBlockMinimumTime.Value();
	}

int CResinReplacementWizard::GetOptimizedDrainingTime(TThermistorsOperationMode val)
{
	//if all the thermistors are low then we can consume less time when emptying the block
	return (LOW_THERMISTORS_MODE == val)
			? m_ParamMgr->MRW_EmptyBlockMinimumTime.Value()
			: m_ParamMgr->MRW_EmptyBlockTime.Value();
}

void CResinReplacementWizard::RestoreAllOpModes()
{
	ApplyThermistorsOpMode(m_CurrentThermistorsOpMode);
	ApplyTanksOpMode(m_CurrentTanksOpMode);
	ApplyPrintingAndPipesOpModes(m_CurrentThermistorsOpMode);
}
void CResinReplacementWizard::BackupAllOpModes()
{
	m_CurrentThermistorsOpMode =
		static_cast<TThermistorsOperationMode>(m_ParamMgr->ThermistorsOperationMode.Value());
	m_CurrentTanksOpMode =
		static_cast<TTankOperationMode>(m_ParamMgr->TanksOperationMode.Value());
}

void CResinReplacementWizard::ApplyThermistorsOpMode(TThermistorsOperationMode val)
{
	m_ParamMgr->ThermistorsOperationMode = val;
	m_ParamMgr->SaveSingleParameter(&m_ParamMgr->ThermistorsOperationMode);
	m_BackEnd->SetDefaultParamsToOCB();
}
void CResinReplacementWizard::ApplyTanksOpMode(TTankOperationMode val)
{
	m_ParamMgr->TanksOperationMode = val;
	m_ParamMgr->SaveSingleParameter(&m_ParamMgr->TanksOperationMode);
}

void CResinReplacementWizard::ApplyPrintingAndPipesOpModes(TThermistorsOperationMode val)
{
	TOperationModeIndex sysOpMode = ((HIGH_THERMISTORS_MODE == val) || (HIGH_AND_LOW_THERMISTORS_MODE == val))
		? SINGLE_MATERIAL_OPERATION_MODE : DIGITAL_MATERIAL_OPERATION_MODE;
	m_ParamMgr->PrintingOperationMode = sysOpMode;
	m_ParamMgr->PipesOperationMode = sysOpMode;
	m_ParamMgr->SaveSingleParameter(&m_ParamMgr->PrintingOperationMode);
	m_ParamMgr->SaveSingleParameter(&m_ParamMgr->PipesOperationMode);
}

void CResinReplacementWizard::LogRequiredWeights()
{
    WriteToLogFile(LOG_TAG_GENERAL,"Required weights for each cartridge:");
	for(TTanksVector::iterator i = m_TanksForReplacement.begin(); i != m_TanksForReplacement.end(); ++i)
		if(i->MarkedForReplacement)
			WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Cartridge: %5s, Weight: %5d gr (%d cycs)",
											TankToStr(static_cast<TTankIndex>(i->TankIndex)).c_str(),
											i->NeededWeight,
											i->WashingCycles));
}

void CResinReplacementWizard::SummaryPageEnter(CWizardPage* WizardPage)
{
	CResinSelectionSummaryWizardPage* Page = dynamic_cast<CResinSelectionSummaryWizardPage *>(WizardPage);

	LogRequiredWeights();

	AssertMandatoryParametersValues();
	CalculateEstimatedTime();

	Page->SetProgressPercentage(0);
	Page->SetEta(QSimpleRound(m_Progress.OverallRemainingTime / 60));

    Page->SetWasteTanksFreeSpace(SUMMARY_WASTE_WEIGHT_OK);

	Page->Refresh(); // Set ETA and zero the progress bar
	Page->SetValidTanks(false);
	Page->SetStartButtonState(false);
	//Provide a bypass for the check if needed
	Page->SetSilentMode(
		dynamic_cast<CResinMainSelectionWizardPage *>(Pages[rrChooseResinsPage])
			->IsSilent);
	try
	{
		// Wait for the user to press the Start button
		while(!CheckTanksValidity(WizardPage) || !Page->CanStartReplacement()) // blocking
		{
			CAutoWizard::YieldAndSleepWizardThread();
			if(IsCancelled()) throw CQ2RTAutoWizardCancelledException();
			if (m_LeavingSummaryPage) return;
		}

		// Start button pressed.
		Page->CancelMessage = WARNING_CANCEL.c_str();
		Page->Refresh(); // To start replacement process
		EnableDisablePrevious(false);
    }
   catch(EMRWLeavingSummaryPageException &err)
   {
   	return; // if Previous button was pressed inside CheckTanksValidity
   }

	//Begin any process that doesn't change the machine's parameters / Printing-Mode / resins
	//User can abort w/o the fear of ruining the printer's block/piping status
	try
	{
		if (! IsWashingComplete(m_TanksForReplacement))
			BeforePerformReplacementCycles(WizardPage);
	}
	catch (EMRWLeavingSummaryPageException &err)
	{
		return;
	}

	// Before starting the actual pumping, "Undefine" all relevant parameters.
	// Thus, if an Exception or wizard cancellation will occur during pumping, the user will be forced to repeat MRW. (Without repeating MRW no Printing can be allowed.)
	MarkOnOffCriticalSection(true); 		// Updating parameter MRW_WithinWizard

	//Apply the target state's parameters
	ApplyThermistorsOpMode		(m_NewThermistorsOpMode);
	ApplyTanksOpMode			(m_NewTanksOpMode);
	ApplyPrintingAndPipesOpModes(m_NewThermistorsOpMode);

	UpdateTypesArrayPerTank(); 				//Amongst others, corrects further calls to TankToChamber

	MarkTypesArrayPerSegmentUndefined();    // Updating parameter TypesArrayPerSegment
	MarkTypesArrayPerChamberUndefined();	// Updating parameter TypesArrayPerChamber
	UpdateTypesArrayPerPipe();				// Updating parameter TypesArrayPerPipe

	if (IsWashingComplete(m_TanksForReplacement)) // Don't perform anything if matrix is zeroed
	{
		//Only update the economy state of the segments
		UpdateEconomySegments();
	} else
	{
		//Updating parameter MRW_IsSegmentEconomy
		UpdateEconomySegments(true);	//Update just before we truly start the physical operation
		try
		{
			PerformReplacementCycles(WizardPage);
		}
		catch(EMRWLeavingSummaryPageException &err)
		{
			return;
		}
		UpdateEconomySegments();
		AfterPerformReplacementCycles(WizardPage); //Purges
	}
 
	// Set the progress at 100% regardless the actual progress. Must be redesigned (removed) at later stage !!
	Page->SetProgressPercentage(100);
	Page->Refresh();

	Page->SetReplacementCompleted(true);
	Page->Refresh(); // to set that replacement was completed

	// After the actual "pumping" is completed, we update all relevant parameters to the names of the newly replaced material. (These parameters were all set previously to "Undefined")
	UpdateTypesArrayPerChamber();    // Updating parameter TypesArrayPerChamber
	UpdateTypesArrayPerSegment();    // Updating parameter TypesArrayPerSegment
	UpdateTypesArrayPerPipe();       // Updating parameter TypesArrayPerPipe
	UpdateHostTypesArrayPerChamber();// Notify host on changes

	// A successful material replacement immitates the similar state as not cancelling Head Service Wizard.
	if (m_ParamMgr->HSW_WithinWizard.Value())
	{
		m_ParamMgr->HSW_WithinWizard = false;
		m_ParamMgr->SaveSingleParameter(&m_ParamMgr->HSW_WithinWizard);
		CQLog::Write(LOG_TAG_GENERAL, "Hsw recovery is complete");
	}

	MarkAllTotalPrintingTime();  //reset mrw's counters
	MarkOnOffCriticalSection(false); // Updating parameter MRW_WithinWizard
	LogSegments();

	GotoNextPage();
}

void CResinReplacementWizard::MarkOnOffCriticalSection(bool isUndefinedState)
{
	 m_ParamMgr->MRW_WithinWizard = isUndefinedState;
	 m_ParamMgr->SaveSingleParameter(&m_ParamMgr->MRW_WithinWizard);
	 m_IsWithinCurrSessionCriticalSection = isUndefinedState;
}

void CResinReplacementWizard::AssertMandatoryParametersValues()
{
	if (CHECK_NOT_EMULATION(m_ParamMgr->OCB_Emulation))
	{
		if (0 == m_ParamMgr->MRW_NumberOfPurgeCycles.Value())
		{
			WriteToLogFile(LOG_TAG_GENERAL,"Error: Wrong parameter - The number of Purge Cycles cannot be zero");
			throw CQ2RTAutoWizardException(MSG_ERR_INCORRECT_PARAM_VALUE);
		}
		if (0 == m_ParamMgr->MRW_EmptyBlockTime.Value())
		{
			WriteToLogFile(LOG_TAG_GENERAL,"Error: Wrong parameter - The block cannot be emptied in 0 seconds");
			throw CQ2RTAutoWizardException(MSG_ERR_INCORRECT_PARAM_VALUE);
		}
		if (0 == m_ParamMgr->MRW_EmptyBlockMinimumTime.Value())
		{
			WriteToLogFile(LOG_TAG_GENERAL,"Error: Wrong parameter - The block cannot be emptied in (minimum) 0 seconds");
			throw CQ2RTAutoWizardException(MSG_ERR_INCORRECT_PARAM_VALUE);
		}
		if ((!m_ParamMgr->SimulatorMode.Value()) &&
		  (0 == m_ParamMgr->MRW_CompensationCyclesThermistorsChange.Value()))
		{
			WriteToLogFile(LOG_TAG_GENERAL,"Warning: compensation is bypassed (cycles = 0)");
			//If we want to disallow performing a QUICK MRW
			//(by zeroing the MRWMATRIX values, and bypassing the compensation)
			//then we must throw an exception here (uncomment next line)
			
			//throw CQ2RTAutoWizardException(MSG_ERR_INCORRECT_PARAM_VALUE);
		}
	}
}

// This method precalculates estimated time for cycles in each section
void CResinReplacementWizard::CalculateEstimatedTime()
{
	//the following times are consideraing the DRAINING time (for a single wash) as well !
	int FloodingTime    = m_ParamMgr->MRW_SingleWashingCycleTimeSecFlooding;
	int NonFloodingTime = m_ParamMgr->MRW_SingleWashingCycleTimeSecNonFlooding;
				 
	//in purges stage, the pump controller fills up the chambers and then
	//the liquid level 'stays around' the thermistor, w/o emptying the chamber(s) completely.
	//1st purge usually takes ~1 minute longer because of the 1st filling. This doesn't appear below.
	float PurgesTime    = m_ParamMgr->MRW_NumberOfPurgeCycles *
		(m_ParamMgr->MRW_DelayBetweenPurges + m_ParamMgr->MRW_PurgeLevelingReadyTimeSec
		+ m_ParamMgr->MotorPostPurgeTimeSec) +
		 m_ParamMgr->MRW_PurgeFirstBlockFillingTimeSec;

	//Use the original thermistors' op-mode to retrieve the correct
	//amount of time to keep the air-valve open (instead of m_OptimizedEmptyBlockTime)
	//this is the 1st(!) drain, prior to washing phase and as such,
	//it may differ from the a/v time inbetween cycles -- m_OptimizedEmptyBlockTime
	int firstAirValveTime = GetOptimizedDrainingTime(m_CurrentThermistorsOpMode);
	if (m_IsHswRecovery)
	{
	   CQLog::Write(LOG_TAG_GENERAL, "Hsw recovery is applied");
	   firstAirValveTime = m_ParamMgr->MRW_EmptyBlockTime.Value(); //use the maximum air valve time possible
	}

	m_Progress.PurgesTime   = PurgesTime;
	m_Progress.DrainingTime = firstAirValveTime;
	m_Progress.ETAEachSection.clear();

	TTanksVector Tanks  = m_TanksForReplacement;                           
	//0-cycles tanks are irrelevent in following algorithms even if they're marked for replacement
	for (TTanksVector::iterator i = Tanks.begin(); i != Tanks.end(); ++i)
	{
		if ((i->MarkedForReplacement) && (i->WashingCycles <= 0))
			i->MarkedForReplacement = false;
	}

    // Mimicing the algorithm in PopulatePumpControllers to calculate num of cycles for each section
	while(! IsWashingComplete(Tanks))
	{
		int Cycles             = INT_MAX;
		float MaxCyclesTimeSec = 0; // Max cycles ETA for current section

		bool redundancySectionsOn = true;
		std::vector<int> ChosenTanks;

		for(TTanksVector::iterator i = Tanks.begin(); i != Tanks.end(); ++i)
		{
			if(i->MarkedForReplacement)
			{
				if(i->PumpingPhase == PHASE_SHORT)
				{
					if(i->WashingCycles > 0)
					{
						redundancySectionsOn = false;
						ChosenTanks.push_back(i->TankIndex);
						if(i->WashingCycles < Cycles)
							Cycles = i->WashingCycles;
					}
					else
					{
						TTankIndex Sibling = GetSiblingTank(static_cast<TTankIndex>(i->TankIndex));
						if(Tanks.at(Sibling).MarkedForReplacement && Tanks.at(Sibling).WashingCycles > 0)
						{
							redundancySectionsOn = false;
							ChosenTanks.push_back(Tanks.at(Sibling).TankIndex);
							if(Tanks.at(Sibling).WashingCycles < Cycles)
								Cycles = Tanks.at(Sibling).WashingCycles;
						}
					}
				}
			}
		}

		if (redundancySectionsOn)
		{	//Repeat previous #cyces fetching, but for tanks marked with "Redundancy"
			for(TTanksVector::iterator i = Tanks.begin(); i != Tanks.end(); ++i)
			{
				if(i->MarkedForReplacement)
				{
					if(i->PumpingPhase == PHASE_SHORT_REDUNDENCY)
					{
						if(i->WashingCycles > 0)
						{
							ChosenTanks.push_back(i->TankIndex);
							if(i->WashingCycles < Cycles)
								Cycles = i->WashingCycles;
						}
						else
						{
							TTankIndex Sibling = GetSiblingTank(static_cast<TTankIndex>(i->TankIndex));
							if(Tanks.at(Sibling).MarkedForReplacement && Tanks.at(Sibling).WashingCycles > 0)
							{
								ChosenTanks.push_back(Tanks.at(Sibling).TankIndex);
								if(Tanks.at(Sibling).WashingCycles < Cycles)
									Cycles = Tanks.at(Sibling).WashingCycles;
							}
						}
					}
				}
			}
        }

		int CycleTime = 0;

		// Calculating time and decrementing cycles for next section
		for(std::vector<int>::iterator i = ChosenTanks.begin(); i != ChosenTanks.end(); ++i)
		{
			//each cycle consists of filling up the chamber(s) and draining it(them) + [stabilization time].
			CycleTime = Cycles * (
				m_ParamMgr->MRW_AirValveStabilizationSecs.Value()
				+ (((Tanks.at(*i).IsFlooding) && ((Tanks.at(*i).TankIndex > TYPE_TANK_SUPPORT2) && (Tanks.at(*i).TankIndex < TYPE_TANK_MODEL13))) ? FloodingTime : NonFloodingTime)); //Support tanks are always not flooded so should use the relevant time for cycle
			if(CycleTime > MaxCyclesTimeSec)
				MaxCyclesTimeSec = CycleTime;

			Tanks.at(*i).WashingCycles -= Cycles;
		}

		TSection CurrentSection;
		CurrentSection.NumPumps = ChosenTanks.size();
		ChosenTanks.clear();

		CurrentSection.Cycles             = Cycles;
		CurrentSection.SectionTimeSec     = MaxCyclesTimeSec;
		CurrentSection.SingleCycleTimeSec = CurrentSection.SectionTimeSec / Cycles;

		m_Progress.ETAEachSection.push_back(CurrentSection);
	}

	int WashingCyclesTimeSec         = 0;
	m_Progress.OverallTimeEstimation = 0;
	
	for(std::vector<TSection>::iterator i = m_Progress.ETAEachSection.begin(); i != m_Progress.ETAEachSection.end(); ++i)
		WashingCyclesTimeSec += i->SectionTimeSec;

	m_Progress.OverallTimeEstimation   = m_Progress.PurgesTime + WashingCyclesTimeSec + m_Progress.DrainingTime;
	m_Progress.OverallRemainingTime    = m_Progress.OverallTimeEstimation;
	m_Progress.DrainingPercent         = (m_Progress.DrainingTime / m_Progress.OverallTimeEstimation) * 100;

	if (0 < m_ParamMgr->MRW_NumberOfPurgeCycles.Value())
		m_Progress.SinglePurgeCyclePercent = ((m_Progress.PurgesTime / m_Progress.OverallTimeEstimation) * 100) / m_ParamMgr->MRW_NumberOfPurgeCycles;
	else //avoid division by zero
		m_Progress.SinglePurgeCyclePercent = ((m_Progress.PurgesTime / m_Progress.OverallTimeEstimation) * 100);

	LogProgressEstimations();
}

void CResinReplacementWizard::LogProgressEstimations()
{
	WriteToLogFile(LOG_TAG_GENERAL,"-------------------------------------------------");
	WriteToLogFile(LOG_TAG_GENERAL,"Time and progress estimations (normalized):");

	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Pumping sections: %d", m_Progress.ETAEachSection.size()));
	
	int sec = 1;
	for(std::vector<TSection>::iterator i = m_Progress.ETAEachSection.begin(); i != m_Progress.ETAEachSection.end(); ++i, sec++)
		WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Section #%d - "
												  "Cycles: %d, "
												  "Pumps: %d, "
												  "Time: %d sec, "
												  "Single cycle time: %d sec", sec, i->Cycles,
																					i->NumPumps,
																					i->SectionTimeSec,
																					i->SingleCycleTimeSec));

	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Single purge cycle: %d %", QSimpleRound(m_Progress.SinglePurgeCyclePercent)));
	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("(1st) Draining: %d %", m_Progress.DrainingPercent));
	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Purges time: %d min", QSimpleRound(m_Progress.PurgesTime / 60)));
	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("(1st) Draining time: %d min", m_Progress.DrainingTime / 60));
	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Overall time: %d min", (int)m_Progress.OverallTimeEstimation / 60));
    WriteToLogFile(LOG_TAG_GENERAL,"-------------------------------------------------");
}

void CResinReplacementWizard::BeforePerformReplacementCycles(CWizardPage* WizardPage)
{
  EnableAllAxesAndHome(AxesTable);
  TQErrCode Err = Q_NO_ERROR;
  TRY_FUNCTION(Err, m_BackEnd->GotoPurgePosition(true, true)); //home ALL axes

  // Turning On the odor fan
  m_BackEnd->SetHoodFanOnOff(true);

  // Making sure that the block is hot.
  WriteToLogFile(LOG_TAG_GENERAL,"Heating heads");
  HeadsHeatingCycle();
  WriteToLogFile(LOG_TAG_GENERAL,"Heads Heating has ended");

  if (IsCancelled())
	throw CQ2RTAutoWizardCancelledException();

  HeadsDrainingCycle(WizardPage, m_Progress.DrainingTime, false, false); //wipes do not need to be performed here
  //Updating the progress bar will be executed in the scope of PerformReplacementCycles

  if (IsCancelled())
	throw CQ2RTAutoWizardCancelledException();
}

void CResinReplacementWizard::AfterPerformReplacementCycles(CWizardPage* WizardPage)
{
  if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();

  WriteToLogFile(LOG_TAG_GENERAL,"Wiping");
  m_BackEnd->WipeHeads(false);
  WriteToLogFile(LOG_TAG_GENERAL,"Finished Wiping");

  // Doing the purge cycles.
  if (0 != m_ParamMgr->MRW_NumberOfPurgeCycles)
  {
	WriteToLogFile(LOG_TAG_GENERAL,"Purging");

	if (PurgeCycle(m_ParamMgr->MRW_NumberOfPurgeCycles, WizardPage) == false)
		throw EQException("CResinReplacementWizard::PurgeCycle() returned an error.");

	WriteToLogFile(LOG_TAG_GENERAL,"Purging has ended");

  }
  else
  {
	  WriteToLogFile(LOG_TAG_GENERAL,"Wiping");

	  m_BackEnd->WipeHeads(false);
	  m_BackEnd->WipeHeads(false);
	  WriteToLogFile(LOG_TAG_GENERAL,"Finished Wiping");
	/*  CMachineSequencer *Sequencer = Q2RTApplication->GetMachineManager()->GetMachineSequencer();
	  Sequencer->FireAll();
	  WriteToLogFile(LOG_TAG_GENERAL,"Finished FireAll");     */
  }
  // Turning Off the odor fan
  m_BackEnd->SetHoodFanIdle();

  // End of page - Turn off heating to avoid "heads burn"
  m_BackEnd->TurnHeadHeaters(false);  
}


void CResinReplacementWizard::PerformReplacementCycles(CWizardPage* WizardPage)
{
	//The 1st draining is done in the scope of BeforePerformReplacementCycles.
	CResinSelectionSummaryWizardPage *Page = dynamic_cast<CResinSelectionSummaryWizardPage *>(WizardPage);
	m_Progress.CurrentProgress      += m_Progress.DrainingPercent;
	m_Progress.OverallRemainingTime -= m_Progress.DrainingTime;
	
	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Current progress: %d %", (int)QSimpleRound(m_Progress.CurrentProgress)));
	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Remaining time: %d min", (int)QSimpleRound(m_Progress.OverallRemainingTime / 60)));
	
	Page->SetProgressPercentage(QSimpleRound(m_Progress.CurrentProgress));
	Page->SetRemaningTime(QSimpleRound(m_Progress.OverallRemainingTime / 60));
	Page->Refresh(); // to update the progress bar

	BlockWashingCycles(WizardPage);
}

void CResinReplacementWizard::BlockWashingCycles(CWizardPage* WizardPage)
{
	CResinSelectionSummaryWizardPage *Page = dynamic_cast<CResinSelectionSummaryWizardPage *>(WizardPage);

	TQErrCode Err = Q_NO_ERROR;

	WriteToLogFile(LOG_TAG_GENERAL,"Starting block washing cycles.");
	TRY_FUNCTION(Err, m_BackEnd->GotoPurgePosition(true, false));

	int TotalNumSections = m_Progress.ETAEachSection.size();

	while(! IsWashingComplete(m_TanksForReplacement) && ! IsCancelled())
	{
		PopulatePumpControllers();

		int CycleNum = 0;
		int SectionNum = m_Progress.PerformedSections; //updated via PopulatePumpControllers

		//sanity check failure - too many actual sections performed than calculated
		if (SectionNum > TotalNumSections) {
			 WriteToLogFile(LOG_TAG_GENERAL, QFormatStr(
				"Error: Current block washing section (%d) is out of bound", SectionNum));
			 throw EQException(MSG_MACHINE_NOT_CALIBRATED);
		}

		m_PumpControllers.clearControllerOperationDoneFlag();
		while(! m_PumpControllers.isControllerOperationDone())
		{
			ResinFlowUtils::AirValve av(this);
			if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();

			/*
				Bug in ResinFlowUtils:
				the *isControllerOperationDone* flag is updated only
				on the NEXT call to *controlAllPumps*.
				As a result,
				anything in this loop must be wrapped with an IF clause
				so that it won't be executed one *extra* time.
			 */
			if (CycleNum < m_Progress.ETAEachSection.at(SectionNum -1).Cycles)
				WriteToLogFile(LOG_TAG_GENERAL, QFormatStr(
					"Starting block washing cycle #%d, section #%d", CycleNum+1, SectionNum));

			else if (CycleNum-1 > m_Progress.ETAEachSection.at(SectionNum -1).Cycles)
			{ //at least 2 cycles differnece between calculated num cycles and performed ones
				WriteToLogFile(LOG_TAG_GENERAL, QFormatStr(
					"Error: Number of washing cycles exceeded current section's declaration (%d cyc)",
					m_Progress.ETAEachSection.at(SectionNum -1).Cycles));
				throw EQException(MSG_MACHINE_NOT_CALIBRATED);
			}

			m_PumpControllers.clearCurrentPumpingCycleDoneFlag();
			while (! m_PumpControllers.isCurrentPumpingCycleDone() && ! IsCancelled())
			{
				try
				{
/*#ifdef _DEBUG
					 [@Bug] This will demonstrate a major bug:
					  ResinFlowUtils cannot communicate with pumps (same UI thread?)
					  while a notification pops-up.
					  Remove this comment if/when the bug is resolved.

					  //if (FindWindow(0, "A_VERY_UNIQUE_MSC_POPUP.txt - Notepad"))
					  //	FrontEndInterface->NotificationMessage("MSC_24V Supply-Wrong received value");
#endif*/
					m_PumpControllers.controlAllPumps();
				}
				catch(const ETimeout &e)
				{ // translate exception message to user readable message
					throw ETimeout("Filling the block during block washing cycle");
				}
				YieldAndSleepWizardThread();
			}

			if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();

			// drainblock
			if (! m_PumpControllers.isPumpsVectorEmpty() && ! m_PumpControllers.isControllerOperationDone())
			{
				av.holdOpen(SEC_TO_MS(m_OptimizedEmptyBlockTime));
				if (Q2RTWizardSleep(m_ParamMgr->MotorPostPurgeTimeSec) == false)
					return;

				//TODO: Check if stabilization time is required. If so - apply it
				//Stabilization is required if the thermistors' temperature aren't valid upon closing the air-valve
				if (Q2RTWizardSleep(m_ParamMgr->MRW_AirValveStabilizationSecs) == false)
					return;
			}

			if (! m_PumpControllers.isControllerOperationDone())
			{
				float CurrentCycleTime  = m_Progress.ETAEachSection.at(m_Progress.PerformedSections - 1).SingleCycleTimeSec;
				float CurrentCyclePercent = CurrentCycleTime / m_Progress.OverallTimeEstimation * 100;
				m_Progress.CurrentProgress      += CurrentCyclePercent;
				m_Progress.OverallRemainingTime -= CurrentCycleTime;
					
				WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Current progress: %d %", (int)QSimpleRound(m_Progress.CurrentProgress)));
				WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Remaining time: %d min", (int)QSimpleRound(m_Progress.OverallRemainingTime / 60)));
	
				Page->SetProgressPercentage(QSimpleRound(m_Progress.CurrentProgress));
				Page->SetRemaningTime(QSimpleRound(m_Progress.OverallRemainingTime / 60));
				Page->Refresh(); // to update the progress bar

				WriteToLogFile(LOG_TAG_GENERAL, QFormatStr("Block washing cycle #%d (section #%d) is finished", ++CycleNum, SectionNum));
			}
		}

		DecrementDoneCycles(CycleNum);
	}

	WriteToLogFile(LOG_TAG_GENERAL,"Block washing cycles completed.");
}

void CResinReplacementWizard::DecrementDoneCycles(int Cycles)
{
	QString Tanks = "";

	m_Progress.CombinedCycles += Cycles;

	for(ResinFlowUtils::TPumpIndicesVector::const_iterator i = m_CurrentPumps.begin(); i != m_CurrentPumps.end(); ++i)
	{
		Tanks += TankToStr(PumpToTank(*i)) + ", ";
		m_TanksForReplacement.at(PumpToTank(*i)).WashingCycles -= Cycles;
	}

	if (0 != Tanks.compare("")) Tanks.erase(Tanks.end() - 2, Tanks.end());

	WriteToLogFile(LOG_TAG_GENERAL, QFormatStr(
		"Decrementing the performed %d cycles from: %s.", Cycles, Tanks.c_str()));
}

bool CResinReplacementWizard::IsWashingComplete(TTanksVector &Tanks)
{
	for(TTanksVector::const_iterator i = Tanks.begin(); i != Tanks.end(); ++i)
		if(i->MarkedForReplacement)
			if(i->WashingCycles > 0)
				return false;
	return true;
}

void CResinReplacementWizard::PopulatePumpControllers()
{
	TTanksIndicesVector chosenTanks;
	chosenTanks.clear();
	m_CurrentPumps.clear();
	m_PumpControllers.deletePumpControllers();

	int Cycles = INT_MAX;

	WriteToLogFile(LOG_TAG_GENERAL,"PopulatePumpControllers: Washing cycles for each tank:");

	bool redundancySectionsOn = true;
	for(TTanksVector::const_iterator i = m_TanksForReplacement.begin(); i != m_TanksForReplacement.end(); ++i)
	{
		WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Tank index: %d, Tank name: %s, Material name: %s, Cycles: %d",
												  i->TankIndex,
												  TankToStr(static_cast<TTankIndex>(i->TankIndex)).c_str(),
												  (i->MaterialName.compare("") == 0) ? "----------" : i->MaterialName.c_str(),
												  i->WashingCycles));

		if(i->MarkedForReplacement)
		{
			if(i->PumpingPhase == PHASE_SHORT)
			{
				if(i->WashingCycles > 0)
				{
					redundancySectionsOn = false;
					m_CurrentPumps.push_back(TankToPump(static_cast<TTankIndex>(i->TankIndex)));
                    chosenTanks.push_back(static_cast<TTankIndex>(i->TankIndex));
					if(i->WashingCycles < Cycles)
						Cycles = i->WashingCycles;
				}
				else
				{
					TTankIndex Sibling = GetSiblingTank(static_cast<TTankIndex>(i->TankIndex));
					if(m_TanksForReplacement.at(Sibling).MarkedForReplacement && m_TanksForReplacement.at(Sibling).WashingCycles > 0)
					{
						redundancySectionsOn = false;
						m_CurrentPumps.push_back(TankToPump(Sibling));
						chosenTanks.push_back(Sibling);
						if(m_TanksForReplacement.at(Sibling).WashingCycles < Cycles)
							Cycles = m_TanksForReplacement.at(Sibling).WashingCycles;
					}
				}
			}
		}
	}

	if (redundancySectionsOn) {
		//Repeat pumps analysis, but for the redundancy support chamber

		for(TTanksVector::const_iterator i = m_TanksForReplacement.begin(); i != m_TanksForReplacement.end(); ++i)
		{
			if(i->MarkedForReplacement)
			{
				if(i->PumpingPhase == PHASE_SHORT_REDUNDENCY)
				{
					if(i->WashingCycles > 0)
					{
						m_CurrentPumps.push_back(TankToPump(static_cast<TTankIndex>(i->TankIndex)));
						chosenTanks.push_back(static_cast<TTankIndex>(i->TankIndex));
						if(i->WashingCycles < Cycles)
							Cycles = i->WashingCycles;
					}
					else
					{
						TTankIndex Sibling = GetSiblingTank(static_cast<TTankIndex>(i->TankIndex));
						if(m_TanksForReplacement.at(Sibling).MarkedForReplacement && m_TanksForReplacement.at(Sibling).WashingCycles > 0)
						{
							m_CurrentPumps.push_back(TankToPump(Sibling));
							chosenTanks.push_back(Sibling);
							if(m_TanksForReplacement.at(Sibling).WashingCycles < Cycles)
								Cycles = m_TanksForReplacement.at(Sibling).WashingCycles;
						}
					}
				}
			}
		}
	}

	if (! m_CurrentPumps.empty())
	{
		CreatePumpController(m_PumpControllers, m_CurrentPumps, Cycles);
		m_Progress.PerformedSections++; // count the performed pumping "sections"

		//Re-calculate the air valve time using only the tanks that are to be used in next Section.
		//If no active tanks' chambers are flooding -
		//optimize by lowering the air valve time to minimum (like in the estimated time).
		SetOptimizedDrainingTime(m_NewThermistorsOpMode, chosenTanks);
	}
}

void CResinReplacementWizard::CreatePumpController(ResinFlowUtils::PumpControllers &Controller,
													  ResinFlowUtils::TPumpIndicesVector &Pumps,
													  int Cycles)
{	
	using namespace ResinFlowUtils;

	for (TPumpIndicesVector::const_iterator i =	Pumps.begin(); i != Pumps.end(); ++i)
	{
		Controller.AddSpecializedPumpController(ResinFlowUtils::FillingCycles,
												*i,
												0, // Ignored for PumpControllerForFillingCyclesSynced.
												m_ParamMgr->MRW_FillingTimeoutInSec * 1000,
												m_ParamMgr->MRW_PumpsTimeDutyOn_ms,
												m_ParamMgr->MRW_PumpsTimeDutyOff_ms,
												static_cast<VerificationAttributes>(vaTankValid | vaTankWeight | vaTankResinType),
												m_TanksForReplacement.at(PumpToTank(*i)).MaterialName,
												Cycles);

		WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Pump controller created for: %s", PumpIndexToDescription(*i).c_str()));										
	}
}

bool CResinReplacementWizard::PurgeCycle(int NumberOfPurgeCycles, CWizardPage* WizardPage)
{
    CResinSelectionSummaryWizardPage *Page = dynamic_cast<CResinSelectionSummaryWizardPage *>(WizardPage);

	using namespace ResinFlowUtils;

	int cycleNum = 0;
	
  	WriteToLogFile(LOG_TAG_GENERAL,"Starting block purge cycles.");	

	TPumpIndicesVector PumpsVector;

	// Create the pumps vector of pumps that will participate in the Purge iterations.
	for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
	{
		TPumpIndex ActivePumpID = TankToPump(GetChamberActiveTankID(static_cast<TChamberIndex>(i)));

		TPumpIndicesVector::const_iterator it;
		for(it = PumpsVector.begin(); it != PumpsVector.end(); ++it)
			if(*it == ActivePumpID)
				break;
        // Don't add duplicate pumps
		if(it == PumpsVector.end())
			PumpsVector.push_back(ActivePumpID);
	}

	// Create the PumpControllers object:
	PumpControllers PurgeCyclesController(this);
	for (TPumpIndicesVector::const_iterator i =	PumpsVector.begin(); i != PumpsVector.end(); ++i)
		PurgeCyclesController.AddSpecializedPumpController(
												ResinFlowUtils::BlockFilling,
												*i,
												0, // Ignored for ResinFlowUtils::FillingCycles
												m_ParamMgr->MRW_FillingTimeoutInSec * 1000,
												m_ParamMgr->MRW_PumpsTimeDutyOn_ms,
												m_ParamMgr->MRW_PumpsTimeDutyOff_ms,
												static_cast<VerificationAttributes>(vaTankValid | vaTankWeight),
												m_TanksForReplacement.at(PumpToTank(*i)).MaterialName);
	
    TQErrCode Err = Q_NO_ERROR;
	TRY_FUNCTION(Err, m_BackEnd->GotoPurgePosition(true, false));
	ResinFlowUtils::AirValve av(this);

	for (int i = NumberOfPurgeCycles; i > 0; i--)
	{
		WriteToLogFile(LOG_TAG_GENERAL, QFormatStr("Block purge cycle #%d.", ++cycleNum));
		PurgeCyclesController.restartAllControllers();
		while (!PurgeCyclesController.isControllerOperationDone())
		{
            if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
			try
            {
				PurgeCyclesController.controlAllPumps();
			}
			catch(const ETimeout &e)
			{ // translate exception message to user readable message
				throw ETimeout("Filling the block during block purging cycle");
			}
			YieldAndSleepWizardThread();
		} // while
        // if wizard was cancelled while in the control loop above, we return here. (without perfroming the next Purge())
        if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();

		// perform actual purge. (AirValve only)
		//We don't want to call the Monitor module becuase the Active Tanks are incorrect
		//until we call UpdateTanks + UpdateChambers (!)
		//		m_BackEnd->Purge(false, false); //DON'T USE IT! Use regular airvalve instead
		av.holdOpen(m_ParamMgr->PurgeLongTime); //[msec]
		if (Q2RTWizardSleep(m_ParamMgr->MotorPostPurgeTimeSec) == false)
			return false;
			
		//TODO: Check if stabilization time is required. If so - apply it
		//Stabilization is required if the thermistors' temperature aren't valid upon closing the air-valve
		//MRW_AirValveStabilizationSecs is apparently the same as MRW_DelayBetweenPurges
		if (Q2RTWizardSleep(m_ParamMgr->MRW_DelayBetweenPurges) == false)
			return false;

		m_Progress.CurrentProgress      += m_Progress.SinglePurgeCyclePercent;
		//the overall time should be decreased by the total PurgesTime divided by the number of purge cycles
		if (0 < m_ParamMgr->MRW_NumberOfPurgeCycles.Value())
			m_Progress.OverallRemainingTime -= (m_Progress.PurgesTime / m_ParamMgr->MRW_NumberOfPurgeCycles);
		else //avoid division by zero
			m_Progress.OverallRemainingTime -=  m_Progress.PurgesTime;

		
		WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Current progress: %d %", (int)QSimpleRound(m_Progress.CurrentProgress)));
		WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Remaining time: %d min", (int)QSimpleRound(m_Progress.OverallRemainingTime / 60)));
		
		Page->SetProgressPercentage(QSimpleRound(m_Progress.CurrentProgress));
		Page->SetRemaningTime(QSimpleRound(m_Progress.OverallRemainingTime / 60));
		Page->Refresh(); // to update the progress bar
	} // for

	if (Q2RTWizardSleep(m_ParamMgr->MotorPostPurgeTimeSec) == false)
		return false;

	WriteToLogFile(LOG_TAG_GENERAL,"Block purge cycles completed.");
	m_BackEnd->WipeHeads(false);
	m_BackEnd->WipeHeads(false);
    //TRY_FUNCTION(Err, m_BackEnd->GotoPurgePosition(true, false));

	return true;
}


void CResinReplacementWizard::SetHowRequiredAfterMRW()
{
   	if(m_ParamMgr->TypesArrayPerChamber[TYPE_CHAMBER_SUPPORT].ValueAsString().compare(m_ParamMgr->HSW_Primary_Voltages_Support_Material) == 0)
		return;

	m_ParamMgr->HSW_Primary_Voltages_Support_Material =   m_ParamMgr->TypesArrayPerChamber[TYPE_CHAMBER_SUPPORT];
	m_ParamMgr->SaveSingleParameter(&m_ParamMgr->HSW_Primary_Voltages_Support_Material);

	for (int i = 0; i < NUM_OF_OPERATION_MODES * NUMBER_OF_QUALITY_MODES; i++)
	{
	  m_ParamMgr->HSW_PrintingAllowedForMode[i] = false;
	}
	m_ParamMgr->SaveSingleParameter(&m_ParamMgr->HSW_PrintingAllowedForMode);
}


void CResinReplacementWizard::HeadsHeatingCycle(void)
{
  CParamScopeRestorePointArray<int,TOTAL_NUMBER_OF_HEATERS> sc(m_ParamMgr->HeatersTemperatureArray);
  for (int i = FIRST_HEAD; i < LAST_HEAD; ++i)
	  m_ParamMgr->HeatersTemperatureArray[i] = m_ParamMgr->HeatersTemperatureArray[i] -
					(IS_MODEL_HEAD(i) ? m_ParamMgr->MRW_ModelTempDelta : m_ParamMgr->MRW_SupportTempDelta);
  CQ2RTAutoWizard::HeadsHeatingCycle();
}

bool CResinReplacementWizard::CheckTanksValidity(CWizardPage* WizardPage)
{
	CResinSelectionSummaryWizardPage* Page = dynamic_cast<CResinSelectionSummaryWizardPage *>(WizardPage);

	bool AllTanksOk = false;

	int NeededAccumulatedWasteSpace = 0;
	for (TTanksVector::const_iterator i = m_TanksForReplacement.begin(); i != m_TanksForReplacement.end(); ++i)
		if (i->MarkedForReplacement)
		  NeededAccumulatedWasteSpace += i->NeededWeight;
	if( CHECK_EMULATION(m_ParamMgr->OCB_Emulation))
		{
		//  Page->SetWasteTanksFreeSpace(SUMMARY_WASTE_WEIGHT_OK);
		  for(TTanksVector::iterator i = m_TanksForReplacement.begin(); i != m_TanksForReplacement.end(); ++i)
				i->Status = STATUS_OK;
		  AllTanksOk = true;
		  Page->SetTanksVector(m_TanksForReplacement);
		Page->Refresh();
		}

		CAutoWizard::YieldAndSleepWizardThread();
		if(IsCancelled()) throw CQ2RTAutoWizardCancelledException();
		if(m_LeavingSummaryPage) throw EMRWLeavingSummaryPageException();

		unsigned TanksToCheck = 1; //consider all waste cartridges as one
		for(TTanksVector::const_iterator i = m_TanksForReplacement.begin(); i != m_TanksForReplacement.end(); ++i)
			if(i->MarkedForReplacement)
				TanksToCheck++;

		unsigned TanksOkCounter = 0;
		for(TTanksVector::iterator i = m_TanksForReplacement.begin(); i != m_TanksForReplacement.end(); ++i)
		{
			CAutoWizard::YieldAndSleepWizardThread();
			if(IsCancelled()) throw CQ2RTAutoWizardCancelledException();
			if(m_LeavingSummaryPage) throw EMRWLeavingSummaryPageException();

			if(i->MarkedForReplacement)
			{
				int validTankWeight = -1;
				TTankIndex ti = static_cast<TTankIndex>(i->TankIndex);

				if(m_BackEnd->GetTankMaterialType(ti) != i->MaterialName)
				{
					i->Status = STATUS_ERROR;
					if(TanksOkCounter > 0)
						TanksOkCounter--;
				}
				else if(m_BackEnd->IsResinExpired(ti)) //The check m_BackEnd->IsMaxPumpTimeExceeded(ti) is no longer relevant for Triplex (Objet) machines
				{
                	i->Status = STATUS_EXPIRED;
					if(TanksOkCounter > 0)
						TanksOkCounter--;
                }
				else if (! m_WeightSampling.IsWeightStable(ti, validTankWeight))
				{
					i->Status = STATUS_STABILIZATION_TIME;
					if(TanksOkCounter > 0)
						TanksOkCounter--;
				}
				else if (validTankWeight < i->NeededWeight)
				{
					i->Status = STATUS_WARNING;
					if(TanksOkCounter > 0)
						TanksOkCounter--;
				}
				else
				{
					i->Status = STATUS_OK;
					if(TanksOkCounter < TanksToCheck)
						TanksOkCounter++;
				}
			}
		}

		//Check that the waste cartridge(s) has/have enough space for all the washed up resin altogether
		int currWasteCapcaityLeft = -1;
		bool isWasteStable = (m_WeightSampling.IsWeightStable(TOTAL_NUMBER_OF_CONTAINERS, currWasteCapcaityLeft));
		if (0 == currWasteCapcaityLeft)
		{
			//The case of all 'waste-cartridges are full' is not relevant because
			//Container module throws an exception that cancels MR's wizard.
			//bool isAnyWasteCartridgeIn = m_BackEnd->IsWasteFull();
			//if the waste's left space is 0 and is not full -- the cartridge is actually not there.
			//Page->SetWasteTanksFreeSpace(isAnyWasteCartridgeIn ? SUMMARY_WASTE_WEIGHT_FULL : SUMMARY_WASTE_WEIGHT_NA);
			Page->SetWasteTanksFreeSpace(SUMMARY_WASTE_WEIGHT_NA);
			--TanksOkCounter;
		}
		else if (! isWasteStable)
		{   //report that the waste needs to be stabilized
			Page->SetWasteTanksFreeSpace(SUMMARY_WASTE_WEIGHT_UNSTABLE);
			--TanksOkCounter;
		}
		else if (currWasteCapcaityLeft < NeededAccumulatedWasteSpace)
		{
			Page->SetWasteTanksFreeSpace(NeededAccumulatedWasteSpace-currWasteCapcaityLeft);
			--TanksOkCounter;
		} else //weight's ok
		{
			Page->SetWasteTanksFreeSpace(SUMMARY_WASTE_WEIGHT_OK);
			++TanksOkCounter;
        }

		AllTanksOk = (TanksOkCounter == TanksToCheck) ? true : false;

		//bypass tanks checks if we deliberately require it or set up emulation mode
	#if defined EMULATION_ONLY
		AllTanksOk = true;
		for (TTanksVector::iterator i = m_TanksForReplacement.begin(); i != m_TanksForReplacement.end(); ++i)
		{
			 i->Status = STATUS_OK;
		}
	#endif
		if (Page->AreWarningsSuppressed()) {
			AllTanksOk = true;
			for (TTanksVector::iterator i = m_TanksForReplacement.begin(); i != m_TanksForReplacement.end(); ++i)
			{
				 i->Status = STATUS_OK;
			}
		}

		if(!AllTanksOk)
			Page->SetValidTanks(false);
		else
			Page->SetValidTanks(true);
#ifdef _DEBUG
		if(FindWindow(0, "AllTanksError.txt - Notepad"))
			for(TTanksVector::iterator i = m_TanksForReplacement.begin(); i != m_TanksForReplacement.end(); ++i)
				i->Status = STATUS_ERROR;
		else if(FindWindow(0, "AllTanksWarning.txt - Notepad"))
			for(TTanksVector::iterator i = m_TanksForReplacement.begin(); i != m_TanksForReplacement.end(); ++i)
				i->Status = STATUS_WARNING;
		else if(FindWindow(0, "AllTanksWaiting.txt - Notepad"))
			for(TTanksVector::iterator i = m_TanksForReplacement.begin(); i != m_TanksForReplacement.end(); ++i)
				i->Status = STATUS_STABILIZATION_TIME;
		else if(FindWindow(0, "AllTanksOk.txt - Notepad"))
		{
			for(TTanksVector::iterator i = m_TanksForReplacement.begin(); i != m_TanksForReplacement.end(); ++i)
				i->Status = STATUS_OK;
			AllTanksOk = true;	
		}

		if(FindWindow(0, "AllWasteTanksOk.txt - Notepad"))
			Page->SetWasteTanksFreeSpace(SUMMARY_WASTE_WEIGHT_OK);
		//else if(FindWindow(0, "AllWasteTanksFull.txt - Notepad"))
		//	Page->SetWasteTanksFreeSpace(SUMMARY_WASTE_WEIGHT_FULL);
		else if(FindWindow(0, "AllWasteTanksOut.txt - Notepad"))
			Page->SetWasteTanksFreeSpace(SUMMARY_WASTE_WEIGHT_NA);
#endif

		Page->SetTanksVector(m_TanksForReplacement);
		Page->Refresh();
		return AllTanksOk;

}

/* ======================== TWeightStabilitySampling's messages ====================== */
void CResinReplacementWizard::TWeightStabilitySampling::ClearSamples()
{
	for (int i = 0; i <= TOTAL_NUMBER_OF_CONTAINERS; ++i) {
		WeightSamples[i].clear();
		CurrStrikeOuts[i] = 0;
	}
}
void CResinReplacementWizard::TWeightStabilitySampling::Initialize(bool isSamplingEnabled,
	unsigned numSamples, unsigned toleranceDelta, unsigned stabilityMaxTryouts)
{
	WeightSamplingEnabled = isSamplingEnabled || (numSamples>1);
	MaxStrikeOuts		  = stabilityMaxTryouts;
	WeightSamplingTolerance	= toleranceDelta;
	MaxNumSamples			= numSamples;
	ClearSamples();
	BackEnd				  = CBackEndInterface::Instance();
}

bool CResinReplacementWizard::TWeightStabilitySampling::IsWeightStable(
	TTankIndex ti, int& outWeight)
{
	if (ti >= TOTAL_NUMBER_OF_CONTAINERS)
	{
		ti = TOTAL_NUMBER_OF_CONTAINERS;
		outWeight = BackEnd->GetRemainingWasteWeight();
	} else
	{
		outWeight = BackEnd->GetTankWeightInGram(ti);
	}

#ifdef _DEBUG
/*
	CQLog::Write(LOG_TAG_GENERAL, QFormatStr(
		"Weight Stabilizer : tank - %6s  ;  weight - %d", TankToStr(ti).c_str(), outWeight));
*/
#endif

	if (WeightSamplingEnabled)
	{
		WeightSamples[(int)ti].push_back(outWeight);
		if (WeightSamples[(int)ti].size() < MaxNumSamples)
		{
			outWeight = -1;
			return false;
		}
		else
		{
			WeightSamples[(int)ti].pop_front(); //remove the oldest sample
			//check if the history of samples is stable (i.e. MAX-MIN < Tolerance)
			int max = *std::max_element(WeightSamples[(int)ti].begin(), WeightSamples[(int)ti].end());
			int min = *std::min_element(WeightSamples[(int)ti].begin(), WeightSamples[(int)ti].end());
			if ((signed)WeightSamplingTolerance < max - min)
			{
				if (CurrStrikeOuts[(int)ti] < MaxStrikeOuts)
				{
					++(CurrStrikeOuts[(int)ti]);
					outWeight = -1;
					return false;
				}
				else //we stroke out too many times - cancel the wizard
				{
					//CQ2RTAutoWizardCancelledException is not good enough,
					//because we want to convey a message.
					throw EQException(QFormatStr(MSG_ERR_CARTRIDGE_CANT_STABILIZE.c_str(),
						TankToStr(ti).c_str()));
				}
			}
		}
	}

#ifdef _DEBUG
/*
	CQLog::Write(LOG_TAG_GENERAL, QFormatStr(
		"Weight Stabilizer : tank %6s is stable", TankToStr(ti).c_str()));
*/		
#endif
	return true; //outWeight already contains a stable value
}
/* ====================== End of TWeightStabilitySampling's messages ================= */

void CResinReplacementWizard::MarkAllTotalPrintingTime()
{
	CMaintenanceCounters *maintenanceCounters = CMaintenanceCounters::GetInstance();
	for (int modelTank = FIRST_MODEL_TANK_TYPE; modelTank < TOTAL_NUMBER_OF_CONTAINERS; ++modelTank)
	{
		TTankIndex ti = static_cast<TTankIndex>(modelTank);
		TChamberIndex chi = TankToStaticChamber(ti);

		if (IsChamberFlooded(chi))
			continue; //chamber's piping segments are irrelevant, so skip.
		else
		{
			m_ParamMgr->MRW_LastTotalPrintingTimeSec[static_cast<int>(modelTank /*ti*/)] =
				maintenanceCounters->GetElapsedSeconds(
					static_cast<int>(TankToPump(ti)));
			//same as calling GetElapsedSeconds(m_PumpCounterIDList(modelTank))
			//assuming the enum of TPumpIndex is correlating with TTankIndex
		}
	}
	m_ParamMgr->SaveSingleParameter(&m_ParamMgr->MRW_LastTotalPrintingTimeSec);
}
void CResinReplacementWizard::AdjustFlushingTypeStateByTotalPrintingTime(int deltaDirtySeconds)
{
	bool needCommit = false;
	CMaintenanceCounters *maintenanceCounters = CMaintenanceCounters::GetInstance();
	for (int modelTank = FIRST_MODEL_TANK_TYPE; modelTank < TOTAL_NUMBER_OF_CONTAINERS; ++modelTank)
	{
		TTankIndex ti = static_cast<TTankIndex>(modelTank);
		TChamberIndex chi = TankToStaticChamber(ti);

		if (IsChamberFlooded(chi))
			continue; //chamber's piping segments are irrelevant, so skip.
		else
		{
			unsigned long currentTPT = maintenanceCounters->GetElapsedSeconds(
				static_cast<int>(TankToPump(ti)));
			unsigned long tptAfterLastSuccessfulMR =
				m_ParamMgr->MRW_LastTotalPrintingTimeSec[static_cast<int>(modelTank)].Value();

			//Tip: if deltaDirtySeconds is to be set to -1, then this feature will always be bypassed
			if ((unsigned)deltaDirtySeconds <= (currentTPT - tptAfterLastSuccessfulMR))
			{   //Resins in the pipes have been washed enough time (during printing)
				//to be considered 'clean' with a homogenous resins.

				//assuming that the chamber's resin is the same as in the relevant LONG segment
				m_ParamMgr->MRW_IsSegmentEconomy[static_cast<int>(ChamberToLongSegment(chi))] = false;

				QString shortSegResinType = m_ParamMgr->MRW_TypesArrayPerSegment[(int)GetTankAdjacentSegment(ti)].Value();
				//if the SHORT segment is active (i.e. valid) - mark it as clean.
				if (0 == m_ParamMgr->TypesArrayPerChamber[(int)chi].Value().compare(shortSegResinType))
				m_ParamMgr->MRW_IsSegmentEconomy[(int)GetTankAdjacentSegment(ti)] = false;

				CQLog::Write(LOG_TAG_GENERAL, QFormatStr(
					"MRW : Piping is considered clean. (%s; %u)",
						TankToStr(ti).c_str(),
						currentTPT - tptAfterLastSuccessfulMR
					));

				//update a single tank with its counter, as opposed to calling MarkAllTotalPrintingTime
				m_ParamMgr->MRW_LastTotalPrintingTimeSec[static_cast<int>(modelTank)] =
					maintenanceCounters->GetElapsedSeconds(static_cast<int>(TankToPump(ti)));
				needCommit = true;
			}
		}
	}

	if (needCommit) {
		m_ParamMgr->SaveSingleParameter(&m_ParamMgr->MRW_IsSegmentEconomy);
		m_ParamMgr->SaveSingleParameter(&m_ParamMgr->MRW_LastTotalPrintingTimeSec);
    }
}

bool CResinReplacementWizard::PlaceAndCalculateWastePageLeave(CWizardPage* WizardPage)
{
	CResinMainSelectionWizardPage* source = dynamic_cast<CResinMainSelectionWizardPage *>(Pages[rrChooseResinsPage]);
	m_Pipeline->ClearResults();

	//Check designated replacement mode
	int numResinsToReplace = 0;

	//Add a new Target Material to fixate Support on the left most chamber.
	//[@Assumption] #Support (dedicated) chambers (aka MAX_NUMBER_OF_SUPPORT_CHAMBERS) = 1
	/******************
		 Support material MUST be added PRIOR to any model,
		 or the const reduced pipeline's "s_perms" permutations won't cut it.
	/******************/
	if (source->SupportVirtTargMaterial.IsValid())
	{
		m_Pipeline->AddTargetMaterial(/*DEDICATED_SUPPORT_CHAMBER, */source->SupportVirtTargMaterial);
	}
	/******************/
	int supportIdx = source->GetIndexExtraSupportChamber();
	for (int i = 0; i < supportIdx; ++i)
	{
		/*
			Restrictions: target resin must be a known resin (i.e. not blank)
			Remark: if the second flushing is enabled but the i-th target material isn't selected
			(yet active), it is expected here that this target material WILL be valid !
			[@Refer] to CCustomResinMainSelectWizardPageViewer::SaveResinProperties
			Number of added targ. materials will set the target op-mode.
			In 2nd-flushing, we want to keep the current op-mode, so we need to
			add "filler" target-materials that will participate in the purges and
			also make sure that the thermistors op-mode does NOT change from current mode. (The "fillers" have 0 washing-cycles)
			[@Assumption] The fillers are valid (i.e. not blank resin-typed)
		*/
		if (source->VirtTargMaterials[i].IsValid())
		{
			++numResinsToReplace;
			m_Pipeline->AddTargetMaterial(/* i,*/source->VirtTargMaterials[i]);
		}
	}

	if (source->VirtTargMaterials[supportIdx].IsValid())
	{	//register the extra support resin, if exists
		m_Pipeline->AddTargetMaterial(source->VirtTargMaterials[supportIdx]);
	}

	if (0 == numResinsToReplace) return false; //no selection was made, cannot continue.

	WriteToLogFile(LOG_TAG_GENERAL, QFormatStr("Placement mode(s): [%s]%s%s",
		(source->ManualMode) ? "Manual" : "Automatic",
		(source->ForceFlushingEnabled) ? " [Forced]" : "",
		(source->IsSilent) ? " [Silent]" : ""));

	if ((int)mrwUndefined < numResinsToReplace)
	{
		QMonitor.Printf("Too many resins (%d) were set to be replaced.", numResinsToReplace);
		return false;
	}

	//Distinguish between DM3 and DM6 requests.
	TResinsReplMode targMode =
		((MIN_NUM_RESINS == numResinsToReplace) && (DM3_MODE == source->PrintingMode)) ?
		mrwDM3 : mrwDM6; //originally mrwDM6 was mrwDM7. It affects all the permutaions taken into account

	COptimalTanks *opt = m_Pipeline->AutoRunPipeline(targMode, source->IsSilent);
	return ApplyPipelineResult(opt);
}

bool CResinReplacementWizard::ApplyPipelineResult(COptimalTanks *result)
{
	if (NULL == result)
	{
		QMonitor.Printf("MRW Pipeline warning: containers' new placements cannot be determined.");
		return false;
	}

	try
	{
		CResinSelectionSummaryWizardPage* page =
			dynamic_cast<CResinSelectionSummaryWizardPage*>(Pages[rrSummaryPage]);

		m_TanksForReplacement.clear();
		for (int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; i++)
		{
			TTankForReplacement Tank;
			TTankIndex currTank = static_cast<TTankIndex>(i);
			Tank.TankIndex            = i;
			Tank.MarkedForReplacement = result->IsActive	(currTank);
			Tank.MaterialName         = result->GetResin	(currTank);
			Tank.NeededWeight         = result->GetMinWeight(currTank);
			Tank.WashingCycles		  = result->GetCycles	(currTank);
			Tank.IsFlooding			  = result->IsFlooding	(currTank);

			//Prioritize pumps. The short phase must be done before the long one.
			TPumpingOrderPriority chkPrio = (result->GetPumpQoS(currTank));
			if (PUMP_FIRST == chkPrio)
				Tank.PumpingPhase = PHASE_SHORT;
			else if (PUMP_SECOND == chkPrio)
				Tank.PumpingPhase = PHASE_LONG;
			else
				Tank.PumpingPhase = NO_PHASE;

#ifdef _DEBUG
			CQLog::Write(LOG_TAG_GENERAL, QFormatStr(
				"MRW Pipeline : optimized target tank %5s ; %12s, cycles=%4d  %8s  %19s %10s",
					TankToStr((TTankIndex)i).c_str(),
					(std::strcmp(Tank.MaterialName.c_str(),"")==0) ? "EMPTY" : Tank.MaterialName.c_str(),
					Tank.WashingCycles,
					Tank.MarkedForReplacement ? "[Active]" : "",
					(Tank.PumpingPhase == PHASE_SHORT) ? "[1st Pumping Phase]" : "",
					Tank.IsFlooding ? "[Flooding]" : ""
					));
#endif

			Tank.Status               = STATUS_EMPTY;
			m_TanksForReplacement.push_back(Tank);
		}

		//===== 2+ Redundancy support ======
#ifdef OBJET_MACHINE_KESHET
		//Re-examine code here (and in SummaryPageEnter()) so we can execute 4 phases in case of dual support chambers
		//i.e. cartridge redundancy ("hot swap") of 3 or 4
		bool areUsingPrimarySupportChamber = false;
		for (int trk = FIRST_SUPPORT_TANK_TYPE;
			(trk < FIRST_MODEL_TANK_TYPE) && !areUsingPrimarySupportChamber; ++trk)
		{
			if (m_TanksForReplacement.at((int)trk).MarkedForReplacement)
				areUsingPrimarySupportChamber = true;
		}
		if (areUsingPrimarySupportChamber) {
			for (int trk = SUPPORT_MODEL_TANK_L; (trk <= SUPPORT_MODEL_TANK_R); ++trk)
			{
				TTankForReplacement& tankSecondarySupport = m_TanksForReplacement.at((int)trk);
				if (tankSecondarySupport.MarkedForReplacement)
				{ //modify the pumping phase so it won't clash with the primary support chamber's pumping
					if (PUMP_FIRST == tankSecondarySupport.PumpingPhase)
						tankSecondarySupport.PumpingPhase = PHASE_SHORT_REDUNDENCY;
					else if (PUMP_SECOND == tankSecondarySupport.PumpingPhase)
						tankSecondarySupport.PumpingPhase = PHASE_LONG_REDUNDENCY;
				}
			}
		}
#endif		
		//================================

		page->SetTanksVector(m_TanksForReplacement);

		//Retreive the thermistors' and tanks' new state
		m_IsHswRecovery         = result->IsRecoveryFromHswNeeded();
		m_NewThermistorsOpMode	= result->GetThermistorsOpMode();
		m_NewTanksOpMode 		= result->GetTanksOpMode();

		WriteToLogFile(LOG_TAG_GENERAL, QFormatStr("Thermistors' Op Mode's new state = "/*%s*/" (%d)"/*, m_ParamMgr->ThermistorsOperationMode.GetValueStringList()[m_NewThermistorsOpMode].c_str()*/, (int)m_NewThermistorsOpMode));
		WriteToLogFile(LOG_TAG_GENERAL, QFormatStr("Tanks' Op Mode's new state = %s (%d)",       m_ParamMgr->TanksOperationMode.GetValueStringList()[m_NewTanksOpMode].c_str(), (int)m_NewTanksOpMode));

		CalculateEconomySegments(result);

		return true;

	} catch (EQException& e) {
		m_TanksForReplacement.clear();
		QMonitor.Printf("MRW Pipline: Wrong placements algorithm result");
		QMonitor.ErrorMessage(e.GetErrorMsg(),ORIGIN_WIZARD_PAGE);
//		return false;
		throw CQ2RTAutoWizardCancelledException();
	}
}

//=====================================
//=== End of Basic Wizard Functions ===
//=====================================

void CResinReplacementWizard::FetchMachineCurrentState(CWizardPage* WizardPage)
{
	CResinMainSelectionWizardPage* Page = dynamic_cast<CResinMainSelectionWizardPage *>(WizardPage);

	if (Page->RestoreDefaults)
	{
		LoadAdvancedSettings();
		for (int i = 0; i < Page->Size(); ++i)
		{
			QString resinType;

			resinType = m_ParamMgr->MRW_TypesArrayPerSegment[FIRST_SHARED_SEGMENT + i].Value();
			/* Show the machine's resins found in the long-pipes
			  (and not in the block's chambers themselves).
			  User can't tell if the chamber is flooded or not. */
			Page->VirtTargMaterials[i].SelectedResin = resinType;
			Page->SelectedResinsIdx[i] = Page->ResinNameToIndex(resinType);


			Page->VirtTargMaterials[i].SecondFlushing = false;

			//TODO (Elad): update leftTankIdx/rightTankIdx using GlobalDefs::GetSpecificTank(i,1/2) and not with bitwise operations
			int leftTankIdx = FIRST_MODEL_TANK_TYPE + (i << 1 /* @Assumption: 2 cartridges per chamber */); 
			int rightTankIdx = leftTankIdx + 1;
			bool leftTankResinMatch  = (m_ParamMgr->TypesArrayPerPipe[leftTankIdx ]
				.Value().compare(resinType) == 0);
			bool rightTankResinMatch = (m_ParamMgr->TypesArrayPerPipe[rightTankIdx]
				.Value().compare(resinType) == 0);

			//Check what cartridges contain the current resin in the chamber
			if (leftTankResinMatch && rightTankResinMatch) {
				Page->VirtTargMaterials[i].FlushingBothCartridges = BOTH_CART;
			}
			else {
				if (!leftTankResinMatch && rightTankResinMatch) {
					Page->VirtTargMaterials[i].FlushingBothCartridges = RIGHT_CART;
				} else if (leftTankResinMatch && !rightTankResinMatch) {
					Page->VirtTargMaterials[i].FlushingBothCartridges = LEFT_CART;
				} else {
				/* The resin in the long pipe did not come from any of the short pipes,
				  i.e., "TypesArrayPerPipe" vs. "MRW_TypesArrayPerSegment"
				  parameters mismatch. Let MRW_TypesArrayPerSegment determine the case. */
				 CQLog::Write(LOG_TAG_GENERAL,
					"Error fetching machine's status: wrong Resin types in piping's segments");
				}
			}

			//In case of 2nd flushing - we want to repeat the last flushing type per se
			//if (Page->ForceFlushingEnabled) //always display system's CURRENT settings
			{
				//LoadAdvancedSettings must be called beforehead
				Page->VirtTargMaterials[i].FlushingPurpose =
					(m_ParamMgr->MRW_IsSegmentEconomy[(int)FIRST_SHARED_SEGMENT+i].Value()
					 ? PROPERTIES_CYCLES : TINT_CYCLES);
			}
		}

		//Modify MRW_IsSegmentEconomy if enough printing time has passed since last M.R.
		//LoadAdvancedSettings must be called beforehead
		AdjustFlushingTypeStateByTotalPrintingTime(Page->DirtyPrintingPeriod);

		Page->PrintingMode =
			((HIGH_THERMISTORS_MODE == m_CurrentThermistorsOpMode) || (HIGH_AND_LOW_THERMISTORS_MODE == m_CurrentThermistorsOpMode)
			 ? DM3_MODE : FULL_COLOR_MODE);

		//Handle the Support chamber's settings (instead of leaving it to the Pipeline module)
		//===========================================================
			QString supportResinType = m_ParamMgr->TypesArrayPerChamber[TYPE_CHAMBER_SUPPORT].Value();
/* =====  Relevant only for 1 Support chamber =====
			//TODO (Elad): think if this hiding-the-support feature is needed. if not - delete code.
			//If the user won't be able to change the support resin, then this resin cannot be a cleanser one.
			if (!COND_MULTIPLE_CHOICE_SUPPORT_RESINS)
			{
				QString ModeName;
				m_BackEnd->BeginModesEnumeration();
				while((ModeName = m_BackEnd->GetNextModeName(SUPPORT_DEFAULT_MATERIALS_MODES_DIR)) != "")
				{ // Check if the mode file is not hidden
					if (IS_MODE_HIDDEN(ModeName) == false && m_BackEnd->IsMaterialLicensed(ModeName))
					{
						supportResinType = ModeName;
						break;
					}
				}
			}
*/
            /* Show the machine's resin found in the block because
				it's always the same as the long-pipe's */
			Page->SupportVirtTargMaterial.SelectedResin = supportResinType;
			Page->SupportVirtTargMaterial.SecondFlushing = false;

			//======= Relevant for 1 Support Chamber only ==========
			////IsSystemAfterShutdown checks SL&SR for the default Cleanser resin (i.e. post SDW)
			//Page->SelectedSupportResinsIdx = Page->ResinNameToSupportIndex(supportResinType);
			//Page->SupportVirtTargMaterial.FlushingBothCartridges = BOTH_CART;
				////By request (ALM #1838), we always want to default to Both Cartridges.
				////Originally: ... = IsSystemAfterShutdown() ? BOTH_CART : SINGLE_CART;
			//======= Relevant for 2 Support Chambers ==============
			//Counting the number of Support cartridges, making sure we present the REAL number
			int xtraSupportIdx = Page->GetIndexExtraSupportChamber();
			QString resinType = m_ParamMgr->MRW_TypesArrayPerSegment[FIRST_SHARED_SEGMENT + xtraSupportIdx].Value();
			Page->SelectedResinsIdx[xtraSupportIdx] = Page->ResinNameToSupportIndex(resinType);
			if (false == Page->ActiveChambers[xtraSupportIdx])
				Page->VirtTargMaterials[xtraSupportIdx].FlushingBothCartridges = NO_CART;

			TReplacementCartridges supportReplCart = NO_CART;
			if (Page->ActiveChambers[(int)FIRST_SUPPORT_CHAMBER_INDEX])
			{   //figure out where did the Support resin comes from (which cartridge)
				//TODO (Elad): update leftTankIdx/rightTankIdx using GlobalDefs::GetSpecificTank(i,1/2) and not with bitwise operations
				int leftTankIdx = FIRST_SUPPORT_TANK_TYPE /* @Assumption: 2 cartridges per chamber */;
				int rightTankIdx = leftTankIdx + 1;
				bool leftTankResinMatch  = (m_ParamMgr->TypesArrayPerPipe[leftTankIdx ]
					.Value().compare(supportResinType) == 0);
				bool rightTankResinMatch = (m_ParamMgr->TypesArrayPerPipe[rightTankIdx]
					.Value().compare(supportResinType) == 0);

				//Check what cartridges contain the current resin in the chamber
				if (leftTankResinMatch && rightTankResinMatch) {
					supportReplCart = BOTH_CART;
				}
				else {
					if (!leftTankResinMatch && rightTankResinMatch) {
						supportReplCart = RIGHT_CART;
					} else if (leftTankResinMatch && !rightTankResinMatch) {
						supportReplCart = LEFT_CART;
					} else {
					 CQLog::Write(LOG_TAG_GENERAL,
						"Error fetching machine's status: wrong Resin types in Support piping's segments");
					}
				}
			}

			Page->SupportVirtTargMaterial.FlushingBothCartridges = supportReplCart;
			if ((NO_CART == supportReplCart) && Page->ManualMode)
				Page->SelectedSupportResinsIdx = 0;
			else
				Page->SelectedSupportResinsIdx = Page->ResinNameToSupportIndex(supportResinType);
			//======================================================

			/*
			//[@Assumption] 2 cartridges per Support chamber
			bool leftTankResinMatch  = (m_ParamMgr->TypesArrayPerPipe[FIRST_SUPPORT_TANK_TYPE  ]
				.Value().compare(supportResinType) == 0);
			bool rightTankResinMatch = (m_ParamMgr->TypesArrayPerPipe[FIRST_SUPPORT_TANK_TYPE+1]
				.Value().compare(supportResinType) == 0);

			//Check what cartridges contain the current resin in the chamber
			if (leftTankResinMatch && rightTankResinMatch) {
				Page->SupportVirtTargMaterial.FlushingBothCartridges = BOTH_CART;
			}
			else {
				if (!leftTankResinMatch && rightTankResinMatch) {
					Page->SupportVirtTargMaterial.FlushingBothCartridges = RIGHT_CART;
				} else if (leftTankResinMatch && !rightTankResinMatch) {
					Page->SupportVirtTargMaterial.FlushingBothCartridges = LEFT_CART;
				} else {
				  CQLog::Write(LOG_TAG_GENERAL,
					"Error fetching machine's status: wrong Support resin type in piping's segments");
				}
			}
			*/

		//End of Support chamber's settings
		//==================================================================
		Page->RestoreDefaults = false;
	}
}

//[Deprecated] due to ALM #1838 - defaulting support resin to Both cartridges (static)
bool CResinReplacementWizard::IsSystemAfterShutdown() const
{
	//[@Assumption] 2 cartridges per Support chamber
	bool leftTankResinMatch  = (m_ParamMgr->TypesArrayPerPipe[FIRST_SUPPORT_TANK_TYPE  ]
		.Value().compare(m_SupportCleanserName) == 0);
	bool rightTankResinMatch = (m_ParamMgr->TypesArrayPerPipe[FIRST_SUPPORT_TANK_TYPE+1]
		.Value().compare(m_SupportCleanserName) == 0);

	if (leftTankResinMatch && rightTankResinMatch)
	{
		CQLog::Write(LOG_TAG_GENERAL, "MRW : Machine Shutdown was detected");
		return true;
	}
	return false;
}

bool CResinReplacementWizard::IsChamberNotFloodedByNeighbour(TChamberIndex ChamberID) const
{
	//Check if the dynamic chamber associated with the tank is the given one, ChamberID.
	//ActiveTanks is dynamic; no need for TankToChamber().
	return ( ChamberID == TankToStaticChamber((TTankIndex)m_ParamMgr->ActiveTanks[ChamberID].Value()));
}
bool CResinReplacementWizard::IsValidPipingToChamberResin(TChamberIndex ChamberID) const
{
	//Check if the long pipe's resin match the chamber's resin.
	//This check is relevant in case of Extra chambers for cartridge redundancy,
	//such in the case of an extra Support chamber ("DM6" feature)
	if (FIRST_SUPPORT_CHAMBER_INDEX == ChamberID)
	{	//Support chamber doesn't have a 'long pipe' defined so we go over all 'short' ones
		QString chamResinType = m_ParamMgr->TypesArrayPerChamber[TYPE_CHAMBER_SUPPORT].Value();
		for (int i = (int)FIRST_SUPPORT_SEGMENT; i < LAST_SUPPORT_SEGMENT; ++i)
		{
			if (m_ParamMgr->TypesArrayPerPipe[i].Value().compare(chamResinType) == 0)
				return true;
		}
	}
	else if (NUMBER_OF_CHAMBERS > ChamberID) {
			TSegmentIndex longSeg = ChamberToLongSegment(ChamberID);
			if (longSeg != NUMBER_OF_SEGMENTS) {
				QString pipeResinType =	m_ParamMgr->MRW_TypesArrayPerSegment[longSeg].Value();
				return (m_ParamMgr->TypesArrayPerChamber[ChamberID].Value().compare(pipeResinType) == 0);
			}
		 }

	return false;
}

void CResinReplacementWizard::ReflectAdvancedSettings(CWizardPage* WizardPage, CWizardPage* WizardDestPage)
{
	CResinSelectionAdvancedWizardPage* srcPage = dynamic_cast<CResinSelectionAdvancedWizardPage*>(WizardPage);
	CResinMainSelectionWizardPage* destPage = dynamic_cast<CResinMainSelectionWizardPage*>(WizardDestPage);

	destPage->RestoreDefaults = true;
	destPage->ManualMode = !(srcPage->IsAutomatic());
	destPage->ForceFlushingEnabled = srcPage->IsFlushingMode();
	
	SaveAdvancedSettings();
}

void CResinReplacementWizard::SetResinsAdvancedChoices(CWizardPage* WizardPage)
{
/*
	CResinMainSelectionWizardPage* srcPage = dynamic_cast<CResinMainSelectionWizardPage*>(Pages[rrChooseResinsPage]);
	CResinSelectionAdvancedWizardPage* destPage = dynamic_cast<CResinSelectionAdvancedWizardPage*>(WizardPage);

	destPage->Refresh(); //This will enlist the new items & also select the preset resin
*/
}

void CResinReplacementWizard::LoadAdvancedSettings()
{
//Restore Advanced page's Parameter Manager settings
//   CResinMainSelectionWizardPage* page =
//		dynamic_cast<CResinMainSelectionWizardPage*>(Pages[rrChooseResinsPage]);
}
void CResinReplacementWizard::SaveAdvancedSettings()
{
//Backup Parameter Manager settings relevant only within the Advanced page
//   CResinMainSelectionWizardPage* page =
//		dynamic_cast<CResinMainSelectionWizardPage*>(Pages[rrChooseResinsPage]);
}

void CResinReplacementWizard::SetResinsChoices(CWizardPage* WizardPage)
{
  CResinMainSelectionWizardPage* Page = dynamic_cast<CResinMainSelectionWizardPage *>(WizardPage);
  QString                    ModeName;

  //On page re-entry, don't reload resins list. Load only once.
  //If user messes with the materials files or changes dongle - he needs to restart the wizard.
  if (!Page->ModelResinsList.empty()) return;
  if (!Page->SupportResinsList.empty()) return;
  
  m_BackEnd->BeginModesEnumeration();
  Page->ModelResinsList.push_back(INVALID_RESIN_NAME); //Typically, this is a blank string

  //Add licensed Model materials
  while((ModeName = m_BackEnd->GetNextModeName(MODEL_DEFAULT_MATERIALS_MODES_DIR)) != "")
  {
	// Check if the mode file is not hidden
	if(IS_MODE_HIDDEN(ModeName) == false && m_BackEnd->IsMaterialLicensed(ModeName))
	{
	  // Add to the 'Materials' list
	  Page->ModelResinsList.push_back(ModeName);
	}
  }

  // In case there are no 'Material Files'
  if (Page->ModelResinsList.size() > 1)
  {
	  //Sort the vector, so that the indexes match what the user sees.
	  //[@CAUTION] The indexes might be wrong if the drop-down component itself is sorting the list
	  //		   and some of the resins are inserted after performing the std::sort below !
	  std::sort(Page->ModelResinsList.begin(), Page->ModelResinsList.end());

	  //Add licensed Service model materials (i.e. M.Cleanser) in the end
	  m_BackEnd->BeginModesEnumeration();
	  while((ModeName = m_BackEnd->GetNextModeName(MODEL_SERVICE_MATERIALS_MODES_DIR)) != "")
	  {
		// Check if the mode file is not hidden
		if(IS_MODE_HIDDEN(ModeName) == false && m_BackEnd->IsServiceModelMaterial(ModeName))
		{
		  // Add to the 'Materials' list
		  Page->ModelResinsList.push_back(ModeName);
		}
	  }
  } else
  {
	  Page->ModelResinsList.clear();
	  throw EQException(LOAD_STRING(IDS_RR_NO_MATERIAL_FILES));
  }
   //===  HANDLE SUPPORT RESINS  ===
  //===============================
  m_BackEnd->BeginModesEnumeration();
  Page->SupportResinsList.push_back(INVALID_RESIN_NAME); //a placeholder for an undefined resin name
  while((ModeName = m_BackEnd->GetNextModeName(SUPPORT_DEFAULT_MATERIALS_MODES_DIR)) != "")
  {
	// Check if the mode file is not hidden
	if (IS_MODE_HIDDEN(ModeName) == false && m_BackEnd->IsMaterialLicensed(ModeName))
		Page->SupportResinsList.push_back(ModeName);
  }

  if (Page->SupportResinsList.size() > 1)
  {
	  //Sort the vector, so that the indexes match what the user sees.
	  //[@CAUTION] The indexes might be wrong if the drop-down component itself is sorting the list
	  //		   and some of the resins are inserted after performing the std::sort below !
	  std::sort(Page->SupportResinsList.begin(), Page->SupportResinsList.end());

	  //Add licensed Service model materials (i.e. S.Cleanser) in the end
	  m_BackEnd->BeginModesEnumeration();
	  while((ModeName = m_BackEnd->GetNextModeName(SUPPORT_SERVICE_MATERIALS_MODES_DIR)) != "")
	  {
		// Check if the mode file is not hidden
		if(IS_MODE_HIDDEN(ModeName) == false && m_BackEnd->IsServiceSupportMaterial(ModeName))
		{
		  // Add to the 'Materials' list
		  Page->SupportResinsList.push_back(ModeName);
		}
	  }
  } else
  {
	  Page->SupportResinsList.clear();
	  throw EQException(LOAD_STRING(IDS_RR_NO_MATERIAL_FILES));
  }
}

void CResinReplacementWizard::UpdateSingleTank(TTankIndex Tank, bool isBlocking)
{
   m_BackEnd->UpdateTanks(Tank);
   if (isBlocking)
	  WaitForIdentificationCompletion(Tank);
}

void CResinReplacementWizard::UpdateAllTanks(bool isBlocking)
{
	/*  method 1:
	for (int Tank = FIRST_TANK_TYPE; Tank < LAST_TANK_TYPE; ++Tank)
		UpdateSingleTank(static_cast<TTankIndex>(Tank), isBlocking);
	*/

	//	method 2: (less tank updates for DM1 with chambers 1,2 unchanged.
	//			   also, it's less risky when updating flooding chamber's sibling's tanks second)
	// Forcing an update of every Tank who's availablity status could potentially have been affected by the MRW:
	//	  Updating the Tanks that were directly used by the MRW.
	//	  Updating the sibling Tanks of the former.
	//	  Updating the Tanks of the sibling *Chamber* (flooding)

	int index = 0;
	for (TTanksVector::iterator i = m_TanksForReplacement.begin(); i != m_TanksForReplacement.end(); ++i)
	{   index++;
		if (i->MarkedForReplacement)
		{
			UpdateSingleTank(static_cast<TTankIndex>(i->TankIndex), isBlocking);
			// Updating the sibling Tank
			UpdateSingleTank(GetSiblingTank(static_cast<TTankIndex>(i->TankIndex)), isBlocking);

			// Updating both Tanks of the sibling *Chamber*
			if (i->IsFlooding)
			{
				TChamberIndex Chamber = TankToStaticChamber(static_cast<TTankIndex>(i->TankIndex));
				TChamberIndex SiblingChamber = GetSiblingChamber(Chamber);

				//Bug: ChamberTankRelation is dynamically updated.
				//IsChamberRelatedToTank(..) should refer to the static (physical/DM3) mode. Thus using TankToStaticChamber
				for(int Tank = LAST_TANK_TYPE-1; Tank >= FIRST_TANK_TYPE; Tank--)
					if (TankToStaticChamber(static_cast<TTankIndex>(Tank)) == SiblingChamber)
						UpdateSingleTank(static_cast<TTankIndex>(Tank), isBlocking);
			}
		}
	}


	//creating a sync point (barrier) to avoid sync problems upon updating
	//the active tanks before they are ready
	if (NO_BLOCKING == isBlocking)
	{
		bool isWndFrozen = false;
		unsigned long timeout = QSecondsToTicks(m_ParamMgr->TanksIdentificationTimeoutSec.Value());
		
		for (int Tank = FIRST_TANK_TYPE; Tank < LAST_TANK_TYPE; ++Tank)
		{
			//set the maximum time we're willing to wait till identification isi complete
			unsigned long startingTime = QGetTicks();
			unsigned long endingTime = startingTime + timeout;

			while (false == m_BackEnd->WaitForIdentificationCompletion(
				static_cast<TTankIndex>(Tank),
				true, //true = DontWaitIfTankIsOut
				100)) //time to wait
			{
				YieldWizardThread();
				if (QGetTicks() > endingTime) {
					isWndFrozen = true;
					break; //while loop
				}
			}

			//if the container takes too long to be identified, bail out
			if (isWndFrozen)
			{
				CQLog::Write(LOG_TAG_GENERAL, QFormatStr(
					"Error: Material Replacement Wizard's window seems to be frozen while identifying cartridge %s",
					TankToStr(static_cast<TTankIndex>(Tank)).c_str()
				));
				break; //break out of for loop and skip containers' identification
			}
		}
	}

}

void CResinReplacementWizard::UpdateTypesArrayPerPipe()
{
   // Todo: Shahar: Note: UpdateTypesArrayPerPipe() should be called always after saving MRW_TypesArrayPerSegment

   // There are Segments that are set to UNDEFINED during MRW. Assign those Segments to the TypesArrayPerPipe[] to make visible outside the MRW scope.
   for (int i = LAST_TANK_TYPE-1; i >= FIRST_TANK_TYPE; --i)
	 m_ParamMgr->TypesArrayPerPipe[i] = m_ParamMgr->MRW_TypesArrayPerSegment[GetTankAdjacentSegment((TTankIndex)i)];

   m_ParamMgr->SaveSingleParameter(&m_ParamMgr->TypesArrayPerPipe);
}

void CResinReplacementWizard::UpdateEconomySegments(bool safeChangesOnly)
{
	for (int seg = NUMBER_OF_SEGMENTS-1; seg >= FIRST_SEGMENT; --seg)
	{
		//We need to be more careful when modifing Economy->Full,
		//because if this change isn't committed in the piping - the resin is actually still dirty.
		if (safeChangesOnly &&
			(1 == m_ParamMgr->MRW_IsSegmentEconomy[seg]) &&
			(0 == m_CalculatedEconomySegments[seg]))
			continue;  //DON'T change (i.e. leave it 'Economy')
  
		m_ParamMgr->MRW_IsSegmentEconomy[seg] = m_CalculatedEconomySegments[seg];
	}
	m_ParamMgr->SaveSingleParameter(&m_ParamMgr->MRW_IsSegmentEconomy);
}

void CResinReplacementWizard::CalculateEconomySegments(COptimalTanks* optimized)
{
	for (int seg = NUMBER_OF_SEGMENTS-1; seg >= FIRST_SEGMENT; --seg)
		m_UndefinedSegments[seg] = false;

#ifdef OBJET_MACHINE_KESHET
	//Force all the Support related tanks to be set to "TINT"-oriented washing cycles,
	//in order to avoid any mistakes done in calculating number of cycles and/or displayed info.
	//This is done together with the CResinReplacementPipeline::AddTargetMaterial method's hack.
	TSegmentIndex tseg;
	for (int seg = NUMBER_OF_SEGMENTS-1; seg >= FIRST_SEGMENT; --seg)
	{
		tseg = static_cast<TSegmentIndex>(seg);
		if (IsSegmentRelatedToTank(tseg, SUPPORT_MODEL_TANK_L) || IsSegmentRelatedToTank(tseg, SUPPORT_MODEL_TANK_R) ||
			IsSegmentRelatedToTank(tseg, TYPE_TANK_SUPPORT1) || IsSegmentRelatedToTank(tseg, TYPE_TANK_SUPPORT2))
			m_ParamMgr->MRW_IsSegmentEconomy[seg] = false;
	}
#endif	

	//Firstly, iterate through all inactive tanks.
	//This is to avoid the case where the Left tank is active, the Right isn't
	//and thus we update the LONG-pipe segment with the RIGHT-tank's economy/full state.
	for (int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; ++i)
	{
		TTankIndex currTank = static_cast<TTankIndex>(i);
		//Beware: IsSegmentRelatedToTank looks at the Segment-Tank relation table,
		//which is known to have SL and SR swapped.
		//Refer to GlobalDefs' TTankIndex-related comments and the SUPPORT_PUMP1/SUPPORT_PUMP0 swap
		if (!optimized->IsActive(currTank))
			for (int seg = NUMBER_OF_SEGMENTS-1; seg >= FIRST_SEGMENT; --seg)
				if (IsSegmentRelatedToTank(static_cast<TSegmentIndex>(seg), currTank))
					m_CalculatedEconomySegments[seg] = m_ParamMgr->MRW_IsSegmentEconomy[seg];
	}

	for (int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; ++i)
	{
		TTankIndex currTank = static_cast<TTankIndex>(i);
		if (optimized->IsActive(currTank))
		{
			for (int seg = NUMBER_OF_SEGMENTS-1; seg >= FIRST_SEGMENT; --seg)
				if (IsSegmentRelatedToTank(static_cast<TSegmentIndex>(seg), currTank))
				{
					//default behaviour: set user's picks
					m_CalculatedEconomySegments[seg] = optimized->IsEconomyFlushed(currTank);
				}

			// In case we replacing only one side Full replacement process and the sibling side was marked as "Economy",
			// We need to mark the sibling side segment as UNDEFINED, because "Economy" side might not be pure material
			// and will contaminate the pipe during "hot-swap" 
			if (! optimized->IsActive(GetSiblingTank(currTank)) && ! optimized->IsEconomyFlushed(currTank))
			{
				TSegmentIndex SiblingSeg = TankToShortSegment(GetSiblingTank(currTank));
				if ((m_ParamMgr->MRW_IsSegmentEconomy[SiblingSeg] == 1) &&
				   (optimized->GetResin(currTank).compare(m_ParamMgr->MRW_TypesArrayPerSegment[SiblingSeg]) == 0))
				   m_UndefinedSegments[SiblingSeg] = true;
			}
		}
	}
}

void CResinReplacementWizard::UpdateTypesArrayPerTank()
{
#if defined EMULATION_ONLY
	//in emulation mode, m_BackEnd->GetTankMaterialType() is emulated
	for (TTanksVector::iterator Tank = m_TanksForReplacement.begin(); Tank != m_TanksForReplacement.end(); ++Tank)
	{
		if (Tank->MarkedForReplacement)
		{
			if (0 == Tank->MaterialName.compare(""))
				m_ParamMgr->TypesArrayPerTank[Tank->TankIndex] = NO_TANK_INSERTED;
			else
				m_ParamMgr->TypesArrayPerTank[Tank->TankIndex] = Tank->MaterialName;
		}
	}
#else
	for(int i = LAST_TANK_TYPE-1; i >= FIRST_TANK_TYPE; i--)
	{
		try {
			m_ParamMgr->TypesArrayPerTank[i] = m_BackEnd->GetTankMaterialType(static_cast<TTankIndex>(i));
			//if no resin is inserted in container - write some default value (don't leave it blank! bug in PM)
			if (0 == m_ParamMgr->TypesArrayPerTank[i].Value().compare(""))
				m_ParamMgr->TypesArrayPerTank[i] = NO_TANK_INSERTED;
		} catch (...) {
			m_ParamMgr->TypesArrayPerTank[i] = NO_TANK_INSERTED;
		}
	}
#endif
	m_ParamMgr->SaveSingleParameter(&m_ParamMgr->TypesArrayPerTank);
}

void CResinReplacementWizard::MarkTypesArrayPerChamberUndefined()
{
	TypesArrayPerChamberSetter(MR_Undefine);
}

void CResinReplacementWizard::UpdateTypesArrayPerChamber()
{
	TypesArrayPerChamberSetter(MR_Update);
}

void CResinReplacementWizard::UpdateHostTypesArrayPerChamber()
{
	for (int i = 0; i < NUMBER_OF_CHAMBERS_INCLUDING_WASTE; i++) {
		m_ParamMgr->MRW_HostTypesPerChamber[i] = m_ParamMgr->TypesArrayPerChamber[i];
	}
	m_ParamMgr->SaveSingleParameter(&m_ParamMgr->MRW_HostTypesPerChamber);
}

void CResinReplacementWizard::TypesArrayPerChamberSetter(TMaterialParametersUpdateMode mode)
{
	for(TTanksVector::iterator Tank = m_TanksForReplacement.begin(); Tank != m_TanksForReplacement.end(); ++Tank)
	{
		if(Tank->MarkedForReplacement)
		{
			// Set the Tank's related Chamber to the relevant material
			TChamberIndex Chamber = TankToStaticChamber(static_cast<TTankIndex>(Tank->TankIndex));

			switch(mode)
			{
				case MR_Undefine:
					//Check if the chamber is going to be dirty with another resin during pumping
					if (m_ParamMgr->TypesArrayPerChamber[Chamber].Value().compare(Tank->MaterialName) != 0)
						m_ParamMgr->TypesArrayPerChamber[Chamber] = m_UndefinedStr;
					break;
				case MR_Update:   m_ParamMgr->TypesArrayPerChamber[Chamber] = Tank->MaterialName; break;
				default: break;
			}

			// Flooding. In case we are Flooding from this Tank, we set here also the Tank's "sibling Chamber" to the same material
			if (Tank->IsFlooding)
			{
				TChamberIndex sibChamber = GetSiblingChamber(Chamber);
				switch(mode)
				{
					case MR_Undefine:
						//Check if the chamber is going to be dirty due to an undefined resin originating from the flooding
						//chamber, or due to a resin other than the one in this (sibling) chamber 
						if ((m_ParamMgr->TypesArrayPerChamber[sibChamber].Value().compare(Tank->MaterialName) != 0)
						  ||(m_ParamMgr->TypesArrayPerChamber[Chamber].Value().compare(m_UndefinedStr) != 0))
							m_ParamMgr->TypesArrayPerChamber[sibChamber] = m_UndefinedStr;
						break;
					case MR_Update:   m_ParamMgr->TypesArrayPerChamber[sibChamber] = Tank->MaterialName; break;
					default: break;
				}
			}
		}
	}
	m_ParamMgr->SaveSingleParameter(&m_ParamMgr->TypesArrayPerChamber);
}


void CResinReplacementWizard::MarkTypesArrayPerSegmentUndefined()
{
	for(int Segment = NUMBER_OF_SEGMENTS-1; Segment >= FIRST_SEGMENT; Segment--)
		if(m_UndefinedSegments[Segment])
            m_ParamMgr->MRW_TypesArrayPerSegment[Segment] = m_UndefinedStr;

	// Undefine segments: Set all (relevant) Segments' material to "Undefined"
	TypesArrayPerSegmentSetter(MR_Undefine);
}

void CResinReplacementWizard::UpdateTypesArrayPerSegment()
{
	// Update segments: Set each of the (relevant) Segments' material according to its related Tank
	TypesArrayPerSegmentSetter(MR_Update);
}

void CResinReplacementWizard::TypesArrayPerSegmentSetter(TMaterialParametersUpdateMode mode)
{
	for(TTanksVector::iterator Tank = m_TanksForReplacement.begin(); Tank != m_TanksForReplacement.end(); ++Tank)
	{
		TTankIndex currTank = static_cast<TTankIndex>(Tank->TankIndex);
		TChamberIndex currChamber = TankToStaticChamber(currTank);

		if(Tank->MarkedForReplacement)
			for(int Segment = NUMBER_OF_SEGMENTS-1; Segment >= FIRST_SEGMENT; Segment--)
			{
				if (IsSegmentRelatedToTank(static_cast<TSegmentIndex>(Segment), currTank))
					switch(mode)
					{
						//Mark only the necessary segments as undefined (+relevant chamber),
						//not taking into consideration the sibling chamber
						case MR_Undefine:
							//Check if the segment is going to be dirty during pumping
							if (m_ParamMgr->MRW_TypesArrayPerSegment[Segment].Value().compare(Tank->MaterialName) != 0) {
								m_ParamMgr->MRW_TypesArrayPerSegment[Segment] = m_UndefinedStr;
								m_ParamMgr->TypesArrayPerChamber[currChamber] = m_UndefinedStr;
								//if the short pipe will be dirty, then the long pipe will
								if ((Segment >= FIRST_MODEL_SEGMENT) && ((Segment < FIRST_SHARED_SEGMENT) || (Segment > LAST_SHARED_SEGMENT)))
								{  //this is a short pipe
								   TSegmentIndex ts = ChamberToLongPipe(currChamber);
								   if (NUMBER_OF_SEGMENTS != ts)
								   m_ParamMgr->MRW_TypesArrayPerSegment[ts] = m_UndefinedStr;
								}
							}
							break;
						case MR_Update:
							m_ParamMgr->MRW_TypesArrayPerSegment[Segment] = Tank->MaterialName;
							break;
						default: break;
					}
			}
	}

	m_ParamMgr->SaveSingleParameter(&m_ParamMgr->MRW_TypesArrayPerSegment);
	m_ParamMgr->SaveSingleParameter(&m_ParamMgr->TypesArrayPerChamber);
}

TSegmentIndex CResinReplacementWizard::ChamberToLongPipe(TChamberIndex ChamberID)
{
	switch(ChamberID)
	{
		case TYPE_CHAMBER_MODEL1: return M1_2_SEGMENT;
//OBJET_MACHINE adjustment
	   case TYPE_CHAMBER_MODEL2: return M3_4_SEGMENT;
	   case TYPE_CHAMBER_MODEL3: return M5_6_SEGMENT;
#ifdef OBJET_MACHINE_KESHET
	   case TYPE_CHAMBER_MODEL4: return M7_8_SEGMENT;
	   case TYPE_CHAMBER_MODEL5: return M9_10_SEGMENT;
	   case TYPE_CHAMBER_MODEL6: return M11_12_SEGMENT;
	   case TYPE_CHAMBER_MODEL7: return M13_14_SEGMENT;
#endif
	   default: return NUMBER_OF_SEGMENTS;
    }
}


// Calling BackEnd::WaitForIdentificationCompletion() periodically for not blocking the Wizard's GUI message-processing.
void CResinReplacementWizard::WaitForIdentificationCompletion(TTankIndex Tank)
{
  unsigned StartTime   = QGetTicks();
  unsigned CurrentTime = StartTime;
  unsigned Timeout     = QSecondsToTicks(15);

  while (false == m_BackEnd->WaitForIdentificationCompletion(Tank,
															 true, /*true = DontWaitIfTankIsOut*/
															 100   /*time to wait*/))
  {
	YieldWizardThread();

	CurrentTime = QGetTicks();

	if (CurrentTime > (StartTime + Timeout))
	  return;

	if (IsCancelled())
	  return;
  }
}

const QString CResinReplacementWizard::GetSegmentDescription(TSegmentIndex idx)
{
	return m_ParamMgr->MRW_TypesArrayPerSegment.GetCellDescription(idx);
}

void CResinReplacementWizard::LogSegments()
{
  WriteToLogFile(LOG_TAG_GENERAL,"Segments are:");
  for (int i=0; i<m_ParamMgr->MRW_TypesArrayPerSegment.Size(); i++)
		  WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("%s = %s - %s",
			GetSegmentDescription(static_cast<TSegmentIndex>(i)).c_str(),
			m_ParamMgr->MRW_TypesArrayPerSegment[i].c_str(),
			(m_ParamMgr->MRW_IsSegmentEconomy[i]) ? "Economy" : "Full"));
}

void CResinReplacementWizard::LogBlockStatus()
{
  TTankOperationMode tom = static_cast<TTankOperationMode>(m_ParamMgr->TanksOperationMode.Value());
  WriteToLogFile(LOG_TAG_GENERAL, QFormatStr("Block status: %s",
	m_ParamMgr->TanksOperationMode.GetValueStringList()[tom].c_str()));
  for (int i = 0; i < NUMBER_OF_CHAMBERS; ++i)
  {
	WriteToLogFile(LOG_TAG_GENERAL, QFormatStr("%s Chamber = %s",
	ChamberToStr((TChamberIndex)i).c_str(),
	m_ParamMgr->TypesArrayPerChamber[i].Value().c_str()));
  }
}

TTankIndex CResinReplacementWizard::GetChamberActiveTankID(TChamberIndex Chamber)
{
	TTankIndex ActiveTankID = NO_TANK;

	for(int Tank = LAST_TANK_TYPE-1; Tank >= FIRST_TANK_TYPE; Tank--)
	{
// From all Tanks that are associated with the current 'Chamber',
// try to find a Tank that is participating in this MRW session, and mark it as this Chamber's active Tank.
		if (IsChamberRelatedToTank(Chamber,static_cast<TTankIndex>(Tank)))
			if (IsTankMarkedForReplacement(static_cast<TTankIndex>(Tank)))
				return (static_cast<TTankIndex>(Tank));
	}

	ActiveTankID = m_BackEnd->GetActiveTankNum(static_cast<TChamberIndex>(Chamber));
// No Tank (that is associated with the current 'Chamber') is participating in this MRW session.	
	if (NO_TANK == ActiveTankID)
	{
		QString s = ChamberToStr(Chamber);
		m_CancelReason = QFormatStr("No active %s Compartment available.",s.c_str(),s.c_str(),s.c_str());
		throw EQException(m_CancelReason);
	}

	return ActiveTankID;
}

bool CResinReplacementWizard::IsTankMarkedForReplacement(TTankIndex TankIndex)
{
	for(TTanksVector::const_iterator i = m_TanksForReplacement.begin(); i != m_TanksForReplacement.end(); ++i)
		if(static_cast<TTankIndex>(i->TankIndex) == TankIndex)
			if(i->MarkedForReplacement)
				return true;
	return false;
}

void CResinReplacementWizard::CheckAndShowHowMessage()
{
   	CResinMainSelectionWizardPage* Page = dynamic_cast<CResinMainSelectionWizardPage *>(Pages[rrChooseResinsPage]);
	bool showHowWarning = false;
	for (int i = 0; i < NUMBER_OF_CHAMBERS_INCLUDING_WASTE; ++i)
	{
		if (m_ChamberStateBeforeMRW[i].compare("RGD515") == 0 || m_ParamMgr->TypesArrayPerChamber[i].Value().compare("RGD515") == 0)
			if (m_ChamberStateBeforeMRW[i].compare(m_ParamMgr->TypesArrayPerChamber[i].Value()) != 0)//chamber change
				showHowWarning = true;
	}
	if(showHowWarning)
	{
		QMonitor.WarningMessage(MSG_HOW_WARNING.c_str(), ORIGIN_WIZARD_PAGE);
	}
}
