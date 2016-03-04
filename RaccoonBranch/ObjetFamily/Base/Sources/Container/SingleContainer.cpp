
#pragma hdrstop
#include "SingleContainer.h"
#pragma package(smart_init)

#include "FrontEnd.h"
#include "QTimer.h"
#include "AppLogFile.h"
#include "MachineManager.h"
#include "MachineSequencer.h"
#include "Q2RTApplication.h"
#include "QThreadUtils.h"
#include "RaccoonAppParamsWrapper.h"

using namespace Raccoon;

/*******************************************************************************
Class CQSingleContainer implementation
*******************************************************************************/

extern const int PumpsCounterID[NUMBER_OF_PUMPS];

const int WEIGHT_QUANTIZATION_UNITS              = 20;
// Constructor
CQSingleContainer::CQSingleContainer(int FullContainerWeight, int EmptyContainerWeight, TTankIndex tank_index, int InplaceSensorTag)
    : CQComponent("CQSingleContainer" + QIntToStr(tank_index))
{
    m_EmptyContainerWeight                 = EmptyContainerWeight;
	m_TankIndex                            = tank_index;
	m_TagInfo.TankIndex                    = tank_index;
    m_ParamsMgr                            = CAppParams::Instance();
    m_NewInsertion                         = true;
    m_DialogAcked                          = false;
    m_ForceUsingTank                       = false;
    m_RunRRWizard                          = false;
	m_ResinExpired                         = false;
    m_MaxPumpTimeExceeded                  = false;
    m_SkipMachineResinTypeChecking         = false;
    m_AllowServiceMaterials                = false;
    m_IdentificationTime                   = 0;
	m_ContainerMaxPumpTime                 = 0;
    m_OnIdentificationCompletedCallback    = NULL;
    m_OnIdentificationCompletedCockie      = NULL;
	m_IgnoreTankInsertionAndRemovalSignals = false;
	m_ConsumedWeight                       = 0;
	m_CurrentSlice                         = 0;
	m_SliceNumAtLastUpdate                 = 0;
	m_InplaceSensorTag                     = InplaceSensorTag;

    // m_FEEnabledControlID = FE_MODEL_CONTAINER_1_ENABLED_STATUS;
    // todo -oNobody -cNone:
    m_FEEnabledControlID   = FE_TANK_ENABLING_STATUS_BASE  + tank_index;
    m_FEExistenceControlID = FE_TANK_EXISTENCE_STATUS_BASE + tank_index;

    m_PumpCounterIDList.clear();

    for(int i = FIRST_PUMP; i < NUMBER_OF_PUMPS; i++)
        if(IsPumpRelatedToTank(static_cast<TPumpIndex>(i), tank_index))
            m_PumpCounterIDList.push_back(PumpsCounterID[i]);

    m_LastInsertionTime                   = QGetTicks();
	m_containerSMTimer                    = 0;
	m_previousTime                        = 0;
	m_WriteErrorTagCntr                   = 0;
}

// Destructor
CQSingleContainer::~CQSingleContainer()
{
    
}

QString CQSingleContainer::GetMaterialType()
{
    QString retVal = "";

    m_MutexPipeMaterialName.WaitFor();
    retVal = m_PipeMaterial;
    m_MutexPipeMaterialName.Release();

    return retVal;
}

QString CQSingleContainer::GetTankMaterialType()
{
    QString retVal = "";

    m_MutexTankMaterialName.WaitFor();
    retVal = m_TankMaterial;
    m_MutexTankMaterialName.Release();

    return retVal;
}

QString CQSingleContainer::GetResinTypeByIndex(int resin_index)
{
    QString ret;
    if(IS_MODEL_TANK(m_TankIndex))
    {
        if(m_ContainerPtr->GetModelResinType(resin_index) == "Unsupported")
            ret = m_ContainerPtr->GetServiceModelResinType(resin_index);
        else
            ret = m_ContainerPtr->GetModelResinType(resin_index);
    }
    else if(m_ContainerPtr->GetSupportResinType(resin_index) == "Unsupported")
    {
        ret = m_ContainerPtr->GetServiceSupportResinType(resin_index);
    }
    else
    {
        ret = m_ContainerPtr->GetSupportResinType(resin_index);
    }
    return ret;
}

unsigned long CQSingleContainer::GetExpirationDate()
{
    return GetTagInfo()->MaterialInfo.ExpirationDate;
}

void CQSingleContainer::SendNotification(TTagIdentifyNotificationMessage& Message)
{
    m_ContainerPtr->GetNotificationDistributeMutex()->WaitFor();

    try
    {
        IdentifyTank(Message);
    }
    catch(...)
    {
        throw ESingleContainer("SingleContainer: TankIDentificationSM() failed.");
    }

    m_ContainerPtr->GetNotificationDistributeMutex()->Release();
}      

