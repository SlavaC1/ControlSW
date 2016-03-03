#ifndef __ERRORCODES_MG_H
#define __ERRORCODES_MG_H

// - Hitag ErrorCodes --------------------------------------------------------
#define        EOK            (0)   // transmission OK
#define        ESERIELL       (-1)  // serial interface error
#define        ENOTAG         (-3)  // no TAG in rf field
#define        ETIMEOUT       (-4)  // not enough energy in TAG to write/read
#define        EPWDRWD        (-5)  // password for RWD -> HITAG2 wrong
#define        EPWDTAG        (-6)  // password for HITAG2 -> RWD wrong
#define        EAUTHENT       (-7)  // authentication error.
#define        EQUIT          (-8)  // no quit (acknowledge) received from RWD
#define        ECRYPTO        (-9)  // data for cryptogaphy not initialized
#define        EE2CHK         (-10) // EEPROM functionality error
#define        EEWRONGOLD     (-11) // wrong old data at write key/log-data
#define        EEWRPROT       (-12) // EEPROM is write protected
#define        EERDPROT       (-13) // EEPROM is read protected
#define        EPIT           (-14) // read blocks of PIT has run out of time
#define        ECRC           (-15) // wrong CRC from HITAG1 in advanced mode
#define        EANTOVERLOAD   (-20) // antenna for long range overloaded

//

#endif
