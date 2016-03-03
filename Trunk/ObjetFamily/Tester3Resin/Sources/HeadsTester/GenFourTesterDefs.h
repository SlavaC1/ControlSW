#ifndef GenFourTesterDefsH
#define GenFourTesterDefsH

#include "QTypes.h"

#define CONVERT_VPP_A2D_TO_VOLT(x) (21 * 2.4 * (x) / 4096)

#define DEL_LAST_2_CHARS(str)(str.erase(str.end() - 2, str.end()))

const int NOZZLES_IN_GEN4_HEAD       = 384;
const int HEADS_NUM                  = 4;
const int NOZZLE_LINES_PER_HEAD      = 2;
const int HEAD_VOLTAGES_NUM          = HEADS_NUM * NOZZLE_LINES_PER_HEAD;
const int HEATERS_NUM                = 13; // including 4 block heaters and pre heater
const int ACTUATORS_NUM              = 64;
const int NOZZLES_PATTERN_SIZE       = NOZZLES_IN_GEN4_HEAD / 8 * HEADS_NUM;

const int FPGA_MASTER_CLOCK     = 33333333; // 33.33 MHz
const int FPGA_STROBE_CLOCK     = 2080000;  // 2.08 MHz

// Actuators
const int ACT_ON  = 1;
const int ACT_OFF = 0;

const int SUCCESS = 0;

// FPGA registers
// ------------------------------------------------
// ------------------------------------------------
const int FPGA_REG_GO           = 0x03; // WR_GO
const int FPGA_REG_DIAG1        = 0x04; // WR_DIAG1
const int FPGA_REG_FIRE_FREQ    = 0x35; // WR_SIMFR
const int FPGA_REG_DIAG2        = 0x0F; // WR_DIAG2
const int FPGA_REG_STATUS       = 0xB0; // RD_STATUS
const int FPGA_REG_RESET_ALL    = 0x10; // W_RSTALL
const int FPGA_REG_RESET_SM     = 0x11; // W_RSTSTT
const int FPGA_REG_DATA_SOURCE  = 0x2B; // FF_WR_SEL
const int FPGA_REG_FIFO_RESET   = 0x12;

// Fire modes configuration
const int FPGA_REG_FIRES_ON_H    = 0x37; // WR_FIRES_ON1
const int FPGA_REG_FIRES_ON_L    = 0x38; // WR_FIRES_OFF1
const int FPGA_REG_FIRES_OFF_H   = 0x39; // WR_FIRES_ON2
const int FPGA_REG_FIRES_OFF_L   = 0x3A; // WR_FIRES_OFF2
const int FPGA_REG_FIRE_CYCLES_L = 0x3B; // WR_FIRES_CYCLS
const int FPGA_REG_FIRE_CYCLES_H = 0x3C; // WR_FIRES_CYCLS
const int FPGA_REG_PRINT_DATA    = 0x2A; // FF_WR_CK

// Pulse registers
// ------------------------------------------------
const int FPGA_REG_PULSE_OP_MODE   = 0x1D; // WR_PLSR_MODE: 0 - single, 1 - double

// Fall time for first pulse
const int FPGA_REG_PULSE1_FT_HEAD1 = 0xB0;
const int FPGA_REG_PULSE1_FT_HEAD2 = 0xB1;
const int FPGA_REG_PULSE1_FT_HEAD3 = 0xB2;
const int FPGA_REG_PULSE1_FT_HEAD4 = 0xB3;
const int FPGA_REG_PULSE1_FT_HEAD5 = 0xB4;
const int FPGA_REG_PULSE1_FT_HEAD6 = 0xB5;
const int FPGA_REG_PULSE1_FT_HEAD7 = 0xB6;
const int FPGA_REG_PULSE1_FT_HEAD8 = 0xB7;

