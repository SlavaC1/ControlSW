#include "stdafx.h"
#include "Method.h"

namespace RSTesterBL
{
	Method::Method(void)
	{
		Parameters = NULL;
		ParametersCount = 0;
		Interval = 1000;
		Repeating = false;
		Timeout = 0;
		Status = false;
	}

	Method::Method(const Method &method)
	{
		Cleanup();

		Name = method.Name;
		ExpectedResult = method.ExpectedResult;
		ParametersCount = method.ParametersCount;
		Parameters = new pair<string, string>[ParametersCount];
		for (size_t i = 0; i < ParametersCount; i++)
		{
			Parameters[i] = method.Parameters[i];
		}

		Interval = method.Interval;
		Repeating = method.Repeating;
		Timeout = method.Timeout;
		Status = method.Status;
	}

	Method::~Method(void)
	{
		Cleanup();
	}

	size_t Method::Encode(unsigned char *destination) const
	{
		size_t offset = 0;
		strcpy((char *)(destination + offset), Name.c_str());
		offset += Name.length() + 1;
		strcpy((char *)(destination + offset), ExpectedResult.c_str());
		offset += ExpectedResult.length() + 1;

		memcpy(destination + offset, &Repeating, sizeof(Repeating)); offset += sizeof(Repeating);
		memcpy(destination + offset, &Timeout, sizeof(Timeout)); offset += sizeof(Timeout);
		memcpy(destination + offset, &Status, sizeof(Status)); offset += sizeof(Status);
		memcpy(destination + offset, &ParametersCount, sizeof(ParametersCount)); offset += sizeof(ParametersCount);
		for (size_t i = 0; i < ParametersCount; i++)
		{
			strcpy((char *)(destination + offset), Parameters[i].first.c_str());
			offset += Parameters[i].first.length() + 1;
			strcpy((char *)(destination + offset), Parameters[i].second.c_str());
			offset += Parameters[i].second.length() + 1;
		}

		return offset;
	}

	size_t Method::Decode(const unsigned char *source)
	{
		Cleanup();

		size_t offset = 0;
		Name = string((char *)source + offset);
		offset += strlen((char *)(source + offset)) + 1;
		ExpectedResult = string((char *)source + offset);
		offset += strlen((char *)(source + offset)) + 1;

		memcpy(&Interval, source + offset, sizeof(Interval)); offset += sizeof(Interval);
		memcpy(&Repeating, source + offset, sizeof(Repeating)); offset += sizeof(Repeating);
		memcpy(&Timeout, source + offset, sizeof(Timeout)); offset += sizeof(Timeout);
		memcpy(&Status, source + offset, sizeof(Status)); offset += sizeof(Status);
		memcpy(&ParametersCount, source + offset, sizeof(ParametersCount)); offset += sizeof(ParametersCount);
		if (ParametersCount > 0)
		{
			Parameters = new pair<string, string>[ParametersCount];
			for (size_t i = 0; i < ParametersCount; i++)
			{
				Parameters[i].first = string((char *)(source + offset));
				offset += Parameters[i].first.length() + 1;
				Parameters[i].second = string((char *)(source + offset));
				offset += Parameters[i].second.length() + 1;
			}
		}

		return offset;
	}

	void Method::Cleanup()
	{
		if (Parameters != NULL)
		{
			delete[] Parameters;
			Parameters = NULL;
		}
	}
}
