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
 * Last upate: 23/07/2003                                           *
 ********************************************************************/

#include "Q2RTApplication.h"
#include "QMenusHandler.h"
#include "QStdComPort.h"
#include "QNullComPort.h"
#include "MCBProtocolClient.h"
#include "OCBProtocolClient.h"
#include "OHDBProtocolClient.h"
#include "EdenProtocolClientIDs.h"
#include "MiniSequencer.h"
#include "AppParams.h"
#include "QUtils.h"
#include "Q2RTErrors.h"
#include "EdenPCISys.h"
#include "MachineSequencer.h"
#include "LayerProcess.h"
#include "Tester.h"
#include "RPCServer.h"
#include "RPCClient.h"
#include "FrontEnd.h"
#include "QFileSystem.h"
#include "HostComm.h"
#include "QTcpIpServerComPort.h"
#include "AppLogFile.h"
#include "QVersionInfo.h"
#include "MaintenanceCounters.h"
#include "FIFOPCI.h"
#include "PrintControl.h"
#include "BITManager.h"
#include "AppBIT.h"
//#include "GlobalDefs.h" objet_machine
#include "WizardPages.h"
#include "AutoWizardDlg.H"
#include "ModesManager.h"
#include "FEResources.h"
#include "rfid.h"
#include "Container.h"
#include <time.h>
#include "Hasp.h"
#include "QPythonIntegrator.h"
#include "BackEndInterface.h" //bug 5950
#include "FrontEndParams.h"
#include "CActionsHistoryTable.h"
#include "MainUnit.h"

#if defined(OS_WINDOWS) && !defined(EDEN_CL) 
// Use functions from the about box for up-to-date build time and date info
#include "About.h"
#endif

// Maximum size of the log file in bytes
const int MAX_LOG_FILE_SIZE = 1024 * 1024 * 3;

// Maintenance counters file name
const int TCP_IP_COM_PORT_NUM = 3;

// The Q2RT instance is global
CQ2RTApplication *Q2RTApplication = NULL;


// Absolute path for the application directory (VxWorks only)
#define APPLICATION_FILE_PATH "c:\\projects\\Quadra2\\Q2RT\\ProjectFiles\\Windows\\"

// Class CQ2RTApplication implementation
// ********************************************************************/

// Constructor
CQ2RTApplication::CQ2RTApplication(void)
{
   m_RPCServer = NULL;
   m_RPCClient = NULL;
   m_Tester = NULL;
   m_BITManager = NULL;
   m_MiniSequencer = NULL;
   m_timeoutTimer = NULL;
  INIT_PROPERTY(CQ2RTApplication,MachineType,SetMachineType,GetMachineType);
  INIT_METHOD(CQ2RTApplication,LogThreadsProperties);
  INIT_METHOD(CQ2RTApplication,IsPciCardInitialized);

  //bug 5950
  INIT_METHOD(CQ2RTApplication,EnableTimer); 
  INIT_METHOD(CQ2RTApplication,SetTimer);
  INIT_METHOD(CQ2RTApplication,SetTimerInterval);

  m_MachineManager = NULL;
  m_LayerProcess   = NULL;
  m_HostComm       = NULL;

  for(int i = 0; i < TOTAL_COM_NUM; i++)
    m_ProtocolEngines[i] = NULL;

  m_IsPciCardInitialized = true;

  CAppGlobalDefs::Init();
  CAppGlobalDefs::Instance();

  IsLogFileInit = false;

  m_QATestsAllowed = false;
  m_SolubleSupportAllowed = false;
}

// Destructor
CQ2RTApplication::~CQ2RTApplication(void)
{
  CAppGlobalDefs::DeInit();
  
  if(CHECK_NOT_EMULATION(CAppParams::Instance()->DataCard_Emulation))
  {
	DWORD dwStatus = EdenPCI_DeInit();
	CQLog::Write(LOG_TAG_PRINT,QFormatStr("EdenPCI_DeInit(), dwStatus: %s", Stat2Str(dwStatus)));
  }

  CAppParams::DeInit();
  CModesManager::DeInit();
//OBJET_MACHINE feature
  CBatchNoTable::GetInstance()->DeInit();
  CActionsHistoryTable::GetInstance()->DeInit();

  DeInitLogFile();
}

// Return machine type code
TMachineType CQ2RTApplication::GetMachineType()
{
#if defined(OBJET_1000)
  return mtObjet1000;
#elif defined(OBJET_500)
  return mtObjet500;
#elif defined(OBJET_350)
  return mtObjet350;
#elif defined(OBJET_260)
  return mtObjet260;
#endif
}

QString CQ2RTApplication::GetMachineTypeAsString()
{
#if defined(OBJET_1000)
  return "Objet1000";
#elif defined(OBJET_500)
  return "Keshet500";
//   return "Objet500";
#elif defined(OBJET_350)
  return "Objet350";
#elif defined(OBJET_260)
  return "Objet260";
#endif
}
// Return machine type code
int CQ2RTApplication::GetHaspFeatureNum(void)
{
#if defined(OBJET_1000)
  return 581;
#elif defined(OBJET_500)
  return 8501;
#elif defined(OBJET_350)
  return 3811;
#elif defined(OBJET_260)
  return 2911;
#endif
}

