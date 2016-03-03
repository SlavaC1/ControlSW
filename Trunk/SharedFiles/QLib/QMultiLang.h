/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib utilities.                                          *
 * Module Description: Multi language string translator.            *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 27/07/2003                                           *
 * Last upate: 27/07/2003                                           *
 ********************************************************************/

#ifndef _Q_MULTI_LANG_
#define _Q_MULTI_LANG_

#include <map>
#include "QObject.h"


// Base class for translator string stream
class CQTranslatorStringStream : public CQObject {
  public:
    // Type for a pair of strings
    typedef std::pair<QString,QString> TStrPair;

    // Optional for sub-class implementation
    virtual void Begin(void) {}
    virtual void End(void) {}

    // Get next pair of strings (mandatory)
    virtual TStrPair GetNext(void) = 0;
};

// Initialize the translator with a specific translator string stream
void QInitMultiLangTranslator(CQTranslatorStringStream *TranslatorStream);

// Clean-up
void QDeInitMultiLangTranslator(void);

// String replace translator
QString QXlt(const QString Str);

#endif

