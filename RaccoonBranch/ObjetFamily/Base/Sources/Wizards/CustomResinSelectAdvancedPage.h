
#ifndef _CUSTOM_RESIN_SELECT_ADVANCED_SETTINGS_PAGE_H_
#define _CUSTOM_RESIN_SELECT_ADVANCED_SETTINGS_PAGE_H_

#include "WizardPages.h"
#include "GlobalDefs.h"

const TWizardPageType wptCustResinSelectionAdvancedWizardPage = wptCustom + 2012;

class CResinSelectionAdvancedWizardPage : public CWizardPage {
private:
	bool		  AutoMode;
	bool		  ForceFlushingEnabled;
	int			  PreferredSingleResin;	//index, not TypeID

public:
    CResinSelectionAdvancedWizardPage(
			  const QString&        Title,
			  int                   ImageID        = -1,
			  TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
        CWizardPage(Title,ImageID,PageAttributes),
		PreferredSingleResin(0),
		AutoMode(true),
		ForceFlushingEnabled(false)
    {}

    ~CResinSelectionAdvancedWizardPage() {}

	void Setup				()
	{
		SetAttributes(true, false);
		SetPreferredResin(0);
    }

	bool IsAutomatic		() 	{ return AutoMode; 				}
	bool IsFlushingMode		() 	{ return ForceFlushingEnabled; 	}
	int	 GetPreferredResin	()  { return PreferredSingleResin; 	}
	
	void SetAttributes		(bool AutomaticMode, bool FlushingMode)
	{ 
		AutoMode = AutomaticMode; 
		ForceFlushingEnabled = FlushingMode; 
	}
	void SetPreferredResin	(int SelectedResin)  { PreferredSingleResin = SelectedResin; }

	TWizardPageType GetPageType()
    {
      return wptCustResinSelectionAdvancedWizardPage;
    }
};

#endif	//_CUSTOM_RESIN_SELECT_ADVANCED_SETTINGS_PAGE_H_