// Create an instance of a com port according to a com number
CQBaseComPort *CQ2RTApplication::ComPortFactory(int ComNum)
{
  CAppParams *ParamsMgr = CAppParams::Instance();
  CQBaseComPort *ComPort = NULL;

  try
  {
    // Com numbers 1 and 2 are standard com ports
    switch(ComNum)
    {
      case 0:
        break;

      case 1:
        if(ParamsMgr->EnableCom1)
          ComPort = new CQStdComPort(ComNum);
        break;

      case 2:
        if(ParamsMgr->EnableCom2)
          ComPort = new CQStdComPort(ComNum);
        break;

      case TCP_IP_COM_PORT_NUM:
        if(ParamsMgr->EnableTcpIpComPort)
          ComPort = new CQTcpIpServerComPort;
        break;

      default:
        // We should never be here
        throw EQ2RTApplication("Invalid com port number");
    }
    // In a case of an error, show warning message and create a NULL com port
  } catch(EQException& Err)
    {
      QMonitor.WarningMessage(Err.GetErrorMsg());
      Q_SAFE_DELETE(ComPort);
    }

  if(! ComPort)
    ComPort = new CQNullComPort(ComNum);

  return ComPort;
}

// Initialize communication related components
void CQ2RTApplication::InitCommComponents(void)
{
  CAppParams *ParamsMgr = CAppParams::Instance();

  // Create all the protocol engines except for the terminal port and the tester
  for(int i = 0; i < TOTAL_COM_NUM; i++)
    if(i != (ParamsMgr->TerminalComNum - 1))
    {
      // Create a protocl engine instance for each com port in the system
      m_ProtocolEngines[i] = new CEdenProtocolEngine(ComPortFactory(i + 1));
    } else
        m_ProtocolEngines[i] = NULL;

  // Attach the communication clients to the engines
  // -----------------------------------------------

  // Initialize the OCB client
  if(m_ProtocolEngines[ParamsMgr->OCBComNum - 1] != NULL)
    COCBProtocolClient::Init(m_ProtocolEngines[ParamsMgr->OCBComNum - 1]);
  else
    throw EQ2RTApplication("Invalid OCB com port number");

  // Initialize the MCB client
  if(m_ProtocolEngines[ParamsMgr->MCBComNum - 1] != NULL)
    CMCBProtocolClient::Init(m_ProtocolEngines[ParamsMgr->MCBComNum - 1]);
  else
    throw EQ2RTApplication("Invalid MCB com port number");

  // Initialize the OHDB client
  if(m_ProtocolEngines[ParamsMgr->OHDBComNum - 1] != NULL)
    COHDBProtocolClient::Init(m_ProtocolEngines[ParamsMgr->OHDBComNum - 1]);
  else
    throw EQ2RTApplication("Invalid OHDB com port number");

//  COCBProtocolClient *OCBClient = COCBProtocolClient::Instance();
//  OCBClient->m_PrintLogMessage = &PrintLogMessage;

//  CMCBProtocolClient *MCBClient =CMCBProtocolClient::Instance();
//  MCBClient->m_PrintLogMessage = &PrintLogMessage;

//  COHDBProtocolClient *OHDBClient = COHDBProtocolClient::Instance();
//  OHDBClient->m_PrintLogMessage = &PrintLogMessage;

  InitRPC();
}

int CQ2RTApplication::GetCommThreadId() const
{
	try {
		return m_ProtocolEngines[(CAppParams::Instance())->OCBComNum -1]->GetTidReceiver();
	} catch (...) {
		return -1;
	}
}

