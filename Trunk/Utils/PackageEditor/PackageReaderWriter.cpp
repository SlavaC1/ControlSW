//---------------------------------------------------------------------------

#pragma hdrstop
#include "PackageReaderWriter.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

#include <XMLDoc.hpp>
#include "hasp_api_cpp_.h"
#include <stdio.h>

CPackageReaderWriter::CPackageReaderWriter()
{
	m_FileBuffer = NULL; 
}

QString CPackageReaderWriter::DecryptDatabaseFile(QString FilePath)
{
	unsigned int FileSize = ReadFileIntoBuffer(FilePath.c_str());
	if(FileSize == 0)
        throw EPackageReaderWriter("The file is empty");

	int Feature = 0;
	Chasp hasp(ChaspFeature::fromFeature(Feature));
	try
	{
		haspStatus status = hasp.login(HASP_OBJET_VENDOR_CODE, HASP_LOCAL_SCOPE);

		if(HASP_SUCCEEDED(status))
		{
		   status = hasp.decrypt(m_FileBuffer, FileSize);
		   if(HASP_SUCCEEDED(status))
		   {
			   m_FileBuffer[FileSize] = '\0';
			   return reinterpret_cast<const char*>(m_FileBuffer);
		   }
		}
	}
	__finally
	{
		hasp.logout();
		delete[] m_FileBuffer;
	}

	return "";
}

unsigned int CPackageReaderWriter::ReadFileIntoBuffer(QString FilePath)
{
	FILE *File = fopen(FilePath.c_str(),"rb");

	try
	{
		if(File)
		{
			fseek(File, 0, SEEK_END);
			long FileSize = ftell(File);
			rewind(File);

			m_FileBuffer = new BYTE[FileSize + 1];

			return fread(m_FileBuffer,sizeof(BYTE),FileSize,File);
		}
	}
	__finally
	{
		fclose(File);
	}

	return 0;
}

void CPackageReaderWriter::ReadDatabaseFile(QString FilePath)
{
    m_MachinesList.clear();

	_di_IXMLDocument XMLDoc = NewXMLDocument();

	AnsiString XML = DecryptDatabaseFile(FilePath).c_str();

	XMLDoc->LoadFromXML(XML);

	_di_IXMLNode MachineFamilyNode = XMLDoc->DocumentElement->ChildNodes->GetNode(0);
	
	while(MachineFamilyNode)
	{	
		_di_IXMLNode MachineModelNode  = MachineFamilyNode->ChildNodes->FindNode(N_MACHINE);

		while(MachineModelNode)
		{
			AnsiString MachineModel = MachineModelNode->Attributes[A_MACHINE_MODEL];

			TLicensedMachine LicensedMachine;
			LicensedMachine.MachineModelName = MachineModel.c_str();

			_di_IXMLNode PackageNode = MachineModelNode->ChildNodes->FindNode(N_PACKAGE);

			while(PackageNode)
			{
				// Get package name and HASP feature
				AnsiString PackageFeature = PackageNode->Attributes[A_PACK_FEATURE];
				AnsiString PackageName    = PackageNode->Attributes[A_PACK_NAME];

				// Add package to the list
				TLicensedPackage LicensedPackage;
				LicensedPackage.PackageName = PackageName.c_str();
				LicensedPackage.HaspFeature = StrToInt(PackageFeature.c_str());

				// Get materials list
				_di_IXMLNode MaterialsNode = PackageNode->ChildNodes->FindNode(N_MATERIALS);
				if(MaterialsNode)
				{
					TLicensedMaterialItem LicensedMaterialItem;

					_di_IXMLNode MaterialNode = MaterialsNode->ChildNodes->FindNode(N_MATERIAL_ITEM);
					while(MaterialNode)
					{
						AnsiString Material = MaterialNode->Text;

						LicensedMaterialItem.PackageName  = PackageName.c_str();
						LicensedMaterialItem.MaterialName = Material.c_str();

						LicensedPackage.PackageMaterialsList.push_back(LicensedMaterialItem);

						TLicensedMaterialsList::iterator it = LicensedMachine.MachineMaterialsList.begin();
						for(; it < LicensedMachine.MachineMaterialsList.end() ; it++)
							if(it->MaterialName.compare(LicensedMaterialItem.MaterialName) == 0)
								break;

						if(it == LicensedMachine.MachineMaterialsList.end())
							LicensedMachine.MachineMaterialsList.push_back(LicensedMaterialItem);

						MaterialNode = MaterialNode->NextSibling();
					}
				}

				// Get materials list
				_di_IXMLNode DigitalMaterialsNode = PackageNode->ChildNodes->FindNode(N_DM_MATERIALS);
				if(DigitalMaterialsNode)
				{
					TLicensedMaterialItem LicensedMaterialItem;

					_di_IXMLNode MaterialNode = DigitalMaterialsNode->ChildNodes->FindNode(N_MATERIAL_ITEM);
					while(MaterialNode)
					{
						AnsiString Material = MaterialNode->Text;

						LicensedMaterialItem.PackageName  = PackageName.c_str();
						LicensedMaterialItem.MaterialName = Material.c_str();

						LicensedPackage.PackageDigitalMaterialsList.push_back(LicensedMaterialItem);

						TLicensedMaterialsList::iterator it = LicensedMachine.MachineDigitalMaterialsList.begin();
						for(; it < LicensedMachine.MachineDigitalMaterialsList.end() ; it++)
							if(it->MaterialName.compare(LicensedMaterialItem.MaterialName) == 0)
								break;

						if(it == LicensedMachine.MachineDigitalMaterialsList.end())
							LicensedMachine.MachineDigitalMaterialsList.push_back(LicensedMaterialItem);

						MaterialNode = MaterialNode->NextSibling();
					}
				}
				
				// Get operation modes list
				_di_IXMLNode OperationModesNode = PackageNode->ChildNodes->FindNode(N_MODES);
				if(OperationModesNode)
				{
					TLicensedModeItem LicensedModeItem;

					_di_IXMLNode ModeNode = OperationModesNode->ChildNodes->FindNode(N_MODE_ITEM);
					while(ModeNode)
					{
						AnsiString Mode = ModeNode->Text;

						LicensedModeItem.PackageName = PackageName.c_str();
						LicensedModeItem.ModeName    = Mode.c_str();

						LicensedPackage.PackageModesList.push_back(LicensedModeItem);

						TLicensedModesList::iterator it = LicensedMachine.MachineModesList.begin();
						for(; it < LicensedMachine.MachineModesList.end() ; it++)
							if(it->ModeName.compare(LicensedModeItem.ModeName) == 0)
								break;

						if(it == LicensedMachine.MachineModesList.end())
							LicensedMachine.MachineModesList.push_back(LicensedModeItem);

						ModeNode = ModeNode->NextSibling();
					}
				}
				
				LicensedMachine.PackagesList.push_back(LicensedPackage);
				
				// Check for next package
				PackageNode = PackageNode->NextSibling();
			}

			m_MachinesList.push_back(LicensedMachine);

			// Check for next machine model inside current FOM
			MachineModelNode = MachineModelNode->NextSibling();
		}
		
		// Check for next FOM
		MachineFamilyNode = MachineFamilyNode->NextSibling();
	}
}

