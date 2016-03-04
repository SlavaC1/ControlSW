/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Generic                                                 *
 * Module: PythonAutoWizard                                         *
 * Module Description: Interface with python script wizards.        *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 29/06/2003                                           *
 * Last upate: 02/07/2003                                           *
 ********************************************************************/

#include "PythonAutoWizard.h"
#include "QPythonIntegrator.h"
#include "QMonitor.h"
#include "PythonWizardPages.h"
#include "QStringList.h"
#include "QPythonUtils.h"


// Default constructor
CPythonAutoWizard::CPythonAutoWizard(void)
{
  InitWizard();
}

// Constructor
CPythonAutoWizard::CPythonAutoWizard(const QString PythonFileName)
{
  // Run the script
  QPythonIntegratorDM->ExecFile(PythonFileName.c_str());

  InitWizard();
}

void CPythonAutoWizard::InitWizard(void)
{
  m_PyEng = GetPythonEngine();

  InitWizardAttributes();
  CreatePages();
}

// Initialize the wizard attributes (like Title,ImageID, etc)
void CPythonAutoWizard::InitWizardAttributes(void)
{
  PPyObject MainModule = m_PyEng->GetMainModule();

  {
    CPyAutoDecRef TmpObj(m_PyEng->PyObject_GetAttrString(MainModule,"Title"));

    // Update 'Title'
    if(TmpObj != NULL)
    {
      AnsiString Str = m_PyEng->PyObjectAsString(TmpObj);
      m_Title = Str.c_str();
    }
  }

  {
    CPyAutoDecRef TmpObj(m_PyEng->PyObject_GetAttrString(MainModule,"HelpVisible"));

    // Update 'HelpVisible'
    if(TmpObj != NULL)
    {
      int TmpInt = m_PyEng->PyInt_AsLong(TmpObj);
      m_HelpVisible = TmpInt;
    }
  }

  {
    CPyAutoDecRef TmpObj(m_PyEng->PyObject_GetAttrString(MainModule,"DefaultImageID"));

    // Update 'DefaultImageID'
    if(TmpObj != NULL)
    {
      int TmpInt = m_PyEng->PyInt_AsLong(TmpObj);

      // If the default equal -1 use the application default
      if(TmpInt != -1)
        m_DefaultImageID = TmpInt;
    }
  }

  {
    CPyAutoDecRef TmpObj(m_PyEng->PyObject_GetAttrString(MainModule,"UserButton1Visible"));

    // Update 'UserButton1Visible'
    if(TmpObj != NULL)
    {
      int TmpInt = m_PyEng->PyInt_AsLong(TmpObj);
      m_UserButton1Visible = TmpInt;
    }
  }

  {
    CPyAutoDecRef TmpObj(m_PyEng->PyObject_GetAttrString(MainModule,"UserButton2Visible"));

    // Update 'UserButton2Visible'
    if(TmpObj != NULL)
    {
      int TmpInt = m_PyEng->PyInt_AsLong(TmpObj);
      m_UserButton2Visible = TmpInt;
    }
  }

  {
    CPyAutoDecRef TmpObj(m_PyEng->PyObject_GetAttrString(MainModule,"UserButton1Caption"));

    // Update 'UserButton1Caption'
    if(TmpObj != NULL)
    {
      AnsiString TmpStr = m_PyEng->PyObjectAsString(TmpObj);
      m_UserButton1Caption = TmpStr.c_str();
    }
  }

  {
    CPyAutoDecRef TmpObj(m_PyEng->PyObject_GetAttrString(MainModule,"UserButton2Caption"));

    // Update 'UserButton2Caption'
    if(TmpObj != NULL)
    {
      AnsiString TmpStr = m_PyEng->PyObjectAsString(TmpObj);
      m_UserButton2Caption = TmpStr.c_str();
    }
  }
}

