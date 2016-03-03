//---------------------------------------------------------------------------
//Assumptions are marked with [@Assumption]; Comments are marked with either
//'ATTENTION', 'Note', 'Comment' or 'Important' keywords.
//---------------------------------------------------------------------------

#include "QUtils.h" 	//free alloc. mem macro
#include "ResinReplacementPipeline.h"
#include "GlobalDefs.h"
#include "Limits.h"
#include "Q2RTApplication.h"
#include "BackEndInterface.h"
#include "QScopeRestorePoint.h"

#include "QMonitor.h"
#include "AppLogFile.h"

//---------------------------------------------------------------------------
#define INVALID_FLOODING_STATE -1
#define MAX(a,b) ((a)>(b) ? (a) : (b))
#define FLOODING_NORMALIZATION_FACTOR(condAreSmallCycles, paramsMgrObj) ((condAreSmallCycles) ? (paramsMgrObj)->MRW_LowToHighThermistorsVolumesRatio : 1.0)
#define DEL_VECTOR_POINTERS(ClassName, VectorVariable) { \
	if (!VectorVariable.empty())\
	for (vector<ClassName*>::iterator it = VectorVariable.begin(); it != VectorVariable.end(); ++it) \
	{ Q_SAFE_DELETE(*it); } \
	VectorVariable.clear(); } //clear() only de-allocates memory for objects it needs not to de-reference (i.e. by-value objects, not pointers)

QString RESIN_TOCYC_MATRIX_FILENAME = LOAD_STRING(IDS_RESIN_MATRIX_FILE_NAME);

using namespace MrwPipeline;

//---------------------------------------------------------------------------
// General Functions not to be published and not related to a specific class
//---------------------------------------------------------------------------
TTankIndex GetPermTankIndex(TChambersLocation permChamberIdx, TReplacementCartridges permContainerIdx);
TSegmentIndex GetSegmentIndex(TChamberIndex chamber, TReplacementCartridges tankSide);
TSegmentIndex GetSegmentIndex(TChamberIndex chamber);

//Preconfiguring the possible permutations, for performance.
//Used the following permutations generator (For Keshet, we have 8!):
//Permutations generator: http://users.telenet.be/vdmoortel/dirk/Maths/permutations.html
const unsigned int s_factorial[NUMBER_OF_CHAMBERS_LOCATIONS+1] =
	{0,0,0,0,PERM_FACTORIAL_DM3_1_SUPPORT,PERM_FACTORIAL_DM3_2_SUPPORTS,0,
		PERM_FACTORIAL_MAX_MODELS_1_SUPPORT,PERM_FACTORIAL_MAX_MODELS_2_SUPPORTS};
const unsigned int s_perms_3mdl_1support[PERM_FACTORIAL_DM3_1_SUPPORT][NUMBER_OF_CHAMBERS_LOCATIONS] =
{
//The Support resin(s) can either be the 1st one inserted via
//CResinReplacementPipeline::AddTargetMaterial(), or the last one (ie the last model) --
//Meaning it's either indexed 0 (1st in the tuple) or 3 (last in the tuple). 
	{0,1,2,3,}, {0,1,3,2,}, {0,2,1,3,}, {0,2,3,1,}, {0,3,1,2,},
	{0,3,2,1,},	{1,0,2,3,}, {1,2,0,3,}, {2,0,1,3,}, {2,1,0,3,},
	{1,2,3,0},	{1,3,2,0,}, {2,1,3,0,}, {2,3,1,0,}, {3,1,2,0,}, {3,2,1,0,}
};
const unsigned int s_perms_3mdl_fullSupport[PERM_FACTORIAL_DM3_2_SUPPORTS][NUMBER_OF_CHAMBERS_LOCATIONS] =
{
	{0,1,2,3,4,}, {0,1,3,2,4,}, {0,2,1,3,4,}, {0,2,3,1,4,}, {0,3,1,2,4,}, {0,3,2,1,4,}, 
	{4,1,2,3,0,}, {4,1,3,2,0,}, {4,2,1,3,0,}, {4,2,3,1,0,}, {4,3,1,2,0,}, {4,3,2,1,0,}
};
#define _MRW_PIPELINE_PERMUTATIONS_LIST_DM6_SingleSupp_
const unsigned int s_perms_max_mdl_1support[PERM_FACTORIAL_MAX_MODELS_1_SUPPORT][NUMBER_OF_CHAMBERS_LOCATIONS] =
{ //6 model resins ; a single support chamber (2 possible options)
	#include "ResinReplacementPipelinePermutations.h"
};
#undef _MRW_PIPELINE_PERMUTATIONS_LIST_DM6_SingleSupp_
#define _MRW_PIPELINE_PERMUTATIONS_LIST_DM6_2S_
const unsigned int s_perms_max_mdl_fullSupport[PERM_FACTORIAL_MAX_MODELS_2_SUPPORTS][NUMBER_OF_CHAMBERS_LOCATIONS] =
{ //6 model resins ; two interchangable support chambers
	#include "ResinReplacementPipelinePermutations.h"
};
#undef _MRW_PIPELINE_PERMUTATIONS_LIST_DM6_2S_

/* The following code is suited *only* for 1 (fixed) support chamber
=====================================================================
const unsigned int s_factorial[NUMBER_OF_CHAMBERS_LOCATIONS+1] =
	{0,0,0,0,NUM_OF_CHAMBERS_FACTORIAL_DM3,0,0,1,NUM_OF_CHAMBERS_FACTORIAL_MAX};
const unsigned int s_perms_3mdl[NUM_OF_CHAMBERS_FACTORIAL_DM3][NUMBER_OF_CHAMBERS_LOCATIONS] =
{
	{3,2,1,}, {3,1,2,}, {2,3,1,}, {1,3,2,}, {2,1,3,}, {1,2,3,}
};
#define _MRW_PIPELINE_PERMUTATIONS_LIST_
const unsigned int s_perms_max_mdl[NUM_OF_CHAMBERS_FACTORIAL_MAX][NUMBER_OF_CHAMBERS_LOCATIONS] =
{ //7 model resins
	#include "ResinReplacementPipelinePermutations.h"
};
#undef _MRW_PIPELINE_PERMUTATIONS_LIST_
================================================================  */

unsigned int GetPermElement(unsigned int permLevel, unsigned int permID, unsigned int permElem)
{
	// check boundaries:
	if (permLevel > NUMBER_OF_CHAMBERS_LOCATIONS)
		throw EQException("GetPermElement : bad argument #1");
	if (permID > s_factorial[permLevel])
		throw EQException("GetPermElement : bad argument #2");
	if (permElem >= permLevel)
		throw EQException("GetPermElement : bad argument #3");

/* The following code is suited *only* for 1 support chamber
================================================================
	//performance: no need to enumerate permutations with support chamber - it can be ignored and fixed on 1st (left most) chamber
	if (0 == permElem) return 0;

	//Performance: permElem-1 is used to compensate on the missing support chamber in the permutations lists
	if (4 == permLevel)
		return s_perms_3mdl[permID][permElem-1];
	else if (NUMBER_OF_CHAMBERS_LOCATIONS == permLevel)
		return s_perms_max_mdl[permID][permElem-1];
	else {
		CQLog::Write(LOG_TAG_GENERAL,"Error: GetPermElement : no permutations for level %d", permLevel);
		throw EQException("GetPermElement : bad argument #1 - no permutations");
	}		
================================================================ */
	switch (permLevel) {
	   case 4:
			return s_perms_3mdl_1support[permID][permElem];
	   case 5:
			return s_perms_3mdl_fullSupport[permID][permElem];
	   case (NUMBER_OF_CHAMBERS_LOCATIONS-1):
			return s_perms_max_mdl_1support[permID][permElem]; //"DM6" w/ 1 support
	   case NUMBER_OF_CHAMBERS_LOCATIONS:
			return s_perms_max_mdl_fullSupport[permID][permElem]; //"DM6" w/ 2 support -OR- "DM7"
	   default:
			CQLog::Write(LOG_TAG_GENERAL,"Error: GetPermElement : no permutations for level %d", permLevel);
			throw EQException("GetPermElement : bad argument #1 - no permutations");
	}
//================================================================
}
unsigned int GetPermsNumAtLevel(unsigned int level)
{
	return s_factorial[level];
}

TTankIndex GetPermTankIndex(TChambersLocation permChamber, TReplacementCartridges permContainer)
{
   //[@Assumption] Left containers have even indexes, where Right ones have odd indexes.
   int offsetContainer = (LEFT_CART == permContainer) ? 0 : 1;

   if (DEDICATED_SUPPORT_CHAMBER == permChamber)
	  return static_cast<TTankIndex>(
		(int)(FIRST_SUPPORT_TANK_TYPE) + offsetContainer);
   else if ((DONT_CARE_INDEX == permChamber) || (permChamber >= NUMBER_OF_CHAMBERS_LOCATIONS))
	  return NO_TANK;//error with argument
   else
   {
	  return static_cast<TTankIndex>(
		 (int)(FIRST_MODEL_TANK_TYPE) +
		 ((int)((int)permChamber - FIRST_MODEL_CHAMBER_LOCATION)<< 1) + offsetContainer
	  );
   }
}

TSegmentIndex GetSegmentIndex(TChamberIndex chamber)
{
   switch (chamber)
   {
		case TYPE_CHAMBER_SUPPORT:
			return FIRST_SUPPORT_SEGMENT;
		case TYPE_CHAMBER_MODEL1:
			return M1_2_SEGMENT;
		case TYPE_CHAMBER_MODEL2:
			return M3_4_SEGMENT;
		case TYPE_CHAMBER_MODEL3:
			return M5_6_SEGMENT;
#ifdef OBJET_MACHINE_KESHET
		case TYPE_CHAMBER_MODEL4:
			return M7_8_SEGMENT;
		case TYPE_CHAMBER_MODEL5:
			return M9_10_SEGMENT;
		case TYPE_CHAMBER_MODEL6:
			return M11_12_SEGMENT;
		case TYPE_CHAMBER_MODEL7:
			return M13_14_SEGMENT;
#endif
		default:
			CQLog::Write(LOG_TAG_GENERAL,"Error: Couldn't get segment. Chamber index is out of range");
			throw EQException("GetSegmentIndex : bad argument");
   }
}
TSegmentIndex GetSegmentIndex(TChamberIndex chamber, TReplacementCartridges tankSide)
{
   int offsetContainer = (LEFT_CART == tankSide) ? 0 : 1;

   return static_cast<TSegmentIndex>(
	   (int)FIRST_SUPPORT_SEGMENT +
	   (((int)chamber - FIRST_CHAMBER_TYPE) << 1) + offsetContainer);
}

//Count number of set bits in a bit array of a 32-bit int
//http://en.wikipedia.org/wiki/Hamming_weight
/*int CountSetBits32Bit(unsigned int i)
{
	i = i - ((i >> 1) & 0x55555555);
	i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
	return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}*/
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

__fastcall CNamedPipingChamber::CNamedPipingChamber(QString resinInChamber, QString longPipeResin,
						 QString shortRightPipeResin, QString shortLeftPipeResin)
: m_resinName(resinInChamber), m_longPipe(longPipeResin),
  m_shortLeftPipe(shortRightPipeResin), m_shortRightPipe(shortLeftPipeResin)
{}

bool CNamedPipingChamber::IsFlooded()
{	//if the resin in the chamber doesn't match the one in the piping - it's flooded.
	return (m_resinName.c_str() != m_longPipe.c_str());
}

//---------------------------------------------------------------------------

__fastcall CResinReplacementPipeline::CResinReplacementPipeline(CQ2RTAutoWizard *Wizard)
: m_numPermutations(0), m_numBasicConfigs(0), m_optimalPermutation(-1), m_numTargetMaterials(0),
  m_silentMode(false)
{
	m_pipelineResult = NULL;
	m_tblCycMatrix   = NULL;
	m_undefinedStr = LOAD_STRING(IDS_MRW_UNDEFINED);
	m_OwnerWizard  = Wizard;

	m_ParamsMgr = CAppParams::Instance();
	m_BackEnd  = CBackEndInterface::Instance();
	Initialize();
}