void CQSingleContainer::InitTank(bool Init)
{
    // check if current SingleContainer had an RF Tag during last program run:
    if(!m_ContainerPtr)
        throw ESingleContainer("m_ContainerPtr not initialized");

    CQLog::Write(LOG_TAG_RACCOON, QFormatStr("Initializing Single Container TankIndex: %d, Status: %s", m_TankIndex, ((Init) ? "true" : "false")));

    // Clear Tag ID and Resolved Array when entering this state:
	m_IdentificationTime          = 0;
	m_DialogAcked                 = false;
	m_ForceUsingTank              = false;
	m_RunRRWizard                 = false;
	m_ResinExpired                = false;
	m_MaxPumpTimeExceeded         = false;
	m_ConsumedWeight              = 0;
	m_SliceNumAtLastUpdate        = 0;
	m_containerSMTimer            = 0;

    SetTankMaterialType("");
    SetMaterialType(m_ParamsMgr->TypesArrayPerPipe[m_TankIndex]);

    m_ContainerPtr->TanksStatusLogic(m_TankIndex);     

	SetTankInserted(Init);
    FrontEndInterface->UpdateStatus(m_FEExistenceControlID,  static_cast<int>(Init));

    if((m_state_text = GetTankMaterialType()) == "")
    {
        CMaintenanceCounters *MaintCounters = CMaintenanceCounters::GetInstance();

        // Initialize the pump time:
        m_LastPumpTimeVector.clear();

        for(TPumpCounterIDs::iterator i = m_PumpCounterIDList.begin(); i != m_PumpCounterIDList.end(); ++i)
            m_LastPumpTimeVector.push_back(MaintCounters->GetElapsedSeconds(*i));

        m_IdentificationTime = QGetTicks();
    }
}      

// This function is called while statemachine is in IDENTIFIED_STABLE state.
bool CQSingleContainer::ContainerEnablingSeq()
{
    // Calc time from last function State Machine iteration:
    unsigned long CurrentTime = QGetTicks();
	m_containerSMTimer       += (QGetTicks() - m_previousTime);
	m_previousTime            = CurrentTime;
	bool EnableTank           = false;

	if(ContainerEnablingSeq_CheckOverConsumption())
	{
		if(ContainerEnablingSeq_CheckResinType())
		{
			if(ContainerEnablingSeq_CheckExpirationDate())
			{
				// Invoking the callback if available
				if(m_OnIdentificationCompletedCallback != NULL && m_OnIdentificationCompletedCockie != NULL)
					(*m_OnIdentificationCompletedCallback)(m_TankIndex, m_OnIdentificationCompletedCockie);

				EnableTank = true; 
			}
		}
    }

	if(EnableTank)
		ContainerEnablingSeq_EnabledTank();
	else
		ContainerEnablingSeq_DisabledTank();

	return EnableTank;
}

bool CQSingleContainer::ContainerEnablingSeq_CheckOverConsumption()
{
	if(0 == GetTagInfo()->CurrentWeight)
	{
		QString Material = GetResinTypeByIndex(GetTagInfo()->MaterialInfo.MaterialType);
		SetTankMaterialType(Material);
		FrontEndInterface->UpdateStatus(FE_UPDATE_RESIN_TYPE_BASE + m_TankIndex, Material);
		DisableTankAfterOverconsumption();
		return false;
	}

	return true;
}