// Create and add pages to the wizard
void CPythonAutoWizard::CreatePages(void)
{
  // Every wizard script need a "GetPages" function
  CPyAutoDecRef GetPagesFunc(m_PyEng->FindFunction("__main__","GetPages"));

  if(GetPagesFunc == NULL)
    throw EPythonAutoWizard("Wizard script must have \"GetPages\" function");

  // Call the "GetPages" function using low level python API
  PPyObject PagesList(m_PyEng->PyObject_CallObject(GetPagesFunc,NULL));

  // Check last python error and raise exception on error (low level APIs does not
  // raise exceptions on their own).
  m_PyEng->CheckError();

  // Check if the return type is a list
  if(!m_PyEng->PyList_Check(PagesList))
    throw EPythonAutoWizard("Invalid \"GetPages\" return value (must be list)");

  int PagesNum = m_PyEng->PyList_Size(PagesList);
  for(int i = 0; i < PagesNum; i++)
  {
    PPyObject PageObj = m_PyEng->PyList_GetItem(PagesList,i);

    CWizardPage *Page = CreatePageFromPyObject(PageObj);

    if(Page != NULL)
      AddPage(Page);
    else
      throw EPythonAutoWizard("Invalid wizard page");
  }
}

// Create a new C++ wizard page object
CWizardPage *CPythonAutoWizard::CreatePageFromPyObject(PPyObject PageObj)
{
  // Get wizard title
  CPyAutoDecRef TitleObj(m_PyEng->PyObject_GetAttr(PageObj,m_PyEng->PyString_FromString("Title")));

  if(TitleObj == NULL)
    throw EPythonAutoWizard("Could not find wizard page \"Title\" property");

  AnsiString Title = m_PyEng->PyObjectAsString(TitleObj);

  // Get wizard ImageID
  CPyAutoDecRef ImageIDObj(m_PyEng->PyObject_GetAttr(PageObj,m_PyEng->PyString_FromString("ImageID")));

  if(ImageIDObj == NULL)
    throw EPythonAutoWizard("Could not find wizard page \"ImageID\" property");

  int ImageID = m_PyEng->PyInt_AsLong(ImageIDObj);

  // Get wizard attributes word
  CPyAutoDecRef AttrObj(m_PyEng->PyObject_GetAttr(PageObj,m_PyEng->PyString_FromString("Attr")));

  if(AttrObj == NULL)
    throw EPythonAutoWizard("Could not find wizard page \"Attr\" property");

  int Attr = m_PyEng->PyInt_AsLong(AttrObj);

  // Try to find the "Type" attribute
  CPyAutoDecRef PageTypeObj(m_PyEng->PyObject_GetAttr(PageObj,m_PyEng->PyString_FromString("Type")));

  if(PageTypeObj == NULL)
    throw EPythonAutoWizard("Can not create type-less wizard pages");

  int PageType = m_PyEng->PyInt_AsLong(PageTypeObj);

  CWizardPage *Page;

  switch(PageType)
  {
    case wptMessage:
      Page = CreatePythonWizardPage<CPythonMessageWizardPage>(Title,ImageID,Attr,PageObj);
      break;

    case wptStatus:
      Page = CreatePythonWizardPage<CPythonStatusWizardPage>(Title,ImageID,Attr,PageObj);      
      break;

    case wptProgress:
      Page = CreatePythonWizardPage<CPythonProgressWizardPage>(Title,ImageID,Attr,PageObj);
      break;

    case wptCheckbox:
      Page = CreatePythonWizardPage<CPythonCheckBoxWizardPage>(Title,ImageID,Attr,PageObj);
      break;

    case wptRadioGroup:
      Page = CreatePythonWizardPage<CPythonRadioGroupWizardPage>(Title,ImageID,Attr,PageObj);      
      break;

    case wptProgressStatus:
      Page = CreatePythonWizardPage<CPythonProgressStatusWizardPage>(Title,ImageID,Attr,PageObj);      
      break;

    case wptDataEntry:
      Page = CreatePythonWizardPage<CPythonDataEntryWizardPage>(Title,ImageID,Attr,PageObj);
      break;

    default:
      Page = CreatePythonWizardPage<CPythonGenericCustomWizardPage>(Title,ImageID,Attr,PageObj);
      break;
  }

  return Page;
}

