#include "stdafx.h"
#include "FCBApi.h"

int FCB_LIB FTDIinit()
{
	return TRUE;
}

int FCB_LIB FCBInit(int FCBNum)
{
	return (FCBNum >= 0 && FCBNum <= 1) ? TRUE : FALSE;
}

int FCB_LIB FCBGetInPlaceStatus(int FCBNum, int *status)
{
	if (FCBNum >= 0 && FCBNum <= 1)
	{
		*status = 0x000000FF;
		return TRUE;
	}

	*status = 0;
	return FALSE;
}

int FCB_LIB FCBSelectChannel(int FCBNum, int cartridgeNum)
{
	return (FCBNum >= 0 && FCBNum <= 1 && cartridgeNum >= 0 && cartridgeNum <= 7) ? TRUE : FALSE;
}

int FCB_LIB FCBSendData(int bufferSize, char *buffer)
{
	return TRUE;
}

int FCB_LIB FCBReceiveData(int bufferSize, char *buffer)
{
	return TRUE;
}