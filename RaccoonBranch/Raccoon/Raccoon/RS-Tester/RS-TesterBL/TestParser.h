#pragma once

#include "Test.h"
#include "tinyxml2.h"

using namespace tinyxml2;

namespace RSTesterBL
{
	class TestParser
	{
	public:
		TestParser(void);
		~TestParser(void);

		Test* ParseTest(const char *path);
	private:

		Scenario* ParseScenario(XMLElement* scenarioEl);
		Method* ParseMethod(XMLElement* methodEl);
	};
}