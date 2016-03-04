#include <windows.h>

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

unsigned __int64 Initialize(DWORD slot, const wchar_t password[], BOOL rsa, DWORD spec, BYTE hash);
unsigned __int64 GenerateKey(DWORD slot, const wchar_t password[], BOOL rsa, DWORD spec);
unsigned __int64 SignData(PBYTE data, PBYTE* signature, LPDWORD size);
unsigned __int64 GetPublicKey(PBYTE* key, LPDWORD size);
unsigned __int64 VerifySignature(PBYTE data, DWORD data_size, PBYTE signature, DWORD sign_size);
DWORD GetKeySpec();
BYTE GetHashAlg();
char* GetError();
void FreeMemory(void* mem);
void Uninitialize();