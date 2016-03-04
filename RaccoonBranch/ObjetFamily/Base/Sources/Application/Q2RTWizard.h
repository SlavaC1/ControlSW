/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Q2RT Wizard class (CQ2RTWizard).                         *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 ********************************************************************/

#ifndef _Q2RT_WIZARD_H_
#define _Q2RT_WIZARD_H_

#include "AutoWizard.h"
#include "PythonAutoWizard.h"
#include "MachineManagerDefs.h"
#include "AppLogFile.h"
#include "FEResources.h"
#include "MotorDefs.h"
#include <vcl.h>
#include "AppParams.h" // for ALLOW_OPENING_MRW_FROM_HOST define
#include "WizardPages.h"
#include "Q2RTErrors.h"

#define Z_DOWN_WAIT_TIME 60
#define FORCE_HOME true
#define DONT_FORCE_HOME false

#define TRY_FUNCTION(_err_, _func_)  {\
                              if (IsCancelled()) throw CQ2RTAutoWizardCancelledException(); \
                              if ( (_err_ = _func_ ) != Q_NO_ERROR) {\
                                  throw CQ2RTAutoWizardException(PrintErrorMessage(_err_), _err_);}}

#define TRY_FUNCTION_IGNORE_CANCELLED(_err_, _func_)  \
                              if ( (_err_ = _func_ ) != Q_NO_ERROR) {\
                                  throw CQ2RTAutoWizardException(PrintErrorMessage(_err_), _err_);}

#define DEFAULT_TIMEOUT -1
                                  
