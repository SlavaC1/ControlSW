#pragma once

#include "stdafx.h"
#include "Scenario.h"

namespace RSTesterBL
{
	class Test
	{
	public:
		Test();
		Test(const Test &);
		~Test();

		size_t Encode(unsigned char *destination) const;
		size_t Decode(const unsigned char *source);

		Scenario** Scenarios;
		size_t ScenariosCount;
		pair<string, string>* DefaultParameters;
		size_t DefaultParametersCount;

	private:
		void Cleanup();
	};
}