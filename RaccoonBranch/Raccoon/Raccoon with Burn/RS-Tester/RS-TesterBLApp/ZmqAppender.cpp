#include "stdafx.h"
#include "zmq.h"
#include "log4cplus/streams.h"
#include "ZmqAppender.h"
#include <sstream>

using namespace log4cplus;

namespace RSTesterBLApp
{
	ZmqAppender::ZmqAppender(void *responder)
		: Appender(),
		m_responder(responder)
	{
	}

	ZmqAppender::ZmqAppender(void* responder, const log4cplus::helpers::Properties & properties)
		: Appender(properties),
		m_responder(responder)
	{

	}

	ZmqAppender::~ZmqAppender()
	{
		close();
	}

	void ZmqAppender::append(const log4cplus::spi::InternalLoggingEvent& event)
	{
		if (m_responder != NULL)
		{
			log4cplus::tostringstream oss;
			layout->formatAndAppend(oss, event);
			tstring formattedEvent = oss.str();

			zmq_send(m_responder, formattedEvent.c_str(), formattedEvent.length(), 0);
			char controlByte;
			zmq_recv(m_responder, &controlByte, 1, 0);
		}
	}

	void ZmqAppender::close()
	{
		m_responder = NULL;
		closed = true;
	}
}