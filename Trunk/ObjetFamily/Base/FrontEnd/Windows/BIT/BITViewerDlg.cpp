//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <algorithm>
#include "BITViewerDlg.h"
#include "BackEndInterface.h"
#include "BITProgressDlg.h"
#include "QException.h"
#include "BITExtResultsDlg.h"
#include "PythonAppBIT.h"
#include "Q2RTApplication.h"
#include "FrontEndParams.h"
#include "MotorDefs.h"
#include "Q2RTErrors.h"
#include "HeatersDefs.h"
#include "QThreadUtils.h"
#include "FrontEnd.h"
#include "QMonitor.h"
#include "HostComm.h"
#include "MachineManager.h"
#include "PrepareForBitDlg.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TBITViewerForm *BITViewerForm;

const int UN_CHECK_IMAGE = 0;
const int CHECK_IMAGE    = 1;
const int GO_IMAGE       = 2;   
const int NO_GO_IMAGE    = 3;
const int UNKNOWN_IMAGE  = 4;
const int WARNING_IMAGE  = 5;

const int GROUP_ITEM_ID             = 0;
const int STATUS_ITEM_ID            = 1;
const int ERROR_DESCRIPTION_ITEM_ID = 2;

const int HEATERS_POLL_TIME = 100;
//const String DOOR_CLOSED = "Door is closed";
//const String DOOR_CLOSED = "Doors are closed";
const char BIT_HELP_FILE_NAME[] = "\\Help\\BIT\\BIT_Help.chm";

// Launch an help file in an external viewer
void ExternalAppExecute(const char *FileName,const char *CmdLineParam);

// Utility fuction for extarcting the group and test name from a string (e.g. MyGroup|MyTest)
QString TestResultToStr(TTestResult TestResult);

// Figure out which attributes are dependent (e.g. home axes will enable a door attribute)
TTestAttributes ResolveAttributesDependency(TTestAttributes TestAttributes);


// Proxy object for BIT services
class CBITProxy : public CQComponent {
  public:
    DEFINE_METHOD(CBITProxy,bool,IsCanceled);
    DEFINE_METHOD_3(CBITProxy,TQErrCode,UpdateStepStatus,QString /*CurrentStepName*/,
                  int /*TotalNumberOfSteps*/,int /*StepsProgressSoFar*/);

    DEFINE_METHOD_1(CBITProxy,TQErrCode,UpdateStatusPanel,QString /*StatusText*/);
    DEFINE_METHOD_2(CBITProxy,TQErrCode,UpdateProgress,int /*MaxValue*/,int /*Value*/);

    // Constructor
    CBITProxy(void) : CQComponent("BITProxy") {
      INIT_METHOD(CBITProxy,IsCanceled);
      INIT_METHOD(CBITProxy,UpdateStepStatus);
      INIT_METHOD(CBITProxy,UpdateStatusPanel);
      INIT_METHOD(CBITProxy,UpdateProgress);
    }
};

bool CBITProxy::IsCanceled(void)
{
  return BITProgressForm->IsCanceled();
}
 //---------------------------------------------------------------------------
