#pragma once

#include "log4cplus/thread/threads.h"
#include "log4cplus/helpers/socket.h"

namespace RSTesterCLI
{
	class ClientThread : public log4cplus::thread::AbstractThread
	{
	public:
		ClientThread(log4cplus::helpers::Socket clientsock_)
			: clientsock(std::move (clientsock_))
		{
		}
		~ClientThread();

		virtual void run();

	private:
		log4cplus::helpers::Socket clientsock;
	};
}