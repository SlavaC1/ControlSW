/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Print Control                                            *
 * Module Description: Interface to the print control related       *
 *                     functions on the OHDB.                       *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran / Rachel / Gedalia.                                  *
 * Start date: 30/01/2002                                           *
 ********************************************************************/

#include "PrintControl.h"
#include "OHDBCommDefs.h"
#include "QMonitor.h"
#include "Q2RTErrors.h"
#include "AppLogFile.h"
#include "FrontEnd.h"
#include "Q2RTApplication.h"
#include "MachineManager.h"
#include "MachineSequencer.h"
#include "QThreadUtils.h"

//const int PRINT_CONTROL_ACK_WAIT_TIMEOUT = OHDB_WAIT_ACK_TIMEOUT + QMsToTicks(1000);


const int FIRE_ALL_ACTIVE = 1;
const int FIRE_ALL_IDLE   = 0;
//
const int ACK_STATUS_SUCCESS = 0;
const int ACK_STATUS_FAILURE = 1;

// Constructor
CPrintControl::CPrintControl(const QString& Name) : CQComponent(Name)
{
    INIT_METHOD(CPrintControl,SetDefaultConfigParams);
    INIT_METHOD(CPrintControl,SetLayerParams);
    INIT_METHOD(CPrintControl,ResetDriverCircuit);
    INIT_METHOD(CPrintControl,ResetDriverStateMachine);
    INIT_METHOD(CPrintControl,ApplyDefaultPrintParams);
    INIT_METHOD(CPrintControl,FireAll);
    INIT_METHOD(CPrintControl,WaitForFireAllFinished);
    INIT_METHOD(CPrintControl,GetFireAllStatus);

    m_OHDBClient = COHDBProtocolClient::Instance();
    m_ParamsMgr = CAppParams::Instance();
    m_Waiting = false;

    m_FireAllStatusCommandInUse=false;

    // Install a receive handler for a specific message ID
    m_OHDBClient->InstallMessageHandler(OHDB_FIRE_ALL_DONE, &FireAllEndMessageReceived, reinterpret_cast<TGenericCockie>(this));
}

// Destructor
CPrintControl::~CPrintControl(void)
{
    if(m_Waiting)
    {
        m_FireAllWaitingEvent.SetEvent();
    }
}

int CPrintControl::ConvertTimeToFPGAClockCounts(double Time)
{
	return (Time / (1 /(FPGA_MASTER_CLOCK / 1000000)));
}     

// Set default parameters for job configuration (should be called once per job)
TQErrCode CPrintControl::SetDefaultConfigParams(void)
{
	int ResolutionDivider = NATIVE_ENCODER_RESOLUTION            / m_ParamsMgr->PrintResolution_InXAxis;
	int DataMultiplier    = m_ParamsMgr->PrintResolution_InXAxis / m_ParamsMgr->SliceResolution_InXAxis;

	SetConfigParams(static_cast<TPulseType>(m_ParamsMgr->PulseType.Value()),
					DataMultiplier, ResolutionDivider,
					m_ParamsMgr->PulseWidthSingle_uSec,
					m_ParamsMgr->PulseWidthDouble_P1_uSec,
					m_ParamsMgr->PulseWidthDouble_P2_uSec,
					m_ParamsMgr->IntervalToSecondPulse_uSec,
					m_ParamsMgr->PropagationDelay_uSec,
                    m_ParamsMgr->VoltagePercentage);        

    return Q_NO_ERROR;
}