bool CQSingleContainer::ContainerEnablingSeq_CheckResinType()
{
    QString MachineMaterialType     = "";
	TAG_TYPE__MATERIAL_TYPE matType = GetTagInfo()->MaterialInfo.MaterialType;
	int TankIDNoticeType            = FE_TANK_ID_NOTICE_DLG_PIPE_NO_MATCH;
	int FEMessageEnabledDisabled    = FE_TANK_DISABLED_STATUS;
	CMachineManager *Mgr            = Q2RTApplication->GetMachineManager();
	TMachineState MachineState      = Mgr->GetCurrentState();
	bool IsWizardRunning            = Mgr->IsWizardRunning();
    bool ResinTypeOK                = false;

    TChamberIndex ti = TankToChamber(m_TankIndex);
    if(NUMBER_OF_CHAMBERS == ti)
		MachineMaterialType = "";
    else
        MachineMaterialType = m_ParamsMgr->TypesArrayPerChamber[ti];

	SetTankMaterialType(m_state_text = GetResinTypeByIndex(matType));

	FrontEndInterface->UpdateStatus(FE_UPDATE_RESIN_TYPE_BASE + m_TankIndex, m_state_text);

    // Handle No Tank: (denoted by an empty string for the Tank Material name.)
    if(GetTankMaterialType() == "")
    {
        // Close any dialog related to this tank if it was open:
        TankIDNoticeType         = FE_TANK_ID_NOTICE_DLG_HIDE;

        // Although tanks are Disabled if not inserted, we don't need the "Disabled" icon.
        // (so send an Enabled notification)
        FEMessageEnabledDisabled = FE_TANK_ENABLED_STATUS;
    }
    // Check for a "Support instead of Model" (manufacturing mistake)
	else if((IS_MODEL_TANK(m_TankIndex)) && (m_ContainerPtr->GetSupportResinType(matType) != "Unsupported"))
    {
        FEMessageEnabledDisabled = FE_TANK_DISABLED_STATUS;
        TankIDNoticeType         = FE_TANK_ID_NOTICE_DLG_SUPPORT_IN_MODEL_MISPLACE;
    }
    // Check for a "Model instead of Support" (manufacturing mistake)
	else if((IS_SUPPORT_TANK(m_TankIndex)) && (m_ContainerPtr->GetModelResinType(matType) != "Unsupported"))
    {
        FEMessageEnabledDisabled = FE_TANK_DISABLED_STATUS;
        TankIDNoticeType         = FE_TANK_ID_NOTICE_DLG_MODEL_IN_SUPPORT_MISPLACE;
    }
    // Model Service material is inserted
    else if(IS_MODEL_TANK(m_TankIndex) && (m_ContainerPtr->GetServiceModelResinType(matType) != "Unsupported") && !AllowServiceMaterials())
    {
        if((MachineState == msPrinting) || (MachineState == msPrePrint) || (true == IsWizardRunning))
        {
            FEMessageEnabledDisabled = FE_TANK_DISABLED_STATUS;
            TankIDNoticeType         = FE_TANK_ID_NOTICE_DLG_MACHINE_NO_MATCH_MSPRINTING;
        }
        else
        {
            FEMessageEnabledDisabled = FE_TANK_DISABLED_STATUS;
            TankIDNoticeType         = FE_TANK_ID_NOTICE_DLG_SERVICE_IN_REGULAR;
        }
    }
    // Support Service material is inserted
	else if(IS_SUPPORT_TANK(m_TankIndex) && (m_ContainerPtr->GetServiceSupportResinType(matType) != "Unsupported") && !AllowServiceMaterials())
    {
        if((MachineState == msPrinting) || (MachineState == msPrePrint) || (true == IsWizardRunning))
        {
            FEMessageEnabledDisabled = FE_TANK_DISABLED_STATUS;
            TankIDNoticeType         = FE_TANK_ID_NOTICE_DLG_MACHINE_NO_MATCH_MSPRINTING;
        }
        else
        {
            FEMessageEnabledDisabled = FE_TANK_DISABLED_STATUS;
            TankIDNoticeType         = FE_TANK_ID_NOTICE_DLG_SERVICE_IN_REGULAR;
        }
    }
    else if(NUMBER_OF_CHAMBERS == ti)
    {
        FEMessageEnabledDisabled = FE_TANK_DISABLED_STATUS;
        TankIDNoticeType         = FE_TANK_NOT_ASSOCIATED_WITH_CHAMBER;

    }
    // Insert Model material if Model Service material is in the machine
    else if((IS_MODEL_TANK(m_TankIndex)) &&
            (CBackEndInterface::Instance()->IsServiceModelMaterial(MachineMaterialType)) &&
			("Unsupported" != m_ContainerPtr->GetModelResinType(matType)))
    {
        FEMessageEnabledDisabled = FE_TANK_DISABLED_STATUS;
        TankIDNoticeType         = FE_TANK_ID_NOTICE_DLG_REGULAR_IN_SERVICE;
    }
    // Insert Support material if Support Service material is in the machine
    else if((IS_SUPPORT_TANK(m_TankIndex)) &&
            (CBackEndInterface::Instance()->IsServiceSupportMaterial(MachineMaterialType)) &&
            ("Unsupported" != m_ContainerPtr->GetSupportResinType(matType)))
    {
        FEMessageEnabledDisabled = FE_TANK_DISABLED_STATUS;
        TankIDNoticeType         = FE_TANK_ID_NOTICE_DLG_REGULAR_IN_SERVICE;
    }
    // Handle Unsupported resins. (new Objet resins that are not recognized by an older SW version.)
    else if(GetTankMaterialType() == "Unsupported")
    {
        FEMessageEnabledDisabled = FE_TANK_DISABLED_STATUS;
        TankIDNoticeType         = FE_TANK_ID_NOTICE_DLG_UNSUPPORTED_RESIN;
    }
    // Check if material is licensed in License Manager
    else if(! Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetLicenseManager()->IsMaterialLicensed(GetTankMaterialType()))
    {
        FEMessageEnabledDisabled = FE_TANK_DISABLED_STATUS;
        //TankStatus               = false; //*
        TankIDNoticeType         = FE_TANK_ID_NOTICE_DLG_NOT_LICENSED_CARTRIDGE;
    }
    // Check if Tank resin matches Machine Resin:
    else if(GetTankMaterialType() != MachineMaterialType)
    {
        if((MachineState == msPrinting) || (MachineState == msPrePrint) || (true == IsWizardRunning))
        {
            FEMessageEnabledDisabled = FE_TANK_DISABLED_STATUS;
            TankIDNoticeType         = FE_TANK_ID_NOTICE_DLG_MACHINE_NO_MATCH_MSPRINTING;
        }
        else
        {
            FEMessageEnabledDisabled = FE_TANK_DISABLED_STATUS;
            TankIDNoticeType         = FE_TANK_ID_NOTICE_DLG_MACHINE_NO_MATCH;
        }
    }
    // Check if current tank resin (as obtained from container's Tag) also matches the short-pipe's resin:
    else if(GetTankMaterialType() != GetMaterialType())
    {
        if((MachineState == msPrinting) || (MachineState == msPrePrint) || (true == IsWizardRunning))
        {
            FEMessageEnabledDisabled = FE_TANK_DISABLED_STATUS;
            TankIDNoticeType         = FE_TANK_ID_NOTICE_DLG_PIPE_NO_MATCH_MSPRINTING;
        }
        else
        {
            FEMessageEnabledDisabled = FE_TANK_DISABLED_STATUS;
            TankIDNoticeType         = FE_TANK_ID_NOTICE_DLG_PIPE_NO_MATCH;
        }
    }
    else
    {
        TankIDNoticeType         = FE_TANK_ID_NOTICE_DLG_HIDE;
        FEMessageEnabledDisabled = FE_TANK_ENABLED_STATUS;
		ResinTypeOK              = true;
    }

    if(TankIDNoticeType == FE_TANK_ID_NOTICE_DLG_HIDE)
        FrontEndInterface->HideTankIDNoticeDlg(m_TankIndex);

    if(SkipMachineResinTypeChecking() &&
            ((TankIDNoticeType == FE_TANK_ID_NOTICE_DLG_MACHINE_NO_MATCH)
             || (TankIDNoticeType == FE_TANK_ID_NOTICE_DLG_PIPE_NO_MATCH)
             || (TankIDNoticeType == FE_TANK_ID_NOTICE_DLG_MACHINE_NO_MATCH_MSPRINTING)
             || (TankIDNoticeType == FE_TANK_ID_NOTICE_DLG_PIPE_NO_MATCH_MSPRINTING)
             ||	(TankIDNoticeType == FE_TANK_ID_NOTICE_DLG_SERVICE_IN_REGULAR)
             || (TankIDNoticeType == FE_TANK_ID_NOTICE_DLG_REGULAR_IN_SERVICE)
             || (TankIDNoticeType == FE_TANK_NOT_ASSOCIATED_WITH_CHAMBER)))
    {
        // Don't show the dialog.
    }
	else if((GetDialogAcked() == false) && (TankIDNoticeType != FE_TANK_ID_NOTICE_DLG_HIDE))
    {
		FrontEndInterface->ShowTankIDNotice(TankIDNoticeType, m_TankIndex, false);
    }

	FrontEndInterface->UpdateStatus(m_FEEnabledControlID, FEMessageEnabledDisabled, false);

	// Check to which state we go:
	return ResinTypeOK;
}

