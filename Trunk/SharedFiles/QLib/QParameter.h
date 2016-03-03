/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib parameter class.                                    *
 * Module Description: By using this class a named parameters from  *
 *                     different types can be defined.              *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 03/07/2001                                           *
 * Last upate: 18/09/2003                                           *
 ********************************************************************/

#ifndef _Q_PARAMETER_H_
#define _Q_PARAMETER_H_

#include <vector>
#include <stack>
#include "QProperty.h"
#include "QStringList.h"

const int SUPER_USER_LEVEL  = 0; // For experiments (Eli K?)
const int LEVEL_ONE         = 1;
const int RD_LEVEL          = 2; // R&D advanced user
const int LEVEL_THREE       = 3;
const int QA_LEVEL          = 4;
const int LEVEL_FIVE        = 5;
const int ALPHA_LEVEL       = 6; // R&D Level
const int LEVEL_SEVEN       = 7;
const int SERVICE_LEVEL     = 8; // Service & Production
const int LEVEL_NINE        = 9;

#define APP_PARAMS_DELIMETER ","

// Extended parameter attributes that can be ORd together
typedef ULONG TParamAttributes;

const ULONG paDefault              = 0;          // Default attributes (visible, read/write, rejecct range errors,
                                                 // show description, allow save, observers call on change only)
const ULONG paHidden               = (1 << 0);   // The parameter is "hidden" (will not be displayed in a GUI dialog)
const ULONG paRDOnly               = (1 << 1);   // The parameter can not be changed (in a GUI dialog not from application)
const ULONG paNoSave               = (1 << 2);   // The parameter will not be write/read to/from an INI file
const ULONG paLimitsClip           = (1 << 3);   // The assigned value to the parameter will be clipped to the min or max limits
const ULONG paLimitsException      = (1 << 4);   // An exception will be thrown on an assignment range error
const ULONG paForceObserversUpdate = (1 << 5);   // The observers will be called on every assignment to the parameter
const ULONG paBlockDefaults        = (1 << 6);   // This option prevents the possibility to do defaults override in parameters
                                                 // interactive dialog.
const ULONG paNoDescription        = (1 << 7);
const ULONG paRestartRequired      = (1 << 8);   // A parameter change require an application restart
const ULONG paFromMaterialMode     = (1 << 9);   // Parameters that were loaded / calculated from materials modes

// Exception class for the QLib parameters
class EQParameterError : public EQException
{
public:
   EQParameterError(const QString& ErrMsg,const TQErrCode ErrCode=0): EQException(ErrMsg,ErrCode) {}
};

class CQParameterBase;

// This type defines a callback for a parameter with the new value as a parameter
typedef void (*TParamChangeEvent)(CQParameterBase* /*Param*/,TGenericCockie /*Cockie*/);

typedef enum
{
    Q_GENERIC_OBSERVER = 0
   ,Q_SAVE_OBSERVER
   ,Q_ENABLE_ALWAYS_OBSERVER
   ,Q_USER_OBSERVER
}TObserverType;

struct TObserverItem
{
  TObserverType Type;
  TParamChangeEvent Event;
  TGenericCockie Cockie;

  // Default constructor
  TObserverItem(void);

  // Initialization constructor
  TObserverItem(TObserverType Type_,TParamChangeEvent Event_,TGenericCockie Cockie_);
};

    // Type for list of specifc observers
    typedef std::vector<TObserverItem> TObserverList;

// Base class for the generic parameter class
class CQParameterBase : public CQPropertyObject
{
private:
   TObserverList m_ObserverList;

protected:
    int  m_ExposureLevel; // Exposure level of the parameter
    QString m_GroupName; // Group name of the parameter
    QString m_Description; // Optional description string
    bool m_CheckLimitsMode; // True indicates limits checking mode
    TParamAttributes m_Attributes; // Extended attribute set
    bool m_ObserversEnabled;  // If set to true, observers notification is enabled    
    static int NextAvailibleObserverType;
    std::stack<QString> m_ValuesStack;

public:    
    static int GetNextAvailibleObserverType();
    CQParameterBase(const CQParameterBase &QParameterBase);
    CQParameterBase(const QString& Name,const QString& GroupName,int ExposureLevel, bool CheckLimitsMode = false);
    TQPropertyKind GetPropertyKind(void);
    QString GroupName(void); // Return the section name of the param
    int ExposureLevel(void); // Return the exposure level of the param
    QString Description(void); // Return the description string
    bool GetLimitsMode(void); // Return true if limits checking is enabled
    void SetLimitsMode(bool); 

    void SetAttributes(TParamAttributes Attributes); // Set extended attributes
    TParamAttributes GetAttributes(void); // Get current attribute mask
    void AddAttribute(TParamAttributes Attribute); // Add a specific attribute to the attributes mask
    void RemoveAttribute(TParamAttributes Attribute); // Remove a specific attribute from the attributes mask
    void SetDescription(const QString& Description); // Set new description string

    // Add an event to the parameter generic observers list
	void AddObserver(TParamChangeEvent Event, TGenericCockie Cockie, TObserverType Type);
    void AddGenericObserver(TParamChangeEvent Event,TGenericCockie Cockie = 0);
	void AddSaveObserver(TParamChangeEvent Event,TGenericCockie Cockie = 0);
    void UnregisterObserver(TParamChangeEvent Event, TObserverType Type);
    void UnregisterGenericObserver(TParamChangeEvent Event);
    void UnregisterSaveObserver(TParamChangeEvent Event);
    void EnableObservers(void);
    void DisableObservers(void);

