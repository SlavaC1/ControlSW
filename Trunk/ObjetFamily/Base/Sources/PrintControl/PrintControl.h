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
 * Author: Ran / Rachel.                                            *
 * Start date: 30/01/2002                                           *
 ********************************************************************/

#ifndef _PRINT_CONTROL_H_
#define _PRINT_CONTROL_H_

#include "OHDBProtocolClient.h"
#include "QEvent.h"
#include "AppParams.h"


const int BiDirectionSequencer = 0;
const int ForwardSequencer     = 1;
const int BackwardSequencer    = 2;

const double FPGA_MASTER_CLOCK         = 33333333; // 33.33 MHz
const int    PULSE_SLEW_RATE           = 9;
const int    NATIVE_ENCODER_RESOLUTION = 4800;

enum TPulseType
{
	pulSingle,
	pulDouble	
};

// Exception class for all the print control class
class EPrintControl : public EQException
{
public:
    EPrintControl(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

// Print control class
class CPrintControl : public CQComponent
{
private:
    COHDBProtocolClient *m_OHDBClient;

    bool m_AckOk;
    bool m_FireAllEndOk;
    bool m_StatusAckOk;
    bool m_Waiting;

    bool m_FireAllStatusCommandInUse;

    CQEvent m_FireAllWaitingEvent;

    CAppParams *m_ParamsMgr;

    // Completion routine for the ACK reply
    static void SetConfigParamsAckCallback(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);
    static void SetLayerAckCallback(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);
    static void GoAckCallback(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);
    static void StopAckCallback(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);
    static void ResetDriverCircuitAckCallback(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);
    static void ResetDriverSMAckCallback(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);
    static void ResetDriverFIFOAckCallback(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);
    static void ApplyDefaultAckCallback(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);
    static void FireAllAckCallback(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);
    static void FireAllEndMessageReceived(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);
	static void FireAllStatusResponseCallback(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);

	// Replay Ack to OCb
	void AckToOHDBNotification (int MessageID, int TransactionID, int AckStatus, TGenericCockie Cockie);

	int ConvertTimeToFPGAClockCounts(double Time);

public:
    // Constructor
    CPrintControl(const QString& Name);

    // Destructor
    ~CPrintControl(void);

    // Set default parameters for job configuration (should be called once per job)
    DEFINE_METHOD(CPrintControl,TQErrCode,SetDefaultConfigParams);

    // Set parameters for job configuration (should be called once per job)
	virtual void SetConfigParams(TPulseType PulseType,
								 int   DataMultiplier,
								 int   ResolutionDivider,
								 float PulseWidthSingle_uSec,
								 float PulseWidthDouble_P1_uSec,
								 float PulseWidthDouble_P2_uSec,
								 float IntervalToSecondPulse_uSec,
								 float PropagationDelay_uSec,
								 int   VoltagePercentage);

    // Set parameters for a single layer
    DEFINE_V_METHOD_3(CPrintControl,TQErrCode,SetLayerParams,USHORT,USHORT,USHORT);

    // Start printing (enable printing circuitry)
    virtual void Go(void);

    // Stop printing (disable printing circuitry)
    virtual void Stop(void);

    // Reset OHDB
    DEFINE_V_METHOD(CPrintControl,TQErrCode,ResetDriverCircuit);
    DEFINE_V_METHOD(CPrintControl,TQErrCode,ResetDriverStateMachine);
    DEFINE_V_METHOD(CPrintControl,TQErrCode,ApplyDefaultPrintParams);

    //Perform Fire all
    DEFINE_V_METHOD(CPrintControl,TQErrCode,FireAll);
    DEFINE_V_METHOD(CPrintControl,TQErrCode,WaitForFireAllFinished);

    DEFINE_V_METHOD(CPrintControl,TQErrCode,GetFireAllStatus);
    virtual TQErrCode FireAll(unsigned int head,int numOfFires,int qualityMode);
    //Cancel
    virtual void Cancel(void);

};


// Print control dummy class
class CPrintControlDummy : public CPrintControl
{
public:
    // Constructor
    CPrintControlDummy(const QString& Name);

    // Destructor
    ~CPrintControlDummy(void);

    // Set parameters for job configuration (should be called once per job)
	void SetConfigParams(TPulseType PulseType,
						 int   DataMultiplier,
						 int   ResolutionDivider,
						 float PulseWidthSingle_uSec,
						 float PulseWidthDouble_P1_uSec,
						 float PulseWidthDouble_P2_uSec,
						 float IntervalToSecondPulse_uSec,
						 float PropagationDelay_uSec,
						 int   VoltagePercentage);

    // Set parameters for a single layer
    TQErrCode SetLayerParams(USHORT StartPEG,USHORT EndPEG,USHORT NoOfFires);

    // Start printing (enable printing circuitry)
    void Go(void);

    // Stop printing (disable printing circuitry)
    void Stop(void);

    // Reset OHDB
    TQErrCode ResetDriverCircuit(void);
    TQErrCode ResetDriverStateMachine(void);
    TQErrCode ApplyDefaultPrintParams(void);
    TQErrCode FireAll(void); //Perform Fire all
    TQErrCode WaitForFireAllFinished(void);
    TQErrCode GetFireAllStatus(void);
    TQErrCode FireAll(unsigned int head, int numOfFires,int qualityMode);
    void Cancel(void);
};

#endif