class CQ2RTAutoWizardException : public EQException
{
  public:
	CQ2RTAutoWizardException(const QString& ErrMsg="",const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

class EQHeadFillingException : public EQException
{
  public:
	EQHeadFillingException();
};

class CQ2RTAutoWizardCancelledException : public EQException
{
  public:
	CQ2RTAutoWizardCancelledException();
};

class TWizardForm;
class CBackEndInterface;

enum
{
    ANSWER_NO  = 0
   ,ANSWER_YES
   ,NUMBER_OF_ANSWERS
};

typedef bool T_AxesTable[MAX_AXIS];

/*//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////*/
class CQ2RTPythonAutoWizard : public CPythonAutoWizard
{
protected:
    CBackEndInterface *m_BackEnd;
public:
    CQ2RTPythonAutoWizard (void);
    CQ2RTPythonAutoWizard (const QString PythonFileName);
    bool Start            (void);
    bool Cancel           (void);
    bool Close            (void);
    bool End              (void);
};

/*//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////*/
class CQ2RTAutoWizard : public CAutoWizard
{
private:

  bool m_InsidePage;
  bool m_isFirstStart;
  void AddServicePages();
    
protected:

  CBackEndInterface *m_BackEnd;
  CAppParams        *m_ParamMgr;
  int                m_CancelPageIndex;
  QString            m_CancelReason;
  TWizardForm*       m_WizardForm;
#ifdef ALLOW_OPENING_MRW_FROM_HOST
  static TTimer*     WizardTimer;
#endif
  bool               m_EnterStandBy;


  virtual void PageEnterWrapper(CWizardPage *WizardPage);
  virtual void PageLeaveWrapper(CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason);

  void EnableAxeAndHome(TMotorAxis Axis, bool Blocking = true, bool ForceHome = false);
  void EmulateCancelClick  (void);
  virtual void Q2RTCleanUp(void);
  virtual void Q2PostRTCleanUp(void);
  void WriteToLogFile(TLogFileTag TagID, QString FormatStr);
  void WriteToLogFile(TLogFileTag TagID, const char* FormatStr, ...);
  unsigned StartAsyncHeatingCycle(const int NumberOfHeadHeaters, bool IncludePreHeater=true, int* HeadsTemperatures=NULL);
  void EndAsyncHeatingCycle(unsigned StartTime, unsigned TimeOut);
  void HeadsHeatingCycle   (const int TimeOut = DEFAULT_TIMEOUT,const int NumberOfHeadHeaters = TOTAL_NUMBER_OF_HEATERS,bool IncludePreHeater = true,int* HeadsTemperatures=NULL);
  void HeadsFillingCycle(unsigned int FillingTimeout);
  void HeadsDrainingCycle (CWizardPage *Page, unsigned int DrainingTime, bool CheckingEmpty=false,bool wipe=true);
  void WaitForState(TMachineState MachineState);
  TMachineState WaitForStateToDiff(TMachineState MachineState);
  QString DefaultHelpFileName(void);

public:
    CQ2RTAutoWizard    (int     TitleResourceID, // id of string resource from FEResources.rc
                        bool    HelpVisible        = false,
                        int     DefaultImageID     = -1,
                        bool    UserButton1Visible = false,
                        bool    UserButton2Visible = false,
                        QString UserButton1Caption = "",
                        QString UserButton2Caption = "");
    bool Start          (void);
    bool Cancel         (void);
    bool Close          (void);
    bool End            (void);
    void SetWizardForm  (TWizardForm*  WizardForm);
    void WaitForEndOfPrint();

    bool Q2RTWizardSleep(int SleepTime);
    void EnableAllAxesAndHome(T_AxesTable a_AxesTable = NULL, bool Blocking = true, bool ForceHome = false);

};

/*//////////////////////////////////////////////////////////////////////////////
Additional Standart pages for wizard
//////////////////////////////////////////////////////////////////////////////*/
class COneCheckBoxWizardPage : public CCheckBoxWizardPage
{
private:

protected:

public:
  COneCheckBoxWizardPage(CAutoWizard* Wizard,int SubTitleStrRes,int CheckStrRes,int ImageID = -1,TWizardPageAttributes PageAttributes = 0);
  COneCheckBoxWizardPage(CAutoWizard* Wizard,QString SubTitleStr,QString CheckStr,int ImageID = -1,TWizardPageAttributes PageAttributes = 0);
  virtual ~COneCheckBoxWizardPage();
};

class CVerifyCleanTrayPage : public COneCheckBoxWizardPage
{
private:

protected:

public:
  CVerifyCleanTrayPage(CAutoWizard* Wizard,int ImageID = -1,TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES);
  virtual ~CVerifyCleanTrayPage();

};

class CCloseDoorPage : public COneCheckBoxWizardPage
{
private:
  CAutoWizard* m_Wizard;
protected:

public:
  CCloseDoorPage(CAutoWizard* Wizard,int ImageID = -1,TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES);
  virtual bool PreEnter(void);
  virtual bool Enter(void);  
};

class CLiquidsShortagePage : public CCheckBoxWizardPage
{
private:
  CAutoWizard* m_Wizard;
protected:

public:
  CLiquidsShortagePage(CAutoWizard* Wizard,int ImageID = -1,TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES);
  virtual bool PreEnter(void);
  virtual bool Enter(void);  
};

class CSuspensionPointsStatusPage : public CStatusWizardPage
{
private:
  TTimer *PointsTimer;
  int PointsCount;


protected:
  void __fastcall PointsTimerEvent(TObject *Sender);

public:
  QString PointsStatusMessage;

  CSuspensionPointsStatusPage(const QString Title, int ImageID = -1,
				TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES);
  virtual ~CSuspensionPointsStatusPage()
  {
	if(PointsTimer !=NULL)
		delete PointsTimer;
  }
  virtual bool PreEnter(void);
  virtual bool Enter(void);
  virtual bool Leave(TWizardPageLeaveReason LeaveReason);

};

class CHomingAxisPage : public CSuspensionPointsStatusPage
{
private:

  CQ2RTAutoWizard* m_Wizard;

public:

  CHomingAxisPage(CQ2RTAutoWizard* Wizard, const QString Title="", int ImageID = -1,
                TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES);
  virtual bool Enter(void);
};

class CInsertTrayPage : public COneCheckBoxWizardPage
{
private:
  CAutoWizard* m_Wizard;

protected:

public:
	CInsertTrayPage(CAutoWizard* Wizard,int ImageID = -1,TWizardPageAttributes PageAttributes = 0);
	virtual bool PreEnter(void);
    virtual bool Enter(void);
	virtual ~CInsertTrayPage();
};

class CBEMonitorActivator : public CQObject
{
private:
  CBackEndInterface *m_BackEnd;

protected:

public:
  CBEMonitorActivator();
  ~CBEMonitorActivator();

};

#endif


