/*===========================================================================
 *   FILENAME       : Door {Door.h}  
 *   PURPOSE        : Door lock control and monitor (header file)
 *   DATE CREATED   : 24/06/2004
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/


#ifndef _DOOR_H_
#define _DOOR_H_

#include "Define.h"

typedef enum{
  DOOR_OPEN_UNLOCKED = 0,
  DOOR_CLOSE_UNLOCKED,
  DOOR_OPEN_LOCKED,
  DOOR_CLOSE_LOCKED,
  DOOR_SEND_NOTIFICATION
} DOOR_STATE; 

// ??? Note, do not order of this enum is important, since it is transfered to Embedded in response to 
// a DoorInputsStatus request.
typedef enum{
  INPUT_CMD_UNLOCK__DOOR_OPEN = 0,
  INPUT_CMD_UNLOCK__DOOR_CLOSED,
  INPUT_CMD_LOCK__DOOR_OPEN,
  INPUT_CMD_LOCK__DOOR_CLOSED 
} DOOR_INPUTS_STATUS; 

// Lock the door
void DoorLock();

// Unlock the door
void DoorUnLock();

// Initialization of the door interface
void DoorInit();

// Request a door lock or unlock
void DoorLockUnlockRequest(BOOL Lock);

// Get the current status of the door
DOOR_INPUTS_STATUS DoorInputsStatus(BYTE DoorID);

// Get door status. Called by: MsgDecode.c 
DOOR_STATE DoorGetStatus(BYTE door_id);

// Get the last door request 
BOOL DoorGetLastRequest(BYTE door_id);

// a helper function.
void NotificationBranch(BYTE afterNotificationState, BYTE DoorId);

// Init the door type
void InitDoorType();

#endif