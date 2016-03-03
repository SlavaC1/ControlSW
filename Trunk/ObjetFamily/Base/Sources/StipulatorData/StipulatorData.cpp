//---------------------------------------------------------------------------

#include "StipulatorData.h"


extern const int mapYOffset[];

//---------------------------------------------------------------------------


StipulatorData::StipulatorData() : m_SoftwareRevision(VERSIONS_NOT_IMPLEMENTED), m_SvnRevision(VERSIONS_NOT_IMPLEMENTED)
{
}

void StipulatorData::addTravelDirection(const int travelNumber)
{
	vector<int> travelDirectionsInSlice;
	
	if(m_FirstTravelInSlice)
	{
		if(travelNumber % 2 == 0)
			travelDirectionsInSlice.push_back(LEFT_TO_RIGHT);
		else
			travelDirectionsInSlice.push_back(RIGHT_TO_LEFT);

		m_TravelDirection.push_back(travelDirectionsInSlice);
		return;
	}

	if (travelNumber % 2 == 0)
		m_TravelDirection.back().push_back(LEFT_TO_RIGHT);
	else
		m_TravelDirection.back().push_back(RIGHT_TO_LEFT);
}

void StipulatorData::addNumberOfFires(const int numberOfFires, const int travelNumber)
{
	vector<int> numberOfFiresInSlice;

	if(m_FirstTravelInSlice)
	{
		numberOfFiresInSlice.push_back(numberOfFires);
		m_NumberOfFiresPerTravel.push_back(numberOfFiresInSlice);
		return;
	}

	m_NumberOfFiresPerTravel.back().push_back(numberOfFires);
}


/* Adds Y position at the start of a travel. */
void StipulatorData::addYPositionAtTravel(const long yPositionAtTravel, const int travelNumber)
{
	vector<long> yPositionsAtSlice;

	if(m_FirstTravelInSlice)
	{
		yPositionsAtSlice.push_back(yPositionAtTravel);
		m_YPositionAtTravel.push_back(yPositionsAtSlice);
		return;
	}

	m_YPositionAtTravel.back().push_back(yPositionAtTravel);


}


void StipulatorData::addXPositionEndAtTravel(const long xPositionEndAtTravel, const int travelNumber)
{
	/* Not implemented for now. doing the same as addXPositionStartAtTravel */
	vector<double> xPositionsAtSlice;

	if(m_FirstTravelInSlice)
	{
		if(travelNumber % 2 == 0)
			xPositionsAtSlice.push_back(xPositionEndAtTravel + X_OFFSET_AT_TRAVEL);
		else
			xPositionsAtSlice.push_back(xPositionEndAtTravel + X_OFFSET_AT_TRAVEL); //was X_OFFSET_AT_TRAVEL_BACKWARDS - will be needed if more fires will add to the end.

		m_XPositionEndAtTravel.push_back(xPositionsAtSlice);
		return;
	}

	if(travelNumber % 2 == 0)
		m_XPositionEndAtTravel.back().push_back(xPositionEndAtTravel + X_OFFSET_AT_TRAVEL);
	else
		m_XPositionEndAtTravel.back().push_back(xPositionEndAtTravel + X_OFFSET_AT_TRAVEL); //was X_OFFSET_AT_TRAVEL_BACKWARDS - will be needed if more fires will add to the end.
}

int StipulatorData::passParams(const int travelNumber, float* move_size, long* ticksCurY)
{

	CAppParams* ParamMgr  = CAppParams::Instance();
	int travelInPass = travelNumber % m_NumberOfTravelsPerPass.back().at(0);
	int base = 0;
	if ((travelNumber != 0) && (travelInPass == 0) && (!m_FirstTravelInSlice))
		m_PassNumber++;

	if (m_NumberOfTravelsPerPass.back().at(0) == 4)
	{
		switch (travelInPass)
		{
			case 0:
				*move_size = base;
				break;
			case 1:
				*move_size = base - ParamMgr->YSecondaryInterlaceNoOfPixels;
				break;
			case 2:
				*move_size = base + ParamMgr->YInterlaceNoOfPixels;
				break;
			case 3:
				*move_size = ParamMgr->YInterlaceNoOfPixels - ParamMgr->YSecondaryInterlaceNoOfPixels + base;
				break;
			default:
				break;
		}
	}
	else if(m_NumberOfTravelsPerPass.back().at(0) == 2)
	{
		switch (travelInPass)
		{
			case 0:
				*move_size = base;
				break;
			case 1:
				*move_size = base - ParamMgr->YInterlaceNoOfPixels;
				break;
			default:
				break;
		}
	}

	*ticksCurY = m_CurrentY;
	
	return m_PassNumber;
}

