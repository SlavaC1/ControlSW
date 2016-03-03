//----------------------------------------------------------------------------
#ifndef AboutH
#define AboutH
//----------------------------------------------------------------------------
#include <vcl\System.hpp>
#include <vcl\Windows.hpp>
#include <vcl\SysUtils.hpp>
#include <vcl\Classes.hpp>
#include <vcl\Graphics.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Buttons.hpp>
#include <vcl\ExtCtrls.hpp>
#include <jpeg.hpp>
#include "QUtils.h"
#include "MaintenanceCounters.h"
#include "AppParams.h"
#include "BackEndInterface.h"
#include <ComCtrls.hpp>


//----------------------------------------------------------------------------
class TAboutBox : public TForm
{
__published:
	TPanel *Panel1;
	TImage *ProgramIcon;
	TLabel *ProductName;
	TLabel *Version;
	TButton *OKButton;
        TLabel *ProductNameLabel;
        TLabel *VersionNum;
        TLabel *Label3;
        TLabel *Label2;
        TLabel *Label7;
        TLabel *BuildDateLabel;
        TLabel *BuildTimeLabel;
        TPageControl *PageControl2;
        TTabSheet *Versions;
        TGroupBox *GroupBox1;
        TLabel *OCBSWLabel;
        TLabel *Label1;
        TLabel *Label5;
        TLabel *OHDBSWLabel;
        TLabel *Label8;
        TLabel *OHDBHWLabel;
        TLabel *MotorSWLabel;
        TLabel *Label11;
        TLabel *MCBSWLabel;
        TLabel *EdenPCILabel;
        TLabel *MotorHWLabel;
        TLabel *MCBHWLabel;
        TLabel *Label4;
        TLabel *ElapsedPrintTimeLabel;
        TLabel *Label6;
        TLabel *ComputerNameLabel;
        TLabel *Label9;
        TLabel *Label10;
        TLabel *OCBA2DLabel;
        TLabel *OHDBA2DLabel;
        TLabel *Label12;
        TListBox *PatchesList;
        TLabel *Label14;
        TLabel *Label15;
        TLabel *Label16;
        TLabel *PatchVersion;
        TLabel *PatchBuildDate;
        TLabel *PatchDescription;
        TTabSheet *Patches;
    TLabel *Label17;
    TLabel *HaspAPILabel;
	TGroupBox *GroupBox2;
	TLabel *Label18;
	TLabel *Label19;
	TLabel *MSC1HWLabel;
    TLabel *Label20;
	TLabel *MSC2HWLabel;
    TLabel *PMType;
	TLabel *PMDueIn;
	TLabel *Label22;
	TLabel *Label23;
	TLabel *Label24;
	TShape *Shape1;
	TPanel *ResetPM;
	TLabel *PCIDriverLabel;
	TLabel *MSC3HWLabel;
	TLabel *MSC4HWLabel;
	TLabel *MSC3Label;
	TLabel *MSC4Label;
	TLabel *Label13;
	TLabel *ReleaseNumLabel;
        void __fastcall OKButtonClick(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall PatchesListClick(TObject *Sender);
	void __fastcall ResetPMMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall ResetPMMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall ResetPMClick(TObject *Sender);
private:

  void UpdateVersions(void);
  QString GetReleaseNumber();
  AnsiString ReadKey(AnsiString Key, AnsiString Value);
  void ReadPatchesFromRegistry();
  AnsiString GetVersionStringFormatted(AnsiString version);
  QString PadNumber(int Num, int NumOfDigitsInPaddingFormat);
  AnsiString m_sMachineType;
  
    void UpdatePMStrings(void);
  bool m_MaintenanceMode;
  CMaintenanceCounters* m_MC;
  CAppParams* m_AppParams;
  CBackEndInterface* m_BackEnd;

public:
	virtual __fastcall TAboutBox(TComponent* AOwner);
	void SetMaintenanceMode(bool a_Mode);
	void SetNextPMType(bool a_PMTypeIsSmall);

	QString GetPMDueIn();
	QString GetPMType();
};

QString GetVersionDate(void);
QString GetVersionTime(void);

//----------------------------------------------------------------------------
extern PACKAGE TAboutBox *AboutBox;
//----------------------------------------------------------------------------
#endif    