// Application init function (override)
bool CQ2RTApplication::AppInit(void)
{
  if(QFileSystemInit() != Q_NO_ERROR)
    QMonitor.WarningMessage("Can not initialize file system");
#ifdef OS_VXWORKS
  else
  {
    // Set starting work directory
    if(QChangeDirectory(APPLICATION_FILE_PATH) != Q_NO_ERROR)
      QMonitor.WarningMessage("Can not change directory to application directory");
  }

  AppFilePath.Value() = APPLICATION_FILE_PATH;
#endif

  // Set high priority for this process (windows only)
#ifdef OS_WINDOWS  
  SetPriorityClass(GetCurrentProcess(),HIGH_PRIORITY_CLASS);
  //Set process affinity
  DWORD_PTR processAffinityMask = 0xCC;
/*  DWORD_PTR Mask = 0x1;
  SYSTEM_INFO sysinfo;
  GetSystemInfo( &sysinfo );
  int numCPU = sysinfo.dwNumberOfProcessors;
  for(int i=0;i<numCPU;i+=2)
	processAffinityMask |= (Mask<<i);
  SetProcessAffinityMask(GetCurrentProcess(),processAffinityMask); */
#endif
  

  // Init application parameters manager
  if( ! CAppParams::Init(AppFilePath.Value() + LOAD_STRING(IDS_CONFIGS_DIRNAME)) )
    return false;
  // Initialize the global error handler
  CErrorHandler::Init();

  // Initialize the log file
  InitLogFile();

  // Create a mini-sequencer object
  m_MiniSequencer = new CMiniSequencer("AppMiniSequencer");

  // Initilize the modes manager
  CModesManager::Init(CAppParams::Instance(), AppFilePath.Value(), m_MiniSequencer);

  CAppParams::Instance()->EnableDisableSaveEvents(true);

  InitFrontEnd();
  CModesManager::Instance()->EnterMode(DEFAULT_MODE, DEFAULT_MODES_DIR);
  CModesManager::Instance()->EnterMode(OPERATION_MODE_NAME(CAppParams::Instance()->PipesOperationMode), GENERAL_MODES_DIR);
  //if(CAppParams::Instance()->USING_RFID)
  //{
#ifndef DONT_USING_RFID
    CRFID::Init();
#endif
  //}

  // Initilize the maintenance counters system
  CMaintenanceCounters::Init(NUM_OF_MAINTENANCE_COUNTERS, AppFilePath.Value() + LOAD_STRING(IDS_CONFIGS_DIRNAME) + "Maintenance.dat");
  SetMaintenanceCountersDefaultWarnings();
  CMaintenanceCounters::GetInstance()->LoadFromFile();
  SetMachineSinceDateParam(); //OBJET_MACHINE
  ApplyParametersChange();  

  // Initialize the data card
  if(CHECK_NOT_EMULATION(CAppParams::Instance()->DataCard_Emulation))
  {
	DWORD dwStatus = EdenPCISystem_Init();
	if(dwStatus != WD_STATUS_SUCCESS)
	{
	  m_IsPciCardInitialized = false;
	  CQLog::Write(LOG_TAG_PRINT,QFormatStr("Data PCI card problem, dwStatus: %s", Stat2Str(dwStatus)));
	  QMonitor.WarningMessage(QFormatStr("Data PCI card problem: %s", Stat2Str(dwStatus)));
	}
	else
    {
      m_IsPciCardInitialized = true;
      SetFIFOUnderrunErrorCallback(FIFOUnderrunErrorCallback,NULL);
    }
  }

  // Enable m_QATestsAllowed flag
  // ------------------------------------------------------------------------
  Chasp hasp1(ChaspFeature::fromFeature(INT_FROM_RESOURCE(IDN_HASP_QA_TESTS_ALLOWED_FEATURE)));
  try
  {
    haspStatus status = hasp1.login(HASP_OBJET_VENDOR_CODE, HASP_LOCAL_SCOPE);
    bool enable = false;
    if(HASP_SUCCEEDED(status))
    {
        enable = true;
    }
    else
    {
        enable = false;
    }
    SetQATestsAllowed(enable);
  }
  __finally
  {
    hasp1.logout();
  }

  // Initialize communication related components
  InitCommComponents();  //note: SetQATestsAllowed must be invoked prior to this call

  m_LayerProcess   = new CLayerProcess;
  m_HostComm = new CHostComm;

  // Create a mini-sequencer object

  // Create other system objects

  m_MachineManager = new CMachineManager;

  if(CHECK_NOT_EMULATION(CAppParams::Instance()->DataCard_Emulation) && CHECK_NOT_EMULATION(CAppParams::Instance()->OHDB_Emulation))
  {
  	m_Tester = new CTester;
  }
  else
  {
  	m_Tester = new CTesterDummy;
  }

  // Initialize BIT system
  m_BITManager = new CBITManager;
  AppBIT::Init();
  RefreshBIT();

//OBJET_MACHINE feature
  CActionsHistoryTable::GetInstance()->Init();
   // ------------------------------------------------------------------------
	 // Enable m_SolubleSupportAllowed flag
  // ------------------------------------------------------------------------

	TLicensedMaterialsList MaterialsList  = CBackEndInterface::Instance()->GetLicensedMaterialsList();

	for (TLicensedMaterialsList::iterator it = MaterialsList.begin() ; it != MaterialsList.end() ; it++ )
	{
		if( it->MaterialName == "SUP706" )
		{
			SetSolubleSupportAllowed(true);
			break;
		}
	}
  // ------------------------------------------------------------------------
  // Protect "Show incoming slices" menu
  // ------------------------------------------------------------------------
  Chasp hasp(ChaspFeature::fromFeature(INT_FROM_RESOURCE(IDN_HASP_INCOMING_SLICES_PROTECTION_FEATURE)));
  try
  {
	haspStatus status = hasp.login(HASP_OBJET_VENDOR_CODE, HASP_LOCAL_SCOPE);
    bool show = false;
    if(HASP_SUCCEEDED(status))
    {
        show = true;
    }
    else
    {
        show = false;
    }
    m_MachineManager->GetMachineSequencer()->HandleIncomingSlicesProtection(show);
  }
  __finally
  {
    hasp.logout();
  }
  // ------------------------------------------------------------------------

#ifndef EMULATION_ONLY
  //HASP feature number is generated from application version, for example Connex500 ver 57.1 - HASP feature: 571
  ChaspFeature feature = ChaspFeature::fromFeature(GetHaspFeatureNum());
  CHaspInterfaceWrapper::Init(feature, LOG_TAG_HASP, m_MachineManager->GetMachineSequencer()->HandleHaspStatus);
#else
  CHaspInterfaceWrapper::Init();
#endif

  LogThreadsProperties();

  try
  {
  	m_timeoutTimer = new TTimer(NULL); //bug 5950
  }
  catch (std::bad_alloc& ba)
  {
     m_timeoutTimer = NULL;
     CQLog::Write(LOG_TAG_GENERAL,"m_timeoutTimer - Memory allocation failed: %s",ba.what());
  }
   QMonitor.SetServiceCallsCheckingMsgSafety(
	CFrontEndInterface::IsWizardRunning,
	CFrontEndInterface::CancelWizard);
  return true;
}

