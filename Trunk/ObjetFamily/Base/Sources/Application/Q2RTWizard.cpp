/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Q2RT Wizard class (CQ22RTWizard).                         *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 ********************************************************************/
#include "Q2RTWizard.h"
#include "BackEndInterface.h"
#include "Q2RTApplication.h"
#include "WizardPages.h"
#include "AutoWizardDlg.h"
#include "QTimer.h"
#include "ResinFlowUtils.h"
#include "WizardImages.h"
#include "QMonitor.h"
#include "CActionsHistoryTable.h"
#include "AddRemoveRFIDCounter.h"

#ifdef ALLOW_OPENING_MRW_FROM_HOST
// This timer is used to allow the host interrupting with the wizard and restart if if required.
// It checks whether their is a new MRW request is pending,
// and if it is, cancel click is emulated and then after the wizard
// exits, MarkWizardRunning(false) sends a message that triggers wizard restart.
TTimer* CQ2RTAutoWizard::WizardTimer = NULL;
#endif

QString YesNoAnswerStrings[NUMBER_OF_ANSWERS] = {"No","Yes"};

CQ2RTAutoWizardCancelledException::CQ2RTAutoWizardCancelledException() : EQException("",0) {}
EQHeadFillingException::EQHeadFillingException() : EQException("Head Filling is not allowed.",0) {}

void CQ2RTAutoWizard::AddServicePages(void)
{
   int PageCount = GetNumberOfPages();
   CWizardPage* pPage = GetPageById(PageCount - 1);
   pPage->SetAttributes(pPage->GetAttributes() | wpDonePage);
   CMessageWizardPage *cancelPage = new CMessageWizardPage(WIZARD_CANCELED_STR,CANCEL_PAGE_IMAGE_ID, wpPreviousDisabled | wpCancelPage | wpHelpNotVisible);
   AddPage(cancelPage);
   m_CancelPageIndex = PageCount;
}


bool CQ2RTAutoWizard::Start(void)
{
  if (m_isFirstStart)
  {
     m_isFirstStart = false;
     AddServicePages();
  }
  WriteToLogFile(LOG_TAG_GENERAL,"started");
  SetDefaultTimeout(m_ParamMgr->DefaultWizardPageTimeoutSec * 1000);
  m_CancelReason = "";
  CMessageWizardPage *Page = dynamic_cast<CMessageWizardPage *>(GetPageById(m_CancelPageIndex));
  Page->SubTitle = m_CancelReason;

  TMachineState MachineState = m_BackEnd->GetCurrentMachineState();
  if (MachineState == msStandby1 || MachineState == msStandby2)
      m_BackEnd->ExitStandbyState();

  // Continue only when we are in 'Idle' state
  while ((MachineState = m_BackEnd->GetCurrentMachineState()) != msIdle)
    YieldAndSleepWizardThread();

  m_EnterStandBy = false;

  // Don't let us enter into 'Stanby' state while performing wizard
  m_BackEnd->DisableStandByMode();
  m_BackEnd->ResetDriverState();

  if(m_IsHistoryFileEnabled)
    HistoryFile = new CHistoryFile(GetTitle());
  else
    HistoryFile = NULL;

  Application->BringToFront();
  return CAutoWizard::Start();
}


void CQ2RTAutoWizard::PageEnterWrapper(CWizardPage *WizardPage)
{
  if (!(WizardPage->GetAttributes() & wpDonePage))
     m_InsidePage = true;
  try
  {
     if(!(WizardPage->GetAttributes() & wpNoRefreshOnShow))
        WizardPage->Refresh();
     WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("PageEnter %d", WizardPage->GetPageNumber()));
     if(m_BackEnd->IsPrintingWizardRunning() && !(WizardPage->GetAttributes() & wpDonePage))
        m_BackEnd->CheckHaspPlugValidityDirectly(); // throws EHaspInterfaceException
     CAutoWizard::PageEnterWrapper(WizardPage);
     return;
  }
  catch( CQ2RTAutoWizardCancelledException& Exception )
  {
     return; //wizard was cancelled; returning
  }
  catch(EQException& Exception)
  {
     m_CancelReason = Exception.GetErrorMsg();
  }
  catch(Exception& VCLException)
  {
     m_CancelReason = VCLException.Message.c_str();
  }
  catch(...)
  {
     m_CancelReason = "Unexpected exception during the wizard";
  }
  CMessageWizardPage *Page = dynamic_cast<CMessageWizardPage *>(GetPageById(m_CancelPageIndex));
  Page->SubTitle = m_CancelReason;
  GotoPage(m_CancelPageIndex);
}

