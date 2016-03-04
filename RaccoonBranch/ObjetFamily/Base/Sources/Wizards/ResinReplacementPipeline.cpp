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
//---------------------------------------------------------------------------
void LogPermutations(TRankedPermutations& perms, std::vector<TVirtTargetMaterial> targMats, int bestPermIdx);
TTankIndex GetPermTankIndex(TChambersLocation permChamberIdx, TReplacementCartridges permContainerIdx);
TSegmentIndex GetSegmentIndex(TChamberIndex chamber, TReplacementCartridges tankSide);
TSegmentIndex GetSegmentIndex(TChamberIndex chamber);

//Fixed mapping from bit-array of active model-chambers to ThermistorsOpMode
//(000 and 100 masks are Don't-care)
/*This const is, in effect, in charge of flooding the system whenever possible.
  i.e., if the user wants 2 model resins (+1 support) then the outcome of washing
  the chambers according to this pipeline's result is such that the user cannot
  use the 3rd model resin, even if the thermistors' opmode has previously been set
  to LOW. This const determines a new thermistors' opmode which doesn't take into
  consideration the previous opmode (exception: DM1, where we change M3).
 */
const TThermistorsOperationMode s_ActiveModelTanksToThermistorsOpMode[1 << MAX_NUMBER_OF_MODEL_CHAMBERS] =
{ 	NUM_OF_THERMISTORS_MODES,
	HIGH_THERMISTORS_MODE, HIGH_THERMISTORS_MODE, SUPPORT_HIGH_MODEL_LOW_THERMISTORS_MODE,
	NUM_OF_THERMISTORS_MODES, SUPPORT_LOW_MODEL_HIGH_THERMISTORS_MODE, SUPPORT_LOW_MODEL_HIGH_THERMISTORS_MODE,
	LOW_THERMISTORS_MODE };

//ATTENTION: DM1 is currently not supported in TTankOperationMode for chamber 3 ! Refer to GetTanksOpMode()
const TTankOperationMode s_ActiveModelTanksToTanksOpMode[1 << MAX_NUMBER_OF_MODEL_CHAMBERS] =
{                       
	NUM_OF_TANKS_OPERATION_MODES,
	S_M1_ACTIVE_TANKS_MODE,S_M2_ACTIVE_TANKS_MODE,S_M1_M2_ACTIVE_TANKS_MODE,
	NUM_OF_TANKS_OPERATION_MODES,S_M1_M3_ACTIVE_TANKS_MODE,S_M2_M3_ACTIVE_TANKS_MODE,
	S_M1_M2_M3_ACTIVE_TANKS_MODE };

const unsigned int s_factorial[NUMBER_OF_CHAMBERS_LOCATIONS+1] = {0,1,2,6,24};
const unsigned int s_perms[NUMBER_OF_CHAMBERS_LOCATIONS][NUM_OF_CHAMBERS_FACTORIAL][NUMBER_OF_CHAMBERS_LOCATIONS] =
{
	{{0,},},
	{{0,1,}, {1,0,},},
	{{2,0,1,}, {0,2,1,}, {0,1,2,}, {2,1,0,}, {1,2,0,}, {1,0,2,},},
	{ {3,2,0,1}, {3,0,2,1}, {3,0,1,2}, {3,2,1,0}, {3,1,2,0}, {3,1,0,2},
	  {2,3,0,1}, {0,3,2,1}, {0,3,1,2}, {2,3,1,0}, {1,3,2,0}, {1,3,0,2},
	  {2,0,3,1}, {0,2,3,1}, {0,1,3,2}, {2,1,3,0}, {1,2,3,0}, {1,0,3,2},
	  {2,0,1,3}, {0,2,1,3}, {0,1,2,3}, {2,1,0,3}, {1,2,0,3}, {1,0,2,3} }
};

