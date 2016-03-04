#include "stdafx.h"
#include "CallbackAppender.h"
#include "log4cplus/streams.h"
#include <sstream>

using namespace log4cplus;

namespace RSCommon
{
	CallbackAppender::CallbackAppender(void(*logMessageFunction)(const char*))
		: Appender(),
		m_logMessageFunction(logMessageFunction)
	{
	}


	CallbackAppender::CallbackAppender(void(*logMessageFunction)(const char*), const helpers::Properties & properties)
		: Appender(properties),
		m_logMessageFunction(logMessageFunction)
	{
	}


	CallbackAppender::~CallbackAppender(void)
	{
		close();
	}

	void CallbackAppender::append(const log4cplus::spi::InternalLoggingEvent& event)
	{
		if (m_logMessageFunction != NULL)
		{
			log4cplus::tostringstream oss;
			layout->formatAndAppend(oss, event);
			m_logMessageFunction(oss.str().c_str());
		}
	}

	void CallbackAppender::close()
	{
		m_logMessageFunction = NULL;
		closed = true;
	}
}