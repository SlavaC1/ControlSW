//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "EditorForm.h"
#pragma package(smart_init)
#pragma resource "*.dfm"
#include "hasp_api_cpp_.h"
#include "About.h"

//---------------------------------------------------------------------------

TPackageEditorForm *PackageEditorForm;

//---------------------------------------------------------------------------
__fastcall TPackageEditorForm::TPackageEditorForm(TComponent* Owner)
	: TForm(Owner)
{
	m_PackageReaderWriter = new CPackageReaderWriter();

	OpenPackageDialog->Filter = "DAT files (*.dat)|*.DAT|Any file (*.*)|*.*";
	SavePackageDialog->Filter = "DAT files (*.dat)|*.DAT|Any file (*.*)|*.*";

	m_DongleIsConnected = false;

	this->Height = 700;
	this->Width  = 1000;
}
//---------------------------------------------------------------------------
void __fastcall TPackageEditorForm::ExitMenuClick(TObject *Sender)
{
	Close();	
}
//---------------------------------------------------------------------------

void __fastcall TPackageEditorForm::OpenPackagesMenuClick(TObject *Sender)
{
	if(! m_DongleIsConnected)
	{
		MessageDlg("HASP dongle is not connected", mtWarning, TMsgDlgButtons() << mbOK, 0);
		return;
	}

	if(OpenPackageDialog->Execute())
	{
		if (FileExists(OpenPackageDialog->FileName))
			m_PackageReaderWriter->ReadDatabaseFile(OpenPackageDialog->FileName.c_str());

		AddImportedPackages();
	}
}
//---------------------------------------------------------------------------

void __fastcall TPackageEditorForm::SavePackagesMenuClick(TObject *Sender)
{
    if(! m_DongleIsConnected)
	{
		MessageDlg("HASP dongle is not connected", mtWarning, TMsgDlgButtons() << mbOK, 0);
		return;
	}

	if(SavePackageDialog->Execute())
		m_PackageReaderWriter->WriteDatabaseFile(SavePackageDialog->FileName.c_str());

}
//---------------------------------------------------------------------------

void __fastcall TPackageEditorForm::FormDestroy(TObject *Sender)
{
	if(m_PackageReaderWriter)
		delete m_PackageReaderWriter;
}
//---------------------------------------------------------------------------

void TPackageEditorForm::AddImportedPackages()
{
	m_PageControl->Free();

	// Page control to hold the machines tabs
	m_PageControl                  = new TPageControl(this);
	m_PageControl->Parent          = this;
	m_PageControl->Align           = alClient;
	m_PageControl->OnMouseActivate = PageControlAddNewTab;

	TMachinesList MachinesList = m_PackageReaderWriter->GetMachinesList();

	int Tag = 0;
	for(TMachinesList::iterator MachinesIt = MachinesList.begin() ; MachinesIt < MachinesList.end() ; MachinesIt++, Tag++)
	{
		// Tab for each machine type
		TTabSheet *Tab   = new TTabSheet(m_PageControl);
		Tab->PageControl = m_PageControl;
		Tab->Caption     = MachinesIt->MachineModelName.c_str();
		Tab->Name        = MachinesIt->MachineModelName.c_str();
		Tab->Tag         = Tag;

		AddPackagesTree(Tab, MachinesIt);
		AddPackagesEditor(Tab, MachinesIt);
	} 

	TTabSheet *AddMachineTab   = new TTabSheet(m_PageControl);
	AddMachineTab->PageControl = m_PageControl;
	AddMachineTab->Caption     = "   +";
	AddMachineTab->Name        = "AddMachineTab";
	AddMachineTab->Tag         = -10;
	
}

void __fastcall TPackageEditorForm::PageControlAddNewTab(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y, int HitTest,
	  TMouseActivate &MouseActivate)
{
	int index = dynamic_cast<TPageControl *>(Sender)->IndexOfTabAt(X,Y);
	if(index > -1)
	{
		if(m_PageControl->Pages[index]->Tag == -10)
		{
			TTabSheet *Tab            = new TTabSheet(m_PageControl);
			Tab->PageControl          = m_PageControl;
			Tab->PageIndex            = index;
			m_PageControl->ActivePage = Tab;
		}
    }
}