unsigned int GetPermElement(unsigned int permLevel, unsigned int permID, unsigned int permElem)
{
	// check boundaries:
	if (permLevel   >= NUMBER_OF_CHAMBERS_LOCATIONS)
		throw EQException("GetPermElement : bad argument #1");
	if (permID   >  s_factorial[permLevel+1])
		throw EQException("GetPermElement : bad argument #2");
	if (permElem >  permLevel)
		throw EQException("GetPermElement : bad argument #3");

	return s_perms[permLevel][permID][permElem];
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
			return H0_1_SEGMENT;

		case TYPE_CHAMBER_MODEL2:
			return H2_3_SEGMENT;

		case TYPE_CHAMBER_MODEL3:
			return H4_5_SEGMENT;

		default:
			CQLog::Write(LOG_TAG_GENERAL, "Error: Couldn't get segment. Chamber index is out of range");
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

__fastcall CResinReplacementPipeline::CResinReplacementPipeline()
: m_numPermutations(0), m_numBasicConfigs(0), m_optimalPermutation(-1), m_numTargetMaterials(0),
  m_preferredSingleResinName("")
{
	m_pipelineResult = NULL;
	m_tblCycMatrix = NULL;
	m_undefinedStr = LOAD_STRING(IDS_MRW_UNDEFINED);

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
		CQLog::Write(LOG_TAG_GENERAL, QFormatStr("Error: %s", e.GetErrorMsg().c_str() ));
		throw e;
	}

	try {
		AnsiString matrixPath = AnsiString(
			(Q2RTApplication->AppFilePath.Value() + LOAD_STRING(IDS_CONFIGS_DIRNAME)
							+ RESIN_TOCYC_MATRIX_FILENAME).c_str());
		m_tblCycMatrix->Init(matrixPath);
	} catch (EQException& e) {
		CQLog::Write(LOG_TAG_GENERAL, "Error: MRW's resin matrix wasn't found or is corrupted.");
		throw EQException("MRW's resin matrix wasn't found or is corrupted.");
	} catch (...) {
		CQLog::Write(LOG_TAG_GENERAL, "Error: MRW's resin matrix wasn't found or is corrupted.");
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
	m_preferredSingleResinName = "";

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

void CResinReplacementPipeline::AddTargetMaterial(int index, TVirtTargetMaterial& demand)
{
	//limit vector to size NUMBER_OF_CHAMBERS. objects positions are crucial here
	if (NUMBER_OF_CHAMBERS <= m_numTargetMaterials)
		throw EQException("MRW Pipeline : number of target material exceeded.");;

	m_targetMaterials.push_back(demand); //[index] = demand;
	++m_numTargetMaterials;
}

void CResinReplacementPipeline::GenerateBasicConfigs()
{
/* Defines what chambers are active in the process of deciding where to put
 * the new required resins during M.R.; Note that false doesn't always say "flooded",
 * but sometimes (DM1) means only "inactive".
 * This data presentation is easier to understand than using hex and bitwise ops to
 * determine if a chamber is true/false.
 * Order of chambers (according to .h file), left to right: Support, M1, M2, M3
 */
	switch(m_targetReplacementMode)
	{
		case mrwSingle:
			m_numBasicConfigs = 2;
			m_basicConfigs.push_back(new CBlockConfiguration(false, true, false, false));
			m_basicConfigs.push_back(new CBlockConfiguration(false, false, true, false));
			break;
		case mrwDM1:
			m_numBasicConfigs = 3;
			m_basicConfigs.push_back(new CBlockConfiguration(false, true, false, false));
			m_basicConfigs.push_back(new CBlockConfiguration(false, false, true, false));
			m_basicConfigs.push_back(new CBlockConfiguration(false, false, false, true));
			break;
		case mrwDM2:
			m_numBasicConfigs = 2;
			//m_basicConfigs.push_back(new CBlockConfiguration(false, true, true, false));
			m_basicConfigs.push_back(new CBlockConfiguration(false, true, false, true));
			m_basicConfigs.push_back(new CBlockConfiguration(false, false, true, true));
			break;
		case mrwDM3:
			m_numBasicConfigs = 1;
			m_basicConfigs.push_back(new CBlockConfiguration(false, true, true, true));
			break;
		default:
			QMonitor.Print("Configs pipeline must define a valid replacement mode first.");
			throw EQException("MRW Pipeline : Configs pipeline must define a valid replacement mode first.");
	}
}

void CResinReplacementPipeline::ForceSystemMaterialsConstraints()
{
	/* [@Deprecated] Already done via ResinReplacementNewWizard. The Support resin is being sent along with the Model resins
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
	
	m_optimizedForPreferredSingleResin = false;
}

void CResinReplacementPipeline::GenerateConstrainedConfigsAndRank()
{
	assert(m_tblCycMatrix);

	// Iterate through the basic configurations
	for (int basicConf = 0; basicConf < m_numBasicConfigs; basicConf++)
	{
		int permsNum			=
			GetPermsNumAtLevel(m_basicConfigs[basicConf]->GetNumActiveChambers());

		// Iterate through the permutations' templates for a given basic config
		for (int permIndex = 0; permIndex < permsNum; permIndex++)
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

	// Prepare to speed-up last-minute optimizations for user's comfort,
	// GetOptCartForUsersComfort()
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
		OptimizeForPreferredSingleMaterial();
		OptimizeForPreservingTankLocation();
		
		LogPermutations(m_permutations, m_targetMaterials, m_optimalPermutation);
	} catch (EQException& e) {
		m_CurrInsertedTanks.clear();
		CQLog::Write(LOG_TAG_GENERAL, "Error in ranking the pipeline's permutations");
		CQLog::Write(LOG_TAG_GENERAL, QFormatStr("%s", e.GetErrorMsg().c_str()));
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

void CResinReplacementPipeline::OptimizeForPreferredSingleMaterial()
{
	m_optimizedForPreferredSingleResin = false;

	if (0 == m_preferredSingleResinName.compare(""))
		return; 

	int xtPreferredPermIndex = 0;
	const int currBestPermRank = m_permutations[m_optimalPermutation]->GetRank();
		
	for (TRankedPermutations::iterator itrRankedPermIdx = m_permutations.begin();
		 itrRankedPermIdx != m_permutations.end(); ++itrRankedPermIdx)
	{
		CRankedBlock* thisPerm = (*itrRankedPermIdx);
		if (currBestPermRank == thisPerm->GetRank()) {
			//check if the permutation contains the preferred material in chambers 1 or 2
			CRankedPipingChamber& c1 = thisPerm->GetChamber(RIGHT_MOST);
			CRankedPipingChamber& c2 = thisPerm->GetChamber(RIGHT_MIDL);
			if (c1.IsValidTag())  {
				if (0 == m_preferredSingleResinName.compare(m_targetMaterials[c1.GetTag()].SelectedResin))
					m_optimizedForPreferredSingleResin = true;
					break;
			}
			else if (c2.IsValidTag()) {
				if (0 == m_preferredSingleResinName.compare(m_targetMaterials[c2.GetTag()].SelectedResin))
					m_optimizedForPreferredSingleResin = true;
					break;
			}
		}
		xtPreferredPermIndex++;
	}
	
	if (true == m_optimizedForPreferredSingleResin)
		m_optimalPermutation = xtPreferredPermIndex;
	
	return;
}

void CResinReplacementPipeline::OptimizeForPreservingTankLocation()
{
	if (true == m_optimizedForPreferredSingleResin)
		return; // We are currently not supporting the combination of both "Preferred material for Single" and "Preserve tank location" optimizations together.

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
		m_optimalPermutation = bestPerm;
}


COptimalTanks* CResinReplacementPipeline::AutoRunPipeline(TResinsReplMode targMode)
{
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

			/* WARNING: Do not exclude the support chamber from being marked for replacement.
			   Later on, when parameters need to be updated, neither Support nor M3 chamber
			   might be updated => Inconsistency error. */

			//Declare settings for left pump/tank
			if (!(RIGHT_CART == location)) {
				m_pipelineResult->SetOptimization(
					 GetPermTankIndex(static_cast<TChambersLocation>(j), LEFT_CART),
					 chamResin , leftCyc, flushMode, isFlood);
			}

			//Declare settings for right pump/tank
			if (!(LEFT_CART == location)) {
				m_pipelineResult->SetOptimization(
					 GetPermTankIndex(static_cast<TChambersLocation>(j), RIGHT_CART),
					 chamResin , rightCyc, flushMode, isFlood);
			}
		}
	}
	m_pipelineResult->RefreshAndApplyCompensation(); //Mandatory(!) after all the opt'd tanks have been set
	return m_pipelineResult;
}

