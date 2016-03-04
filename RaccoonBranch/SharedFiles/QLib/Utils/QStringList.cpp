/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib utilities.                                          *
 * Module Description: A string list class.                         *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 09/08/2001                                           *
 * Last upate: 18/08/2003                                           *
 ********************************************************************/

#include <algorithm>
#include <stdio.h>
#include <cstring>
#include "QStringList.h"
#include "QFile.h"


// Sizeof of the buffer for single line read
const int MAX_LINE_SIZE = 500;


// Add a new string
void CQStringList::Add(const QString Str)
{
  m_Strings.push_back(Str);
}

void CQStringList::RemoveByString(const QString Str)
{
	int Index;
	Index = Find(Str);

	if (Index != -1)
		m_Strings.erase(m_Strings.begin() + Index);
}

void CQStringList::Erase(int index)
{
	if (index != -1)
		m_Strings.erase(m_Strings.begin() + index);
}

// Sort the strings
void CQStringList::Sort(void)
{
  sort(m_Strings.begin(),m_Strings.end());
}

// Explicit get/set item
QString CQStringList::GetItem(int Index) const
{
  if((Index < 0) || (Index >= (int)m_Strings.size()))
    throw EQStringList("StringList Index is out of range in function GetItem()");

  return m_Strings[Index];
}

void CQStringList::SetItem(int Index,QString Item)
{
  if((Index < 0) || (Index >= (int)m_Strings.size()))
    throw EQStringList("StringList Index is out of range in function SetItem()");

  m_Strings[Index] = Item;
}

// Load the entire list from a file
void CQStringList::LoadFromFile(const QString FileName)
{
	QOSFileCreator FileCreator;

    MutexFilesMap.InsertFileMutex(FileName);//Creating and inserting a mutex for this specific file(if not inserted already)

    //Entering the critical section.
	MutexFilesMap.WaitFor(FileName);

	try
    {
		// Open the file for write
		FileCreator.CreateFile(FileName,"rt");
	}
	catch(EQOSFileCreator& Err)
	{
        MutexFilesMap.Release(FileName);
		throw Err;
	}

	FILE *fp = FileCreator.toSTDIO();

	if(fp == NULL)
    {
        MutexFilesMap.Release(FileName);
	    throw EQStringList("Can not open file '" + FileName + "' for read");
    }

	// Clear all item currently in the list
	Clear();

	// Define temporary line buffer
	char LineBuffer[MAX_LINE_SIZE];

	try
	{
		while(!feof(fp))
		{
		  // Read single line and check if error
		  if(fgets(LineBuffer,MAX_LINE_SIZE,fp) == NULL)
			break;

		  // Check if we need to remove the new-line character from the end of the string
		  int l = strlen(LineBuffer);

		  if(l > 0)
			if(LineBuffer[l - 1] == '\n')
			  LineBuffer[l - 1] = NULL;

		  // Add the line to the string list
		  Add(LineBuffer);
		}
	}
    catch(...)
	{
		// Close the file on error
		fclose(fp);
		MutexFilesMap.Release(FileName);
		// Throw our own error
		throw EQStringList("Error while reading to string list from file '" + FileName + "'");
	}

	// Close the file
  fclose(fp);
  MutexFilesMap.Release(FileName);
}

// Save the entire list to a file
void CQStringList::SaveToFile(const QString FileName,char * mode)
{
	QOSFileCreator FileCreator;

    MutexFilesMap.InsertFileMutex(FileName);//Creating and inserting a mutex for this specific file(if not inserted already)

    //Entering the critical section.
	MutexFilesMap.WaitFor(FileName);

	try
    {
		// Open the file for write
        FileCreator.CreateFile(FileName,mode);
	}
	catch(EQOSFileCreator& Err)
	{
        MutexFilesMap.Release(FileName);
		throw Err;
	}

	FILE *fp = FileCreator.toSTDIO();

	if(fp == NULL)
    {
        MutexFilesMap.Release(FileName);
		throw EQStringList("Can not open file '" + FileName + "' for write");
    }

	try
	{
		for(TStringsContainer::iterator i = m_Strings.begin(); i != m_Strings.end(); i++)
		{
			// Write each line to the file (add new-lines characters)
			if(fprintf(fp,"%s\n",(*i).c_str()) == EOF)
			{
				throw;
			}
		}
	}
    catch(...)
	{
		// Close the file on error
		fclose(fp);
		MutexFilesMap.Release(FileName);

		// Throw our own error
		throw EQStringList("Error while writing file '" + FileName + "' from string list");
	}

	// Close the file
	fclose(fp);
	MutexFilesMap.Release(FileName);
}

int CQStringList::FindSubStr(const QString SubStr)
{
  int ret = -1;
  for(unsigned i = 0; i < Count(); i++)
  {
     if (( ret = GetItem(i).find(SubStr)) > 0)
        break;
  }
  return ret;
}

// Load from string, use the "Delimeter" parameter to break the string
void CQStringList::LoadFromString(const QString Str,char Delimeter)
{
  // Clear all items currently in the list
  Clear();

  unsigned i,j = 0;

  for(i = 0; i < Str.length(); i++)
  {
    if(Str[i] == Delimeter)
    {
      QString Tmp = Str.substr(j,i-j);
      Add(Tmp);

      j = i + 1;
    }
  }

  // Add last item
  if(j < i)
  {
    QString Tmp = Str.substr(j,i-j);
    Add(Tmp);
  }
}

QString CQStringList::LoadElementFromString(const QString Str,char Delimeter, int Index)
{
   CQStringList QStringList;
   QStringList.LoadFromString(Str,Delimeter);
   return QStringList.GetItem(Index);
}