    // Pure virtual functions
    // Conversion functions from internal data to string
    virtual QString DefaultValueAsString(void) = 0;
    virtual QString MinLimitAsString(void) = 0;
    virtual QString MaxLimitAsString(void) = 0;
    virtual QString LimitRangeAsString(void);


    virtual void RestoreDefault(void) = 0; // Restore the parameter default value
    virtual void Push(); // Insert the current value to the stack
    virtual void Pop(); // Remove the top item from the stack and set it to be the current value
    virtual bool IsStackEmpty(void);
    virtual void CollapseStack(void); // Collapse the parameters stack saving the current value
    virtual void ResetStack(void); // Collapse the parameters stack and restore the original value
    virtual QString OriginalValueAsString(); // Get the value at the bottom of the stack
    virtual void DuringSaveEvent(void);

    // Call all register observer callbacks
    void NotifyObservers(TObserverType Type);
    void NotifySaveObservers(void);	
    void NotifyGenericObservers(void);
    virtual QString ValueAsString(void) = 0;
};

#define DEFINE_ASSIGNMENT_OPERATOR(_op_) inline CQParameter<TParameterValue> & operator _op_ (const CQParameter<TParameterValue> & Other) \
                                         {return (*this _op_ Other.Value());} \
                                         inline CQParameter<TParameterValue> & operator _op_ (const TParameterValue & Other) \
                                         {TParameterValue Value = m_Value; AssignValue(Value _op_ Other);return *this;}

#define DEFINE_0PARAM_OPERATOR(_op_,_retType_)     inline _retType_ operator _op_ () {return _op_(m_Value);}

#define DEFINE_BOOL_RET_PARAM_OPERATOR(_op_,_Type1_) \
                            template <class _Type1_,class _Type2_> \
                            bool operator _op_ (const CQParameter<_Type1_> & P1,const CQParameter<_Type2_> & P2) \
                            {return (P1.Value() _op_ P2.Value());} \
                            \
                            template <class _Type1_,class _Type2_> \
                            bool operator _op_ (const CQParameter<_Type1_> & P,const _Type2_ & V) \
                            {_Type2_ tmp(V); return (P.Value() _op_ (_Type1_)tmp);} \
                            \
                            template <class _Type1_,class _Type2_> \
                            bool operator _op_ (const _Type2_ & V,const CQParameter<_Type1_> & P) \
                            {_Type2_ tmp(V); return ((_Type1_)tmp _op_ P.Value());}
                                                                                   
#define DEFINE_1PARAM_OPERATOR(_op_,_Type1_,_retType_) \
                            template <class _Type1_,class _Type2_> \
                            _retType_ operator _op_ (const CQParameter<_Type1_> & P1,const CQParameter<_Type2_> & P2) \
                            {return (P1.Value() _op_ P2.Value());} /*\
                            \
                            template <class _Type1_,class _Type2_> \
                            _retType_ operator _op_ (const CQParameter<_Type1_> & P,const _Type2_ & V) \
                            {_Type2_ tmp(V); return (P.Value() _op_ (_Type1_)tmp);} \
                            \
                            template <class _Type1_,class _Type2_> \
                            _retType_ operator _op_ (const _Type2_ & V,const CQParameter<_Type1_> & P) \
                            {_Type2_ tmp(V); return ((_Type1_)tmp _op_ P.Value());}*/

// Generic parameter class
template <class TParameterValue>
class CQParameter : public CQParameterBase
{
private:

    // Parameter values (template type)
    TParameterValue m_Value;
    TParameterValue m_DefaultValue;
    TParameterValue m_MinValue;
    TParameterValue m_MaxValue;

    // This variable is required for the notification system - notifications are
    // triggered only if the parameter has changed since last assignment.
    TParameterValue m_OldValue;

    // Return true if the value is in the min/max limits (return false if limit
    // error).
    bool CheckLimits(const TParameterValue& Value);

protected:
    QString ConvertValueToString(const TParameterValue& v); // Utility function for converting a value from type TParameterValue to a string
    // Assign a new value to the parameter
    void AssignValue(const TParameterValue& NewValue);

public:
    CQParameter(const QString& Name,const TParameterValue& InitialValue = TParameterValue(),const QString& GroupName="", int ExposureLevel = RD_LEVEL): CQParameterBase(Name,GroupName,ExposureLevel)
    {
       // Initialize the minimum and maximum values
       m_MinValue = 0;
       m_MaxValue = 0;
       // Set initial (default) values
       m_DefaultValue = m_Value = InitialValue;
    }

    CQParameter(void) : CQParameterBase("","",RD_LEVEL){}
    CQParameter(const CQParameter &QParameter) : CQParameterBase(QParameter)
    {
       m_Value = QParameter.m_Value;
       m_DefaultValue = QParameter.m_DefaultValue;
       m_MinValue = QParameter.m_MinValue;
       m_MaxValue = QParameter.m_MaxValue;
       m_OldValue = QParameter.m_OldValue;
    }
   
    // Return a pointer to the base class (this is required because we overloaded
    // the & operator so we can not take the address of a parameter directly)
    virtual TPropertyTypeID GetTypeID(void); // Get the type signature
    void UnregisterSaveObserver(TParamChangeEvent Event); // Remove an existing event from the parameter observers list

    // Set the parameter properties
    void Init(const QString& Name,const TParameterValue& InitialValue,const QString& GroupName, int ExposureLevel,
                       bool CheckLimitsMode,const TParameterValue& MinValue,const TParameterValue& MaxValue);
    // Conversion operators to make this class look like variable from type TParameterValue

