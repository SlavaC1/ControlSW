#pragma once

#ifndef _FCB_API_H_
#define _FCB_API_H_

#if defined(WIN32) || defined(_WIN32)
#   ifdef FCB_EXPORTS
#       define FCB_LIB __declspec(dllexport)
#   elif defined(FCB_IMPORT)
#       define FCB_LIB __declspec(dllimport)
#   else
#       define FCB_LIB
#   endif
#else
#   define FCB_LIB
#endif

#ifdef __cplusplus
extern "C" {
#endif

int FCB_LIB FTDIinit();
int FCB_LIB FCBInit(int FCBNum);
int FCB_LIB FCBGetInPlaceStatus(int FCBNum, int *status);
int FCB_LIB FCBSelectChannel(int FCBNum, int cartridgeNum);
int FCB_LIB FCBSendData(int bufferSize, char *buffer);
int FCB_LIB FCBReceiveData(int bufferSize, char *buffer);

#ifdef __cplusplus
}
#endif

#endif

