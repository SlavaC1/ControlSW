/********************************************************************
 *                    Generic parameters manager                    *
 *                    --------------------------                    *
 * Module description: This module implement a generic parameters   *
 *                     container capable of saving and loading      *
 *                     of parameters to INI files.                  *
 *                                                                  *
 * Compilation: Standard C++ / BCB.                                 *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 26/03/2001                                           *
 * Last upate: 04/06/2001                                           *
 ********************************************************************/

#ifndef _PARAMS_CONTAINER_H_
#define _PARAMS_CONTAINER_H_

#include <sstream>
#include <vector>
#include <list>
#include <limits>
#include <IniFiles.hpp>


// Disable warnings about inline functions
#pragma option push -w-inl

// These constants are used to identify the "knowm" parameter types
typedef enum {ptInt,ptReal,ptBool,ptGeneric} TParamTypeID;

// Extended parameter attributes that can be ORd together
typedef unsigned long TParamAttributes;

const paDefault              = 0;          // Default attributes (visible, read/write, rejecct range errors,
                                           // show description, allow save, observers call on change only)
const paHidden               = (1 << 0);   // The parameter is "hidden" (will not be displayed in a GUI dialog)
const paRDOnly               = (1 << 1);   // The parameter can not be changed (in a GUI dialog not from application)
const paNoSave               = (1 << 2);   // The parameter will not be write/read to/from an INI file
const paLimitsClip           = (1 << 3);   // The assigned value to the parameter will be clipped to the min or max limits
const paLimitsException      = (1 << 4);   // An exception will be thrown on an assignment range error
const paNoDescription        = (1 << 5);   // The description string will not be shown and a comment will not be used
const paForceObserversUpdate = (1 << 6);   // The observers will be called on every assignment to the parameter
const paBlockDefaults        = (1 << 7);   // This option prevents the possibility to do defaults override in parameters
                                           // GUI dialog. 

// Exception class for the name parameter
class ENameParameter : public Exception {
  public:
    __fastcall ENameParameter(const AnsiString& Msg) : Exception(Msg) {}
};

// Forward declaration for the parameters container class
class CParamsContainer;

// Base class for the generic parameter class
class CNameParamBase {
  protected:
    // Parameter name
    AnsiString m_Name;

    // Name of the section in the INI of the parameter
    AnsiString m_SectionName;

    // Optional description string
    AnsiString m_Description;

    // True indicates limits checking mode
    bool m_CheckLimitsMode;

    // Extended attribute set
    TParamAttributes m_Attributes;

    // Pointer to the parent container of the parameter
    CParamsContainer *m_Container;

  public:
    // Constructor
    CNameParamBase(const AnsiString& Name,const AnsiString& SectionName,bool CheckLimitsMode=false) {
      // Initialize internal variables
      m_Name = Name;
      m_SectionName = SectionName;
      m_CheckLimitsMode = CheckLimitsMode;
      m_Attributes = paDefault;
      m_Container = NULL;
    }

    // Return the name of the param
    AnsiString Name(void) {
      return m_Name;
    }

    // Return the section name of the param
    AnsiString SectionName(void) {
      return m_SectionName;
    }

    // Return the description string
    AnsiString Description(void) {
      return m_Description;
    }

    // Return true if limits checking is enabled
    bool GetLimitsMode(void) {
      return m_CheckLimitsMode;
    }

    // Set extended attributes
    void SetAttributes(TParamAttributes Attributes) {
      m_Attributes = Attributes;
    }

    // Get current attribute mask
    TParamAttributes GetAttributes(void) {
      return m_Attributes;
    }

    // Set new description string
    void SetDescription(const AnsiString& Description) {
      m_Description = Description;
    }

    // Save the parameter to the container INI file
    inline void Save(void);

    // Load the parameter from the container INI file
    inline void Load(void);

    // Pure virtual functions
    virtual bool StringToValue(const AnsiString& s) = 0;

    // Conversion functions from internal data to string
    virtual AnsiString ValueToString(void) = 0;
    virtual AnsiString DefaultValueToString(void) = 0;
    virtual AnsiString MinLimitToString(void) = 0;
    virtual AnsiString MaxLimitToString(void) = 0;

    // Restore the parameter default value
    virtual void RestoreDefault(void) = 0;

    // Get the type signature
    virtual TParamTypeID GetTypeID(void) = 0;
};

// Generic parameter class
template <class T>
class CNameParam : public CNameParamBase {
  private:
    // This type defines a callback for a parameter with the new value as a parameter
    typedef void __fastcall (__closure *TParamChangeEvent)(const T& NewValue);

    // This type defines a callback for a generic parameter
    typedef void __fastcall (__closure *TGenericParamChangeEvent)(CNameParamBase *Param);

    // Type for list of specifc observers
    typedef std::vector<TParamChangeEvent> TObserverList;

