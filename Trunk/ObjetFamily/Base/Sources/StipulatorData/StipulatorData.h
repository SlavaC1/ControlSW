//---------------------------------------------------------------------------

#ifndef _STIPULATOR_DATA_H_
#define _STIPULATOR_DATA_H_
//---------------------------------------------------------------------------

#include <vector>
#include <map>
#include "QUtils.h"
#include "AppParams.h"
#include "QLogFile.h"
#include "Q2RTApplication.h"
#include "LayerProcessDefs.h"
#include <iostream>
#include <dirent.h>

using namespace std;

const int LEFT_TO_RIGHT = 0;
const int RIGHT_TO_LEFT = 1;

const double VERSIONS_NOT_IMPLEMENTED = 0;
const int DEFAULT_GENERAL_VALUE = 0;
const int PRESENTATION = 0; //Cartesiean presentation.
const int OUTPUT_TO_LOG = 1; 
const int SEPARATE_SLICE_TO_TRAVELS = 0;
const int DEFAULT_FULL_TRAY_MODE = 1;
const int DEFAULT_SLICES_RANGE = -1; //end to end.
const int SAVE_3D_MODEL = 0;
const int ONLY_MODEL = 0;
const int MODEL_FORM = 1;
const int DISTANCE_BETWEEN_SLICES = 5;
const int SAMPLING_OMITION_RATE = -1;
const int SAMPLING_INFLATION_RATE = 100;
const int RANGE_RELATED_INFLATION = 1;
const int DEFAULT_COLOR_FOR_RGB_BACKGROUND = 0; //0 for red, green and blue.
const int DEFAULT_BINARY_DATA_STORAGE_FREQUENCY = 0; //0 -per slice. 1-per pass. 2-per travel.
const QString DEFAULT_SLICES_FORMAT = ".tiff";
const QString DEFAULT_OUTPUT_3DMODEL_PATH = "default";

const int KESHET_FLIPPING_FLAG = 1;
const int DEFAULT_DATA_VERSION = 1;
const int DEFAULT_INVERT_BYTES_FIRST = 0;
const int DEFAULT_INVERT_HEAD_LAST = 0;
const int DEFAULT_INVERT_LOGICAL_HEADS_ORDER = 1;
const int DEFAULT_YOFFSET = 192;

const int X_OFFSET_AT_TRAVEL = 50;  //TODO In future: find how to extract the real XPosition without this value. (  CalculateXStartPosition )
const int PixelResolutionX = 600;
const int PixelResolutionY = 300;
const int TicksResolutionX = 1200;
const int STEPS_PER_TICKS_Z_THIKNESS = 1;
const int directionOfTravelPerPassValue = 1;
const int bidirectionalityPerPassValue  = 1;
const int TRAY_Z_SIZE = 200;
const int Y_POSITION_ERROR = 200;
const int Y_POSITION_PASS_JUMP = 3111;


//class CLayerProcess;
struct RGBColor {
	RGBColor() {}
	RGBColor(int r, int g, int b):red(r),green(g),blue(b) {}
	RGBColor(const RGBColor &c):red(c.red), green(c.green), blue(c.blue) {}
	int red;
	int green;
	int blue;
};
           
struct Material{
	Material() {}
	Material(int i, QString n, struct RGBColor c):id(i),name(n),color(c) {}
	Material(const Material &m):id(m.id), name(m.name), color(m.color) {}
	int id;
	QString name;
	struct RGBColor color;
};



class StipulatorData
{
	public:
		StipulatorData();
		~StipulatorData(){};

		/* Adds a new travel direction */
		void addTravelDirection(const int travelNumber);

		/* Adds number of fires for a certain travel */
		void addNumberOfFires(const int numberOfFires, const int traelNumber);

		/* Adds y position for a certain travel */
		void addYPositionAtTravel(const long yPositionAtTravel, const int travelNumber);

		/* Adds x start position for a certain travel */
		void addXPositionStartAtTravel(const long xPositionStartAtTravel, const int travelNumber);

		/* Adds x end position for a certain travel */
		void addXPositionEndAtTravel(const long xPositionEndAtTravel, const int travelNumber);

		/* Updates pass parameters for, mainly for calculating y positions of the travels */
		int  passParams(const int travelNumber, float* move_size, long* ticksCurY);

		/* Create output folder if not exist, deletes all files in it and resets all fields */
		void resetStipulatorData(THeadMapEntry offsetsArray[NUM_OF_LOGICAL_HEADS][NOZZLES_PER_HEAD]);

