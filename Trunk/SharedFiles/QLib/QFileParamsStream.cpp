/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib parameters stream for disk files.                   *
 * Module Description: Implement the base class CQParamsStream for  *
 *                     standard files I/O.                          *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 04/07/2001                                           *
 * Last upate: 09/09/2001                                           *
 ********************************************************************/

#include "QFileParamsStream.h"
#include "QUtils.h"
#include "QErrors.h"
#include "QFile.h"


// Define this compilation switch to raise exceptions on file errors
//#define FILE_ERROR_EXCEPTIONS



// Maximum number of characters in a configuration file
const int MAX_LINE_WIDTH = 500;

// Constructor
CQParamsFileStream::CQParamsFileStream(void)
{}

CQParamsFileStream::CQParamsFileStream(const QString FileName)
{
  Init(FileName);
}

CQParamsFileStream::CQParamsFileStream(const QString FileName, bool& InitOK)
{
  Init(FileName);
  InitOK = m_StatusInit;
}

void CQParamsFileStream::Init(const QString FileName)
{
  m_FileName = FileName;
  m_ParamsFileErrorCallback = NULL;
  m_ParamsFileErrorCockie = 0;

  if(FileName == "")
     return;

  try
  {
     m_StatusInit = LoadFileIntoStringMap(m_FileName,m_KeyToValue);
  }
  catch(...)
  {
     NotifyError(fpsOpenReadError); //fixme
     throw;
  }
}

// Destructor
CQParamsFileStream::~CQParamsFileStream(void)
{}

void CQParamsFileStream::LogParams(const QString FileName, CQParamsContainer *AttachedContainer)
{
  CQStringList ModeFile = LogParamsToStringList(AttachedContainer);
  ModeFile.SaveToFile(FileName); // Save to file
}

CQStringList CQParamsFileStream::LogParamsToStringList(CQParamsContainer *AttachedContainer)
{
  CQStringList ModeFile;
  CQParameterBase *Param;
  QString Line,Key,Value;
  // Prepare a string list
  for(TQStringMap::iterator Iter = m_KeyToValue.begin(); Iter != m_KeyToValue.end(); Iter++)
  {
     Key = Iter->first;
     Value = Iter->second;
     if (AttachedContainer)
     {
	// Search for a matching parameter in the container
  	Param = dynamic_cast<CQParameterBase *>(AttachedContainer->FindProperty(/*Key*/Iter->first));
    	if (Param) // Push the current parameter value and assign a new one
        Value = Param->ValueAsString();
     }
     if (!AttachedContainer || Param) // Push the current parameter value and assign a new one
     {
        Line = Key + "=" + Value;
        ModeFile.Add(Line);
     }
  }
  return ModeFile;
}

void CQParamsFileStream::Save(void)
{
  SaveFile(m_FileName);
}

// Write parameters and checksum to a given file
void CQParamsFileStream::SaveFile(const QString FileName)
{
   LogParams(FileName,NULL);
}

// Write a single parameter
void CQParamsFileStream::WriteParameter(CQParameterBase *Param)
{
  // If the parameter has the "no save" attribute, do nothing
  if(Param->GetAttributes() & paNoSave)
    return;
  m_KeyToValue[Param->Name()] = Param->OriginalValueAsString();
}

void CQParamsFileStream::SetCurrentValueToParameter(CQParameterBase *Param)
{
   SetValueToParameter(Param->Name(), Param->ValueAsString(), Param->GetAttributes());
}

void CQParamsFileStream::SetValueToParameter(QString Param, QString Value, TParamAttributes ParamAttributes)
{
  if(ParamAttributes & paNoSave)
    return;
  if( m_KeyToValue.find(Param) !=  m_KeyToValue.end() )
     m_KeyToValue[Param] = Value;
}

// Read a single parameter
void CQParamsFileStream::ReadParameter(CQParameterBase *Param)
{
  // If the parameter has the "no save" attribute, do nothing
  if(Param->GetAttributes() & paNoSave)
    return;

  try
  {
     TQStringMap::iterator Iter = m_KeyToValue.find( Param->Name() );
     if (Iter != m_KeyToValue.end() )
        if (Iter->second != "")
        {
           Param->AssignFromString(Iter->second);
           return;
        }
        else
  // We didn't find a match, go back to default value or an empty value
  Param->RestoreDefault();
  }
  catch(...) {}
}

// Utility function for loading a text file content into a string list structure
bool CQParamsFileStream::LoadFileIntoStringList(QString FileName,CQStringList& StrList)
{
   bool ret = false;
   try
   {
	 StrList.LoadFromFile(FileName);
     if(StrList.Count()>1) 
        ret = true; // fix 5955 E.E
   }
   catch(...)
   {
      // ret = false;
   }
   return ret;
}

bool CQParamsFileStream::LoadFileIntoStringMap(QString FileName,TQStringMap& KeyToValueMap)
{
   CQStringList StrList;
   bool ret = LoadFileIntoStringList(FileName,StrList);
   KeyToValueMap.clear();
   SplitToKeysAndValues(StrList,KeyToValueMap,StrList.Count()); // fix 5955 E.E
   return ret;
}

// Use the error callback to notify params stream errors
void CQParamsFileStream::NotifyError(TFileParamsStreamError Error)
{
  if(m_ParamsFileErrorCallback)
    (*m_ParamsFileErrorCallback)(Error,m_ParamsFileErrorCockie);
}

// Return true if the parameter can be found in the parameters stream
bool CQParamsFileStream::IsParamInStream(CQParameterBase *Param)
{
  // Try to match the current key to the requested parameter
  return ( m_KeyToValue.find(Param->Name()) !=  m_KeyToValue.end() );
}

void CQParamsFileStream::SetErrorsCallback(TParamsFileErrorCallback ParamsFileErrorCallback,
                                           TGenericCockie ParamsFileErrorCockie)
{
  m_ParamsFileErrorCallback = ParamsFileErrorCallback;
  m_ParamsFileErrorCockie = ParamsFileErrorCockie;
}

QString CQParamsFileStream::ReadRawParam(const QString ParamName)
{
  return m_KeyToValue[ParamName];
}

// Split a single string list into values and keys pairs
void CQParamsFileStream::SplitToKeysAndValues(const CQStringList& StrList,
                                              TQStringMap& KeyToValueMap,
                                              int LinesToProcess)
{
  KeyToValueMap.clear();
  QString Key,Value;
  for(int i = 0; i < LinesToProcess; i++)
  {
    // Break the line to key and value (ignore empty key lines)
    if(StrList.GetItem(i) != "")
      if(QBreakKeyAndValueStr(StrList.GetItem(i),Key,Value))
        KeyToValueMap[Key] = Value;
  }
}

TQStringMap CQParamsFileStream::GetKeyToValueMap()
{
   return m_KeyToValue;
}

