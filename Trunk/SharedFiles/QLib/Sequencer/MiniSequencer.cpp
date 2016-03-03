/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Generic.                                                *
 * Module Description: Mini actions sequencer based on the the QLib *
 *                     objects model.                               *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 18/07/2001                                           *
 * Last upate: 22/08/2001                                           *
 ********************************************************************/

#include "QUtils.h"
#include "QThreadUtils.h"
#include "QMonitor.h"
#include "QErrors.h"
#include "QEvaluate.h"
#include "MiniSequencer.h"




// Constructor
CMiniSequencer::CMiniSequencer(const QString& Name) : CQThread(true,Name)
{
  INIT_METHOD(CMiniSequencer,Sleep);
  INIT_METHOD(CMiniSequencer,Stop);
  INIT_VAR_PROPERTY(Busy,false);

  m_StopFlag = false;
  m_FreeCommandsSequence = false;

  // Since CMiniSequencer::Execute() references class members,
  //   we create the Thread 'suspended', 
  //   and Resume() it only at the end of the constructor:
  Resume();
}

// Destructor
CMiniSequencer::~CMiniSequencer(void)
{
  Terminate();
  m_StopFlag = true;
  m_StartExecution.SetEvent();
  WaitFor();
}

// Sleep for a given time
TQErrCode CMiniSequencer::Sleep(TQWaitTime SleepTime)
{
  QSleep(SleepTime);
  return Q_NO_ERROR;
}

// Execute a single line of script
QString CMiniSequencer::EvaluateExpression(const QString Input)
{
  CQEvaluate Eval;
  return Eval.Evaluate(Input);
}
// no need to process the script

#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
void CMiniSequencer::ProcessScript(TStrings *StrList)
{
}
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"



// Extract individual arguments and the number of different arguments
int CMiniSequencer::ExtractMethodArguments(const QString& ArgsString,QString *Arguments)
{
  int BeginPos,EndPos = 0;
  int ArgsCount;

  const QString Delimeters(" ,\t)");

  // Extract all the arguments
  for(ArgsCount = 0; ArgsCount < MAX_QMETHOD_ARGUMENTS; ArgsCount++)
  {
    BeginPos = ArgsString.find_first_not_of(Delimeters,EndPos);

    // Check if we are done
    if(BeginPos == -1)
      break;

    EndPos = ArgsString.find_first_of(Delimeters,BeginPos);

    // if EndPos is equal to -1 we have a missing closing brackets
    if(EndPos == -1)
      throw EMiniSequencer("Invalid method call");

    Arguments[ArgsCount] = QTrimString(ArgsString.substr(BeginPos,EndPos - BeginPos));
  }

  return ArgsCount;
}

// Perform a sequence of commands
void CMiniSequencer::ExecuteSequence(CQStringList *CommandsSequence,bool FreeCommandsSequence)
{
  // Yell if the sequencer is currently busy
  if(Busy)
    throw EMiniSequencer("Can not start new sequence while busy");

  m_CommandsSequence = CommandsSequence;
  m_FreeCommandsSequence = FreeCommandsSequence;

  m_StartExecution.SetEvent();
}

// Stop the sequencer
TQErrCode CMiniSequencer::Stop(void)
{
  // If currently busy
  if(Busy)
    // Mark as stopped
    m_StopFlag = true;

  return Q_NO_ERROR;
}

// Wait for stop
void CMiniSequencer::WaitForStop(void)
{
  // If currently busy
  if(Busy)
    // Wait for stop event
    m_StopEvent.WaitFor();
}

// Thread execute function
void CMiniSequencer::Execute(void)
{
  do
  {
    // Wait for start signal
    m_StartExecution.WaitFor();

    if(m_StopFlag)
      break;

    // Mark that the sequencer is busy
    Busy = true;

    for(unsigned i = 0; i < m_CommandsSequence->Count(); i++)
    {
      // Evaluate each expression 
      EvaluateExpression((*m_CommandsSequence)[i]);

      // Check if terminated or stop flag raised
      if(Terminated || m_StopFlag)
        break;
    }

    // If the FreeCommandsSequence flag is set, free the commands sequence
    if(m_FreeCommandsSequence)
    {
      delete m_CommandsSequence;
      m_CommandsSequence = NULL;
    }

    Busy = false;

    // Signal stop condition
    if(m_StopFlag)
    {
      m_StopFlag = false;
      m_StopEvent.SetEvent();
    }

  } while(!Terminated);
}

