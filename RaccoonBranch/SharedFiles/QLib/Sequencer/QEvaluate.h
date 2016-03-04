/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib.                                                   *
 * Module Description: Evaluate and execute an expression.          *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 27/04/2003                                           *
 * Last upate: 11/08/2003                                           *
 ********************************************************************/

#ifndef _Q_EVALUATE_H_
#define _Q_EVALUATE_H_

#include <vector>
#include "QObject.h"
#include "QException.h"


class CToken;
class CStringSerializer;

// Exception class for the QSequencer
class EQEvaluate : public EQException {
  public:
    EQEvaluate(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

// QEvaluate class
class CQEvaluate : public CQObject {
  private:
    typedef std::vector<CToken *> TTokenList;

    CToken *GetNextToken(CStringSerializer& StringSerializer);

    // Return tokens list data structure
    TTokenList CreateTokensList(const QString Expression);

    // Execute a property assignment
    QString ExecutePropertyAssignment(const QString ObjectName,const QString PropName,const QString PropValue);

    // Execute a method call
    QString ExecuteMethodCall(const QString ObjectName,const QString MethodName,const QString *Args,int ArgsNum);

    // Return the value of a given property
    QString EvaluateProperty(const QString ObjectName,const QString PropName);

    // Execute a token list
    QString ExecuteTokenList(const TTokenList& TokenList);

    void ClearTokensList(TTokenList& TokenList);

  public:
    QString Evaluate(const QString Expression);
};

#endif
