/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: Objects roster.                                          *
 * Module Description: The objects roster maintain a list of        *
 *                     component objects. The roster provides access*
 *                     functions for searching and traversing       *
 *                     the registered components.                   *
 *                                                                  *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 03/07/2001                                           *
 * Last upate: 27/07/2001                                           *
 ********************************************************************/

#include <algorithm>
#include "QObjectsRoster.h"
#include "QComponent.h"
#include "QMutex.h"


// std is required
using namespace std;


CQObjectsRoster *CQObjectsRoster::m_LocalInstance = 0;
CQMutex *CQObjectsRoster::m_GuardMutex;


// Init the roster for operation
void CQObjectsRoster::Init(void)
{
  if(m_LocalInstance == 0)
  {
    m_LocalInstance = new CQObjectsRoster;
    m_GuardMutex = new CQMutex;
  }
}

// Init the roster for operation
void CQObjectsRoster::DeInit(void)
{
  if(m_LocalInstance)
  {
    delete m_LocalInstance;
    m_LocalInstance = 0;
  }

  if(m_GuardMutex)
  {
    delete m_GuardMutex;
    m_GuardMutex = 0;
  }
}

// Return a reference to the roster components list
TQComponentList *CQObjectsRoster::GetComponentList(void)
{
  return &m_ComponentsList;
}

// Register components
void CQObjectsRoster::RegisterComponent(CQComponent *Component)
{
  if(m_LocalInstance)
  {
     m_GuardMutex->WaitFor();
     m_LocalInstance->m_ComponentsList.push_back(Component);
     m_GuardMutex->Release();
  }
}

// UnRegister components
void CQObjectsRoster::UnRegisterComponent(CQComponent *Component)
{
  if(m_LocalInstance)
  {
     m_GuardMutex->WaitFor();
     // Erase the specified component
     m_LocalInstance->RemoveComponentFromList(Component);
     m_GuardMutex->Release();
  }
}

// Find a specific component by name (NULL if not found)
// Note: The roster does not force uniquness for a component name.
CQComponent *CQObjectsRoster::FindComponent(const QString ComponentName)
{
  for(TQComponentList::iterator i = m_ComponentsList.begin(); i != m_ComponentsList.end(); i++)
    if((*i)->Name() == ComponentName)
        return (*i);

  // Not found
  return NULL;
}

// Find a specific component by index relative to the list start (NULL if not found)
CQComponent *CQObjectsRoster::FindComponent(int ComponentIndex)
{
  int Index = 0;
  TQComponentList::iterator i;

  for(i = m_ComponentsList.begin(); Index < ComponentIndex; i++,Index++)
    if(i == m_ComponentsList.end())
      return NULL;

  return (*i);
}

// Return an instance to the singleton
CQObjectsRoster *CQObjectsRoster::Instance(void)
{
  return m_LocalInstance;
}

// Help function for removing a component from the components list
void CQObjectsRoster::RemoveComponentFromList(CQComponent *Component)
{
  TQComponentList::iterator i = find(m_ComponentsList.begin(),m_ComponentsList.end(),Component);

  if(i != m_ComponentsList.end())
    m_ComponentsList.erase(i);
}

// Utility function for direct method invocation
QString CQObjectsRoster::InvokeMethod(const QString ComponentName,const QString MethodName,
                                      const QString *Args,unsigned ArgsNum)
{
  // Search for the requested component
  CQComponent *Component = FindComponent(ComponentName);

  // If not found, raise an error
  if(Component == NULL)
    throw EQObjectsRoster("Component \"" + ComponentName + "\" not found");

  // Search for the method
  CQMethodObject *Method = Component->FindMethod(MethodName);

  // If not found, raise an error
  if(Method == NULL)
    throw EQObjectsRoster("Method \"" + MethodName + "\" is not a member of \"" + ComponentName + "\"");

  // Invoke the method
  return Method->Invoke(Args,ArgsNum);
}

// Utility function for setting a new value to a property
void CQObjectsRoster::SetProperty(const QString ComponentName,const QString PropertyName,
                                     const QString PropertyValue)
{
  // Search for the requested component
  CQComponent *Component = FindComponent(ComponentName);

  // If not found, raise an error
  if(Component == NULL)
    throw EQObjectsRoster("Component \"" + ComponentName + "\" not found");

  // Search for the method
  CQPropertyObject *Property = Component->FindProperty(PropertyName);

  // If not found, raise an error
  if(Property == NULL)
    throw EQObjectsRoster("Property \"" + PropertyName + "\" is not a member of \"" + ComponentName + "\"");

  // Assign to the property
  Property->AssignFromString(PropertyValue);
}

// Utility function for setting a new value to a property
QString CQObjectsRoster::GetProperty(const QString ComponentName,const QString PropertyName)
{
  // Search for the requested component
  CQComponent *Component = FindComponent(ComponentName);

  // If not found, raise an error
  if(Component == NULL)
    throw EQObjectsRoster("Component \"" + ComponentName + "\" not found");

  // Search for the method
  CQPropertyObject *Property = Component->FindProperty(PropertyName);

  // If not found, raise an error
  if(Property == NULL)
    throw EQObjectsRoster("Property \"" + PropertyName + "\" is not a member of \"" + ComponentName + "\"");

  // Get the property
  return Property->ValueAsString();
}


