/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Eden                                                    *
 * Module: Maintenance counters file format definitions.            *
 *                                                                  *
 * Compilation: Standard C/C++.                                     *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 08/10/2003                                           *
 * Last upate: 08/10/2003                                           *
 ********************************************************************/

#ifndef _MAINTENANCE_COUNTERS_FILE_DEFS_H_
#define _MAINTENANCE_COUNTERS_FILE_DEFS_H_

#include <vector>

// Align all structures to byte boundry

// Is it VxWorks?
#ifdef OS_VXWORKS
  #define PACKED_STRUCT __attribute__ ((packed))
#else
  // "PACKED_STRUCT" does nothing in windows
  #define PACKED_STRUCT

  // "Pragma" directive for Borland and Microsoft compilers
  #pragma pack(push,1)
#endif

// File format defintions
// *******************************************************************

// File header format
typedef struct {
  char Signature[2];     // Must be 'MC'
  ULONG VersionNum;      // File format version number, original version is 1
  ULONG HeaderSize;      // File header size in bytes
  ULONG RecordsNum;      // Number of counters stored in the file
  ULONG RecordSize;      // Size of each counter record in bytes

} TMaintenanceCountersFileHeader PACKED_STRUCT;

// Counter record format
typedef struct {
  ULONG  CounterID;
  double ElapsedSeconds;
  long   WarningTime;
  ULONG  ResetDate;
  bool   NeedsUserUpdate;

} TMaintenanceCountersFileRecord PACKED_STRUCT;

typedef std::vector<TMaintenanceCountersFileRecord> TMaintenanceCountersFileRecordList; 

// Restore previous alignment (windows only)
#ifndef OS_VXWORKS
  #pragma pack(pop)
#endif

#endif

