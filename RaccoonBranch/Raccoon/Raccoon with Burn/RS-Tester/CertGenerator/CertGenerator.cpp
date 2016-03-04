#include "stdafx.h"
#include "CertGenerator.h"

#include <assert.h>
#include <iostream>
#include <string>
#include <time.h>
#include <direct.h>
#include <iomanip>

#include "osrng.h"
#include "aes.h"
#include "integer.h"
#include "sha.h"
#include "filters.h"
#include "files.h"
#include "eccrypto.h"
#include "oids.h"

#include "log4cplus/configurator.h"
#include "log4cplus/loggingmacros.h"

using namespace CryptoPP;
using namespace std;
using namespace log4cplus;

namespace CertGen
{
	CertGenerator::CertGenerator(void)
	{
		PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplusRaccoon.prop"));
	}

	CertGenerator::~CertGenerator(void)
	{
	}

	void CertGenerator::GenerateCertificate(const unsigned char *idd, unsigned short iddLength)
	{
		cout << "Creating files...\n";
		tchar cCurrentPath[FILENAME_MAX];
		getcwd(cCurrentPath, FILENAME_MAX);
		string folderName(cCurrentPath);
		memset(cCurrentPath, 0, sizeof(cCurrentPath));

		folderName.append("\\Cert");

		time_t now = time(NULL);
		struct tm nowTm;
		localtime_s(&nowTm, &now);
		_tcsftime(cCurrentPath, FILENAME_MAX, "_%Y.%m.%d_%H.%M.%S", &nowTm);
		folderName.append(cCurrentPath);
		_tmkdir(folderName.c_str());
		_tchdir(folderName.c_str());

		string idcFileName("idd.bin");
		ofstream idcFile(idcFileName, ios::binary);
		idcFile.write((char *)idd, iddLength);
		SaveByteArrayAsText("idd.bin", idd, iddLength);

		ECDSA<EC2N, SHA256>::PrivateKey prvKS;
		ECDSA<EC2N, SHA256>::PublicKey pubKS;
		string prvKSFileName("prvKS.key");
		string pubKSFileName("pubKS.key");
		cout << "Generating pair of keys...\n";
		GenerateAndSaveKeyPair(ASN1::sect163k1(), prvKS, pubKS, prvKSFileName, pubKSFileName);

		idcFile.close();
	}

	void CertGenerator::TestCrypto()
	{
		Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("Raccoon"));

		ECDSA<EC2N, SHA256>::PrivateKey prvKS;
		ECDSA<EC2N, SHA256>::PublicKey pubKS;

		//LOG4CPLUS_INFO(logger, "Generating private and public key pair...");
		//GenerateAndSaveKeyPair(ASN1::sect163k1(), prvKS, pubKS, "prvKS.key", "pubKS.key");
		LOG4CPLUS_INFO(logger, "Loading private and public key pair...");
		LoadPrivateKey("prvKS.key", prvKS);
		LoadPublicKey("pubKS.key", pubKS);
		PrintPublicKey(pubKS);
		AutoSeededRandomPool prng;
		bool ok = pubKS.Validate(prng, 3);

		byte message[] = 
		{
			0x01, 0xc9, 0xad, 0x03, 0x82, 0xcd, 0xf8, 0x05, 0x5b, 0x07, 0xff, 0xdd, 0xd3, 0x71, 0x3e, 0xd6,
			0x1c, 0x1b, 0x2c, 0x9d, 0xb7, 0x04, 0x23, 0x14, 0x6d, 0x3c, 0x98, 0x54, 0xe2, 0xad, 0xcf, 0xe0,
			0xdd, 0xd8, 0xc7, 0x4e, 0x0b, 0x41, 0x2e, 0xd7, 0x57, 0xd1,
		};

