// This is the main DLL file.

#include "stdafx.h"
#include "zmq.h"

#include "RSTesterCLI.h"

using namespace System;
using namespace System::Text;
using namespace System::Runtime::InteropServices;

namespace RSTesterCLI {
	RSTesterUICLI::RSTesterUICLI()
		: TestFilesReq(1), LogAck(2), RunTestEnd(3), m_connected(false)
	{
		m_context = NULL;
		m_requester = NULL;
	}

	RSTesterUICLI::~RSTesterUICLI()
	{
		Disconnect();
	}

	bool RSTesterUICLI::Connect(String^ ipAddress)
	{
		m_context = zmq_ctx_new();
		m_requester = zmq_socket(m_context, ZMQ_REQ);
		if (m_requester == NULL)
		{
			return false;
		}

		IntPtr pIPAddress = Marshal::StringToHGlobalAnsi(ipAddress);
		int rc = zmq_connect(m_requester, (const char*)pIPAddress.ToPointer());
		Marshal::FreeHGlobal(pIPAddress);

		m_connected = (rc == 0);
		return m_connected;
	}

	void RSTesterUICLI::Disconnect()
	{
		m_connected = false;

		if (m_requester != NULL)
		{
			zmq_close(m_requester);
			m_requester = NULL;
		}

		if (m_context != NULL)
		{
			zmq_ctx_destroy(m_context);
			m_context = NULL;
		}
	}

	void RSTesterUICLI::GetTestFiles(StringBuilder^ sb)
	{
		if (!m_connected)
		{
			return;
		}

		const size_t BufferLength = 0xffff;
		char buffer[BufferLength];
		unsigned char controlByte = TestFilesReq;
		zmq_send(m_requester, &controlByte, 1, 0);
		int received;
		do
		{
			received = zmq_recv(m_requester, buffer, BufferLength, 0);
			if (received > 0)
			{
				if (received == 1)
				{
					zmq_send(m_requester, &controlByte, 1, 0);
				}
				else
				{
					String^ message = gcnew String(buffer, 0, received);
					sb->Append(message);
				}
			}
		} while (received == BufferLength || received == 1);
	}

	void RSTesterUICLI::RunTests(System::Collections::Generic::IEnumerable<System::String^>^ fileNames)
	{
		if (!m_connected)
		{
			return;
		}

		String^ joindFileNames = String::Join(Environment::NewLine, fileNames);
		void* message = Marshal::StringToHGlobalAnsi(joindFileNames).ToPointer();
		zmq_send(m_requester, message, joindFileNames->Length + 1, 0);

		Marshal::FreeHGlobal(IntPtr(message));

		message = new char[0xffff];
		char controlByte = LogAck;
		memset(message, 0, 0xffff);
		while (m_connected)
		{
			int numOfBytes = zmq_recv(m_requester, message, 0xffff, 0);
			if (numOfBytes <= 0)
			{
				break;
			}

			LogArrived(this, gcnew LogEventArgs(gcnew String((char *)message, 0, numOfBytes)));
			if (!m_connected || ((char *)message)[0] == RunTestEnd)
			{
				break;
			}

			zmq_send(m_requester, &controlByte, 1, 0);
		}

		delete[] message;
	}
}