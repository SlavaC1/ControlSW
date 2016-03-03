/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Quick Single head replacement.                           *
 * Module Description: Quick Single head replacement wizard.        *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 ********************************************************************/

/*********************includes*************************************************/

#include "HeadServiceWizard.h"
#include "ModesDefs.h"
#include "HeadsButtonsEntryPage.h"
#include "MissingNozzlesScaleBasePage.h"
#include "WizardImages.h"
#include "WizardPages.h"
#include "AgreementPage.h"
#include "HeadsCheckBoxesWizardPage.h"
#include "AppParams.h"
#include "BackEndInterface.h"
#include "Q2RTApplication.h"
#include "QFileWithCheckSum.h"
#include "HostMessages.h"
#include "FrontEnd.h"
#include "Configuration.h"
#include "QMonitor.h"
#include "QThreadUtils.h"
#include "HeadReplacementDefs.h"
#include "FEResources.h"
#include <fstream>
#include "QUtils.h"
#include "QScopeRestorePoint.h"
#include "ScatterGenerator.h"
#include "ResinFlowUtils.h"
#include "MaintenanceCountersDefs.h" //itamar
#include "MachineSequencer.h"
#include "TankIdentificationNotice.h"
#include "ParamsDefault.h"
#include "Q2RTApplication.h"

const QString UndefinedMaterialMsg = "The printer has detected an undefined material in row M3 of the materials cabinet, and cannot perform the Head Optimization Wizard.\n\
This usually results when the Material Replacement Wizard was not completed for the material in row M3.\n\
Run the Material Replacement Wizard in DM mode and select the material in row M3 to complete the process, using at least one cartridge of the material in row M3.\n\
Afterwards, you will be able to run the Head Optimization Wizard.";

enum TWizardMode {hswOptimizationMode,hswReplacementMode};
enum {hswCarpetPrint=1,hswWeightTestPrint};//indicate number of z offset sections
                              
T_AxesTable AxesTable = {true, true, true, true};
extern int XPrintOrder[NUMBER_OF_VOLTAGE_MODES][NUMBER_OF_QUALITY_MODES];
extern int YPrintOrder[NUMBER_OF_VOLTAGE_MODES][NUMBER_OF_QUALITY_MODES];

#define LH_BELOW_MINIMUM_FOUND false
#define LH_BELOW_MINIMUM_NOT_FOUND true
#define OVER_MAX_MISSING_NOZZLES_FOUND false
#define OVER_MAX_MISSING_NOZZLES_NOT_FOUND true


/*********************defines**************************************************/

extern const int mapYOffset[];
QString VoltageStrings[NUMBER_OF_VOLTAGE_MODES] = {"Low","High"};

#define HSW_WEIGHT_TEST_FILE_NAME(ch,qm,om) (ChamberToStr((TChamberIndex)ch,true)  + "-" + GetModeShortStr(qm,om) + "- HSW Weight Test.bmp").c_str()
#define NUMBER_OF_CARPET_SLICES	   6
#define CARPET_FILE_NAME		   "Carpet.bmp"
#define TEMPLATE_FILE_NAME		   "Template.bmp"
#define CARPET_MODE_QUALITY_MODE   HQ_INDEX
#define CARPET_VOLTAGE             HIGH_VOLTAGE
#define FULL_PATH(file)           ( Q2RTApplication->AppFilePath.Value() + \
									"CalibrationFiles" + "\\" + GetTitle() + "\\" + file)

/*********************declarations***********************************/
void ExternalAppExecute(const char *FileName,const char *CmdLineParam);

/*********************declarations***********************************/
extern QString YesNoAnswerStrings[];

enum TContinuationMode {hswReplacementAndOptimizationMode, hswAbortMode};

typedef enum 
{
	qrWelcomePage,
	qrWeightProblemPage,
	qrAgreementPage,
	qrVerifyHeadCleaning,
	qrSelectWizardMode,
	qrSelectCalibrationMode,
	// Template printing
	qrInsertTray,
	qrTemplatePrintingPreparation,
	qrTemplatePrinting,
    // Head replacement
	qrSelectHeads,
	qrInsertTrayReplacementMode,
	qrCloseDoorReplacementMode,
    qrHomingAxisReplacementMode,
    qrReplacementLiquidsShortage,
    qrPreparingForReplacement,
	qrReplacementPosition,
    qrReplacementWarning,
	qrReplaceHead,
	qrReInsertHead,

    // Installation check
    qrIntallationCompleteQuery,
    qrRemoveTools,
    qrCloseDoor2ReplacementMode,
   	qrCheckCommunication,
    qrCheckInsertion,
    qrNoCharacterizationData,
    qrHeadsHeating2,
    qrFillBlock,
    qrVacuumLeakage,
	qrPurge,
	qrStressTest,
	qrTestPatternPreparation,
	qrSecondPatternTest,
	qrTestPatternPrinting,
    
    //Heads Cleaning
    qrCloseDoorCleanHeads1,
    qrOptimizeLiquidsShortage,
	qrCleanHeadsHeatingPage,
	qrAreHeadsCleaned,
	qrCheckTray,
	qrCloseDoorCleanHeads2,
    qrCleanHeadsPurge,

    // Optimization
	qrInsertTrayOptimizationMode,
    qrCloseDoorOptimizeMode,
    qrHomingAxisOptimizeMode,
	qrWeightTest,
	qrEnterWeights,
	qrEnterMissingNozzles,
	qrScaleSetup,
	qrScaleCommunicationError,
	qrTargetWeightNotReached,
	qrFindTargetLayer,
	qrFirstFire,
	qrCheckTest,
	qrCloseDoor,
	qrMaterialReplacement,
    qrHomingAxis3,
	qrAdjustHeadsVoltage,
	qrWizardCompleted,
	qrResumeWizard,
	qrCloseDoorResinToPurgeUnit,
	qrPutResinToPurgeUnit,
	qrUVScanning,
	qrUVIgnitionProblemPage,
	qrScaleRemoving,
	qrIllegalMaterialForHSW,
	qrVoltageProblem,
	qrIllegalAmbientTemp,
	qrScaleProblem,
	qrSelectContinuationOption,
	qrWizardCompletedWithoutHeadsReplacement,
	qrLiquidsShortagePage,
	qrPagesCount, // this must element must be the last in this enum
	qrHeadReplacementStart  = qrSelectHeads,
    qrHeadReplacementEnd    = qrPurge,
    qrHeadCleaningStart     = qrHeadReplacementEnd+1,
    qrHeadCleaningEnd       = qrCleanHeadsPurge ,
    qrHeadOptimizationStart = qrHeadCleaningEnd+1,

}THeadServiceWizardPagesIndex;

/********************************************************************/
CHeadServiceWizard::~CHeadServiceWizard(void)
/********************************************************************/
{
  FOR_ALL_HEADS(h)
  {
     FOR_ALL_VOLTAGE_MODES(v)
     {
        delete[] WeightResults[h][v];
		delete[] MissingNozzlesResults[h][v];
		delete[] MarkHeadsToBeReplaced[h][v];
	 }
	 delete[] WeightResults[h];
	 delete[] MissingNozzlesResults[h];
	 delete[] MarkHeadsToBeReplaced[h];
  }
  delete[] WeightResults;
  delete[] MissingNozzlesResults;
  delete[] MarkHeadsToBeReplaced;
  if(m_UVLampsTimer!=NULL)
	delete m_UVLampsTimer;
  delete m_hswScaleBasedData;
  delete m_scale;
}

/********************************************************************/
CHeadServiceWizard::CHeadServiceWizard(void) : CQ2RTSHRWizardBase(IDS_HEAD_SERVICE_WIZARD,"Edit Data")
/********************************************************************/
{

   m_BackEnd->HeadHeatersSetIgnoreError(false);
   m_DuringWeightTestPhase = false;
   m_headReplacementDone = false;

   memset(m_FinalLayerHeight, 0, sizeof(m_FinalLayerHeight));
   memset(m_PrevLayerHeightDPI_um, 0, sizeof(m_FinalLayerHeight));
   memset(m_VoltagePerHeadModel, 0, sizeof(m_VoltagePerHeadModel));
   memset(m_VoltagePerHeadSupport, 0, sizeof(m_VoltagePerHeadSupport));

   m_GoToContinuationPage = false;
   m_IsFirstWizardIteration = false ;
   m_AskRunHeadAlignmentWizard = false ;
   m_DuringHOW = false ;
   m_targetWeightNotReached = false;

   m_PatternTestNeeded = false ;
   memset(m_LayerHeightPerHead, 0, sizeof(m_LayerHeightPerHead));
   memset(m_GainPerHeadSupport, 0, sizeof(m_GainPerHeadSupport));


   memset(m_GainPerHeadModel, 0, sizeof(m_GainPerHeadModel));
   memset(m_HeadsToBeSkipped, 0, sizeof(m_HeadsToBeSkipped));

   memset(LayerHeightGross, 0, sizeof(LayerHeightGross));
   memset(m_PrevHeadPotentiometerValues, 0, sizeof(m_PrevHeadPotentiometerValues));

   memset(m_PrevHeadsVoltagesModel, 0, sizeof(m_PrevHeadsVoltagesModel));
   memset(m_PrevHeadsVoltagesSupport, 0, sizeof(m_PrevHeadsVoltagesSupport));


  SetIndexes(qrVacuumLeakage, qrPurge);
  MarkHeadsToBeReplaced = new bool**[TOTAL_NUMBER_OF_HEADS];
  WeightResults = new float**[TOTAL_NUMBER_OF_HEADS];
  MissingNozzlesResults = new int**[TOTAL_NUMBER_OF_HEADS];
  m_OHDBClient = COHDBProtocolClient::Instance();
  m_hswScaleBasedData =  new HSWScaledBaseData();
  m_hswCalibrationData = new HSWCalibrationData();

  if(m_ParamMgr->HSW_SCALE_EMULATION == true)
	m_scale = new CScalesDummy(m_ParamMgr->HSW_SCALE_PORT);
  else
  	m_scale = new CScales(m_ParamMgr->HSW_SCALE_PORT);
  m_UVLampsTimer = NULL;
  m_bNeedRestartAfterParamsChanged = false;

  FOR_ALL_HEADS(h)
  {
	 MarkHeadsToBeReplaced[h] = new bool*[NUMBER_OF_VOLTAGE_MODES];
	 WeightResults[h] = new float*[NUMBER_OF_VOLTAGE_MODES];
	 MissingNozzlesResults[h] = new int*[NUMBER_OF_VOLTAGE_MODES];
     FOR_ALL_VOLTAGE_MODES(v)
     {
        MarkHeadsToBeReplaced[h][v] = new bool[NUMBER_OF_QUALITY_MODES];
		WeightResults[h][v] = new float[NUMBER_OF_QUALITY_MODES];
        MissingNozzlesResults[h][v] = new int[NUMBER_OF_QUALITY_MODES];
	 }
  }
/*******************************************************************************************************************/
#pragma warn -8066 // Disable ""Unreachable code" warning (MAX_NUM_OF_DOORS)
#pragma warn -8008 // Disable "Condition is always true" warning (MAX_NUM_OF_DOORS)
/*******************************************************************************************************************/

   if(MAX_NUM_OF_DOORS > 1)
   {
	  m_doorIsClosed =  "All doors are closed";
	  m_closeCover =  "Close all the doors";
   }
   else
   {
	   m_closeCover =  "Close the printer cover.";
	   m_doorIsClosed =  "The cover is closed.";
   }
/*******************************************************************************************************************/
#pragma warn .8066 // Enable "Unreachable code" warning (MAX_NUM_OF_DOORS)
#pragma warn .8008 // Enable "Condition is always true" warning (MAX_NUM_OF_DOORS)
/*******************************************************************************************************************/
  TPagePointers pagesVector(qrPagesCount, 0);
  {
    CMessageWizardPage *pPage = new CMessageWizardPage(GetTitle(),HSW_HEADS_PICTURE_ID);
    pPage->SubTitle = "Two options are available when print heads affect\nthe printer’s performance:\
					  - Calibrating print heads for optimum printing \n  - Replacing faulty print heads and calibrating all print\n   heads for optimum printing\
					  Important: Verify that the scale is fully charged,\nor use the power cable with the scale during this procedure.\
					  \nFor more details before running the wizard, click 'Help'.";

	pagesVector[qrWelcomePage] = pPage;
  }
  {
	CCheckBoxWizardPage *pPage = new CCheckBoxWizardPage("Insufficient Material",-1,  wpHelpNotVisible|wpNextWhenSelected);
	pPage->SubTitle = "Confirm before continuing:";
	pagesVector[qrWeightProblemPage] = pPage;
  }
  {
   CAgreementWizardPage *pPage = new CAgreementWizardPage(LOAD_STRING(IDS_WIZARD_CONDITIONS),HEADS_IMAGE_ID,wpHelpNotVisible | wpPreviousDisabled);
	pPage->Label1  = "Stratasys permits you to replace print heads if you agree to the following conditions. By selecting 'I Agree' and clicking Next, you affirm your acceptance of these conditions.\
					   \n\nIf you do not agree to the conditions, contact Stratasys customer support.";
    pPage->Label2  = "          PLEASE READ CAREFULLY";
    pPage->Label3  = "\n1. I will replace print heads only after consulting with an Stratasys-authorized customer-support engineer.";
    pPage->Label4  = "2. I will carefully follow the instructions supplied by Stratasys for replacing print heads.";
    pPage->Label5  = "Replacement instructions are provided with the new print head. You can also find instructions by clicking Help or by referring to the user guide.";
	pagesVector[qrAgreementPage] = pPage;
  }
  {
  //CCheckBoxWizardPage *pPage = new CCheckBoxWizardPage("Are the heads clean?",HSW_HEADS_PICTURE_ID,  wpHelpNotVisible|wpNextWhenSelected);
    CRadioGroupWizardPage *pPage = new CRadioGroupWizardPage(LOAD_STRING(IDS_SYSTEM_ACTION_SELECT),QUESTION_IMAGE_ID,  wpHelpNotVisible|wpNextWhenSelected);
	pPage->SubTitle = "If you are not sure that the heads are clean, cancel this wizard and run the Head Cleaning wizard first.";

	pPage->Strings.Add("Cancel this wizard to clean the heads first.");
	pPage->Strings.Add("Continue with Head Optimization.");
	pPage->DefaultOption = 0;
	pagesVector[qrVerifyHeadCleaning] = pPage;
  }
  {
	CCheckBoxWizardPage *pPage = new CCheckBoxWizardPage(LOAD_STRING(IDS_SYSTEM_CHECKLIST),PREPARATIONS_IMAGE_ID,  wpHelpNotVisible|wpNextWhenSelected);
	pPage->SubTitle = "Confirm before continuing:";
	pPage->Strings.Add("The build tray is empty.");
	pPage->Strings.Add(m_doorIsClosed);

	pagesVector[qrTemplatePrintingPreparation] = pPage;
  }
  {
	CCheckBoxWizardPage *pPage = new CCheckBoxWizardPage("Preparations for pattern test",HSW_PLACE_PAPER_PICTURE_ID,  wpPreviousDisabled |wpNextWhenSelected);
	pPage->SubTitle = "Confirm before continuing:";
	pPage->Strings.Add("A new test pattern paper is properly positioned");
	pPage->Strings.Add(m_doorIsClosed);

	pagesVector[qrTestPatternPreparation] = pPage;
  }
   {
	CElapsingTimeWizardPage *pPage = new CElapsingTimeWizardPage("Printing Frame",HSW_IN_PROGRESS_PICTURE_ID,wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible);
	pagesVector[qrTemplatePrinting] = pPage;
	 }
	 {
	  CCheckBoxWizardPage *pPage = new CCheckBoxWizardPage("Pattern test retry",CAUTION_IMAGE_ID,wpPreviousDisabled | wpHelpNotVisible| wpCancelDisabled );
	  pPage->SubTitle = " There is at least one head with more than 10 missing nozzles";
	  pPage->Strings.Add(m_doorIsClosed);
	  pagesVector[qrSecondPatternTest] =  pPage;
     }
   {
	CCheckBoxWizardPage *pPage = new CCheckBoxWizardPage("Removing the Scale",HSW_REMOVE_RESIN_PICTURE_ID,  wpHelpNotVisible|wpNextWhenSelected| wpPreviousDisabled|wpNoTimeout);
	pPage->SubTitle = "Confirm before continuing:";
	pPage->Strings.Add("The scale and cable are removed");
	pPage->Strings.Add("The tray is clear");
	pPage->Strings.Add(m_doorIsClosed);

	pagesVector[qrScaleRemoving] = pPage;
  }
  {
	CCloseDoorPage *pPage = new CCloseDoorPage(this,HSW_DOOR_PICTURE_ID, wpPreviousDisabled | wpHelpNotVisible);
	pagesVector[qrCloseDoorResinToPurgeUnit] = pPage;
  }
  {
	CCheckBoxWizardPage *pPage = new CCheckBoxWizardPage("Empty the Resin Container",HSW_REMOVE_RESIN_PICTURE_ID,  wpHelpNotVisible|wpNextWhenSelected| wpPreviousDisabled|wpNoTimeout);
	pPage->SubTitle = "Perform the following:";
	pPage->Strings.Add("Empty container into purge unit.");
	pPage->Strings.Add("Return the container to the scale.");
	pPage->Strings.Add(m_doorIsClosed);

	pagesVector[qrPutResinToPurgeUnit] = pPage;
  }
   {
	CElapsingTimeWizardPage *pPage = new CElapsingTimeWizardPage("UV Scanning",HSW_IN_PROGRESS_PICTURE_ID,wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible);
	pagesVector[qrUVScanning] = pPage;
  }
  {
	 CMessageWizardPage *pPage = new CMessageWizardPage(LOAD_STRING(IDS_IGNITION_MALFUNCTION),-1,wpPreviousDisabled | wpHelpNotVisible);
	 pPage->SubTitle = LOAD_STRING(IDS_CHECK_UV_CABLE);
	 pagesVector[qrUVIgnitionProblemPage] = pPage;
  }
  {
	CRadioGroupWizardPage *pPage = new CRadioGroupWizardPage(LOAD_STRING(IDS_SYSTEM_ACTION_SELECT),QUESTION_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled);
	pPage->Strings.Add(LOAD_STRING(IDS_OPTIMIZE_ONLY));
	pPage->Strings.Add(LOAD_STRING(IDS_REPLACE_AND_OPTIMIZE));
	pPage->DefaultOption = 0;
	pagesVector[qrSelectWizardMode] = pPage;
  }
  {
	CRadioGroupWizardPage *pPage = new CRadioGroupWizardPage(LOAD_STRING(IDS_SYSTEM_ACTION_SELECT),QUESTION_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled);
   //DM6 -> HS = High Mix.
	pPage->Strings.Add("High Speed/High Mix");
	pPage->Strings.Add("High Quality");
   //DM6
   //	pPage->Strings.Add("High Mix");
 //	pPage->DefaultOption = 0;
	pagesVector[qrSelectCalibrationMode] = pPage;
  }

  {
	CHeadsCheckboxesWizardPage *pPage = new CHeadsCheckboxesWizardPage(LOAD_STRING(IDS_SELECT_HEADS),PREPARATIONS_IMAGE_ID,wpNextDisabled | wpHelpNotVisible);
	pPage->CheckedArray = m_HeadsToBeReplaced;
    pagesVector[qrSelectHeads] = pPage;
  }
  {
    CInsertTrayPage *pPage = new CInsertTrayPage(this, PREPARATIONS_IMAGE_ID);
	pagesVector[qrInsertTrayReplacementMode] = pPage;
  }
  {
	CInsertTrayPage *pPage = new CInsertTrayPage(this, -1);
	pagesVector[qrInsertTray] = pPage;
  }
  {
	CCloseDoorPage *pPage = new CCloseDoorPage(this,HSW_DOOR_PICTURE_ID, wpPreviousDisabled | wpHelpNotVisible);
	pagesVector[qrCloseDoorReplacementMode] = pPage;
  }
  {
	CHomingAxisPage *pPage = new CHomingAxisPage(this);
    pagesVector[qrHomingAxisReplacementMode] = pPage;
  }
  {
    CCheckBoxWizardPage *pPage = new CLiquidsShortagePage(this,HSW_CARTRIDGE_PICTURE_ID,wpNextWhenSelected | wpPreviousDisabled | wpHelpNotVisible);
    pagesVector[qrReplacementLiquidsShortage] = pPage;
  }
  { // Head replacement
    CElapsingTimeWizardPage *pPage = new CElapsingTimeWizardPage("Preparing for Head Replacement",HSW_IN_PROGRESS_PICTURE_ID,wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible);
    pPage->SubTitle = "This step can take up to 15 min.";
    pagesVector[qrPreparingForReplacement] = pPage;
  }
  {
    CStatusWizardPage *pPage = new CStatusWizardPage("",HSW_IN_PROGRESS_PICTURE_ID,wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible);
	pPage->StatusMessage = "Moving to replacement position";
    pagesVector[qrReplacementPosition] = pPage;
  }
  {
	CMessageWizardPage *pPage = new CMessageWizardPage("Print Head Replacement",HSW_HEADS_OUT_PICTURE_ID,wpNextWhenSelected | wpPreviousDisabled | wpHelpNotVisible | wpNoTimeout);
	pPage->SubTitle = LOAD_STRING(IDS_IMMEDIATE_REPLACEMENT);
	pagesVector[qrReplacementWarning] = pPage;
  }
  {
  CMessageWizardPage *pPage = new CMessageWizardPage("Incompatible Material Inserted",HSW_HEADS_OUT_PICTURE_ID,wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible | wpNoTimeout);
	pagesVector[qrIllegalMaterialForHSW] = pPage;
  }
   {
  CMessageWizardPage *pPage = new CMessageWizardPage("Printer chamber too hot",HSW_HEADS_PICTURE_ID,wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible | wpNoTimeout);
  pPage->SubTitle = "Cancel wizard and wait until the printer chamber is cooler than 40°C.";
  pagesVector[qrIllegalAmbientTemp] = pPage;
  }
  {
  CMessageWizardPage *pPage = new CMessageWizardPage("Scale Problem",HSW_HEADS_PICTURE_ID,wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible | wpNoTimeout);
  pPage->SubTitle = "Cancel wizard , check scale and perform pattern-test.";
  pagesVector[qrScaleProblem] = pPage;
  }

  {
    CCheckBoxWizardPage *pPage = new CCheckBoxWizardPage("Replace print heads",HEAD_ORDER_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled  | wpClearOnShow | wpNoTimeout);
    pPage->SubTitle = "Refer to the instructions included in the head replacement kit, or click 'Help'. Confirm:";
    pagesVector[qrReplaceHead] = pPage;
  }
  {
    CCheckBoxWizardPage *pPage = new CCheckBoxWizardPage("Head Installation Failure",HEAD_IN_OUT_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpFlipButton2Visible | wpHelpNotVisible);
	pPage->SubTitle = "Remove and re-install these heads:";
    pagesVector[qrReInsertHead] = pPage;
  }
  {// Installation check
    CCheckBoxWizardPage *pPage = new CCheckBoxWizardPage("Installation Check",LEVEL_HEADS_IMAGE_ID,wpNextWhenSelected | wpClearOnShow );
	pPage->SubTitle = "Check that the print heads are level and even.";
    pPage->Strings.Add(LOAD_STRING(IDS_DONE));
	pagesVector[qrIntallationCompleteQuery] = pPage;
  }
  {
	CVerifyCleanTrayPage *pPage = new CVerifyCleanTrayPage(this,PREPARATIONS_IMAGE_ID, wpPreviousDisabled | wpHelpNotVisible);
    pagesVector[qrRemoveTools] = pPage;
  }
  {
    CCloseDoorPage *pPage = new CCloseDoorPage(this,HSW_DOOR_PICTURE_ID, wpPreviousDisabled | wpHelpNotVisible);
	pagesVector[qrCloseDoor2ReplacementMode] = pPage;
  }
  //	There is no EEPROM on GEN4 - irrelevant !!
  {
	CElapsingTimeWizardPage *pPage = new CElapsingTimeWizardPage("Head Installation Validation",HSW_IN_PROGRESS_PICTURE_ID,wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible);
	pagesVector[qrCheckCommunication] = pPage;
  }    
  {
    CCheckBoxWizardPage *pPage = new CCheckBoxWizardPage(LOAD_STRING(IDS_NOT_INSERTED_CORRECTLY),HEAD_NOT_INSERTED_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpHelpNotVisible);
    pPage->SubTitle = LOAD_STRING(IDS_REINSERT_HEAD_DRIVER);
    pPage->Strings.Add(LOAD_STRING(IDS_DONE));
    pagesVector[qrCheckInsertion] = pPage;
  }
  {
	CCheckBoxWizardPage *pPage = new CCheckBoxWizardPage(LOAD_STRING(IDS_NO_HEAD_FACTORY_DATA),HEDA_FACTORY_DATA_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpHelpNotVisible);
	pPage->SubTitle = LOAD_STRING(IDS_INSERT_OTHER_HEAD);
	pPage->Strings.Add(LOAD_STRING(IDS_OTHER_HEAD_INSERTED));
	pagesVector[qrNoCharacterizationData] = pPage;
  }
  {
    CElapsingTimeWizardPage* pPage = new CElapsingTimeWizardPage(LOAD_STRING(IDS_HEADS_HEATING),-1,wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible);
	pPage->SubTitle = PROCESS_DURATION(IDN_HEADS_HEATING);
    pagesVector[qrHeadsHeating2] = pPage;
  }
  {
    CElapsingTimeWizardPage *pPage = new CElapsingTimeWizardPage("Filling Heads",HSW_IN_PROGRESS_PICTURE_ID,wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible);
    pPage->SubTitle = STEP_DURATION("5");
    pagesVector[qrFillBlock] = pPage;
  }
  {
	CMessageWizardPage *pPage = new CMessageWizardPage(LOAD_QSTRING(IDS_VACUUM_LEAKAGE_DETECTED),VACUUM_TEST_IMAGE_ID,wpPreviousDisabled | wpHelpNotVisible);
	pPage->SubTitle = LOAD_STRING(IDS_WRONG_HEAD_INSERTION);
	pagesVector[qrVacuumLeakage] = pPage;
  }
  {
	CSuspensionPointsStatusPage *pPage = new CSuspensionPointsStatusPage("",HSW_IN_PROGRESS_PICTURE_ID,wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible);
	pPage->PointsStatusMessage = "Purging";
	pagesVector[qrPurge] = pPage;
  }
  {
	CSuspensionPointsStatusPage *pPage = new CSuspensionPointsStatusPage("",HSW_IN_PROGRESS_PICTURE_ID,wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible);
	pPage->PointsStatusMessage = "Printing";
	pagesVector[qrTestPatternPrinting] = pPage;
  }
  {
	CCloseDoorPage *pPage = new CCloseDoorPage(this,HSW_DOOR_PICTURE_ID, wpPreviousDisabled | wpHelpNotVisible);
	pagesVector[qrCloseDoorCleanHeads1] = pPage;
  }
  {
    CCheckBoxWizardPage *pPage = new CLiquidsShortagePage(this,HSW_CARTRIDGE_PICTURE_ID,wpNextWhenSelected | wpPreviousDisabled | wpHelpNotVisible);
    pagesVector[qrOptimizeLiquidsShortage] = pPage;
  }
  { // Clean heads
    CSuspensionPointsStatusPage *pPage = new CSuspensionPointsStatusPage("Preparing Print Heads",HSW_IN_PROGRESS_PICTURE_ID,wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible);
    pPage->PointsStatusMessage = "Moving to cleaning position";
    pagesVector[qrCleanHeadsHeatingPage] = pPage;
  }
  {
    CCheckBoxWizardPage *pPage = new COneCheckBoxWizardPage(this,IDS_ARE_HEADS_CLEAN,IDS_HEADS_CLEAN, -1, wpPreviousDisabled | wpHelpNotVisible);
    pPage->Title = "Clean the print heads";
    pagesVector[qrAreHeadsCleaned] = pPage;
  }
  {
	CVerifyCleanTrayPage *pPage = new CVerifyCleanTrayPage(this,PREPARATIONS_IMAGE_ID, wpPreviousDisabled | wpHelpNotVisible);
	pagesVector[qrCheckTray] = pPage;
  }
  {
	CCloseDoorPage *pPage = new CCloseDoorPage(this,HSW_DOOR_PICTURE_ID, wpPreviousDisabled | wpHelpNotVisible);
    pagesVector[qrCloseDoorCleanHeads2] = pPage;
  }
  {
    CSuspensionPointsStatusPage *pPage = new CSuspensionPointsStatusPage("",HSW_IN_PROGRESS_PICTURE_ID,wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible);
    pPage->PointsStatusMessage = "Purging";
    pagesVector[qrCleanHeadsPurge] = pPage;
  }
  {
	CInsertTrayPage *pPage = new CInsertTrayPage(this, PREPARATIONS_IMAGE_ID, wpHelpNotVisible);
	pagesVector[qrInsertTrayOptimizationMode] = pPage;
  }
  {
	CCloseDoorPage *pPage = new CCloseDoorPage(this,HSW_DOOR_PICTURE_ID, wpPreviousDisabled | wpHelpNotVisible);
    pagesVector[qrCloseDoorOptimizeMode] = pPage;
  }
  {
    CHomingAxisPage *pPage = new CHomingAxisPage(this, "", -1, wpHelpNotVisible);
    pagesVector[qrHomingAxisOptimizeMode] = pPage;
  }
  {
    CElapsingTimeWizardPage *pPage = new CElapsingTimeWizardPage("Printing Weight Test",HSW_WEIGHT_TEST_PICTURE_ID, wpNextDisabled | wpPreviousDisabled | wpNoTimeout);
#ifdef CONNEX_260
pPage->SubTitle = STEP_DURATION("80");
#else
pPage->SubTitle = STEP_DURATION("55");
#endif

    pagesVector[qrWeightTest] = pPage;
  }
  {
	CHeadsButtonsEntryPage *pPage = new CHeadsButtonsEntryPage("Weight Test Data Entry",SCALES_IMAGE_ID,wpPreviousDisabled | wpFlipButton1Visible | wpNoTimeout);
	pPage->WeightResults         = WeightResults;
	pPage->MissingNozzlesResults = MissingNozzlesResults;
	pPage->MarkHeadsToBeReplaced = MarkHeadsToBeReplaced;
	pPage->HeadsToBeSkipped      = m_HeadsToBeSkipped;
	pagesVector[qrEnterWeights]  = pPage;
  }
   {
	CMissingNozzlesScaleBasePage *pPage = new CMissingNozzlesScaleBasePage("Enter Missing Nozzles",HSW_REMOVE_PAPER_PICTURE_ID,wpPreviousDisabled | wpNoTimeout);
	pagesVector[qrEnterMissingNozzles]  = pPage;
  }
  {
	CSuspensionPointsStatusPage* pPage= new CSuspensionPointsStatusPage("Replacing Material",-1,wpHelpNotVisible | wpNextDisabled | wpPreviousDisabled);
	pPage->PointsStatusMessage = "In Progress...";
	pagesVector[qrMaterialReplacement]  = pPage;
  }
   {
	CElapsingTimeWizardPage* pPage= new CElapsingTimeWizardPage("Stress Test...",-1,wpNextDisabled | wpPreviousDisabled);
	pagesVector[qrStressTest]  = pPage;
   }
  {
  CCheckBoxWizardPage *pPage = new CCheckBoxWizardPage("Preparing the Scale",HSW_SCALE_PLACE_PICTURE_ID,wpNextWhenSelected | wpPreviousDisabled );

	pPage->SubTitle = "Perform the steps in the order below and confirm:";
						
	pPage->Strings.Add("Position scale on tray and connect cable to printer");
	pPage->Strings.Add("Connect scale to power cable");
	pPage->Strings.Add("Press 'I' to turn on the scale (without the container)");
	pPage->Strings.Add("Wait for the scale to display '0.00'");
	pPage->Strings.Add("Place the container on the scale");
	pPage->Strings.Add("Press 'T' to zero the scale with the container");
	pPage->Strings.Add(m_closeCover);
	pagesVector[qrScaleSetup] = pPage;
  }
  {
   CCheckBoxWizardPage *pPage = new CCheckBoxWizardPage("Error: No Communication with Scale",HSW_SCALE_PLACE_PICTURE_ID,wpNextWhenSelected | wpPreviousDisabled );
	pPage->SubTitle = "Confirm before continuing:";
	pPage->Strings.Add("Scale cables connected, scale turned ON");
	pPage->Strings.Add(m_doorIsClosed);
	pagesVector[qrScaleCommunicationError] = pPage;
  }

   {
	CMessageWizardPage *pPage = new CMessageWizardPage("Target Weight can not be reached ",-1,wpPreviousDisabled | wpHelpNotVisible);

	pagesVector[qrTargetWeightNotReached] = pPage;
   }
  {

	CSuspensionPointsStatusPage* pPage= new CSuspensionPointsStatusPage("Find target layer",-1,wpNextDisabled | wpPreviousDisabled |wpNoTimeout| wpHelpNotVisible);
	pagesVector[qrFindTargetLayer]  = pPage;
  }
   {

	CSuspensionPointsStatusPage* pPage= new CSuspensionPointsStatusPage("First Fire",-1,wpNextDisabled | wpPreviousDisabled |wpNoTimeout| wpHelpNotVisible);
	pagesVector[qrFirstFire]  = pPage;
  }
  {

	CSuspensionPointsStatusPage* pPage= new CSuspensionPointsStatusPage("Check current calibration",-1,wpNextDisabled | wpPreviousDisabled |wpNoTimeout| wpHelpNotVisible);
	pagesVector[qrCheckTest]  = pPage;
  }
  {
    CCloseDoorPage *pPage = new CCloseDoorPage(this,HSW_DOOR_PICTURE_ID, wpPreviousDisabled | wpHelpNotVisible);
	pagesVector[qrCloseDoor] = pPage;
  }
  {
    CHomingAxisPage *pPage = new CHomingAxisPage(this, "", -1, wpHelpNotVisible);
    pagesVector[qrHomingAxis3] = pPage;
  }
  {
    CSuspensionPointsStatusPage *pPage = new CSuspensionPointsStatusPage("Calculating Data",HSW_WEIGHT_TEST_PICTURE_ID,wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible);
    pPage->PointsStatusMessage = "Please wait";
    pagesVector[qrAdjustHeadsVoltage] = pPage;
  }
  {
	CMessageWizardPage *pPage = new CMessageWizardPage(WIZARD_COMPLETED_STR,SUCCESSFULLY_COMPLETED_IMAGE_ID, wpDonePage | wpPreviousDisabled | wpHelpNotVisible);
	pagesVector[qrWizardCompleted] = pPage;
  }
  {
    CMessageWizardPage *pPage = new CMessageWizardPage(LOAD_QSTRING(IDS_RESUME_WIZARD),-1, wpPreviousDisabled | wpHelpNotVisible);
    pPage->SubTitle = LOAD_STRING(IDS_PRESS_NEXT_TO_CONTINUE);
	pagesVector[qrResumeWizard] = pPage;
  }
  {
	CRadioGroupWizardPage *pPage = new CRadioGroupWizardPage("Error: High Missing Nozzle Count",HSW_QUESTION_PICTURE_ID,wpNextWhenSelected | wpPreviousDisabled );
    pPage->Strings.Add("Replace defective head(s).");
    pPage->Strings.Add("Abort wizard.");
    pPage->DefaultOption = 0;
    pagesVector[qrSelectContinuationOption] = pPage;
  }
  {
    CMessageWizardPage *pPage = new CMessageWizardPage(WIZARD_COMPLETED_STR,SUCCESSFULLY_COMPLETED_IMAGE_ID, wpPreviousDisabled  | wpDonePage | wpHelpNotVisible);
    pagesVector[qrWizardCompletedWithoutHeadsReplacement] = pPage;
  }
  {
	CCheckBoxWizardPage *pPage = new CLiquidsShortagePage(this,-1,wpNextWhenSelected | wpHelpNotVisible);
	pagesVector[qrLiquidsShortagePage] = pPage;
  }
  {
	CMessageWizardPage *pPage = new CMessageWizardPage("Print Head Voltage Problem",SHD_INFORMATION_PICTURE_ID,wpPreviousDisabled | wpHelpNotVisible | wpCancelDisabled | wpNoTimeout);
	pagesVector[qrVoltageProblem] = pPage;
  }

  for(TPagePointersIterator p = pagesVector.begin(); p != pagesVector.end(); p++)
	 AddPage(*p);

};