bool CQSingleContainer::ContainerEnablingSeq_CheckExpirationDate()
{ 
    // Check if Resin has expired, show dialog and raise a flag: 
	if(static_cast<unsigned>(QGetCurrentTime()) > GetTagInfo()->MaterialInfo.ExpirationDate)  
	{
		if(m_ResinExpired == false)
		{
			m_ResinExpired = true;
			if(SkipMachineResinTypeChecking())
				// Don't show the dialog.
			{}
			else
			{
				FrontEndInterface->ShowTankIDNotice(FE_TANK_ID_NOTICE_DLG_RESIN_EXPIRED, m_TankIndex, false);
			}
			FrontEndInterface->UpdateStatus(m_FEEnabledControlID, FE_TANK_DISABLED_STATUS, false);
			SetTankEnabled(false);
		}
	}  

	return (! m_ResinExpired);
}

void CQSingleContainer::ContainerEnablingSeq_DisabledTank()
{
    SetTankEnabled(false);
    FrontEndInterface->UpdateStatus(m_FEEnabledControlID, FE_TANK_DISABLED_STATUS, false); 
}

void CQSingleContainer::ContainerEnablingSeq_EnabledTank()
{
    SetTankEnabled(true);
	FrontEndInterface->UpdateStatus(m_FEEnabledControlID, FE_TANK_ENABLED_STATUS, false);
}

