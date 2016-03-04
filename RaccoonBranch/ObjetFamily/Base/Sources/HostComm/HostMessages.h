#ifndef _HOST_MESSAGES_H_
#define _HOST_MESSAGES_H_

#include "Q2RTErrors.h"
//===========================================================================
//              Data types for INCOMING messages FROM Host :

// Align all structures to byte boundry
#ifdef OS_WINDOWS

// The following pragma is for BCB only
#ifndef __BCPLUSPLUS__
#error Q2RT: This module contain pragma statements for Borland C++ Builder
#endif

#pragma pack(push,1)

#endif

#ifdef OS_VXWORKS
#define STRUCT_ATTRIBUTE __attribute__ ((packed))
#elif defined(OS_WINDOWS)
#define STRUCT_ATTRIBUTE
#endif

#include "GlobalDefs.h"
#define NUMBER_OF_ADDITIONAL_RESINS (NUMBER_OF_CHAMBERS - 2)/*1 Model + 1 Support*/

const int MAX_RESIN_NAME_LENGTH = 32;
const int MAX_MODEL_NAME_LENGTH = 128;

const int NUM_OF_POSSIBLE_MATERIALS = 100;
const int NUM_OF_POSSIBLE_PACKAGES  = 10;
const int MAX_STRING_SIZE           = 1000;
//--------------------------------------
// Types for Slice Packages.

typedef struct BITMAPRECTtag
{
  WORD x;
  WORD y;
  WORD width;
  WORD height;
}   TBmpRect STRUCT_ATTRIBUTE;               // Nested inside the TBmp structure.

typedef struct BMPtag
{
  TBmpRect rect;          // body rect, interface
  UINT     len;
  BYTE     bmp[1];
}   TBmp STRUCT_ATTRIBUTE;                   // Nested inside the TSlicePackageType structure.

typedef struct SlicePackHdrtag
{
  UINT  headerSize;    // (in bytes);
  UINT  sliceNumber;   // Slice # in the model
  float sliceHeight;   // in mm;
  WORD  bitmapRes;     // in DPI;
  BYTE  NofBitmaps;    // 1,2,…
  BYTE  compressType;  // (0,1,…);
}   TSlicePackageHeader STRUCT_ATTRIBUTE;    // Nested inside the TSlicePackageType structure.

typedef struct SlicePackagetag
{
  TSlicePackageHeader  header;
  TBmp                 model[1];       // malloc(… dataSize);
  TBmp                 support[1];     // comment in N-Plex change
#if defined CONNEX_MACHINE || defined OBJET_MACHINE
  short                NumberOfAdditionalModelResins;
  short                NumberOfAdditionalSupportResins;
  TBmp                 AdditionalBitmaps[NUMBER_OF_ADDITIONAL_RESINS]; // comment N-Plex change
#endif
} TSlicePackageType STRUCT_ATTRIBUTE;


//--------------------------------------
// Type for New Model data.

typedef char TResinName [MAX_RESIN_NAME_LENGTH]; 
typedef char TModelName [MAX_MODEL_NAME_LENGTH];
typedef char TPackageName [MAX_RESIN_NAME_LENGTH];

typedef struct ResinReplacementRequesttag
{
    BYTE        MaintenanceMode;                 /*0- Current;1:HQ-2R;2:HS-2R;1:HQ-3R;2:HS-3R*/
    BYTE        IsMix;                           /*0 - False;  1 - True*/
    short       NumberOfModels;
    TResinName  Materials[MAX_NUMBER_OF_MODEL_CHAMBERS]; /*1-MAX_NUMBER_OF_MODELS*/
} ResinReplacementRequestType STRUCT_ATTRIBUTE;


typedef struct FlexibleParameter
{
   short ParamID; //the parameter ID
   short paramType; //int, float, string...
   int size;//1..*
   void * val;//the parameter's value 
} FlexibleParameter STRUCT_ATTRIBUTE;

typedef struct FlexibleParametersTag
{
    int NumofFlexibleParams; //num of flexible parameters (currently static in our design)
	FlexibleParameter FlexibleParamsArr[NUM_OF_FLEXIBLE_PARAMETERS];
} FlexibleParametersType STRUCT_ATTRIBUTE;


typedef struct StartModeltag
{
    TModelName  ModelName;
    float       TotalHeight;    // Height of Model - mm
    WORD        TotalSlices;    // In Model.
    WORD        Resolution;     // of Image        - DPI.
    BYTE        BitsPerPixel;    // //N-Plex change
    short       XMul;           // Size of Multiple grid of Model to print.
    short       YMul;           //
    float       XGap;           // Distance between multiple copies - mm
    float       YGap;           //
    ULONG       JobId;
    BYTE        PrintMode;      // HS/HQ Print Mode. (This field obsoletes the 'Resolution' field.)
#if defined CONNEX_MACHINE || defined OBJET_MACHINE
    short       NumberOfAdditionalModelResins;
    short       NumberOfAdditionalSupportResins;
    TResinName  AdditionalMaterials[NUMBER_OF_ADDITIONAL_RESINS]; 
#endif
}   StartModelType STRUCT_ATTRIBUTE;

