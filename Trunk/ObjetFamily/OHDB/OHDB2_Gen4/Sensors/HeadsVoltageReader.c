/*===========================================================================
 *   FILENAME       : Heads voltage reader {HeadsVoltageReader.c}  
 *   PURPOSE        : Voltage A2D reader for Gen4 heads. Because of a hardware limitation, 
 *                    we need to switch between heads during reading, by sending a command 
 *                    to FPGA via SPI using MUX.   
 *   DATE CREATED   : 12/12/2013
 *   PROGRAMMER     : Slava Chuhovich 
 *   PROCEDURES     : 
 *===========================================================================*/

#include "HeadsVoltageReader.h"
#include "A2D.h"
#include "XilinxInterface.h"
#include "MiniScheduler.h"
#include <string.h>

// Mapping of the sensors inputs to the analog inputs
// --------------------------------------------------
#define HEAD_1_VOLTAGE_SENSOR		CPU_ANALOG_IN_1
#define HEAD_2_VOLTAGE_SENSOR		CPU_ANALOG_IN_2
#define HEAD_3_VOLTAGE_SENSOR		CPU_ANALOG_IN_3
#define HEAD_4_VOLTAGE_SENSOR		CPU_ANALOG_IN_4	
#define HEAD_5_VOLTAGE_SENSOR		CPU_ANALOG_IN_5
#define HEAD_6_VOLTAGE_SENSOR		CPU_ANALOG_IN_6
#define HEAD_7_VOLTAGE_SENSOR		CPU_ANALOG_IN_7
#define HEAD_8_VOLTAGE_SENSOR		CPU_ANALOG_IN_8


#define WR_VPP_MUX 0x1E

bit SwitchData = TRUE;

TTaskHandle xdata HeadsVoltageReaderTaskHandle;

static WORD xdata ReaderA2D0SamplesBuffer[NO_OF_CPU_ANALOG_INPUTS];

BYTE xdata OrderedIndex[NO_OF_CPU_ANALOG_INPUTS] = {0,2,4,6,1,3,5,7,8};

void HeadsVoltageReaderInit()
{
	memset(ReaderA2D0SamplesBuffer, 0, NO_OF_CPU_ANALOG_INPUTS * sizeof(WORD));
	
	HeadsVoltageReaderTaskHandle = SchedulerInstallTask(HeadsVoltageReaderTask);
	SchedulerResumeTask(HeadsVoltageReaderTaskHandle, 0);	
}

void HeadsVoltageReaderTask(BYTE Arg)
{
	BYTE data SelectedInput;
	
	enum
	{
		START_A2D_CONVERSION,
		GET_CONVERSION_RESULTS,
		SEND_SWITCH_COMMAND_TO_FPGA_MUX,
		WAIT_FOR_OP_DONE		
	};

	TXilinxMessage Msg;

	switch(Arg)
	{
		case START_A2D_CONVERSION:
		{
			SetA2D0BufferReadiness(FALSE);
			
			// Start the A2D conversion process
			AD0BUSY = 1;
			
			SchedulerLeaveTask(GET_CONVERSION_RESULTS);
		}
		break;

		case GET_CONVERSION_RESULTS:
		{
			if(IsA2D0BufferReady() == TRUE)
			{
				SelectedInput = GetA2D0SelectedInput();
				
				// We read all even heads in a row and all odd heads in a row
				// and converting back to the correct order for the read buffer (H1, H2, H3, H4, ...)
				ReaderA2D0SamplesBuffer[OrderedIndex[SelectedInput]] = A2D0_GetReading(SelectedInput);				

				// Select the next analog input for the next conversion
				A2D0_SelectNextInput();
				
				if(SelectedInput == 3 || SelectedInput == NO_OF_CPU_ANALOG_INPUTS - 1)	
					SchedulerLeaveTask(SEND_SWITCH_COMMAND_TO_FPGA_MUX);
				else
					SchedulerLeaveTask(START_A2D_CONVERSION);								
			}
			else
			{
				SchedulerLeaveTask(GET_CONVERSION_RESULTS);				
			}				
		}
		break;
		
		case SEND_SWITCH_COMMAND_TO_FPGA_MUX:
		{			
			Msg.Address = WR_VPP_MUX;
			Msg.Data    = SwitchData;
			
			if(XilinxWrite(&Msg) == XILINX_NO_ERROR)
				SchedulerLeaveTask(WAIT_FOR_OP_DONE);
			else
				SchedulerLeaveTask(SEND_SWITCH_COMMAND_TO_FPGA_MUX);
		}
		break;
		
		case WAIT_FOR_OP_DONE:
		{
			if(XilinxGetWriteActionStatus() != XILINX_BUSY)
			{				
				SwitchData = (SwitchData == TRUE) ? FALSE : TRUE; 				
				
				SchedulerLeaveTask(START_A2D_CONVERSION);			
			}
			else
				SchedulerLeaveTask(WAIT_FOR_OP_DONE);
		}
		break;		
		
		default:			
			break;
	}
}

WORD A2D0Reader_GetReading(BYTE AnalogInNum)
{	
	return ReaderA2D0SamplesBuffer[AnalogInNum];	
}

void A2D0Reader_GetBlockReadings(BYTE FirstAnalogIn, BYTE LastAnalogIn, WORD *Buffer)
{	
	memcpy(Buffer, &(ReaderA2D0SamplesBuffer[FirstAnalogIn]), (LastAnalogIn - FirstAnalogIn + 1) * sizeof(WORD));		
}

void A2D0Reader_GetHeadsVoltages(WORD* Voltages)
{
	A2D0Reader_GetBlockReadings(HEAD_1_VOLTAGE_SENSOR, HEAD_8_VOLTAGE_SENSOR, Voltages);
}