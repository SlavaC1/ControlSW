#pragma once

#include "log4cplus/config.hxx"
#include "log4cplus/appender.h"

using namespace log4cplus;

namespace RSTesterBLApp
{
	class ZmqAppender : public Appender
	{
	public:
		ZmqAppender(void* responder);
		ZmqAppender(void* responder, const helpers::Properties & properties);
		~ZmqAppender();

		virtual void close();

	protected:
        virtual void append(const spi::InternalLoggingEvent& event);

	private:
		ZmqAppender(const ZmqAppender&);
		ZmqAppender& operator=(const ZmqAppender&);

		void* m_responder;
	};
}