// Application start function (override)
void CQ2RTApplication::AppStart(void)
{
  // Start the protocol engines
  for(int i = 0; i < TOTAL_COM_NUM; i++)
    if(m_ProtocolEngines[i] != NULL)
      m_ProtocolEngines[i]->Start();

  m_LayerProcess->Resume();
  m_MachineManager->Start();
  m_HostComm->Start();
  m_RPCServer->Start();

  CMaintenanceCounters *MCounters = CMaintenanceCounters::GetInstance();

  MCounters->Resume();

  // Enable the vacuum filter counter
  MCounters->EnableCounter(VACUUM_FILTER_COUNTER_ID);
  MCounters->EnableCounter(VACUUM_PUMP_COUNTER_ID);
  MCounters->EnableCounter(HEAD_FAN_FILTER_COUNTER_ID);

}

// Application stop function (override)
void CQ2RTApplication::AppStop(void)
{
  MainForm->Enabled = false; // so the user can't exit twice
  m_HostComm->SetMute(true); // so that pending answers are skipped
  m_HostComm->Terminate(); // so that are no new requests
  CRFID::Instance()->TerminateReaderTasks(); // so that no new errors interfere
  m_MachineManager->GetMachineSequencer()->GetContainerInstance()->Terminate(); // so that the RF readers are not polled while they are freed

  CMaintenanceCounters *MCounters = CMaintenanceCounters::GetInstance();
  MCounters->DisableCounter(VACUUM_FILTER_COUNTER_ID);
  MCounters->DisableCounter(VACUUM_PUMP_COUNTER_ID);
  MCounters->DisableCounter(HEAD_FAN_FILTER_COUNTER_ID);

//  m_HostComm->Terminate();
//  m_LayerProcess->Terminate();
//  m_MiniSequencer->Terminate();

//  QSleep(1000); // let everybody else finish what they are doing

  if(m_MachineManager != NULL)
    Q_SAFE_DELETE(m_MachineManager);

  for(int i = 0; i < TOTAL_COM_NUM; i++)
    if(m_ProtocolEngines[i] != NULL)
    {
      m_ProtocolEngines[i]->Stop(); // so that no new notifications interfere
    }

  if(m_LayerProcess != NULL)
    Q_SAFE_DELETE(m_LayerProcess);

  if(m_HostComm != NULL)
    Q_SAFE_DELETE(m_HostComm);

  if(m_MiniSequencer != NULL)
    Q_SAFE_DELETE(m_MiniSequencer);

  COCBProtocolClient::DeInit();
  CMCBProtocolClient::DeInit();
  COHDBProtocolClient::DeInit();

  Q_SAFE_DELETE(m_Tester);
  Q_SAFE_DELETE(m_RPCClient);
  Q_SAFE_DELETE(m_RPCServer);
  Q_SAFE_DELETE(m_BITManager);
  AppBIT::DeInit();

  CMaintenanceCounters::DeInit();

  // Stop all the protocol engines
  for(int i = 0; i < TOTAL_COM_NUM; i++)
    if(m_ProtocolEngines[i] != NULL)
    {
//      m_ProtocolEngines[i]->Stop();
      Q_SAFE_DELETE(m_ProtocolEngines[i]);
	}

  m_timeoutTimer->Enabled = false;
  Q_SAFE_DELETE(m_timeoutTimer);

  CRFID::DeInit();

  // Deinit the error handler
  CErrorHandler::DeInit();

  CFrontEndParams::DeInit();
  DeInitFrontEnd();

  CHaspInterfaceWrapper::DeInit();
}

