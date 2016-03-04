// ECDSA.KeyGen.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <assert.h>

#include <iostream>
using std::cout;
using std::endl;

#include <string>
using std::string;

#include "osrng.h"
// using CryptoPP::AutoSeededX917RNG;
using CryptoPP::AutoSeededRandomPool;

#include "aes.h"
using CryptoPP::AES;

#include "integer.h"
using CryptoPP::Integer;

#include "sha.h"
using CryptoPP::SHA1;
using CryptoPP::SHA256;

#include "filters.h"
using CryptoPP::StringSource;
using CryptoPP::StringSink;
using CryptoPP::ArraySink;
using CryptoPP::SignerFilter;
using CryptoPP::SignatureVerificationFilter;

#include "files.h"
using CryptoPP::FileSource;
using CryptoPP::FileSink;

#include "eccrypto.h"
using CryptoPP::ECDSA;
using CryptoPP::ECP;
using CryptoPP::EC2N;
using CryptoPP::DL_GroupParameters_EC;

#if _MSC_VER <= 1200 // VS 6.0
using CryptoPP::ECDSA<EC2N, SHA256>;
using CryptoPP::DL_GroupParameters_EC<ECP>;
#endif

#include "oids.h"
using CryptoPP::OID;

#include <ctime>
//#include <vld.h>

int TestECDSA();
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

bool SignMessage( const ECDSA<EC2N, SHA256>::PrivateKey& key, const string& message, string& signature );
bool VerifyMessage( const ECDSA<EC2N, SHA256>::PublicKey& key, const string& message, const string& signature );

//////////////////////////////////////////
// In 2010, use SHA-256 and P-256 curve
//////////////////////////////////////////

int main(int argc, char* argv[])
{
	int result = TestECDSA();
	return result;
}

int TestECDSA()
{
	// Scratch result
    bool result = false;

    // Private and Public keys
    ECDSA<EC2N, SHA256>::PrivateKey privateKey;
    ECDSA<EC2N, SHA256>::PublicKey publicKey;

	ECDSA<EC2N, SHA256>::PrivateKey privateKey2;
	ECDSA<EC2N, SHA256>::PublicKey publicKey2;

    /////////////////////////////////////////////
    // Generate Keys
	result = GeneratePrivateKey( CryptoPP::ASN1::sect283k1(), privateKey );
    assert( true == result );
    if( !result ) { return -1; }

    result = GeneratePublicKey( privateKey, publicKey );
	SavePublicKey("public.key", publicKey);
    assert( true == result );
    if( !result ) { return -2; }

	result = GeneratePrivateKey( CryptoPP::ASN1::sect113r2(), privateKey2 );
	assert( true == result );
	if( !result ) { return -1; }

	result = GeneratePublicKey( privateKey2, publicKey2 );
	assert( true == result );
	if( !result ) { return -2; }

    /////////////////////////////////////////////
    // Print Domain Parameters and Keys
    PrintDomainParameters( publicKey );
    PrintPrivateKey( privateKey );
    PrintPublicKey( publicKey );

    /////////////////////////////////////////////
    // Save key in PKCS#9 and X.509 format
    SavePrivateKey( "ec.private.key", privateKey );
    SavePublicKey( "ec.public.key", publicKey );

    /////////////////////////////////////////////
    // Load key in PKCS#9 and X.509 format
    //LoadPrivateKey( "ec.private.key", privateKey );
    //LoadPublicKey( "ec.public.key", publicKey );

    /////////////////////////////////////////////
    // Print Domain Parameters and Keys
    // PrintDomainParameters( publicKey );
    // PrintPrivateKey( privateKey );
    // PrintPublicKey( publicKey );

    /////////////////////////////////////////////
    // Sign and Verify a message
    string message = "Yoda said, Do or do not. There is no try.";
    string message2 = "fake message";
    string signature, signature2;

    result = SignMessage( privateKey, message, signature );
    assert( true == result );

    result = SignMessage( privateKey2, message, signature2 );
    assert( true == result );

    result = VerifyMessage( publicKey, message, signature );
    assert( true == result );

	result = VerifyMessage( publicKey2, message, signature2 );
    assert( true == result );

	result = VerifyMessage( publicKey2, message, signature );
	assert( false == result );

	result = VerifyMessage( publicKey, message, signature2 );
	assert( false == result );

	result = VerifyMessage( publicKey, message2, signature );
	assert( false == result );

	result = VerifyMessage( publicKey2, message2, signature2 );
	assert( false == result );

	return 0;
}

bool GeneratePrivateKey( const OID& oid, ECDSA<EC2N, SHA256>::PrivateKey& key )
{
	cout << "Generating private key... ";
	unsigned int start = clock();
    AutoSeededRandomPool prng;

    key.Initialize( prng, oid );

    bool ok = key.Validate( prng, 3 );
	unsigned int end = clock() - start;
	cout << end << " msec" << endl;
    assert( ok );
	return ok;
}