void CQ2RTAutoWizard::PageLeaveWrapper(CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason)
{
  try //__finally
  {
        WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("PageLeave %d", WizardPage->GetPageNumber()));
        CAutoWizard::PageLeaveWrapper(WizardPage,LeaveReason);
        m_InsidePage = false;
        return;
  }
  catch( CQ2RTAutoWizardCancelledException& Exception )
  {
     m_InsidePage = false;
     return; //wizard was cancelled; returning
  }
  catch(EQException& Exception)
  {
     m_CancelReason = Exception.GetErrorMsg();
  }
  catch(Exception& VCLException)
  {
     m_CancelReason = VCLException.Message.c_str();
  }
  catch(...)
  {
     m_CancelReason = "Unexpected exception during the wizard";
  }
  m_InsidePage = false;
  CMessageWizardPage *Page = dynamic_cast<CMessageWizardPage *>(GetPageById(m_CancelPageIndex));
  Page->SubTitle = m_CancelReason;
  SetNextPage(m_CancelPageIndex);
}

void CQ2RTAutoWizard::WaitForEndOfPrint()
{
  // Wait till the machine starts printing
  m_EnterStandBy = true;
  WaitForState(msPrePrint); //throws canceling exceptions
  TMachineState State;
  do
  {
     if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
     State = m_BackEnd->GetCurrentMachineState();
     YieldAndSleepWizardThread();
  }while( State != msStopped && State != msIdle && State != msStandby1 && State != msStandby2 );
  // Find out if the print was completed
  if(! m_BackEnd->IsPrintCompleted())
	throw EQException("Error during printing.");  
}


bool CQ2RTAutoWizard::Q2RTWizardSleep(int SleepTime)
{
  int CurrentPageIndex = GetCurrentPageIndex();
  unsigned EndTime = QGetTicks() + QSecondsToTicks(SleepTime);
  while(!IsCancelled() && (CurrentPageIndex == GetCurrentPageIndex()))
  {
     YieldAndSleepWizardThread();
     if (QGetTicks() > EndTime)
        return true;
  }
  return false;
}

void CQ2RTAutoWizard::Q2RTCleanUp(void)
{
  TMachineState State = m_BackEnd->GetCurrentMachineState();
  if (State == msPrinting || State == msPrePrint || State == msPurge || State == msTestPattern)
  {
     m_BackEnd->EnableErrorMessageDuringStop(false);
     m_BackEnd->StopPrint(Q2RT_EMBEDDED_RESPONSE);
     do
     {
        State = m_BackEnd->GetCurrentMachineState();
        YieldAndSleepWizardThread();
     }while (!( State == msIdle || State == msStandby1 ));
     m_BackEnd->EnableErrorMessageDuringStop(true);
  }

  // Continue only when we are in 'Idle' state
  while(m_BackEnd->GetCurrentMachineState() != msIdle) // todo -oNobody -cNone: Is there any guarantee it will ever stop ?
	YieldAndSleepWizardThread();

  Q_SAFE_DELETE(HistoryFile);	

#ifdef ALLOW_OPENING_MRW_FROM_HOST
  WizardTimer->Enabled  = false;
#endif
}

void CQ2RTAutoWizard::Q2PostRTCleanUp(void)
{
  m_BackEnd->EnableStandByMode();
  if (m_EnterStandBy)
  {
      m_BackEnd->TurnHeadHeaters(false);
      m_BackEnd->EnterStandbyState();
  }
//OBJET_MACHINE feature
  m_BackEnd->UpdateWeightAfterWizard();
  if(!IsCancelled())
	m_BackEnd->SetMachineActionsDate(m_Title);
}

