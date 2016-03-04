#include "stdafx.h"
#include "Test.h"

namespace RSTesterBL
{
	Test::Test(void)
	{
		Scenarios = NULL;
		ScenariosCount = 0;
		DefaultParameters = NULL;
		DefaultParametersCount = 0;
	}

	Test::Test(const Test &test)
	{
		Cleanup();

		ScenariosCount = test.ScenariosCount;
		Scenarios = new Scenario*[ScenariosCount];
		for (size_t i = 0; i < ScenariosCount; i++)
		{
			Scenarios[i] = new Scenario();
			*Scenarios[i] = *test.Scenarios[i];
		}

		DefaultParametersCount = test.DefaultParametersCount;
		DefaultParameters = new pair<string, string>[DefaultParametersCount];
		for (size_t i = 0; i < DefaultParametersCount; i++)
		{
			DefaultParameters[i] = test.DefaultParameters[i];
		}
	}

	Test::~Test(void)
	{
		Cleanup();
	}

	size_t Test::Encode(unsigned char *destination) const
	{
		size_t offset = 0;
		memcpy(destination + offset, &ScenariosCount, sizeof(ScenariosCount)); offset += sizeof(ScenariosCount);
		for (size_t i = 0; i < ScenariosCount; i++)
		{
			offset += Scenarios[i]->Encode(destination + offset);
		}

		memcpy(destination + offset, &DefaultParametersCount, sizeof(DefaultParametersCount)); offset += sizeof(DefaultParametersCount);
		for (size_t i = 0; i < DefaultParametersCount; i++)
		{
			strcpy((char *)(destination + offset), DefaultParameters[i].first.c_str());
			offset += DefaultParameters[i].first.length() + 1;

			strcpy((char *)(destination + offset), DefaultParameters[i].second.c_str());
			offset += DefaultParameters[i].second.length() + 1;
		}

		return offset;
	}

	size_t Test::Decode(const unsigned char *source)
	{
		Cleanup();

		size_t offset = 0;
		memcpy(&ScenariosCount, source + offset, sizeof(ScenariosCount)); offset += sizeof(ScenariosCount);
		if (ScenariosCount == 0)
		{
			Scenarios = NULL;
		}
		else
		{
			Scenarios = new Scenario*[ScenariosCount];
			for (size_t i = 0; i < ScenariosCount; i++)
			{
				Scenarios[i] = new Scenario();
				offset += Scenarios[i]->Decode(source + offset);
			}
		}

		if (DefaultParametersCount == 0)
		{
			DefaultParameters = NULL;
		}
		else
		{
			DefaultParameters = new pair<string, string>[DefaultParametersCount];
			for (size_t i = 0; i < DefaultParametersCount; i++)
			{
				DefaultParameters[i].first = string((char*)(source + offset));
				offset += DefaultParameters[i].first.length() + 1;

				DefaultParameters[i].second = string((char*)(source + offset));
				offset += DefaultParameters[i].second.length() + 1;
			}
		}

		return offset;
	}

	void Test::Cleanup()
	{
		if (Scenarios != NULL)
		{
			delete[] Scenarios;
		}

		if (DefaultParameters != NULL)
		{
			delete[] DefaultParameters;
		}
	}
}