TQErrCode CBITProxy::UpdateStepStatus(QString CurrentStepName,int TotalNumberOfSteps,int StepsProgressSoFar)
{
  BITProgressForm->UpdateStepStatus(CurrentStepName.c_str(),TotalNumberOfSteps,StepsProgressSoFar);
  return Q_NO_ERROR;
}
//---------------------------------------------------------------------------
TQErrCode CBITProxy::UpdateStatusPanel(QString StatusText)
{
  BITProgressForm->UpdateStatusPanel(StatusText.c_str());
  return Q_NO_ERROR;
}
//---------------------------------------------------------------------------
TQErrCode CBITProxy::UpdateProgress(int MaxValue,int Value)
{
  BITProgressForm->UpdateProgress(MaxValue,Value);
  return Q_NO_ERROR;
}
//---------------------------------------------------------------------------
__fastcall TBITViewerForm::TBITViewerForm(TComponent* Owner)
        : TForm(Owner)
{
  QString str;
  CBackEndInterface *endInterface = CBackEndInterface::Instance();
  if (endInterface !=NULL )
  {
	  str =  endInterface->GetDoorMessage();
  }
  else
  {
	str =   "Door is Closed";
  }
   m_str_Door_Msg =  str.c_str();
  LastSelectedItem = NULL;
  m_BITProxy = NULL;
  m_IsTrayClean = false;
  m_IsFirstTrayCleanQuery = false;
}
//---------------------------------------------------------------------------
void TBITViewerForm::UpdateCombinedStatus(TTestResult Status)
{
  int ImageID;

  switch(Status)
  {
    case trGo:
      ImageID = GO_IMAGE;
      break;

    case trNoGo:
      ImageID = NO_GO_IMAGE;
      break;

    case trWarning:
      ImageID = WARNING_IMAGE;
      break;

    default:
      ImageID = UNKNOWN_IMAGE;
      break;
  }

  CombinedStatusImage->Picture->Bitmap->Width = 0;
  CombinedStatusImage->Picture->Bitmap->Height = 0;
  ImageList1->GetBitmap(ImageID,CombinedStatusImage->Picture->Bitmap);
}
//---------------------------------------------------------------------------
// (Un)Select an items in a specific test group
void TBITViewerForm::SelectItemsInGroup(AnsiString GroupName,bool Select)
{
  for(int i = 0; i < TestsListView->Items->Count; i++)
  {
    TListItem *Item = TestsListView->Items->Item[i];

    // Check if group name match
    if(Item->SubItems->Strings[GROUP_ITEM_ID] == GroupName)
      Item->Checked = Select;
  }
}
//---------------------------------------------------------------------------
bool TBITViewerForm::DoTest(TListItem *Item,AnsiString GroupName,AnsiString TestName,

                            int TotalNumberOfTests,int CurrentTestNum)
{
  bool Ret = true;

  try
  {
    // If the progress/status form is hidden, show it now (non modal)
    if(!BITProgressForm->Visible)
      BITProgressForm->Show();

    // Check if the user didn't canceled
    if(!BITProgressForm->IsCanceled())
    {
      // Do the test
      CBackEndInterface *BEInterface = CBackEndInterface::Instance();

      // Update initial status...
      BITProgressForm->UpdateStatus(GroupName + " | " + TestName,TotalNumberOfTests,CurrentTestNum);
      BITProgressForm->UpdateStepStatus("Starting Test",1,0);
	  TTestAttributes Attr = ResolveAttributesDependency((TTestAttributes)Item->Data);

      if(!PrepareForTest(Attr))
        return false;

      int /*TTestResult*/ StartResult = BEInterface->StartTest(GroupName.c_str(),TestName.c_str());

      if(StartResult == trGo)
      {
        // Get test steps number
        int StepsNum = BEInterface->GetCurrentTestStepsNum();

        // Execute each test step
        for(int i = 0; i < StepsNum; i++)
        {
          // If canceled
          if(BITProgressForm->IsCanceled())
          {
            BEInterface->EndTest();
            return false;
          }

          AnsiString StepName = BEInterface->GetCurrentTestStepName().c_str();

          BITProgressForm->UpdateStepStatus(StepName,StepsNum,i);

          if(BEInterface->ExecNextTestStep() != trUnknown)
          {
            BITProgressForm->UpdateStepStatus("Finishing Test",StepsNum,i + 1);
            break;
          }
        }
		// If canceled
        if(BITProgressForm->IsCanceled())
        {
           BEInterface->EndTest();
           return false;
        }
        BEInterface->EndTest();
      }

      // Get test results
      TTestResult TestResult = (TTestResult)CBackEndInterface::Instance()->GetLastTestResult();
      UpdateTestResult(Item,TestResult);

      if(TestResult == trNoGo)
      {
        // Check if we need to cancel all other tests
        if(Attr & taBreakAll)
          Ret = false;
      }

      // Update the combined results variable
      switch(TestResult)
      {
        case trGo:
          if(m_LastCombinedResults == trUnknown)
            m_LastCombinedResults = trGo;
          break;

        case trNoGo:
          m_LastCombinedResults = trNoGo;
          break;

        case trWarning:
          if(m_LastCombinedResults != trNoGo)
            m_LastCombinedResults = trWarning;
      }

      UpdateCombinedStatus(m_LastCombinedResults);

      // Clean up after test (except door lock)
      UnPrepareForTest(Attr & ~taDoorLock);
    } else
        // Canceled...
        return false;

    // If we are done, close the status form
    if(CurrentTestNum == TotalNumberOfTests - 1)
    {
      // Give the user some time to see the "done" message
      BITProgressForm->UpdateStatus("Done",TotalNumberOfTests,TotalNumberOfTests);
      Sleep(500);

      BITProgressForm->Close();
    }
  }
  // Catch QException classes
  catch(EQException& QException)
  {
    BITProgressForm->Close();
    MessageDlg(QException.GetErrorMsg().c_str(),mtError,TMsgDlgButtons() << mbOK,0);
    Ret = false;
  }
  // Catch VCL Exceptions
  catch(Exception& VCLException)
  {
    BITProgressForm->Close();
    MessageDlg(VCLException.Message,mtError,TMsgDlgButtons() << mbOK,0);
    Ret = false;
  }
  // Catch unexpected exceptions
  catch(...)
  {
    BITProgressForm->Close();
    MessageDlg("Unexpected exception in test -" + GroupName + "|" + TestName,mtError,
               TMsgDlgButtons() << mbOK,0);
    Ret = false;
  }

  return Ret;
}
//---------------------------------------------------------------------------
// Run a single test and handle everything
void TBITViewerForm::RunSingleTest(TListItem *Item)
{
  try
  {
    // Get selected item group name
    AnsiString GroupName = Item->SubItems->Strings[GROUP_ITEM_ID];

    Enabled = false;

    try
    {
      Q2RTApplication->GetMachineManager()->MarkBITRunning(true);
      DoTest(Item,GroupName,Item->Caption,1,0);
      Q2RTApplication->GetMachineManager()->MarkBITRunning(false);

      // Clean up everything (just to be on the safe side)
      UnPrepareForTest(taDoorLock |
                       taTrayInsert |
                       taHomeAxes |
                       taHeadsHeating |
                       taTrayHeating |
                       taHeadsFilling);

      // Make sure the progress form is closed
      BITProgressForm->Close();
    } __finally
    {
        Enabled = true;
    }
  }
  catch(EQException& QException)
  {
     MessageDlg(QException.GetErrorMsg().c_str(),mtError,TMsgDlgButtons() << mbOK,0);
  }
}
//---------------------------------------------------------------------------
void TBITViewerForm::UpdateExtendedInfoForm(void)
{
  // Get selected item group name and test name
  AnsiString GroupName = TestsListView->Selected->SubItems->Strings[GROUP_ITEM_ID];
  AnsiString TestName = TestsListView->Selected->Caption;

  int StatusImageIndex = TestsListView->Selected->SubItemImages[STATUS_ITEM_ID];

  // If the extended error string is empty, use the standard error description as default
  QString ErrorStrEx = TestsListView->Selected->SubItems->Strings[ERROR_DESCRIPTION_ITEM_ID].c_str();

  BITExtendedResultsForm->UpdateResults(GroupName,TestName,QFixLineBreaks(ErrorStrEx).c_str(),
                                        ImageList1,StatusImageIndex);

  // Update extended results table
  CBackEndInterface *BEInterface = CBackEndInterface::Instance();

  // Extract extended results for each step
  int n = BEInterface->BeginTestResultExEnumeration(GroupName.c_str(),TestName.c_str());

  BITExtendedResultsForm->StartResultsTableUpdate();

  for(int i = 0; i < n; i++)
  {
    QString ExtResults = BEInterface->GetNextTestResultEx();
    BITExtendedResultsForm->AddTableEntry(ExtResults);
  }                      
}
//---------------------------------------------------------------------------
// Test and extended update current focused item
void TBITViewerForm::TestAndUpdateSelectedItem(void)
{
  if(TestsListView->Selected != NULL)
  {
    AnsiString GroupName = TestsListView->Selected->SubItems->Strings[GROUP_ITEM_ID];
    AnsiString TestName = TestsListView->Selected->Caption;

    RunSingleTest(TestsListView->Selected);
    UpdateExtendedInfoForm();
  }
}
//---------------------------------------------------------------------------
void TBITViewerForm::UpdateTestResult(TListItem *Item,TTestResult TestResult)
{
  CBackEndInterface *BEInterface = CBackEndInterface::Instance();

  int ImageID = -1;

  switch(TestResult)
  {
    case trUnknown:
      ImageID = UNKNOWN_IMAGE;
      break;

    case trGo:
      ImageID = GO_IMAGE;
      break;

    case trNoGo:
      ImageID = NO_GO_IMAGE;
      break;

    case trWarning:
      ImageID = WARNING_IMAGE;
      break;
  }

  Item->SubItemImages[STATUS_ITEM_ID] = ImageID;

  // Update error description string
  QString ErrDescription = BEInterface->GetLastTestErrorDescription();
  Item->SubItems->Strings[ERROR_DESCRIPTION_ITEM_ID] = ErrDescription.c_str();
}
//---------------------------------------------------------------------------
void TBITViewerForm::RefreshTestListItems(void)
{
  TestsListView->Clear();

  CBackEndInterface *BEInterface = CBackEndInterface::Instance();

  // Enumerate test groups
  BEInterface->BeginTestGroupsEnumeration();

  SelectionGroupComboBox->Clear();
  SelectionGroupComboBox->Items->Add("-- None --");

  QString TestGroupName;
  while((TestGroupName = BEInterface->GetNextTestGroup()) != "")
  {
    QString TestName;

    // Enumerate items in the group
    BEInterface->BeginTestsEnumeration(TestGroupName);

    while((TestName = BEInterface->GetNextTest()) != "")
    {
       TestsListView->AddItem(TestName.c_str(),NULL);
       TListItem *Item = TestsListView->Items->Item[TestsListView->Items->Count - 1];

       // Remember the test attributes in the data member
       Item->Data = (void *)(BEInterface->GetNextTestAttributes());

      // Set item properties
      Item->Checked = true; /*was false*/

      // Add test group
      Item->SubItems->Add(TestGroupName.c_str());

      // Add status image and error description items
      Item->SubItems->Add("");
      Item->SubItems->Add("");


      UpdateTestResult(Item,(TTestResult)BEInterface->GetLastTestResult());
    }
    // Add to the selection combo
    SelectionGroupComboBox->AddItem(TestGroupName.c_str(),NULL);
  }

  SelectionGroupComboBox->ItemIndex = 0;
}
//---------------------------------------------------------------------------
void TBITViewerForm::RefreshSelectionSetItems(void)
{
  SelectionSetComboBox->Clear();

  CBackEndInterface *BEInterface = CBackEndInterface::Instance();

  // Enumerate BIT sets
  BEInterface->BeginBITSetsEnumeration();

  SelectionSetComboBox->Clear();
  SelectionSetComboBox->Items->Add("-- None --");

  QString BITSetName;
  while((BITSetName = BEInterface->GetNextBITSet()) != "")
  {
    // Add to the selection set combo
    SelectionSetComboBox->AddItem(BITSetName.c_str(),NULL);
  }

  SelectionSetComboBox->ItemIndex = 0;
}
//---------------------------------------------------------------------------
// (un)Select a specific test by its full name
void TBITViewerForm::SelectTestByName(const QString TestFullName,bool Select)
{
  QString GroupName,TestName;

  if(QBreakKeyAndValueStr(TestFullName,GroupName,TestName,'|'))
  {
    // Find the requested item
    for(int i = 0; i < TestsListView->Items->Count; i++)
    {
      TListItem *Item = TestsListView->Items->Item[i];

      AnsiString ItemGroupName = Item->SubItems->Strings[GROUP_ITEM_ID];
      AnsiString ItemTestName = Item->Caption;

      if((ItemGroupName == GroupName.c_str()) && (ItemTestName == TestName.c_str()))
      {
        Item->Checked = Select;
        break;
      }
    }
  }
}
//---------------------------------------------------------------------------
// Run a BIT selection set
TTestResult TBITViewerForm::RunBITSet(AnsiString BITSetName)
{
  SelectItemsInBITSet(BITSetName);
  RunAction->Execute();

  return m_LastCombinedResults;
}//---------------------------------------------------------------------------
bool TBITViewerForm::SelectItemsInBITSet(AnsiString BITSetName)
{
  CBackEndInterface *BEInterface = CBackEndInterface::Instance();

  try
  {
    // Deselect all items
    for(int i = 0; i < TestsListView->Items->Count; i++)
      TestsListView->Items->Item[i]->Checked = true;//changed from false (see note in TestsListViewGetImageIndex)

    if(!BEInterface->BeginTestsInSetEnumeration(BITSetName.c_str()))
      return false;

    QString FullTestName;

    while((FullTestName = BEInterface->GetNextTestInSet()) != "")
      SelectTestByName(FullTestName,false);//changed from true (see note in TestsListViewGetImageIndex)

    SelectionGroupComboBox->ItemIndex = 0;
  }
  catch(EQException& QException)
  {
      MessageDlg(QException.GetErrorMsg().c_str(),mtError,TMsgDlgButtons() << mbOK,0);
      return false;
  }

  return true;
}
//---------------------------------------------------------------------------
void TBITViewerForm::RefreshTests(void)
{
  RefreshTestListItems();
  RefreshSelectionSetItems();
}
//---------------------------------------------------------------------------
bool TBITViewerForm::BuildActionsForBit(TTestAttributes TestAttributes)
{
	bool showDialog = false; // no need to show dialog .
	#if !defined(OBJET_500)  // NOT applicable to Objet1000 because it has removable tray
	if(TestAttributes & taTrayInsert)
	 PrepareBitDlg ->AddAttribute(TRAY_INSIDE);
	#endif
	if(TestAttributes & taHomeAxes)
		PrepareBitDlg ->AddAttribute(TRAY_EMPTY);
	if(TestAttributes & taDoorLock)
	   PrepareBitDlg ->AddAttribute(m_str_Door_Msg);

	return showDialog;
}
//---------------------------------------------------------------------------
bool TBITViewerForm::PrepareForBit(TTestAttributes TestAttributes,bool createChbx)
{
	bool showDialog = false; // no need to show dialog .
	bool checkAgain = false;
	CBackEndInterface *BEInterface = CBackEndInterface::Instance();
	ShowPrepareBitDialog(createChbx);
		 // Verify that the tray is inserted
    if (PrepareBitDlg->WasCancelled())
    {
    	return false;
    }
         // Heads should be hot
    if(TestAttributes & taHeadsHeating)
    {
    	BITProgressForm->UpdateStatusPanel("Heads Heating");
        int CurrentTempParam[TOTAL_NUMBER_OF_HEATERS];
        BEInterface->GetCurrentHeadHeatersTempParameters(CurrentTempParam, true);
        BEInterface->TurnHeadHeaters(true, CurrentTempParam, TOTAL_NUMBER_OF_HEATERS);
    }
    if(TestAttributes & taTrayInsert)
    {
    	if(BEInterface->IsTrayInserted() == true)
        {
        	if(BEInterface->VerifyTrayInserted(showDialog) != Q_NO_ERROR)
                {
                	PrepareBitDlg->EnableCheckBox(TRAY_INSIDE,true);
                        PrepareBitDlg->SelectCheckBox(TRAY_INSIDE,false);
                        checkAgain = true;
                }
                else
                {
                	PrepareBitDlg->EnableCheckBox(TRAY_INSIDE,false);
                        PrepareBitDlg->SelectCheckBox(TRAY_INSIDE,true);
                        PrepareBitDlg->RemoveAttribute(TRAY_INSIDE);

                }
        }
        else
        {
        	PrepareBitDlg->EnableCheckBox(TRAY_INSIDE,true);
                PrepareBitDlg->SelectCheckBox(TRAY_INSIDE,false);
                checkAgain = true;
        }
    }
		// Close the door
    if(TestAttributes & taDoorLock)
    {
    	if(BEInterface->CheckIfDoorIsClosed() == Q_NO_ERROR)
        {
        	if(BEInterface->EnableDoor(true,showDialog) != Q_NO_ERROR)
                {
                	PrepareBitDlg->EnableCheckBox(m_str_Door_Msg,true);
                        PrepareBitDlg->SelectCheckBox(m_str_Door_Msg,false);
                        checkAgain = true;
                }
                else
                {
                	PrepareBitDlg->EnableCheckBox(m_str_Door_Msg,false);
                	PrepareBitDlg->SelectCheckBox(m_str_Door_Msg,true);
                        PrepareBitDlg->RemoveAttribute(m_str_Door_Msg);
                }
        }
        else
        {
        	PrepareBitDlg->EnableCheckBox(m_str_Door_Msg,true);
                PrepareBitDlg->SelectCheckBox(m_str_Door_Msg,false);
                checkAgain = true;
        }
    }
    CBackEndInterface *backendInterface = CBackEndInterface::Instance();
    if(checkAgain == true)
    {
    	createChbx = false;
        PrepareForBit(TestAttributes,createChbx);
        if(NULL != backendInterface)
        {
        	backendInterface->LockDoor(false); // maybe need to insert the tray
        }
    }
    else
    {

          //To avoid the appliction to freeze when runing some BITs in parallel
    	if( PrepareBitDlg->IsTrayEmptyIsOnlyAttribute())
        {
        	PrepareBitDlg->RemoveAttribute(TRAY_EMPTY);
            PrepareBitDlg->Clean();
            if(NULL != backendInterface)
        	{
                    backendInterface->LockDoor(true);
	        }
        }
    }
    if (PrepareBitDlg->WasCancelled())
    {
  	return false;
    }
    return true;
}
//---------------------------------------------------------------------------