QString CQ2RTAutoWizard::DefaultHelpFileName(void)
{
   QString FileName = Q2RTApplication->AppFilePath.Value() + "\\" + LOAD_QSTRING(IDS_HELP_DIR) + GetTitle() + "\\" + GetTitle() + ".chm";
   return FileName;
}

bool CQ2RTAutoWizard::Cancel(void)
{
  // In case we're cancelling from FEOP_CANCEL_WIZARD, m_CancelReason might be empty, because we don't have
  // access to it in AutoWizard. Taking the text from the SubTitle.
  if(m_CancelReason.compare("") == 0)
  {
	CMessageWizardPage *Page = dynamic_cast<CMessageWizardPage *>(GetPageById(m_CancelPageIndex));
	m_CancelReason = Page->SubTitle;
  }

  WriteToLogFile(LOG_TAG_GENERAL,"Cancel event: " + m_CancelReason);
  while(m_InsidePage)
    YieldAndSleepWizardThread();
  
  Q2RTCleanUp();
  bool ret = CAutoWizard::Cancel();

  Q2PostRTCleanUp();
  WriteToLogFile(LOG_TAG_GENERAL,"was cancelled");
  return ret;
}

bool CQ2RTAutoWizard::Close(void)
{
  WriteToLogFile(LOG_TAG_GENERAL,"closing wizard window");
  bool ret = CAutoWizard::Close();
  WriteToLogFile(LOG_TAG_GENERAL,"window closed");
  return ret;
}

bool CQ2RTAutoWizard::End(void)
{
  bool ret = false;
  try
  {
  EnableDisableNext(false);
  WriteToLogFile(LOG_TAG_GENERAL,"End event");
  m_InsidePage = false;
  //Busy waiting for pageLeave to finish
  ret = CAutoWizard::End(false);
  Q2RTCleanUp();
  Q2PostRTCleanUp();
  WriteToLogFile(LOG_TAG_GENERAL,"ended");
  }
  __finally
  {
  EnableDisableNext(true);
  return ret;
  }
}

CQ2RTAutoWizard::CQ2RTAutoWizard(int           Title,
                                 bool          HelpVisible,
                                 int           DefaultImageID,
                                 bool          UserButton1Visible,
                                 bool          UserButton2Visible,
                                 QString       UserButton1Caption,
                                 QString       UserButton2Caption) :
CAutoWizard(WIZARD_TITLE(Title),
            HelpVisible,
            DefaultImageID,
            UserButton1Visible,
            UserButton2Visible,
            UserButton1Caption,
            UserButton2Caption),
            HistoryFile(NULL),
m_WizardForm(NULL),
m_EnterStandBy(false),
m_InsidePage(false),
m_isFirstStart(true),
m_CancelPageIndex(-1),
m_IsHistoryFileEnabled(false),
m_EchoLogToHistory(false)
{
#ifdef ALLOW_OPENING_MRW_FROM_HOST
  if (NULL == WizardTimer)
  {
     WizardTimer = new TTimer(NULL);
     WizardTimer->Enabled  = false;
  }
#endif
  m_BackEnd  = CBackEndInterface::Instance();
  m_ParamMgr = CAppParams::Instance();
}

void CQ2RTAutoWizard::EnableHistoryFile()
{
	m_IsHistoryFileEnabled = true;
}

void CQ2RTAutoWizard::EchoLogToHistory()
{
	m_EchoLogToHistory = true;
}

void CQ2RTAutoWizard::SetWizardForm(TWizardForm* WizardForm)
{
   m_WizardForm = WizardForm;
}

void CQ2RTAutoWizard::WriteToLogFile(TLogFileTag TagID, QString Str)
{
  CQLog::Write(TagID,GetTitle() + ": " + Str);

  if(m_IsHistoryFileEnabled && HistoryFile && m_EchoLogToHistory)
  	HistoryFile->Append(Str);
}