// Pulse width for second pulse
const int FPGA_REG_PULSE2_WIDTH_HEAD1 = 0xB8;
const int FPGA_REG_PULSE2_WIDTH_HEAD2 = 0xB9;
const int FPGA_REG_PULSE2_WIDTH_HEAD3 = 0xBA;
const int FPGA_REG_PULSE2_WIDTH_HEAD4 = 0xBB;
const int FPGA_REG_PULSE2_WIDTH_HEAD5 = 0xBC;
const int FPGA_REG_PULSE2_WIDTH_HEAD6 = 0xBD;
const int FPGA_REG_PULSE2_WIDTH_HEAD7 = 0xBE;
const int FPGA_REG_PULSE2_WIDTH_HEAD8 = 0xBF;

// Dwell time for first pulse
const int FPGA_REG_PULSE1_DWELL_HEAD1 = 0xC0;
const int FPGA_REG_PULSE1_DWELL_HEAD2 = 0xC1;
const int FPGA_REG_PULSE1_DWELL_HEAD3 = 0xC2;
const int FPGA_REG_PULSE1_DWELL_HEAD4 = 0xC3;
const int FPGA_REG_PULSE1_DWELL_HEAD5 = 0xC4;
const int FPGA_REG_PULSE1_DWELL_HEAD6 = 0xC5;
const int FPGA_REG_PULSE1_DWELL_HEAD7 = 0xC6;
const int FPGA_REG_PULSE1_DWELL_HEAD8 = 0xC7;

// Delay between first and second pulses
const int FPGA_REG_DELAY_HEAD1 = 0xD0;
const int FPGA_REG_DELAY_HEAD2 = 0xD1;
const int FPGA_REG_DELAY_HEAD3 = 0xD2;
const int FPGA_REG_DELAY_HEAD4 = 0xD3;
const int FPGA_REG_DELAY_HEAD5 = 0xD4;
const int FPGA_REG_DELAY_HEAD6 = 0xD5;
const int FPGA_REG_DELAY_HEAD7 = 0xD6;
const int FPGA_REG_DELAY_HEAD8 = 0xD7;
// ------------------------------------------------ 

// Strobe LED control
const int FPGA_REG_STROBE_PULSE_DELAY = 0x1A; // WR_STRB_DLY
const int FPGA_REG_STROBE_PULSE_WIDTH = 0x1B; // WR_STRB_PLS_W
// ------------------------------------------------
// ------------------------------------------------

// Diagnostics register values
const int DIAG1_ENABLE_SIMS   = 0x393;  // The strobe is OFF by default
const int DIAG1_STROBE_ON     = 0x1000; // OR  with DIAG1 to set bit 12 high
const int DIAG1_STROBE_OFF    = 0xEFFF; // AND with DIAG1 to set bit 12 low
const int DIAG1_FIRE_INFINITE = 0x820;  // OR  with DIAG1 to set bits 5, 11 high
const int DIAG1_FIRE_CYCLES   = 0xF7DF; // AND with DIAG1 to set bits 5, 11 low


// Icon type enumeration from the image list
enum TIconType
{
	icoSet,
	icoClear,
	icoSave,
	icoLoad,
	icoDownload,
	icoLedGreen,
	icoLedRed,
	icoLedGrey
};

const int PULSE1_FT_REGS[HEAD_VOLTAGES_NUM] =
{
	FPGA_REG_PULSE1_FT_HEAD1,
	FPGA_REG_PULSE1_FT_HEAD2,
	FPGA_REG_PULSE1_FT_HEAD3,
	FPGA_REG_PULSE1_FT_HEAD4,
	FPGA_REG_PULSE1_FT_HEAD5,
	FPGA_REG_PULSE1_FT_HEAD6,
	FPGA_REG_PULSE1_FT_HEAD7,
	FPGA_REG_PULSE1_FT_HEAD8
};

const int PULSE2_WIDTH_REGS[HEAD_VOLTAGES_NUM] =
{
	FPGA_REG_PULSE2_WIDTH_HEAD1,
	FPGA_REG_PULSE2_WIDTH_HEAD2,
	FPGA_REG_PULSE2_WIDTH_HEAD3,
	FPGA_REG_PULSE2_WIDTH_HEAD4,
	FPGA_REG_PULSE2_WIDTH_HEAD5,
	FPGA_REG_PULSE2_WIDTH_HEAD6,
	FPGA_REG_PULSE2_WIDTH_HEAD7,
	FPGA_REG_PULSE2_WIDTH_HEAD8
};

