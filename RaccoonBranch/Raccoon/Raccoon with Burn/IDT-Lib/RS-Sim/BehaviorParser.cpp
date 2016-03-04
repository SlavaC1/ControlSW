#include "stdafx.h"
#include "BehaviorParser.h"
//#include <bits/basic_string.h>
#include <vector>
#include <string>

using namespace std;

namespace RSSim
{
	BehaviorParser::BehaviorParser(void)
	{
	}


	BehaviorParser::~BehaviorParser(void)
	{
	}

	Behavior* BehaviorParser::ParseBehavior(const char *path)
	{
		Behavior *behavior = NULL;
		try
		{
			XMLDocument doc;
			doc.LoadFile(path);
			XMLElement* root = doc.RootElement();
			if (root != NULL)
			{
				behavior = new Behavior();

				XMLElement* cartridgesEl = root->FirstChildElement("Cartridges");
				if (cartridgesEl != NULL)
				{
					int cartridgesCount;
					bool state;
					int xmlError = cartridgesEl->QueryAttribute("Count", &cartridgesCount);
					if (xmlError == XML_NO_ATTRIBUTE)
					{
						cartridgesCount = 16;
					}

					xmlError = cartridgesEl->QueryBoolAttribute("State", &state);
					if (xmlError == XML_NO_ATTRIBUTE)
					{
						state = true;
					}

					behavior->CartridgesCount = (unsigned char)cartridgesCount;
					behavior->Status = state
						? (1 << cartridgesCount) - 1
						: 0;

					XMLElement* lastCartridgeEl = cartridgesEl->LastChildElement();
					XMLElement* cartridgeEl = cartridgesEl->FirstChildElement();
					while (cartridgeEl != NULL)
					{
						unsigned int uiCartridgeNum;
						cartridgeEl->QueryUnsignedAttribute("Num", &uiCartridgeNum);
						XMLElement* stateEl = cartridgeEl->FirstChildElement();
						stateEl->QueryBoolText(&state);
						if (state)
						{
							behavior->Status |= (1 << uiCartridgeNum);
						}
						else
						{
							behavior->Status &= ~(1 << uiCartridgeNum);
						}

						cartridgeEl = (cartridgeEl == lastCartridgeEl) ? NULL : cartridgeEl->NextSiblingElement();
					}
				}

				vector<Function*> functions;
				XMLElement* lastFunctionEl = root->LastChildElement("Method");
				XMLElement* functionEl = root->FirstChildElement("Method");
				while (functionEl != NULL)
				{
					Function* function = ParseFunction(functionEl);
					if (function != NULL)
					{
						functions.push_back(function);
					}

					functionEl = (functionEl == lastFunctionEl) ? NULL : functionEl->NextSiblingElement("Method");
				}

				behavior->FunctionsCount = functions.size();
				behavior->Functions = new Function*[behavior->FunctionsCount];
				for (size_t i = 0; i < behavior->FunctionsCount; i++)
				{
					behavior->Functions[i] = functions[i];
				}
			}
		}
		catch (...)
		{
		}

		return behavior;
	}

	Function* BehaviorParser::ParseFunction(XMLElement* functionEl)
	{
		Function* function = new Function();
		const XMLAttribute* nameAttr = functionEl->FirstAttribute();
		function->Name = nameAttr->Value();

		const XMLAttribute* paramAttr = nameAttr->Next();
		vector< pair< string, string > > parameters;
		while (paramAttr != NULL)
		{
			parameters.push_back(pair<string, string>(paramAttr->Name(), paramAttr->Value()));
			paramAttr = paramAttr->Next();
		}

		function->ParametersCount = parameters.size();
		function->Parameters = new pair<string, string>[function->ParametersCount];
		for (size_t i = 0; i < function->ParametersCount; i++)
		{
			function->Parameters[i] = parameters[i];
		}

		XMLElement* returnValueEl = functionEl->FirstChildElement("ReturnValue");
		string result = returnValueEl->GetText();
		function->ReturnValue = (int)strtol(result.c_str(), NULL, 16);

		return function;
	}
}