// Start the wizard session event
/********************************************************************/
void CHeadServiceWizard::StartEvent()
/********************************************************************/
{
  m_LastStateParam = &m_ParamMgr->HSW_LastState;
  m_IsFirstWizardIteration = true;
  m_RunHeadAlignmentWizard = false;
  m_AskRunHeadAlignmentWizard = false;
  m_VacuumLeakage = false;
  m_DuringHOW = false;
  AxesTable[AXIS_Z] = true;
  m_PatternTestNeeded = true;
  m_bNeedRestartAfterParamsChanged = false;
  m_targetWeightNotReached = false;
  m_performSecondPatternTest = false;
  m_SelectedMode = NO_MODE_SELECTED;
  // Activating the Waste Pump
  m_BackEnd->SetActuatorState(ACTUATOR_ID_WASTE_PUMP,true);

  // Disable vacuum errors
  m_BackEnd->EnableDisableVacuumErrorHandling(false);

  FOR_ALL_MODES(qm, om)
  {
     if (GetModeAccessibility(qm, om))
     {
        m_BackEnd->EnableConfirmationBypass(PER_MACHINE_MODE[qm][om], MACHINE_QUALITY_MODES_DIR(qm,om));
		if (HSW_OPERATION_MODE == om)
            m_BackEnd->EnableConfirmationBypass(CALIBRATE_FOR_HSW[qm], MACHINE_QUALITY_MODES_DIR(qm,om));
		m_BackEnd->GotoDefaultMode();
		m_BackEnd->EnterMode(PRINT_MODE[qm][om],MACHINE_QUALITY_MODES_DIR(qm,om));
		/*save backup layer height*/
		m_PrevLayerHeightDPI_um[qm][om] = m_ParamMgr->LayerHeightDPI_um;
		// Save a backup of the heads voltages
        FOR_ALL_HEADS(i)
        {
			// Save a backup of the potentiometer values
			if(m_ParamMgr->IsModelHead(i))
				m_PrevHeadsVoltagesModel[i][qm][om] = m_ParamMgr->RequestedHeadVoltagesModel[i];
			else
				m_PrevHeadsVoltagesSupport[i][qm][om] = m_ParamMgr->RequestedHeadVoltagesSupport[i];

			m_PrevHeadPotentiometerValues[i][qm][om] = m_ParamMgr->PotentiometerValues[i];
        }
	 }
  }
  m_BackEnd->GotoDefaultMode();
  m_DuringWeightTestPhase = false;
  m_headReplacementDone = false;
  m_UVLampsTimer           = new TTimer(NULL);
  m_UVLampsTimer->Enabled  = false;
  m_UVLampsTimer->Interval = m_ParamMgr->HSW_UVLampsTimeoutSec * 1000;
  m_UVLampsTimer->OnTimer  = UVTurnOFFTimerEvent;
  m_hswScaleBasedData->setFramePrinted(false);
  FOR_ALL_DOUBLE_HEADS(i)
  {
  m_HeadsToBeReplaced[i]         = false;
  m_HeadsInsertedWrong[i]        = false;
  }
  FOR_ALL_HEADS(i)
  {
	  m_HeadsToBeSkipped[i]          = false;
	  m_LayerHeightPerHead[i]        = 0;
	  m_GainPerHeadModel[i]               = 0;
	  m_GainPerHeadSupport[i]               = 0;
	  FOR_ALL_MODES(qm, om)
	  {
		 m_VoltagePerHeadModel[i][qm][om] = 0;
		 m_VoltagePerHeadSupport[i][qm][om] = 0;
	  }
  }
  m_hswCalibrationData->ClearData();
}//StartEvent

// End the wizard session event
/********************************************************************/
void CHeadServiceWizard::EndEvent()
/********************************************************************/
{
  m_ParamMgr->HSW_LastState = qrWelcomePage;
  m_ParamMgr->SaveSingleParameter(&m_ParamMgr->HSW_LastState);
  	//check with which support material HOW was done and reset its flags
		if( m_ParamMgr->TypesArrayPerChamber[TYPE_CHAMBER_SUPPORT].ValueAsString()==SUPPORT_MATERIAL_TYPE[SUP705] )
	{
		m_ParamMgr->HSW_FirstTimeCalibrationFlag[SUP705] = false;
		m_ParamMgr->SaveSingleParameter(&m_ParamMgr->HSW_FirstTimeCalibrationFlag);


	}
	else if( m_ParamMgr->TypesArrayPerChamber[TYPE_CHAMBER_SUPPORT].ValueAsString()==SUPPORT_MATERIAL_TYPE[SUP706] )
	{
		m_ParamMgr->HSW_FirstTimeCalibrationFlag[SUP706] = false;
		m_ParamMgr->SaveSingleParameter(&m_ParamMgr->HSW_FirstTimeCalibrationFlag);
	}
	
  CleanUp();
}

/********************************************************************/
void CHeadServiceWizard::CancelOccurrence(CWizardPage *WizardPage)
{
  if (WizardPage->GetPageNumber() == qrPreparingForReplacement ||
	  WizardPage->GetPageNumber() == qrMaterialReplacement)
  {
         ResinFlowUtils::AirValve av(this);
         av.close();
  	 m_BackEnd->WipeHeads(true);
  	 YieldWizardThread();
	 m_BackEnd->WipeHeads(false);
  }
}
/* Revert parameters*/
void CHeadServiceWizard::RevertParams()
{
	FOR_ALL_MODES(qm, om)
		{
		 if (GetModeAccessibility(qm, om))
		 {
			m_BackEnd->GotoDefaultMode();
			m_BackEnd->EnterMode(PRINT_MODE[qm][om],MACHINE_QUALITY_MODES_DIR(qm,om));
			// Save a backup of  LayerHeightDPI
			m_ParamMgr->LayerHeightDPI_um = m_PrevLayerHeightDPI_um[qm][om];
			// Save a backup of the heads voltages
			for (int i = 0; i < TOTAL_NUMBER_OF_HEADS_HEATERS; i++)
			{
				if(m_ParamMgr->IsModelHead(i))
				{
					m_ParamMgr->RequestedHeadVoltagesModel[i] = m_PrevHeadsVoltagesModel[i][qm][om];
				//		   VoltagesStr += GetHeadName(i) + " voltage: " + QFloatToStr(m_ParamMgr->RequestedHeadVoltagesModel[i]) + "V\n";
				}
				else
				{
					m_ParamMgr->RequestedHeadVoltagesSupport[i] = m_PrevHeadsVoltagesSupport[i][qm][om];
			//		VoltagesStr += GetHeadName(i) + " voltage: " + QFloatToStr(m_ParamMgr->RequestedHeadVoltagesSupport[i]) + "V\n";
				}
			 m_ParamMgr->PotentiometerValues[i] = m_PrevHeadPotentiometerValues[i][qm][om];
			}
		 }
		 m_ParamMgr->SaveSingleParameter(&m_ParamMgr->RequestedHeadVoltagesModel);
		m_ParamMgr->SaveSingleParameter(&m_ParamMgr->RequestedHeadVoltagesSupport);
		m_ParamMgr->SaveSingleParameter(&m_ParamMgr->PotentiometerValues);
		m_ParamMgr->SaveSingleParameter(&m_ParamMgr->LayerHeightDPI_um);
		}
}
// Cancel the wizard session event
/********************************************************************/
void CHeadServiceWizard::CancelEvent(CWizardPage *WizardPage)
/********************************************************************/
{
  if (m_ParamMgr->KeepUvOn)
     m_ParamMgr->KeepUvOn = false;
  if (m_ParamMgr->KeepDoorLocked)
	 m_ParamMgr->KeepDoorLocked = false;

  //Avoiding canceling the wizard more then one time in a wizard life time.
  if (!m_BackEnd->IsPowerTurnedOn())
  	 TurnPowerOn();
  CancelOccurrence(WizardPage);
   //Check in which pages no need to revert the parameters values
  if(WizardPage->GetPageNumber() != qrCloseDoorResinToPurgeUnit &&
	  WizardPage->GetPageNumber() != qrPutResinToPurgeUnit	   &&
	  WizardPage->GetPageNumber() != qrUVScanning &&
	  WizardPage->GetPageNumber() != qrUVIgnitionProblemPage	   &&
	  WizardPage->GetPageNumber() != qrScaleRemoving &&
	  WizardPage->GetPageNumber() != qrSelectContinuationOption	   &&
	  WizardPage->GetPageNumber() != qrWizardCompletedWithoutHeadsReplacement)
  {
	 RevertParams();
  }
   else
  {
	SaveHighMixCalibrationToHighSpeed();

	if(m_AskRunHeadAlignmentWizard)
		 QMonitor.NotificationMessage("The wizard has optimized the print head parameters.\nClick ‘OK’ to restart the printer application. \nAfter the application restarts, run the Head Alignment Wizard.",ORIGIN_WIZARD_PAGE);
	 else
		 QMonitor.NotificationMessage("The wizard has optimized the print head parameters.\nClick ‘OK’ to restart the printer application.",ORIGIN_WIZARD_PAGE);
	 m_AskRunHeadAlignmentWizard = false;
	 m_bNeedRestartAfterParamsChanged = true;
  }
  m_BackEnd->GotoDefaultMode();
  m_RunHeadAlignmentWizard = false;
   //1. UV Turn off
  m_BackEnd->TurnUVLamps(false);
  if(m_DuringHOW == true)
	if(m_BackEnd->CheckIfDoorIsClosed() == Q_NO_ERROR)
	   if(m_BackEnd->EnableDoor(true,true) == Q_NO_ERROR)
	{
	 CSuspensionPointsStatusPage* pPage= dynamic_cast< CSuspensionPointsStatusPage *>(Pages[qrFindTargetLayer]);
	 pPage->PointsStatusMessage = "";
	 if (m_BackEnd->IsDoorLocked())
	 {
		 EnableAxeAndHome(AXIS_Y, true, true);
		 EnableAxeAndHome(AXIS_X, true, true);
	 }
	 m_DuringHOW = false;
	 }
  CleanUp();
}
void CHeadServiceWizard::SaveHighMixCalibrationToHighSpeed()
{
//DM 6
	// in case HighMix - save the same parameters to HS
	if(m_hswScaleBasedData->GetCurrentQualityMode() == HS_INDEX  &&
		m_hswScaleBasedData->GetCurrentOperationMode() == DIGITAL_MATERIAL_OPERATION_MODE)      // HighMix
		{
		EnterMode(HS_INDEX,SINGLE_MATERIAL_OPERATION_MODE); /* go to HS mode*/
		FOR_ALL_HEADS(i)
			{
				m_ParamMgr->PotentiometerValues[i] = m_hswCalibrationData->GetHeadPotentiometerValues(i);
				m_ParamMgr->RequestedHeadVoltagesModel[i] = m_hswCalibrationData->GetHeadsVoltagesModel(i);
				m_ParamMgr->RequestedHeadVoltagesSupport[i] = m_hswCalibrationData->GetHeadsVoltagesSupport(i);
			}
		m_ParamMgr->LayerHeightDPI_um =  m_hswCalibrationData->GetLayerHeightDPI_um();
		m_ParamMgr->SaveSingleParameter(&m_ParamMgr->RequestedHeadVoltagesModel);
		m_ParamMgr->SaveSingleParameter(&m_ParamMgr->RequestedHeadVoltagesSupport);
		m_ParamMgr->SaveSingleParameter(&m_ParamMgr->PotentiometerValues);
		m_ParamMgr->SaveSingleParameter(&m_ParamMgr->LayerHeightDPI_um);
		}
}
/********************************************************************/
void CHeadServiceWizard::HelpEvent(CWizardPage *WizardPage)
/********************************************************************/
{
	Application->HelpFile = DefaultHelpFileName().c_str();

	switch(WizardPage->GetPageNumber())
	{
		case qrWelcomePage: Application->HelpContext(10); break;
		case qrSelectWizardMode: Application->HelpContext(20); break;
		case qrTestPatternPreparation: Application->HelpContext(30); break;
		case qrEnterMissingNozzles: Application->HelpContext(40); break;
		case qrSelectContinuationOption: Application->HelpContext(50); break;
		case qrReplaceHead: Application->HelpContext(60); break;
		case qrIntallationCompleteQuery: Application->HelpContext(70); break;
		case qrScaleSetup: Application->HelpContext(80); break;
		case qrScaleCommunicationError: Application->HelpContext(90); break;
        default: break;
	}
}

/********************************************************************/
void CHeadServiceWizard::UserButton1Event(CWizardPage *WizardPage)
/********************************************************************/
{
  switch(WizardPage->GetPageNumber())
  {
	case qrEnterWeights:
	{
		 CHeadsButtonsEntryPage *Page = dynamic_cast<CHeadsButtonsEntryPage *>(WizardPage);
		 Page->EnterEditModeEvent();
		 Page->Refresh();
		 break;
	}

  }
}

/********************************************************************/
void CHeadServiceWizard::UserButton2Event(CWizardPage *WizardPage)
/********************************************************************/
{
  switch(WizardPage->GetPageNumber())
  {
    case qrReplaceHead:
    case qrReInsertHead:
    {
         QString FileName = Q2RTApplication->AppFilePath.Value() + LOAD_STRING(IDS_HELP_MOVIE_FILE_NAME);
         ExternalAppExecute(FileName.c_str(),"");
         break;
    }
  }
}

/********************************************************************/
void CHeadServiceWizard::AgreementPageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason)
/********************************************************************/
{
  CAgreementWizardPage* Page = dynamic_cast<CAgreementWizardPage*>(WizardPage);
  if (!Page->GetAgreement())
	 throw EQException("You did not agree to the End User License Agreement.");

  // Checking if the print start parameters in X and Y axes are OK
  float XMaxPosInStep = CONFIG_ConvertXmmToStep(FLOAT_FROM_RESOURCE(IDN_HSW_TRAY_START_POSITION_X_OFFSET)*INT_FROM_RESOURCE(IDN_HSW_MAX_PRINT_NUM_IN_AXIS_X) +
                                                m_ParamMgr->TrayStartPositionX);
  float YMaxPosInStep = CONFIG_ConvertXmmToStep(FLOAT_FROM_RESOURCE(IDN_HSW_TRAY_START_POSITION_Y_OFFSET)*(NUMBER_OF_QUALITY_MODES*NUMBER_OF_VOLTAGE_MODES + 1) +
												m_ParamMgr->TrayStartPositionY);

  if (YMaxPosInStep > m_ParamMgr->MaxPositionStep[AXIS_Y])
     throw EQException("Tray start Y position is not properly calibrated. Contact the customer support");

  if (XMaxPosInStep > m_ParamMgr->MaxPositionStep[AXIS_X])
     throw EQException("Tray start X position is not properly calibrated. Contact the customer support");

  EnableDisableNext(false);
  m_BackEnd->TurnUVLamps(false);                
}//AgreementPageLeave

/********************************************************************/
void CHeadServiceWizard::SelectWizardModePageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason)
/********************************************************************/
{
        
        InitHeadsToBeReplacedArray();// bug 3424
	CRadioGroupWizardPage *Page = dynamic_cast<CRadioGroupWizardPage *>(WizardPage);
	m_DuringHOW = true;
	m_headReplacementDone = false;
	if (Page->GetSelectedOption() == hswReplacementMode)
	{
		WriteToLogFile(LOG_TAG_GENERAL,"Replacement and optimization option selected");
		m_headReplacementDone = true;
		SetNextPage(qrHeadReplacementStart);
	}
	else     //Optimize mode
	{
		WriteToLogFile(LOG_TAG_GENERAL,"Optimization option selected");

		StartAsyncHeatingCycle(TOTAL_NUMBER_OF_HEATERS, true);
		m_hswScaleBasedData->SetStartHeatingTime(QGetTicks());

	}
}