void CResinReplacementPipeline::InitializeMatrix()
{
	Q_SAFE_DELETE(m_tblCycMatrix);
	try {
		m_tblCycMatrix = new CMaterialsMatrix();
	} catch (EQException& e) {
		WriteToLog("Error: " + e.GetErrorMsg());
		throw e;
	}

	try {
		AnsiString matrixPath = AnsiString(
			(Q2RTApplication->AppFilePath.Value() + LOAD_STRING(IDS_CONFIGS_DIRNAME)
							+ RESIN_TOCYC_MATRIX_FILENAME).c_str());
		m_tblCycMatrix->Init(matrixPath);
	} catch (EQException& e) {
		WriteToLog("Error: MRW's resin matrix wasn't found or is corrupted.");
		throw EQException("MRW's resin matrix wasn't found or is corrupted.");
	} catch (...) {
		WriteToLog("Error: MRW's resin matrix wasn't found or is corrupted.");
		throw EQException("MRW's resin matrix wasn't found or is corrupted.");
	}


}

void CResinReplacementPipeline::GetCurrentPiping()
{
	DEL_VECTOR_POINTERS(CNamedPipingChamber, m_currState);
	
	//fetch the current machine's state from the piping
	//[@Assumption] Only 1 dedicated support chamber
	//Support Chamber(s)
	m_currState.push_back(new CNamedPipingChamber(
		m_ParamsMgr->TypesArrayPerChamber[TYPE_CHAMBER_SUPPORT],      //chamber
		m_ParamsMgr->TypesArrayPerChamber[TYPE_CHAMBER_SUPPORT], //"long pipe" is taken from block's resin, because no chamber floods the support chamber
		m_ParamsMgr->MRW_TypesArrayPerSegment[FIRST_SUPPORT_SEGMENT], //short,left
		m_ParamsMgr->MRW_TypesArrayPerSegment[LAST_SUPPORT_SEGMENT]   //short,right
	));

	//Model Chambers
	//[@Assumption] 2 tanks per chamber
	int nextLeftTankOfChamber = 1;
	for (int i = 1; i < NUMBER_OF_CHAMBERS; ++i) {
		m_currState.push_back(new CNamedPipingChamber(
			m_ParamsMgr->TypesArrayPerChamber[FIRST_MODEL_CHAMBER_INDEX + i-1],
			m_ParamsMgr->MRW_TypesArrayPerSegment[FIRST_SHARED_SEGMENT + i-1],
			m_ParamsMgr->MRW_TypesArrayPerSegment[LAST_SUPPORT_SEGMENT + nextLeftTankOfChamber  ],
			m_ParamsMgr->MRW_TypesArrayPerSegment[LAST_SUPPORT_SEGMENT + nextLeftTankOfChamber+1]
		));
		nextLeftTankOfChamber += 2; //translating a chamber index to left-tank index
		//TODO: prefer using: GetSpecificTank(static_cast<TChamberIndex>(c), 1); to allow flexibility with number of tanks per chamber
	}
}

void CResinReplacementPipeline::Initialize()
{
	Q_SAFE_DELETE(m_pipelineResult);
	m_pipelineResult = new COptimalTanks();
	InitializeMatrix();
	ClearResults();
	GetCurrentPiping();
}


__fastcall CResinReplacementPipeline::~CResinReplacementPipeline()
{
	 ClearResults();

	DEL_VECTOR_POINTERS(CNamedPipingChamber, m_currState);

	Q_SAFE_DELETE(m_tblCycMatrix);
	Q_SAFE_DELETE(m_pipelineResult);
}

void CResinReplacementPipeline::AddTargetMaterial(TVirtTargetMaterial& demand)
{
	//limit vector to size NUMBER_OF_CHAMBERS. objects positions are crucial here
	if (NUMBER_OF_CHAMBERS <= m_numTargetMaterials)
		throw EQException("MRW Pipeline : number of target material exceeded.");

	//Override any potential bug related to the support's flushing purpose.
	//This is done together with the Parameter Manager's MRW_IsSegmentEconomy
	//parameter change (to TINT) in the main MRW module (CResinReplacementWizard::CalculateEconomySegments).
	if (demand.IsSupport())
		demand.FlushingPurpose = TINT_CYCLES;

	m_targetMaterials.push_back(demand);
	++m_numTargetMaterials;
}

void CResinReplacementPipeline::GenerateBasicConfigs()
{
/* Defines what chambers are active in the process of deciding where to put
 * the new required resins during M.R.; Note that false doesn't always say "flooded",
 * but may sometimes mean only "inactive".
 * This data presentation is easier to understand than using hex and bitwise ops to
 * determine if a chamber is true/false.
 * Order of chambers (according to .h file), left to right: Support, M1, M2, M3, ...
 */
	switch(m_targetReplacementMode)
	{
		case mrwDM3:
			m_numBasicConfigs = 8; //extra Support chamber // originally (single Support chamber): 8;
//			m_basicConfigs.push_back(new CBlockConfiguration(Chamber2TankOperationModeRelation[(int)S_M1_M3_M5_ACTIVE_TANKS_MODE]));
//			m_basicConfigs.push_back(new CBlockConfiguration(Chamber2TankOperationModeRelation[(int)S_M1_M3_M6_ACTIVE_TANKS_MODE]));
//			m_basicConfigs.push_back(new CBlockConfiguration(Chamber2TankOperationModeRelation[(int)S_M1_M4_M5_ACTIVE_TANKS_MODE]));
//			m_basicConfigs.push_back(new CBlockConfiguration(Chamber2TankOperationModeRelation[(int)S_M1_M4_M6_ACTIVE_TANKS_MODE]));
//			m_basicConfigs.push_back(new CBlockConfiguration(Chamber2TankOperationModeRelation[(int)S_M2_M3_M5_ACTIVE_TANKS_MODE]));
//			m_basicConfigs.push_back(new CBlockConfiguration(Chamber2TankOperationModeRelation[(int)S_M2_M3_M6_ACTIVE_TANKS_MODE]));
//			m_basicConfigs.push_back(new CBlockConfiguration(Chamber2TankOperationModeRelation[(int)S_M2_M4_M5_ACTIVE_TANKS_MODE]));
//			m_basicConfigs.push_back(new CBlockConfiguration(Chamber2TankOperationModeRelation[(int)S_M2_M4_M6_ACTIVE_TANKS_MODE]));
			{
				const unsigned extraNumModes = (NUM_OF_TANKS_OPERATION_MODES-1);// << 1;
				//Add support for an extra Support chamber ("DM6" feature)
				const bool ExtraSupportRelation[extraNumModes][NUMBER_OF_CHAMBERS] =
				{//  S 1 2 3 4 5 6 7
//					{0,1,0,1,0,1,0,1}, //S_M1_M3_M5_ACTIVE_TANKS_MODE + M7 - S
//					{0,1,0,1,0,0,1,1}, //S_M1_M3_M6_ACTIVE_TANKS_MODE + M7 - S
//					{0,1,0,0,1,1,0,1}, //S_M1_M4_M5_ACTIVE_TANKS_MODE + M7 - S
//					{0,1,0,0,1,0,1,1}, //S_M1_M4_M6_ACTIVE_TANKS_MODE + M7 - S
//					{0,0,1,1,0,1,0,1}, //S_M2_M3_M5_ACTIVE_TANKS_MODE + M7 - S
//					{0,0,1,1,0,0,1,1}, //S_M2_M3_M6_ACTIVE_TANKS_MODE + M7 - S
//					{0,0,1,0,1,1,0,1}, //S_M2_M4_M5_ACTIVE_TANKS_MODE + M7 - S
//					{0,0,1,0,1,0,1,1}, //S_M2_M4_M6_ACTIVE_TANKS_MODE + M7 - S

					{1,1,0,1,0,1,0,1}, //S_M1_M3_M5_ACTIVE_TANKS_MODE + M7
					{1,1,0,1,0,0,1,1}, //S_M1_M3_M6_ACTIVE_TANKS_MODE + M7
					{1,1,0,0,1,1,0,1}, //S_M1_M4_M5_ACTIVE_TANKS_MODE + M7
					{1,1,0,0,1,0,1,1}, //S_M1_M4_M6_ACTIVE_TANKS_MODE + M7
					{1,0,1,1,0,1,0,1}, //S_M2_M3_M5_ACTIVE_TANKS_MODE + M7
					{1,0,1,1,0,0,1,1}, //S_M2_M3_M6_ACTIVE_TANKS_MODE + M7
					{1,0,1,0,1,1,0,1}, //S_M2_M4_M5_ACTIVE_TANKS_MODE + M7
					{1,0,1,0,1,0,1,1}, //S_M2_M4_M6_ACTIVE_TANKS_MODE + M7
				};
				for (unsigned cfg = 0; cfg < extraNumModes; ++cfg) {
					m_basicConfigs.push_back(new CBlockConfiguration(ExtraSupportRelation[cfg]));
				}
			}
			break;

		case mrwDM6:
			m_numBasicConfigs = 1;
			{
			//Simulate all kinds of flooding attempts with in the support chambers
			//bool const floodFromSecondarySupport[NUMBER_OF_CHAMBERS] = {1,1,1,1,1,1,1,0};
			//bool const floodFromPrimarySupport	[NUMBER_OF_CHAMBERS] = {0,1,1,1,1,1,1,1};
			//m_basicConfigs.push_back(new CBlockConfiguration(floodFromSecondarySupport));
			//m_basicConfigs.push_back(new CBlockConfiguration(floodFromPrimarySupport));
			//Use both support chambers (for redundancy)
			m_basicConfigs.push_back(new CBlockConfiguration(
				Chamber2TankOperationModeRelation[(int)S_M1_M7_ACTIVE_TANKS_MODE]));
			}
			break;

		case mrwDM7:
			m_numBasicConfigs = 1;
			m_basicConfigs.push_back(
				new CBlockConfiguration(Chamber2TankOperationModeRelation[(int)S_M1_M7_ACTIVE_TANKS_MODE]));
			break;
		default:
			QMonitor.Print("Configs pipeline must define a valid replacement mode first.");
			throw EQException("MRW Pipeline : Configs pipeline must define a valid replacement mode first.");
	}
}

void CResinReplacementPipeline::ForceSystemMaterialsConstraints()
{
	//This method is irrelevant if we have an EXTRA Support chamber
	if ((mrwDM6 == m_targetReplacementMode) || (mrwDM3 == m_targetReplacementMode))
		return;

	/* [@Deprecated] Already done via ResinReplacementWizard. The Support resin is being sent along with the Model resins
		//Create a fixed system target-material
		TVirtTargetMaterial 	m_fixedSupportTargetMaterial;
		m_fixedSupportTargetMaterial.SelectedResin = SUPPORT_MATERIAL_NAME; //SUPPORT_MATERIAL_NAME==GetDefaultSupportMaterial()
		m_fixedSupportTargetMaterial.FixedLocation = DEDICATED_SUPPORT_CHAMBER;
		m_fixedSupportTargetMaterial.FlushingBothCartridges =
			IsSystemAfterShutdown() ? BOTH_CART : SINGLE_CART; //IsSystemAfterShutdown checks SL&SR for the default Cleanser resin

		//Add a new Target Material to fixate Support on the left most chamber.
		//[@Assumption] #Support (dedicated) chambers (aka MAX_NUMBER_OF_SUPPORT_CHAMBERS) = 1
		AddTargetMaterial(DEDICATED_SUPPORT_CHAMBER, m_fixedSupportTargetMaterial);
	*/

	//Make sure all the configs define the support chamber as active.
	TConfigArray::const_iterator it = m_basicConfigs.begin();
	for (; it != m_basicConfigs.end(); ++it)
	{
		//ATTENTION! if we change a chamber's activation,
		// we MUST call SetFloodingChamberIfActive() on each basic config,
		// to fix their current flooding state. Activate() will also perform this task.
		(*it)->Activate(DEDICATED_SUPPORT_CHAMBER, true);
	}
}

