/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT.                                                   *
 * Module: BIT                                                      *
 * Module Description: Python scripts BIT tests support.            *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg                                                *
 * Start date: 31/07/2003                                           *
 * Last upate: 31/07/2003                                           *
 ********************************************************************/

#include "PythonTestGroup.h"
#include "BITManager.h"
#include "QFileSystem.h"
#include "Q2RTApplication.h"
#include "QMonitor.h"


namespace PythonAppBIT {

// Register all test python scripts to the given BIT manager
void Register(CBITManager *BITManager)
{
  // Get a list of all available script files
  TQFileSearchRec SearchRec;

  QString PathStr = Q2RTApplication->AppFilePath.Value() + "BIT\\";

  // Enumerate scripts directory
  if(QFindFirst(PathStr + "*.py",faAnyFile,SearchRec))
    do
    {
      // Ignore files that starts with the '~' character
      if(SearchRec.Name[1] != '~')
      {
        QString FullFileName = PathStr + SearchRec.Name.c_str();

        // Create a test group object
        CPythonTestGroup *PythonTestGroup = new CPythonTestGroup(FullFileName);

        try
        {
          PythonTestGroup->Refresh();
          BITManager->RegisterTestGroup(PythonTestGroup);
        }
        catch(EQException& Err)
        {
          QMonitor.ErrorMessage("Error in BIT script \"" + PythonTestGroup->Name() + "\" - " + Err.GetErrorMsg());
        }
        catch(...)
        {

        }
      }

    } while(QFindNext(SearchRec));

  QFindClose(SearchRec);

  //BITManager->SortByGroup();
  BITManager->Sort();
}

} // End of namespace "PythonAppBIT"
