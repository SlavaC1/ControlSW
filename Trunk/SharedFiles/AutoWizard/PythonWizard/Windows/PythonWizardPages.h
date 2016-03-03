/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Generic                                                 *
 * Module: Automatic wizards generator                              *
 * Module Description: Wizard pages implementation for python.      *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 30/06/2003                                           *
 * Last upate: 30/06/2003                                           *
 ********************************************************************/

#ifndef _PYTHON_WIZARD_PAGES_H_
#define _PYTHON_WIZARD_PAGES_H_

#include "WizardPages.h"
#include "VarPyth.hpp"

class CPythonWizardPageBase : virtual public CWizardPage // inerit this class together with a specific wizard page class to create a page class that supports python. See examples below.
{
	public:
    	virtual void RefreshPythonAttributes() = 0; // todo -oNobody -cNone: poor choise of identifiers: using the word "attributes" for both page behavior attributes and python page attributes
		virtual void Refresh(void)
		{
			RefreshPythonAttributes();
			CWizardPage::Refresh();
		}
		virtual bool Enter(void)
		{
			RefreshPythonAttributes();
			return CWizardPage::Enter();
		}
};


// Wizard message page
class CPythonMessageWizardPage : public CMessageWizardPage, public CPythonWizardPageBase {
  public:
    CPythonMessageWizardPage(const QString& Title,int ImageID = -1,
                       TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
    CWizardPage(Title,ImageID,PageAttributes)
      {

      }

    virtual void RefreshPythonAttributes();
};

// Wizard status page
class CPythonStatusWizardPage : public CPythonWizardPageBase , public CStatusWizardPage {
  public:
    CPythonStatusWizardPage(const QString& Title,int ImageID = -1,
                            TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
    CWizardPage(Title,ImageID,PageAttributes) {}

    virtual void RefreshPythonAttributes();
};

// Wizard progress page
class CPythonProgressWizardPage : public CPythonWizardPageBase , public CProgressWizardPage {
  public:
    CPythonProgressWizardPage(const QString& Title,int ImageID = -1,
                              TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
      CWizardPage(Title,ImageID,PageAttributes)
      {}

    virtual void RefreshPythonAttributes();
};

// Wizard checkboxes page
class CPythonCheckBoxWizardPage : public CPythonWizardPageBase , public CCheckBoxWizardPage {
  public:
    CPythonCheckBoxWizardPage(const QString& Title,int ImageID = -1,
                        TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
      CWizardPage(Title,ImageID,PageAttributes) {}

    // Override notify event function
    bool NotifyEvent(int Param1,int Param2);

    virtual void RefreshPythonAttributes();

};

// Wizard radiogroup page
class CPythonRadioGroupWizardPage : public CPythonWizardPageBase , public CRadioGroupWizardPage {
  public:
    CPythonRadioGroupWizardPage(const QString& Title,int ImageID = -1,
                                TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
      CWizardPage(Title,ImageID,PageAttributes) {}

    // Override notify event function
    bool NotifyEvent(int Param1,int Param2);

    virtual void RefreshPythonAttributes();
};

// Wizard progress status page
class CPythonProgressStatusWizardPage : public CPythonWizardPageBase , public CProgressStatusWizardPage {
  public:
    CPythonProgressStatusWizardPage(const QString& Title,int ImageID = -1,
                                    TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
      CWizardPage(Title,ImageID,PageAttributes) {}

    virtual void RefreshPythonAttributes();
};

// Generic custom wizard page
class CPythonGenericCustomWizardPage : public CPythonWizardPageBase , public CGenericCustomWizardPage {
  public:
    CPythonGenericCustomWizardPage(const QString& Title,int ImageID = -1,
                                    TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
      CWizardPage(Title,ImageID,PageAttributes) {}

    QString SubTitle;

    // Set argument value (override)
    void SetArg(const QString Arg,int ArgNum);

    // Get argument value (override)
    QString GetArg(int ArgNum);

    TWizardPageType GetPageType(void);

    virtual void RefreshPythonAttributes();
};

// Generic custom wizard page
class CPythonDataEntryWizardPage : public CPythonWizardPageBase , public CDataEntryWizardPage {
  public:
    CPythonDataEntryWizardPage(const QString& Title,int ImageID = -1,
                                    TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
      CWizardPage(Title,ImageID,PageAttributes) {}

    // Override notify event function
    bool NotifyEvent(int Param1,int Param2);

    virtual void RefreshPythonAttributes();

    TWizardPageType GetPageType(void);    
};

#endif
