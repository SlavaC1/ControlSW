/*===========================================================================
 *   FILENAME       : Define {Define.h}  
 *   PURPOSE        : Hold's All Bit,byte,register definition's 
 *   DATE CREATED   : 02/Sep/2001
 *   PROGRAMMER     : Juval Izhaki 
 *   Description    : This file hold's software definition 
 *===========================================================================*/
/*Master Include file */
/*=============================*/
#ifndef _DEFINE_H_
#define _DEFINE_H_

#define FALSE				 0
#define TRUE				 1
#define SET          1
#define RESET        0
#define Uint         unsigned int 
#define uchar        unsigned char
#define BYTE         unsigned char 
#define WORD         unsigned int
#define DWORD        unsigned long
#define BOOL 				 unsigned char
#define PSHORT			 short*
#define PUSHORT			 unsigned short*	 
#define PBYTE				 BYTE*
#define PULONG			 unsigned long*
#define PLONG				 long*
#define DISABLE      0
#define ENABLE       1
#define CLEAR        0
#define STX          0x024
#define ETX          0x023
#define LSB_MASK		 0x00ff
#define MSB_MASK		 0xff00





#ifndef NULL
#define NULL         ((void *) 0L)
#endif



#endif
