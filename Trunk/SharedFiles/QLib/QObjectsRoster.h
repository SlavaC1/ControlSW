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
 * Last upate: 18/07/2001                                           *
 ********************************************************************/

#ifndef _Q_OBJECTS_ROSTER_H_
#define _Q_OBJECTS_ROSTER_H_

#include <list>
#include "QTypes.h"
#include "QException.h"


// Exception class for the objects roster
class EQObjectsRoster : public EQException {
  public:
    EQObjectsRoster(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};


// We need forward declaration for classCQComponent to avoid circular reference ("QObjectsRoster.h"
// is included by "QComponent.h").
class CQComponent;

class CQMutex;

// Type for the a components list
typedef std::list<CQComponent *> TQComponentList;

// The roster is implemented as a singleton
class CQObjectsRoster {
  private:
    TQComponentList m_ComponentsList;

    // Protection mutex for the local instance pointer
    static CQMutex *m_GuardMutex;

    // Private constructor and destructor to prevent unauthorized creation of this class
    CQObjectsRoster(void) {}

    // Pointer to the singleton object
    static CQObjectsRoster *m_LocalInstance;

    // Help function for removing a component from the components list
    void RemoveComponentFromList(CQComponent *Component);

  public:
    // Init the roster for operation
    static void Init(void);

    // Release the roster
    static void DeInit(void);

    // Functions for registering and unregistering a specific components
    static void RegisterComponent(CQComponent *Component);
    static void UnRegisterComponent(CQComponent *Component);

    // Return a reference to the roster components list
    TQComponentList *GetComponentList(void);

    // Find a specific component by name (NULL if not found)
    // Note: The roster does not force uniquness for a component name.
    CQComponent *FindComponent(const QString ComponentName);

    // Find a specific component by index relative to the list start (NULL if not found)
    CQComponent *FindComponent(int ComponentIndex);

    // Return an instance to the singleton
    static CQObjectsRoster *Instance(void);

    // Utility function for direct method invocation
    QString InvokeMethod(const QString ComponentName,const QString MethodName,
                         const QString *Args,unsigned ArgsNum);

    // Utility function for setting and getting a property value
    void SetProperty(const QString ComponentName,const QString PropertyName,const QString PropertyValue);
    QString GetProperty(const QString ComponentName,const QString PropertyName);
};

#endif

