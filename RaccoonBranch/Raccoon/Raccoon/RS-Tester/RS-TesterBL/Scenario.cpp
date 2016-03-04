#include "stdafx.h"
#include "Scenario.h"

namespace RSTesterBL
{
	Scenario::Scenario(void)
	{
		Methods = NULL;
		MethodsCount = 0;
	}

	Scenario::Scenario(const Scenario &scenario)
	{
		Cleanup();

		MethodsCount = scenario.MethodsCount;
		if (MethodsCount == 0)
		{
			Methods = NULL;
		}
		else
		{
			Methods = new Method*[MethodsCount];
			for (size_t i = 0; i < MethodsCount; i++)
			{
				Methods[i] = new Method();
				*Methods[i] = *scenario.Methods[i];
			}
		}
	}

	Scenario::~Scenario(void)
	{
		Cleanup();
	}

	size_t Scenario::Encode(unsigned char *destination) const
	{
		size_t offset = 0;
		memcpy(destination + offset, &MethodsCount, sizeof(MethodsCount)); offset += sizeof(MethodsCount);
		for (size_t i = 0; i < MethodsCount; i++)
		{
			offset += Methods[i]->Encode(destination + offset);
		}

		return offset;
	}

	size_t Scenario::Decode(const unsigned char *source)
	{
		Cleanup();

		size_t offset = 0;
		memcpy(&MethodsCount, source + offset, sizeof(MethodsCount)); offset += sizeof(MethodsCount);
		if (MethodsCount == 0)
		{
			Methods = NULL;
		}
		else
		{
			Methods = new Method*[MethodsCount];
			for (size_t i = 0; i < MethodsCount; i++)
			{
				Methods[i] = new Method();
				offset += Methods[i]->Decode(source + offset);
			}
		}

		return offset;
	}

	void Scenario::Cleanup()
	{
		if (Methods != NULL)
		{
			delete[] Methods; 
		}
	}
}