/* Adds X position at the start of a travel. */
void StipulatorData::addXPositionStartAtTravel(const long xPositionStartAtTravel, const int travelNumber)
{
	vector<double> xPositionsAtSlice;

	if(m_FirstTravelInSlice)
	{
		if(travelNumber % 2 == 0)
			xPositionsAtSlice.push_back(xPositionStartAtTravel + X_OFFSET_AT_TRAVEL);
		else
			xPositionsAtSlice.push_back(xPositionStartAtTravel + X_OFFSET_AT_TRAVEL); //was X_OFFSET_AT_TRAVEL_BACKWARDS - will be needed if more fires will add to the end.

		m_XPositionStartAtTravel.push_back(xPositionsAtSlice);
		return;
	}

	if(travelNumber % 2 == 0)
		m_XPositionStartAtTravel.back().push_back(xPositionStartAtTravel + X_OFFSET_AT_TRAVEL);
	else
		m_XPositionStartAtTravel.back().push_back(xPositionStartAtTravel + X_OFFSET_AT_TRAVEL); //was X_OFFSET_AT_TRAVEL_BACKWARDS - will be needed if more fires will add to the end.
}

//TODO in future: aquire real colors.
void StipulatorData::initializeMaterialsMaps()
{
   m_MaterialsCatalog["VeroWhite"] = 837;
   m_MaterialsColors["VeroWhite"] = RGBColor(245, 245, 245);
   m_MaterialsCatalog["VeroBlue"] = 840;
   m_MaterialsColors["VeroBlue"] = RGBColor(0, 0, 255);
   m_MaterialsCatalog["VeroBlack"] = 870;
   m_MaterialsColors["VeroBlack"] = RGBColor(10, 150, 20);
   m_MaterialsCatalog["FullCure720"] = 720;
   m_MaterialsColors["FullCure720"] = RGBColor(255, 255, 170);
   m_MaterialsCatalog["FullCure705"] = 705;
   m_MaterialsColors["FullCure705"] = RGBColor(255, 120, 120);
   m_MaterialsCatalog["TangoPlus"] = 930;
   m_MaterialsColors["TangoPlus"] = RGBColor(255, 47, 100);
   m_MaterialsCatalog["TangoPlusBlack"] = 980;
   m_MaterialsColors["TangoPlusBlack"] = RGBColor(80, 80, 12);
   m_MaterialsCatalog["TangoGray"] = 950;
   m_MaterialsColors["TangoGray"] = RGBColor(172, 172, 172);
   m_MaterialsCatalog["TangoBlack"] = 970;
   m_MaterialsColors["TangoBlack"] = RGBColor(58, 68, 78);
   m_MaterialsCatalog["FullCure640Clear"] = 640;
   m_MaterialsColors["FullCure640Clear"] = RGBColor(96, 158, 255);
   m_MaterialsCatalog["FullCure660RoseClear"] = 660;
   m_MaterialsColors["FullCure660RoseClear"] = RGBColor(255, 47, 130);
   m_MaterialsCatalog["FullCure680SkinTone"] = 680;
   m_MaterialsColors["FullCure680SkinTone"] = RGBColor(247, 219, 187);
   m_MaterialsCatalog["DurusWhite"] = 430;
   m_MaterialsColors["DurusWhite"] = RGBColor(225, 225, 225);
   m_MaterialsCatalog["RGD450"] = 450;
   m_MaterialsColors["RGD450"] = RGBColor(250, 206, 156);
   m_MaterialsCatalog["RGD515"] = 515;
   m_MaterialsColors["RGD515"] = RGBColor(187, 224, 227);
   m_MaterialsCatalog["RGD525"] = 525;
   m_MaterialsColors["RGD525"] = RGBColor(224, 224, 202);
   m_MaterialsCatalog["RGD531"] = 531;
   m_MaterialsColors["RGD531"] = RGBColor(251, 255, 253);
   m_MaterialsCatalog["RGD535"] = 535;
   m_MaterialsColors["RGD535"] = RGBColor(0, 128, 64);
   m_MaterialsCatalog["SolubleSupport"] = 707;
   m_MaterialsColors["SolubleSupport"] = RGBColor(1, 10, 100);
   m_MaterialsCatalog["MED610"] = 610;
   m_MaterialsColors["MED610"] = RGBColor(4, 40, 255);
   m_MaterialsCatalog["VeroYellow"] = 836;
   m_MaterialsColors["VeroYellow"] = RGBColor(255, 255, 0);
   m_MaterialsCatalog["VeroMagenta"] = 841;
   m_MaterialsColors["VeroMagenta"] = RGBColor(255, 0, 255);
   m_MaterialsCatalog["VeroCyan"] = 843;
   m_MaterialsColors["VeroCyan"] = RGBColor(0, 255, 255);
   m_MaterialsCatalog["VeroClear"] = 810;
   m_MaterialsColors["VeroClear"] = RGBColor(100, 10, 10);
   m_MaterialsCatalog["VeroGrey"] = 850;
   m_MaterialsColors["VeroGrey"] = RGBColor(172, 172, 172);
   m_MaterialsCatalog["VeroMgnt"] = 851;
   m_MaterialsColors["VeroMgnt"] = RGBColor(255, 0, 255);
   m_MaterialsCatalog["Undefined"] = 0;
   m_MaterialsColors["Undefined"] = RGBColor(133, 122, 99);


}
/* Initializes the materials catalog */
void StipulatorData::initializeMaterialsCatalog()
{
	CAppParams* ParamMgr  = CAppParams::Instance();
	initializeMaterialsMaps();
	/*Initializing catalog */
	for(map<QString, int>::iterator it= m_MaterialsCatalog.begin(); it!= m_MaterialsCatalog.end(); ++it)
	{
		QString name = it->first;
		int id = it->second;
		RGBColor color = RGBColor(m_MaterialsColors.find(it->first)->second);

		m_MaterialsTypes.push_back(Material(id, name, color));
	}

	vector<Material>::iterator it_materialIndex;
	vector<Material>::iterator it_undefinedMaterialIndex;
	
	/* Initializing materials inside heads  - TODO: I might need to change the order of the material according to the heads arrangement.*/
	 for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
	{
	   QString material = ParamMgr->TypesArrayPerChamber[i];
	   it_materialIndex = findMaterialInCatalog(material);
	   it_undefinedMaterialIndex = findMaterialInCatalog("Undefined");
	   if (it_materialIndex != m_MaterialsTypes.end()) {
			int headMaterialIndex = it_materialIndex - m_MaterialsTypes.begin();
			
			Material m = m_MaterialsTypes.at(headMaterialIndex);
			m_LogicalHeadMaterials.push_back(m);
			continue;
	   }
	   m_LogicalHeadMaterials.push_back(m_MaterialsTypes.at(it_undefinedMaterialIndex - m_MaterialsTypes.begin()));

	}
}