void LogPermutations(TRankedPermutations& perms, std::vector<TVirtTargetMaterial> targMats, int bestPermIdx)
{
	const char* EMPTY_CHAMBER = "EMPTY";

	int permID = 0;
	//QFormatStr() fixes problem (garbage strings) when logging more than 1 formatted argument
	CQLog::Write(LOG_TAG_GENERAL,
		QFormatStr("MRW's possible resins placements (%s, %s, %s, %s) [Rank]",
		ChamberToStr(TYPE_CHAMBER_SUPPORT).c_str(),
		ChamberToStr(TYPE_CHAMBER_MODEL3).c_str(),
		ChamberToStr(TYPE_CHAMBER_MODEL2).c_str(),
		ChamberToStr(TYPE_CHAMBER_MODEL1).c_str())
	);

	for (TRankedPermutations::iterator itr = perms.begin(); itr != perms.end(); ++itr)
	{
		CRankedBlock* perm = (*itr);
		CQLog::Write(LOG_TAG_GENERAL,
			QFormatStr("MRW Permutation #%02d: (%s, %s, %s, %s) [Rank=%d]",
			++permID,
			(perm->GetChamber(LEFT_MOST).IsValidTag()) ?
				targMats[perm->GetChamber(LEFT_MOST).GetTag()].SelectedResin.c_str() : EMPTY_CHAMBER,
			(perm->GetChamber(LEFT_MIDL).IsValidTag()) ?
				targMats[perm->GetChamber(LEFT_MIDL).GetTag()].SelectedResin.c_str() : EMPTY_CHAMBER,
			(perm->GetChamber(RIGHT_MIDL).IsValidTag()) ?
				targMats[perm->GetChamber(RIGHT_MIDL).GetTag()].SelectedResin.c_str() : EMPTY_CHAMBER,
			(perm->GetChamber(RIGHT_MOST).IsValidTag()) ?
				targMats[perm->GetChamber(RIGHT_MOST).GetTag()].SelectedResin.c_str() : EMPTY_CHAMBER,
			perm->GetRank())
		);
	}

	CQLog::Write(LOG_TAG_GENERAL, "MRW's possible resins placements -- End");

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
			
		CQLog::Write(LOG_TAG_GENERAL,
			QFormatStr("Permutation #%02d was chosen for M.R. (%s, %s, %s, %s) [Rank=%d]",
			bestPermIdx + 1,
			(0 <= validCham[LEFT_MOST]) ? targMats[validCham[LEFT_MOST]].SelectedResin.c_str() : EMPTY_CHAMBER,
			(0 <= validCham[LEFT_MIDL]) ? targMats[validCham[LEFT_MIDL]].SelectedResin.c_str() : EMPTY_CHAMBER,
			(0 <= validCham[RIGHT_MIDL])? targMats[validCham[RIGHT_MIDL]].SelectedResin.c_str() : EMPTY_CHAMBER,
			(0 <= validCham[RIGHT_MOST])? targMats[validCham[RIGHT_MOST]].SelectedResin.c_str() : EMPTY_CHAMBER,
			perm->GetRank())
		);

		CRankedPipingChamber& Chamber_0 = perm->GetChamber(LEFT_MOST);
		CRankedPipingChamber& Chamber_3 = perm->GetChamber(LEFT_MIDL);
		CRankedPipingChamber& Chamber_2 = perm->GetChamber(RIGHT_MIDL);
		CRankedPipingChamber& Chamber_1 = perm->GetChamber(RIGHT_MOST);
		if (0 < perm->GetRank()) {
			CQLog::Write(LOG_TAG_GENERAL,
				QFormatStr("Non-compensating washing cycles [normalized] : (%d [%d], %d [%d], %d [%d], %d [%d])",
				Chamber_0.GetTotalCycles(), Chamber_0.GetNormalizedCycles(Chamber_0.GetTotalCycles()),
				Chamber_3.GetTotalCycles(), Chamber_3.GetNormalizedCycles(Chamber_3.GetTotalCycles()),
				Chamber_2.GetTotalCycles(), Chamber_2.GetNormalizedCycles(Chamber_2.GetTotalCycles()),
				Chamber_1.GetTotalCycles(), Chamber_1.GetNormalizedCycles(Chamber_1.GetTotalCycles())
			));
		}
		CQLog::Write(LOG_TAG_GENERAL,
			QFormatStr("Flushing type: (%s, %s, %s, %s)",
			(0 <= validCham[LEFT_MOST]) ? targMats[validCham[LEFT_MOST]].PurposeToString() : EMPTY_CHAMBER,
			(0 <= validCham[LEFT_MIDL]) ? targMats[validCham[LEFT_MIDL]].PurposeToString() : EMPTY_CHAMBER,
			(0 <= validCham[RIGHT_MIDL])? targMats[validCham[RIGHT_MIDL]].PurposeToString() : EMPTY_CHAMBER,
			(0 <= validCham[RIGHT_MOST])? targMats[validCham[RIGHT_MOST]].PurposeToString() : EMPTY_CHAMBER)
		);
		CQLog::Write(LOG_TAG_GENERAL,
			QFormatStr("Cartridges to replace: (%s, %s, %s, %s)",
			(0 <= validCham[LEFT_MOST]) ? targMats[0].CartridgesToString(
				perm->GetChamber(LEFT_MOST).GetAssignedCartridges()) : EMPTY_CHAMBER,
			(0 <= validCham[LEFT_MIDL]) ? targMats[0].CartridgesToString(
				perm->GetChamber(LEFT_MIDL).GetAssignedCartridges()) : EMPTY_CHAMBER,
			(0 <= validCham[RIGHT_MIDL])? targMats[0].CartridgesToString(
				perm->GetChamber(RIGHT_MIDL).GetAssignedCartridges()) : EMPTY_CHAMBER,
			(0 <= validCham[RIGHT_MOST])? targMats[0].CartridgesToString(
				perm->GetChamber(RIGHT_MOST).GetAssignedCartridges()) : EMPTY_CHAMBER)
		);
	}
}


