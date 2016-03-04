#pragma once

#include "tinyxml2.h"
#include "Behavior.h"

using namespace tinyxml2;
using namespace std;

namespace RSSim
{
	class BehaviorParser
	{
	public:
		BehaviorParser(void);
		~BehaviorParser(void);

		static Behavior* ParseBehavior(const char *path);

	private:
		static Function* ParseFunction(XMLElement* functionEl);
	};
}