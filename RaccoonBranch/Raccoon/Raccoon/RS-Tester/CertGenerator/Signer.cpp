#include "StdAfx.h"
#include "Signer.h"

#include "oids.h"
using CryptoPP::OID;

#include "integer.h"
using CryptoPP::Integer;

#include "filters.h"
using CryptoPP::StringSource;
using CryptoPP::StringSink;
using CryptoPP::ArraySink;
using CryptoPP::SignerFilter;
using CryptoPP::SignatureVerificationFilter;

#include "files.h"

Signer::Signer()
	: _eccSigner(_privateKey)
{
}

Signer::~Signer()
{
}

bool Signer::Load(BufferedTransformation &bt)
{
	try
	{
		bool result;
		AutoSeededRandomPool prng;

		_privateKey.Load(bt);
		result = _privateKey.Validate(prng, 3);
		if (!result)
		{
			// log
			return false;
		}

		_privateKey.MakePublicKey(_publicKey);
		result = _publicKey.Validate(prng, 3);
		if (!result)
		{
			return false;
		}
		return true;
	}
	catch(...)
	{
		return false;
	}
}

const ECP::Point& Signer::GetPublicElement() const
{
	return _publicKey.GetPublicElement();
}

const string Signer::SignToken(string token)
{
	AutoSeededRandomPool prng;

	string signature;

	StringSource( token, true,
		new SignerFilter( prng,
		_eccSigner,
		new StringSink( signature )
		) // SignerFilter
		); // StringSource

	return signature;
}