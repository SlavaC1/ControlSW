/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib parameters stream for disk files.                   *
 * Module Description: Implement the base class CQParamsStream for  *
 *                     text file with checksum.                     *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 23/07/2003                                           *
 * Last upate: 23/07/2003                                           *
 ********************************************************************/

#include "QFileWithChecksumParamsStream.h"
#include "QUtils.h"
#include "QErrors.h"
#include "QMonitor.h"
#include "QParamsContainer.h"
#include "QFile.h"
#include "QScopeRestorePoint.h"


// Maximum number of characters in a configuration file
const int MAX_LINE_WIDTH = 500;


// Convert string to checksum value (sum all ASCII values of the characters in the string)
static unsigned StrToChecksum(const QString Str)
{
  unsigned Checksum = 0;

  for(unsigned i = 0; i < Str.length(); i++)
    Checksum += unsigned(Str[i]);

  return Checksum;
}

static bool IsAllNumbersStr(const QString Str)
{
  for(unsigned i = 0; i < Str.length(); i++)
    if(!isdigit(Str[i]))
      return false;

  return true;
}

// Constructor
CQParamsFileWithChecksumStream::CQParamsFileWithChecksumStream(const QString FileName, TInitStatus_E &StatusInit) :
CQParamsFileStream(FileName)
{
	Init(FileName,StatusInit,false/*GenerateNewCheckSumFile*/,true/*Silent*/);
}
//Triplex
CQParamsFileWithChecksumStream::CQParamsFileWithChecksumStream(const QString FileName):
CQParamsFileStream(FileName)
{}

// Constructor for genarating new checksum file from corrupted one.
//Silent - means if to promt user(in Export import) : to generate new checksum or do it in a quiet mode.
CQParamsFileWithChecksumStream::CQParamsFileWithChecksumStream(const QString FileName, TInitStatus_E &StatusInit,bool GenerateNewCheckSumFile, bool Silent) :
CQParamsFileStream(FileName),
m_GenerateNewCheckSumFile(false)
{
	Init(FileName,StatusInit,GenerateNewCheckSumFile,Silent);
}

void CQParamsFileWithChecksumStream::Init(const QString FileName, TInitStatus_E &StatusInit,bool GenerateNewCheckSumFile, bool Silent)
{
	CScopeRestorePoint<bool> scKeepGenerateNewCheckSumFile(m_GenerateNewCheckSumFile);
	m_GenerateNewCheckSumFile = GenerateNewCheckSumFile;
    m_BackupFileName = FileName + ".bak";
    StatusInit 		 = INIT_OK_E;

    if (FileName == "")
      return;
    TQStringMap BackUpKeyToValue;

    // Load and verify file integrity
	if(!LoadFileIntoStringMap(m_FileName,m_KeyToValue))
    {
       // Load and verify backup file integrity
       if(!LoadFileIntoStringMap(m_BackupFileName,BackUpKeyToValue))
       {
	  // If the files are empty/missing, try to create new files
	  if((m_KeyToValue.size() == 0) && (BackUpKeyToValue.size() == 0))
	  {
	     StatusInit = BOTH_FILES_ARE_MISSING_E;
          }
	  else
	  {
	      StatusInit = BOTH_FILES_ARE_CORRUPTED_E;
	      // The backup file is also invalid, ask the user what to do
              //Silent - means if to promt user(in Export import) for example : to generate new checksum or do it in a quiet mode.
	      //so silent is true-  not to enter the AskOKCancel screen.
              if ( ! Silent && m_KeyToValue.size()>0)
              {
		 if (!QMonitor.AskOKCancel("Press 'OK' to generate new checksum with this current file.\n\rPress 'Cancel' to abort the program; " ))
		   throw EQException("Bye Bye ...");
                 else
                   StatusInit = INIT_OK_E; // fix 5955 E.E
              }
          }
       }
       else
       { //main file is corrupted but the bak file is fine.
	  QMonitor.WarningMessageWaitOk("The validation(checksum) of the parameters file " +m_FileName+ " failed.\n\r.Backup file was loaded");
          m_KeyToValue = BackUpKeyToValue;
	  StatusInit = MAIN_FILE_CORRUPT_E;
       }
    }
  if((StatusInit != BOTH_FILES_ARE_MISSING_E)&&(StatusInit != BOTH_FILES_ARE_CORRUPTED_E))
     Save();
  return;
}