void TPackageEditorForm::AddPackagesEditor(TTabSheet *MachineTab, TMachinesList::iterator &MachinesIt)
{
	TPageControl *PackagesPageControl = new TPageControl(MachineTab);
	PackagesPageControl->Parent       = MachineTab;
	PackagesPageControl->Align        = alClient;

	for(int i = 0 ; i < MachineTab->ControlCount ; i++)
	{
		TSplitter *Splitter = dynamic_cast<TSplitter *>(MachineTab->Controls[i]);

		if(Splitter)
			PackagesPageControl->Left = Splitter->Left + Splitter->Width; 
	}

	int Tag = 0;
	for(TPackagesList::iterator PackagesIt = MachinesIt->PackagesList.begin() ; PackagesIt < MachinesIt->PackagesList.end() ; PackagesIt++, Tag++)
	{
		// Tab for each machine type
		TTabSheet *Tab   = new TTabSheet(PackagesPageControl);
		Tab->PageControl = PackagesPageControl;
		Tab->Caption     = PackagesIt->PackageName.c_str();
		Tab->Name        = PackagesIt->PackageName.c_str();
		Tab->Tag         = Tag;
		Tab->Padding->SetBounds(5,0,5,5);

		// Grid to hold the contols inside the tab
		TGridPanel *GridPanel = new TGridPanel(Tab);
		GridPanel->Parent     = Tab;
		GridPanel->Align      = alClient;
		GridPanel->BevelOuter = bvNone;
		GridPanel->ColumnCollection->BeginUpdate();
		GridPanel->ColumnCollection->Delete(1);
		GridPanel->RowCollection->Add();
		GridPanel->RowCollection->Items[0]->SizeStyle = ssPercent;
		GridPanel->RowCollection->Items[0]->Value     = 10;
		GridPanel->RowCollection->Items[1]->SizeStyle = ssPercent;
		GridPanel->RowCollection->Items[1]->Value     = 40;
		GridPanel->RowCollection->Items[2]->SizeStyle = ssPercent;
		GridPanel->RowCollection->Items[2]->Value     = 20;

		// Package feature edit group
		TGroupBox *FeatureGroupBox = new TGroupBox(GridPanel);
		FeatureGroupBox->Parent  = GridPanel;
		FeatureGroupBox->Caption = "HASP Feature";
		FeatureGroupBox->Align   = alClient;
		FeatureGroupBox->Tag     = Tag;
		FeatureGroupBox->Padding->Left = 10;

		TGridPanel *FeatureGridPanel = new TGridPanel(FeatureGroupBox);
		FeatureGridPanel->Parent     = FeatureGroupBox;
		FeatureGridPanel->Align      = alClient;
		FeatureGridPanel->BevelOuter = bvNone;
		FeatureGridPanel->RowCollection->Delete(1);
		FeatureGridPanel->ColumnCollection->Add();
		FeatureGridPanel->ColumnCollection->Add();

		FeatureGridPanel->ColumnCollection->BeginUpdate();

		TEdit *FeatureEdit  = new TEdit(FeatureGridPanel);
		FeatureEdit->Parent = FeatureGridPanel;
		FeatureEdit->Text   = IntToStr(PackagesIt->HaspFeature);
		FeatureEdit->Tag    = Tag;
		FeatureEdit->Name   = "FeatureEdit";

		TButton *FeatureApplyButton = new TButton(FeatureGridPanel);
		FeatureApplyButton->Parent  = FeatureGridPanel;
		FeatureApplyButton->Caption = "Apply";
		FeatureApplyButton->Name    = "ApplyFeature";
		FeatureApplyButton->Tag     = Tag;
		FeatureApplyButton->Top     = FeatureEdit->Top + FeatureEdit->Height / 2 - FeatureApplyButton->Height / 2;
		FeatureApplyButton->Left    = FeatureEdit->Left + FeatureEdit->Width + 5;

		FeatureGridPanel->ColumnCollection->EndUpdate();

		TGridPanel *MaterialsGridPanel = new TGridPanel(GridPanel);
		MaterialsGridPanel->Parent     = GridPanel;
		MaterialsGridPanel->Align      = alClient;
		MaterialsGridPanel->BevelOuter = bvNone;
		MaterialsGridPanel->RowCollection->Delete(1);

		MaterialsGridPanel->ColumnCollection->BeginUpdate();

		// Materials group box
		TGroupBox *MaterialsGroupBox        = new TGroupBox(MaterialsGridPanel);
		MaterialsGroupBox->Parent           = MaterialsGridPanel;
		MaterialsGroupBox->Caption          = "Materials";
		MaterialsGroupBox->Align            = alClient;
		MaterialsGroupBox->Tag              = Tag;
		MaterialsGroupBox->Name             = "MaterialsGroupBox";
		MaterialsGroupBox->AlignWithMargins = true;
		MaterialsGroupBox->Margins->SetBounds(0,0,5,0);

		TTreeView *MatView   = new TTreeView(MaterialsGroupBox);
		MatView->Parent      = MaterialsGroupBox;
		MatView->Align       = alClient;
		MatView->Tag         = Tag;
		MatView->ReadOnly    = true;
		MatView->BorderStyle = bsNone;
		MatView->Name        = "MaterialsTreeView";

		for(TLicensedMaterialsList::iterator MaterialsIt = PackagesIt->PackageMaterialsList.begin() ; MaterialsIt < PackagesIt->PackageMaterialsList.end() ; MaterialsIt++)
		{
            // Add only the materials from current package
			if(MaterialsIt->PackageName.compare(PackagesIt->PackageName) == 0)
				MatView->Items->Add(0,MaterialsIt->MaterialName.c_str());
		}

		// Digital Materials group box
		TGroupBox *DMGroupBox = new TGroupBox(MaterialsGridPanel);
		DMGroupBox->Parent    = MaterialsGridPanel;
		DMGroupBox->Caption   = "Digital Materials";
		DMGroupBox->Align     = alClient;
		DMGroupBox->Tag       = Tag;
		DMGroupBox->Name      = "DigitalMaterialsGroupBox";

		TTreeView *DMView   = new TTreeView(MaterialsGroupBox);
		DMView->Parent      = DMGroupBox;
		DMView->Align       = alClient;
		DMView->Tag         = Tag;
		DMView->ReadOnly    = true;
		DMView->BorderStyle = bsNone;
		DMView->Name        = "DigitalMaterialsTreeView";

		for(TLicensedMaterialsList::iterator MaterialsIt = PackagesIt->PackageDigitalMaterialsList.begin() ; MaterialsIt < PackagesIt->PackageDigitalMaterialsList.end() ; MaterialsIt++)
		{
            // Add only the materials from current package
			if(MaterialsIt->PackageName.compare(PackagesIt->PackageName) == 0)
				DMView->Items->Add(0,MaterialsIt->MaterialName.c_str());
		}

		MaterialsGridPanel->ColumnCollection->EndUpdate();

		// Modes group box
		TGroupBox *ModesGroupBox = new TGroupBox(GridPanel);
		ModesGroupBox->Parent    = GridPanel;
		ModesGroupBox->Caption   = "Operation modes";
		ModesGroupBox->Align     = alClient;
		ModesGroupBox->Tag       = Tag;
		ModesGroupBox->Name      = "ModesGroupBox";

		TTreeView *ModesView   = new TTreeView(ModesGroupBox);
		ModesView->Parent      = ModesGroupBox;
		ModesView->Align       = alClient;
		ModesView->Tag         = Tag;
		ModesView->ReadOnly    = true;
		ModesView->BorderStyle = bsNone;
		ModesView->Name        = "ModesTreeView";

		for(TLicensedModesList::iterator ModesIt = PackagesIt->PackageModesList.begin() ; ModesIt < PackagesIt->PackageModesList.end() ; ModesIt++)
		{
            // Add only the materials from current package
			if(ModesIt->PackageName.compare(PackagesIt->PackageName) == 0)
				ModesView->Items->Add(0,ShortModeNameToFullName(ModesIt->ModeName).c_str());
		}

		GridPanel->ColumnCollection->EndUpdate();
	}
}

