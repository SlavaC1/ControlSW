
#ifndef _USER_VALUES_ENTRY_PAGES_H_
#define _USER_VALUES_ENTRY_PAGES_H_

#include "WizardPages.h"
#include "GlobalDefs.h"

const TWizardPageType wptUserValuesEntryPage = wptCustom;

typedef enum {evInt, evFloat, evUnsigned, evString}TUserValuesType;
typedef QString (*FPQstringInt)( int );

class CUserValuesEntryWizardPage : public CWizardPage {
  public:
    // Constructor
    CUserValuesEntryWizardPage(const QString& Title,int ImageID = -1,
                               TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
                               CWizardPage(Title,ImageID,PageAttributes) 
							   {
							   memset(&ValuesType, 0, sizeof(TUserValuesType));
							   memset(LabelsColor, 0, sizeof(LabelsColor));
							   }

    QString         SubTitle;
    TUserValuesType ValuesType;
    QString         Data                 [TOTAL_NUMBER_OF_HEADS];
    QString         UserEntryDescriptions[TOTAL_NUMBER_OF_HEADS];
    int             LabelsColor          [TOTAL_NUMBER_OF_HEADS];

    // Return the example page type
    TWizardPageType GetPageType(void)
    {
      return wptUserValuesEntryPage;
    }

    void SetUserEntryDescriptions(FPQstringInt FuncPtr)
    {
      FOR_ALL_HEADS(i)
        UserEntryDescriptions[i] = FuncPtr(i);
    }

    void ClearUserEntryValues(void)
    {
      FOR_ALL_HEADS(i)
        Data[i] = "0";
    }

    void ClearUserEntryColors(void)
    {
      FOR_ALL_HEADS(i)
        LabelsColor[i] = 0x0;
    }
};

#endif
 