		byte signature[42];
		LOG4CPLUS_INFO(logger, "Signing message...");
		ok = SignMessage(prvKS, message, sizeof(message), signature);
		LOG4CPLUS_INFO(logger, "Verifying message...");
		ok &= VerifyMessage(pubKS, message, sizeof(message), signature, sizeof(signature));
		if (ok)
		{
			LOG4CPLUS_INFO(logger, "Message verified successfully.");
		}
		else
		{
			LOG4CPLUS_ERROR(logger, "Error verifying message.");
		}
	}

	void CertGenerator::Test2()
	{
		unsigned char device_public_key[42] = {
			0x01 ,0xc9 ,0xad ,0x03 ,0x82 ,0xcd ,0xf8 ,0x05 ,0x5b ,0x07 ,0xff, 0xdd ,0xd3 ,0x71 ,0x3e ,0xd6
			,0x1c ,0x1b ,0x2c ,0x9d ,0xb7 ,0x04 ,0x23 ,0x14 ,0x6d ,0x3c ,0x98 ,0x54 ,0xe2 ,0xad ,0xcf ,0xe0
			,0xdd ,0xd8 ,0xc7 ,0x4e ,0x0b ,0x41 ,0x2e ,0xd7 ,0x57 ,0xd1};

		byte digest[32];
		SHA256().CalculateDigest(digest, device_public_key, 42);
		PrintHex(digest, 32);
	}

	void CertGenerator::SafeNetPOC()
	{
		Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("Raccoon"));

		ECDSA<EC2N, SHA256>::PrivateKey prvKS;
		ECDSA<EC2N, SHA256>::PublicKey pubKS;

		//LOG4CPLUS_INFO(logger, "Generating private and public key pair...");
		//GenerateAndSaveKeyPair(ASN1::sect163k1(), prvKS, pubKS, "prvKS.key", "pubKS.key");
		LOG4CPLUS_INFO(logger, "Loading private and public key pair...");
		LoadPrivateKey("prvKS.key", prvKS);
		LoadPublicKey("pubKS.key", pubKS);
		AutoSeededRandomPool prng;
		bool ok = pubKS.Validate(prng, 3);
		if (!ok)
		{
			LOG4CPLUS_ERROR(logger, "Invalid public key.");
			return;
		}
		EC2NPoint Q = pubKS.GetPublicElement();
		int keyLength = Q.x.ByteCount() + Q.y.ByteCount();
		byte* QBuf = new byte[keyLength];
		Q.x.Encode(QBuf, Q.x.ByteCount());
		Q.y.Encode(QBuf + Q.x.ByteCount(), Q.y.ByteCount());
		PrintHex(QBuf, keyLength);
		delete[] QBuf;

		LOG4CPLUS_INFO(logger, "Generating cartridge key pair...");
		ECDSA<EC2N, SHA256>::PrivateKey prvKC;
		ECDSA<EC2N, SHA256>::PublicKey pubKC;
		GeneratePrivateKey(ASN1::sect163k1(), prvKC);
		GeneratePublicKey(prvKC, pubKC);
		Q = pubKC.GetPublicElement();
		QBuf = new byte[keyLength];
		Q.x.Encode(QBuf, Q.x.ByteCount());
		Q.y.Encode(QBuf + Q.x.ByteCount(), Q.y.ByteCount());
		PrintHex(QBuf, keyLength);

		LOG4CPLUS_INFO(logger, "Signing cartridge public key...");
		byte signedPubKC[42];
		ok = SignMessage(prvKS, QBuf, keyLength, signedPubKC);
		if (!ok)
		{
			LOG4CPLUS_ERROR(logger, "Error signing.");
			delete[] QBuf;
			return;
		}

		PrintHex(signedPubKC, sizeof(signedPubKC));
		ok = VerifyMessage(pubKS, QBuf, keyLength, signedPubKC, 42);
		delete[] QBuf;

		LOG4CPLUS_INFO(logger, "Generating challenge..");
		byte challenge[32];
		prng.GenerateBlock(challenge, sizeof(challenge));
		PrintHex(challenge, sizeof(challenge));

		LOG4CPLUS_INFO(logger, "Signing challenge by cartridge private key...");
		byte signedChallenge[42];
		ok = SignMessage(prvKC, challenge, sizeof(challenge), signedChallenge);
		if (!ok)
		{
			LOG4CPLUS_ERROR(logger, "Error signing.");
			return;
		}

		ok = VerifyMessage(pubKS, challenge, 32, signedChallenge, 42);
		PrintHex(signedChallenge, sizeof(signedChallenge));
	}

	void CertGenerator::ValidateSafeNetPOC()
	{
		//byte pubKSEl[] = { 0x04, 0x3d, 0xe1, 0x71, 0xb3, 0xa4, 0xfd, 0xb4, 0xe0, 0x50, 0x73, 0xb7, 0x09, 0xcd, 0x7f, 0x83, 0x02, 0x63, 0x1e, 0x46, 0xd0, 0x07, 0x88, 0xc1, 0x1d, 0x9f, 0x4a, 0x41, 0x93, 0x9b, 0xe2, 0x38, 0xe6, 0x99, 0x04, 0xd7, 0x85, 0xba, 0x8e, 0x42, 0x7f, 0x97 };
		//byte pubKCEl[] = { 0x03, 0x66, 0xa1, 0x17, 0xf4, 0x17, 0x31, 0x85, 0x7a, 0x71, 0xdd, 0x1e, 0x84, 0xfa, 0xba, 0x5f, 0xe5, 0x3e, 0x66, 0x26, 0x42, 0x06, 0x41, 0x5c, 0xbe, 0x1e, 0x82, 0xb0, 0x1c, 0x41, 0x70, 0x71, 0x94, 0x83, 0x7c, 0x03, 0x82, 0xd2, 0x8b, 0xa3, 0x48, 0xa2 };
		//byte pubKCSignedByPrvKS[] = { 0x02, 0xa5, 0x68, 0x7c, 0x16, 0x84, 0x11, 0x92, 0x2b, 0x02, 0x79, 0x63, 0xd0, 0x73, 0xad, 0xd3, 0xf4, 0x03, 0xac, 0xca, 0xc9, 0x03, 0x69, 0xa6, 0xd2, 0x5e, 0xcb, 0xfc, 0x4e, 0x8b, 0xaf, 0x61, 0x8e, 0x13, 0x3a, 0x11, 0xf5, 0xaf, 0x5f, 0xb7, 0xd5, 0x67 };
		//byte challengeSignedByPrvKC[] = { 0x01, 0xf1, 0xb8, 0xf3, 0x90, 0xdd, 0xc6, 0x72, 0x67, 0xd2, 0x59, 0x5a, 0x30, 0x00, 0x82, 0x99, 0x21, 0x0a, 0xec, 0xb7, 0x44, 0x03, 0x6c, 0x99, 0x4d, 0xb4, 0xea, 0xe2, 0xe6, 0x34, 0xb3, 0x7a, 0x68, 0x52, 0x97, 0x90, 0xa0, 0xac, 0xce, 0x80, 0x82, 0x94 };
		//byte challenge[] = { 0x1b, 0xc2, 0xab, 0xfa, 0x25, 0x59, 0x74, 0xa1, 0x64, 0x9a, 0x79, 0x5d, 0x13, 0x13, 0xc7, 0x95, 0x46, 0x71, 0x38, 0x47, 0xe1, 0x8b, 0x7f, 0x78, 0x13, 0xe8, 0x2e, 0x77, 0x37, 0x9f, 0x52, 0xc9 };

		byte pubKSEl[] = { 0x04, 0x3d, 0xe1, 0x71, 0xb3, 0xa4, 0xfd, 0xb4, 0xe0, 0x50, 0x73, 0xb7, 0x09, 0xcd, 0x7f, 0x83, 0x02, 0x63, 0x1e, 0x46, 0xd0, 0x07, 0x88, 0xc1, 0x1d, 0x9f, 0x4a, 0x41, 0x93, 0x9b, 0xe2, 0x38, 0xe6, 0x99, 0x04, 0xd7, 0x85, 0xba, 0x8e, 0x42, 0x7f, 0x97 };
		byte pubKCEl[] = { 0x01, 0xc9, 0xad, 0x03, 0x82, 0xcd, 0xf8, 0x05, 0x5b, 0x07, 0xff, 0xdd, 0xd3, 0x71, 0x3e, 0xd6, 0x1c, 0x1b, 0x2c, 0x9d, 0xb7, 0x04, 0x23, 0x14, 0x6d, 0x3c, 0x98, 0x54, 0xe2, 0xad, 0xcf, 0xe0, 0xdd, 0xd8, 0xc7, 0x4e, 0x0b, 0x41, 0x2e, 0xd7, 0x57, 0xd1 };
		byte pubKCSignedByPrvKS[] = { 0x03, 0xa4, 0xb3, 0x30, 0x32, 0xef, 0x75, 0xdd, 0x60, 0xb9, 0x1a, 0x71, 0x08, 0x50, 0xe0, 0xd3, 0xf8, 0x6f, 0xa2, 0x92, 0xde, 0x01, 0x67, 0x2b, 0x52, 0xfb, 0x8f, 0x7a, 0x74, 0x5c, 0x1a, 0x26, 0x1b, 0xa1, 0x99, 0xb4, 0x58, 0xf7, 0x5e, 0x8e, 0x42, 0x9d };
		byte challengeSignedByPrvKC[] = { 0x01, 0x3c, 0x18, 0x81, 0xf1, 0x88, 0xec, 0xc1, 0x7d, 0xf5, 0x8f, 0x20, 0xfb, 0xc4, 0x58, 0xb9, 0x60, 0x6d, 0x68, 0x1d, 0x15, 0x01, 0x08, 0xd8, 0x06, 0x9d, 0xe5, 0x07, 0xe3, 0x31, 0x6e, 0x51, 0xc5, 0xa7, 0xc8, 0xd9, 0xdf, 0x56, 0xf8, 0x42, 0x25, 0xd1 };
		byte challenge[] = { 0xd9, 0x30, 0x8c, 0xf0, 0xe8, 0xc1, 0x39, 0x73, 0xe3, 0x7e, 0x7e, 0xa5, 0xe4, 0x4c, 0x6c, 0x67, 0x53, 0x74, 0x72, 0x61, 0x74, 0x61, 0x73, 0x79, 0x73, 0x50, 0x72, 0x69, 0x6e, 0x74, 0x65, 0x72 };

		ECDSA<EC2N, SHA1>::PublicKey pubKS;
		EC2NPoint QS;
		QS.identity = false;
		QS.x.Decode(pubKSEl, sizeof(pubKSEl) / 2);
		QS.y.Decode(pubKSEl + sizeof(pubKSEl) / 2, sizeof(pubKSEl) / 2);
		pubKS.Initialize(ASN1::sect163k1(), QS);
		bool ok = VerifyMessage(pubKS, pubKCEl, sizeof(pubKCEl), pubKCSignedByPrvKS, sizeof(pubKCSignedByPrvKS));

		ECDSA<EC2N, SHA1>::PublicKey pubKC;
		EC2NPoint QC;
		QC.identity = false;
		QC.x.Decode(pubKCEl, sizeof(pubKCEl) / 2);
		QC.y.Decode(pubKCEl + sizeof(pubKCEl) / 2, sizeof(pubKCEl) / 2);
		pubKC.Initialize(ASN1::sect163k1(), QC);
		ECDSA<EC2N,SHA1>::Verifier verifyer2(pubKC);
		ok = VerifyMessage(pubKC, challenge, sizeof(challenge), challengeSignedByPrvKC, sizeof(challengeSignedByPrvKC));
	}

	void CertGenerator::GenerateAndSaveKeyPair(const OID& oid, ECDSA<EC2N, SHA256>::PrivateKey& privateKey, ECDSA<EC2N, SHA256>::PublicKey& publicKey, const string& prvFileName, const string& pubFileName)
	{
		GeneratePrivateKey(oid, privateKey);
		PrintPrivateKey(privateKey);
		GeneratePublicKey(privateKey, publicKey);
		PrintPublicKey(publicKey);
		SavePrivateKey(prvFileName, privateKey);
		SavePublicKey(pubFileName, publicKey);
	}

	bool CertGenerator::GeneratePrivateKey( const OID& oid, ECDSA<EC2N, SHA256>::PrivateKey& key )
	{
		AutoSeededRandomPool prng;

		key.Initialize( prng, oid );
		assert( key.Validate( prng, 3 ) );

		return key.Validate( prng, 3 );
	}

	bool CertGenerator::GeneratePublicKey( const ECDSA<EC2N, SHA256>::PrivateKey& privateKey, ECDSA<EC2N, SHA256>::PublicKey& publicKey )
	{
		AutoSeededRandomPool prng;

		// Sanity check
		assert( privateKey.Validate( prng, 3 ) );

		privateKey.MakePublicKey(publicKey);
		assert( publicKey.Validate( prng, 3 ) );

		return publicKey.Validate( prng, 3 );
	}

	void CertGenerator::PrintDomainParameters( const ECDSA<EC2N, SHA256>::PrivateKey& key )
	{
		PrintDomainParameters( key.GetGroupParameters() );
	}

	void CertGenerator::PrintDomainParameters( const ECDSA<EC2N, SHA256>::PublicKey& key )
	{
		PrintDomainParameters( key.GetGroupParameters() );
	}

	void CertGenerator::PrintDomainParameters( const DL_GroupParameters_EC<EC2N>& params )
	{
		cout << endl;

		cout << "Modulus:" << endl;
		cout << " " << std::hex << params.GetCurve().GetField().GetModulus() << endl;

		cout << "Coefficient A:" << endl;
		cout << " " << std::hex << params.GetCurve().GetA() << endl;

		cout << "Coefficient B:" << endl;
		cout << " " << std::hex << params.GetCurve().GetB() << endl;

		cout << "Base Point:" << endl;
		cout << " X: " << std::hex << params.GetSubgroupGenerator().x << endl; 
		cout << " Y: " << std::hex << params.GetSubgroupGenerator().y << endl;

		cout << "Subgroup Order:" << endl;
		cout << " " << std::hex << params.GetSubgroupOrder() << endl;

		cout << "Cofactor:" << endl;
		cout << " " << std::hex << params.GetCofactor() << endl;    
	}

	void CertGenerator::PrintPrivateKey( const ECDSA<EC2N, SHA256>::PrivateKey& key )
	{   
		cout << endl;
		cout << "Private Exponent:" << endl;
		cout << " " << std::hex << key.GetPrivateExponent() << endl; 
	}

	void CertGenerator::PrintPublicKey( const ECDSA<EC2N, SHA256>::PublicKey& key )
	{   
		cout << endl;
		cout << "Public Element:" << endl;
		cout << " X: " << std::hex << key.GetPublicElement().x << endl; 
		cout << " Y: " << std::hex << key.GetPublicElement().y << endl;
	}

	void CertGenerator::PrintHex( const byte* data, size_t length )
	{
		cout << "{ ";
		for (size_t i = 0; i < length; ++i)
		{
			cout << "0x" << hex << setfill('0') << setw(2) << (int)data[i];
			if (i == length - 1)
			{
				cout << " }";
			}
			else
			{
				cout << ", ";
			}
		}

		cout << endl;
	}

	void CertGenerator::SavePrivateKey( const string& filename, const ECDSA<EC2N, SHA256>::PrivateKey& key )
	{
		string ba;
		key.Save( FileSink( filename.c_str(), true /*binary*/ ).Ref() );
		key.Save( StringSink( ba ) );
		SaveByteArrayAsText(filename, (byte *)ba.data(), (unsigned short)ba.length());
	}

	void CertGenerator::SavePublicKey( const string& filename, const ECDSA<EC2N, SHA256>::PublicKey& key )
	{   
		string ba;
		key.Save( FileSink( filename.c_str(), true /*binary*/ ).Ref() );
		key.Save( StringSink( ba ) );
		SaveByteArrayAsText(filename, (byte *)ba.data(), (unsigned short)ba.length());
	}

	void CertGenerator::LoadPrivateKey( const string& filename, ECDSA<EC2N, SHA256>::PrivateKey& key )
	{   
		key.Load( FileSource( filename.c_str(), true /*pump all*/ ).Ref() );
	}

	void CertGenerator::LoadPublicKey( const string& filename, ECDSA<EC2N, SHA256>::PublicKey& key )
	{
		key.Load( FileSource( filename.c_str(), true /*pump all*/ ).Ref() );
	}

	bool CertGenerator::SignMessage( const ECDSA<EC2N, SHA256>::PrivateKey& key, const string& message, string& signature )
	{
		AutoSeededRandomPool prng;

		signature.erase();    

		StringSource( message, true,
			new SignerFilter( prng,
			ECDSA<EC2N,SHA256>::Signer(key),
			new StringSink( signature )
			) // SignerFilter
			); // stringSource

		return !signature.empty();
	}

	bool CertGenerator::SignMessage( const ECDSA<EC2N, SHA256>::PrivateKey& key, const byte* message, size_t messageLength, byte* signature )
	{
		AutoSeededRandomPool prng;

		ECDSA<EC2N, SHA256>::Signer signer(key);
		size_t sigLen = signer.SignMessage(prng, message, messageLength, signature);
		return sigLen > 0;
	}

	bool CertGenerator::VerifyMessage( const ECDSA<EC2N, SHA256>::PublicKey& key, const string& message, const string& signature )
	{
		bool result = false;

		//stringSource( signature+message, true,
		//    new SignatureVerificationFilter(
		//        ECDSA<EC2N,SHA256>::Verifier(key),
		//        new ArraySink( (byte*)&result, sizeof(result) )
		//    ) // SignatureVerificationFilter
		//);

		ECDSA<EC2N,SHA256>::Verifier verifyer(key);
		result = verifyer.VerifyMessage((const byte *)message.c_str(), message.size(), (const byte*)signature.c_str(), signature.size());
		return result;
	}

	bool CertGenerator::VerifyMessage( const ECDSA<EC2N, SHA256>::PublicKey& key, const byte* message, size_t messageLength, const byte* signature, size_t signatureLength )
	{
		ECDSA<EC2N,SHA256>::Verifier verifyer(key);
		bool result = verifyer.VerifyMessage(message, messageLength, signature, signatureLength);
		return result;
	}

	void CertGenerator::SaveByteArrayAsText(const string& filename, const byte *ba, unsigned short length)
	{
		ofstream outputKey((filename + ".txt").c_str());
		char buffer[3];
		buffer[2] = 0;
		for (unsigned short us = 0; us < length; us++)
		{
			sprintf(buffer, "%*X", 2, ba[us]);
			if (buffer[0] == ' ') buffer[0] = '0';
			outputKey.write(buffer, 2);
		}

		outputKey.close();
	}
}