		/* toString for either GeneralParameters.txt and SlicesInformation.txt */
		QString toString(string fileName);

		/* Filling fields values for current slice */
		void stipulatorAddSliceInformation(int numberOfPasses, int numberOfTravels, int yOffset, int StartY, int trayOffset);

		/* Writes all data to files */
		void writeStipulatorData();

		bool isFirstTravelInSlice();
		void setIsFirstTravelInSlice(bool isFirst);
		void setYOffset(int scatterOffset);

	private:

		map<QString, int> m_MaterialsCatalog;
		map<QString, struct RGBColor> m_MaterialsColors;

		/* System */
		double 		 			m_SoftwareRevision; 
		double 		 			m_SvnRevision;
		bool					m_OutputToLog;
		bool					m_SeparateSliceToTravels;
		bool					m_FullTray;
		vector<int>				m_PrintSlicesRange;
		int						m_BinaryDataStorageFrequency;
		vector<int>				m_BackgroundColor;
		QString					m_slicesFormat;
		QString					m_outputPath;

		/* 3D Model */
		bool					m_Save3DModel;
		bool					m_OnlyModel;
		bool					m_ModelForm;
		int 					m_DistanceBetweenSlices;
		int						m_SamplingOmitionRate;
		int						m_SamplingInflationRate;
		bool					m_RangeRelatedInflation;
		
		/* Data Order */
		bool					m_DataVersion;
		bool					m_InvertBytesFirst;
		bool					m_InvertHeadLast;
		bool					m_InvertLogicalHeadsOrder;
		int						m_Yoffset;

		/* Printer */
		int 		 			m_Presentation;
		int 		 			m_NozzlesPerLogicalHead;
		int 		 			m_NozzlesPerPartialPhysicalHead;
		int						m_NumberOfLogicalHeads;
		int						m_NumberOfPartialPhysicalHeads;
		vector<double>  		m_PixelResolutionXYZ;
		double					m_LogicalHeadNozzleResolution;
		vector<double>  		m_StepsPerTickXYZ;
		vector<double>  		m_TicksResolutionXYZ;
		vector<double>  		m_ActualDistanceBetweenPartialPhysicalHeadsX;
		vector<double>  		m_ActualDistanceBetweenPartialPhysicalHeadsY;
		vector<Material>  		m_LogicalHeadMaterials;
		vector<Material>  		m_MaterialsTypes;
		vector<QString>			m_MaterialsNames;
		vector<double>  		m_TrayLengthXYZ;

		/* Slices Information */
		vector<double>      	m_NumberOfPasses;
		vector<vector<int> > 	m_NumberOfTravelsPerPass;
		vector<vector<int> >   	m_BidirectionalityPerPass;
		vector<vector<int> >   	m_TravelDirection; /* Changed to per travel instead of per pass!!*/
		vector<vector<int> >	m_NumberOfFiresPerTravel;
		vector<vector<long> >   m_YPositionAtTravel;
		vector<vector<double> > m_XPositionStartAtTravel;
		vector<vector<double> > m_XPositionEndAtTravel;

		/* Temps */
		bool 					m_IsPritingTime;
		int 					m_PassNumber;
		int						m_SliceNumber;
		bool					m_FirstTravelInSlice;
		long 					m_CurrentY;

		/* Initializes materials Map<MaterialName, MaterialIndex> and Map<MaterialName, MaterialColor> */
		void initializeMaterialsMaps();

		/* Initializes materials catalog */
		void initializeMaterialsCatalog();

		/* Finds Material in catalog by its name */
		vector<Material>::iterator findMaterialInCatalog(QString material);

		/* Fills general stipulator(Romi) fields */
		void initializingGeneralStipulatorFields(THeadMapEntry offsetsArray[NUM_OF_LOGICAL_HEADS][NOZZLES_PER_HEAD]);

		/* Deletes all files in a folder */
		void deleteFolder(QString path);

		/* Resets all fields*/
		void resetFields();

		/* Rounds a floating point number with n digits after the point.*/
		double f_round(double dval, int n);

		/* Writes data to file */
		void writeDataToFile(QString copyiedPath, QString fileName);

		/* toString for GeneralParameters.txt */
		QString toStringGeneral();

		/* toString for SlicesInformation.txt */
		QString toStringSlices();

		/* toString for Materials.txt */
		QString toStringMaterials();
};


#endif