// Set parameters for job configuration (should be called once per job)
void CPrintControl::SetConfigParams(TPulseType PulseType,
									int   DataMultiplier,
									int   ResolutionDivider,
									float PulseWidthSingle_uSec,
									float PulseWidthDouble_P1_uSec,
									float PulseWidthDouble_P2_uSec,
									float IntervalToSecondPulse_uSec,
									float PropagationDelay_uSec,
								    int   VoltagePercentage)
{  
	int ADuCurrVoltages[HEAD_ELEMENTS];
	double volt;
	double P1_TDelay;
	CAppParams *ParamsMgr = CAppParams::Instance();
	CBackEndInterface::Instance()->GetHeadsA2DVoltages(ADuCurrVoltages, HEAD_ELEMENTS);

	TOHDBSetPrintingHeadsParams PrintingHeadsParams;

	PrintingHeadsParams.MessageID = OHDB_SET_PRINTING_HEADS_PARAMS;

	PrintingHeadsParams.PulseType         = PulseType;
	PrintingHeadsParams.DataMultiplier    = DataMultiplier;
	PrintingHeadsParams.ResolutionDivider = ResolutionDivider;
	PropagationDelay_uSec = PropagationDelay_uSec/1000;
	volt = (double)VoltagePercentage / 100;
	for(int i = 0; i < HEAD_ELEMENTS; i++)
	{
		if(PulseType == pulSingle)
		{
			PrintingHeadsParams.FirstPulseDwellTime[i] = 0;
			PrintingHeadsParams.FirstPulseFallTime[i]  = ConvertTimeToFPGAClockCounts(PulseWidthSingle_uSec + PropagationDelay_uSec);
			PrintingHeadsParams.SecondPulseWidth[i]    = 0;
			PrintingHeadsParams.DelayToSecondPulse[i]  = 0;
		}
		else // pulDouble
		{
			// TDelay calculation by the formula: Tdelay = Voltage x % of voltage / slew rate
			P1_TDelay = CONVERT_HEAD_VPP_A2D_TO_VOLT(ADuCurrVoltages[i]) * volt / (double)ParamsMgr->PulseSlewRate;
			PrintingHeadsParams.FirstPulseFallTime[i]  = ConvertTimeToFPGAClockCounts(P1_TDelay + PropagationDelay_uSec);
			PrintingHeadsParams.FirstPulseDwellTime[i] = ConvertTimeToFPGAClockCounts(PulseWidthDouble_P1_uSec   - P1_TDelay);
			PrintingHeadsParams.SecondPulseWidth[i]    = ConvertTimeToFPGAClockCounts(PulseWidthDouble_P2_uSec   + PropagationDelay_uSec);
			PrintingHeadsParams.DelayToSecondPulse[i]  = ConvertTimeToFPGAClockCounts(IntervalToSecondPulse_uSec + P1_TDelay);
		}
    }

    m_AckOk = false;

    if(m_OHDBClient->SendInstallWaitReply(&PrintingHeadsParams,
                                           sizeof(TOHDBSetPrintingHeadsParams),
                                           SetConfigParamsAckCallback,
                                           reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
    {
        CQLog::Write(LOG_TAG_PRINT,"OHDB didn't get ack for \"SetConfigParams\" message");
        throw EPrintControl("OHDB didn't get ack for \"SetConfigParams\" message");
    }

    // Check if we really got ack
    if(! m_AckOk)
        throw EPrintControl("PrintControl: SetConfigParams Invalid reply from OHDB");
}

// Completion routine for the ACK reply
void CPrintControl::SetConfigParamsAckCallback(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
    CPrintControl *InstancePtr = reinterpret_cast<CPrintControl *>(Cockie);

    // Check if we got ack message
    // Ignore messages smaller the ack message size
    if(DataLength == sizeof(TOHDBAck))
    {
        // Check if the ping is sucessful
        TOHDBAck *Message = static_cast<TOHDBAck *>(Data);

        if((Message->MessageID == OHDB_ACK) && (Message->RespondedMessageID == OHDB_SET_PRINTING_HEADS_PARAMS))
            InstancePtr->m_AckOk = (Message->AckStatus == OHDB_Ack_Success);
        else
            InstancePtr->m_AckOk = false;
    }
    else
        InstancePtr->m_AckOk = false;
}


// Set parameters for a single layer
TQErrCode CPrintControl::SetLayerParams(USHORT StartPEG, USHORT EndPEG, USHORT NoOfFires)
{
    TOHDBSetPrintingPassParams PrintingPassParams;

	CAppParams *ParamsMgr = CAppParams::Instance();

    PrintingPassParams.MessageID      = OHDB_SET_PRINTING_PASS_PARAMS;
	PrintingPassParams.StartPEG       = StartPEG;
	PrintingPassParams.EndPEG         = EndPEG;
	PrintingPassParams.NoOfFires      = NoOfFires;
    PrintingPassParams.BumperStartPEG = StartPEG - ParamsMgr->BumperPEGStartDelta;
	PrintingPassParams.BumperEndPEG   = EndPEG   + ParamsMgr->BumperPEGEndDelta;

	CQLog::Write(LOG_TAG_PRINT, QFormatStr("CPrintControl::SetLayerParams, StartPEG: %d, EndPEG: %d, NoOfFires: %d", StartPEG, EndPEG, NoOfFires));

    m_AckOk = false;

	if (m_OHDBClient->SendInstallWaitReply(&PrintingPassParams,
                                           sizeof(TOHDBSetPrintingPassParams),
                                           SetLayerAckCallback,
                                           reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
    {
        CQLog::Write(LOG_TAG_PRINT,"OHDB didn't get ack for \"SetLayerParams\" message");
        throw EPrintControl("OHDB didn't get ack for \"SetLayerParams\" message");
    }

    // Check if we really got ack
    if(!m_AckOk)
        throw EPrintControl("PrintControl:Set Printing Pass Params: Invalid reply from OHDB:"
                            + QIntToStr(m_AckOk));

    return Q_NO_ERROR;
}

// Completion routine for the ACK reply
void CPrintControl::SetLayerAckCallback(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
    CPrintControl *InstancePtr = reinterpret_cast<CPrintControl *>(Cockie);

    // Check if we got ack message
    // Ignore messages smaller the ack message size
    if(DataLength == sizeof(TOHDBAck))
    {
        // Check if the ping is sucessful
        TOHDBAck *Message = static_cast<TOHDBAck *>(Data);

        if((Message->MessageID == OHDB_ACK) && (Message->RespondedMessageID == OHDB_SET_PRINTING_PASS_PARAMS))
            InstancePtr->m_AckOk = (Message->AckStatus == OHDB_Ack_Success);
        else
            InstancePtr->m_AckOk = false;
    }
    else
        InstancePtr->m_AckOk = false;
}


// Start printing (enable printing circuitry)
void CPrintControl::Go(void)
{
    TOHDBGo OHDBGo;

    OHDBGo.MessageID = OHDB_ENABLE_PRINTING_CIRCUIT;

    m_AckOk = false;

    if (m_OHDBClient->SendInstallWaitReply(&OHDBGo, sizeof(TOHDBGo), GoAckCallback, reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
    {
        CQLog::Write(LOG_TAG_PRINT,"OHDB didn't get ack for \"Go\" message");
        throw EPrintControl("OHDB didn't get ack for \"Go\" message");
    }

    // Check if we really got ack
    if(!m_AckOk)
        throw EPrintControl("PrintControl:Go: Invalid reply from OHDB");
}

// Completion routine for the ACK reply
void CPrintControl::GoAckCallback(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
    CPrintControl *InstancePtr = reinterpret_cast<CPrintControl *>(Cockie);

    // Check if we got ack message
    // Ignore messages smaller the ack message size
    if(DataLength == sizeof(TOHDBAck))
    {
        // Check if the ping is sucessful
        TOHDBAck *Message = static_cast<TOHDBAck *>(Data);

        if((Message->MessageID == OHDB_ACK) && (Message->RespondedMessageID == OHDB_ENABLE_PRINTING_CIRCUIT))
            InstancePtr->m_AckOk = (Message->AckStatus == OHDB_Ack_Success);
        else
            InstancePtr->m_AckOk = false;
    }
    else
        InstancePtr->m_AckOk = false;
}

// Stop printing (disable printing circuitry)
void CPrintControl::Stop(void)  // Go Off
{
    TOHDBStop OHDBStop;

    OHDBStop.MessageID = OHDB_DISABLE_PRINTING_CIRCUIT;

    m_AckOk = false;

    if(m_OHDBClient->SendInstallWaitReply(&OHDBStop, sizeof(TOHDBStop), StopAckCallback, reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
    {
        CQLog::Write(LOG_TAG_PRINT,"OHDB didn't get ack for \"Go off\" message");
        throw EPrintControl("OHDB didn't get ack for \"Go off\" message");
    }

    // Check if we really got ack
    if(!m_AckOk)
        throw EPrintControl("PrintControl:Stop: Invalid reply from OHDB");
}

// Completion routine for the ACK reply
void CPrintControl::StopAckCallback(int TransactionID, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
    CPrintControl *InstancePtr = reinterpret_cast<CPrintControl *>(Cockie);

    // Check if we got ack message
    // Ignore messages smaller the ack message size
    if(DataLength == sizeof(TOHDBAck))
    {
        // Check if the ping is sucessful
        TOHDBAck *Message = static_cast<TOHDBAck *>(Data);

        if((Message->MessageID == OHDB_ACK) && (Message->RespondedMessageID == OHDB_DISABLE_PRINTING_CIRCUIT))
            InstancePtr->m_AckOk = (Message->AckStatus == OHDB_Ack_Success);
        else
            InstancePtr->m_AckOk = false;
    }
    else
        InstancePtr->m_AckOk = false;
}


// Reset OHDB
TQErrCode CPrintControl::ResetDriverCircuit(void)
{
    TOHDBGenericMessage GenericMessage;

    GenericMessage.MessageID = OHDB_RESET_DRIVER_CIRCUIT;

    m_AckOk = false;

    if(m_OHDBClient->SendInstallWaitReply(&GenericMessage,sizeof(TOHDBGenericMessage), ResetDriverCircuitAckCallback, reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
    {
        CQLog::Write(LOG_TAG_PRINT,"OHDB didn't get ack for \"ResetDriverCircuit\" message");
        throw EPrintControl("OHDB didn't get ack for \"ResetDriverCircuit\" message");
    }

    // Check if we really got ack
    if(!m_AckOk)
        throw EPrintControl("PrintControl Reset driver circuit: Invalid reply from OHDB");

    return Q_NO_ERROR;
}

void CPrintControl::ResetDriverCircuitAckCallback(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
    CPrintControl *InstancePtr = reinterpret_cast<CPrintControl *>(Cockie);

    // Check if we got ack message
    // Ignore messages smaller the ack message size
    if(DataLength == sizeof(TOHDBAck))
    {
        // Check if the ping is sucessful
        TOHDBAck *Message = static_cast<TOHDBAck *>(Data);

        if(Message->MessageID == OHDB_ACK)
        {
            if (Message->RespondedMessageID == OHDB_RESET_DRIVER_CIRCUIT)
                InstancePtr->m_AckOk = (Message->AckStatus == OHDB_Ack_Success);
            else
                InstancePtr->m_AckOk = false;
        }
        else
            InstancePtr->m_AckOk = false;
    }
    else
        InstancePtr->m_AckOk = false;
}


TQErrCode CPrintControl::ResetDriverStateMachine(void)
{
    TOHDBGenericMessage GenericMessage;

    GenericMessage.MessageID = OHDB_RESET_DRIVER_STATE_MACHINE;

    m_AckOk = false; // assuming no messages are sent thrugh PrintControl from separate threads

    if (m_OHDBClient->SendInstallWaitReply(&GenericMessage,sizeof(TOHDBGenericMessage), ResetDriverSMAckCallback, reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
    {
        CQLog::Write(LOG_TAG_PRINT,"OHDB didn't get ack for \"ResetDriverStateMachine\" message");
        throw EPrintControl("OHDB didn't get ack for \"ResetDriverStateMachine\" message");
    }

    // Check if we really got ack
    if(!m_AckOk)
        throw EPrintControl("PrintControl Reset driver SM: Invalid reply from OHDB");

    GenericMessage.MessageID = OHDB_RESET_DRIVER_FIFO;

    m_AckOk = false;

    if (m_OHDBClient->SendInstallWaitReply(&GenericMessage,sizeof(TOHDBGenericMessage), ResetDriverFIFOAckCallback, reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
    {
        CQLog::Write(LOG_TAG_PRINT,"OHDB didn't get ack for \"ResetDriverFIFO\" message");
        throw EPrintControl("OHDB didn't get ack for \"ResetDriverFIFO\" message");
    }

    // Check if we really got ack
    if(!m_AckOk)
        throw EPrintControl("PrintControl Reset driver FIFO: Invalid reply from OHDB");

    return Q_NO_ERROR;
}

void CPrintControl::ResetDriverSMAckCallback(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
    CPrintControl *InstancePtr = reinterpret_cast<CPrintControl *>(Cockie);

    // Check if we got ack message
    // Ignore messages smaller the ack message size
    if(DataLength == sizeof(TOHDBAck))
    {
        // Check if the ack is sucessful
        TOHDBAck *Message = static_cast<TOHDBAck *>(Data);

        if(Message->MessageID == OHDB_ACK)
        {
            if (Message->RespondedMessageID == OHDB_RESET_DRIVER_STATE_MACHINE)
                InstancePtr->m_AckOk = (Message->AckStatus == OHDB_Ack_Success);
            else
                InstancePtr->m_AckOk = false;
        }
        else
            InstancePtr->m_AckOk = false;
    }
    else
        InstancePtr->m_AckOk = false;
}


void CPrintControl::ResetDriverFIFOAckCallback(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
    CPrintControl *InstancePtr = reinterpret_cast<CPrintControl *>(Cockie);

    // Check if we got ack message
    // Ignore messages smaller the ack message size
    if (DataLength == sizeof(TOHDBAck))
    {
        // Check if the ack is sucessful
        TOHDBAck *Message = static_cast<TOHDBAck *>(Data);

        if (Message->MessageID == OHDB_ACK)
        {
            if (Message->RespondedMessageID == OHDB_RESET_DRIVER_FIFO)
            {
                InstancePtr->m_AckOk = (Message->AckStatus == OHDB_Ack_Success);
                return;
            }
        }
    }

    InstancePtr->m_AckOk = false;
}


TQErrCode CPrintControl::ApplyDefaultPrintParams(void)
{
    TOHDBGenericMessage GenericMessage;

    GenericMessage.MessageID = OHDB_APPLY_DEFAULT_PRINT_PARAMS;

    m_AckOk = false;

    if (m_OHDBClient->SendInstallWaitReply(&GenericMessage,sizeof(TOHDBGenericMessage),  ApplyDefaultAckCallback,  reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
    {
        CQLog::Write(LOG_TAG_PRINT,"OHDB didn't get ack for \"ApplyDefaultPrintParams\" message");
        throw EPrintControl("OHDB didn't get ack for \"ApplyDefaultPrintParams\" message");
    }

    // Check if we really got ack
    if(!m_AckOk)
        throw EPrintControl("PrintControl Apply default params: Invalid reply from OHDB");

    return Q_NO_ERROR;
}

void CPrintControl::ApplyDefaultAckCallback(int TransactionID, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
    CPrintControl *InstancePtr = reinterpret_cast<CPrintControl *>(Cockie);

    // Check if we got ack message
    // Ignore messages smaller the ack message size
    if(DataLength == sizeof(TOHDBAck))
    {
        // Check if ack is sucessful
        TOHDBAck *Message = static_cast<TOHDBAck *>(Data);

        if(Message->MessageID == OHDB_ACK)
        {
            if (Message->RespondedMessageID == OHDB_APPLY_DEFAULT_PRINT_PARAMS)
                InstancePtr->m_AckOk = (Message->AckStatus == OHDB_Ack_Success);
            else
                InstancePtr->m_AckOk = false;
        }
        else
            InstancePtr->m_AckOk = false;
    }
    else
        InstancePtr->m_AckOk = false;
}


//FireAll
TQErrCode CPrintControl::FireAll()
{
    TOHDBFireAllMessage FireAllMessage;

	CQLog::Write(LOG_TAG_PRINT,"Send Fire All message");

    FireAllMessage.MessageID  = OHDB_FIRE_ALL;
	FireAllMessage.Frequency  = m_ParamsMgr->FireAllFrequency;
    FireAllMessage.NumOfFires = 0; // no need to define num of fires, working with time
    FireAllMessage.Head       = 0; // all heads
	FireAllMessage.Time       = m_ParamsMgr->FireAllTime_ms;

    m_AckOk = false;

    if(m_OHDBClient->SendInstallWaitReply(&FireAllMessage,sizeof(TOHDBFireAllMessage), FireAllAckCallback, reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
    {
        CQLog::Write(LOG_TAG_PRINT,"OHDB didn't get ack for \"FireAll\" message");
        throw EPrintControl("OHDB didn't get ack for \"FireAll\" message");
    }

    // Check if we really got ack
    if(!m_AckOk)
        throw EPrintControl("PrintControl FireAll: Invalid reply from OHDB");

    return Q_NO_ERROR;
}
//FireAll
TQErrCode CPrintControl::FireAll(unsigned int head, int numOfFires,int qualityMode)
{
    TOHDBFireAllMessage FireAllMessage;

    CQLog::Write(LOG_TAG_PRINT,"Send Fire All message");

    FireAllMessage.MessageID  = OHDB_FIRE_ALL;
    FireAllMessage.Frequency  = m_ParamsMgr->HSW_FrequencyArray[qualityMode].Value();
    FireAllMessage.NumOfFires = numOfFires;
    FireAllMessage.Head       = head;
    FireAllMessage.Time       = m_ParamsMgr->FireAllTime_ms;

	CQLog::Write(LOG_TAG_GENERAL,"FireAllFrequency = %d ",FireAllMessage.Frequency);
	#ifndef OBJET_MACHINE_KESHET
	if(qualityMode == 0 )*/ /*for DM,HS we will devide frequency*/
	{
		FireAllMessage.Frequency = FireAllMessage.Frequency/2;
		CQLog::Write(LOG_TAG_GENERAL,"This is DM/HS mode,FireAllFrequency = %d ",FireAllMessage.Frequency);
    } 
	#endif
    CQLog::Write(LOG_TAG_GENERAL,"NumOfFires = %d ",FireAllMessage.NumOfFires);
    CQLog::Write(LOG_TAG_GENERAL,"FireAllTime_ms = %d ",FireAllMessage.Time);

    m_AckOk = false;

    if (m_OHDBClient->SendInstallWaitReply(&FireAllMessage,sizeof(TOHDBFireAllMessage), FireAllAckCallback, reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
    {
        CQLog::Write(LOG_TAG_PRINT,"OHDB didn't get ack for \"FireAll\" message");
        throw EPrintControl("OHDB didn't get ack for \"FireAll\" message");
    }

    // Check if we really got ack
    if(!m_AckOk)
        throw EPrintControl("PrintControl FireAll: Invalid reply from OHDB");

    return Q_NO_ERROR;
}


void CPrintControl::FireAllAckCallback(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
    CPrintControl *InstancePtr = reinterpret_cast<CPrintControl *>(Cockie);

    // Check if we got ack message
    // Ignore messages smaller the ack message size
    if(DataLength == sizeof(TOHDBAck))
    {
        // Check if the ack is sucessful
        TOHDBAck *Message = static_cast<TOHDBAck *>(Data);

        if(Message->MessageID == OHDB_ACK)
        {
            if (Message->RespondedMessageID == OHDB_FIRE_ALL)
                InstancePtr->m_AckOk = (Message->AckStatus == OHDB_Ack_Success);
            else
                InstancePtr->m_AckOk = false;
        }
        else
            InstancePtr->m_AckOk = false;
    }
    else
        InstancePtr->m_AckOk = false;
}

//FireAll
TQErrCode CPrintControl::WaitForFireAllFinished(void)
{
    const int WAIT_FOR_FIRE_ALL_END = (CAppParams::Instance()->SimulatorMode == true ) ? CAppParams::Instance()->FactorTimeBetween * QMsToTicks(2000) : QMsToTicks(2000);
    TQErrCode Err;
    m_Waiting=true;

    // Wait for ack
    if(m_FireAllWaitingEvent.WaitFor(WAIT_FOR_FIRE_ALL_END + QMsToTicks(m_ParamsMgr->FireAllTime_ms)) != QLib::wrSignaled)
    {
        CQLog::Write(LOG_TAG_GENERAL,"OHDB: fire all done notification not received");
        m_Waiting = false;

        if(GetFireAllStatus() != Q_NO_ERROR)
        {
            QSleepSec(3); // Wait and s
            if((Err = GetFireAllStatus()) != Q_NO_ERROR)
            {
                return Err;
                //throw EPrintControl("OHDB:FireAll - end of fire all not detected");
            }
        }
        return Q_NO_ERROR;
    }

    m_Waiting = false;
    return Q_NO_ERROR;
}

void CPrintControl::FireAllEndMessageReceived(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
    CPrintControl *InstancePtr = reinterpret_cast<CPrintControl *>(Cockie);

    // Check if the ack is sucessful
    TOHDBFireAllDoneResponse *Message = static_cast<TOHDBFireAllDoneResponse *>(Data);

    if(Message->MessageID != OHDB_FIRE_ALL_DONE)
    {
        CQLog::Write(LOG_TAG_GENERAL,"Fire all done response Message Id error:%d",
                     (int)Message->MessageID);
        return;
    }

    // Check if we got ack message
    // Ignore messages different then ack message size
    if(DataLength != sizeof(TOHDBFireAllDoneResponse))
    {
        CQLog::Write(LOG_TAG_GENERAL,"Fire all done response length error:%d",
                     (int)DataLength);
        return;
    }

    int Counter = static_cast<int>(Message->FireAllCounter);

    // Trigger the event
    if(InstancePtr->m_Waiting)
    {
        InstancePtr->m_FireAllWaitingEvent.SetEvent();
        CQLog::Write(LOG_TAG_GENERAL,"Fire all done message received (Counter=%d)",Counter);
    }
    else
    {
        CQLog::Write(LOG_TAG_GENERAL,"Fire all done message received after timeout (Counter=%d)",
                     Counter);
    }

    InstancePtr->AckToOHDBNotification(OHDB_FIRE_ALL_DONE,  TransactionId, ACK_STATUS_SUCCESS, Cockie);
}


//FireAll Status message
TQErrCode CPrintControl::GetFireAllStatus(void)
{
    TOHDBGetFireAllStatusMessage FireAllStatusMessage;

    // Verify if we are already not performing this command
    if (m_FireAllStatusCommandInUse)
        throw EPrintControl("WaitForFireAllFinished:two message send in the same time");

    m_FireAllStatusCommandInUse=true;

    FireAllStatusMessage.MessageID = OHDB_GET_FIRE_ALL_STATUS;

    m_StatusAckOk = false;
    m_FireAllEndOk = false;

    if(m_OHDBClient->SendInstallWaitReply(&FireAllStatusMessage,sizeof(TOHDBGetFireAllStatusMessage), FireAllStatusResponseCallback, reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
    {
        m_FireAllStatusCommandInUse=false;
        CQLog::Write(LOG_TAG_PRINT,"OHDB didn't get ack for \"GetFireAllStatus\" message");
        throw EPrintControl("OHDB didn't get ack for \"GetFireAllStatus\" message");
    }

    // Check if we really got ack
    if(! m_StatusAckOk)
    {
        m_FireAllStatusCommandInUse=false;
        throw EPrintControl("PrintControl FireAll Status: Invalid reply from OHDB");
    }

    if(! m_FireAllEndOk)
    {
        m_FireAllStatusCommandInUse=false;
        return Q2RT_FATAL_ERROR;
        //throw EPrintControl("Fire all did not finish(status)",Q2RT_FATAL_ERROR);
    }

    m_FireAllStatusCommandInUse=false;
    return Q_NO_ERROR;
}

void CPrintControl::FireAllStatusResponseCallback(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
    // Get a pointer to the instance
    CPrintControl *InstancePtr = reinterpret_cast<CPrintControl *>(Cockie);

    // Build the fire all message
    TOHDBFireAllStatusResponse *StatusMsg = static_cast<TOHDBFireAllStatusResponse *>(Data);

    // Check if we got ack message
    // Ignore messages smaller the ack message size
    if(DataLength != sizeof(TOHDBFireAllStatusResponse))
    {
        FrontEndInterface->NotificationMessage("Fire All \"GetStatusAckResponse\" length error");
        CQLog::Write(LOG_TAG_HEAD_FILLING,"Fire All \"GetStatusAckResponse\" length error");

        if(StatusMsg->MessageID != OHDB_FIRE_ALL_STATUS)
            CQLog::Write(LOG_TAG_HEAD_FILLING,"Fire All Message ID Error = %d",(int)StatusMsg->MessageID );

        InstancePtr->m_StatusAckOk = false;
        return;
    }

    if(StatusMsg->MessageID != OHDB_FIRE_ALL_STATUS)
    {
        InstancePtr->m_StatusAckOk = false;
        return;
    }

    int Counter = static_cast<int>(StatusMsg->FireAllCounter);
    if(StatusMsg->Status == FIRE_ALL_ACTIVE)
    {
        CQLog::Write(LOG_TAG_GENERAL,"Fire All status ACTIVE (not end error)- Counter=%d",Counter);
        InstancePtr->m_FireAllEndOk = false;
    }
    else
    {
        CQLog::Write(LOG_TAG_GENERAL,"Fire All status idle - Counter=%d",Counter);
        InstancePtr->m_FireAllEndOk = true;
    }
    InstancePtr->m_StatusAckOk = true;
}

//-----------------------------------------------
void CPrintControl::AckToOHDBNotification(int MessageID, int TransactionID, int AckStatus, TGenericCockie Cockie)
{
    TOHDBAck AckMsg;

    // Get a pointer to the instance
    CPrintControl *InstancePtr = reinterpret_cast<CPrintControl *>(Cockie);

    AckMsg.MessageID          = static_cast<BYTE>(OHDB_EDEN_ACK);
    AckMsg.RespondedMessageID = static_cast<BYTE>(MessageID);
    AckMsg.AckStatus          = static_cast<BYTE>(AckStatus);

    // Send a Ack message
    InstancePtr->m_OHDBClient->SendNotificationAck(TransactionID, &AckMsg, sizeof(TOHDBAck));
}

//-----------------------------------------------
void CPrintControl::Cancel(void)
{
    if(m_Waiting)
    {
        CQLog::Write(LOG_TAG_GENERAL,"Fire All - cancel procedure called");
        m_FireAllWaitingEvent.SetEvent();
        m_Waiting=false;
    }
}


//-----------------------------------------------
// Dummy Procedures

// Constructor
CPrintControlDummy::CPrintControlDummy (const QString& Name) : CPrintControl (Name)
{
}

// Destructor
CPrintControlDummy::~CPrintControlDummy(void)
{
}

// Set parameters for job configuration (should be called once per job)
void CPrintControlDummy::SetConfigParams(TPulseType PulseType,
										 int   DataMultiplier,
										 int   ResolutionDivider,
										 float PulseWidthSingle_uSec,
										 float PulseWidthDouble_P1_uSec,
										 float PulseWidthDouble_P2_uSec,
										 float IntervalToSecondPulse_uSec,
										 float PropagationDelay_uSec,
										 int   VoltagePercentage)
{
}

// Set parameters for a single layer
TQErrCode CPrintControlDummy::SetLayerParams(USHORT StartPEG,USHORT EndPEG,USHORT NoOfFires)
{
    return Q_NO_ERROR;
}
// Start printing (enable printing circuitry)
void CPrintControlDummy::Go(void)
{
}

// Stop printing (disable printing circuitry)
void CPrintControlDummy::Stop(void)
{
}

// Reset OHDB
TQErrCode CPrintControlDummy::ResetDriverCircuit(void)
{
    return Q_NO_ERROR;
}

TQErrCode CPrintControlDummy::ResetDriverStateMachine(void)
{
    return Q_NO_ERROR;
}

TQErrCode CPrintControlDummy::ApplyDefaultPrintParams(void)
{
    return Q_NO_ERROR;
}

TQErrCode CPrintControlDummy::FireAll(void)
{

    return Q_NO_ERROR;
}

TQErrCode CPrintControlDummy::FireAll(unsigned int head,int numOfFires,int qualityMode)
{
    return Q_NO_ERROR;
}

TQErrCode CPrintControlDummy::WaitForFireAllFinished(void)
{
    return Q_NO_ERROR;
}

void CPrintControlDummy::Cancel(void)
{
}

TQErrCode CPrintControlDummy::GetFireAllStatus(void)
{
    return Q_NO_ERROR;
}