TQErrCode CQSingleContainer::UpdateWeightAfterWizard()
{
    return Q_NO_ERROR;
    // Slava: Todo: Raccoon: This is the place to insert a call to "Consumption Update" after a resin consuming wizard was completed. (e.g: MRW)  (Shahar)
    /*
    	TQErrCode err = Q_NO_ERROR;
    	m_MutexUpdateTagWeight.WaitFor();
    	err = m_TagAccess->TagReadWeight(m_currReadTagWeight);
    	//it is better to not update the weight due to erroneous read than to block the cartridge
    	if(err != Q_NO_ERROR || m_currReadTagWeight < -MAX_DEVIATION_WEIGHT || m_currReadTagWeight > m_ParamsMgr->CartridgeInitialWeight + MAX_DEVIATION_WEIGHT)
    	{
    		m_MutexUpdateTagWeight.Release();
    		CQLog::Write(LOG_TAG_OBJET_MACHINE, "Failed reading m_currReadTagWeight TankIndex:%d Value:%d", m_TankIndex, m_currReadTagWeight);
    		return -1;
    	}
    	else
    	{
    		int currWeight = GetTankWeightInGram();
    		if(m_currReadTagWeight == 0) //first insertion of the cartridge
    		{
    			m_currReadTagWeight = m_ParamsMgr->CartridgeInitialWeight;
    			m_TagAccess->TagWriteWeight(m_currReadTagWeight);
    			CQLog::Write(LOG_TAG_OBJET_MACHINE, "R_P Wizard: TankIndex:%d currWeight:%d", m_TankIndex, currWeight);
    		}
    		else if((currWeight - m_currReadTagWeight) < MAX_DEVIATION_WEIGHT)
    		{
    			int weightTag = currWeight - m_weightGap;
    			if(weightTag < m_currReadTagWeight && m_weightGap != INITIAL_GAP) //write to tag only in material reduction
    				m_TagAccess->TagWriteWeight(weightTag);
    			CQLog::Write(LOG_TAG_OBJET_MACHINE, "R_P Wizard: TankIndex:%d currWeight:%d TagWeight:%d gap:%d", m_TankIndex, currWeight, m_currReadTagWeight, m_weightGap);
    		}
    		else if((currWeight - m_currReadTagWeight) >= MAX_DEVIATION_WEIGHT) //there was a refill
    		{
    			FrontEndInterface->ShowTankIDNotice(FE_TANK_ID_NOTICE_DLG_CHECK_CONTAINER_REFILL, m_TankIndex, false);
    			FrontEndInterface->UpdateStatus(m_FEEnabledControlID, FE_TANK_DISABLED_STATUS, false);
    			SetTankEnabled(false);
    			//write error code to tag
    			int errCode = CARTRIDGE_REFILL_TAG_ERROR;
    			m_TagAccess->TagWriteErrorCode(errCode);
				LogUserTampering(REFILL_PROTECTION_EVENT);
    			CQLog::Write(LOG_TAG_OBJET_MACHINE, "Error R_P Wizard: TankIndex:%d currWeight:%d TagWeight:%d errCode:%d", m_TankIndex, currWeight, m_currReadTagWeight, errCode);
    			m_TankIDState = DISABLED_TANK;
    		}
    	}
    	CMaintenanceCounters *MaintCounters = CMaintenanceCounters::GetInstance();

    	// Initialize the pump time:
    	m_LastPumpTimeVector.clear();

    	for(TPumpCounterIDs::iterator i = m_PumpCounterIDList.begin(); i != m_PumpCounterIDList.end(); i++)
    		m_LastPumpTimeVector.push_back(MaintCounters->GetElapsedSeconds(*i));

    	m_MutexUpdateTagWeight.Release();
    	return err;
    */
}

void CQSingleContainer::SetMaterialType(QString MaterialType)
{
	m_MutexPipeMaterialName.WaitFor();
    m_PipeMaterial = MaterialType;  
    m_MutexPipeMaterialName.Release();
}

void CQSingleContainer::SetTankMaterialType(QString MaterialType)
{
	m_MutexTankMaterialName.WaitFor();
	m_TankMaterial = MaterialType;
    m_MutexTankMaterialName.Release();
}

bool CQSingleContainer::IsTankInserted()
{
    bool Inserted;

    m_MutexTankInserted.WaitFor();
    Inserted = m_IsInserted;
    m_MutexTankInserted.Release();

    return Inserted;
}

unsigned int CQSingleContainer::GetLastInsertionTimeInTicks()
{
    return m_LastInsertionTime;
}

void CQSingleContainer::SetTankInserted(bool Inserted)
{
    if(Inserted && ! m_IsInserted)
        m_LastInsertionTime = QGetTicks();

    // Setting the 'Inserted' flag
    m_MutexTankInserted.WaitFor();
    m_IsInserted = Inserted;
    m_MutexTankInserted.Release();

    // In case of container removal detection - set the drain flag to true.....
    if(Inserted == false)
    {
        m_IsDrainNeeded = true;
        m_NewInsertion  = true;
    }
}

// This method checks expiration directly bypassing the SM
bool CQSingleContainer::IsResinExpiredDirect()
{
	if(static_cast<unsigned>(QGetCurrentTime()) > GetTagInfo()->MaterialInfo.ExpirationDate)
 		return true;
    return false;
}

void CQSingleContainer::SetTankWeight(float CurrentWeightA2D)
{
    m_MutexWeights.WaitFor();

    // Updating the A2D Value
    m_CurrentWeightInA2D = CurrentWeightA2D;

    // Calculating the weight according to the A2D Value, the gain & the offset
    int CurrWeight = CurrentWeightA2D * m_Gain + m_Offset - m_EmptyContainerWeight;
    if(CurrWeight < m_CurrentWeightInGram)
    {
        m_CurrentWeightInGram = CurrWeight + WEIGHT_QUANTIZATION_UNITS;
        m_CurrentWeightInGram -= m_CurrentWeightInGram % WEIGHT_QUANTIZATION_UNITS;
    }
    else
        m_CurrentWeightInGram = CurrWeight - CurrWeight % WEIGHT_QUANTIZATION_UNITS;

    m_MutexWeights.Release();

    // If there is more than 40 gr difference than last 'Monitor' update
    m_NeedToShowWeight = (abs(m_PrevWeightInGram - m_CurrentWeightInGram) > WEIGHT_QUANTIZATION_UNITS);
    if(m_NeedToShowWeight)
        m_PrevWeightInGram = m_CurrentWeightInGram;

}

