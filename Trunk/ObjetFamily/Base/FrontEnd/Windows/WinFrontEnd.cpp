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

#include "FrontEnd.h"

#ifndef EDEN_CL
#include "MainUnit.h"
#endif

// Global fron-end interface object
CFrontEndInterface *FrontEndInterface = 0;

void InitFrontEnd(void)
{
  if(FrontEndInterface == 0)
#ifdef EDEN_CL
    FrontEndInterface = new CFrontEndInterface();
#else
    FrontEndInterface = new CFrontEndInterface(MainForm->GetUIFrame());
#endif    
}

void DeInitFrontEnd(void)
{
  if(FrontEndInterface != 0)
    delete FrontEndInterface;
}

