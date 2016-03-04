#ifndef _SHR_WIZARD_BASE_H_
#define _SHR_WIZARD_BASE_H_

#define OHDB_POWER_UP_WAIT 1
#define E2PROM_TEST_DATA 0xAA
#define E2PROM_TEST_ADDRESS 450

#include "HeadParameters.h"
#include "Q2RTWizard.h"

class CQFileWithCheckSum;


class CQ2RTSHRWizardBase : public CQ2RTAutoWizard
{
private:
    int m_VacuumLeakagePageIndex;
    int m_PurgePageIndex;

private:
    void OpenHistoryFile   ();
    void CloseHistoryFile  ();

protected:
//Members
	THeadParameters m_CurrHeadsParams[TOTAL_NUMBER_OF_HEADS_HEATERS];
    THeadParameters m_PreviousHeadsParams[TOTAL_NUMBER_OF_HEADS_HEATERS];
	TMachineState m_PrevMachineState;
	FILE *m_HeadReplacementFile;
	bool m_HeadsInsertedWrong[TOTAL_NUMBER_OF_HEADS_HEATERS];
	bool m_RunHeadAlignmentWizard;
	bool m_VacuumLeakage;
	QString m_NoCharacterizationDescription;
    CQFileWithCheckSum* m_TempFile;
    CQParameterBase* m_LastStateParam;
    
    bool m_HeadsToBeReplaced          [TOTAL_NUMBER_OF_HEADS_HEATERS];

//Methods
    virtual void ResumeWizardSpecificData() = 0;
    virtual QString GetResumeWarningString() = 0;
    virtual void SaveWizardSpecificDataToTemporaryFile() = 0;
    virtual void SetResumingPage() = 0;
    virtual void Q2RTCleanUp(void);
    virtual bool Start(void);

    void SetIndexes(int VacuumLeakagePageIndex, int PurgePageIndex);
    void HeadsHeating1PageEnter          (CWizardPage* WizardPage);
    void HeadsHeatingCycle(const int TimeOut=DEFAULT_TIMEOUT,int* HeadsTemperatures=NULL, bool IncludePreHeater=true);
    void HeadsOnlyHeatingCycle(const int TimeOut=DEFAULT_TIMEOUT,int* HeadsTemperatures=NULL);
    void SelectedHeadsHeatingCycle(const int TimeOut=DEFAULT_TIMEOUT, int* HeadsTemperatures=NULL);
    
    void SaveWizardDataToTemporaryFile();
    void ResumeWizardPageEnter           (CWizardPage* WizardPage);

    bool CoolingDownPageEnter            (CWizardPage* WizardPage);
    void ReplacementPositionPageEnter    (CWizardPage* WizardPage, int PageIndex);
    void ReplaceHeadPageEnter            (CWizardPage* WizardPage, int PageIndex);
    void ReInsertHeadPageEnter           (CWizardPage* WizardPage);
    void CheckInsertionPageEnter         (CWizardPage* WizardPage);
    void NoCharacterizationDataPageEnter (CWizardPage* WizardPage);
    bool FillBlockPageEnter              (CWizardPage* WizardPage, T_AxesTable a_AxesTable=NULL);
    bool PurgePageEnter                  (CWizardPage* WizardPage, int qm, int om, T_AxesTable a_AxesTable=NULL);
    void HeadsHeating2PageEnter          (CWizardPage* WizardPage);
    void ReplaceHeadPageLeave            (CWizardPage* WizardPage);

    void TurnPowerOff();
    void TurnPowerOn();        
    void SaveLastState();
    virtual bool SetHeadsTemperature();

public:
    CQ2RTSHRWizardBase(int NameResourceID, QString UserButton1Caption="");
    bool RunHeadAlignment();
};
#endif
