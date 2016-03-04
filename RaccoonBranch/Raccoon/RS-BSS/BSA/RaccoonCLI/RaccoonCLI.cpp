// This is the main DLL file.

#include "stdafx.h"
#include "RaccoonCLI.h"

#pragma unmanaged
#include "Utils.h"
#pragma managed

using namespace RSCommon;
using namespace StratasysIDTSrv;
using namespace System::IO;
using namespace System::Runtime::InteropServices;
using namespace System::Text;
using namespace System::Threading;

namespace RaccoonCLI {

#define UNHANDLED_EXCEPTION -1;

	/// <summary>
	/// Initializes a new instance of the <see cref="MaterialMonitorWrapper"/> class.
	/// </summary>
	MaterialMonitorWrapper::MaterialMonitorWrapper()
	{
		m_materialMonitor = new MaterialMonitor();
		m_syncObj = gcnew Object();
	}

	/// <summary>
	/// Finalizes an instance of the <see cref="MaterialMonitorWrapper"/> class.
	/// </summary>
	MaterialMonitorWrapper::~MaterialMonitorWrapper()
	{
		delete m_materialMonitor;
	}

	int MaterialMonitorWrapper::InitHW(Byte %cartridgesCount)
	{
		bool taken = false;
		Monitor::Enter(m_syncObj, taken);
		try
		{
			pin_ptr<Byte> pCartridgesCount = &cartridgesCount;
			unsigned char* npCartridgesCount = pCartridgesCount;
			return m_materialMonitor->InitHW(npCartridgesCount);
		}
		catch (...)
		{
			return UNHANDLED_EXCEPTION;
		}
		finally
		{
			if (taken)
			{
				Monitor::Exit(m_syncObj);
			}
		}
	}

	int MaterialMonitorWrapper::SetPubKS(array<Byte>^ pubKS)
	{
		bool taken = false;
		Monitor::Enter(m_syncObj, taken);
		try
		{
			unsigned char* npPubKS = NULL;
			if (pubKS != nullptr && pubKS->Length > 0)
			{
				pin_ptr<Byte> pPubKS = &pubKS[0];
				npPubKS = pPubKS;
			}

			return m_materialMonitor->SetPubKS(npPubKS);
		}
		catch (...)
		{
			return UNHANDLED_EXCEPTION;
		}
		finally
		{
			if (taken)
			{
				Monitor::Exit(m_syncObj);
			}
		}
	}


	int MaterialMonitorWrapper::GetInPlaceStatus(Int32% status)
	{
		bool taken = false;
		Monitor::Enter(m_syncObj, taken);
		try
		{
			pin_ptr<Int32> pStatus = &status;
			int* npStatus = pStatus;
			return m_materialMonitor->GetInPlaceStatus(npStatus);
		}
		catch (...)
		{
			return UNHANDLED_EXCEPTION;
		}
		finally
		{
			if (taken)
			{
				Monitor::Exit(m_syncObj);
			}
		}
	}

	int MaterialMonitorWrapper::ResetCartridge(Byte cartridgeNum, array<Byte>^ password)
	{
		bool taken = false;
		Monitor::Enter(m_syncObj, taken);
		try
		{
			unsigned char* npPassword = NULL;
			unsigned char passwordLength = 0;
			if (password != nullptr && password->Length > 0)
			{
				pin_ptr<Byte> pPassword = &password[0];
				npPassword = pPassword;
				passwordLength = password->Length;
			}

			int returnValue = m_materialMonitor->ResetCartridge(cartridgeNum - 1, npPassword, passwordLength, NULL);
			return returnValue;
		}
		catch (...)
		{
			return UNHANDLED_EXCEPTION;
		}
		finally
		{
			if (taken)
			{
				Monitor::Exit(m_syncObj);
			}
		}
	}

	int MaterialMonitorWrapper::BurnSignedIdd(Byte cartridgeNum, array<Byte>^ materialInfo, array<Byte>^ signature)
	{
		bool taken = false;
		Monitor::Enter(m_syncObj, taken);
		try
		{
			unsigned char* npMaterialInfo = NULL;
			unsigned short materialInfoLength = 0;
			if (materialInfo != nullptr && materialInfo->Length > 0)
			{
				pin_ptr<Byte> pMaterialInfo = &materialInfo[0];
				npMaterialInfo = pMaterialInfo;
				materialInfoLength = materialInfo->Length;
			}

			unsigned char* npSignature = NULL;
			unsigned short signatureLength = 0;
			if (signature != nullptr && signature->Length > 0)
			{
				pin_ptr<Byte> pSignature = &signature[0];
				npSignature = pSignature;
				signatureLength = signature->Length;
			}

			int returnValue = m_materialMonitor->BurnSignedIdd(cartridgeNum - 1, npMaterialInfo, materialInfoLength, npSignature, signatureLength);
			return returnValue;
		}
		catch (...)
		{
			return UNHANDLED_EXCEPTION;
		}
		finally
		{
			if (taken)
			{
				Monitor::Exit(m_syncObj);
			}
		}
	}

	///
	/// Removes cartridge.
	///
	int MaterialMonitorWrapper::RemoveCartridge(Byte cartridgeNum)
	{
		bool taken = false;
		Monitor::Enter(m_syncObj, taken);
		try
		{
			return m_materialMonitor->RemoveCartridge(cartridgeNum - 1);
		}
		catch (...)
		{
			return UNHANDLED_EXCEPTION;
		}
		finally
		{
			if (taken)
			{
				Monitor::Exit(m_syncObj);
			}
		}
	}

