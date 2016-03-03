
#ifndef _MACHINE_MANAGER_DEFS_H_
#define _MACHINE_MANAGER_DEFS_H_


// Type for the possible machine states
typedef enum {
  msPowerUp,
  msIdle,
  msPrePrint,
  msPrinting,
  msStopping,
  msPausing,
  msPaused,
  msStopped,
  msTerminate,
  msPurge,
  msFireAll,
  msTestPattern,
  msGoToPurge,
  msWipe,
  msStandby1,
  msStandby2,
  msRemovalTray,
  msHeadInspection, 
  msInitializing,
  msPowerFailShutdown,
  msLowerTray

} TMachineState;

extern const QString StatesLookup[];

#endif
 