    /* http://www.cplusplus.com/doc/tutorial/classes2 */
    /* a@b :	= += -= *= /= %= ^= &= |= <<= >>= */
    //Assignment
    DEFINE_ASSIGNMENT_OPERATOR(=);
    //Compound assignment (+=, -=, *=, /=, %=, >>=, <<=, &=, ^=, |=)
    DEFINE_ASSIGNMENT_OPERATOR(+=);
    DEFINE_ASSIGNMENT_OPERATOR(-=);
    DEFINE_ASSIGNMENT_OPERATOR(*=);
    DEFINE_ASSIGNMENT_OPERATOR(/=);
    DEFINE_ASSIGNMENT_OPERATOR(%=);
    DEFINE_ASSIGNMENT_OPERATOR(<<=);
    DEFINE_ASSIGNMENT_OPERATOR(>>=);
    DEFINE_ASSIGNMENT_OPERATOR(&=);
    DEFINE_ASSIGNMENT_OPERATOR(^=);
    DEFINE_ASSIGNMENT_OPERATOR(|=);

    inline operator TParameterValue & ()        {      return m_Value;    }
    // Explicit "get value" access function
    inline TParameterValue Value(void) const    {      return m_Value;    }
    const char* c_str ( ) const                 {      return m_Value.c_str();    }

    inline const TParameterValue & DefaultValue(void);
    inline const TParameterValue & MinLimit(void);
    inline const TParameterValue & MaxLimit(void);
    void SetMaxLimit(TParameterValue MaxValue);
    void SetMinLimit(TParameterValue MinValue);
    void RestoreDefault(void);
    void AssignFromString(const QString& s);
    QString ValueAsString(void);
    QString DefaultValueAsString(void);
    QString MinLimitAsString(void);
    QString MaxLimitAsString(void);

    DEFINE_0PARAM_OPERATOR(!,bool);
    DEFINE_0PARAM_OPERATOR(~,TParameterValue);
};

/* a@b :	+ - * / % ^ & | < > == != <= >= << >> && ||  */
//Arithmetic operators ( +, -, *, /, % )
DEFINE_1PARAM_OPERATOR(+,TParameterValue,TParameterValue);
DEFINE_1PARAM_OPERATOR(-,TParameterValue,TParameterValue);
DEFINE_1PARAM_OPERATOR(*,TParameterValue,TParameterValue);
DEFINE_1PARAM_OPERATOR(/,TParameterValue,TParameterValue);
DEFINE_1PARAM_OPERATOR(%,TParameterValue,TParameterValue);

template<class Type>
QString operator+(CQParameter<QString> & P,const Type & V)
{
  Type tmp(V);
  return (P.Value() + (QString)tmp);
}

template<class Type>
QString operator+(const Type & V,CQParameter<QString> & P)
{
  Type tmp(V);
  return ((QString)tmp + P.Value());
}

//Relational and equality operators ( ==, !=, >, <, >=, <= )
DEFINE_BOOL_RET_PARAM_OPERATOR(==,TParameterValue);
DEFINE_BOOL_RET_PARAM_OPERATOR(!=,TParameterValue);
DEFINE_BOOL_RET_PARAM_OPERATOR(>,TParameterValue);
DEFINE_BOOL_RET_PARAM_OPERATOR(<,TParameterValue);
DEFINE_BOOL_RET_PARAM_OPERATOR(>=,TParameterValue);
DEFINE_BOOL_RET_PARAM_OPERATOR(<=,TParameterValue);
//Logical operators ( !, &&, || )
DEFINE_BOOL_RET_PARAM_OPERATOR(&&,TParameterValue);
DEFINE_BOOL_RET_PARAM_OPERATOR(||,TParameterValue);
//Bitwise Operators ( &, |, ^, ~, <<, >> )
DEFINE_1PARAM_OPERATOR(|,TParameterValue,TParameterValue);
DEFINE_1PARAM_OPERATOR(^,TParameterValue,TParameterValue);
DEFINE_1PARAM_OPERATOR(<<,TParameterValue,TParameterValue);
DEFINE_1PARAM_OPERATOR(>>,TParameterValue,TParameterValue);
DEFINE_1PARAM_OPERATOR(&,TParameterValue,TParameterValue);

template <class TParameterValue>
bool CQParameter<TParameterValue>::CheckLimits(const TParameterValue& Value)
{
  if(m_CheckLimitsMode)
    return !((Value < m_MinValue) || (Value > m_MaxValue));

  // If the limits checking mode is off, always allow value assignment
  return true;
}

template <class TParameterValue>
QString CQParameter<TParameterValue>::ConvertValueToString(const TParameterValue& v) // Utility function for converting a value from type TParameterValue to a string
{
  return QValueToStr(v); // return as QString
}

template <class TParameterValue>
void CQParameter<TParameterValue>::AssignValue(const TParameterValue& NewValue)
{
   if (!CheckLimits(NewValue)) // Check the value agains the limits
   { // The value is outside the limits range
     // If limits clipping is enabled, clip the value and assign to the value member
     if (m_Attributes & paLimitsClip)
     {
        m_OldValue = m_Value; // Save old value
        if (NewValue > m_MaxValue) // Clip to maximum
            m_Value = m_MaxValue;
        else if(NewValue < m_MinValue) // Clip to minimum
            m_Value = m_MinValue;
     }
     else if(m_Attributes & paLimitsException)  // If exception throwing is enabled raise an exception
        throw EQParameterError("Parameter: " + Name() + " is out of range (Value = " +
                               ConvertValueToString(NewValue) +
                               ", Min = " + MinLimitAsString() + " ,Max = " + MaxLimitAsString() + ")");
   }
   else
   {
     m_OldValue = m_Value; // Save old value
     m_Value = NewValue; // Assign normally
   }
   // If the force observers update flag is set, do not check for parameter value change
   if ((m_Attributes & paForceObserversUpdate) || (m_Value != m_OldValue))
      NotifyGenericObservers();
}

