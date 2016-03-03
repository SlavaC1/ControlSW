//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "HaspPlugInfoDlg.h"
#include "AppParams.h"
#include "BackEndInterface.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
THaspPlugInformationDlg *HaspPlugInformationDlg;
//---------------------------------------------------------------------------
__fastcall THaspPlugInformationDlg::THaspPlugInformationDlg(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall THaspPlugInformationDlg::FormShow(TObject *Sender)
{
    LicensedPackagesTab->PageIndex = 0;
	PageControl1->ActivePageIndex = 0;

    // Packages version number 
	PacksVersionLabel->Caption = ("Packages database version: " +
								  CBackEndInterface::Instance()->GetPackagesDatabaseVersion()).c_str();

	// Packages information tab
	TPackagesList          PackagesList         = CBackEndInterface::Instance()->GetLicensedPackagesList();
	TLicensedMaterialsList MaterialsList        = CBackEndInterface::Instance()->GetLicensedMaterialsList();
	TLicensedMaterialsList DigitalMaterialsList = CBackEndInterface::Instance()->GetLicensedDigitalMaterialsList();
	TLicensedModesList     ModesList            = CBackEndInterface::Instance()->GetLicensedModesList();
	TTrayPropertiesList    TrayPropertiesList   = CBackEndInterface::Instance()->GetLicensedTrayPropertiesList();

	int NodesCounter = 0;

	PackagesTreeView->Items->Clear();
	PackagesTreeView->Items->Add(NULL, "Packages");

	TTreeNode *RootNode = PackagesTreeView->Items->Item[NodesCounter];

	for (TPackagesList::iterator it = PackagesList.begin() ; it != PackagesList.end() ; ++it, NodesCounter++)
		PackagesTreeView->Items->AddChild(RootNode, it->PackageName.c_str());

	NodesCounter++;

	PackagesTreeView->Items->Add(NULL, "Materials");
	RootNode = PackagesTreeView->Items->Item[NodesCounter];

	for (TLicensedMaterialsList::iterator it = MaterialsList.begin() ; it != MaterialsList.end() ; ++it, NodesCounter++)
		PackagesTreeView->Items->AddChild(RootNode, it->MaterialName.c_str());

	if(! DigitalMaterialsList.empty())
	{
		NodesCounter++;

		PackagesTreeView->Items->Add(NULL, "Digital Materials");
		RootNode = PackagesTreeView->Items->Item[NodesCounter];

		for (TLicensedMaterialsList::iterator it = DigitalMaterialsList.begin() ; it != DigitalMaterialsList.end() ; ++it, NodesCounter++)
			PackagesTreeView->Items->AddChild(RootNode, it->MaterialName.c_str());
    }

	if(! TrayPropertiesList.empty())
	{
		NodesCounter++;

		PackagesTreeView->Items->Add(NULL, "Tray Properties");
		RootNode = PackagesTreeView->Items->Item[NodesCounter];

		for (TTrayPropertiesList::iterator it = TrayPropertiesList.begin() ; it != TrayPropertiesList.end() ; ++it, NodesCounter++)
			PackagesTreeView->Items->AddChild(RootNode, it->PropertyName.c_str());
	}

	if(! ModesList.empty())
	{
		NodesCounter++;

		PackagesTreeView->Items->Add(NULL, "Operation modes");
		RootNode = PackagesTreeView->Items->Item[NodesCounter];

		for (TLicensedModesList::iterator it = ModesList.begin() ; it != ModesList.end() ; ++it, NodesCounter++)
			PackagesTreeView->Items->AddChild(RootNode, it->ModeFullName.c_str());
	}

	// Plug information tab

	HaspInfoGrid->Strings->Clear();
    THaspConnectionStatus ConnectionStatus = CBackEndInterface::Instance()->GetHaspConnectionStatus();

    if(ConnectionStatus == hsDisconnected)
    {
		HaspInfoGrid->InsertRow("Hasp is not detected","", true);
//		PackagesTreeView->Items->Add(NULL, "Hasp is not detected");
        return;
    }

    if(ConnectionStatus == hsInvalid)
    {
        HaspInfoGrid->InsertRow("Invalid product key","", true);
        return;
    }

    THaspPlugInfo PlugInfo = CBackEndInterface::Instance()->GetHaspPlugInfo();
    CAppParams* ParamMgr   = CAppParams::Instance();

    if(! PlugInfo.UptodateDriver)
    {
        HaspInfoGrid->InsertRow("Unable to provide plug info.","Upgrade your HASP driver.", true);
        return;
    }

    HaspInfoGrid->InsertRow("Computer name:",   PlugInfo.HostName.c_str(), true);
	HaspInfoGrid->InsertRow("Plug ID:",         PlugInfo.PlugID.c_str(), true);
    HaspInfoGrid->InsertRow("Plug model:",      PlugInfo.PlugModel.c_str(), true);
    HaspInfoGrid->InsertRow("License type:",    PlugInfo.LicenseType.c_str(), true);
    HaspInfoGrid->InsertRow("License status:", (PlugInfo.IsFeatureUsable == "true") ? "OK" : "Expired", true);

    if(PlugInfo.LicenseType == "trial" || PlugInfo.LicenseType == "expiration")
    {
        HaspInfoGrid->InsertRow("License expiration date:", IntToStr(PlugInfo.ExpirationDate.Day) + "-" +
                                                            IntToStr(PlugInfo.ExpirationDate.Month) + "-" +
                                                            IntToStr(PlugInfo.ExpirationDate.Year) + " - " +
                                                            IntToStr(PlugInfo.ExpirationDate.Hour) + ":" +
                                                          ((IntToStr(PlugInfo.ExpirationDate.Minute) == "0") ?
                                                            IntToStr(PlugInfo.ExpirationDate.Minute) + "0" : // pad with additional 0 if 0
                                                            IntToStr(PlugInfo.ExpirationDate.Minute)) + " (GMT)", true);                  

        HaspInfoGrid->InsertRow("Battery expiration date:", IntToStr(PlugInfo.ProductionDate.Day) + "-" +
                                                            IntToStr(PlugInfo.ProductionDate.Month) + "-" +
                                                            IntToStr(PlugInfo.ProductionDate.Year + ParamMgr->HASP_BatteryLifeYears), true);
	}
}
//---------------------------------------------------------------------------

void __fastcall THaspPlugInformationDlg::OKButtonClick(TObject *Sender)
{
    Close();    
}
//---------------------------------------------------------------------------

void __fastcall THaspPlugInformationDlg::HelpButtonClick(TObject *Sender)
{
    QString path = "Help\\HASP\\HASP.chm";

    CBackEndInterface::Instance()->DispatchHelp(10 /*tag*/, path);
}
//---------------------------------------------------------------------------


