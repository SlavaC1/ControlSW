/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib property class.                                     *
 * Module Description: By using this class a named properties from  *
 *                     different types can be defined. A "set" and  *
 *                     "get" methods can be attached to the         *
 *                     property.                                    *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 01/07/2001                                           *
 * Last upate: 18/09/2003                                           *
 ********************************************************************/

#ifndef _Q_PROPERTY_H_
#define _Q_PROPERTY_H_

#include <iomanip.h>
#include "QObject.h"
#include "QException.h"
#include "QUtils.h"


// This enumerated type is used for detecting property object types
typedef enum {pkUndefined,pkProperty,pkVariableProperty,pkParameter} TQPropertyKind;

// These constants are used to identify the "known" property types
typedef enum {ptInt,ptReal,ptBool,ptGeneric,ptEnum,ptArray} TPropertyTypeID;


// Utility function for detecting a known type.
// This function uses specialization to categorize a template type to one of several known groups.
template<class T>
inline TPropertyTypeID TypeIDSelector(void)
{
  return ptGeneric;
}

template<>
inline TPropertyTypeID TypeIDSelector<int>(void)
{
  return ptInt;
}

template<>
inline TPropertyTypeID TypeIDSelector<unsigned>(void)
{
  return ptInt;
}

template<>
inline TPropertyTypeID TypeIDSelector<char>(void)
{
  return ptInt;
}

template<>
inline TPropertyTypeID TypeIDSelector<unsigned char>(void)
{
  return ptInt;
}

template<>
inline TPropertyTypeID TypeIDSelector<short>(void)
{
  return ptInt;
}

template<>
inline TPropertyTypeID TypeIDSelector<unsigned short>(void)
{
  return ptInt;
}

template<>
inline TPropertyTypeID TypeIDSelector<long>(void)
{
  return ptInt;
}

template<>
inline TPropertyTypeID TypeIDSelector<unsigned long>(void)
{
  return ptInt;
}

template<>
inline TPropertyTypeID TypeIDSelector<float>(void)
{
  return ptReal;
}

template<>
inline TPropertyTypeID TypeIDSelector<double>(void)
{
  return ptReal;
}

template<>
inline TPropertyTypeID TypeIDSelector<long double>(void)
{
  return ptReal;
}

template<>
inline TPropertyTypeID TypeIDSelector<bool>(void)
{
  return ptBool;
}

