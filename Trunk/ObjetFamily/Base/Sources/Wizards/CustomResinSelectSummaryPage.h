
#ifndef _CUSTOM_RESIN_SELECT_SUMMARY_PAGE_H_
#define _CUSTOM_RESIN_SELECT_SUMMARY_PAGE_H_

#include "WizardPages.h"
#include "GlobalDefs.h"

enum TTankStatus
{
	STATUS_OK      = 0,
	STATUS_WARNING,
	STATUS_ERROR,
	STATUS_EXPIRED,
	STATUS_STABILIZATION_TIME, /* Weight stabilization period */
	STATUS_EMPTY
};

enum TPumpPhase
{
	PHASE_SHORT,
	PHASE_LONG,
	//redundency means we looking at the sibling chamber's redundant cartridges (hot swap x3,x4)
	PHASE_SHORT_REDUNDENCY,
	PHASE_LONG_REDUNDENCY,
	NO_PHASE
};

const int SUMMARY_WASTE_WEIGHT_OK 	= 0;  //there's enough free space in the waste cartridges
//const int SUMMARY_WASTE_WEIGHT_FULL = -1; //no more free space in the waste cartridges
const int SUMMARY_WASTE_WEIGHT_NA 	= -2; //couldn't determine the weight (e.g. cartridges are out)
const int SUMMARY_WASTE_WEIGHT_UNSTABLE = -4; //weight is not stable (i.e. wait)

struct TTankForReplacement
{
	TTankForReplacement() : MarkedForReplacement(false),
							IsFlooding(false),
							TankIndex(0),
							MaterialName(""),
							NeededWeight(0),
							WashingCycles(0),
							PumpingPhase(NO_PHASE),
							Status(STATUS_EMPTY){}

	bool    	MarkedForReplacement;
	bool        IsFlooding;
	int     	TankIndex;
	QString 	MaterialName;
	int     	NeededWeight;
	int     	WashingCycles;
	TPumpPhase	PumpingPhase;
	TTankStatus Status;
};

typedef std::vector <TTankForReplacement> TTanksVector;

const TWizardPageType wptCustResinSelectionSummaryWizardPage = wptCustom + 1983;

class CResinSelectionSummaryWizardPage : public CWizardPage
{
private:

	TTanksVector m_Tanks;
	TTanksVector m_PrevTanks; // for previous status
	int			 m_WasteTanksFreeSpace;
	int			 m_PrevWasteTanksFreeSpace;
	bool         m_StartButtonState;
	bool         m_CanStartReplacement;
	int          m_EtaProgress;
	int			 m_ProgressPercentage;
	bool         m_ReplacementCompleted;
	bool 		 m_validTanks;
	bool		 m_noWarnings;

public:

    CResinSelectionSummaryWizardPage(
							  const QString&        Title,
                              int                   ImageID        = -1,
                              TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
		CWizardPage(Title,ImageID,PageAttributes), m_StartButtonState(false),
												   m_CanStartReplacement(false),
												   m_EtaProgress(0),
												   m_ProgressPercentage(0),
												   m_ReplacementCompleted(false),
												   m_WasteTanksFreeSpace(0),
												   m_PrevWasteTanksFreeSpace(0),
												   m_validTanks(false),
												   m_noWarnings(false)
	{}

    ~CResinSelectionSummaryWizardPage() {}

	// Return the example page type
	TWizardPageType GetPageType()
	{
      return wptCustResinSelectionSummaryWizardPage;
	}

	TTanksVector GetTanksVestor() const {return m_Tanks;}
	TTanksVector GetPrevTanksVestor() const {return m_PrevTanks;}

	int GetWasteTanksFreeSpace() const {return m_WasteTanksFreeSpace;}
	int GetPrevWasteTanksFreeSpace() const {return m_PrevWasteTanksFreeSpace;}

	void SetWasteTanksFreeSpace(int weight)
	{
		 m_PrevWasteTanksFreeSpace = m_WasteTanksFreeSpace;
		 m_WasteTanksFreeSpace = weight;
	}

	void SetTanksVector(TTanksVector &TanksVector)
	{
		m_PrevTanks = m_Tanks;
		m_Tanks     = TanksVector;
	}
	void SetValidTanks(bool valid) { m_validTanks = valid; }
	bool AreTanksValid() { return m_validTanks; }


	void Setup()
	{
		m_Tanks.clear();
		m_PrevTanks.clear();

		m_StartButtonState     = false;
		m_CanStartReplacement  = false;
		m_EtaProgress 		   = 0;
		m_ProgressPercentage   = 0;
		m_ReplacementCompleted = false;
		CancelMessage = DEFAULT_CANCEL_PAGE_MSG;
		m_noWarnings = false;
    }

	void SetStartButtonState(bool Enabled){m_StartButtonState = Enabled;}
	bool GetStartButtonState(){return m_StartButtonState;}
	void MarkStartReplacement(bool mark){m_CanStartReplacement = mark;}
	bool CanStartReplacement(){return m_CanStartReplacement;}
	int GetEta(){return m_EtaProgress;}
	void SetEta(int EtaProgress){m_EtaProgress = EtaProgress;}
	void SetProgressPercentage(int Prog){m_ProgressPercentage = Prog;}
	int GetProgressPercentage(){return m_ProgressPercentage;}
	bool IsReplacementCompleted(){return m_ReplacementCompleted;}
	void SetReplacementCompleted(bool ReplacementCompleted){m_ReplacementCompleted = ReplacementCompleted;}
	void SetRemaningTime(int RemaningTime){m_EtaProgress = RemaningTime;}
	int GetRemainingTime(){return m_EtaProgress;}
	void SetSilentMode(bool SuppressWarnings) { m_noWarnings = SuppressWarnings; }
	bool AreWarningsSuppressed() { return m_noWarnings; }

};

#endif
