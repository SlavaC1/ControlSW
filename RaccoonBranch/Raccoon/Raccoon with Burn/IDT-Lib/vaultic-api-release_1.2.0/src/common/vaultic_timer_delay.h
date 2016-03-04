/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_timer_delay.h
 * 
 * \brief Interface to timer delay functions.
 * 
 * \par Description:
 * The VaultIC API calls these functions to delay for a time period specified.
 * in microseonds.  The customer should provide their own implementation of
 * these implementations.
 */

#ifndef VAULTIC_TIMER_DELAY_H
#define VAULTIC_TIMER_DELAY_H

/**
* Delays the code execution by uSecDelay microseconds
*/
void VltSleep( VLT_U32 uSecDelay );

#endif/*VAULTIC_TIMER_DELAY_H*/
