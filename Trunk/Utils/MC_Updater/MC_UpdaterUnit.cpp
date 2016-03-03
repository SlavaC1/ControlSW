
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

//---------------------------------------------------------------------------

#pragma argsused

#include "QStringList.h"
#include "QTypes.h"
#include "QFileParamsStream.h"
#include "QFileWithChecksumParamsStream.h"
#include "MaintenanceCountersFile.h"
#include "MaintenanceCountersFileDefs.h"
#include "QErrors.h"

enum
{
    PROGRAMM = 0
   ,FROMFILE
   ,TOFILE
   ,DEFSFILE
   ,MIN_ARGC
   ,ARGC
};


int main(int argc, char* argv[])
{
    CMaintenanceCountersFile FromFile;
    CMaintenanceCountersFile ToFile;
    bool InitOK;
    QString ToNewValue,FromParamValue;
    double elapsedSeconds=0;
    int parIndexTo, parIndexFrom;
    time_t resetDate;
    QString mc;
    
  map<QString, int> StrToEnumEden;
  map<QString, int> StrToEnumConnex;

// Create the Eden Name to const int map
#define MC_UPDATER
#define DEFINE_MC(name, id) StrToEnumEden[#name]= id; 
#include "..\\..\\EdenFamily\\Base\\Sources\\MaintenanceCounters\\MaintenanceCountersIDs.h"
#undef DEFINE_MC
#undef MC_UPDATER

// Create the Connex Name to Enum map
#include "..\\..\\ConnexFamily\\Base\\Sources\\MaintenanceCounters\\MaintenanceCountersDefs.h"
#define DEFINE_MAINTENANCE_COUNTER(Name,_str_,_groupStr_,_warningTime_,_needToInformUser)  StrToEnumConnex[#Name + QString("_COUNTER_ID")] = Name##_COUNTER_ID;
#define DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(Name,_str_,_groupStr_,_warningTime_,_needToInformUser_,_displayName_) StrToEnumConnex[#Name + QString("_COUNTER_ID")] = Name##_COUNTER_ID;
#include "..\\..\\ConnexFamily\\Base\\Sources\\MaintenanceCounters\\MaintenanceCountersDefsList.h"
#undef DEFINE_MAINTENANCE_COUNTER
#undef DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME

     try
     {

        InitOK = true;
        if (argc < MIN_ARGC || argc > ARGC - 1)
           throw EQException("Invalid number of arguments\nUSAGE: MC_Updater FromFile ToFile DefsFile");

        //maint.Counters files location for the MC_Updater:
     	FromFile.AssignFile(argv[FROMFILE]);
      	ToFile.AssignFile(argv[TOFILE]);
        CQParamsFileStream DefsFile(argv[DEFSFILE],InitOK); //list of counters that transfer

        if (!InitOK)
           throw EQException(QFormatStr("Can not open file: %s", argv[DEFSFILE]));

           

        TQStringMap DefsFileMap = DefsFile.GetKeyToValueMap();
        TQStringMap::iterator IterFirst = DefsFileMap.begin();
        ToFile.SetNumOfCounters(QStrToInt(IterFirst->first));
        FromFile.SetNumOfCounters(QStrToInt(IterFirst->second));//in the begining of defs file located number of counters that are for each machine
        IterFirst++;
        unsigned line=1;
        FromFile.LoadFromFile();
        ToFile.LoadFromFile();
        /*     testing stage.. -OK writes and reads from Maintanence counters file.
        FromFile.SetCountersFileName(argv[FROMFILE]);
        FromFile.LoadFromFile();
        ToFile.LoadFromFile();
        elapsedSeconds = ToFile.GetElapsedSeconds(0);
        ToFile.SetElapsedSeconds(0,9);
        ToFile.SetElapsedSeconds(1,19);
        ToFile.SetElapsedSeconds(2,29);
        ToFile.SaveToFile();
        ToFile.LoadFromFile();
        elapsedSeconds = ToFile.GetElapsedSeconds(DIRT_PUMP_COUNTER_ID);
        elapsedSeconds = ToFile.GetElapsedSeconds(1);
         */

        /*for testing only
        for (int i=0; i<FromFile.GetNumfCounters() ;i++)
        {
           FromFile.SetElapsedSeconds(i,i);
        }
        FromFile.SaveToFile();
        */
        

        bool counterDcr=false;
        for (TQStringMap::iterator Iter = IterFirst; Iter != DefsFileMap.end(); Iter++,line++)
        {
          mc = Iter-> first ;
          parIndexTo     = StrToEnumConnex[mc];

          mc = Iter-> second ;
    	  parIndexFrom   = StrToEnumEden[mc];

          if (parIndexTo == 0 )
             counterDcr=true;
             
          //basic checking of parameter - if the parameter exist - not to nil it by mistake with parameter that not exists.
          if ((parIndexTo != 0 && parIndexFrom != 0 ) || (parIndexTo == 0 && parIndexFrom == 0 ))
          {
          	elapsedSeconds = FromFile.GetElapsedSeconds(parIndexFrom);
          	resetDate      = FromFile.GetResetDate(parIndexFrom);
          	ToFile.SetElapsedSeconds(parIndexTo,elapsedSeconds);
          	ToFile.SetResetDate(parIndexTo,resetDate);
          }
          else //ShowMessage("problem in defs file parameter not found in machine" );
          {
             ;

          }
        }

        //save the transffered parameters:
        ToFile.SaveToFile();

     }
     catch(EQException& Err)
     {
        ShowMessage(Err.GetErrorMsg().c_str());
     }
     catch(...)
     {
        ShowMessage("Unexpected error");
     }


      

	return 0;
}
//---------------------------------------------------------------------------
 