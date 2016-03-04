#include "stdafx.h"
#include "Function.h"

namespace RSSim
{
	Function::Function(void)
		: Parameters(NULL), ParametersCount(0), ReturnValue(0)
	{
	}


	Function::~Function(void)
	{
		if (Parameters != NULL)
		{
			delete[] Parameters;
		}
	}
}