void CHeadServiceWizard::SelectCalibrationModePageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason)
/********************************************************************/
{
	int HIGH_SPEED = 0;
	//int HIGH_QUALITY = 1;
//	int HIGH_MIX = 2;
// Before DM6
// 	  if(dynamic_cast<CRadioGroupWizardPage *>(WizardPage)->GetSelectedOption() == HIGH_SPEED)
//   {
//		EnterMode(HS_INDEX,SINGLE_MATERIAL_OPERATION_MODE); /* go to HS mode*/
//   }
//   else if (dynamic_cast<CRadioGroupWizardPage *>(WizardPage)->GetSelectedOption() == HIGH_QUALITY)
//   {
//		EnterMode(HQ_INDEX,DIGITAL_MATERIAL_OPERATION_MODE); /* go to Hq mode*/
//   }
//   else
//		EnterMode(HS_INDEX,DIGITAL_MATERIAL_OPERATION_MODE ); /*High Mix */
// For DM6
	if(dynamic_cast<CRadioGroupWizardPage *>(WizardPage)->GetSelectedOption() == HIGH_SPEED)
	{
	   EnterMode(HS_INDEX,DIGITAL_MATERIAL_OPERATION_MODE ); /*High Mix */
	   WriteToLogFile(LOG_TAG_GENERAL,"HS/HM was selected");
	   m_SelectedMode = HS_HM;
	}
	 else
	 {
	   EnterMode(HQ_INDEX,DIGITAL_MATERIAL_OPERATION_MODE); /* go to Hq mode*/
	   WriteToLogFile(LOG_TAG_GENERAL,"HQ was selected");
	   m_SelectedMode = HQ;
	 }
	   
	SetNextPage(qrTemplatePrintingPreparation);
}
/********************************************************************/
void CHeadServiceWizard::EnterMode(int qualityMode,int operationMode)
{
	m_BackEnd->GotoDefaultMode();
	m_BackEnd->EnterMode(PRINT_MODE[qualityMode][operationMode],MACHINE_QUALITY_MODES_DIR(qualityMode,operationMode));
	m_hswScaleBasedData->SetCurrentOperationMode(operationMode);
	m_hswScaleBasedData->SetCurrentQualityMode(qualityMode);
}
/********************************************************************/
void CHeadServiceWizard::TemplatePrintingPreparationPageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason)
/********************************************************************/
{
	   if( ! m_BackEnd->IsTrayInserted())
   {
	  SetNextPage(qrInsertTray);
   }
   //1. Check if Door is closed
   else if(m_BackEnd->CheckIfDoorIsClosed() == Q_NO_ERROR)
	 {
				if(m_BackEnd->EnableDoor(true,true) != Q_NO_ERROR)
				{
				  CCheckBoxWizardPage* CheckBoxPage = dynamic_cast<CCheckBoxWizardPage *>(Pages[qrTemplatePrintingPreparation]);
				  CheckBoxPage->SetChecksMask(1);
				  SetNextPage(qrTemplatePrintingPreparation);
				}
				else
				{

				  //for test only!!!!!!!!!!!!!
				 //  2. home for X,Y,Z,T
				  EnableAllAxesAndHome();
				 // 3 . Perform Purge
				 //  SetNextPage(qrPurge);
				   SetNextPage(qrTemplatePrinting);
				}
	 }

	 else
	 {
	   CCheckBoxWizardPage* CheckBoxPage = dynamic_cast<CCheckBoxWizardPage *>(Pages[qrTemplatePrintingPreparation]);
	   CheckBoxPage->SetChecksMask(1);
	   SetNextPage(qrTemplatePrintingPreparation);
	 }
}
void CHeadServiceWizard::ResumeWizardPageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason)
/********************************************************************/
{
  switch (m_ParamMgr->HSW_LastState)
  {
	 case qrSelectHeads:
	 case qrReplaceHead:
	 case qrReInsertHead:
	 case qrIntallationCompleteQuery:
	 case qrRemoveTools:
	 case qrCloseDoor2ReplacementMode:
	 case qrCheckInsertion:
	 case qrNoCharacterizationData:
	 	break;

	 default:
		EnableDisableNext(false);
	 	EnableAllAxesAndHome();
	 	EnableDisableNext(true);
	 	break;
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
void CHeadServiceWizard::SelectContinuationOptionPageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason)
{
  CRadioGroupWizardPage *Page = dynamic_cast< CRadioGroupWizardPage* >(WizardPage);
  if (hswAbortMode == Page->GetSelectedOption())
  {
	  WriteToLogFile(LOG_TAG_GENERAL,"Abort option was selected");
	  throw EQException(m_ContinuationOptionPageSubtitle);
  }
  else
  {
	  m_IsFirstWizardIteration = false; // Starting second and above iteration of the wizard
	  WriteToLogFile(LOG_TAG_GENERAL,"Replacement and optimization option was selected");
	  SetNextPage(qrSelectHeads);
  }
}//SelectContinuationOptionPageLeave
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
void CHeadServiceWizard::PageEventsHandler(CWizardPage *WizardPage,int Param1,int Param2)
{
    /*THeadServiceWizardPagesIndex PageIndex = (THeadServiceWizardPagesIndex)WizardPage->GetPageNumber();
  switch(PageIndex)
  {
    case qrWizardCompleted: // Mark whether to run 'Head Alignment Wizard'
         m_RunHeadAlignmentWizard =
           (ANSWER_YES == dynamic_cast<CRadioGroupWizardPage *>(WizardPage)->GetSelectedOption());
		 break;
  }*/

}

///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
void CHeadServiceWizard::PageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason)
{

  THeadServiceWizardPagesIndex PageIndex = (THeadServiceWizardPagesIndex)WizardPage->GetPageNumber();
  CMachineSequencer *Sequencer = Q2RTApplication->GetMachineManager()->GetMachineSequencer();

  if (LeaveReason == lrCanceled)
     CancelOccurrence(WizardPage);
  if (LeaveReason != lrGoNext)
     switch(PageIndex)
     {        
        case qrEnterWeights:
             break;
        default:
             return;
     }

  try
  {
     
   switch(PageIndex)
   {
	case qrWelcomePage: //bug 6483
	  if(Sequencer->GetCurrentTemperatureCelcius()>40)
	  {
	  CQLog::Write(LOG_TAG_GENERAL,"Ambient temperature more than 40 C");
		SetNextPage(qrIllegalAmbientTemp);
	  }
	  else if(CheckIfMaterialsAreLegal() == false)
	  {
		SetNextPage(qrIllegalMaterialForHSW);
	  }
	  else
	  if(CheckLiquidWeightAccordingMode() == false)
	  {
		SetNextPage(qrWeightProblemPage);
	  }
	  else
	  {

		   SetNextPage(qrAgreementPage);
	  }
		 break;
	case qrWeightProblemPage:
		if(CheckLiquidWeightAccordingMode() == false)
		{
		  SetNextPage(qrWeightProblemPage);
		}
		 break;
	case qrAgreementPage:
			// Save current material data for each head
		  SaveCurrentData();
		  AgreementPageLeave(WizardPage, LeaveReason);
		 break;

	case qrSelectWizardMode:
		 SelectWizardModePageLeave(WizardPage, LeaveReason);
		 break;
	case qrSelectCalibrationMode:
		 SelectCalibrationModePageLeave(WizardPage, LeaveReason);
		 break;
	case qrInsertTrayOptimizationMode :
		 SetNextPage(qrSelectWizardMode);
		 break;
	case qrInsertTray :
		 SetNextPage(qrTemplatePrintingPreparation);
		 break;	 
	case qrTemplatePrintingPreparation:
		 TemplatePrintingPreparationPageLeave(WizardPage, LeaveReason);
		 break;
    case qrCheckInsertion:
		 SetNextPage(qrCloseDoor2ReplacementMode);
         break;
	case qrNoCharacterizationData:
		 SetNextPage(qrRemoveTools);
		 break;
		 
    case qrReplaceHead:
         ReplaceHeadPageLeave(WizardPage);
         break;

    case qrRemoveTools:
     	 TurnPowerOn();
         break;

    case qrResumeWizard:
		 ResumeWizardPageLeave(WizardPage, LeaveReason);
		 break;

    case qrAreHeadsCleaned:
         StartAsyncHeatingCycle(TOTAL_NUMBER_OF_HEATERS, true);
         break;
         
	case qrWeightTest:
         m_BackEnd->EnableMotor(true, AXIS_Z);
         m_BackEnd->MoveMotorToZTrayOutPosition(); //At the end of all printing moving z manually to original ZEnd param value
		 m_BackEnd->WaitForEndOfMovement(AXIS_Z, Z_DOWN_WAIT_TIME);

	  QDeleteFile(FULL_PATH(CARPET_FILE_NAME));
		 for(int ch = FIRST_CHAMBER_TYPE; ch < OperationModeToBitmapsNums(HSW_OPERATION_MODE); ch++)
			FOR_ALL_QUALITY_MODES(qm)
			QDeleteFile(FULL_PATH(HSW_WEIGHT_TEST_FILE_NAME(ch,qm,HSW_OPERATION_MODE)));
      
		 dynamic_cast<CHeadsButtonsEntryPage*>(GetPageById(qrEnterWeights))->Nulify();
         if (LeaveReason == lrGoNext)
            SetNextPage(qrEnterWeights);
         break;

	case qrSelectContinuationOption:
		 SelectContinuationOptionPageLeave(WizardPage, LeaveReason);
		 break;
	case qrEnterMissingNozzles:
		 EnterMissingNozzlesPageLeave(WizardPage, LeaveReason);
		break;
	case qrScaleSetup:
		ScaleSetupPageLeave(WizardPage, LeaveReason);
		break;
	case qrScaleCommunicationError:
		ScaleCommunicationPageLeave(WizardPage, LeaveReason);
		break;
	 case qrScaleRemoving:
		 ScaleRemovingPageLeave(WizardPage);
		 break;
	  case qrPutResinToPurgeUnit:
	 {
	   PutResinToPurgeUnitPageLeave(WizardPage, LeaveReason);
		break;
	 }
	 case qrVerifyHeadCleaning:
		 VerifyHeadCleaningPageLeave(WizardPage, LeaveReason);
	  break;	 
	 case qrTestPatternPreparation:
		TestPatternPreparationPageLeave(WizardPage, LeaveReason);
		break;
	 case qrSecondPatternTest :
		SecondPatternTestReqPageLeave(WizardPage, LeaveReason);
		break;
	 case qrUVIgnitionProblemPage:
		SetNextPage(qrUVScanning);
		break;
	 case qrTargetWeightNotReached:
		TargetWeightNotReachedPageLeave(WizardPage, LeaveReason);
		break;
	 case qrVoltageProblem:
		VoltageProblemPageLeave(WizardPage, LeaveReason);
		break;
	 case qrFindTargetLayer:
		 CSuspensionPointsStatusPage* pPage= dynamic_cast< CSuspensionPointsStatusPage *>(Pages[qrFindTargetLayer]);
		 pPage->PointsStatusMessage = "";
		break;
  }
   }
  catch(EQHeadFillingException& E)
  {
	SetNextPage(qrLiquidsShortagePage);
	return;
  }
} // PageLeave
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
bool CHeadServiceWizard::CleanHeadsPurgePageEnter(CWizardPage *WizardPage)
{
    m_BackEnd->GotoDefaultMode();
    m_BackEnd->EnterMode(PRINT_MODE[CARPET_MODE_QUALITY_MODE][HSW_OPERATION_MODE],
                         MACHINE_QUALITY_MODES_DIR(CARPET_MODE_QUALITY_MODE,HSW_OPERATION_MODE));
	EnableAllAxesAndHome(NULL,NO_BLOCKING);			

	//Turn heating back on
	HeadsHeatingCycle();

	TQErrCode Err = Q_NO_ERROR;
	TRY_FUNCTION(Err, m_BackEnd->WaitForEndOfMovement(AXIS_Z, Z_DOWN_WAIT_TIME));
	TRY_FUNCTION(Err, m_BackEnd->WaitForEndOfMovement(AXIS_Y));
    TRY_FUNCTION(Err, m_BackEnd->WaitForEndOfMovement(AXIS_X));
    TRY_FUNCTION(Err, m_BackEnd->WaitForEndOfMovement(AXIS_T));

	// Ignore 'Temperature' errors at the filling stage...
	m_BackEnd->SetReportTemperatureError(false);
	CBEMonitorActivator MonitorActivator; //meaning m_BackEnd->TurnHeadFilling(true);
	HeadsFillingCycle(m_ParamMgr->HeadsHeatingTimeoutSec);

	// Goto Purge Position
	TRY_FUNCTION(Err, m_BackEnd->GotoPurgePosition(true, true));
	for (int i = 0; i < m_ParamMgr->CleanHeadWizardNumOfPurges; i++)
	{
		m_BackEnd->Purge(false, false);
		if (Q2RTWizardSleep(m_ParamMgr->MotorPostPurgeTimeSec) == false)
		   return false;
	}
	YieldWizardThread();

	if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();

	m_BackEnd->WipeHeads(false);
	YieldWizardThread();

	m_BackEnd->WipeHeads(false);
	YieldWizardThread();

	// Home All Axis
	EnableAllAxesAndHome();

	WriteToLogFile(LOG_TAG_GENERAL,"'Heads cleaning' stage is finished.");
        m_BackEnd->ResetMaintenanceCounter(LAST_HCW_ACTIVATION_TIME_COUNTER_ID);  //itamar, Super purge
	GotoNextPage();
    return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////
bool CHeadServiceWizard::TestPatternPrintingPageEnter(CWizardPage *WizardPage,bool homeZ)
{
   //	if (PurgePageEnter(WizardPage, 0, 0, AxesTable) == false)
   //		return false;
	WriteToLogFile(LOG_TAG_GENERAL,"Starting 'Test pattern printing.");
	 //1. Print Test Pattern
	bool rc = PrintTestPattern(homeZ);
	 //2. Print template  TBD
	 //3.Open door
	 // go to prev. mode ,because testPattern changing mode to default
	 EnterMode(m_hswScaleBasedData->GetCurrentQualityMode(),m_hswScaleBasedData->GetCurrentOperationMode());
	 // turn on the heaters (at the end of pattern test heaters switched to Off)
	 StartAsyncHeatingCycle(TOTAL_NUMBER_OF_HEATERS, true);
     m_PatternTestNeeded = false;
	 return rc;
}
bool CHeadServiceWizard::PrintTestPattern(bool homeZ)
{
  if(homeZ == true)
  {
	m_BackEnd->EnableMotor(true, AXIS_Z);
	m_BackEnd->MoveMotorToZStartPosition();
	m_BackEnd->WaitForEndOfMovement(AXIS_Z, Z_DOWN_WAIT_TIME);
  }
  m_BackEnd->DoTestPattern(false,false,false);
  
  m_BackEnd->SetOnlineOffline(true);
  if (WaitForStateToDiff(msTestPattern) == msStopped)
     return false;

  TMachineState State;
  do
  {
	State = m_BackEnd->GetCurrentMachineState();
	YieldAndSleepWizardThread();
    if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
  }
  while(State == msTestPattern || State == msStopping || State == msStopped);

  return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////
bool CHeadServiceWizard::CleanHeadsHeatingPageEnter(CWizardPage *WizardPage)
{
	WriteToLogFile(LOG_TAG_GENERAL,"Starting 'Heads cleaning' stage.");

	//If replacement was performed, no need in moving Z axe (homing up and back down for cleaning)
	EnableAllAxesAndHome(AxesTable);

	//moving to replacement position in order to clean the heads

    TQErrCode Err = Q_NO_ERROR;

    if (AxesTable[AXIS_Z]) //this indicates whether head replacement wa performed
       TRY_FUNCTION(Err, m_BackEnd->MoveZAxisDown());
	TRY_FUNCTION(Err, m_BackEnd->MoveMotorToAbsolutePosition(AXIS_Y, m_ParamMgr->YCleanHeadsPosition, NO_BLOCKING, muMM));
	TRY_FUNCTION(Err, m_BackEnd->MoveMotorToAbsolutePosition(AXIS_X, m_ParamMgr->XCleanHeadsPosition, NO_BLOCKING, muMM));
        m_BackEnd->HeadHeatersSetIgnoreError(true);  //ignoring head error during HSW
        HeadsOnlyHeatingCycle(DEFAULT_TIMEOUT,NULL,true); //true= if one of the head is short/open circuit, application keeps heating (except the short/open head)
        m_BackEnd->HeadHeatersSetIgnoreError(false);
    if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
    if (AxesTable[AXIS_Z])
       TRY_FUNCTION(Err, m_BackEnd->WaitForEndOfMovement(AXIS_Z, Z_DOWN_WAIT_TIME));
    TRY_FUNCTION(Err, m_BackEnd->WaitForEndOfMovement(AXIS_Y));
	TRY_FUNCTION(Err, m_BackEnd->WaitForEndOfMovement(AXIS_X));

	m_BackEnd->LockDoor(false);
    AxesTable[AXIS_Z] = true;
	GotoNextPage();
    return true;
}//CleanHeadsHeatingPageEnter	
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
bool CHeadServiceWizard::CheckCommunicationPageEnter(CWizardPage* WizardPage)
{
  //Important : we assuming that the connection is O.K and during the way we are checking the connection
  bool ConnectionWithEprom = true;
//  int i = 0;
  // Read EPROMs and head thermistor in order to check that the heads are inserted correctly
/*  try
  {
	 for(; i < TOTAL_NUMBER_OF_HEADS_HEATERS/2; i++)
	 {
		if (m_HeadsToBeReplaced[i])
		{						
		   BYTE Data = E2PROM_TEST_DATA;
    	   // Write to a certain data to a certain address, read and compare
		   m_BackEnd->WriteToE2PROM(i*2, E2PROM_TEST_ADDRESS, E2PROM_TEST_DATA);
		   if (CHECK_NOT_EMULATION(m_ParamMgr->OHDB_Emulation))
			   Data = m_BackEnd->ReadFromE2PROM(i*2, E2PROM_TEST_ADDRESS);
		   if (Data != E2PROM_TEST_DATA)
			   m_HeadsInsertedWrong[i] = true;
		   if (m_HeadsInsertedWrong[i])
			   ConnectionWithEprom = false;
		   // for next 1/2 head
		   m_BackEnd->WriteToE2PROM(i*2+1, E2PROM_TEST_ADDRESS, E2PROM_TEST_DATA);
		   if (CHECK_NOT_EMULATION(m_ParamMgr->OHDB_Emulation))
			   Data = m_BackEnd->ReadFromE2PROM(i*2+1, E2PROM_TEST_ADDRESS);
		   if (Data != E2PROM_TEST_DATA)
			   m_HeadsInsertedWrong[i] = true;
		   if (m_HeadsInsertedWrong[i])
			   ConnectionWithEprom = false;
		}
	 } //for
  }
  catch(char* str)
  {
	 ConnectionWithEprom = false;
	 m_NoCharacterizationDescription += "\n" + GetHeadName(i) + ": " + str;
  }   */
  if (ConnectionWithEprom)
  {
	 ConnectionWithEprom = m_BackEnd->AreHeadHeatersThermistorsValid(false);

	//For each head that was selected to be replaced:
	//read the data and add (the correct head type) to heads list
	//and write the removal date onto the defctive head memory.
	for(int i = 0; i < TOTAL_NUMBER_OF_HEADS_HEATERS/2; i++)
	{
		if (m_HeadsToBeReplaced[i])
		{
		   try
		   {
			   m_BackEnd->ReadHeadParametersFromE2PROM(i*2, &m_CurrHeadsParams[i*2]);
			   m_BackEnd->ReadHeadParametersFromE2PROM(i*2+1, &m_CurrHeadsParams[i*2+1]);

		   }
		   catch(EQException& err)
		   {
			   m_CancelReason	+= "Head Replacement Wizard was canceled due to a communication problem, while trying to read from the heads";
			   QMonitor.ErrorMessage(err.GetErrorMsg(),ORIGIN_WIZARD_PAGE);
               throw CQ2RTAutoWizardException(m_CancelReason);
           }
           //writing to history file
		   QString HeadReplacementLog = QFormatStr("Head %s was installed. S/N: %s",GetDoubleHeadName(i).c_str(), m_CurrHeadsParams[i].SerialNumber);
		   WriteToLogFile(LOG_TAG_GENERAL, HeadReplacementLog);
		   HistoryFile->Append(HeadReplacementLog);
		   HistoryFile->SkipLines(1);

        }
    }// End for
	// Everything is ok - continue wizard; Some error found - report it....
	GotoPage((ConnectionWithEprom) ? qrHeadsHeating2 : qrNoCharacterizationData);
	return true;
  }//ConnectionWithEprom is false
  else
  {
	// Turn the power off and open the door, for insertion check
	m_BackEnd->EnableDoor(false);
	TurnPowerOff();
	GotoNextPage();
  }
  return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////
bool CHeadServiceWizard::AdjustHeadsVoltagePageEnter(CWizardPage* WizardPage)
{
  bool ret = OVER_MAX_MISSING_NOZZLES_NOT_FOUND;
  if (ret == OVER_MAX_MISSING_NOZZLES_NOT_FOUND)
  {
    CStatusWizardPage *Page = dynamic_cast<CStatusWizardPage *>(WizardPage);
	m_GoToContinuationPage = false;

    float Weight,Length,Width,MaterialFactor;
    int   Slices,MissingNozzles;

    //---------------------------
    // Calculations for Formula 8

	float CarpetForSupportWeight = 0,WeightModelHeadsCarpetPrint  = 0;
	FOR_ALL_HEADS(h)
	{
	 if(m_ParamMgr->IsModelHead(h))
		WeightModelHeadsCarpetPrint += WeightResults[h][CARPET_VOLTAGE][CARPET_MODE_QUALITY_MODE];
	}
    CarpetForSupportWeight = m_ParamMgr->HSW_NumberOfSlicesArray[CARPET_MODE_QUALITY_MODE];
    CarpetForSupportWeight *= NUMBER_OF_MODEL_HEADS;
	CarpetForSupportWeight = WeightModelHeadsCarpetPrint / CarpetForSupportWeight;
	CarpetForSupportWeight *= NUMBER_OF_CARPET_SLICES;

	Length = m_ParamMgr->HSW_BmpHeight;    //fixme - should it updated by enter Mode?
	Width  = m_ParamMgr->HSW_BmpWidth;
	//---------------------------

    //For each item in the list do the calculations: Layer Thickness
    //Calculation per Print!!!
    bool BelowMinimumFound     = LH_BELOW_MINIMUM_NOT_FOUND;
	FOR_ALL_MODES(qm, om)
    {
      if (! GetModeAccessibility(qm, om))
         continue;
      Page->StatusMessage = GetModeStr(qm, om);
      Page->Refresh();
	  m_BackEnd->GotoDefaultMode();
	  m_BackEnd->EnterMode(PRINT_MODE[qm][om],MACHINE_QUALITY_MODES_DIR(qm,om));
	  WriteToLogFile(LOG_TAG_GENERAL, "Calibrating " + GetModeStr(qm, om));
      FOR_ALL_VOLTAGE_MODES(v)
      {
		  FOR_ALL_HEADS(i)
          {
            YieldWizardThread();
      	    //	Layer Thickness (LH_gross) (formula 2 - ICD ):
			//*******************************//
            Weight = WeightResults[i][v][qm];
			MissingNozzles = MissingNozzlesResults[i][v][qm];
			Slices = m_ParamMgr->HSW_NumberOfSlicesArray[qm];
			MaterialFactor = GET_MATERIAL_FACTOR(i,om);
			// Part of Formula 8
			if ( !m_ParamMgr->IsModelHead(i) )
			   Weight -= CarpetForSupportWeight;
			if(NOZZLES_PER_HEAD != MissingNozzles)
            {
			   int Divider = Length * Width * FLOAT_FROM_RESOURCE(IDN_SPECIFIC_GRAVITY) * Slices *(NOZZLES_PER_HEAD - MissingNozzles);
               if (0 == Divider)
                   throw EQException("Layer Height Gross calculation : Dividing by zero");
			   LayerHeightGross[i][v][qm] = (Weight * MaterialFactor * NOZZLES_PER_HEAD * 10000 / Divider);
			}
			else //NOZZLES_PER_HEAD == MissingNozzles
               LayerHeightGross[i][v][qm] = 0;

          }
        }
        YieldWizardThread();
		//VoltageAdjustment(qm, om){
        if (CalcVoltage(qm, om) == LH_BELOW_MINIMUM_FOUND)
		{
           BelowMinimumFound = LH_BELOW_MINIMUM_FOUND;
           continue;
        }
        CalcFinalLayerHeight(qm, om);
        CalcFinalVoltage(qm, om);
        //}VoltageAdjustment(qm, om)
   }

   if (BelowMinimumFound == LH_BELOW_MINIMUM_NOT_FOUND)
   {
      FOR_ALL_MODES(qm, om)
      {
         if (! GetModeAccessibility(qm, om))
            continue;
         //VoltageAdjustment(qm, om){
		 //Trying to find heads with minimal layer height and suggest the user to replace them.
         ExcludeHeadsWithMinimalLayerHeight(qm, om); // Recursive function
         //}VoltageAdjustment(qm, om)
	  }
   }
  }
  if(m_ParamMgr->HSW_SCALE_EMULATION == false)
	InformTheUserOfTheDefectiveHeads();
  if (!m_GoToContinuationPage)
  {
	SaveAndCalibrate();
	SetNextPage(m_AskRunHeadAlignmentWizard ? qrWizardCompleted : qrWizardCompletedWithoutHeadsReplacement);
  }

  GotoNextPage();
  return true;
}//AdjustHeadsVoltagePageEnter

///////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////
void CHeadServiceWizard::ResumeWizardSpecificData()
{
  m_TempFile->Read((BYTE*)m_HeadsToBeReplaced,sizeof(bool)*TOTAL_NUMBER_OF_HEADS/2);
  m_TempFile->Read((BYTE*)m_HeadsToBeSkipped,sizeof(bool)*TOTAL_NUMBER_OF_HEADS);
  m_TempFile->Read((BYTE*)&m_IsFirstWizardIteration,sizeof(m_IsFirstWizardIteration));
  m_TempFile->Read((BYTE*)&m_RunHeadAlignmentWizard,sizeof(m_RunHeadAlignmentWizard));
  m_TempFile->Read((BYTE*)&m_AskRunHeadAlignmentWizard,sizeof(m_AskRunHeadAlignmentWizard));

  FOR_ALL_QUALITY_MODES(qm)
      FOR_ALL_VOLTAGE_MODES(v)
           FOR_ALL_HEADS(h)
		   {
			   m_TempFile->Read((BYTE*)&(WeightResults[h][v][qm]),sizeof(float));
               m_TempFile->Read((BYTE*)&(MissingNozzlesResults[h][v][qm]),sizeof(int));
           }
}//ResumeWizardSpecificData
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
bool CHeadServiceWizard::ReplacementWarningPageEnter(CWizardPage *WizardPage)
{
  //For each head that was selected to be replaced:
  //read the data and add (the correct head type) to heads list
  //and write the removal date onto the defctive head memory.

  // Write the date to the Head Service Wizard History file
  HistoryFile->WriteLine();
  HistoryFile->SkipLines(1);
  HistoryFile->WriteDate();
  HistoryFile->WriteMachineName();
  WriteToHistoryFileMaterialDataStr();
  HistoryFile->SkipLines(1);


  for (int i = 0; i < TOTAL_NUMBER_OF_HEADS_HEATERS/2; i++)
  {
      try
      {
		  m_BackEnd->ReadHeadParametersFromE2PROM(i*2, &m_PreviousHeadsParams[i*2]);
		  m_BackEnd->ReadHeadParametersFromE2PROM(i*2+1, &m_PreviousHeadsParams[i*2+1]);
      }
      catch(EQException& err)
      {
		  throw CQ2RTAutoWizardException( GetTitle() + " was canceled because of a communication problem while trying to receive data from the heads.");
      }
	  if (m_HeadsToBeReplaced[i])
      {
		  //writing to history file
		  QString HeadReplacementLog = QFormatStr("Head %s was taken out. S/N: %s Drop counter for H%d : %lu. Drop counter for H%d : %lu",
													GetDoubleHeadName(i).c_str(),
													m_PreviousHeadsParams[i].SerialNumber,
													(i*2), m_BackEnd->GetMaintenanceCounterElapsedSeconds(HeadsCounter_DC_ID[i*2]),
													(i*2+1), m_BackEnd->GetMaintenanceCounterElapsedSeconds(HeadsCounter_DC_ID[i*2+1]));
		  WriteToLogFile(LOG_TAG_GENERAL, HeadReplacementLog);
	      HistoryFile->Append(HeadReplacementLog);

      }
  }
  m_BackEnd->EnableDoor(false);
  TurnPowerOff();
  return true;

}
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
void CHeadServiceWizard::PageEnter(CWizardPage *WizardPage)
{
  THeadServiceWizardPagesIndex PageIndex = (THeadServiceWizardPagesIndex)WizardPage->GetPageNumber();
  TQErrCode Err = Q_NO_ERROR;
  CRadioGroupWizardPage *Page = dynamic_cast<CRadioGroupWizardPage *>(WizardPage);
  try
  {
   switch(PageIndex)
   {
	case qrWelcomePage:
		 SaveLastState();
	for(int i = LAST_TANK_TYPE-1; i >= FIRST_TANK_TYPE; i--)
		{
		 m_BackEnd->SkipMachineResinTypeChecking(static_cast<TTankIndex>(i), true);
	   	TTankIDNotice::HideDialog(static_cast<TTankIndex>(i));
  		}
         break;

    case qrIntallationCompleteQuery:
	 m_AskRunHeadAlignmentWizard = true;
		 SaveLastState();
	 break;
	 
	case qrSelectCalibrationMode:
	 /*before DM6*/
	  //	if (m_ParamMgr->ThermistorsOperationMode == LOW_THERMISTORS_MODE) /*HighMix(DM7),HQ*/
	  //	  Page->SetDisabledMask(1);
	  //	else if ((m_ParamMgr->ThermistorsOperationMode == HIGH_THERMISTORS_MODE)        ||
	  //			 (m_ParamMgr->ThermistorsOperationMode == HIGH_AND_LOW_THERMISTORS_MODE)  ) /*HS*/
	  //	Page->SetDisabledMask(6);
	  //	else  /*undefined mode for Keshet,cancel*/
	  //	{
	  //	  CancelOccurrence(WizardPage);
	  //	}
	  	Page->Refresh();
	break;
	case qrSelectHeads:
	     m_PatternTestNeeded = true;
         SaveLastState();
         if (!m_IsFirstWizardIteration)
            GotoNextPage();
         break;

    case qrPreparingForReplacement:
         SaveLastState();
         m_BackEnd->HeadHeatersSetIgnoreError(true);
         SelectedHeadsHeatingCycle(DEFAULT_TIMEOUT, NULL,true);
         m_BackEnd->HeadHeatersSetIgnoreError(false);

	  if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
          // Disable temperature error reporting, because we're going to drain the block
          m_BackEnd->SetReportTemperatureError(false);
          m_BackEnd->TurnHeadFilling(false);

          HeadsDrainingCycle(WizardPage,m_ParamMgr->HSW_EmptyBlockTime);
          //To keep head above the tub if leaks
          TRY_FUNCTION(Err, m_BackEnd->GotoPurgePosition(true, false));

         if (CoolingDownPageEnter(WizardPage) == false)
         {
             m_BackEnd->SetReportTemperatureError(true);
			 return;
         }
         m_BackEnd->SetReportTemperatureError(true);
         GotoNextPage();
         break;

    case qrReplacementPosition:
		 ReplacementPositionPageEnter(WizardPage, qrReInsertHead);
         AxesTable[AXIS_Z] = false;
		 GotoNextPage();
         break;
    
    case qrReplacementWarning:
		 if (ReplacementWarningPageEnter(WizardPage) == false)
            return;
         break;

    case qrReplaceHead:
		 ReplaceHeadPageEnter(WizardPage, qrIntallationCompleteQuery);
		 break;
	case qrTemplatePrinting:
		TemplatePrintingPageEnter(WizardPage);
		break;
    case qrReInsertHead:
         SaveLastState();
         ReInsertHeadPageEnter(WizardPage);
         break;
	case qrStressTest:
		 PatternTestStressTest();
		 SetNextPage(qrTestPatternPrinting);
		 GotoNextPage();
		 break;

	case qrCheckCommunication:    -
		 CheckCommunicationPageEnter(WizardPage);
		 break;

	case qrScaleCommunicationError:
		 m_BackEnd->LockDoor(false); // open the door.
		 break;
    case qrCheckInsertion:
		 CheckInsertionPageEnter(WizardPage);
         break;

	case qrNoCharacterizationData:
         NoCharacterizationDataPageEnter(WizardPage);
         break;

	case qrHeadsHeating2:
		 //SaveLastState();
		 HeadsHeating2PageEnter(WizardPage);
		 break;

    case qrFillBlock:
		 if (FillBlockPageEnter(WizardPage, AxesTable) == false)
            return;
		 break;

	case qrCleanHeadsPurge:
		 if (CleanHeadsPurgePageEnter(WizardPage) == false)
			return;
		 break;

    case qrCleanHeadsHeatingPage:
		 if (CleanHeadsHeatingPageEnter(WizardPage) == false)
			return;
         break;
	case qrTestPatternPrinting:
		if(TestPatternPrintingPageEnter(WizardPage) == false)
			return;
		MoveZtoScalePosition();
		SetNextPage(qrEnterMissingNozzles);
		GotoNextPage();
		 break;
	case qrTestPatternPreparation:
		 TestPatternPreparationPageEnter(WizardPage);
   		break;
	case qrVacuumLeakage:
		 SetNextPage(qrPreparingForReplacement);
		 m_VacuumLeakage = true;
		 break;

	case qrPurge:
		 if((m_ParamMgr->HSW_Perform_PT_stress_test == true)&&  // stress test unusable , so does not checked
			 (m_hswScaleBasedData->m_stressTestDone == false))
			SetNextPage(qrStressTest);
		 else
			if((m_ParamMgr->HSW_Perform_PT_stress_test == true)&& // stress test unusable , so does not checked
			 (m_hswScaleBasedData->m_stressTestDone == true))
			 {
			 if (PurgePageEnter(WizardPage, 0, 0, AxesTable) == false)
				return;
			 SetNextPage(qrFindTargetLayer);
			 }
		 else
		 if(m_headReplacementDone == true)
			{
			 m_headReplacementDone = false;
			 if (PurgePageEnter(WizardPage, 0, 0, AxesTable) == false)
				return;
				/*home X,Y*/
				if (m_BackEnd->IsDoorLocked())
					{
					 EnableAxeAndHome(AXIS_Y, true, true);
					 EnableAxeAndHome(AXIS_X, true, true);
					 m_BackEnd->WaitForEndOfMovement(AXIS_Y);
					 m_BackEnd->WaitForEndOfMovement(AXIS_X);
					}
			 SetNextPage(qrSelectWizardMode);
			}
		 else
			if((m_ParamMgr->HSW_Perform_PT_stress_test == false)&&
			 (m_hswScaleBasedData->IsFramePrinted() == true) &&(m_PatternTestNeeded == false))
			 {
				if (PurgePageEnter(WizardPage, 0, 0, AxesTable) == false)
					return;
				SetNextPage(qrFindTargetLayer);
			 }
		 else
			if((m_ParamMgr->HSW_Perform_PT_stress_test == false)&&(m_hswScaleBasedData->IsFramePrinted() == false))
				{
				 if (PurgePageEnter(WizardPage, 0, 0, AxesTable) == false)
					return;
				 SetNextPage(qrTemplatePrintingPreparation);
				}
         else
			if((m_ParamMgr->HSW_Perform_PT_stress_test == false)&&
			(m_hswScaleBasedData->IsFramePrinted() == true)&&(m_PatternTestNeeded == true))
			{
                if (PurgePageEnter(WizardPage, 0, 0, AxesTable) == false)
					return;
				/*home X,Y*/
				if (m_BackEnd->IsDoorLocked())
					{
					 EnableAxeAndHome(AXIS_Y, true, true);
					 EnableAxeAndHome(AXIS_X, true, true);
					 m_BackEnd->WaitForEndOfMovement(AXIS_Y);
					 m_BackEnd->WaitForEndOfMovement(AXIS_X);
					}
				SetNextPage(qrTestPatternPreparation);
			}
			  //	SetNextPage(qrTestPatternPrinting);
		 GotoNextPage();
         break;

	case qrWeightTest:
	  /*	 if (WeightTestPageEnter(WizardPage) == false)
			return;    */
		 break;

    case qrAdjustHeadsVoltage:
         if (AdjustHeadsVoltagePageEnter(WizardPage) == false)
            return;
		 break;

	case qrWizardCompleted:
	{
		CMessageWizardPage *Page = dynamic_cast<CMessageWizardPage *>(WizardPage);
		if(m_targetWeightNotReached == true) /*no need to restart */
		{
		 RevertParams();
		 m_AskRunHeadAlignmentWizard = false;
		 m_bNeedRestartAfterParamsChanged = false;
		 Page->SubTitle = "Click ‘Done’, calibration was not finished.";
		 Page->Refresh();
		}
		else
		{
			if(m_SelectedMode == HS_HM)
			{
			   m_ParamMgr->HSW_PrintingAllowedForMode[0] = true;
			   m_ParamMgr->HSW_PrintingAllowedForMode[1] = true;
			   m_BackEnd->ResetMaintenanceCounter(HOW_HS_HM_COUNTER_ID);
			   m_BackEnd->SetMachineActionsDate("Head Optimization Wizard HS/HM");
			}
			else if(m_SelectedMode == HQ)
			{
				 m_ParamMgr->HSW_PrintingAllowedForMode[3] = true;
				 m_BackEnd->ResetMaintenanceCounter(HOW_HQ_COUNTER_ID);
				 m_BackEnd->SetMachineActionsDate("Head Optimization Wizard HQ");
			}
			m_ParamMgr->SaveSingleParameter(&m_ParamMgr->HSW_PrintingAllowedForMode);
			
		 SaveHighMixCalibrationToHighSpeed();
		 if(m_AskRunHeadAlignmentWizard)
			 Page->SubTitle = "Click ‘Done’ to restart the printer application. \nAfter the application restarts, run the Head Alignment Wizard.";
		 else
			 Page->SubTitle = "Click ‘Done’ to restart the printer application.";
		 Page->Refresh();
		 m_AskRunHeadAlignmentWizard = false;
		 m_bNeedRestartAfterParamsChanged = true;
		 }
		break;
	}

	case qrScaleRemoving:
		 m_BackEnd->LockDoor(false); // open the door.
		 break;
	case qrUVScanning:
		/*purge,wipe*/
		try
		{
		if (PurgePageEnter(WizardPage, 0, 0, AxesTable) == false)
			WriteToLogFile(LOG_TAG_GENERAL,"UV Scanning , purge sequence was failed");
		}
		 catch(EQException& err)
		 {
			WriteToLogFile(LOG_TAG_GENERAL,"UV Scanning , some problem in purge sequence.");
		 }
		UVScanning(WizardPage);
		break;
	case qrWizardCompletedWithoutHeadsReplacement:
	 {
		 CMessageWizardPage *Page = dynamic_cast<CMessageWizardPage *>(WizardPage);
		 if(m_targetWeightNotReached == true) /*no need to restart */
		{
		 RevertParams();
		 m_AskRunHeadAlignmentWizard = false;
		 m_bNeedRestartAfterParamsChanged = false;
		 Page->SubTitle = "Click ‘Done’, calibration was not finished.";
		 Page->Refresh();
		}
		else
		{
			if(m_SelectedMode == HS_HM)
			{
			   m_ParamMgr->HSW_PrintingAllowedForMode[0] = true;
			   m_ParamMgr->HSW_PrintingAllowedForMode[1] = true;
			   m_BackEnd->ResetMaintenanceCounter(HOW_HS_HM_COUNTER_ID);
			   m_BackEnd->SetMachineActionsDate("Head Optimization Wizard HS/HM");
			}
			else if(m_SelectedMode == HQ)
			{
				 m_ParamMgr->HSW_PrintingAllowedForMode[3] = true;
				 m_BackEnd->ResetMaintenanceCounter(HOW_HQ_COUNTER_ID);
				 m_BackEnd->SetMachineActionsDate("Head Optimization Wizard HQ");
			}
			m_ParamMgr->SaveSingleParameter(&m_ParamMgr->HSW_PrintingAllowedForMode);
		 SaveHighMixCalibrationToHighSpeed();
		 Page->SubTitle = "Click ‘Done’ to restart the printer application.";
		 Page->Refresh();
		 m_AskRunHeadAlignmentWizard = false;
		 m_bNeedRestartAfterParamsChanged = true;
		}
		 break;
	 }

	case qrSelectContinuationOption:
	 {
	 CRadioGroupWizardPage *Page = dynamic_cast<CRadioGroupWizardPage *>(WizardPage);
	 Page->SubTitle = m_ContinuationOptionPageSubtitle;
	 Page->Refresh();
	 break;
	 }
	case qrMaterialReplacement:
	{
		if(m_ParamMgr->HSW_DM3_Enable == true) /*M3 can be Support and Model*/
		{
			// need to do MRW in case HSW was stopped and final material replacement was not done.
			m_ParamMgr->MRW_WithinWizard = 1;
			m_ParamMgr->HSW_WithinWizard = 1;
			m_ParamMgr->SaveSingleParameter(&m_ParamMgr->MRW_WithinWizard);
			m_ParamMgr->SaveSingleParameter(&m_ParamMgr->HSW_WithinWizard);
	 //		ReplaceMaterial(WizardPage);
		}
		if(m_ParamMgr->HSW_Perform_Pattern_Test_After_Material_Replacement == true)
			TestPatternPrintingPageEnter(WizardPage,false);

		if(m_hswScaleBasedData->m_lastMRW == false) //cont with HSW
		{
			if(m_hswScaleBasedData->IsFramePrinted() == false)
			  SetNextPage(qrTemplatePrintingPreparation);
			else
			SetNextPage(qrPurge);   // Perform Purge
		}
		else // it was the last replacement
		{
			m_ParamMgr->MRW_WithinWizard = 0;
			m_ParamMgr->HSW_WithinWizard = 0;
			m_ParamMgr->SaveSingleParameter(&m_ParamMgr->MRW_WithinWizard);
            m_ParamMgr->SaveSingleParameter(&m_ParamMgr->HSW_WithinWizard);
			SetNextPage(qrCloseDoorResinToPurgeUnit);
		}
		GotoNextPage();
		break;
	}

	case qrFindTargetLayer:
	{
	   FindTargetLayer(WizardPage,m_hswScaleBasedData->GetCurrentOperationMode(),m_hswScaleBasedData->GetCurrentQualityMode());

		 break;
	}
	case qrFirstFire:
	{
		FirstFire(WizardPage,m_hswScaleBasedData->GetCurrentOperationMode(),m_hswScaleBasedData->GetCurrentQualityMode());
		break;
	}
	case qrCheckTest:
	{
		CheckCalibrationTest(WizardPage,m_hswScaleBasedData->GetCurrentOperationMode(),m_hswScaleBasedData->GetCurrentQualityMode());
		break;
    }
	case qrScaleSetup:
		break;
	case qrPutResinToPurgeUnit:
	{
	  RemoveResinFromContainerPageEnter(WizardPage);
	  break;
    }	
	case qrResumeWizard:
		 ResumeWizardPageEnter(WizardPage);
		 SetNextPage(m_ParamMgr->HSW_LastState);
         break;
   } // end of switch
  }
  catch(EQHeadFillingException& E)
  {
	GotoPage(qrLiquidsShortagePage);
	return;
  }
} // end of CHeadServiceWizard::PageEnter

int CHeadServiceWizard::GetContinuePageNumber()
{
  return qrResumeWizard;
}

void CHeadServiceWizard::CalcFinalVoltage(int QualityMode, int OperationMode, THeadIndex HeadNumber)
{
  static int WeakestHeadIndex = -1;
  bool isModel =  m_ParamMgr->IsModelHead( HeadNumber);
  YieldWizardThread();
  
  if (ALL_HEADS == HeadNumber)
  {
     WeakestHeadIndex = FindWeakestHead();
	 FOR_ALL_DOUBLE_HEADS(i)
		 if (!m_HeadsToBeReplaced[i])
		 {

			  CalcFinalVoltage(QualityMode, OperationMode, (THeadIndex)(i*2));
              CalcFinalVoltage(QualityMode, OperationMode, (THeadIndex)(i*2+1));
		 }
	 WeakestHeadIndex = -1;
     return;
  }

  QString HeadsName    = GetHeadName(HeadNumber);
  int     HeadsChamber = GetHeadsChamberForHSW(HeadNumber,OperationMode);
  float ProductLT_m    = m_ParamMgr->ProductLT_m;
  float ProductLT_n    = m_ParamMgr->ProductLT_n;
  float MaxLayerHeight = m_ParamMgr->HSW_MaxLayerHeightArray[QualityMode];

  // finding the worst head (highest voltage and lowest LT)
  float HeadTestV_High = m_ParamMgr->HSW_HeadTestV_HighArray[HeadsChamber];
  float LayerHeightGrossHigh = LayerHeightGross[HeadNumber][HIGH_VOLTAGE][QualityMode];

  //Calculate Final Voltage per head (formula 5 - ICD)
  float MaxLayerHeightGross = (MaxLayerHeight + ProductLT_m) * ProductLT_n;
  float CurrentLayer = m_ParamMgr->LayerHeightDPI_um;
  float TargetLayer = m_FinalLayerHeight[QualityMode][OperationMode] / m_ParamMgr->ProductLT_n - m_ParamMgr->ProductLT_m;
  if ((m_LayerHeightPerHead[WeakestHeadIndex] == m_LayerHeightPerHead[HeadNumber]) &&
	   (m_LayerHeightPerHead[WeakestHeadIndex] < MaxLayerHeightGross))
		if(isModel)
		{
			m_VoltagePerHeadModel[HeadNumber][QualityMode][OperationMode]= m_ParamMgr->HSW_MaxVoltageArray[HeadsChamber];
			if(HeadNumber == HEAD_2 || HeadNumber == HEAD_3)
				m_VoltagePerHeadSupport[HeadNumber][QualityMode][OperationMode]=
				m_PrevHeadsVoltagesSupport[HeadNumber][QualityMode][OperationMode]+ ((TargetLayer-CurrentLayer)/m_GainPerHeadSupport[HeadNumber]);

		}
		else
		{
			m_VoltagePerHeadSupport[HeadNumber][QualityMode][OperationMode]= m_ParamMgr->HSW_MaxVoltageArray[HeadsChamber];
			if(HeadNumber == HEAD_2 || HeadNumber == HEAD_3)
				m_VoltagePerHeadModel[HeadNumber][QualityMode][OperationMode]=
				m_PrevHeadsVoltagesModel[HeadNumber][QualityMode][OperationMode]+ (TargetLayer-CurrentLayer)/m_GainPerHeadModel[HeadNumber];
		}
  else
	if(isModel)
	{
		m_VoltagePerHeadModel[HeadNumber][QualityMode][OperationMode] = HeadTestV_High + (m_FinalLayerHeight[QualityMode][OperationMode] - LayerHeightGrossHigh) / m_GainPerHeadModel[HeadNumber];
		if(HeadNumber == HEAD_2 || HeadNumber == HEAD_3)
			m_VoltagePerHeadSupport[HeadNumber][QualityMode][OperationMode]=
			m_PrevHeadsVoltagesSupport[HeadNumber][QualityMode][OperationMode]+ (TargetLayer-CurrentLayer)/m_GainPerHeadSupport[HeadNumber];
	}

	else
	{
		m_VoltagePerHeadSupport[HeadNumber][QualityMode][OperationMode] = HeadTestV_High + (m_FinalLayerHeight[QualityMode][OperationMode] - LayerHeightGrossHigh) / m_GainPerHeadSupport[HeadNumber];
		if(HeadNumber == HEAD_2 || HeadNumber == HEAD_3)
			m_VoltagePerHeadModel[HeadNumber][QualityMode][OperationMode]=
			m_PrevHeadsVoltagesModel[HeadNumber][QualityMode][OperationMode]+ (TargetLayer-CurrentLayer)/m_GainPerHeadModel[HeadNumber];
	}
  if(isModel)
	WriteToLogFile(LOG_TAG_GENERAL, QFormatStr("%s: Voltage for Model= %s", HeadsName.c_str(),
				 ( (m_VoltagePerHeadModel[HeadNumber][QualityMode][OperationMode] > 50) ? "Incorrect" :
				   QFloatToStr(m_VoltagePerHeadModel[HeadNumber][QualityMode][OperationMode]).c_str() ) ));
  else
	 WriteToLogFile(LOG_TAG_GENERAL, QFormatStr("%s: Voltage for Support= %s", HeadsName.c_str(),
				 ( (m_VoltagePerHeadSupport[HeadNumber][QualityMode][OperationMode] > 50) ? "Incorrect" :
				   QFloatToStr(m_VoltagePerHeadSupport[HeadNumber][QualityMode][OperationMode]).c_str() ) ));
}

bool CHeadServiceWizard::CalcVoltage(int QualityMode, int OperationMode, THeadIndex HeadNumber)
{
  bool ret = LH_BELOW_MINIMUM_NOT_FOUND;
  YieldWizardThread();

  if (ALL_HEADS == HeadNumber)
  {
     FOR_ALL_DOUBLE_HEADS(i)
		 if (!m_HeadsToBeReplaced[i])
		 {
			if (CalcVoltage(QualityMode, OperationMode, (THeadIndex)(i*2)) == LH_BELOW_MINIMUM_FOUND)
			   ret = LH_BELOW_MINIMUM_FOUND;
            if (CalcVoltage(QualityMode, OperationMode, (THeadIndex)(i*2+1)) == LH_BELOW_MINIMUM_FOUND)
			   ret = LH_BELOW_MINIMUM_FOUND;
		 }
     return ret;
  }

  int   ChamberIndex = GetHeadsChamberForHSW(HeadNumber,OperationMode);
  float LayerHeightGrossHigh = LayerHeightGross[HeadNumber][HIGH_VOLTAGE][QualityMode];
  float LayerHeightGrossLow = LayerHeightGross[HeadNumber][LOW_VOLTAGE][QualityMode];
  float MaxLayerHeightGross = m_ParamMgr->HSW_MaxLayerHeightArray[QualityMode];
  float MinLayerHeightGross = m_ParamMgr->HSW_MinLayerHeightArray[QualityMode];
  float HeadTestV_High = m_ParamMgr->HSW_HeadTestV_HighArray[ChamberIndex];
  float HeadTestV_Low  = m_ParamMgr->HSW_HeadTestV_LowArray[ChamberIndex];
  float MaxVoltage  = m_ParamMgr->HSW_MaxVoltageArray[ChamberIndex];
  float ProductLT_m = m_ParamMgr->ProductLT_m;
  float ProductLT_n = m_ParamMgr->ProductLT_n;
  float MaxHeadLayer;
  float CorrectionFactor =  m_ParamMgr->CorrectionFactor;
  bool isModel = m_ParamMgr->IsModelHead(HeadNumber);
  QString QHeadName = GetHeadName(HeadNumber);
  const char* HeadName = QHeadName.c_str();
  // Correction Factor
  WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("%s, CorrectionFactor = %f", HeadName, CorrectionFactor));
    // Gain for Triplex HSW
   if(isModel)
   {
	// Gain  (formula 3 - ICD ):
	m_GainPerHeadModel[HeadNumber] =  (LayerHeightGrossHigh - LayerHeightGrossLow) / (HeadTestV_High - HeadTestV_Low);
	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("%s, Gain for Model = %f", HeadName, m_GainPerHeadModel[HeadNumber]));
	// formula 1
	if(HeadNumber == HEAD_2 || HeadNumber == HEAD_3)
	{
		m_GainPerHeadSupport[HeadNumber] = m_GainPerHeadModel[HeadNumber]+CorrectionFactor;
		WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("%s, Gain for Support = %f", HeadName, m_GainPerHeadSupport[HeadNumber]));
	}
   }
   else
   {
	// Gain  (formula 3 - ICD ):
	m_GainPerHeadSupport[HeadNumber] =  (LayerHeightGrossHigh - LayerHeightGrossLow) / (HeadTestV_High - HeadTestV_Low);
	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("%s, Gain for Support = %f", HeadName, m_GainPerHeadSupport[HeadNumber]));
	// formula1
	if(HeadNumber == HEAD_2 || HeadNumber == HEAD_3)
	{
		m_GainPerHeadModel[HeadNumber] = m_GainPerHeadSupport[HeadNumber]-CorrectionFactor;
		WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("%s, Gain for Model = %f", HeadName, m_GainPerHeadModel[HeadNumber]));
	}
   }
  // Max Head Layer  (formula 4 - ICD ):
  if(isModel)
	MaxHeadLayer =  LayerHeightGrossHigh + (MaxVoltage - HeadTestV_High ) * m_GainPerHeadModel[HeadNumber];
  else
    MaxHeadLayer =  LayerHeightGrossHigh + (MaxVoltage - HeadTestV_High ) * m_GainPerHeadSupport[HeadNumber];


  //save all in order to work on them later in the ExcludeHeadsWithMinimalLayerHeight function.
  WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("%s, Maximal head layer = %f", HeadName, MaxHeadLayer));

  //check max limit
  MaxLayerHeightGross = (MaxLayerHeightGross + ProductLT_m ) * ProductLT_n;
  m_LayerHeightPerHead[HeadNumber] = std::min(MaxHeadLayer, MaxLayerHeightGross); //Calc LH(i)
  WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("%s, Layer height per head = %f", HeadName, m_LayerHeightPerHead[HeadNumber]));

  //check min limit
  MinLayerHeightGross = (MinLayerHeightGross + ProductLT_m) * ProductLT_n;
  if (m_LayerHeightPerHead[HeadNumber] <  MinLayerHeightGross)
  {
      WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("%s, LayerHeightPerHead (%f) is lower then MinLayerHeightGross (%f)",
                                     HeadName, m_LayerHeightPerHead[HeadNumber], MinLayerHeightGross));
      m_HeadsToBeReplaced[HeadNumber/2] = true;
      return LH_BELOW_MINIMUM_FOUND;//this head will be replaced, no point to continue
  }
  return ret;
}


void CHeadServiceWizard::SaveWizardSpecificDataToTemporaryFile()
{
  m_TempFile->Write((BYTE*)&m_HeadsToBeReplaced,sizeof(bool)*TOTAL_NUMBER_OF_HEADS/2);
  m_TempFile->Write((BYTE*)&m_HeadsToBeSkipped,sizeof(bool)*TOTAL_NUMBER_OF_HEADS);
  m_TempFile->Write((BYTE*)&m_IsFirstWizardIteration,sizeof(m_IsFirstWizardIteration));
  m_TempFile->Write((BYTE*)&m_RunHeadAlignmentWizard,sizeof(m_RunHeadAlignmentWizard));
  m_TempFile->Write((BYTE*)&m_AskRunHeadAlignmentWizard,sizeof(m_AskRunHeadAlignmentWizard));

  FOR_ALL_QUALITY_MODES(qm)
      FOR_ALL_VOLTAGE_MODES(v)
           for (int h = 0; h < TOTAL_NUMBER_OF_HEADS; h++)
           {
			   m_TempFile->Write((BYTE*)&(WeightResults[h][v][qm]),sizeof(float));
               m_TempFile->Write((BYTE*)&(MissingNozzlesResults[h][v][qm]),sizeof(int));
           }
}//SaveWizardSpecificDataToTemporaryFile

void CHeadServiceWizard::SetResumingPage()
{
  SetNextPage(m_ParamMgr->HSW_LastState);
}

QString CHeadServiceWizard::GetResumeWarningString()
{
  return GetResumeWarningString(m_ParamMgr->HSW_LastState);
}

QString CHeadServiceWizard::GetResumeWarningString(int PageNumber)
{
  QString ResumeString;

  switch (PageNumber)
    {
    case qrReplacementPosition:
	case qrReplaceHead:
    case qrReInsertHead:
	case qrIntallationCompleteQuery:
	case qrHeadsHeating2:
	  ResumeString = "Warning: It is assumed that at this stage, the new head(s) is(are) inserted. If this is not the case press 'Cancel' and start the wizard again. To continue press 'Next'.";
      break;

	case qrPreparingForReplacement:
	case qrVacuumLeakage:
    case qrEnterWeights:
    case qrWeightTest:
	case qrPurge:
	  ResumeString = "Press 'Next' to continue";
      break;

    default:
      ResumeString = "This is not a legal resume stage. Press 'Cancel' and start the wizard again.";
      break;
    }
    
  return ResumeString;
}