template <class TParameterValue>
TPropertyTypeID CQParameter<TParameterValue>::GetTypeID(void)
{
   return TypeIDSelector<TParameterValue>();
}

template <class TParameterValue>
// Remove an existing event from the parameter observers list
void CQParameter<TParameterValue>::UnregisterSaveObserver(TParamChangeEvent Event)
{
   TObserverList::iterator i;
   for(i = m_ObserverList.begin(); i != m_ObserverList.end(); i++)
     if((*i).Event == Event)
        break;
   if(i != m_ObserverList.end())
      m_ObserverList.erase(i);
}

template <class TParameterValue>
    // Set the parameter properties
void CQParameter<TParameterValue>::Init(const QString& Name,const TParameterValue& InitialValue,const QString& GroupName, int ExposureLevel,
                                        bool CheckLimitsMode,const TParameterValue& MinValue,const TParameterValue& MaxValue)
{
   InitProperty(Name);
   m_GroupName = GroupName;
   m_ExposureLevel = ExposureLevel;
   // Set initial (and default) values
   m_DefaultValue = m_Value = InitialValue;
   m_CheckLimitsMode = CheckLimitsMode;
   // If we are not in limits mode, reset the range variables to maximum/minimum
   if(m_CheckLimitsMode)
   {  // Remember limit values
      m_MinValue = MinValue;
      m_MaxValue = MaxValue;
   }
}

template <class TParameterValue>
inline const TParameterValue & CQParameter<TParameterValue>::DefaultValue(void)
{
   return m_DefaultValue;
}
	
template <class TParameterValue>
inline const TParameterValue & CQParameter<TParameterValue>::MinLimit(void)
{
   return m_MinValue;
}

template <class TParameterValue>
inline const TParameterValue & CQParameter<TParameterValue>::MaxLimit(void)
{
   return m_MaxValue;
}

template <class TParameterValue>
void CQParameter<TParameterValue>::SetMaxLimit(TParameterValue MaxValue)
{
   m_MaxValue = MaxValue;
   AssignValue(m_Value);
}

template <class TParameterValue>
void CQParameter<TParameterValue>::SetMinLimit(TParameterValue MinValue)
{
   m_MinValue = MinValue;
   AssignValue(m_Value);
}

template <class TParameterValue>
void CQParameter<TParameterValue>::RestoreDefault(void)
{
   m_Value = m_DefaultValue;
}

template <class TParameterValue>
    // Get the parameter value from a string
void CQParameter<TParameterValue>::AssignFromString(const QString& s)
{
   TParameterValue TmpValue;
   TmpValue = QStrToValue<TParameterValue>(s);
   AssignValue(TmpValue);
}

template <class TParameterValue>
QString CQParameter<TParameterValue>::ValueAsString(void)
{
   return ConvertValueToString(m_Value);
}

template <class TParameterValue>
QString CQParameter<TParameterValue>::DefaultValueAsString(void)
{
   return ConvertValueToString(m_DefaultValue);
}

template <class TParameterValue>
QString CQParameter<TParameterValue>::MinLimitAsString(void)
{
   return ConvertValueToString(m_MinValue);
}

template <class TParameterValue>
QString CQParameter<TParameterValue>::MaxLimitAsString(void)
{
   return ConvertValueToString(m_MaxValue);
}

template<>
QString CQParameter<QString>::ConvertValueToString(const QString& v)
{
  return v;
}

template<>
void CQParameter<QString>::AssignFromString(const QString& s)
{
  AssignValue(s);
}

// Array parameter base class
class CQArrayParameterBase : public CQParameterBase
{
  public:
    // Constructor
    CQArrayParameterBase(const QString& Name,const QString& GroupName, int ExposureLevel)
      : CQParameterBase(Name,GroupName,ExposureLevel) {}

    // Set description for a specific cell
    virtual void SetCellDescription(int CellIndex,const QString& Description) = 0;

    // Get the description of a specific cell
    virtual QString GetCellDescription(int CellIndex) = 0;
};

// Array parameter class


template<class TParameterValue,int ArraySize>
struct Typedef
{
  typedef CQParameter<TParameterValue>TParameterValueArray [ArraySize] ;
  typedef TParameterValue TValueArray [ArraySize] ;
};

#define DEFINE_ARRAY_ASSIGNMENT_OPERATOR(_op_) \
         inline CQArrayParameter<TArrayType,ArraySize> & operator _op_ (const CQArrayParameter<TArrayType,ArraySize> & Other) \
         {for(int i=0;i < ArraySize; i++) m_ArrayData[i] _op_ Other[i]; return *this;} \
         inline CQArrayParameter<TArrayType,ArraySize> & operator _op_ (Typedef<TArrayType,ArraySize>::TParameterValueArray & Other) \
         {for(int i=0;i < ArraySize; i++) m_ArrayData[i] _op_ Other[i]; return *this;} \
         inline CQArrayParameter<TArrayType,ArraySize> & operator _op_ (Typedef<TArrayType,ArraySize>::TValueArray & Other) \
         {for(int i=0;i < ArraySize; i++) m_ArrayData[i] _op_ Other[i]; return *this;}

