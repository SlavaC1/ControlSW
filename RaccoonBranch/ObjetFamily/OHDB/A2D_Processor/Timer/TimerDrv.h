/*===========================================================================
 *   FILENAME       : TIMER DRIVER H FILE {TIMERDRV.H}  
 *   PURPOSE        : Hold's All TIMER routine  
 *   DATE CREATED   : 02/Sep/2001
 *   PROGRAMMER     : Juval Izhaki 
 *   Description    : TIMER H File 
 *   Include        : Define.h , HardWareIO.h
 *===========================================================================*/
#ifndef _TIMER_H_
#define _TIMER_H_

#include "Define.h"

#define TIMEOUT_NOT_EXPIRED   -1
#define TIMEOUT_EXPIRED        1

#define TIMER0_MS_PER_TICKS 	10

#define TIMER0_MS_TO_TICKS(x)	((x) / TIMER0_MS_PER_TICKS)

// Structure definitions
// =====================
typedef struct
{
 WORD	StartTime;
 WORD Timeout; 
}TIMER_struct;



// Function Prototype 
// ====================
	void Timer_2_Init(void);
	void Timer_0_Init(void);
	void SysClkInit  (void);
	void Timer_2_ISR (void);
	BYTE TimerDelay(WORD Delay);
	char TimerSetTimeout(TIMER_struct *Timer,WORD Timeout);
	char TimerTimeoutExpired(TIMER_struct *Timer);
	TIMER_struct *GetTimerStructPtr(void);
	WORD Timer0GetTimerCounter();

#endif	
