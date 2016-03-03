//---------------------------------------------------------------------------
#ifndef _ResinReplacement_Pipeline_H_
#define _ResinReplacement_Pipeline_H_

#include "GlobalDefs.h"
#include "AppParams.h"
#include "CustomResinMainSelectPage.h" //TVirtTargetMaterial; TChambersLocation
#include "Q2RTWizard.h"
#include "MRWMaterialMatrix.h" //Table of Cycles (XML Support)

#define NO_TARGET_MATERIAL_ASSOCIATED -1
#define ROUNDUP_INT(x) (ceil(x))

namespace MrwPipeline {
typedef enum {
	mrwDM3, mrwDM6, mrwDM7,
	mrwUndefined = NUMBER_OF_CHAMBERS
} TResinsReplMode;

/*
 * ======= Non-DM6 suitable ========
 * The following version assumes the support is fixed on a specific chamber --
 * Factorials do NOT take the support chamber into consideration.
 */
//#permutations. e.g. 7! for 7 model resins (DM7)
//const unsigned int NUM_OF_CHAMBERS_FACTORIAL_MAX = 5040; //(#all_model_chambers)!
//const unsigned int NUM_OF_CHAMBERS_FACTORIAL_DM3 = 6;
/*
 * ======= Suitable for case where the support resin can be
 *		   located in either 2 left-most chambers ("DM6") ========
 * Factorials take the support chamber into consideration.
 * They are based on the current TChamberLocation - Support, then M1, M2,...M7 order
 */
const unsigned int PERM_FACTORIAL_DM3_1_SUPPORT = 16; //2*3!-2!+3! ; 3! for models * 2 possible location for a fixed support - {0,K[1,2,3]} and {K[0,1,2], 3} minus overlapping permutations, plus 6 cases (trailing 0) where the support chamber can actually be the extra chamber (automatic mode, with 1-2 cartridges) 
const unsigned int PERM_FACTORIAL_DM3_2_SUPPORTS = 12;//2*3! ; 3! for models * 2 positions (support-swap)
const unsigned int PERM_FACTORIAL_MAX_MODELS_1_SUPPORT = 2040; //2*(6!)-(5!)+(6!) ; DM6 w/ 1 support chamber
const unsigned int PERM_FACTORIAL_MAX_MODELS_2_SUPPORTS = 1440; //2*(6!) ; DM6 w/ 2 support chambers
// ========================


const char* COLOR_CYAN 		= "VeroCyan";
const char* COLOR_MAGENTA 	= "VeroMgnt";
const char* COLOR_YELLOW 	= "VeroYellow";

const int ILLEGAL_NUM_OF_CYCLES = 999; //For illegal transitions between certain resins which are prohibited by Chemistry department.
const int FICTIVE_VALUE  = 1000; //For returning a non-realistiv value, just for avoiding calling a function with NULL value.

class CNamedPipingChamber
{
	private:
		QString m_resinName;
		QString m_longPipe;
		QString m_shortLeftPipe, m_shortRightPipe;

	public:
		__fastcall CNamedPipingChamber(QString resinInChamber, QString longPipeResin,
					 QString shortRightPipeResin, QString shortLeftPipeResin);
		__fastcall ~CNamedPipingChamber() {}

		QString GetBlockResin() const { return m_resinName; 	}
		QString GetLongPipe() 	const { return m_longPipe; 	 	}
		QString GetLeftPipe() 	const { return m_shortLeftPipe; }
		QString GetRightPipe() 	const { return m_shortRightPipe;}

		bool IsFlooded();
};

class CRankedPipingChamber
{
	bool	m_toBeFlushed;		// is the chamber participating in current MRW flushing party?
	int		m_demandOrigin;		// holds the origin for the requirement, i.e. Target Material ID
	TReplacementCartridges	m_cartridges; // holds which cartridge is to be flushed
	int		m_leftCycles;		// filled by ranker
	int		m_rightCycles;		// filled by ranker
	bool	m_economyMode;		// filled by ranker
	float	m_floodingFactor;	// filled by ranker; used for cycles normalization

public:
	void SetActive(bool tf) {m_toBeFlushed = tf;}
	bool IsActive () const {return m_toBeFlushed;}

