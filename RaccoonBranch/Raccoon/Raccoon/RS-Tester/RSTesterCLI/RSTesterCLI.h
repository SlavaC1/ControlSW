// RSTesterCLI.h

#pragma once

#include "LogEventArgs.h"

namespace RSTesterCLI {

	public delegate void LogEventHandler(System::Object^ sender, LogEventArgs^ e);

	public ref class RSTesterUICLI
	{
	public:
		RSTesterUICLI();
		~RSTesterUICLI();
		bool Connect(System::String^ ipAddress);
		void Disconnect();
		void GetTestFiles(System::Text::StringBuilder^ sb);
		void RunTests(System::Collections::Generic::IEnumerable<System::String^>^ fileNames);
		event LogEventHandler^ LogArrived;

	private:		
		void* m_context;
		void* m_requester;
		bool m_connected;

		const unsigned char TestFilesReq;
		const unsigned char LogAck;
		const unsigned char RunTestEnd;
	};
}
