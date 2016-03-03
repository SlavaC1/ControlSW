// VideoCameraComWrapper.cpp : Implementation of CVideoCameraComWrapper

#include "stdafx.h"
#include "VideoCameraComWrapper.h"


// CVideoCameraComWrapper

// TODO: maybe catch the exceptions here and return error values

STDMETHODIMP CVideoCameraComWrapper::sampleToBitmap(HBITMAP* phbitmap)
{
	try
	{
		*phbitmap = _spVideoCamera->sampleFrameAsBitmap();
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CVideoCameraComWrapper::get_Gamma(double* pVal)
{
	
	try
	{
		*pVal = _spVideoCamera->getGamma();
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CVideoCameraComWrapper::put_Gamma(double newVal)
{
	try
	{
		_spVideoCamera->setGamma(newVal);
	}
	catch(...)
	{
		return E_FAIL;
	}
	

	return S_OK;
}

STDMETHODIMP CVideoCameraComWrapper::openSettingsDialog(HWND hwndOwner)
{
	try
	{
		_spVideoCamera->openPropertyPages(0); // TODO: here 0 is passes instead of hwndOwner because I did not know how to pass the appropriate hwnd from the clien written in Borland. Find out how to do that and pass hwndOwner here.
	}
	catch(...)
	{
		return E_FAIL;
	}
	

	return S_OK;
}