void CQ2RTAutoWizard::WriteToLogFile(TLogFileTag TagID, const char* FormatStr, ...)
{
  va_list arglist;
  va_start(arglist,FormatStr);  
  WriteToLogFile(TagID, QFormatStr(FormatStr, arglist));
  va_end(arglist);  
}

void CQ2RTAutoWizard::EnableAxeAndHome(TMotorAxis Axis, bool Blocking, bool ForceHome)
{
   m_BackEnd->EnableMotor(true, Axis);
   m_BackEnd->SetMotorParameters(Axis);

   if (ForceHome || !m_BackEnd->IsMotorHomeDone(Axis))
	{

	  m_BackEnd->GoToMotorHomePosition(Axis,Blocking);

	}

   YieldWizardThread();
}

int CQ2RTAutoWizard::MoveToWiperPosition()
{
   int err  =  m_BackEnd->GotoHeadInspectionPositionUnderWiperCalibration();
   if(  err !=  Q_NO_ERROR  )    // Need to go to head inspection position.
   {
           CQLog::Write(LOG_TAG_GENERAL, "Go to head inspection position during wiper clibration was failed!!!!");
   }
   else
   {
             CQLog::Write(LOG_TAG_GENERAL, "Go to head inspection position during wiper clibration succeed");
   }

   return err;
}


void CQ2RTAutoWizard::XY_AxisGoHomePosition_Z_AxisGoHeadInspectionPosition(T_AxesTable a_AxesTable, bool Blocking, bool ForceHome, bool bIsAModelOnTheTray)
{
  T_AxesTable AxesTable = {true, true, true, true};
  m_BackEnd->EnableDoor(true);

  if( !bIsAModelOnTheTray) //if the tray is empy, we move the tray to
  {
      EnableAxeAndHome(AXIS_Z, Blocking, ForceHome);
      m_BackEnd->EnableMotor(true, AXIS_Z);
      m_BackEnd->SetMotorParameters(AXIS_Z);

      MoveToWiperPosition();
  }
  if (a_AxesTable)
	 for (int Axis = 0; Axis < MAX_AXIS; Axis++)
		 AxesTable[Axis] = a_AxesTable[Axis];

  if (AxesTable[AXIS_T])
	 EnableAxeAndHome(AXIS_T, Blocking, ForceHome);

  if (AxesTable[AXIS_Y])
	 EnableAxeAndHome(AXIS_Y, Blocking, ForceHome);

  if (AxesTable[AXIS_X])
	 EnableAxeAndHome(AXIS_X, Blocking, ForceHome);


}

void CQ2RTAutoWizard::EnableAllAxesAndHome(T_AxesTable a_AxesTable, bool Blocking, bool ForceHome)
{
  T_AxesTable AxesTable = {true, true, true, true};
  m_BackEnd->EnableDoor(true);

  if (a_AxesTable)
	 for (int Axis = 0; Axis < MAX_AXIS; Axis++)
		 AxesTable[Axis] = a_AxesTable[Axis];

  if (AxesTable[AXIS_T])
	 EnableAxeAndHome(AXIS_T, Blocking, ForceHome);

  if (AxesTable[AXIS_Y])
	 EnableAxeAndHome(AXIS_Y, Blocking, ForceHome);

  if (AxesTable[AXIS_X])
	 EnableAxeAndHome(AXIS_X, Blocking, ForceHome);

  if (AxesTable[AXIS_Z])
	 EnableAxeAndHome(AXIS_Z, Blocking, ForceHome);
}

unsigned CQ2RTAutoWizard::StartAsyncHeatingCycle(const int NumberOfHeadHeaters, bool IncludePreHeater, int* HeadsTemperatures, const bool& bKeepHeatingEvenOpenOrShortCircuit )
{
  int CurrentTempParam[TOTAL_NUMBER_OF_HEATERS];
  m_EnterStandBy = true;
  if (!HeadsTemperatures)
  {
     m_BackEnd->GetCurrentHeadHeatersTempParameters(CurrentTempParam, IncludePreHeater);
     HeadsTemperatures = CurrentTempParam;
  }
  m_BackEnd->TurnHeadHeaters(true, HeadsTemperatures, TOTAL_NUMBER_OF_HEATERS, bKeepHeatingEvenOpenOrShortCircuit);
  WriteToLogFile(LOG_TAG_GENERAL,"Heating heads.");

  return QGetTicks();
}