void CResinReplacementPipeline::ClearResults()
{
	m_targetReplacementMode = mrwUndefined;

	DEL_VECTOR_POINTERS(CBlockConfiguration, m_basicConfigs);
	m_numBasicConfigs = 0;

	DEL_VECTOR_POINTERS(CRankedBlock, m_permutations);
	m_numPermutations = 0;
	m_optimalPermutation = -1;
	m_pipelineResult->Clear();

	m_targetMaterials.clear();
	m_numTargetMaterials = 0;
}

void CResinReplacementPipeline::GenerateConstrainedConfigsAndRank()
{
	assert(m_tblCycMatrix);

	// Iterate through the basic configurations
	for (int basicConf = 0; basicConf < m_numBasicConfigs; ++basicConf)
	{
		//Skip basic configurations that do not hold enough place for all the desired resins, including their redundancy
		int activeChambers = m_basicConfigs[basicConf]->GetNumActiveChambers();
		if (m_numTargetMaterials != activeChambers)
			continue;

		int permsNum = GetPermsNumAtLevel(activeChambers);

		if (0 == permsNum) { //Double-checking the provided internal argument
			WriteToLog(QFormatStr("Error: No permutations are available (%d)",
				m_basicConfigs[basicConf]->GetNumActiveChambers()));
		}

		// Iterate through the permutations' templates for a given basic config
		for (int permIndex = 0; permIndex < permsNum; ++permIndex)
		{
			CRankedBlock* newPermutation =
				AllocateMaterialsToPermutation(m_basicConfigs[basicConf], permIndex);
			if (NULL != newPermutation)
			{
				m_permutations.push_back(newPermutation);
				++m_numPermutations;
			}
		}
	}

	// Prepare to speed-up last-minute optimizations
	// for user's comfort, GetOptCartForUsersComfort()
	m_CurrInsertedTanks.clear();
	for (int t = 0; t < TOTAL_NUMBER_OF_CONTAINERS; ++t)
	{
		TTankInfo tti;
		tti.Weight = m_BackEnd->GetTankWeightInGram(static_cast<TTankIndex>(t));
		tti.ResinType = m_BackEnd->GetTankMaterialType(static_cast<TTankIndex>(t));
		m_CurrInsertedTanks.push_back(tti);
	}

	try {
		// Iterate through all assigned-permutations and rank them
		SetFirstBestPermutation();

		// Apply secondary filter(s) to pick the optimal permutation
		if (!OptimizeForPreservingTankLocation())
			OptimizeForSeparateChambersCMY();
		// We are currently not supporting the combination of both
		//"Preserve DM7's CMY locations" and "Preserve tank location" optimizations together.

		LogPermutations(m_permutations, m_targetMaterials, m_optimalPermutation,
			m_ParamsMgr->MRW_LogPermutations.Value());
	} catch (EQException &e) {
		m_CurrInsertedTanks.clear();
		WriteToLog("Error in ranking the pipeline's permutations");
		WriteToLog(QFormatStr("%s", e.GetErrorMsg().c_str()));
//		QMonitor.ErrorMessage(e.GetErrorMsg(),ORIGIN_WIZARD_PAGE); //Popup an error msg
		throw e;
	}
	m_CurrInsertedTanks.clear();
}

void CResinReplacementPipeline::SetFirstBestPermutation()
{
	int minRank = INT_MAX, currPermIndex = 0;
	for (TRankedPermutations::iterator itrRankedPermIdx = m_permutations.begin();
		 itrRankedPermIdx != m_permutations.end(); ++itrRankedPermIdx)
	{
		Rank(*itrRankedPermIdx);

		 //Find best permutation (minimum rank) so far
		if ((*itrRankedPermIdx)->GetRank() < minRank)
		{
			minRank = (*itrRankedPermIdx)->GetRank();
			m_optimalPermutation = currPermIndex;
		}
		currPermIndex++;
	}
}

//When in DM7, prefer an arrangement where VeroCyan, VeroMgnt & VeroYellow
//each have there own chamber such that there's an easy transition TO them
//when flooding (DM7->DM3).
bool CResinReplacementPipeline::OptimizeForSeparateChambersCMY()
{
	int hasBestPermutationBeenAltered = false;

	//[@Assumption] no mode exists where only SOME of the thermistors are high
	if (mrwDM3 == m_targetReplacementMode)
		return hasBestPermutationBeenAltered; //Optimization isn't relevant

	//Check if any of the CMY resin are even chosen
	int numCMYcomponents = 0;
	int tagsCMYpositions[NUMBER_OF_CHAMBERS];
	int currTag = 0;
	for (std::vector<TVirtTargetMaterial>::iterator itrTargetResinIdx = m_targetMaterials.begin();
		 itrTargetResinIdx != m_targetMaterials.end(); ++itrTargetResinIdx)
	{
		QString chosenResin = (*itrTargetResinIdx).SelectedResin;
		if ((0 == chosenResin.compare(COLOR_CYAN))    ||
			(0 == chosenResin.compare(COLOR_MAGENTA)) ||
			(0 == chosenResin.compare(COLOR_YELLOW)))
		{  //save the CMY color's tag found
		   tagsCMYpositions[numCMYcomponents++] = currTag;
		}
		++currTag;
	}
	if (2 > numCMYcomponents)
		return hasBestPermutationBeenAltered;


	int xtPreferredPermIndex = 0;
	const int currBestPermRank = m_permutations[m_optimalPermutation]->GetRank();
		
	for (TRankedPermutations::iterator itrRankedPermIdx = m_permutations.begin();
		 itrRankedPermIdx != m_permutations.end(); ++itrRankedPermIdx)
	{
		CRankedBlock* thisPerm = (*itrRankedPermIdx);
		if (currBestPermRank == thisPerm->GetRank()) {

			bool faultyPermutation = false;

			//[@Assumption] each 2 chambers are connected and can be flooded within
			for (int k=0; k < LAST_MODEL_CHAMBER_LOCATION; k=k+2) {

				//check if the permutation contains adjacent (in each flooding chambers pair) CMY resins
				CRankedPipingChamber& c1 = thisPerm->GetChamber(static_cast<TChambersLocation>(RIGHT_MOST + k));
				CRankedPipingChamber& c2 = thisPerm->GetChamber(static_cast<TChambersLocation>(RIGHT_MIDL + k));

				//if (c1.IsValidTag() && c2.IsValidTag()) //always True in DM7
				for (int i = 0; i < numCMYcomponents; ++i) {
					if (c1.GetTag() == tagsCMYpositions[i])
					{
						for (int j = 0; j < numCMYcomponents; ++j) {
							if ((i != j) && (c2.GetTag() == tagsCMYpositions[j])) {
								faultyPermutation = true;
								break; //found a faulty pair of resins C<->M / M<->Y / Y<->C
							}
						}
						if (faultyPermutation)
							break;
					}
				}

				if (faultyPermutation)
					break;
			}

			if (!faultyPermutation)
			{
				hasBestPermutationBeenAltered = true;
				break;
			}
		}
		xtPreferredPermIndex++;
	}
	
	if (true == hasBestPermutationBeenAltered)
	{
		m_optimalPermutation = xtPreferredPermIndex;
		if (0 < m_optimalPermutation)
			WriteToLog(QFormatStr("CMY Optimized(%d)", m_optimalPermutation).c_str());
	}
	
	return hasBestPermutationBeenAltered;
}

bool CResinReplacementPipeline::OptimizeForPreservingTankLocation()
{
	int hasBestPermutationBeenAltered = false;

	int xtPreferredPermIndex  = 0;
	int bestDisplacementCount = INT_MAX; // Smallest number of displacaments found so far in the checked permutations.
	int bestPerm			  = 0;       // The permutation id that holds the smallest number of displacaments found so far.
	int permDisplacementCount = 0;		 // Displacaments Counter for the current inspected permutation.
	int displacementCountOfOptimalPermutation = 0; // Displacaments count in the permutation originally chosen by Ranker()

	const int currBestPermRank = m_permutations[m_optimalPermutation]->GetRank();
		
	for (TRankedPermutations::iterator itrRankedPermIdx = m_permutations.begin();
		 itrRankedPermIdx != m_permutations.end(); ++itrRankedPermIdx)
	{
		CRankedBlock* thisPerm = (*itrRankedPermIdx);
		if (currBestPermRank < thisPerm->GetRank())
			continue;

		permDisplacementCount = 0;
		for (int j = 0; j < NUMBER_OF_CHAMBERS_LOCATIONS; ++j) {
			CRankedPipingChamber& c = thisPerm->
							GetChamber(static_cast<TChambersLocation>(j));
			if (c.IsValidTag())
			{
				//Get the target resin's name
				QString chamResin = m_targetMaterials[c.GetTag()].SelectedResin;
				//Get the source (current) resin's name
				TChamberIndex chamIndex = CBlockConfiguration::ConvertToChamberIndex(static_cast<TChambersLocation>(j));
				QString chamCurrResin = m_currState[chamIndex]->GetLongPipe();

				if (0 != chamResin.compare(chamCurrResin)) {
					permDisplacementCount++;
				}
			}
		}
		if (permDisplacementCount < bestDisplacementCount)
		{
			bestDisplacementCount = permDisplacementCount;
			bestPerm 			  = xtPreferredPermIndex;
		}

		//While iterating through the permutations, save #displacement for the current optimal permutation
		if (xtPreferredPermIndex == m_optimalPermutation)
			displacementCountOfOptimalPermutation = permDisplacementCount;

		xtPreferredPermIndex++;
	}

	//The alternative permutation must be BETTER than the current optimal one
	if (bestDisplacementCount < displacementCountOfOptimalPermutation)
	{
		m_optimalPermutation = bestPerm;
		hasBestPermutationBeenAltered = true;
	}

	return hasBestPermutationBeenAltered;
}

void CResinReplacementPipeline::WriteToLog(const QString str) const
{
	CQLog::Write(LOG_TAG_GENERAL, str.c_str());

	if(m_OwnerWizard)
		if(m_OwnerWizard->HistoryFile)
        	m_OwnerWizard->HistoryFile->Append(str);
}

void CResinReplacementPipeline::SetBypassComputations(bool isQuiet)
{
	m_silentMode = isQuiet;
	m_pipelineResult->SetBypassComputations(isQuiet);
}

COptimalTanks* CResinReplacementPipeline::AutoRunPipeline(
	TResinsReplMode targMode, bool silent)
{
	SetBypassComputations(silent);
	SetReplacementMode(targMode);
	GenerateBasicConfigs();
	ForceSystemMaterialsConstraints(); // fix the Support onto leftmost chamber
	GenerateConstrainedConfigsAndRank(); //create ranked permutations
	return GetOptimalPermutation();
}