float CQSingleContainer::GetTankWeightInA2D()
{
    float WeightInA2D;

    m_MutexWeights.WaitFor();
    WeightInA2D = m_CurrentWeightInA2D;
    m_MutexWeights.Release();

    return WeightInA2D;
}

int CQSingleContainer::GetTankWeightInGram(bool ForceRealValue/* = false by default */)
{
    int WeightInGr;
    //objet_machine test
    if(FindWindow(0, QFormatStr("Tank%dEmpty.txt - Notepad", (int)m_TankIndex).c_str()))
        return 0;

    if(ForceRealValue == false)
    {
        // In the case Tank is out, or we have "Disabled Tanks" (eg: E250, E350, E260) return 'zero'.
        if((IsTankInserted() == false) || (GetMaterialType() == "Disabled-Tank"))
            return 0;
        //  CQLog::Write(LOG_TAG_OBJET_MACHINE,"GetTankWeightInGram() IsTankInserted: %d, GetMaterialType: %s",IsTankInserted(),GetMaterialType());
    }

    m_MutexWeights.WaitFor();
    WeightInGr = m_CurrentWeightInGram;
    m_MutexWeights.Release();

    return WeightInGr;
}



void CQSingleContainer::SetTankGain(float Gain)
{
    m_MutexGainOffset.WaitFor();
    m_Gain = Gain;
    m_MutexGainOffset.Release();
}


void CQSingleContainer::SetTankOffset(float Offset)
{
    m_MutexGainOffset.WaitFor();
    m_Offset = Offset;
    m_MutexGainOffset.Release();
}


bool CQSingleContainer::IsTankEnabled()
{
    bool Enabled;

    m_MutexTankEnabled.WaitFor();
    Enabled = m_Enabled;
    m_MutexTankEnabled.Release();

    return Enabled;
}

void CQSingleContainer::SetTankEnabled(bool Enabled)
{
    m_MutexTankEnabled.WaitFor();
    m_Enabled = Enabled;
    m_MutexTankEnabled.Release();
}

bool CQSingleContainer::IsPrintWeightNecessary()
{
    return m_NeedToShowWeight;
}

unsigned __int64 CQSingleContainer::GetTagID()
{
	return GetTagInfo()->SerialNumber;
}

Raccoon::TTagInfo* CQSingleContainer::GetTagInfo()
{
    CQMutexHolder GuardHolder(&m_MutexTagInfo);
	return &m_TagInfo;
}

TQErrCode CQSingleContainer::IgnoreTankInsertionAndRemovalSignals()
{
    m_IgnoreTankInsertionAndRemovalSignals = true;
    return Q_NO_ERROR;
}

TQErrCode CQSingleContainer::AllowTankInsertionAndRemovalSignals()
{
    m_IgnoreTankInsertionAndRemovalSignals = false;
    return Q_NO_ERROR;
}

void CQSingleContainer::SetEventIdentificationCompletion()
{
    m_EventWaitIdentificationCompletion.SetEvent();
}

void CQSingleContainer::ResetEventIdentificationCompletion()
{
    m_EventWaitIdentificationCompletion.WaitFor(0);
}

void CQSingleContainer::SetIdentificationCompletedCallback(TStateEnterCallback CallbackFunction, TGenericCockie Cockie)
{

    m_OnIdentificationCompletedCallback = CallbackFunction;
    m_OnIdentificationCompletedCockie   = Cockie;
}

QLib::TQWaitResult CQSingleContainer::WaitEventIdentificationCompletion(unsigned int timetowait)
{
    return m_EventWaitIdentificationCompletion.WaitFor(timetowait);
}

void CQSingleContainer::SetContainerPtr(IContainerInterface* cont_ptr)
{
    m_ContainerPtr = cont_ptr;
}

bool CQSingleContainer::IsItContainerFirstUse()
{
    return m_NewInsertion;
}

char* CQSingleContainer::GetBatchNo()
{
    return m_batchNo;
}

void CQSingleContainer::SetContainerAsUsed()
{
    m_NewInsertion = false;
}

void CQSingleContainer::SetDialogAcked(bool val)
{
    m_DialogAcked = val;
}

bool CQSingleContainer::GetDialogAcked()
{
    return m_DialogAcked;
}

void CQSingleContainer::SetForceUsingTank(bool val)
{
    m_ForceUsingTank = val;
}

void CQSingleContainer::SetForceUsingMode(TOperationModeIndex val)
{
    m_ForcerUsingMode = val;
}

bool CQSingleContainer::GetForceUsingTank()
{
    return m_ForceUsingTank;
}

TOperationModeIndex CQSingleContainer::GetForceUsingMode()
{
    return m_ForcerUsingMode;
}

