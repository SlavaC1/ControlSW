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

#include <string.h>
#include "PascalString.h"


// Help function for converting pascal style strings to QString
QString PascalStrToQStr(PBYTE PascalStr,unsigned& StrLength)
{
  QString Str;

  StrLength = (*((PUSHORT)PascalStr));

  Str.assign((PCHAR)(PascalStr + 2),StrLength);

  return Str;
}

// Help function for converting QString to wide pascal style string
void QStrToPascalStr(QString QStr,PBYTE PascalStr,unsigned& StrLength)
{
  StrLength = QStr.length();

  *((PUSHORT)PascalStr) = (USHORT)StrLength;
  memcpy(PascalStr + 2,QStr.data(),StrLength);
}

