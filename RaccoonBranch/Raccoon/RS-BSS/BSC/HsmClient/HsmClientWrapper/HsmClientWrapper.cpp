// This is the main DLL file.

#include "stdafx.h"

#include "HsmClientWrapper.h"
#include "HsmClient.h"
#include <msclr\lock.h>

using namespace System::IO;
using namespace System::Runtime::InteropServices;

namespace BCS
{
	namespace HsmClient {
		typedef unsigned __int64 (*pfnInitialize)(DWORD slot, const wchar_t* password, BOOL rsa, DWORD spec, BYTE hash);
		typedef unsigned __int64 (*pfnGetPublicKey)(PBYTE* key, LPDWORD size);
		typedef unsigned __int64 (*pfnSignData)(PBYTE data, PBYTE* signature, LPDWORD size);
		typedef unsigned __int64 (*pfnVerifySignature)(PBYTE data, DWORD data_size, PBYTE signature, DWORD sign_size);
		typedef unsigned __int64 (*pfnGenerateKey)(DWORD slot, const wchar_t* password, BOOL rsa, DWORD spec);
		typedef PCHAR (*pfnGetError)();
		typedef void (*pfnFreeMemory)(void* mem);
		typedef void (*pfnUninitialize)();

		HsmClientWrapper::HsmClientWrapper()
		{
			m_initialized = false;
			m_syncObj = gcnew Object();
		}

		HsmClientWrapper::~HsmClientWrapper()
		{
			this->!HsmClientWrapper();
		}

		HsmClientWrapper::!HsmClientWrapper()
		{
			if (m_hMod != NULL)
			{
				pfnUninitialize Uninitialize = (pfnUninitialize)GetProcAddress(m_hMod, "Uninitialize");
				if (Uninitialize != NULL)
				{
					Uninitialize();
				}

				FreeLibrary(m_hMod);
			}
		}


		HsmClientWrapper^ HsmClientWrapper::GetInstance()
		{
			if (m_instance == nullptr)
			{
				m_instance = gcnew HsmClientWrapper();
			}

			return m_instance;
		}

		bool HsmClientWrapper::LoadHsmLibrary(String^ path)
		{
			m_hMod = nullptr;
			if (path == nullptr)
			{
				return false;
			}

			IntPtr pPath = IntPtr::Zero;

			try
			{
				pPath = Marshal::StringToHGlobalUni(path);
				wchar_t *npPath = static_cast<wchar_t*>(pPath.ToPointer());
				m_hMod = LoadLibrary(npPath);
			}
			catch (Exception^ ex)
			{
				m_error = ex->Message;
			}

			if (pPath != IntPtr::Zero)
			{
				Marshal::FreeHGlobal(pPath);
			}

			return m_hMod != nullptr;
		}

		bool HsmClientWrapper::IsInitialized()
		{
			return m_initialized;
		}

		unsigned __int64 HsmClientWrapper::Initialize(bool useSimulator, int slot, String^ password)
		{
			m_error = nullptr;
			if (password == nullptr)
			{
				m_error = "No password provided.";
				return 1;
			}

			if (m_hMod == NULL)
			{
				m_error = "HSM library was not loaded.";
				return 2;
			}

			if (useSimulator)
			{
				password = Path::GetFullPath(password);
			}

			//tmp - SHAI - GENERATE KEY. in order to switch between slots (dev & prod), shall be called
			// 1. Uninitialize
			//if (m_hMod != NULL)
			//{
			//	pfnUninitialize Uninitialize = (pfnUninitialize)GetProcAddress(m_hMod, "Uninitialize");
			//	if (Uninitialize == NULL)
			//	{
			//		m_error = "'UnInitialize' method was not found in HSM library.";
			//		return 3;
			//	}
			//	Uninitialize();
			//}
			// tmp - SHAI

			pfnInitialize Initialize = (pfnInitialize)GetProcAddress(m_hMod, "Initialize");
			if (Initialize == NULL)
			{
				m_error = "'Initialize' method was not found in HSM library.";
				return 3;
			}

			IntPtr pPassword = Marshal::StringToHGlobalUni(password);
			wchar_t *npPassword = static_cast<wchar_t*>(pPassword.ToPointer());

			unsigned __int64 err = Initialize(slot, npPassword, FALSE, Binary283, HA_SHA256);

			//tmp - SHAI - GENERATE KEY. shall be called independently of init\unInit. 2 keys were already generated: dev & prod.
			// 2. GenerateKey()
/*			if(err != 0)
			{
				m_error = "Failed to Init, " + GetError();
				return 3;
			}
			pfnGenerateKey GenerateKey = (pfnGenerateKey)GetProcAddress(m_hMod, "GenerateKey");
			if (GenerateKey == NULL)
			{
				m_error = "'GenerateKey' method was not found in HSM library.";
				return 3;
			}
			err = GenerateKey(slot, npPassword, FALSE, Binary283);	*/		
			//tmp - SHAI

			Marshal::FreeHGlobal(pPassword);
			m_initialized = (err == 0);
			if (!m_initialized)
			{
				m_error = GetError();
			}

			return err;
		}