	///
	/// Authenticates cartridge.
	///
	int MaterialMonitorWrapper::AuthenticateCartridge(Byte cartridgeNum, array<Byte>^% idd, UInt32% currentVolume)
	{
		bool taken = false;
		Monitor::Enter(m_syncObj, taken);
		try
		{
			unsigned char npIdd[89]; // VaultIC file maximal length
			UInt16 iddLength = 0;
			pin_ptr<unsigned short> pIddLength = &iddLength;
			unsigned short* npIddLength = pIddLength;

			pin_ptr<unsigned int> pCurrentVolume = &currentVolume;
			unsigned int* npCurrentVolume = pCurrentVolume;
			int returnValue = m_materialMonitor->AuthenticateCartridge(cartridgeNum - 1, nullptr, npIdd, npIddLength, npCurrentVolume);
			if (returnValue == IDTLIB_SUCCESS)
			{
				idd = gcnew array<Byte>(iddLength);
				IntPtr ip = IntPtr((void*)npIdd);
				Marshal::Copy(ip, idd, 0, iddLength);
			}

			return returnValue;
		}
		catch (...)
		{
			return UNHANDLED_EXCEPTION;
		}
		finally
		{
			if (taken)
			{
				Monitor::Exit(m_syncObj);
			}
		}
	}

	///
	/// Verifies material consumption.
	//
	int MaterialMonitorWrapper::UpdateConsumption(Byte cartridgeNum, UInt32 comsumption, array<Byte>^% counterResponse, array<Byte>^% counterSignature)
	{
		bool taken = false;
		Monitor::Enter(m_syncObj, taken);
		try
		{
			counterResponse = nullptr;
			CounterResponse npCounterResponse;

			Boolean skipAuthentication = false;
			int returnValue = m_materialMonitor->UpdateConsumption(cartridgeNum - 1, comsumption, &npCounterResponse);
			if (returnValue == IDTLIB_SUCCESS)
			{
				int length = npCounterResponse.Encode(nullptr);
				unsigned char* buffer = new unsigned char[length];
				npCounterResponse.Encode(buffer);

				counterResponse = gcnew array<Byte>(RND_LENGTH);
				IntPtr ip = IntPtr((void*)buffer);
				Marshal::Copy(ip, counterResponse, 0, RND_LENGTH);

				counterSignature = gcnew array<Byte>(length - RND_LENGTH);
				ip = IntPtr((void*)(buffer + RND_LENGTH));
				Marshal::Copy(ip, counterSignature, 0, length - RND_LENGTH);

				delete[] buffer;
			}

			return returnValue;
		}
		catch (...)
		{
			return UNHANDLED_EXCEPTION;
		}
		finally
		{
			if (taken)
			{
				Monitor::Exit(m_syncObj);
			}
		}
	}

	int MaterialMonitorWrapper::GetDeviceSerialNumber(Byte cartridgeNum, array<Byte>^% serialNumber)
	{
		bool taken = false;
		Monitor::Enter(m_syncObj, taken);
		try
		{
			serialNumber = nullptr;
			unsigned char npSerialNumber[32]; // maximal length of serial number
			unsigned char usSerialNumberLength = 0;

			int returnValue = m_materialMonitor->GetDeviceSerialNumber(cartridgeNum - 1, npSerialNumber, &usSerialNumberLength);
			if (returnValue == IDTLIB_SUCCESS)
			{
				serialNumber = gcnew array<Byte>(usSerialNumberLength);
				IntPtr ip = IntPtr((void*)npSerialNumber);
				Marshal::Copy(ip, serialNumber, 0, usSerialNumberLength);
			}

			return returnValue;
		}
		catch (...)
		{
			return UNHANDLED_EXCEPTION;
		}
		finally
		{
			if (taken)
			{
				Monitor::Exit(m_syncObj);
			}
		}
	}

	int MaterialMonitorWrapper::ReadTagPublicKey(Byte cartridgeNum, array<Byte>^% pubKC)
	{
		bool taken = false;
		Monitor::Enter(m_syncObj, taken);
		try
		{
			pubKC = nullptr;
			unsigned char npPubKC[PUBLIC_KEY_SIZE];
			unsigned short usPubKCLength = 0;

			int returnValue = m_materialMonitor->ReadTagPublicKey(cartridgeNum - 1, npPubKC, &usPubKCLength);
			if (returnValue == IDTLIB_SUCCESS)
			{
				pubKC = gcnew array<Byte>(usPubKCLength);
				IntPtr ip = IntPtr((void*)npPubKC);
				Marshal::Copy(ip, pubKC, 0, usPubKCLength);
			}

			return returnValue;
		}
		catch (...)
		{
			return UNHANDLED_EXCEPTION;
		}
		finally
		{
			if (taken)
			{
				Monitor::Exit(m_syncObj);
			}
		}
	}

	void MaterialMonitorWrapper::SetLogMessageMethod(Action<String^>^ logMessageMethod)
	{
		s_logMessageMethod = logMessageMethod;
		if (s_logMessageDel == nullptr)
		{
			s_logMessageDel = gcnew LogMessageDel(LogMessage);
			IntPtr ip = Marshal::GetFunctionPointerForDelegate(s_logMessageDel);
			pfnLogMessage s_logMessageFunction = (pfnLogMessage)ip.ToPointer();
			SetLogMessageFunction(s_logMessageFunction);
		}
	}

	void MaterialMonitorWrapper::LogMessage(const char* message)
	{
		if (s_logMessageMethod != nullptr)
		{
			IntPtr ip = IntPtr((void *)message);
			String^ messageStr = Marshal::PtrToStringAnsi(ip);
			s_logMessageMethod(messageStr);
		}
	}
}
