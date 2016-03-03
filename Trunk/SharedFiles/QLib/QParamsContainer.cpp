/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib paraneters container class.                         *
 * Module Description: This class is placeholder for parameters.    *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 04/07/2001                                           *
 * Last upate: 23/12/2002                                           *
 ********************************************************************/

#include "QParamsContainer.h"
#include "QUtils.h"
#include "QFileParamsStream.h"
#include "QErrors.h"



CQParamsStream::~CQParamsStream(){};

/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
void CQParamsStream::LogParams(const QString FileName, CQParamsContainer *AttachedContainer)
{}

void CQParamsStream::WriteOnlyParametersFromStream(CQParameterBase *Param, CQParamsStream *ImportStream)
{}

bool CQParamsStream::IsParamInStream(CQParameterBase *Param)
{return true;}


void CQParamsContainer::AfterSaveEvent (CQParameterBase* ParamToSave){}
/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/


QString CQParamsStream::ReadRawParam(const QString ParamName)
{return "";}

void CQParamsContainer::BeforeSaveEvent(void)
{}

CQParamsContainer::~CQParamsContainer(void)
{}

void CQParamsContainer::DuringSaveEvent(CQParameterBase* ParamToSave)
{
   ParamToSave->DuringSaveEvent();
}

// Constructor
CQParamsContainer::CQParamsContainer(CQParamsStream *ParamsStream,const QString Name,bool DoRegister) :
  CQComponent(Name,DoRegister)
{
  m_ParamsStream = ParamsStream;

  // Initialize methods
  INIT_METHOD(CQParamsContainer,LoadAll);
  INIT_METHOD(CQParamsContainer,RestoreDefaults);
  INIT_METHOD(CQParamsContainer,PushAll);
  INIT_METHOD(CQParamsContainer,PopAll);
  INIT_METHOD(CQParamsContainer,CollapseAll);
  INIT_METHOD(CQParamsContainer,ResetAll);
  INIT_METHOD(CQParamsContainer,ParamPush);
  INIT_METHOD(CQParamsContainer,ParamPop);
  INIT_METHOD(CQParamsContainer,ParamSave);
  INIT_METHOD(CQParamsContainer,ParamLoad);
  INIT_METHOD(CQParamsContainer,ParamRestoreDefault);
  INIT_METHOD(CQParamsContainer,ReadRawFromStream);
}

// Assign a new params stream to the container
void CQParamsContainer::AssignParamsStream(CQParamsStream *ParamsStream)
{
  m_ParamsStream = ParamsStream;
}



// Save a single parameter
void CQParamsContainer::SaveSingleParameter(CQParameterBase *ParamToSave,CQParamsStream *AlternativeParamsStream)
{
	CQParamsStream*       Stream;

	// If no alternative stream is assigned use the class stream member
	if(AlternativeParamsStream == NULL)
		Stream = m_ParamsStream;
	else
		// Use the alternative stream
		Stream = AlternativeParamsStream;

	// If no stream is assigned raise error
	if(Stream == NULL)
		throw EQParamsStreamError("No stream is assigned for parameter saving");

	try
    {
		// Write the parameter to the stream
   	    BeforeSaveEvent();
		DuringSaveEvent(ParamToSave);
		Stream->WriteParameter(ParamToSave);
		Stream->Save();
		AfterSaveEvent(ParamToSave);
        ParamToSave->NotifySaveObservers();
	}
	catch(EQException& Err)
	{
	  return;
	}
}

TQErrCode CQParamsContainer::LogParameters(QString FileName)
{
  // If no stream is assigned raise error
  if(m_ParamsStream == NULL)
    throw EQParamsStreamError("No stream is assigned for parameters saving");

  // Write all the parameter to the stream
  m_ParamsStream->LogParams(FileName, this);

  return Q_NO_ERROR;
}

