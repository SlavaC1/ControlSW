/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib Error codes.                                        *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 11/07/2001                                           *
 * Last upate: 26/06/2003                                           *
 ********************************************************************/

#ifndef _Q_ERRORS_H_
#define _Q_ERRORS_H_

const int Q_NO_ERROR                      =  0;
const int Q_FAIL_ACTION                             = 1;
const int Q_CRITICAL_FAIL_ACTION                    = 2;
const int Q_CAN_NOT_START_TASK            = -1;
const int Q_FILE_SYS_INIT_ERR             = -2;
const int Q_CHANGE_DIR_ERR                = -3;
const int Q_PARAM_NOT_FOUND_ERR           = -4;
const int Q_OPEN_FILE_ERR                 = -5;
const int Q_OPEN_BACKUP_FILE_ERR          = -6;
const int Q_OPEN_FILE_AND_BACKUP_FILE_ERR = -7;
const int Q_FILE_NOT_FOUND_ERR            = -9;
const int Q_OS_ERR                        = -10;

// The base error number for user errors (non QLib errors)
const int Q_USER_ERROR = -10000;

#endif

