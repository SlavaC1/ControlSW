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
 * Last upate: 23/08/2001                                           *
 ********************************************************************/

#ifndef _Q_FILE_PARAMS_STREAM_H_
#define _Q_FILE_PARAMS_STREAM_H_

#include <stdio.h>
#include <map>
#include "QParamsContainer.h"


typedef enum {fpsOpenWriteError,fpsOpenReadError,fpsParamWriteError,
              fpsWriteCloseError,fpsReadCloseError,fpsSeekError} TFileParamsStreamError;

// Calloback type for errors reporting
typedef void (*TParamsFileErrorCallback)(TFileParamsStreamError Error,TGenericCockie Cockie);


class CQParamsFileStream : public CQParamsStream
{
protected:
    QString m_FileName;
    TQStringMap m_KeyToValue;
    bool m_StatusInit;
    
    TParamsFileErrorCallback m_ParamsFileErrorCallback;
    TGenericCockie m_ParamsFileErrorCockie;

    // Use the error callback to notify params stream errors
    void NotifyError(TFileParamsStreamError Error);

    // Split a single string list into values and keys pairs

    // Utility function for loading a text file content into a string list structure
    virtual bool LoadFileIntoStringMap(QString FileName,TQStringMap& KeyToValueMap);
    virtual void LogParams(const QString FileName, CQParamsContainer *AttachedContainer);
    virtual CQStringList LogParamsToStringList(CQParamsContainer *AttachedContainer);
    virtual void SaveFile(const QString FileName);

public:

    static void SplitToKeysAndValues(const CQStringList& StrList,TQStringMap& KeyToValueMap,int LinesToProcess);

    CQParamsFileStream();
    CQParamsFileStream(const QString FileName);
    CQParamsFileStream(const QString FileName, bool& InitOK);

    virtual ~CQParamsFileStream(void);

    void Init(const QString FileName);
    virtual bool LoadFileIntoStringList(QString FileName,CQStringList& StrList);
    virtual void Save(void);
    virtual void WriteParameter(CQParameterBase *Param); // Write a single parameter
    virtual void SetCurrentValueToParameter(CQParameterBase *Param);
    virtual void SetValueToParameter(QString Param, QString Value, TParamAttributes ParamAttributes);
    virtual void ReadParameter(CQParameterBase *Param); // Read a single parameter
    // Return true if the parameter can be found in the parameters stream
    virtual bool IsParamInStream(CQParameterBase *Param);
    virtual void SetErrorsCallback(TParamsFileErrorCallback ParamsFileErrorCallback,TGenericCockie ParamsFileErrorCockie);
    virtual QString ReadRawParam(const QString ParamName);
    virtual TQStringMap GetKeyToValueMap();
    
};


#endif

