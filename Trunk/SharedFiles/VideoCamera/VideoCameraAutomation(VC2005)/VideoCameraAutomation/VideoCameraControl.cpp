#include "StdAfx.h"
#include "VideoCameraControl.h"
#include "utils.h"

VideoCameraCapturing::VideoCameraControl::VideoCameraControl(void)
{
	// Create the graph builder

    HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL,
                  CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&_spGraphBuilder);

	// TODO: test all the HRESULTs

    // Create the capture graph builder.
    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC,
                          IID_ICaptureGraphBuilder2, (void **)&_spCaptureBuilder);
    if (FAILED(hr))
    	throw hr;

	hr = _spCaptureBuilder->SetFiltergraph(_spGraphBuilder);
    if (FAILED(hr))
    	throw hr;


    // Create the system device enumerator.
    CComPtr<ICreateDevEnum> spDevEnum;
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
                          IID_ICreateDevEnum, (void **)&spDevEnum);
    if (FAILED(hr))
    	throw hr;


    // Create an enumerator for video capture devices.
    CComPtr<IEnumMoniker> spClassEnum;
    hr = spDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &spClassEnum, 0);
    if (hr != S_OK)
    	throw hr;


    ULONG cFetched;
    CComPtr<IMoniker> spMoniker;
	if (spClassEnum->Next(1, &spMoniker, &cFetched) != S_OK)
        throw -1;

    // Bind the moniker to a filter object.
    hr = spMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&_spGrabberBaseFilter);
    if (FAILED(hr))
    	throw hr;
    ////////////////////



    hr = _spGraphBuilder->AddFilter(_spGrabberBaseFilter, L"Capture");
    if (FAILED(hr))
        throw hr;


    // Adding the Sample Grabber to the Filter Graph
    // =============================================

    CComPtr<IBaseFilter>     spF;
    AM_MEDIA_TYPE   mt;

    hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC,
                          IID_IBaseFilter, (LPVOID *)&spF);
    if (FAILED(hr))
		throw hr;

    hr = spF->QueryInterface(IID_ISampleGrabber, (void **)&_spSampleGrabber);
    if (FAILED(hr))
		throw hr;

    hr = _spGraphBuilder->AddFilter(spF, L"Grabber");
    if (FAILED(hr))
		throw hr;

    ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
    mt.majortype = MEDIATYPE_Video;
    mt.subtype = MEDIASUBTYPE_RGB24;
    mt.formattype = FORMAT_VideoInfo;
    hr = _spSampleGrabber->SetMediaType(&mt);
    if (FAILED(hr))
    	throw hr;



    hr = _spCaptureBuilder->RenderStream(
            &PIN_CATEGORY_CAPTURE, // changed from ..._PREVIEW to ..._CAPTURE
            &MEDIATYPE_Video,
            _spGrabberBaseFilter, // src
            spF,            // via
            NULL           // dest ; or pNullRenderer
        );
    if (FAILED(hr))
    	throw hr;



	// TODO: allow the user controlling the video window ?
    // Hide Video Window.
	/* Sample code:
	CComPtr<IVideoWindow> spWindow;
    hr = _spGraphBuilder->QueryInterface(IID_IVideoWindow, (void **)&spWindow);
    if (FAILED(hr))
    {
        ShowMessage("Video Window interface could not be found.");
        throw hr;
    }
    hr = spWindow->put_AutoShow(OAFALSE);
    if (FAILED(hr))
    {
        ShowMessage("Video Window hiding failed.");
        throw hr;
    }
	*/

	hr = _spGraphBuilder->QueryInterface(IID_IMediaControl, (void **)&_spMediaControl);
    if (FAILED(hr))
		throw hr;

    hr = _spSampleGrabber->SetBufferSamples(TRUE);
    if (FAILED(hr))
		throw hr; // TODO: use exceptions hirarchy

    // Set up one-shot mode.
    hr = _spSampleGrabber->SetOneShot(FALSE);
    if (FAILED(hr))
		throw hr;


	///////////////////// Set resolution //////////////////////////
	CComPtr<IAMStreamConfig> spConfig = NULL;
    hr = _spCaptureBuilder->FindInterface(
        &PIN_CATEGORY_CAPTURE, // Preview pin.
        0,    // Any media type.
        _spGrabberBaseFilter, // Pointer to the capture filter.
        IID_IAMStreamConfig, (void**)&spConfig);
    if (!SUCCEEDED(hr))
    	throw hr;


    int iCount = 0, iSize = 0;
    hr = spConfig->GetNumberOfCapabilities(&iCount, &iSize);

    // Check the size to make sure we pass in the correct structure.
    if (iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS))
    {
        // Use the video capabilities structure.

        for (int iFormat = 0; iFormat < iCount; iFormat++)
        {
            VIDEO_STREAM_CONFIG_CAPS scc;
            AM_MEDIA_TYPE *pmtConfig;
            hr = spConfig->GetStreamCaps(iFormat, &pmtConfig, (BYTE*)&scc);
            if (SUCCEEDED(hr))
            {
                /* Examine the format, and possibly use it. */
                if ((pmtConfig->majortype == MEDIATYPE_Video) &&
                    (pmtConfig->subtype == MEDIASUBTYPE_RGB24) &&
                    (pmtConfig->formattype == FORMAT_VideoInfo) &&
                    (pmtConfig->cbFormat >= sizeof (VIDEOINFOHEADER)) &&
                    (pmtConfig->pbFormat != NULL))
                {
                    VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER*)pmtConfig->pbFormat;
                    // pVih contains the detailed format information.
                    if (640 == pVih->bmiHeader.biWidth)
                    {
                    	hr = spConfig->SetFormat(pmtConfig);
                        if (!SUCCEEDED(hr))
                        	throw hr;
						DeleteMediaType(pmtConfig);
						break;
						
                    }

                }



                // Delete the media type when you are done.
                DeleteMediaType(pmtConfig);
            }
		}
		///////////////////////////////////////////////
    }

    // Query the capture filter for the IAMVideoProcAmp interface.
    hr = _spGrabberBaseFilter->QueryInterface(IID_IAMVideoProcAmp, (void**)&_spProcAmp);
    if (FAILED(hr))
    {
		_spProcAmp = NULL;
    }
	/*  Sample code:
	else
	{
	    long Min, Max, Step, Default, Flags, Val;

        // Get the range and default value.
        hr = _spProcAmp->GetRange(VideoProcAmp_Gamma, &Min, &Max, &Step,
            &Default, &Flags);
        if (SUCCEEDED(hr))
        {
            // Get the current value.
            hr = _spProcAmp->Get(VideoProcAmp_Gamma, &Val, &Flags);
        }
	}
	*/

}

