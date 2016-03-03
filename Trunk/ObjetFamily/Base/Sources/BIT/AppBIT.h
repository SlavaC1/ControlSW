/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT.                                                   *
 * Module: BIT                                                      *
 * Module Description: Application specific BIT tests.              *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg                                                *
 * Start date: 28/07/2003                                           *
 * Last upate: 27/07/2003                                           *
 ********************************************************************/

#ifndef _APP_BIT_H_
#define _APP_BIT_H_

class CBITManager;

namespace AppBIT {

  // Initialize
  void Init(void);

  // De-Initialize
  void DeInit(void);

  // Register application specific test to the given BIT manager
  void Register(CBITManager *BITManager);
}

#endif
