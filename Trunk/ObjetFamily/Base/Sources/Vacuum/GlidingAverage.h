//---------------------------------------------------------------------------

#ifndef _AVERAGE_H_
#define _AVERAGE_H_


#include "ErrorHandler.h"
#include "AppParams.h"

  
// Exception class for all the QLib RTTI elements
class EGlidingAverage : public EQException {
  public:
    EGlidingAverage(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

template <class T = float>
class GlidingAverage : public CQComponent {

protected:

	T		*m_GlidingWindowReading;
	T		m_Average;
	T		m_Sum;
	int   	m_WindowSize;
	int     m_CurrentReadingIndex;
	T    	GetCalculatedSum(T Reading);
	T		GetFullWindowSum(T Reading);
	GlidingAverage()//There is no use in defualt constructor
	{
	    m_Average = 0 ;
	    m_Sum = 0;
	    m_GlidingWindowReading = NULL;
	    m_WindowSize = 0;
        m_CurrentReadingIndex = 0;
	};
	
public:


	GlidingAverage(T DefaultAverageValue,int WindowSize);
	~GlidingAverage()
	{
		if(m_GlidingWindowReading)
			delete[] m_GlidingWindowReading;
	};
	
	void Initialize(int WindowSize, T DefaultAverageValue);	
	
	inline T GetAverage(){return m_Average;}
	void AddReading(T Reading);
	T GetCurrentReading();

};


template <class T>
T GlidingAverage<T>::GetCurrentReading()
{
	assert(m_GlidingWindowReading);
	int Index = ((m_CurrentReadingIndex - 1) < 0 )? ( m_WindowSize -1) : (m_CurrentReadingIndex -1) ;
	return m_GlidingWindowReading[Index];
}

template <class T>
void GlidingAverage<T>::Initialize(int WindowSize,T DefaultAverageValue)
{
	try{

		m_Average 				= 0;
		m_CurrentReadingIndex 	= 0;
		m_Sum 					= 0;
		m_WindowSize = WindowSize;	
		
		assert(m_WindowSize);
		m_GlidingWindowReading = new T[WindowSize];
			
		for(m_CurrentReadingIndex = 0; m_CurrentReadingIndex < m_WindowSize; m_CurrentReadingIndex++)
		{
			m_GlidingWindowReading[m_CurrentReadingIndex] = DefaultAverageValue;
			m_Sum += DefaultAverageValue;
		}

		m_CurrentReadingIndex = m_CurrentReadingIndex % m_WindowSize;
		
		assert(m_WindowSize);
		m_Average = m_Sum/m_WindowSize;


	}
	catch(EQException& err)
	{
		QMonitor.WarningMessage(err.GetErrorMsg());
		
	}

}

template <class T >
void GlidingAverage<T>::AddReading(T Reading)
{
	try{

		//Replacing the new value with the old one	
		m_GlidingWindowReading[m_CurrentReadingIndex] = Reading;
		m_CurrentReadingIndex++;
		m_CurrentReadingIndex = (m_CurrentReadingIndex >= m_WindowSize) ? 0 : m_CurrentReadingIndex;

	}
	catch(EQException& err)
	{
		QMonitor.WarningMessage(Err.GetErrorMsg());
	}	
}

template <class T>
GlidingAverage<T>::GlidingAverage(T DefaultAverageValue,int WindowSize): CQComponent("GlidingAverage")
{
	Initialize(WindowSize, DefaultAverageValue);
}

template <class T>
T GlidingAverage<T>::GetCalculatedSum(T Reading)
{	
	T Sum = m_Sum;
	assert(m_GlidingWindowReading);
	try{

		Sum = Sum - m_GlidingWindowReading[m_CurrentReadingIndex] + Reading;
	}
	catch(...)
	{
		QMonitor.WarningMessage("something got wrong in CalculatedSum");
	}
	return Sum;
}

template <class T>
T GlidingAverage<T>::GetFullWindowSum(T Reading)
{	
	T Sum = 0;
	assert(m_GlidingWindowReading);
	try{

		// Calculates the real sum of the whole buffer
		for (int i=0; i < m_WindowSize; i++)
		{
			Sum += m_GlidingWindowReading[i];
		}
		// Subtracts the oldest value and adds the newest and recent one to the sum
		Sum = Sum - m_GlidingWindowReading[m_CurrentReadingIndex] + Reading;
	}
	catch(...)
	{
		QMonitor.WarningMessage("something got wrong in GetFullWindowSum");
	}
	return Sum;
}

#endif 


