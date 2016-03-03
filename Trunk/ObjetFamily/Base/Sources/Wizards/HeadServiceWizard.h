
#ifndef _HEAD_SERVICE_WIZARD_H_
#define _HEAD_SERVICE_WIZARD_H_

#include <stdio.h>
#include "Q2RTSHRWizardBase.h"
#include <ExtCtrls.hpp> // needed for TTimer
#include "OHDBProtocolClient.h"
#include "OHDBCommDefs.h"
#include "QOSWrapper.h"
#include "HSWScaleBasedData.h"
#include "BaseScaleInterface.h"
#include "Scale.h"

class CQFileWithCheckSum;

class CHeadServiceWizard : public CQ2RTSHRWizardBase
{
private:
//Private Members:
	int m_PrevHeadPotentiometerValues[TOTAL_NUMBER_OF_HEADS][NUMBER_OF_QUALITY_MODES][NUM_OF_OPERATION_MODES];
	float m_PrevLayerHeightDPI_um[NUMBER_OF_QUALITY_MODES][NUM_OF_OPERATION_MODES];
	float m_PrevHeadsVoltagesModel[TOTAL_NUMBER_OF_HEADS][NUMBER_OF_QUALITY_MODES][NUM_OF_OPERATION_MODES];
    float m_PrevHeadsVoltagesSupport[TOTAL_NUMBER_OF_HEADS][NUMBER_OF_QUALITY_MODES][NUM_OF_OPERATION_MODES];
    QString m_ContinuationOptionPageSubtitle;
	bool m_DuringWeightTestPhase;
	bool m_headReplacementDone;
	float m_FinalLayerHeight[NUMBER_OF_QUALITY_MODES][NUM_OF_OPERATION_MODES];
	float m_VoltagePerHeadModel[TOTAL_NUMBER_OF_HEADS][NUMBER_OF_QUALITY_MODES][NUM_OF_OPERATION_MODES];
	float m_VoltagePerHeadSupport[TOTAL_NUMBER_OF_HEADS][NUMBER_OF_QUALITY_MODES][NUM_OF_OPERATION_MODES];
	
    bool m_GoToContinuationPage;
    bool m_IsFirstWizardIteration; // Used to print and show in GUI only the selected heads at second and above wizard iteration
	bool m_AskRunHeadAlignmentWizard;
    bool m_DuringHOW;
	bool m_performSecondPatternTest;

	enum HOW_MODE{
		HS_HM,
		HQ,
		NO_MODE_SELECTED
    };

	HOW_MODE  m_SelectedMode;

	QString m_doorIsClosed;
	QString m_closeCover;

	bool m_PatternTestNeeded;
    float m_LayerHeightPerHead[TOTAL_NUMBER_OF_HEADS];
	float m_GainPerHeadSupport[TOTAL_NUMBER_OF_HEADS];
	float m_GainPerHeadModel[TOTAL_NUMBER_OF_HEADS];

    float*** WeightResults        /*[TOTAL_NUMBER_OF_HEADS][NUMBER_OF_VOLTAGE_MODES][NUMBER_OF_QUALITY_MODES]*/;
    int***   MissingNozzlesResults/*[TOTAL_NUMBER_OF_HEADS][NUMBER_OF_VOLTAGE_MODES][NUMBER_OF_QUALITY_MODES]*/;
    bool***  MarkHeadsToBeReplaced/*[TOTAL_NUMBER_OF_HEADS][NUMBER_OF_VOLTAGE_MODES][NUMBER_OF_QUALITY_MODES]*/;
    bool     m_HeadsToBeSkipped     [TOTAL_NUMBER_OF_HEADS_HEATERS];
    float    LayerHeightGross[TOTAL_NUMBER_OF_HEADS][NUMBER_OF_VOLTAGE_MODES][NUMBER_OF_QUALITY_MODES];
	TTimer*  m_UVLampsTimer;
	CQMutex m_SetVirtualEncoderMutex;
    HSWScaledBaseData *m_hswScaleBasedData;
	HSWCalibrationData *m_hswCalibrationData;
	CBaseScales *m_scale;

