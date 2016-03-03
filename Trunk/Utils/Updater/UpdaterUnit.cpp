//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

//---------------------------------------------------------------------------

#pragma argsused

#include "QStringList.h"
#include "QTypes.h"
#include "QFileParamsStream.h"
#include "QFileWithChecksumParamsStream.h"

enum
{
    PROGRAMM = 0
   ,FROMFILE
   ,TOFILE
   ,DEFSFILE
   ,MIN_ARGC
   ,IS_CHECKSUM = MIN_ARGC
   ,ARGC
};

const QString NO_CHANGE_SIGN("***");

int main(int argc, char* argv[])
{
   CQParamsFileStream* ToFile = NULL;
   try
   {
     try
     {
        if (argc < MIN_ARGC || argc > ARGC)
           throw EQException("Invalid number of arguments\nUSAGE: Updater _FromFile_ _ToFile_ _DefsFile_ [_IsCheckSum_='Y'/'N']");

        bool InitOK = true;
        CQParamsFileStream FromFile(argv[FROMFILE],InitOK);
        if (!InitOK)
           throw EQException(QFormatStr("Can not open file: %s", argv[FROMFILE]));
        CQParamsFileStream DefsFile(argv[DEFSFILE],InitOK);
        if (!InitOK)
           throw EQException(QFormatStr("Can not open file: %s", argv[DEFSFILE]));

        if (((argc > IS_CHECKSUM) && (argv[IS_CHECKSUM] == QString("Y"))))
        {
          TInitStatus_E Status = INIT_OK_E;
          ToFile = new CQParamsFileWithChecksumStream(argv[TOFILE],Status,true,false);
          if (Status != INIT_OK_E)
             throw EQException(QFormatStr("Can not open file: %s", argv[TOFILE]));
        }
        else
        {
          ToFile = new CQParamsFileStream(argv[TOFILE],InitOK);
          if (!InitOK)
             throw EQException(QFormatStr("Can not open file: %s", argv[DEFSFILE]));
        }

        TQStringMap DefsFileMap = DefsFile.GetKeyToValueMap();
        QString FromParamName,ToParamName,ToNewValue;
        unsigned line=1;
        for (TQStringMap::iterator Iter = DefsFileMap.begin(); Iter != DefsFileMap.end(); Iter++,line++)
        {
           ToParamName = Iter->first;
           FromParamName = Iter->second;

           TQStringVector FromParamTokens;
           Tokenize(FromParamName, FromParamTokens);
           TQStringVector FromParamValueTokens(FromParamTokens.size());
           for(unsigned i = 0; i < FromParamTokens.size(); i++)
           {
              if ((FromParamTokens[i].size() >= NO_CHANGE_SIGN.size()) &&  (FromParamTokens[i].substr(FromParamTokens[i].length() - NO_CHANGE_SIGN.size(), FromParamTokens[i].size()) == NO_CHANGE_SIGN))
                 FromParamValueTokens[i] = FromParamTokens[i].substr(0,FromParamTokens[i].length() - NO_CHANGE_SIGN.size());
              else   
                 FromParamValueTokens[i] = FromFile.ReadRawParam(FromParamTokens[i]);
              if (FromParamValueTokens[i] == "" && FromParamTokens[i] != NO_CHANGE_SIGN)
                 throw EQException(QFormatStr("Can not find '%s'(line %d in %s) in file %s", FromParamTokens[i].c_str(), line, argv[DEFSFILE], argv[FROMFILE]));
           }
           JoinTokens(ToNewValue,FromParamValueTokens);
           ToFile->SetValueToParameter(ToParamName,ToNewValue,paDefault);
        }
        ToFile->Save();
     }
     catch(EQException& Err)
     {
        ShowMessage(Err.GetErrorMsg().c_str());
     }
     catch(...)
     {
        ShowMessage("Unexpected error");
     }
   }
   __finally
   {
      Q_SAFE_DELETE(ToFile);
   }
   return 0;
}
//---------------------------------------------------------------------------