//===========================================================================
//              Data types for OUTGOING messages TO Host :

//--------------------------------------
// Type for Model Status report.

typedef union STATUStag
{
    struct  // Status bit fields.
    {
        int running : 1;
        int paused  : 1;
        int loaded  : 1;
        int warming : 1;
        int ready   : 1;
        int spare   : 2;
        int fault   : 1;
    } st;
    BYTE status;
} TStatusType STRUCT_ATTRIBUTE;

//--------------------------------------
// Type for Model Progress report.

typedef struct PROGRESStag
{
    float   height; //mm
    WORD    layers;
    WORD    slice;
}   TProgressType STRUCT_ATTRIBUTE;

typedef BYTE TPrintStopReason;

#define DEFINE_ERROR(_err_, _desc_) const BYTE STOP_##_err_ = _err_##_ERROR_ID;
#include "Q2RTErrorsDefsList.h"
#define ERR_ENUM_TO_HOST_ERROR(_err_)  (_err_)
#undef DEFINE_ERROR

typedef struct
{
  long             TimeStamp;              // Seconds since 1970
  TPrintStopReason StopReason;             // Details Below
  WORD             LastPrintedSlice;
  ULONG            TotalNumberOfPasses;
  ULONG            ActualNumberOfPasses;
} TPrinterStoppedMsg;

// StopReasons:
//-------------
// 0 - Unused,
// 1 - Response to host stop,
// 2 - Response to embedded stop,
// 3 - EOL without user response,
// 4 - UV lamps error,
// 5 - Bumper detection
// 6 - Vacuum error,
// 7 - Cabin temperature error,
// 8 - Heads filling error
// 9 -
// 10- Communication error with host

typedef struct {
  long  TimeStamp;   // Seconds since 1970
  WORD  LastPrintedSlice;
  ULONG TotalNumberOfPasses;
  ULONG ActualNumberOfPasses;
} TPrinterEndOfModelMsg;

struct TContainerInfo
{
    BYTE  ContainerExists;
    ULONG ContainerWeight;   // In Grams
};

typedef struct
{
  long           TimeStamp;
  BYTE           PrinterMode;             // See related definitions
  BYTE           PrinterSubMode;          // See related definitions
  BYTE           TrayExistence;           // 0 - Tray is out, 1 - Tray is in
  BYTE           ModelOnTray;             // 0 - No model is on tray, 1 - Model is on tray
  BYTE           DoorStatus;              // DOOR_OPEN_UNLOCKED or DOOR_CLOSED_UNLOCKED or DOOR_CLOSE_LOCKED or DOOR_OPEN_LOCKED
  BYTE           UVLampsStatus;           // BOTH_LAMPS_OFF or LEFT_LAMP_ON or RIGHT_LAMP_ON or BOTH_LAMPS_ON
  ULONG          CurrentJobID;
  ULONG          NumberOfJobSlices;
  float          CurrentModelHeight;     // In Milimeters
  ULONG          CurrentPrintedSlice;
  ULONG          CurrentLayer;
  TContainerInfo ModelContainer;
  TContainerInfo SupportContainer;
  TContainerInfo WasteContainer;
#if defined CONNEX_MACHINE || defined OBJET_MACHINE
  short          NumberOfAdditionalModelResins;
  short          NumberOfAdditionalSupportResins;
  TContainerInfo AdditionalContainersInfo[NUMBER_OF_ADDITIONAL_RESINS];
#endif
} TPrinterStatusMsg;

typedef struct
{
  long  TimeStamp;
  ULONG TotalNumberOfPasses;
  ULONG ActualNumberOfPasses;
} TPrinterBuildStatusMsg;

// Related definitions

// Printer mode
const BYTE PRINTER_MODE_IDLE      = 0;
const BYTE PRINTER_MODE_PRE_PRINT = 1;
const BYTE PRINTER_MODE_PRINTING  = 2;
const BYTE PRINTER_MODE_STOPPING  = 3;
const BYTE PRINTER_MODE_STOPPED   = 4;
const BYTE PRINTER_MODE_PAUSING   = 5;
const BYTE PRINTER_MODE_PAUSED    = 6;

// Printer mode
const BYTE PRINTER_SUB_MODE_NORMAL   = 0;
const BYTE PRINTER_SUB_MODE_STANDBY1 = 1;
const BYTE PRINTER_SUB_MODE_STANDBY2 = 2;

// Version string type
typedef char TVersionStr[32];

