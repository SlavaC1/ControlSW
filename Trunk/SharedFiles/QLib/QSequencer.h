/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib.                                                   *
 * Module Description: QLib generic sequencer.                      *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 30/08/2001                                           *
 * Last upate: 30/08/2001                                           *
 ********************************************************************/

#ifndef _Q_SEQUENCER_H_
#define _Q_SEQUENCER_H_

#include "QComponent.h"


// Exception class for the QSequencer
class EQSequencer : public EQException {
  public:
    EQSequencer(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

// Special exception class for sequence cancelation
class EQCancelSequence : public EQException {
  public:
    EQCancelSequence(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};


// QSequencer class
class CQSequencer : public CQComponent {
  private:

    // This flag is used to indicate that the sequence has been canceled
    bool m_CancelFlag;

    // Set/Get functions fot the canceled property
    void SetCanceled(bool /*Value*/) {}
    bool GetCanceled(void) {
      return m_CancelFlag;
    }

  protected:

    // The derived classes can override this function in order to provide
    // custom cancel logic.
    virtual void OnCancel(void) {}

  public:

    // This property is true if the sequence has been canceled
    DEFINE_PROPERTY(CQSequencer,bool,Canceled);

    // Constructor
    CQSequencer(const QString& Name);

    // Destructor
    ~CQSequencer(void);

    // Cancel the current sequence
    void Cancel(void);
};

#endif