	void SetTag (int ID = NO_TARGET_MATERIAL_ASSOCIATED)
	{
		m_demandOrigin = ID;
		SetActive(NO_TARGET_MATERIAL_ASSOCIATED != ID);
	}
	int  GetTag () const {return m_demandOrigin;}
	bool IsValidTag() const {return NO_TARGET_MATERIAL_ASSOCIATED != m_demandOrigin;}

	TReplacementCartridges GetAssignedCartridges() const { return m_cartridges; }
	void SetCartridges(TReplacementCartridges newVal) { m_cartridges = newVal; }

	void SetLeftCycles(int cycles)
	{
		m_leftCycles	 = cycles;
	}

	void SetRightCycles(int cycles)
	{
		m_rightCycles = cycles;
	}

	void SetFlushingMode(bool isEconomy) { m_economyMode = isEconomy; }
	void SetNormalizationFactor(float newVal = 1.0) { m_floodingFactor = newVal; }

	int GetLeftCycles()  const { return m_leftCycles;  }
	int GetRightCycles() const { return m_rightCycles; }
	int GetTotalCycles() const { return m_leftCycles + m_rightCycles; }
	bool IsEconomyMode() const { return m_economyMode; }
	int GetNormalizedCycles(int cycles = 1) const { return ROUNDUP_INT(m_floodingFactor*cycles); }

	CRankedPipingChamber();
	~CRankedPipingChamber();
};

class CBlockConfiguration
{
	bool  				  m_chambers[NUMBER_OF_CHAMBERS];
	int					  m_numActiveChambers;
	//holds which chamber(s) is-a(are) flooding one(s); Bit array of order TChamberIndex (S,M1,M2,...)
	int					  m_floodingState;

private:
	void SetFloodingChamberIfActive(TChambersLocation chamber);
	void SetActive				   (TChambersLocation chamber, bool active);
	void RecalcFloodingState	   ();
public:

	//Note the order of the Model booleans. It's not necessary like in the machine itself!
	CBlockConfiguration		(bool const chamberIsActive[NUMBER_OF_CHAMBERS]);
	~CBlockConfiguration	();

	bool IsActive				 (TChambersLocation ch) const;
	//A wrapper for private SetActive, that forces refreshing the class members
	void Activate			 	 (TChambersLocation chamber, bool active = true);

	// Getters
	int GetNumActiveChambers () const { return m_numActiveChambers; }

	// Types Conversion
	static TChambersLocation ConvertToChamberLocation(TChamberIndex const c);
	static TChamberIndex ConvertToChamberIndex(TChambersLocation const c);

	bool IsChamberFlooding	(TChambersLocation chamber) const;
};


class CRankedBlock {
private:
	int m_Rank;
	CBlockConfiguration* m_baseCfg;
	CRankedPipingChamber* m_Block;

public:
	//[IMPORTANT!] GetChamber works according to Chambers' locations(!) and not indexes.
	//TChambersLocation might NOT BE equal to TChamberIndex enum!
	CRankedPipingChamber& GetChamber(const TChambersLocation idx) { return m_Block[(int)idx]; }
	int  GetRank	() const 	{ return m_Rank; }
	void SetRank	(int value) { m_Rank = value; }

	bool IsFlooding	(TChambersLocation chamberPos) const
	{
		return m_baseCfg->IsChamberFlooding(chamberPos);
	}

	CRankedBlock	(CBlockConfiguration* _baseCfg);
	~CRankedBlock	();
};

typedef enum { PUMP_FIRST, PUMP_SECOND, DONT_PUMP } TPumpingOrderPriority;
class COptimalTanks {

	QString m_UndefinedStr;
	bool 	m_Active	[TOTAL_NUMBER_OF_CONTAINERS];
	QString m_Resins	[TOTAL_NUMBER_OF_CONTAINERS];
	int 	m_Cycles	[TOTAL_NUMBER_OF_CONTAINERS];
	bool	m_Flooding	[TOTAL_NUMBER_OF_CONTAINERS];
	bool	m_Economy	[TOTAL_NUMBER_OF_CONTAINERS];

//	bool	m_dirty;
//	TThermistorsOperationMode	m_ThermistorsOpMode;
	TTankOperationMode 			m_TanksOpMode;