void CQ2RTAutoWizard::EndAsyncHeatingCycle(unsigned StartTime, unsigned TimeOut)
{
  unsigned CurrTime  = QGetTicks();
  unsigned Timeout   = QSecondsToTicks(TimeOut);

  // While Wizard is not canceled and
  while(CurrTime < (StartTime + Timeout))
  {
    if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
    if (m_BackEnd->AreHeadsTemperaturesOk())
       return;
    CurrTime = QGetTicks();
    YieldAndSleepWizardThread();
  }
  throw EQException(TIMEOUT_REASON(IDS_HEADS_HEATING));
}

void CQ2RTAutoWizard::HeadsHeatingCycle(const int TimeOut,const int NumberOfHeadHeaters, bool IncludePreHeater, int* HeadsTemperatures, const bool& bKeepHeatingEvenOpenOrShortCircuit)
{
  unsigned StartTime = StartAsyncHeatingCycle(NumberOfHeadHeaters, IncludePreHeater, HeadsTemperatures, bKeepHeatingEvenOpenOrShortCircuit);
  EndAsyncHeatingCycle(StartTime, ((TimeOut == DEFAULT_TIMEOUT) ? m_ParamMgr->HeadsHeatingTimeoutSec : TimeOut));
}

void CQ2RTAutoWizard::HeadsFillingCycle(unsigned int FillingTimeout)
{
   if (!m_BackEnd->IsHeadFillingAllowed())
      throw EQHeadFillingException();

   WriteToLogFile(LOG_TAG_GENERAL,"Starting filling cycle.");
   unsigned Timeout   = /*m_ParamMgr->*/FillingTimeout ;
   Timeout = QSecondsToTicks((Timeout>0) ? Timeout : 0);

   unsigned StartTime = QGetTicks();
   unsigned CurrTime  = QGetTicks();

   while(!m_BackEnd->AreHeadsFilled() && (CurrTime < (StartTime + Timeout)))
   {
      YieldAndSleepWizardThread();
      if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
	  if (!m_BackEnd->IsHeadFillingAllowed()) throw EQHeadFillingException();
	  CurrTime = QGetTicks();
   }
   YieldWizardThread();

   if (!m_BackEnd->AreHeadsFilled())
   {
       WriteToLogFile(LOG_TAG_GENERAL,"Error: Time out during Heads Filling Cycle, Current time is:%d start time is:%d, and time out is: %d", QIntToStr(CurrTime), QIntToStr(StartTime),QIntToStr(Timeout) );
      throw EQException(TIMEOUT_REASON(IDS_HEAD_FILLING));
   }
   WriteToLogFile(LOG_TAG_GENERAL,"Heads filling cycle was completed successfully");
}

void CQ2RTAutoWizard::HeadsDrainingCycle(CWizardPage *WizardPage, unsigned int DrainingTime,
                                         bool CheckingEmpty,bool wipe, bool bHomeT)
{
  WriteToLogFile(LOG_TAG_GENERAL,"Starting draining cycle.");
    if(bHomeT)
  {
      m_BackEnd->GotoPurgePosition(true, false);
  }
  else
  {
//     T_AxesTable AxesTable1 = {false, false, false, false};      //disable T home
     m_BackEnd->GotoPurgePosition(true, true);
  }
  ResinFlowUtils::AirValve av(this); // in case of exceptions, closes air valve when gets out of scope
  if (CheckingEmpty)
     av.holdOpenUntilNoFullChambers(QSecondsToTicks(DrainingTime));
  else
     av.holdOpen(QSecondsToTicks(DrainingTime));
  if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
  YieldAndSleepWizardThread();
  WriteToLogFile(LOG_TAG_GENERAL,"Delay MotorPostPurgeTimeSec between AirValve OFF and Wiping.");
  Q2RTWizardSleep(CAppParams::Instance()->MotorPostPurgeTimeSec);
  if(wipe == true)
  {
	m_BackEnd->WipeHeads(false);
	YieldWizardThread();
	m_BackEnd->WipeHeads(false);
  }
  WriteToLogFile(LOG_TAG_GENERAL,"Draining cycle completed.");
}

