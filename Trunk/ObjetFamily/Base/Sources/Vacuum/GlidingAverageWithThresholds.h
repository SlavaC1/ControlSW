//---------------------------------------------------------------------------

#ifndef _AVERAGE_WITH_THRESHOLDS_H_
#define _AVERAGE_WITH_THRESHOLDS_H_

#include "GlidingAverage.h"
#include <assert.h> 

const int SINGLE_ERROR_IN_PERCENTAGE = 70;

template <class T =  float>
class GlidingAverageWithThresholds : public GlidingAverage<T> {

protected:

	bool	*m_GlidingFlaggedWindow;
	bool	*m_GlidingFlaggedSinglesWindow;
	T		*m_GlidingAverageWindow;
	bool	*m_ParticipatedInTheSum;

	T 		m_HighThreshold;
	T 		m_LowThreshold;
	int 	m_AverageOutOfRangeCounter;
	int     m_SingleReadingOutOfRangeCounter;
	int     m_ErrorsNumberAccordingPercentage;
	bool    IsThisReadingOutOfRange(T Reading);
	bool    m_VacuumProblem;

	
public:
	GlidingAverageWithThresholds()
	{
		m_VacuumProblem = false;
	    m_HighThreshold = 0;
		m_LowThreshold = 0;
		m_GlidingAverageWindow = NULL;
		m_GlidingFlaggedWindow = NULL;
		m_GlidingFlaggedSinglesWindow = NULL;
		m_ParticipatedInTheSum = NULL;
		m_AverageOutOfRangeCounter = 0;
		m_SingleReadingOutOfRangeCounter = 0;
		m_ErrorsNumberAccordingPercentage = 0;
	}
	~GlidingAverageWithThresholds()
	{
		if(m_GlidingFlaggedWindow)
			delete[] m_GlidingFlaggedWindow;

 		if(m_GlidingFlaggedSinglesWindow)
			delete[] m_GlidingFlaggedSinglesWindow;

		if(m_GlidingAverageWindow)
			delete[] m_GlidingAverageWindow;

		if(m_ParticipatedInTheSum)
			delete[] m_ParticipatedInTheSum;			
			
	};
	GlidingAverageWithThresholds(T DefaultAverageValue,int WindowSize, T HighThreshold, T LowThreshold, int ErrorsInPercentage);
	void Initialize(int WindowSize,T DefaultAverageValue,T HighThreshold, T LowThreshold, int ErrorsInPercentage);
	void 	AddReading(T Reading);
	inline bool    IsVacuumProblem(){return m_VacuumProblem;}
	inline int    GetAverageOutOfRangeCounter(){return m_AverageOutOfRangeCounter;}
	inline int    GetSingleReadingOutOfRangeCounter(){return m_SingleReadingOutOfRangeCounter;}
		
};


template <class T>
void GlidingAverageWithThresholds<T>::Initialize(int WindowSize,T DefaultAverageValue,T HighThreshold, T LowThreshold, int ErrorsInPercentage)
{
		GlidingAverage<float>::Initialize (WindowSize,DefaultAverageValue);
		
		m_HighThreshold = HighThreshold;
		m_LowThreshold  = LowThreshold;
		m_WindowSize = WindowSize;
		m_AverageOutOfRangeCounter = 0;
		m_SingleReadingOutOfRangeCounter = 0;
		assert(m_WindowSize);
		m_ErrorsNumberAccordingPercentage = static_cast<int>(WindowSize * ErrorsInPercentage/100);
		m_Average = m_Sum/m_WindowSize;

		try{
		
			m_GlidingFlaggedWindow = new bool[m_WindowSize];
			m_GlidingFlaggedSinglesWindow = new bool[m_WindowSize];
			m_ParticipatedInTheSum = new bool[m_WindowSize]; 			
			m_GlidingAverageWindow = new T[m_WindowSize];
			
			for(int i = 0; i < m_WindowSize; i++)
			{
				m_GlidingFlaggedWindow[i] = false;
				m_GlidingFlaggedSinglesWindow[i] = false;
				m_ParticipatedInTheSum[i] = true;				
				m_GlidingAverageWindow[i] = 0;
			}
		}
		catch(EQException& err)
		{
			QMonitor.WarningMessage(err.GetErrorMsg());
		}
		catch(...)
		{
			  QMonitor.WarningMessage("GlidingAverage - Unknown exception.");
		}
		
}