/* Finds a material inside the material catalog */
vector<Material>::iterator StipulatorData::findMaterialInCatalog(QString material)
{
	
   for(vector<Material>::iterator it = m_MaterialsTypes.begin(); it != m_MaterialsTypes.end(); ++it)
   {
	  if(strcmp(material.c_str(),(it->name).c_str()) ==0)
		 return it;
   }

   return m_MaterialsTypes.end();
}

QString StipulatorData::toString(string configurationFileName)
{
	QString configurationFileData = "";

	if (strcmp(configurationFileName.c_str(),"GeneralParameters.txt") == 0)
		configurationFileData = toStringGeneral();
	else if (strcmp(configurationFileName.c_str(),"SlicesInformation.txt") == 0)
		configurationFileData = toStringSlices();
	else if (strcmp(configurationFileName.c_str(),"Materials.txt") == 0)
		configurationFileData = toStringMaterials();

	return configurationFileData;

 }

 QString StipulatorData::toStringGeneral()
 {
 	QString configurationFileData = "";
	/* System */
	configurationFileData.append("/* System */\n");
	configurationFileData.append("SWRevision = ").append(QFloatToStr(m_SoftwareRevision)).append(" #none ").append("@draft.\n");
	configurationFileData.append("SVNRevision = ").append(QFloatToStr(m_SvnRevision)).append(" #none ").append("@Travails Original Keshet.\n");
	configurationFileData.append("OutputToLog = ").append(QIntToStr(m_OutputToLog)).append(" #none ").append("@No comment.\n");
	configurationFileData.append("SeparateSliceToTravels = ").append(QIntToStr(m_SeparateSliceToTravels)).append(" #none ").append("@ If set, output bitmaps will be for each travel. Will crush if save3DModel = true.\n");
	configurationFileData.append("FullTray = ").append(QIntToStr(m_FullTray)).append(" #none ").append("@ If set, output bitmaps will be created in tray size.\n");
	configurationFileData.append("printSlicesRange = ").append(QIntToStr(m_PrintSlicesRange.at(0))).append(", ");
	configurationFileData.append(QIntToStr(m_PrintSlicesRange.at(1))).append(" #none ").append("@-1,-1 for all slices or other range.\n");
	configurationFileData.append("BinaryDataStorageFrequency = ").append(QIntToStr(m_BinaryDataStorageFrequency)).append(" #none ").append("@ 0 - per slice. 1 - per pass. 2 - per travel.\n");
	configurationFileData.append("BackgroundColor = ").append(QIntToStr(m_BackgroundColor.at(0))).append(",");
	configurationFileData.append(QIntToStr(m_BackgroundColor.at(1))).append(",");
	configurationFileData.append(QIntToStr(m_BackgroundColor.at(2))).append(" #none ").append("@Background color.\n");
	configurationFileData.append("SlicesFormat = ").append(m_slicesFormat).append(" #none ").append("@Pictures file format.(choose from .jpeg, .tiff, .png).\n");
	configurationFileData.append("OutputPath = ").append(m_outputPath).append(" #none ").append("@output pictures and 3dModel path. Directory mush exist!.\n\n");
	
	/* 3D Model */
	configurationFileData.append("/* 3D Model */\n");
	configurationFileData.append("save3DModel = ").append(QIntToStr(m_Save3DModel)).append(" #none ").append("@saves a volume of the 3d model.\n");
	configurationFileData.append("OnlyModel = ").append(QIntToStr(m_OnlyModel)).append(" #none ").append("@Creating only the model from slices pictures.\n");
	configurationFileData.append("ModelForm = ").append(QIntToStr(m_ModelForm)).append(" #none ").append("@0 - slices, 1 - inflation.\n");
	configurationFileData.append("DistanceBetweenSlices = ").append(QIntToStr(m_DistanceBetweenSlices)).append(" #none ").append("@Used only when \"ModelForm = 0\" - value is in scene units.\n");
	configurationFileData.append("SamplingOmitionRate = ").append(QIntToStr(m_SamplingOmitionRate)).append(" #none ").append("@Omit 1 to x slices for building 3d model, -1 : don't omit slices.\n");
	configurationFileData.append("SamplingInflationRate = ").append(QIntToStr(m_SamplingInflationRate)).append(" #none ").append("@Used only when \"ModelForm = 1\" - Take 1 to x slices for building 3d model.\n");
	configurationFileData.append("RangeRelatedInflation = ").append(QIntToStr(m_RangeRelatedInflation)).append(" #none ").append("@Used only when \"ModelForm = 1\" - keep the model height even if partial range of the slices are printed.\n\n");

    /* Data Order */
	configurationFileData.append("/* Data Order */\n");
	configurationFileData.append("DataVersion = ").append(QIntToStr(m_DataVersion)).append(" #none ").append("@ 0: good old, do nothing.\n");
	configurationFileData.append("InvertBytesFirst = ").append(QIntToStr(m_InvertBytesFirst)).append(" #none ").append("@No comment.\n");
	configurationFileData.append("InvertHeadLast = ").append(QIntToStr(m_InvertHeadLast)).append(" #none ").append("@No comment.\n");
	configurationFileData.append("InvertLogicalHeadsOrder = ").append(QIntToStr(m_InvertLogicalHeadsOrder)).append(" #none ").append("@invert heads from left to right.\n");
	configurationFileData.append("YOffset = ").append(QIntToStr(m_Yoffset)).append(" #pixels ").append("@set to 0 in good old version 0.\n\n");

	/* Printer */
	configurationFileData.append("/* Printer */\n");
	configurationFileData.append("Presentation = ").append(QIntToStr(m_Presentation)).append(" #none ").append("@Presentation form. 0 - Cartesien. 1 - Polar.\n");
	configurationFileData.append("NumberOfLogicalHeads = ").append(QIntToStr(m_NumberOfLogicalHeads)).append(" #none ").append("@number of logical heads.\n");
	configurationFileData.append("NumberOfPartialPhysicalHeads = ").append(QIntToStr(m_NumberOfPartialPhysicalHeads)).append(" #none ").append("@number of partial physical heads.\n");
	configurationFileData.append("NozzlesPerLogicalHead = ").append(QIntToStr(m_NozzlesPerLogicalHead)).append(" #none ").append("@number of nozzles in one logical head.\n");
	configurationFileData.append("NozzlesPerPartialPhysicalHead = ").append(QIntToStr(m_NozzlesPerPartialPhysicalHead)).append(" #none ").append("@number of nozzles in one partial physical head.\n");

	configurationFileData.append("PixelResolutionXYZ = ");
	for (vector<double>::iterator it = m_PixelResolutionXYZ.begin(); it != m_PixelResolutionXYZ.end(); ++it)
	{
		configurationFileData.append(QFloatToStr(*it)).append(", ");
	}
	configurationFileData = configurationFileData.substr(0, configurationFileData.size()-2);
	configurationFileData.append(" #dpi ").append("@image prepared to this dpi, used for head distance Y.\n");

	configurationFileData.append("LogicalHeadNozzleResolution = ").append(QFloatToStr(m_LogicalHeadNozzleResolution)).append(" #dpi ").append("@no comment.\n");

	configurationFileData.append("StepsPerTickXYZ = ");
	for (vector<double>::iterator it = m_StepsPerTickXYZ.begin(); it != m_StepsPerTickXYZ.end(); ++it)
	{
		configurationFileData.append(QFloatToStr(*it)).append(", ");
	}
	configurationFileData = configurationFileData.substr(0, configurationFileData.size()-2);
	configurationFileData.append(" #ticks ").append("@: 615.7/x=1202.5, 1199.2*y=297.9764.\n");

	configurationFileData.append("TicksResolutionXYZ = ");
	for (vector<double>::iterator it = m_TicksResolutionXYZ.begin(); it != m_TicksResolutionXYZ.end(); ++it)
	{
		configurationFileData.append(QFloatToStr(*it)).append(", ");
	}
	configurationFileData = configurationFileData.substr(0, configurationFileData.size()-2);
	configurationFileData.append(" #dpi ").append("@: from the above, defines what is a tick, ticks X used for head distance.\n");

	configurationFileData.append("ActualDistanceBetweenPartialPhysicalHeadsX = ");
	for (vector<double>::iterator it = m_ActualDistanceBetweenPartialPhysicalHeadsX.begin(); it != m_ActualDistanceBetweenPartialPhysicalHeadsX.end(); ++it)
	{
		configurationFileData.append(QFloatToStr(*it)).append(", ");
	}
	configurationFileData = configurationFileData.substr(0, configurationFileData.size()-2);
	configurationFileData.append(" #ticks ").append("@of 1200, 1cm between heads.\n");

	configurationFileData.append("ActualDistanceBetweenPartialPhysicalHeadsY = ");
	for (vector<double>::iterator it = m_ActualDistanceBetweenPartialPhysicalHeadsY.begin(); it != m_ActualDistanceBetweenPartialPhysicalHeadsY.end(); ++it)
	{
		configurationFileData.append(QFloatToStr(*it)).append(", ");
	}
	configurationFileData = configurationFileData.substr(0, configurationFileData.size()-2);
	configurationFileData.append(" #pixels ").append("@of 300 exactly.\n");
	configurationFileData.append("TrayLengthXYZ = ");
	for (vector<double>::iterator it = m_TrayLengthXYZ.begin(); it != m_TrayLengthXYZ.end(); ++it)
	{
		configurationFileData.append(QFloatToStr(*it)).append(", ");
	}
	configurationFileData = configurationFileData.substr(0, configurationFileData.size()-2);
	configurationFileData.append(" #mm ").append("@no comment.");

	return  configurationFileData;
 }
 QString StipulatorData::toStringSlices()
 {
	QString configurationFileData = "";
	configurationFileData.append("*Slices Information*\n\n");
	
 	unsigned int sliceNum;
	for (sliceNum = 0; sliceNum < m_NumberOfPasses.size(); sliceNum++)
	{
		configurationFileData.append("---------- Slice ").append(QIntToStr(sliceNum)).append(" ----------\n");

		configurationFileData.append("NumberOfPasses = ").append(QFloatToStr(m_NumberOfPasses.at(sliceNum))).append(" #none ").append("@number of passes.\n");
		configurationFileData.append("NumberOfTravelsPerPass = ");
		
		for (vector<int>::iterator it = m_NumberOfTravelsPerPass.at(sliceNum).begin(); it != m_NumberOfTravelsPerPass.at(sliceNum).end(); ++it)
		{
			configurationFileData.append(QIntToStr(*it)).append(", ");
		}
		configurationFileData = configurationFileData.substr(0, configurationFileData.size()-2);
		configurationFileData.append(" #none ").append("@number of travels per pass.\n");
	
		configurationFileData.append("BidirectionalityPerPass = ");
		for (vector<int>::iterator it = m_BidirectionalityPerPass.at(sliceNum).begin(); it != m_BidirectionalityPerPass.at(sliceNum).end(); ++it)
		{
			configurationFileData.append(QIntToStr(*it)).append(", ");
		}
		configurationFileData = configurationFileData.substr(0, configurationFileData.size()-2);
		configurationFileData.append(" #none ").append("@no comment.\n");

		configurationFileData.append("TravelDirection = ");
		for (vector<int>::iterator it = m_TravelDirection.at(sliceNum).begin(); it != m_TravelDirection.at(sliceNum).end(); ++it)
		{
			configurationFileData.append(QFloatToStr(*it)).append(", ");
		}
		configurationFileData = configurationFileData.substr(0, configurationFileData.size()-2);
		configurationFileData.append(" #none ").append("@1 - right to left, 0 - left to right.\n");

		configurationFileData.append("NumberOfFires = ");
		for (vector<int>::iterator it = m_NumberOfFiresPerTravel.at(sliceNum).begin(); it != m_NumberOfFiresPerTravel.at(sliceNum).end(); ++it)
		{
			configurationFileData.append(QFloatToStr(*it)).append(", ");
		}
		configurationFileData = configurationFileData.substr(0, configurationFileData.size()-2);
		configurationFileData.append(" #none ").append("@number of fires.\n");
		
		try {
			configurationFileData.append("YPositionAtTravel = ");
			for (vector<long>::iterator it = m_YPositionAtTravel.at(sliceNum).begin(); it != m_YPositionAtTravel.at(sliceNum).end(); ++it)
			{
				configurationFileData.append(QFloatToStr(*it)).append(", ");
			}
		}
		catch(...)
		{
		}
		
		configurationFileData = configurationFileData.substr(0, configurationFileData.size()-2);
		configurationFileData.append(" #steps ").append("@steps of 1199.2dpi, 0.4236mm jumps.\n");
			
		configurationFileData.append("XPositionStartOfTravel = ");
		for (vector<double>::iterator it = m_XPositionStartAtTravel.at(sliceNum).begin(); it != m_XPositionStartAtTravel.at(sliceNum).end(); ++it)
		{
			configurationFileData.append(QFloatToStr(*it)).append(", ");
		}
		configurationFileData = configurationFileData.substr(0, configurationFileData.size()-2);
		configurationFileData.append(" #pixels ").append("@pixels of 600.\n");

		configurationFileData.append("XPositionEndOfTravel = ");
		for (vector<double>::iterator it = m_XPositionEndAtTravel.at(sliceNum).begin(); it != m_XPositionEndAtTravel.at(sliceNum).end(); ++it)
		{
			configurationFileData.append(QFloatToStr(*it)).append(", ");
		}
		configurationFileData = configurationFileData.substr(0, configurationFileData.size()-2);
		configurationFileData.append(" #pixels ").append("@pixels of 600.\n");

		configurationFileData.append("---------- End of Slice ").append(QIntToStr(sliceNum)).append(" ----------\n");

	}

	return configurationFileData;
 }

 QString StipulatorData::toStringMaterials()
 {
	QString configurationFileData = "";

	/* Materials */
	configurationFileData.append("/* Materials */\n\n");
	configurationFileData.append("LogicalHeadMaterials = ");
	for (vector<Material>::iterator it = m_LogicalHeadMaterials.begin(); it != m_LogicalHeadMaterials.end(); ++it)
	{
		if(it->id == 0)
			configurationFileData.append("000, ");
		else
			configurationFileData.append(QIntToStr(it->id)).append(", ");
	}
	configurationFileData = configurationFileData.substr(0, configurationFileData.size()-2);
	configurationFileData.append(" #none ").append("@currently loaded materials.\n\n");

	configurationFileData.append("/* Materials Catalog */\n\n");
	configurationFileData.append("< Material Code | Material Name | Red Value | Green Value | Blue Value >\n");
	for(unsigned int i = 0; i < m_MaterialsTypes.size(); i++)
	{
		configurationFileData.append("< ");
		
		if (m_MaterialsTypes.at(i).id == 0)
			configurationFileData.append("000 | ");
		else
        	configurationFileData.append(QIntToStr(m_MaterialsTypes.at(i).id)).append(" | ");

		configurationFileData.append(m_MaterialsTypes.at(i).name).append(" | ");
		configurationFileData.append(QIntToStr(m_MaterialsTypes.at(i).color.red)).append(" | ");
		configurationFileData.append(QIntToStr(m_MaterialsTypes.at(i).color.green)).append(" | ");
		configurationFileData.append(QIntToStr(m_MaterialsTypes.at(i).color.blue)).append(" >\n");
    }
	configurationFileData.append("\n/* End Of Materials Catalog */\n");
 	return configurationFileData;
 }

 /* Initializes the folder which contains the data and the stipulator fields */
 void StipulatorData::resetStipulatorData(THeadMapEntry offsetsArray[NUM_OF_LOGICAL_HEADS][NOZZLES_PER_HEAD])
 {
	CAppParams* params = CAppParams::Instance();
	
	QString copyiedPath = params->DumpPassToFilePath;

	// Deleting directories in order to initialize travels name from 0 in GenerateOneTravelData(...)
	ForceDirectories(copyiedPath.c_str());
	deleteFolder(copyiedPath);

	resetFields();
	initializingGeneralStipulatorFields(offsetsArray);
 }

 /* Resets the Stipulator data fields.*/
 void StipulatorData::resetFields()
 {
	m_SoftwareRevision = 0;
	m_SvnRevision = 0;
	m_OutputToLog = 0;
	m_SeparateSliceToTravels = 0;
	m_FullTray = 0;
	m_PrintSlicesRange.clear();
	m_BinaryDataStorageFrequency = 0;
	m_BackgroundColor.clear();
	m_slicesFormat = "";
	m_outputPath = "";
	
	m_Save3DModel = 0;
	m_OnlyModel = 0;
	m_ModelForm = 0;
	m_DistanceBetweenSlices = 1;
	m_SamplingOmitionRate = -1;
	m_SamplingInflationRate = 1;
	m_RangeRelatedInflation = 1;

	m_DataVersion = 0;
	m_InvertBytesFirst = 0;
	m_InvertHeadLast = 0;
	m_InvertLogicalHeadsOrder = 0;
	m_Yoffset = 0;

	m_Presentation = 0;
	m_NozzlesPerLogicalHead = 0;
	m_NozzlesPerPartialPhysicalHead = 0;
	m_NumberOfLogicalHeads = 0;
	m_NumberOfPartialPhysicalHeads = 0;
	m_PixelResolutionXYZ.clear();
	m_LogicalHeadNozzleResolution = 0;
	m_StepsPerTickXYZ.clear();
	m_TicksResolutionXYZ.clear();
	m_ActualDistanceBetweenPartialPhysicalHeadsX.clear();
	m_ActualDistanceBetweenPartialPhysicalHeadsY.clear();
	m_LogicalHeadMaterials.clear();
	m_MaterialsTypes.clear();
	m_MaterialsNames.clear();
	m_TrayLengthXYZ.clear();

	m_NumberOfPasses.clear();
	m_NumberOfTravelsPerPass.clear();
	m_BidirectionalityPerPass.clear();
	m_TravelDirection.clear();
	m_NumberOfFiresPerTravel.clear();
	m_YPositionAtTravel.clear();
	m_XPositionStartAtTravel.clear();
	m_XPositionEndAtTravel.clear();

	m_IsPritingTime = true;
	m_PassNumber = 0;
	m_SliceNumber = -1;
	m_FirstTravelInSlice = true;
	m_CurrentY = 0;
 }

