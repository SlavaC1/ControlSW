#include "TempAppParams.h"
#include "ModesManager.h"
#include "Q2RTApplication.h"
#include "ModesDefs.h"
#include "QErrors.h"
#include "AppLogFile.h"

CTempAppParams::CTempMiniSequencer::CTempMiniSequencer(const QString& Name) :
CMiniSequencer(Name+"MiniSequencer"),
m_TempName(Name)
{}

QString CTempAppParams::CTempMiniSequencer::EvaluateExpression(const QString Input)
{
  QString NewInput,NewInputTmp = Input;
  NewInput    = QReplaceSubStr(NewInputTmp, "ModesManager", m_TempName+"ModesManager");
  NewInputTmp = QReplaceSubStr(NewInput,    "AppParams",    m_TempName+"AppParams");
  return CMiniSequencer::EvaluateExpression(NewInputTmp);
}

//bug 5719
// substitute every AppParams with m_TempName+"AppParams" in the script
void CTempAppParams::CTempMiniSequencer::ProcessScript(TStrings * StrList)
{
    QString NewInput;
    for(int i=0;i<StrList->Count;i++)
    {
        NewInput = QReplaceSubStr(StrList->Strings[i].c_str(),"AppParams",m_TempName+"AppParams");
        StrList->Strings[i] = NewInput.c_str();
    }
}


CTempAppParams::~CTempAppParams()
{
  delete m_TempModesManager;
  delete m_TempMiniSequencer;
}

CTempAppParams::CTempAppParams(QString Name, TInitStatus_E &StatusInit, bool DoRegister) :
CAppParams(CAppParams::Instance()->GetConfigPath(), StatusInit, DoRegister, Name+"AppParams")
{
  m_TempMiniSequencer = new CTempMiniSequencer(Name);
  m_TempModesManager  = new CModesManager(this, Q2RTApplication->AppFilePath.Value(), m_TempMiniSequencer, Name+"ModesManager", false /* don't refresh UI at mode change*/);
  LoadAll();
}

//bug 5719
void CTempAppParams::Init()
{
  CQLog::Write(LOG_TAG_TRAY_HEATERS, "ThermistorOpMode debugging: temp init. Temp mode mgr = 0x%x; Main mode mgr = 0x%x", m_TempModesManager, CModesManager::Instance());
  m_TempModesManager->GotoDefaultMode();
}

TQErrCode CTempAppParams::EnterMode(const QString ModeName, QString Dir)
{
  return m_TempModesManager->EnterMode(ModeName, Dir);
}