// Initialize the RPC system
void CQ2RTApplication::InitRPC(void)
{
  CAppParams *ParamsMgr = CAppParams::Instance();

  if(m_ProtocolEngines[ParamsMgr->TesterComNum - 1] != NULL)
  {
    // Set RPC baud-rate
    CQBaseComPort *ComPort = m_ProtocolEngines[ParamsMgr->TesterComNum - 1]->GetComPort();

    ComPort->Init(ComPort->GetComNum(),ParamsMgr->TesterBaudRate);

    // RPC server and client objects
    m_RPCServer = new CRPCServer(m_ProtocolEngines[ParamsMgr->TesterComNum - 1]);
    m_RPCClient = new CRPCClient(m_ProtocolEngines[ParamsMgr->TesterComNum - 1]);
  }
}

QString CQ2RTApplication::GetLastLogFileName()
{
	QString LastLogFileName = "";	
	
	// Make a list off all the log files in this directory
	TLogFileList LogFiles;

	TQFileSearchRec SearchRec;
	QString PathStr = AppFilePath.Value() + "Log\\" + "*.log";
	
	LogFiles.clear();

	// Enumerate log directory
	if(QFindFirst(PathStr,faAnyFile,SearchRec))
	{
		do
		{
			TLogFile LogFile;
			LogFile.FileName = SearchRec.Name.c_str();

			int Day,Month,Year,Hour,Minute;			
			
			if (sscanf(LogFile.FileName.c_str(), "%d-%d-%d-%d-%d", &Day, &Month, &Year, &Hour, &Minute) == 5)
			{
				struct tm LogTime;

				if (Year < 100)
					Year += 100;

				LogTime.tm_year = Year;
				LogTime.tm_mon  = Month - 1;
				LogTime.tm_mday = Day;
				LogTime.tm_hour = Hour;
				LogTime.tm_min  = Minute;
				LogTime.tm_sec  = 0;
				LogTime.tm_isdst = -1;

				LogFile.FileDateTime = mktime(&LogTime);

				if(LogFile.FileDateTime != -1)
					LogFiles.push_back(LogFile);				
			}
		} while(QFindNext(SearchRec));
	}

	QFindClose(SearchRec);

	// Sort the list order from oldest log file to newest
	std::sort(LogFiles.begin(),LogFiles.end());

	if (! LogFiles.empty())
	{
		TLogFile LogFile;
		
		LogFile          = LogFiles.back();
		LastLogFileName  = LogFile.FileName;	
	}
		
	return LastLogFileName;
}

// Initialize the log file
void CQ2RTApplication::InitLogFile(void)
{
  CQLog::SetMaxNumberOfLogFiles(CAppParams::Instance()->MaxLogFilesNumber);
  // Force Directory creation
  ForceDirectories((AppFilePath.Value() + "Log").c_str());
  CQLog::Init(MAX_LOG_FILE_SIZE,LAST_LOG_TAG,AppFilePath.Value() + "Log\\",
              AppFilePath.Value() + "Log\\" + GetLastLogFileName(),LogFileChangeEventHandler);

  CQEncryptedLog::Init(2*MAX_LOG_FILE_SIZE,LAST_LOG_TAG,AppFilePath.Value() + "Configs\\",
              AppFilePath.Value() + "Configs\\follow - up log.txt",LogFileChangeEventHandler);

  CQEncryptedLog::EnableEchoToMonitor(false);
  CQEncryptedLog::EnableDisableTag(LOG_TAG_GENERAL,true);
  CQEncryptedLog::EnableDisableTag(LOG_TAG_HASP,true);
  
  CQLog::WriteUnformatted("---------------------------------------------------------");
  CQEncryptedLog::Write(LOG_TAG_GENERAL,"---------------------------------------------------------");

#if defined(OS_WINDOWS) && !defined(EDEN_CL)
  QString HelloMsg = (QString)APPLICATION_NAME + " is up (version: " + QGetAppVersionStr() + " Build: " +
                      GetVersionDate() + " " + GetVersionTime() + ")";
#else
  QString HelloMsg = "1 - Eden is up (version: " + QGetAppVersionStr() + " Build: " + __DATE__ + " " + __TIME__ ")";
#endif

  CQLog::EnableDisableTag(LOG_TAG_GENERAL,true);
  CQLog::Write(LOG_TAG_GENERAL,HelloMsg.c_str());
  CQEncryptedLog::Write(LOG_TAG_GENERAL,HelloMsg.c_str());

  CQLog::WriteUnformatted("Tags dictionary:");
  CQLog::WriteUnformatted(QFormatStr("<%d>",LOG_TAG_GENERAL) + " - General");
  CQLog::WriteUnformatted(QFormatStr("<%d>",LOG_TAG_PROCESS) + " - Layer process");  
  CQLog::WriteUnformatted(QFormatStr("<%d>",LOG_TAG_HOST_COMM) + " - Host communication");
  CQLog::WriteUnformatted(QFormatStr("<%d>",LOG_TAG_PRINT) + " - Printing");
  CQLog::WriteUnformatted(QFormatStr("<%d>",LOG_TAG_MOTORS) + " - Motors");
  CQLog::WriteUnformatted(QFormatStr("<%d>",LOG_TAG_HEAD_HEATERS) + " - Head heating");
  CQLog::WriteUnformatted(QFormatStr("<%d>",LOG_TAG_TRAY_HEATERS) + " - Tray heating");
  CQLog::WriteUnformatted(QFormatStr("<%d>",LOG_TAG_UV_LAMPS) + " - UV lamps");
  CQLog::WriteUnformatted(QFormatStr("<%d>",LOG_TAG_EOL) + " - End-of-liquid");
  CQLog::WriteUnformatted(QFormatStr("<%d>",LOG_TAG_HEAD_FILLING) + " - Head filling");
  CQLog::WriteUnformatted(QFormatStr("<%d>",LOG_TAG_HEAD_VACUUM) + " - Head vacuum");
  CQLog::WriteUnformatted(QFormatStr("<%d>",LOG_TAG_AMBIENT) + " - Ambient temperature");
  CQLog::WriteUnformatted(QFormatStr("<%d>",LOG_TAG_POWER) + " - Power");
  CQLog::WriteUnformatted(QFormatStr("<%d>",LOG_TAG_HEAD_FILLING_THERMISTORS) + " - Head filling thermistor readings");
  CQLog::WriteUnformatted(QFormatStr("<%d>",LOG_TAG_RFID) + " - RFID");
  CQLog::WriteUnformatted(QFormatStr("<%d>",LOG_TAG_HASP) + " - HASP");
  CQLog::WriteUnformatted(QFormatStr("<%d>",LOG_TAG_ROLLER_SUCTION_SYSTEM) + " - Roller Suction System");
//OBJET_MACHINE feature 
  CQLog::WriteUnformatted(QFormatStr("<%d>",LOG_TAG_OBJET_MACHINE) + " - Objet machine log");
  CQLog::WriteUnformatted(QFormatStr("<%d>",LOG_TAG_CURRENT_VACUUM) + " - Current vacuum values");
  CQLog::WriteUnformatted("---------------------------------------------------------");

  IsLogFileInit = true;
}

