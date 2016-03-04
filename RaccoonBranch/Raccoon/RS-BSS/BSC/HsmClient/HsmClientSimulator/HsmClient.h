#pragma once

#if defined(WIN32) || defined(_WIN32)
#	ifdef _LIB
#		define HSMCLIENTSIMULATOR_LIB
#	elif HSMCLIENTSIMULATOR_EXPORTS
#       define HSMCLIENTSIMULATOR_LIB __declspec(dllexport)
#   elif defined(HSMCLIENTSIMULATOR_IMPORT)
#       define HSMCLIENTSIMULATOR_LIB __declspec(dllimport)
#   else
#       define HSMCLIENTSIMULATOR_LIB
#   endif
#else
#   define HSMCLIENTSIMULATOR_LIB
#endif

#ifdef __cplusplus
extern "C" {
#endif

	unsigned __int64 HSMCLIENTSIMULATOR_LIB Initialize(DWORD slot, const wchar_t password[], BOOL rsa, DWORD spec, BYTE hash);

	unsigned __int64 HSMCLIENTSIMULATOR_LIB GetPublicKey(PBYTE* key, LPDWORD size);

	unsigned __int64 HSMCLIENTSIMULATOR_LIB SignData(PBYTE data, PBYTE* signature, LPDWORD size);

	unsigned __int64 HSMCLIENTSIMULATOR_LIB VerifySignature(PBYTE data, DWORD data_size, PBYTE signature, DWORD sign_size);

	unsigned __int64 HSMCLIENTSIMULATOR_LIB VerifySignatureWithPublicKey(PBYTE publicKey, DWORD publicKeySize, PBYTE data, DWORD data_size, PBYTE signature, DWORD sign_size);

	PCHAR HSMCLIENTSIMULATOR_LIB GetError();

	void HSMCLIENTSIMULATOR_LIB FreeMemory(void* mem);

	void HSMCLIENTSIMULATOR_LIB Uninitialize();

#ifdef __cplusplus
}
#endif
