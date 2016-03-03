//---------------------------------------------------------------------------

#ifndef MCBDiagDlgH
#define MCBDiagDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------

class CBackEndInterface;
class CAppParams;  

class TMCBDiagForm : public TForm
{
__published:	// IDE-managed Components
        TButton *CloseButton;
        TMemo *DiagText;
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TTabSheet *TabSheet2;
	TLabel *Label1;
	TEdit *DiagParameterEdit;
	TLabel *Label2;
	TEdit *DiagValueEdit;
	TButton *ReadButton;
	TButton *WriteButton;
	TButton *ReadAllButton;
	TLabel *Label3;
	TEdit *Edit1;
	TButton *Browse;
	TButton *MCBSWLoadStart;
	TProgressBar *ProgressBar1;
	TOpenDialog *OpenDialog1;
        TButton *Button1;
        void __fastcall ReadButtonClick(TObject *Sender);
        void __fastcall ReadAllButtonClick(TObject *Sender);
        void __fastcall WriteButtonClick(TObject *Sender);
        void __fastcall CloseButtonClick(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall FormShow(TObject *Sender);
	void __fastcall MCBSWLoadStartClick(TObject *Sender);
	void __fastcall BrowseClick(TObject *Sender);
        void __fastcall Button1Click(TObject *Sender);
private:	// User declarations

        CBackEndInterface* m_BackEndInterface;
        CAppParams*        m_ParamsMgr;
				bool               m_bCommOK;

				void MCBLoadInit();
				void MCBLoadProcess();
				void MCBLoadFinalize();
				int m_WiperHeightRestore;
				int m_YAxisHomeCountRestore;

public:		// User declarations
        __fastcall TMCBDiagForm(TComponent* Owner);  

};
//---------------------------------------------------------------------------
extern PACKAGE TMCBDiagForm *MCBDiagForm;
//---------------------------------------------------------------------------
#endif