    // Type for list of generic observers
    typedef std::vector<TGenericParamChangeEvent> TGenericObserverList;

    // Lists of events for notification
    TObserverList m_ObserverList;
    TGenericObserverList m_GenericObserverList;

    // Parameter values (template type)
    T m_Value;
    T m_DefaultValue;
    T m_MinValue;
    T m_MaxValue;

    // This variable is required for the notification system - notifications are
    // triggered only if the parameter has changed since last assignment.
    T m_OldValue;

    // Return true if the value is in the min/max limits (return false if limit
    // error).
    bool CheckLimits(T Value) {
      if(m_CheckLimitsMode)
        return !((Value < m_MinValue) || (Value > m_MaxValue));

      // If the limits checking mode is off, always allow value assignment
      return true;
    }

    // Utility function for converting a value from type T to a string
    AnsiString ConvertValueToString(const T& v) {
      std::stringstream StrStrm;

      // Convert value to string
      StrStrm << v;

      // return as AnsiString
      return StrStrm.str().c_str();
    }

    // Assign a new value to the parameter
    void AssignValue(T NewValue) {
      // Check the value agains the limits
      if(!CheckLimits(NewValue)) {
        // The value is outside the limits range

        // If limits clipping is enabled, clip the value and assign to the value member
        if(m_Attributes & paLimitsClip) {
          // Save old value
          m_OldValue = m_Value;

          // Clip to maximum
          if(NewValue > m_MaxValue)
            m_Value = m_MaxValue;
          // Clip to minimum
          else if(NewValue < m_MinValue)
            m_Value = m_MinValue;
        }
        else
          // If exception throwing is enabled raise an exception
          if(m_Attributes & paLimitsException)
            throw ENameParameter("Parameter: " + Name() + " is out of range (Value = " +
                                 ConvertValueToString(NewValue) +
                                 ", Min = " + MinLimitToString() + " ,Max = " + MaxLimitToString() + ")");
      } else {
          // Save old value
          m_OldValue = m_Value;

          // Assign normally
          m_Value = NewValue;
        }

      // If the force observers update flag is set, do not check for parameter value change
      if(m_Attributes & paForceObserversUpdate)
        NotifyObservers();
      else
        // If the value has changed, notify all observers
        if(m_Value != m_OldValue)
          NotifyObservers();
    }

    // Call all register observer events
    void NotifyObservers(void) {
      for(TObserverList::iterator i = m_ObserverList.begin(); i != m_ObserverList.end(); i++)
        (*i)(m_Value);

      for(TGenericObserverList::iterator i = m_GenericObserverList.begin(); i != m_GenericObserverList.end(); i++)
        (*i)(this);
    }

  public:
    // Constructor 1
    CNameParam(const AnsiString& Name,const T& InitialValue = T(),const AnsiString& SectionName="")
      : CNameParamBase(Name,SectionName) {

      // Initialize the minimum and maximum values
      m_MinValue = std::numeric_limits<T>::min();
      m_MaxValue = std::numeric_limits<T>::max();

      // Set initial (default) values
      m_DefaultValue = m_Value = InitialValue;
    }

    // Default constructor
    CNameParam(void) : CNameParamBase("","") {}

    // Return a pointer to the base class (this is required because we overloaded
    // the & operator so we can not take the address of a parameter directly)
    CNameParamBase *GetBaseClassPtr(void) {
      return this;
    }

    // Add an event to the parameter observers list
    void AddObserver(TParamChangeEvent Event) {
      m_ObserverList.push_back(Event);
    }

    // Add an event to the parameter generic observers list
    void AddGenericObserver(TGenericParamChangeEvent Event) {
      m_GenericObserverList.push_back(Event);
    }

    // Remove an existing event from the parameter observers list
    void UnregisterObserver(TParamChangeEvent Event) {
      m_ObserverList.erase(std::remove(m_ObserverList.begin(),m_ObserverList.end(),Event),m_ObserverList.end());
    }

    // Remove an existing event from the parameter generic observers list
    void UnregisterGenericObserver(TParamChangeEvent Event) {
      m_GenericObserverList.erase(std::remove(m_GenericObserverList.begin(),m_GenericObserverList.end(),Event),
                                  m_GenericObserverList.end());
    }

