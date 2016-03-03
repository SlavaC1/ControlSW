#ifndef _MAINTENANCE_COUNTER_H_
#define _MAINTENANCE_COUNTER_H_

#define DEFINE_MAINTENANCE_COUNTER(_id_,_str_,_groupStr_,_warningTime_,_needToInformUser,_isAdvanced) _id_##_COUNTER_ID,
#define DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(_id_,_str_,_groupStr_,_warningTime_,_needToInformUser_,_isAdvanced_,_displayName_) _id_##_COUNTER_ID,

const int TIME_AFTER_HSW_WARNING_TIME               = 300; 

enum
{
  #include "MaintenanceCountersDefsList.h"
  NUM_OF_MAINTENANCE_COUNTERS,
  WASTE_CARTRIDGE_ALERT_ID = NUM_OF_MAINTENANCE_COUNTERS,
  ALL_COUNTERS = -1
};

extern const bool ServiceNeedToInformLookup[NUM_OF_MAINTENANCE_COUNTERS+1];
extern const bool ServiceIsAdvancedLookup[NUM_OF_MAINTENANCE_COUNTERS+1];
extern const char *ServiceWarningLookup[NUM_OF_MAINTENANCE_COUNTERS+1];
extern const char *CounterNameLookup[NUM_OF_MAINTENANCE_COUNTERS+1];
extern const char *ServiceGroupStrLookup[NUM_OF_MAINTENANCE_COUNTERS+1];
extern const int ServiceCounterIDLookup[NUM_OF_MAINTENANCE_COUNTERS+1];
extern const long ServiceWarningTimeLookup[NUM_OF_MAINTENANCE_COUNTERS+1];

#undef DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME
#undef DEFINE_MAINTENANCE_COUNTER

#endif /*_MAINTENANCE_COUNTER_H_*/

