#pragma once

#include "stdafx.h"
#include "Method.h"

namespace RSTesterBL
{
	class Scenario
	{
	public:
		Scenario();
		Scenario(const Scenario &);
		~Scenario();

		size_t Encode(unsigned char *destination) const;
		size_t Decode(const unsigned char *source);

		Method** Methods;
		size_t MethodsCount;

	private:
		void Cleanup();
	};
}