VideoCameraCapturing::VideoCameraControl::~VideoCameraControl(void)
{

}


HBITMAP VideoCameraCapturing::VideoCameraControl::sampleFrameAsBitmap()
{
	HBITMAP hBitmap = 0;
	try
    {
    	HRESULT hr;

        hr = _spMediaControl->Run(); // fixme: xxx // TODO: test hr


        Sleep(500); // TODO: use event instead (see also code below)

/*
		CComPtr<IMediaEvent> pMediaEventEx;
	    _spMediaControl->QueryInterface(IID_IMediaEvent, (void **)&pMediaEventEx);
		if (!pMediaEventEx)
        	throw Exception("Could not get media events interface.");

        long evCode;
        hr = pMediaEventEx->WaitForCompletion(INFINITE, &evCode);
        if (FAILED(hr))
            throw hr;
*/


        AM_MEDIA_TYPE MediaType;
        ZeroMemory(&MediaType,sizeof(MediaType));
        hr = _spSampleGrabber->GetConnectedMediaType(&MediaType);
        if (FAILED(hr))
            throw hr;

        // Get a pointer to the video header.
        VIDEOINFOHEADER *pVideoHeader = (VIDEOINFOHEADER*)MediaType.pbFormat;
        if (pVideoHeader == NULL)
            throw E_FAIL;

        // The video header contains the bitmap information.
        // Copy it into a BITMAPINFO structure.
        BITMAPINFO BitmapInfo;
        ZeroMemory(&BitmapInfo, sizeof(BitmapInfo));
        CopyMemory(&BitmapInfo.bmiHeader, &(pVideoHeader->bmiHeader),
                   sizeof(BITMAPINFOHEADER));


        // Create a DIB from the bitmap header, and get a pointer to the buffer.
        void *buffer = NULL;
        hBitmap = ::CreateDIBSection(0, &BitmapInfo, DIB_RGB_COLORS, &buffer,
                                             NULL, 0);
        GdiFlush();
        // Copy the image into the buffer.
        long size = 0;

        hr = _spSampleGrabber->GetCurrentBuffer(&size, (long *)NULL);
        hr = _spSampleGrabber->GetCurrentBuffer(&size, (long *)buffer);
        if (hr != S_OK)
            throw  hr;

	}
    catch(HRESULT hr)
    {
        switch (hr)
        {
			CASE E_INVALIDARG:
				MessageBox(0, L"Getting the sample grabber's current buffer failed (Samples are not being buffered).", L"Objet Video Camera", 0);
			CASE E_POINTER:
				MessageBox(0, L"Getting the sample grabber's current buffer failed (NULL pointer argument).", L"Objet Video Camera", 0);
			CASE VFW_E_NOT_CONNECTED:
				MessageBox(0, L"Getting the sample grabber's current buffer failed (The filter is not connected).", L"Objet Video Camera", 0);
			CASE VFW_E_WRONG_STATE:
				MessageBox(0, L"Getting the sample grabber's current buffer failed (The filter did not buffer a sample yet).", L"Objet Video Camera", 0);
			CASE E_OUTOFMEMORY:
				MessageBox(0, L"Buffer is too small.", L"Objet Video Camera", 0);
			DEFAULT:
				MessageBox(0, L"UNKNOWN CASE!!", L"Objet Video Camera", 0);
				throw Exception(); // TODO: use meaningful exception
        }
		_spMediaControl->Stop(); // fixme: xxx
		throw hr;

    }
    catch(Exception)
    {
    	MessageBox(0, L"Exception", L"Objet Video Camera", 0);
    }
    catch(...)
    {
    	MessageBox(0, L"Unknown Exception", L"Objet Video Camera", 0);
    }
	_spMediaControl->Stop();
	return hBitmap;
}


