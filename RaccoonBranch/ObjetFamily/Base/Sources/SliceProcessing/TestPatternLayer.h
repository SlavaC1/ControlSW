/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Test Pattern Printing
 *
 * Module Description:                                              *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Rachel / Ran                                             *
 * Start date: 08/01/2002                                           *
 * Last upate: 29/01/2002                                           *
 ********************************************************************/

#ifndef _TEST_PATTERN_H_
#define _TEST_PATTERN_H_

#include "Layer.h"


// Exception class for the CTestPatternLayer class
class ETestPattern : public EQException {
  public:
    ETestPattern(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};


class CTestPatternLayer : public CLayer {
  private:

    void PrepareTestPattern(void);

  public:
    // Constructor
    CTestPatternLayer(void);

    // Destructor
    ~CTestPatternLayer(void);
};

#endif

