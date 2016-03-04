//---------------------------------------------------------------------------

#ifndef ProgressReportingH
#define ProgressReportingH

#include "QTypes.h"

/*
A use example of the ProgressListener can be found in the following revision. (revision 2320 actually removes this feature from the code)
Revision: 2320
Author: arcady
Date: 1:50:09 PM, Sunday, January 18, 2009
Message:
Removed all progress bar handling from ShutdownWizard
----
Modified : /Trunk/ConnexFamily/Base/Sources/Wizards/ShutdownWizard.cpp
*/

namespace ProgressReporting
{



class CProgressListenerInterface
{
public:
    virtual void reportProgressPercentage(TQPercentage progress) = 0;
    virtual void reportStatus(const QString &status) = 0;


};

class CSerialProgressHandlerBase : public CProgressListenerInterface
{
    TQPercentage			_completedTasksPercentage;
    TQPercentage			_currentTaskCompletedPercentage;
    TQPercentage			_currentTaskAbsoluteProgressShare;

protected:

    TQPercentage	getProgressPercentage(); // Returns the total percentage of all of the Tasks, including the current.
	virtual	void	onProgressReport(TQPercentage percent) = 0;
	virtual	void	onStatusReport(const QString &status) = 0;

public:

    CSerialProgressHandlerBase();
	virtual void	resetProgress(); // Resets all Task's prgress including the current Task, and sets the "Next Task" share to 100%.
    void			setNextTaskAbsoluteProgressShare(TQPercentage percent);
    virtual void	reportProgressPercentage(TQPercentage progress);
    virtual void	reportStatus(const QString &status);



};


}// namespace ProgressReporting


//---------------------------------------------------------------------------
#endif
