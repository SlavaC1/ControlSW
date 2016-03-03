#ifndef _MSG_EXTERN_H_
#define _MSG_EXTERN_H_

#include "TimerDrv.h" //TIMER_struct type

#ifdef DEFINE_MSG_EXTERN_VARIABLES
  #define EXTERN                  extern
  //#define INITIALIZER(...)        /* nothing */
#else
  #define EXTERN                  /* nothing */
  //#define INITIALIZER(...)        = __VA_ARGS__
#endif /* DEFINE_MSG_EXTERN_VARIABLES */

//When the timer expires, this means that the communication has just been lost (short-term)
EXTERN TIMER_struct xdata ImmediateCommunicationLossTimer;
EXTERN BOOL xdata TargetIsUnresponsive;

#endif //_MSG_EXTERN_H_