typedef struct
{
  long        TimeStamp;
  TVersionStr HostPrinterProtocol;
  BYTE        PrinterType;  //N-Plex change (objet == 10)
  TVersionStr RPJetSoftware;
  TVersionStr RPJetHardware;
  TVersionStr OCBSoftware;
  TVersionStr OCBHardware;
  TVersionStr OHDBSoftware;
  TVersionStr OHDBHardware;
  TVersionStr MCBSoftware;
  TVersionStr MCBHardware;
  long        HaspExpirationTimeStamp;

} TPrinterVersionsMsg;

typedef struct
{
  long       TimeStamp;
  ULONG      ScatterFactor;
  ULONG      LongPurgePeriod;             // Interval in seconds
  ULONG      ShortPurgePeriod;            // Interval in seconds
  ULONG      ExtraPurgePeriod;            // Interval in seconds
  ULONG      WipePeriod;                  // Interval in seconds
  ULONG      FireAllPeriod;               // Interval in seconds
  float      AverageLayer600DPI;          // In Micrometer
  ULONG      ModelXResolution;            // In DPI
  ULONG      ModelYResolution;            // In DPI
  BYTE       PassMaskUsed;                // 0 - not used, 1 - used
  BYTE       YScanMode;                   // Y_SCAN_MODE_BACKWARD or Y_SCAN_MODE_FORWARD or Y_SCAN_MODE_ALTERNATE
  ULONG      YDeltaReturn;                // In pixels
  float      AverageLayer1200DPI;         // In Micrometer
  ULONG      MaxScatter;
  ULONG      TrayRearOffset;              // Tray dimensions in mm
  ULONG      TrayFrontOffset;
  ULONG      TrayYSize;
  TResinName ModelMaterial;               // Model Material Type
  TResinName SupportMaterial;             // Support Material Type
  float      XVelocity;
  float      YStepsPerPixel;
  short      NumberOfAdditionalModelResins;
  short      NumberOfAdditionalSupportResins;
  TResinName AdditionalMaterials [NUMBER_OF_ADDITIONAL_RESINS];
} TPrinterDataMsg;

typedef struct ResinPrinterDatatag
{
   BYTE        PrintMode;
   TResinName  ModelMaterial;
   TResinName  SupportMaterial;
   short       NumberOfAdditionalModelResins;
   short       NumberOfAdditionalSupportResins;
   TResinName  AdditionalMaterials[NUMBER_OF_ADDITIONAL_RESINS];
}  ResinPrinterDataType STRUCT_ATTRIBUTE;

typedef struct
{
  long       TimeStamp;
  ULONG      ScatterFactor;
  ULONG      LongPurgePeriod;             // Interval in seconds
  ULONG      ShortPurgePeriod;            // Interval in seconds
  ULONG      ExtraPurgePeriod;            // Interval in seconds
  ULONG      WipePeriod;                  // Interval in seconds
  ULONG      FireAllPeriod;               // Interval in seconds
  float      AverageLayer600DPI;          // In Micrometer
  ULONG      ModelXResolution;            // In DPI
  ULONG      ModelYResolution;            // In DPI
  BYTE       PassMaskUsed;                // 0 - not used, 1 - used
  BYTE       YScanMode;                   // Y_SCAN_MODE_BACKWARD or Y_SCAN_MODE_FORWARD or Y_SCAN_MODE_ALTERNATE
  ULONG      YDeltaReturn;                // In pixels
  float      AverageLayer1200DPI;         // In Micrometer
  ULONG      MaxScatter;
  ULONG      TrayRearOffset;              // Tray dimensions in mm
  ULONG      TrayFrontOffset;
  ULONG      TrayYSize;
  float      XVelocity;
  float      YStepsPerPixel;
  int        AtLeastDelayTimeBetweenLayers;
//for the new time estimation algorithm  
  float      AccelerationX;
  float      AccelerationY;
  float      YVelocity;
  float      YSecondaryInterlaceNoOfPixels;
  //float      YStartsUponXDec;
} TResinPrinterDataMsg;

typedef struct
{
	long TimeStamp;
	BYTE OperationModes;
	int  NumOfPackages;
	TPackageName Packages[NUM_OF_POSSIBLE_PACKAGES];
	int  PackageIDs[NUM_OF_POSSIBLE_PACKAGES];
	int  NumOfMaterials;
	TResinName Materials[NUM_OF_POSSIBLE_MATERIALS];
	int  NumOfDigitalMaterials;
	TResinName DigitalMaterials[NUM_OF_POSSIBLE_MATERIALS];
	BYTE TrayProperties;
	int  AdditionalHostParamsSize;
	char AdditionalHostParams[MAX_STRING_SIZE];
}TLicensedInfoMsg;

// Restore previous align settings
#ifdef OS_WINDOWS
#pragma pack(pop)
#endif

#endif