double VideoCameraCapturing::VideoCameraControl::getGamma(void)
{
	if (!_spProcAmp)
		throw Exception(); // TODO: use meaningful exceptions

	long val, flags;
	HRESULT hr = _spProcAmp->Get(VideoProcAmp_Gamma, &val, &flags); // TODO: test hr


	return 0;
}

void VideoCameraCapturing::VideoCameraControl::setGamma(double newVal) // TODO: gamma calue should be long.
{
	if (_spProcAmp)
		_spProcAmp->Set(VideoProcAmp_Gamma, newVal, VideoProcAmp_Flags_Manual); // TODO: test HRESULT

}
// Opens property pages dialog (filter settings).
void VideoCameraCapturing::VideoCameraControl::openPropertyPages(HWND hwndOwner)
{
	_spMediaControl->Run();
	try
	{
		CComPtr<ISpecifyPropertyPages> spSpec;
		HRESULT hr = _spGrabberBaseFilter->QueryInterface(IID_ISpecifyPropertyPages, (void **)&spSpec);
		if(!SUCCEEDED(hr))
    		throw hr;

		CAUUID cauuid; 
		hr = spSpec->GetPages(&cauuid); // TODO: test HRESULT
		
		hr = OleCreatePropertyFrame(hwndOwner, 30, 30, NULL, 1,
			(IUnknown **)&_spGrabberBaseFilter.p, cauuid.cElems,
			(GUID *)cauuid.pElems, 0, 0, NULL);
	}
	catch(...)
	{
		_spMediaControl->Stop();
		throw;
	}

	_spMediaControl->Stop();

}