TMachineState CQ2RTAutoWizard::WaitForStateToDiff(TMachineState MachineState)
{
  TMachineState State;
  do
  {
      State = m_BackEnd->GetCurrentMachineState();
      YieldAndSleepWizardThread();
      if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
  }while(State == MachineState);
  return State;
}

void CQ2RTAutoWizard::WaitForState(TMachineState MachineState)
{
  TMachineState State;
  do 
  {
      State = m_BackEnd->GetCurrentMachineState();
      YieldAndSleepWizardThread();
      if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
  }while(State != MachineState);
}

CQ2RTPythonAutoWizard::CQ2RTPythonAutoWizard(void)
{
   m_BackEnd = NULL;
}

CQ2RTPythonAutoWizard::CQ2RTPythonAutoWizard(const QString PythonFileName) :
CPythonAutoWizard(PythonFileName)
{
  m_BackEnd  = CBackEndInterface::Instance();
}

bool CQ2RTPythonAutoWizard::Start(void)
{
  bool ret;
  SetDefaultTimeout(CAppParams::Instance()->DefaultWizardPageTimeoutSec * 1000);

  if (m_BackEnd)
  {
      m_BackEnd->ResetDriverState();
  }

  CQLog::Write(LOG_TAG_GENERAL,GetTitle() + " Start event");
  ret = CPythonAutoWizard::Start();
  CQLog::Write(LOG_TAG_GENERAL,GetTitle() + " started");
  return ret;
}

bool CQ2RTPythonAutoWizard::Cancel(void)
{
  CQLog::Write(LOG_TAG_GENERAL,GetTitle() + " Cancel event");
  bool ret = CPythonAutoWizard::Cancel();
  CQLog::Write(LOG_TAG_GENERAL,GetTitle() + " was cancelled");
  return ret;
}

bool CQ2RTPythonAutoWizard::Close(void)
{
  CQLog::Write(LOG_TAG_GENERAL,GetTitle() + " closing wizard window");
  bool ret = CPythonAutoWizard::Close();
  CQLog::Write(LOG_TAG_GENERAL,GetTitle() + " window closed");
  return ret;
}

bool CQ2RTPythonAutoWizard::End(void)
{
  CQLog::Write(LOG_TAG_GENERAL,GetTitle() + " End event");
  bool ret = CPythonAutoWizard::End();
//OBJET_MACHINE feature
  m_BackEnd->SetMachineActionsDate(m_Title);
  CQLog::Write(LOG_TAG_GENERAL,GetTitle() + " ended");
  return ret;
}

COneCheckBoxWizardPage::COneCheckBoxWizardPage(CAutoWizard* Wizard, int SubTitleStrRes, int CheckStrRes,int ImageID,TWizardPageAttributes PageAttributes):
CWizardPage(Wizard->GetTitle(),ImageID,PageAttributes | wpNextWhenSelected | wpClearOnShow),
CCheckBoxWizardPage(Wizard->GetTitle(),ImageID,PageAttributes | wpNextWhenSelected | wpClearOnShow)
{
  SubTitle = LOAD_STRING(SubTitleStrRes);
  Strings.Add(LOAD_STRING(CheckStrRes));
}

COneCheckBoxWizardPage::COneCheckBoxWizardPage(CAutoWizard* Wizard, QString SubTitleStr, QString CheckStr,int ImageID,TWizardPageAttributes PageAttributes):
CWizardPage(Wizard->GetTitle(),ImageID,PageAttributes | wpNextWhenSelected | wpClearOnShow),
CCheckBoxWizardPage(Wizard->GetTitle(),ImageID,PageAttributes | wpNextWhenSelected | wpClearOnShow)
{
  SubTitle = SubTitleStr;
  Strings.Add(CheckStr);
}