template <class TPageClass>
CWizardPage *CPythonAutoWizard::CreatePythonWizardPage(AnsiString Title,int ImageID,int Attr,PPyObject PageObj)
{
  TPageClass *Page = new TPageClass(Title.c_str(),ImageID,Attr);
  Page->SetTag1(PageObj);
  Page->RefreshPythonAttributes();

  return Page;
}

// Add a page to the wizard
void CPythonAutoWizard::AddPage(CWizardPage *NewPage)
{
  CAutoWizard::AddPage(NewPage);

  PPyObject PyPageObj = reinterpret_cast<PPyObject>(NewPage->GetTag1());
  SetAttrOfPyObjAsInt(PyPageObj,"PageNumber",NewPage->GetPageNumber());
}

// Help function for calling a pyhton function with page object
void CPythonAutoWizard::CallPythonFunctionWithPage(const AnsiString FuncName,CWizardPage *WizardPage)
{
  // EveryTry to find the page events handler
  CPyAutoDecRef PageEventsFunc(m_PyEng->FindFunction("__main__",FuncName));

  // Nothing happened if we don't find it
  if(PageEventsFunc != NULL)
  {
    PPyObject ParamsTuple = NULL;

    if(WizardPage != NULL)
    {
      PPyObject PyPageObj = reinterpret_cast<PPyObject>(WizardPage->GetTag1());
      ParamsTuple = m_PyEng->ArrayToPyTuple(ARRAYOFCONST((PyPageObj)));
    }

    m_PyEng->PyObject_CallObject(PageEventsFunc,ParamsTuple);
    m_PyEng->CheckError();
  }
}

void CPythonAutoWizard::PageEventsHandler(CWizardPage *Page,int Param1,int Param2)
{
  CallPythonFunctionWithPage("OnPageEvents",Page);
}

void CPythonAutoWizard::PageEnter(CWizardPage *Page)
{
  CallPythonFunctionWithPage("OnPageEnter",Page);
}

void CPythonAutoWizard::PageLeave(CWizardPage *Page,TWizardPageLeaveReason LeaveReason)
{
  // EveryTry to find the page events handler
  CPyAutoDecRef PageEventsFunc(m_PyEng->FindFunction("__main__","OnPageLeave"));

  // Nothing happened if we don't find it
  if(PageEventsFunc != NULL)
  {
    PPyObject ParamsTuple;

    if(Page != NULL)
    {
      PPyObject PyPageObj = reinterpret_cast<PPyObject>(Page->GetTag1());
      ParamsTuple = m_PyEng->ArrayToPyTuple(ARRAYOFCONST((PyPageObj,LeaveReason)));
    }
    else
      ParamsTuple = NULL;

    m_PyEng->PyObject_CallObject(PageEventsFunc,ParamsTuple);
    m_PyEng->CheckError();
  }
}

void CPythonAutoWizard::StartEvent(void)
{
  CallPythonFunctionWithPage("OnStart",NULL);
}

void CPythonAutoWizard::EndEvent(void)
{
  CallPythonFunctionWithPage("OnEnd",NULL);
}

void CPythonAutoWizard::CloseEvent(void)
{
  CallPythonFunctionWithPage("OnClose",NULL);
}

void CPythonAutoWizard::CancelEvent(CWizardPage *Page)
{
  CallPythonFunctionWithPage("OnCancel",Page);
}

void CPythonAutoWizard::HelpEvent(CWizardPage *Page)
{
  CallPythonFunctionWithPage("OnHelp",Page);
}

void CPythonAutoWizard::UserButton1Event(CWizardPage *Page)
{
  CallPythonFunctionWithPage("OnUserButton1",Page);
}

void CPythonAutoWizard::UserButton2Event(CWizardPage *Page)
{
  CallPythonFunctionWithPage("OnUserButton2",Page);
}