void TBITViewerForm::ShowPrepareBitDialog(bool needToCreateChBx)
{
  bool bNeedToUnLockDoor = false;
  CBackEndInterface *BEInterface = CBackEndInterface::Instance();
  if(PrepareBitDlg->NeedToShow())
  {
	bool showDialog = false; // no need to show dialog .
	PrepareBitDlg->UpdateView(needToCreateChBx);

	if(BEInterface->CheckIfDoorIsClosed() == Q_NO_ERROR)
	{
		if(BEInterface->EnableDoor(true,showDialog) != Q_NO_ERROR)
		{
		  PrepareBitDlg->EnableCheckBox(m_str_Door_Msg,true);
		  PrepareBitDlg->SelectCheckBox(m_str_Door_Msg,false);
		}
		else
		{
		   PrepareBitDlg->EnableCheckBox(m_str_Door_Msg,false);
		   PrepareBitDlg->SelectCheckBox(m_str_Door_Msg,true);
                   PrepareBitDlg->RemoveAttribute(m_str_Door_Msg);
		}
	}
	else
	{
		PrepareBitDlg->EnableCheckBox(m_str_Door_Msg,true);
		PrepareBitDlg->SelectCheckBox(m_str_Door_Msg,false);
	}
	PrepareBitDlg->Refresh();

	if(PrepareBitDlg->IsAttributeExists(TRAY_INSIDE))
	{
	   if(BEInterface->IsTrayInserted() == true)
	   {
		  if(BEInterface->VerifyTrayInserted(showDialog) != Q_NO_ERROR)
		  {
			 PrepareBitDlg->EnableCheckBox(TRAY_INSIDE,true);
			 PrepareBitDlg->SelectCheckBox(TRAY_INSIDE,false);
                         bNeedToUnLockDoor = true;
		  }
		  else
		  {
			 PrepareBitDlg->EnableCheckBox(TRAY_INSIDE,false);
			 PrepareBitDlg->SelectCheckBox(TRAY_INSIDE,true);
             		 PrepareBitDlg->RemoveAttribute(TRAY_INSIDE);

		  }
	   }
	   else
	   {
		   PrepareBitDlg->EnableCheckBox(TRAY_INSIDE,true);
		   PrepareBitDlg->SelectCheckBox(TRAY_INSIDE,false);
                   bNeedToUnLockDoor = true;
	   }
	   PrepareBitDlg->Refresh();
	}
        if(bNeedToUnLockDoor)
        {
        	BEInterface->LockDoor(false);
        }
	PrepareBitDlg->ShowModal();
   }
}
//---------------------------------------------------------------------------
void TBITViewerForm::ClosePrepareBitDialog()
{
  PrepareBitDlg->CloseDialog();
}
//---------------------------------------------------------------------------
bool TBITViewerForm::PrepareForTest(TTestAttributes TestAttributes)
{
  CBackEndInterface *BEInterface = CBackEndInterface::Instance();
  // Axes homing
  if(TestAttributes & taHomeAxes)
  {

    BITProgressForm->UpdateStatusPanel("Homing Axes");

    for(int Axis = AXIS_X; Axis <= AXIS_T; Axis++)
    {
      BEInterface->EnableMotor(true,Axis);
      BEInterface->GoToMotorHomePosition(Axis,true);

      if(BITProgressForm->IsCanceled())
        return false;
    }
  }

  // Heads should be hot
  if(TestAttributes & taHeadsHeating)
  {
	BITProgressForm->UpdateStatusPanel("Heads Heating");
	int CurrentTempParam[TOTAL_NUMBER_OF_HEATERS];
		BEInterface->GetCurrentHeadHeatersTempParameters(CurrentTempParam, true);
		BEInterface->TurnHeadHeaters(true, CurrentTempParam, TOTAL_NUMBER_OF_HEATERS);
    while(!BEInterface->AreHeadsTemperaturesOk())
    {
      if(BITProgressForm->IsCanceled())
        return false;

      QSleep(HEATERS_POLL_TIME);
    }
  }

  // Tray should be hot
  if(TestAttributes & taTrayHeating)
  {
    BITProgressForm->UpdateStatusPanel("Tray Heating");

    BEInterface->TurnTrayHeater(true,BEInterface->GetCurrentTrayTemperatureParameter());

    while(BEInterface->GetTrayTemperatureStatus() != TRAY_TEMPERATURE_OK)
    {
      if(BITProgressForm->IsCanceled())
        return false;

      QSleep(HEATERS_POLL_TIME);
    }
  }

  // Heads filling
  if(TestAttributes & taHeadsFilling)
  {
    BITProgressForm->UpdateStatusPanel("Heads Filling");

    BEInterface->TurnHeadFilling(true);

    while(!BEInterface->AreHeadsFilled())
    {
      if(BITProgressForm->IsCanceled())
        return false;

      QSleep(HEATERS_POLL_TIME);
    }
  }

  return true;
}
//---------------------------------------------------------------------------
void TBITViewerForm::UnPrepareForTest(TTestAttributes TestAttributes)
{
  CBackEndInterface *BEInterface = CBackEndInterface::Instance();

  // Turn off head heaters
  if(TestAttributes & taHeadsHeating)
    BEInterface->TurnHeadHeaters(false);

  // Turn off tray heater
  if(TestAttributes & taTrayHeating)
    BEInterface->TurnTrayHeater(false);

  // Turn off head filling
  if(TestAttributes & taHeadsFilling)
    BEInterface->TurnHeadFilling(false);

  // Open door
  if(TestAttributes & taHeadsFilling)
    BEInterface->EnableDoor(false);
}
//---------------------------------------------------------------------------
// Create a report file (remember last file name)
void TBITViewerForm::CreateBITReportFile(AnsiString BITReportFileName)
{
  ViewReportButton->Visible = true;
  CreatReportButton->Visible = true;
  
  m_LastReportFileName = BITReportFileName;
  CBackEndInterface::Instance()->CreateBITReportFile(BITReportFileName.c_str());
}
//---------------------------------------------------------------------------
bool TestExecListSortFunc(const TTestExecItem &A,const TTestExecItem &B)
{
  return (A.Priority > B.Priority);
}
//---------------------------------------------------------------------------
// Prepare a sorted by priority list of tests
TTestExecList TBITViewerForm::PrepareExecutionList(void)
{
  TTestExecList Result;

  // Execute all selected tests
  for(int i = 0; i < TestsListView->Items->Count; i++)
  {
    TListItem *Item = TestsListView->Items->Item[i];

    if(!Item->Checked)//changed from without ! (see note in TestsListViewGetImageIndex)
    {
      AnsiString GroupName = Item->SubItems->Strings[GROUP_ITEM_ID];

      // Extract test item priority (upper 16bits of attribute mask)
      int Priority = (unsigned)Item->Data >> 16;

      Result.push_back(TTestExecItem(Item,GroupName,Item->Caption,Priority));
    }
  }

  // Do priority sort
  std::sort(Result.begin(),Result.end(),TestExecListSortFunc);
  return Result;
}
//---------------------------------------------------------------------------










