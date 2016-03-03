/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Tester.                                                  *
 * Module Description: Wide (2 bytes length) pascal style string    *
 *                     utilities.                                   *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 03/09/2001                                           *
 * Last upate: 03/09/2001                                           *
 ********************************************************************/

#ifndef _PASCAL_STRING_H_
#define _PASCAL_STRING_H_

#include "QTypes.h"

// Help function for converting wide pascal style string to QString
QString PascalStrToQStr(PBYTE PascalStr,unsigned& StrLength);

// Help function for converting QString to wide pascal style string
void QStrToPascalStr(QString QStr,PBYTE PascalStr,unsigned& StrLength);


#endif
