/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Q2RT Application class.                                  *
 * Module Description: This class inherits from the CQApplication   *
 *                     class and implement the specific Q2RT        *
 *                     initializations.                             *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 12/08/2001                                           *
 * Last upate: 19/05/2003                                           *
 ********************************************************************/

#ifndef _Q2RT_APPLICATION_H_
#define _Q2RT_APPLICATION_H_

#include "QApplication.h"
#include <Classes.hpp> //bug 5719
#include <ExtCtrls.hpp>
#include "GlobalDefs.h" //objet_machine
#include "Tester.h"


// Total number of different com ports in the system
const int TOTAL_COM_NUM    = 3;
/*
const int mtEden260        = 0;
const int mtEden330        = 1;
const int mtEden500        = 2;
const int mtEden250        = 3;
const int mtEden350        = 4;
const int mtEden350_V      = 5;
const int mtEden3Resin500v = 6;
const int mtEden3Resin350v = 7;
const int mtEden260_V      = 8;
const int mtEden3Resin260v = 9;
const int mtObjet260     = 10;
const int mtObjet350     = 11;
const int mtObjet500     = 12;
const int mtObjet1000     = 13;

const int mfEden        = 0;
const int mfConnex      = 1;
const int mfObjet     = 2;
typedef int TMachineFOM;       */

typedef int TMachineType;

// Exception class for the application
class EQ2RTApplication : public EQException {
  public:
    EQ2RTApplication(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

// Class forwards
class CEdenProtocolEngine;
class CEdenProtocolClient;
class CQBaseComPort;
class CMachineManager;
class CLayerProcess;
class CTester;
class CRPCClient;
class CRPCServer;
class CFrontEndInterface;
class CHostComm;
class CMiniSequencer;
class CBITManager;
class CRadioGroupWizardPage;
class CCheckBoxWizardPage;

struct TLogFile
{
	QString FileName;
	time_t FileDateTime;
};

typedef std::vector<TLogFile> TLogFileList;

bool operator < (const TLogFile& A,const TLogFile& B)
{
  return (A.FileDateTime < B.FileDateTime);
}

class CQ2RTApplication : public CQApplication
{
  private:
    // Protocol engines for all the system com ports (except the terminal com port)
    // Note: The extra "1" in theis array initialization is in order to save place
    //       for emulation purposes. If the macro Q2RT_EMULATION is defined the 0 entry
    //       of this array is initialized with an emulated com port.
    CEdenProtocolEngine *m_ProtocolEngines[TOTAL_COM_NUM];

    // Pointer to the instance of the machine manager class
    CMachineManager *m_MachineManager;

    // Pointer to the instance of the layer process class
    CLayerProcess *m_LayerProcess;

    // RPC server and client objects
    CRPCServer *m_RPCServer;
    CRPCClient *m_RPCClient;    

    // Tester object
    CTesterBase *m_Tester;

    // Build-In-Test (BIT) manager 
    CBITManager *m_BITManager;

    CHostComm *m_HostComm;

    // A mini sequencer is required to support expressions evaluation
    CMiniSequencer *m_MiniSequencer;

    bool m_IsPciCardInitialized;

    // Application init function (override)
    bool AppInit(void);

    // Application start function (override)
    void AppStart(void);

    // Application stop function (override)
    void AppStop(void);

    // Menu event action for expression evaluation
    static void EvaluateExpressionAction(TGenericCockie Cockie);

    // Create an instance of a com port according to a com number
    CQBaseComPort *ComPortFactory(int ComNum);

    // Initialize communication related components
    void InitCommComponents(void);

    // Initialize the RPC system
    void InitRPC(void);

    // Initialize the log file
    void InitLogFile(void);

    // indicates that the log file has been initialized
    bool IsLogFileInit;

    // De-Initialize the log file
	void DeInitLogFile(void);

    void SetMachineType(TMachineType) {}

	int GetHaspFeatureNum(void);
	
    void SetMaintenanceCountersDefaultWarnings(void);

    static void LogFileChangeEventHandler(const QString& NewLogFileName,TGenericCockie Cockie);

    //Display procedure for the Edenprotocolclient use
    static void PrintLogMessage(int,TGenericCockie);

    static void FIFOUnderrunErrorCallback(TGenericCockie);

	bool m_QATestsAllowed;
	bool m_SolubleSupportAllowed; 


  public:
    // Constructor
    CQ2RTApplication(void);

    // Destructor
    virtual ~CQ2RTApplication(void);

    DEFINE_PROPERTY(CQ2RTApplication,TMachineType,MachineType);

    // Refresh the BIt system registration (only built in tests not scriptable tests)
    void RefreshBIT();

    // Returns the main communications thread's id (Lotus), or negative number upon error
	int GetCommThreadId() const;

    // Apply "pushed" parameters changes
    void ApplyParametersChange(void);

    // Utility function to flexibly specify a file name with the following options:
    // 1) Full path.
    // 2) Relative to application root dir
    QString FlexibleFileName(const QString FileName);

    // Return a pointer to the machine manager instance
    CMachineManager *GetMachineManager(void);

    // Return a pointer to the layer process instance
    CLayerProcess *GetLayerProcess(void) ;

    // Return a pointer to the RPC server object
    CRPCClient *GetRPCClient(void) ;

    // Return a pointer to the host communication object
    CHostComm *GetHostComm(void) ;

    // Return a pointer to the internal tester object
    CTesterBase *GetTester(void) ;

    CMiniSequencer *GetMiniSequencer(void) ;

    CBITManager *GetBITManager(void) ;

    TQErrCode ImportConfiguration(const QString FileName);
    DEFINE_METHOD(CQ2RTApplication,TQErrCode,LogThreadsProperties);
    DEFINE_METHOD(CQ2RTApplication,bool,IsPciCardInitialized);

    //bug 5950
    TTimer * m_timeoutTimer;
    DEFINE_METHOD_1(CQ2RTApplication,bool,EnableTimer,bool);
    DEFINE_METHOD_1(CQ2RTApplication,bool,SetTimer,int);
    DEFINE_METHOD_1(CQ2RTApplication,bool,SetTimerInterval,int);
    void __fastcall TimeoutTimerEvent(TObject *Sender);

    //bug 5719
    void ExecStringSafe(TStrings *Strings,bool ClearDictionary = true);
    QString GetLastLogFileName();
    void ExecFileSafe(QString FileName);
    void SetQATestsAllowed(bool allowed);
	void SetSolubleSupportAllowed(bool allowed);
	bool GetQATestsAllowed();
	bool GetSolubleSupportAllowed();
	void RegisterToPythonExceptionThrowers(DWORD ThreadID);
	void SetMachineSinceDateParam();

	QString GetMachineTypeAsString();
    TMachineType GetMachineType(void);

};

// The Q2RT instance is global
extern CQ2RTApplication *Q2RTApplication;

#endif
