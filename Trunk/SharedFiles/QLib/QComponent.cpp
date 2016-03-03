/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib "active" component class.                           *
 * Module Description: Methods and properties with run-time info    *
 *                     can be defined in this class (and subclasses)*
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 03/07/2001                                           *
 * Last upate: 16/03/2003                                           *
 ********************************************************************/

#include "QComponent.h"


// Constructor
CQComponent::CQComponent(const QString Name,bool RosterRegistration)
{
  m_Name = Name;

  // Regiuster the component in the objects roster
  if(RosterRegistration)
    CQObjectsRoster::RegisterComponent(this);
}

// Destructor
CQComponent::~CQComponent(void)
{
  // Unregister from the roster
  CQObjectsRoster::UnRegisterComponent(this);
}

// Find a specific property by its name
CQPropertyObject *CQComponent::FindProperty(const QString PropertyName)
{
  for(TPropertiesContainer::iterator i = Properties.begin(); i != Properties.end(); i++)
    if((*i)->Name() == PropertyName)
      return (*i);

  // Not found
  return NULL;
}

// Find a specific method by its name
CQMethodObject *CQComponent::FindMethod(const QString MethodName)
{
  for(TMethodsContainer::iterator i = Methods.begin(); i != Methods.end(); i++)
    if((*i)->Name() == MethodName)
      return (*i);

  // Not found
  return NULL;
}