	//if this flag is set then the flooding state won't be changed manually
	bool	m_AutoRecheckFlooding;

	CAppParams *m_ParamsMgr;
//	static TThermistorsOperationMode s_ActiveModelTanksToThermistorsOpMode[];
//	static TTanksOperationMode s_ActiveModelTanksToTanksOpMode[];
	bool	m_Silent;

private:
	bool  GetFloodingState		(TTankIndex i);
	float GetNormalizationFactor	(TTankIndex i) const;
	void IsModelOrSupportThermOpModeChanged(bool (&IsThermChanged)[(int)NUM_OF_SM_CHAMBERS_THERMISTORS]);
	bool IsTankOpModeChanged(TChamberIndex chamber) const;
	bool IsHswRecoveryRelated(TTankIndex i) const;

public:
	COptimalTanks();
	~COptimalTanks();

	void SetBypassComputations(bool isQuiet) { m_Silent = isQuiet; }
	void SetOptimization(int tankIdx, QString tankName, int tankCycles, bool economyFlush);
	//isFlooding can't have a default value. This is to make sure that the devloper really intends to manually set the flooding state
	void SetOptimization(int tankIdx, QString tankName, int tankCycles, bool economyFlush, bool isFlooding);
	void RefreshAndApplyCompensation();	//Post-processing the optimized tanks
	void Clear();

	bool 	IsActive		(TTankIndex i) const { return m_Active[i]; }
	bool 	IsEconomyFlushed(TTankIndex i) const { return m_Economy[i]; }
	QString GetResin		(TTankIndex i) const { return m_Resins[i]; }
	/* Computes number of washing cycles needed for a tank, taking into consideration
	   that this tank may/may-not be involved in flooding */
	int 	GetCycles		(TTankIndex i) const;
	/* Computes the weight needed for a tank, taking into consideration
	   that this tank may/may-not be involved in flooding */
	int  	GetMinWeight	(TTankIndex i) const;
	/* True, if the given tank's chamber is flooding its sibling chamber.
	   If the tank isn't Active, returns false. */
	bool 	IsFlooding		(TTankIndex i) const { return m_Flooding[i]; }
	bool IsRecoveryFromHswNeeded() const;
	TThermistorsOperationMode	GetThermistorsOpMode();
	// Derive the TTankOperationMode by combining the 'ThermistorsOpMode' together with the 'IsFlooding'.
	TTankOperationMode			GetTanksOpMode() const;
	//True, if the provided tank should be pumped from before its cross-pumping neighbour.
	TPumpingOrderPriority 	GetPumpQoS	(TTankIndex i) const;
};

struct TTankInfo
{
	int Weight;
	QString ResinType;
};

typedef std::vector<CRankedBlock*> TRankedPermutations;
class CResinReplacementPipeline
{
	//m_currState's 0-index = Support chamber, 1-index = Model1, 2-index = Model2...
	std::vector<CNamedPipingChamber*> m_currState; //of max size [NUMBER_OF_CHAMBERS];
	std::vector<TVirtTargetMaterial>  m_targetMaterials; //of max size [NUMBER_OF_CHAMBERS];
	int 					m_numTargetMaterials;
	TResinsReplMode 		m_targetReplacementMode;
	QString					m_undefinedStr;

	typedef std::vector<CBlockConfiguration*> TConfigArray; //TODO: check mem de-alloc.
	TConfigArray			m_basicConfigs;
	int						m_numBasicConfigs;
	bool					m_silentMode;

	//holds data to speed up processing time for GetOptCartForUsersComfort
	std::vector<TTankInfo> m_CurrInsertedTanks;

	//TRankedPermutations' 0-index = Support chamber, 1-index = Model1, 2-index = Model2...
	TRankedPermutations   	m_permutations;
	int						m_numPermutations;
	int						m_optimalPermutation;
	COptimalTanks*          m_pipelineResult;

	CMaterialsMatrix*		m_tblCycMatrix;

	CAppParams*				m_ParamsMgr;
	CBackEndInterface* 		m_BackEnd;

	CQ2RTAutoWizard*        m_OwnerWizard;