		unsigned __int64 HsmClientWrapper::GetPublicKey([Out] array<Byte>^% publicKey)
		{
			m_error = nullptr;
			if (m_hMod == NULL)
			{
				m_error = "HSM library was not loaded.";
				return 1;
			}

			if (!m_initialized)
			{
				m_error = "HSM client was not m_initialized.";
				return 2;
			}

			pfnGetPublicKey GetPublicKey = (pfnGetPublicKey)GetProcAddress(m_hMod, "GetPublicKey");
			if (GetPublicKey == NULL)
			{
				m_error = "'GetPublicKey' method was not found in HSM library.";
				return 3;
			}

			msclr::lock l(m_syncObj);

			publicKey = nullptr;
			PBYTE npPublicKey;
			DWORD publicKeyLength;
			unsigned __int64 err = GetPublicKey(&npPublicKey, &publicKeyLength);
			if (err == 0)
			{
				publicKey = gcnew array<Byte>(publicKeyLength);
				IntPtr ip = IntPtr((void*)npPublicKey);
				Marshal::Copy(ip, publicKey, 0, publicKeyLength);
				pfnFreeMemory FreeMemory = (pfnFreeMemory)GetProcAddress(m_hMod, "FreeMemory");
				if (FreeMemory != NULL)
				{
					FreeMemory(npPublicKey);
				}
			}

			return err;
		}

		/*unsigned __int64 HsmClientWrapper::GenerateKey(int slot, const String^ password)
		{
		m_error = nullptr;
		if (m_hMod == NULL)
		{
		m_error = "HSM library was not loaded.";
		return 1;
		}

		if (!m_initialized)
		{
		m_error = "HSM client was not m_initialized.";
		return 2;
		}
		pfnGenerateKey GenerateKey = (pfnGenerateKey)GetProcAddress(m_hMod, "GenerateKey");
		if (GenerateKey == NULL)
		{
		m_error = "'GenerateKey' method was not found in HSM library.";
		return 3;
		}			

		unsigned __int64 err = GenerateKey(slot, password, false, 2048);
		return err;
		}*/

		unsigned __int64 HsmClientWrapper::SignData(array<Byte>^ message, [Out] array<Byte>^% signature)
		{
			m_error = nullptr;
			if (m_hMod == NULL)
			{
				m_error = "HSM library was not loaded.";
				return 1;
			}

			if (!m_initialized)
			{
				m_error = "HSM client was not m_initialized.";
				return 2;
			}

			pfnSignData SignData = (pfnSignData)GetProcAddress(m_hMod, "SignData");
			if (SignData == NULL)
			{
				m_error = "'SignData' method was not found in HSM library.";
				return 3;
			}

			msclr::lock l(m_syncObj);

			signature = nullptr;
			PBYTE npMessage = NULL;
			DWORD length = 0;
			if (message != nullptr && message->Length > 0)
			{
				pin_ptr<Byte> pMessage = &message[0];
				npMessage = pMessage;
				length = message->Length;
			}

			PBYTE npSignature;
			unsigned __int64 err = SignData(npMessage, &npSignature, &length);
			if (err == 0)
			{
				signature = gcnew array<Byte>(length);
				IntPtr ip = IntPtr((void*)npSignature);
				Marshal::Copy(ip, signature, 0, length);
				pfnFreeMemory FreeMemory = (pfnFreeMemory)GetProcAddress(m_hMod, "FreeMemory");
				if (FreeMemory != NULL)
				{
					FreeMemory(npSignature);
				}
			}

			return err;
		}

