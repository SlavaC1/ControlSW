//---------------------------------------------------------------------------


#pragma hdrstop

#include "ProgressReporting.h"
#include <math.h>


#ifndef _DEBUG
#define NDEBUG
#endif
#include <assert.h>






// Default Constructor
ProgressReporting::CSerialProgressHandlerBase::CSerialProgressHandlerBase() :
	_completedTasksPercentage(0),
    _currentTaskCompletedPercentage(0),
    _currentTaskAbsoluteProgressShare(100.0)
{
}

void ProgressReporting::CSerialProgressHandlerBase::reportProgressPercentage(TQPercentage progress)
{
	_currentTaskCompletedPercentage = progress;
	onProgressReport(getProgressPercentage());
}

void ProgressReporting::CSerialProgressHandlerBase::reportStatus(const QString &status)
{
	onStatusReport(status);
}

TQPercentage ProgressReporting::CSerialProgressHandlerBase::getProgressPercentage()
{
	return _completedTasksPercentage + _currentTaskAbsoluteProgressShare * _currentTaskCompletedPercentage / 100.0;
}

void ProgressReporting::CSerialProgressHandlerBase::setNextTaskAbsoluteProgressShare(TQPercentage percent)
{
     if (std::fabs(100.0 - _currentTaskCompletedPercentage) < 1)
	     _completedTasksPercentage += _currentTaskAbsoluteProgressShare;

	assert(percent <= (100.0 - _completedTasksPercentage));

	if ((100.0 - _completedTasksPercentage) < percent) // in release, progress reporting should neither throw (because it is not that important) nor result in inconsistent progress
		percent = (100.0 - _completedTasksPercentage);


	_currentTaskAbsoluteProgressShare = percent;
    _currentTaskCompletedPercentage = 0;
	
}

void ProgressReporting::CSerialProgressHandlerBase::resetProgress()
{
	_completedTasksPercentage = 0;
    _currentTaskCompletedPercentage = 0;
    _currentTaskAbsoluteProgressShare = 100.0;
    onProgressReport(0);
}
//---------------------------------------------------------------------------

#pragma package(smart_init)