void CHeadServiceWizard::CleanUp()
{
  WriteToLogFile(LOG_TAG_GENERAL,"Starting clean up");

  m_BackEnd->TurnUVLamps(false);
  m_BackEnd->TurnHeadFilling(false);
  m_BackEnd->SetHoodFanIdle();
  m_BackEnd->TurnCoolingFansOnOff(false);

  // Stop ignoring 'Temperature' errors at the filling stage...
  m_BackEnd->SetReportTemperatureError(true);

  //when canceling during Weight test there is folding back from MashineManager
  //during that ,Homing to all axes were performed.   and there is no need to do homing at this phase -
  //it can lead to collision when calling door->lock after door-unlock (at this example from MashineManager)
 /* if(!m_DuringWeightTestPhase)
	if (m_BackEnd->IsDoorLocked())
	  EnableAllAxesAndHome(NULL, BLOCKING, true);
  */
   if (m_BackEnd->IsDoorLocked()) /* no need to do home for Z*/
	 {
		 EnableAxeAndHome(AXIS_Y, true, true);
		 EnableAxeAndHome(AXIS_X, true, true);
	 }
  m_DuringWeightTestPhase = false;
  m_headReplacementDone = false;
  m_hswScaleBasedData->SetHeadReplacement(false);
  // Unlock the Door
  m_BackEnd->EnableDoor(false);

  // Enable vacuum errors
  m_BackEnd->EnableDisableVacuumErrorHandling(true);

   //revert unused parameters in DM mode to default (32.0)
  m_BackEnd->EnterMode(PRINT_MODE[HS_INDEX][DIGITAL_MATERIAL_OPERATION_MODE],MACHINE_QUALITY_MODES_DIR(HS_INDEX,DIGITAL_MATERIAL_OPERATION_MODE));
  m_ParamMgr->RequestedHeadVoltagesSupport[HEAD_2] = 19.0;
  m_ParamMgr->RequestedHeadVoltagesSupport[HEAD_3] = 19.0;
  m_ParamMgr->SaveSingleParameter(&m_ParamMgr->RequestedHeadVoltagesSupport);

  FOR_ALL_QUALITY_MODES(qm)
    FOR_ALL_OPERATION_MODES(om)
	  if (GetModeAccessibility(qm, om))
      {
         m_BackEnd->DisableConfirmationBypass(PER_MACHINE_MODE[qm][om], MACHINE_QUALITY_MODES_DIR(qm,om));
         if (HSW_OPERATION_MODE == om)
            m_BackEnd->DisableConfirmationBypass(CALIBRATE_FOR_HSW[qm], MACHINE_QUALITY_MODES_DIR(qm,om));
	  }

  m_BackEnd->GotoDefaultMode();

  // Activating the Waste Pump through residue valves
  // its better to activate this function after set/get actuators to prevent "two message send in the same time"
 // m_BackEnd->SetActuatorState(ACTUATOR_ID_WASTE_PUMP,false);
  m_BackEnd->ActivateRollerAndPurgeWaste(false);
  FOR_ALL_DOUBLE_HEADS(i)
  {
	m_HeadsToBeReplaced[i]         = false;
	m_HeadsInsertedWrong[i]        = false;
  }
  FOR_ALL_HEADS(i)
  {
	  m_HeadsToBeSkipped[i]          = false;
      m_LayerHeightPerHead[i]        = 0;
	  m_GainPerHeadModel[i]               = 0;
      m_GainPerHeadSupport[i]               = 0;
	  FOR_ALL_MODES(qm, om)
	  {
		 m_VoltagePerHeadModel[i][qm][om] = 0;
		 m_VoltagePerHeadSupport[i][qm][om] = 0;
      }
  }

  m_BackEnd->HeadHeatersSetIgnoreError(false);
}                  


/********************************************************************/
void CHeadServiceWizard::EnterMissingNozzlesPageLeave(CWizardPage* WizardPage, TWizardPageLeaveReason LeaveReason)
/********************************************************************/
{
   CMissingNozzlesScaleBasePage *Page = dynamic_cast<CMissingNozzlesScaleBasePage *>(WizardPage);
   m_hswScaleBasedData->ClearMissingNozzles();
   FOR_ALL_DOUBLE_HEADS(h)
   {
	  m_HeadsToBeReplaced[h] = false;  // reset data
   }
   bool needToReplace = false;
   std::vector<int>missingNozzles;
   m_GoToContinuationPage = false;
   Page->GetMissingNozzles(missingNozzles);
   m_hswScaleBasedData->SetMissingNozzles(missingNozzles);
   WriteToHistoryFileEnteredDataStr();
   WriteToHistoryFileMaterialDataStr();
   CAppParams* ParamMgr = CAppParams::Instance();
   if( m_performSecondPatternTest == false)
	m_performSecondPatternTest =  true;
   else
    m_performSecondPatternTest =  false;
	
	FOR_ALL_HEADS(h)
   {
	QString QHeadName = GetHeadName(h);
	bool isModel = m_ParamMgr->IsModelHead(h);
	const char* HeadName = QHeadName.c_str();
	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Num of missing Nozzles for head %s is %d", HeadName, m_hswScaleBasedData->GetMissingNozzlesPerHead(h)));
	 if(m_hswScaleBasedData->GetMissingNozzlesPerHead(h) > ParamMgr->HSW_MaxMissingNozzles)
		{
		  m_HeadsToBeReplaced[h/2] = true;
		  needToReplace = true;
		  if( m_performSecondPatternTest == true)
		  {
			// set new voltage
        	if(isModel)
			{
			m_ParamMgr->RequestedHeadVoltagesModel[h] += 1.5;
			if (m_ParamMgr->RequestedHeadVoltagesModel[h] > m_ParamMgr->HSW_MaxVoltageArray[0])
				m_ParamMgr->RequestedHeadVoltagesModel[h] = m_ParamMgr->HSW_MaxVoltageArray[0];
				WriteToLogFile(LOG_TAG_GENERAL,QFormatStr(" Next voltage for PT: Head %s calibrated for %f voltage",
												GetHeadName(h).c_str(),m_ParamMgr->RequestedHeadVoltagesModel[h].Value()));
			}
			else
			{
			m_ParamMgr->RequestedHeadVoltagesSupport[h]+= 1.5;
			if (m_ParamMgr->RequestedHeadVoltagesSupport[h] > m_ParamMgr->HSW_MaxVoltageArray[0])
				m_ParamMgr->RequestedHeadVoltagesSupport[h] = m_ParamMgr->HSW_MaxVoltageArray[0];
				WriteToLogFile(LOG_TAG_GENERAL,QFormatStr(" Next voltage for PT: Head %s calibrated for %f voltage",
												GetHeadName(h).c_str(),m_ParamMgr->RequestedHeadVoltagesSupport[h].Value()));
			 }
			 m_ParamMgr->SaveSingleParameter(&m_ParamMgr->RequestedHeadVoltagesModel);
 			 m_ParamMgr->SaveSingleParameter(&m_ParamMgr->RequestedHeadVoltagesSupport);
		  }
		}
	}
	if((m_performSecondPatternTest == true) && (needToReplace == true))
	{
		WriteToLogFile(LOG_TAG_GENERAL,"Too many missing nozzles, will repeat pattern-test at a higher voltage");
		//SetNextPage(qrTestPatternPreparation);
		SetNextPage(qrSecondPatternTest);
	}
	else if(needToReplace == true )
	{
	 //  SetNextPage(qrHeadReplacementStart);
	  InformTheUserOfTheDefectiveHeads();
	 if (m_GoToContinuationPage == true)
		{
		 SetNextPage(qrSelectContinuationOption);
		}
	}
	else
		SetNextPage(qrScaleSetup);
}
/********************************************************************/
void CHeadServiceWizard::ScaleSetupPageLeave(CWizardPage* WizardPage, TWizardPageLeaveReason LeaveReason)
/********************************************************************/
{
   bool communicationProblem = false;
   // check scale communication
   QString errMsg = "";
   m_scale->CheckCommunication(errMsg);
   bool cont = true;
   if(errMsg!="")
   {
	communicationProblem = true;
   }
   else
	m_hswScaleBasedData->SetScaleOnTray(true); /*the scale is on the tray*/
   //if scale communication is not OK , go to scalCommunicationProblem  screen
   if(communicationProblem == true)
   {

	  SetNextPage(qrScaleCommunicationError);
   }
   else
   {
	 if(m_BackEnd->CheckIfDoorIsClosed() == Q_NO_ERROR)
	 {
		if(m_BackEnd->EnableDoor(true,true) != Q_NO_ERROR)
			cont = false;
	 }
	 else
	 {
	  cont = false;
	 }
	  
	 if(cont == true)
		SetNextPage(qrFirstFire);
	 else
	 {
		 CCheckBoxWizardPage* CheckBoxPage = dynamic_cast<CCheckBoxWizardPage *>(Pages[qrScaleSetup]);
		  CheckBoxPage->SetChecksMask(31);
		 SetNextPage(qrScaleSetup);
	 }
   }
}
/********************************************************************/
void CHeadServiceWizard::ScaleCommunicationPageLeave(CWizardPage* WizardPage, TWizardPageLeaveReason LeaveReason)
/********************************************************************/
{
   bool communicationProblem = false;
   //if scale communication is not OK , go to scaleCommunicationProblem  screen
   QString errMsg = "";
   m_scale->CheckCommunication(errMsg);
   bool cont = true;
   if(errMsg!="")
   {
	WriteToLogFile(LOG_TAG_GENERAL,"The scale is not connected");
	communicationProblem = true;
   }
   else
   {
	WriteToLogFile(LOG_TAG_GENERAL,"The scale is connected");
	 m_hswScaleBasedData->SetScaleOnTray(true); /*the scale is on the tray*/
   }
	
   if(communicationProblem == true)
   {
	  CCheckBoxWizardPage* CheckBoxPage = dynamic_cast<CCheckBoxWizardPage *>(Pages[qrScaleCommunicationError]);
	  CheckBoxPage->SetChecksMask(0);
	  WriteToLogFile(LOG_TAG_GENERAL,"The scale isn't connected");
	  cont = false;
   }
   else
   {
	   if(m_BackEnd->CheckIfDoorIsClosed() == Q_NO_ERROR)
	   {
	        WriteToLogFile(LOG_TAG_GENERAL,"ScaleCommunicationPageLeave- doors are closed");
			if(m_BackEnd->EnableDoor(true,true) != Q_NO_ERROR)
			{
			    WriteToLogFile(LOG_TAG_GENERAL,"ScaleCommunicationPageLeave- Enable Doors failed");
				cont = false;
			    CCheckBoxWizardPage* CheckBoxPage = dynamic_cast<CCheckBoxWizardPage *>(Pages[qrScaleCommunicationError]);
				CheckBoxPage->SetChecksMask(1);
			}
		}
		else
		{
		   WriteToLogFile(LOG_TAG_GENERAL,"ScaleCommunicationPageLeave- doors are open");
		   cont = false;
		   CCheckBoxWizardPage* CheckBoxPage = dynamic_cast<CCheckBoxWizardPage *>(Pages[qrScaleCommunicationError]);
		   CheckBoxPage->SetChecksMask(1);
		}
   }
   if (cont == true)
   {
      CCheckBoxWizardPage* CheckBoxPage = dynamic_cast<CCheckBoxWizardPage *>(Pages[qrScaleCommunicationError]);
	  CheckBoxPage->SetChecksMask(0);
	  SendXYToPosition(m_ParamMgr->HSW_SCALE_X_POSITION,m_ParamMgr->HSW_SCALE_Y_POSITION);
	  SetNextPage(qrCheckTest);
   }
   else
   {
		WriteToLogFile(LOG_TAG_GENERAL,"SetNextPage(qrScaleCommunicationError)");
		SetNextPage(qrScaleCommunicationError);
	}

}

/********************************************************************/
QString CHeadServiceWizard::EnteredDataStr()
/********************************************************************/
{
	QString Str, Nozzles;
	Str = "Values entered by user: weight(missing nozzles): ";
	Str += "\n";
	 FOR_ALL_HEADS(h)
	 {
	 QString QHeadName = GetHeadName(h);
	 const char* HeadName = QHeadName.c_str();
	 Str += QFormatStr("Num of missing Nozzles for head %s is %d", HeadName, m_hswScaleBasedData->GetMissingNozzlesPerHead(h));
	 Str += ":\n";
	 }
	
	return Str;
}
/********************************************************************/
QString CHeadServiceWizard::MaterialDataStr()
/********************************************************************/
{
	
 QString Str = "";

	for (int HeadID = FIRST_HEAD ; HeadID < LAST_HEAD; HeadID++)
	{
	int chamberIndex = GetHeadsChamberForHSW(HeadID,true);

	TTankIndex ActiveTankID = m_BackEnd->GetActiveTankNum((TChamberIndex)chamberIndex);
	if(m_ParamMgr->HSW_DM3_Enable != true && ((HeadID != 2) && (HeadID!= 3)))
	 if (NO_TANK == ActiveTankID)
		throw EQException("Error while trying to get active tank number " );
	QString material = m_ParamMgr->TypesArrayPerChamber[TankToStaticChamber(ActiveTankID)];
	Str += QFormatStr("Head %s Chamber %s MaterialType(of Chamber) %s",GetHeadName(HeadID).c_str(),
													ChamberToStr((TChamberIndex)chamberIndex).c_str() ,
													material.c_str());
	Str += "\n";
  }
  return Str;
}
/********************************************************************/
void CHeadServiceWizard::WriteToHistoryFileEnteredDataStr()
/********************************************************************/
{
	HistoryFile->WriteLine();
	HistoryFile->SkipLines(1);
	HistoryFile->WriteDate();
	HistoryFile->WriteMachineName();
	HistoryFile->SkipLines(1);
	HistoryFile->Append(EnteredDataStr());
}

/********************************************************************/
void CHeadServiceWizard::WriteToHistoryFileMaterialDataStr()
/********************************************************************/
{
    HistoryFile->WriteLine();
	HistoryFile->SkipLines(1);
	HistoryFile->WriteDate();
	HistoryFile->WriteMachineName();
	HistoryFile->SkipLines(1);			
	HistoryFile->Append(MaterialDataStr());

}
/********************************************************************/
void CHeadServiceWizard::PrintCarpet()
/********************************************************************/
{
  TFileNamesArray FileNames = NULL;
  try
  {
  // No need in Enter mode every time
   TFileNamesArray FileNames = (TFileNamesArray)malloc(TFileNamesArraySIZE);
   memset(FileNames, 0, TFileNamesArraySIZE);
   strcpy((TBMPFileName)FileNames + FIRST_MODEL_CHAMBER_INDEX*MAX_FILE_PATH,
		  FULL_PATH(CARPET_FILE_NAME).c_str());

   LoadLocalPrintJob( FileNames,DPI[CARPET_MODE_QUALITY_MODE],CARPET_MODE_QUALITY_MODE,DIGITAL_MATERIAL_OPERATION_MODE,NUMBER_OF_CARPET_SLICES);
   m_BackEnd->SetOnlineOffline(true);
   m_UVLampsTimer->Enabled = false;
   m_UVLampsTimer->Enabled = true;

   WaitForEndOfPrint();
  }
  __finally
  {
   if(FileNames)
	  free(FileNames);
	m_UVLampsTimer->Enabled = false;
  }
}
/********************************************************************/
void CHeadServiceWizard::PrintTemplate()
/********************************************************************/
{
  TFileNamesArray FileNames = NULL;
  try
  {
   if(m_ParamMgr->HSW_SCALE_EMULATION == false)
   {
  // No need in Enter mode every time
   TFileNamesArray FileNames = (TFileNamesArray)malloc(TFileNamesArraySIZE);
   memset(FileNames, 0, TFileNamesArraySIZE);
   strcpy((TBMPFileName)FileNames + 1*MAX_FILE_PATH,
		  FULL_PATH(TEMPLATE_FILE_NAME).c_str());


   CScopeRestorePoint<float> scKeepXStartPosition(m_ParamMgr->TrayStartPositionX);
   CScopeRestorePoint<float> scKeepYStartPosition(m_ParamMgr->TrayStartPositionY);
   
   m_ParamMgr->TrayStartPositionX = m_ParamMgr->HSW_Scales_X_bitmap;
   m_ParamMgr->TrayStartPositionY = m_ParamMgr->HSW_Scales_Y_bitmap;
   WriteToLogFile(LOG_TAG_GENERAL,"Frame printing ... ");
   LoadLocalPrintJob(FileNames,DPI[CARPET_MODE_QUALITY_MODE],HQ_INDEX,DIGITAL_MATERIAL_OPERATION_MODE,2);
   m_BackEnd->SetOnlineOffline(true);
   m_UVLampsTimer->Enabled = false;
   m_UVLampsTimer->Enabled = true;

   WaitForEndOfPrint();
   }
   m_hswScaleBasedData->setFramePrinted(true);
   m_BackEnd->SetOnlineOffline(false);
  }
  __finally
  {
   if(FileNames)
	  free(FileNames);
	m_UVLampsTimer->Enabled = false;
  }
}
void CHeadServiceWizard::ClearBitmap(Graphics::TBitmap * bmp, int Width, int Height)
{
  bmp->Monochrome  = true;
  bmp->Height      = Height;
  bmp->PixelFormat = pf1bit;
  bmp->Width       = Width;

  bmp->Canvas->Brush->Color = clBlack;
  bmp->Canvas->FloodFill(0, 0, clWhite, fsSurface);
}


void CHeadServiceWizard::RecalcStartPosition(int PrintCount,int TotalPrintCount,int AxisZLevel, int* XOrder, int* YOrder)
/********************************************************************/
{ //The assumption is : voltage is the inner loop, quality is the outer

  float AdditionalZOffset = 0.0;
#ifdef CONNEX_260
  AdditionalZOffset = 1.0;
#endif
  const int PrintsInAxisX = INT_FROM_RESOURCE(IDN_HSW_MAX_PRINT_NUM_IN_AXIS_X);
  if (PrintCount == 0)
	 m_ParamMgr->Z_StartPrintPosition += AxisZLevel*FLOAT_FROM_RESOURCE(IDN_HSW_TRAY_START_POSITION_Z_OFFSET) + AdditionalZOffset;

  if (PrintCount%(TotalPrintCount/PrintsInAxisX) == 0)
  {
	 m_ParamMgr->TrayStartPositionY    = 0;
     if (YOrder) *YOrder = 0;
     if (PrintCount != 0)
     {
       m_ParamMgr->TrayStartPositionX += FLOAT_FROM_RESOURCE(IDN_HSW_TRAY_START_POSITION_X_OFFSET);
       if (XOrder) (*XOrder)++;
     }
  }
  else
  {
     m_ParamMgr->TrayStartPositionY += FLOAT_FROM_RESOURCE(IDN_HSW_TRAY_START_POSITION_Y_OFFSET);
     if (YOrder) (*YOrder)++;
  }


  CScatterGenerator::Init();
  const int TrayRearOffset = (int)(CScatterGenerator::s_TrayRearOffset  * MM_PER_INCH / m_ParamMgr->DPI_InYAxis) + 1;
  m_ParamMgr->TrayStartPositionY += TrayRearOffset;
}

void CHeadServiceWizard::SaveToFile(Graphics::TBitmap* bmp, QString file)
{
  QString FileName = FULL_PATH("");
  try
  {
    ForceDirectories(FULL_PATH("").c_str()); //force the path of file
    FileName += file;
	bmp->SaveToFile(FileName.c_str());
  	WriteToLogFile(LOG_TAG_GENERAL,FileName + " was created");
  }
  catch(...)
  {
  	throw EQException("Error while trying to save " + FileName);
  }
}

/********************************************************************/
void CHeadServiceWizard::CreateHeadRect(Graphics::TBitmap * bmp,
                                        int RectStart,
                                        int RectEnd,
                                        int OnSize,
                                        int OffSize,
                                        int RecOffset)
/********************************************************************/
{
  TRect Rect;
  Rect.Left = RectStart;
  Rect.Right = RectEnd;
  bmp->Canvas->Brush->Color = clWhite;
  for(int i = 0;; i++)
  {
     Rect.Top  = RecOffset*OnSize + i*(OnSize + OffSize);
	 if (Rect.Top > bmp->Height)
        return;
	 Rect.Bottom = Rect.Top + OnSize;
     bmp->Canvas->FillRect(Rect);
  }
}//CreateHeadRect


//Trying to find heads with minimal layer height and suggest the user to replace them.
void CHeadServiceWizard::ExcludeHeadsWithMinimalLayerHeight(int QualityMode, int OperationMode)
{
	if( !IsThereHeadWithVoltageBelowMinimum(QualityMode, OperationMode) ) // No such head
	{
		WriteToLogFile(LOG_TAG_GENERAL,"There is no head with voltage below minimum");
		return;
	}
   	//Add to the defective head list the lowest head.
    RemoveDefectiveHead(QualityMode,OperationMode);

    CalcVoltage(QualityMode, OperationMode);
    //Recalculate final layer height - the min from all the heads - only with the good heads  - m_FinalLayerHeight
    CalcFinalLayerHeight(QualityMode, OperationMode);
    //Recalculate new voltages - only with the good heads - CalculateVoltageForNonDefectiveHeads
	CalcFinalVoltage(QualityMode, OperationMode);
	ExcludeHeadsWithMinimalLayerHeight(QualityMode, OperationMode);
}

void CHeadServiceWizard::RemoveDefectiveHead(int QualityMode,int OperationMode)
{
  FOR_ALL_HEADS(i)
	 if (m_FinalLayerHeight[QualityMode][OperationMode] == m_LayerHeightPerHead[i])
         m_HeadsToBeReplaced[i/2] = true;
}

void CHeadServiceWizard::CalcFinalLayerHeight(int QualityMode, int OperationMode) //calculate final layer height - the min from all the heads
{
  YieldWizardThread();
  m_FinalLayerHeight[QualityMode][OperationMode] = 10000;
  FOR_ALL_HEADS(i)
	 if (!m_HeadsToBeReplaced[i/2])
         m_FinalLayerHeight[QualityMode][OperationMode] = std::min(m_FinalLayerHeight[QualityMode][OperationMode], m_LayerHeightPerHead[i]);
}

bool CHeadServiceWizard::IsThereHeadWithVoltageBelowMinimum(int QualityMode, int OperationMode)
{
  FOR_ALL_HEADS(i)
  {
	bool isModel = m_ParamMgr->IsModelHead(i);
     if (m_HeadsToBeReplaced[i/2])
        continue;
        
	 // Finding if the calculated voltage of the certain head is lower then minimum allowed voltage
	 if(isModel)
	 {
		if (m_VoltagePerHeadModel[i][QualityMode][OperationMode] < m_ParamMgr->HSW_MinVoltageArray[GetHeadsChamberForHSW(i,OperationMode)])
		 {
		CQLog::Write(LOG_TAG_GENERAL,GetHeadName(i) + " head voltage is below minimum.");
		return true; //FoundHeadWithVoltageBelowMinimum
		}
	 }
	 else
	 {
	   if (m_VoltagePerHeadSupport[i][QualityMode][OperationMode] < m_ParamMgr->HSW_MinVoltageArray[GetHeadsChamberForHSW(i,OperationMode)])
		 {
		CQLog::Write(LOG_TAG_GENERAL,GetHeadName(i) + " head voltage is below minimum.");
		return true; //FoundHeadWithVoltageBelowMinimum
		}
	 }
  }
  return false;
}

void CHeadServiceWizard::InformTheUserOfTheDefectiveHeads()
{
  m_ContinuationOptionPageSubtitle = "";
  TQStringVector tokens;
  FOR_ALL_DOUBLE_HEADS(i)
	 if (m_HeadsToBeReplaced[i])
		tokens.push_back(GetDoubleHeadName(i));

  if (tokens.size() > 0)
  {
	 JoinTokens(m_ContinuationOptionPageSubtitle, tokens, ",");
	 m_ContinuationOptionPageSubtitle += " defective. \nHead replacement required.";
	 m_GoToContinuationPage = true;
  }
}

void CHeadServiceWizard::SaveAndCalibrate(TQualityModeIndex QualityMode, int OperationMode)
{
  if (ALL_QUALITY_MODES == QualityMode)
  {
	 FOR_ALL_QUALITY_MODES(qm)
		 SaveAndCalibrate((TQualityModeIndex)qm, OperationMode);
	 return;
  }

  if (ALL_OPERATION_MODES == OperationMode)
  {
	 FOR_ALL_OPERATION_MODES(om)
		 SaveAndCalibrate(QualityMode, om);
     return;
  }

  if ( !GetModeAccessibility(QualityMode, OperationMode) )
	 return;

  // Everything went well - we are going to calibration the voltages and to save all the relevant parameters
  m_BackEnd->GotoDefaultMode();
  m_BackEnd->EnterMode(PRINT_MODE[QualityMode][OperationMode],
					   MACHINE_QUALITY_MODES_DIR(QualityMode,OperationMode));
  WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Final Layer Height %s = %f",
											GetModeStr(QualityMode,OperationMode).c_str(),
											m_FinalLayerHeight[QualityMode][OperationMode]));
  m_ParamMgr->LayerHeightDPI_um = m_FinalLayerHeight[QualityMode][OperationMode] / m_ParamMgr->ProductLT_n - m_ParamMgr->ProductLT_m;
  m_ParamMgr->SaveSingleParameter(&m_ParamMgr->LayerHeightDPI_um);
  m_hswCalibrationData->SetLayerHeightDPI_um(m_ParamMgr->LayerHeightDPI_um);
  WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Final Layer Height Net %s = %f",
											GetModeStr(QualityMode,OperationMode).c_str(),
										   (float)m_ParamMgr->LayerHeightDPI_um));

  QString ExceptionStr,VoltagesStr;
  FOR_ALL_HEADS(i)
  {
	bool isModel = m_ParamMgr->IsModelHead(i);
	 try
	 {
		if(isModel)
		{
			m_BackEnd->SetAndMeasureHeadVoltage(i, m_VoltagePerHeadModel[i][QualityMode][OperationMode]);
			m_ParamMgr->RequestedHeadVoltagesModel[i] = m_VoltagePerHeadModel[i][QualityMode][OperationMode];
			VoltagesStr += GetHeadName(i) + " voltage: " + QFloatToStr(m_ParamMgr->RequestedHeadVoltagesModel[i]) + "V\n";
		}
		else
		{
			m_BackEnd->SetAndMeasureHeadVoltage(i, m_VoltagePerHeadSupport[i][QualityMode][OperationMode]);
			m_ParamMgr->RequestedHeadVoltagesSupport[i] = m_VoltagePerHeadSupport[i][QualityMode][OperationMode];
			VoltagesStr += GetHeadName(i) + " voltage: " + QFloatToStr(m_ParamMgr->RequestedHeadVoltagesSupport[i]) + "V\n";
		 }
		m_ParamMgr->PotentiometerValues[i]   = m_BackEnd->GetCurrHeadPotentiometerValue(i);
	 }
	 catch(EQException& err)
	 {
		ExceptionStr += GetHeadName(i)+", ";
	 }
  }
  if (ExceptionStr != "")//Error has been found
  {
	 DEL_LAST_2_CHARS(ExceptionStr);
	 throw EQException("Failed to calibrate " + ExceptionStr + "." + "\nContact Customer Support for assistance.");
  }
  m_ParamMgr->SaveSingleParameter(&m_ParamMgr->RequestedHeadVoltagesModel);
  m_ParamMgr->SaveSingleParameter(&m_ParamMgr->RequestedHeadVoltagesSupport);
  m_ParamMgr->SaveSingleParameter(&m_ParamMgr->PotentiometerValues);

  // Write to the Head Service Wizard History file
    HistoryFile->Append("\n---- Parameters which calibrated in " + GetModeStr(QualityMode,OperationMode) + " mode ----\n");
  HistoryFile->Append("Final layer height: " + QFloatToStr(m_ParamMgr->LayerHeightDPI_um) + " mic\n");
  HistoryFile->Append(VoltagesStr);

}//SaveAndCalibrate

int CHeadServiceWizard::FindWeakestHead()
{
  int WeakestHeadIndex = 0;
  float Minimum = m_LayerHeightPerHead[WeakestHeadIndex];
  FOR_ALL_HEADS(i)
      if (Minimum > m_LayerHeightPerHead[i])
      {
		 WeakestHeadIndex = i;
		 Minimum = m_LayerHeightPerHead[WeakestHeadIndex];
      }
  return WeakestHeadIndex;
}

void CHeadServiceWizard::LoadLocalPrintJob(const TFileNamesArray FileNames,
                                          int                   BitmapResolution,
										  int                   QualityMode,
                                          int                   OperationMode,
                                          int                   SliceNum)
{
  return m_BackEnd->LoadLocalPrintJob(FileNames,BitmapResolution,QualityMode,OperationMode,SliceNum);
}