TMachinesList CPackageReaderWriter::GetMachinesList()
{
    return m_MachinesList;
}

void CPackageReaderWriter::WriteDatabaseFile(QString FilePath)
{
	_di_IXMLDocument XMLDoc = NewXMLDocument();

	XMLDoc->Active       = true;
	XMLDoc->SetNodeIndentStr("    ");
	XMLDoc->Options      = XMLDoc->Options + (TXMLDocOptions() << doNodeAutoIndent);
	XMLDoc->ParseOptions = XMLDoc->ParseOptions + (TParseOptions() << poPreserveWhiteSpace);

	XMLDoc->DocumentElement = XMLDoc->CreateNode(N_ROOT, ntElement);

	for(TMachinesList::iterator MachinesIt = m_MachinesList.begin() ; MachinesIt < m_MachinesList.end() ; MachinesIt++)
	{
		_di_IXMLNode MachineNode = XMLDoc->DocumentElement->AddChild(N_MACHINE);
		MachineNode->SetAttribute(A_MACHINE_MODEL, MachinesIt->MachineModelName.c_str());

		for(TPackagesList::iterator PackagesIt = MachinesIt->PackagesList.begin() ; PackagesIt < MachinesIt->PackagesList.end() ; PackagesIt++)
		{
			_di_IXMLNode PackageNode = MachineNode->AddChild(N_PACKAGE);
			PackageNode->SetAttribute(A_PACK_FEATURE, PackagesIt->HaspFeature);
			PackageNode->SetAttribute(A_PACK_NAME,    PackagesIt->PackageName.c_str());

			_di_IXMLNode MaterialsNode = PackageNode->AddChild(N_MATERIALS);
			for(TLicensedMaterialsList::iterator MatIt = PackagesIt->PackageMaterialsList.begin() ; MatIt < PackagesIt->PackageMaterialsList.end() ; MatIt++)
			{
				_di_IXMLNode MatNode = MaterialsNode->AddChild(N_MATERIAL_ITEM);
				MatNode->Text        = MatIt->MaterialName.c_str();
			}

			_di_IXMLNode ModesNode = PackageNode->AddChild(N_MODES);
			for(TLicensedModesList::iterator ModesIt = PackagesIt->PackageModesList.begin() ; ModesIt < PackagesIt->PackageModesList.end() ; ModesIt++)
			{
				_di_IXMLNode ModeNode = ModesNode->AddChild(N_MODE_ITEM);
				ModeNode->Text        = ModesIt->ModeName.c_str();
			}
        }
    }

	//XMLDoc->XML->Text = FormatXMLData(XMLDoc->XML->Text);

	EncryptAndSaveFile(FilePath, XMLDoc->GetXML()->GetText());
}

unsigned int CPackageReaderWriter::EncryptAndSaveFile(QString FilePath, QString XmlAsString)
{
   m_FileBuffer = new BYTE[XmlAsString.size()];

   // Encryption comes here !!!!!

   m_FileBuffer = reinterpret_cast<BYTE *>(const_cast<char *>(XmlAsString.c_str()));
   m_FileBuffer[XmlAsString.size()] = '\0';

   // File saving

   FILE *File = fopen(FilePath.c_str(),"w");

	try
	{
		if(File && m_FileBuffer)
			return fwrite(m_FileBuffer,sizeof(BYTE),XmlAsString.size(),File);
	}
	__finally
	{
		fclose(File);
		delete[] m_FileBuffer;
	}

	return 0;
}