// Tree to show the packages content for each machine
void TPackageEditorForm::AddPackagesTree(TTabSheet *MachineTab, TMachinesList::iterator &MachinesIt)
{
	TTreeView *PackagesTree = new TTreeView(MachineTab);
	PackagesTree->Parent    = MachineTab;
	PackagesTree->Align     = alLeft;
	PackagesTree->Width     = this->Width / 5;
	PackagesTree->Items->Clear();
	PackagesTree->Items->Add(NULL, "Packages");

	// Counting the nodes in the tree
	int NodesCounter = 0;
	TTreeNode *RootNode = PackagesTree->Items->Item[NodesCounter];

	for(TPackagesList::iterator PackagesIt = MachinesIt->PackagesList.begin() ; PackagesIt < MachinesIt->PackagesList.end() ; PackagesIt++, NodesCounter++)
	{
		QString Package = PackagesIt->PackageName + " - " + IntToStr(PackagesIt->HaspFeature).c_str();
		PackagesTree->Items->AddChild(RootNode, Package.c_str());
	}

	NodesCounter++;
	PackagesTree->Items->Add(NULL, "Materials");
	RootNode = PackagesTree->Items->Item[NodesCounter];

	for(TLicensedMaterialsList::iterator MaterialsIt = MachinesIt->MachineMaterialsList.begin() ; MaterialsIt < MachinesIt->MachineMaterialsList.end() ; MaterialsIt++, NodesCounter++)
	{
		PackagesTree->Items->AddChild(RootNode, MaterialsIt->MaterialName.c_str());
	}

	NodesCounter++;
	PackagesTree->Items->Add(NULL, "Digital Materials");
	RootNode = PackagesTree->Items->Item[NodesCounter];

	for(TLicensedMaterialsList::iterator MaterialsIt = MachinesIt->MachineDigitalMaterialsList.begin() ; MaterialsIt < MachinesIt->MachineDigitalMaterialsList.end() ; MaterialsIt++, NodesCounter++)
	{
		PackagesTree->Items->AddChild(RootNode, MaterialsIt->MaterialName.c_str());
	}

	NodesCounter++;
	PackagesTree->Items->Add(NULL, "Operation modes");
	RootNode = PackagesTree->Items->Item[NodesCounter];

	for(TLicensedModesList::iterator ModesIt = MachinesIt->MachineModesList.begin() ; ModesIt < MachinesIt->MachineModesList.end() ; ModesIt++, NodesCounter++)
	{
		PackagesTree->Items->AddChild(RootNode, ShortModeNameToFullName(ModesIt->ModeName).c_str());
	}

	PackagesTree->FullExpand();
	SetScrollPos(PackagesTree->Handle, SB_VERT, 0, true);

	TSplitter *Splitter = new TSplitter(MachineTab);
	Splitter->Parent    = MachineTab;
	Splitter->Left      = PackagesTree->Left + PackagesTree->Width;
}

