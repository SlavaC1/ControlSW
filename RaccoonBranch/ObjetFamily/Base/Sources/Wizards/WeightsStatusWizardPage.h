
#ifndef _WEIGHTS_STATUS_WIZARD_PAGE_H
#define _WEIGHTS_STATUS_WIZARD_PAGE_H

#include "WizardPages.h"
#include "GlobalDefs.h"
const TWizardPageType wptWeightsStatusWizardPage = wptCustom + 11;

class WeightsStatusWizardPageData
{
  public:
       int m_CurrValue;
	   bool m_enable;
	   WeightsStatusWizardPageData(int value,bool enable)
	   {
		 SetWeightsStatusWizardPageData(value,enable);
	   }
	   void SetWeightsStatusWizardPageData(int value,bool enable)
	   {
		 m_CurrValue = value;
		 m_enable = enable;
       }

};
class CWeightsStatusWizardPage : public CWizardPage {
  private:
	QString m_SubTitle;
	
  public:
    // Constructor
    CWeightsStatusWizardPage(const QString& Title,
                           int ImageID = -1,
                           TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
						   CWizardPage(Title,ImageID,PageAttributes), m_SubTitle("")
							{
							for(int i = 0;i<TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE;i++)
								m_WeightsStatusWizardPageData[i] = new WeightsStatusWizardPageData(0,true);
							}
	virtual ~CWeightsStatusWizardPage()
	{
       for(int i = 0;i<TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE;i++)
	  {
        delete m_WeightsStatusWizardPageData[i];
      }
	}

	void SetSubTitle(QString& caption) { m_SubTitle = caption; }
	QString GetSubTitle() const { return m_SubTitle; }

    QString GroupBoxCaption;
    QString UnitsCaption;
    QString CheckBoxText;

	WeightsStatusWizardPageData *m_WeightsStatusWizardPageData[TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE];

    // Return the example page type
    TWizardPageType GetPageType(void) {
      return wptWeightsStatusWizardPage;
	}
  void SetWeightsStatusWizardPageData(int i,int value,bool enable)
   {
	 if(m_WeightsStatusWizardPageData[i]!=NULL)
		 m_WeightsStatusWizardPageData[i]->SetWeightsStatusWizardPageData(value,enable);
	 else
	 {
		m_WeightsStatusWizardPageData[i] = new WeightsStatusWizardPageData(value,enable);
     	m_WeightsStatusWizardPageData[i]->SetWeightsStatusWizardPageData(value,enable);
	 }
   }
	WeightsStatusWizardPageData* GetWeightsStatusWizardPageData (int i)
	{
	WeightsStatusWizardPageData *pageData = NULL;
	   if(m_WeightsStatusWizardPageData[i]!=NULL)
		pageData = m_WeightsStatusWizardPageData[i];
     return pageData;
	}
	void ResetPageDataArray ()
	{
	  for(int i = 0;i<TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE;i++)
	  {
	  if(m_WeightsStatusWizardPageData[i] !=NULL)
	  {
		 m_WeightsStatusWizardPageData[i]->m_enable = true;
		 m_WeightsStatusWizardPageData[i]->m_CurrValue = 0;
	  }
	  else
		m_WeightsStatusWizardPageData[i] = new WeightsStatusWizardPageData(0,true);
	  }
	}

};


#endif