// Exception class for all the QLib RTTI elements
class EQProperty : public EQException {
  public:
    EQProperty(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

// Property base class
class CQPropertyObject : public CQObject {
  private:

    // Name of the method
    QString m_Name;

  public:

    // Basic property initialization
    void InitProperty(const QString& Name) {
      m_Name = Name;
    }

    // Return the method name
    QString& Name(void) {
      return m_Name;
    }

    // Return the kind the property
    virtual TQPropertyKind GetPropertyKind(void) {
      return pkUndefined;
    }

    // Get the type signature
    virtual TPropertyTypeID GetTypeID(void) = 0;
    
    // Assign a value to the property from string (return false if error)
    virtual void AssignFromString(const QString& s) = 0;

    // Conversion functions from internal data to string
    virtual QString ValueAsString(void) = 0;
};


// The custom property class needs to know two types - the owner class type and the RTTI element value type
template <class TRTTIValue>
class CQCustomProperty : public CQPropertyObject {
  protected:

    // Utility function for converting a value from type TRTTIValue to a string
    QString ValueToString(const TRTTIValue& v) {
      return QValueToStr(v);
    }

    // Utility function for converting a value from type TRTTIValue to a string
    TRTTIValue StringToValue(const QString& s) {
      return QStrToValue<TRTTIValue>(s);
    }

  public:
    // Constructor (default)
    CQCustomProperty(const QString& Name="") {
      InitProperty(Name);
    }

    // General case for type ID
    TPropertyTypeID GetTypeID(void) {
      return TypeIDSelector<TRTTIValue>();
    }
};

// Specialized version of conversion values

template<>
QString CQCustomProperty<QString>::ValueToString(const QString& v)
{
  return v;
}

template<>
QString CQCustomProperty<QString>::StringToValue(const QString& v)
{
  return v;
}

// The property class needs to know two types - the owner class type and the property value type
template <class COwnerClass,class TPropertyValue>
class CQProperty : public CQCustomProperty<TPropertyValue> {
  private:
    // Typedefs for the set and get methods
    typedef void (COwnerClass::*TSetMethod)(TPropertyValue);
    typedef TPropertyValue (COwnerClass::*TGetMethod)(void);

    // Pointers to the "set" and "get" functions
    TSetMethod m_SetMethod;
    TGetMethod m_GetMethod;

    COwnerClass *m_Owner; 

    // Return property kind
    TQPropertyKind GetPropertyKind(void) {
      return pkProperty;
    }

  public:
    // Constructor (default)
    CQProperty(void) {
      Init(0,"",0,0);
    }

    // Value (cast operator to type TPropertyValue)
    inline operator TPropertyValue () {
      return ((*m_Owner).*m_GetMethod)();
    }

    // Value as string (cast operator to type QString)
    inline operator QString () {
      return ValueToString(((*m_Owner).*m_GetMethod)());
    }

    // Explicit value getter
    inline TPropertyValue Value(void) {
      return ((*m_Owner).*m_GetMethod)();
    }

    // Assignment of value from type T
    inline CQProperty<COwnerClass,TPropertyValue> & operator = (const TPropertyValue& Value) {
      ((*m_Owner).*m_SetMethod)(Value);
      return *this;
    }

    // Assignment of value from type QString
    inline CQProperty<COwnerClass,TPropertyValue> & operator = (const QString& Value) {
      ((*m_Owner).*m_SetMethod)(StringToValue(Value));
      return *this;
    }

    // Initialize internal members
    void Init(COwnerClass *Owner,const QString& Name,TSetMethod SetMethod,TGetMethod GetMethod) {
      InitProperty(Name);

      m_Owner = Owner;
      m_SetMethod = SetMethod;
      m_GetMethod = GetMethod;
    }

    // Assign a value to the property from string (return false if error)
    void AssignFromString(const QString& s) {
      ((*m_Owner).*m_SetMethod)(StringToValue(s));
    }

    // Conversion functions from internal data to string
    QString ValueAsString(void) {
      return ValueToString(((*m_Owner).*m_GetMethod)());
    }
};

// This property class implement a named variable (no set/get methods just plain value)
template <class TPropertyValue>
class CQVariableProperty : public CQCustomProperty<TPropertyValue> {
  private:
    // Current value
    TPropertyValue m_Value;

    // Return variable property kind
    TQPropertyKind GetPropertyKind(void) {
      return pkVariableProperty;
    }

  public:
    // Constructor (default)
    CQVariableProperty(void) {
      Init("",TPropertyValue());
    }

    // Value (cast operator to type TPropertyValue)
    inline operator TPropertyValue () {
      return m_Value;
    }

    // Explicit value getter
    inline TPropertyValue& Value(void) {
      return m_Value;
    }

    // Assignment of value from type T
    inline CQVariableProperty<TPropertyValue> & operator = (const TPropertyValue& Value) {
      m_Value = Value;
      return *this;
    }

    // Assignment of value from type QString
    inline CQVariableProperty<TPropertyValue> & operator = (const QString& Value) {
      m_Value = StringToValue(Value);
      return *this;
    }

    // Initialize internal members
    void Init(const QString& Name,const TPropertyValue& InitialValue) {
      InitProperty(Name);
      m_Value = InitialValue;
    }

    // Assign a value to the property from string (return false if error)
    void AssignFromString(const QString& s) {
      m_Value = StringToValue(s);
    }

    // Conversion functions from internal data to string
    QString ValueAsString(void) {
      return ValueToString(m_Value);
    }
};

// Define a standard property
#define DEFINE_PROPERTY(ClassName,PropertyType,PropName) CQProperty<ClassName,PropertyType> PropName

// Initialize a standard property
#define INIT_PROPERTY(ClassName,PropName,SetMethod,GetMethod)   \
PropName.Init(this,#PropName,&ClassName::SetMethod,&ClassName::GetMethod);   \
this->RegisterProperty(&PropName)

// Initialize a standard property without registering in a component
#define INIT_PROPERTY_N(ClassName,PropName,SetMethod,GetMethod)   \
PropName.Init(this,#PropName,&ClassName::SetMethod,&ClassName::GetMethod)

// Define a property variable
#define DEFINE_VAR_PROPERTY(PropertyType,PropName) CQVariableProperty<PropertyType> PropName

// Initialize a property variable
#define INIT_VAR_PROPERTY(PropName,InitialValue)   \
PropName.Init(#PropName,InitialValue);   \
this->RegisterProperty(&PropName)

// Initialize a property variable without registering in a component
#define INIT_VAR_PROPERTY_N(PropName,InitialValue)   \
PropName.Init(#PropName,InitialValue)

#endif

