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

#ifndef _Q_FILE_WITH_CHECKSUM_PARAMS_STREAM_H_
#define _Q_FILE_WITH_CHECKSUM_PARAMS_STREAM_H_

#include <stdio.h>
#include <list>
#include "QStringList.h"
#include "QFileParamsStream.h"
#include "QFileSystem.h"

typedef enum
{
  INIT_OK_E = 0,
  BOTH_FILES_ARE_MISSING_E,
  MAIN_FILE_CORRUPT_E,
  BOTH_FILES_ARE_CORRUPTED_E,
  INIT_FAIL_E
}TInitStatus_E;

class CQParamsContainer;

// Exception class for the parameters streaming classes
class EQParamsFileWithChecksumStream : public EQException
{
public:
    EQParamsFileWithChecksumStream(const QString& ErrMsg, const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

class CQParamsFileWithChecksumStream : public CQParamsFileStream
{
private:
    QString m_BackupFileName;
    bool m_GenerateNewCheckSumFile;
    
protected:
    // Write parameters and checksum to a given file
    virtual CQStringList LogParamsToStringList(CQParamsContainer *AttachedContainer);

public:
	// Constructor
	CQParamsFileWithChecksumStream() {}
	// Constructor
	CQParamsFileWithChecksumStream(const QString FileName);
	// Constructor
	CQParamsFileWithChecksumStream(const QString FileName, TInitStatus_E &StatusInit);
	// Constructor for genarating new checksum file from corrupted one (for ParameterImportAndExport use only!!!).
	//Silent - means if to promt user(in Export import) for example : to generate new checksum or do it in a quiet mode.
	CQParamsFileWithChecksumStream(const QString FileName, TInitStatus_E &StatusInit,bool GenerateNewCheckSumFile, bool Silent=false);
	void Init(const QString FileName, TInitStatus_E &StatusInit,bool GenerateNewCheckSumFile , bool Silent=false);

    virtual ~CQParamsFileWithChecksumStream(void);
	virtual bool LoadFileIntoStringList(QString FileName,CQStringList& StrList);

    //Save parameters to file with checksum
	bool SaveFileWithChecksum();

    // Write a single parameter
    void WriteParameter(CQParameterBase *Param);
    virtual void Save(void);

    // Write the parameter to the output file only if it exists in the stream file
    void WriteOnlyParametersFromStream(CQParameterBase *Param, CQParamsStream *ImportStream);
};


#endif