COneCheckBoxWizardPage::~COneCheckBoxWizardPage(){}

CVerifyCleanTrayPage::CVerifyCleanTrayPage(CAutoWizard* Wizard, int ImageID,TWizardPageAttributes PageAttributes) :
CWizardPage(LOAD_STRING(IDS_SYSTEM_CHECKLIST) /*originally: "Inspect the build tray"*/,ImageID,PageAttributes | wpNextWhenSelected | wpClearOnShow | wpHelpNotVisible),
COneCheckBoxWizardPage(Wizard,IDS_VERIFY_TRAY_IS_EMPTY,IDS_TRAY_IS_EMPTY)
{}


CVerifyCleanTrayPage::~CVerifyCleanTrayPage(){}


//==============================================================================
CCloseDoorPage::CCloseDoorPage(CAutoWizard* Wizard, int ImageID,TWizardPageAttributes PageAttributes) :
CWizardPage(LOAD_STRING(IDS_SYSTEM_CHECKLIST) /*originally: Wizard->GetTitle()*/,ImageID,PageAttributes | wpNextWhenSelected | wpClearOnShow | wpNoRefreshOnShow | wpHelpNotVisible),
COneCheckBoxWizardPage(Wizard,CBackEndInterface::Instance()->GetDoorStatusMessage().c_str(),CBackEndInterface::Instance()->GetDoorMessage().c_str())
{
  m_Wizard = Wizard;
}


bool CCloseDoorPage::PreEnter(void)
{
  CBackEndInterface* BackEnd = CBackEndInterface::Instance();
  if (BackEnd->CheckIfDoorIsClosed() != Q_NO_ERROR)
      return true;

  BackEnd->LockDoor(true);
  while(!BackEnd->IsDoorLocked())
      CAutoWizard::YieldAndSleepWizardThread();
  return false;
}

bool CCloseDoorPage::Enter(void)
{
  if (PreEnter())
     m_Wizard->SetNextPage(this);
  else
     return COneCheckBoxWizardPage::Enter();
  return true;
}
//==============================================================================


//==============================================================================
CLiquidsShortagePage::CLiquidsShortagePage(CAutoWizard* Wizard, int ImageID,TWizardPageAttributes PageAttributes) :
CWizardPage(LOAD_STRING(IDS_LIQUID_LEVEL),ImageID,PageAttributes | wpNextWhenSelected | wpClearOnShow | wpNoRefreshOnShow),
CCheckBoxWizardPage(LOAD_STRING(IDS_LIQUID_LEVEL),ImageID,PageAttributes | wpNextWhenSelected | wpClearOnShow)
{
  SubTitle = LOAD_STRING(IDS_CONFIRM);
  m_Wizard = Wizard;
}

bool CLiquidsShortagePage::PreEnter(void)
{
  CBackEndInterface* BackEnd = CBackEndInterface::Instance();
  CAppParams* ParamMgr  = CAppParams::Instance();
  
  Strings.Clear();
  Reset();

  bool Shortage  = false;
  for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
  {
  	if(ParamMgr->HSW_DM3_Enable == false && i ==TYPE_CHAMBER_MODEL3) /*no need to check M3*/
	 continue;
        // Check if there is enough ilquid for the wizard
        if ((BackEnd->GetTotalWeight(i) < MIN_LIQUID_WEIGHT) ||
            !(BackEnd->IsActiveLiquidTankInserted(i)))
        {
           if (Strings.FindSubStr(ParamMgr->TypesArrayPerChamber[i]) == -1)
              Strings.Add(INSERTED_RESIN_CATRIDGES_STR(ParamMgr->TypesArrayPerChamber[i].c_str()) );
           Shortage = true;
        }
  }
  return Shortage;
}

