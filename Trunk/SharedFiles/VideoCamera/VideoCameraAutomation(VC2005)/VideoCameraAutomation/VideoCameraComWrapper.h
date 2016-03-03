// VideoCameraComWrapper.h : Declaration of the CVideoCameraComWrapper

#pragma once
#include "stdafx.h"

#include "resource.h"       // main symbols

#include "VideoCameraAutomation.h"
#include "VideoCameraControl.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CVideoCameraComWrapper

class ATL_NO_VTABLE CVideoCameraComWrapper :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CVideoCameraComWrapper, &CLSID_VideoCameraComWrapper>,
	public IDispatchImpl<IVideoCamera, &IID_IVideoCamera, &LIBID_VideoCameraAutomationLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
private:
	std::auto_ptr<VideoCameraCapturing::VideoCameraControl> _spVideoCamera;
	std::string _errorDescription;
public:
	CVideoCameraComWrapper() :
	_errorDescription("")
	{
	
	}

DECLARE_REGISTRY_RESOURCEID(IDR_VIDEOCAMERACOMWRAPPER)


BEGIN_COM_MAP(CVideoCameraComWrapper)
	COM_INTERFACE_ENTRY(IVideoCamera)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{

		try
		{
			_spVideoCamera = new VideoCameraCapturing::VideoCameraControl();
		}
		catch(...)
		{
			_errorDescription = "Could not create video camera control objet.";
			return E_FAIL;
		}

		return S_OK;
	}

	void FinalRelease()
	{
	}

public:

	STDMETHOD(sampleToBitmap)(HBITMAP* phbitmap);
	STDMETHOD(get_Gamma)(double* pVal);
	STDMETHOD(put_Gamma)(double newVal);
	STDMETHOD(openSettingsDialog)(HWND hwndOwner);
};

OBJECT_ENTRY_AUTO(__uuidof(VideoCameraComWrapper), CVideoCameraComWrapper)
