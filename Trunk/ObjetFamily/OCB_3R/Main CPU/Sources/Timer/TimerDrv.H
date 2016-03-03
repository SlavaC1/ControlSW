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


#define TIMER0_MS_PER_TICKS 	10

#define TIMER0_MS_TO_TICKS(x)	  ((x) / TIMER0_MS_PER_TICKS)
#define TIMER0_SEC_TO_TICKS(x)  ((DWORD)(x) * 1000 / TIMER0_MS_PER_TICKS)

// Structure definitions
// =====================
typedef struct
{
 WORD	StartTime;
 WORD Timeout; 
}TIMER_struct;

typedef struct
{
 WORD	StartTime;
 WORD Timeout; 
}TIMER_SEC_struct;




// Function Prototype 
// ====================
void Timer_0_Init(void);
void Timer_1_Init(void);
void Timer_2_Init(void);
void Timer_4_Init(void);

void SysClkInit  (void);
void Timer_2_ISR (void);
void TimerDelay(WORD Delay);

void TimerSetTimeout(TIMER_struct *Timer,WORD Timeout);
void TimerSEC_SetTimeout(TIMER_SEC_struct *Timer,WORD Timeout); // In Sec.

BOOL TimerHasTimeoutExpired(TIMER_struct *Timer);
BOOL TimerSEC_HasTimeoutExpired(TIMER_SEC_struct *Timer); // In Sec.

#ifdef DEBUG
TIMER_struct *GetTimerStructPtr(void);
#endif
//TIMER_SEC_struct *GetTimerSEC_StructPtr(void); // unused

WORD Timer0GetTimerCounter();
WORD TimerSEC_0GetTimerCounter(); // In Sec.

#endif	
