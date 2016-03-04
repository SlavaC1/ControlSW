#pragma once

#include "stdafx.h"
#include "TestManager.h"

typedef struct
{
    pair<string, string>* Parameters;
    size_t ParametersCount;
    pair<string, string>* DefaultParameters;
    size_t DefaultParametersCount;
    int ExpectedResult;
    int* Result;
} TestParameters;

typedef struct
{
    TestManager* TestManager;
    Method* Method;
    pair<string, string>* DefaultParameters;
    size_t DefaultParametersCount;
} ExecuteMethodParameters;

