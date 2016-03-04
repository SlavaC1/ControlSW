// HsmClientWrapper.h

#pragma once

using namespace System;

namespace BCS
{
	namespace HsmClient {

#define RsaKey		0
#define Prime256	1
#define Prime384	2
#define Prime521	3
#define Binary163	4
#define Binary283	5

#define HA_SHA1		1
#define HA_SHA256	2
#define HA_SHA384	3
#define HA_SHA512	4

		public ref class HsmClientWrapper
		{
		public:
			~HsmClientWrapper();
			!HsmClientWrapper();
			static HsmClientWrapper^ GetInstance();

			bool LoadHsmLibrary(String^ path);
			bool IsInitialized();
			unsigned __int64 Initialize(bool useSimulator, int slot, String^ password);
			unsigned __int64 GetPublicKey(array<Byte>^% publicKey);
			unsigned __int64 SignData(array<Byte>^ message, array<Byte>^% signature);
			unsigned __int64 VerifySignature(array<Byte>^ message, array<Byte>^ signature);
			unsigned __int64 VerifySignature(array<Byte>^ publicKey, array<Byte>^ message, array<Byte>^ signature);
			String^ GetError();

		private:
			HsmClientWrapper();

		private:
			static HsmClientWrapper^ m_instance;

			HMODULE m_hMod;
			bool m_initialized;
			String^ m_error;
			Object^ m_syncObj;
		};
	}
}