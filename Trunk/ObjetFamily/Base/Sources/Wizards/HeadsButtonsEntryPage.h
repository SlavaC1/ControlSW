#ifndef _HEADS_BUTTONS_ENTRY_PAGES_H_
#define _HEADS_BUTTONS_ENTRY_PAGES_H_

#include "WizardPages.h"
#include "GlobalDefs.h"

typedef void (*FuncPtr)( void* );

#define HSW_OPERATION_MODE FIRST_OPERATION_MODE //fixme - make it another tag
#define UNINITIALIZED -1

const TWizardPageType wptHeadsButtonsEntryPage = wptCustom +24;

class CHeadsButtonsEntryPage : public CWizardPage
{
private:

   int StepsNum, QualityIndex, VoltageIndex, HeadIndex;

public:

   float*** WeightResults /*[TOTAL_NUMBER_OF_HEADS][NUMBER_OF_VOLTAGE_MODES][NUMBER_OF_QUALITY_MODES]*/;
   int***   MissingNozzlesResults/*[TOTAL_NUMBER_OF_HEADS][NUMBER_OF_VOLTAGE_MODES][NUMBER_OF_QUALITY_MODES]*/;
   bool***  MarkHeadsToBeReplaced/*[TOTAL_NUMBER_OF_HEADS][NUMBER_OF_VOLTAGE_MODES][NUMBER_OF_QUALITY_MODES]*/;
   bool*    HeadsToBeSkipped/*[TOTAL_NUMBER_OF_HEADS]*/;

   FuncPtr EnterEditModeEventPtr;
   void*   EnterEditModeEventArg;

   CHeadsButtonsEntryPage(const QString& Title,int ImageID = -1,TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES) :
     CWizardPage(Title,ImageID,PageAttributes | wpUserButton1Disabled | wpUserButton2Disabled),HeadsToBeSkipped(NULL)
   {	
	WeightResults = NULL;
	MissingNozzlesResults = NULL;
	MarkHeadsToBeReplaced = NULL;
	EnterEditModeEventPtr = NULL;
	StepsNum = 0;
	QualityIndex = 0;
	VoltageIndex = 0;
	HeadIndex = 0;
	EnterEditModeEventArg = NULL;
	HeadsToBeSkipped = NULL;
   }
   
   bool* GetHeadsToSkipped()
   {
      return HeadsToBeSkipped;
   }

   bool*** GetMarkHeadsToReplace()
   {
      return MarkHeadsToBeReplaced;
   }

   bool PreEnter(void)
   {
  #ifdef _DEBUG
      if(FindWindow(0, "HeadsButtonsEntryPageEdit.txt - Notepad"))
      {
         FILE* f = fopen("HeadsButtonsEntryPageEdit.txt", "r");
//         int n;
         // use file
         for (int qm = NUMBER_OF_QUALITY_MODES-1; qm >= 0; qm--)
          for (int h = TOTAL_NUMBER_OF_HEADS-1; h >= 0 ; h--)
              FOR_ALL_VOLTAGE_MODES(v)
                  {
                      fscanf (f,"%f",&(WeightResults[h][v][qm]));
                      fscanf (f,"%d",&(MissingNozzlesResults[h][v][qm]));
                  }
         fclose(f);
      }
  #endif
      return CWizardPage::PreEnter();
   }

   void Reset()
   {
     Nulify();
     FOR_ALL_HEADS(h)
     {
         if (HeadsToBeSkipped[h])
            continue;
         FOR_ALL_VOLTAGE_MODES(v)
              FOR_ALL_QUALITY_MODES(qm)
              {
                  WeightResults        [h][v][qm] = UNINITIALIZED;
                  MissingNozzlesResults[h][v][qm] = UNINITIALIZED;
              }
     }
   }

   void Nulify()
   {
     StepsNum = 0;
     QualityIndex = LAST_QUALITY_MODE-1;
     VoltageIndex = LAST_VOLTAGE_MODE-1;
     HeadIndex = LAST_HEAD-1;
     EnterEditModeEventPtr = NULL;
     EnterEditModeEventArg = NULL;

     FOR_ALL_HEADS(h)
         FOR_ALL_VOLTAGE_MODES(v)
              FOR_ALL_QUALITY_MODES(qm)
                  MarkHeadsToBeReplaced[h][v][qm] = false;
   }
   
   void EnterEditModeEvent()
   {
      if (EnterEditModeEventPtr)
        EnterEditModeEventPtr(EnterEditModeEventArg);
   }

   bool IsAllDone(void)
   {
      return (StepsNum >= (TOTAL_NUMBER_OF_HEADS * NUMBER_OF_VOLTAGE_MODES * NUMBER_OF_QUALITY_MODES));
   }

   TWizardPageType GetPageType(void)
   {
   	  return wptHeadsButtonsEntryPage;
   }

   float GetCurrentWeight(void)
   { 
       return WeightResults[HeadIndex][VoltageIndex][QualityIndex];
   }

   int GetCurrentMissingNozzlesValue()
   {
       return MissingNozzlesResults[HeadIndex][VoltageIndex][QualityIndex];
   }

    float*** GetWeightsPtr(void)
    {
       return WeightResults;
    }

    int*** GetMissingNozzlesPtr(void)
    {
       return MissingNozzlesResults;
    }

    int& GetStepsNum(void)
    {
       return StepsNum;
    }

    int& GetQualityIndex(void)
    {
       return QualityIndex;
    }

    int& GetVoltageIndex(void)
    {
       return VoltageIndex;
    }

    int& GetHeadIndex(void)
    {
       return HeadIndex;
    }
};


#endif
