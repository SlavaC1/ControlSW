//---------------------------------------------------------------------------

#ifndef PFFPrintJobH
#define PFFPrintJobH
//---------------------------------------------------------------------------
#include "PrintJob.h"
#include "HostPrintJob.h"
#include "QMessageQueue.h"
#include "AppParams.h"



class CSlice;
class CPFFSliceQueueThread;
typedef CQMessageQueue<CSlice *> TSliceMessageQueue;


// Exception class for the file print job
class EPFFPrintJob : public EPrintJob {
  public:
    EPFFPrintJob(const QString& ErrMsg,const TQErrCode ErrCode=0) : EPrintJob(ErrMsg,ErrCode) {}
};



class CPFFPrintJob : public CPrintJob {
  private:
	CPFFSliceQueueThread* 	m_PFFSliceQueueThreadPtr;
	TSliceMessageQueue* m_SliceMessageQueue;
	int m_CurrentSliceNumber;
  public:

    // Constructor
	CPFFPrintJob();
	// Destructor
	~CPFFPrintJob(void);

	void Init(CPFFSliceQueueThread* PFFSliceQueueThread);

    // Bring a single slice (override)
    // If returned with false, no more slices are available
    CSlice *BringSlice(void);

    // Return true if the job is a continue (recovered) job (override)
    bool IsContinue(void) {
//      return m_Continued;
		return false;
    }

    // Notify that we are done with a specific slice
    void DoneWithSlice(CSlice *Slice);

};

#endif
