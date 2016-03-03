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

#include "QParameter.h"

TObserverItem::TObserverItem(void)
{
  Event = NULL;
  Cockie = 0;
}

// Initialization constructor
TObserverItem::TObserverItem(TObserverType Type_,TParamChangeEvent Event_,TGenericCockie Cockie_)
{
  Type   = Type_;
  Event  = Event_;
  Cockie = Cockie_;
}

////////////////////////////////////////////////////////////////////////////////
//class CQParameterBase
////////////////////////////////////////////////////////////////////////////////
// Constructor
int CQParameterBase::NextAvailibleObserverType = Q_USER_OBSERVER;

int CQParameterBase::GetNextAvailibleObserverType()
{
  return NextAvailibleObserverType++;
}

CQParameterBase::CQParameterBase(const CQParameterBase &QParameterBase)
{
    m_ExposureLevel = QParameterBase.m_ExposureLevel;
    m_GroupName = QParameterBase.m_GroupName;
    m_Description = QParameterBase.m_Description;
    m_CheckLimitsMode = QParameterBase.m_CheckLimitsMode;
    m_Attributes = QParameterBase.m_Attributes;
}

CQParameterBase::CQParameterBase(const QString& Name,const QString& GroupName,int ExposureLevel, bool CheckLimitsMode)
{
  // Basic initialization
  InitProperty(Name);

  // Initialize internal variables
  m_GroupName        = GroupName;
  m_ExposureLevel    = ExposureLevel;
  m_CheckLimitsMode  = CheckLimitsMode;
  m_Attributes       = paDefault;
  m_ObserversEnabled = true;
}

// Return the kind of an RTTI object
TQPropertyKind CQParameterBase::GetPropertyKind(void)
{
  return pkParameter;
}

// Return the section name of the param
QString CQParameterBase::GroupName(void)
{
  return m_GroupName;
}

// Return the exposure level of the param
int CQParameterBase::ExposureLevel(void)
{
  return m_ExposureLevel;
}

// Return the description string
QString CQParameterBase::Description(void)
{
  return m_Description;
}

// Return true if limits checking is enabled
bool CQParameterBase::GetLimitsMode(void)
{
  return m_CheckLimitsMode;
}

void CQParameterBase::SetLimitsMode(bool CheckLimitsMode)
{
  m_CheckLimitsMode = CheckLimitsMode;
}

// Set extended attributes
void CQParameterBase::SetAttributes(TParamAttributes Attributes)
{
  m_Attributes = Attributes;
}

// Get current attribute mask
TParamAttributes CQParameterBase::GetAttributes(void)
{
  return m_Attributes;
}

// Add a specific attribute to the attributes mask
void CQParameterBase::AddAttribute(TParamAttributes Attribute)
{
  m_Attributes |= Attribute;
}

// Remove a specific attribute from the attributes mask
void CQParameterBase::RemoveAttribute(TParamAttributes Attribute)
{
  m_Attributes &= ~Attribute;
}

// Set new description string
void CQParameterBase::SetDescription(const QString& Description)
{
  m_Description = Description;
}

void CQParameterBase::NotifyObservers(TObserverType Type)
{
   for(TObserverList::iterator i = m_ObserverList.begin(); i != m_ObserverList.end(); ++i)
      if ((*i).Type == Type)
        (*i).Event(this,(*i).Cockie);
}

void CQParameterBase::NotifySaveObservers(void)
{
    NotifyObservers(Q_SAVE_OBSERVER);
}

void CQParameterBase::NotifyGenericObservers(void)
{
	NotifyObservers(Q_ENABLE_ALWAYS_OBSERVER);
    if(m_ObserversEnabled)
		NotifyObservers(Q_GENERIC_OBSERVER);
}

void CQParameterBase::AddObserver(TParamChangeEvent Event, TGenericCockie Cockie, TObserverType Type)
{
  m_ObserverList.push_back(TObserverItem(Type,Event,Cockie));
}

void CQParameterBase::AddSaveObserver(TParamChangeEvent Event,TGenericCockie Cockie)
{
  AddObserver(Event, Cockie, Q_SAVE_OBSERVER);
}

// Add an event to the parameter generic observers list
void CQParameterBase::AddGenericObserver(TParamChangeEvent Event,TGenericCockie Cockie)
{
  AddObserver(Event, Cockie, Q_GENERIC_OBSERVER);
}
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
void AddSpecificObserver(TParamChangeEvent Event,TGenericCockie Cockie = 0, TObserverType Type = Q_GENERIC_OBSERVER);    