bool CLiquidsShortagePage::Enter(void)
{
  if (PreEnter())
     m_Wizard->SetNextPage(m_Wizard->GetPreviousPage()); // This page is always a result of an exception during another page execution. So we should always return to that page. 
  else
     return CCheckBoxWizardPage::Enter();
  return true;
}

//==============================================================================
#define NUMBER_OF_POINTS 3
CSuspensionPointsStatusPage::CSuspensionPointsStatusPage(const QString Title, int ImageID, TWizardPageAttributes PageAttributes):
CWizardPage(Title,ImageID,PageAttributes),
CStatusWizardPage(Title,ImageID,PageAttributes)
{
   PointsTimer           = new TTimer(NULL);
   PointsTimer->Enabled  = false;
   PointsTimer->OnTimer  = PointsTimerEvent;
   PointsCount = 0;
}

void __fastcall CSuspensionPointsStatusPage::PointsTimerEvent(TObject *Sender)
{
   StatusMessage = PointsStatusMessage;
   for(int i = 0; i < NUMBER_OF_POINTS; i++)
      StatusMessage += (i<=PointsCount) ? "." : " ";
   PointsCount++;
   PointsCount %= NUMBER_OF_POINTS; 
   Refresh();
}

bool CSuspensionPointsStatusPage::PreEnter(void)
{
   StatusMessage = PointsStatusMessage;
   for(int i = 0; i < NUMBER_OF_POINTS; i++)
      StatusMessage += " ";
   return CStatusWizardPage::PreEnter();
}

bool CSuspensionPointsStatusPage::Enter(void)
{
   PointsTimer->Enabled  = true;
   return CStatusWizardPage::Enter();
}

bool CSuspensionPointsStatusPage::Leave(TWizardPageLeaveReason LeaveReason)
{
   PointsTimer->Enabled  = false;
   return CStatusWizardPage::Leave(LeaveReason);
}

//==============================================================================
CHomingAxisPage::CHomingAxisPage(CQ2RTAutoWizard* Wizard, const QString Title, int ImageID, TWizardPageAttributes PageAttributes):
CWizardPage(Title,ImageID,PageAttributes | wpPreviousDisabled | wpNextDisabled),
CSuspensionPointsStatusPage(Title,ImageID,PageAttributes | wpPreviousDisabled | wpNextDisabled)
{
  m_Wizard = Wizard;
  PointsStatusMessage = LOAD_STRING(IDS_HOME_AXIS);
}

bool CHomingAxisPage::Enter(void)
{
  if ( !CSuspensionPointsStatusPage::Enter() )
     return false;
  m_Wizard->EnableAllAxesAndHome();
  m_Wizard->GotoNextPage();
  return true;
}
//==============================================================================


//#if defined EDEN_260 || defined EDEN_250 || defined EDEN_260_V || defined CONNEX_260
CInsertTrayPage::CInsertTrayPage(CAutoWizard* Wizard, int ImageID,TWizardPageAttributes PageAttributes) :
CWizardPage(LOAD_STRING(IDS_SYSTEM_CHECKLIST) /*originally: Wizard->GetTitle()*/,ImageID,PageAttributes | wpNextWhenSelected | wpClearOnShow | wpNoRefreshOnShow),
COneCheckBoxWizardPage(Wizard,IDS_INSERT_TRAY,IDS_TRAY_IS_INSERTED)
{
  m_Wizard = Wizard;
}

bool CInsertTrayPage::PreEnter(void)
{
  CBackEndInterface* BackEnd = CBackEndInterface::Instance();
  return (! BackEnd->IsTrayInserted());
}

bool CInsertTrayPage::Enter(void)
{
  if (PreEnter())
     m_Wizard->SetNextPage(this);
  else
     return COneCheckBoxWizardPage::Enter();
  return true;
}

CInsertTrayPage::~CInsertTrayPage(){}
//#endif

CBEMonitorActivator::CBEMonitorActivator()
{
   m_BackEnd->TurnHeadFilling(true);
}

CBEMonitorActivator::~CBEMonitorActivator()
{
   m_BackEnd->TurnHeadFilling(false);
}

