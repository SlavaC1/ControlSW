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

#ifndef _MINI_SEQUENCER_H_
#define _MINI_SEQUENCER_H_

#include "QThread.h"
#include <Classes.hpp>
#include "QStringList.h"


// Exception class for the mini sequencer
class EMiniSequencer : public EQException {
  public:
    EMiniSequencer(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

// The mini sequencer is a thread
class CMiniSequencer : public CQThread {
  private:
    // Pointer to the current commands sequence string list
    CQStringList *m_CommandsSequence;

    CQEvent m_StartExecution,m_StopEvent;

    // Flag used for marking stop signal
    bool m_StopFlag;

    // If this flag is set, the m_CommandsSequence is freed after the sequence is done
    bool m_FreeCommandsSequence;

    // Thread execute function
    void Execute(void);

    // Help function for extracting all the arguments of a method
    int ExtractMethodArguments(const QString& ArgsString,QString *Arguments);

  public:
    // Constructor
    CMiniSequencer(const QString& Name = "MiniSequencer");

    // Destructor
    virtual ~CMiniSequencer(void);

    // Execute a single line of command
    virtual QString EvaluateExpression(const QString Input);

    virtual void ProcessScript(TStrings *StrList); //bug 5719

    // Perform a sequence of commands
    void ExecuteSequence(CQStringList *CommandsSequence,bool FreeCommandsSequence = false);

    // Wait until the sequencer is stopped (or terminated)
    void WaitForStop(void);

    // Stop the sequencer
    DEFINE_METHOD(CMiniSequencer,TQErrCode,Stop);

    // Sleep for a given time
    DEFINE_METHOD_1(CMiniSequencer,TQErrCode,Sleep,TQWaitTime);

    // True if a sequence is currently in progress 
    DEFINE_VAR_PROPERTY(bool,Busy);
};

#endif