// Save all parameters, version 1: use the class internal member as a stream
TQErrCode CQParamsContainer::SaveAll(void)
{
	// If no stream is assigned raise error
	if(m_ParamsStream == NULL)
		throw EQParamsStreamError("No stream is assigned for parameters saving");

	BeforeSaveEvent();
	CQParameterBase* Parameter;
	for(int i=0; i < PropertyCount(); i++)
	{       Parameter = dynamic_cast<CQParameterBase *>(Properties[i]) ;
		if( Parameter )
		{
			DuringSaveEvent(Parameter);
			m_ParamsStream->WriteParameter(Parameter);
            		Parameter->NotifySaveObservers();
		}
	}
	try
    {
		// Write all the parameter to the stream
		m_ParamsStream->Save();
	}
	catch(EQException& Err)
	{
	  return Q_OPEN_FILE_ERR;	
	}
	AfterSaveEvent();
	return Q_NO_ERROR;
}

// Save all parameters, version 1: use the class internal member as a stream
TQErrCode CQParamsContainer::SaveAllRestrictedToImportStream(CQParamsStream *ImportStream)
{
  // If no stream is assigned raise error
  if(m_ParamsStream == NULL)
    throw EQParamsStreamError("No stream is assigned for parameters saving");

  BeforeSaveEvent();

  CQParameterBase* Parameter;
  for(int i=0; i < PropertyCount(); i++)
  {
    Parameter = dynamic_cast<CQParameterBase *>(Properties[i]);
    if (Parameter)
    {
        DuringSaveEvent(Parameter);
        m_ParamsStream->WriteOnlyParametersFromStream(Parameter, ImportStream);
    }
  }
	try
    {
		// Write all the parameter to the stream
		m_ParamsStream->Save();
	}
	catch(EQException& Err)
	{
	  return Q_OPEN_FILE_ERR;
	}

  AfterSaveEvent();

  return Q_NO_ERROR;
}

// Save all parameters, version 2: use an alternative params stream
TQErrCode CQParamsContainer::SaveAll(CQParamsStream *AlternativeParamsStream)
{
  // If no stream is assigned raise error
  if(AlternativeParamsStream == NULL)
    throw EQParamsStreamError("No stream is assigned for parameters saving");
  BeforeSaveEvent();
  try
  {
     CQParameterBase* Parameter;
     for(int i=0; i < PropertyCount(); i++)
     {
        Parameter = dynamic_cast<CQParameterBase *>(Properties[i]);
        if ( Parameter)
        {
           DuringSaveEvent(Parameter);
           AlternativeParamsStream->WriteParameter(Parameter);
        }
     }
     AlternativeParamsStream->Save();
  }
  catch(EQException& Err)
  {
	 return Q_OPEN_FILE_ERR;
  }
  AfterSaveEvent();
  return Q_NO_ERROR;
}

// Load a single parameter
void CQParamsContainer::LoadSingleParameter(CQParameterBase *ParamToLoad,CQParamsStream *AlternativeParamsStream)
{
  CQParamsStream *Stream;

  // If no alternative stream is assigned use the class stream member
  if(AlternativeParamsStream == NULL)
    Stream = m_ParamsStream;
  else
    // Use the alternative stream
    Stream = AlternativeParamsStream;

  // If no stream is assigned raise error
  if(Stream == NULL)
    throw EQParamsStreamError("No stream is assigned for parameter loading");

  // Load the parameter from the stream
  Stream->ReadParameter(ParamToLoad);
}

// Load all parameters, version 1: use the class internal member as a stream
TQErrCode CQParamsContainer::LoadAll(void)
{
  // If no stream is assigned raise error
  if(m_ParamsStream == NULL)
    throw EQParamsStreamError("No stream is assigned for parameters saving");

  for(int i=0; i < PropertyCount(); i++)
    m_ParamsStream->ReadParameter(dynamic_cast<CQParameterBase *>(Properties[i]));
  return Q_NO_ERROR;
}

// Load all parameters, version 2: use an alternative params stream
TQErrCode CQParamsContainer::LoadAll(CQParamsStream *AlternativeParamsStream)
{
  // If no stream is assigned raise error
  if(AlternativeParamsStream == NULL)
    throw EQParamsStreamError("No stream is assigned for parameters saving");

  for(int i=0; i < PropertyCount(); i++)
    AlternativeParamsStream->ReadParameter(dynamic_cast<CQParameterBase *>(Properties[i]));
  return Q_NO_ERROR;
}

