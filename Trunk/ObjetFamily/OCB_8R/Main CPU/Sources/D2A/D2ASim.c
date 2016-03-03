/*===========================================================================
 *   FILENAME       : D/A  {D2A.C}  
 *   PURPOSE        : D/A driver module  
 *   DATE CREATED   : 25/11/2004
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/
#include "D2A.h"
#ifdef OCB_SIMULATOR
#include "c8051F120.h"
#else
#include "c8051F020.h"
#endif
#include "Define.h"

/****************************************************************************
 *
 *  NAME        : D2A0_Init
 *
 *  DESCRIPTION : Initialization of the D/A (0) driver
 *
 ****************************************************************************/
void D2A0_Init()
{
  //Dummy

}


/****************************************************************************
 *
 *  NAME        : D2A0_Write
 *
 *  DESCRIPTION : Set the output of the D2A (0)
 *
 ****************************************************************************/
void D2A0_Write(WORD NewValue)
{
 //Dummy
   NewValue = 0 ;
}

/****************************************************************************
 *
 *  NAME        : D2A1_Init
 *
 *  DESCRIPTION : Initialization of the D/A (1) driver 
 *
 ****************************************************************************/
void D2A1_Init()
{
 //Dummy


}


/****************************************************************************
 *
 *  NAME        : D2A1_Write
 *
 *  DESCRIPTION : Set the output of the D2A (1)
 *
 ****************************************************************************/
void D2A1_Write(WORD NewValue)
{
  //Dummy
  NewValue = 0;

}


// Set the output of the one of the D2A's 
void D2A_Write(BYTE DeviceId, WORD Value)
{
  //Dummy
 DeviceId = 0;
 Value = 0;
}