    // Set the parameter properties
    void SetProperties(const AnsiString& Name,const T& InitialValue,const AnsiString& SectionName,
                       CParamsContainer *Container = NULL,
                       bool CheckLimitsMode = false,
                       T MinValue = std::numeric_limits<T>::min(),
                       T MaxValue = std::numeric_limits<T>::max()) {
      m_Name = Name;
      m_SectionName = SectionName;

      // Set initial (and default) values
      m_DefaultValue = m_Value = InitialValue;

      m_CheckLimitsMode = CheckLimitsMode;

      // If we are not in limits mode, reset the range variables to maximumu/minimum
      if(m_CheckLimitsMode) {
        // Remember limit values
        m_MinValue = MinValue;
        m_MaxValue = MaxValue;
      } else {
          // Initialize the minimum and maximum values
          m_MinValue = std::numeric_limits<T>::min();
          m_MaxValue = std::numeric_limits<T>::max();
        }

      m_Container = Container;

      // Register the parameter in the container
      if(m_Container)
        m_Container->RegisterParamForIO(this);
    }

    // Conversion operators to make this class look like variable from type T

    // Value
    inline operator T & () {
      return m_Value;
    }

    // Pointer (address of...)
    inline T * operator & () {
      return &m_Value;
    }

    // Assignment of value from type T
    inline CNameParam<T> & operator = (const T& Value) {
      AssignValue(Value);

      return *this;
    }

    // Explicit "get value" access function
    inline T& Value(void) {
      return m_Value;
    }

    // Get the default value
    inline T & DefaultValue(void) {
      return m_DefaultValue;
    }

    // Restore default value
    void RestoreDefault(void) {
      m_Value = m_DefaultValue;
    }

    // Get the parameter value from a string
    bool StringToValue(const AnsiString& s) {
      std::stringstream StrStrm(s.c_str());

      T TmpValue;
      StrStrm >> TmpValue;

      // Check if not ok
      if(StrStrm.fail())
        return false;

      AssignValue(TmpValue);
      return true;
    }

    // Make a string from the current value
    AnsiString ValueToString(void) {
      return ConvertValueToString(m_Value);
    }

    // Make a string from the default value
    AnsiString DefaultValueToString(void) {
      return ConvertValueToString(m_DefaultValue);
    }

    // Make a string from the minimum limit value
    AnsiString MinLimitToString(void) {
      return ConvertValueToString(m_MinValue);
    }

    // Make a string from the maximum limit value
    AnsiString MaxLimitToString(void) {
      return ConvertValueToString(m_MaxValue);
    }

    // General case for type ID
    TParamTypeID GetTypeID(void) {
      return ptGeneric;
    }
};

/* AnsiString does not support the << or >> operators. That's why
   Template specialization is required in order to support parameters of
   type AnsiString. */
template<>
inline bool CNameParam<AnsiString>::StringToValue(const AnsiString& s)
{
  m_Value = s;
  return true;
}

template<>
inline AnsiString CNameParam<AnsiString>::ConvertValueToString(const AnsiString& v)
{
  return v;
}

// The GetTypeID() function is used to classify the built in parameters, unknown types,
// are considered to be from type ptGeneric.
template<>
inline TParamTypeID CNameParam<int>::GetTypeID(void)
{
  return ptInt;
}

template<>
inline TParamTypeID CNameParam<unsigned>::GetTypeID(void)
{
  return ptInt;
}

template<>
inline TParamTypeID CNameParam<char>::GetTypeID(void)
{
  return ptInt;
}

template<>
inline TParamTypeID CNameParam<unsigned char>::GetTypeID(void)
{
  return ptInt;
}

template<>
inline TParamTypeID CNameParam<short>::GetTypeID(void)
{
  return ptInt;
}

template<>
inline TParamTypeID CNameParam<unsigned short>::GetTypeID(void)
{
  return ptInt;
}

template<>
inline TParamTypeID CNameParam<long>::GetTypeID(void)
{
  return ptInt;
}

template<>
inline TParamTypeID CNameParam<unsigned long>::GetTypeID(void)
{
  return ptInt;
}

template<>
inline TParamTypeID CNameParam<float>::GetTypeID(void)
{
  return ptReal;
}

template<>
inline TParamTypeID CNameParam<double>::GetTypeID(void)
{
  return ptReal;
}

template<>
inline TParamTypeID CNameParam<long double>::GetTypeID(void)
{
  return ptReal;
}

template<>
inline TParamTypeID CNameParam<bool>::GetTypeID(void)
{
  return ptBool;
}

// Type for the basic paramters list data structure
typedef std::list<CNameParamBase *> TNameParamList;

// Parameters container class
class CParamsContainer {
  private:
    // Parameters are assigned to this container
    TNameParamList m_ParamList;

    // VCL INI object
    TIniFile *m_IniFile;

    // Utility function for adding a ';' char "comment" to a string
    AnsiString AddCommentToString(const AnsiString& S,const AnsiString& Comment) {
      // If no comment is given, just return the input string
      if(Comment.IsEmpty())
        return S.TrimRight();

      return (S.TrimRight() + "   ;" + Comment);
    }