void __fastcall CHeadServiceWizard::UVTurnOFFTimerEvent(TObject *Sender)
{
   WriteToLogFile(LOG_TAG_GENERAL,"UV lamps turn OFF after timeout.");
   //Make sure the uv really gets off
   CScopeRestorePoint<bool> scKeepUvOn(m_ParamMgr->KeepUvOn);
   m_ParamMgr->KeepUvOn = false;
   m_BackEnd->TurnUVLamps(false);
}
void CHeadServiceWizard::CheckCalibrationTest(CWizardPage *WizardPage,int operationMode,
										 int qualityMode)
{
	 std::vector<float> maxLayerHeadV;
     float ProductLT_m    = m_ParamMgr->ProductLT_m;
	 float ProductLT_n    = m_ParamMgr->ProductLT_n;
	 float targetLayerGross;
	 #ifdef OBJET_MACHINE_KESHET
	 int numOfFires = m_ParamMgr->HSW_Num_of_fires;
	#else
	 int numOfFires = m_ParamMgr->HSW_Num_of_fires/1000;
	 #endif
   m_hswScaleBasedData->ClearMeasurements();

   WriteToLogFile(LOG_TAG_GENERAL,"Head Optimization Wizard: Check Current Calibration test");
   CSuspensionPointsStatusPage* pPage= dynamic_cast< CSuspensionPointsStatusPage *>(Pages[qrCheckTest]);
	float minMaxLayerHead ;
		// read current voltage and perform measurement
	FOR_ALL_HEADS(i)
  {
	 QString QHeadName = GetHeadName(i);
	 const char* HeadName = QHeadName.c_str();
	 QString title = "Check calibration: " +  QHeadName;
	 pPage->PointsStatusMessage = title.c_str();
	 pPage->Refresh();
	 YieldWizardThread();
	 m_ParamMgr->HSW_HeadStatus[i] = 1 ;
	 if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
	 if(m_ParamMgr->IsModelHead(i))
		{                  
		if(m_ParamMgr->HSW_LimitVoltage[qualityMode]< m_ParamMgr->RequestedHeadVoltagesModel[i])
			{
			  WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head Optimization Wizard:  head %s limit > requested  = %s",HeadName,QFloatToStr(m_ParamMgr->RequestedHeadVoltagesModel[i].Value()).c_str()));

			 m_ParamMgr->HSW_HeadStatus[i] = 2 ;
			  WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head Optimization Wizard: Head %s status is 2",HeadName));
			}
		}
		else
		{
		   if(m_ParamMgr->HSW_LimitVoltage[qualityMode]< m_ParamMgr->RequestedHeadVoltagesSupport[i])
		   {
			 WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head Optimization Wizard:  head %s limit > requested  = %s",HeadName,QFloatToStr(m_ParamMgr->RequestedHeadVoltagesSupport[i].Value()).c_str()));

			m_ParamMgr->HSW_HeadStatus[i] = 2 ;
			 WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head Optimization Wizard: Head %s status is 2",HeadName));
		   }
		}
   if(m_ParamMgr->HSW_HeadStatus[i] == 1)
   {
	 //weightTest
	float measurement;
	float gain_m,gain;
	int error = 0;
	if(m_ParamMgr->IsModelHead(i))
	{
		VoltageCalibration(i, m_ParamMgr->RequestedHeadVoltagesModel[i]);
	}
		else
		{
		 VoltageCalibration(i, m_ParamMgr->RequestedHeadVoltagesSupport[i]);
		}

   if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
   if(WeightTest(i,measurement,qualityMode,error) == false)
   {
		if(error == SCALE_COMMUNICATION_FAIL)
		{
		 SetNextPage(qrScaleCommunicationError);
		 GotoNextPage();
		 return;
		}
		else
		if(error == WEIGHT_TEST_FAIL)
		{
		   WriteToLogFile(LOG_TAG_GENERAL,"CheckCalibrationTest , wrong weight test, continue");
		}
		else
		if(error == SCALE_PROBLEM)
		{
		  SetNextPage(qrScaleProblem);
		  GotoNextPage();
		  return;
		}
   }
	else
	if(measurement == 0 && m_ParamMgr->HSW_SCALE_EMULATION == false)
	{
	  SetNextPage(qrScaleCommunicationError);
	  GotoNextPage();
	  return;
	}
	m_hswScaleBasedData->AddMeasurement(measurement);
	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("%s,Measurement  = %f",HeadName, measurement));
   //calculate maxLayerHead formula 2.1 , gross value
	float factor;
	if (m_ParamMgr->IsModelHead(i))
	{
	   factor = m_ParamMgr->MaterialsWeightFactorPerModeArrayModel[operationMode];
	}
	else
		factor = m_ParamMgr->MaterialsWeightFactorPerModeArraySupport[operationMode];
	int     HeadsChamber = GetHeadsChamberForHSW( i , operationMode,true);

	if(HeadsChamber==TYPE_CHAMBER_MODEL7)  // DM6
		  HeadsChamber = TYPE_CHAMBER_SUPPORT;

	double maxLayerHead =  m_ParamMgr->HSW_MaxLayerHeightArray[qualityMode];

  //	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("%s, Maximal layer head   = %f", HeadName, maxLayerHead));

	maxLayerHeadV.push_back(maxLayerHead);
  }
  else
  {
	m_hswScaleBasedData->AddMeasurement(0);
	double maxLayerHead = (m_ParamMgr->HSW_MaxLayerHeightArray[qualityMode]+ ProductLT_m) * ProductLT_n;
	maxLayerHeadV.push_back(maxLayerHead);
  }

}
	minMaxLayerHead =  (m_ParamMgr->HSW_MaxLayerHeightArray[qualityMode]+ ProductLT_m) * ProductLT_n;
    WriteToLogFile(LOG_TAG_GENERAL,QFormatStr(" MinMaximal layer head   = %f", minMaxLayerHead));
   // calculate target layer

	targetLayerGross = CalculateTargetLayer(qualityMode,minMaxLayerHead);
	// calculate target weight, formula 3
	//weight=  (target_layer_gross×(nozzles_head-missing nozzles)×#_of_fires×SG×2.54^2×head_factor)/(ResX×ResY×material_factor×10^4 )
	std::vector<float>targetWeight;
	CalculateTargetWeight(targetWeight,targetLayerGross,operationMode);
  FOR_ALL_HEADS(i)
	{
	if(m_ParamMgr->HSW_HeadStatus[i] == 1)
	{
      QString QHeadName = GetHeadName(i);
	 const char* HeadName = QHeadName.c_str();
	  float currentWeight = m_hswScaleBasedData->GetMeasurement(i);
	  WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head Optimization Wizard:  current weight  = %f",currentWeight));
	  float deltaW = targetWeight[i] - currentWeight;
	  WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head Optimization Wizard:  head %s delta weight  = %f",HeadName,deltaW));
	  if (abs(deltaW) < m_ParamMgr->HSW_Scales_res * 1.5)
	   {
		  m_ParamMgr->HSW_HeadStatus[i] = 0 ;
		  WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head Optimization Wizard: Head %s calibration is satisfactory",HeadName));
	   }
	   else
	   if(abs(deltaW) < m_ParamMgr->HSW_Scales_res * 3)
	   {
		 float dV = 0.0;
		 float a =  deltaW * pow((double)10, (double)9) * 0.75;
		 float b =  numOfFires * (NOZZLES_PER_HEAD - m_hswScaleBasedData->GetMissingNozzlesPerHead(i)) * m_ParamMgr->HSW_Gain_max ;
		 dV = a/b;
		 WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head Optimization Wizard:  a  = %f",a));
		 WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head Optimization Wizard:  b  = %f",b));
		 WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head Optimization Wizard:  dV  = %f",dV));
		 WriteToLogFile(LOG_TAG_GENERAL,"Head Optimization Wizard: Head ** voltage tweak done");
         m_ParamMgr->HSW_HeadStatus[i] = 0 ;
		 // save voltage

	   if(m_ParamMgr->IsModelHead(i))
	   {
		m_ParamMgr->RequestedHeadVoltagesModel[i] += dV;
		m_BackEnd->SetAndMeasureHeadVoltage(i,m_ParamMgr->RequestedHeadVoltagesModel[i]);
		WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Save voltage %s for head %s",
												QFloatToStr(m_ParamMgr->RequestedHeadVoltagesModel[i].Value()).c_str(),
												GetHeadName(i).c_str()));
	   }
		else
		{
		 m_ParamMgr->RequestedHeadVoltagesSupport[i] += dV;
		 m_BackEnd->SetAndMeasureHeadVoltage(i,m_ParamMgr->RequestedHeadVoltagesSupport[i]);
		 WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Save voltage %s for head %s",
												QFloatToStr(m_ParamMgr->RequestedHeadVoltagesSupport[i].Value()).c_str(),
												GetHeadName(i).c_str()));
		}

		m_ParamMgr->PotentiometerValues[i]   = m_BackEnd->GetCurrHeadPotentiometerValue(i);
		m_ParamMgr->SaveSingleParameter(&m_ParamMgr->RequestedHeadVoltagesModel);
		m_ParamMgr->SaveSingleParameter(&m_ParamMgr->RequestedHeadVoltagesSupport);
		m_ParamMgr->SaveSingleParameter(&m_ParamMgr->PotentiometerValues);
	   }
	   else
	   {
		  m_ParamMgr->HSW_HeadStatus[i] = 1 ;
		  WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head Optimization Wizard: Head %s status is 1",HeadName));
	   }

	}
	}
    FOR_ALL_HEADS(i)
	{
	   if( m_ParamMgr->HSW_HeadStatus[i] == 2)
		 m_ParamMgr->HSW_HeadStatus[i]=1;
	}
   SetNextPage(qrFindTargetLayer);
   GotoNextPage();

}
void CHeadServiceWizard::FirstFire(CWizardPage *WizardPage,int operationMode,
										 int qualityMode)
{
 CSuspensionPointsStatusPage* pPage= dynamic_cast< CSuspensionPointsStatusPage *>(Pages[qrFirstFire]);
  pPage->PointsStatusMessage = "";
  pPage->Refresh();
  int headID = 0;
  float measurement_v1;
  float MaxLayerHeight = m_ParamMgr->HSW_MaxLayerHeightArray[qualityMode];
  QString QHeadName = GetHeadName(headID);
  const char* HeadName = QHeadName.c_str();
  QString title = "First Fire " +  QHeadName;
  pPage->PointsStatusMessage = title.c_str();
  pPage->Refresh();
  YieldWizardThread();
  if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
  m_BackEnd->EnableMotor(true, AXIS_Y);
  m_BackEnd->EnableMotor(true, AXIS_X);
  TQErrCode Err = Q_NO_ERROR;
  TRY_FUNCTION(Err, m_BackEnd->MoveMotorToAbsolutePosition(AXIS_Y, m_ParamMgr->HSW_SCALE_Y_POSITION, NO_BLOCKING));
  TRY_FUNCTION(Err, m_BackEnd->MoveMotorToAbsolutePosition(AXIS_X, m_ParamMgr->HSW_SCALE_X_POSITION, NO_BLOCKING));

  TRY_FUNCTION(Err, m_BackEnd->WaitForEndOfMovement(AXIS_Y));
  TRY_FUNCTION(Err, m_BackEnd->WaitForEndOfMovement(AXIS_X));

  m_BackEnd->EnableMotor(false, AXIS_Y);
  m_BackEnd->EnableMotor(false, AXIS_X);
  /*print current voltage */
 for (int i = 0; i < TOTAL_NUMBER_OF_HEADS; i++)
	{
  if(m_ParamMgr->IsModelHead(i))
	   {
		WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("------Model voltage %s for head %s",
												QFloatToStr(m_ParamMgr->RequestedHeadVoltagesModel[i].Value()).c_str(),
												GetHeadName(i).c_str()));
	   }
		else
		{
		 WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("------Support voltage %s for head %s",
												QFloatToStr(m_ParamMgr->RequestedHeadVoltagesSupport[i].Value()).c_str(),
												GetHeadName(i).c_str()));
		}
  }
  // SetFireAll
  	if(m_ParamMgr->IsModelHead(headID))
	{
		VoltageCalibration(headID, m_ParamMgr->RequestedHeadVoltagesModel[headID]);
	}
	else
	{
		VoltageCalibration(headID, m_ParamMgr->RequestedHeadVoltagesSupport[headID]);
	}

  //weightTest
  float measurement;
  int error = 0;
   if(WeightTest(headID,measurement_v1,qualityMode,error) == false)
   {
		if(error == SCALE_COMMUNICATION_FAIL)
		{
		 WriteToLogFile(LOG_TAG_GENERAL,"First Fire , communication error");
		 SetNextPage(qrScaleCommunicationError);
		 GotoNextPage();
		 return;
		}
		else
		if(error == WEIGHT_TEST_FAIL)
		{
		   WriteToLogFile(LOG_TAG_GENERAL,"First Fire , wrong weight test, continue");
		}
		else
		if(error == SCALE_PROBLEM)
		{
		  WriteToLogFile(LOG_TAG_GENERAL,"First Fire , scale problem");
		}
   }
	else
	if(measurement_v1 == 0 && m_ParamMgr->HSW_SCALE_EMULATION == false)
	{
	  SetNextPage(qrScaleCommunicationError);
	  GotoNextPage();
	  return;
	}

	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("%s,Measurement V1  = %f",HeadName, measurement_v1));
	// compare   the Layer-Height (ParameterManager->Setup->LayerHeightDPI_um) with the MaxLayerHeight parameter (ParameterManager->HOW):
	if (m_ParamMgr->LayerHeightDPI_um < MaxLayerHeight)
	{
	 // head status = 1
	 for (int i = 0; i < TOTAL_NUMBER_OF_HEADS; i++)
	{
	 m_ParamMgr->HSW_HeadStatus[i] = 1;
	}
	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("MaxLayerHeight  = %f",MaxLayerHeight));
	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("LayerHeightDPI_um  = %f",m_ParamMgr->LayerHeightDPI_um.Value()));
	 WriteToLogFile(LOG_TAG_GENERAL,"Head Optimization Wizard: Going to full calibration");
	 SetNextPage(qrFindTargetLayer);
	 GotoNextPage();
	}
	else
	{
	WriteToLogFile(LOG_TAG_GENERAL,"Head Optimization Wizard: Going to Check Current Calibration Test");
	SetNextPage(qrCheckTest);
	GotoNextPage();
	}
}
void CHeadServiceWizard::GetMinMaxLHnetSeqeunce(CWizardPage *WizardPage,int operationMode,
										 int qualityMode)
{
  int voltage_iteration_1 = 0;
  int voltage_iteration_2 = 0;
  float ProductLT_m    = m_ParamMgr->ProductLT_m;
  float ProductLT_n    = m_ParamMgr->ProductLT_n;
  std::vector<float> maxLayerHeadV;
  bool headStatusWasZero = false;
  float targetLayerGross;
  float targetLayerNet;
  #ifdef OBJET_MACHINE_KESHET
 int numOfFires = m_ParamMgr->HSW_Num_of_fires/m_ParamMgr->HSW_Num_of_cycles_for_fires;
 #else
 int numOfFires = m_ParamMgr->HSW_Num_of_fires/m_ParamMgr->HSW_Num_of_cycles_for_fires/1000;
 #endif
   CSuspensionPointsStatusPage* pPage= dynamic_cast< CSuspensionPointsStatusPage *>(Pages[qrFindTargetLayer]);
  pPage->PointsStatusMessage = "";
  pPage->Refresh();
  FOR_ALL_HEADS(i)
  {
	 QString QHeadName = GetHeadName(i);
	 const char* HeadName = QHeadName.c_str();
	 maxLayerHeadV.push_back((m_ParamMgr->HSW_MaxLayerHeightArray[qualityMode] + ProductLT_m) * ProductLT_n);
	if (m_ParamMgr->HSW_HeadStatus[i] == 1)
	{

	 QString title = "Voltage Calibration " +  QHeadName;
	 pPage->PointsStatusMessage = title.c_str();
	 pPage->Refresh();
	 YieldWizardThread();
	 WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("GetMinMaxLHnetSeqeunce Head %s",HeadName));

	 if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
	// SetFireAll
	//calibration to V1
	if (m_ParamMgr->IsModelHead(i))
	{
		voltage_iteration_1 = m_ParamMgr->HSW_VOLTAGE_ITERATION_M_1[qualityMode];
		voltage_iteration_2 = m_ParamMgr->HSW_VOLTAGE_ITERATION_M_2[qualityMode];
	}
	else
	{
		voltage_iteration_1 = m_ParamMgr->HSW_VOLTAGE_ITERATION_S_1[qualityMode];
		voltage_iteration_2 = m_ParamMgr->HSW_VOLTAGE_ITERATION_S_2[qualityMode];
	}

	VoltageCalibration(i, voltage_iteration_1);
	//weightTest
	float measurement;
	float measurement_v1,measurement_v2;
	float gain_m,gain;
	int error = 0;
   if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
   if(WeightTest(i,measurement_v1,qualityMode,error) == false)
   {
		if(error == SCALE_COMMUNICATION_FAIL)
		{
		 SetNextPage(qrScaleCommunicationError);
		 GotoNextPage();
		 return;
		}
		else
		if(error == WEIGHT_TEST_FAIL)
		{
		  CMessageWizardPage *Page = dynamic_cast<CMessageWizardPage *>(GetPageById(qrVoltageProblem));
		   QString msg = "The wizard cannot optimize the voltage for ";
		   msg+= GetDoubleHeadName(i/2).c_str();
		   msg+= ".\nYou should replace this print head.\nRun the wizard from the begining.\nSelect 'Replace faulty heads...' to replace the head,\nand then continue the HOW to optimize the print heads.";
		   Page->SubTitle = msg;
		   i=m_ParamMgr->HSW_Max_count;
		   m_targetWeightNotReached = true;
		   SetNextPage(qrVoltageProblem);
		   GotoNextPage();
		   return;
		}
		else
		if(error == SCALE_PROBLEM)
		{
		  SetNextPage(qrScaleProblem);
		  GotoNextPage();
		  return;
		}
   }
	else
	if(measurement_v1 == 0 && m_ParamMgr->HSW_SCALE_EMULATION == false)
	{
	  SetNextPage(qrScaleCommunicationError);
	  GotoNextPage();
	  return;
	}
	m_hswScaleBasedData->AddMeasurementForV1(measurement_v1);
	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("%s,Measurement V1  = %f",HeadName, measurement_v1));

	// V2
	//calibration to V2
	VoltageCalibration(i, voltage_iteration_2);
	if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
	if(WeightTest(i,measurement_v2,qualityMode,error) == false)
   {
		if(error == SCALE_COMMUNICATION_FAIL)
		{
		 SetNextPage(qrScaleCommunicationError);
		 GotoNextPage();
		 return;
		}
		else
		if(error == WEIGHT_TEST_FAIL)
		{
		   CMessageWizardPage *Page = dynamic_cast<CMessageWizardPage *>(GetPageById(qrVoltageProblem));
		   QString msg = "The wizard cannot optimize the voltage for ";
		   msg+= GetDoubleHeadName(i/2).c_str();
		   msg+= ".\nYou should replace this print head.\nRun the wizard from the begining.\nSelect 'Replace faulty heads...' to replace the head,\nand then continue the HOW to optimize the print heads.";
		   Page->SubTitle = msg;
		   i=m_ParamMgr->HSW_Max_count;
		   m_targetWeightNotReached = true;
		   SetNextPage(qrVoltageProblem);
		   GotoNextPage();
		   return;
		}
		else
		if(error == SCALE_PROBLEM)
		{
		  SetNextPage(qrScaleProblem);
		  GotoNextPage();
		  return;
		}
   }
	else
	if(measurement_v2 == 0 && m_ParamMgr->HSW_SCALE_EMULATION == false)
	{
	  SetNextPage(qrScaleCommunicationError);
	  GotoNextPage();
	  return;
	}
	m_hswScaleBasedData->AddMeasurementForV2(measurement_v2);
	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("%s,Measurement V2  = %f",HeadName, measurement_v2));

	gain_m = ((measurement_v1 - measurement_v2)/
	(voltage_iteration_1 - voltage_iteration_2))*1.01;
	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("%s,Gain_m before checking gain limit = %f",HeadName, gain_m));

	if( gain_m < (m_ParamMgr->HSW_Gain_min*(NOZZLES_PER_HEAD - m_hswScaleBasedData->GetMissingNozzlesPerHead(i))*numOfFires*m_ParamMgr->HSW_Num_of_cycles_for_fires/
	(pow((double)10, (double)9))))
		gain_m = m_ParamMgr->HSW_Gain_min*(NOZZLES_PER_HEAD - m_hswScaleBasedData->GetMissingNozzlesPerHead(i))*numOfFires*m_ParamMgr->HSW_Num_of_cycles_for_fires/
		(pow((double)10, (double)9)) ;
	 else
		if( gain_m > (m_ParamMgr->HSW_Gain_max*(NOZZLES_PER_HEAD - m_hswScaleBasedData->GetMissingNozzlesPerHead(i))*numOfFires*m_ParamMgr->HSW_Num_of_cycles_for_fires/
	(pow((double)10, (double)9))))
		gain_m = m_ParamMgr->HSW_Gain_max*(NOZZLES_PER_HEAD - m_hswScaleBasedData->GetMissingNozzlesPerHead(i))*numOfFires*m_ParamMgr->HSW_Num_of_cycles_for_fires/
		(pow((double)10, (double)9)) ;

	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("%s,Gain_m  = %f",HeadName, gain_m));
	m_hswScaleBasedData->AddGainV1(gain_m);
	gain = gain_m*(pow((double)10, (double)9))/
	(m_ParamMgr->HSW_Num_of_fires*((NOZZLES_PER_HEAD-m_hswScaleBasedData->GetMissingNozzlesPerHead(i))));
	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("%s,Gain   = %f",HeadName, gain));
	m_hswScaleBasedData->AddGainV2(gain);
	
	m_hswScaleBasedData->AddMeasurement(measurement_v2);
	if(voltage_iteration_1 == m_ParamMgr->HSW_MaxVoltageArray[0] )
		 m_hswScaleBasedData->AddMeasurementForMaxVoltage(measurement_v1);
	else
	if(voltage_iteration_1 < m_ParamMgr->HSW_MaxVoltageArray[0])
		{
			measurement = gain_m*(m_ParamMgr->HSW_MaxVoltageArray[0] -voltage_iteration_1) + measurement_v1;
			m_hswScaleBasedData->AddMeasurementForMaxVoltage(measurement);
		}
	 else
	 {
		 // ask Dana , is it possible V1> m_ParamMgr->HSW_MaxVoltageArray[0]
		 m_hswScaleBasedData->AddMeasurementForMaxVoltage(measurement_v1);
     }
	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("%s,Weight_maxV   = %f",HeadName, m_hswScaleBasedData->GetMeasurementForMaxVoltage(i)));


	//calculate maxLayerHead formula 2.1 , gross value
	float factor;
	if (m_ParamMgr->IsModelHead(i))
	{
	   factor = m_ParamMgr->MaterialsWeightFactorPerModeArrayModel[operationMode];
	}
	else
		factor = m_ParamMgr->MaterialsWeightFactorPerModeArraySupport[operationMode];
	int     HeadsChamber = GetHeadsChamberForHSW( i , operationMode,true);

	if(HeadsChamber==TYPE_CHAMBER_MODEL7)  // DM6
  		  HeadsChamber = TYPE_CHAMBER_SUPPORT;

	double materialFactor = m_ParamMgr->MaterialsWeightFactorArray[HeadsChamber]* factor;
  //	float materialFactor = GET_MATERIAL_FACTOR(i,operationMode);
	float headFactor = 1.0;

		if(m_hswScaleBasedData->GetCurrentQualityMode() == HS_INDEX  &&
		m_hswScaleBasedData->GetCurrentOperationMode() == DIGITAL_MATERIAL_OPERATION_MODE)
		{
			//current mode is DM
		   headFactor = m_ParamMgr->HSW_HeadFactorArray_DM[i];
		}
	 else
		if(m_hswScaleBasedData->GetCurrentQualityMode() == HS_INDEX  &&
		m_hswScaleBasedData->GetCurrentOperationMode() == SINGLE_MATERIAL_OPERATION_MODE)
		{
			//mode is HS
		  headFactor = m_ParamMgr->HSW_HeadFactorArray_HS[i];
		}
		else
			// mode is HQ
			headFactor = m_ParamMgr->HSW_HeadFactorArray_HQ[i];

	double a = m_hswScaleBasedData->GetMeasurementForMaxVoltage(i) * m_ParamMgr->PrintResolution_InXAxis * m_ParamMgr->DPI_InYAxis * (pow((double)10, (double)4) * materialFactor*factor);
	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("%s,Material Factor  = %f",HeadName, materialFactor));
	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("%s,Heads Factor  = %f",HeadName, headFactor));
	double b = (NOZZLES_PER_HEAD-m_hswScaleBasedData->GetMissingNozzlesPerHead(i))*
						  (m_ParamMgr->HSW_Num_of_fires*FLOAT_FROM_RESOURCE(IDN_SPECIFIC_GRAVITY)*pow((double)2.54,(double)2)*headFactor);
	double maxLayerHead = (a/b);
	maxLayerHead = maxLayerHead - m_ParamMgr->PrintResolution_InXAxis * 2.8/(pow((double)10, (double)4));
	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("%s, Maximal layer head  = %f", HeadName, maxLayerHead));

	maxLayerHeadV[i] = maxLayerHead;

	float minLayerHeightGross = (m_ParamMgr->HSW_MinLayerHeightArray[qualityMode]+ ProductLT_m) * ProductLT_n;
	 WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("minLayerHeightGross  = %f", minLayerHeightGross));

	 if(maxLayerHead < minLayerHeightGross)
	 {
		  m_HeadsToBeReplaced[i/2] = true;
	 }
	}
	else // no need to calibrate this head
	{
	   m_hswScaleBasedData->AddMeasurementForV1(0);
	   m_hswScaleBasedData->AddMeasurementForV2(0);
	   m_hswScaleBasedData->AddGainV1(0);
	   m_hswScaleBasedData->AddGainV2(0);
	   m_hswScaleBasedData->AddMeasurement(0);
	   m_hswScaleBasedData->AddMeasurementForMaxVoltage(0);
	   double maxLayerHead = (m_ParamMgr->HSW_MaxLayerHeightArray[qualityMode]+ ProductLT_m) * ProductLT_n;
	   maxLayerHeadV[i] = maxLayerHead;
	}
  }

  pPage->PointsStatusMessage = "";
 pPage->Refresh();
 if(m_ParamMgr->HSW_SCALE_EMULATION == false)
	InformTheUserOfTheDefectiveHeads();
  if ((m_GoToContinuationPage == true)&&(m_hswScaleBasedData->IsScaleOnTray() == false))
  {
	SetNextPage(qrSelectContinuationOption);
	GotoNextPage();
  }
  else
  if ((m_GoToContinuationPage == true)&&(m_hswScaleBasedData->IsScaleOnTray() == true))
  {
	  m_hswScaleBasedData->SetHeadReplacement(true);
	// move X to maxx ,Y to 0 position
	SetNextPage(qrCloseDoorResinToPurgeUnit);
	GotoNextPage();
  }
  else
  {
	  //Calculate Target Layer
	// formula 2.2
	//minMaxlayer = min(max_layer_headgross(i)) , i= 1 to n, n=# of heads
	//Target layergross = min(minMaxlayer, max_layermode)
	//find min maxLayer
	float minMaxLayerHead =  maxLayerHeadV[0];
	FOR_ALL_HEADS(i)
	{
	if (m_ParamMgr->HSW_HeadStatus[i] == 1)
		{
		 QString QHeadName = GetHeadName(i);

		 minMaxLayerHead = std::min(minMaxLayerHead,maxLayerHeadV[i]);
		}
		else
			headStatusWasZero = true;
	}
	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Minimum of Maximal layer head  = %f",minMaxLayerHead));
	// calculate target layer
	targetLayerGross = CalculateTargetLayer(qualityMode,minMaxLayerHead);
	targetLayerNet =   targetLayerGross/ProductLT_n - ProductLT_m;
	// if we have some head_status = 0
	if( (targetLayerNet < m_ParamMgr->HSW_MaxLayerHeightArray[qualityMode]) && (headStatusWasZero == true))
		{
			//  put head_status = 1
			FOR_ALL_HEADS(i)
			{
				m_ParamMgr->HSW_HeadStatus[i] = 1;
			}
		  WriteToLogFile(LOG_TAG_GENERAL,"Found head status 0, run ''Find Target Layer'' again.");
		  SetNextPage(qrFindTargetLayer);
		  GotoNextPage();
		  return;
		}
	// calculate target weight, formula 3
	//weight=  (target_layer_gross×(nozzles_head-missing nozzles)×#_of_fires×SG×2.54^2×head_factor)/(ResX×ResY×material_factor×10^4 )
	std::vector<float>targetWeight;
	CalculateTargetWeight(targetWeight,targetLayerGross,operationMode);

	if ( !VoltageAdjustment(qualityMode,operationMode,targetWeight) )
		return;

	if (m_targetWeightNotReached == true)
	{
		return;
	}
	// check if all modes are done , if not enter to the next mode.
	// the order is DM,HS,HQ
	if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();


	// finish wizard.
	// check if material replace is needed :
	//bool needToReplace = false;
	m_hswScaleBasedData->m_lastMRW = true;

	m_ParamMgr->MRW_WithinWizard = 0; /*wizard allready replaced resin, and we are with the same resin like we start the wizard.*/
	m_ParamMgr->HSW_WithinWizard = 0;
	m_ParamMgr->SaveSingleParameter(&m_ParamMgr->MRW_WithinWizard);
	m_ParamMgr->SaveSingleParameter(&m_ParamMgr->HSW_WithinWizard);
	SetNextPage(qrCloseDoorResinToPurgeUnit);
	GotoNextPage();
  }

}
void CHeadServiceWizard::FindTargetLayer(CWizardPage *WizardPage,int operationMode,
										 int qualityMode)
{
 m_GoToContinuationPage = false;

 // HeadHeating
  WriteToLogFile(LOG_TAG_GENERAL,"Find Target Layer");
  // heat heads //
  HeadsHeatingCycle();
  unsigned currTime       = QGetTicks();
  unsigned timeDelta = currTime - m_hswScaleBasedData->GetStartHeatingTime();
 
	
  // Update the curr time....
  if(timeDelta < QSecondsToTicks(m_ParamMgr->HSW_TempStabilizationTimeSec))
	QSleepSec(m_ParamMgr->HSW_TempStabilizationTimeSec - QTicksToSeconds(timeDelta));
 //	purge
  //PurgeCicle();
  m_hswScaleBasedData->ClearMeasurements();
	m_BackEnd->EnableMotor(true, AXIS_Y);
	m_BackEnd->EnableMotor(true, AXIS_X);
	TQErrCode Err = Q_NO_ERROR;
	TRY_FUNCTION(Err, m_BackEnd->MoveMotorToAbsolutePosition(AXIS_Y, m_ParamMgr->HSW_SCALE_Y_POSITION, NO_BLOCKING));
	TRY_FUNCTION(Err, m_BackEnd->MoveMotorToAbsolutePosition(AXIS_X, m_ParamMgr->HSW_SCALE_X_POSITION, NO_BLOCKING));

	TRY_FUNCTION(Err, m_BackEnd->WaitForEndOfMovement(AXIS_Y));
	TRY_FUNCTION(Err, m_BackEnd->WaitForEndOfMovement(AXIS_X));

		m_BackEnd->EnableMotor(false, AXIS_Y);
		m_BackEnd->EnableMotor(false, AXIS_X);
  CSuspensionPointsStatusPage* pPage= dynamic_cast< CSuspensionPointsStatusPage *>(Pages[qrFindTargetLayer]);
  pPage->PointsStatusMessage = "";
  pPage->Refresh();

  //minmaxLHNet sequence
  GetMinMaxLHnetSeqeunce(WizardPage, operationMode, qualityMode);

 }


/*----------------------------------------------------------------------------*/

void CHeadServiceWizard::HeatingForHQMode()
{
  WriteToLogFile(LOG_TAG_GENERAL,"Heating For HQ Mode");
  /*heat heads*/
  m_hswScaleBasedData->SetStartHeatingTime(QGetTicks());
  HeadsHeatingCycle();
  unsigned currTime       = QGetTicks();
  unsigned timeDelta = currTime - m_hswScaleBasedData->GetStartHeatingTime();
  // Update the curr time....
  if(timeDelta < QSecondsToTicks(m_ParamMgr->HSW_TempStabilizationTimeSec_HQ))
	QSleepSec(m_ParamMgr->HSW_TempStabilizationTimeSec - QTicksToSeconds(timeDelta));
}
/*----------------------------------------------------------------------------*/
void CHeadServiceWizard::HeatingForHSMode()
{
  WriteToLogFile(LOG_TAG_GENERAL,"Heating For HS Mode");
  /*heat heads*/
  m_hswScaleBasedData->SetStartHeatingTime(QGetTicks());
  HeadsHeatingCycle();
  unsigned currTime       = QGetTicks();
  unsigned timeDelta = currTime - m_hswScaleBasedData->GetStartHeatingTime();
  // Update the curr time....
  if(timeDelta < QSecondsToTicks(m_ParamMgr->HSW_TempStabilizationTimeSec_HQ))
	QSleepSec(m_ParamMgr->HSW_TempStabilizationTimeSec - QTicksToSeconds(timeDelta));
}


/*----------------------------------------------------------------------------*/
bool CHeadServiceWizard::VoltageAdjustment(int qualityMode,int operationMode,std::vector<float> targetWeight)
{
  float currentVoltage ;
  float gain = 0;
  float *measurementForIteration = new float[m_ParamMgr->HSW_Max_count]; /*save measurements for each iteration*/
  float *voltageForIteration = new float[m_ParamMgr->HSW_Max_count]; /*save voltage for each iteration*/
  float *deltaWeightForIteration = new float[m_ParamMgr->HSW_Max_count]; /*save delta measurements for each iteration*/
  CSuspensionPointsStatusPage* pPage= dynamic_cast< CSuspensionPointsStatusPage *>(Pages[qrFindTargetLayer]);
  pPage->PointsStatusMessage = "";
  pPage->Refresh();
  m_hswCalibrationData->ClearData();
  try
  {

  FOR_ALL_HEADS(h)
	{
	if(m_ParamMgr->HSW_HeadStatus[h] ==1)
	{
	// this gain only for the first iteration
	 if(m_ParamMgr->IsModelHead(h))
	 {
		gain = m_ParamMgr->HSW_Model_Gain;
		currentVoltage = m_ParamMgr->HSW_VOLTAGE_ITERATION_M_2[qualityMode];
	 }
	  else
	  {
		gain = m_ParamMgr->HSW_Support_Gain;
		currentVoltage = m_ParamMgr->HSW_VOLTAGE_ITERATION_S_2[qualityMode];
	  }
	for(int i = 0;i<m_ParamMgr->HSW_Max_count;i++)
	{
	  YieldWizardThread();
	  WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Voltage adjustment for head %s iteration %d",
												GetHeadName(h).c_str(),i));
	  QString title = "Voltage adjustment for " + GetHeadName(h) + " iter " + QIntToStr(i+1) ;
	  if (IsCancelled())
		throw CQ2RTAutoWizardCancelledException();
	  pPage->PointsStatusMessage = title.c_str();
	  pPage->Refresh();
	  QString QHeadName = GetHeadName(h);
	  //const char* HeadName = QHeadName.c_str();
	  float currentWeight = m_hswScaleBasedData->GetMeasurement(h);

	  // calculate next voltage
	  //Next voltage = ((target_weight-current_weight)×10^9)/
	  //(#_of_fires×(nozzles_head-missing nozzles)×DW_gain)+current_voltage
	  float nextVoltage = 0;
	  deltaWeightForIteration[i] = targetWeight[h] - currentWeight;
	  WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("%s, Delta Weight   = %f",GetHeadName(h).c_str(),deltaWeightForIteration[i]));
	  if(m_hswScaleBasedData->GetGainV2(h) != 0)
		nextVoltage = ((deltaWeightForIteration[i]*pow((double)10,(double)9))/
						   (m_ParamMgr->HSW_Num_of_fires*(NOZZLES_PER_HEAD - m_hswScaleBasedData->GetMissingNozzlesPerHead(h))*
							m_hswScaleBasedData->GetGainV2(h)))+ currentVoltage;
	  else
		nextVoltage =  currentVoltage;

	  if( nextVoltage < m_ParamMgr->HSW_MinVoltageArray[0])
		 nextVoltage = m_ParamMgr->HSW_MinVoltageArray[0]+0.01;

	  WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("%s, Next Voltage   = %f",GetHeadName(h).c_str(),nextVoltage));
      WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Gain for head %s %f",
												GetHeadName(h).c_str(),m_hswScaleBasedData->GetGainV2(h)));
	  if((nextVoltage>m_ParamMgr->HSW_MaxVoltageArray[0]) ||(nextVoltage<m_ParamMgr->HSW_MinVoltageArray[0]))
		// go to new window "Cancel"
	   {
			if(nextVoltage < m_ParamMgr->HSW_MinVoltageArray[0])
			{
			  if((voltageForIteration[i-1]< m_ParamMgr->HSW_MinVoltageArray[0]) && (i>0))
			  {
				nextVoltage = m_ParamMgr->HSW_MinVoltageArray[0];
				m_BackEnd->SetAndMeasureHeadVoltage(h,nextVoltage);
				voltageForIteration[i] = nextVoltage;
				i = m_ParamMgr->HSW_Max_count;
				WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head Optimization Wizard: voltage Head %s requires voltage lower than minimum",GetHeadName(h).c_str()));
				WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("%s, Head Optimization Wizard: voltage = %f",GetHeadName(h).c_str(),nextVoltage));
            	break;
			  }
			  else
			  {
				nextVoltage = m_ParamMgr->HSW_MinVoltageArray[0];
			  }
			}
			else
			{
			if((voltageForIteration[i-1]== m_ParamMgr->HSW_MaxVoltageArray[0]) && (i>0))
			{
			  CMessageWizardPage *Page = dynamic_cast<CMessageWizardPage *>(GetPageById(qrVoltageProblem));
			   QString msg = "The wizard cannot optimize the voltage for ";
			   msg+= GetDoubleHeadName(h/2).c_str();
			   msg+= ".\nYou should replace this print head.\nRun the wizard from the begining.\nSelect 'Replace faulty heads...' to replace the head,\nand then continue the HOW to optimize the print heads.";
			   Page->SubTitle = msg;
			   i=m_ParamMgr->HSW_Max_count;
			  m_targetWeightNotReached = true;
			  SetNextPage(qrVoltageProblem);
			  GotoNextPage();
			  return false;
			}
			else
			{
			  nextVoltage = m_ParamMgr->HSW_MaxVoltageArray[0];
            }
		}
	   }
	  // calibrateVoltage
	  m_BackEnd->SetAndMeasureHeadVoltage(h,nextVoltage);
	  voltageForIteration[i] = nextVoltage;
	  WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head %s calibrated for %f voltage",
												GetHeadName(h).c_str(),
												nextVoltage));
	  // weight test
	  float measurement = 0.0;
	  int error = 0;
	  if(WeightTest(h,measurement,qualityMode,error) == false)
	  {
		if(error == SCALE_COMMUNICATION_FAIL)
			{
			SetNextPage(qrScaleCommunicationError);
			GotoNextPage();
			return false;
			}
		 else
		 if(error == SCALE_PROBLEM)
		{
		  SetNextPage(qrScaleProblem);
		  GotoNextPage();
		  return false;
		}
		else
		if(error == WEIGHT_TEST_FAIL)
		{
			CMessageWizardPage *Page = dynamic_cast<CMessageWizardPage *>(GetPageById(qrVoltageProblem));
		   QString msg = "The wizard cannot optimize the voltage for ";
		   msg+= GetDoubleHeadName(h/2).c_str();
		   msg+= ".\nYou should replace this print head.\nRun the wizard from the begining.\nSelect 'Replace faulty heads...' to replace the head,\nand then continue the HOW to optimize the print heads.";
		   Page->SubTitle = msg;
		   i=m_ParamMgr->HSW_Max_count;
		   m_targetWeightNotReached = true;
		   SetNextPage(qrVoltageProblem);
		   GotoNextPage();
		   return false;
		}
		}
	  else if( measurement == 0 && m_ParamMgr->HSW_SCALE_EMULATION == false)
	  {
		SetNextPage(qrScaleCommunicationError);
		 GotoNextPage();
		 return false;
	  }
	  measurementForIteration[i] = measurement;
	  m_hswScaleBasedData->ChangeMeasurement(measurement,h);
	  float scalesResolution = m_ParamMgr->HSW_Scales_res;
	  if(fabs(measurement-targetWeight[h])<= scalesResolution)
	  {
		//save Voltage
	   if(m_ParamMgr->IsModelHead(h))
			m_ParamMgr->RequestedHeadVoltagesModel[h] = nextVoltage;
		else
			m_ParamMgr->RequestedHeadVoltagesSupport[h] = nextVoltage;

		m_ParamMgr->PotentiometerValues[h]   = m_BackEnd->GetCurrHeadPotentiometerValue(h);
		WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Save voltage %f for head %s",
												nextVoltage,
												GetHeadName(h).c_str()));
		m_ParamMgr->SaveSingleParameter(&m_ParamMgr->RequestedHeadVoltagesModel);
		m_ParamMgr->SaveSingleParameter(&m_ParamMgr->RequestedHeadVoltagesSupport);
		m_ParamMgr->SaveSingleParameter(&m_ParamMgr->PotentiometerValues);


		i = m_ParamMgr->HSW_Max_count; // leave this loop,go to next head
	  }
	  else
	  { // >scalesResolution, save average voltage
		currentVoltage = nextVoltage;
		if(i >= m_ParamMgr->HSW_Max_count-1)
			{
				currentVoltage = (voltageForIteration[m_ParamMgr->HSW_Max_count-2]+voltageForIteration[m_ParamMgr->HSW_Max_count-1])/2;
                //save Voltage
	   if(m_ParamMgr->IsModelHead(h))
			m_ParamMgr->RequestedHeadVoltagesModel[h] = currentVoltage;
		else
			m_ParamMgr->RequestedHeadVoltagesSupport[h] = currentVoltage;

			m_ParamMgr->PotentiometerValues[h]   = m_BackEnd->GetCurrHeadPotentiometerValue(h);
			WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head Optimization Wizard: final voltage taken as average of last 2 voltages"));
			WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Save voltage %f for head %s",
												currentVoltage,
												GetHeadName(h).c_str()));
			m_ParamMgr->SaveSingleParameter(&m_ParamMgr->RequestedHeadVoltagesModel);
			m_ParamMgr->SaveSingleParameter(&m_ParamMgr->RequestedHeadVoltagesSupport);
			m_ParamMgr->SaveSingleParameter(&m_ParamMgr->PotentiometerValues);
			}
			else  //i<HSW_Max_count
			{
				if(i>0)
				{
					if(abs(measurement-targetWeight[h])<abs(deltaWeightForIteration[i]*1.2))
					{
						WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head Optimization Wizard: delta weight < delta prev. weight"));
					}
					else
					{
						 WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head Optimization Wizard: delta weight >= delta prev. weight"));
						 if((deltaWeightForIteration[i] >0 && (targetWeight[h]- measurement) >0) ||
						 ((deltaWeightForIteration[i] <0 && (targetWeight[h]- measurement) <0)))
							{
								WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head Optimization Wizard: no progress, repeating measurement."));
								//purge
                                TQErrCode Err = Q_NO_ERROR;
								m_BackEnd->GotoPurgePosition(/* blocking */ true, /* perform home */ true);
								m_BackEnd->Purge(true, true);
								Q2RTWizardSleep(m_ParamMgr->MotorPostPurgeTimeSec);
								m_BackEnd->WipeHeads(false);
								YieldWizardThread();
								m_BackEnd->WipeHeads(false);
								m_BackEnd->EnableMotor(true, AXIS_Y);
								m_BackEnd->EnableMotor(true, AXIS_X);
								TRY_FUNCTION(Err, m_BackEnd->MoveMotorToAbsolutePosition(AXIS_Y, m_ParamMgr->HSW_SCALE_Y_POSITION, NO_BLOCKING));
								TRY_FUNCTION(Err, m_BackEnd->MoveMotorToAbsolutePosition(AXIS_X, m_ParamMgr->HSW_SCALE_X_POSITION, NO_BLOCKING));

								TRY_FUNCTION(Err, m_BackEnd->WaitForEndOfMovement(AXIS_Y));
								TRY_FUNCTION(Err, m_BackEnd->WaitForEndOfMovement(AXIS_X));

								m_BackEnd->EnableMotor(false, AXIS_Y);
								m_BackEnd->EnableMotor(false, AXIS_X);
								// Repeat measurement with the same voltage
								  if(WeightTest(h,measurement,qualityMode,error) == false)
									{
										if(error == SCALE_COMMUNICATION_FAIL)
												{
													SetNextPage(qrScaleCommunicationError);
													GotoNextPage();
													return false;
												}
										 else
											 if(error == SCALE_PROBLEM)
												{
												  SetNextPage(qrScaleProblem);
												  GotoNextPage();
												  return false;
												}
										else
											if(error == WEIGHT_TEST_FAIL)
												{
												     CMessageWizardPage *Page = dynamic_cast<CMessageWizardPage *>(GetPageById(qrVoltageProblem));
													 QString msg = "The wizard cannot optimize the voltage for ";
													 msg+= GetDoubleHeadName(h/2).c_str();
													 msg+= ".\nYou should replace this print head.\nRun the wizard from the begining.\nSelect 'Replace faulty heads...' to replace the head,\nand then continue the HOW to optimize the print heads.";
													 Page->SubTitle = msg;
													 i=m_ParamMgr->HSW_Max_count;
													 m_targetWeightNotReached = true;
													 SetNextPage(qrVoltageProblem);
													 GotoNextPage();
													 return false;
												}
										}
								  else if( measurement == 0 && m_ParamMgr->HSW_SCALE_EMULATION == false)
									  {
										SetNextPage(qrScaleCommunicationError);
										 GotoNextPage();
										 return false;
									  }
								  measurementForIteration[i] = measurement;
								  m_hswScaleBasedData->ChangeMeasurement(measurement,h);

								//save Voltage
								if(fabs(measurement-targetWeight[h])<= scalesResolution)
								 {
									//save Voltage
								   if(m_ParamMgr->IsModelHead(h))
										m_ParamMgr->RequestedHeadVoltagesModel[h] = nextVoltage;
								   else
									m_ParamMgr->RequestedHeadVoltagesSupport[h] = nextVoltage;

									m_ParamMgr->PotentiometerValues[h]   = m_BackEnd->GetCurrHeadPotentiometerValue(h);
									WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Save voltage %f for head %s",
												nextVoltage,
												GetHeadName(h).c_str()));
												m_ParamMgr->SaveSingleParameter(&m_ParamMgr->RequestedHeadVoltagesModel);
												m_ParamMgr->SaveSingleParameter(&m_ParamMgr->RequestedHeadVoltagesSupport);
												m_ParamMgr->SaveSingleParameter(&m_ParamMgr->PotentiometerValues);


												i = m_ParamMgr->HSW_Max_count; // leave this loop,go to next head
								  }
								 } //same sign
								 else
								 {
									// new gain
									m_hswScaleBasedData->ChangeGain(m_hswScaleBasedData->GetGainV2(h)*1.05,h);
									WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head Optimization Wizard: Increasing gain by 5%"));
								 }

							} //deltaWeightForIteration[i]<abs(deltaWeightForIteration[i-1]*1.2

					} //i>0
				else// (i=0)
				{
					// ????DANA
				}
			}
		} // >scalesResolution, save average voltage
	} // iterations
		m_hswCalibrationData->SetHeadsVoltagesModel(m_ParamMgr->RequestedHeadVoltagesModel[h]);
		m_hswCalibrationData->SetHeadsVoltagesSupport(m_ParamMgr->RequestedHeadVoltagesSupport[h]);
		m_hswCalibrationData->SetHeadPotentiometerValues(m_ParamMgr->PotentiometerValues[h]);

	} // head_status = 1
	else
    	{
		   m_hswCalibrationData->SetHeadsVoltagesModel(m_ParamMgr->RequestedHeadVoltagesModel[h]);
		   m_hswCalibrationData->SetHeadsVoltagesSupport(m_ParamMgr->RequestedHeadVoltagesSupport[h]);
		   m_hswCalibrationData->SetHeadPotentiometerValues(m_ParamMgr->PotentiometerValues[h]);
        }
  } // heads
  pPage->PointsStatusMessage = "";
  }
	__finally
	{
	  delete []measurementForIteration;
	  delete []voltageForIteration;
	  delete []deltaWeightForIteration;
	}
	return true;
}

