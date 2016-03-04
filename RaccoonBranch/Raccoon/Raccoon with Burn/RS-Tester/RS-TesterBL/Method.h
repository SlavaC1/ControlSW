#pragma once

#include "stdafx.h"
#include <vector>
#include <string>
#include <utility>

using namespace std;

#ifdef _WIN32
#	ifdef _LIB
#		define RSTESTER_BL_LIB
#	elif RSTESTER_BL_EXPORTS
#       define RSTESTER_BL_LIB __declspec(dllexport)
#   elif defined(RSTESTER_BL_IMPORT)
#       define RSTESTER_BL_LIB __declspec(dllimport)
#   else
#       define RSTESTER_BL_LIB
#   endif
#else
#   define RSTESTER_BL_LIB
#endif

namespace RSTesterBL
{
	class Method
	{
	public:
		Method(void);
		Method(const Method &);
		~Method(void);

		size_t Encode(unsigned char *destination) const;
		size_t Decode(const unsigned char *source);

		string Name;
		pair<string, string>* Parameters;
		size_t ParametersCount;
		string ExpectedResult;
		// true to repeat execution.
		bool Repeating;
		// Interval between execution in milliseconds.
		int Interval;
		// Timout for executions in seconds.
		int Timeout;
		bool Status;

	private:
		void Cleanup();
	};
}