    bool m_bNeedRestartAfterParamsChanged;
    bool m_targetWeightNotReached;
    	// The weight is OK
	bool CheckLiquidWeight();
	bool CheckLiquidWeightAccordingMode();
    //Private Methods:
    int FindWeakestHead();
    void PageEventsHandler(CWizardPage *WizardPage,int Param1,int Param2);

    // Start the wizard session event
    void StartEvent();
    // End the wizard session event
    void EndEvent();
    // Cancel the wizard session event
    void CancelOccurrence(CWizardPage *WizardPage);
    void CancelEvent(CWizardPage *WizardPages);
    void HelpEvent(CWizardPage *WizardPage);
    void UserButton1Event(CWizardPage *WizardPage);
    void UserButton2Event(CWizardPage *WizardPage);

    void AgreementPageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason);
	void SelectWizardModePageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason);
	void SelectCalibrationModePageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason);
    void ResumeWizardPageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason);
    void SelectContinuationOptionPageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason);
	void MaterialReplacement();
	void ReplaceMaterialS2M(CWizardPage *WizardPage);
	void ReplaceMaterialM2S(CWizardPage *WizardPage);
	void HeatingForHQMode();
	void HeatingForHSMode();
	bool PatternTestStressTest();
	void EnterMissingNozzlesPageLeave(CWizardPage* WizardPage, TWizardPageLeaveReason LeaveReason);
	void ScaleSetupPageLeave(CWizardPage* WizardPage, TWizardPageLeaveReason LeaveReason);
	void ScaleCommunicationPageLeave(CWizardPage* WizardPage, TWizardPageLeaveReason LeaveReason);
	void FindTargetLayer(CWizardPage *WizardPage,int operationMode,int qualityMode);
	void GetMinMaxLHnetSeqeunce (CWizardPage *WizardPage,int operationMode,int qualityMode);
	void PageLeave(CWizardPage *Page, TWizardPageLeaveReason LeaveReason);
	
	void TargetWeightNotReachedPageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason);
	void VoltageProblemPageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason);
	bool CheckCommunicationPageEnter(CWizardPage* WizardPage);
    bool CleanHeadsPurgePageEnter(CWizardPage *WizardPage);
	bool CleanHeadsHeatingPageEnter(CWizardPage *WizardPage);
	bool PurgeBeforeSecondPatternTest(CWizardPage* WizardPage);

    bool AdjustHeadsVoltagePageEnter(CWizardPage *Page);
	bool ReplacementWarningPageEnter(CWizardPage *Page);
    void RevertParams();
	bool TestPatternPrintingPageEnter(CWizardPage *WizardPage,bool homeZ=true);
    bool PrintTestPattern(bool homeZ=true);
	void PageEnter(CWizardPage *Page);
    void ReplaceMateria(CWizardPage *WizardPage);
	bool WizardCompletedWithoutHeadsReplacementPageEnter(CWizardPage* WizardPage);
	bool ScaleRemovingPageEnter(CWizardPage* WizardPage);
	bool ScaleRemovingPageLeave(CWizardPage* WizardPage);
	bool UVScanning(CWizardPage* WizardPage);
	bool TemplatePrintingPageEnter(CWizardPage* WizardPage);
    void DrainCycle();
    void CleanUp();
    float CalculateLineGain(float x1, float x2, float y1, float y2);
    float CalculateLineOffset(float x1, float x2, float y1, float y2);

    void SaveWizardSpecificDataToTemporaryFile();
    void ResumeWizardSpecificData();
    void SetResumingPage();

    QString GetResumeWarningString(int PageNumber);
    QString GetResumeWarningString();

    void AddPrintToTheWeightTestList(THeadIndex HeadType,TVoltageMode WeightTestVoltageMode);
    void CalcFinalVoltage(int qm, int om, THeadIndex HeadType = ALL_HEADS);
    bool CalcVoltage(int QualityMode, int om, THeadIndex HeadType = ALL_HEADS);

	QString EnteredDataStr();
	QString MaterialDataStr();
	void WriteToHistoryFileEnteredDataStr();
	void WriteToHistoryFileMaterialDataStr();
    void RecalcStartPosition(int PrintCount,int TotalPrintCount,int AxisZLevel,int* XOrder=NULL,int* YOrder=NULL);
	void PrintCarpet();
	void PrintTemplate();
	void CreateTemplateBMP();
    void ClearBitmap(Graphics::TBitmap * bmp, int Width, int Height);
    void CreateHeadRect(Graphics::TBitmap * bmp, int RectStart, int RectEnd, int OnSize, int OffSize,int RecOffset);
    void SaveToFile(Graphics::TBitmap*, QString file);
 

    void CalcFinalLayerHeight(int QualityMode, int OperationMode);
    bool IsThereHeadWithVoltageBelowMinimum(int QualityMode, int OperationMode);
    void ExcludeHeadsWithMinimalLayerHeight(int QualityMode, int OperationMode);
    void RemoveDefectiveHead(int QualityMode,int OperationMode);
    void SaveAndCalibrate(TQualityModeIndex QualityMode = ALL_QUALITY_MODES, int OperationMode = ALL_OPERATION_MODES);
    void InformTheUserOfTheDefectiveHeads();
    void LoadLocalPrintJob(const TFileNamesArray FileNames,int BitmapResolution,int QualityMode,int OperationMode,int SliceNum);

	void __fastcall UVTurnOFFTimerEvent(TObject *Sender);
	void TemplatePrintingPreparationPageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason);
	TQErrCode SetVirtualEncoder(bool Enable);
	static void VirtualEncoderAckCallback(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);
	void SaveCurrentData();
	bool PurgePageEnter(CWizardPage* WizardPage, int qm, int om, T_AxesTable a_AxesTable);
	void SetActiveThermistors(bool lowThermistor);
	bool PurgeCicle(int num_of_purges);
	void RevertToOriginalThresholds();
	void VoltageCalibration (int headID, float voltage);
	bool WeightTest(int headID,float &measurement,int qualityMode, int &weightTestError);
	void EnterMode(int qualityMode,int operationMode);
	float CalculateTargetLayer(int qualityMode,float minMaxLayerHead);
	void CalculateTargetWeight(std::vector<float>&targetWeight,float targetLayerGross,int operationMode);
	bool VoltageAdjustment(int qualityMode,int operationMode,std::vector<float>targetWeight);
	bool CheckIfMaterialsAreLegal();
	void HeadFillingCycle();
	bool UVLampIgnition();
	/*
		A barrier that allows each Tank a certain amount of time to finish
		its stabilization process.
		If a tank's identification process encounters a problem -
		BAILS-OUT from the barrier, to handle wizard freeze in the meantime.

		Returns whether or not the identification process is stuck
	*/
	bool AllTanksIdentificationSyncPoint();
	void TestPatternPreparationPageEnter(CWizardPage* WizardPage);
	void TestPatternPreparationPageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason);
	void SecondPatternTestReqPageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason);
	void SendXYToPosition(int x_position,int y_position);
	bool RemoveResinFromContainerPageEnter(CWizardPage* WizardPage);
	void PutResinToPurgeUnitPageLeave (CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason);
	void VerifyHeadCleaningPageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason);
	void MoveZtoScalePosition();
	void SaveHighMixCalibrationToHighSpeed();
	void FirstFire(CWizardPage *WizardPage,int operationMode,int qualityMode);
	void CheckCalibrationTest(CWizardPage *WizardPage,int operationMode,int qualityMode);
protected:
	COHDBProtocolClient *m_OHDBClient;


public:
	//Public Methods:

    CHeadServiceWizard();
    virtual ~CHeadServiceWizard();
	int GetContinuePageNumber();
	bool IsRestartRequired();
};

#endif