// De-Initialize the log file
void CQ2RTApplication::DeInitLogFile(void)
{
  if( IsLogFileInit )
  {

    // Band messages from going to the monitor
    CQLog::EnableEchoToMonitor(false);

    CQLog::EnableDisableTag(LOG_TAG_GENERAL,true);
    CQLog::Write(LOG_TAG_GENERAL,(QString)APPLICATION_NAME +" is down");
    CQLog::WriteUnformatted("---------------------------------------------------------");
    CQLog::DeInit();
    CQEncryptedLog::Write(LOG_TAG_GENERAL,(QString)APPLICATION_NAME + " is down");
    CQEncryptedLog::Write(LOG_TAG_GENERAL,"---------------------------------------------------------");
    CQEncryptedLog::DeInit();

    IsLogFileInit = false;
  }
}

// Apply "pushed" parameters changes
void CQ2RTApplication::ApplyParametersChange(void)
{
  CAppParams *ParamsMgr = CAppParams::Instance();

  CQLog::EnableEchoToMonitor(ParamsMgr->EchoToMonitor);
  CQLog::SetMaxNumberOfLogFiles(ParamsMgr->MaxLogFilesNumber);

  CQLog::EnableDisableTag(LOG_TAG_GENERAL,ParamsMgr->GeneralLog);
  CQLog::EnableDisableTag(LOG_TAG_PROCESS,ParamsMgr->ProcessLog);
  CQLog::EnableDisableTag(LOG_TAG_HOST_COMM,ParamsMgr->HostCommLog);
  CQLog::EnableDisableTag(LOG_TAG_PRINT,ParamsMgr->PrintLog);
  CQLog::EnableDisableTag(LOG_TAG_MOTORS,ParamsMgr->MotorsLog);
  CQLog::EnableDisableTag(LOG_TAG_HEAD_HEATERS,ParamsMgr->HeadHeatersLog);
  CQLog::EnableDisableTag(LOG_TAG_TRAY_HEATERS,ParamsMgr->TrayHeaterLog);
  CQLog::EnableDisableTag(LOG_TAG_UV_LAMPS,ParamsMgr->UVLampsLog);
  CQLog::EnableDisableTag(LOG_TAG_EOL,ParamsMgr->EOLLog);
  CQLog::EnableDisableTag(LOG_TAG_HEAD_FILLING,ParamsMgr->HeadFillingLog);
  CQLog::EnableDisableTag(LOG_TAG_HEAD_VACUUM,ParamsMgr->VacuumLog);
  CQLog::EnableDisableTag(LOG_TAG_AMBIENT,ParamsMgr->AmbientLog);
  CQLog::EnableDisableTag(LOG_TAG_POWER,ParamsMgr->PowerLog);
  CQLog::EnableDisableTag(LOG_TAG_HEAD_FILLING_THERMISTORS,ParamsMgr->HeadFillingThermistorLog);
  CQLog::EnableDisableTag(LOG_TAG_RFID,ParamsMgr->RFIDLog);
  CQLog::EnableDisableTag(LOG_TAG_HASP,ParamsMgr->HASPLog);
  CQLog::EnableDisableTag(LOG_TAG_ROLLER_SUCTION_SYSTEM,ParamsMgr->RollerSuctionSystemLog);
  CQLog::EnableDisableTag(LOG_TAG_CURRENT_VACUUM,ParamsMgr->CurrentVacuumValuesLog);
//OBJET_MACHINE feature
  CQLog::EnableDisableTag(LOG_TAG_OBJET_MACHINE,ParamsMgr->ObjetMachineLog);
  
  // change the warning of the maintenance counter of the PM according to the value of PMIntervalHours
  int PMInterval = ParamsMgr->PMIntervalHours;
  int PMWarning = ParamsMgr->NextPMTypeIsSmall ? PMInterval : ( PMInterval * 2 );
  CMaintenanceCounters* MC = CMaintenanceCounters::GetInstance();
  // if the PM warning (warning for service call) is different than the parameter, then it means that the parameter has changed (externally, perhaps due to upgrade) and the warning has to be updated accordingly
  if(MC) {
	if( MC->GetWarningTimeInSeconds(SERVICE_COUNTER_ID) / 3600 != PMWarning ) {
	  MC->SetWarningTimeInSecs(SERVICE_COUNTER_ID, PMWarning * 3600);
	  MC->SetUserWarning(SERVICE_COUNTER_ID, PMWarning * 3600);
	  MC->SaveToFile();
	}
  }
}

