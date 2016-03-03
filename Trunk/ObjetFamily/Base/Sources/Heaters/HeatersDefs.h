/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Heaters definitions.                                     *
 * Module Description: Heaters (Head,Tray,Ambient) related          *
 *                     definitions.                                 *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 03/03/2003                                           *
 * Last update: 03/03/2003                                          *
 ********************************************************************/

#ifndef _HEATERS_DEFS_H_
#define _HEATERS_DEFS_H_

const int TRAY_COLD           = 0;
const int TRAY_TEMPERATURE_OK = 1;
const int TRAY_HOT            = 2;

//const int MIN_HEAD_VOLTAGE = 19;
//const int MAX_HEAD_VOLTAGE = 36;

#define REQUESTED_VOLTAGE_LIMITS(_minVoltage_, _maxVoltage_) QFormatStr(LOAD_STRING(IDS_REQUESTED_VOLTAGE_LIMITS), _minVoltage_, _maxVoltage_)
#define CHECK_REQUESTED_VOLTAGE_LIMITS(_val_, _excType_, _minVoltage_, _maxVoltage_) if ((_val_) < _minVoltage_ || (_val_) > _maxVoltage_) throw _excType_(REQUESTED_VOLTAGE_LIMITS(_minVoltage_, _maxVoltage_));

#endif