    // Utility function for removing a ';' char "comment" from a string
    AnsiString RemoveCommentFromString(const AnsiString& S) {
      // Find the position of the comment start delimiter
      int i = S.LastDelimiter(";");

      // If not found, trim the string to the right and return
      if(i == 0)
        return S.TrimRight();

      // Cut the string up to the ';' char, trim and return
      return S.SubString(1,i - 1).TrimRight();
    }

  public:
    // Default constructor
    CParamsContainer(void) {
      m_IniFile = NULL;
    }

    // Constructor
    CParamsContainer(const AnsiString& IniFileName) {
      // If a file name is assigned, create a TIniFile object
      if(IniFileName != "")
        m_IniFile = new TIniFile(IniFileName);
      else
        m_IniFile = NULL;
    }

    // Params destructor
    ~CParamsContainer(void) {
      if(m_IniFile)
				Q_SAFE_DELETE(m_IniFile);
    }

    // Change the current attached INI file
    void AssignToINIFile(const AnsiString& IniFileName) {
      if(m_IniFile)
        Q_SAFE_DELETE(m_IniFile);

      m_IniFile = new TIniFile(IniFileName);
    }

    // Register a parameter for an automatic I/O operation
    void RegisterParamForIO(CNameParamBase *Param) {
      m_ParamList.push_back(Param);
    }

    // Get a reference to the internal parametes list
    TNameParamList& GetParamList(void) {
      return m_ParamList;
    }

    // Save a specific parameter
    void SaveSpecificParam(CNameParamBase *Param) {
      // If the NoSave options is selected, do nothing
      if(!(Param->GetAttributes() & paNoSave)) {
        // Check if the parameter has Name,SectionName and INI object assigned Ok
        if((Param->Name() != "") && (Param->SectionName() != "") && m_IniFile) {

          // Convert the parameter value to string
          AnsiString ValueInINI(Param->ValueToString());

          // If the parameter doesnt have paNoDescription attribute, add the description as a comment
          // in the INI file.
          if(!(Param->GetAttributes() & paNoDescription))
            ValueInINI = AddCommentToString(ValueInINI,Param->Description());

          m_IniFile->WriteString(Param->SectionName(),Param->Name(),ValueInINI);
        }
      }
    }

    // Load a specific parameter
    void LoadSpecificParam(CNameParamBase *Param) {
      // If the NoSave options is selected, do nothing
      if(!(Param->GetAttributes() & paNoSave)) {
        // Check if the parameter has Name and SectionName assigned Ok
        if((Param->Name() != "") && (Param->SectionName() != "") && m_IniFile) {

          // Read the raw string value from the INI file
          AnsiString ValueInINI = m_IniFile->ReadString(Param->SectionName(),Param->Name(),
                                                        Param->DefaultValueToString());

          // Remove the comment and convert to value
          Param->StringToValue(RemoveCommentFromString(ValueInINI));
        }
      }
    }

    // Save all parameters to INI file
    void SaveAll(void) {
      for(TNameParamList::iterator i = m_ParamList.begin(); i != m_ParamList.end(); i++)
        SaveSpecificParam(*i);
    }

    // Load all parameters from INI file
    void LoadAll(void) {
      for(TNameParamList::iterator i = m_ParamList.begin(); i != m_ParamList.end(); i++)
        LoadSpecificParam(*i);
    }

    // Restore defaults for all parameters
    void RestoreDefaults(void) {
      for(TNameParamList::iterator i = m_ParamList.begin(); i != m_ParamList.end(); i++)
        (*i)->RestoreDefault();
    }
};

// These methods must be implemented AFTER the CParamsContainer class implementation

// Save the parameter to the container INI file
inline void CNameParamBase::Save(void)
{
  if(m_Container)
    m_Container->SaveSpecificParam(this);
}

// Load the parameter from the container INI file
inline void CNameParamBase::Load(void)
{
  if(m_Container)
    m_Container->LoadSpecificParam(this);
}

// Restore previous options
#pragma option pop

/* Macro to define a name parameter */
#define DEFINE_PARAM(ParamType,ParamName) \
  CNameParam<ParamType> ParamName

/* Macro to initialize a name parameter (inside a container) */
#define INIT_PARAM(ParamName,InitialValue,SectionName) \
  ParamName.SetProperties(#ParamName,InitialValue,SectionName,this)

/* Macro to initialize a name parameter with limits checking (inside a container) */
#define INIT_PARAM_WITH_LIMITS(ParamName,InitialValue,MinValue,MaxValue,SectionName) \
  ParamName.SetProperties(#ParamName,InitialValue,SectionName,this,true,MinValue,MaxValue)

/* Macro to initialize a name parameter with extended settings */
#define INIT_PARAM_EXTENDED(ParamName,InitialValue,MinValue,MaxValue,EnableLimits,ParamNameStr,SectionName) \
  ParamName.SetProperties(ParamNameStr,InitialValue,SectionName,this,EnableLimits,MinValue,MaxValue)

#endif