void CQ2RTApplication::PrintLogMessage(int MessageID,TGenericCockie Cockie)
{
   CEdenProtocolClient *InstancePtr = reinterpret_cast<CEdenProtocolClient *>(Cockie);
   QString PrintMessage = InstancePtr->Name() + " :Retry Message: " +
                          InstancePtr->ConvertMessageToString(MessageID) +
                          " (0x" + QIntToHex((const int)MessageID,2) + ")";
   CQLog::Write(LOG_TAG_GENERAL,PrintMessage.c_str());
}

TQErrCode CQ2RTApplication::ImportConfiguration(const QString FileName)
{
  CQParamsFileStream *FileStream = NULL;
  TQErrCode ErrCode = Q_NO_ERROR;

  // Prepare a parameters file stream and import the stream
  try
  {
    FileStream = new CQParamsFileStream(FileName);
    CAppParams::Instance()->Import(FileStream,false);

  } catch(EQException& Err)
    {
      ErrCode = Q2RT_PARAMETERS_IMPORT_ERR;
    }

  delete FileStream;
  return ErrCode;
}

void CQ2RTApplication::SetMaintenanceCountersDefaultWarnings(void)
{
  CMaintenanceCounters *CountersInstance = CMaintenanceCounters::GetInstance();
  for(int i = 0; i < NUM_OF_MAINTENANCE_COUNTERS; i++)
    CountersInstance->SetDefaultWarningTimeInHours(ServiceCounterIDLookup[i],ServiceWarningTimeLookup[i],ServiceNeedToInformLookup[i]);
}

// Utility function to flexibly specify a file name with the following options:
// 1) Full path.
// 2) Relative to application root dir
QString CQ2RTApplication::FlexibleFileName(const QString FileName)
{
  // Ignore empty file names
  if(FileName == "")
    return "";

  // Test if the file has a full path or not
  QString FilePath = QExtractFilePath(FileName);

  // If the file name contain a driver path, return it as is
  if(QExtractFileDrive(FilePath) != "")
    return FileName;

  return (AppFilePath.Value() + FileName);
}

void CQ2RTApplication::FIFOUnderrunErrorCallback(TGenericCockie Cockie)
{
  CQ2RTApplication *InstancePtr = reinterpret_cast<CQ2RTApplication *>(Cockie);

  InstancePtr->m_MachineManager->GetMachineSequencer()->GetPrintControlInstance()->Stop();
  CQLog::Write(LOG_TAG_PRINT,"FIFO underrun error");
}

// Refresh the BIt system registration (only built in tests not scriptable tests)
void CQ2RTApplication::RefreshBIT(void)
{
  m_BITManager->Clear();
  AppBIT::Register(m_BITManager);
  m_BITManager->RefreshBITSets();
}

