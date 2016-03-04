/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers                                         *
 * Module Description: Cross-Platform application version           *
 *                     information service.                         *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 23/04/2002                                           *
 * Last upate: 23/04/2002                                           *
 ********************************************************************/

#include "QTypes.h"


// Get application version string
QString QGetAppVersionStr(void)
{
  return "1.0.0.0";
}

// Get application version string (short format, e.g. 2.5)
QString QGetShortAppVersionStr(void)
{
  return "1.0";
}
