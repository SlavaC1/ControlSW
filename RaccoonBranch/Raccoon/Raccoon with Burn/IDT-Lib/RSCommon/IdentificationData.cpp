#include "stdafx.h"
#include "IdentificationData.h"

using namespace CryptoPP;

namespace RSCommon
{
	MaterialInformation::MaterialInformation(const MaterialInformation& rhs)
	{
		TagStructID = rhs.TagStructID;
		MaterialType = rhs.MaterialType;
		MfgDateTime = rhs.MfgDateTime;
		memcpy(BatchNo, rhs.BatchNo, BATCH_NO_SIZE);
		InitialWeight = rhs.InitialWeight;
		ExpirationDate = rhs.ExpirationDate;
	}

	size_t MaterialInformation::Encode(unsigned char *destination) const
	{
		size_t offset = 0;

		memcpy(destination + offset, &TagStructID, sizeof(TagStructID));
		offset += sizeof(TagStructID);
		memcpy(destination + offset, &MaterialType, sizeof(MaterialType));
		offset += sizeof(MaterialType);
		memcpy(destination + offset, &MfgDateTime, sizeof(MfgDateTime));
		offset += sizeof(MfgDateTime);
		memcpy(destination + offset, BatchNo, BATCH_NO_SIZE);
		offset += BATCH_NO_SIZE;
		memcpy(destination + offset, &InitialWeight, sizeof(InitialWeight));
		offset += sizeof(InitialWeight);
		memcpy(destination + offset, &ExpirationDate, sizeof(ExpirationDate));
		offset += sizeof(ExpirationDate);

		return offset;
	}

	size_t MaterialInformation::Decode(const unsigned char *source)
	{
		size_t offset = 0;

		memcpy(&TagStructID, source + offset, sizeof(TagStructID));
		offset += sizeof(TagStructID);
		memcpy(&MaterialType, source + offset, sizeof(MaterialType));
		offset += sizeof(MaterialType);
		memcpy(&MfgDateTime, source + offset, sizeof(MfgDateTime));
		offset += sizeof(MfgDateTime);
		memcpy(BatchNo, source + offset, BATCH_NO_SIZE);
		offset += BATCH_NO_SIZE;
		memcpy(&InitialWeight, source + offset, sizeof(InitialWeight));
		offset += sizeof(InitialWeight);
		memcpy(&ExpirationDate, source + offset, sizeof(ExpirationDate));
		offset += sizeof(ExpirationDate);

		return offset;
	}

	IdentificationData::IdentificationData()
	{
		m_pubKC = new PubKey();
		m_verifier = NULL;
	}

	IdentificationData::IdentificationData(const IdentificationData& rhs)
	{
		MaterialInfo = rhs.MaterialInfo;
		memcpy(SerialNo, rhs.SerialNo, SERIAL_NO_SIZE);
		m_pubKC = new PubKey(*rhs.m_pubKC);
		m_verifier = NULL;
	}

	IdentificationData::~IdentificationData()
	{
		if (m_verifier != NULL)
		{
			delete m_verifier;
		}
		if (m_pubKC != NULL)
		{
			delete m_pubKC;
		}
	}

	size_t IdentificationData::Encode(unsigned char *destination) const
	{
		size_t offset = 0;

		offset += this->MaterialInfo.Encode(destination);
		memcpy(destination + offset, SerialNo, SERIAL_NO_SIZE);
		offset += SERIAL_NO_SIZE;

		m_pubKC->Save(ArraySink(destination + offset, PUBLIC_KEY_SIZE).Ref());
		offset += PUBLIC_KEY_SIZE;

		return offset;
	}

	size_t IdentificationData::Decode(const unsigned char *source)
	{
		size_t offset = 0;

		offset += this->MaterialInfo.Decode(source);
		memcpy(SerialNo, source + offset, SERIAL_NO_SIZE);
		offset += SERIAL_NO_SIZE;

		m_pubKC->Load(ArraySource(source + offset, PUBLIC_KEY_SIZE, true).Ref());
		offset += PUBLIC_KEY_SIZE;

		return offset;
	}

	PubKey* IdentificationData::GetCartridgePublicKey() const
	{
		return m_pubKC;
	}

	void IdentificationData::SetCartridgePublicKey(const PubKey& pubKC)
	{
		if (m_verifier != NULL)
		{
			delete m_verifier;
			m_verifier = NULL;
		}

		if (m_pubKC != NULL)
		{
			delete m_pubKC;
		}

		m_pubKC = new PubKey(pubKC);
	}

	Verif* IdentificationData::GetVerifier()
	{
		if (m_verifier == NULL)
		{
			m_verifier = new Verif(*m_pubKC);
		}

		return m_verifier;
	}
}