/*----------------------------------------------------------------------------*/
void CHeadServiceWizard::CalculateTargetWeight(std::vector<float>&targetWeight,float targetLayerGross,int operationMode)
{
  targetWeight.clear();
  float targetWeightForHead = 0;
  FOR_ALL_HEADS(i)
	{
		if(m_ParamMgr->HSW_HeadStatus[i] == 1 )
		{
	   QString QHeadName = GetHeadName(i);
	   const char* HeadName = QHeadName.c_str();
		float factor;
		if (m_ParamMgr->IsModelHead(i))
		{
		 factor = m_ParamMgr->MaterialsWeightFactorPerModeArrayModel[operationMode];
		}
		else
			factor = m_ParamMgr->MaterialsWeightFactorPerModeArraySupport[operationMode];
			int     HeadsChamber = GetHeadsChamberForHSW( i , operationMode,true);
		if(HeadsChamber==TYPE_CHAMBER_MODEL7)  // DM6
		  HeadsChamber = TYPE_CHAMBER_SUPPORT;
	   double materialFactor = m_ParamMgr->MaterialsWeightFactorArray[HeadsChamber]* factor;
		float headFactor = 1.0;

		if(m_hswScaleBasedData->GetCurrentQualityMode() == HS_INDEX  &&
		m_hswScaleBasedData->GetCurrentOperationMode() == DIGITAL_MATERIAL_OPERATION_MODE)
		{
			//current mode is DM
		   headFactor = m_ParamMgr->HSW_HeadFactorArray_DM[i];
		}
	 else
		if(m_hswScaleBasedData->GetCurrentQualityMode() == HS_INDEX  &&
		m_hswScaleBasedData->GetCurrentOperationMode() == SINGLE_MATERIAL_OPERATION_MODE)
		{
			//mode is HS
		  headFactor = m_ParamMgr->HSW_HeadFactorArray_HS[i];
		}
		else
			// mode is HQ
			headFactor = m_ParamMgr->HSW_HeadFactorArray_HQ[i];
	   WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("%s,Heads Factor  = %f",HeadName, headFactor));

	   targetWeightForHead = ((targetLayerGross*(NOZZLES_PER_HEAD - m_hswScaleBasedData->GetMissingNozzlesPerHead(i))*
									 m_ParamMgr->HSW_Num_of_fires*FLOAT_FROM_RESOURCE(IDN_SPECIFIC_GRAVITY)*pow((double)2.54,(double)2)*headFactor)/
									 (m_ParamMgr->PrintResolution_InXAxis * m_ParamMgr->DPI_InYAxis*materialFactor *
									 pow((double)10,(double)4)));
	   WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("%s, Target weight   = %f",HeadName,targetWeightForHead));
	   targetWeight.push_back(targetWeightForHead);
	}
	else
	{
       targetWeight.push_back(0);
    }
	}
}
/*----------------------------------------------------------------------------*/
float CHeadServiceWizard::CalculateTargetLayer(int qualityMode,float minMaxLayerHead)
{
	float ProductLT_m    = m_ParamMgr->ProductLT_m;
	float ProductLT_n    = m_ParamMgr->ProductLT_n;
	float maxLayerModeGross =  (m_ParamMgr->HSW_MaxLayerHeightArray[qualityMode]+ ProductLT_m) * ProductLT_n;
	float targetLayerGross = std::min(minMaxLayerHead,maxLayerModeGross);
	float targetLayerNet =   targetLayerGross/ProductLT_n - ProductLT_m;
	//Round down to nearest half
	float targetLayerNetrounded =   (floor((targetLayerNet * 2.0))/2.0);
	
	if( targetLayerNetrounded != targetLayerNet) //need to update targetLayerGross
	{
		targetLayerGross = (targetLayerNetrounded + ProductLT_m) * ProductLT_n;
	}


	// save to param Mngr
	m_ParamMgr->LayerHeightDPI_um = targetLayerNetrounded;
	m_ParamMgr->SaveSingleParameter(&m_ParamMgr->LayerHeightDPI_um);
	m_hswCalibrationData->SetLayerHeightDPI_um(m_ParamMgr->LayerHeightDPI_um);
	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Target layer Net   = %f",targetLayerNetrounded));
	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Target layer Net (before rounding)  = %f",targetLayerNet));
	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Target layer Gross  = %f",targetLayerGross));

	return targetLayerGross;
}
/*----------------------------------------------------------------------------*/
void CHeadServiceWizard::SaveCurrentData()
{
  // before starting HSW we need to save :
  // 1 - material in each head
  m_hswScaleBasedData->ClearActiveThermistors();
  FOR_ALL_HEADS(i)
  {
//	bool  retValue = true;
	CBackEndInterface* BackEnd = CBackEndInterface::Instance();
	int chamberIndex = GetHeadsChamberForHSW(i,true);
	
	TTankIndex ActiveTankID = BackEnd->GetActiveTankNum((TChamberIndex)chamberIndex);
	if(m_ParamMgr->HSW_DM3_Enable != true && ((i != 2) && (i!= 3)))
	 if (NO_TANK == ActiveTankID)
		throw EQException("Error while trying to get active tank number " );
	int length = (m_ParamMgr->ActiveThermistors).Size();
	m_hswScaleBasedData->ClearActiveThermistors();

	for(int i = 0;i< length;i++)
		m_hswScaleBasedData->AddActiveThermistor(m_ParamMgr->ActiveThermistors[i]);

	 m_hswScaleBasedData->SetM3ModelName(m_ParamMgr->MRW_TypesArrayPerSegment[LAST_MODEL_SEGMENT]);
  }
}

/*
	A barrier that allows each Tank a certain amount of time to finish
	its stabilization process.
	If a tank's identification process encounters a problem -
	BAILS-OUT from the barrier, to handle wizard freeze in the meantime.

	Returns whether or not the identification process is stuck
*/
//	This is a portioned CLONE from CResinReplacementWizard::UpdateAllTanks() 
bool CHeadServiceWizard::AllTanksIdentificationSyncPoint()
{
	bool isWndFrozen = false;
	unsigned long timeout = QSecondsToTicks(m_ParamMgr->TanksIdentificationTimeoutSec.Value());

	for (int Tank = FIRST_TANK_TYPE; Tank < LAST_TANK_TYPE; ++Tank)
	{
		//set the maximum time we're willing to wait till identification is complete
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
				"Error: Head Optimization Wizard's window seems to be frozen while identifying cartridge %s",
				TankToStr(static_cast<TTankIndex>(Tank)).c_str()
			));
			break; //break out of for loop and skip containers' identification
		}
	}

	return isWndFrozen;
}

 /*******************************************************************************************************************/
#pragma warn -8004 // Enable warning Number 8004 "Assigned a value that is never used"
/*******************************************************************************************************************/
void CHeadServiceWizard::ReplaceMateria(CWizardPage *WizardPage)
{
	WriteToLogFile(LOG_TAG_GENERAL,"Starting material replacement");
    m_BackEnd->SetHoodFanParamOnOff(true,m_ParamMgr->HSW_OdourFanMRWVoltage);
    // In case of exceptions, closes air valve when gets out of scope
   //	ResinFlowUtils::AirValve av(this);

	// Block heating
	//-----------------------------------------
	unsigned CurrentTime       = 0;
	unsigned StabilizationTime = QSecondsToTicks(m_ParamMgr->HSW_TempStabilizationTimeSec);
	unsigned HeatingStartTime  = QGetTicks();

	HeadsHeatingCycle();

	// Temperature stabilization
	do
	{
		CurrentTime = QGetTicks();
		
		YieldWizardThread();
		if(IsCancelled())
			throw CQ2RTAutoWizardCancelledException();
	}
	while(CurrentTime - HeatingStartTime < StabilizationTime);
	//-----------------------------------------

	// We assuming here that the machine cover is already closed
	m_BackEnd->GotoPurgePosition(/* blocking */ true, /* perform home */ true);

	// Disable temperature error reporting, because we're going to drain the block
	m_BackEnd->SetReportTemperatureError(false);
	m_BackEnd->TurnHeadFilling(false);
	// Drain the block
	HeadsDrainingCycle(WizardPage,m_ParamMgr->HSW_EmptyBlockTime,false,false);

	// Change the filling thermistors operation mode according to a required material in chamber #3
	//-----------------------------------------
	int TherOpMode = m_ParamMgr->ThermistorsOperationMode;
	WriteToLogFile(LOG_TAG_GENERAL,"Changing thermistorOperationMode");
	TTankIndex ActiveTankID 		= NO_TANK;
	switch(TherOpMode)
	{
        case HIGH_AND_LOW_THERMISTORS_MODE:
		case HIGH_THERMISTORS_MODE: // Currently chamber #3 fillied with Support
			WriteToLogFile(LOG_TAG_GENERAL,"Current thermistorsMode is : HIGH(_AND_LOW)_THERMISTORS_MODE");
			EnterMode(HS_INDEX,DIGITAL_MATERIAL_OPERATION_MODE);
			m_ParamMgr->ThermistorsOperationMode = SUPPORT_LOW_MODEL_HIGH_THERMISTORS_MODE;
			ActiveTankID = m_BackEnd->GetActiveTankNum(TYPE_CHAMBER_MODEL1);
#ifdef OBJET_MACHINE_KESHET 
			//8 resin adjustments here
#else
			if(ActiveTankID == 2 || ActiveTankID == 3)
				m_ParamMgr->TanksOperationMode = S_M1_M3_ACTIVE_TANKS_MODE;
			else
				m_ParamMgr->TanksOperationMode = S_M2_M3_ACTIVE_TANKS_MODE;
#endif
			WriteToLogFile(LOG_TAG_GENERAL,"Going to : SUPPORT_LOW_MODEL_HIGH_THERMISTORS_MODE");
			break;

		case LOW_THERMISTORS_MODE: // Currently chamber #3 fillied with Model
			WriteToLogFile(LOG_TAG_GENERAL,"Current thermistorsMode is : LOW_THERMISTORS_MODE");
			EnterMode(HS_INDEX,SINGLE_MATERIAL_OPERATION_MODE);
			m_ParamMgr->ThermistorsOperationMode = SUPPORT_HIGH_MODEL_LOW_THERMISTORS_MODE;
#ifdef OBJET_MACHINE_KESHET
			//8 resin adjustments here
#else
			m_ParamMgr->TanksOperationMode = S_M1_M2_ACTIVE_TANKS_MODE;
#endif
			WriteToLogFile(LOG_TAG_GENERAL,"Going to : SUPPORT_HIGH_MODEL_LOW_THERMISTORS_MODE");
			break;

		case SUPPORT_HIGH_MODEL_LOW_THERMISTORS_MODE: // Currently chamber #3 fillied with Support
			WriteToLogFile(LOG_TAG_GENERAL,"Current thermistorsMode is : SUPPORT_HIGH_MODEL_LOW_THERMISTORS_MODE");
			EnterMode(HS_INDEX,DIGITAL_MATERIAL_OPERATION_MODE);
			m_ParamMgr->ThermistorsOperationMode = LOW_THERMISTORS_MODE;
#ifdef OBJET_MACHINE_KESHET
			//8 resin adjustments here
#else
			m_ParamMgr->TanksOperationMode = S_M1_M2_M3_ACTIVE_TANKS_MODE;
#endif
			WriteToLogFile(LOG_TAG_GENERAL,"Going to : LOW_THERMISTORS_MODE");
			break;

		case SUPPORT_LOW_MODEL_HIGH_THERMISTORS_MODE: // Currently chamber #3 fillied with Model
			WriteToLogFile(LOG_TAG_GENERAL,"Current thermistorsMode is : SUPPORT_LOW_MODEL_HIGH_THERMISTORS_MODE");
			EnterMode(HS_INDEX,SINGLE_MATERIAL_OPERATION_MODE);
			m_ParamMgr->ThermistorsOperationMode = GetThermOpModeForHQHS();
			ActiveTankID = m_BackEnd->GetActiveTankNum(TYPE_CHAMBER_MODEL1);
#ifdef OBJET_MACHINE_KESHET
			//8 resin adjustments here
#else
			if(ActiveTankID == 2 || ActiveTankID == 3)
				m_ParamMgr->TanksOperationMode = S_M1_ACTIVE_TANKS_MODE;
			else
				m_ParamMgr->TanksOperationMode = S_M2_ACTIVE_TANKS_MODE;
#endif
			WriteToLogFile(LOG_TAG_GENERAL,"Going to : HIGH(_AND_LOW)_THERMISTORS_MODE");
			break;

		default:
			throw EQException("Wrong thermistor operation mode");
	}
	TherOpMode = m_ParamMgr->ThermistorsOperationMode;
	//If we're about to switch to high thermistors only - set the op-mode to SINGLE
	TOperationModeIndex sysOpMode = (SUPPORT_LOW_MODEL_HIGH_THERMISTORS_MODE == TherOpMode)
		? SINGLE_MATERIAL_OPERATION_MODE : DIGITAL_MATERIAL_OPERATION_MODE;
	m_ParamMgr->PrintingOperationMode = sysOpMode;
	m_ParamMgr->PipesOperationMode = sysOpMode;
	m_ParamMgr->SaveSingleParameter(&m_ParamMgr->PrintingOperationMode);
	m_ParamMgr->SaveSingleParameter(&m_ParamMgr->PipesOperationMode);
	
	m_ParamMgr->SaveSingleParameter(&m_ParamMgr->ThermistorsOperationMode);
	m_ParamMgr->SaveSingleParameter(&m_ParamMgr->TanksOperationMode);
	m_BackEnd->SetDefaultParamsToOCB();

	 /*from SUPPORT_HIGH_MODEL_LOW_THERMISTORS_MODE to LOW*/
	m_BackEnd->SkipMachineResinTypeChecking(TYPE_TANK_MODEL5, true);
	m_BackEnd->SkipMachineResinTypeChecking(TYPE_TANK_MODEL6, true);
	TTankIDNotice::HideDialog(TYPE_TANK_MODEL6);
    TTankIDNotice::HideDialog(TYPE_TANK_MODEL5);
   
	/*update single tank*/

	/*update array per chamber "UNDEFINED"*/
	m_ParamMgr->TypesArrayPerChamber[TYPE_CHAMBER_MODEL3] 	= "UNDEFINED";
	m_ParamMgr->SaveSingleParameter(&m_ParamMgr->TypesArrayPerChamber);

 	////////////////////////////////////////// End of Undefined //////////////////////////////////////////

   QString materialName = "";
#ifdef OBJET_MACHINE_KESHET
	//8 resin adjustments here
#else
   if(m_ParamMgr->TanksOperationMode == S_M1_M2_M3_ACTIVE_TANKS_MODE ||
	 m_ParamMgr->TanksOperationMode == S_M1_M3_ACTIVE_TANKS_MODE ||
	 m_ParamMgr->TanksOperationMode == S_M2_M3_ACTIVE_TANKS_MODE)
	 {
		materialName = m_hswScaleBasedData->GetM3ModelName(); // TypesArrayPerChamber[TYPE_CHAMBER_MODEL3]
	 }
	 else
	 {
		materialName = m_BackEnd->GetTankMaterialType(TYPE_TANK_SUPPORT1);
	 }
#endif
	/*update array per chamber*/
	m_ParamMgr->TypesArrayPerChamber[TYPE_CHAMBER_MODEL3] 	= materialName;
	m_ParamMgr->SaveSingleParameter(&m_ParamMgr->TypesArrayPerChamber);

  //	m_ParamMgr->LogParameters(Q2RTApplication->AppFilePath.Value() + LOAD_STRING(IDS_CONFIGS_DIRNAME) + "HSW_2.cfg");
	m_BackEnd->UpdateTanks(SUPPORT_MODEL_TANK_L);
	//WaitForIdentificationCompletion(SUPPORT_MODEL_TANK_L);
	//QSleep(5000);

	m_BackEnd->UpdateTanks(SUPPORT_MODEL_TANK_R);

	if (!AllTanksIdentificationSyncPoint())
	{
		//Just-in-case excessive mechanism:




		//Enough time to allow all tanks to COMPLETE the CQSingleContainer::ContainerEnablingSM cycles and become stable/identified
		Q2RTWizardSleep(20);
	}

	m_BackEnd->UpdateChambers(false);

	//Enough time to allow OCBStatus to invoke CContainer::GetLiquidTanksWeight again
	//so that the Heads Filling would become re-enabled (in GetIfAllLiquidTankAreEnabled).
	//GetIfAllLiquidTankAreEnabled disables Heads Filling if the active chambers do not have enabled tanks at that point.
	Q2RTWizardSleep(5);

	m_BackEnd->TurnHeadFilling(true);
	int numOfCycles = m_ParamMgr->HSW_REFILL_CYCLES.Value();
	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Washing cycles : will perform %d cycles " ,numOfCycles));
	//-----------------------------------------
	for(int i = 0; i < m_ParamMgr->HSW_REFILL_CYCLES; i++)
	{
	   	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Washing cycle #%d",i + 1));
		HeadFillingCycle();
		HeadsDrainingCycle(WizardPage,m_ParamMgr->HSW_EmptyBlockTime,false,false);
	}


	// Turning filling ON again here to call SetDefaultParams of HeadsFilling to send new threshold params to OCB
	CBEMonitorActivator MonActivator;

	HeadsFillingCycle(m_ParamMgr->HeadsHeatingTimeoutSec);
	m_BackEnd->TurnHeadFilling(false);
	//BackEnd->WipeHeads(false);
	m_BackEnd->SetReportTemperatureError(true);
	m_BackEnd->SkipMachineResinTypeChecking(SUPPORT_MODEL_TANK_L, false);
  	m_BackEnd->SkipMachineResinTypeChecking(SUPPORT_MODEL_TANK_R, false);
   for(int i = LAST_TANK_TYPE-1; i >= FIRST_TANK_TYPE; i--)
	 TTankIDNotice::HideDialog(static_cast<TTankIndex>(i));

    m_BackEnd->SetReportTemperatureError(false);
   // Goto Purge Position
   TQErrCode Err = Q_NO_ERROR;
   TRY_FUNCTION(Err, m_BackEnd->GotoPurgePosition(true, true));
   m_BackEnd->TurnHeadFilling(true);
	HeadsFillingCycle(m_ParamMgr->FillingTimeout);
   m_BackEnd->TurnHeadFilling(false);
  for(int i = 0; i < m_ParamMgr->HSW_NumOfPurgesAfterMRW; i++)
   {
	m_BackEnd->Purge(true, true);
	Q2RTWizardSleep(m_ParamMgr->MotorPostPurgeTimeSec);
	m_BackEnd->WipeHeads(false);
	YieldWizardThread();
	m_BackEnd->WipeHeads(false);
	QSleepSec(20);
   }
   // wait , maybe no need to it...
	QSleepSec(m_ParamMgr->HSW_Wait_After_MRW);
   // do again purge and wipe
	m_BackEnd->Purge(true, true);
	Q2RTWizardSleep(m_ParamMgr->MotorPostPurgeTimeSec);
	m_BackEnd->WipeHeads(false);
	YieldWizardThread();
	m_BackEnd->WipeHeads(false);

   if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
   //TRY_FUNCTION(Err, m_BackEnd->GotoPurgePosition(true, false));
   YieldWizardThread();

  // Stop ignoring 'Temperature' errors at the filling stage...
   m_BackEnd->SetReportTemperatureError(true);
  // return to HSW default oudor fan voltage
  m_BackEnd->SetHoodFanParamOnOff(true,m_ParamMgr->HSW_OdourFanVoltage);

  WriteToLogFile(LOG_TAG_GENERAL,"Material replacement is finished");

}

void CHeadServiceWizard::HeadFillingCycle()
{
	CBEMonitorActivator MonitorActivator;
	HeadsFillingCycle(m_ParamMgr->HeadsHeatingTimeoutSec);
}

