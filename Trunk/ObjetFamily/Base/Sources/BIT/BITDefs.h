/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT.                                                   *
 * Module: Built-In-Tests                                           *
 * Module Description: Definitions and constants.                   *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg                                                *
 * Start date: 30/07/2003                                           *
 * Last upate: 21/09/2003                                           *
 ********************************************************************/

#ifndef _BIT_DEFS_H_
#define _BIT_DEFS_H_

// Test attribute type and constants
typedef unsigned TTestAttributes;
const unsigned taNormal       = 0;
const unsigned taBreakGroup   = 1 << 0;   // Break the group test sequence on error
const unsigned taBreakAll     = 1 << 1;   // Break the entire (all groups) test sequence on error
const unsigned taDoorLock     = 1 << 2;   // Require that the door will be locked
const unsigned taTrayInsert   = 1 << 3;   // Require that the tray will be inserted
const unsigned taHomeAxes     = 1 << 4;   // Require axes homing before test
const unsigned taHeadsHeating = 1 << 5;   // Require hot heads before test
const unsigned taTrayHeating  = 1 << 6;   // Require hot tray before test
const unsigned taHeadsFilling = 1 << 7;   // Require heads filled before test

// Priority attributes (upper 16 bit of attributes mask) larger number - higher priority
const unsigned taAboveNormalPriority = 1 << 16;
const unsigned taMediumPriority      = 1 << 17;
const unsigned taHighPriority        = 1 << 18;
const unsigned taVeryHighPriority    = 1 << 19;

// Test result codes
typedef enum {trUnknown,trGo,trNoGo,trWarning} TTestResult;

#endif