COptimalTanks* CResinReplacementPipeline::GetOptimalPermutation()
{
	if ((0 > m_optimalPermutation) || (m_optimalPermutation > m_numPermutations))
		return NULL;

	CRankedBlock* chosenPermutation = m_permutations[m_optimalPermutation];
	for (int j = 0; j < NUMBER_OF_CHAMBERS_LOCATIONS; ++j) {
		CRankedPipingChamber& c = chosenPermutation->
						GetChamber(static_cast<TChambersLocation>(j));
		if (c.IsValidTag())
		{
			QString chamResin = m_targetMaterials[c.GetTag()].SelectedResin;
			TReplacementCartridges location = c.GetAssignedCartridges();
			int leftCyc = c.GetLeftCycles();
			int rightCyc = c.GetRightCycles();
			bool flushMode = c.IsEconomyMode();
			bool isFlood = chosenPermutation->IsFlooding(static_cast<TChambersLocation>(j));

			/* Fixes a potential bug: If the source resin in the piping matches
			 * the target resin, and if the transition is 'Full'->Economy'
			 * then do NOT change to 'Economy' (i.e. RESET to FULL).
			 * We don't want the successive M.R. to 'Full' to cost a lot (special Undefined->XXX case).*/
			bool resetLeft = false, resetRight = false;
			if (flushMode && //target is Economy
				(0 == chamResin.compare(m_currState[j]->GetLongPipe()))) {
				TChamberIndex tc = static_cast<TChamberIndex>(j); //== ConvertToChamberIndex(j)
				
				resetLeft = !(m_ParamsMgr->MRW_IsSegmentEconomy[
					GetSegmentIndex(tc, LEFT_CART)].Value()); //source is Full
				if (resetLeft)
					resetLeft = (0 == chamResin.compare(m_currState[j]->GetLeftPipe()));

				resetRight = !(m_ParamsMgr->MRW_IsSegmentEconomy[
					GetSegmentIndex(tc, LEFT_CART)].Value()); //source is Full
				if (resetRight)
					resetRight = (0 == chamResin.compare(m_currState[j]->GetRightPipe()));
			}

			/* WARNING: Do not exclude the support chamber from being marked for replacement.
			   Later on, when parameters need to be updated, neither Support nor M3(objet)/M7(keshet) chamber
			   might be updated => Inconsistency error. */

			//Declare settings for left pump/tank (or 'both')
			if (!(RIGHT_CART == location)) {
				if (resetLeft) {
					 WriteToLog(QFormatStr("MRW : Reverted to Full (%dL)",j));
				}
				m_pipelineResult->SetOptimization(
					 GetPermTankIndex(static_cast<TChambersLocation>(j), LEFT_CART),
					 chamResin , leftCyc, resetLeft ? false : flushMode, isFlood);
			}

			//Declare settings for right pump/tank (or 'both')
			if (!(LEFT_CART == location)) {
				if (resetRight) {
					 WriteToLog(QFormatStr("MRW : Reverted to Full (%dR)",j));
				}
				m_pipelineResult->SetOptimization(
					 GetPermTankIndex(static_cast<TChambersLocation>(j), RIGHT_CART),
					 chamResin , rightCyc, resetRight ? false : flushMode, isFlood);
			}
		}
	}
	m_pipelineResult->RefreshAndApplyCompensation(); //Mandatory(!) after all the opt'd tanks have been set
	return m_pipelineResult;
}

void CResinReplacementPipeline::LogPermutations(TRankedPermutations& perms,
	std::vector<TVirtTargetMaterial> targMats, int bestPermIdx, bool verbose) const
{
#ifdef OBJET_MACHINE_KESHET
	const char* EMPTY_CHAMBER = "EMPTY";

	//QFormatStr() fixes problem (garbage strings) when logging more than 1 formatted argument
	WriteToLog(QFormatStr("MRW's possible resins placements (%s, %s, %s, %s, %s, %s, %s, %s) [Rank]",
		   ChamberToStr(TYPE_CHAMBER_SUPPORT).c_str(),
		   ChamberToStr(TYPE_CHAMBER_MODEL7).c_str(),
		   ChamberToStr(TYPE_CHAMBER_MODEL6).c_str(),
		   ChamberToStr(TYPE_CHAMBER_MODEL5).c_str(),
		   ChamberToStr(TYPE_CHAMBER_MODEL4).c_str(),
		   ChamberToStr(TYPE_CHAMBER_MODEL3).c_str(),
		   ChamberToStr(TYPE_CHAMBER_MODEL2).c_str(),
		   ChamberToStr(TYPE_CHAMBER_MODEL1).c_str())
	);

	//Note: if verbose=true and number of permutation is big,
	//      you may experience slow page transition.
	if (verbose)
	{
		int permID = 0;
		for (TRankedPermutations::iterator itr = perms.begin(); itr != perms.end(); ++itr)
		{
			CRankedBlock* perm = (*itr);
			WriteToLog(QFormatStr("MRW Permutation #%04d: (%s, %s, %s, %s, %s, %s, %s, %s) [Rank=%d]",
				   ++permID,
					(perm->GetChamber(LEFT_MOST).IsValidTag()) ?
					targMats[perm->GetChamber(LEFT_MOST).GetTag()].SelectedResin.c_str() : EMPTY_CHAMBER,
					(perm->GetChamber(LEFT_MIDL3).IsValidTag()) ?
					targMats[perm->GetChamber(LEFT_MIDL3).GetTag()].SelectedResin.c_str() : EMPTY_CHAMBER,
					(perm->GetChamber(LEFT_MIDL2).IsValidTag()) ?
					targMats[perm->GetChamber(LEFT_MIDL2).GetTag()].SelectedResin.c_str() : EMPTY_CHAMBER,
					(perm->GetChamber(LEFT_MIDL).IsValidTag()) ?
					targMats[perm->GetChamber(LEFT_MIDL).GetTag()].SelectedResin.c_str() : EMPTY_CHAMBER,
					(perm->GetChamber(RIGHT_MIDL3).IsValidTag()) ?
					targMats[perm->GetChamber(RIGHT_MIDL3).GetTag()].SelectedResin.c_str() : EMPTY_CHAMBER,
					(perm->GetChamber(RIGHT_MIDL2).IsValidTag()) ?
					targMats[perm->GetChamber(RIGHT_MIDL2).GetTag()].SelectedResin.c_str() : EMPTY_CHAMBER,
					(perm->GetChamber(RIGHT_MIDL).IsValidTag()) ?
					targMats[perm->GetChamber(RIGHT_MIDL).GetTag()].SelectedResin.c_str() : EMPTY_CHAMBER,
					(perm->GetChamber(RIGHT_MOST).IsValidTag()) ?
					targMats[perm->GetChamber(RIGHT_MOST).GetTag()].SelectedResin.c_str() : EMPTY_CHAMBER,
					perm->GetRank())
			);
		}

		WriteToLog("MRW's possible resins placements -- End");
	}

	if (bestPermIdx >= 0)
	{
		CRankedBlock* perm = perms.at(bestPermIdx);
		int validCham[NUMBER_OF_CHAMBERS_LOCATIONS];
		for (int i=0; i<NUMBER_OF_CHAMBERS_LOCATIONS; ++i)
		{
			if (perm->GetChamber(static_cast<TChambersLocation>(i)).IsValidTag()) {
				validCham[i] = perm->GetChamber(static_cast<TChambersLocation>(i)).GetTag();
			} else
				validCham[i] = -1;
		}
			
		WriteToLog(
			QFormatStr("Permutation #%04d was chosen for M.R. (%s, %s, %s, %s, %s, %s, %s, %s) [Rank=%d]",
			bestPermIdx + 1,
			(0 <= validCham[LEFT_MOST]) ? targMats[validCham[LEFT_MOST]].SelectedResin.c_str() : EMPTY_CHAMBER,
			(0 <= validCham[LEFT_MIDL3]) ? targMats[validCham[LEFT_MIDL3]].SelectedResin.c_str() : EMPTY_CHAMBER,
			(0 <= validCham[LEFT_MIDL2]) ? targMats[validCham[LEFT_MIDL2]].SelectedResin.c_str() : EMPTY_CHAMBER,
			(0 <= validCham[LEFT_MIDL]) ? targMats[validCham[LEFT_MIDL]].SelectedResin.c_str() : EMPTY_CHAMBER,
			(0 <= validCham[RIGHT_MIDL3])? targMats[validCham[RIGHT_MIDL3]].SelectedResin.c_str() : EMPTY_CHAMBER,
			(0 <= validCham[RIGHT_MIDL2])? targMats[validCham[RIGHT_MIDL2]].SelectedResin.c_str() : EMPTY_CHAMBER,
			(0 <= validCham[RIGHT_MIDL])? targMats[validCham[RIGHT_MIDL]].SelectedResin.c_str() : EMPTY_CHAMBER,
			(0 <= validCham[RIGHT_MOST])? targMats[validCham[RIGHT_MOST]].SelectedResin.c_str() : EMPTY_CHAMBER,
			perm->GetRank())
		);

		CRankedPipingChamber& Chamber_0 = perm->GetChamber(LEFT_MOST);
		CRankedPipingChamber& Chamber_7 = perm->GetChamber(LEFT_MIDL3);
		CRankedPipingChamber& Chamber_6 = perm->GetChamber(LEFT_MIDL2);
		CRankedPipingChamber& Chamber_5 = perm->GetChamber(LEFT_MIDL);
		CRankedPipingChamber& Chamber_4 = perm->GetChamber(RIGHT_MIDL3);
		CRankedPipingChamber& Chamber_3 = perm->GetChamber(RIGHT_MIDL2);
		CRankedPipingChamber& Chamber_2 = perm->GetChamber(RIGHT_MIDL);
		CRankedPipingChamber& Chamber_1 = perm->GetChamber(RIGHT_MOST);
		if (0 < perm->GetRank()) {
			WriteToLog(
				QFormatStr("Non-compensating washing cycles [normalized] : (%d [%d], %d [%d], %d [%d], %d [%d], %d [%d], %d [%d], %d [%d], %d [%d])",
				Chamber_0.GetTotalCycles(), Chamber_0.GetNormalizedCycles(Chamber_0.GetTotalCycles()),
				Chamber_7.GetTotalCycles(), Chamber_7.GetNormalizedCycles(Chamber_7.GetTotalCycles()),
				Chamber_6.GetTotalCycles(), Chamber_6.GetNormalizedCycles(Chamber_6.GetTotalCycles()),
				Chamber_5.GetTotalCycles(), Chamber_5.GetNormalizedCycles(Chamber_5.GetTotalCycles()),
				Chamber_4.GetTotalCycles(), Chamber_4.GetNormalizedCycles(Chamber_4.GetTotalCycles()),
				Chamber_3.GetTotalCycles(), Chamber_3.GetNormalizedCycles(Chamber_3.GetTotalCycles()),
				Chamber_2.GetTotalCycles(), Chamber_2.GetNormalizedCycles(Chamber_2.GetTotalCycles()),
				Chamber_1.GetTotalCycles(), Chamber_1.GetNormalizedCycles(Chamber_1.GetTotalCycles())
			));
		}
		WriteToLog(
			QFormatStr("Flushing type: (%s, %s, %s, %s, %s, %s, %s, %s)",
			(0 <= validCham[LEFT_MOST]) ? targMats[validCham[LEFT_MOST]].PurposeToString() : EMPTY_CHAMBER,
			(0 <= validCham[LEFT_MIDL3]) ? targMats[validCham[LEFT_MIDL3]].PurposeToString() : EMPTY_CHAMBER,
			(0 <= validCham[LEFT_MIDL2]) ? targMats[validCham[LEFT_MIDL2]].PurposeToString() : EMPTY_CHAMBER,
			(0 <= validCham[LEFT_MIDL]) ? targMats[validCham[LEFT_MIDL]].PurposeToString() : EMPTY_CHAMBER,
			(0 <= validCham[RIGHT_MIDL3])? targMats[validCham[RIGHT_MIDL3]].PurposeToString() : EMPTY_CHAMBER,
			(0 <= validCham[RIGHT_MIDL2])? targMats[validCham[RIGHT_MIDL2]].PurposeToString() : EMPTY_CHAMBER,
			(0 <= validCham[RIGHT_MIDL])? targMats[validCham[RIGHT_MIDL]].PurposeToString() : EMPTY_CHAMBER,
			(0 <= validCham[RIGHT_MOST])? targMats[validCham[RIGHT_MOST]].PurposeToString() : EMPTY_CHAMBER)
		);
		WriteToLog(
			QFormatStr("Cartridges to replace: (%s, %s, %s, %s, %s, %s, %s, %s)",
			(0 <= validCham[LEFT_MOST]) ? targMats[0].CartridgesToString(
				perm->GetChamber(LEFT_MOST).GetAssignedCartridges()) : EMPTY_CHAMBER,
			(0 <= validCham[LEFT_MIDL3]) ? targMats[0].CartridgesToString(
				perm->GetChamber(LEFT_MIDL3).GetAssignedCartridges()) : EMPTY_CHAMBER,
			(0 <= validCham[LEFT_MIDL2]) ? targMats[0].CartridgesToString(
				perm->GetChamber(LEFT_MIDL2).GetAssignedCartridges()) : EMPTY_CHAMBER,
			(0 <= validCham[LEFT_MIDL]) ? targMats[0].CartridgesToString(
				perm->GetChamber(LEFT_MIDL).GetAssignedCartridges()) : EMPTY_CHAMBER,
			(0 <= validCham[RIGHT_MIDL3])? targMats[0].CartridgesToString(
				perm->GetChamber(RIGHT_MIDL3).GetAssignedCartridges()) : EMPTY_CHAMBER,
			(0 <= validCham[RIGHT_MIDL2])? targMats[0].CartridgesToString(
				perm->GetChamber(RIGHT_MIDL2).GetAssignedCartridges()) : EMPTY_CHAMBER,
			(0 <= validCham[RIGHT_MIDL])? targMats[0].CartridgesToString(
				perm->GetChamber(RIGHT_MIDL).GetAssignedCartridges()) : EMPTY_CHAMBER,
			(0 <= validCham[RIGHT_MOST])? targMats[0].CartridgesToString(
				perm->GetChamber(RIGHT_MOST).GetAssignedCartridges()) : EMPTY_CHAMBER)
		);
	}
#endif
}


