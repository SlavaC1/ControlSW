
#ifndef _CUSTOM_RESIN_SELECT_ADVANCED_SETTINGS_PAGE_H_
#define _CUSTOM_RESIN_SELECT_ADVANCED_SETTINGS_PAGE_H_

#include "WizardPages.h"
#include "GlobalDefs.h"

const TWizardPageType wptCustResinSelectionAdvancedWizardPage = wptCustom + 2012;

class CResinSelectionAdvancedWizardPage : public CWizardPage {
private:
	bool		  AutoMode;
	bool		  ForceFlushingEnabled;

public:
	CResinSelectionAdvancedWizardPage(
			  const QString&        Title,
			  int                   ImageID        = -1,
			  TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
		CWizardPage(Title,ImageID,PageAttributes),
		AutoMode(true),
		ForceFlushingEnabled(false)
    {}

    ~CResinSelectionAdvancedWizardPage() {}

	void Setup				()
	{
		SetAttributes(true, false);
	}

	bool IsAutomatic			() 	{ return AutoMode; 				}
	bool IsFlushingMode			()  { return ForceFlushingEnabled; 	}

	void SetAttributes		(bool AutomaticMode, bool FlushingMode)
	{
		AutoMode = AutomaticMode;
		ForceFlushingEnabled = FlushingMode;
	}

	TWizardPageType GetPageType()
    {
      return wptCustResinSelectionAdvancedWizardPage;
    }
};

#undef DEFAULT_PERIOD_DIRTY_PIPES_WHILE_PRINTING
#endif	//_CUSTOM_RESIN_SELECT_ADVANCED_SETTINGS_PAGE_H_

