/*===========================================================================
 *   FILENAME       : Purge {Purge.h}  
 *   PURPOSE        :  (header file)
 *   DATE CREATED   : 02/08/2004
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/


#ifndef _PURGE_H_
#define _PURGE_H_

#include "Define.h"

void PurgeInit();

// Initiate the purge sequence. If the prge is currently running the command will
// be ignored and a FALSE will be returned. 
BOOL PerformPurge(WORD PurgeTime);

BOOL PurgeGetStatus();



#endif

