#include "stdafx.h"
#include "LogEventArgs.h"

namespace RSTesterCLI
{
	LogEventArgs::LogEventArgs(System::String^ logEvent)
	{
		m_logEvent = logEvent;
	}

	System::String^ LogEventArgs::GetLogEvent()
	{
		return m_logEvent;
	}
}