int CResinReplacementPipeline::GetNumCycles(QString fromResinName, QString toResinName,
				 TReplacementPurpose tintOrProp, TTubeType desiredKeyProperty,
				 bool restrictSecondFlushing)
{
#if defined EMULATION_ONLY
	return 0;	//bypass the use of the Matrix

#else
	if (m_silentMode)
		return 0;

	if ((fromResinName == toResinName) && (restrictSecondFlushing))
		return 0; 
	else
	{
		AnsiString fromMat, toMat;
		fromMat = AnsiString(fromResinName.c_str());
		toMat = AnsiString(toResinName.c_str());

		if (toMat == "") //to DM3 for an unselected cell
		{
			return FICTIVE_VALUE;
		}
		else
		{
		return m_tblCycMatrix->GetCycles(
			fromMat, toMat, desiredKeyProperty,
			(TINT_CYCLES == tintOrProp) ? tFlushTint : tFlushMechanical );
		}
	}
#endif
}

void CResinReplacementPipeline::Rank(CRankedBlock* permutation)
{
	int overallBlockCycles = 0;

	for (int chamLocation = 0; chamLocation < NUMBER_OF_CHAMBERS_LOCATIONS; ++chamLocation)
	{
		//Correlate a chamber's physical position with its index number
		CRankedPipingChamber& currChamber = permutation->GetChamber(static_cast<TChambersLocation>(chamLocation));
		int tmID = currChamber.GetTag();
		if (DONT_CARE_INDEX == tmID) continue; //This chamber isn't part of the ranking decisions.

		TChamberIndex chamIndex = CBlockConfiguration::ConvertToChamberIndex(static_cast<TChambersLocation>(chamLocation));

		CNamedPipingChamber* srcChamber = m_currState[chamIndex];  //m_currState is ordered by TChamberIndex, which is not necessarily the same as TChambersLocation !!
		QString destResin = m_targetMaterials[tmID].SelectedResin;
		bool secondFlushingDisabled = !(m_targetMaterials[tmID].SecondFlushing);
		TReplacementPurpose replPurpose = m_targetMaterials[tmID].FlushingPurpose;

		TReplacementCartridges cartPos = (m_targetMaterials[tmID].FlushingBothCartridges);
		currChamber.SetCartridges(cartPos);
		// save requirement: economy/full mode
		currChamber.SetFlushingMode(PROPERTIES_CYCLES == replPurpose);

		// If the current resin in the relevant pipe is Economy mode,
		// and we want to M.R in Full mode (i.e. Economy->Full) - use Undefined as the Source resin.
		QString srcResinLongPipe  = srcChamber->GetLongPipe();
		QString srcResinLeftPipe  = srcChamber->GetLeftPipe();
		QString srcResinRightPipe = srcChamber->GetRightPipe();
		if (TINT_CYCLES == replPurpose) {
			//[@Assumption] There cannot be one support cartridge "full" and the other "economy". If this happens, GetSegmentIndex() should be revised to check against a virtual LONG ("shared") Support Pipe
			if (m_ParamsMgr->MRW_IsSegmentEconomy[
				GetSegmentIndex(chamIndex)])
				srcResinLongPipe  = m_undefinedStr;
			if (m_ParamsMgr->MRW_IsSegmentEconomy[
				GetSegmentIndex(chamIndex,LEFT_CART)])
				srcResinLeftPipe  = m_undefinedStr;
			if (m_ParamsMgr->MRW_IsSegmentEconomy[
				GetSegmentIndex(chamIndex,RIGHT_CART)])
				srcResinRightPipe  = m_undefinedStr;
		}

		switch (cartPos) {
		   case (LEFT_CART):
		   { 
			//Max-Long-Cycles[ long-pipe, short-left-pipe ]
			 int cycLong = GetNumCycles(srcResinLongPipe, destResin, replPurpose, tLongTube, secondFlushingDisabled);
			 int cycLeft = GetNumCycles(srcResinLeftPipe, destResin, replPurpose, tLongTube, secondFlushingDisabled);
			 currChamber.SetLeftCycles( MAX(cycLong,cycLeft) );
		   }

		   break;
		   case	(RIGHT_CART):
		   {
			 //Max-Long-Cycles[ long-pipe, short-right-pipe ]
			 int cycLong = GetNumCycles(srcResinLongPipe , destResin, replPurpose, tLongTube, secondFlushingDisabled);
			 int cycRight= GetNumCycles(srcResinRightPipe, destResin, replPurpose, tLongTube, secondFlushingDisabled);
			 currChamber.SetRightCycles( MAX(cycLong,cycRight) );
		   }

		   break;
		   case (SINGLE_CART):
		   {
			 int cycLong = GetNumCycles(srcResinLongPipe, destResin, replPurpose, tLongTube, secondFlushingDisabled);
			 int cycLeft = GetNumCycles(srcResinLeftPipe, destResin, replPurpose, tLongTube, secondFlushingDisabled);
			 int cycRight= GetNumCycles(srcResinRightPipe,destResin, replPurpose, tLongTube, secondFlushingDisabled);
			 int totalLeft = MAX(cycLong,cycLeft);
			 int totalRight= MAX(cycLong,cycRight);
			 //whatever side that is less resin consuming - pick it
			 if ( (totalLeft < totalRight) ||
				  ((totalLeft == totalRight) &&
				   (RIGHT_CART != GetOptCartForUsersComfort(static_cast<TChambersLocation>(chamLocation),destResin)))
				)
			 {
				currChamber.SetLeftCycles( totalLeft );
				currChamber.SetCartridges(LEFT_CART);
			 } else {
				currChamber.SetRightCycles( totalRight );
				currChamber.SetCartridges(RIGHT_CART);
			 }
		   }

		   break;
		   default: //	BOTH_CART
		   {
			 // Min-Short-Cycles[ short-left-pipe, short-right-pipe ]
			 //+Max-Long-Cycles [ short-left-pipe, short-right-pipe ]
			 bool leftCartIsFirst = false;
			 int cycLeft = GetNumCycles(srcResinLeftPipe, destResin, replPurpose, tShortTube, secondFlushingDisabled);
			 int cycRight= GetNumCycles(srcResinRightPipe,destResin, replPurpose, tShortTube, secondFlushingDisabled);
			 if (cycLeft <= cycRight) {
				 currChamber.SetLeftCycles(cycLeft);
				 leftCartIsFirst = true; //Mark the left container to be pumped from first
			 } else {
				 currChamber.SetRightCycles(cycRight);
			 }

			 cycLeft = GetNumCycles(srcResinLeftPipe, destResin, replPurpose, tLongTube, secondFlushingDisabled);
			 cycRight= GetNumCycles(srcResinRightPipe,destResin, replPurpose, tLongTube, secondFlushingDisabled);
			 //make sure we pump the rest of the cycles from the OTHER container
			 if (leftCartIsFirst)
				 currChamber.SetRightCycles( MAX(cycLeft,cycRight) );
			 else
				 currChamber.SetLeftCycles( MAX(cycLeft,cycRight) );
		   }
		}

//#ifdef _DEBUG
//   //Checking if normalization (optimized flooding state) is OK
//   WriteToLog(, QFormatStr("[Info] Ranking -- Chamber = %s ; isFlooding? = %s",
//		ChamberToStr(chamIndex).c_str(),
//		(1.0 == GetNormalizationFactorToShortCycles(permutation, static_cast<TChambersLocation>(chamLocation))) ? "no" : "YES"
//		 ));
//#endif
		currChamber.SetNormalizationFactor(
			GetNormalizationFactorToShortCycles(permutation, static_cast<TChambersLocation>(chamLocation)));

		//TODO (Elad): Check if compensation cycles are needed and add them to overallBlockCycles (without normalizing them !). Thus, making OptimalTanks::Refresh obsolete and all its calls to GetNormalization
		//We do NOT want to normalize currChamber.GetTotalCycles() because we
		//need a unified environment to be able to compare permutations.
		//The Rank is not the number of cycles but rather the total WEIGHT of waste.
		overallBlockCycles += currChamber.GetTotalCycles();
	}

	permutation->SetRank(overallBlockCycles);
}

float CResinReplacementPipeline::GetNormalizationFactorToShortCycles(CRankedBlock const* perm, TChambersLocation chamber) const
{
	//TODO (Elad): Consider prefetching MRW_LowToHighThermistorsVolumesRatio to speed up successive calls
	return FLOODING_NORMALIZATION_FACTOR((perm->IsFlooding(chamber)), m_ParamsMgr);
}

TReplacementCartridges CResinReplacementPipeline::GetOptCartForUsersComfort(TChambersLocation chamber, QString& desiredResinType)
{
	TTankIndex leftTank  = GetPermTankIndex(static_cast<TChambersLocation>(chamber), LEFT_CART );
	TTankIndex rightTank = GetPermTankIndex(static_cast<TChambersLocation>(chamber), RIGHT_CART);

	bool isCorrectLeftResin, isCorrectRightResin;
	isCorrectLeftResin  = (m_CurrInsertedTanks.at((int)leftTank).ResinType == desiredResinType);
	isCorrectRightResin = (m_CurrInsertedTanks.at((int)rightTank).ResinType == desiredResinType);

	if (isCorrectLeftResin && !isCorrectRightResin)
	{
		return LEFT_CART;
	}
	else if (!isCorrectLeftResin && isCorrectRightResin)
	{
		return RIGHT_CART;
	}
	else if (isCorrectLeftResin && isCorrectRightResin)
	{   //if both tanks are valid - choose the heavier one
		return
		  (m_CurrInsertedTanks.at((int)leftTank).Weight > m_CurrInsertedTanks.at((int)rightTank).Weight)
		  ? LEFT_CART : RIGHT_CART;
	}

	//None of the tanks match - can't decide which container is better
	return SINGLE_CART;
}

