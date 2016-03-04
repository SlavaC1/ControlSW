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

#ifndef _Q_PARAMS_CONTAINER_H_
#define _Q_PARAMS_CONTAINER_H_

#include <stdio.h>
#include "QComponent.h"
#include "QParameter.h"


class CQParamsContainer;

// Exception class for the parameters streaming classes
class EQParamsStreamError : public EQException {
  public:
    EQParamsStreamError(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};


// This class is an abstract interface to parameters streaming media (stdio, file, GUI, FLASH memory, etc.)
class CQParamsStream : public CQObject
{
public:
    virtual void Save(void) = 0;
    virtual ~CQParamsStream();    
    virtual void LogParams(const QString FileName, CQParamsContainer *AttachedContainer);

    // End the write to the stream (optional)

    // Write a single parameter
    virtual void WriteParameter(CQParameterBase *Param) = 0;

    //wrie a parameter only if it exists in the import stream
    virtual void WriteOnlyParametersFromStream(CQParameterBase *Param, CQParamsStream *ImportStream);

    // Read a single parameter
    virtual void ReadParameter(CQParameterBase *Param) = 0;

    // Return true if the parameter can be found in the parameters stream
    virtual bool IsParamInStream(CQParameterBase *Param);

    virtual QString ReadRawParam(const QString ParamName); 
};


// Parameters container class
class CQParamsContainer : public CQComponent
{
  private:
    // Parameters stream
    CQParamsStream *m_ParamsStream;

  protected:

    virtual void BeforeSaveEvent(void);
    virtual void DuringSaveEvent(CQParameterBase* ParamToSave);
    virtual void AfterSaveEvent (CQParameterBase* ParamToSave = NULL);

  public:
    // Constructor
    CQParamsContainer(CQParamsStream *ParamsStream=NULL,const QString Name = "Parameters",bool DoRegister = true);

    // Destructor
    virtual ~CQParamsContainer(void);

    // Assign a new params stream to the container
    void AssignParamsStream(CQParamsStream *ParamsStream);

    // Save a single parameter
    void SaveSingleParameter(CQParameterBase *ParamToSave,CQParamsStream *AlternativeParamsStream = NULL);

    // Load a single parameter
    void LoadSingleParameter(CQParameterBase *ParamToLoad,CQParamsStream *AlternativeParamsStream = NULL);

    DEFINE_METHOD_1(CQParamsContainer,TQErrCode,LogParameters,QString);

    // Save all parameters, version 1: use the class internal member as a stream
    TQErrCode SaveAll(); // This method is not in the roster because it is not safe. It may save parameters that were changed somewhere else without the intention to save them. Arcady.

    // Save all parameters, but only the ones that also exist in the import stream
    TQErrCode SaveAllRestrictedToImportStream(CQParamsStream *ImportStream);

    // Save all parameters, version 2: use an alternative params stream
    TQErrCode SaveAll(CQParamsStream *AlternativeParamsStream);
    
    // Load all parameters, version 1: use the class internal member as a stream
    DEFINE_METHOD(CQParamsContainer,TQErrCode,LoadAll);

    // Load all parameters, version 2: use an alternative params stream
    TQErrCode LoadAll(CQParamsStream *AlternativeParamsStream);

    // Restore all the parameters do their defaults
    DEFINE_METHOD(CQParamsContainer,TQErrCode,RestoreDefaults);

    // Push all parameters (each parameter has its own stack)
    DEFINE_METHOD(CQParamsContainer,TQErrCode,PushAll);

    // Pop all parameters
    DEFINE_METHOD(CQParamsContainer,TQErrCode,PopAll);

    // Caollapse the stack of all parameters
    DEFINE_METHOD(CQParamsContainer,TQErrCode,CollapseAll);

    // Reset the stack for all parameters
    DEFINE_METHOD(CQParamsContainer,TQErrCode,ResetAll);

    // Push a specific parameter to its stack
    DEFINE_METHOD_1(CQParamsContainer,TQErrCode,ParamPush,QString /*ParamName*/);

    // Pop a specific parameter from its stack
    DEFINE_METHOD_1(CQParamsContainer,TQErrCode,ParamPop,QString /*ParamName*/);

    // Save a specific parameter
    DEFINE_METHOD_1(CQParamsContainer,TQErrCode,ParamSave,QString /*ParamName*/);

    // Load a specific parameter
    DEFINE_METHOD_1(CQParamsContainer,TQErrCode,ParamLoad,QString /*ParamName*/);

    // Restore a single parameter to default
    DEFINE_METHOD_1(CQParamsContainer,TQErrCode,ParamRestoreDefault,QString /*ParamName*/);

   // Read raw parameter value from the associated stream
    DEFINE_METHOD_1(CQParamsContainer,QString,ReadRawFromStream,QString /*ParamName*/);

    // Import a parameters collection using a params stream
    void Import(CQParamsStream *ParamsStream,bool PushInStack);

    // Go one mode back from a given parameters collection
    void UnImport(CQParamsStream *ImportStream);
};

#endif

