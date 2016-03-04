/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Generic                                                 *
 * Module: Automatic wizards generator                              *
 * Module Description: Wizard pages implementation.                 *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 01/01/2003                                           *
 * Last upate: 03/07/2003                                           *
 ********************************************************************/

#ifndef _WIZARD_PAGES_H_
#define _WIZARD_PAGES_H_

#include "QStringList.h"
#include "AutoWizard.h"
#include "ProgressReporting.h"

// Wizard message page
class CMessageWizardPage : virtual public CWizardPage {
  public:
    CMessageWizardPage()
                       {
                       }
    CMessageWizardPage(const QString Title,int ImageID = -1,
                       TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
      CWizardPage(Title,ImageID,PageAttributes) {}

    QString SubTitle;

    // Get the polymorphic page type
    TWizardPageType GetPageType(void) {
      return wptMessage;
    }
};

// Wizard status page
class CStatusWizardPage : virtual public CWizardPage {
  public:
    CStatusWizardPage(){}
	CStatusWizardPage(const QString& Title,int ImageID = -1,
                       TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
      CWizardPage(Title,ImageID,PageAttributes) {}

    QString SubTitle;
    QString StatusMessage;

    // Get the polymorphic page type
    TWizardPageType GetPageType(void) {
      return wptStatus;
    }
};

// Wizard progress page
class CProgressWizardPage :
	virtual public CWizardPage,
	public ProgressReporting::CSerialProgressHandlerBase
{
	virtual void onProgressReport(TQPercentage percent)
	{
		Progress = percent;
        Refresh();
	}
	
	
	virtual void onStatusReport(const QString &status)
	{
		SubTitle = status;
		Refresh();
	}
	
  public:
    CProgressWizardPage(){}
	CProgressWizardPage(const QString& Title,int ImageID = -1,
                        TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
      CWizardPage(Title,ImageID,PageAttributes) {
        Min = 0;
        Max = 100;
        Progress = 0;
      }

    QString SubTitle;

    // Progress parameters
    int Min,Max,Progress;

    // Get the polymorphic page type
    TWizardPageType GetPageType(void) {
      return wptProgress;
    }
	
};

// Wizard checkboxes page
class CCheckBoxWizardPage : virtual public CWizardPage {
  private:
    // Bit mask for the checkboxes page
    unsigned m_ChecksMask;
    unsigned m_NumOfDisclosedElements;
    TProgressiveDisclosureState m_DisclosureDefaultState;
    int m_BoldIndex;


  public:
    CCheckBoxWizardPage()
    {
     m_BoldIndex = 0;
    }
	CCheckBoxWizardPage(const QString& Title,int ImageID = -1,
                        TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
      CWizardPage(Title,ImageID,PageAttributes) {
        m_ChecksMask = 0;
        m_NumOfDisclosedElements = 0;
        m_DisclosureDefaultState = PROGRESSIVE_DISCLOSURE_COLLAPSED;
        m_BoldIndex = 0;
      }

    QString SubTitle;

    CQStringList Strings;
	
	virtual void Reset()
	{
		CWizardPage::Reset();
		SetChecksMask(0);
	}

    // Override notify event function
    bool NotifyEvent(int Param1,int Param2) {
      // Update bit mask
      if(Param2)
        m_ChecksMask |= 1 << Param1;
      else
        m_ChecksMask &= ~(1 << Param1);

      return CWizardPage::NotifyEvent(Param1,Param2);
    }

    unsigned GetChecksMask(void) {
      return m_ChecksMask;
    }

    void SetChecksMask(unsigned mask) {
      m_ChecksMask = mask;
    }

    // Get the polymorphic page type
    TWizardPageType GetPageType(void) {
      return wptCheckbox;
    }

    void SetNumOfDisclosedElements(unsigned int num) {
      m_NumOfDisclosedElements = num;
     }

    unsigned int GetNumOfDisclosedElements() {
      return m_NumOfDisclosedElements;
     }

    void SetDisclosureDefaultState(TProgressiveDisclosureState state) {
      m_DisclosureDefaultState = state;
     }

    TProgressiveDisclosureState GetDisclosureDefaultState(void) {
      return m_DisclosureDefaultState;
     }
    void SetBoldIndex(int num)
    {
        m_BoldIndex = num;
    }
    int GetBoldIndex() {
        return m_BoldIndex;
    }
};

// Wizard radiogroup page
class CRadioGroupWizardPage : virtual public CWizardPage {
  private:
    int m_SelectedOption;
    unsigned m_DisabledMask;

  public:
    CRadioGroupWizardPage(){}
	CRadioGroupWizardPage(const QString Title,int ImageID = -1,
                        TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
      CWizardPage(Title,ImageID,PageAttributes) {
        DefaultOption = m_SelectedOption = -1;
        m_DisabledMask = 0;
      }

    QString SubTitle;

    CQStringList Strings;
    int DefaultOption;

    // Override notify event function
    bool NotifyEvent(int Param1,int Param2) {
      // Update current selected option
      m_SelectedOption = Param1;

      return CWizardPage::NotifyEvent(Param1,Param2);
    }

    // Return the current selected option
    int GetSelectedOption(void) {
      return m_SelectedOption;
    }
    unsigned GetDisabledMask(void) {
      return m_DisabledMask;
    }
   // Disable a row or rows in radio choose options
    void SetDisabledMask(unsigned mask) {
      m_DisabledMask = mask;
    }

    // Get the polymorphic page type
    TWizardPageType GetPageType(void) {
      return wptRadioGroup;
    }
};

// Wizard progress/status page
class CProgressStatusWizardPage : virtual public CWizardPage {
  public:
    typedef enum {psmProgress,psmStatus} TProgressStatusMode;

    CProgressStatusWizardPage(){}
	CProgressStatusWizardPage(const QString& Title,int ImageID = -1,
                        TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
      CWizardPage(Title,ImageID,PageAttributes) {
        Min = 0;
        Max = 100;
        Progress = 0;
        CurrentMode = DefaultMode = psmStatus;
      }

    QString SubTitle;

    // Progress parameters
    int Min,Max,Progress;

    QString StatusMessage;

    TProgressStatusMode DefaultMode;
    TProgressStatusMode CurrentMode;

    // Get the polymorphic page type
    TWizardPageType GetPageType(void) {
      return wptProgressStatus;
    }
};

const int MAX_GENERIC_CUSTOM_PAGE_ARGS = 32;

// Generic custom page
class CGenericCustomWizardPage : virtual public CWizardPage {
  private:
    QString m_Args[MAX_GENERIC_CUSTOM_PAGE_ARGS];

    int m_CustomPageType;

  public:
    CGenericCustomWizardPage(){}
	CGenericCustomWizardPage(const QString& Title,int ImageID = -1,
                            TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
      CWizardPage(Title,ImageID,PageAttributes) {
        m_CustomPageType = (int)wptCustom;
      }

    void SetCustomPageType(int PageType) {
      m_CustomPageType = PageType;
    }

    // Set argument value
    virtual void SetArg(const QString Arg,int ArgNum) {
      if(ArgNum >= 0 && ArgNum < MAX_GENERIC_CUSTOM_PAGE_ARGS)
        m_Args[ArgNum] = Arg;
    }

    // Get argument value
    virtual QString GetArg(int ArgNum) {
      if(ArgNum >= 0 && ArgNum < MAX_GENERIC_CUSTOM_PAGE_ARGS)
        return m_Args[ArgNum];

      return "";
    }

    // Get the polymorphic page type
    TWizardPageType GetPageType(void) {
      return m_CustomPageType;
    }
};

const int MAX_DATA_ENTRY_FIELDS = 32;

// Field types
typedef enum {ftString,ftInt,ftUnsigned,ftFloat} TDataEntryFieldType;

// Wizard data entry page
class CDataEntryWizardPage : virtual public CWizardPage {
  public:
    CDataEntryWizardPage(){}
    CDataEntryWizardPage(const QString& Title,int ImageID = -1,
                        TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
      CWizardPage(Title,ImageID,PageAttributes) {
        for(int i = 0; i < MAX_DATA_ENTRY_FIELDS; i++)
        {
          FieldsTypes[i] = ftString;
          FieldsValues.Add("");
        }
      }

    QString SubTitle;

    CQStringList Strings;
    CQStringList FieldsValues;
    TDataEntryFieldType FieldsTypes[MAX_DATA_ENTRY_FIELDS];    

    // Get the polymorphic page type
    TWizardPageType GetPageType(void) {
      return wptDataEntry;
    }
};

// Wizard elapsing time page
class CElapsingTimeWizardPage : public CWizardPage {
  public:
    // Constructor
    CElapsingTimeWizardPage(const QString& Title,
                            int ImageID = -1,
                            TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
                            CWizardPage(Title,ImageID,PageAttributes),
                            IsPaused(false),
                            m_SecondsCounter(0),
                            m_MinutesCounter(0) {}

    QString SubTitle;
    bool    IsPaused;
    int m_SecondsCounter;
    int m_MinutesCounter;

    void Pause() {IsPaused = true;  Refresh();}
    void Resume(){IsPaused = false; Refresh();}
    void Reset(void) {m_SecondsCounter=0; m_MinutesCounter=0;IsPaused = false;}

    // Return the example page type
    TWizardPageType GetPageType(void)
    {
      return wptElapsingTime;
    }
};


#endif
