#ifndef _HEADS_VOLTAGE_READER_H_
#define _HEADS_VOLTAGE_READER_H_

#include "Define.h"

void HeadsVoltageReaderInit();
void HeadsVoltageReaderTask(BYTE Arg);
WORD A2D0Reader_GetReading(BYTE AnalogInNum);
void A2D0Reader_GetBlockReadings(BYTE FirstAnalogIn, BYTE LastAnalogIn, WORD *Buffer);
void A2D0Reader_GetHeadsVoltages(WORD* Voltages);


#endif