bool GeneratePublicKey( const ECDSA<EC2N, SHA256>::PrivateKey& privateKey, ECDSA<EC2N, SHA256>::PublicKey& publicKey )
{
    cout << "Generating public key... ";
	unsigned int start = clock();
    AutoSeededRandomPool prng;

    // Sanity check
    assert( privateKey.Validate( prng, 3 ) );

    privateKey.MakePublicKey(publicKey);
    bool ok = publicKey.Validate( prng, 3 );

	unsigned int end = clock() - start;
	cout << end << " msec" << endl;
    assert( ok );
	return ok;
}

void PrintDomainParameters( const ECDSA<EC2N, SHA256>::PrivateKey& key )
{
    PrintDomainParameters( key.GetGroupParameters() );
}

void PrintDomainParameters( const ECDSA<EC2N, SHA256>::PublicKey& key )
{
    PrintDomainParameters( key.GetGroupParameters() );
}

void PrintDomainParameters( const DL_GroupParameters_EC<EC2N>& params )
{
    cout << endl;

    cout << "Modulus:" << endl;
    cout << " " << params.GetCurve().GetField().GetModulus() << endl;

    cout << "Coefficient A:" << endl;
    cout << " " << params.GetCurve().GetA() << endl;

    cout << "Coefficient B:" << endl;
    cout << " " << params.GetCurve().GetB() << endl;

    cout << "Base Point:" << endl;
    cout << " X: " << params.GetSubgroupGenerator().x << endl;
    cout << " Y: " << params.GetSubgroupGenerator().y << endl;

    cout << "Subgroup Order:" << endl;
    cout << " " << params.GetSubgroupOrder() << endl;

    cout << "Cofactor:" << endl;
    cout << " " << params.GetCofactor() << endl;
}

void PrintPrivateKey( const ECDSA<EC2N, SHA256>::PrivateKey& key )
{
    cout << endl;
    cout << "Private Exponent:" << endl;
    cout << " " << std::hex << key.GetPrivateExponent() << endl;
}

void PrintPublicKey( const ECDSA<EC2N, SHA256>::PublicKey& key )
{
    cout << endl;
    cout << "Public Element:" << endl;
    cout << " X: " << std::hex << key.GetPublicElement().x << endl;
    cout << " Y: " << std::hex << key.GetPublicElement().y << endl;
}

void SavePrivateKey( const string& filename, const ECDSA<EC2N, SHA256>::PrivateKey& key )
{
    key.Save( FileSink( filename.c_str(), true /*binary*/ ).Ref() );
}

void SavePublicKey( const string& filename, const ECDSA<EC2N, SHA256>::PublicKey& key )
{
    key.Save( FileSink( filename.c_str(), true /*binary*/ ).Ref() );
}

void LoadPrivateKey( const string& filename, ECDSA<EC2N, SHA256>::PrivateKey& key )
{
    key.Load( FileSource( filename.c_str(), true /*pump all*/ ).Ref() );
}

void LoadPublicKey( const string& filename, ECDSA<EC2N, SHA256>::PublicKey& key )
{
    key.Load( FileSource( filename.c_str(), true /*pump all*/ ).Ref() );
}

bool SignMessage( const ECDSA<EC2N, SHA256>::PrivateKey& key, const string& message, string& signature )
{
    cout << "Signing message... ";
	unsigned int start = clock();
    AutoSeededRandomPool prng;

    //signature.erase();

    //StringSource( message, true,
    //    new SignerFilter( prng,
    //        ECDSA<EC2N, SHA256>::Signer(key),
    //        new StringSink( signature )
    //    ) // SignerFilter
    //); // StringSource
	
	ECDSA<EC2N, SHA256>::Signer signer(key);

	// Determine maximum size, allocate a string with the maximum size
	size_t siglen = signer.MaxSignatureLength();
	signature.assign(siglen, 0);

	// Sign, and trim signature to actual size
	siglen = signer.SignMessage( prng, (const byte*)message.data(), message.size(), (byte*)signature.data() );
	signature.resize(siglen);
	
	unsigned int end = clock() - start;
	cout << end << " msec" << endl;

    return !signature.empty();
}

bool VerifyMessage( const ECDSA<EC2N, SHA256>::PublicKey& key, const string& message, const string& signature )
{
    cout << "Verifying message... ";
	unsigned int start = clock();
    bool result = false;

    //StringSource( signature+message, true,
    //    new SignatureVerificationFilter(
    //        ECDSA<EC2N, SHA256>::Verifier(key),
    //        new ArraySink( (byte*)&result, sizeof(result) )
    //    ) // SignatureVerificationFilter
    //);

	ECDSA<EC2N, SHA256>::Verifier verifier(key);
	result = verifier.VerifyMessage( (const byte*)message.data(), message.size(), (const byte*)signature.data(), signature.size() );
	
	unsigned int end = clock() - start;
	cout << end << " msec" << endl;
    
    return result;
}