template <class TArrayType,int ArraySize>
class CQArrayParameter : public CQArrayParameterBase
{
private:
    CQParameter<TArrayType> m_ArrayData[ArraySize];
    QString CellDescription[ArraySize]; // Description strings for each cell
    // Return true if the value is in the min/max limits (return false if limit error).
    bool CheckLimits(const TArrayType *ValueToCompare);
    // Utility function for converting a value from the array type to a string
    QString ConvertValueToString(const TArrayType *Value);

public:
    static void ChangeObserver(CQParameterBase *Param,TGenericCockie Cockie);
    CQArrayParameter(const QString& Name,const QString& InitialValue="",const QString& GroupName="", int ExposureLevel = RD_LEVEL)
      : CQArrayParameterBase(Name,GroupName, ExposureLevel)
    {
       Init(Name,InitialValue,GroupName,ExposureLevel,false,InitialValue,InitialValue);
    }//Constructor 1

    CQArrayParameter(void) : CQArrayParameterBase("","",RD_LEVEL) {}
    void StaticInit(const QString& Name,const TArrayType& InitialValue,const QString& GroupName, int ExposureLevel,
                       bool CheckLimitsMode,const TArrayType& MinValue,const TArrayType& MaxValue);
    void Init(const QString& Name,const QString& InitialValue,const QString& GroupName, int ExposureLevel,
                       bool CheckLimitsMode,const QString& MinValue,const QString& MaxValue);
    inline TArrayType& GetValueAt(int i);
    inline CQParameter<TArrayType>& GetParamAt(int i);
    inline CQParameter<TArrayType>& operator[](int i);
    inline int Size(void);
    DEFINE_ARRAY_ASSIGNMENT_OPERATOR(=);
    //Compound assignment (+=, -=, *=, /=, %=, >>=, <<=, &=, ^=, |=)
    DEFINE_ARRAY_ASSIGNMENT_OPERATOR(+=);
    DEFINE_ARRAY_ASSIGNMENT_OPERATOR(-=);
    DEFINE_ARRAY_ASSIGNMENT_OPERATOR(*=);
    DEFINE_ARRAY_ASSIGNMENT_OPERATOR(/=);
    DEFINE_ARRAY_ASSIGNMENT_OPERATOR(%=);
    DEFINE_ARRAY_ASSIGNMENT_OPERATOR(<<=);
    DEFINE_ARRAY_ASSIGNMENT_OPERATOR(>>=);
    DEFINE_ARRAY_ASSIGNMENT_OPERATOR(&=);
    DEFINE_ARRAY_ASSIGNMENT_OPERATOR(^=);
    DEFINE_ARRAY_ASSIGNMENT_OPERATOR(|=);

    void SetAttributes(TParamAttributes Attributes); // Set extended attributes
    void AddAttribute(TParamAttributes Attribute); // Add a specific attribute to the attributes mask
    void RemoveAttribute(TParamAttributes Attribute); // Remove a specific attribute from the attributes mask
    
    void RestoreDefault(void);
    void AssignFromString(const QString& s);
    QString ValueAsString(void);
    inline TArrayType DefaultValue(int i);
    QString DefaultValueAsString(void);
    QString MinLimitAsString(void);
    QString MaxLimitAsString(void);
    QString LimitRangeAsString(void);
    TPropertyTypeID GetTypeID(void);
    void SetCellDescription(int CellIndex,const QString& Description);
    QString GetCellDescription(int CellIndex);
};

template <class TArrayType,int ArraySize>
void CQArrayParameter<TArrayType,ArraySize>::SetAttributes(TParamAttributes Attributes)
{
  for(int i = 0; i < ArraySize; i++)
     m_ArrayData[i].SetAttributes(Attributes);
  CQArrayParameterBase::SetAttributes(Attributes);
}

template <class TArrayType,int ArraySize>
void CQArrayParameter<TArrayType,ArraySize>::AddAttribute(TParamAttributes Attribute)
{
  for(int i = 0; i < ArraySize; i++)
     m_ArrayData[i].AddAttribute(Attribute);
  CQArrayParameterBase::AddAttribute(Attribute);
}

template <class TArrayType,int ArraySize>
void CQArrayParameter<TArrayType,ArraySize>::RemoveAttribute(TParamAttributes Attribute)
{
  for(int i = 0; i < ArraySize; i++)
     m_ArrayData[i].RemoveAttribute(Attribute);
  CQArrayParameterBase::RemoveAttribute(Attribute);
}

template <class TArrayType,int ArraySize>
bool CQArrayParameter<TArrayType,ArraySize>::CheckLimits(const TArrayType *ValueToCompare)
{
  bool ret = true;
  for(int i = 0; i < ArraySize; i++)
     ret &= m_ArrayData[i].CheckLimits(ValueToCompare[i]);
  return ret;
}

template <class TArrayType,int ArraySize>
QString CQArrayParameter<TArrayType,ArraySize>::ConvertValueToString(const TArrayType *Value)
{
  QString Str;
  int     i = 0;

  for(; i < ArraySize - 1; i++)
      Str += QValueToStr<TArrayType>(Value[i]) + ',';
  Str += QValueToStr<TArrayType>(Value[i]);
  return Str;
}