bool CHeadServiceWizard::CheckLiquidWeightAccordingMode()
{
   bool isWeightOK= true;
   CCheckBoxWizardPage *Page = dynamic_cast<CCheckBoxWizardPage *>(Pages[qrWeightProblemPage]);
   Page->Strings.Clear();
   TTankIndex ActiveTankID 		= NO_TANK;
   bool needToCheck = true;

  ActiveTankID = m_BackEnd->GetActiveTankNum((TChamberIndex)TYPE_CHAMBER_MODEL1);
  if(IsChamberFlooded(static_cast<TChamberIndex>(TYPE_CHAMBER_MODEL1)) == true) /*chamber that no need to be checked*/
	 {
		needToCheck = false;
	 }
  if ((ActiveTankID == TYPE_TANK_MODEL1) || (ActiveTankID == TYPE_TANK_MODEL2) && (needToCheck == true))
  {
   if( (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL1_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M1_2_SEGMENT ]) ||  //m1left short = m1 long
			 (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL2_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M1_2_SEGMENT ]))  //m1 right short = m1 long
		 {
		   if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL1_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M1_2_SEGMENT ])&& //m1left short = m1 long
			  (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL2_SEGMENT] != m_ParamMgr->MRW_TypesArrayPerSegment[M1_2_SEGMENT ]))  //m1 right short != m1 long
		   {
			 if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL1_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL1)) ||
				(m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL1) - m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL1)]< HSW_MIN_LIQUID_WEIGHT)) //m3 left != tank resin left
			   {
			   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL1_SEGMENT]) == -1)
					{
					WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %f ",m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL1) - m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL1)]));
					QString msg= "Please insert to M1 Left " ;
					msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL1_SEGMENT]);
					Page->Strings.Add(msg.c_str());
					isWeightOK = false;
					Page->SetChecksMask(0);
					}
			   }
		  }
		else if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL1_SEGMENT] != m_ParamMgr->MRW_TypesArrayPerSegment[M1_2_SEGMENT ])&& //m1left short = m1 long
				   (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL2_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M1_2_SEGMENT ]))  //m1 right short != m1 long
				{
				if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL2_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL2)) ||
					(m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL2)- m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL2)]< HSW_MIN_LIQUID_WEIGHT)) //m1 right != tank resin right
				 {
					if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL2_SEGMENT]) == -1)
						{
						WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %f ",m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL2) - m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL2)]));
						QString msg= "Please insert to M1 Right " ;
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL2_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				 }
				}
		   else // m1 left = m1 long and m1 right = m1 long
		   {
			   if(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL1_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL1) && //m1 left != tank resin left
				 (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL2_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL2))) //m1 right != tank resin right
				{
				  QString msg= "Please insert to M1 Left or M1 Right " ;
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL1_SEGMENT]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL2_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				}
			   else if(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL1_SEGMENT] == m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL1) && //m1 left == tank resin left
					 (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL2_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL2))) //m1 right != tank resin right
				{
				   QString msg= "Please insert to M1 Left" ;
				   int leftTankWeight ,rightTankWeight;
				   leftTankWeight = m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL1)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL1)];
				   leftTankWeight = leftTankWeight > 0 ? leftTankWeight : 0;
				   if((leftTankWeight < HSW_MIN_LIQUID_WEIGHT))
				   {
					msg= "Please insert to M1 Left" ;
					WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %d ",leftTankWeight));
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL1_SEGMENT]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL2_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				   }
				}
				else if(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL1_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL1) && //m1 left != tank resin left
					 (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL2_SEGMENT] == m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL2))) //m1 right == tank resin right
				{
				   QString msg= "Please insert to M1 Right " ;
				   int rightTankWeight;
				   rightTankWeight = m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL2)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL2)];
				   rightTankWeight = rightTankWeight > 0 ? rightTankWeight : 0;
				   if((rightTankWeight < HSW_MIN_LIQUID_WEIGHT))
				   {
					msg= "Please insert to M1 Right " ;
						WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %d ",rightTankWeight));
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL1_SEGMENT]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL2_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
						}
				}
				else  /*two tanks material are OK*/
				{
				   QString msg= "Please insert to M1 Left or M1 Right " ;
				   int leftTankWeight ,rightTankWeight;
				   leftTankWeight = m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL1)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL1)];
				   leftTankWeight = leftTankWeight > 0 ? leftTankWeight : 0;
				   rightTankWeight = m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL2)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL2)];
				   rightTankWeight = rightTankWeight > 0 ? rightTankWeight : 0;
				   if(((leftTankWeight + rightTankWeight)< HSW_MIN_LIQUID_WEIGHT))
				   {
					msg= "Please insert to M1 Left or M1 Right " ;
					WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight (left+right)%d ",rightTankWeight+leftTankWeight));
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL1_SEGMENT]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL2_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				   }
				}

		   }
		 }
		 else   /*there is no short = long*/
		 {
		 }
   }
	 needToCheck = true;
	 ActiveTankID = m_BackEnd->GetActiveTankNum((TChamberIndex)TYPE_CHAMBER_MODEL2);
	  if(IsChamberFlooded(static_cast<TChamberIndex>(TYPE_CHAMBER_MODEL2) )== true) /*chamber that no need to be checked*/
	 {
		needToCheck = false;
		WriteToLogFile(LOG_TAG_GENERAL," no need to check");
	 }
	WriteToLogFile(LOG_TAG_GENERAL,"m2 checking");
	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("ActiveTankID %d ",ActiveTankID));

   if ((ActiveTankID == TYPE_TANK_MODEL3) || (ActiveTankID == TYPE_TANK_MODEL4)&&(	needToCheck == true))
  {
	 WriteToLogFile(LOG_TAG_GENERAL,"m2 active && need to check");
   if( (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL3_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M3_4_SEGMENT ]) ||  //m2left short = m2 long
			 (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL4_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M3_4_SEGMENT ]))  //m right short = m2 long
		 {
			 WriteToLogFile(LOG_TAG_GENERAL,"m2left short = m2 long && m2 right short != m2 long");
		   if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL3_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M3_4_SEGMENT ])&& //m2left short = m2 long
			  (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL4_SEGMENT] != m_ParamMgr->MRW_TypesArrayPerSegment[M3_4_SEGMENT ]))  //m2 right short != m2 long
		   {
			WriteToLogFile(LOG_TAG_GENERAL,"m2left != material or weight < 200");
			 if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL3_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL3)) ||
				(m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL3) - m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL3)]< HSW_MIN_LIQUID_WEIGHT)) //m3 left != tank resin left
			   {
			   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL3_SEGMENT]) == -1)
					{
					WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %f ",m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL3) - m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL3)]));
					QString msg= "Please insert to M2 Left " ;
					msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL3_SEGMENT]);
					Page->Strings.Add(msg.c_str());
					isWeightOK = false;
					Page->SetChecksMask(0);
					}
			   }
		  }
		else if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL3_SEGMENT] != m_ParamMgr->MRW_TypesArrayPerSegment[M3_4_SEGMENT ])&& //m2left short = m2 long
				   (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL4_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M3_4_SEGMENT ]))  //m2 right short != m2 long
				{
				  WriteToLogFile(LOG_TAG_GENERAL,"m2left short != m2 long && m2 right short = m2 long");
				if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL4_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL4)) ||
					(m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL4)- m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL4)]< HSW_MIN_LIQUID_WEIGHT)) //m1 right != tank resin right
				 {
                 		WriteToLogFile(LOG_TAG_GENERAL,"m2 right != material or weight < 200");
					if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL4_SEGMENT]) == -1)
						{
						WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %f ",m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL4) - m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL4)]));
						QString msg= "Please insert to M2 Right " ;
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL4_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				 }
				}
		   else // m2 left = m2 long and m2 right = m2 long
		   {
			   if(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL3_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL3) && //m2 left != tank resin left
				 (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL4_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL4))) //m2 right != tank resin right
				{
				 WriteToLogFile(LOG_TAG_GENERAL,"m2 left and m2 right != resin material");
				  QString msg= "Please insert to M2 Left or M2 Right " ;
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL3_SEGMENT]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL4_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				}
				   else if(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL3_SEGMENT] == m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL3) && //m2 left == tank resin left
					 (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL4_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL4))) //m2 right != tank resin right
				{
				   QString msg= "Please insert to M2 Left" ;
				   int leftTankWeight ,rightTankWeight;
				   leftTankWeight = m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL3)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL3)];
				   leftTankWeight = leftTankWeight > 0 ? leftTankWeight : 0;
				   if((leftTankWeight < HSW_MIN_LIQUID_WEIGHT))
				   {
					msg= "Please insert to M2 Left" ;
					WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %d ",leftTankWeight));
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL3_SEGMENT]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL4_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				   }
				}
				else if(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL3_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL3) && //m2 left != tank resin left
					 (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL4_SEGMENT] == m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL4))) //m2 right == tank resin right
				{
				   QString msg= "Please insert to M2 Right " ;
				   int rightTankWeight;
				   rightTankWeight = m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL4)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL4)];
				   rightTankWeight = rightTankWeight > 0 ? rightTankWeight : 0;
				   if((rightTankWeight < HSW_MIN_LIQUID_WEIGHT))
				   {
					msg= "Please insert to M2 Right " ;
						WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %d ",rightTankWeight));
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL3_SEGMENT]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL4_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
						}
				}
				else  /*two tanks material are OK*/
				{
				   QString msg= "Please insert to M2 Left or M2 Right " ;
				   int leftTankWeight ,rightTankWeight;
				   leftTankWeight = m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL3)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL3)];
				   WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("M2 weight (left)%d ",leftTankWeight ));
				   leftTankWeight = leftTankWeight > 0 ? leftTankWeight : 0;
				   rightTankWeight = m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL4)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL4)];
				   WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("M2 weight (right)%d ",rightTankWeight ));
				   rightTankWeight = rightTankWeight > 0 ? rightTankWeight : 0;
				   if(((leftTankWeight + rightTankWeight)< HSW_MIN_LIQUID_WEIGHT))
				   {
					msg= "Please insert to M2 Left or M2 Right " ;
				    WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight (left+right)%d ",rightTankWeight+leftTankWeight));
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL3_SEGMENT]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL4_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				   }
				}
		   }
		 }
		 else   /*there is no short = long*/
		 {
		 }
	 }
   needToCheck = true;	 
   ActiveTankID = m_BackEnd->GetActiveTankNum((TChamberIndex)TYPE_CHAMBER_MODEL3);
  if(IsChamberFlooded(static_cast<TChamberIndex>(TYPE_CHAMBER_MODEL3) )== true) /*chamber that no need to be checked*/
	 {
		needToCheck = false;
	 }
  if ((ActiveTankID == TYPE_TANK_MODEL5) || (ActiveTankID == TYPE_TANK_MODEL6) && (needToCheck == true))
  {
   if( (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL5_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M5_6_SEGMENT ]) ||  //m3left short = m3 long
			 (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL6_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M5_6_SEGMENT ]))  //m3 right short = m3 long
		 {
		   if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL5_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M5_6_SEGMENT ])&& //m3left short = m3 long
			  (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL6_SEGMENT] != m_ParamMgr->MRW_TypesArrayPerSegment[M5_6_SEGMENT ]))  //m3 right short != m3 long
		   {
			 if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL5_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL5)) ||
				(m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL5) - m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL5)]< HSW_MIN_LIQUID_WEIGHT)) //m3 left != tank resin left
			   {
			   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL5_SEGMENT]) == -1)
					{
					WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %f ",m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL5) - m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL5)]));
					QString msg= "Please insert to M3 Left " ;
					msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL5_SEGMENT]);
					Page->Strings.Add(msg.c_str());
					isWeightOK = false;
					Page->SetChecksMask(0);
					}
			   }
		  }
		else if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL5_SEGMENT] != m_ParamMgr->MRW_TypesArrayPerSegment[M5_6_SEGMENT ])&& //m3left short = m3 long
				   (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL6_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M5_6_SEGMENT ]))  //m3 right short != m3 long
				{
				if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL6_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL6)) ||
					(m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL6)- m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL6)]< HSW_MIN_LIQUID_WEIGHT)) //m3 right != tank resin right
				 {
					if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL6_SEGMENT]) == -1)
						{
						WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %f ",m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL6) - m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL6)]));
						QString msg= "Please insert to M3 Right " ;
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL6_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				 }
				}
		   else // m3 left = m3 long and m3 right = m3 long
		   {
			   if(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL5_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL5) && //m3 left != tank resin left
				 (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL6_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL6))) //m3 right != tank resin right
				{
				  QString msg= "Please insert to M3 Left or M3 Right " ;
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL5_SEGMENT]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL6_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				}
			   else if(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL5_SEGMENT] == m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL5) && //m3 left == tank resin left
					 (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL6_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL6))) //m3 right != tank resin right
				{
				   QString msg= "Please insert to M3 Left" ;
				   int leftTankWeight ,rightTankWeight;
				   leftTankWeight = m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL5)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL5)];
				   leftTankWeight = leftTankWeight > 0 ? leftTankWeight : 0;
				   if((leftTankWeight < HSW_MIN_LIQUID_WEIGHT))
				   {
					msg= "Please insert to M3 Left" ;
					WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %d ",leftTankWeight));
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL5_SEGMENT]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL6_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				   }
				}
				else if(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL5_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL5) && //m1 left != tank resin left
					 (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL6_SEGMENT] == m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL6))) //m1 right == tank resin right
				{
				   QString msg= "Please insert to M3 Right " ;
				   int rightTankWeight;
				   rightTankWeight = m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL6)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL6)];
				   rightTankWeight = rightTankWeight > 0 ? rightTankWeight : 0;
				   if((rightTankWeight < HSW_MIN_LIQUID_WEIGHT))
				   {
					msg= "Please insert to M3 Right " ;
						WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %d ",rightTankWeight));
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL5_SEGMENT]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL6_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
						}
				}
				else  /*two tanks material are OK*/
				{
				   QString msg= "Please insert to M3 Left or M3 Right " ;
				   int leftTankWeight ,rightTankWeight;
				   leftTankWeight = m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL5)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL5)];
				   leftTankWeight = leftTankWeight > 0 ? leftTankWeight : 0;
				   rightTankWeight = m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL6)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL6)];
				   rightTankWeight = rightTankWeight > 0 ? rightTankWeight : 0;
				   if(((leftTankWeight + rightTankWeight)< HSW_MIN_LIQUID_WEIGHT))
				   {
					msg= "Please insert to M3 Left or M3 Right " ;
					WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight (left+right)%d ",rightTankWeight+leftTankWeight));
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL5_SEGMENT]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL6_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				   }
				}

		   }
		 }
		 else   /*there is no short = long*/
		 {
		 }
   }
    needToCheck = true;
	 ActiveTankID = m_BackEnd->GetActiveTankNum((TChamberIndex)TYPE_CHAMBER_MODEL4);
	  if(IsChamberFlooded(static_cast<TChamberIndex>(TYPE_CHAMBER_MODEL4)) == true) /*chamber that no need to be checked*/
	 {
		needToCheck = false;
	 }
   if ((ActiveTankID == TYPE_TANK_MODEL7) || (ActiveTankID == TYPE_TANK_MODEL8)&&(	needToCheck == true))
  {
   if( (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL7_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M7_8_SEGMENT ]) ||  //m4left short = m4 long
			 (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL8_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M7_8_SEGMENT ]))  //m right short = m4 long
		 {
		   if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL7_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M7_8_SEGMENT ])&& //m4 left short = m4 long
			  (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL8_SEGMENT] != m_ParamMgr->MRW_TypesArrayPerSegment[M7_8_SEGMENT ]))  //m4 right short != m4 long
		   {
			 if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL7_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL7)) ||
				(m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL7) - m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL7)]< HSW_MIN_LIQUID_WEIGHT)) //m4 left != tank resin left
			   {
			   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL7_SEGMENT]) == -1)
					{
					WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %f ",m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL7) - m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL7)]));
					QString msg= "Please insert to M4 Left " ;
					msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL7_SEGMENT]);
					Page->Strings.Add(msg.c_str());
					isWeightOK = false;
					Page->SetChecksMask(0);
					}
			   }
		  }
		else if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL7_SEGMENT] != m_ParamMgr->MRW_TypesArrayPerSegment[M7_8_SEGMENT ])&& //m4left short = m4 long
				   (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL8_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M7_8_SEGMENT ]))  //m4 right short != m4 long
				{
				if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL8_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL8)) ||
					(m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL8)- m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL7)]< HSW_MIN_LIQUID_WEIGHT)) //m4 right != tank resin right
				 {
					if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL8_SEGMENT]) == -1)
						{
						WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %f ",m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL8) - m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL8)]));
						QString msg= "Please insert to M4 Right " ;
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL8_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				 }
				}
		   else // m4 left = m4 long and m4 right = m4 long
		   {
			   if(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL7_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL7) && //m4 left != tank resin left
				 (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL8_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL8))) //m4 right != tank resin right
				{
				  QString msg= "Please insert to M4 Left or M4 Right " ;
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL7_SEGMENT]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL8_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				}
				 else if(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL7_SEGMENT] == m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL7) && //m4 left == tank resin left
					 (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL8_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL8))) //m4 right != tank resin right
				{
				   QString msg= "Please insert to M4 Left" ;
				   int leftTankWeight ,rightTankWeight;
				   leftTankWeight = m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL7)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL7)];
				   leftTankWeight = leftTankWeight > 0 ? leftTankWeight : 0;
				   if((leftTankWeight < HSW_MIN_LIQUID_WEIGHT))
				   {
					msg= "Please insert to M4 Left" ;
					WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %d ",leftTankWeight));
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL7_SEGMENT]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL8_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				   }
				}
				else if(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL7_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL7) && //m4 left != tank resin left
					 (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL8_SEGMENT] == m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL8))) //m4 right == tank resin right
				{
				   QString msg= "Please insert to M4 Right " ;
				   int rightTankWeight;
				   rightTankWeight = m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL8)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL8)];
				   rightTankWeight = rightTankWeight > 0 ? rightTankWeight : 0;
				   if((rightTankWeight < HSW_MIN_LIQUID_WEIGHT))
				   {
					msg= "Please insert to M4 Right " ;
						WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %d ",rightTankWeight));
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL7_SEGMENT]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL8_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
						}
				}
				else  /*two tanks material are OK*/
				{
				   QString msg= "Please insert to M4 Left or M4 Right " ;
				   int leftTankWeight ,rightTankWeight;
				   leftTankWeight = m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL7)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL7)];
				   leftTankWeight = leftTankWeight > 0 ? leftTankWeight : 0;
				   rightTankWeight = m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL8)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL8)];
				   rightTankWeight = rightTankWeight > 0 ? rightTankWeight : 0;
				   if(((leftTankWeight + rightTankWeight)< HSW_MIN_LIQUID_WEIGHT))
				   {
					msg= "Please insert to M4 Left or M4 Right " ;
				    WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight (left+right)%d ",rightTankWeight+leftTankWeight));
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL7_SEGMENT]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL8_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				   }
				}
		   }
		 }
		 else   /*there is no short = long*/
		 {
		 }
	 }
	 needToCheck = true;
	 ActiveTankID = m_BackEnd->GetActiveTankNum((TChamberIndex)TYPE_CHAMBER_MODEL5);
	  if(IsChamberFlooded(static_cast<TChamberIndex>(TYPE_CHAMBER_MODEL5)) == true) /*chamber that no need to be checked*/
	 {
		needToCheck = false;
	 }
   if ((ActiveTankID == TYPE_TANK_MODEL9) || (ActiveTankID == TYPE_TANK_MODEL10)&&(	needToCheck == true))
  {
   if( (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL9_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M9_10_SEGMENT ]) ||  //m5left short = m5 long
			 (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL10_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M9_10_SEGMENT ]))  //m5 right short = m5 long
		 {
		   if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL9_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M9_10_SEGMENT ])&& //m5left short = m5 long
			  (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL10_SEGMENT] != m_ParamMgr->MRW_TypesArrayPerSegment[M9_10_SEGMENT ]))  //m5 right short != m5 long
		   {
			 if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL9_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL9)) ||
				(m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL9) - m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL9)]< HSW_MIN_LIQUID_WEIGHT)) //m5 left != tank resin left
			   {
			   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL9_SEGMENT]) == -1)
					{
					WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %f ",m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL9) - m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL9)]));
					QString msg= "Please insert to M5 Left " ;
					msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL9_SEGMENT]);
					Page->Strings.Add(msg.c_str());
					isWeightOK = false;
					Page->SetChecksMask(0);
					}
			   }
		  }
		else if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL9_SEGMENT] != m_ParamMgr->MRW_TypesArrayPerSegment[M3_4_SEGMENT ])&& //m5left short = m5 long
				   (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL10_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M3_4_SEGMENT ]))  //m5 right short != m5 long
				{
				if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL10_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL10)) ||
					(m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL10)- m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL10)]< HSW_MIN_LIQUID_WEIGHT)) //m5 right != tank resin right
				 {
					if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL10_SEGMENT]) == -1)
						{
						WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %f ",m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL10) - m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL10)]));
						QString msg= "Please insert to M5 Right " ;
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL10_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				 }
				}
		   else // m5 left = m5 long and m5 right = m5 long
		   {
			   if(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL9_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL9) && //m5 left != tank resin left
				 (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL10_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL10))) //m5 right != tank resin right
				{
				  QString msg= "Please insert to M5 Left or M5 Right " ;
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL9_SEGMENT]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL10_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				}
				 else if(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL9_SEGMENT] == m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL9) && //m5 left == tank resin left
					 (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL10_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL10))) //m5 right != tank resin right
				{
				   QString msg= "Please insert to M5 Left" ;
				   int leftTankWeight ,rightTankWeight;
				   leftTankWeight = m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL9)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL9)];
				   leftTankWeight = leftTankWeight > 0 ? leftTankWeight : 0;
				   if((leftTankWeight < HSW_MIN_LIQUID_WEIGHT))
				   {
					msg= "Please insert to M5 Left" ;
					WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %d ",leftTankWeight));
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL9_SEGMENT]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL10_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				   }
				}
				else if(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL9_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL9) && //m5 left != tank resin left
					 (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL10_SEGMENT] == m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL10))) //m5 right == tank resin right
				{
				   QString msg= "Please insert to M5 Right " ;
				   int rightTankWeight;
				   rightTankWeight = m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL10)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL10)];
				   rightTankWeight = rightTankWeight > 0 ? rightTankWeight : 0;
				   if((rightTankWeight < HSW_MIN_LIQUID_WEIGHT))
				   {
					msg= "Please insert to M5 Right " ;
						WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %d ",rightTankWeight));
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL9_SEGMENT]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL10_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
						}
				}
				else  /*two tanks material are OK*/
				{
				   QString msg= "Please insert to M5 Left or M5 Right " ;
				   int leftTankWeight ,rightTankWeight;
				   leftTankWeight = m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL9)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL9)];
				   leftTankWeight = leftTankWeight > 0 ? leftTankWeight : 0;
				   rightTankWeight = m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL10)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL10)];
				   rightTankWeight = rightTankWeight > 0 ? rightTankWeight : 0;
				   if(((leftTankWeight + rightTankWeight)< HSW_MIN_LIQUID_WEIGHT))
				   {
					msg= "Please insert to M5 Left or M5 Right " ;
					WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight (left+right)%d ",rightTankWeight+leftTankWeight));
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL9_SEGMENT]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL10_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				   }
				}
		   }
		 }
		 else   /*there is no short = long*/
		 {
		 }
	 }
	 needToCheck = true;
	 ActiveTankID = m_BackEnd->GetActiveTankNum((TChamberIndex)TYPE_CHAMBER_MODEL6);
	  if(IsChamberFlooded(static_cast<TChamberIndex>(TYPE_CHAMBER_MODEL6)) == true) /*chamber that no need to be checked*/
	 {
		needToCheck = false;
	 }
   if ((ActiveTankID == TYPE_TANK_MODEL11) || (ActiveTankID == TYPE_TANK_MODEL12)&&(	needToCheck == true))
  {
   if( (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL11_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M11_12_SEGMENT ]) ||  //m6left short = m6 long
			 (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL12_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M11_12_SEGMENT ]))  //m6 right short = m6 long
		 {
		   if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL11_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M11_12_SEGMENT ])&& //m6left short = m6 long
			  (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL12_SEGMENT] != m_ParamMgr->MRW_TypesArrayPerSegment[M11_12_SEGMENT ]))  //m6 right short != m6 long
		   {
			 if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL11_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL11)) ||
				(m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL11) - m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL11)]< HSW_MIN_LIQUID_WEIGHT)) //m6 left != tank resin left
			   {
			   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL11_SEGMENT]) == -1)
					{
					WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %f ",m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL11) - m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL11)]));
					QString msg= "Please insert to M6 Left " ;
					msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL11_SEGMENT]);
					Page->Strings.Add(msg.c_str());
					isWeightOK = false;
					Page->SetChecksMask(0);
					}
			   }
		  }
		else if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL11_SEGMENT] != m_ParamMgr->MRW_TypesArrayPerSegment[M11_12_SEGMENT ])&& //m6left short = m6 long
				   (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL12_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M11_12_SEGMENT ]))  //m6 right short != m6 long
				{
				if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL12_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL12)) ||
					(m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL12)- m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL12)]< HSW_MIN_LIQUID_WEIGHT)) //m6 right != tank resin right
				 {
					if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL12_SEGMENT]) == -1)
						{
						WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %f ",m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL12) - m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL12)]));
						QString msg= "Please insert to M6 Right " ;
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL12_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				 }
				}
		   else // m6 left = m6 long and m6 right = m6 long
		   {
			   if(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL11_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL11) && //m6 left != tank resin left
				 (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL12_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL12))) //m6 right != tank resin right
				{
				  QString msg= "Please insert to M6 Left or M6 Right " ;
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL11_SEGMENT]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL12_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				}
					 else if(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL11_SEGMENT] == m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL11) && //m6 left == tank resin left
					 (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL12_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL12))) //m6 right != tank resin right
				{
				   QString msg= "Please insert to M6 Left" ;
				   int leftTankWeight ,rightTankWeight;
				   leftTankWeight = m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL11)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL11)];
				   leftTankWeight = leftTankWeight > 0 ? leftTankWeight : 0;
				   if((leftTankWeight < HSW_MIN_LIQUID_WEIGHT))
				   {
					msg= "Please insert to M6 Left" ;
					WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %d ",leftTankWeight));
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL11_SEGMENT]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL12_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				   }
				}
				else if(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL11_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL11) && //m6 left != tank resin left
					 (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL12_SEGMENT] == m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL12))) //m6 right == tank resin right
				{
				   QString msg= "Please insert to M6 Right " ;
				   int rightTankWeight;
				   rightTankWeight = m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL12)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL12)];
				   rightTankWeight = rightTankWeight > 0 ? rightTankWeight : 0;
				   if((rightTankWeight < HSW_MIN_LIQUID_WEIGHT))
				   {
					msg= "Please insert to M6 Right " ;
						WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %d ",rightTankWeight));
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL11_SEGMENT]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL12_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
						}
				}
				else  /*two tanks material are OK*/
				{
				   QString msg= "Please insert to M6 Left or M6 Right " ;
				   int leftTankWeight ,rightTankWeight;
				   leftTankWeight = m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL11)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL11)];
				   leftTankWeight = leftTankWeight > 0 ? leftTankWeight : 0;
				   rightTankWeight = m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL12)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL12)];
				   rightTankWeight = rightTankWeight > 0 ? rightTankWeight : 0;
				   if(((leftTankWeight + rightTankWeight)< HSW_MIN_LIQUID_WEIGHT))
				   {
					msg= "Please insert to M6 Left or M6 Right " ;
				    WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight (left+right)%d ",rightTankWeight+leftTankWeight));
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL11_SEGMENT]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL12_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				   }
				}
		   }
		 }
		 else   /*there is no short = long*/
		 {
		 }
	 }
	 needToCheck = true;
	 ActiveTankID = m_BackEnd->GetActiveTankNum((TChamberIndex)TYPE_CHAMBER_MODEL7);
	  if(IsChamberFlooded(static_cast<TChamberIndex>(TYPE_CHAMBER_MODEL7)) == true) /*chamber that no need to be checked*/
	 {
		needToCheck = false;
	 }
  if ((ActiveTankID == TYPE_TANK_MODEL13) || (ActiveTankID == TYPE_TANK_MODEL14)&&(needToCheck == true))
  {
   if( (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL13_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M13_14_SEGMENT ]) ||  //m7left short = m7 long
			 (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL14_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M13_14_SEGMENT ]))  //m7 right short = m7 long
		 {
		   if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL13_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M13_14_SEGMENT ])&& //m7left short = m7 long
			  (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL14_SEGMENT] != m_ParamMgr->MRW_TypesArrayPerSegment[M13_14_SEGMENT ]))  //m7 right short != m7 long
		   {
			 if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL13_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL13)) ||
				(m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL13) - m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL13)]< HSW_MIN_LIQUID_WEIGHT)) //m7 left != tank resin left
			   {
			   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL13_SEGMENT]) == -1)
					{
					WriteToLogFile(LOG_TAG_GENERAL,QFormatStr(" S weight %f ",m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL13) - m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL13)]));
					QString msg= "Please insert to SH Left "; // DM6
					msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL13_SEGMENT]);
					Page->Strings.Add(msg.c_str());
					isWeightOK = false;
					Page->SetChecksMask(0);
					}
			   }
		  }
		else if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL13_SEGMENT] != m_ParamMgr->MRW_TypesArrayPerSegment[M13_14_SEGMENT ])&& //m7left short = m7 long
				   (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL14_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M13_14_SEGMENT ]))  //m7 right short != m7 long
				{
				if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL14_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL14)) ||
					(m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL14)- m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL14)]< HSW_MIN_LIQUID_WEIGHT)) //m7 right != tank resin right
				 {
					if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL14_SEGMENT]) == -1)
						{
						WriteToLogFile(LOG_TAG_GENERAL,QFormatStr(" S weight %f ",m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL14) - m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL14)]));
						QString msg= "Please insert to SH Right "; // DM6
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL14_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				 }
				}
		   else // m7 left = m1 long and m7 right = m7 long
		   {
			   if(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL13_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL13) && //m7 left != tank resin left
				 (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL14_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL14))) //m7 right != tank resin right
				{
				WriteToLogFile(LOG_TAG_GENERAL,"Support left != tank resin left or support right != tank resin right" );
				  QString msg= "Please insert to SH Left or SH Right "; // DM6
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL13_SEGMENT]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL14_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				}
				else  /*two tanks material are OK*/
				{
				   QString msg= "Please insert to SH Left or SH Right "; // DM6
				   int leftTankWeight ,rightTankWeight;
				   leftTankWeight = m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL13)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL13)];
				   leftTankWeight = leftTankWeight > 0 ? leftTankWeight : 0;
				   WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("S weight (left)%d ",leftTankWeight ));

				   rightTankWeight = m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL14)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL14)];
				   rightTankWeight = rightTankWeight > 0 ? rightTankWeight : 0;
				   WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("S weight (right)%d ",rightTankWeight ));
				   
				   if(((leftTankWeight + rightTankWeight)< HSW_MIN_LIQUID_WEIGHT))
				   {
					msg= "Please insert to SH Left or SH Right "; // DM6
					WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("S weight (left+right)%d ",rightTankWeight+leftTankWeight));
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL13_SEGMENT]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL14_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				   }
				}
		   }
		 }
		 else   /*there is no short = long*/
		 {
		 }
	 }

   	int totalSupportWeight = 0;
	totalSupportWeight = m_BackEnd->GetTotalWeight(TYPE_CHAMBER_SUPPORT);
	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight totalSupportWeight%d ",totalSupportWeight));

	if(totalSupportWeight < HSW_MIN_LIQUID_WEIGHT)
	{
	  QString msg= "Please insert " ;
	  msg+=  m_ParamMgr->TypesArrayPerChamber[TYPE_CHAMBER_SUPPORT].c_str();
	  Page->Strings.Add(msg.c_str());
	  isWeightOK = false;
	  Page->SetChecksMask(0);
	  WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight (left+right)%d ",totalSupportWeight));
	}


	   for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
  {
	if (!(m_BackEnd->IsActiveLiquidTankInserted(i)))
	   {
	 // check if there is same material in m7left(short) Or M7Right(short) and M3(long) and M3 Tank.
		 if( (m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_SEGMENTL] == m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_LONG ]) ||  //m7left short = m7 long
			 (m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_SEGMENTR] == m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_LONG ]))  //m7 right short = m7 long

		  if (Page->Strings.FindSubStr(m_ParamMgr->TypesArrayPerChamber[i]) == -1)
		   {
            QString msg= "Please insert " ;
            msg +=  m_ParamMgr->TypesArrayPerChamber[i].c_str();
			msg += " to ";
            msg += LOAD_STRING_ARRAY(i, IDS_GRAPHIC_CHAMBER_STR_SINGLE);
/*			if( i == 0)
			 msg+=" to S";
			else
			{
			msg+=" to M" + QIntToStr(i);
			}*/
			Page->Strings.Add(msg.c_str());
			isWeightOK = false;
			Page->SetChecksMask(0);
		   }
	   }
  }

  if(isWeightOK == true)
  {
	 for(int i = LAST_TANK_TYPE-1; i >= FIRST_TANK_TYPE; i--)
	 {
		m_BackEnd->SkipMachineResinTypeChecking(i, false);
	}
  }
  return isWeightOK;
  }

/*-----------------------------------------------------------------------------*/
bool CHeadServiceWizard::CheckLiquidWeight()
{
  bool isWeightOK= true;
   CCheckBoxWizardPage *Page = dynamic_cast<CCheckBoxWizardPage *>(Pages[qrWeightProblemPage]);
   Page->Strings.Clear();
   TTankIndex ActiveTankID 		= NO_TANK;

   if(m_ParamMgr->HSW_DM3_Enable == true) /*check M3 if needed*/
   {
	 // check if there is same material in m3left(short) Or M3Right(short) and M3(long) and M3 Tank.
		 if( (m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_SEGMENTL] == m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_LONG ]) ||  //m3left short = m3 long
			 (m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_SEGMENTR] == m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_LONG ]))  //m3 right short = m3 long
		 {
		   /*check if same material is in tank*/
		   if((m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_SEGMENTL] == m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_LONG ])&& //m3left short = m3 long
			  (m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_SEGMENTR] != m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_LONG ]))  //m3 right short != m3 long
		   {
			  if((m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_SEGMENTL] != m_BackEnd->GetTankMaterialType(SUPPORT_MODEL_TANK_L)) ||
				(m_BackEnd->GetTankWeightInGram(SUPPORT_MODEL_TANK_L) - m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(SUPPORT_MODEL_TANK_L)]< HSW_MIN_LIQUID_WEIGHT)) //m3 left != tank resin left
			   {
			   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_SEGMENTL]) == -1)
					{
					WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %f ",m_BackEnd->GetTankWeightInGram(SUPPORT_MODEL_TANK_L) - m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(SUPPORT_MODEL_TANK_L)]));
					QString msg= "Please insert to M3 Left " ;
					msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_SEGMENTL]);
					Page->Strings.Add(msg.c_str());
					isWeightOK = false;
					Page->SetChecksMask(0);
					}
			   }
		   }
		   else if((m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_SEGMENTL] != m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_LONG ])&& //m3left short = m3 long
				   (m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_SEGMENTR] == m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_LONG ]))  //m3 right short != m3 long
				{
				if((m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_SEGMENTR] != m_BackEnd->GetTankMaterialType(SUPPORT_MODEL_TANK_R)) ||
					(m_BackEnd->GetTankWeightInGram(SUPPORT_MODEL_TANK_R)- m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(SUPPORT_MODEL_TANK_R)]< HSW_MIN_LIQUID_WEIGHT)) //m3 right != tank resin right
				 {
					if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_SEGMENTR]) == -1)
						{
						WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %f ",m_BackEnd->GetTankWeightInGram(SUPPORT_MODEL_TANK_R) - m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(SUPPORT_MODEL_TANK_R)]));
						QString msg= "Please insert to M3 Right " ;
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_SEGMENTR]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				 }
				}
		   else // m3 left = m3 long and m3 right = m3 long
		   {
			   if(m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_SEGMENTL] != m_BackEnd->GetTankMaterialType(SUPPORT_MODEL_TANK_L) && //m3 left != tank resin left
				 (m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_SEGMENTR] != m_BackEnd->GetTankMaterialType(SUPPORT_MODEL_TANK_R))) //m3 right != tank resin right
				{
				/*two tanks material are not OK*/
				  QString msg= "Please insert to M3 Left or M3 Right " ;
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_SEGMENTL]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_SEGMENTR]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				}
				else if(m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_SEGMENTL] == m_BackEnd->GetTankMaterialType(SUPPORT_MODEL_TANK_L) && //m3 left == tank resin left
				 (m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_SEGMENTR] != m_BackEnd->GetTankMaterialType(SUPPORT_MODEL_TANK_R))) //m3 right != tank resin right)
				 {
					 QString msg= "Please insert to M3 Left " ;
				   int leftTankWeight ;
				   leftTankWeight = m_BackEnd->GetTankWeightInGram(SUPPORT_MODEL_TANK_L)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(SUPPORT_MODEL_TANK_L)];
				   leftTankWeight = leftTankWeight > 0 ? leftTankWeight : 0;
				   if(((leftTankWeight )< HSW_MIN_LIQUID_WEIGHT))
				   {
					msg= "Please insert to M3 Left " ;
					WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %d ",leftTankWeight));
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_SEGMENTL]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_SEGMENTR]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				   }
				 }
				else if(m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_SEGMENTL] != m_BackEnd->GetTankMaterialType(SUPPORT_MODEL_TANK_L) && //m3 left != tank resin left
				 (m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_SEGMENTR] == m_BackEnd->GetTankMaterialType(SUPPORT_MODEL_TANK_R))) //m3 right == tank resin right)
				{
				   QString msg= "Please insert to M3 Right " ;
				   int rightTankWeight;
				   rightTankWeight = m_BackEnd->GetTankWeightInGram(SUPPORT_MODEL_TANK_R)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(SUPPORT_MODEL_TANK_R)];
				   rightTankWeight = rightTankWeight > 0 ? rightTankWeight : 0;
				   if((rightTankWeight < HSW_MIN_LIQUID_WEIGHT))
				   {
					msg= "Please insert to M3 Right " ;
					WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %d ",rightTankWeight));
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_SEGMENTL]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_SEGMENTR]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				   }
				}
				else  /*two tanks material are OK*/
				{
				   QString msg= "Please insert to M3 Left or M3 Right " ;
				   int leftTankWeight ,rightTankWeight;
				   leftTankWeight = m_BackEnd->GetTankWeightInGram(SUPPORT_MODEL_TANK_L)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(SUPPORT_MODEL_TANK_L)];
				   leftTankWeight = leftTankWeight > 0 ? leftTankWeight : 0;
				   rightTankWeight = m_BackEnd->GetTankWeightInGram(SUPPORT_MODEL_TANK_R)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(SUPPORT_MODEL_TANK_R)];
				   rightTankWeight = rightTankWeight > 0 ? rightTankWeight : 0;
				   if(((leftTankWeight + rightTankWeight)< HSW_MIN_LIQUID_WEIGHT))
				   {
					msg= "Please insert to M3 Left or M3 Right " ;
					WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight(left +right) %d ",leftTankWeight+rightTankWeight));
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_SEGMENTL]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_SEGMENTR]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				   }
				}

		   }
		 }
		 else   /*there is no short = long*/
		 {
			 /*illegal case , no need to check , within_wizard will be 1*/
		 }
   } // M3
   ActiveTankID = m_BackEnd->GetActiveTankNum((TChamberIndex)TYPE_CHAMBER_MODEL1);
  if ((ActiveTankID == TYPE_TANK_MODEL1) || (ActiveTankID == TYPE_TANK_MODEL2))
  {
   if( (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL1_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M1_2_SEGMENT ]) ||  //m1left short = m1 long
			 (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL2_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M1_2_SEGMENT ]))  //m1 right short = m1 long
		 {
		   if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL1_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M1_2_SEGMENT ])&& //m1left short = m1 long
			  (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL2_SEGMENT] != m_ParamMgr->MRW_TypesArrayPerSegment[M1_2_SEGMENT ]))  //m1 right short != m1 long
		   {
			 if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL1_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL1)) ||
				(m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL1) - m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL1)]< HSW_MIN_LIQUID_WEIGHT)) //m3 left != tank resin left
			   {
			   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL1_SEGMENT]) == -1)
					{
					WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %f ",m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL1) - m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL1)]));
					QString msg= "Please insert to M1 Left " ;
					msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL1_SEGMENT]);
					Page->Strings.Add(msg.c_str());
					isWeightOK = false;
					Page->SetChecksMask(0);
					}
			   }
		  }
		else if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL1_SEGMENT] != m_ParamMgr->MRW_TypesArrayPerSegment[M1_2_SEGMENT ])&& //m1left short = m1 long
				   (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL2_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M1_2_SEGMENT ]))  //m1 right short != m1 long
				{
				if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL2_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL2)) ||
					(m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL2)- m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL2)]< HSW_MIN_LIQUID_WEIGHT)) //m1 right != tank resin right
				 {
					if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL2_SEGMENT]) == -1)
						{
						WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %f ",m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL2) - m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL2)]));
						QString msg= "Please insert to M1 Right " ;
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL2_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				 }
				}
		   else // m1 left = m1 long and m1 right = m1 long
		   {
			   if(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL1_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL1) && //m1 left != tank resin left
				 (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL2_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL2))) //m1 right != tank resin right
				{
				  QString msg= "Please insert to M1 Left or M1 Right " ;
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL1_SEGMENT]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL2_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				}
			   else if(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL1_SEGMENT] == m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL1) && //m1 left == tank resin left
					 (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL2_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL2))) //m1 right != tank resin right
				{
				   QString msg= "Please insert to M1 Left" ;
				   int leftTankWeight ,rightTankWeight;
				   leftTankWeight = m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL1)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL1)];
				   leftTankWeight = leftTankWeight > 0 ? leftTankWeight : 0;
				   if((leftTankWeight < HSW_MIN_LIQUID_WEIGHT))
				   {
					msg= "Please insert to M1 Left" ;
					WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %d ",leftTankWeight));
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL1_SEGMENT]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL2_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				   }
				}
				else if(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL1_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL1) && //m1 left != tank resin left
					 (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL2_SEGMENT] == m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL2))) //m1 right == tank resin right
				{
				   QString msg= "Please insert to M1 Right " ;
				   int rightTankWeight;
				   rightTankWeight = m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL2)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL2)];
				   rightTankWeight = rightTankWeight > 0 ? rightTankWeight : 0;
				   if((rightTankWeight < HSW_MIN_LIQUID_WEIGHT))
				   {
					msg= "Please insert to M1 Right " ;
						WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %d ",rightTankWeight));
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL1_SEGMENT]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL2_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
						}
				}
				else  /*two tanks material are OK*/
				{
				   QString msg= "Please insert to M1 Left or M1 Right " ;
				   int leftTankWeight ,rightTankWeight;
				   leftTankWeight = m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL1)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL1)];
				   leftTankWeight = leftTankWeight > 0 ? leftTankWeight : 0;
				   rightTankWeight = m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL2)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL2)];
				   rightTankWeight = rightTankWeight > 0 ? rightTankWeight : 0;
				   if(((leftTankWeight + rightTankWeight)< HSW_MIN_LIQUID_WEIGHT))
				   {
					msg= "Please insert to M1 Left or M1 Right " ;
					WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight (left+right)%d ",rightTankWeight+leftTankWeight));
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL1_SEGMENT]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL2_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				   }
				}

		   }
		 }
		 else   /*there is no short = long*/
		 {
		 }
   }
  ActiveTankID = m_BackEnd->GetActiveTankNum((TChamberIndex)TYPE_CHAMBER_MODEL2);
   if ((ActiveTankID == TYPE_TANK_MODEL3) || (ActiveTankID == TYPE_TANK_MODEL4))
  {
   if( (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL3_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M3_4_SEGMENT ]) ||  //m2left short = m2 long
			 (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL4_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M3_4_SEGMENT ]))  //m right short = m2 long
		 {
		   if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL3_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M3_4_SEGMENT ])&& //m2left short = m2 long
			  (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL4_SEGMENT] != m_ParamMgr->MRW_TypesArrayPerSegment[M3_4_SEGMENT ]))  //m2 right short != m2 long
		   {
			 if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL3_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL3)) ||
				(m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL3) - m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL3)]< HSW_MIN_LIQUID_WEIGHT)) //m3 left != tank resin left
			   {
			   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL3_SEGMENT]) == -1)
					{
					WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %f ",m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL3) - m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL3)]));
					QString msg= "Please insert to M2 Left " ;
					msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL3_SEGMENT]);
					Page->Strings.Add(msg.c_str());
					isWeightOK = false;
					Page->SetChecksMask(0);
					}
			   }
		  }
		else if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL3_SEGMENT] != m_ParamMgr->MRW_TypesArrayPerSegment[M3_4_SEGMENT ])&& //m2left short = m2 long
				   (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL4_SEGMENT] == m_ParamMgr->MRW_TypesArrayPerSegment[M3_4_SEGMENT ]))  //m2 right short != m2 long
				{
				if((m_ParamMgr->MRW_TypesArrayPerSegment[MODEL4_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL4)) ||
					(m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL4)- m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL4)]< HSW_MIN_LIQUID_WEIGHT)) //m1 right != tank resin right
				 {
					if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL4_SEGMENT]) == -1)
						{
						WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight %f ",m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL4) - m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL4)]));
						QString msg= "Please insert to M2 Right " ;
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL4_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				 }
				}
		   else // m1 left = m1 long and m1 right = m1 long
		   {
			   if(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL3_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL3) && //m2 left != tank resin left
				 (m_ParamMgr->MRW_TypesArrayPerSegment[MODEL4_SEGMENT] != m_BackEnd->GetTankMaterialType(TYPE_TANK_MODEL4))) //m2 right != tank resin right
				{
				  QString msg= "Please insert to M2 Left or M2 Right " ;
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL3_SEGMENT]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL4_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				}
				else  /*two tanks material are OK*/
				{
				   QString msg= "Please insert to M2 Left or M2 Right " ;
				   int leftTankWeight ,rightTankWeight;
				   leftTankWeight = m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL3)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL3)];
				   leftTankWeight = leftTankWeight > 0 ? leftTankWeight : 0;
				   rightTankWeight = m_BackEnd->GetTankWeightInGram(TYPE_TANK_MODEL4)-m_ParamMgr->WeightLevelLimitArray[TankToStaticChamber(TYPE_TANK_MODEL4)];
				   rightTankWeight = rightTankWeight > 0 ? rightTankWeight : 0;
				   if(((leftTankWeight + rightTankWeight)< HSW_MIN_LIQUID_WEIGHT))
				   {
					msg= "Please insert to M2 Left or M2 Right " ;
				    WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight (left+right)%d ",rightTankWeight+leftTankWeight));
				   if(Page->Strings.FindSubStr(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL3_SEGMENT]) == -1)
						{
						msg+= QString(m_ParamMgr->MRW_TypesArrayPerSegment[MODEL4_SEGMENT]) ;
						Page->Strings.Add(msg.c_str());
						isWeightOK = false;
						Page->SetChecksMask(0);
						}
				   }
				}
		   }
		 }
		 else   /*there is no short = long*/
		 {
		 }
	 }
	int totalSupportWeight = 0;
	totalSupportWeight = m_BackEnd->GetTotalWeight(TYPE_CHAMBER_SUPPORT);
	if(totalSupportWeight < HSW_MIN_LIQUID_WEIGHT)
	{
	  QString msg= "Please insert " ;
	  msg+=  m_ParamMgr->TypesArrayPerChamber[TYPE_CHAMBER_SUPPORT].c_str();
	  Page->Strings.Add(msg.c_str());
	  isWeightOK = false;
	  Page->SetChecksMask(0);
	  WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("weight (left+right)%d ",totalSupportWeight));
	}
	   for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
  {
	if (!(m_BackEnd->IsActiveLiquidTankInserted(i)))
	   {
	   if(m_ParamMgr->HSW_DM3_Enable == false && i ==TYPE_CHAMBER_MODEL3) /*no need to check M3*/
		continue;
	   else
		{
	 // check if there is same material in m3left(short) Or M3Right(short) and M3(long) and M3 Tank.
		 if( (m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_SEGMENTL] == m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_LONG ]) ||  //m3left short = m3 long
			 (m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_SEGMENTR] == m_ParamMgr->MRW_TypesArrayPerSegment[SUPPORT_MODEL_LONG ]))  //m3 right short = m3 long

		  if (Page->Strings.FindSubStr(m_ParamMgr->TypesArrayPerChamber[i]) == -1)
		   {
			QString msg= "Please insert " ;
			msg+=  m_ParamMgr->TypesArrayPerChamber[i].c_str();
			if( i == 0)
			 msg+=" to S";
			else
			{
			msg+=" to M" + QIntToStr(i);
			}
			Page->Strings.Add(msg.c_str());
			isWeightOK = false;
			Page->SetChecksMask(0);
		   }
		}
	   }
  }

  if(isWeightOK == true)
  {
	 for(int i = LAST_TANK_TYPE-1; i >= FIRST_TANK_TYPE; i--)
	 {
		m_BackEnd->SkipMachineResinTypeChecking(i, false);
	}
  }
  return isWeightOK;
}

