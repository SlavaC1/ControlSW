/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib utilities.                                          *
 * Class Description: For moving average calculation.               *
 *                                                                  *
 ********************************************************************/

#ifndef CMovingAverageH
#define CMovingAverageH

#include <queue>

template <class Type>
class CMovingAverage
{
   public:
	 CMovingAverage(const int NumOfSamples) : m_numOfSamples(NumOfSamples),m_avg(0),m_sum(0) {};
	 ~CMovingAverage();

	 void AddSample(Type NewSample);
	 Type GetAverage();
	 bool IsQueueFull();
	 void Clear();

   protected:
	 const unsigned int m_numOfSamples;
	 Type m_avg;
	 Type m_sum;
	 std::queue<Type> m_samplesQ;
};

template <class Type>
CMovingAverage<Type>::~CMovingAverage()
{
  while(!m_samplesQ.empty())
  {
	m_samplesQ.pop();
  }
}

template <class Type>
void CMovingAverage<Type>::Clear()
{
  while(!m_samplesQ.empty())
  {
	m_samplesQ.pop();
  }
   m_avg = 0;
   m_sum = 0;
}

template <class Type>
void CMovingAverage<Type>::AddSample(Type NewSample)
{
  if(m_samplesQ.size() >= m_numOfSamples)
  {
	  Type OldSample = m_samplesQ.front();
  	  m_samplesQ.pop();

	  m_samplesQ.push(NewSample);
	  m_sum = m_sum - OldSample + NewSample;
	  m_avg = m_sum/m_numOfSamples;
  }
  else  //queue is not full
  {
	  m_sum += NewSample;
  	  m_samplesQ.push(NewSample);
	  m_avg = m_sum/m_samplesQ.size();
  }
}

template <class Type>
Type CMovingAverage<Type>::GetAverage()
{
  return m_avg;
}

template <class Type>
bool CMovingAverage<Type>::IsQueueFull()
{
  return (m_samplesQ.size() >= m_numOfSamples);
}

//---------------------------------------------------------------------------
#endif