/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
template <class TArrayType,int ArraySize>
void CQArrayParameter<TArrayType,ArraySize>::ChangeObserver(CQParameterBase *Param,TGenericCockie Cockie)
{
  CQArrayParameter *Instance = reinterpret_cast<CQArrayParameter*>(Cockie);
  Instance->NotifyGenericObservers();
}
/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
template <class TArrayType,int ArraySize>
void CQArrayParameter<TArrayType,ArraySize>::StaticInit(const QString& Name,const TArrayType& InitialValue,
                                                        const QString& GroupName, int ExposureLevel,
                                                        bool CheckLimitsMode,const TArrayType& MinValue,const TArrayType& MaxValue)
{
    TQStringVector InitialValueTokens(ArraySize),MinValueTokens(ArraySize),MaxValueTokens(ArraySize);

    QString InitialValueStr = QValueToStr<TArrayType>(InitialValue);
    QString MinValueStr     = QValueToStr<TArrayType>(MinValue);
    QString MaxValueStr     = QValueToStr<TArrayType>(MaxValue);

    for(int i = 0; i < ArraySize; i++)
    {
       InitialValueTokens[i] = InitialValueStr;
       MinValueTokens[i]     = MinValueStr;
       MaxValueTokens[i]     = MaxValueStr;
    }
    JoinTokens(InitialValueStr, InitialValueTokens);
    JoinTokens(MinValueStr, MinValueTokens);
    JoinTokens(MaxValueStr, MaxValueTokens);
    Init(Name,InitialValueStr,GroupName,ExposureLevel,CheckLimitsMode,MinValueStr,MaxValueStr);
}

template <class TArrayType,int ArraySize>
void CQArrayParameter<TArrayType,ArraySize>::Init(const QString& Name,const QString& InitialValue,const QString& GroupName,
                                    int ExposureLevel,bool CheckLimitsMode,const QString& MinValue,const QString& MaxValue)
{
    InitProperty(Name);
    m_GroupName = GroupName;
    m_ExposureLevel = ExposureLevel;
    m_CheckLimitsMode = CheckLimitsMode;
    TQStringVector InitialValueTokens,MinValueTokens,MaxValueTokens;
    
    Tokenize(InitialValue, InitialValueTokens, APP_PARAMS_DELIMETER, true);
    Tokenize(MinValue, MinValueTokens, APP_PARAMS_DELIMETER, true);
    Tokenize(MaxValue, MaxValueTokens, APP_PARAMS_DELIMETER, true);
    
    for(int i = 0; i < ArraySize; i++)
    {
       m_ArrayData[i].Init(Name+QIntToStr(i),
                           QStrToValue<TArrayType>(InitialValueTokens[i]),
                           GroupName,
                           ExposureLevel,
                           CheckLimitsMode,
                           QStrToValue<TArrayType>(MinValueTokens[i]),
                           QStrToValue<TArrayType>(MaxValueTokens[i]));
       m_ArrayData[i].AddGenericObserver(ChangeObserver,reinterpret_cast<TGenericCockie>(this));
    }
}

template <class TArrayType,int ArraySize>
inline CQParameter<TArrayType>& CQArrayParameter<TArrayType,ArraySize>::GetParamAt(int i)
{
  return m_ArrayData[i];
}

template <class TArrayType,int ArraySize>
inline CQParameter<TArrayType>& CQArrayParameter<TArrayType,ArraySize>::operator[](int i)
{
  return m_ArrayData[i];
}

template <class TArrayType,int ArraySize>
inline int CQArrayParameter<TArrayType,ArraySize>::Size(void)
{
  return ArraySize;
}

template <class TArrayType,int ArraySize>
void CQArrayParameter<TArrayType,ArraySize>::RestoreDefault(void)
{
   for(int i = 0; i < ArraySize; i++)
      m_ArrayData[i].RestoreDefault();
}

template <class TArrayType,int ArraySize>
void CQArrayParameter<TArrayType,ArraySize>::AssignFromString(const QString& s)
{
   TQStringVector tokens;
   Tokenize(s, tokens, ",", true);
   if (tokens.size() != ArraySize)
      throw EQParameterError("Parameter: " + Name() + " can not assign value '" + s + "' while parameter array size is " + QIntToStr(ArraySize));
   for(int i = 0; i < ArraySize; i++)
       m_ArrayData[i].AssignFromString(tokens[i]);
}

template <class TArrayType,int ArraySize>
QString CQArrayParameter<TArrayType,ArraySize>::ValueAsString(void)
{
  TQStringVector tokens;
  tokens.resize (ArraySize);
  QString Value;
  for(int i = 0; i < ArraySize; i++)
     tokens[i] = m_ArrayData[i].ValueAsString();
  JoinTokens(Value, tokens);
  return Value;
}

template <class TArrayType,int ArraySize>
inline TArrayType CQArrayParameter<TArrayType,ArraySize>::DefaultValue(int i)
{
  return m_ArrayData[i].DefaultValue();
}

template <class TArrayType,int ArraySize>
QString CQArrayParameter<TArrayType,ArraySize>::DefaultValueAsString(void)
{
  TQStringVector tokens(ArraySize);
  QString Value;
  for(int i = 0; i < ArraySize; i++)
     tokens[i] = m_ArrayData[i].DefaultValueAsString();
  JoinTokens(Value, tokens);
  return Value;
}

template <class TArrayType,int ArraySize>
QString CQArrayParameter<TArrayType,ArraySize>::MinLimitAsString(void)
{
   TQStringVector tokens(ArraySize);
   QString Value;
   for(int i = 0; i < ArraySize; i++)
      tokens[i] = m_ArrayData[i].MinLimitAsString();
   JoinTokens(Value, tokens);
   return Value;
}