// Remove an existing event from the parameter generic observers list
void CQParameterBase::UnregisterObserver(TParamChangeEvent Event, TObserverType Type)
{
  TObserverList::iterator i;

  for(i = m_ObserverList.begin(); i != m_ObserverList.end(); ++i)
    if((*i).Event == Event)
      break;

  if(i != m_ObserverList.end())
    m_ObserverList.erase(i);
}
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"

// Remove an existing event from the parameter generic observers list
void CQParameterBase::UnregisterGenericObserver(TParamChangeEvent Event)
{
  UnregisterObserver(Event, Q_GENERIC_OBSERVER);
}

void CQParameterBase::UnregisterSaveObserver(TParamChangeEvent Event)
{
  UnregisterObserver(Event, Q_SAVE_OBSERVER);
}

void CQParameterBase::EnableObservers(void)
{
  m_ObserversEnabled = true;
}

void CQParameterBase::DisableObservers(void)
{
  m_ObserversEnabled = false;
}

void CQParameterBase::DuringSaveEvent(void) {};

void CQParameterBase::Push()
{
   m_ValuesStack.push(ValueAsString());
}

void CQParameterBase::Pop()
{
   if(! IsStackEmpty())
   {
      QString Value = m_ValuesStack.top();
      m_ValuesStack.pop();
      AssignFromString(Value);
   }
}

bool CQParameterBase::IsStackEmpty(void)
{
   return m_ValuesStack.empty();
}

void CQParameterBase::CollapseStack(void)
{
   while(! IsStackEmpty())
     m_ValuesStack.pop();
}

void CQParameterBase::ResetStack(void)
{
   bool StackEmpty = IsStackEmpty();
   QString Value;
   while(! IsStackEmpty())
   {
      Value = m_ValuesStack.top();
      m_ValuesStack.pop();
   }
   if (!StackEmpty)
      AssignFromString(Value);
}

QString CQParameterBase::OriginalValueAsString()
{
  std::stack<QString> Stack;
  QString OriginalValue;
  Stack = m_ValuesStack;

  if (Stack.empty())
    return ValueAsString();

  while(! Stack.empty())
  {
    OriginalValue = Stack.top();
    Stack.pop();
  }
  return OriginalValue;
}        

QString CQParameterBase::LimitRangeAsString(void)
{
  return (MinLimitAsString() + " - " + MaxLimitAsString());
}

// Search for a matching value in the values list (return an index to the item, or -1 if not found)
int CQEnumParameter::FindValue(const int Value)
{
  for(int i = 0; i < (int)m_ValueList.size(); i++)
    if(m_ValueList[i].Value == Value)
      return i;

  return -1;
}

int CQEnumParameter::FindStr(const QString& Str)
{
  for(int i = 0; i < (int)m_ValueList.size(); i++)
    if(m_ValueList[i].Str == Str)
      return i;

  return -1;
}

void CQEnumParameter::AddValue(const QString& Str,const int Value)
{
  m_ValueList.push_back(TEnumValue(Str,Value));
}

TPropertyTypeID CQEnumParameter::GetTypeID(void)
{
  return ptEnum;
}

// Get a reference to the values list
CQStringList CQEnumParameter::GetValueStringList(void)
{
  CQStringList StringList;
  for(unsigned i = 0; i < m_ValueList.size(); i++)
     StringList.Add(m_ValueList[i].Str);
  return StringList;
}

// Return the best string representation of the current value
QString CQEnumParameter::GetBestValueStr(int Value)
{
  int i = FindValue(Value);

  if(i == -1)
    return ConvertValueToString(Value);

  return m_ValueList[i].Str;
}

QString CQEnumParameter::BestValueFromString(const QString& Str)
{
  int i = FindStr(Str);

  if(i == -1)
    return Str;

  return ConvertValueToString(m_ValueList[i].Value);
}

// Return the string with the maximum length
QString CQEnumParameter::GetMaxLengthString(void)
{
  int MaxLen = 0;
  int MaxLenStrIndex = -1;

  for(int i = 0; i < (int)m_ValueList.size(); i++)
    if((int)m_ValueList[i].Str.size() > MaxLen)
    {
      MaxLen = m_ValueList[i].Str.size();
      MaxLenStrIndex = i;
    }

  if(MaxLenStrIndex == -1)
    return "";

  return m_ValueList[MaxLenStrIndex].Str;
}

