
#ifndef _CUSTOM_RESIN_SELECT_PAGES_NOI_H_
#define _CUSTOM_RESIN_SELECT_PAGES_NOI_H_

#include "WizardPages.h"
#include "GlobalDefs.h"
//#include "QUtils.h"

#define INVALID_RESIN_NAME ""

typedef enum {
	DONT_CARE_INDEX = -1
	,LEFT_MOST = 0
	,DEDICATED_SUPPORT_CHAMBER = LEFT_MOST
	,FIRST_CHAMBER_LOCATION = LEFT_MOST
	,RIGHT_MOST
	,RIGHT_MIDL
#ifdef OBJET_MACHINE
	,LEFT_MIDL
#endif
	,NUMBER_OF_CHAMBERS_LOCATIONS //== NUMBER_OF_CHAMBERS (without waste chamber)
	,LAST_MODEL_CHAMBER_LOCATION = NUMBER_OF_CHAMBERS_LOCATIONS - 1
	,FIRST_MODEL_CHAMBER_LOCATION = RIGHT_MOST
} TChambersLocation; //S=0, M1, M2, M3 like in TChamberIndex enum. [Important!] Order may change !

typedef std::vector<QString> TResinsList;
typedef enum { SINGLE_MODE = 0, MULTIPLE_MODE, FORCE_MODE } TPrnMode;
typedef enum { TINT_CYCLES = 0, PROPERTIES_CYCLES, NUM_OPTIONS_PURPOSE} TReplacementPurpose;
//Singles cartridges' (Left/Right) values should start from 0
typedef enum { LEFT_CART = 0, RIGHT_CART, BOTH_CART, SINGLE_CART, MAX_CART_PER_CHAMBER = BOTH_CART} TReplacementCartridges;

class TVirtTargetMaterial
{
public:
	QString        			SelectedResin;
	bool		 			SecondFlushing;
	TReplacementCartridges	FlushingBothCartridges;
	TReplacementPurpose 	FlushingPurpose;
	//if not negative - marks that this object is manually associated with a specific chamber
	//(does NOT necessarily imply SecondFlushing the chamber).
	//The index is correlated with CResinReplacementPipeline's TChambersLocation enum.
	TChambersLocation		FixedLocation;

	TVirtTargetMaterial() :
	  SelectedResin			(INVALID_RESIN_NAME),
	  SecondFlushing		(false),
	  FlushingBothCartridges(BOTH_CART),
	  FlushingPurpose		(TINT_CYCLES),
	  FixedLocation			(DONT_CARE_INDEX)
	{}

	bool IsValid() const {
		//resin is valid if it's not empty in normal mode.
		//It's NOT a complete check if the 2nd flushing option is ON !
		return (0 != std::strcmp(INVALID_RESIN_NAME,SelectedResin.c_str()));
	}
	const char* PurposeToString() const
	{
		switch (FlushingPurpose) {
			case TINT_CYCLES: return "Full";
			case PROPERTIES_CYCLES: return "Economy";
			default: return "ERROR";
		}
	}
	const char* CartridgesToString() const
	{
		return CartridgesToString(FlushingBothCartridges);
	}
	const char* CartridgesToString(TReplacementCartridges t) const
	{
		switch (t) {
			case LEFT_CART: return "Left";
			case RIGHT_CART: return "Right";
			case BOTH_CART: return "Both";
			case SINGLE_CART: return "a Single";
			default: return "ERROR";
		}
    }
	void Init()
	{
		  SelectedResin				= INVALID_RESIN_NAME;
		  SecondFlushing			= false;
		  FlushingBothCartridges	= BOTH_CART;
		  FlushingPurpose			= TINT_CYCLES;
		  FixedLocation				= DONT_CARE_INDEX;
    }
};

const TWizardPageType wptCustResinSelectionWizardPage = wptCustom + 5;

