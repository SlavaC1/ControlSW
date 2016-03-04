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

#ifndef _Q_VERSION_INFO_H_
#define _Q_VERSION_INFO_H_

#include "QTypes.h"

// Get application version string (long format, e.g. 2.5.0.1)
QString QGetAppVersionStr(void);

// Get application version string (short format, e.g. 2.5)
QString QGetShortAppVersionStr(void);

#endif