const int PULSE1_DWELL_REGS[HEAD_VOLTAGES_NUM] =
{
	FPGA_REG_PULSE1_DWELL_HEAD1,
	FPGA_REG_PULSE1_DWELL_HEAD2,
	FPGA_REG_PULSE1_DWELL_HEAD3,
	FPGA_REG_PULSE1_DWELL_HEAD4,
	FPGA_REG_PULSE1_DWELL_HEAD5,
	FPGA_REG_PULSE1_DWELL_HEAD6,
	FPGA_REG_PULSE1_DWELL_HEAD7,
	FPGA_REG_PULSE1_DWELL_HEAD8
};

const int PULSE_DELAY_REGS[HEAD_VOLTAGES_NUM] =
{
	FPGA_REG_DELAY_HEAD1,
	FPGA_REG_DELAY_HEAD2,
	FPGA_REG_DELAY_HEAD3,
	FPGA_REG_DELAY_HEAD4,
	FPGA_REG_DELAY_HEAD5,
	FPGA_REG_DELAY_HEAD6,
	FPGA_REG_DELAY_HEAD7,
	FPGA_REG_DELAY_HEAD8
};

struct TFPGACommand
{
	TFPGACommand(BYTE Add, USHORT Dat) : Address(Add), Data(Dat) {}

	BYTE   Address;
	USHORT Data; // same as WORD
};

enum THeadsFillingError
{
	hfNoError,
	hfTempTooHigh,
	hfTempTooLow,
	hfSupportTimeout,
	hfM1Timeout,
	hfM2Timeout,
	hfM3Timeout,
	hfM4Timeout,
	hfM5Timeout,
	hfM6Timeout,
	hfM7Timeout,
	hfSupportM1Timeout,
	hfM1M2Timeout,
	hfM3M4Timeout,
	hfM5M6Timeout,
	NUM_OF_FILLING_ERRORS
};

const char* FillingError2Str[NUM_OF_FILLING_ERRORS] =
{
	"No error",
	"Heads temperature too high",
	"Heads temperature too low",
	"Support filling timeout",
	"M1 chamber filling timeout",
	"M2 chamber filling timeout",
	"M3 chamber filling timeout",
	"M4 chamber filling timeout",
	"M5 chamber filling timeout",
	"M6 chamber filling timeout",
	"M7 chamber filling timeout",
	"Support - M1 chamber filling timeout",
	"M1 - M2 chamber filling timeout",
	"M3 - M4 chamber filling timeout",
	"M5 - M6 chamber filling timeout",
};

enum TMSCError
{
	mscNoError,
	mscNoSlaveMSC1,
	mscNoSlaveMSC2,
	mscNoSlaveMSC3,
	mscNoSlaveMSC4,
	mscHwErrorMSC1,
	mscHwErrorMSC2,
	mscHwErrorMSC3,
	mscHwErrorMSC4,
	mscComErrorMSC1,
	mscComErrorMSC2,
	mscComErrorMSC3,
	mscComErrorMSC4,
	NUM_OF_MSC_ERRORS
};

const char* MSCError2Str[NUM_OF_MSC_ERRORS] =
{
	"No error",
	"No slave MSC1",
	"No slave MSC2",
	"No slave MSC3",
	"No slave MSC4",
	"Hardware error MSC1",
	"Hardware error MSC2",
	"Hardware error MSC3",
	"Hardware error MSC4",
	"Comm error MSC1",
	"Comm error MSC2",
	"Comm error MSC3",
	"Comm error MSC4"
};

// Log tags
enum TJSLogTag
{
	JS_LOG_TAG_GENERAL,
	JS_LOG_TAG_FILLING,
	JS_LOG_TAG_HEATING,
	JS_LOG_TAGS_NUM
};

const QString LogTagDescription[JS_LOG_TAGS_NUM] =
{
	"General",
	"Filling",
	"Heating"
};

#endif