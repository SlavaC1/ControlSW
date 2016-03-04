#pragma once

#include "log4cplus/config.hxx"
#include "log4cplus/appender.h"

using namespace log4cplus;

namespace RSCommon
{
	class CallbackAppender : public Appender
	{
	public:
		CallbackAppender(void(*logMessageFunction)(const char*));
		CallbackAppender(void(*logMessageFunction)(const char*), const helpers::Properties & properties);
		~CallbackAppender();

		virtual void close();

	protected:
		virtual void append(const spi::InternalLoggingEvent& event);

	private:
		void(*m_logMessageFunction)(const char*);
	};
}