		unsigned __int64 HsmClientWrapper::VerifySignature(array<Byte>^ message, array<Byte>^ signature)
		{
			m_error = nullptr;
			if (m_hMod == NULL)
			{
				m_error = "HSM library was not loaded.";
				return 1;
			}

			if (!m_initialized)
			{
				m_error = "HSM client was not m_initialized.";
				return 2;
			}

			pfnVerifySignature VerifySignature = (pfnVerifySignature)GetProcAddress(m_hMod, "VerifySignature");
			if (VerifySignature == NULL)
			{
				m_error = "'VerifySignature' method was not found in HSM library.";
				return 3;
			}

			msclr::lock l(m_syncObj);

			PBYTE npMessage = NULL;
			DWORD messageLength = 0;
			if (message != nullptr && message->Length > 0)
			{
				pin_ptr<Byte> pMessage = &message[0];
				npMessage = pMessage;
				messageLength = message->Length;
			}

			PBYTE npSignature = NULL;
			DWORD signatureLength = 0;
			if (signature != nullptr && signature->Length > 0)
			{
				pin_ptr<Byte> pSignature = &signature[0];
				npSignature = pSignature;
				signatureLength = signature->Length;
			}

			unsigned __int64 err = VerifySignature(npMessage, messageLength, npSignature, signatureLength);
			if (err != 0)
			{
				m_error = "Invalid signature";
			}

			return err;
		}

		unsigned __int64 HsmClientWrapper::VerifySignature(array<Byte>^ publicKey, array<Byte>^ message, array<Byte>^ signature)
		{
			m_error = nullptr;

			if (publicKey == nullptr)
			{
				m_error = "Public key not provided";
				return 11;
			}

			msclr::lock l(m_syncObj);

			PBYTE npPublicKey = NULL;
			DWORD publicKeyLength = 0;
			if (publicKey != nullptr && publicKey->Length > 0)
			{
				pin_ptr<Byte> pPublicKey = &publicKey[0];
				npPublicKey = pPublicKey;
				publicKeyLength = publicKey->Length;
			}

			PBYTE npMessage = NULL;
			DWORD messageLength = 0;
			if (message != nullptr && message->Length > 0)
			{
				pin_ptr<Byte> pMessage = &message[0];
				npMessage = pMessage;
				messageLength = message->Length;
			}

			PBYTE npSignature = NULL;
			DWORD signatureLength = 0;
			if (signature != nullptr && signature->Length > 0)
			{
				pin_ptr<Byte> pSignature = &signature[0];
				npSignature = pSignature;
				signatureLength = signature->Length;
			}

			unsigned __int64 err = VerifySignatureWithPublicKey(npPublicKey, publicKeyLength, npMessage, messageLength, npSignature, signatureLength);
			if (err != 0)
			{
				m_error = "Invalid signature for given public key";
			}

			return err;
		}

		String^ HsmClientWrapper::GetError()
		{
			pfnGetError GetError = (pfnGetError)GetProcAddress(m_hMod, "GetError");
			if (GetError == NULL)
			{
				return nullptr;
			}

			if (m_error != nullptr)
			{
				return m_error;
			}

			char* hsmError = GetError();
			String^ s = gcnew String(hsmError);

			pfnFreeMemory FreeMemory = (pfnFreeMemory)GetProcAddress(m_hMod, "FreeMemory");
			if (FreeMemory != NULL)
			{
				FreeMemory(hsmError);
			}

			return s;
		}
	}
}