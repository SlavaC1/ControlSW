#include "stdafx.h"
#include "IDTagAdapter.h"
#include "osrng.h"
#include "oids.h"
#include "IDTHWInf.h"
#include "Utils.h"

using namespace std;
using namespace CryptoPP;
using namespace RSCommon;

namespace StratasysIDTSrv
{
	IDTagAdapter::IDTagAdapter()
		: m_bIsInitialized(false), m_ucCartridgesCount(0), m_ucCurrentCartridgeNum(0)
	{

	}

	IDTagAdapter::~IDTagAdapter()
	{
		IdtClose();
	}

	int IDTagAdapter::InitHW(unsigned char *pucCartridgesCount)
	{
		int cartridgesCount;
		IdtClose();
		int returnValue = IdtInit(&cartridgesCount);
		if (returnValue != TRUE)
		{
			return FCB_INIT_FAILED;
		}

		m_ucCartridgesCount = *pucCartridgesCount = (unsigned char)(cartridgesCount);
		return IDTLIB_SUCCESS;
	}

	int IDTagAdapter::GetInPlaceStatus(int *pStatus)
	{
		int returnValue = IdtGetInPlaceStatus(pStatus);
		if (returnValue != TRUE)
		{
			return FCB_GET_IN_PLACE_STATUS_FAILED;
		}

		return IDTLIB_SUCCESS;
	}

	int IDTagAdapter::SwitchToCartridge(unsigned char ucCartridgeNum)
	{
		LOG_EXT(LEVEL_DEBUG, "Switch FCB mux...");
		if (ucCartridgeNum >= m_ucCartridgesCount)
		{
			return FCB_SELECT_CHANNEL_FAILED;
		}

		int returnValue = IdtSelectChannel(ucCartridgeNum);
		if (returnValue != TRUE)
		{
			return FCB_SELECT_CHANNEL_FAILED;
		}

		m_ucCurrentCartridgeNum = ucCartridgeNum;
		return IDTLIB_SUCCESS;
	}

	void IDTagAdapter::GenerateCertificate(const unsigned char *aucMaterialInfo, unsigned short usMaterialInfoLength, const unsigned char* aucQx, const unsigned char* aucQy, const unsigned char *aucPrvKS, unsigned char *aucCertificate, unsigned short *usCertificateLength)
	{
		size_t offset = 0;
		memcpy(aucCertificate + offset, aucMaterialInfo, usMaterialInfoLength);
		offset += usMaterialInfoLength;

		unsigned char aucSerialNumber[SERIAL_NO_SIZE];
		unsigned char ucSerialNumberLength = SERIAL_NO_SIZE;
		GetSerialNumber(aucSerialNumber, &ucSerialNumberLength);
		memcpy(aucCertificate + offset, aucSerialNumber, SERIAL_NO_SIZE);
		offset += SERIAL_NO_SIZE;

		PolynomialMod2 Qx, Qy;
		Qx.Decode(aucQx, KEY_COEFF_SIZE);
		Qy.Decode(aucQy, KEY_COEFF_SIZE);
		ECDSA<EC2N, SHA256>::PublicKey pubKC;
		pubKC.Initialize(ASN1::sect283k1(), EC2NPoint(Qx, Qy));

		unsigned char aucPubKC[PUBLIC_KEY_SIZE];
		pubKC.Save(ArraySink(aucPubKC, PUBLIC_KEY_SIZE).Ref());
		memcpy(aucCertificate + offset, aucPubKC, PUBLIC_KEY_SIZE);
		offset += PUBLIC_KEY_SIZE;

		string message((const char *)aucCertificate, offset);

		AutoSeededRandomPool prng;
		ECDSA<EC2N, SHA256>::PrivateKey prvKS;
		prvKS.Load(StringSource(aucPrvKS, PRIVATE_KEY_SIZE, true /*pump all*/ ).Ref());
		ECDSA<EC2N, SHA256>::Signer signer(prvKS);
		string signature(signer.MaxSignatureLength(), 0x00);
		signer.SignMessage(prng, aucCertificate, offset, (byte*)signature.data());
		memcpy(aucCertificate + offset, signature.data(), signature.length());
		offset += signature.length();
		*usCertificateLength = (unsigned short)offset;
	}
}