template <class TArrayType,int ArraySize>
QString CQArrayParameter<TArrayType,ArraySize>::MaxLimitAsString(void)
{
   TQStringVector tokens(ArraySize);
   QString Value;
   for(int i = 0; i < ArraySize; i++)
     tokens[i] = m_ArrayData[i].MaxLimitAsString();
   JoinTokens(Value, tokens);
   return Value;
}

template <class TArrayType,int ArraySize>
QString CQArrayParameter<TArrayType,ArraySize>::LimitRangeAsString(void)
{
   TQStringVector tokens(ArraySize);
   QString Value;
   for(int i = 0; i < ArraySize; i++)
     tokens[i] = m_ArrayData[i].LimitRangeAsString();
   JoinTokens(Value, tokens);
   return Value;
}

template <class TArrayType,int ArraySize>
TPropertyTypeID CQArrayParameter<TArrayType,ArraySize>::GetTypeID(void)
{
  return ptArray;
}

template <class TArrayType,int ArraySize>
void CQArrayParameter<TArrayType,ArraySize>::SetCellDescription(int CellIndex,const QString& Description)
{
  CellDescription[CellIndex] = Description;
}

template <class TArrayType,int ArraySize>
QString CQArrayParameter<TArrayType,ArraySize>::GetCellDescription(int CellIndex)
{
  if(CellIndex >= 0 && CellIndex < ArraySize)
    return CellDescription[CellIndex];
  return "";
}

#define DEFINE_EASSIGNMENT_OPERATOR(_op_) inline CQEnumParameter & operator _op_ (const int & Other) \
                                          {if (FindValue(Other) != -1) {int Value = this->Value(); AssignValue(Value _op_ Other);} return *this;} \
                                          inline CQEnumParameter & operator _op_ (const CQEnumParameter & Other) \
                                          {return (*this _op_ Other.Value());}
                                          
#define DEFINE_SPARAM_OPERATOR(_op_,_retType_,subClassType,TParameterValue) \
               _retType_ operator _op_ (const subClassType & P1, const subClassType & P2) \
               {CQParameter<TParameterValue> p1(P1); CQParameter<TParameterValue> p2(P2); return (p1 _op_ p2);}
                             
class CQEnumParameter : public CQParameter<int>
{
public:
    struct TEnumValue
    {
      QString Str;
      int Value;

      // Constructor
      TEnumValue(const QString& StrArg,const int ValueArg)
      {
        Str = StrArg;
        Value = ValueArg;
      }
    };

    typedef std::vector<TEnumValue> TEnumValueList;

private:
    TEnumValueList m_ValueList;

    // Search for a matching value in the values list (return an index to the item, or -1 if not found)
    int FindValue(const int Value);
    // Search for a matching value in the values list (return an index to the item, or -1 if not found)
    int FindStr(const QString& Str);

public:
    // Constructor 1
    CQEnumParameter(const QString& Name,const int InitialValue = 0,const QString SectionName="") :
      CQParameter<int>(Name,InitialValue,SectionName) {}

    CQEnumParameter(const CQEnumParameter &QEnumParameter) : CQParameter<int>(QEnumParameter)
    {
       m_ValueList = QEnumParameter.m_ValueList;
    }

    // Default constructor
    CQEnumParameter(void) {}

    /* http://www.cplusplus.com/doc/tutorial/classes2 */
    /* a@b :	= += -= *= /= %= ^= &= |= <<= >>= */
    //Assignment
    DEFINE_EASSIGNMENT_OPERATOR(=);
    //Compound assignment (+=, -=, *=, /=, %=, >>=, <<=, &=, ^=, |=)
    DEFINE_EASSIGNMENT_OPERATOR(+=);
    DEFINE_EASSIGNMENT_OPERATOR(-=);
    DEFINE_EASSIGNMENT_OPERATOR(*=);
    DEFINE_EASSIGNMENT_OPERATOR(/=);
    DEFINE_EASSIGNMENT_OPERATOR(%=);
    DEFINE_EASSIGNMENT_OPERATOR(<<=);
    DEFINE_EASSIGNMENT_OPERATOR(>>=);
    DEFINE_EASSIGNMENT_OPERATOR(&=);
    DEFINE_EASSIGNMENT_OPERATOR(^=);
    DEFINE_EASSIGNMENT_OPERATOR(|=);

    // Add a new possible value to the enumerated values list
    void AddValue(const QString& Str,const int Value);
    TPropertyTypeID GetTypeID(void);
    // Get a reference to the values list
    CQStringList GetValueStringList(void);
    // Return the best string representation of the current value
    QString GetBestValueStr(int Value);
    QString BestValueFromString(const QString& Str);
    // Return the string with the maximum length
    QString GetMaxLengthString(void);
};

