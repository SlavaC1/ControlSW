/*===========================================================================
 *   FILENAME       : Bumper  {Bumper.C}  
 *   PURPOSE        : Bumper impact handler module  
 *   DATE CREATED   : 17/08/2003
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/
#ifdef OCB_SIMULATOR
	#include "c8051F120.h"
#else
	#include "c8051F120.h" // Keep it here for consistency with OCB_SIMULATOR define
#endif

#include "Bumper.h"
#include "MiniScheduler.h"
#include "D2A.h"
#include "Comparator.h"
#include "MsgDecodeOHDB.h"

#ifdef OCB_SIMULATOR
	#include "EdenProtocol.h"
#else
	#include "EdenProtocolOHDB.h"
#endif


TTaskHandle xdata ImpactDetectionTaskHandle;
BYTE xdata Bumper0Impact;
BYTE xdata Bumper1Impact;
WORD xdata BumperResetTime;
BYTE xdata SetImpactCount;
BYTE xdata Bumper0CurrImpactCount;
BYTE xdata Bumper1CurrImpactCount;


// This task check if an impact occurred, sends a notification 
// in case of an impact and sleeps for 'Reset Time'
void Bumper_ImpactDetectionTask(BYTE Arg);

// Disable the bumper interrupt
void DisableBumperInterrupt();

// Enable the bumper interrupt
void EnableBumperInterrupt();


/****************************************************************************
 *
 *  NAME        : Bumper_Init
 *
 *  DESCRIPTION : Initialization of the bumper 
 *
 ****************************************************************************/
void Bumper_init()
{
 	// Initialize D2A 0 and 1 
	D2A0_Init();
	D2A1_Init();

	// Initialize compartors 0 and 1
	Comparator0_Init();
  	Comparator1_Init();

	// Set external interrupt 0 as edge triggered
  	IT0 = 1;

	// Set external interrupt 1 as edge triggered
  	IT1 = 1;	

  	ImpactDetectionTaskHandle = SchedulerInstallTask(Bumper_ImpactDetectionTask);
	
	Bumper0Impact = FALSE;
  	Bumper1Impact = FALSE;
	BumperResetTime = 0;
	SetImpactCount = 1;
	Bumper0CurrImpactCount = 0;
	Bumper1CurrImpactCount = 0;
}


/****************************************************************************
 *
 *  NAME        : Bumper_SetParameters
 *
 *  DESCRIPTION : Set the bumper parameters (sensitivity, reset time, impact count) 
 *
 ****************************************************************************/
void Bumper_SetParameters(WORD Sensitivity, WORD ResetTime, BYTE ImpactCount)
{
  	// Write the sensitivity to the D2A
  	D2A0_Write(Sensitivity);
	D2A1_Write(Sensitivity);

	BumperResetTime = ResetTime;
	SetImpactCount = ImpactCount;
}


/****************************************************************************
 *
 *  NAME        : Bumper_SetOnOff
 *
 *  DESCRIPTION : Set the bumper impact detection mechanism on/off 
 *
 ****************************************************************************/
void Bumper_SetOnOff(BOOL OnOff)
{
	if (OnOff)
	{
		Bumper0Impact = FALSE;
		Bumper1Impact = FALSE;
		Bumper0CurrImpactCount = 0;
		Bumper1CurrImpactCount = 0;
		
		IE0 = 0;
		IE1 = 0;

		EnableBumperInterrupt();
		
		// Resume the impact detection task
		SchedulerResumeTask(ImpactDetectionTaskHandle,0);
	}
	else
	{
		DisableBumperInterrupt();
		
		// Suspend the impact detection task
		SchedulerSuspendTask(ImpactDetectionTaskHandle);
	}
}

/****************************************************************************
 *
 *  NAME        : Bumper_ImpactDetectionTask
 *
 *  DESCRIPTION : This task check if an impact occurred, sends a notification 
 *                in case of an impact and sleeps for 'Reset Time'
 *
 ****************************************************************************/
void Bumper_ImpactDetectionTask(BYTE Arg)
{
	enum
	{
		DETECT_IMPACT,
		SEND_NOTIFICATION
	};

  	BOOL SendBumperImpactNotifiaction;

	switch (Arg)
	{
    	case DETECT_IMPACT:
	    	SendBumperImpactNotifiaction = FALSE;
			DisableBumperInterrupt();
			if(Bumper0Impact || Bumper1Impact)
			{
				if(Bumper0Impact)
				{
					Bumper0Impact = FALSE;
					if (Bumper0CurrImpactCount >= SetImpactCount)
					{
						Bumper0CurrImpactCount = 0;
						SendBumperImpactNotifiaction = TRUE;
					}
				}
				if (Bumper1Impact)
				{
					Bumper1Impact = FALSE;
					if (Bumper1CurrImpactCount >= SetImpactCount)
					{
						Bumper1CurrImpactCount = 0;
						SendBumperImpactNotifiaction = TRUE;    			
					}
				}
			}
      		EnableBumperInterrupt();
			if (SendBumperImpactNotifiaction)
			{
          		SchedulerLeaveTask(SEND_NOTIFICATION);

				// Fall to the next state
  			}
			else
			{
				SchedulerLeaveTask(DETECT_IMPACT);
				break;
			}

		case SEND_NOTIFICATION:
		{
			TBumperImpactDetectedMsg xdata Msg;
			
			Msg.MsgId = BUMPER_IMPACT_DETECTED;
			if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TBumperImpactDetectedMsg),EDEN_DEST_ID,0,FALSE) == EDEN_PROTOCOL_NO_ERROR)
			{
				SchedulerLeaveTask(DETECT_IMPACT);
				SchedulerTaskSleep(-1, BumperResetTime);
			}
			else
				SchedulerLeaveTask(SEND_NOTIFICATION);
			
			break;
		}

		default:
			SchedulerLeaveTask(DETECT_IMPACT);
			break;
	}
}


/****************************************************************************
 *
 *  NAME        : Bumper0Isr
 *
 *  DESCRIPTION : Bumper0 (external interrupt 0) ISR
 *
 ****************************************************************************/
#ifdef OCB_SIMULATOR
void Bumper0Isr() interrupt 25 using 3	  //originally interrupt 0(collision with OcbMain.c - void ExInterrupt_0_ISR())
#else
void Bumper0Isr() interrupt 0 using 3
#endif
{
	Bumper0Impact = TRUE;
	Bumper0CurrImpactCount++;
}


/****************************************************************************
 *
 *  NAME        : Bumper1Isr
 *
 *  DESCRIPTION : Bumper1 (external interrupt 0) ISR
 *
 ****************************************************************************/
#ifdef OCB_SIMULATOR
void Bumper1Isr() interrupt 26 using 3 //originally interrupt 2
#else
void Bumper1Isr() interrupt 2 using 3
#endif
{
	Bumper1Impact = TRUE;
	Bumper1CurrImpactCount++;
}

// Disable the bumper interrupt
void DisableBumperInterrupt()
{
	// Disable external interrupt 0
	EX0 = 0;
	
	// Disable external interrupt 1
	//  EX1 = 0;

}


// Enable the bumper interrupt
void EnableBumperInterrupt()
{
	// Enable external interrupt 0
	EX0 = 1;
	
	// Enable external interrupt 1
	//EX1 = 1;
}

