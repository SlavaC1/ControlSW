/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib hierarchy base class.                               *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 07/01/2001                                           *
 * Last upate: 07/01/2001                                           *
 ********************************************************************/

#ifndef _Q_OBJECT_H_
#define _Q_OBJECT_H_

#include "QTypes.h"

// Master base class
class CQObject {
  public:
    virtual ~CQObject(void) {};
};

#endif
