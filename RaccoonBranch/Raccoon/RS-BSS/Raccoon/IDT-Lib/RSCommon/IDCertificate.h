#ifndef IDCertificateH
#define IDCertificateH

#include "stdafx.h"
#include "IdentificationData.h"

namespace RSCommon
{
	class RSCOMMON_LIB IDCertificate
	{
	public:
		IDCertificate() {}
		~IDCertificate() {}

		/// <summary>
		/// Encodes instance content into destination buffer.
		/// </summary>
		/// <param name="destination">Destination buffer.</param>
		/// <returns>Number of encoded bytes.</returns>
		size_t Encode(unsigned char *destination) const;
		/// <summary>
		/// Decodes instance content from source buffer.
		/// </summary>
		/// <param name="destination">Source buffer.</param>
		/// <returns>Number of decoded bytes.</returns>
		size_t Decode(const unsigned char *source);

		IdentificationData IDD;
		unsigned char SignedIDD[SIGNATURE_SIZE];
	};
}

#endif