	static  unsigned int s_factorial[];
//	static  unsigned int s_perms_3mdl[NUM_OF_CHAMBERS_FACTORIAL_DM3][NUMBER_OF_CHAMBERS_LOCATIONS];
//	static  unsigned int s_perms_max_mdl[NUM_OF_CHAMBERS_FACTORIAL_MAX][NUMBER_OF_CHAMBERS_LOCATIONS];
	static  unsigned int s_perms_3mdl_1support		[PERM_FACTORIAL_DM3_1_SUPPORT]		  [NUMBER_OF_CHAMBERS_LOCATIONS];
	static  unsigned int s_perms_3mdl_fullSupport	[PERM_FACTORIAL_DM3_2_SUPPORTS]		  [NUMBER_OF_CHAMBERS_LOCATIONS];
	static  unsigned int s_perms_max_mdl_1support	[PERM_FACTORIAL_MAX_MODELS_1_SUPPORT] [NUMBER_OF_CHAMBERS_LOCATIONS];
	static  unsigned int s_perms_max_mdl_fullSupport[PERM_FACTORIAL_MAX_MODELS_2_SUPPORTS][NUMBER_OF_CHAMBERS_LOCATIONS];

 private:
 
	void GetCurrentPiping();

	float GetNormalizationFactorToShortCycles(CRankedBlock const* perm, TChambersLocation chamber) const;

	//Returns the cartridge location best suited for user's comfort, when the amount of waste is not an issue
	//The method checks the cartridges of a given chamber, and doesn't offer to change the chamber.  
	TReplacementCartridges GetOptCartForUsersComfort(TChambersLocation chamber, QString& desiredResinType);
	//Creates a permutation, assigning its target materials. returns NULL on error.
	CRankedBlock* AllocateMaterialsToPermutation(CBlockConfiguration* c, unsigned int permID);
	//Calculates washing cycles needed for the entire block.
	//Ranks each chamber separately, overlooking flooding from other chambers.
	//Rank is optimized for Waste-Reduction not for Time-Consumption
	void Rank(CRankedBlock* permutation);
	void SetFirstBestPermutation();
	bool OptimizeForSeparateChambersCMY(); //if possible, reselects a different best permutation
	bool OptimizeForPreservingTankLocation(); //if possible, reselects a different best permutation
	float GetNormalizationFactorToShortCycles(CRankedBlock& perm, TChambersLocation chamber);
	void InitializeMatrix();
	void WriteToLog(const QString str) const;
	void LogPermutations(TRankedPermutations& perms,
		std::vector<TVirtTargetMaterial> targMats, int bestPermIdx, bool verbose) const;

//	static unsigned int GetPermElement(unsigned int permLevel, unsigned int permID, unsigned int permElem);
//	static unsigned int GetPermsNumAtLevel(unsigned int level);
//  static TTankIndex GetTankIndex(TChambersLocation permChamberIdx, TReplacementCartridges permContainerIdx);

	void SetBypassComputations(bool isQuiet);

 protected:
	//Sets the target mode. If the user performs a mix of 2nd-flushing and regular M.R.,
	//it's up to him to pass the correct target mode.
	void SetReplacementMode(TResinsReplMode rm) {m_targetReplacementMode = rm;}
	void GenerateBasicConfigs();
	// this function forces material constraints that must always hold regardless
	// of the user selections. Support on left chamber is the most prominent example.
	void ForceSystemMaterialsConstraints();
	void GenerateConstrainedConfigsAndRank();
	COptimalTanks* GetOptimalPermutation();

 public:
	__fastcall CResinReplacementPipeline(CQ2RTAutoWizard *Wizard = NULL);
	__fastcall ~CResinReplacementPipeline();

	void Initialize(); //Clear last results and fetch the current machine's piping state
	void ClearResults();//Prepare to repeat the pipeline process using different settings

	void AddTargetMaterial(TVirtTargetMaterial& demand);

	//Gets number of cycles needed to wash a resin, limited by the LOW thermistor ([@Assumption])
	int GetNumCycles(QString fromResinName, QString toResinName,
		TReplacementPurpose tintOrProp, TTubeType desiredKeyProperty,
		bool restrictSecondFlushing);
	//Automatically runs all the pipeline steps sequencially and returns an optimal permutation
	COptimalTanks* AutoRunPipeline(TResinsReplMode targMode, bool silent);
};
}

//---------------------------------------------------------------------------
#endif

