//---------------------------------------------------------------------------
#ifndef _ResinReplacement_Pipeline_H_
#define _ResinReplacement_Pipeline_H_

#include "GlobalDefs.h"
#include "AppParams.h"
#include "CustomResinMainSelectPage.h" //TVirtTargetMaterial; TChambersLocation
#include "MRWMaterialMatrix.h" //Table of Cycles (XML Support)

#define NO_TARGET_MATERIAL_ASSOCIATED -1
#define ROUNDUP_INT(x) (ceil(x))

typedef enum {
	mrwSingle = 0,
	mrwDM1, mrwDM2, mrwDM3,
	mrwUndefined
} TResinsReplMode;

const unsigned int NUM_OF_CHAMBERS_FACTORIAL = 24; //(#chambers)!


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

		QString GetBlockResin(){ return m_resinName; 	 }
		QString GetLongPipe()  { return m_longPipe; 	 }
		QString GetLeftPipe()  { return m_shortLeftPipe; }
		QString GetRightPipe() { return m_shortRightPipe;}

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

	int GetLeftCycles() const { return m_leftCycles; }
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

	//TODO (Elad): Consider passing a 'vector' of booleans if number of arguments grow
	//Note the order of the Model booleans. It's not necessary like in the machine itself!
	CBlockConfiguration		(bool actSupport, bool actM1, bool actM2, bool actM3);
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

	bool 	m_Active	[TOTAL_NUMBER_OF_CONTAINERS];
	QString m_Resins	[TOTAL_NUMBER_OF_CONTAINERS];
	int 	m_Cycles	[TOTAL_NUMBER_OF_CONTAINERS];
	bool	m_Flooding	[TOTAL_NUMBER_OF_CONTAINERS];
	bool	m_Economy	[TOTAL_NUMBER_OF_CONTAINERS];

	bool	m_dirty;
	TThermistorsOperationMode	m_ThermistorsOpMode;
	TTankOperationMode 			m_TanksOpMode;

	//if this flag is set then the flooding state won't be changed manually
	bool	m_AutoRecheckFlooding;

	CAppParams *m_ParamsMgr;
//	static TThermistorsOperationMode s_ActiveModelTanksToThermistorsOpMode[];
//	static TTanksOperationMode s_ActiveModelTanksToTanksOpMode[];

private:
	bool  GetFloodingState		(TTankIndex i);
	float GetNormalizationFactor	(TTankIndex i) const;
	TThermistorsOperationMode IsModelOrSupportThermOpModeChanged(bool &ModelThermChanged, bool &SupportThermChanged);
	bool IsModelTankOpModeChanged();
	//Returns true if i should be affected by the HSW recovery (within MRW)
	bool IsHswRecoveryRelated(TTankIndex i) const;

public:
	COptimalTanks();
	~COptimalTanks();

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
	//Returns whether or not the M3/S chamber should be specially treated due to HSW cancellation beforehand
	bool IsRecoveryFromHswNeeded() const;
		
	TThermistorsOperationMode	GetThermistorsOpMode();
	// Derive the TTankOperationMode by combining the 'ThermistorsOpMode' together with the 'IsFlooding'.
	TTankOperationMode			GetTanksOpMode() const;
	//True, if the provided tank should be pumped from before its cross-pumping neighbour.
	TPumpingOrderPriority 		GetPumpQoS	(TTankIndex i) const;
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
	QString					m_preferredSingleResinName;
	QString					m_undefinedStr;
	bool					m_optimizedForPreferredSingleResin;

	typedef std::vector<CBlockConfiguration*> TConfigArray; //TODO: check mem de-alloc.
	TConfigArray			m_basicConfigs;
	int						m_numBasicConfigs;

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

	static  unsigned int s_factorial[];
	static  unsigned int s_perms[NUMBER_OF_CHAMBERS_LOCATIONS][NUM_OF_CHAMBERS_FACTORIAL][NUMBER_OF_CHAMBERS_LOCATIONS];

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
	//Gets number of cycles needed to wash a resin, limited by the LOW thermistor ([@Assumption])
	int GetNumCycles(QString fromResinName, QString toResinName,
				 TReplacementPurpose tintOrProp, TTubeType desiredKeyProperty,
				 bool restrictSecondFlushing);
	void SetFirstBestPermutation();
	void OptimizeForPreferredSingleMaterial(); //if needed, reselects a different best permutation
	void OptimizeForPreservingTankLocation(); //if possible, reselects a different best permutation
    float GetNormalizationFactorToShortCycles(CRankedBlock& perm, TChambersLocation chamber);
	void InitializeMatrix();
	
//	static unsigned int GetPermElement(unsigned int permLevel, unsigned int permID, unsigned int permElem);
//	static unsigned int GetPermsNumAtLevel(unsigned int level);
//  static TTankIndex GetTankIndex(TChambersLocation permChamberIdx, TReplacementCartridges permContainerIdx);

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
	__fastcall CResinReplacementPipeline();
	__fastcall ~CResinReplacementPipeline();

	void Initialize(); //Clear last results and fetch the current machine's piping state
	void ClearResults();//Prepare to repeat the pipeline process using different settings

	void AddTargetMaterial(int index, TVirtTargetMaterial& demand);
	void SetPreferredSingleResin(QString const resinName) { m_preferredSingleResinName = resinName;}

	//Automatically runs all the pipeline steps sequencially and returns an optimal permutation
	COptimalTanks* AutoRunPipeline(TResinsReplMode targMode);
};

//---------------------------------------------------------------------------
#endif

