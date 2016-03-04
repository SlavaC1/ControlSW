/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers                                         *
 * Module Description: Collection of various utility functions and  *
 *                     declarations related to threads.             *
 *                                                                  *
 * Compilation: Standard C++, VxWorks                               *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 12/07/2001                                           *
 * Last upate: 22/08/2001                                           *
 ********************************************************************/

#include <taskLib.h>
#include "QThreadUtils.h"


// Cross-platform sleep (time is measured in ms)
void QSleepTicks(TQWaitTime SleepTime)
{
  taskDelay(SleepTime); 
}


