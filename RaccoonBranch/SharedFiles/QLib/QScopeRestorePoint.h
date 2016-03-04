#include "QParameter.h"

template<class T>
class CScopeRestorePoint
{

protected:

	const T _objOrigVal;
    T &_objRef;
    CQParameter<T>* m_QParam;

public:

	CScopeRestorePoint(T &obj) :
    _objOrigVal(obj),
    _objRef(obj),
    m_QParam(NULL)
    {}

	CScopeRestorePoint(CQParameter<T>& QParam) :
    _objRef(QParam),
    _objOrigVal(QParam),
    m_QParam(&QParam)
    {}

    ~CScopeRestorePoint()
    {
    	_objRef = _objOrigVal;
        if (m_QParam)
           m_QParam->NotifyGenericObservers();
    }
};

template<class T, int size>
class CScopeRestorePointArray
{
    CScopeRestorePoint<T>* _obj[size];

public:

	CScopeRestorePointArray(T obj[size])
    {
        for(int i=0; i<size; i++)
    	   _objRef[i] = new CScopeRestorePoint<T>(obj[i]);
    }

    ~CScopeRestorePointArray()
    {
        for(int i=0; i<size; i++)
    	   delete _objRef[i];
    }
};

template<class T, int size>
class CParamScopeRestorePointArray
{
    CScopeRestorePoint<T>** _obj;
    CQArrayParameter<T,size>& m_ArrayParameter;
public:

	CParamScopeRestorePointArray(CQArrayParameter<T,size>& ArrayParameter) :
        m_ArrayParameter(ArrayParameter)
    {
    	_obj = new CScopeRestorePoint<T>*[size];
        for(int i=0; i<size; i++)
    	   _obj[i] = new CScopeRestorePoint<T>(ArrayParameter[i]);
    }

    ~CParamScopeRestorePointArray()
    {
        for(int i=0; i<size; i++)
    	   delete _obj[i];
        delete _obj;

        m_ArrayParameter.NotifyGenericObservers();
    }
};

