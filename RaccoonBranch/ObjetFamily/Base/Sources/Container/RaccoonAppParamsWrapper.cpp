/*
	This class is needed because of some type ambiguity between CryptoPP and System
*/    

#pragma hdrstop
#include "RaccoonAppParamsWrapper.h"
#pragma package(smart_init)

#include "AppParams.h"

using namespace Raccoon;

bool CRaccoonAppParamsWrapper::IsLowLevelLoggingEnabled()
{
    return CAppParams::Instance()->EnableLowLevelLogging; 
}



