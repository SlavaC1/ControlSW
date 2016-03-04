// RaccoonCLI.h

#pragma once

#pragma unmanaged
#include "MaterialMonitor.h"
#pragma managed

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace StratasysIDTSrv;

namespace RaccoonCLI {

	delegate void LogMessageDel(const char* message);

	public ref class MaterialMonitorWrapper
	{
	public:
		MaterialMonitorWrapper();
		~MaterialMonitorWrapper();

		int InitHW([Out] Byte% cartridgesCount);
		int SetPubKS(array<Byte>^ pubKS);
		int GetInPlaceStatus([Out] Int32% status);
		int BurnSignedIdd(Byte cartridgeNum, array<Byte>^ materialInfo, array<Byte>^ signature);
		///
		/// Removes cartridge.
		///
		int RemoveCartridge(Byte cartridgeNum);
		///
		/// Authenticates cartridge.
		///
		int AuthenticateCartridge(Byte cartridgeNum, [Out] array<Byte>^% idd, [Out] UInt32% currentVolume);
		///
		/// Verifies material consumption.
		//
		int UpdateConsumption(Byte cartridgeNum, UInt32 comsumption, [Out] array<Byte>^% counterResponse, [Out] array<Byte>^% counterSignature);

		int GetDeviceSerialNumber(Byte cartridgeNum, [Out] array<Byte>^% serialNumber);

		int ResetCartridge(Byte cartridgeNum, array<Byte>^ password);

		int ReadTagPublicKey(Byte cartridgeNum, [Out] array<byte>^% pubKC);

		static void SetLogMessageMethod(Action<String^>^ logMessageMethod);

	private:
		static void LogMessage(const char* message);

	private:
		MaterialMonitor* m_materialMonitor;
		Object^ m_syncObj;
		static Action<String^>^ s_logMessageMethod;
		static LogMessageDel^ s_logMessageDel;
	};
}