/* Initializes general information for the whole print job */
 void StipulatorData::initializingGeneralStipulatorFields(THeadMapEntry offsetsArray[NUM_OF_LOGICAL_HEADS][NOZZLES_PER_HEAD])
{
	CAppParams *m_ParamsMgr = CAppParams::Instance();
	int logicalHeadNum;
	int partialPhysicalHeadInLogicalHead;

	/* System */
	m_SoftwareRevision = VERSIONS_NOT_IMPLEMENTED;
	m_SvnRevision = VERSIONS_NOT_IMPLEMENTED;
	m_OutputToLog = OUTPUT_TO_LOG;
	m_SeparateSliceToTravels = SEPARATE_SLICE_TO_TRAVELS;
	m_FullTray = DEFAULT_FULL_TRAY_MODE;
	m_PrintSlicesRange.push_back(DEFAULT_SLICES_RANGE); //start
	m_PrintSlicesRange.push_back(DEFAULT_SLICES_RANGE); //end
	m_BinaryDataStorageFrequency = DEFAULT_BINARY_DATA_STORAGE_FREQUENCY;
	m_BackgroundColor.push_back(DEFAULT_COLOR_FOR_RGB_BACKGROUND); //red
	m_BackgroundColor.push_back(DEFAULT_COLOR_FOR_RGB_BACKGROUND); //green
	m_BackgroundColor.push_back(DEFAULT_COLOR_FOR_RGB_BACKGROUND); //blue
	m_slicesFormat = DEFAULT_SLICES_FORMAT;
	m_outputPath = DEFAULT_OUTPUT_3DMODEL_PATH;
	
	/* 3D Model */
	m_Save3DModel = SAVE_3D_MODEL;
	m_OnlyModel = ONLY_MODEL;
	m_ModelForm = MODEL_FORM;
	m_DistanceBetweenSlices = DISTANCE_BETWEEN_SLICES;
	m_SamplingOmitionRate = SAMPLING_OMITION_RATE;
	m_SamplingInflationRate = SAMPLING_INFLATION_RATE;
	m_RangeRelatedInflation = RANGE_RELATED_INFLATION;

	/* Data Order */
	m_DataVersion = DEFAULT_DATA_VERSION;
	m_InvertBytesFirst = DEFAULT_INVERT_BYTES_FIRST;
	m_InvertHeadLast = DEFAULT_INVERT_HEAD_LAST;
	m_InvertLogicalHeadsOrder = DEFAULT_INVERT_LOGICAL_HEADS_ORDER;
	m_Yoffset =  DEFAULT_YOFFSET;

	/* Printer */
	m_Presentation = PRESENTATION;
	m_NozzlesPerLogicalHead = NOZZLES_PER_HEAD;
	m_NozzlesPerPartialPhysicalHead = NOZZLES_PER_HEAD;
	m_NumberOfLogicalHeads = NUM_OF_LOGICAL_HEADS;
	m_NumberOfPartialPhysicalHeads = NUM_OF_LOGICAL_HEADS;
	m_PixelResolutionXYZ.push_back(PixelResolutionX);
	m_PixelResolutionXYZ.push_back(PixelResolutionY);
	m_PixelResolutionXYZ.push_back(m_ParamsMgr->LayerHeightDPI_um);
	m_LogicalHeadNozzleResolution = LOGICAL_HEAD_PRINT_Y_DPI; //TODO: check if this number is 150 in triplex. important for  NumberOfTravelsPerPass calculation.
	m_StepsPerTickXYZ.push_back(m_ParamsMgr->XStepsPerPixel);
	m_StepsPerTickXYZ.push_back(m_ParamsMgr->YStepsPerPixel);
	m_StepsPerTickXYZ.push_back(STEPS_PER_TICKS_Z_THIKNESS);
	m_TicksResolutionXYZ.push_back(TicksResolutionX);
	m_TicksResolutionXYZ.push_back(f_round((1199.2 / m_ParamsMgr->YStepsPerPixel),8));
	m_TicksResolutionXYZ.push_back(25.4 / (m_ParamsMgr->LayerHeightDPI_um / 1000));
	//m_TicksResolutionXYZ.push_back(TICKS_RESOLUTION_Z_THIKNESS);
	
	for(int headNum = NUM_OF_LOGICAL_HEADS-1; headNum >= 0; headNum--)
	{
		//m_ActualDistanceBetweenHeadsX.push_back(-QSimpleRound(xOffsetsArray[headNum] + m_ParamsMgr->HeadMapArray[headNum])); //This is according to the machine but looks bad.
		m_ActualDistanceBetweenPartialPhysicalHeadsX.push_back(offsetsArray[headNum][0].Offset_X_1200DPI); //This is without the round, looks better
		m_ActualDistanceBetweenPartialPhysicalHeadsY.push_back(offsetsArray[headNum][0].Offset_Y);
	} 
	initializeMaterialsCatalog();
	m_TrayLengthXYZ.push_back(m_ParamsMgr->TrayXSize);
	m_TrayLengthXYZ.push_back(m_ParamsMgr->TrayYSize);
	m_TrayLengthXYZ.push_back(TRAY_Z_SIZE);
	 
	
}
/* Deletes all files inside a folder */
void StipulatorData::deleteFolder(QString path)
{
   struct dirent *next_file;
   DIR *theFolder;

   CAppParams *m_ParamsMgr = CAppParams::Instance();

   char filepath[256];

   try {
		theFolder = opendir(path.c_str());

		if (theFolder == NULL)
		{
			m_ParamsMgr->DumpPassToFile = 0;
			return;
		}

		while ( (next_file = readdir(theFolder)) != 0 )
		{
			// build the full path for each file in the folder
			sprintf(filepath, "%s/%s", path.c_str(), next_file->d_name);
			remove(filepath);
		}
   } catch (...)
   {
   		m_ParamsMgr->DumpPassToFile = 0;		
   }

}
double StipulatorData::f_round(double dval, int n)
{
	char l_fmtp[32], l_buf[64];
	char *p_str;
	sprintf (l_fmtp, "%%.%df", n);
	if (dval>=0)
			sprintf (l_buf, l_fmtp, dval);
	else
			sprintf (l_buf, l_fmtp, dval);
	return ((double)strtod(l_buf, &p_str));

}
/* Adding slice information */
void StipulatorData::stipulatorAddSliceInformation(int numberOfPasses, int numberOfTravels, int yOffset, int StartY, int trayOffset)
{
	m_SliceNumber++;
	m_FirstTravelInSlice = true;

	//CQLog::Write(1,"Stipulator: sliceNum: %d, numberOfPasses: %d", m_NumberOfTravelsPerPass.size() ,numberOfPasses);
	m_NumberOfTravelsPerPass.push_back(vector<int>());
	m_BidirectionalityPerPass.push_back(vector<int>());
	m_NumberOfPasses.push_back(numberOfPasses);
	
	for(int pass = 0; pass < numberOfPasses; pass++)
	{
		m_NumberOfTravelsPerPass.back().push_back(numberOfTravels);
		m_BidirectionalityPerPass.back().push_back(bidirectionalityPerPassValue);
	}

	m_CurrentY = yOffset + StartY + trayOffset;
	m_PassNumber = 0;
	
}
/* Writes all the data to configuration files. */
void StipulatorData::writeStipulatorData()
{
	QString copyiedPath;
    CAppParams* params = CAppParams::Instance();

    copyiedPath = params->DumpPassToFilePath;
	ForceDirectories(copyiedPath.c_str());

	writeDataToFile(copyiedPath, "GeneralParameters.txt");
	writeDataToFile(copyiedPath, "SlicesInformation.txt");
	writeDataToFile(copyiedPath, "Materials.txt");
	
	m_IsPritingTime = false;
	resetFields();

}

/* Writes the data to a spesific file. */
void StipulatorData::writeDataToFile(QString copyiedPath, QString fileName)
{
	QString configurationFileName;
	QString configurationFileData = "";
	FILE *out = NULL;
	CAppParams *m_ParamsMgr = CAppParams::Instance();

	try
	{
		configurationFileName = copyiedPath + "\\" + fileName;
		out = fopen(configurationFileName.c_str(), "w");
		configurationFileData = toString(fileName);
		fwrite(configurationFileData.c_str(), sizeof(BYTE), sizeof(BYTE) * configurationFileData.length(), out);

		if (out)
			fclose(out);
	}
	catch (...)
	{
		m_ParamsMgr->DumpPassToFile = 0;	
	}


	

}
bool StipulatorData::isFirstTravelInSlice()
{
    return m_FirstTravelInSlice;
}

void StipulatorData::setIsFirstTravelInSlice(bool isFirst)
{
	m_FirstTravelInSlice = isFirst;
}

void StipulatorData::setYOffset(int scatterOffset)
{
                m_Yoffset = scatterOffset;
}


