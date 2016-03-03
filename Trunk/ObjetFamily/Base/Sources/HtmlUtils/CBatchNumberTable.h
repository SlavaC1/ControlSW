//---------------------------------------------------------------------------

#ifndef CBatchNumberTableH
#define CBatchNumberTableH

#include "QTypes.h"
#include "GlobalDefs.h"
#include "CHTML_BatchNoReport.h"

//max num of batch numbers for tank depends on max num of swaps during printing
const int MAX_NUM_OF_TANK_SWAPS = 5;

//holds all the batch numbers (and resin type) per tank during printing
class CTankBatchNoTable
{
private:
  QString m_resinType;
  QString batchNoArr[MAX_NUM_OF_TANK_SWAPS]/*[BATCH_NO_SIZE]*/; //check if 5 is ok
  int m_nextBatchNoIndex;

public:
  //Constructor
  CTankBatchNoTable();
  void SetBatchNo(char* batchNumber);
  QString GetBatchNo(int index)const;
  QString GetLastBatchNo()const;
  void SetResinType(QString resinType)
  {
	//need to set only once
    if(m_nextBatchNoIndex==0)
      m_resinType = resinType;
  }
  QString GetResinType() const;
  int GetBatchNoIndex() const { return m_nextBatchNoIndex; }

  void PrintBatchNoTable();
  void ClearBatchNoTable() { m_nextBatchNoIndex=0; }
};

//the class handles the batch numbers of all tanks and some additional data during printing
//the class is singleton
class CBatchNoTable
{
private:
  QString m_printStart;
  QString m_printEnd;
  QString m_trayID;
  QString m_printingMode;
  bool m_isPrinting;
  CTankBatchNoTable m_batchNoArr[TOTAL_NUMBER_OF_CONTAINERS];
  CHTML_BatchNoReport * m_BatchNoReport;
  static CBatchNoTable* m_this;

  //constructor
  CBatchNoTable();
  //destructor
  ~CBatchNoTable();
public:
  static CBatchNoTable* GetInstance();
  void Init(bool isPrinting, QString printStart, QString trayID, QString printingMode);
  void DeInit();
  QString GetPrintStart()const { return m_printStart; }
  QString GetPrintEnd() const { return m_printEnd; }
  QString GetTrayID() const { return m_trayID; }
  QString GetPrintingMode() const { return m_printingMode; }
  QString GetTankBatchNo(int tankIndex, int batchNoIndex) { return m_batchNoArr[tankIndex].GetBatchNo(batchNoIndex); }
  void SetPrintEnd(QString printEnd);

  void SetTankResinType(int tankIndex, QString resinType) { m_batchNoArr[tankIndex].SetResinType(resinType); }
  void SetTankBatchNo(int tankIndex, char* batchNumber) { m_batchNoArr[tankIndex].SetBatchNo(batchNumber); }
  void AddTankBatchNumber(int tankIndex,char * batchNo, QString resinType);

  QString GetTankLastBatchNo(int tankIndex) { return m_batchNoArr[tankIndex].GetLastBatchNo(); }
  QString GetTankResinType(int tankIndex) { return m_batchNoArr[tankIndex].GetResinType(); }
  int GetTankBatchNoIndex(int tankIndex) { return m_batchNoArr[tankIndex].GetBatchNoIndex(); }
  void ClearBatchNoTable(); //clears all tanks' current batch numbers
  void SetIsPrinting(bool isPrinting) { m_isPrinting = isPrinting; }
  bool IsPrinting()const{ return m_isPrinting; }

  void PrintTanksBatchNoTable();
  void ShowMaterialBatchNoReport();

};



//---------------------------------------------------------------------------
#endif
