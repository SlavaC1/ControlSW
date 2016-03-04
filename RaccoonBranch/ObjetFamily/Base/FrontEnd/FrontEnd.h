/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Q2RT front-end interface - upper level.                  *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 05/02/2002                                           *
 * Last upate: 05/02/2002                                           *
 ********************************************************************/

#ifndef _FRONT_END_H_
#define _FRONT_END_H_

#include "FrontEndInterface.h"
#include "FrontEndControlIDs.h"

// Global fron-end interface object
extern CFrontEndInterface *FrontEndInterface;

void InitFrontEnd(void);
void DeInitFrontEnd(void);

#endif