int CResinReplacementPipeline::GetNumCycles(QString fromResinName, QString toResinName,
				 TReplacementPurpose tintOrProp, TTubeType desiredKeyProperty,
				 bool restrictSecondFlushing)
{
#if defined EMULATION_ONLY
	return 0;	//bypass the use of the Matrix

#else
	if ((fromResinName == toResinName) && (restrictSecondFlushing))
		return 0;
	else
	{
		AnsiString fromMat, toMat;
		fromMat = AnsiString(fromResinName.c_str());
		toMat = AnsiString(toResinName.c_str());

		return m_tblCycMatrix->GetCycles(
			fromMat, toMat, desiredKeyProperty,
			(TINT_CYCLES == tintOrProp) ? tFlushTint : tFlushMechanical );
	}
#endif
}

void CResinReplacementPipeline::Rank(CRankedBlock* permutation)
{
	int overallBlockCycles = 0;

	for (int chamLocation = 0; chamLocation < NUMBER_OF_CHAMBERS_LOCATIONS; chamLocation++)
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
//   CQLog::Write(LOG_TAG_GENERAL, QFormatStr("[Info] Ranking -- Chamber = %s ; isFlooding? = %s",
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
	unsigned int activeChambersLastIndex = activeChambersNum - 1; //0-based
	//holds the index of the target material associated with a current location in a permutation.
	unsigned int nextLinkedTargMaterialIdx;

	CRankedBlock* currPerm = new CRankedBlock(c);

	//[@Assumption] #active-chambers in permutation match #target-materials
	//[@Assumption] Target Materials' fixed location values (i.e. tags) are aligned
	//				with the actual locations in block, defined by TChambersLocation enum.
	int nextFreeActiveChamber = 0;
	for (unsigned int j = 0; j < activeChambersNum; j++)
	{
		int currPermChamberIdx;

		//fetch the target material's index to be placed in the next
		//available location in the current permutation's block of chambers.
		 nextLinkedTargMaterialIdx = GetPermElement(
		 			activeChambersLastIndex, permID, j);

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
			   CQLog::Write(LOG_TAG_GENERAL, "Warning: MRW Pipeline: Two target materials were assigned to the same chamber (%d)", targetChamberIdx);
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

CBlockConfiguration::CBlockConfiguration(bool actSupport, bool actM1, bool actM2, bool actM3)
: m_numActiveChambers(0), m_floodingState(INVALID_FLOODING_STATE)
{
	SetActive(LEFT_MOST , actSupport);  // Support
	SetActive(RIGHT_MOST, actM1);  		// M1
	SetActive(RIGHT_MIDL, actM2);  		// M2
#ifdef OBJET_MACHINE
	SetActive(LEFT_MIDL , actM3);  		// M3/Support
#endif

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
	m_Block = new CRankedPipingChamber[NUMBER_OF_CHAMBERS_LOCATIONS]; //==NUMBER_OF_CHAMBERS
}
CRankedBlock::~CRankedBlock()
{
	Q_SAFE_DELETE_ARRAY(m_Block);
}

//---------------------------------------------------------------------------

COptimalTanks::COptimalTanks() : m_dirty(false), m_AutoRecheckFlooding(true)
{
	Clear();
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

	m_dirty = true;
}

void COptimalTanks::SetOptimization(int tankIdx, QString tankName, int tankCycles, bool economyFlush, bool isFlooding)
{
	SetOptimization(tankIdx, tankName, tankCycles, economyFlush);

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
	return FLOODING_NORMALIZATION_FACTOR(IsFlooding(i), m_ParamsMgr);
}

bool COptimalTanks::IsRecoveryFromHswNeeded() const
{
	return (m_ParamsMgr->HSW_WithinWizard.Value());
}

TThermistorsOperationMode COptimalTanks::GetThermistorsOpMode()
{
	//use cache if possible. NUM_OF_THERMISTORS_MODES serves as an invalid OpMode
	if (m_dirty)
	{
		int key = 0; //bit array order: M3 (MSB), M2, M1 (LSB)
		const int BASE_INDEX = FIRST_MODEL_TANK_TYPE;
		//iterate through all Model chambers (only), setting up corresponding
		//bits if they are active.
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

		if (0 == key)
		{
			CQLog::Write(LOG_TAG_GENERAL, "Error: Pipeline's GetThermistorsOpMode :: invalid state - no model chamber was activated");
			throw EQException("Error - GetThermistorsOpMode - no model chamber was activated");
 		}
		else if (4 == key) {
			//In case the active chambers' mask is (1)100
			//(i.e., Support&M3 are active, as in DM1),
			//we need to maintain the current thermistor op mode for chambers 1&2.
			return (0 == m_ParamsMgr->ActiveThermistors[M1_M2_CHAMBER_THERMISTOR]) ?
			   LOW_THERMISTORS_MODE	: SUPPORT_LOW_MODEL_HIGH_THERMISTORS_MODE;
		}

		//get a fixed thermistors op mode, depending on mask
		m_ThermistorsOpMode = s_ActiveModelTanksToThermistorsOpMode[key];
		m_dirty = false;
	}
	
	return m_ThermistorsOpMode;
}

//Go through all the active tanks and mark their respective chambers.
//Then, use this bit array (without the Support chamber) to retrieve the tanks operation mode.
TTankOperationMode COptimalTanks::GetTanksOpMode() const
{        
	int key = 0; //bit array order: M3 (MSB), M2, M1 (LSB)
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

	if (0 == key)
	{
		CQLog::Write(LOG_TAG_GENERAL, "Error: Pipeline's GetTanksOpMode :: invalid state - no model chamber was activated");
		throw EQException("Error - Pipeline's GetTanksOpMode :: invalid state - no model chamber was activated");
	}
	else if (4 == key) {
		CQLog::Write(LOG_TAG_GENERAL, "Error: Pipeline's GetTanksOpMode :: DM1 is not supported for Chamber 3");
		throw EQException("Error - Pipeline's GetTanksOpMode :: DM1 is not supported for Chamber 3");
	}

	//get a fixed tanks op mode, depending on mask
	return s_ActiveModelTanksToTanksOpMode[key];
}

TThermistorsOperationMode COptimalTanks::IsModelOrSupportThermOpModeChanged(bool &ModelThermChanged, bool &SupportThermChanged)
{
	/* If the Thermistors OpMode should be changed from the current one,
	 and number of cycles is zero in a marked-for-replacement tank -
	 fix the number of cycles.
	 Logic:
		Consider the case where user switches DM3->Single->DM3 without
	 changing the resins. A washing cycle must be done to fill the once-flooded
	 chamber with correct resin.
		Also, if system needs to flood, it should
	 consider the resin in the flooded chamber, because it might be more difficult
	 to clean than the resin in the flooding chamber.
   */
   TThermistorsOperationMode newTherm  = GetThermistorsOpMode();
   TThermistorsOperationMode currTherm = (TThermistorsOperationMode)m_ParamsMgr->ThermistorsOperationMode.Value();

   // Check whether there is a thermistor change specifically in the Left part of the block (S & M3), and in the Right side of the block (M1 & M2).
	const bool LOW  = false;
	const bool HIGH = true;
	bool currSupportTherm = LOW;
	bool currModelTherm   = LOW;
	bool newSupportTherm  = LOW;
	bool newModelTherm    = LOW;

	switch (currTherm) {
		case HIGH_THERMISTORS_MODE:								currSupportTherm = HIGH; currModelTherm = HIGH; break;
		case SUPPORT_HIGH_MODEL_LOW_THERMISTORS_MODE:			currSupportTherm = HIGH; currModelTherm = LOW;  break;
		case SUPPORT_LOW_MODEL_HIGH_THERMISTORS_MODE:			currSupportTherm = LOW;  currModelTherm = HIGH; break;
		case LOW_THERMISTORS_MODE:								currSupportTherm = LOW;  currModelTherm = LOW;  break;
	default:
		CQLog::Write(LOG_TAG_GENERAL, "Error: Pipeline's Refresh :: invalid state");
		throw EQException("Error - Pipeline's Refresh :: invalid state");
	}

	switch (newTherm) {
		case HIGH_THERMISTORS_MODE:								newSupportTherm = HIGH; newModelTherm = HIGH; break;
		case SUPPORT_HIGH_MODEL_LOW_THERMISTORS_MODE:			newSupportTherm = HIGH; newModelTherm = LOW;  break;
		case SUPPORT_LOW_MODEL_HIGH_THERMISTORS_MODE:			newSupportTherm = LOW;  newModelTherm = HIGH; break;
		case LOW_THERMISTORS_MODE:								newSupportTherm = LOW;  newModelTherm = LOW;  break;
	default:
		CQLog::Write(LOG_TAG_GENERAL, "Error: Pipeline's Refresh :: invalid state");
		throw EQException("Error - Pipeline's Refresh :: invalid state");
	}

	ModelThermChanged   = (currModelTherm   != newModelTherm);
	SupportThermChanged = (currSupportTherm != newSupportTherm);

	return newTherm;
}

bool COptimalTanks::IsModelTankOpModeChanged()
{
	bool ret = false;

	TTankOperationMode newTanksOpMode  = GetTanksOpMode();
	TTankOperationMode currTanksOpMode = (TTankOperationMode)m_ParamsMgr->TanksOperationMode.Value();

	switch(currTanksOpMode)
	{
		case S_M1_ACTIVE_TANKS_MODE:
		case S_M1_M3_ACTIVE_TANKS_MODE:
			if(newTanksOpMode == S_M2_ACTIVE_TANKS_MODE || newTanksOpMode == S_M2_M3_ACTIVE_TANKS_MODE)
				ret = true;
			break;

		case S_M2_ACTIVE_TANKS_MODE:
		case S_M2_M3_ACTIVE_TANKS_MODE:
			if(newTanksOpMode == S_M1_ACTIVE_TANKS_MODE || newTanksOpMode == S_M1_M3_ACTIVE_TANKS_MODE)
				ret = true;
			break;	
	}
	return ret;
}
							
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

   bool ModelThermChange, SupportThermChange;
   TThermistorsOperationMode newState = IsModelOrSupportThermOpModeChanged(ModelThermChange, SupportThermChange);

	//Waste chamber's index; irrelevant for compensation
   TChamberIndex hswRecoveryChamberIndex = LAST_CHAMBER_TYPE_INCLUDING_WASTE;
   if (IsRecoveryFromHswNeeded())
   {
	 #ifdef OBJET_MACHINE //i.e. MODEL 3's CHAMBER is defined
		//if we are going to use the high thermistor in the support half,
		//then we may need to compensate for the support chamber in order to re-flood M3 chamber.
		//With low thermistor - we need to compensate for dirty M3 chamber since the last HSW. 
	   if ((HIGH_THERMISTORS_MODE == newState) || (SUPPORT_HIGH_MODEL_LOW_THERMISTORS_MODE == newState))
		   hswRecoveryChamberIndex = TYPE_CHAMBER_SUPPORT;
	   else
		   hswRecoveryChamberIndex = TYPE_CHAMBER_MODEL3;
	 #endif
   }

#if defined EMULATION_ONLY
   int wCycThermistorsChangeOnly = 0;
#else
   int wCycThermistorsChangeOnly = m_ParamsMgr->MRW_CompensationCyclesThermistorsChange.Value();
#endif

   for (int i = FIRST_TANK_TYPE; i < TOTAL_NUMBER_OF_CONTAINERS; ++i) {

		//If the Hsw didn't end well, then the parameters for specific tank(s) do not
		//reflect the conditions in the block (i.e. the right thermistor is bogus) -- FORCE compensation

	   TTankIndex ii = static_cast<TTankIndex>(i);
	   if (hswRecoveryChamberIndex == TankToStaticChamber(ii))
	   {
		   CQLog::Write(LOG_TAG_GENERAL, "Compensation is forced (Hsw recovery)");
	   } else
	   {
		   // If this container is feeding a "half-of-block" that had no "thermistor change" - skip this container.
		   if (IsTankInSupportBlockHalf(ii))
		   {
			  if (! SupportThermChange)
				continue;

			   //== Filter-out specific cases from compensating them. See below. bug #8827/8852
//TankToChamber (dynamic) might not result with a valid value in 2-model chambers machines.
#ifndef LIMIT_TO_DM2   //Just to be safe. May be uncommented when checked on Objet1000 for instance.
				  if ((0 == m_Cycles[i]) && m_Active[i] && (SupportThermChange && (!m_Flooding[i]) &&
						(true == m_ParamsMgr->ActiveThermistors[GetTankRelatedHighThermistor(ii)].Value()))
				   && (0 == m_Resins[i].compare(m_ParamsMgr->TypesArrayPerChamber[TankToStaticChamber(ii)].Value().c_str()))
				   && (! (m_ParamsMgr->MRW_IsSegmentEconomy[GetSegmentIndex(TankToChamber(ii))].Value() && !m_Economy[i]))
					 )
					  continue;
#endif
			  //== End bug #8827/8852 fix (1/2)

		   } else
		   if (IsTankInModelBlockHalf(ii))
		   {
			  if (! ModelThermChange && ! IsModelTankOpModeChanged())
				continue;

			  //== Filter-out specific cases from compensating them. bug #8827/8852
			  //if     (1) thermistors are to be changed from high to low, in models-half of the block,
			  //   AND (2) curr. resin in this tank's chamber (in the block itself)
			  //           is the same as the new proposed resin (m_Resins),
			  //   AND NOT (3) the currently active flooding chamber in M1-M2 contains a Full-typed resin in its long-pipe,
			  //               and the target chamber will be filled using Economy-mode
			  //			(i.e. everything but Economy->Full is ok and can be skipped)
			  //   AND (4) number of washing cycles is zero and is marked for replacement
			  //=> don't add compensation cycles
			  if ((0 == m_Cycles[i]) && m_Active[i] && (ModelThermChange && (!m_Flooding[i]) &&
					(true == m_ParamsMgr->ActiveThermistors[GetTankRelatedHighThermistor(ii)].Value()))
			   && (0 == m_Resins[i].compare(m_ParamsMgr->TypesArrayPerChamber[TankToStaticChamber(ii)].Value().c_str()))
			   && (! (m_ParamsMgr->MRW_IsSegmentEconomy[GetSegmentIndex(TankToChamber(ii))].Value() && !m_Economy[i]))
				 )
				  continue;
			  //== End bug #8827/8852 fix (2/2)
		   }
	   }
	   
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


