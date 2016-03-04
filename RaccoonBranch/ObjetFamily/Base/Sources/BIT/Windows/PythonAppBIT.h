/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT.                                                   *
 * Module: BIT                                                      *
 * Module Description: Python scripts BIT tests support.            *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg                                                *
 * Start date: 31/07/2003                                           *
 * Last upate: 31/07/2003                                           *
 ********************************************************************/

#ifndef _PYTHON_APP_BIT_H_
#define _PYTHON_APP_BIT_H_

class CBITManager;

namespace PythonAppBIT {

// Register application specific test to the given BIT manager
void Register(CBITManager *BITManager);

}

#endif