// Modes names conversion
QString TPackageEditorForm::ShortModeNameToFullName(QString ShortName)
{
	if(ShortName.compare("HQ") == 0)             return "High Quality";
	else if(ShortName.compare("HS") == 0)        return "High Speed";
	else if(ShortName.compare("DM") == 0)        return "Digital Materials";
	else if(ShortName.compare("DR") == 0)        return "Draft";
	else if(ShortName.compare("Emulation") == 0) return "Emulation";
	else                                         return "";	
}

// Refresh method for a status bar content
void __fastcall TPackageEditorForm::ConnectionStatusBarDrawPanel(
      TStatusBar *StatusBar, TStatusPanel *Panel, const TRect &Rect)
{
	AnsiString Text;
	int LedIconIndex;

	if(m_DongleIsConnected)
	{
		Text = "HASP dongle is connected";
		LedIconIndex = 1;
	}
	else
	{
		Text = "HASP dongle is not connected";
		LedIconIndex = 0;
    }

	StatusBar->Canvas->TextRect(Rect,10 + LedsImageList->Width + Rect.Left, 2 + Rect.Top, Text);
	LedsImageList->Draw(ConnectionStatusBar->Canvas, Rect.Left, Rect.Top, LedIconIndex);
}
//---------------------------------------------------------------------------

// Timer that checks HASP dongle connectivity
void __fastcall TPackageEditorForm::DongleConnectionTimerTimer(TObject *Sender)
{
	int Feature = 0;
	Chasp hasp(ChaspFeature::fromFeature(Feature));
	try
	{
		haspStatus status = hasp.login(HASP_OBJET_VENDOR_CODE, HASP_LOCAL_SCOPE);
		m_DongleIsConnected = HASP_SUCCEEDED(status) ? true : false;
		ConnectionStatusBar->Refresh();
    }
	__finally
	{
		hasp.logout();
	}
}
//---------------------------------------------------------------------------

void __fastcall TPackageEditorForm::AboutMenuClick(TObject *Sender)
{
	AboutForm->ShowModal();	
}
//---------------------------------------------------------------------------


