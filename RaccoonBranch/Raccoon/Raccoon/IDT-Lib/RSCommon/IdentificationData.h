#ifndef IdentificationDataH
#define IdentificationDataH

#include "stdafx.h"
#include "RSCommonDefinitions.h" 

using namespace CryptoPP;

namespace RSCommon
{
	// Mfg Data
	typedef unsigned short  TAG_TYPE__STRUCT_ID;
	typedef unsigned short  TAG_TYPE__MATERIAL_TYPE;
	typedef unsigned long   TAG_TYPE__MFG_DATE_TIME;
	typedef char*			TAG_TYPE__BATCH_NO;
	typedef unsigned short  TAG_TYPE__INITIAL_WEIGHT; // grams
	typedef unsigned long   TAG_TYPE__EXPIRATION_DATE;
	typedef unsigned short  TAG_TYPE__EXTRA_WEIGHT;

	class RSCOMMON_LIB MaterialInformation
	{
	public:
		MaterialInformation() {}
		MaterialInformation(const MaterialInformation&);
		~MaterialInformation() {}

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

		TAG_TYPE__STRUCT_ID				TagStructID; // Tag revision number.
		TAG_TYPE__MATERIAL_TYPE			MaterialType;
		TAG_TYPE__MFG_DATE_TIME			MfgDateTime;
		char							BatchNo[BATCH_NO_SIZE];
		TAG_TYPE__INITIAL_WEIGHT		InitialWeight;
		TAG_TYPE__EXPIRATION_DATE		ExpirationDate;
		TAG_TYPE__EXTRA_WEIGHT			ExtraWeight;
	};

	class RSCOMMON_LIB IdentificationData
	{
	public:
		IdentificationData();
		IdentificationData(const IdentificationData&);
		~IdentificationData();

		unsigned char* GetSerialNo(unsigned char* pucSerialNoLength) const;
		void SetSerialNo(const unsigned char* acSerialNo, unsigned char ucSerialNoLength);
		PubKey* GetCartridgePublicKey() const;
		void SetCartridgePublicKey(const PubKey& pubKC);
		Verif* GetVerifier();

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

		MaterialInformation				MaterialInfo;

	private:
		unsigned char m_serialNoLength;
		unsigned char* m_serialNo;
		PubKey* m_pubKC;
		Verif* m_verifier;
	};
}

#endif
