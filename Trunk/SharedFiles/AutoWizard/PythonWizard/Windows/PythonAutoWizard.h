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
 * Last upate: 03/07/2003                                           *
 ********************************************************************/

#ifndef _PYTHON_AUTO_WIZARD_H_
#define _PYTHON_AUTO_WIZARD_H_

#include "AutoWizard.h"
#include "QException.h"
#include "VarPyth.hpp"


class CQStringList;

// Exception class for the parameters streaming classes
class EPythonAutoWizard : public EQException {
  public:
    EPythonAutoWizard(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};


class CPythonAutoWizard : public CAutoWizard {
  private:
    TPythonEngine *m_PyEng;

    void InitWizard(void);

    // Initialize the wizard attributes (like Title,ImageID, etc)
    void InitWizardAttributes(void);

    // Create and add pages to the wizard
    void CreatePages(void);

    CWizardPage *CreatePageFromPyObject(PPyObject PageObj);

    template <class TPageClass>
    CWizardPage *CreatePythonWizardPage(AnsiString Title,int ImageID,int Attr,PPyObject PageObj);

    // Add a page to the wizard
    virtual void AddPage(CWizardPage *NewPage);

    // Help function for calling a pyhton function with page object
    void CallPythonFunctionWithPage(const AnsiString FuncName,CWizardPage *WizardPage);

    // Override base class functions
    void PageEventsHandler(CWizardPage *Page,int Param1,int Param2);
    void PageEnter(CWizardPage *Page);
    void PageLeave(CWizardPage *Page,TWizardPageLeaveReason LeaveReason);
    void StartEvent(void);
    void EndEvent(void);
    void CloseEvent(void);
    void CancelEvent(CWizardPage *Page);
    void HelpEvent(CWizardPage *Page);
    void UserButton1Event(CWizardPage *Page);
    void UserButton2Event(CWizardPage *Page);

  public:
    // Default constructor
    CPythonAutoWizard(void);

    // Constructor
    CPythonAutoWizard(const QString PythonFileName);
};

#endif



