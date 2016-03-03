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

#ifndef _Q_STRING_LIST_H_
#define _Q_STRING_LIST_H_

#include <vector>
#include "QObject.h"
#include "QException.h"


// Exception class for all the QLib RTTI elements
class EQStringList : public EQException {
  public:
    EQStringList(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};


// Utility string list class
class CQStringList : public CQObject
{
  private:
    // Type for the actual string list container
    typedef std::vector<QString> TStringsContainer;

    // Actual strings storage
    TStringsContainer m_Strings;

  public:
    // Default constructor
    CQStringList(void) {}

    // Open file constructor
    CQStringList(const QString FileName)
    {
      LoadFromFile(FileName);
    }

    // A string list object is accessible as an array
    QString& operator [](unsigned Index)
    {
      return m_Strings[Index];
    }

    // Return the number of elements in the list
    unsigned Count(void)
    {
      return m_Strings.size();
    }

    // Clear the content of the list
    void Clear(void)
    {
      m_Strings.clear();
    }

    // Explicit get/set item
    QString GetItem(int Index) const;
    void SetItem(int Index,QString Item);

    // Add a new string
    void Add(const QString Str);
	
	void RemoveByString(const QString Str);
	void Erase(int index);

    // Sort the strings
    void Sort(void);

    // Load from string, use the "Delimeter" parameter to break the string
    void LoadFromString(const QString Str,char Delimeter = ',');

    // Load the entire list from a file
    void LoadFromFile(const QString FileName);

    // Save the entire list to a file
    void SaveToFile(const QString FileName, char * mode = "wt"); 

    int Find(const QString Str)
    {
      for(unsigned i = 0; i < Count(); i++)
         if (GetItem(i) == Str)
            return (int)i;
      return -1;
    }
    
    int FindSubStr(const QString SubStr);

    static QString LoadElementFromString(const QString Str,char Delimeter, int Index);

};

#endif