// Restore all the parameters do their defaults
TQErrCode CQParamsContainer::RestoreDefaults(void)
{
  for(int i=0; i < PropertyCount(); i++)
    dynamic_cast<CQParameterBase *>(Properties[i])->RestoreDefault();

  return Q_NO_ERROR;
}

// Push all parameters (each parameter has its own stack)
TQErrCode CQParamsContainer::PushAll(void)
{
  for(int i=0; i < PropertyCount(); i++)
  {
    CQParameterBase *Param = dynamic_cast<CQParameterBase *>(Properties[i]);
    Param->Push();
  }

  return Q_NO_ERROR;
}

// Pop all parameters
TQErrCode CQParamsContainer::PopAll(void)
{
  for(int i=0; i < PropertyCount(); i++)
  {
    CQParameterBase *Param = dynamic_cast<CQParameterBase *>(Properties[i]);
    Param->Pop();
  }

  return Q_NO_ERROR;
}

// Collapse the stack of all parameters
TQErrCode CQParamsContainer::CollapseAll(void)
{
  for(int i=0; i < PropertyCount(); i++)
  {
    CQParameterBase *Param = dynamic_cast<CQParameterBase *>(Properties[i]);
    Param->CollapseStack();
  }

  return Q_NO_ERROR;
}

// Reset the stack of all parameters
TQErrCode CQParamsContainer::ResetAll(void)
{
  for(int i=0; i < PropertyCount(); i++)
  {
    CQParameterBase *Param = dynamic_cast<CQParameterBase *>(Properties[i]);
    Param->ResetStack();
  }

  return Q_NO_ERROR;
}

// Push a specific parameter to its stack
TQErrCode CQParamsContainer::ParamPush(QString ParamName)
{
  CQPropertyObject *Prop = FindProperty(ParamName);

  if(Prop == NULL)
    return Q_PARAM_NOT_FOUND_ERR;

  dynamic_cast<CQParameterBase *>(Prop)->Push();
  return Q_NO_ERROR;
}

// Pop a specific parameter from its stack
TQErrCode CQParamsContainer::ParamPop(QString ParamName)
{
  CQPropertyObject *Prop = FindProperty(ParamName);

  if(Prop == NULL)
    return Q_PARAM_NOT_FOUND_ERR;

  dynamic_cast<CQParameterBase *>(Prop)->Pop();
  return Q_NO_ERROR;
}

// Save a specific parameter
TQErrCode CQParamsContainer::ParamSave(QString ParamName)
{
  CQPropertyObject *Prop = FindProperty(ParamName);

  if(Prop == NULL)
    return Q_PARAM_NOT_FOUND_ERR;

  SaveSingleParameter(dynamic_cast<CQParameterBase *>(Prop));
  return Q_NO_ERROR;
}

// Load a specific parameter
TQErrCode CQParamsContainer::ParamLoad(QString ParamName)
{
  CQPropertyObject *Prop = FindProperty(ParamName);

  if(Prop == NULL)
    return Q_PARAM_NOT_FOUND_ERR;

  LoadSingleParameter(dynamic_cast<CQParameterBase *>(Prop));
  return Q_NO_ERROR;
}

// Restore a single parameter to default
TQErrCode CQParamsContainer::ParamRestoreDefault(QString ParamName)
{
  CQPropertyObject *Prop = FindProperty(ParamName);

  if(Prop == NULL)
    return Q_PARAM_NOT_FOUND_ERR;

  dynamic_cast<CQParameterBase *>(Prop)->RestoreDefault();
  return Q_NO_ERROR;
}

// Read raw parameter value from the associated stream
QString CQParamsContainer::ReadRawFromStream(QString ParamName)
{
  return m_ParamsStream->ReadRawParam(ParamName);
}

// Import a parameters collection
void CQParamsContainer::Import(CQParamsStream *ImportStream,bool PushInStack)
{
  for(int i=0; i < PropertyCount(); i++)
  {
    CQParameterBase *CurrentParam = dynamic_cast<CQParameterBase *>(Properties[i]);

    // Check if a specific parameter is in the import stream
    if(ImportStream->IsParamInStream(CurrentParam))
    {
      // Push current value
      if(PushInStack)
        CurrentParam->Push();

      // Get new value from the import stream
      ImportStream->ReadParameter(CurrentParam);
    }
  }
}


