#include "stdafx.h"
#include "TestParser.h"
#include "tinyxml2.h"
#include <vector>
#include <math.h>

using namespace std;
using namespace tinyxml2;

namespace RSTesterBL
{
	TestParser::TestParser(void)
	{
	}


	TestParser::~TestParser(void)
	{

	}

	Test* TestParser::ParseTest(const char *path)
	{
		Test *test = NULL;
		try
		{		
			XMLDocument doc;
			doc.LoadFile(path);
			XMLElement* root = doc.RootElement();
			if (root != NULL)
			{
				test = new Test();

				XMLElement* defaultParametersEl = root->FirstChildElement("Defaults");
				if (defaultParametersEl != NULL)
				{
					XMLElement* lastDefaultParam = defaultParametersEl->LastChildElement();
					XMLElement* defaultParam = defaultParametersEl->FirstChildElement();
					vector< pair<string, string> > defaultParameters;
					while (defaultParam != NULL)
					{
						pair<string, string> parameter;
						parameter.first = defaultParam->Value();
						parameter.second = defaultParam->GetText();
						defaultParameters.push_back(parameter);

						defaultParam = (defaultParam == lastDefaultParam) ? NULL : defaultParam->NextSiblingElement();
					}

					test->DefaultParametersCount = defaultParameters.size();
					test->DefaultParameters = new pair<string, string>[test->DefaultParametersCount];
					for (size_t i = 0; i < test->DefaultParametersCount; i++)
					{
						test->DefaultParameters[i] = defaultParameters[i];
					}
				}

				vector<Scenario*> scenarios;
				XMLElement* lastScenarioEl = root->LastChildElement("Scenario");
				XMLElement* scenarioEl = root->FirstChildElement("Scenario");
				while (scenarioEl != NULL)
				{
					Scenario* scenario = ParseScenario(scenarioEl);
					if (scenario != NULL)
					{
						scenarios.push_back(scenario);
					}

					scenarioEl = (scenarioEl == lastScenarioEl) ? NULL : scenarioEl->NextSiblingElement("Scenario");
				}

				test->ScenariosCount = scenarios.size();
				test->Scenarios = new Scenario*[test->ScenariosCount];
				for (size_t i = 0; i < test->ScenariosCount; i++)
				{
					test->Scenarios[i] = scenarios[i];
				}
			}
		}
		catch (...)
		{
		}

		return test;
	}

	Scenario* TestParser::ParseScenario(XMLElement* scenarioEl)
	{
		Scenario *scenario = new Scenario();
		vector<Method*> methods;
		XMLElement* lastMethodEl = scenarioEl->LastChildElement();
		XMLElement* methodEl = scenarioEl->FirstChildElement();
		while (methodEl != NULL)
		{
			Method *method = ParseMethod(methodEl);
			if (method != NULL)
			{
				methods.push_back(method);
			}

			methodEl = (methodEl == lastMethodEl) ? NULL : methodEl->NextSiblingElement();
		}

		scenario->MethodsCount = methods.size();
		scenario->Methods = new Method*[scenario->MethodsCount];
		for (size_t i = 0; i < scenario->MethodsCount; i++)
		{
			scenario->Methods[i] = methods[i];
		}
		return scenario;
	}

	Method* TestParser::ParseMethod(XMLElement* methodEl)
	{
		Method *method = new Method();
		vector< pair<string, string> > parameters;

		method->Name = methodEl->Name();
		method->ExpectedResult = methodEl->Attribute("ExpectedResults");

		methodEl->QueryBoolAttribute("Repeating", &method->Repeating);
		methodEl->QueryIntAttribute("Timeout", &method->Timeout);
		float interval = 1;
		methodEl->QueryFloatAttribute("Interval", &interval);
		method->Interval = (int)floorf(1000 * interval + 0.5f);

		XMLElement* lastDefaultParameterEl = methodEl->LastChildElement();
		XMLElement* parameterEl = methodEl->FirstChildElement();
		while (parameterEl != NULL)
		{
			pair<string, string> parameter;
			parameter.first = parameterEl->Value();
			parameter.second = parameterEl->GetText();
			parameters.push_back(parameter);
			parameterEl = (parameterEl == lastDefaultParameterEl) ? NULL : parameterEl->NextSiblingElement();
		}

		method->ParametersCount = parameters.size();
		method->Parameters = new pair<string, string>[method->ParametersCount];
		for (size_t i = 0; i < method->ParametersCount; i++)
		{
			method->Parameters[i] = parameters[i];
		}
		return method;
	}
}
