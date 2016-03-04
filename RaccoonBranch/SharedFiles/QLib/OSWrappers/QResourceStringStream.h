/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib utilities.                                          *
 * Module Description: Resource file string stream -                *
 *                     implement a CQTranslatorStringStream class   *
 *                     for windows resource file.                   *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 02/03/2003                                           *
 * Last upate: 02/03/2003                                           *
 ********************************************************************/

#ifndef _Q_STRING_TABLE_STREAM_H_
#define _Q_STRING_TABLE_STREAM_H_

#include "QMultiLang.h"

class CQResourceStringStream : public CQTranslatorStringStream {
  private:
    char **m_OriginalStringTable;
    int m_ResStartID;
    int m_ResIndex;
    int m_NumberOfStrings;

    char *m_StrBuffer;

  public:
    // Constructor
    CQResourceStringStream(char **OriginalStringTable,int ResStartID,int NumberOfStrings);

    // Destructor
    ~CQResourceStringStream(void);

    void Begin(void);

    // Get next pair of strings (override)
    CQTranslatorStringStream::TStrPair GetNext(void);
};

#endif
