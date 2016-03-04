#pragma once

#include <stdlib.h>
#include <windows.h>
#include <tchar.h>
#include "stdafx.h"
#include "aes.h"
#include "eccrypto.h"

using namespace CryptoPP;
using namespace std;

extern "C" {

namespace CertGen
{
	class __declspec(dllexport) CertGenerator
	{
	public:
		CertGenerator(void);
		~CertGenerator(void);
		void GenerateCertificate(const unsigned char *idd, unsigned short iddLength);
		void TestCrypto();
		void Test2();
		void SafeNetPOC();
		void ValidateSafeNetPOC();

	private:
		void GenerateAndSaveKeyPair(const OID& oid, ECDSA<EC2N, SHA256>::PrivateKey& privateKey, ECDSA<EC2N, SHA256>::PublicKey& publicKey, const string& prvFileName, const string& pubFileName);
		void GenerateMessage(const unsigned char *idd, unsigned short iddLength, const ECDSA<EC2N, SHA256>::PublicKey& publicKey);

		bool GeneratePrivateKey( const OID& oid, ECDSA<EC2N, SHA256>::PrivateKey& key );
		bool GeneratePublicKey( const ECDSA<EC2N, SHA256>::PrivateKey& privateKey, ECDSA<EC2N, SHA256>::PublicKey& publicKey );
		void SavePrivateKey( const string& filename, const ECDSA<EC2N, SHA256>::PrivateKey& key );
		void SavePublicKey( const string& filename, const ECDSA<EC2N, SHA256>::PublicKey& key );
		void LoadPrivateKey( const string& filename, ECDSA<EC2N, SHA256>::PrivateKey& key );
		void LoadPublicKey( const string& filename, ECDSA<EC2N, SHA256>::PublicKey& key );

		void PrintDomainParameters( const ECDSA<EC2N, SHA256>::PrivateKey& key );
		void PrintDomainParameters( const ECDSA<EC2N, SHA256>::PublicKey& key );
		void PrintDomainParameters( const DL_GroupParameters_EC<EC2N>& params );
		void PrintPrivateKey( const ECDSA<EC2N, SHA256>::PrivateKey& key );
		void PrintPublicKey( const ECDSA<EC2N, SHA256>::PublicKey& key );
		void PrintHex( const byte* data, size_t length );

		bool SignMessage( const ECDSA<EC2N, SHA256>::PrivateKey& key, const string& message, string& signature );
		bool SignMessage( const ECDSA<EC2N, SHA256>::PrivateKey& key, const byte* message, size_t messageLength, byte* signature );
		bool VerifyMessage( const ECDSA<EC2N, SHA256>::PublicKey& key, const string& message, const string& signature );
		bool VerifyMessage( const ECDSA<EC2N, SHA256>::PublicKey& key, const byte* message, size_t messageLength, const byte* signature, size_t signatureLength );

		void SaveByteArrayAsText(const string& filename, const byte *ba, unsigned short length);
	};
}}