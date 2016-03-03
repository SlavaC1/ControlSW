/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT.                                                   *
 * Module: Log file                                                 *
 * Module Description: Q2RT log file related definitions.           *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 02/05/2002                                           *
 * Last upate: 02/05/2002                                           *
 ********************************************************************/

#ifndef _LOG_FILE_H_
#define _LOG_FILE_H_

#include "QLogFile.h"

const int LOG_TAG_GENERAL      = 0;
const int LOG_TAG_PROCESS      = 1;
const int LOG_TAG_HOST_COMM    = 2;
const int LOG_TAG_PRINT        = 3;
const int LOG_TAG_MOTORS       = 4;
const int LOG_TAG_HEAD_HEATERS = 5;
const int LOG_TAG_TRAY_HEATERS = 6;
const int LOG_TAG_UV_LAMPS     = 7;
const int LOG_TAG_EOL          = 8;
const int LOG_TAG_HEAD_FILLING = 9;
const int LOG_TAG_HEAD_VACUUM  = 10;
const int LOG_TAG_AMBIENT      = 11;
const int LOG_TAG_POWER        = 12;
const int LOG_TAG_HEAD_FILLING_THERMISTORS = 13;
const int LOG_TAG_RFID         = 14;
const int LOG_TAG_HASP         = 15;
const int LOG_TAG_ROLLER_SUCTION_SYSTEM    = 16; /*RSS, itamar*/
//OBJET_MACHINE feature
const int LOG_TAG_OBJET_MACHINE = 17;
const int LOG_TAG_CURRENT_VACUUM = 18;
const int LOG_TAG_PFF			= 19;
const int LAST_LOG_TAG          = 20;

#endif