CRankedBlock* CResinReplacementPipeline::AllocateMaterialsToPermutation(CBlockConfiguration* c, unsigned int permID)
{
	unsigned int activeChambersNum = c->GetNumActiveChambers();   //1-based

	CRankedBlock* currPerm = new CRankedBlock(c);

	//[@Assumption] #active-chambers in permutation match #target-materials
	//[@Assumption] Target Materials' fixed location values (i.e. tags) are aligned
	//				with the actual locations in block, defined by TChambersLocation enum.
	int nextFreeActiveChamber = 0;
	for (unsigned int j = 0, nextLinkedTargMaterialIdx; j < activeChambersNum; ++j)
	{
		int currPermChamberIdx;

		//fetch the target material's index to be placed in the next
		//available location in the current permutation's block of chambers.
		 nextLinkedTargMaterialIdx = GetPermElement(
		 			activeChambersNum, permID, j);

		 int targetChamberIdx = m_targetMaterials[nextLinkedTargMaterialIdx].FixedLocation;
		 if (DONT_CARE_INDEX == targetChamberIdx)
		 { /* find the next available, active chamber in the permutation
			 (which is not occupied by previous allocations and is available
			  according to the current base-configuration template) */
		   while ( (nextFreeActiveChamber < NUMBER_OF_CHAMBERS_LOCATIONS)
				   && (
						currPerm->GetChamber(static_cast<TChambersLocation>(nextFreeActiveChamber)).IsActive()
						|| (! c->IsActive(static_cast<TChambersLocation>(nextFreeActiveChamber)))
					  )
				 )
				nextFreeActiveChamber++;

		   //**Double-check**: do not let a model resin be placed in a support chamber, and vice versa.
		   //Also enforced by limiting the permutations available themselves.
		   if (m_targetMaterials[nextLinkedTargMaterialIdx].IsSupport())
		   {
			   if ( ((int)DEDICATED_SUPPORT_CHAMBER != nextFreeActiveChamber) &&
					((int)EXTRA_SUPPORT_CHAMBER != nextFreeActiveChamber) )
                     nextFreeActiveChamber = NUMBER_OF_CHAMBERS_LOCATIONS;
		   }
		   else //Aforementioned chamber is of a Model-typed resin
			   if ( ((int)DEDICATED_SUPPORT_CHAMBER == nextFreeActiveChamber) ||
					((int)EXTRA_SUPPORT_CHAMBER == nextFreeActiveChamber) )
					 nextFreeActiveChamber = NUMBER_OF_CHAMBERS_LOCATIONS;


		   if (NUMBER_OF_CHAMBERS_LOCATIONS == nextFreeActiveChamber)
		   {
			   delete currPerm;
			   return NULL; //#target-materials is bigger than configuration's #active-chambers.
		   }

		   currPermChamberIdx = nextFreeActiveChamber;
		 }
		 else
		 { //fixate target material on a specific chamber in the permutation
		   if (!c->IsActive(static_cast<TChambersLocation>(targetChamberIdx)))
		   {
			   delete currPerm;
			   return NULL; //The permutation doesn't fit the basic configuration.
		   }

		   if (currPerm->GetChamber(static_cast<TChambersLocation>(targetChamberIdx)).IsActive())
		   {
			   delete currPerm;
#ifdef _DEBUG
			   //This is typically OK if the chamber's index is forced by the system,
			   //and not user-selected, e.g. Support chamber (0).
			   WriteToLog(QFormatStr("Warning: MRW Pipeline: Two target materials were assigned to the same chamber (%d)", targetChamberIdx));
#endif
			   return NULL; //a chamber was already assigned with a target material.
		   }
		   currPermChamberIdx = targetChamberIdx;
		 }

		 currPerm->GetChamber(static_cast<TChambersLocation>(currPermChamberIdx)).SetTag(nextLinkedTargMaterialIdx);
	}

	return currPerm;
}

//---------------------------------------------------------------------------

//CBlockConfiguration::CBlockConfiguration(bool actSupport, bool actM1, bool actM2, bool actM3, bool actM4, bool actM5, bool actM6, bool actM7)
CBlockConfiguration::CBlockConfiguration(bool const chamberIsActive[NUMBER_OF_CHAMBERS])
: m_numActiveChambers(0), m_floodingState(INVALID_FLOODING_STATE)
{
	int i=0;
	SetActive(LEFT_MOST  , chamberIsActive[i++]);  // Support
	SetActive(RIGHT_MOST , chamberIsActive[i++]);  // M1
	SetActive(RIGHT_MIDL , chamberIsActive[i++]);
	SetActive(RIGHT_MIDL2, chamberIsActive[i++]);
	SetActive(RIGHT_MIDL3, chamberIsActive[i++]);
	SetActive(LEFT_MIDL  , chamberIsActive[i++]);
	SetActive(LEFT_MIDL2 , chamberIsActive[i++]);
	SetActive(LEFT_MIDL3 , chamberIsActive[i  ]);  // Model/Support chamber

	//Find out which active chambers are also flooding and mark them
	RecalcFloodingState(); //This is just a fail-safe in case nobody calls Activate(...)
}

TChambersLocation CBlockConfiguration::ConvertToChamberLocation(TChamberIndex const c)
{
	//TChambersLocation might not be the same order as the TChamberIndex enum
	return static_cast<TChambersLocation>(c);
}
TChamberIndex CBlockConfiguration::ConvertToChamberIndex(TChambersLocation const c)
{
	//TChambersLocation might not be the same order as the TChamberIndex enum
	return static_cast<TChamberIndex>(c);
}

void CBlockConfiguration::RecalcFloodingState()
{
	m_floodingState = INVALID_FLOODING_STATE;
	for (int ch=0; ch < NUMBER_OF_CHAMBERS_LOCATIONS; ch++)
		SetFloodingChamberIfActive(static_cast<TChambersLocation>(ch));
}

void CBlockConfiguration::SetFloodingChamberIfActive(TChambersLocation chamber)
{
	TChamberIndex currIdx = ConvertToChamberIndex(chamber);
	TChambersLocation sibling = ConvertToChamberLocation(GetSiblingChamber(currIdx));
	if (IsActive(chamber) && !IsActive(sibling)
		/* && (sibling != DEDICATED_SUPPORT_CHAMBER) isn't needed if SetFloodingChamberIfActive()
		   is called AFTER there are no further changes in m_chambers (via SetActive/Activate messages)*/
		) {
		if (m_floodingState < 0) m_floodingState = 0; //leave the invalid state
		//commulate a bit ("1") representing a flooding chamber
		m_floodingState |= (1 << ((int)currIdx)); //(encoding)
	}
}

bool CBlockConfiguration::IsChamberFlooding(TChambersLocation chamber) const
{
	if (INVALID_FLOODING_STATE != m_floodingState) {
		 return (0 < (int)(m_floodingState & (1 << (ConvertToChamberIndex(chamber))))); //(decoding)
	}
	return false;
}

CBlockConfiguration::~CBlockConfiguration() {}

void CBlockConfiguration::Activate(TChambersLocation chamber, bool active)
{
	SetActive(chamber, active);
	RecalcFloodingState();
}

void CBlockConfiguration::SetActive(TChambersLocation chamber, bool active)
{
	m_chambers[chamber] = active;

	// recount number of active chambers:
	m_numActiveChambers = 0;
	for (int ch=0; ch < NUMBER_OF_CHAMBERS_LOCATIONS; ch++)
		if (IsActive(static_cast<TChambersLocation>(ch)))
			m_numActiveChambers++;
}

bool CBlockConfiguration::IsActive(TChambersLocation ch) const
{
	return m_chambers[ch];
}

//---------------------------------------------------------------------------

CRankedPipingChamber::CRankedPipingChamber()
:
  m_toBeFlushed	(false)
, m_cartridges	(MAX_CART_PER_CHAMBER)
, m_demandOrigin (NO_TARGET_MATERIAL_ASSOCIATED)
, m_leftCycles	(0)
, m_rightCycles	(0)
, m_economyMode (false)
, m_floodingFactor (1.0)
{}

CRankedPipingChamber::~CRankedPipingChamber() {}

//---------------------------------------------------------------------------

CRankedBlock::CRankedBlock(CBlockConfiguration* _baseCfg)
: m_baseCfg(_baseCfg), m_Rank(0)
{
	//a block of 4 cells - represents a single permutation
	m_Block = new CRankedPipingChamber[NUMBER_OF_CHAMBERS_LOCATIONS];
}
CRankedBlock::~CRankedBlock()
{
	Q_SAFE_DELETE_ARRAY(m_Block);
}

//---------------------------------------------------------------------------

COptimalTanks::COptimalTanks() : m_AutoRecheckFlooding(true), m_Silent(false) //,m_dirty(false)
{
	Clear();
	m_UndefinedStr = LOAD_STRING(IDS_MRW_UNDEFINED);
	m_ParamsMgr = CAppParams::Instance();
}

COptimalTanks::~COptimalTanks() {}

void COptimalTanks::Clear()
{
	for (int i = FIRST_TANK_TYPE; i < TOTAL_NUMBER_OF_CONTAINERS; ++i) {
		m_Active[i]  = false;
		m_Resins[i]  = "";
		m_Cycles[i]  = 0;
		m_Economy[i] = false;
		m_Flooding[i] = false;
	}
}

void COptimalTanks::SetOptimization(int tankIdx, QString tankName, int tankCycles, bool economyFlush)
{
	m_Active[tankIdx]  = true;
	m_Resins[tankIdx]  = tankName;
	m_Cycles[tankIdx]  = tankCycles;
	m_Economy[tankIdx] = economyFlush;

//	m_dirty = true; //marks the bare minimum data that should be changed to force recalculation of new thermistors mode 
}

void COptimalTanks::SetOptimization(int tankIdx, QString tankName, int tankCycles, bool economyFlush, bool isFlooding)
{
	SetOptimization(tankIdx, tankName, tankCycles, economyFlush);

	//"DM6" check - no matter what - we always flood the support pair of chambers
	//Setting the correct flooding state will keep the queried #cycles, needed-weight correct.  
	TChamberIndex dualSupportCheck = TankToStaticChamber(static_cast<TTankIndex>(tankIdx));
	if ((FIRST_SUPPORT_CHAMBER_INDEX == dualSupportCheck) || (TYPE_CHAMBER_MODEL7 == dualSupportCheck))
		m_Flooding[tankIdx] = true;
	else
	   m_Flooding[tankIdx] = isFlooding;

	m_AutoRecheckFlooding = false;
}


TPumpingOrderPriority COptimalTanks::GetPumpQoS(TTankIndex i) const
{
	if ((!m_Active[i]) || (0 == m_Cycles[i])) return DONT_PUMP;

	int neighbourTank = (int)GetSiblingTank(i);//(0 == i % 2) ? i+1 : i-1;
	//returns true if the current tank is the shorter in washing cycles
	//than its neighbour tank, or if they are equal in cycles and the tank
	//is the left one (i.e. even index).
	bool condAmIOnlySibling = !m_Active[neighbourTank];
	bool condLessCyclesThanSibling = (m_Cycles[i] < m_Cycles[neighbourTank]);
	bool condCompensatingForBothTanks = ((0 == m_Cycles[neighbourTank]) && m_Active[neighbourTank]);
	bool condSameNumCyclesAndLeftSibling = /* Left side is chosen arbitrary */
		((m_Cycles[i] == m_Cycles[neighbourTank]) && (i+1 == neighbourTank));

	return (condAmIOnlySibling || condLessCyclesThanSibling
		|| condSameNumCyclesAndLeftSibling || condCompensatingForBothTanks)
		? PUMP_FIRST : PUMP_SECOND;
}

int COptimalTanks::GetMinWeight(TTankIndex i) const
{
   /* GetNormalizationFactor is obsolete here. It doesn't matter if we need to
	  pump till the high thermistor, because we need to remember that the amount
	  of material is what's important and there no correlation between amount
	  of material needed to wash the pipes and the choosing of a thermistor.
	  Hence, No need to compensate by enlarging the min weight request.

	  MRW_ExtraRequiredTankWeight is a parameter specifing
	  how much material must a container weigh at least, for purges [grams].
	  WeightLevelLimitArray is the weight of extra resin in an 'empty' container. */
   return ROUNDUP_INT(
		  m_ParamsMgr->WeightLevelLimitArray[TankToStaticChamber(i)]
		+ m_ParamsMgr->MRW_ExtraRequiredTankWeight
		+(m_Cycles[i] * m_ParamsMgr->MRW_ResinWeightInLowThermistorVolume));
}