void CQSingleContainer::SetRunRRWizard(bool val)
{
    m_RunRRWizard = val;
}

bool CQSingleContainer::GetRunRRWizard()
{
    return m_RunRRWizard;
}

bool CQSingleContainer::IsResinExpired()
{
	return m_ResinExpired;
}

bool CQSingleContainer::IsMaxPumpTimeExceeded()
{
    return m_MaxPumpTimeExceeded;
}

int CQSingleContainer::GetFEMessageID()
{
    return m_TankIndex;   // for the message sent to TankIdentificationNotice dlg.
}

int CQSingleContainer::GetFEEnabledControlID()
{
    return m_FEEnabledControlID;
}

void CQSingleContainer::SetAllowServiceMaterials(bool val)
{
    m_AllowServiceMaterials = val;
}

bool CQSingleContainer::AllowServiceMaterials()
{
    return m_AllowServiceMaterials;   // todo -oNobody -cNone: bad names
}

void CQSingleContainer::SetSkipMachineResinTypeChecking(bool val)
{
    m_SkipMachineResinTypeChecking = val;
}

bool CQSingleContainer::SkipMachineResinTypeChecking()
{
    return m_SkipMachineResinTypeChecking;
}

void CQSingleContainer::SetRemountInProgress(bool inProgress)
{
    m_RemountInProgress = inProgress;
}

bool CQSingleContainer::IsRemountInProgress()
{
    return m_RemountInProgress;
}

bool CQSingleContainer::GetIgnoreTankInsertionAndRemovalSignalsState()
{
    return m_IgnoreTankInsertionAndRemovalSignals;
}

int CQSingleContainer::GetFEExistenceControlID()
{
    return m_FEExistenceControlID;
}

// Perform insertion / removal and consumption update tasks
void CQSingleContainer::IdentifyTank(TTagIdentifyNotificationMessage& Message)
{
    m_ContainerPtr->GetStateMachineMutex()->WaitFor();

	TTagIdentificationEventType Input = Message.EventType;

	switch(Input)
	{
		case TANK_INSERTED:
		{
			FrontEndInterface->UpdateStatus(FE_CURRENT_OCB_SENSOR_STATUS_START + m_InplaceSensorTag, true); // Show Red / Green led on Actuators & Sensors
			InitTank(true);                                                                                 // Show the tank on main UI
			AuthenticateTank();                                                                             // Attempt to authenticate
			break;
		}

		case TANK_REMOVED:
		{
            InitTank(false);                                                                                 // Show the tank on main UI
			CRaccoon::Instance()->RemoveCartridge(m_TankIndex);
			FrontEndInterface->UpdateStatus(FE_CURRENT_OCB_SENSOR_STATUS_START + m_InplaceSensorTag, false); // Show Red / Green led on Actuators & Sensors
			FrontEndInterface->UpdateStatus(FE_UPDATE_RESIN_TYPE_BASE + m_TankIndex, "");                    // Show material name on main UI
			break;
		}

		case TANK_UPDATE_CONSUMPTION:
		{
			m_MutexConsumptionData.WaitFor();

			m_ConsumedWeight += Message.Weight;
			m_CurrentSlice   = Message.SliceNum;
			
			m_MutexConsumptionData.Release();

			int Weight, Slice;
			GetConsumptionData(Weight, Slice);

			if(Weight > GramsToMilliGrams(m_ParamsMgr->ConsumedWeightBeforeError_Gr.Value()))
			{
                CQLog::Write(LOG_TAG_RACCOON, QFormatStr("Consumption not updated. TankIndex: %d", m_TankIndex));
				CBackEndInterface::Instance()->StopPrint(Q2RT_RACCOON_CONSUMPTION_NOT_UPDATED);
            }

			break;
        }
    }

    m_ContainerPtr->GetStateMachineMutex()->Release();
}

void CQSingleContainer::GetConsumptionData(int &Weight, int &Slice)
{
	CQMutexHolder Holder(&m_MutexConsumptionData);
	Weight = m_ConsumedWeight;
	Slice  = m_CurrentSlice;
}

void CQSingleContainer::SetConsumptionData(int Weight, int Slice)
{
	CQMutexHolder Holder(&m_MutexConsumptionData);
	m_ConsumedWeight = Weight;
	m_CurrentSlice   = Slice;
}

// Called from CConsumptionUpdater thread
void CQSingleContainer::ResetConsumedData(int CurrentSlice)
{
	SetConsumptionData(0, CurrentSlice);
	m_SliceNumAtLastUpdate = CurrentSlice;
}

