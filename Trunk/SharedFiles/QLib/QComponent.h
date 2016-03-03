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

#ifndef _Q_COMPONENT_H_
#define _Q_COMPONENT_H_

#include <vector>
#include "QProperty.h"
#include "QMethod.h"
#include "QObjectsRoster.h"


// Component base class
class CQComponent : public CQObject {
  protected:
  	typedef std::vector<CQPropertyObject *> TPropertiesContainer;
	typedef std::vector<CQMethodObject *> TMethodsContainer;
	
  private:
    // Name of the component
    QString m_Name;

  public:
    // Constructor
    CQComponent(const QString Name = "NoName",bool RosterRegistration = true);

    // Destructor
    ~CQComponent(void);

    // Return reference to the name of the component
    QString& Name(void) {
      return m_Name;
    }

    // For better performance and simplicity two different arrays are used in the component -
    // Properties - a list of all the properties in the container
    // Methods - a list of all the methods in the container.
    TPropertiesContainer Properties;
    TMethodsContainer Methods;

    // Register a property object int the component
    void RegisterProperty(CQPropertyObject *ObjectToRegister) {
      Properties.push_back(ObjectToRegister);
    }

    // Register a method object int the component
    void RegisterMethod(CQMethodObject *ObjectToRegister) {
      Methods.push_back(ObjectToRegister);
    }

    // Return the number of properties
    inline int PropertyCount(void) {
      return Properties.size();
    }

    // Return the number of methods
    inline int MethodCount(void) {
      return Methods.size();
    }

    // Find a specific property by its name
    CQPropertyObject *FindProperty(const QString PropertyName);

    // Find a specific method by its name
    CQMethodObject *FindMethod(const QString MethodName);
};

#endif