int COptimalTanks::GetCycles(TTankIndex i) const
{
	//If we need to pump till the high thermistor, then each washing cycle
	//is equal to more grams of resin. Compensate by reducing num of cycles.
	return ROUNDUP_INT( m_Cycles[i] * GetNormalizationFactor(i) );
}

//[@Assumption] GetCycles & GetMinWeight have a linear correlation 
//when it comes to cleaning the resin, i.e., there's no real difference in
//making short pulses of pumping over one long pumping pulse.
float COptimalTanks::GetNormalizationFactor(TTankIndex i) const
{
	if ((i==TYPE_TANK_SUPPORT1) || (i==TYPE_TANK_SUPPORT2) || (i==TYPE_TANK_MODEL13) || (i==TYPE_TANK_MODEL14)) // If tanks of support there's no flooding so no factor is needed
	{
		return 1.0;
	}

	return FLOODING_NORMALIZATION_FACTOR(IsFlooding(i), m_ParamsMgr);
}

bool COptimalTanks::IsRecoveryFromHswNeeded() const
{
//OBJET_MACHINE adjustment//i.e. MODEL 3's CHAMBER is defined
	if (!m_ParamsMgr->HSW_WithinWizard.Value()) return false;
	for (int i=0; i<TOTAL_NUMBER_OF_CONTAINERS; ++i)
		if (m_Active[i])
			if (IsHswRecoveryRelated(static_cast<TTankIndex>(i)))
				return true; //at least one M3(/M7)-associated tank is marked for replacement
	return false;
}
bool COptimalTanks::IsHswRecoveryRelated(TTankIndex i) const
{
#ifdef OBJET_MACHINE_KESHET
	return (TYPE_CHAMBER_MODEL7 == TankToStaticChamber(i));
#else
	return (TYPE_CHAMBER_MODEL3 == TankToStaticChamber(i));
#endif
}
TThermistorsOperationMode COptimalTanks::GetThermistorsOpMode()
{
    TThermistorsOperationMode m_ThermistorsOpMode = NUM_OF_THERMISTORS_MODES;
	//NUM_OF_THERMISTORS_MODES serves as an invalid OpMode
//   	if (m_dirty) //Uncomment this line if-only-if we call GetThermistorsOpMode() only ONCE!
	{
		int key = 0;
		const int BASE_INDEX = FIRST_MODEL_TANK_TYPE;
		//iterate through all Model chambers (only), counting #active chambers
		int firstTankOffset = 0;
		int currChamberNumber = 0;

		//TODO (Elad): Transform the following FOR loop to iterate through TANKS, NOT chambers, in order to suppress the assumption of 2 tanks per chamber !
		for (int i=FIRST_MODEL_CHAMBER_INDEX; i < NUMBER_OF_CHAMBERS; ++i) {
			//if one of the tanks related to the current chamber is active - mark it.
			if (m_Active[BASE_INDEX + firstTankOffset  ] ||
				m_Active[BASE_INDEX + firstTankOffset+1])
			{
				key += 1;
			}
			firstTankOffset += 2; //[@Assumption] 2 tanks per chamber
			++currChamberNumber;
		}

		switch (key) {
			case 3:  //united with '4' case
			case 4:  m_ThermistorsOpMode = GetThermOpModeForHQHS(); break;
			case 6:
				//SUPPORT_HIGH_MODEL_LOW_THERMISTORS_MODE here will only affect compensation cycles (if commented - they are disabled for the Support chamber(s))
			case 7:  //m_ThermistorsOpMode = SUPPORT_HIGH_MODEL_LOW_THERMISTORS_MODE;break; //By intention; "DM6"
					m_ThermistorsOpMode = LOW_THERMISTORS_MODE;  break;
			case 0:
//				m_ThermistorsOpMode = NUM_OF_THERMISTORS_MODES;
				CQLog::Write(LOG_TAG_GENERAL,"Error: Pipeline's GetThermistorsOpMode :: invalid state - no model chamber was activated");
				throw EQException("Error - GetThermistorsOpMode - no model chamber was activated");
			
			default:
//				m_ThermistorsOpMode = NUM_OF_THERMISTORS_MODES;
				CQLog::Write(LOG_TAG_GENERAL,"Error: Pipeline's GetThermistorsOpMode :: invalid state");
				throw EQException("Error - GetThermistorsOpMode - mode is invalid");
		}

//		m_dirty = false;
	}
	
	return m_ThermistorsOpMode;
}

//Go through all the active tanks and mark their respective chambers.
//Then, use this bit array (without the Support chamber) to retrieve the tanks operation mode.
TTankOperationMode COptimalTanks::GetTanksOpMode() const
{        
	int key = 0; //bit array order: M7 (MSB), ..., M2, M1 (LSB)
	const int BASE_INDEX = FIRST_MODEL_TANK_TYPE;

	//iterate through all Model chambers (only), setting up corresponding bits if they are active.
	int firstTankOffset = 0;
	int currChamberNumber = 0;

	//TODO (Elad): Transform the following FOR loop to iterate through TANKS, NOT chambers, in order to suppress the assumption of 2 tanks per chamber !
	for (int i=FIRST_MODEL_CHAMBER_INDEX; i < NUMBER_OF_CHAMBERS; ++i) {
		//if one of the tanks related to the current chamber is active - mark it.
		if (m_Active[BASE_INDEX + firstTankOffset  ] ||
			m_Active[BASE_INDEX + firstTankOffset+1])
		{
			key |= (1 << currChamberNumber); //mark the chamber in mask
		}
		firstTankOffset += 2; //[@Assumption] 2 tanks per chamber
		++currChamberNumber;
	}

	switch (key) {  //The key is of form: [M7][M6][M5]...[M1] (support is excluded)
#ifdef OBJET_MACHINE_KESHET
		case 0x3f: return S_M1_M7_ACTIVE_TANKS_MODE;    //0111111 (63) - Relevant for "DM6" only
		case 0x7f: return S_M1_M7_ACTIVE_TANKS_MODE;    //1111111 (127)

		//Relevant for an extra support chamber only (adding 0x40 (64))
		case 0x55: return S_M1_M3_M5_ACTIVE_TANKS_MODE; //1010101 (85)
		case 0x56: return S_M2_M3_M5_ACTIVE_TANKS_MODE; //1010110 (86)
		case 0x59: return S_M1_M4_M5_ACTIVE_TANKS_MODE; //1011001 (89)
		case 0x5A: return S_M2_M4_M5_ACTIVE_TANKS_MODE; //1011010 (90)
		case 0x65: return S_M1_M3_M6_ACTIVE_TANKS_MODE; //1100101 (101)
		case 0x66: return S_M2_M3_M6_ACTIVE_TANKS_MODE; //1100110 (102)
		case 0x69: return S_M1_M4_M6_ACTIVE_TANKS_MODE; //1101001 (105)
		case 0x6A: return S_M2_M4_M6_ACTIVE_TANKS_MODE; //1101010 (106)
		//End of relevance

		case 0x15: return S_M1_M3_M5_ACTIVE_TANKS_MODE; //0010101 (21)
		case 0x16: return S_M2_M3_M5_ACTIVE_TANKS_MODE; //0010110 (22)
		case 0x19: return S_M1_M4_M5_ACTIVE_TANKS_MODE; //0011001 (25)
		case 0x1A: return S_M2_M4_M5_ACTIVE_TANKS_MODE; //0011010 (26)
		case 0x25: return S_M1_M3_M6_ACTIVE_TANKS_MODE; //0100101 (37)
		case 0x26: return S_M2_M3_M6_ACTIVE_TANKS_MODE; //0100110 (38)
		case 0x29: return S_M1_M4_M6_ACTIVE_TANKS_MODE; //0101001 (41)
		case 0x2A: return S_M2_M4_M6_ACTIVE_TANKS_MODE; //0101010 (42)
#endif
		case 0:
			CQLog::Write(LOG_TAG_GENERAL, "Error: Pipeline's GetTanksOpMode :: invalid state - no model chamber was activated");
			throw EQException("Error - Pipeline's GetTanksOpMode :: invalid state - no model chamber was activated");
		default:
			CQLog::Write(LOG_TAG_GENERAL,"Error: Pipeline's GetTanksOpMode :: mode is not supported");
			throw EQException("Error - Pipeline's GetTanksOpMode - unsupported mode");
	}
#pragma warn -8066 // Disable "Unreachable code" warning
	return NUM_OF_TANKS_OPERATION_MODES;
#pragma warn .8066 // Enable "Unreachable code" warning 	
}

void COptimalTanks::IsModelOrSupportThermOpModeChanged(bool (&IsThermChanged)[(int)NUM_OF_SM_CHAMBERS_THERMISTORS])
{
	/* If the Thermistors OpMode should be changed from the current one,
	 and number of cycles is zero in a marked-for-replacement tank -
	 fix the number of cycles.
	 Logic:
		Consider the case where user switches DM7->DM3->DM7 without
	 changing the resins. A washing cycle must be done to fill the once-flooded
	 chamber with correct resin.
		Also, if system needs to flood, it should
	 consider the resin in the flooded chamber, because it might be more difficult
	 to clean than the resin in the flooding chamber.
   */
   // Check whether there is a thermistor change specifically in the MOST Left
   // part of the block (S & Mx; x=max), and in the Right side of the block (M1..My ; y=x-1).
   TThermistorsOperationMode newTherm  = GetThermistorsOpMode();
   TThermistorsOperationMode currTherm = (TThermistorsOperationMode)m_ParamsMgr->ThermistorsOperationMode.Value();

	for (int i=0; i<(int)NUM_OF_SM_CHAMBERS_THERMISTORS; ++i)
		IsThermChanged[i] = false;

#ifdef OBJET_MACHINE_KESHET
	//in "DM6", SUPPORT_HIGH_MODEL_LOW_THERMISTORS_MODE  *IS*  defined so we don't include it here
	if (SUPPORT_LOW_MODEL_HIGH_THERMISTORS_MODE == newTherm)
	{
		CQLog::Write(LOG_TAG_GENERAL, "Error: Pipeline's Refresh :: thermistors state is undefined");
		throw EQException("Error - Pipeline's Refresh :: new thermistors state is invalid");
	}
#endif
	//TODO Elad: DON'T HAVE A CLUE what HIGH_AND_LOW_THERMISTORS_MODE is. Treating it as a general intermediate mode
	switch (currTherm) {
		case HIGH_THERMISTORS_MODE:
			if ((LOW_THERMISTORS_MODE == newTherm) ||
				(SUPPORT_HIGH_MODEL_LOW_THERMISTORS_MODE == newTherm))
			{   //all pairs of chambers' thermistors are about to change (model only)
				for (int i=1; i<(int)NUM_OF_SM_CHAMBERS_THERMISTORS; ++i)
					IsThermChanged[i] = true;
			}
			break;
		case LOW_THERMISTORS_MODE:	//"DM7"
			if (LOW_THERMISTORS_MODE != newTherm)
			{
			 	//support always changes to high thermistor (even "DM6")
				IsThermChanged[0] = true;

				if (SUPPORT_HIGH_MODEL_LOW_THERMISTORS_MODE != newTherm)
				{   //all pairs of chambers' thermistors are about to change (model)
					for (int i=1; i<(int)NUM_OF_SM_CHAMBERS_THERMISTORS; ++i)
						IsThermChanged[i] = true;
				}
			}
			break;
		case SUPPORT_HIGH_MODEL_LOW_THERMISTORS_MODE:
			if (SUPPORT_HIGH_MODEL_LOW_THERMISTORS_MODE != newTherm)
			{
				if (LOW_THERMISTORS_MODE == newTherm)
					IsThermChanged[0] = true;
				else
				{   //all pairs of chambers' thermistors are about to change (model)
					for (int i=1; i<(int)NUM_OF_SM_CHAMBERS_THERMISTORS; ++i)
						IsThermChanged[i] = true;
				}
			}
			break;
		case HIGH_AND_LOW_THERMISTORS_MODE:
			if (newTherm != HIGH_AND_LOW_THERMISTORS_MODE)
			{   //all pairs of chambers' thermistors are about to change (support+model)
				CQLog::Write(LOG_TAG_GENERAL, "Warning: proceeding with an intermediate hi-and-lo thermistors state");
				for (int i=0; i<(int)NUM_OF_SM_CHAMBERS_THERMISTORS; ++i)
					IsThermChanged[i] = true;
			}
			break;
		default:
			CQLog::Write(LOG_TAG_GENERAL, "Error: Pipeline's Refresh :: current thermistors state is undefined");
			throw EQException("Error - Pipeline's Refresh :: current thermistors state is invalid");
	}
}

