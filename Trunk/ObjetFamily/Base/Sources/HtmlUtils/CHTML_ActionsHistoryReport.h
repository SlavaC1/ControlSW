/*********************************************************************
 *                        Objet Geometries LTD.                      *
 *                        ---------------------                      *
 * Project: Objet.                                                 *
 *                                                                   *
 * Class description: Write the actions dates table to html file.    *
 *                                                                   *
 * Compilation: Standard C++.                                        *
 *                                                                   *
 * Author: Itamar Aharon                                             *
 * Start date: 18/07/2011                                            *
 * Last upate: 18/07/2011                                            *
 ********************************************************************/
//---------------------------------------------------------------------------

#ifndef CHTML_ActionsHistoryReportH
#define CHTML_ActionsHistoryReportH

#include "QStringList.h"
#include "CHTML_ReportFile.h"

// Class for HTML BIT report
class CHTML_ActionsHistoryReport : public CHTML_ReportFile
{
  private:
	//Read data from m_StrList into CActionsHistoryTable object
	void ReadActionHistoryData();

  public:
	// Constructor
	CHTML_ActionsHistoryReport();
	// Destructor
	//~CHTML_ActionsHistoryReport();
	
	virtual TQErrCode Init();

	// Create the report file
	void CreateActionsHistoryReport();

	virtual void OpenReportFile();

};
//---------------------------------------------------------------------------
#endif