/* a@b :	+ - * / % ^ & | < > == != <= >= << >> && ||  */
//Arithmetic operators ( +, -, *, /, % )
DEFINE_SPARAM_OPERATOR(+,int,CQEnumParameter,int);
DEFINE_SPARAM_OPERATOR(-,int,CQEnumParameter,int);
DEFINE_SPARAM_OPERATOR(*,int,CQEnumParameter,int);
DEFINE_SPARAM_OPERATOR(/,int,CQEnumParameter,int);
DEFINE_SPARAM_OPERATOR(%,int,CQEnumParameter,int);
//Relational and equality operators ( ==, !=, >, <, >=, <= )
DEFINE_SPARAM_OPERATOR(==,bool,CQEnumParameter,int);
DEFINE_SPARAM_OPERATOR(!=,bool,CQEnumParameter,int);
DEFINE_SPARAM_OPERATOR(>,bool,CQEnumParameter,int);
DEFINE_SPARAM_OPERATOR(<,bool,CQEnumParameter,int);
DEFINE_SPARAM_OPERATOR(>=,bool,CQEnumParameter,int);
DEFINE_SPARAM_OPERATOR(<=,bool,CQEnumParameter,int);
//Logical operators ( !, &&, || )
DEFINE_SPARAM_OPERATOR(&&,bool,CQEnumParameter,int);
DEFINE_SPARAM_OPERATOR(||,bool,CQEnumParameter,int);
//Bitwise Operators ( &, |, ^, ~, <<, >> )
DEFINE_SPARAM_OPERATOR(|,int,CQEnumParameter,int);
DEFINE_SPARAM_OPERATOR(^,int,CQEnumParameter,int);
DEFINE_SPARAM_OPERATOR(<<,int,CQEnumParameter,int);
DEFINE_SPARAM_OPERATOR(>>,int,CQEnumParameter,int);
DEFINE_SPARAM_OPERATOR(&,int,CQEnumParameter,int);

// Macro to define a parameter
#define DEFINE_PARAM(ParamType,ParamName) CQParameter<ParamType> ParamName
// Macro to define an array parameter
#define DEFINE_ARRAY_PARAM(ArrayType,ArraySize,ParamName) CQArrayParameter<ArrayType,ArraySize> ParamName
// Macro to define an enumerated parameter (int)
#define DEFINE_ENUM_PARAM(ParamName)  CQEnumParameter ParamName
// Macro which are using the exposure level
#ifdef VISUAL_PARAMETER_MANAGER

// Macro to initialize a parameter (inside a container)
#define INIT_PARAM(ParamName,InitialValue,GroupName,ExposureLevel) \
  ParamName.Init(#ParamName,InitialValue,GroupName,ExposureLevel,false,InitialValue,InitialValue);   \
  this->RegisterProperty(&ParamName)

// Macro to initialize a name parameter with limits checking (inside a container)
#define INIT_PARAM_WITH_LIMITS(ParamName,InitialValue,MinValue,MaxValue,GroupName,ExposureLevel) \
  ParamName.Init(#ParamName,InitialValue,GroupName,ExposureLevel,true,MinValue,MaxValue);   \
  this->RegisterProperty(&ParamName)

// Macro to initialize a name parameter with extended settings
#define INIT_PARAM_EXTENDED(ParamName,InitialValue,MinValue,MaxValue,EnableLimits,ParamNameStr,GroupName,ExposureLevel) \
  ParamName.Init(ParamNameStr,InitialValue,GroupName,ExposureLevel,EnableLimits,MinValue,MaxValue);   \
  this->RegisterProperty(&ParamName)

// Macro to initialize a parameter (inside a container)
#define INIT_PARAM_ARRAY_SAME_VALUE(ParamName,InitialValue,GroupName,ExposureLevel) \
  ParamName.StaticInit(#ParamName,InitialValue,GroupName,ExposureLevel,false,InitialValue,InitialValue);   \
  this->RegisterProperty(&ParamName)

// Macro to initialize a name parameter with limits checking (inside a container)
#define INIT_PARAM_ARRAY_SAME_VALUE_WITH_LIMITS(ParamName,InitialValue,MinValue,MaxValue,GroupName,ExposureLevel) \
  ParamName.StaticInit(#ParamName,InitialValue,GroupName,ExposureLevel,true,MinValue,MaxValue);   \
  this->RegisterProperty(&ParamName)

// Macro which are NOT using the exposure level (all kind of testers)
#else

// Macro to initialize a parameter (inside a container)
#define INIT_PARAM(ParamName,InitialValue,GroupName) \
  ParamName.Init(#ParamName,InitialValue,GroupName,RD_LEVEL,false,InitialValue,InitialValue);   \
  this->RegisterProperty(&ParamName)

// Macro to initialize a name parameter with limits checking (inside a container)
#define INIT_PARAM_WITH_LIMITS(ParamName,InitialValue,MinValue,MaxValue,GroupName) \
  ParamName.Init(#ParamName,InitialValue,GroupName,RD_LEVEL,true,MinValue,MaxValue);   \
  this->RegisterProperty(&ParamName)

// Macro to initialize a name parameter with extended settings
#define INIT_PARAM_EXTENDED(ParamName,InitialValue,MinValue,MaxValue,EnableLimits,ParamNameStr,GroupName) \
  ParamName.Init(ParamNameStr,InitialValue,GroupName,RD_LEVEL,EnableLimits,MinValue,MaxValue);   \
  this->RegisterProperty(&ParamName)

// Macro to initialize a parameter (inside a container)
#define INIT_PARAM_ARRAY_SAME_VALUE(ParamName,InitialValue,GroupName) \
  ParamName.StaticInit(#ParamName,InitialValue,GroupName,RD_LEVEL,false,InitialValue,InitialValue);   \
  this->RegisterProperty(&ParamName)

// Macro to initialize a name parameter with limits checking (inside a container)
#define INIT_PARAM_ARRAY_SAME_VALUE_WITH_LIMITS(ParamName,InitialValue,MinValue,MaxValue,GroupName) \
  ParamName.StaticInit(#ParamName,InitialValue,GroupName,RD_LEVEL,true,MinValue,MaxValue);   \
  this->RegisterProperty(&ParamName)

#endif

#endif