template <class T >
void GlidingAverageWithThresholds<T>::AddReading(T Reading)
{
	try{

		T NewSum = 0;
		T Average = 0;
		bool AverageOutOfRange = false;
		bool ReadingOutOfRange = false;

		int SingleErrorNumber = static_cast<int>(m_WindowSize * SINGLE_ERROR_IN_PERCENTAGE/100);

		//if the current reading was not participated in that sum,
		//we need to reduce the average which was calculated than and add the new reading.
		if(m_ParticipatedInTheSum[m_CurrentReadingIndex] == false)
		{
			NewSum = m_Sum - m_GlidingAverageWindow[m_CurrentReadingIndex] + Reading;
		}
		else
		{
		   NewSum =  GetCalculatedSum(Reading);
		}
			
		if(m_GlidingFlaggedWindow[m_CurrentReadingIndex] == true)
		{
			m_GlidingFlaggedWindow[m_CurrentReadingIndex] = false;
			m_AverageOutOfRangeCounter--; //subtracting 1 before inserting the new reading.
			m_AverageOutOfRangeCounter = (m_AverageOutOfRangeCounter < 0) ? 0 : m_AverageOutOfRangeCounter;

			// If there are no more "invalid" samples in the buffer then re-calculate the sum.
			if (m_AverageOutOfRangeCounter == 0)
			{
				NewSum = GetFullWindowSum(Reading);
			}
		}

		if(m_GlidingFlaggedSinglesWindow[m_CurrentReadingIndex] == true)
		{
            m_GlidingFlaggedSinglesWindow[m_CurrentReadingIndex] = false;
			m_SingleReadingOutOfRangeCounter--;
			m_SingleReadingOutOfRangeCounter = (m_SingleReadingOutOfRangeCounter < 0) ? 0 : m_SingleReadingOutOfRangeCounter;
		}

		assert(m_WindowSize);
		Average = NewSum/m_WindowSize;

		//This line should come before inserting the new reading to the array!!!
		AverageOutOfRange = IsThisReadingOutOfRange(Average);

		ReadingOutOfRange = IsThisReadingOutOfRange(Reading);
			
		if(ReadingOutOfRange)
		{
        	m_GlidingFlaggedSinglesWindow[m_CurrentReadingIndex] = true;
			m_SingleReadingOutOfRangeCounter++;
			m_SingleReadingOutOfRangeCounter = (m_SingleReadingOutOfRangeCounter > m_WindowSize) ? m_WindowSize : m_SingleReadingOutOfRangeCounter;
		}
			
		//Replacing the old value with the new one			
		m_GlidingWindowReading[m_CurrentReadingIndex] = Reading;

		if(AverageOutOfRange && ReadingOutOfRange)//Average + Single reading are out of range
		{
			m_GlidingFlaggedWindow[m_CurrentReadingIndex] = true;
			m_ParticipatedInTheSum[m_CurrentReadingIndex] = false;
			m_AverageOutOfRangeCounter++;
			m_AverageOutOfRangeCounter = (m_AverageOutOfRangeCounter > m_WindowSize)? m_WindowSize : m_AverageOutOfRangeCounter;
		}
		else
		{
			m_ParticipatedInTheSum[m_CurrentReadingIndex] = true;
			m_Sum = NewSum;
			assert(m_WindowSize);
			m_Average = m_Sum/m_WindowSize;
		}

		bool CriteriaForSingleReading  =  ((m_SingleReadingOutOfRangeCounter > SingleErrorNumber))? true : false;
		bool CriteriaForAverageReading =  ((m_AverageOutOfRangeCounter > m_ErrorsNumberAccordingPercentage))? true : false;

		// if the critical counter is above/equal to the critical precentage we want to insert the critical values.
		if(CriteriaForAverageReading || CriteriaForSingleReading)
		{
			m_Sum = NewSum;
			assert(m_WindowSize);
			m_Average = m_Sum/m_WindowSize;
			m_VacuumProblem = true;
			m_ParticipatedInTheSum[m_CurrentReadingIndex] = true;
		}
		else if (AverageOutOfRange == false)
		{
			m_VacuumProblem = false;
		}

		m_GlidingAverageWindow[m_CurrentReadingIndex] = m_Average;
		
		m_CurrentReadingIndex++;
		m_CurrentReadingIndex = m_CurrentReadingIndex % m_WindowSize;

	}
	catch(EQException& err)
	{
		QMonitor.WarningMessage(err.GetErrorMsg());
	}	
}


template <class T>
bool GlidingAverageWithThresholds<T>::IsThisReadingOutOfRange(T Reading)
{	
	try{

		if((Reading <= m_HighThreshold) && (Reading >= m_LowThreshold))//in range
		{
			return false;
		}
		else//out of range
		{
			return true;
		}
			
	}
	catch(...)
	{
		QMonitor.WarningMessage("something got wrong in CalculatedSum");
		
	}
	return true;
}

template <class T>
GlidingAverageWithThresholds<T>::GlidingAverageWithThresholds(T DefaultAverageValue,int WindowSize, T HighThreshold, T LowThreshold, int ErrorsInPercentage)
: GlidingAverage(DefaultAverageValue,WindowSize)
{
		assert(HighThreshold);
		assert(LowThreshold);
		assert(WindowSize);

		Initialize(WindowSize,DefaultAverageValue,HighThreshold,LowThreshold,ErrorsInPercentage);
		
}

#endif 


