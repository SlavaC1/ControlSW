/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Generic                                                 *
 * Module: Automatic wizards generator                              *
 * Module Description: Wizard viewing dialog.                       *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 01/01/2003                                           *
 * Last upate: 05/01/2003                                           *
 ********************************************************************/

#ifndef AutoWizardDlgH
#define AutoWizardDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "WizardViewer.h"
//---------------------------------------------------------------------------

class TWizardForm : public TForm
{
__published:	// IDE-managed Components
        TWizardViewerFrame *WizardViewerFrame;
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall WizardViewerFramePrevButtonClick(TObject *Sender);
        void __fastcall WizardViewerFrameNextButtonClick(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall WizardViewerFrameCancelButtonClick(
          TObject *Sender);
private:	// User declarations

  void __fastcall OnDoneHandler(TObject *Sender);
  void __fastcall OnCancelHandler(TObject *Sender);  
  
  MESSAGE void HandleStatusUpdateMessage(TMessage &Message);  
  
public:		// User declarations
        __fastcall TWizardForm(TComponent* Owner);

  // Register a custom wizard page viwer
  void RegisterCustomPageViewer(TWizardPageType PageType,CCustomWizardPageViewer *WizardPageViewer);
  void UnRegisterCustomPageViewers(void);

  // Restart the wizard
  void Restart(int StartingPage = 0);

  bool RunWizard(CAutoWizard *Wizard,int StartingPage = 0);

  void CancelWizard(); // Cancels the wizard without asking the user.
  void GotoNextPageAsync();  

  TWizardViewerFrame* GetUIFrame();

  void UpdateStatus(int ControlID, QString Status);

  virtual void __fastcall Dispatch(void *Message);

};
//---------------------------------------------------------------------------
extern PACKAGE TWizardForm *WizardForm;
//---------------------------------------------------------------------------
#endif
