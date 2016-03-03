/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Module: PKWARE compression utilities (uses the implode library). *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 16/12/2002                                           *
 * Last upate: 16/12/2002                                           *
 ********************************************************************/

#ifndef _PKWARE_H_
#define _PKWARE_H_

/* Implementation note
   -------------------
   This implementation is not re-entrant or thread safe.
*/

// (De)Initialization functions
void InitPKExtract(void);
void DeInitPKExtract(void);

// Compress buffer
int PKCompress(void *SourceBuffer,void *DestBuffer,unsigned SourceBufferSize,
               unsigned DestBufferMaxSize,unsigned& DestBufferActualSize);

// Extract buffer               
int PKExtract(void *SourceBuffer,void *DestBuffer,unsigned SourceBufferSize,
              unsigned DestBufferMaxSize,unsigned& DestBufferActualSize);

#endif
