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

#include <algorithm>
#include "QConditionMonitor.h"


// std namespace is required for this module
using namespace std;


// CQConditionMonitor class implementation
// *********************************************************************************

// Enter to resource protection area
void CQConditionMonitor::Enter(void)
{
  // Take the mutex
  m_GuardingMutex.WaitFor();
}

// Leave resource protection area, and re-evaluate conditions
void CQConditionMonitor::Leave(void)
{
  // Re-evaluate all conditions
  for(TConditionList::iterator i = m_ConditionList.begin(); i != m_ConditionList.end(); ++i)
    // If the condition is true set the condition event
    if((*i)->Condition())
      (*i)->SetEvent();

  m_GuardingMutex.Release();
}

// Wait for a specific condition
QLib::TQWaitResult CQConditionMonitor::WaitForCondition(CQCondition *NewCondition,TQWaitTime Timeout)
{
  // Take the mutex
  m_GuardingMutex.WaitFor();

  // Evaluate the condition
  if(NewCondition->Condition())
  {
    // If true, don't add the the list and return immediately
    m_GuardingMutex.Release();
    return QLib::wrSignaled;
  }

  // Add the condition to the list and release the mutex
  m_ConditionList.push_back(NewCondition);

  m_GuardingMutex.Release();

  // Wait for the condition event
  QLib::TQWaitResult Result = NewCondition->WaitFor(Timeout);

  // Remove the condition from the list
  m_ConditionList.erase(remove(m_ConditionList.begin(),m_ConditionList.end(),NewCondition),m_ConditionList.end());

  return Result;
}

// CQSingleCondition class implementation
// *********************************************************************************

// Enter to resource protection area
void CQSingleCondition::Enter(void)
{
  // Take the mutex
  m_GuardingMutex.WaitFor();
}

// Leave resource protection area, and re-evaluate conditions
void CQSingleCondition::Leave(void)
{
  // Evaluate the conditions
  if(Condition())
    m_ConditionEvent.SetEvent();

  m_GuardingMutex.Release();
}