/**
	This class holds the front-end's UI components' (i.e. beauty) properties
	we use for the logic part (i.e. brains) of the Resin ("Material") Replacement wizard.

	Contains merely getters and setters.
**/
class CResinMainSelectionWizardPage : public CWizardPage {
private:
	int _Size; //Typically, this the number of model chambers (i.e. 3 for Objet)

public:
	TResinsList   ModelResinsList;	//contains the list of licensed model resins to choose from
	TResinsList   SupportResinsList;//contains the list of licensed support resins to choose from
	bool*		  ActiveChambers;	//if False then the resin in the long-pipe
									//doesn't match resin in chamber (i.e. flooded by neightbour)  
	int*		  SelectedResinsIdx; //indexes of the selected model resins
	int 		  SelectedSupportResinsIdx; //index of the selected support resin (not using an array if not needed)

	//As long as RestoreDefaults == false, user doesn't see the current machine's status, but his defined settings
	bool		  RestoreDefaults;
	bool		  ManualMode;  //true = target resins' cartridges placements will be decided automatically.
	bool		  ForceFlushingEnabled; //If true, the flushing array is taken into consideration, else it's ignored.
	TPrnMode	  PrintingMode;
	int			  PreferredSingleResin; //index. The resin name is saved in ParamMgr

	TVirtTargetMaterial*	VirtTargMaterials;
	TVirtTargetMaterial		SupportVirtTargMaterial;
	
public:

	int Size() const { return _Size; }
	
	int ResinNameToIndex(QString const name) const
	{
	  int matchedIndex = 0;

	  if (!ModelResinsList.empty()) {
		  //loop through the resins list and find a match
		  for (TResinsList::const_iterator itr = ModelResinsList.begin(); itr != ModelResinsList.end(); ++itr)
		  {
			if (itr->compare(name) == 0)
				return matchedIndex;

			++matchedIndex;
		  }
	  }
	  return 0;
	}

	QString ResinIndexToName(int index) const
	{
		if (index < 0 ) index = 0; //ignore out of bounds index. Use default.
		return ModelResinsList[index];
	}

	int ResinNameToSupportIndex(QString const name) const
	{
	  int matchedIndex = 0;

	  if (!SupportResinsList.empty()) {
		  //loop through the resins list and find a match
		  for (TResinsList::const_iterator itr = SupportResinsList.begin(); itr != SupportResinsList.end(); ++itr)
		  {
			if (itr->compare(name) == 0)
				return matchedIndex;

			++matchedIndex;
		  }
	  }
	  return 0;
	}

	QString ResinIndexToSupportName(int index) const
	{
		if (index < 0 ) index = 0; //ignore out of bounds index. Use default.
		return SupportResinsList[index];
	}

	// Constructor
    CResinMainSelectionWizardPage(
							  const QString&        Title,
                              const int&            Size,
                              int                   ImageID        = -1,
                              TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
		CWizardPage(Title,ImageID,PageAttributes), _Size(Size),
		RestoreDefaults(true),
		PrintingMode(SINGLE_MODE),
		ManualMode(false),
		ForceFlushingEnabled(false),
		SelectedSupportResinsIdx(DONT_CARE_INDEX)
	{
		VirtTargMaterials = new TVirtTargetMaterial	[_Size];
		SelectedResinsIdx = new int					[_Size];
		ActiveChambers	  = new bool				[_Size];
	}

    ~CResinMainSelectionWizardPage()
	{
	  Q_SAFE_DELETE_ARRAY(VirtTargMaterials	);
	  Q_SAFE_DELETE_ARRAY(SelectedResinsIdx	);
	  Q_SAFE_DELETE_ARRAY(ActiveChambers	);
	}

	void Setup()
	{
		 PrintingMode = SINGLE_MODE;
		 ManualMode = false;
		 RestoreDefaults = true;
		 ModelResinsList.clear();
		 SupportResinsList.clear();
		 ForceFlushingEnabled = false;
		 SelectedSupportResinsIdx = DONT_CARE_INDEX;
		 SupportVirtTargMaterial.Init();
		 for (int i=0; i<_Size; ++i)
		 	VirtTargMaterials[i].Init();
	}

	// Return the example page type
	TWizardPageType GetPageType()
	{
      return wptCustResinSelectionWizardPage;
	}

};


#endif	//_CUSTOM_RESIN_SELECT_PAGES_NOI_H_
