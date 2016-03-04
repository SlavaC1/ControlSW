#include "stdafx.h"
#include "Behavior.h"
#include "RSCommonDefinitions.h"

namespace RSSim
{
	Behavior::Behavior(void)
		: Functions(NULL), FunctionsCount(0), CartridgesCount(MAX_CARTRIDGES_COUNT), Status((1 << MAX_CARTRIDGES_COUNT) - 1)
	{
		
	}

	Behavior::~Behavior(void)
	{
		if (Functions != NULL)
		{
			delete[] Functions;
		}
	}

	int Behavior::GetReturnValue(string name, vector< pair< string, string > > parameters)
	{
		int returnValue = IDTLIB_SUCCESS;
		for (size_t i = 0; i < FunctionsCount; i++)
		{
			// Check function name matching:
			if (Functions[i]->Name.compare(name) != 0)
			{
				continue;
			}

			// Check matching of all function parameters:
			// matching occurs if parameter name doesn't exist or both name and value are equal:
			bool skip = false;
			for (size_t j = 0; j < Functions[i]->ParametersCount; j++)
			{
				// Check parameters[j] name matching:
				string value;
				for (size_t k = 0; k < parameters.size(); k++)
				{
					if (Functions[i]->Parameters[j].first.compare(parameters[k].first) == 0)
					{
						value = parameters[k].second;
						break;
					}
				}

				// check parameters[j] value matching:
				if (Functions[i]->Parameters[j].second.compare(value) != 0)
				{
					skip = true;
					break;
				}
			}

			// if found match between name and all parameters
			if (!skip)
			{
				returnValue = Functions[i]->ReturnValue;
			}
		}

		return returnValue;
	}
}
