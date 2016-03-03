#pragma once

#include "AtlBase.h"	// For atl smart pointers
#include "dShow.h"		// DirectShow header
#include "Qedit.h"
#include "C:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2\Samples\Multimedia\DirectShow\BaseClasses\Mtype.h"

// #include <ExtCtrls.hpp>
// #include <ComCtrls.hpp>		// SampleGrabber filter


namespace VideoCameraCapturing
{

class Exception
{
};

class VideoCameraControl
{
private:	// User declarations
        CComPtr<IGraphBuilder> 			_spGraphBuilder;
        CComPtr<ICaptureGraphBuilder2>	_spCaptureBuilder;
        CComPtr<IBaseFilter>			_spGrabberBaseFilter;
        CComPtr<ISampleGrabber>			_spSampleGrabber;
        CComPtr<IMediaControl>			_spMediaControl;
		CComPtr<IAMVideoProcAmp> 		_spProcAmp;

public:
	VideoCameraControl(void);
	~VideoCameraControl(void);
	HBITMAP sampleFrameAsBitmap();

	double	getGamma(void);
	void	setGamma(double newVal);

	// Opens property pages dialog (filter settings).
	void openPropertyPages(HWND hwndOwner);
};

}