// Destructor
CQParamsFileWithChecksumStream::~CQParamsFileWithChecksumStream(void)
{}

CQStringList CQParamsFileWithChecksumStream::LogParamsToStringList(CQParamsContainer *AttachedContainer)
{
  int Sum = 0;
  CQStringList ModeFile = CQParamsFileStream::LogParamsToStringList(AttachedContainer);
  for (unsigned int i = 0; i < ModeFile.Count(); ++i)
	  Sum += StrToChecksum(ModeFile[i]); // Add line to checksum
  if (Sum > 0)
	 ModeFile.Add(QIntToStr(Sum));
  return ModeFile;
}
//Save parameters to file with checksum, doesn't check parameter integrity
bool CQParamsFileWithChecksumStream::SaveFileWithChecksum()
{
  int Sum = 0;
  bool chksumOK = false;
  CQStringList ModeFile;
  
  if(!CQParamsFileStream::LoadFileIntoStringList(m_FileName,ModeFile))
	return false;

  for (unsigned int i = 0; i < ModeFile.Count()-1; ++i)
	  Sum += StrToChecksum(ModeFile[i]); // Add line to checksum
  if(Sum > 0)
  {
	if(IsAllNumbersStr(ModeFile[ModeFile.Count()-1])) //replace the old checksum
	{
	  ModeFile.SetItem((ModeFile.Count()-1),QIntToStr(Sum));
	}
	else
	  ModeFile.Add(QIntToStr(Sum)); //no checksum indicated, add checksum to file
	ModeFile.SaveToFile(m_FileName);
	ModeFile.SaveToFile(m_BackupFileName);
	chksumOK = true;
  }
  else
	chksumOK = false;

  return chksumOK;
}


// End the write to the file
void CQParamsFileWithChecksumStream::Save(void)
{
  CQParamsFileStream::Save();
  SaveFile(m_BackupFileName);
}

// Write a single parameter
void CQParamsFileWithChecksumStream::WriteParameter(CQParameterBase *Param)
{
  // If the parameter has the "no save" attribute, do nothing
  if(Param->GetAttributes() & paNoSave)
    return;

  m_KeyToValue[Param->Name()] = Param->OriginalValueAsString();
}

void CQParamsFileWithChecksumStream::WriteOnlyParametersFromStream(CQParameterBase *Param, CQParamsStream *ImportStream)
{
  // If the parameter has the "no save" attribute, do nothing
  if(Param->GetAttributes() & paNoSave)
    return;

  if (( m_KeyToValue.find( Param->Name() ) != m_KeyToValue.end() ) ||
      ImportStream->IsParamInStream(Param) )
    m_KeyToValue[Param->Name()] = Param->OriginalValueAsString();
}

/*
Loads the file then calcs & checks for expected checksum
Return : True - file loaded and checksum is valid
othrwise returns False in next cases: 
	1. file not exist
	2. file corrupted
	3. checksum not exist in file
	4. checksum not valid
*/
bool CQParamsFileWithChecksumStream::LoadFileIntoStringList(QString FileName,CQStringList& StrList)
{
  if (CQParamsFileStream::LoadFileIntoStringList(FileName,StrList))
  {
     bool ret =  false; //m_GenerateNewCheckSumFile;
     if(StrList.Count() > 1) // A file with checksum must have at least 2 lines
     {
	 unsigned Sum = 0;
	 for (unsigned i = 0; i < StrList.Count() - 1; i++) // Sum all lines until the last one
	 	Sum += StrToChecksum(StrList[i]);

	 // Check last line format- is it a checksum (all numbers) format?
		 if (IsAllNumbersStr(StrList[StrList.Count() - 1])) //fix 5955  E.E
         {
		  try // Convert last line to numerical value and compare
		  {
			unsigned ChecksumInFile = QStrToInt(StrList[StrList.Count() - 1]);
				ret = (ChecksumInFile == Sum); // fix 5955 E.E
		  }
			  catch(...)
		  {
				// ret = m_GenerateNewCheckSumFile;
		  }
        }
		else
			ret = m_GenerateNewCheckSumFile;
    }
    return ret;
  }
  return false;
}

