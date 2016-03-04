
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
	STATUS_UNAUTHENTICATED,
	STATUS_EMPTY //always leave it the last one and don't load any icon to the imagelist
};

enum TPumpPhase
{
	PHASE_SHORT,
	PHASE_LONG,
	NO_PHASE
};

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
	bool         m_StartButtonState;
	bool         m_CanStartReplacement;
	int          m_EtaProgress;
	int			 m_ProgressPercentage;
	bool         m_ReplacementCompleted;
	bool 		 m_validTanks;
	
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
												   m_validTanks(false)
	{}

    ~CResinSelectionSummaryWizardPage() {}

	// Return the example page type
	TWizardPageType GetPageType()
	{
      return wptCustResinSelectionSummaryWizardPage;
	}

	TTanksVector GetTanksVestor(){return m_Tanks;}
	TTanksVector GetPrevTanksVestor(){return m_PrevTanks;}

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

};

#endif