// Called from CConsumptionUpdater thread each CONSUMPTION_UPDATE_INTERVAL
bool CQSingleContainer::IsConsumptionUpdateNeeded()
{
	bool res = false;
	
	int WeightThreshold = GramsToMilliGrams(m_ParamsMgr->WeightUpdateThreshold_Gr);
	int SlicesThreshold = m_ParamsMgr->SlicesUpdateThreshold;

	int ConsumedWeight, CurrentSlice;
	GetConsumptionData(ConsumedWeight, CurrentSlice);

	if(ConsumedWeight >= WeightThreshold || (CurrentSlice - m_SliceNumAtLastUpdate) >= SlicesThreshold)
		res = true;

#ifdef _DEBUG
	CRaccoonAppParamsWrapper AppParams;
	if(AppParams.IsLowLevelLoggingEnabled())
	{
		QString str = ((CurrentSlice == 0) ? "-" : QFormatStr("Slice: %d", CurrentSlice).c_str());
		CQLog::Write(LOG_TAG_RACCOON, QFormatStr("IsConsumptionUpdateNeeded: Tank: %s (%d), Weight: %.2f Gr, %s, Status: %s",
				TankToStr(static_cast<TTankIndex>(m_TankIndex)).c_str(), m_TankIndex, MilliGramsToGrams(ConsumedWeight), str.c_str(), (res ? "true" : "false")));
	}
#endif	

	return res;
}

// Perform tank authentication process
void CQSingleContainer::AuthenticateTank()
{
	try
	{
		CQLog::Write(LOG_TAG_RACCOON, QFormatStr("Starting authentication process. TankIndex: %d", m_TankIndex));

		// Clear the tag from previous data
		ClearTagInfo();

		CRaccoon::Instance()->Authenticate(GetTagInfo());

		// Log tag information
		QString TagInfoStr = QFormatStr("TankIndex: %d, SerialNo: %lld, TagStructID: %hu, MaterialType: %hu, BatchNo: %s, InitialWeight: %ld, CurrentWeight: %ld, MfgDate: %s, ExpDate: %s",
										m_TankIndex, GetTagInfo()->SerialNumber, GetTagInfo()->MaterialInfo.TagStructID,
																			 GetTagInfo()->MaterialInfo.MaterialType,
																			 GetTagInfo()->MaterialInfo.BatchNo,
																			 GetTagInfo()->InitialWeight_mGr,
																			 GetTagInfo()->CurrentWeight,
																			 QDateToStr(GetTagInfo()->MaterialInfo.MfgDateTime).c_str(),
																			 QDateToStr(GetTagInfo()->MaterialInfo.ExpirationDate).c_str());
										
		CQLog::Write(LOG_TAG_RACCOON, TagInfoStr.c_str());
	}
	catch(ERaccoon &e)
	{
		// Show tank as disabled if authentication has failed
		DisableTankAfterAuthenticationFail();
		return;
	}
	catch(...)
	{
		CQLog::Write(LOG_TAG_RACCOON, "Authenticate threw unknown exception");
    }

	if(ContainerEnablingSeq())
    {
        QString TankNotice = "";
        FrontEndInterface->UpdateStatus(FE_TANK_DISABLING_HINT_BASE + m_TankIndex, TankNotice, false);
    }
    else
	{
        QSleep(50); // Make sure that the dialog is shown to get text 
		QString TankNotice = FrontEndInterface->GetTankIDNoticeText(m_TankIndex);
        FrontEndInterface->UpdateStatus(FE_TANK_DISABLING_HINT_BASE + m_TankIndex, TankNotice, false);
        SetDialogAcked(false);
    }         
}

unsigned int CQSingleContainer::GetCurrentWeightInTag()
{
	return GetTagInfo()->CurrentWeight;
}

void CQSingleContainer::ClearTagInfo()
{
	GetTagInfo()->SerialNumber                = 0;
	GetTagInfo()->CurrentWeight               = 0;
	GetTagInfo()->MaterialInfo.TagStructID    = 0;
	GetTagInfo()->MaterialInfo.MaterialType   = 0;
	GetTagInfo()->MaterialInfo.MfgDateTime    = 0;
	GetTagInfo()->MaterialInfo.ExpirationDate = 0;
	GetTagInfo()->MaterialInfo.InitialWeight  = 0;
	GetTagInfo()->InitialWeight_mGr           = 0;
	memset(GetTagInfo()->MaterialInfo.BatchNo, 0, sizeof(GetTagInfo()->MaterialInfo.BatchNo));
}

void CQSingleContainer::DisableTankAfterAuthenticationFail()
{
	DisableTankAfterEvent(FE_TANK_ID_NOTICE_DLG_AUTHENTICATION_FAILED);
}

void CQSingleContainer::DisableTankAfterOverconsumption()
{
	DisableTankAfterEvent(FE_TANK_ID_NOTICE_DLG_OVERCONSUMPTION);
}

void CQSingleContainer::DisableTankAfterEvent(int TankIDNoticeType)
{
	if(GetDialogAcked())
		return;

	FrontEndInterface->ShowTankIDNotice(TankIDNoticeType, m_TankIndex, false);
	FrontEndInterface->UpdateStatus(m_FEEnabledControlID, FE_TANK_DISABLED_STATUS, false);

    QSleep(60); // Make sure that the dialog is shown to get text 
	QString TankNotice = FrontEndInterface->GetTankIDNoticeText(m_TankIndex);
	FrontEndInterface->UpdateStatus(FE_TANK_DISABLING_HINT_BASE + m_TankIndex, TankNotice, false);
}