/*-----------------------------------------------------------------------------*/
bool CHeadServiceWizard::PurgeCicle(int num_of_purges)
{
// Goto Purge Position
	bool ret = true;
   TQErrCode Err = Q_NO_ERROR;
   TRY_FUNCTION(Err, m_BackEnd->GotoPurgePosition(true, true));
   for(int i = 0; i < num_of_purges; i++)
   {
	  m_BackEnd->Purge(false, false);
	  WriteToLogFile(LOG_TAG_GENERAL,"Purging");
	  if ((ret = Q2RTWizardSleep(m_ParamMgr->MotorPostPurgeTimeSec)) == false)
		 break;
   }
  return ret;
}
/*-----------------------------------------------------------------------------*/
void CHeadServiceWizard::VoltageCalibration (int headID, float voltage)
{
	   /*	float HeadTestV = 0;
		if(voltageIndex == LOW_VOLTAGE)
		   HeadTestV = m_ParamMgr->HSW_MaxVoltageArray[GetHeadsChamberForHSW(headID,SINGLE_MATERIAL_OPERATION_MODE)];
		else
		   HeadTestV = m_ParamMgr->HSW_MaxVoltageArray[GetHeadsChamberForHSW(headID,DIGITAL_MATERIAL_OPERATION_MODE)];
		 */
		float HeadTestV = voltage;
		m_BackEnd->SetAndMeasureHeadVoltage(headID,HeadTestV);
		WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head %s calibrated for %f voltage",
												GetHeadName(headID).c_str(),voltage));
		if(m_ParamMgr->IsModelHead(headID))
			m_ParamMgr->RequestedHeadVoltagesModel[headID] = HeadTestV;
		else
			m_ParamMgr->RequestedHeadVoltagesSupport[headID] = HeadTestV;

		m_ParamMgr->PotentiometerValues[headID]   = m_BackEnd->GetCurrHeadPotentiometerValue(headID);
}
/*-----------------------------------------------------------------------------*/
bool CHeadServiceWizard::WeightTest(int headID,float &measurement,int qualityMode, int &weightTestError)
{
	float *fireallMeasurement;
	weightTestError = WEIGHT_TEST_OK;
	fireallMeasurement = new float[m_ParamMgr->HSW_Num_of_cycles_for_fires];
	QString errMsg = "";
	bool status = true;
	TQErrCode Err = Q_NO_ERROR;
	float tmpMeasurement = 0;
	int frequency = m_ParamMgr->HSW_FrequencyArray[qualityMode].Value() ;
	CScopeRestorePoint<int>fireAllTimeout(m_ParamMgr->FireAllTime_ms);
	int currTimeOut = (m_ParamMgr->HSW_Num_of_fires/m_ParamMgr->HSW_Num_of_cycles_for_fires/frequency)*1000+500;
	m_ParamMgr->FireAllTime_ms = currTimeOut;
	int num_of_retries = m_ParamMgr->HSW_Num_of_purges_in_weigh_test;
	CMachineSequencer *Sequencer = Q2RTApplication->GetMachineManager()->GetMachineSequencer();
try
  {
	//ResetScale
	//m_scale->Reset(errMsg);
	// read measurement from the scale
	float prevWeight = m_scale->GetImmWeight(errMsg);
	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head %s prev. measurement %f ",
												GetHeadName(headID).c_str(),
												prevWeight));
	if(errMsg!="")
		{
			WriteToLogFile(LOG_TAG_GENERAL,QFormatStr(errMsg.c_str()));
			weightTestError = SCALE_COMMUNICATION_FAIL;
			return false;
		}
	//SetData for Fireall
	//FireAll
	int headMask = 1;
	#ifdef OBJET_MACHINE_KESHET
	int numOfFires = m_ParamMgr->HSW_Num_of_fires/m_ParamMgr->HSW_Num_of_cycles_for_fires;
	#else
	int numOfFires = m_ParamMgr->HSW_Num_of_fires/m_ParamMgr->HSW_Num_of_cycles_for_fires/1000;
	#endif
	headMask  = ~(headMask<< headID);
	YieldWizardThread();
	for(int i=0;i<m_ParamMgr->HSW_Num_of_cycles_for_fires;i++)  // fire all cycles.
	{
	  fireallMeasurement[i] = 0;
	  float fireallPrevMeasurement = 0;
	  float fireallTmpMeasurement = 0;
	  float fireallAvgMeasurement = 0;
	  fireallPrevMeasurement = m_scale->GetImmWeight(errMsg);
	  if(Sequencer->FireAllSequence(headMask,numOfFires,qualityMode) != Q_NO_ERROR)
	  {
	  WriteToLogFile(LOG_TAG_GENERAL,"Fail to FireAll for head %s",GetHeadName(headID).c_str());
		 return false;
	  }
	  YieldWizardThread();
	  
	  QSleep(m_ParamMgr->HSW_Sc_stab_time*1000);
	  
	  for(int j= 0;j<m_ParamMgr->HSW_Num_Of_ScaleReading_Between_Fires;j++)
		{
			// read measurement from the scale
		  fireallTmpMeasurement = m_scale->GetImmWeight(errMsg);
		  WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head %s fireall tmp measurement %f ",
												GetHeadName(headID).c_str(),
												fireallTmpMeasurement));

			if( fireallTmpMeasurement < m_ParamMgr->HSW_NOISE_LEVEl_ERROR )
			{
			  WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head Optimization Wizard: Head %s Scale reading error: Measurement too low, repeat scale reading %f ",
														GetHeadName(headID).c_str(),
														fireallAvgMeasurement));
			 QSleep(m_ParamMgr->HSW_Sc_stab_time*1000);
			 fireallTmpMeasurement = m_scale->GetImmWeight(errMsg);
			 if( fireallTmpMeasurement > m_ParamMgr->HSW_NOISE_LEVEl_ERROR )
				{
				   WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head Optimization Wizard: Head %s Scale reading error: second scale reading successful %f ",
														GetHeadName(headID).c_str(),
														fireallAvgMeasurement));
				}
				else
				{
				   WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head Optimization Wizard: Head %s Scale reading error: cancel wizard, check scale and check heads. ",
														GetHeadName(headID).c_str()));
				   //	Cancel wizard with request to check scale and perform pattern-test
					weightTestError = SCALE_PROBLEM ;
                    return false;
                }
			}
			fireallAvgMeasurement += fireallTmpMeasurement;
			if(errMsg!="")
			{
				errMsg.clear();
				// try again
				 fireallAvgMeasurement += m_scale->GetImmWeight(errMsg);
				 if(errMsg!="")
				{
					WriteToLogFile(LOG_TAG_GENERAL,QFormatStr(errMsg.c_str()));
					weightTestError = SCALE_COMMUNICATION_FAIL;
					return false;
				}
				WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head %s AVG fireall measurement %f ",
														GetHeadName(headID).c_str(),
													fireallAvgMeasurement));
			}
		}
		fireallMeasurement[i] = fireallAvgMeasurement/m_ParamMgr->HSW_Num_Of_ScaleReading_Between_Fires; // avg
		fireallMeasurement[i] = fireallMeasurement[i] - fireallPrevMeasurement;
		WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head %s Fireall measurement %f ",
													GetHeadName(headID).c_str(),
													fireallMeasurement[i]));
		if( i==1 ) /*for now we are working with 2 cycles of fireall , for more iterations formula need to be changed.*/
		{
		   if(fireallMeasurement[i]< m_ParamMgr->HSW_Fire_All_Weight_Factor*fireallMeasurement[0])
		   {
			if(num_of_retries >0)
			{
			  WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Weight for last fire all is less than prev.weight*factor"));
			  /*purge and do same fireall again.*/
				m_BackEnd->GotoPurgePosition(/* blocking */ true, /* perform home */ true);
				m_BackEnd->Purge(true, true);
				Q2RTWizardSleep(m_ParamMgr->MotorPostPurgeTimeSec);
				m_BackEnd->WipeHeads(false);
				YieldWizardThread();
				m_BackEnd->WipeHeads(false);
				m_BackEnd->EnableMotor(true, AXIS_Y);
				m_BackEnd->EnableMotor(true, AXIS_X);
				TRY_FUNCTION(Err, m_BackEnd->MoveMotorToAbsolutePosition(AXIS_Y, m_ParamMgr->HSW_SCALE_Y_POSITION, NO_BLOCKING));
				TRY_FUNCTION(Err, m_BackEnd->MoveMotorToAbsolutePosition(AXIS_X, m_ParamMgr->HSW_SCALE_X_POSITION, NO_BLOCKING));

				TRY_FUNCTION(Err, m_BackEnd->WaitForEndOfMovement(AXIS_Y));
				TRY_FUNCTION(Err, m_BackEnd->WaitForEndOfMovement(AXIS_X));

				m_BackEnd->EnableMotor(false, AXIS_Y);
				m_BackEnd->EnableMotor(false, AXIS_X);
				/*do fireall again*/
			  i = -1;
			  num_of_retries --;
			  prevWeight = prevWeight+fireallMeasurement[0]+fireallMeasurement[1];
			  WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head %s prev. measurement before purge %f ",
												GetHeadName(headID).c_str(),
												prevWeight));
			}
			else // num of retries <=0
			{
				WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("After purges the weight still is incorrect"));
				weightTestError = WEIGHT_TEST_FAIL;
			}
		   }
		}  // second fire
	}  //fire all cycles
	//Check if Scale is stabilized TBD
	// wait for stabilize , why we need it???
	QSleep(m_ParamMgr->HSW_Sc_stab_time*1000);
	float avgMeasurement = 0;

	for(int i= 0;i<m_ParamMgr->HSW_Num_Of_ScaleReading_For_Measurement;i++)
	{
	YieldWizardThread();
	// read measurement from the scale
	tmpMeasurement = m_scale->GetImmWeight(errMsg);
	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head %s tmp measurement %f ",
												GetHeadName(headID).c_str(),
												tmpMeasurement));
	avgMeasurement += tmpMeasurement;
	if(errMsg!="")
		{
			errMsg.clear();
			// try again
			 avgMeasurement += m_scale->GetImmWeight(errMsg);
			 if(errMsg!="")
			{
				WriteToLogFile(LOG_TAG_GENERAL,QFormatStr(errMsg.c_str()));
				weightTestError = SCALE_COMMUNICATION_FAIL;
				return false;
			}
			WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head %s AVG measurement %f ",
												GetHeadName(headID).c_str(),
												avgMeasurement));
		}
	}
	measurement = avgMeasurement/m_ParamMgr->HSW_Num_Of_ScaleReading_For_Measurement; // avg
	measurement = measurement - prevWeight;
	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head %s measurement %f ",
											GetHeadName(headID).c_str(),measurement));
    }
	__finally
  {
	Q_SAFE_DELETE_ARRAY(fireallMeasurement);
  }
 return status;
}
/*-----------------------------------------------------------------------------*/
bool CHeadServiceWizard::PurgeBeforeSecondPatternTest(CWizardPage* WizardPage)
{
 bool ret = true;

   HeadsHeatingCycle();
    if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
   // Ignore 'Temperature' errors at the filling stage...
   m_BackEnd->SetReportTemperatureError(false);

   CBEMonitorActivator MonitorActivator; //meaning m_BackEnd->TurnHeadFilling(true);
   HeadsFillingCycle(m_ParamMgr->FillingTimeout);
   // Goto Purge Position
   ret = PurgeCicle(2);
   YieldWizardThread();
   m_BackEnd->WipeHeads(false);
   YieldWizardThread();
   m_BackEnd->WipeHeads(false);
   YieldWizardThread();
   if (!ret) return ret;
   if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
   YieldWizardThread();
   // Stop ignoring 'Temperature' errors at the filling stage...
   m_BackEnd->SetReportTemperatureError(true);
   CMachineSequencer *Sequencer = Q2RTApplication->GetMachineManager()->GetMachineSequencer();
   Sequencer->FireAllSequence();
   return true;
}
/*------------------------------------------------------------------------------*/
bool CHeadServiceWizard::PurgePageEnter(CWizardPage* WizardPage, int qm, int om, T_AxesTable a_AxesTable)
{
   bool ret = true;

   HeadsHeatingCycle();
   unsigned currTime       = QGetTicks();
   unsigned timeDelta = currTime - m_hswScaleBasedData->GetStartHeatingTime();
  // Update the curr time....
   if(timeDelta < QSecondsToTicks(m_ParamMgr->HSW_TempStabilizationTimeSec))
	QSleep(m_ParamMgr->HSW_TempStabilizationTimeSec - QTicksToSeconds(timeDelta)); //Wait TST time
   if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
   // Ignore 'Temperature' errors at the filling stage...
   m_BackEnd->SetReportTemperatureError(false);

   CBEMonitorActivator MonitorActivator; //meaning m_BackEnd->TurnHeadFilling(true);
   HeadsFillingCycle(m_ParamMgr->FillingTimeout);
   // Goto Purge Position
   ret = PurgeCicle(m_ParamMgr->HSW_NumOfPurges_AfterHeadReplacement);
   YieldWizardThread();
   m_BackEnd->WipeHeads(false);
   YieldWizardThread();
   m_BackEnd->WipeHeads(false);
   YieldWizardThread();
   if (!ret) return ret;
   if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
   //TRY_FUNCTION(Err, m_BackEnd->GotoPurgePosition(true, false));
   YieldWizardThread();
   // Stop ignoring 'Temperature' errors at the filling stage...
   m_BackEnd->SetReportTemperatureError(true);
   return true;
}
bool CHeadServiceWizard::TemplatePrintingPageEnter(CWizardPage* WizardPage)
{

    //In Objet260 we print test pattern before printing frame
	if( Q2RTApplication->GetMachineType() == mtObjet260 )
	{
		//turn on UV lamps
		if( UVLampIgnition() == false )
		{
			SetNextPage(qrUVIgnitionProblemPage);
			GotoNextPage();
			return false;
		}
		
		//print pattern test
		if(TestPatternPrintingPageEnter(WizardPage) == false )
		{
			m_BackEnd->TurnUVLamps(false);
			return false;
		}
		
	}

   bool status = true;
	CScopeRestorePoint<bool> MRW_WithinWizard(m_ParamMgr->MRW_WithinWizard);
	m_ParamMgr->MRW_WithinWizard = 0;
   //@TODO template printing
   EnableAllAxesAndHome();

  // CreateTemplateBMP();
   PrintTemplate();
   // Perform Purge
   if( Q2RTApplication->GetMachineType() == mtObjet260 )
	{
		// go to prev. mode ,because PrintTemplate changes mode
		EnterMode(m_hswScaleBasedData->GetCurrentQualityMode(),m_hswScaleBasedData->GetCurrentOperationMode());

		//Move Z down so we can place Scale in place
		MoveZtoScalePosition();
		SetNextPage(qrEnterMissingNozzles);
	}
	else
	{
		SetNextPage(qrTestPatternPreparation);
	}
	
   SetNextPage(qrTestPatternPreparation);
   GotoNextPage();
   return status;
}
bool CHeadServiceWizard::WizardCompletedWithoutHeadsReplacementPageEnter(CWizardPage* WizardPage)
{
	bool status = true;
	m_BackEnd->SetOnlineOffline(false);
	WriteToLogFile(LOG_TAG_GENERAL,"Head Optimisation wizard is completed");
	return status;
}
bool CHeadServiceWizard::UVScanning(CWizardPage* WizardPage)
{
 bool status = true;
	/*move X,Y to home position*/
	SendXYToPosition(0,0);
	/*1. lamp ignition */
	if(UVLampIgnition() == true)
	{
	m_BackEnd->EnableMotor(true, AXIS_Y);
	m_BackEnd->EnableMotor(true, AXIS_X);
	TQErrCode Err = Q_NO_ERROR;
	TRY_FUNCTION(Err, m_BackEnd->MoveMotorToAbsolutePosition(AXIS_Y, m_ParamMgr->HSW_SCALE_Y_POSITION, NO_BLOCKING));
	TRY_FUNCTION(Err, m_BackEnd->MoveMotorToAbsolutePosition(AXIS_X, m_ParamMgr->HSW_SCALE_X_POSITION, NO_BLOCKING));

	TRY_FUNCTION(Err, m_BackEnd->WaitForEndOfMovement(AXIS_Y));
	TRY_FUNCTION(Err, m_BackEnd->WaitForEndOfMovement(AXIS_X));
	// go over the scale
	int maxXPosition = CONFIG_GetPrintTrayXSize_step();
	for(int i = 0;i<m_ParamMgr->HSW_Scale_Num_Of_UV;i++)
	{
		TRY_FUNCTION(Err, m_BackEnd->MoveMotorToAbsolutePosition(AXIS_X, 0, NO_BLOCKING));
		TRY_FUNCTION(Err, m_BackEnd->WaitForEndOfMovement(AXIS_X));

		TRY_FUNCTION(Err, m_BackEnd->MoveMotorToAbsolutePosition(AXIS_X,maxXPosition, NO_BLOCKING));
		TRY_FUNCTION(Err, m_BackEnd->WaitForEndOfMovement(AXIS_X));
	}
	// turn off the lamp
	m_BackEnd->TurnUVLamps(false);
   // move X ,Y to 0 position
   SendXYToPosition(0,0);
   /*open door*/
   m_BackEnd->EnableDoor(false);
   
   SetNextPage(qrScaleRemoving);
   GotoNextPage();
	}
	else
	{
		status = false;
		SetNextPage(qrUVIgnitionProblemPage);
		GotoNextPage();
	}
   return status;
}

bool CHeadServiceWizard::ScaleRemovingPageLeave(CWizardPage* WizardPage)
{
  bool status = true;
  	/*2. Open the door,remove the scale*/
	bool doorLocked = false;
	bool scaleComunication = true;
	 if(m_BackEnd->CheckIfDoorIsClosed() == Q_NO_ERROR)
	 {
				if(m_BackEnd->EnableDoor(true,true) != Q_NO_ERROR)
				  doorLocked = false;
				else
				 doorLocked = true;
	 }

   // check scale communication
   QString errMsg = "";
   m_scale->CheckCommunication(errMsg);
   WriteToLogFile(LOG_TAG_GENERAL,errMsg.c_str());
   if(errMsg!="")
   {
	scaleComunication = false;
	m_hswScaleBasedData->SetScaleOnTray(false); // scale was removed
   }
   if(m_ParamMgr->HSW_SCALE_EMULATION == true)
	 scaleComunication = false; /*for emulation - scale is not on the tray*/

   if(scaleComunication == false && doorLocked == false)
	{
	 CCheckBoxWizardPage* CheckBoxPage = dynamic_cast<CCheckBoxWizardPage *>(Pages[qrScaleRemoving]);
	 CheckBoxPage->SetChecksMask(3);
	 WriteToLogFile(LOG_TAG_GENERAL,"Scale removed, door is open");
	 SetNextPage(qrScaleRemoving);
	}
	else if(scaleComunication == true && doorLocked == false)
	{
	 CCheckBoxWizardPage* CheckBoxPage = dynamic_cast<CCheckBoxWizardPage *>(Pages[qrScaleRemoving]);
	 CheckBoxPage->SetChecksMask(2);
	 WriteToLogFile(LOG_TAG_GENERAL,"Scale is not removed, door is open");
	 SetNextPage(qrScaleRemoving);
	}
	else if(scaleComunication == true && doorLocked == true)
	{
	 CCheckBoxWizardPage* CheckBoxPage = dynamic_cast<CCheckBoxWizardPage *>(Pages[qrScaleRemoving]);
	 CheckBoxPage->SetChecksMask(6);
	 WriteToLogFile(LOG_TAG_GENERAL,"Scale is not removed, door is not open");
	 SetNextPage(qrScaleRemoving);
	}
	else
	{
	   if(m_hswScaleBasedData->IsHeadReplacement() == true)
		{
		CCheckBoxWizardPage* CheckBoxPage = dynamic_cast<CCheckBoxWizardPage *>(Pages[qrTestPatternPreparation]);
		CheckBoxPage->SetChecksMask(0);
		CCheckBoxWizardPage* CheckBoxPageScale = dynamic_cast<CCheckBoxWizardPage *>(Pages[qrScaleSetup]);
		CheckBoxPageScale->SetChecksMask(0);
		SetNextPage(qrSelectContinuationOption);
		m_hswScaleBasedData->SetHeadReplacement(false);
		}
	   else
	   {
	    CCheckBoxWizardPage* CheckBoxPage = dynamic_cast<CCheckBoxWizardPage *>(Pages[qrScaleRemoving]);
		CheckBoxPage->SetChecksMask(0);
		SetNextPage(m_AskRunHeadAlignmentWizard ? qrWizardCompleted : qrWizardCompletedWithoutHeadsReplacement);
	   }
	}
  //   GotoNextPage();
	return status;
}
bool CHeadServiceWizard::PatternTestStressTest()
{
	TQErrCode Err = Q_NO_ERROR;
	//1.Go To purge position
	//2.set heads to HS temperature
	//3.fire for all heads
	//4. wait stress_test_off_time
	TRY_FUNCTION(Err, m_BackEnd->GotoPurgePosition(true, true));
	//Turn heating on
	HeadsHeatingCycle();
	//FireAll
	int headMask = 0; // all heads
	CMachineSequencer *Sequencer = Q2RTApplication->GetMachineManager()->GetMachineSequencer();
	int prevTimeOut = m_ParamMgr->FireAllTime_ms;
	int currTimeOut = (m_ParamMgr->HSW_Num_of_fires_StressTest/m_ParamMgr->HSW_FrequencyArray[0])*1000+500;
	WriteToLogFile(LOG_TAG_GENERAL,"FireAllFrequency = %d ",m_ParamMgr->HSW_FrequencyArray[0]);
	m_ParamMgr->FireAllTime_ms = currTimeOut;
	CElapsingTimeWizardPage* pPage= dynamic_cast< CElapsingTimeWizardPage *>(Pages[qrStressTest]);
	pPage->SubTitle = "";
	pPage->Refresh();
	for(int i=0;i< m_ParamMgr->HSW_Num_cicles_StressTest;i++)
	{
		QString title = "Test Stress cycle " + QIntToStr(i+1);
		pPage->SubTitle = title.c_str();
        pPage->Refresh();
		if(Sequencer->FireAllSequence(headMask,m_ParamMgr->HSW_Num_of_fires_StressTest/1000,0) != Q_NO_ERROR)
		{
		   WriteToLogFile(LOG_TAG_GENERAL,"Fail to FireAll for all heads");
		   return false;
		}
		QSleep(m_ParamMgr->HSW_StressTest_OffTime);
	}
	m_ParamMgr->FireAllTime_ms = prevTimeOut;
	m_hswScaleBasedData->m_stressTestDone = true;
    return true;
}
/********************************************************************/
void CHeadServiceWizard::CreateTemplateBMP()
/********************************************************************/
{
  /*Graphics::TBitmap *bmp = new Graphics::TBitmap;
  WriteToLogFile(LOG_TAG_GENERAL,"Creating template BMP.");
  int ScaleSize = m_ParamMgr->HSW_Scale_Size;
  int ScaleFrameBorderSize = 20;

  float a = DPI[CARPET_MODE_QUALITY_MODE]/ 2.54;
  bmp->PixelFormat = pf24bit ;
  bmp->Width  = ScaleSize * a+ (ScaleFrameBorderSize * 2);
  bmp->Height = ScaleSize * a+ (ScaleFrameBorderSize * 2);
  bmp->Monochrome  = true;
  bmp->PixelFormat = pf1bit;

  bmp->Canvas->Brush->Color = clWhite;
  bmp->Canvas->FloodFill(0, 0, clBlack, fsSurface);

  TRect Rect;
  Rect.Left   = ScaleFrameBorderSize;
  Rect.Right  = bmp->Width - ScaleFrameBorderSize ;
  Rect.Top    = ScaleFrameBorderSize;
  Rect.Bottom = bmp->Height-ScaleFrameBorderSize;
  bmp->Canvas->Brush->Color = clBlack;
  bmp->Canvas->FillRect(Rect);
 
  try
  {
	SaveToFile(bmp, TEMPLATE_FILE_NAME);
  }
  __finally
  {
	 Q_SAFE_DELETE(bmp);
  }*/
}
 /********************************************************************/
 // Check if all materials are legal
 /********************************************************************/
 bool CHeadServiceWizard::CheckIfMaterialsAreLegal()
{
	bool allAreLegal = true;  
	bool needToCheck = true;
	TTankIndex ActiveTankID 		= NO_TANK;
	QString msg ="The following material(s) cannot be used in the Head Optimization Wizard: ";
    	for(int i = 0; i < NUM_OF_ILLEGAL_MATERIALS; i++)
	{
		 for(int c = FIRST_CHAMBER_TYPE; c < LAST_CHAMBER_TYPE; c++)
		{
			 ActiveTankID = m_BackEnd->GetActiveTankNum((TChamberIndex)c);
			if(IsChamberFlooded(static_cast<TChamberIndex>(ActiveTankID)) == true) /*chamber that no need to be checked*/
			 {
				needToCheck = false;
			 }
			if((m_ParamMgr->HSW_IllegalMaterialArray[i] == m_ParamMgr->TypesArrayPerChamber[c]) && (needToCheck == true))
		   {
				if(msg.find(m_ParamMgr->TypesArrayPerChamber[c]) == QString::npos)
				{
					msg+= QString(m_ParamMgr->TypesArrayPerChamber[c]) + ",";
					allAreLegal = false;
				}
		   }
		}
	}

	if(allAreLegal == false)
	{
		CMessageWizardPage *Page = dynamic_cast<CMessageWizardPage *>(GetPageById(qrIllegalMaterialForHSW)); // error msg and quit wizard.
		msg += "\n\nCancel this wizard, and replace the material(s) using the Material Replacement Wizard.\n\nAfterwards, continue with Head Optimization." ;
		Page->SubTitle = msg;
	}
  return allAreLegal;
}

bool CHeadServiceWizard::UVLampIgnition()
{
  unsigned EndTime;

  // Entering 'High Quality' Mode
  m_BackEnd->EnterMode(PRINT_MODE[0][0],MACHINE_QUALITY_MODES_DIR(0,0));

  // Setting PowerSupply params according to the mode
   FOR_ALL_UV_LAMPS(l)
      m_BackEnd->SetUVD2AValue(l, m_ParamMgr->UVLampPSValueArray[l]);

  // Turn UV lamps on
  m_BackEnd->SetDefaultParmIgnitionTimeout();
  m_BackEnd->TurnUVLamps(true);

  EndTime = QGetTicks() + QSecondsToTicks(m_ParamMgr->UVLampIgnitionTime);
  // Wait for UV lamps to turn on
  while (IsCancelled() != true)
  {
    if (QGetTicks() > EndTime)
      return false;

    if (m_BackEnd->AreUVLampsTurnedOn())
      break;

    YieldAndSleepWizardThread();
    QSleep(100);
  }

  if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
  EndTime = QGetTicks() + QSecondsToTicks(m_ParamMgr->UVCalibrationPostIgnitionTime);

  // Wait for UV lamps post ignition time
  while (IsCancelled() != true)
  {
    if (QGetTicks() > EndTime)
      break;

    YieldAndSleepWizardThread();
	QSleep(100);
  }
  return true;
}

void CHeadServiceWizard::TestPatternPreparationPageEnter(CWizardPage* WizardPage)
{
   m_BackEnd->LockDoor(false);
  
}
void CHeadServiceWizard::SecondPatternTestReqPageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason)
{
  // check door
  if(m_BackEnd->CheckIfDoorIsClosed() == Q_NO_ERROR)
	 {
				if(m_BackEnd->EnableDoor(true,true) != Q_NO_ERROR)
				{
				  CCheckBoxWizardPage* CheckBoxPage = dynamic_cast<CCheckBoxWizardPage *>(Pages[qrSecondPatternTest]);
				  CheckBoxPage->SetChecksMask(0);
				  SetNextPage(qrSecondPatternTest);
				}
				else
				{
				  CCheckBoxWizardPage* CheckBoxPage = dynamic_cast<CCheckBoxWizardPage *>(Pages[qrSecondPatternTest]);
				  CheckBoxPage->SetChecksMask(0);
				  EnableAllAxesAndHome();
				  // purge + 2 wipes + fireall
				  PurgeBeforeSecondPatternTest(WizardPage);
				  SetNextPage(qrTestPatternPreparation);
				}
	 }

	 else
	 {
	   CCheckBoxWizardPage* CheckBoxPage = dynamic_cast<CCheckBoxWizardPage *>(Pages[qrSecondPatternTest]);
	   CheckBoxPage->SetChecksMask(0);
	   SetNextPage(qrSecondPatternTest);
	 }
}
void CHeadServiceWizard::TestPatternPreparationPageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason)
/********************************************************************/
{
   //1. Check if Door is closed
   if(m_BackEnd->CheckIfDoorIsClosed() == Q_NO_ERROR)
	 {
				if(m_BackEnd->EnableDoor(true,true) != Q_NO_ERROR)
				{
				  CCheckBoxWizardPage* CheckBoxPage = dynamic_cast<CCheckBoxWizardPage *>(Pages[qrTestPatternPreparation]);
				  CheckBoxPage->SetChecksMask(1);
				  SetNextPage(qrTestPatternPreparation);
				}
				else
				{
				  CCheckBoxWizardPage* CheckBoxPage = dynamic_cast<CCheckBoxWizardPage *>(Pages[qrTestPatternPreparation]);
				  CheckBoxPage->SetChecksMask(0);
				  EnableAllAxesAndHome();
				  SetNextPage(qrTestPatternPrinting);
				}
	 }

	 else
	 {
	   CCheckBoxWizardPage* CheckBoxPage = dynamic_cast<CCheckBoxWizardPage *>(Pages[qrTestPatternPreparation]);
	   CheckBoxPage->SetChecksMask(1);
	   SetNextPage(qrTestPatternPreparation);
	 }
}
bool CHeadServiceWizard::RemoveResinFromContainerPageEnter(CWizardPage* WizardPage)
{
  TQErrCode Err;
  bool status = true;
  if(m_BackEnd->CheckIfDoorIsClosed() == Q_NO_ERROR)
  {
	if(m_BackEnd->EnableDoor(true,true) == Q_NO_ERROR)
	{
	EnableAxeAndHome(AXIS_T, true, true);
	EnableAxeAndHome(AXIS_X, true, true);
	EnableAxeAndHome(AXIS_Y, true, true);
	/*move to special position */
	long position =  m_ParamMgr->MaxPositionStep[AXIS_X];
	SendXYToPosition(position,0); /*x = maxx position, y=0*/
   	/*start waste*/
	m_BackEnd->MoveMotorToTPurgePosition();
	}
   }
  /*open door*/
  m_BackEnd->EnableDoor(false);
  return status;
}
void CHeadServiceWizard::PutResinToPurgeUnitPageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason)
{
  /*check that door is opened*/
  bool doorLocked = false;
  if(m_BackEnd->CheckIfDoorIsClosed() == Q_NO_ERROR)
  {
	if(m_BackEnd->EnableDoor(true,true) != Q_NO_ERROR)
		doorLocked = false;
	else
		doorLocked = true;
  }
  
   if(doorLocked == false) /*close the door and check again*/
   {
	CCheckBoxWizardPage* CheckBoxPage = dynamic_cast<CCheckBoxWizardPage *>(Pages[qrPutResinToPurgeUnit]);
	CheckBoxPage->SetChecksMask(3);
	SetNextPage(qrPutResinToPurgeUnit);
   }
   else /*door is closed, got to UV*/
   {
	  /*close T axes*/
	  /*home X,Y */
	  EnableAxeAndHome(AXIS_T, true, true);
	  EnableAxeAndHome(AXIS_Y, true, true);
	  EnableAxeAndHome(AXIS_X, true, true);
	  m_BackEnd->WaitForEndOfMovement(AXIS_T);
	  m_BackEnd->WaitForEndOfMovement(AXIS_Y);
	  m_BackEnd->WaitForEndOfMovement(AXIS_X);
	  CCheckBoxWizardPage* CheckBoxPage = dynamic_cast<CCheckBoxWizardPage *>(Pages[qrPutResinToPurgeUnit]);
	  CheckBoxPage->SetChecksMask(0);
	  SetNextPage(qrUVScanning);
   }
}
void CHeadServiceWizard::VerifyHeadCleaningPageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason)
{
	bool contWithHSW = true;
	CRadioGroupWizardPage *Page = dynamic_cast<CRadioGroupWizardPage *>(WizardPage);
	if (Page->GetSelectedOption() == contWithHSW)
	{
		WriteToLogFile(LOG_TAG_GENERAL,"Heads are clean, go to HSW");
		if( (Q2RTApplication->GetMachineType()) != (mtObjet1000 ))
		{

			SetNextPage(qrSelectWizardMode);
		}
		else
			SetNextPage(qrInsertTrayOptimizationMode);
	}
	else     //Optimize mode
	{
		WriteToLogFile(LOG_TAG_GENERAL,"Heads are not clean, cancel was selected");
      	 throw EQException(m_ContinuationOptionPageSubtitle);
	}

}
void CHeadServiceWizard::TargetWeightNotReachedPageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason)
{
	m_hswScaleBasedData->SetHeadReplacement(false);
	SetNextPage(qrCloseDoorResinToPurgeUnit);
}
void CHeadServiceWizard::VoltageProblemPageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason)
{
   	m_hswScaleBasedData->SetHeadReplacement(false);
	SetNextPage(qrCloseDoorResinToPurgeUnit);
}

void CHeadServiceWizard::SendXYToPosition(int x_position,int y_position)
{
	TQErrCode Err = Q_NO_ERROR;
	m_BackEnd->EnableMotor(true, AXIS_X);
	m_BackEnd->EnableMotor(true, AXIS_Y);

	TRY_FUNCTION(Err, m_BackEnd->MoveMotorToAbsolutePosition(AXIS_X,x_position, NO_BLOCKING));
	TRY_FUNCTION(Err, m_BackEnd->WaitForEndOfMovement(AXIS_X));
	TRY_FUNCTION(Err, m_BackEnd->MoveMotorToAbsolutePosition(AXIS_Y,y_position, NO_BLOCKING));
	TRY_FUNCTION(Err, m_BackEnd->WaitForEndOfMovement(AXIS_Y));

	m_BackEnd->EnableMotor(false, AXIS_X);
	m_BackEnd->EnableMotor(false, AXIS_Y);
}
bool CHeadServiceWizard::IsRestartRequired()
{
  return m_bNeedRestartAfterParamsChanged;
}
void CHeadServiceWizard::MoveZtoScalePosition()
{
    m_BackEnd->LockDoor(true);
	TQErrCode Err = Q_NO_ERROR;
	m_BackEnd->EnableMotor(true, AXIS_Z);
	TRY_FUNCTION(Err, m_BackEnd->MoveMotorToAbsolutePosition(AXIS_Z, m_ParamMgr->HSW_SCALE_Z_POSITION, NO_BLOCKING));
	TRY_FUNCTION(Err, m_BackEnd->WaitForEndOfMovement(AXIS_Z, Z_DOWN_WAIT_TIME));
	m_BackEnd->EnableMotor(false, AXIS_Z);
	m_BackEnd->SetHoodFanParamOnOff(true,m_ParamMgr->HSW_OdourFanVoltage);
	m_BackEnd->LockDoor(false);
}