TQErrCode CQ2RTApplication::LogThreadsProperties(void)
{
  // Get a pointer to the objects roster (we will need it later)
  CQObjectsRoster *Roster = CQObjectsRoster::Instance();

  // Holds the ThreadID accepted from GetThreadID() method.
  QString RetValue = "";

  // If the roster is NULL, no objects are registered
  if(Roster == NULL)
    QMonitor.Print("No objects found.");
  else
  {
    // Get a pointer to the list of the components
    TQComponentList *CompList = Roster->GetComponentList();

    // Print the name of all the components in the list
    for(TQComponentList::iterator i = CompList->begin(); i != CompList->end(); ++i)
    {
      // Search for the method
      CQMethodObject *Method = (*i)->FindMethod("GetThreadID");

      // If found
      if(Method != NULL)
      {
        try
        {
          // Invoke the method
          RetValue = Method->Invoke(NULL,0);
        }
        catch(EQMethod){}
        catch(EQException& Err){}
        CQLog::Write(LOG_TAG_GENERAL,"Thread: " + (*i)->Name() + " ID: " + RetValue);
      }
    }
  }

  return Q_NO_ERROR;
}


bool CQ2RTApplication::IsPciCardInitialized(void)
{
  return m_IsPciCardInitialized;
}

CMachineManager *CQ2RTApplication::GetMachineManager(void)
{
  return m_MachineManager;
}

// Return a pointer to the layer process instance
CLayerProcess *CQ2RTApplication::GetLayerProcess(void)
{
  return m_LayerProcess;
}

// Return a pointer to the RPC server object
CRPCClient *CQ2RTApplication::GetRPCClient(void)
{
  return m_RPCClient;
}

// Return a pointer to the host communication object
CHostComm *CQ2RTApplication::GetHostComm(void)
{
  return m_HostComm;
}

// Return a pointer to the internal tester object
CTesterBase *CQ2RTApplication::GetTester(void)
{
  return m_Tester;
}

CMiniSequencer *CQ2RTApplication::GetMiniSequencer(void)
{
  return m_MiniSequencer;
}

CBITManager *CQ2RTApplication::GetBITManager(void)
{
  return m_BITManager;
}

void CQ2RTApplication::ExecFileSafe(QString FileName)
{
  bool ClearDictionary = false;
  QPythonIntegratorDM->ExecFileSafe(FileName.c_str(), ClearDictionary);
}

void CQ2RTApplication::SetQATestsAllowed(bool allowed)
{
    m_QATestsAllowed = allowed;
}
void CQ2RTApplication::SetSolubleSupportAllowed(bool allowed)
{
	m_SolubleSupportAllowed = allowed;
}


bool CQ2RTApplication::GetQATestsAllowed()
{
    return m_QATestsAllowed;
}
bool CQ2RTApplication::GetSolubleSupportAllowed()
{
  #ifndef EMULATION_ONLY
	return m_SolubleSupportAllowed;
  #else
	return true;
	#endif
}

void CQ2RTApplication::RegisterToPythonExceptionThrowers(DWORD ThreadID)
{
   QPythonIntegratorDM->RegisterExceptionThrowingThread(ThreadID);
}

//bug 5719
void CQ2RTApplication::ExecStringSafe(TStrings *Strings,bool ClearDictionary)
{
  QPythonIntegratorDM->ExecStringsSafe(Strings, ClearDictionary);
}


bool CQ2RTApplication::SetTimer(int interval)
{
  CQLog::Write(LOG_TAG_GENERAL,"Set Timer: %d", interval);
  m_timeoutTimer->Enabled  = false;
  m_timeoutTimer->OnTimer  = TimeoutTimerEvent;
  m_timeoutTimer->Interval = interval;
  return true;
}

bool CQ2RTApplication::EnableTimer(bool enable)
{
  CQLog::Write(LOG_TAG_GENERAL,"Timer enable: %d", enable);
  m_timeoutTimer->Enabled  = enable;
  return true;
}

bool CQ2RTApplication::SetTimerInterval(int interval)
{
  CQLog::Write(LOG_TAG_GENERAL,"Timer interval: %d", interval);
  m_timeoutTimer->Interval  = interval;
  return true;
}

//OBJET_MACHINE
void CQ2RTApplication::SetMachineSinceDateParam()
{
  CAppParams *ParamsMgr = CAppParams::Instance();
  if(strcmp(ParamsMgr->MachineSinceDate.c_str(),"0")==0) //first time
  {
	// Prepare the date/time string
	char DateTimeStr[MAX_DATE_SIZE];
	time_t CurrentTime = time(NULL);
	struct tm *LocalTimeTM = localtime(&CurrentTime);
	strftime(DateTimeStr,MAX_DATE_SIZE,"%d/%m/%y",LocalTimeTM);
	ParamsMgr->MachineSinceDate = DateTimeStr;
	ParamsMgr->SaveSingleParameter(&ParamsMgr->MachineSinceDate);
  }
}



/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
//bug 5950
void __fastcall CQ2RTApplication::TimeoutTimerEvent(TObject *Sender)
{
   CBackEndInterface* BackEnd = CBackEndInterface::Instance();
   BackEnd->TurnUVLamps(false);
   m_timeoutTimer->Enabled = false;
   CQLog::Write(LOG_TAG_GENERAL,"User Entry response timeout!, Turning off UV lamps");
}

void CQ2RTApplication::LogFileChangeEventHandler(const QString& NewLogFileName,TGenericCockie /*Cockie*/)
{
}

/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
