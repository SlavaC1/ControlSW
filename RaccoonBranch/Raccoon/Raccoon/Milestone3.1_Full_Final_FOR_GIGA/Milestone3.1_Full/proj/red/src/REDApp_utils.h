#ifndef REDAPP_UTILS_H
#define REDAPP_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>

/* some type definitions to make old MSVC compiler aware of C99 types */
#if (_MSC_VER < 1300)
typedef signed char     int8_t;
typedef signed short    int16_t;
typedef signed int      int32_t;
typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned int    uint32_t;
#else
typedef signed __int8     int8_t;
typedef signed __int16    int16_t;
typedef signed __int32    int32_t;
typedef unsigned __int8   uint8_t;
typedef unsigned __int16  uint16_t;
typedef unsigned __int32  uint32_t;
#endif

#include "hasp_api.h"
#include "hasp_io_buffer.h"

#define REDAppResourceId            1

enum REDAppMethodIds
{
  REDAppMethodId_init               = 0,
  REDAppMethodId_verifyTag          = 1,
  REDAppMethodId_verifyChallenge    = 2,
  REDAppMethodId_removeTag          = 3,
  REDAppMethodId_updateConsumption  = 4,
  REDAppMethodId_verifyWeight       = 5,
  REDAppMethodId_CNOF               = 6,
  REDAppMethodId_CEOP               = 7,
  REDAppMethodId_CNOP               = 8,
  REDAppMethodId_CYSO               = 9,
};

/* Basic AoC service methods */
hasp_status_t openAoC(hasp_handle_t *handle);
void closeAoC(hasp_handle_t handle);

/* REDApp wrappers */
hasp_status_t verifyTag(hasp_handle_t handle, uint8_t tagNo, int isActive, const uint8_t *certificate, unsigned char certificateLen, uint8_t *random, int *result);
hasp_status_t verifyChallenge(hasp_handle_t handle, uint8_t tagNo, int isActive, const uint8_t *signedChallenge, unsigned char signedChallengeLen, int *result);
hasp_status_t removeTag(hasp_handle_t handle, uint8_t tagNo, int *result);
hasp_status_t updateConsumption(hasp_handle_t handle, uint8_t tagNo, int isActive, int consumption, uint8_t *random, int *result);
hasp_status_t verifyWeight(hasp_handle_t handle, uint8_t tagNo, int isActive, const uint8_t *signedWeight, unsigned char signedWeightLen, int *result);
hasp_status_t CNOF(hasp_handle_t handle, int SW, int SR, int LHO1200, int SRX, int IEF, int *result);
hasp_status_t CEOP(hasp_handle_t handle, int SOP, int SW, int SR, int LHO1200, int AF, int IEF, int *result);
hasp_status_t CNOP(hasp_handle_t handle, int SH, int SST, int SYO, int HPW, int SPEO, int CH, int *result);
hasp_status_t CYSO(hasp_handle_t handle, int SH, int SST, int SYO, int HPW, int NGIP, int NOP, int SPEO, int YINOP, int YSINOP, int *result);

#ifdef __cplusplus
}
#endif

#endif /* #ifdef REDAPP_UTILS_H */
