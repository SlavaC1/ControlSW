#pragma once

namespace RSTesterCLI
{
	public ref class LogEventArgs : System::EventArgs
	{
	public:
		LogEventArgs(System::String^ logEvent);

		System::String^ GetLogEvent();
	private:
		System::String^ m_logEvent;
	};

}