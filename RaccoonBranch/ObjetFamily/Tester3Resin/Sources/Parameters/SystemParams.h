/********************************************************************
 *                Quadra1 Application parameters set                *
 *                ----------------------------------                *
 * Module description: This module define the set of parameters     *
 *                     required for the application.                *
 *                                                                  *
 * Compilation: Standard C++ / BCB.                                 *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 7/05/2000                                            *
 * Last upate: 16/08/2000                                           *
 ********************************************************************/

#ifndef _SYSTEM_PARAMS_H_
#define _SYSTEM_PARAMS_H_

#include "QParamsContainer.h"
#include "QFileParamsStream.h"

#define SYSTEM_CONFIG_FILE_NAME  "\\Config.ini"
#define SYSTEM_ROOT_DIR          "\\Tester\\"
typedef int TConnectionKind;
const int NONE              = 0;
const int DIRECT            = 1;
const int THROUGH_EMBEDDED  = 2;
const int THROUGH_OCB       = 3;

// This class is implemented as a singleton with Init/DeInit factory static functions
class CSystemParams : public CQParamsContainer {
  private:
  CQParamsFileStream *m_ParamsStream;

  public:
    CSystemParams(const QString& IniFileName);
    ~CSystemParams();
    void AssignToINIFile(QString FileName);

    // Parameters defintion start here
    // -------------------------------------
    DEFINE_PARAM(QString,Date);
    DEFINE_PARAM(QString,Time);
    DEFINE_PARAM(QString,OperatorName);
    DEFINE_PARAM(QString,System_SN);
    DEFINE_PARAM(QString,Containers_SN);
    DEFINE_PARAM(QString,LCD_SN);
    DEFINE_PARAM(QString,MCB_SN);
    DEFINE_PARAM(QString,OCB_SN);
    DEFINE_PARAM(QString,OHDB_SN);

    DEFINE_PARAM(int,EmbeddedComNum);
    DEFINE_PARAM(TConnectionKind,EmbeddedConnectionKind);
    DEFINE_PARAM(int,ContainersComNum);
    DEFINE_PARAM(TConnectionKind,ContainersConnectionKind);
    DEFINE_PARAM(int,LCDComNum);
    DEFINE_PARAM(TConnectionKind,LCDConnectionKind);
    DEFINE_PARAM(int,MCBComNum);
    DEFINE_PARAM(TConnectionKind,MCBConnectionKind);
    DEFINE_PARAM(int,OCBComNum);
    DEFINE_PARAM(TConnectionKind,OCBConnectionKind);
    DEFINE_PARAM(int,OHDBComNum);
    DEFINE_PARAM(TConnectionKind,OHDBConnectionKind);
    DEFINE_PARAM(int,OCBSimComNum);
    DEFINE_PARAM(TConnectionKind,OCBSimConnectionKind);
    DEFINE_PARAM(int,RFIDComNum);
    DEFINE_PARAM(TConnectionKind,RFIDConnectionKind);
};
#endif

