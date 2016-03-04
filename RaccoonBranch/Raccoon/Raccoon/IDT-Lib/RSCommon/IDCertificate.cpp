#include "stdafx.h"
#include "IDCertificate.h"

namespace RSCommon
{
	size_t IDCertificate::Encode(unsigned char *destination) const
	{
		size_t offset;

		offset = IDD.Encode(destination);
		memcpy(destination + offset, SignedIDD, SIGNATURE_SIZE);
		offset += SIGNATURE_SIZE;

		return offset;
	}

	size_t IDCertificate::Decode(const unsigned char *source)
	{
		size_t offset;

		offset = IDD.Decode(source);
		memcpy(SignedIDD, source + offset, SIGNATURE_SIZE);
		offset += SIGNATURE_SIZE;

		return offset;
	}
}
