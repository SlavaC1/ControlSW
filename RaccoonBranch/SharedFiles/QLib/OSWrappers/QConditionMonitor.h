/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (condition monitor)                     *
 * Module Description: This class implement thread condition.       *
 *                     A set of threads can wait for a different set*
 *                     of conditions.                               *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 22/07/2001                                           *
 * Last upate: 29/08/2001                                           *
 ********************************************************************/

#ifndef _Q_CONDITION_MONITOR_H_
#define _Q_CONDITION_MONITOR_H_

#include <vector.h>
#include "QMutex.h"
#include "QEvent.h"


// Condition class - used with the condition monitor
class CQCondition : public CQObject {
  private:

    // Event for signaling a change in the condition
    CQEvent m_ConditionEvent;

  public:
    // Wait for the change event
    QLib::TQWaitResult WaitFor(TQWaitTime Timeout = Q_INFINITE_TIME) {
      return m_ConditionEvent.WaitFor(Timeout);
    }

    // Mark that the condition is satisfied
    void SetEvent(void) {
      m_ConditionEvent.SetEvent();
    }

    // Pure virtual condition function - implemented by the user
    virtual bool Condition(void) = 0;
};

// Condition monitor class
class CQConditionMonitor : public CQObject {
  private:
    // Type for a list of conditions
    typedef std::vector<CQCondition *> TConditionList;

    // List of active conditions
    TConditionList m_ConditionList;

    // Mutex for protecting access to condition resources
    CQMutex m_GuardingMutex;

  public:
    // Enter to resource protection area
    void Enter(void);

    // Leave resource protection area, and re-evaluate conditions
    void Leave(void);

    // Wait for a specific condition
	QLib::TQWaitResult WaitForCondition(CQCondition *Condition,TQWaitTime Timeout = Q_INFINITE_TIME);
};

// Single condition class - a single thread can wait for a single condition (used as standalone class)
class CQSingleCondition : public CQObject {
  private:
    // Event for signaling a change in the condition
    CQEvent m_ConditionEvent;

    // Mutex for protecting access to condition resources
    CQMutex m_GuardingMutex;


  public:
    // Wait for the condition
    QLib::TQWaitResult WaitFor(TQWaitTime Timeout = Q_INFINITE_TIME) {
      return m_ConditionEvent.WaitFor(Timeout);
    }

    // Enter to resource protection area
    void Enter(void);

    // Leave resource protection area, and re-evaluate conditions
    void Leave(void);

    // Pure virtual condition function - implemented by the user
    virtual bool Condition(void) = 0;
};

#endif

