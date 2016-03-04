/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#include "vaultic_common.h"
#include "vaultic_timer_delay.h"
#if( VLT_PLATFORM == VLT_LINUX )
    #include <unistd.h>
#endif


void VltSleep( VLT_U32 uSecDelay )
{
#if( VLT_PLATFORM == VLT_WINDOWS )
    Sleep( uSecDelay / VLT_MICRO_SECS_IN_MSEC );
#elif( VLT_PLATFORM == VLT_LINUX )
    usleep( uSecDelay  );
#endif
}
