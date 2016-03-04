#pragma once

#include <vector>
#include <string>
#include <utility>
#include "Function.h"

using namespace std;

namespace RSSim
{

#define MAX_CARTRIDGES_COUNT	16

	class Behavior
	{
	public:
		Behavior(void);
		~Behavior(void);

		int GetReturnValue(string name, vector< pair< string , string> > parameters);

		Function** Functions;
		size_t FunctionsCount;
		unsigned char CartridgesCount;
		int Status;
	};
}
