#include "stdafx.h"
#include "CertGenerator.h"

#include <assert.h>
#include <iostream>
#include <string>
#include <time.h>
#include <direct.h>

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
		GenerateAndSaveKeyPair(ASN1::sect283k1(), prvKS, pubKS, prvKSFileName, pubKSFileName);

		idcFile.close();
	}

	void CertGenerator::TestCrypto()
	{
		Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("Raccoon"));
		
		ECDSA<EC2N, SHA256>::PrivateKey prvKS;
		ECDSA<EC2N, SHA256>::PublicKey pubKS;
		
		LOG4CPLUS_INFO(logger, "Generating private key...");
		GeneratePrivateKey(ASN1::sect283k1(), prvKS);
		LOG4CPLUS_INFO(logger, "Generating public key...");
		GeneratePublicKey(prvKS, pubKS);
		
		string message = "Message to sign";
		string signature;
		LOG4CPLUS_INFO(logger, "Signing message...");
		bool ok = SignMessage(prvKS, message, signature);
		LOG4CPLUS_INFO(logger, "Verifying message...");
		ok &= VerifyMessage(pubKS, message, signature);
		if (ok)
		{
			LOG4CPLUS_INFO(logger, "Message verified successfully.");
		}
		else
		{
			LOG4CPLUS_ERROR(logger, "Error verifying message.");
		}
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
		result = verifyer.VerifyMessage((const byte *)message.c_str(), message.size(), (const byte*)signature.c_str(), message.size());
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