/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
void __fastcall TBITViewerForm::TestsListViewGetImageIndex(TObject *Sender,TListItem *Item)
{
/*bug:    all tests are checked in the opening of the BITscreen, suppose to be: UNchecked.
reason:  raised with the adding of themeManager(GUI)  the real checkboxes are not in use, statuses and checkboxes that are showen in the BIT are painted, if adding the real checkbox the statuses  disappear.
solution: from the first time that the user enters the BIT screen the checked items are marked as Unchecked and the Unchecked are marked as checked (this way all that were checked are shown as unchecked).
for the rest of the  operations (like ALL,None, Reset...) takes the ones that are not checked in the data but marked on the screen and works with them as they were the ones that are checked
so for each place in the BITViewerDlg - Item->Checked means that the Item is really unchecked for the user.*/

  if(!Item->Checked) //changed from Item->Checked
	Item->ImageIndex = CHECK_IMAGE;
  else
    Item->ImageIndex = UN_CHECK_IMAGE;
}
//---------------------------------------------------------------------------
void __fastcall TBITViewerForm::TestsListViewMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
  TListItem *Item = NULL;
  Item = TestsListView->GetItemAt(X,Y);
  if(Item != NULL)
  {
    if(X < 18)
    {
      Item->Checked = !Item->Checked;
      SelectionGroupComboBox->ItemIndex = 0;
      SelectionSetComboBox->ItemIndex = 0;
      TestsListView->Refresh();
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TBITViewerForm::FormDestroy(TObject *Sender)
{
  delete m_BITProxy;
}
//---------------------------------------------------------------------------
void __fastcall TBITViewerForm::HelpActionExecute(TObject *Sender)
{
  QString FileName = Q2RTApplication->AppFilePath.Value();
  FileName += BIT_HELP_FILE_NAME;

  ExternalAppExecute(FileName.c_str(),"");
}
//---------------------------------------------------------------------------
void __fastcall TBITViewerForm::NoTestWasSelectedActionExecute(TObject *Sender)
{
  MessageDlg("Please select a test", mtInformation, TMsgDlgButtons() << mbOK, 0);
}
//---------------------------------------------------------------------------
void __fastcall TBITViewerForm::TestsListViewSelectItem(TObject *Sender,
      TListItem *Item, bool Selected)
{
  LastSelectedItem = Item;
}
//---------------------------------------------------------------------------
// Update an item test result
void __fastcall TBITViewerForm::RunActionExecute(TObject *Sender)
{


  // Reset the 'Empty Tray' variables...
  m_IsFirstTrayCleanQuery = true;
  m_IsTrayClean           = true;

  m_LastCombinedResults = trUnknown;
  UpdateCombinedStatus(trUnknown);

  try
  {
	int TotalTestsNum = 0;
    // Set the status of all selected tests to "unknown"
    for(int i = 0; i < TestsListView->Items->Count; i++)
    {
      TListItem *Item = TestsListView->Items->Item[i];

      if(!Item->Checked) //changed from Item->Checked (see note in TestsListViewGetImageIndex)
      {
        TotalTestsNum++;

        AnsiString GroupName = Item->SubItems->Strings[GROUP_ITEM_ID];
        AnsiString TestName = Item->Caption;

        CBackEndInterface::Instance()->ResetTest(GroupName.c_str(),TestName.c_str());
        Item->SubItemImages[STATUS_ITEM_ID] = UNKNOWN_IMAGE;
        Item->SubItems->Strings[ERROR_DESCRIPTION_ITEM_ID] = "";
      }
    }



    // Disable form until all selected tests are executed
    Enabled = false;

    // Prepate a list of items for execution
    TTestExecList ExecList = PrepareExecutionList();

    try
	{
		bool prepareStatus = true;

		for (TTestExecList::iterator i = ExecList.begin(); i != ExecList.end(); ++i)
		{
		TTestAttributes Attr = ResolveAttributesDependency((TTestAttributes)i->Item->Data);
		 BuildActionsForBit(Attr);
		}
		for (TTestExecList::iterator i = ExecList.begin(); i != ExecList.end(); ++i)
		{
		TTestAttributes Attr = ResolveAttributesDependency((TTestAttributes)i->Item->Data);
		 if(!PrepareForBit(Attr))
			prepareStatus = false;
		}

	  if(prepareStatus == false)
		{
			return;
		}
		int CurrentTestCounter = 0;
	  // Execute all tests in list
	  for(TTestExecList::iterator i = ExecList.begin(); i != ExecList.end(); ++i)
	  {
		Q2RTApplication->GetMachineManager()->MarkBITRunning(true);

		// Execute each test, break if canceled
        if(!DoTest(i->Item,i->GroupName,i->TestName,TotalTestsNum,CurrentTestCounter++))
        {
			Q2RTApplication->GetMachineManager()->MarkBITRunning(false);
			break;
		}
		
		Q2RTApplication->GetMachineManager()->MarkBITRunning(false);
	}

      // If at least one test was selected, do cleanup
      if(TotalTestsNum > 0)
      {
      	try
        {
            // Clean up everything (just to be on the safe side)
            UnPrepareForTest(taDoorLock |
                             taTrayInsert |
                             taHomeAxes |
                             taHeadsHeating |
                             taTrayHeating |
                             taHeadsFilling);
        }
        catch(EQException& QException)
        {
          MessageDlg(QException.GetErrorMsg().c_str(),mtError,TMsgDlgButtons() << mbOK,0);
        }

      }
    } __finally
      {
        // Re-enable the form in any case
        Enabled = true;

        // Make sure the progress form is closed
        BITProgressForm->Close();
      }

  AnsiString ReportFileName = Q2RTApplication->AppFilePath.Value().c_str() + AnsiString("BITReport.htm");
  CreateBITReportFile(ReportFileName);
//OBJET_MACHINE feature
  CBackEndInterface *BEInterface = CBackEndInterface::Instance();
  BEInterface->SetMachineActionsDate("BIT");

  } catch(EQException& QException)
    {
      MessageDlg(QException.GetErrorMsg().c_str(),mtError,TMsgDlgButtons() << mbOK,0);
    }
}
//---------------------------------------------------------------------------
void __fastcall TBITViewerForm::ResetAllActionExecute(TObject *Sender)
{
  CBackEndInterface::Instance()->ResetAllTests();

  for(int i = 0; i < TestsListView->Items->Count; i++)
  {
    TListItem *Item = TestsListView->Items->Item[i];
    Item->SubItemImages[STATUS_ITEM_ID] = UNKNOWN_IMAGE;
    Item->SubItems->Strings[ERROR_DESCRIPTION_ITEM_ID] = "";
  }

  TestsListView->Refresh();

  SelectNoneAction->Execute();

  m_LastCombinedResults = trUnknown;
  UpdateCombinedStatus(trUnknown);
}
//---------------------------------------------------------------------------
void __fastcall TBITViewerForm::CreateReportActionExecute(TObject *Sender)
{
  // Prepare the date/time string
  char DateTimeStr[MAX_DATE_SIZE];
  time_t CurrentTime = time(NULL);
  struct tm *LocalTimeTM = localtime(&CurrentTime);
  strftime(DateTimeStr,MAX_DATE_SIZE," %d-%m-%y %H-%M",LocalTimeTM);

  QString FileName;

  FileName += "BITReport";
  FileName += DateTimeStr;
  FileName += ".htm";

  // Create a report file
  SaveDialog1->FileName   = FileName.c_str();
//  SaveDialog1->FileName   = "Oved";
  SaveDialog1->InitialDir = Q2RTApplication->AppFilePath.Value().c_str();

  if(SaveDialog1->Execute())
    CreateBITReportFile(SaveDialog1->FileName);
}
//---------------------------------------------------------------------------
void __fastcall TBITViewerForm::ViewReportActionExecute(TObject *Sender)
{
  // View the report file
  if(m_LastReportFileName != "")
    Q2RTApplication->LaunchFile(m_LastReportFileName.c_str(),"");
}
//---------------------------------------------------------------------------
void __fastcall TBITViewerForm::SelectAllActionExecute(TObject *Sender)
{
  for(int i = 0; i < TestsListView->Items->Count; i++)
    TestsListView->Items->Item[i]->Checked = false; //changed from true (see note in TestsListViewGetImageIndex)

  SelectionGroupComboBox->ItemIndex = 0;
  SelectionSetComboBox->ItemIndex = 0;
}
//---------------------------------------------------------------------------
void __fastcall TBITViewerForm::SelectNoneActionExecute(TObject *Sender)
{
  for(int i = 0; i < TestsListView->Items->Count; i++)
    TestsListView->Items->Item[i]->Checked = true; //changed from false (see note in TestsListViewGetImageIndex)

  SelectionGroupComboBox->ItemIndex = 0;
  SelectionSetComboBox->ItemIndex = 0;
}
//---------------------------------------------------------------------------
void __fastcall TBITViewerForm::SelectionGroupComboBoxChange(TObject *Sender)
{
  if(SelectionGroupComboBox->ItemIndex > 0)
  {
    // Get the status of the shift and control keys
    bool ShiftIsDown = (GetKeyState(VK_LSHIFT) & 0x8000) || (GetKeyState(VK_RSHIFT) & 0x8000);
    bool CtrlIsDown = (GetKeyState(VK_LCONTROL) & 0x8000) || (GetKeyState(VK_RCONTROL)  & 0x8000);

    // Select none...
    if(!ShiftIsDown && !CtrlIsDown)
    {
      // De-select all items
      for(int i = 0; i < TestsListView->Items->Count; i++)
        TestsListView->Items->Item[i]->Checked = true;//changed from false (see note in TestsListViewGetImageIndex)

      // Select only items in the group
      SelectItemsInGroup(SelectionGroupComboBox->Text,false);//changed from true(see note in TestsListViewGetImageIndex)
    } else
      {
        // Add/remove to/from selection
        SelectItemsInGroup(SelectionGroupComboBox->Text,ShiftIsDown);
        SelectionGroupComboBox->ItemIndex = 0;
      }

    SelectionSetComboBox->ItemIndex = 0;
  }else
  {
    SelectNoneAction->Execute();
  }
}
//---------------------------------------------------------------------------
void __fastcall TBITViewerForm::RunSpecificTestActionExecute(TObject *Sender)
{
  if(LastSelectedItem != NULL)
  {
    LastSelectedItem->Selected = true;
    TestsListView->Refresh();
  }

  if(TestsListView->Selected != NULL)
    RunSingleTest(TestsListView->Selected);
  else
    NoTestWasSelectedAction->Execute();
}
//---------------------------------------------------------------------------
void __fastcall TBITViewerForm::ResetSelectedTestActionExecute(TObject *Sender)
{
  if(TestsListView->Selected != NULL)
  {
    // Get selected item group name and test name
    AnsiString GroupName = TestsListView->Selected->SubItems->Strings[GROUP_ITEM_ID];
    AnsiString TestName = TestsListView->Selected->Caption;

    CBackEndInterface::Instance()->ResetTest(GroupName.c_str(),TestName.c_str());
    TestsListView->Selected->SubItemImages[STATUS_ITEM_ID] = UNKNOWN_IMAGE;
    TestsListView->Selected->SubItems->Strings[ERROR_DESCRIPTION_ITEM_ID] = "";
  }
  else
    NoTestWasSelectedAction->Execute();
}
//---------------------------------------------------------------------------
// Show extended information about the test
void __fastcall TBITViewerForm::TestsListViewDblClick(TObject *Sender)
{
  if(TestsListView->Selected != NULL)
  {
    UpdateExtendedInfoForm();

    if(!BITExtendedResultsForm->Visible)
      //BITExtendedResultsForm->ShowModal();
	  ;
    else
      BITExtendedResultsForm->SetFocus();
  }
  else
    NoTestWasSelectedAction->Execute();
}
//---------------------------------------------------------------------------
void __fastcall TBITViewerForm::FormShow(TObject *Sender)
{  
  CFrontEndParams *FrontEndParams = CFrontEndParams::Instance();

  AlwaysOnTopAction->Checked = (FormStyle == fsStayOnTop);

  // Display the dialog in the last position
  if (FrontEndParams->BITDlgLeftPosition != -1)
    Left = FrontEndParams->BITDlgLeftPosition;
  else
    Left = (Screen->Width - Width) / 2;

  if (FrontEndParams->BITDlgTopPosition != -1)
    Top = FrontEndParams->BITDlgTopPosition;
  else
    Top = (Screen->Height - Height) / 2;

  GoButton->SetFocus();
  LastSelectedItem = NULL;  
  RefreshTests();
}
//---------------------------------------------------------------------------
void __fastcall TBITViewerForm::RefreshBITActionExecute(TObject *Sender)
{
  CBackEndInterface::Instance()->RefreshBIT();

  // Add python scripts to the BIT manager
  PythonAppBIT::Register(Q2RTApplication->GetBITManager());

  RefreshTestListItems();
  RefreshSelectionSetItems();  
}
//---------------------------------------------------------------------------
void __fastcall TBITViewerForm::AlwaysOnTopActionExecute(TObject *Sender)
{
  AlwaysOnTopAction->Checked = !AlwaysOnTopAction->Checked;

  // Temporarly disable the OnShow event
  TNotifyEvent TmpEvent = OnShow;
  OnShow = NULL;
  FormStyle = AlwaysOnTopAction->Checked ? fsStayOnTop : fsNormal;
  OnShow = TmpEvent;

  SelectNoneAction->Execute();
}
//---------------------------------------------------------------------------
void __fastcall TBITViewerForm::CloseDialogActionExecute(TObject *Sender)
{
  Close();        
}
//---------------------------------------------------------------------------
void __fastcall TBITViewerForm::FormClose(TObject *Sender,TCloseAction &Action)
{
  CFrontEndParams *FrontEndParams = CFrontEndParams::Instance();

  FrontEndParams->BITDlgLeftPosition = Left;
  FrontEndParams->BITDlgTopPosition = Top;
  FrontEndParams->BITDlgAlwaysOnTop = (FormStyle == fsStayOnTop);

  FrontEndParams->SaveSingleParameter(&FrontEndParams->BITDlgLeftPosition);
  FrontEndParams->SaveSingleParameter(&FrontEndParams->BITDlgTopPosition);
  FrontEndParams->SaveSingleParameter(&FrontEndParams->BITDlgAlwaysOnTop);

  // Restore the UI to idle mode
  FrontEndInterface->UpdateStatus(FE_UPDATE_UI_MODE,FE_UI_IDLE_MODE,true);
  FrontEndInterface->EnableDisableControl(FE_ONLINE_ENABLE,true,true);
}
//---------------------------------------------------------------------------
void __fastcall TBITViewerForm::CloseButtonClick(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------
void __fastcall TBITViewerForm::FormCreate(TObject *Sender)
{
  m_LastCombinedResults = trUnknown;
  UpdateCombinedStatus(trUnknown);

  // Create the BIT proxy object
  m_BITProxy = new CBITProxy;
}
//---------------------------------------------------------------------------
void __fastcall TBITViewerForm::SelectUnknownActionExecute(TObject *Sender)
{
  // Select all items in unknown state
  for(int i = 0; i < TestsListView->Items->Count; i++)
  {
    TListItem *Item = TestsListView->Items->Item[i];

    Item->Checked = !(Item->SubItemImages[STATUS_ITEM_ID] == UNKNOWN_IMAGE);//changed from without ! (see note in TestsListViewGetImageIndex)
  }

  SelectionGroupComboBox->ItemIndex = 0;
  SelectionSetComboBox->ItemIndex = 0;
}
//---------------------------------------------------------------------------
void __fastcall TBITViewerForm::SelectionSetComboBoxChange(TObject *Sender)
{
  if(SelectionSetComboBox->ItemIndex > 0)
    SelectItemsInBITSet(SelectionSetComboBox->Text);
  else
    SelectNoneAction->Execute();
}
//---------------------------------------------------------------------------
void __fastcall TBITViewerForm::AddSelectionSetActionExecute(TObject *Sender)
{
  AnsiString Str = InputBox("Add Selection Set","Enter selection set name:","");

  if(Str != "")
  {
    CBackEndInterface *BEInterface = CBackEndInterface::Instance();

    // Begin set definitions
    BEInterface->StartBITSetDefine(Str.c_str());

    // Add items
    for(int i = 0; i < TestsListView->Items->Count; i++)
    {
      TListItem *Item = TestsListView->Items->Item[i];


      if(!Item->Checked)//changed from without ! (see note in TestsListViewGetImageIndex)
      {
        AnsiString GroupName = Item->SubItems->Strings[GROUP_ITEM_ID];
        AnsiString TestName = Item->Caption;

        BEInterface->AddToBITSet(GroupName.c_str(),TestName.c_str());
      }
	}

    // Save to file
    BEInterface->SaveBITSet();
  }
}
/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/


