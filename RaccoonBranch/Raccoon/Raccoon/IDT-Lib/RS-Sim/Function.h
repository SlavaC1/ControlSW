#pragma once

#include <string>
#include <utility>

using namespace std;

namespace RSSim
{
	class Function
	{
	public:
		Function(void);
		~Function(void);

		string Name;
		pair<string, string>* Parameters;
		size_t ParametersCount;
		int ReturnValue;
	};
}
