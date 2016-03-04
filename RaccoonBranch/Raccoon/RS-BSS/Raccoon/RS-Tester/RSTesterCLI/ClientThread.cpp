#include "stdafx.h"
#include "ClientThread.h"
#include "log4cplus/configurator.h"
#include "log4cplus/socketappender.h"
#include "log4cplus/spi/loggingevent.h"

using namespace log4cplus;
using namespace log4cplus::helpers;
using namespace log4cplus::spi;

namespace RSTesterCLI
{
	void ClientThread::run()
	{
		while (true)
		{
			if (!clientsock.isOpen())
			{
				return;
			}

			SocketBuffer msgSizeBuffer(sizeof(unsigned int));
			if(!clientsock.read(msgSizeBuffer))
			{
				return;
			}

			unsigned int msgSize = msgSizeBuffer.readInt();
			SocketBuffer buffer(msgSize);
			if (!clientsock.read(buffer))
			{
				return;
			}

			InternalLoggingEvent event = readFromBuffer(buffer);
			Logger logger = Logger::getInstance(event.getLoggerName());
			logger.callAppenders(event);
		}
	}
}