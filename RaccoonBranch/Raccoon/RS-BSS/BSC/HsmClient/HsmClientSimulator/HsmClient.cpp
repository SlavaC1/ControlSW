#include "stdafx.h"
#include "HsmClient.h"
#include "osrng.h"
#include "files.h"
#include "eccrypto.h"
#include "oids.h"

#include <string>

using namespace std;
using namespace CryptoPP;

static bool bLoggedIn;
static ECDSA<EC2N, SHA256>::PrivateKey prvKS;
static ECDSA<EC2N, SHA256>::PublicKey pubKS;
static string sError;

unsigned __int64 Initialize(DWORD slot, const wchar_t password[], BOOL rsa, DWORD spec, BYTE hash)
{
	if (!bLoggedIn)
	{
		wstring acPrvKeyFileName, acPubKeyFileName;
		try
		{
			acPrvKeyFileName = wstring(password, wcslen(password));
			acPrvKeyFileName.append(L"\\prvKS.key");
			prvKS.Load(FileSource(acPrvKeyFileName.c_str(), true).Ref());

			acPubKeyFileName = wstring(password, wcslen(password));
			acPubKeyFileName.append(L"\\pubKS.key");
			pubKS.Load(FileSource(acPubKeyFileName.c_str(), true).Ref());

			bLoggedIn = true;
		}
		catch (exception& ex)
		{
			sError = ex.what();
			cerr << ex.what() << endl;
		}
	}

	return bLoggedIn ? 0 : 1;
}

unsigned __int64 GetPublicKey(PBYTE* key, LPDWORD size)
{
	if (!bLoggedIn)
	{
		sError = "HSM client was not initialized.";
		return 1;
	}

	try
	{
		EC2NPoint Q = pubKS.GetPublicElement();
		BYTE* tmp = new BYTE[Q.x.ByteCount() + Q.y.ByteCount()];
		*size = 0;
		Q.x.Encode(tmp + *size, Q.x.ByteCount());
		*size += Q.x.ByteCount();
		Q.y.Encode(tmp + *size, Q.y.ByteCount());
		*size += Q.x.ByteCount();
		*key = tmp;
		return 0;
	}
	catch (exception& ex)
	{
		sError = ex.what();
		return 2;
	}
}

unsigned __int64 SignData(PBYTE data, PBYTE* signature, LPDWORD size)
{
	if (!bLoggedIn)
	{
		sError = "HSM client was not initialized.";
		return 1;
	}

	try
	{
		int err;

		AutoSeededRandomPool prng;
		ECDSA<EC2N, SHA256>::Signer signer(prvKS);
		byte* abSignature = new byte[signer.MaxSignatureLength()];
		memset(abSignature, 0x00, signer.MaxSignatureLength());

		size_t szSigLen = signer.SignMessage(prng, data, *size, abSignature);
		if (szSigLen == 0)
		{
			sError = "Error signing data.";
			err = 2;
		}
		else
		{
			BYTE* tmp = new BYTE[szSigLen];
			memcpy(tmp, abSignature, szSigLen);
			*signature = tmp;
			*size = (DWORD)szSigLen;
			err = 0;
		}

		delete[] abSignature;
		return err;
	}
	catch (exception& ex)
	{
		sError = ex.what();
		return 3;
	}
}

unsigned __int64 VerifySignature(PBYTE data, DWORD data_size, PBYTE signature, DWORD sign_size)
{
	if (!bLoggedIn)
	{
		sError = "HSM client was not initialized.";
		return 1;
	}

	try
	{
		ECDSA<EC2N, SHA256>::Verifier verifier(pubKS);
		bool ok = verifier.VerifyMessage(data, data_size, signature, sign_size);
		if (!ok)
		{
			sError = "Invalid signature.";
			return 2;
		}

		return 0;
	}
	catch (exception& ex)
	{
		sError = ex.what();
		return 3;
	}
}

unsigned __int64 VerifySignatureWithPublicKey(PBYTE public_key, DWORD public_key_size, PBYTE data, DWORD data_size, PBYTE signature, DWORD sign_size)
{
	if (public_key == NULL)
	{
		return VerifySignature(data, data_size, signature, sign_size);
	}

	// no need to check login

	try
	{
		EC2NPoint Q;
		Q.identity = false;
		Q.x.Decode(public_key, public_key_size / 2);
		Q.y.Decode(public_key + public_key_size / 2, public_key_size / 2);

		ECDSA<EC2N, SHA256>::PublicKey publicKey;
		publicKey.Initialize(ASN1::sect283k1(), Q);		
		ECDSA<EC2N, SHA256>::Verifier verifier(publicKey);
		bool ok = verifier.VerifyMessage(data, data_size, signature, sign_size);
		if (!ok)
		{
			sError = "Invalid signature.";
			return 2;
		}

		return 0;
	}
	catch (exception& ex)
	{
		sError = ex.what();
		return 3;
	}
}

char* GetError()
{
	char* pcError = new char[sError.length() + 1];
	memcpy(pcError, sError.c_str(), sError.length());
	pcError[sError.length()] = 0;
	return pcError;
}

void FreeMemory(void* mem)
{
	delete mem;
}

void Logout()
{
	bLoggedIn = false;
}