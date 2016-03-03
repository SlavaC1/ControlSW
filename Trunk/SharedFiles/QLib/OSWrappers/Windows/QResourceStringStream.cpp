/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib utilities.                                          *
 * Module Description: Resource file string stream -                *
 *                     implement a CQTranslatorStringStream class   *
 *                     for windows resource file.                   *
 *                                                                  *
 * Compilation: BCB                                                 *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 02/03/2003                                           *
 * Last upate: 02/03/2003                                           *
 ********************************************************************/

#include <vcl.h>
#pragma hdrstop
 
#include "QResourceStringStream.h"


const int STR_BUFFER_LENGTH = 1024;


// Constructor
CQResourceStringStream::CQResourceStringStream(char **OriginalStringTable,int ResStartID,int NumberOfStrings)
{
  m_OriginalStringTable = OriginalStringTable;
  m_ResStartID = ResStartID;
  m_ResIndex = 0;
  m_NumberOfStrings = NumberOfStrings;

  m_StrBuffer = new char[STR_BUFFER_LENGTH];
}

// Destructor
CQResourceStringStream::~CQResourceStringStream(void)
{
  delete m_StrBuffer;
}

void CQResourceStringStream::Begin(void)
{
  m_ResIndex = 0;
}

// Get next pair of strings (override)
CQTranslatorStringStream::TStrPair CQResourceStringStream::GetNext(void)
{
  CQTranslatorStringStream::TStrPair Result;

  if(m_ResIndex < m_NumberOfStrings)
  {
    // Load from resource and check result
    if(LoadString(HInstance,m_ResStartID + m_ResIndex,m_StrBuffer,STR_BUFFER_LENGTH - 1) != 0)
    {
      // Make sure that the last byte in the buffer is NULL
      m_StrBuffer[STR_BUFFER_LENGTH - 1] = NULL;

      Result.first = QString(m_OriginalStringTable[m_ResIndex]);
      Result.second = QString(m_StrBuffer);

      m_ResIndex++;

    } else
        Result.first = Result.second = "";
  }

  return Result;    
}