/*******************************************************************************************************************/
#pragma warn -8004 // Enable warning Number 8004 "Assigned value that is never used"
/*******************************************************************************************************************/
//returns true if the tanks operation mode is about to change for the given chamber.
bool COptimalTanks::IsTankOpModeChanged(TChamberIndex chamber) const
{
	TTankOperationMode newTanksOpMode  = GetTanksOpMode();
	TTankOperationMode currTanksOpMode = (TTankOperationMode)m_ParamsMgr->TanksOperationMode.Value();

	return
		(Chamber2TankOperationModeRelation[(int)currTanksOpMode][(int)chamber] !=
		 Chamber2TankOperationModeRelation[(int)newTanksOpMode ][(int)chamber]);
}
/*******************************************************************************************************************/
#pragma warn .8004 // Disable warning Number 8004 "Assigned value that is never used"
/*******************************************************************************************************************/

void COptimalTanks::RefreshAndApplyCompensation()
{
   //Check if number of cycles throughout the tanks is zero,
   //if so, it may happen that the only change needed is setting thermistors' opmode.

   //update the flooding flags, if the user doesn't supply the flooding state for at least 1 cartridge
   if (m_AutoRecheckFlooding)
   {
	  for (int i = FIRST_TANK_TYPE; i < TOTAL_NUMBER_OF_CONTAINERS; ++i) {
	   m_Flooding[i] = GetFloodingState((TTankIndex)i);
      }
   }

   //[@Assumption] SUPPORT_MODEL_CHAMBER_THERMISTOR - NUM_OF_DM_CHAMBERS_THERMISTORS == 0
   //i.e. SUPPORT_MODEL_CHAMBER_THERMISTOR is the first HIGH thermistor 
   bool isSectionThermChange[NUM_OF_SM_CHAMBERS_THERMISTORS];
   IsModelOrSupportThermOpModeChanged(isSectionThermChange);
   bool bypassNeeded = IsRecoveryFromHswNeeded();

#if defined EMULATION_ONLY
   int wCycThermistorsChangeOnly = 0;
#else
   int wCycThermistorsChangeOnly =
   	   (m_Silent) ? 0 : m_ParamsMgr->MRW_CompensationCyclesThermistorsChange.Value();
#endif

   for (int i = FIRST_TANK_TYPE; i < TOTAL_NUMBER_OF_CONTAINERS; ++i) {

		TTankIndex ii = static_cast<TTankIndex>(i);
		TChamberIndex ci = TankToStaticChamber(ii);

		int currTankHighThermistor = (int)GetTankRelatedHighThermistor(ii);
		int currTankHighThermistorSmOnly = currTankHighThermistor -(int)FIRST_SM_CHAMBER;

	   if (bypassNeeded && IsHswRecoveryRelated(ii))
	   {
		   //If HSW recovery is needed, it's not because of any changes
		   //in chambers M1, M2, Mx (x=max-1, i.e. not related to Support) thus -
		   //no need to add compensation cycles to them.
			if (IsTankInModelBlockHalf(ii))
			{
				if (!isSectionThermChange[currTankHighThermistorSmOnly])
					continue;
			}
			else
				CQLog::Write(LOG_TAG_GENERAL, "Compensation is forced (Hsw recovery)");
	   } else
	   {
			/*
			   ALM defect #418 check: Reentering MRW after a cancellation (aka mrw_withinwizard=1)
			   can cause 'Undefined' to appear in the block while the entire piping is ok.
			   If the printing mode is unchanged - we must complete the previous operation.
			*/
			 //[@assumption] no 'long' pipe for Support chamber. Use an arbitrary 'short' one.
			 //   Problematic when there is more than 1 support available (e.g. soluble support).
			int pipeToCheckIfUndefined = (TYPE_CHAMBER_SUPPORT == ci) ?
			   (int)TankToShortSegment(ii)	:
			   FIRST_SHARED_SEGMENT -1 + (int)(ci);

			bool isUndefinedResinStuckInChamberOnly = (0 == m_Cycles[i]) && m_Active[i]
				&& ((IsTankInSupportBlockHalf(ii) && !isSectionThermChange[0]) ||
					(IsTankInModelBlockHalf(ii) && !isSectionThermChange[currTankHighThermistorSmOnly] && !IsTankOpModeChanged(ci)))
				&& (m_Flooding[i] /*next state is flooding*/
					|| (false == m_ParamsMgr->ActiveThermistors[currTankHighThermistor].Value())
					/*current state is NOT flooding*/)
				&& (0 == m_UndefinedStr.compare(m_ParamsMgr->TypesArrayPerChamber[ci].Value().c_str()))
				&& (0 != m_UndefinedStr.compare(
						m_ParamsMgr->MRW_TypesArrayPerSegment[pipeToCheckIfUndefined].Value().c_str()));
		   
			if (!isUndefinedResinStuckInChamberOnly) {

			   // If this container is feeding a the support "half-of-block" that had no "thermistor change" - skip this container.
			   if (IsTankInSupportBlockHalf(ii))
			   {
					//[Assumption] Support chamber is connected to the S/Model chamber (and not to another Support chamber),
					//so ANY change in the thermistor means a change in the TanksOperationMode.
				  if (! isSectionThermChange[0 /*SUPPORT_MODEL_CHAMBER_THERMISTOR's offset*/])
					continue;

				  //== Filter-out specific cases from compensating them. See below. bug #8827/8852
				  //TankToChamber (dynamic) might not result with a valid value in 2-model chambers machines.

				  //if (! (bypassNeeded && IsHswRecoveryRelated(ii))) {
					  if ((0 == m_Cycles[i]) && m_Active[i] &&
							(isSectionThermChange[0] && (!m_Flooding[i]) &&
							(1 == m_ParamsMgr->ActiveThermistors[currTankHighThermistor].Value()))
					   && (0 == m_Resins[i].compare(m_ParamsMgr->TypesArrayPerChamber[ci].Value().c_str()))
					   && (! (m_ParamsMgr->MRW_IsSegmentEconomy[GetSegmentIndex(TankToStaticChamber((TTankIndex)m_ParamsMgr->ActiveTanks[ci].Value()))].Value() && !m_Economy[i]))
						 )
						  continue;
				  //}
				  //== End bugzilla #8827/8852 fix (1/2)

			   } else
			   if (IsTankInModelBlockHalf(ii))
			   {
				  if (! isSectionThermChange[currTankHighThermistorSmOnly] && ! IsTankOpModeChanged(ci))
					continue;

				  //== Filter-out specific cases from compensating them. bug #8827/8852
				  //if     (1) thermistors are to be changed from high to low, in models-half of the block,
				  //   AND (2) curr. resin in this tank's chamber (in the block itself)
				  //           is the same as the new proposed resin (m_Resins),
				  //   AND NOT (3) {the currently active flooding chamber in M1-M2 contains a Economy-typed resin in its long-pipe,
				  //               and the target chamber will be filled using Full-mode -
				  //			   The reason for converting to active tank fron chamber and then back to the static chamber is there
				  //			   might be a case in which there are two chambers in DM3 with the same resin and there was a bug
				  //			   where the tankToChamber array returned an illegal value and the wizard got an execption,
				  //			   so we need to check the actual active tank and then connect it to its chamber}
				  //			(i.e. everything but Economy->Full is ok and can be skipped)
				  //   AND (4) number of washing cycles is zero and is marked for replacement
				  //=> don't add compensation cycles
				  if ((0 == m_Cycles[i]) && m_Active[i] && (isSectionThermChange[currTankHighThermistorSmOnly] && (!m_Flooding[i]) &&
						(1 == m_ParamsMgr->ActiveThermistors[currTankHighThermistor].Value()))
				   && (0 == m_Resins[i].compare(m_ParamsMgr->TypesArrayPerChamber[ci].Value().c_str()))
				   && (! (m_ParamsMgr->MRW_IsSegmentEconomy[GetSegmentIndex(TankToStaticChamber((TTankIndex)m_ParamsMgr->ActiveTanks[ci].Value()))].Value() && !m_Economy[i]))
					 )
					  continue;
				  //== End bug #8827/8852 fix (2/2)
			   }
		   }
	   }

	   //TODO (Elad): Bug fix - there is a case where compensation is not needed
	   //(relevant to case where flooding can be done both ways, i.e. in Model half, Single-mode)
	   /*   Review these 2 states (left/right, economy/full is irrelevant):
			(1) Chamber 1: VeroGrey (Full); Piping: VeroBlue (Economy)
				Chamber 2: VeroGrey; Piping: VeroGrey.  Single,Flooding from M2.
			(2) Same as (1), but TangoBlack instead of VeroGrey.

			We want to m.r. to VeroBlue (Economy), Single-Flooding from M1.
			m_Cycles is 0 because the pipes don't need a change (VeroBlue->VeroBlue).
			However,
			(1) VeroGrey->VeroBlue is 0, because Full->Economy is 0 for the same resin-family
				Hence, we don't need compensation cycles.
			(2) TangoBlack->VeroBlue is NOT 0. Hence, we DO need compensation cycles to clean the block

			The thumb rule should be to skip compensation if the (!isSectionThermChange) //not IsTankOpModeChanged
			is true and 0 is #cycles to m.r. from the tank's static-chamber's resin
			to the tank's resin. 
	   */

	   //If both cartridges are involved in the M.R., cycles from just 1 cartridge
	   //   are suffice upon thermistors (only) change.
	   //And if these 2 cartridges have '0' cycles:
	   //   The first cartridge that is checked for compensating - will have compensation cycles added to.
	   //   Now, when the other cartridge is checked, it will report that its sibling already has >0 cycles, and will therefor have no compensation cycles added to.

	   TTankIndex mySibTank = GetSiblingTank((TTankIndex)i);
	   bool chkSingleBothTank = ( (!m_Active[mySibTank]) || (m_Active[mySibTank] && (0 == m_Cycles[mySibTank])) );

	   if ((0 == m_Cycles[i]) && m_Active[i] && chkSingleBothTank)
	   {
			//if (0==wCycThermistorsChangeOnly) then all the chamber's tanks will be set 
			m_Cycles[i] = wCycThermistorsChangeOnly;
			CQLog::Write(LOG_TAG_GENERAL, QFormatStr(
				"Therm. OpMode at Block's %s-side has changed, adding compensation cycles to %s",
				IsTankInSupportBlockHalf((TTankIndex)i) ? "support" : "model",
				TankToStr(static_cast<TTankIndex>(i)).c_str()));
	   }
   }
}

bool COptimalTanks::GetFloodingState(TTankIndex i)
{
	if (!m_Active[i]) return false;
	TThermistorsOperationMode therm = GetThermistorsOpMode();
	CScopeRestorePoint<int> scThermOpMode(m_ParamsMgr->ThermistorsOperationMode);
	//update op mode, to access correct ActiveThermistors values
	m_ParamsMgr->ThermistorsOperationMode = therm;
	//Look at the relevant high thermistor's state. If it's lit - we're flooding
	return (1 == m_ParamsMgr->ActiveThermistors[GetTankRelatedHighThermistor(i)]);
}
//---------------------------------------------------------------------------

